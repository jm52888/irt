/*
 * Driver for the NCT75 temperature sensor.
 *
 *  Created on: Feb 8, 2014
 *      Author: jasondel
 */
#include "temperature.h"
#include "twi_master.h"

/* Interrupt Macros */
#define _BIT(x)					(0x01 << x)

#define NCT75_I2C_ADDRESS 		0x48
#define NCT75_WRITE				(NCT75_I2C_ADDRESS << 1)	// 0x3A
#define NCT75_READ				(NCT75_WRITE | 0x1)			// 0x3B

/* Bit fields */
#define NCT75_ONE_SHOT_CONFIG	_BIT(5)		// One shot bit for

/* Register Map for the NCT75. */
enum {
	REGNCT75_STORED_TEMP = 0x00,	// Read-only register.
	REGNCT75_CONFIG,
	REGNCT75_THYST,
	REGNCT75_TOS,
	REGNCT75_ONE_SHOT
};


/* Configures single shot mode for power savings.
 * In single-shot device is powered down and wakes only when asked for temp.
 * */
void temperature_init()
{
	uint8_t buffer[2];
	buffer[0] = REGNCT75_CONFIG;
	buffer[1] = NCT75_ONE_SHOT_CONFIG;

	twi_master_transfer(NCT75_WRITE,
						buffer,
						sizeof(buffer),
						true);

	// Test it out:
	memset(&buffer, 0, sizeof(buffer));
	temperature_read(buffer);
}


// Reads the current temperature 12 bit value as a one-shot operation.
void temperature_read(uint8_t *temperature)
{
	uint8_t reg = REGNCT75_ONE_SHOT;
	twi_master_transfer(NCT75_WRITE,
						&reg,
						sizeof(uint8_t),
						true);

	reg = REGNCT75_STORED_TEMP;
	twi_master_transfer(NCT75_WRITE,
						&reg,
						sizeof(uint8_t),
						false);

	twi_master_transfer(NCT75_READ,
						temperature,
						sizeof(uint16_t),	// read two-bytes.
						true);
}
