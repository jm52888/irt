/* Copyright (c) 2013 Inside Ride Technologies, LLC. All Rights Reserved.
*/

#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include <stddef.h>
#include "string.h"
#include "app_util.h"
#include "ant_bike_power.h"
#include "ant_parameters.h"
#include "ant_interface.h"
#include "ant_error.h"
#include "app_error.h"
#include "nordic_common.h"
#include "irt_common.h"
#include "speed_event_fifo.h"
#include "debug.h"

#define POWER_PAGE_INTERLEAVE_COUNT			5u
#define EXTRA_INFO_PAGE_INTERLEAVE_COUNT	8u
#define MANUFACTURER_PAGE_INTERLEAVE_COUNT	121u
#define PRODUCT_PAGE_INTERLEAVE_COUNT		242u
#define BATTERY_PAGE_INTERLEAVE_COUNT		61u

#define EVENT_COUNT_INDEX               1u                  /**< Index of the event count field in the power-only main data page. */
#define PEDAL_POWER_INDEX               2u                  /**< Index of the pedal power field in the power-only main data page. */
#define INSTANT_CADENCE_INDEX           3u                  /**< Index of the instantaneous cadence field in the power-only main data page. */
#define ACCUM_POWER_LSB_INDEX    		4u                  /**< Index of the accumulated power LSB field in the power-only main data page. */
#define ACCUM_POWER_MSB_INDEX    		5u                  /**< Index of the accumulated power MSB field in the power-only main data page. */
#define INSTANT_POWER_LSB_INDEX         6u                  /**< Index of the instantaneous power LSB field in the power-only main data page. */
#define INSTANT_POWER_MSB_INDEX         7u                  /**< Index of the instantaneous power MSB field in the power-only main data page. */

// Standard Wheel Torque Main Data Page (0x11)
#define WHEEL_TICKS_INDEX				2u
#define	WHEEL_PERIOD_LSB_INDEX			4u
#define	WHEEL_PERIOD_MSB_INDEX			5u
#define ACCUM_TORQUE_LSB_INDEX			6u
#define ACCUM_TORQUE_MSB_INDEX			7u

#define ANT_BP_CHANNEL_TYPE          0x00                   /**< Channel Type RX. */
#define ANT_BP_DEVICE_TYPE           0x0B                   /**< Channel ID device type. */
#define ANT_BP_TRANS_TYPE            0x00 					/**< Transmission Type. */
#define ANT_BP_MSG_PERIOD            0x1FF6                 /**< Message Periods, decimal 8182 (~4.00Hz) data is transmitted every 8182/32768 seconds. */
#define ANT_BP_EXT_ASSIGN            0	                    /**< ANT Ext Assign. */

// Battery Status page.
#define ANT_BAT_ID_INDEX		 	 2
#define ANT_BAT_TIME_LSB_INDEX	 	 3
#define ANT_BAT_TIME_INDEX	 	 	 4
#define ANT_BAT_TIME_MSB_INDEX	 	 5
#define ANT_BAT_FRAC_VOLT_INDEX	 	 6
#define ANT_BAT_DESC_INDEX	 	 	 7

#define ANT_BURST_MSG_ID_SET_RESISTANCE	0x48									/** Message ID used when setting resistance via an ANT BURST. */
#define ANT_BURST_MSG_ID_SET_POSITIONS	0x59									/** Message ID used when setting servo button stop positions via an ANT BURST. */
#define ANT_BURST_MSG_ID_SET_MAGNET_CA	0x60									/** Message ID used when setting magnet calibration via an ANT BURST. */

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define BP_LOG debug_log
#else
#define BP_LOG(...)
#endif // ENABLE_DEBUG_LOG

/**@brief Callback to invoke when power data arrives.
 *
 */
static bp_evt_handler_t m_on_bp_power_data;

/**@brief Standard Bike Power Only page structure.
 *
 */
typedef struct 
{
	uint8_t			page_number;
	uint8_t			event_count;
	uint8_t			pedal_power;
	uint8_t			instant_cadence;
	uint8_t			accum_power_lsb;
	uint8_t			accum_power_msb;
	uint8_t			instant_power_lsb;
	uint8_t			instant_power_msb;	
} ant_bp_standard_power_only_t;

typedef struct
{
	uint8_t 		event_count;
	uint16_t 		power_watts;
} power_event_t;

/**@brief Maintain a buffer of events.
 *
 */
static power_event_t m_power_only_buffer[SPEED_EVENT_CACHE_SIZE];
static event_fifo_t m_power_only_fifo;

/**@brief	Calculates average power in watts between two events.
 *
 */
static float CalcAveragePower(ant_bp_standard_power_only_t first, 
		ant_bp_standard_power_only_t last)
{
	// // Deltas between first and last events.
	// uint16_t event_count;

	// //
	// // Calculate ticks in the event period.
	// //
	// if (last. < first.accum_flywheel_ticks)
	// {
	// 	// Handle ticks rollover.
	// 	flywheel_ticks = (first.accum_flywheel_ticks ^ 0xFFFF) +
	// 			last.accum_flywheel_ticks;

	// 	SP_LOG("[SP] speed_calc had a accum tick rollover.\r\n");
	// }
	// else
	// {
	// 	flywheel_ticks = last.accum_flywheel_ticks - first.accum_flywheel_ticks;
	// }

	// //
	// // Only calculate speed if the flywheel has rotated.
	// //
	// if (flywheel_ticks == 0)
	// {
	// 	return 0.0f;
	// }

	// //
	// // Calculate delta in time between events.
	// //
	// if (last.event_time_2048 < first.event_time_2048)
	// {
	// 	// Handle time rollover.
	// 	event_period = (first.event_time_2048 ^ UINT32_MAX) + last.event_time_2048;
	// }
	// else
	// {
	// 	event_period = last.event_time_2048 - first.event_time_2048;
	// }

	// // Virtual road distance traveled in meters.
	// distance_m = flywheel_ticks / FLYWHEEL_TICK_PER_METER;

	// // Calculate speed in meters per second.
	// return (distance_m / (event_period / 2048.0f));

	return 0.0f;
}

/**@brief Parses ant data page to combine bits for power in watts.
 *
 */
static uint16_t GetWatts(ant_bp_standard_power_only_t* p_page)
{
	//									    LSB MSB
	// Example Tx: [10][44][FF][5A][2C][4B][1B][01] // 283 watts
	uint16_t watts = p_page->instant_power_msb << 8 | p_page->instant_power_lsb;
	return  watts;
}

static void HandleStandardPowerOnlyPage(uint8_t* p_payload)
{
	power_event_t event;

	// Parse page.
	ant_bp_standard_power_only_t* p_page = (ant_bp_standard_power_only_t*)p_payload;
	event.event_count = p_page->event_count;
	event.power_watts = GetWatts(p_page);

	// Add to the queue for later averaging.
	speed_event_fifo_put(&m_power_only_fifo, (uint8_t*)&event);

	// Raise event with new power data.
	m_on_bp_power_data(event.power_watts);
}

/**@brief Invoked when a event occurs on the ant_bp channel.
 *
 */
void ant_bp_rx_handle(ant_evt_t * p_ant_evt)
{
	ANT_MESSAGE* p_mesg = (ANT_MESSAGE*)p_ant_evt->evt_buffer;

	// Switch on page number.
	switch (p_mesg->ANT_MESSAGE_aucPayload[0])
	{
		case ANT_BP_PAGE_STANDARD_POWER_ONLY:
			HandleStandardPowerOnlyPage(p_mesg->ANT_MESSAGE_aucPayload);
			break;

		default:
			BP_LOG("[BP]:unrecognized message [%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\r\n",
					p_mesg->ANT_MESSAGE_aucPayload[0],
					p_mesg->ANT_MESSAGE_aucPayload[1],
					p_mesg->ANT_MESSAGE_aucPayload[2],
					p_mesg->ANT_MESSAGE_aucPayload[3],
					p_mesg->ANT_MESSAGE_aucPayload[4],
					p_mesg->ANT_MESSAGE_aucPayload[5],
					p_mesg->ANT_MESSAGE_aucPayload[6],
					p_mesg->ANT_MESSAGE_aucPayload[7],
					p_mesg->ANT_MESSAGE_aucPayload);
			break;
	}
}

/**@brief Initialize the module with callback and power meter device id to search for.
 *
 */
void ant_bp_rx_init(bp_evt_handler_t on_bp_power_data, uint16_t device_id)
{
    uint32_t err_code;
    
	// Assign callback for when resistance message is processed.	
    m_on_bp_power_data = on_bp_power_data;
    
    err_code = sd_ant_channel_assign(ANT_BP_TX_CHANNEL,
                                     ANT_BP_CHANNEL_TYPE,
                                     ANTPLUS_NETWORK_NUMBER,
                                     ANT_BP_EXT_ASSIGN);
    APP_ERROR_CHECK(err_code);

    err_code = sd_ant_channel_id_set(ANT_BP_TX_CHANNEL,
                                     device_id,
                                     ANT_BP_DEVICE_TYPE,
                                     ANT_BP_TRANS_TYPE);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_radio_freq_set(ANT_BP_TX_CHANNEL, ANTPLUS_RF_FREQ);
    APP_ERROR_CHECK(err_code);
    
    err_code = sd_ant_channel_period_set(ANT_BP_TX_CHANNEL, ANT_BP_MSG_PERIOD);
    APP_ERROR_CHECK(err_code);
}

/**@brief Opens the channel and begins to search.
 *
 */
void ant_bp_rx_start(void)
{
    uint32_t err_code = sd_ant_channel_open(ANT_BP_TX_CHANNEL);
    APP_ERROR_CHECK(err_code);

	// Initialize fifo for collecting power events to average.
	m_power_only_fifo = speed_event_fifo_init((uint8_t*)m_power_only_buffer, 
		sizeof(power_event_t));

}

/**@brief Calculates average power for the period in seconds.
 *
 */
uint16_t ant_bp_avg_power(uint8_t seconds)
{
	power_event_t* p_oldest = 
		(power_event_t*)speed_event_fifo_oldest(&m_power_only_fifo);
	power_event_t* p_current = 
		(power_event_t*)speed_event_fifo_get(&m_power_only_fifo);

	//SP_LOG("[SP] %i, %i\r\n", p_oldest->accum_flywheel_ticks, p_current->accum_flywheel_ticks);

	//return speed_calc_mps(*p_oldest, *p_current);	
	return 0;
}
