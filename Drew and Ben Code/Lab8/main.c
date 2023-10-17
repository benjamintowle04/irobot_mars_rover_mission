/*
 * main.c
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

extern volatile char command_byte;
extern volatile int command_flag;

void main(void)
{

    oi_t *sensor_dataT = oi_alloc();
    oi_init(sensor_dataT);
    oi_update(sensor_dataT);
    lcd_init();
    lcd_clear();
    timer_init();
    cyBOT_init_Scan(0b0111);
    adc_init();
    uart_init();
    uint32_t sample;
    cyBOT_Scan_t scan;
    cyBOT_Scan(90, &scan);

    //cyBOT_SERVRO_cal_t Cal;
    //Cal = cyBOT_SERVO_cal();
    right_calibration_value = 238000;  //bot8  //274750; //bot 9            //280000; //Cybot 4
    left_calibration_value = 1214500;//bot8  //1256500; //bot 9       //1261750; //Cybot 4

    while (1)
    {
        if (command_flag == 1)
        {
            if (command_byte == 'm')
            {
                measurement(sensor_dataT);
                command_flag = 0;
            }
            return 0;
        }
    }

    oi_free(sensor_dataT);
}

