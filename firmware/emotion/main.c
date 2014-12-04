/****************************************************************************** 
 * Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * E-Motion Roller firmware using BLE and ANT.
 *
 * The application uses the follwowing BLE services:
 *		Cycling Power Service
 *		Device Information Service, 
 * 		Battery Service 
 *		Nordic UART Service (for debugging purposes)
 *
 * The application uses the following ANT services:
 *		ANT Cycling Power RX profile
 *		ANT Cycling Power TX profile
 *		ANT Speed Sensor TX profile
 *
 * @file 		main.c 
 * @brief 	The purpose of main is to control program flow and manage any
 *					application specific state.  All other protocol details are handled 
 *					in referenced modules.
 *
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "float.h"
#include "nordic_common.h"
#include "nrf51.h"
#include "softdevice_handler.h"
#include "nrf_error.h"
#include "app_scheduler.h"
#include "pstorage.h"
#include "irt_common.h"
#include "irt_peripheral.h"
#include "temperature.h"
#include "resistance.h"
#include "accelerometer.h"
#include "speed.h"
#include "power.h"
#include "user_profile.h"
#include "wahoo.h"
#include "ble_ant.h"
#include "ant_bike_power.h"
#include "ant_bike_speed.h"
#include "nrf_delay.h"
#include "ant_ctrl.h"
#include "app_timer.h"
#include "debug.h"
#include "boards.h"
#include "battery.h"
#include "irt_error_log.h"
#include "irt_led.h"

#define ANT_4HZ_INTERVAL				APP_TIMER_TICKS(250, APP_TIMER_PRESCALER)  	// Remote control & bike power sent at 4hz.
#define SENSOR_READ_INTERVAL			APP_TIMER_TICKS(128768, APP_TIMER_PRESCALER) // ~2 minutes sensor read interval, which should be out of sequence with 4hz.
#define CALIBRATION_READ_INTERVAL		APP_TIMER_TICKS(50, APP_TIMER_PRESCALER) 	// 20HZ, every 50ms - read

#define WATCHDOG_TICK_COUNT				APP_TIMER_CLOCK_FREQ * 60 * 5 				// (NRF_WDT->CRV + 1)/32768 seconds * 60 seconds * 'n' minutes
#define WDT_RELOAD()					NRF_WDT->RR[0] = WDT_RR_RR_Reload			// Keep the device awake.

#ifdef ENABLE_DEBUG_LOG
#define SCHED_QUEUE_SIZE                32
#else // ENABLE_DEBUG_LOG
#define SCHED_QUEUE_SIZE                8                                          /**< Maximum number of events in the scheduler queue. */
#endif // ENABLE_DEBUG_LOG
#define SCHED_MAX_EVENT_DATA_SIZE       MAX(APP_TIMER_SCHED_EVT_SIZE,\
                                            BLE_STACK_HANDLER_SCHED_EVT_SIZE)       /**< Maximum size of scheduler events. */
//
// Default profile and simulation values.
//
#define DEFAULT_WHEEL_SIZE_MM			2096ul										// Default wheel size.
#define DEFAULT_TOTAL_WEIGHT_KG			8180ul 										// Default weight (convert 180.0lbs to KG).
#define DEFAULT_ERG_WATTS				175u										// Default erg target_watts when not otherwise set.
#define DEFAULT_SETTINGS				SETTING_INVALID								// Default 32bit field of settings.
#define DEFAULT_CRR						30ul										// Default Co-efficient for rolling resistance used when no slope/intercept defined.  Divide by 1000 to get 0.03f.
#define SIM_CRR							0.0033f										// Default crr for typical outdoor rolling resistance (not the same as above).
#define SIM_C							0.60f										// Default co-efficient for drag.  See resistance sim methods.
#define CRR_ADJUST_VALUE				1											// Amount to adjust (up/down) CRR on button command.
#define SHUTDOWN_VOLTS					6											// Shut the device down when battery drops below 6.0v
#define RESISTANCE_MIN_SPEED_ADJUST		3.0f	// (~6.71mph)						// Minimum speed in meters per second at which we adjust resistance.

//
// General purpose retention register states used.
//
#define IRT_GPREG_DFU	 				0x1
#define IRT_GPREG_ERROR 				0x2

#define DATA_PAGE_RESPONSE_TYPE			0x80										// 0X80 For acknowledged response or number of times to send broadcast data requests.

static uint8_t 							m_resistance_level;
static resistance_mode_t				m_resistance_mode;

static app_timer_id_t               	m_ant_4hz_timer_id;                    		// ANT 4hz timer.  TOOD: should rename since it's the core timer for all reporting (not just ANT).
static app_timer_id_t					m_sensor_read_timer_id;						// Timer used to read sensors, out of phase from the 4hz messages.
static app_timer_id_t					m_ca_timer_id;								// Calibration timer.

static user_profile_t  					m_user_profile  __attribute__ ((aligned (4))); // Force required 4-byte alignment of struct loaded from flash.
static rc_sim_forces_t					m_sim_forces;
static accelerometer_data_t 			m_accelerometer_data;
static float							m_temperature = 0.0f;						// Last temperature read.

static uint16_t							m_ant_ctrl_remote_ser_no; 					// Serial number of remote if connected.

static irt_battery_status_t				m_battery_status;
static uint32_t 						m_battery_start_ticks;						// __attribute__ ((section (".noinit")));			// Time (in ticks) when we started running on battery.

static bool								m_crr_adjust_mode;							// Indicator that we're in manual calibration mode.

// Type declarations for profile updating.
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size);
static void profile_update_sched(void);

static void send_data_page2(ant_request_data_page_t* p_request);
static void send_temperature();
static void on_enable_dfu_mode();


/* TODO:	Total hack for request data page & resistance control ack, we will fix.
 *		 	Simple logic right now.  If there is a pending request data page, send
 *		 	up to 2 of these messages as priority (based on requested tx count).
 *		 	If there is a pending resistance acknowledgment, send that as second
 *		 	priority.  Otherwise, resume normal power message.
 *		 	Ensure that at least once per second we are sending an actual power
 *		 	message.
 */
static uint8_t							m_resistance_ack_pending[3] = {0};		// byte 0: operation, byte 1:2: value
static ant_request_data_page_t			m_request_data_pending;
#define ANT_RESPONSE_LIMIT				3										// Only send max 3 sequential response messages before interleaving real power message.

/**@brief Debug logging for main module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define LOG debug_log
#else
#define LOG(...)
#endif // ENABLE_DEBUG_LOG

/*----------------------------------------------------------------------------
 * Error Handlers
 * ----------------------------------------------------------------------------*/

/**@brief Error handler function, which is called when an error has occurred. 
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze 
 *          how your product is supposed to react in case of error.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name. 
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
	// Set LED indicator.
	led_set(LED_ERROR);

	// Fetch the stack and save the error.
	irt_error_save(error_code, line_num, p_file_name);

    // Indicate error state in General Purpose Register.
	sd_power_gpregret_set(IRT_GPREG_ERROR);

	// Kill the softdevice and any pending interrupt requests.
	sd_softdevice_disable();
	__disable_irq();

#if defined(ENABLE_DEBUG_ASSERT)
	irt_error_log_data_t* p_error = irt_error_last();

	LOG("[MAIN]:app_error_handler {HALTED ON ERROR: %#.8x}: %s:%lu\r\nCOUNT = %i\r\n",
			error_code, p_file_name, line_num, p_error->failure);

	#if defined(PIN_EN_SERVO_PWR)
	// Kill power to the servo in case it's in flight.
	nrf_gpio_pin_clear(PIN_EN_SERVO_PWR);
	#endif // PIN_EN_SERVO_PWR

	// TODO: figure out how to display the stack (p_error->stack_info).

#if defined(PIN_PBSW)
	// Note this function does not return - it will hang waiting for a debugger to attach.
	LOG("[MAIN]:app_error_handler -- PRESS BUTTON TO RESET\r\n.");
	while (nrf_gpio_pin_read(PIN_PBSW) == 1) {}			// Do nothing, attach debugger until the button is pressed.
#else // PIN_PBSW
	for (;;) {}
#endif // PIN_PBSW

#endif // ENABLE_DEBUG_ASSERT
	// Wait 1 second, to give user a chance to see LED indicators and then reset the system.
	nrf_delay_ms(1000);
	NVIC_SystemReset();
}

/**@brief	Handle Soft Device system events. */
static void sys_evt_dispatch(uint32_t sys_evt)
{
	// Process storage events.
	pstorage_sys_event_handler(sys_evt);
}

/**@brief	Queues up the last resistance control acknowledgment.
 *
 */
static void queue_resistance_ack(uint8_t op_code, uint16_t value)
{
	m_resistance_ack_pending[0] = op_code;
	m_resistance_ack_pending[1] = LSB(value);
	m_resistance_ack_pending[2] = MSB(value);

	LOG("[MAIN] Queued resistance ack for [%.2x][%.2x]:\r\n",
			m_resistance_ack_pending[1],
			m_resistance_ack_pending[2]);
}

static void queue_data_response(ant_request_data_page_t request)
{
	m_request_data_pending = request;
}

static void queue_data_response_clear()
{
	memset(&m_request_data_pending, 0, sizeof(ant_request_data_page_t));
}

/**@brief	Dispatches ANT messages in response to requests such as Request Data Page and
 * 			resistance control acknowledgments.
 *
 *@return	Returns true if it had messages to dequeue and sent, false if there were none.
 *@note		Will send no more than ANT_RESPONSE_LIMIT in sequential calls.
 */
static bool dequeue_ant_response(void)
{
	// Static response count so that no more than ANT_RESPONSE_LIMIT are sent sequentially
	// as not to starve the normal power messages.
	static uint8_t response_count = 0;

	if (response_count >= ANT_RESPONSE_LIMIT)
	{
		// Reset sequential response count and return.
		response_count = 0;
		return false;
	}

	// Prioritize data response messages first.
	if (m_request_data_pending.data_page == ANT_PAGE_REQUEST_DATA)
	{

		switch (m_request_data_pending.tx_page)
		{
			case ANT_PAGE_MEASURE_OUTPUT:
				// TODO: This is the only measurement being sent right now, but we'll have more
				// to cycle through here.
				send_temperature();
				break;

			case ANT_PAGE_BATTERY_STATUS:
				ant_bp_battery_tx_send(m_battery_status);
				break;

			default:
				send_data_page2(&m_request_data_pending);
				break;
		}
		// byte 1 of the buffer contains the flag for either acknowledged (0x80) or a value
		// indicating how many times to send the message.
		if (m_request_data_pending.tx_response == 0x80 || (m_request_data_pending.tx_response & 0x7F) <= 1)
		{
			// Clear the buffer.
			queue_data_response_clear();
		}
		else
		{
			// Decrement the count, we'll need to send again.
			m_request_data_pending.tx_response--;
		}

	}
	else if (m_resistance_ack_pending[0] != 0)
	{
		uint16_t* value;
		value = (uint16_t*)&m_resistance_ack_pending[1];

		ble_ant_resistance_ack(m_resistance_ack_pending[0], *value);
		// Clear the buffer.
		memset(&m_resistance_ack_pending, 0, sizeof(m_resistance_ack_pending));
	}
	else
	{
		return false;
	}

	response_count++;
	return true;
}

/*----------------------------------------------------------------------------
 * KICKR resistance commands.
 * ----------------------------------------------------------------------------*/

/**@brief Parses the wheel params message from the KICKR.  It then advises the
 * 		  speed module and updates the user profile.  */
static void set_wheel_params(uint8_t *pBuffer)
{
	uint16_t wheel_size;
	
	// Comes as 20700 for example, we'll round to nearest mm.
	wheel_size = uint16_decode(pBuffer) / 10;

	if (m_user_profile.wheel_size_mm != wheel_size && wheel_size > 1800)
	{
		LOG("[MAIN]:set_wheel_params {wheel:%lu}\r\n", wheel_size);

		m_user_profile.wheel_size_mm = wheel_size;
		// Schedule an update to persist the profile to flash.
		profile_update_sched();

		// Call speed module to update the wheel size.
		speed_wheel_size_set(m_user_profile.wheel_size_mm);
	}
}

/**@brief	Parses the SET_SIM message from the KICKR and has user profile info.
 * 			Reinitializes the power module if weight changed.
 */
static void set_sim_params(uint8_t *pBuffer)
{
	// Weight comes through in KG as 8500 85.00kg for example.
	float c, crr;
	uint16_t weight;

	weight = uint16_decode(pBuffer);

	if (weight > 30.0f) // minimum weight.
	{
		if (m_user_profile.total_weight_kg != weight)
		{
			m_user_profile.total_weight_kg = weight;

			// Schedule an update to persist the profile to flash.
			profile_update_sched();

			// Re-initialize the power module with updated weight.
			power_init(&m_user_profile, DEFAULT_CRR);
		}
	}

	// Co-efficient for rolling resistance.
	crr = wahoo_decode_crr(&(pBuffer[2]));
	// Co-efficient of drag.
	c = wahoo_decode_c(&(pBuffer[4]));

	// Store and just precision if new values came across.
	if (crr > 0.0f)
		m_sim_forces.crr = crr;
	if (c > 0.0f)
		m_sim_forces.c = c;

	/*
	LOG("[MAIN]:set_sim_params {weight:%.2f, crr:%i, c:%i}\r\n",
		(m_user_profile.total_weight_kg / 100.0f),
		(uint16_t)(m_sim_forces.crr * 10000),
		(uint16_t)(m_sim_forces.c * 1000) ); */
}

/**@brief	Initializes user profile and loads from flash.  Sets defaults for
 * 				simulation parameters if not set.
 */
static void profile_init(void)
{
		uint32_t err_code;

		err_code = user_profile_init();
		APP_ERROR_CHECK(err_code);

		// Initialize hard coded user profile for now.
		memset(&m_user_profile, 0, sizeof(m_user_profile));
		
		// Initialize simulation forces structure.
		memset(&m_sim_forces, 0, sizeof(m_sim_forces));

		// Attempt to load stored profile from flash.
		err_code = user_profile_load(&m_user_profile);
		APP_ERROR_CHECK(err_code);

		// TODO: this should be refactored into profile class?
		// should all DEFAULT_x defines live in profile.h? The reason they don't today is that
		// certain settings don't live in the profile - so it makes sense to keep all the default DEFINES together
		// Check profile defaults.

		//
		// Check the version of the profile, if it's not the current version
		// set the default parameters.
		//
		if (m_user_profile.version != PROFILE_VERSION)
		{
			LOG("[MAIN]: Older profile version %i. Setting defaults where needed.\r\n",
					m_user_profile.version);

			m_user_profile.version = PROFILE_VERSION;

			// TODO: under what situation would these ever be 0?
			if (m_user_profile.wheel_size_mm == 0 ||
					m_user_profile.wheel_size_mm == 0xFFFF)
			{
				LOG("[MAIN]:profile_init using default wheel circumference.\r\n");

				// Wheel circumference in mm.
				m_user_profile.wheel_size_mm = DEFAULT_WHEEL_SIZE_MM;
			}

			if (m_user_profile.total_weight_kg == 0 ||
					m_user_profile.total_weight_kg == 0xFFFF)
			{
				LOG("[MAIN]:profile_init using default weight.\r\n");

				// Total weight of rider + bike + shoes, clothing, etc...
				m_user_profile.total_weight_kg = DEFAULT_TOTAL_WEIGHT_KG;
			}

			if (m_user_profile.servo_offset == -1) 	// -1 == (int16_t)0xFFFF
			{
				LOG("[MAIN]:profile_init using default servo offset.\r\n");
				m_user_profile.servo_offset = 0; // default to 0 offset.
			}

			// Check for default servo positions.
			if (m_user_profile.servo_positions.count == 0xFF)
			{
				LOG("[MAIN]: Setting default servo positions.\r\n");

				m_user_profile.servo_positions.count = 7;
				m_user_profile.servo_positions.positions[0] = 2000;
				m_user_profile.servo_positions.positions[1] = 1300;
				m_user_profile.servo_positions.positions[2] = 1200;
				m_user_profile.servo_positions.positions[3] = 1100;
				m_user_profile.servo_positions.positions[4] = 1000;
				m_user_profile.servo_positions.positions[5] = 900;
				m_user_profile.servo_positions.positions[6] = 800;
			}

			// Schedule an update.
			profile_update_sched();
		}

	 /*	fCrr is the coefficient of rolling resistance (unitless). Default value is 0.004. 
	 *
	 *	fC is equal to A*Cw*Rho where A is effective frontal area (m^2); Cw is drag 
	 *	coefficent (unitless); and Rho is the air density (kg/m^3). The default value 
	 *	for A*Cw*Rho is 0.60. 	
	 */
		m_sim_forces.crr = SIM_CRR;
		m_sim_forces.c = SIM_C;

		LOG("[MAIN]:profile_init:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t settings: %lu \r\n\t ca_slope: %i \r\n\t ca_intercept: %i \r\n",
				m_user_profile.total_weight_kg,
				m_user_profile.wheel_size_mm,
				m_user_profile.settings,
				m_user_profile.ca_slope,
				m_user_profile.ca_intercept);
}

/**@brief Function for handling profile update.
 *
 * @details This function will be called by the scheduler to update the profile.
 * 			Writing to flash causes the radio to stop, so we don't want to do this
 * 			too often, so we let the scheduler decide when.
 */
static void profile_update_sched_handler(void *p_event_data, uint16_t event_size)
{
	UNUSED_PARAMETER(p_event_data);
	UNUSED_PARAMETER(event_size);

	uint32_t err_code;

	// This method ensures the device is in a proper state in order to update
	// the profile.
	err_code = user_profile_store(&m_user_profile);
	APP_ERROR_CHECK(err_code);

	LOG("[MAIN]:profile_update:\r\n\t weight: %i \r\n\t wheel: %i \r\n\t settings: %lu \r\n\t slope: %i \r\n\t intercept: %i \r\n",
			m_user_profile.total_weight_kg,
			m_user_profile.wheel_size_mm,
			m_user_profile.settings,
			m_user_profile.ca_slope,
			m_user_profile.ca_intercept);
}

/**@brief Schedules an update to the profile.  See notes above in handler.
 *
 */
static void profile_update_sched(void)
{
	uint32_t err_code;

	err_code = app_sched_event_put(NULL, 0, profile_update_sched_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief	Sends a heart beat message for the ANT+ remote control.
  */
static void ant_ctrl_available(void)
{
	// Send remote control availability.
	ant_ctrl_device_avail_tx((uint8_t) (m_ant_ctrl_remote_ser_no != 0));
}

/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

/**@brief Called during calibration to send frequent calibration messages.
 *
 */
static void calibration_timeout_handler(void * p_context)
{
	#define TICKS	5
	UNUSED_PARAMETER(p_context);
	static uint8_t count = 0;
	static uint8_t tick_buffer[TICKS];
	static uint16_t last_flywheel = 0;
	uint16_t flywheel = flywheel_ticks_get();

	if (last_flywheel > 0)
	{
		tick_buffer[count] = (uint8_t)(flywheel - last_flywheel);
	}
	else
	{
		tick_buffer[count] = 0;
	}

	last_flywheel = flywheel;

	// Increment, every 5th tick read, send and rollover count.
	if (++count == TICKS)
	{
		ant_bp_calibration_speed_tx_send(tick_buffer);
		count = 0;

		// Keep device awake.
		WDT_RELOAD();
	}
}


/**@brief Function for handling the ANT 4hz measurement timer timeout.
 *
 * @details This function will be called each timer expiration.  The default period
 * 			for the ANT Bike Power service is 4hz.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void ant_4hz_timeout_handler(void * p_context)
{
	UNUSED_PARAMETER(p_context);
	static uint16_t event_count;

	uint32_t err_code;
	float rr_force;	// Calculated rolling resistance force.
	irt_power_meas_t* p_power_meas_current 		= NULL;
	irt_power_meas_t* p_power_meas_first 		= NULL;
	irt_power_meas_t* p_power_meas_last 		= NULL;

	// All ANT messages on the Bike Power channel are sent in this function:

	// Standard ANT+ Device broadcast (cycle of 5 messages)
	// Request Data Page responses
	// Extra Info debug messages (to be removed before production and replaced with request data page)
	// Acknowledgments to resistance control

	// If there was another ANT+ message response dequeued and sent, wait until next timeslot..
	if (dequeue_ant_response())
		return;

	// Maintain a static event count, we don't do everything each time.
	event_count++;

	// Get pointers to the event structures.
	p_power_meas_current = irt_power_meas_fifo_next();
	p_power_meas_last = irt_power_meas_fifo_last();

	// Set current resistance state.
	p_power_meas_current->resistance_mode = m_resistance_mode;

	switch (m_resistance_mode)
	{
		case RESISTANCE_SET_STANDARD:
			p_power_meas_current->resistance_level = m_resistance_level;
			break;
		case RESISTANCE_SET_ERG: // TOOD: not sure why we're falling through here with SIM?
		case RESISTANCE_SET_SIM:
			p_power_meas_current->resistance_level = (uint16_t)(m_sim_forces.erg_watts);
			break;
		default:
			break;
	}

	p_power_meas_current->servo_position = resistance_position_get();
	p_power_meas_current->battery_status = m_battery_status;

	// Report on accelerometer data.
	if (m_accelerometer_data.source & ACCELEROMETER_SRC_FF_MT)
	{
		p_power_meas_current->accel_y_lsb = m_accelerometer_data.out_y_lsb;
		p_power_meas_current->accel_y_msb = m_accelerometer_data.out_y_msb;

		// Clear the source now that we've read it.
		m_accelerometer_data.source = 0;
	}

	// Record event time.
	p_power_meas_current->event_time_2048 = seconds_2048_get();

	// Last temperature reading.
	p_power_meas_current->temp = m_temperature;

	// Calculate speed.
	err_code = speed_calc(p_power_meas_current, p_power_meas_last);
	APP_ERROR_CHECK(err_code);

	// If we're moving.
	if (p_power_meas_current->instant_speed_mps > 0.0f)
	{
		// Reload the WDT since there was motion, preventing the device from going to sleep.
		WDT_RELOAD();

		// Calculate power and get back the rolling resistance force for use in adjusting resistance.
		err_code = power_calc(p_power_meas_current, p_power_meas_last, &rr_force);
		APP_ERROR_CHECK(err_code);

		// Adjust only if above a certain speed threshold.
		if (p_power_meas_current->instant_speed_mps > RESISTANCE_MIN_SPEED_ADJUST)
		{
			// If in erg or sim mode, adjusts the resistance.
			if (m_resistance_mode == RESISTANCE_SET_ERG || m_resistance_mode == RESISTANCE_SET_SIM)
			{
				// Twice per second adjust resistance.
				if (event_count % 2 == 0)
				{
					// Use the oldest record we have to average with.
					p_power_meas_first = irt_power_meas_fifo_first();
					resistance_adjust(p_power_meas_first, p_power_meas_current, &m_sim_forces,
							m_resistance_mode, rr_force);
				}
			}
		}
	}
	else
	{
		//
		// Stopped, no speed = no power.
		//

		// From the ANT spec:
		// To indicate zero rotational velocity, do not increment the accumulated wheel period and do not increment the wheel ticks.
		// The update event count continues incrementing to indicate that updates are occurring, but since the wheel is not rotating
		// the wheel ticks do not increase.
		p_power_meas_current->accum_wheel_revs = p_power_meas_last->accum_wheel_revs;
		p_power_meas_current->accum_wheel_period = p_power_meas_last->accum_wheel_period;
		p_power_meas_current->last_wheel_event_2048 = p_power_meas_last->last_wheel_event_2048;
		p_power_meas_current->instant_speed_mps = 0.0f;

		// Use the last torque as well since we are not calculating power.
		p_power_meas_current->accum_torque = p_power_meas_last->accum_torque;
		p_power_meas_current->instant_power	 = 0;
	}

	// Transmit the power messages.
	cycling_power_send(p_power_meas_current);

	// Send speed data.
	ant_sp_tx_send(p_power_meas_current->last_wheel_event_2048 / 2, 	// Requires 1/1024 time
			(int16_t)p_power_meas_current->accum_wheel_revs); 			// Requires truncating to 16 bits.

	// Send remote control a heartbeat.
	ant_ctrl_available();
}

/**@brief	Timer handler for reading sensors.
 *
 */
static void sensor_read_timeout_handler(void * p_context)
{
	// Increment battery ticks.  Each "tick" represents 16 seconds.
	m_battery_start_ticks += (uint32_t)(SENSOR_READ_INTERVAL / 32768 / 16);

	// Initiate async battery read.
	battery_read_start();

	// Read temperature sensor.
	m_temperature = temperature_read();
	LOG("[MAIN] Temperature read: %2.1f. \r\n", m_temperature);
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start regular application timers.
    err_code = app_timer_start(m_ant_4hz_timer_id, ANT_4HZ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_start(m_sensor_read_timer_id, SENSOR_READ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
	uint32_t err_code;
		
	err_code = app_timer_stop_all();
    APP_ERROR_CHECK(err_code);
}

/**@brief	Initializes the watchdog timer which is configured to
 * 			force a device reset after a period of inactivity, either
 * 			a CPU hang or no real user activity.  The reaset reason is
 * 			inspected on load to see if the WDT caused the reset.
 */
static void wdt_init(void)
{
    // Configure to keep running while CPU is in sleep mode.
    NRF_WDT->CONFIG = (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = WATCHDOG_TICK_COUNT;
    // This is the reload register, write to this to signal activity.
    NRF_WDT->RREN = WDT_RREN_RR0_Enabled << WDT_RREN_RR0_Pos;
    NRF_WDT->TASKS_START = 1;
}

/**@brief Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;

	// Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, true);

    // Standard ANT 4hz interval to calculate and send messages.
    err_code = app_timer_create(&m_ant_4hz_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                ant_4hz_timeout_handler);
	APP_ERROR_CHECK(err_code);

    // Reading battery / temp sensors less frequently.
	err_code = app_timer_create(&m_sensor_read_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                sensor_read_timeout_handler);
	APP_ERROR_CHECK(err_code);

	// Record data much more frequently during calibration.
	err_code = app_timer_create(&m_ca_timer_id,
								APP_TIMER_MODE_REPEATED,
								calibration_timeout_handler);
	APP_ERROR_CHECK(err_code);
}

/**@brief Scheduler initialization.
 *
 * @details Initializes the scheduler module which is used under the covers to send
 * the ANT/BLE messages.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}

/**@brief	Sets relevant servo positions in response.
 */
static void servo_pos_to_response(ant_request_data_page_t* p_request, uint8_t* p_response)
{
	uint8_t roffset = 0;	// Start data position offset in the response buffer.
	uint8_t start_idx;

	// If not specified, start at index 0.
	start_idx = p_request->descriptor[1] == 0xFF ? 0 : p_request->descriptor[1];

	p_response[roffset++] = RESISTANCE_LEVELS;	// # of levels
	p_response[roffset++] = start_idx; // start index.

	LOG("[MAIN] sending servo_positions count:%i, start %i\r\n", p_response[0], p_response[1]);

	// send 2 positions, 2 bytes each
	for (uint8_t i = start_idx; i < (start_idx + 2); i++)
	{
		LOG("[MAIN] servo_position[%i] %i\r\n", i,
				m_user_profile.servo_positions.positions[i]);

		if (i >= RESISTANCE_LEVELS)
		{
			p_response[roffset++] = 0xFF;
			p_response[roffset++] = 0xFF;
		}
		else
		{
			p_response[roffset++] = LOW_BYTE(m_user_profile.servo_positions.positions[i]);
			p_response[roffset++] = HIGH_BYTE(m_user_profile.servo_positions.positions[i]);
		}
	}
}

/**@brief	Copies the last error to a response structure.
 */
static void error_to_response(uint8_t* p_response)
{
	irt_error_log_data_t* p_err;

	p_err = irt_error_last();
	memcpy(p_response, &(p_err->failure), sizeof(uint16_t));
	memcpy(&(p_response[2]), &(p_err->line_number), sizeof(uint16_t));
	memcpy(&(p_response[4]), &(p_err->err_code), sizeof(uint8_t));
}

/**@brief	Sends data page 2 response.
 */
static void send_data_page2(ant_request_data_page_t* p_request)
{
	uint8_t subpage;
	uint8_t response[6];

	subpage = p_request->descriptor[0];
	memset(&response, 0, sizeof(response));

	switch (subpage)
	{
		case IRT_MSG_SUBPAGE_SETTINGS:
			memcpy(&response, &m_user_profile.settings, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_CRR:
			memcpy(&response, &m_user_profile.ca_slope, sizeof(uint16_t));
			memcpy(&response[2], &m_user_profile.ca_intercept, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_WEIGHT:
			memcpy(&response, &m_user_profile.total_weight_kg, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_WHEEL_SIZE:
			memcpy(&response, &m_user_profile.wheel_size_mm, sizeof(uint16_t));
			break;

		case IRT_MSG_SUBPAGE_GET_ERROR:
			error_to_response(response);
			break;

		case IRT_MSG_SUBPAGE_SERVO_OFFSET:
			memcpy(&response, &m_user_profile.servo_offset, sizeof(int16_t));
			break;

		case IRT_MSG_SUBPAGE_CHARGER:
			response[0] = battery_charge_status();
			break;

		case IRT_MSG_SUBPAGE_FEATURES:
			response[0] = *FEATURES;
			break;

		case IRT_MSG_SUBPAGE_SERVO_POS:
			// Start index is stored in descriptor[1]
			servo_pos_to_response(p_request, response);
			break;

		default:
			LOG("[MAIN] Unrecognized page request. \r\n");
			return;
	}

	ant_bp_page2_tx_send(subpage, response, p_request->tx_response);
}

/**@brief	Sends temperature value as measurement output page (0x03)
 *
 */
static void send_temperature()
{
	float temp;
	int16_t value;

	temp = temperature_read();
	value = (int16_t)(1024.0 * temp);  // TODO: look closer at how the scale factor works. (2^10)

	LOG("[MAIN] Sending temperature as: %i \r\n", value);

	ant_bp_page3_tx_send(1, TEMPERATURE, 10, 0, value);
}

/**@brief 	Suspends normal messages and starts sending calibration messages.
 */
static void calibration_start(void)
{
	uint32_t err_code;

	if (m_resistance_mode != RESISTANCE_SET_STANDARD || m_resistance_level != 0)
	{
		// Force standard level 0 resistance.  TODO: this should really be better encapsulated.
		// We don't queue a resistance ack since the timer shuts down right away.
		m_resistance_level = 0;
		m_resistance_mode = RESISTANCE_SET_STANDARD;
		resistance_level_set(m_resistance_level);
	}

	// Stop existing ANT timer.
	err_code = app_timer_stop(m_ant_4hz_timer_id);
	APP_ERROR_CHECK(err_code);

	// Start the calibration timer.
    err_code = app_timer_start(m_ca_timer_id, CALIBRATION_READ_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);

    m_crr_adjust_mode = true;

	led_set(LED_CALIBRATION_ENTER);
}

/**@brief 	Stops calibration and resumes normal activity.
 */
static void calibration_stop(void)
{
	uint32_t err_code;

	// Stop the calibration timer.
	err_code = app_timer_stop(m_ca_timer_id);
    APP_ERROR_CHECK(err_code);
	m_crr_adjust_mode = false;

	led_set(LED_CALIBRATION_EXIT);

	// Restart the normal ANT timer.
	err_code = app_timer_start(m_ant_4hz_timer_id, ANT_4HZ_INTERVAL, NULL);
}

/**@brief	Updates settings either temporarily or with persistence.
 * @warn	NOTE that if any other profile persistence occurs during the session
 * 			these settings will be updated to flash.
 * 			TODO: fix this as it is a bug -- use a settings dirty flag for instance.
 */
static void settings_update(uint8_t* buffer)
{
	uint16_t settings;

	// The actual settings are a 32 bit int stored in bytes [2:5] IRT_MSG_PAGE2_DATA_INDEX
	// Need to use memcpy, Cortex-M proc doesn't support unaligned casting of 32bit int.
	// MSB is flagged to indicate if we should persist or if this is just temporary.
	memcpy(&settings, &buffer[IRT_MSG_PAGE2_DATA_INDEX], sizeof(uint16_t));

	m_user_profile.settings = SETTING_VALUE(settings);

	/*LOG("[MAIN] Request to update settings to: ACCEL:%i \r\n",
				SETTING_IS_SET(m_user_profile.settings, SETTING_ACL_SLEEP_ON) );*/

	LOG("[MAIN]:settings_update raw: %i, translated: %i\r\n",
			settings, m_user_profile.settings);

	if (SETTING_PERSIST(settings))
	{
		LOG("[MAIN] Scheduled persistence of updated settings: %i\r\n",
				m_user_profile.settings);
		// Schedule update to the profile.
		profile_update_sched();
	}
}

/**@brief	Adjusts the crr intercept by the value.
 *
 */
static void crr_adjust(int8_t value)
{
	ant_request_data_page_t request = {
			.data_page = 0x46, // RequestDataPage -- TODO: this is defined somewhere
			.descriptor[0] = IRT_MSG_SUBPAGE_CRR,
			.tx_page = 0x02, // Get / set parameter page.
			.tx_response = DATA_PAGE_RESPONSE_TYPE
	};

	if (m_user_profile.ca_intercept != 0xFFFF)
	{
		// Wire value of intercept is sent in 1/1000, i.e. 57236 == 57.236
		m_user_profile.ca_intercept += (value * 1000);
		power_init(&m_user_profile, DEFAULT_CRR);
		queue_data_response(request);
	}
}

/*----------------------------------------------------------------------------
 * Event handlers
 * ----------------------------------------------------------------------------*/

// TODO: This event should be registered for a callback when it's time to
// power the system down.
static void on_power_down(bool accelerometer_wake_disable)
{
	LOG("[MAIN]:on_power_down \r\n");

	CRITICAL_REGION_ENTER()
	//sd_softdevice_disable();							// Disable the radios, so no new commands come in.
	application_timers_stop();							// Stop running timers.
	//irt_power_meas_fifo_free();							// Free heap allocation.
	peripheral_powerdown(accelerometer_wake_disable);	// Shutdown peripherals.
	CRITICAL_REGION_EXIT()

	sd_power_system_off();								// Enter lower power mode.
}

static void on_resistance_off(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = 0;
	resistance_level_set(m_resistance_level);
	queue_resistance_ack(m_resistance_mode, m_resistance_level);

	// Quick blink for feedback.
	led_set(LED_MODE_STANDARD);
}

static void on_resistance_dec(void)
{
	// decrement
	switch (m_resistance_mode)
	{
		case RESISTANCE_SET_STANDARD:
			if (m_resistance_level > 0)
			{
				resistance_level_set(--m_resistance_level);
				queue_resistance_ack(m_resistance_mode, m_resistance_level);
				led_set(LED_BUTTON_DOWN);
			}
			else
			{
				LOG("[MAIN] on_resistance_dec hit minimum.\r\n");
				led_set(LED_MIN_MAX);
			}
			break;

		case RESISTANCE_SET_ERG:
			if (m_sim_forces.erg_watts > 50u)
			{
				// Decrement by n watts;
				m_sim_forces.erg_watts -= ERG_ADJUST_LEVEL;
				queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);
				led_set(LED_BUTTON_DOWN);
			}
			else
			{
				led_set(LED_MIN_MAX);
			}
			break;

		default:
			break;
	}
}

static void on_resistance_inc(void)
{
	// increment
	switch (m_resistance_mode)
	{
		case RESISTANCE_SET_STANDARD:
			if (m_resistance_level < (RESISTANCE_LEVELS-1))
			{
				resistance_level_set(++m_resistance_level);
				queue_resistance_ack(m_resistance_mode, m_resistance_level);
				led_set(LED_BUTTON_UP);
			}
			else
			{
				led_set(LED_MIN_MAX);
			}

			break;

		case RESISTANCE_SET_ERG:
			// Increment by x watts;
			m_sim_forces.erg_watts += ERG_ADJUST_LEVEL;
			queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);

			led_set(LED_BUTTON_UP);
			break;

		default:
			break;
	}
}

static void on_resistance_max(void)
{
	m_resistance_mode = RESISTANCE_SET_STANDARD;
	m_resistance_level = RESISTANCE_LEVELS-1;
	resistance_level_set(m_resistance_level);
	queue_resistance_ack(m_resistance_mode, m_resistance_level);

	// Quick blink for feedback.
	led_set(LED_BUTTON_UP);
}

static void on_button_menu(void)
{

	// Start / stop TR whenever the middle button is pushed.
	if ( SETTING_IS_SET(m_user_profile.settings, SETTING_ANT_TR_PAUSE_ENABLED) )
	{
		ant_bp_resistance_tx_send(RESISTANCE_START_STOP_TR, 0);
	}

	// Toggle between erg mode.
	if (m_resistance_mode == RESISTANCE_SET_STANDARD)
	{
		m_resistance_mode = RESISTANCE_SET_ERG;
		if (m_sim_forces.erg_watts == 0)
		{
			m_sim_forces.erg_watts = DEFAULT_ERG_WATTS;
		}
		queue_resistance_ack(m_resistance_mode, m_sim_forces.erg_watts);

		// Quick blink for feedback.
		led_set(LED_MODE_ERG);
	}
	else
	{
		m_resistance_mode = RESISTANCE_SET_STANDARD;
		on_resistance_off();

		led_set(LED_MODE_STANDARD);
	}
}

// This is the button on the board.
static void on_button_pbsw(bool long_press)
{
	if (long_press)
	{
		// TODO: this button needs to be debounced and a LONG press should power down.
		LOG("[MAIN] Push button switch pressed (long).\r\n");
		on_enable_dfu_mode();
	}
	else
	{
		LOG("[MAIN] Push button switch pressed (short).\r\n");
		// Shutting device down.
		on_power_down(true);
	}
}

// This event is triggered when there is data to be read from accelerometer.
static void on_accelerometer(void)
{
	uint32_t err_code;

	// Reload the WDT.
	WDT_RELOAD();

	// Clear the struct.
	memset(&m_accelerometer_data, 0, sizeof(m_accelerometer_data));

	// Read event data from the accelerometer.
	err_code = accelerometer_data_get(&m_accelerometer_data);
	APP_ERROR_CHECK(err_code);

	// Received a sleep interrupt from the accelerometer meaning no motion for a while.
	if (m_accelerometer_data.source == ACCELEROMETER_SRC_WAKE_SLEEP)
	{
		//
		// This is a workaround to deal with GPIOTE toggling the SENSE bits which forces
		// the device to wake up immediately after going to sleep without this.
		//
        NRF_GPIO->PIN_CNF[PIN_SHAKE] &= ~GPIO_PIN_CNF_SENSE_Msk;
        NRF_GPIO->PIN_CNF[PIN_SHAKE] |= GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos;

        LOG("[MAIN]:on_accelerometer received sleep signal.\r\n");;
	}
	else
	{
		LOG("[MAIN]:on_accelerometer source:%i y:%i \r\n",
				m_accelerometer_data.source,
				m_accelerometer_data.out_y_lsb |=
						m_accelerometer_data.out_y_msb << 8);
	}
}

// Called when the power adapter is plugged in.
static void on_power_plug(bool plugged_in)
{
	if (plugged_in)
	{
		LOG("[MAIN] Power plugged in.\r\n");
	}
	else
	{
		LOG("[MAIN] Power unplugged.\r\n");
	}

	// Either way, reset the battery ticks.
	m_battery_start_ticks = 0;
}

static void on_ble_connected(void) 
{
	led_set(LED_BLE_CONNECTED);
	LOG("[MAIN]:on_ble_connected\r\n");
}
	
static void on_ble_disconnected(void) 
{
	// Clear connection LED.
	led_set(LED_BLE_DISCONNECTED);
	LOG("[MAIN]:on_ble_disconnected\r\n");

	// Restart advertising.
	ble_advertising_start();
}

static void on_ble_timeout(void) 
{
	LOG("[MAIN]:on_ble_timeout\r\n");
	led_set(LED_BLE_TIMEOUT);
}

static void on_ble_advertising(void)
{
	led_set(LED_BLE_ADVERTISTING);
}

static void on_ble_uart(uint8_t * data, uint16_t length)
{
	LOG("[MAIN]:on_ble_uart data: %*.*s\r\n", length, length, data);
}

/*@brief	Event handler that the cycling power service calls when a set resistance
 *				command is received.
 *
 */
static void on_set_resistance(rc_evt_t rc_evt)
{
	LOG("[MAIN]:on_set_resistance {OP:%#.2x,VAL:%i}\r\n",
			(uint8_t)rc_evt.operation,
			uint16_decode(rc_evt.pBuffer));
	// 
	// Parse the messages and set state or resistance as appropriate.
	//
	switch (rc_evt.operation)
	{
		case RESISTANCE_SET_STANDARD:
			m_resistance_mode = RESISTANCE_SET_STANDARD;
			m_resistance_level = rc_evt.pBuffer[0];
			resistance_level_set(m_resistance_level);
			break;
			
		case RESISTANCE_SET_PERCENT:
			// Reset state since we're not in standard mode any more.
			m_resistance_level = 0;
			m_resistance_mode = RESISTANCE_SET_PERCENT;
			
			// Parse the buffer for percentage.
			float percent = wahoo_resistance_pct_decode(rc_evt.pBuffer);
			resistance_pct_set(percent);
			break;

		case RESISTANCE_SET_ERG:
			// Assign target watt level.
			m_resistance_mode = RESISTANCE_SET_ERG;
			m_sim_forces.erg_watts = uint16_decode(rc_evt.pBuffer);
			break;

		// This message never seems to come via the apps?
		case RESISTANCE_SET_SIM:
			m_resistance_mode = RESISTANCE_SET_SIM;
			set_sim_params(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_SLOPE:
			m_resistance_mode = RESISTANCE_SET_SIM;
			// Parse out the slope.
			m_sim_forces.grade = wahoo_sim_grade_decode(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_WIND:
			m_resistance_mode = RESISTANCE_SET_SIM;
			// Parse out the wind speed.
			m_sim_forces.wind_speed_mps = wahoo_sim_wind_decode(rc_evt.pBuffer);
			LOG("[MAIN]:on_set_resistance: set wind_speed_mps %i\r\n",
					(uint16_t)(m_sim_forces.wind_speed_mps * 1000));
			break;
			
		case RESISTANCE_SET_WHEEL_CR:
			//  Parse wheel size and update accordingly.
			set_wheel_params(rc_evt.pBuffer);
			break;
			
		case RESISTANCE_SET_BIKE_TYPE:
			m_resistance_mode = RESISTANCE_SET_SIM;
			m_sim_forces.crr = wahoo_decode_crr(rc_evt.pBuffer);
			break;

		case RESISTANCE_SET_C:
			m_resistance_mode = RESISTANCE_SET_SIM;
			m_sim_forces.c = wahoo_decode_c(rc_evt.pBuffer);
			break;

		case RESISTANCE_SET_SERVO_POS:
			// Move the servo to a specific position.
			resistance_position_set(uint16_decode(rc_evt.pBuffer));
			break;

		case RESISTANCE_SET_WEIGHT:
			// Set sim params without changing the mode.
			set_sim_params(rc_evt.pBuffer);
			break;

		default:
			break;
	}

	// Send acknowledgment.
	queue_resistance_ack(rc_evt.operation, (int16_t)*rc_evt.pBuffer);
}

// Invoked when a button is pushed on the remote control.
static void on_ant_ctrl_command(ctrl_evt_t evt)
{
	// Remote can transmit no serial number as 0xFFFF, in which case
	// we can't bond specifically, so we'll just process commands.
	if (evt.remote_serial_no != 0xFFFF)
	{
		if (m_ant_ctrl_remote_ser_no == 0)
		{
			// Track the remote we're now connected to.
			m_ant_ctrl_remote_ser_no = evt.remote_serial_no;

			LOG("[MAIN]:on_ant_ctrl_command Remote {serial no:%i} now connected.\r\n",
					evt.remote_serial_no);
		}
		else if (m_ant_ctrl_remote_ser_no != evt.remote_serial_no)
		{
			// If already connected to a remote, don't process another's commands.
			LOG("[MAIN]:on_ant_ctrl_command received command from another serial no: %i\r\n",
					evt.remote_serial_no);
			return;
		}
	}

	LOG("[MAIN]:on_ant_ctrl_command Command: %i\r\n", evt.command);

	switch (evt.command)
	{
		case ANT_CTRL_BUTTON_UP:
			if (m_crr_adjust_mode)
			{
				crr_adjust(CRR_ADJUST_VALUE);
				led_set(LED_BUTTON_UP);
			}
			else
			{
				// Increment resistance.
				on_resistance_inc();
			}
			break;

		case ANT_CTRL_BUTTON_DOWN:
			if (m_crr_adjust_mode)
			{
				crr_adjust(CRR_ADJUST_VALUE*-1);
				led_set(LED_BUTTON_DOWN);
			}
			else
			{
				// Decrement resistance.
				on_resistance_dec();
			}
			break;

		case ANT_CTRL_BUTTON_LONG_UP:
			// Set full resistance
			on_resistance_max();
			break;

		case ANT_CTRL_BUTTON_LONG_DOWN:
			// Turn off resistance
			on_resistance_off();
			break;

		case ANT_CTRL_BUTTON_MIDDLE:
			if (m_crr_adjust_mode)
			{
				// Exit crr mode.
				calibration_stop();
			}
			else
			{
				on_button_menu();
			}
			break;

		case ANT_CTRL_BUTTON_LONG_MIDDLE:
			// Requires double long push of middle to shut down.
			if (m_crr_adjust_mode)
			{
				// Stop calibration if we get a long hold while in calibration.
				calibration_stop();
			}
			else
			{
				// Change into crr mode.
				calibration_start();
			}

			break;

		default:
			break;
	}
}

// Called when instructed to enable device firmware update mode.
static void on_enable_dfu_mode(void)
{
	uint32_t err_code;

	LOG("[MAIN]:Enabling DFU mode.\n\r");

	// TODO: share the mask here in a common include file with bootloader.
	// bootloader needs to share PWM, device name / manuf, etc... so we need
	// to refactor anyways.
	err_code = sd_power_gpregret_set(IRT_GPREG_DFU);
	APP_ERROR_CHECK(err_code);

	// Resets the CPU to start in DFU mode.
	NVIC_SystemReset();
}

/**@brief	Device receives page (0x46) requesting data page.
 */
static void on_request_data(uint8_t* buffer)
{
	ant_request_data_page_t request;
	memcpy(&request, buffer, sizeof(ant_request_data_page_t));

	/* Hard-coded case for testing error handler.
	if (request.descriptor[0] == 0xFF)
	{
		APP_ERROR_HANDLER(NRF_ERROR_INVALID_PARAM);
	}*/

	switch (request.tx_page)
	{
		// Request is for get/set parameters or measurement output.
		case ANT_PAGE_GETSET_PARAMETERS:
		case ANT_PAGE_MEASURE_OUTPUT:
		case ANT_PAGE_BATTERY_STATUS:
			queue_data_response(request);
			LOG("[MAIN] Request to get data page (subpage): %#x, type:%i\r\n",
					request.descriptor[0],
					request.tx_response);
			break;

		default:
			LOG("[MAIN] Unrecognized request page:%i, descriptor:%i. \r\n",
					request.data_page, request.descriptor[0]);
	}
}

/**@brief	Device receives page (0x02) with values to set.
 */
static void on_set_parameter(uint8_t* buffer)
{
	uint32_t err_code;

	LOG("[MAIN]:set param message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
			buffer[0],
			buffer[1],
			buffer[2],
			buffer[3],
			buffer[4],
			buffer[5],
			buffer[6],
			buffer[7]);

	// SubPage index
	switch (buffer[IRT_MSG_PAGE2_SUBPAGE_INDEX])
	{
		case IRT_MSG_SUBPAGE_SETTINGS:
			settings_update(buffer);
			break;

		case IRT_MSG_SUBPAGE_CRR:
			// Update CRR.
			memcpy(&m_user_profile.ca_slope, &buffer[IRT_MSG_PAGE2_DATA_INDEX],
					sizeof(uint16_t));
			memcpy(&m_user_profile.ca_intercept, &buffer[IRT_MSG_PAGE2_DATA_INDEX+2],
					sizeof(uint16_t));
			LOG("[MAIN] Updated slope:%i intercept:%i \r\n",
					m_user_profile.ca_slope, m_user_profile.ca_intercept);
			// Reinitialize power.
			power_init(&m_user_profile, DEFAULT_CRR);

			// Schedule update to the user profile.
			profile_update_sched();
			break;

		case IRT_MSG_SUBPAGE_SERVO_OFFSET:
			m_user_profile.servo_offset = (int16_t)(buffer[IRT_MSG_PAGE2_DATA_INDEX] |
				buffer[IRT_MSG_PAGE2_DATA_INDEX+1] << 8);
			LOG("[MAIN] Updated servo_offset:%i \r\n", m_user_profile.servo_offset);

			// Schedule update to the user profile.
			profile_update_sched();
			break;

		case IRT_MSG_SUBPAGE_SLEEP:
			LOG("[MAIN] on_set_parameter: Request device sleep.\r\n");
			on_power_down(true);
			break;

		case IRT_MSG_SUBPAGE_CHARGER:
			if (FEATURE_AVAILABLE(FEATURE_BATTERY_CHARGER))
			{
				// Turns charger on if currently off, else turns off.
				battery_charge_set( (BATTERY_CHARGER_IS_OFF) );

				LOG("[MAIN] on_set_parameter: Toggled battery charger.\r\n");
			}
			else
			{
				LOG("[MAIN] on_set_parameter: No battery charger present.\r\n");
			}
			break;

		case IRT_MSG_SUBPAGE_FEATURES:
			err_code = features_store(&buffer[IRT_MSG_PAGE2_DATA_INDEX]);
			APP_ERROR_CHECK(err_code);
			break;

#ifdef SIM_SPEED
		case IRT_MSG_SUBPAGE_DEBUG_SPEED:
			// # of ticks to simulate in debug mode.
			LOG("[MAIN] setting debug speed ticks to: %i\r\n",
					buffer[IRT_MSG_PAGE2_DATA_INDEX]);
			speed_debug_ticks = buffer[IRT_MSG_PAGE2_DATA_INDEX];
			break;
#endif

		default:
			LOG("[MAIN] on_set_parameter: Invalid setting, skipping. \r\n");
			return;
	}
}

/**@brief	Called when a command is received to set servo positions.
 *
 */
static void on_set_servo_positions(servo_positions_t* positions)
{
	if (!resistance_positions_validate(positions))
	{
		LOG("[MAIN] on_set_servo_positions ERROR: invalid servo positions.\r\n");
		return;
	}

	// Save the value.
	m_user_profile.servo_positions = *positions;

#ifdef ENABLE_DEBUG_LOG
	LOG("[MAIN] on_set_servo_positions count:%i\r\n",
			m_user_profile.servo_positions.count);

	for (uint8_t i = 0; i <= m_user_profile.servo_positions.count-1; i++)
	{
		LOG("[MAIN] handle_burst_set_position[%i]: %i\r\n", i,
				m_user_profile.servo_positions.positions[i]);
	}
#endif

	profile_update_sched();
}

/**@brief	Called when a charge status has been indicated.
 *
 */
static void on_charge_status(irt_charger_status_t status)
{
	LOG("[MAIN] on_charge_status: %i\r\n", status);

	switch  (status)
	{
		case BATTERY_CHARGING:
			led_set(LED_CHARGING);
			break;

		case BATTERY_CHARGE_COMPLETE:
			led_set(LED_CHARGED);
			break;

		default:
			led_set(LED_NOT_CHARGING);
			break;
	}
}

/**@brief	Called when the result of the battery is determined.
 *
 */
static void on_battery_result(uint16_t battery_level)
{
	LOG("[MAIN] on_battery_result %i, ticks: %i, seconds: %i \r\n", battery_level,
			m_battery_start_ticks,
			ROUNDED_DIV(NRF_RTC1->COUNTER, TICK_FREQUENCY) );

	m_battery_status = battery_status(battery_level, m_battery_start_ticks);

	// If we're not plugged in and we have a LOW or CRITICAL status, display warnings.
	if (!peripheral_plugged_in())
	{
		switch (m_battery_status.status)
		{
			case BAT_LOW:
				// Blink a warning.
				led_set(LED_BATT_LOW);

				break;

			case BAT_CRITICAL:

				// Set the servo to HOME position.
				on_resistance_off();

				// If we're below 6 volts, shut it all the way down.
				if (m_battery_status.coarse_volt < SHUTDOWN_VOLTS)
				{
					// Indicate and then shut down.
					led_set(LED_BATT_CRITICAL);

					// Critical battery level.
					LOG("[MAIN] on_battery_result critical low battery coarse volts: %i, powering down.\r\n",
							m_battery_status.coarse_volt);
					nrf_delay_ms(3000);
					on_power_down(false);
				}
				else
				{
					// Blink a warning.
					led_set(LED_BATT_WARN);

					// Critical battery level.
					LOG("[MAIN] on_battery_result critical low battery coarse volts: %i, displaying warning.\r\n",
							m_battery_status.coarse_volt);

					// Turn all extra power off.
					peripheral_low_power_set();
				}
				break;

			default:
				break;
		}
	}
}

/**@brief	Configures chip power options.
 *
 * @note	Note this must happen after softdevice is enabled.
 */
static void config_dcpower()
{
	uint32_t err_code;

    // Enabling constant latency as indicated by PAN 11 "HFCLK: Base current with HFCLK
    // running is too high" found at Product Anomaly document found at
    // https://www.nordicsemi.com/eng/Products/Bluetooth-R-low-energy/nRF51822/#Downloads
    //
    // @note This setting will ensure correct behavior when routing TIMER events through
    //       PPI and low power mode simultaneously.
    // NRF_POWER->TASKS_CONSTLAT = 1; // Use sd call as the sd is enabled.
    err_code = sd_power_mode_set(NRF_POWER_MODE_CONSTLAT);
    APP_ERROR_CHECK(err_code);

	// Forces a reset if power drops below 2.7v.
	//NRF_POWER->POFCON = POWER_POFCON_POF_Enabled | POWER_POFCON_THRESHOLD_V27;
    err_code = sd_power_pof_threshold_set(NRF_POWER_THRESHOLD_V27);
    APP_ERROR_CHECK(err_code);

    err_code = sd_power_pof_enable(SD_POWER_POF_ENABLE);
    APP_ERROR_CHECK(err_code);

    // Configure the DCDC converter to save battery.
    // This shows now appreciable difference, see https://devzone.nordicsemi.com/question/5980/dcdc-and-softdevice/
    //err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_AUTOMATIC);
	//err_code = sd_power_dcdc_mode_set(NRF_POWER_DCDC_MODE_OFF);
    APP_ERROR_CHECK(err_code);
}


/**@brief	Check if there is a reset reason and log if enabled.
 */
static uint32_t check_reset_reason()
{
	uint32_t reason, gpreg;
	irt_error_log_data_t* p_error;

	// Read the reset reason
	reason = NRF_POWER->RESETREAS;
	if (reason > 0)
	{
		// Clear the reason by writing 1 bit.
		NRF_POWER->RESETREAS = reason;
		LOG("[MAIN]:Reset reason: %#08lx\r\n", reason);

		// If reset from the WDT, we timed out from no activity or system hang.
		if (reason & POWER_RESETREAS_DOG_Msk)
		{
			LOG("[MAIN] WDT timeout caused reset, enabling interrupts and shutting down.\r\n");

			// Initialize the status LEDs, which ensures they are off.
			led_init();

			// Enable interrupts that will wake the system since retained registers are reset on WDT reset.
			peripheral_wakeup_set();

			// Shut the system down.
			NRF_POWER->SYSTEMOFF = 1;
		}

		if (reason & POWER_RESETREAS_OFF_Msk)
		{
			// Reset was because of a SENSE input, print out the current state.
			LOG("[MAIN] Pins: %u\r\n", NRF_GPIO->IN);
		}
	}
	else
	{
		LOG("[MAIN]:Normal power on.\r\n");
	}

	gpreg = NRF_POWER->GPREGRET;
	// Check and see if the device last reset due to error.
	if (gpreg == IRT_GPREG_ERROR)
	{
		// Log the occurrence.
		p_error = irt_error_last();

		LOG("[MAIN]:check_reset_reason Resetting from previous error.\r\n");
		LOG("\t{COUNT: %i, ERROR: %#.8x}: %s:%lu\r\n",
				p_error->failure,
				p_error->err_code,
				p_error->message,
				p_error->line_number);

		// Now clear the register.
		NRF_POWER->GPREGRET = 0;
	}
	else
	{
		// Initialize the error structure.
		irt_error_init();
	}

	return reason;
}

/**@brief	Initializes the Nordic Softdevice.  This must be done before
 * 			initializing ANT/BLE or calling any softdevice functions.
 */
static void s310_init()
{
	uint32_t err_code;

	// Initialize SoftDevice
	SOFTDEVICE_HANDLER_INIT(NRF_CLOCK_LFCLKSRC_XTAL_20_PPM, true);

    // Subscribe for system events.
    err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
    APP_ERROR_CHECK(err_code);
}

/**@brief Power manager.
 */
void power_manage(void)
{
    uint32_t err_code;

    err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

/*----------------------------------------------------------------------------
 * Main program functions
 * ----------------------------------------------------------------------------*/

/**@brief Application main function.
 */
int main(void)
{
	// Initialize default remote serial number.
	m_ant_ctrl_remote_ser_no = 0;

	// Initialize debug logging if enabled.
	debug_init();

	LOG("**********************************************************************\r\n");
	LOG("[MAIN]:Starting ANT+ id: %i, firmware: %s, serial: %#.8x \r\n",
			ANT_DEVICE_NUMBER, SW_REVISION, SERIAL_NUMBER);
	LOG("[MAIN]:Features: %i\r\n", *FEATURES);
	LOG("**********************************************************************\r\n");

	// Determine what the reason for startup is and log appropriately.
	check_reset_reason();

	// Initialize the watchdog timer.
	wdt_init();

    // Initialize the scheduler, make sure to init before timers.
	scheduler_init();

	// Initialize timers.
	timers_init();

	// Peripheral interrupt event handlers.
	static peripheral_evt_t on_peripheral_handlers = {
		on_button_pbsw,
		on_accelerometer,
		on_power_plug,
		on_battery_result,
		on_charge_status
	};

	// Initialize connected peripherals (temp, accelerometer, buttons, etc..).
	peripheral_init(&on_peripheral_handlers);

	// ANT+, BLE event handlers.
	static ant_ble_evt_handlers_t ant_ble_handlers = {
		on_ble_connected,
		on_ble_disconnected,
		on_ble_timeout,
		on_ble_advertising,
		on_ble_uart,
		on_set_resistance,
		on_ant_ctrl_command,
		on_enable_dfu_mode,
		on_request_data,
		on_set_parameter,
		on_set_servo_positions
	};

	// Initialize and enable the softdevice.
	s310_init();

	// Configure the chip's power options (after sd enabled).
	config_dcpower();

	// Initializes the Bluetooth and ANT stacks.
	ble_ant_init(&ant_ble_handlers);

	// initialize the user profile.
	profile_init();

	// Initialize resistance module and initial values.
	resistance_init(PIN_SERVO_SIGNAL, &m_user_profile);
	// TODO: This state should be moved to resistance module.
	m_resistance_level = 0;
	m_resistance_mode = RESISTANCE_SET_STANDARD;

	// Initialize module to read speed from flywheel.
	speed_init(PIN_FLYWHEEL, m_user_profile.wheel_size_mm);

	// Initialize power module with user profile.
	power_init(&m_user_profile, DEFAULT_CRR);

	// Initialize the FIFO queue for holding events.
	irt_power_meas_fifo_init(IRT_FIFO_SIZE);

	// Begin advertising and receiving ANT messages.
	ble_ant_start();

	// Start the main loop for reporting ble services.
	application_timers_start();

	// Initiate first battery read.
	battery_read_start();

	LOG("[MAIN]:Initialization done.\r\n");

    // Enter main loop
    for (;;)
    {
		app_sched_execute();
    	power_manage();
    }
}
