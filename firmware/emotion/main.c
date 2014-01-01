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

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf_error.h"
#include "irt_peripheral.h"
#include "resistance.h"
#include "speed.h"
#include "power.h"
#include "user_profile.h"
#include "ble_ant.h"

#define CYCLING_POWER_MEAS_INTERVAL       APP_TIMER_TICKS(1000, APP_TIMER_PRESCALER)/**< Bike power measurement interval (ticks). */

static uint8_t 														m_resistance_level = 0;
static app_timer_id_t               			m_cycling_power_timer_id;                    /**< Cycling power measurement timer. */
static user_profile_t 										m_user_profile;

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
    nrf_gpio_pin_set(ASSERT_LED_PIN_NO);

    // This call can be used for debug purposes during development of an application.
    // @note CAUTION: Activating this code will write the stack to flash on an error.
    //                This function should NOT be used in a final product.
    //                It is intended STRICTLY for development/debugging purposes.
    //                The flash write will happen EVEN if the radio is active, thus interrupting
    //                any communication.
    //                Use with care. Un-comment the line below to use.
    // ble_debug_assert_handler(error_code, line_num, p_file_name);

    // On assert, the system can only recover with a reset.
    NVIC_SystemReset();
}


/*----------------------------------------------------------------------------
 * Timer functions
 * ----------------------------------------------------------------------------*/

/**@brief Function for handling the cycling power measurement timer timeout.
 *
 * @details This function will be called each timer expiration.
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void cycling_power_meas_timeout_handler(void * p_context)
{
		// Hang on to accumulated torque for a session duration.
		// TODO: make sure this gets cleared at some point, probably
		// the wrong spot to keep this.
		static uint16_t 			m_accum_torque = 0;

    uint32_t err_code;
    UNUSED_PARAMETER(p_context);

		// Initialize structures.
		ble_cps_meas_t cps_meas;
		memset(&cps_meas, 0, sizeof(cps_meas));
		
		speed_event_t			speed_event;
		memset(&speed_event, 0, sizeof(speed_event));

		// Calculate speed.
		// TODO: use error handling here.
		calc_speed(&speed_event);
		float speed_mph = get_speed_mph(speed_event.speed_mps);
		
		// Calculate power.
		int16_t		watts					= 0;
		err_code = calc_power(speed_mph, m_user_profile.total_weight_lb, m_resistance_level, &watts);
		// TODO: Handle the error for real here, not sure what the overall error 
		// handling strategy will be, but this is not critical, just move on.
		if (err_code != IRT_SUCCESS)
			return;		
		
		// Calculate torque.
		uint16_t 	torque				= 0;
		err_code = calc_torque(watts, speed_event.period_2048, &torque);
		if (err_code != IRT_SUCCESS)
			return;
		
		// Store accumulated torque for the session.
		m_accum_torque += torque;
		
		cps_meas.instant_power 				= watts;
		cps_meas.accum_torque 				= m_accum_torque; //(not working?)
		cps_meas.accum_wheel_revs 		= speed_event.accum_wheel_revs;
		cps_meas.last_wheel_event_time= speed_event.event_time_2048;

		cycling_power_send(&cps_meas);
		
		uint8_t data[14] = "";
		sprintf(&data[0], "revs:%i,%i", speed_event.accum_flywheel_revs, speed_event.accum_wheel_revs);
		debug_send(&data[0], sizeof(data));		
}

/**@brief Function for starting the application timers.
 */
static void application_timers_start(void)
{
    uint32_t err_code;

    // Start application timers
    err_code = app_timer_start(m_cycling_power_timer_id, CYCLING_POWER_MEAS_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

static void application_timers_stop(void)
{
		uint32_t err_code;
		
		err_code = app_timer_stop(m_cycling_power_timer_id);
    APP_ERROR_CHECK(err_code);
}

/**@brief Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    uint32_t err_code;

		// Initialize timer module
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_MAX_TIMERS, APP_TIMER_OP_QUEUE_SIZE, false);

    err_code = app_timer_create(&m_cycling_power_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                cycling_power_meas_timeout_handler);
    APP_ERROR_CHECK(err_code);

}

/*----------------------------------------------------------------------------
 * Event handlers
 * ----------------------------------------------------------------------------*/

static void on_button_ii_event(void)
{
	// TODO: wrap this in an ifdef for DEBUG.
	uint8_t data[] = "button_ii_event";
	debug_send(&data[0], sizeof(data));
	//
	
	// decrement
	if (m_resistance_level > 0)
		set_resistance(--m_resistance_level);	
}

static void on_button_iii_event(void)
{
	// TODO: wrap this in an ifdef for DEBUG.
	uint8_t data[] = "button_iii_event";
	debug_send(&data[0], sizeof(data));
	//
	
	// increment
	if (m_resistance_level < (MAX_RESISTANCE_LEVELS-1))
		set_resistance(++m_resistance_level);
}


static void on_ble_connected(void) 
{
		nrf_gpio_pin_set(CONNECTED_LED_PIN_NO);
		nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);	
}
	
static void on_ble_disconnected(void) 
{
		nrf_gpio_pin_clear(CONNECTED_LED_PIN_NO);
}

static void on_ble_timeout(void) 
{
		nrf_gpio_pin_clear(ADVERTISING_LED_PIN_NO);
}

static void on_ble_advertising(void)
{
		nrf_gpio_pin_set(ADVERTISING_LED_PIN_NO);	
}

static void on_ble_uart(uint8_t * data, uint16_t length)
{
#ifdef LOOPBACK
    debug_send(data, length);
#endif
}

static void on_ant_channel_closed(void) {}
static void on_ant_power_data(void) {}

/*@brief	Event handler that the cycling power service calls when a set resistance
 *				command is received.
 *
 */
static void on_set_resistance(ble_cps_t * p_cps, ble_cps_rc_evt_t * p_evt)
{
	switch (p_evt->resistance_mode)
	{
		case BLE_CPS_RESISTANCE_SET_STANDARD:
			m_resistance_level = (uint8_t)p_evt->p_value[0];
			set_resistance(m_resistance_level);
			break;
			/*
		case BLE_CPS_RESISTANCE_SET_PERCENT:
			set_resistance_pct(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_ERG:
			set_resistance_erg(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_SLOPE:
			set_resistance_slope(x);
			break;
			
		case BLE_CPS_RESISTANCE_SET_WIND:
			set_resistance_wind(x);
			break;
			*/
		default:
			break;
	}
}

		
static void on_button_evt(uint8_t pin_no)
{
    switch (pin_no)
    {
				case PIN_BUTTON_II:
						on_button_ii_event();
						break;

				case PIN_BUTTON_III:
						on_button_iii_event();
						break;

        default:
            APP_ERROR_HANDLER(pin_no);
    }	
}
		
/*----------------------------------------------------------------------------
 * Main program functions
 * ----------------------------------------------------------------------------*/

/**@brief Application main function.
 */
int main(void)
{
		// Initialize hard coded user profile for now.
		memset(&m_user_profile, 0, sizeof(m_user_profile));
		m_user_profile.wheel_size_mm = 2070;
		m_user_profile.total_weight_lb = 175.0f;
		
		// Initialize timers.
		timers_init();
		
    // Initialize peripherals
		peripheral_init(on_button_evt);

		static ant_ble_evt_handlers_t handlers = { 
			on_ble_connected,
			on_ble_disconnected,
			on_ble_timeout,
			on_ble_advertising,
			on_ble_uart,
			on_ant_channel_closed,
			on_ant_power_data,
			on_set_resistance
		};

		// Initialize Bluetooth and ANT stacks.
		ble_ant_init(&handlers);

		// Begin advertising and receiving ANT messages.
		ble_ant_start();

		// Start off with resistance at 0.
		set_resistance(m_resistance_level);	
		init_speed(PIN_DRUM_REV, m_user_profile.wheel_size_mm);
		
		// TOOD: we need to ensure we're being woken up here by a HW
		// interrupt or something before we actually start reporting.
		// Start the main loop for reporting ble services.
		application_timers_start();
		
		// TODO: when do we stop timers? We need to track a timeout since last
		// HW interupt, i.e. 5 mins or so? 
		// Stop reporting ble services.
		// application_timers_stop();

		
    // Enter main loop
    for (;;)
    {
        power_manage();
    }
}