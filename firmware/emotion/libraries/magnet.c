/******************************************************************************
 * Copyright (c) 2015 Inside Ride Technologies, LLC. All Rights Reserved.
 *
 * @file  	magnet.c
 * @brief	Encapsulates the calculations for magnetic resistance.
 *
 */

#include <stdint.h>
#include <float.h>
#include "magnet.h"
#include "math.h"
#include "debug.h"

/**@brief Debug logging for module.
 *
 */
#ifdef ENABLE_DEBUG_LOG
#define MAG_LOG debug_log
#else
#define MAG_LOG(...)
#endif // ENABLE_DEBUG_LOG

#define COEFF_COUNT			4				// Cubic poynomial has 4 coefficients.
#define MAX_POSITION		1464
#define MIN_POSITION		800

/**@brief	Calculates the coefficient values for a cubic polynomial
 *			that plots a power curve for the magnet at a given speed.
 *
 *@note		Uses linear interpolation to calculate each coefficient
 *			value based on known curves for 15 mph & 25 mph.
 */
static void curve_coeff(float speed_mps, float *coeff)
{
	static const float SPEED1 =	15.0 * 0.44704;	// Convert to meters per second
	static const float SPEED2 = 25.0 * 0.44704;	// Convert to meters per second

	// 15 mph (6.7056 mps)
	static const float COEFF_1 [] = {
		0.00000233333,
		-0.0078375,
		8.044166667,
		-2277 };

	// 25 mph (11.176 mph)
	static const float COEFF_2 [] = {
		0.00000508333,
		-0.017,
		17.60666667,
		-5221 };

	for (uint8_t ix = 0; ix < COEFF_COUNT; ix++)
	{
		coeff[ix] = COEFF_1[ix] +
			((speed_mps - SPEED1) / (SPEED2 - SPEED1)) *
			(COEFF_2[ix] - COEFF_1[ix]);
	}
}

/**@brief	Calculates watts added by the magnet for a given speed at magnet
 *			position.
 */
float magnet_watts(float speed_mps, uint16_t position)
{
	float coeff[COEFF_COUNT];
	float watts;

	curve_coeff(speed_mps, coeff);

	watts =
		coeff[0] * pow(position, 3) +
		coeff[1] * pow(position, 2) +
		coeff[2] * position +
		coeff[3];

	return watts;
}

/**@brief	Calculates magnet position for a given speed and watt target.
 *
 */
uint16_t magnet_position(float speed_mps, float mag_watts)
{
	float coeff[COEFF_COUNT];

	// A set of math-intensive formula friendly names.
	#define a	coeff[0]
	#define b	coeff[1]
	#define c	coeff[2]
	#define d	coeff[3]

	float f, g, h, r, m, m2, n, n2, theta, rc;
	float /*x1, x2,*/ x2a, x2b, x2c, x2d, x3;
	int8_t k;

	// Interpolate to calculate the coefficients of the position:pwoercurve.
	curve_coeff(speed_mps, coeff);

	// To solve for a specific watt target, subtract from coefficient d.
	d = d - mag_watts;

	//<!--EVALUATING THE 'f'TERM-->
	f = (((3 * c) / a) - (((b*b) / (a*a)))) / 3;

	//<!--EVALUATING THE 'g'TERM-->
	g = ((2 * ((b*b*b) / (a*a*a)) - (9 * b*c / (a*a)) + ((27 * (d / a))))) / 27;

	//<!--EVALUATING THE 'h'TERM-->
	h = (((g*g) / 4) + ((f*f*f) / 27));

	/* Original code adopted from javascript website, need to refactor, but it
	 * works.  Code could solve for 3 solutions (x1, x2, x3) given a cubic
	 * polynomial, however we only need to solve for the last form (x3).
	 */
	if (h > 0)
	{
		m = (-(g / 2) + (sqrt(h)));

		//<!--K is used because math.pow cannot compute negative cube roots-->
		k = 1;
		if (m < 0) k = -1; else k = 1;
		m2 = pow((m*k), (1.0 / 3.0));
		m2 = m2*k;
		k = 1;
		n = (-(g / 2) - (sqrt(h)));
		if (n < 0) k = -1; else k = 1;
		n2 = (pow((n*k), (1.0 / 3.0)));
		n2 = n2*k;
		//<!-- - (S + U) / 2 - (b / 3a) + i*(S - U)*(3) ^ .5-->
		x3 = (-1 * (m2 + n2) / 2 - (b / (3 * a)));
	}
	else
	{
		//<!-- - (S + U) / 2 - (b / 3a) - i*(S - U)*(3) ^ .5-->

		r = ((sqrt((g*g / 4) - h)));
		k = 1;
		if (r < 0) k = -1;
		//<!--rc is the cube root of 'r' -->
		rc = pow((r*k), (1.0 / 3.0))*k;
		k = 1;
		theta = acos((-g / (2 * r)));
		//x1 = (2 * (rc*cos(theta / 3)) - (b / (3 * a)));
		x2a = rc*-1;
		x2b = cos(theta / 3);
		x2c = sqrt(3)*(sin(theta / 3));
		x2d = (b / 3 * a)*-1;
		//x2 = (x2a*(x2b + x2c)) - (b / (3 * a));
		x3 = (x2a*(x2b - x2c)) - (b / (3 * a));
	}

	return (uint16_t)x3;
}
