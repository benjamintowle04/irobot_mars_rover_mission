/*
 * adc.c
 *
 *  Created on: Mar 24, 2023
 *      Author: btowle04
 */
#include "movement.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"
#include "uart-interrupt.h"
#include "uart.h"
#include "open_interface.h"
#include <inc/tm4c123gh6pm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "adc.h"

void adc_init(void) {
    SYSCTL_RCGCADC_R |= 0x0001;
    SYSCTL_RCGCGPIO_R |= 0x02;
    while((SYSCTL_PRGPIO_R & 0x10) != 0x010){};
    GPIO_PORTB_DIR_R &= ~0x10;
    GPIO_PORTB_AFSEL_R |= 0x10;
    GPIO_PORTB_DEN_R &= ~0x10;
    GPIO_PORTB_AMSEL_R |= 0x10;
    //while((SYSCTL_PRADC_R & 0x0001) != 0x0001){};

    //ADC0_PC_R &= ~0xF;
    //ADC0_PC_R |= 0x1;
    //ADC0_SSPRI_R = 0x0123;
    ADC0_ACTSS_R &= ~0x0008;
    ADC0_EMUX_R &= ~0xF000;
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R += 0xA;
    ADC0_SSCTL3_R = 0x0006;
    ADC0_IM_R &= ~0x0008;
    ADC0_ACTSS_R |= 0x0008;

}

uint32_t adc_read(void) {
    uint32_t result;
    float ir;
    ADC0_PSSI_R |= 0x0008;
    while((ADC0_RIS_R & 0x08) == 0) {};
    result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R |= 0x0008;
    ir = ir_convert(result);

    return ir;
}

uint32_t measurement(oi_t* sensor_dataT) {
    int i = 0;
    int r = 90;
    int t;
    int c;
    int sum;

    char *data = (char*)malloc(10 * sizeof(char));
    cyBOT_Scan_t scan;


    for(i = 0; i < 20; i++) {
        for(c = 0; c < 17; c++) {
            cyBOT_Scan(r,  &scan);
            sum += adc_read();
        }
        sprintf(data, "\r%d\t\t\t%0.3f\t\t\t%d", i, scan.sound_dist, adc_read());
        for(t = 0; data[t]; t+=1) {
            uart_sendChar(data[t]);
        }
        uart_sendChar('\n');
        moveForward(sensor_dataT, 20);
    }
    return 0;
}

float ir_convert(uint32_t irVal) {
    float distance = 0;
    float a;
    float b;
    float c;
//    lcd_init();
//    adc_init();

    distance = (((2*12.511)/(1630.3 + sqrt(pow(1630.3, 2) - (4*12.511*52035))))/irVal)*1000000; //bot 11
    lcd_printf("Ir Value: %d \n distance %f (cm)", irVal, distance);
    return distance;
}



