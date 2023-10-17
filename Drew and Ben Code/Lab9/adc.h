/*
 * adc.h
 *
 *  Created on: Mar 24, 2023
 *      Author: btowle04
 */

#ifndef ADC_H_
#define ADC_H_
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <string.h>

void adc_init(void);

uint32_t adc_read(void);
float ir_convert(uint32_t irVal);
uint32_t measurement(oi_t* sensor_dataT);


#endif /* ADC_H_ */
