/*
 * acd.c
 *
 *  Created on: Mar 24, 2023
 *      Author: avick
 */

#include <adc.h>
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "timer.h"


void adc_init(void){

    SYSCTL_RCGCADC_R |= 0b000001;

    SYSCTL_RCGCGPIO_R |= 0b000010;

    while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
    while ((SYSCTL_PRADC_R & 0x01) == 0) {};


    GPIO_PORTB_DIR_R &= 0xEF;

    GPIO_PORTB_AFSEL_R |= 0x10;   //Initilizes PB4

    GPIO_PORTB_DEN_R |= 0xEF;     //Allows PB4's analog function of AIN10 to be used by pushing a 0 onto bit 4 location

    GPIO_PORTB_AMSEL_R |= 0x10;;

    ADC0_ACTSS_R &= 0x7; //May not be needed but file said needs to be disabled to be enabled so we disable it before enabling

    ADC0_EMUX_R &= 0x0FFF; //Configure the trigger event for the sample sequencer

    ADC0_SSMUX3_R &= 0x0; //Sample channel

    ADC0_SSMUX3_R += 10; // maay need to change if channel is indeed not 9

    ADC0_SSCTL3_R &= 0x6; // Sets behavior of ADC
    ADC0_SSCTL3_R |= 0x6;

    ADC0_ACTSS_R |= 0x8;

}

uint16_t adcRead(void){
    uint16_t result;
    ADC0_SAC_R = 3;
    ADC0_PSSI_R = 0x0008;

        while((ADC0_RIS_R & 0x08) == 0){}   //wait for code to be converted

        result = ADC0_SSFIFO3_R & 0xFFF;    //grab value
        ADC0_ISC_R = 0x0008;    //clear flags


    return result;

}

float adcDistance(void){
    uint16_t scan;
    float result;
        scan = adcRead();
    result = 197656 * pow(scan, -1.25);

    return result;

}



