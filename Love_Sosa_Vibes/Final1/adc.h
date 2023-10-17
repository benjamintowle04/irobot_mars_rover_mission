/*
 * acd.h
 *
 *  Created on: Mar 24, 2023
 *      Author: avick
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <math.h>
#include <inc/tm4c123gh6pm.h>

void adc_init(void);
uint16_t adcRead(void);
float adcDistance(void);

#endif /* ADC_H_ */
