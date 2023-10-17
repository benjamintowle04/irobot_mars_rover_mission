/*
 * movement.h
 *
 *  Created on: Feb 3, 2023
 *      Author: Andrew Vick
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

double  move_forward (oi_t *sensor_data,   double distance_mm);

double turn_right(oi_t *sensor_data, double degrees);

double turn_left(oi_t *sensor_data, double degrees);

void move_forward_checks(double distance_mm, oi_t *sensor_data);

double backup(oi_t *sensor_data, double distance_mm, int movementcall);

int CliffSensorCheck(oi_t *sensor_data);

int theDreadedWhiteLineCheck(oi_t *sensor_data);

int BumpCheck(oi_t *sensor_data);




#endif /* MOVEMENT_H_ */
