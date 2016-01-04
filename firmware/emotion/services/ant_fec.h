/* Copyright (c) 2014 Inside Ride Technologies, LLC. All Rights Reserved.
*
*	Encapsulates the interaction with the ANT+ FE-C Profile. 
*
*/

#ifndef ANT_FE_C_H__
#define ANT_FE_C_H__

#include <stdbool.h>
#include <stdint.h>
#include "ble_ant.h"
#include "irt_common.h"
#include "ant_stack_handler_types.h"

#define EQUIPMENT_TRAINER			25

#define GENERAL_FE_DATA_PAGE		16
#define GENERAL_SETTINGS_PAGE		17
#define SPECIFIC_TRAINER_PAGE		25
#define BASIC_RESISTANCE_PAGE		48
#define TARGET_POWER_PAGE			49
#define WIND_RESISTANCE_PAGE		50
#define TRACK_RESISTANCE_PAGE		51
#define COMMAND_STATUS_PAGE			71

#define HR_DATA_SOURCE 				0	
#define DISTANCE_TRAVELED_ENABLED	1	
#define EQUIPMENT_TYPE				25
#define CADENCE_INVALID				0xFF
#define HEARTRATE_INVALID			0xFF
#define INCLINE_INVALID				0x7FFF
#define RESISTANCE_INVALID			0xFF

/**@ brief	Macro to set the value of FE State bit field (nibble) from irt_context_t. 	
				fe_state 			// bits 0-2 
				lap_toggle 			// bit 3 
 */
#define FESTATE_CONTEXT(context) \
		(context->fe_state | (context->lap_toggle << 3))			

/**@ brief	Macro to set the value of FE capabilities bit field (nibble) from irt_context_t.
		HR_DATA_SOURCE  						// bits 0-1
		DISTANCE_TRAVELED_ENABLED 				// bit 2
		virtual_speed_flag   					// bit 3
 */
#define CAPABILITIES_CONTEXT(context) \
		(HR_DATA_SOURCE | \	
		(DISTANCE_TRAVELED_ENABLED << 2) | \	
		(context->virtual_speed_flag << 3))   	

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t 	EquipmentType;
	uint8_t 	ElapsedTime;
	uint8_t 	Distance;
	uint8_t 	SpeedLSB;
	uint8_t 	SpeedMSB;
	uint8_t 	HeartRate;
	/* On the wire FEState bits appear in most significant nibble, before capabilities
	   nibble. bit 0 of EACH nibble (4 bits) is the right most bit (least significant). */
	uint8_t 	Capabilities:4;
	uint8_t 	FEState:4;
} FEC_Page16; // General FE Data Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint16_t 	Reserved;
	uint8_t 	CycleLength;				// Wheel Circumference on a Trainer in meters. 0.01 - 2.54m
	uint8_t 	InclineLSB;
	uint8_t 	InclineMSB;
	uint8_t		ResistanceLevelFEC; 		// Percentage of maximum applicable resitsance (0-100%)
	uint8_t		Capabilities:4; 			// Reserved for future, set to: 0x0
	uint8_t		FEState:4;					//  
} FEC_Page17; // General Settings Page

typedef struct {
	uint8_t 	DataPageNumber;
	uint8_t		UpdateEventCount;
	uint8_t		InstantCadence;
	uint8_t		AccumulatedPowerLSB;
	uint8_t		AccumulatedPowerMSB;
	uint8_t		InstantPowerLSB;
	uint8_t		InstantPowerMSB:4;			// Uses 1.5 bytes
	uint8_t		TrainerStatusBit:4;			
	uint8_t		Flags:4;
	uint8_t		FEState:4;
} FEC_Page25; // Specific Trainer Data Page

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[6];
	uint8_t		TotalResistance;			// 0-100%.
} FEC_Page48; // Basic Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[5];
	uint8_t		TargetPowerLSB;
	uint8_t		TargetPowerMSB;				// 0 - 4000W in 0.25W units.				
} FEC_Page49; // Target Power

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[4];
	uint8_t		WindResistanceCoeff;
	uint8_t		WindSpeed;					// -127 - +127 km/h
	uint8_t		DraftingFactor;				// 0 - 1.00				
} FEC_Page50; // Wind Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		Reserved[4];
	uint8_t		GradeLSB;
	uint8_t		GradeMSB;
	uint8_t		CoeffRollingResistance;				
} FEC_Page51; // Track Resistance

typedef struct {
	uint8_t		DataPageNumber;
	uint8_t		LastReceivedCommandID;
	uint8_t		Sequence; 					// Default to 255 if no control page yet been receieved.
	uint8_t		CommandStatus;				
	uint32_t	Data;						// Response data specific to command ID.
} FEC_Page71; // Command Status Page

void ant_fec_tx_init(ant_ble_evt_handlers_t * evt_handlers);

void ant_fec_tx_start(void);

void ant_fec_tx_send(irt_context_t* p_power_meas);

void ant_fec_rx_handle(ant_evt_t* p_ant_evt);

#endif	// ANT_FE_C_H__
