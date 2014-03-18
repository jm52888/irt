/*
*******************************************************************************
*
* By Jason De Lorme <jjdelorme@yahoo.com>
* http://www.roadacious.com
*
********************************************************************************/

#include "power.h"
#include "app_error.h"
#include "speed.h"

#define	MATH_PI			3.14159265358979f

static const float slope[10] = { 0, 2.6, 3.8, 5.0, 6.1, 7.1, 8.2, 9.2, 10.1, 11.0 };
static const float intercept [10] = { 0, -9.60, -18.75, -25.00, -28.94, -29.99, -29.23, -26.87, -20.90, -13.34 };

// Hang on to accumulated torque for a session duration.
static uint16_t m_accum_torque = 0;

int16_t __DEBUG_POWER[2];

static float calc_mag0_force(float weight_kg, float speed_mps)
{
#define SPEED_FORCE_A_SLOPE 			0.194426f
#define SPEED_FORCE_A_INTERCEPT 	23.45062f
#define SPEED_FORCE_A_WEIGHT			94.34714f
#define SPEED_FORCE_B_SLOPE 			-0.03644f
#define SPEED_FORCE_B_INTERCEPT 	22.31482f
#define SPEED_FORCE_B_WEIGHT			79.83219f

	float force = (speed_mps*SPEED_FORCE_A_SLOPE + SPEED_FORCE_A_INTERCEPT) -
		(((speed_mps*SPEED_FORCE_A_SLOPE + SPEED_FORCE_A_INTERCEPT) -
		(speed_mps*SPEED_FORCE_B_SLOPE + SPEED_FORCE_B_INTERCEPT)) / (SPEED_FORCE_A_WEIGHT - SPEED_FORCE_B_WEIGHT))*
		(SPEED_FORCE_A_WEIGHT - weight_kg);

	return force;
}

static float calc_servo_force(float speed_mps, uint16_t servo_pos)
{
#define FORCE_SERVO_A_SLOPE 			-0.037121681f
#define FORCE_SERVO_A_INTERCEPT 	56.97064764f
#define FORCE_SERVO_A_SPEED				8.94f
#define FORCE_SERVO_B_SLOPE 			-0.036220909f
#define FORCE_SERVO_B_INTERCEPT 	53.76137452f
#define FORCE_SERVO_B_SPEED			 	4.47f

// Assuming no additional force exists beyond this servo position (mag off).
#define MIN_SERVO_FORCE_POS				1489

	if (servo_pos > MIN_SERVO_FORCE_POS)
		return 0;

	float force = (servo_pos*FORCE_SERVO_A_SLOPE + FORCE_SERVO_A_INTERCEPT) -
		(((servo_pos*FORCE_SERVO_A_SLOPE + FORCE_SERVO_A_INTERCEPT) -
		(servo_pos*FORCE_SERVO_B_SLOPE + FORCE_SERVO_B_INTERCEPT)) / (FORCE_SERVO_A_SPEED - FORCE_SERVO_B_SPEED))*
		(FORCE_SERVO_A_SPEED - speed_mps);

	return force;
}

/* Calculates angular velocity based on wheel ticks and time.
static float calc_angular_vel(uint8_t wheel_ticks, uint16_t period_2048)
{
	float value;

	value = (2.0f * MATH_PI * wheel_ticks) / (period_2048 / 2048.0f);

	return value;
}*/

uint16_t power_watts_calc(float speed_mps, float weight_kg,	uint16_t servo_pos)
{
	int16_t watts;
	
	if (speed_mps == 0)
	{
		watts = 0;
	}
	else
	{
		float mag0_force = calc_mag0_force(weight_kg, speed_mps);
		float servo_force = calc_servo_force(speed_mps, servo_pos);
		watts = (uint16_t) ((mag0_force + servo_force) * speed_mps);
	}

	return watts;
}

// TODO: This only works if you have a predefined LEVEL 0-9.  I need to make it 
// more dynamic given any position the servo might have from min->max.
// We should also be able to calc power far more acurately based on flywheel revs
// instead of speed.
/*
uint8_t old_power_calc_watts(float speed_mph, float total_weight_lb,
	uint8_t resistance_level, int16_t* p_watts)
{
	int16_t power = 0;
	
	if (resistance_level > 9)
	{
		return IRT_ERROR_INVALID_RESISTANCE_LEVEL;
	}	

	if (speed_mph == 0)
	{
		power = 0;
	}
	else
	{
		// All calculations start with what level 0 (no resistance) would be.
		float level0 = (speed_mph*14.04 - 33.6) - (((speed_mph*14.04 - 33.06) -
			(speed_mph*8.75 - 16.21)) / 90)*(220 - total_weight_lb);

		if (resistance_level == 0)
		{
			power = (int16_t)level0;
		}
		else 
		{
			// TODO: Verify truncation vs. rounding that may occur here? Watts needs to
			// be returned as a signed 16 bit integer.
			power = (int16_t)(level0 + speed_mph *
				slope[resistance_level] + intercept[resistance_level]);
		}
	}
	*p_watts = power;

	return IRT_SUCCESS;
}*/

uint16_t power_torque_calc(int16_t watts, uint16_t period_seconds_2048)
{
	uint16_t torque;

	if (watts == 0 || period_seconds_2048 == 0)
	{
		torque = 0;
	}
	else
	{
		torque = (watts * period_seconds_2048) / (128 * MATH_PI);
	}
	
	return torque;
}

/**@brief	Reads speed and calculates power.
 *
 */
int32_t power_measure(float total_weight_kg, irt_power_meas_t* p_power_meas)
{
	uint32_t err_code;
	uint16_t torque;

	// Calculate speed.
	err_code = calc_speed(p_power_meas);
	APP_ERROR_CHECK(err_code);

	/*float speed_mph = get_speed_mph(speed_event.speed_mps);
	/*err_code = calc_power(speed_mph, m_user_profile.total_weight_kg * 2.20462262, m_resistance_level, &watts);
	// TODO: Handle the error for real here, not sure what the overall error
	// handling strategy will be, but this is not critical, just move on.
	if (err_code != IRT_SUCCESS)
		return;
	*/

	// Calculate power.
	p_power_meas->instant_power = power_watts_calc(
			p_power_meas->instant_speed_mps,
			total_weight_kg,
			p_power_meas->servo_position);

	// Calculate torque.
	torque = power_torque_calc(p_power_meas->instant_power,
			p_power_meas->wheel_period_2048);

	// Store accumulated torque for the session.
	m_accum_torque += torque;
	p_power_meas->accum_torque = m_accum_torque;

	return IRT_SUCCESS;
}
