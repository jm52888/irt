/*
(1) Launch developer command prompt:
%comspec% /k ""C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\VsDevCmd.bat""
(2) build with debug symbols.
cl test.c /ZI 

cl test.c ../emotion/libraries/math/acosf.c ../emotion/libraries/math/sqrtf.c ../emotion/libraries/math/cosf.c ../emotion/libraries/math/sinf.c ../emotion/libraries/math/fabsf.c ../emotion/libraries/math/rem_pio2f.c /I ../emotion/libraries/math

*/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <float.h>

typedef struct speed_event_s
{
	uint16_t	event_time_2048;											// Event time in 1/2048s.
	uint16_t	accum_flywheel_ticks;										// Currently 2 ticks per flywheel rev.
} speed_event_t;

static int speed_calc_mps(speed_event_t first, speed_event_t last)
{
	return first.event_time_2048 + last.event_time_2048;
}

int main(int argc, char *argv [])
{
	uint16_t total = 0;

	speed_event_t event1;
	speed_event_t event2;

	event1.event_time_2048 = 1;
	event2.event_time_2048 = 2;

	speed_event_t* p_event1;
	speed_event_t* p_event2;

	p_event1 = &event1;
	p_event2 = &event2;

	total = speed_calc_mps(*p_event1, event2);

	printf("total: %i\r\n", total);
	return 0;
}
