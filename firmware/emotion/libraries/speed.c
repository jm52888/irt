/*
*******************************************************************************
* 
* (c) Jason De Lorme <jason@insideridetech.com>
* Inside Ride Technologies, LLC
*
********************************************************************************/

#include <string.h>
#include <math.h>
#include "speed.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "app_util.h"

// RTC1 is based on a 32.768khz crystal, or in other words it oscillates
// 32768 times per second.  The PRESCALER determins how often a tick gets
// counted.  With a prescaler of 0, there are 32,768 ticks in 1 second
// 1/2048th of a second would be 16 ticks (32768/2048)
// # of 2048th's would then be ticks / 16.
#define	TICK_FREQUENCY	(32768 / (NRF_RTC1->PRESCALER + 1))
#define FLYWHEEL_SIZE	0.11176f

static uint16_t m_wheel_size;					// Wheel diameter size in mm.
static float m_flywheel_to_wheel_revs;			// Ratio of flywheel revolutions for 1 wheel revolution.
static uint32_t m_last_accum_flywheel_revs = 0;
static uint16_t m_last_event_time_2048 = 0;

/**@brief	Configure GPIO input from flywheel revolution pin and create an 
 *				event on achannel. 
 */
static void revs_init_gpiote(uint32_t pin_flywheel_rev)
{
	nrf_gpio_cfg_input(pin_flywheel_rev, NRF_GPIO_PIN_NOPULL);
	
	nrf_gpiote_event_config(REVS_CHANNEL_TASK_TOGGLE, 
													pin_flywheel_rev, 
													NRF_GPIOTE_POLARITY_HITOLO);
}

/**@brief		Enable PPI channel 3, combined with previous settings.
 *
 */
static void revs_init_ppi()
{
	uint32_t err_code; 
	
	// Using hardcoded channel 3.
	err_code = sd_ppi_channel_assign(3, 
																&NRF_GPIOTE->EVENTS_IN[REVS_CHANNEL_TASK_TOGGLE], 
																&REVS_TIMER->TASKS_COUNT);
	
	if (err_code == NRF_ERROR_SOC_PPI_INVALID_CHANNEL)
		APP_ERROR_HANDLER(NRF_ERROR_SOC_PPI_INVALID_CHANNEL);

	sd_ppi_channel_enable_set(PPI_CHEN_CH3_Enabled << PPI_CHEN_CH3_Pos);
}

/**@brief 	Function called when a specified number of flywheel revolutions occur.
 *
 */
static void REVS_IRQHandler()
{
	REVS_TIMER->EVENTS_COMPARE[0] = 0;	// This stops the IRQHandler from getting called indefinetly.
	/*
	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; */
}

/**@brief		Initializes the counter which tracks the # of flywheel revolutions.
 *
 */
static void revs_init_timer()
{
	REVS_TIMER->MODE				=	TIMER_MODE_MODE_Counter;
	REVS_TIMER->BITMODE   	= TIMER_BITMODE_BITMODE_16Bit << TIMER_BITMODE_BITMODE_Pos;
	REVS_TIMER->TASKS_CLEAR = 1;
	
	/**
		Uncomment this code to set up a trigger that will call REVS_IRQHandler after a 
		certain number of revolutions.
	 **

	REVS_TIMER->CC[0] 			= REVS_TO_TRIGGER;

	// Clear the counter every time we hit the trigger value.
	REVS_TIMER->SHORTS 			= TIMER_SHORTS_COMPARE0_CLEAR_Msk;
	
	// Interrupt setup.
  REVS_TIMER->INTENSET 		= (TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos);	
	*/
}

/**@brief 	Returns the accumulated count of flywheel revolutions since the
 *					counter started.
 *					
 */
static uint32_t get_flywheel_revs()
{
// DEBUG ONLY CODE
#if defined(SIM_SPEED)
	// DEBUG PURPOSES ONLY. Simulates speed for 1/4 second.
	// 
	// ((((speed_kmh / 3600) * 250) / wheel_size_m) * 18.5218325f);
	// Where speed_kmh = 28.0f, ~17 revolutions per 1/4 of a second.
	//
	static uint32_t r = 0;
	return r+=13;
#endif

	uint32_t revs = 0;

	REVS_TIMER->TASKS_CAPTURE[0] = 1;
	revs = REVS_TIMER->CC[0]; 
	
	return revs;
}

/**@brief 	Returns the count of 1/2048th seconds (2048 per second) since the 
 *					the counter started.  
 *
 * @note		This value rolls over at 32 seconds.
 */
static uint16_t get_seconds_2048()
{
	// Get current tick count.
	uint32_t ticks = NRF_RTC1->COUNTER;

	// Based on frequence of ticks, calculate 1/2048 seconds.
	// freq (hz) = times per second.
	uint16_t seconds_2048 = ROUNDED_DIV(ticks, (TICK_FREQUENCY / 2048));

	return seconds_2048;
}

float get_speed_mps(float wheel_revolutions, uint16_t period_seconds_2048)
{
	if (wheel_revolutions == 0.0f || period_seconds_2048 == 0)
		return 0.0f;
	
	// Convert mm to meters, muliply by revs, multiply by 1,000 for km.
	float distance_m = (((float)m_wheel_size) / 1000.0f) * wheel_revolutions;
	// Get speed in meters per second.
	float mps = distance_m / (period_seconds_2048 / 2048.0f);

	return mps;
}

/**@brief		Calculates how long it would have taken since the last complete 
 *					wheel revolution given current speed (in meters per second).  
 *					Returns a value in 1/2048's of a second.
 *
 */
static uint16_t fractional_wheel_rev_time_2048(float speed_mps, float wheel_revs)
{
	uint16_t time_to_full_rev_2048 = 0;
	
	if (speed_mps > 0)
	{
		// Get the speed in meters per 1/2048s.
		float speed_2048 = speed_mps / 2048;
	
		// A single wheel rev at this speed would take this many 1/2048's of a second.	
		float single_wheel_rev_time = (m_wheel_size / 1000.0f) / speed_2048;

		// Difference between calculated partial wheel revs and the last full wheel rev.
		float partial_wheel_rev = fmod(wheel_revs, 1);

		// How long ago in 1/2048's of a second would the last full wheel rev have occurred?
		time_to_full_rev_2048 = round(partial_wheel_rev * single_wheel_rev_time);
	}
	
	return time_to_full_rev_2048;
}


/*****************************************************************************
*
* Public functions, see function descriptions in header.
*
*****************************************************************************/

float get_speed_kmh(float speed_mps)
{
	return speed_mps * 3.6;
}

float get_speed_mph(float speed_mps)
{
	// Convert km/h to mp/h.
	return get_speed_kmh(speed_mps) * 0.621371;
}

//
// Calculates the speed in meters per second and sets the following values:
//		accumulated wheel revolutions
//		last wheel event time (1/2048s second)
//		current speed (meters per second)
//		period length (last_period_2048)
//
// Maintains state for the last wheel event and accumulated flywheel revolutions
// in order to cacluate the next event.
//
// Returns IRT_SUCCESS or error.
//
uint32_t calc_speed(irt_power_meas_t* p_power_meas)
{
	uint16_t event_time_2048;
	uint32_t accum_flywheel_revs;
	uint32_t flywheel_revs;
	uint16_t time_since_full_rev_2048;
	float wheel_revs_partial;

	// Current time stamp.
	event_time_2048 = get_seconds_2048();
	
	// Flywheel revolution count.
	accum_flywheel_revs = get_flywheel_revs();

	// Flywheel revolutions in current period.
	flywheel_revs = accum_flywheel_revs - m_last_accum_flywheel_revs;

	// Only calculate speed if the flywheel has rotated.
	if (flywheel_revs > 0)
	{
		// Handle time rollover situations.
		if (event_time_2048 < m_last_event_time_2048)
			p_power_meas->period_2048 = (m_last_event_time_2048 ^ 0xFFFF) + event_time_2048;
		else
			p_power_meas->period_2048 = event_time_2048 - m_last_event_time_2048;

		// Calculate partial wheel revs in the period.
		wheel_revs_partial = ((float)flywheel_revs / m_flywheel_to_wheel_revs);

		// Calculate the current speed in meters per second.
		p_power_meas->instant_speed_mps = get_speed_mps(
			wheel_revs_partial,
			p_power_meas->period_2048);	// Current time period in 1/2048 seconds.

		/*
		 Speed (mps) is calculated based on flywheel revolutions.
		 The Bicycle Power service only reports on full wheel revolutions,
		 so we track two different event periods.
		*/
		// Determine time since a full wheel rev in 1/2048's of a second at this speed.
		time_since_full_rev_2048 = fractional_wheel_rev_time_2048(
			p_power_meas->instant_speed_mps,
			wheel_revs_partial);

		// Assign the speed event to the last calculated complete wheel revolution.
		p_power_meas->last_wheel_event_time = event_time_2048 - time_since_full_rev_2048;
		
		// Cast to int and truncate any partial wheel rev.
		p_power_meas->accum_wheel_revs = 
			(uint32_t) (accum_flywheel_revs / m_flywheel_to_wheel_revs);

		// Save state for next calculation.
		m_last_event_time_2048 = event_time_2048;
		m_last_accum_flywheel_revs = accum_flywheel_revs;
	}

	return IRT_SUCCESS;
}

void set_wheel_size(uint16_t wheel_size_mm)
{
	m_wheel_size = wheel_size_mm;
	/*
		For example, assuming a 2.07m wheel circumference:
		 0.01 miles : 144 flywheel_revs 
		 0.01 miles = 16.09344 meters
		 1 servo_rev = 0.11176 distance_meters (FLYWHEEL_SIZE)
	*/
	// For every 1 wheel revolution, the flywheel revolves this many times.
	m_flywheel_to_wheel_revs = (wheel_size_mm / 1000.0f) / FLYWHEEL_SIZE;
}

void init_speed(uint32_t pin_flywheel_rev)
{
	set_wheel_size(DEFAULT_WHEEL_SIZE_MM);	
	
	revs_init_gpiote(pin_flywheel_rev);
	revs_init_ppi();
	revs_init_timer();
	
	// Enable interrupt handler which will internally map to REVS_IRQHandler.
	NVIC_EnableIRQ(REVS_IRQn);
	//__enable_irq();	// <-- TODO: not sure what this call does, leaving it commented.
	
	// Start the counter.
	REVS_TIMER->TASKS_START = 1;
}
