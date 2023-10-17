/*
 * movement.c
 *
 *  Created on: Feb 10, 2023
 *      Author: btowle04
 */
#include "Timer.h"
#include "lcd.h"
#include "movement.h"
#include "open_interface.h"

double moveForward(oi_t*sensor_data, double distance) {
    oi_setWheels(500,500);
    double sum = 0;
    while(sum < distance) {
        oi_update(sensor_data);
        sum += sensor_data->distance;
    }
    oi_setWheels(0,0);
    return sum;
}

double turnRight(oi_t*sensor_data, double angle) {
    oi_setWheels(-50,50);
    double sum = 0;
    while(sum < angle) {
        oi_update(sensor_data);
        sum -= sensor_data->angle;
    }

    oi_setWheels(0,0);
    return sum;
}

double turnLeft(oi_t*sensor_data, double angle) {
    oi_setWheels(50,-50);
    double sum = 0;
    while(sum < angle) {
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
