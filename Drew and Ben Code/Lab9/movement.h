/*
 * movement.h
 *
 *  Created on: Feb 10, 2023
 *      Author: btowle04
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"


#endif /* MOVEMENT_H_ */


double moveForward(oi_t*sensor_data, double distance);
double turnRight(oi_t*sensor_data, double angle);
double turnLeft(oi_t*sensor_data, double angle);
double moveBackward(oi_t*sensor_data, double distance);
int bumper(oi_t*sensor_data);
void bump_Check(oi_t*sensor_data, double distance_mm);
