/* 
Analog to Digital Converter wrapper.

Copyright (c) Inside Ride Technologies, LLC 2014
All rights reserved.
*/

#ifndef IRT_ADC_H
#define IRT_ADC_H

/**@brief	Callback.
 *
 */
typedef void (*adc_result_t)(uint16_t);

/**@brief	Initiate a read of the ADC.
 * 			Returns NRF_ERROR_BUSY if already reading.
 *
 */
uint32_t irt_adc_start(void);

/**@brief	Initialize and configure with ADC configuration.
 *
 */
void irt_adc_init(uint32_t nrf_adc_config, adc_result_t on_adc_result);

#endif // IRT_ADC_H
