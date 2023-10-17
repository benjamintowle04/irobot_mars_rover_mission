/*
 * movement.c
 *
 *  Created on: Feb 10, 2023
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
#include "main.h"

double moveForward(oi_t*sensor_data, double distance_mm) {
    oi_setWheels(100,100);
    double sum = 0;
    while (sum < distance_mm) {
        oi_update(sensor_data);
        sum += sensor_data->distance;
        if(bumper(sensor_data) == 1) {
            bump_Check(sensor_data,150);
            scan_Objects(sensor_data);
            uart_sendChar('X');
        }
    }
    oi_setWheels(0,0);
    return sum;
}


double turnRight(oi_t*sensor_data, double angle) {
    oi_setWheels(-50,50);
    double sum = 0;
    while (sum < angle) {
        oi_update(sensor_data);
        sum -= sensor_data->angle;
    }

    oi_setWheels(0,0);
    return sum;
}

double turnLeft(oi_t*sensor_data, double angle) {
    oi_setWheels(50,-50);
    double sum = 0;
    while (sum < angle) {
        oi_update(sensor_data);
        sum += sensor_data->angle;
    }

    oi_setWheels(0,0);
    return sum;
}

double moveBackward(oi_t*sensor_data, double distance) {
    oi_setWheels(-100,-100);
    double sum = 0;
    while(sum < distance) {
        oi_update(sensor_data);
        sum -= sensor_data->distance;
    }
    oi_setWheels(0,0);
    return sum;
}


int bumper(oi_t*sensor_data) {
    if(sensor_data->bumpLeft) {
        return 1;
    }else if(sensor_data->bumpRight){
        return 1;
    }else {
        return 0;
    }
}



void bump_Check(oi_t*sensor_data, double distance_mm) {
    oi_setWheels(0,0);
    double sum = 0;
    if(sensor_data->bumpLeft){
        oi_setWheels(-100,-100);
        while(sum < distance_mm) {
            oi_update(sensor_data);
            sum -= sensor_data->distance;
        }
        turnRight(sensor_data, 90);
        moveForward(sensor_data, 250);
        turnLeft(sensor_data, 90);
        moveForward(sensor_data, 250);
        turnLeft(sensor_data, 90);
    }
    if(sensor_data->bumpRight){
        oi_setWheels(-100,-100);
        while(sum < distance_mm) {
            oi_update(sensor_data);
            sum -= sensor_data->distance;
        }
        turnLeft(sensor_data, 90);
        moveForward(sensor_data, 250);
        turnRight(sensor_data, 90);
        moveForward(sensor_data, 250);
        turnRight(sensor_data, 90);
    }
    uart_sendChar('Y');
    //moveForward(sensor_data, 150);
    oi_setWheels(0,0);
}
