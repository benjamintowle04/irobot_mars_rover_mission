/*
 * movement.c
 *
 *  Created on: Feb 3, 2023
 *      Author: Andrew Vick
 */

#include "open_interface.h"
#include "movement.h"
#include "uart-interrupt.h"
double move_forward(oi_t *sensor_data, double distance_mm)
{
    oi_update(sensor_data);
    oi_setWheels(250, 250);
    double sum = 0;
    while (sum <= distance_mm)
    {
        oi_update(sensor_data);
        sum = sum + sensor_data->distance;
    }

    oi_setWheels(0, 0);

    return sum;

}

/**
 * This function allows use to safely move forward while checking different sensors on the robot to ensure we haven't hit any objects
 *
 * uses BumpCheck function ad CliffSensorCheck line 58 and 73 respectively
 */
void move_forward_checks(double distance_mm, oi_t *sensor_data)
{

    // oi_update(sensor_data);
    double traveled_distance = 0;

    oi_setWheels(200, 200);
    while (traveled_distance < distance_mm)
    {
        if (command_byte != 'w')
        { //since this function is used in one place currently we are allowed to exploit the command_byte as if the cyBot is moving forward the only case this will happen is if the byte is 'w'
          //This allows us to stop the cyBot's movement in case it was a mistake
            break;
        }

        oi_update(sensor_data);
        int bumpC = BumpCheck(sensor_data);
        int CliffCheck = CliffSensorCheck(sensor_data);
        if (CliffCheck == 1 || bumpC == 1)
        {
            break;
        }
        int check = theDreadedWhiteLineCheck(sensor_data);
        if (check == 1)
        {
            backup(sensor_data, 100, 0);
            break;
        }

        traveled_distance += sensor_data->distance;
        oi_update(sensor_data);
    }
    oi_setWheels(0, 0);

}
/**
 * checks if bump sensors have been activated if so stop the robot and reverse 10cm
 */
int BumpCheck(oi_t *sensor_data)
{
    // oi_update(sensor_data);
    char *temp;
    int len;
    if (sensor_data->bumpLeft == 1 || sensor_data->bumpRight == 1) //checks the 2 bump sensors
    {
        oi_setWheels(0, 0);
        temp = (char*) malloc(50 * sizeof(char));
        if (sensor_data->bumpLeft == 1 && sensor_data->bumpRight == 1)
        {
            int front;
            front = 1;
            sprintf(temp, "/Bumped Object Front %d\r", front);
            len = strlen(temp);
            uart_sendStr(temp, len);
        }
        else
        {
            sprintf(temp, "/Bumped Object Left: %d  Right: %d\r",
                    sensor_data->bumpLeft, sensor_data->bumpRight);
            len = strlen(temp);
            uart_sendStr(temp, len);
        }
        backup(sensor_data, 100, 0);
        return 1;

    }

    return 0;

}
/**
 * Uses the belly sensors to check if there is a cliff if so the robot will stop and backup 10cm
 */
int CliffSensorCheck(oi_t *sensor_data)
{
    char *temp;
    //oi_update(sensor_data);
    if (sensor_data->cliffFrontLeft == 1 || sensor_data->cliffFrontRight == 1
            || sensor_data->cliffLeft == 1 || sensor_data->cliffRight == 1)
    {   //Checks the 4 different cliff sensors

        oi_setWheels(0, 0);
        temp = (char*) malloc(50 * sizeof(char));
        sprintf(temp,
                "/Cliff Left: %d Front Left: %d -- Font Right: %d Right: %d\r",
                sensor_data->cliffLeft, sensor_data->cliffFrontLeft,
                sensor_data->cliffFrontRight, sensor_data->cliffRight);
        int len = strlen(temp);
        uart_sendStr(temp, len);
        timer_waitMillis(100);
        backup(sensor_data, 100,  0);
        return 1;
    }
    return 0;

}

int theDreadedWhiteLineCheck(oi_t *sensor_data)
{
    //oi_update(sensor_data);
    char *temp;
    int FL = 0;
    int L = 0;
    int FR = 0;
    int R = 0;
    if (sensor_data->cliffFrontLeftSignal >= 2700
            || sensor_data->cliffLeftSignal >= 2700
            || sensor_data->cliffFrontRightSignal >= 2700
            || sensor_data->cliffRightSignal >= 2700)
    {
        if (sensor_data->cliffFrontLeftSignal >= 2700)
        {
            FL = 1;
        }

        if (sensor_data->cliffLeftSignal >= 2700)
        {
            L = 1;
        }

        if (sensor_data->cliffFrontRightSignal >= 2700)
        {
            FR = 1;
        }

        if (sensor_data->cliffRightSignal >= 2700)
        {
            R = 1;
        }

        oi_setWheels(0, 0);
        temp = (char*) malloc(50 * sizeof(char));
        sprintf(temp,
                "/White Line Left: %d Front Left: %d -- Font Right: %d Right: %d\r",
                L, FL, FR, R);
        int len = strlen(temp);
        uart_sendStr(temp, len);
        return 1;

    }
    return 0;
}

double backup(oi_t *sensor_data, double distance_mm,int movementcall)
{
    //oi_update(sensor_data);
    double sum = 0;
    oi_setWheels(-100, -100);
    while (sum > (-1 * (distance_mm)))
    {
        oi_update(sensor_data);
        if(movementcall == 1){
            int check = theDreadedWhiteLineCheck(sensor_data);
            if (check == 1)
                    {
                        move_forward(sensor_data, 50);
                        break;
                    }
        }

        sum += sensor_data->distance;
    }

    oi_setWheels(0, 0);
    oi_update(sensor_data);

    return sum;

}
double turn_right(oi_t *sensor_data, double degrees)
{
    oi_update(sensor_data);
    float degreesRight = 0;
    oi_setWheels(-50, 50);
    while (degreesRight < degrees)
    {
        oi_update(sensor_data);
        degreesRight = degreesRight + (-1 * (sensor_data->angle));

    }

    oi_setWheels(0, 0);
    return degreesRight;

}

double turn_left(oi_t *sensor_data, double degrees)
{
    oi_update(sensor_data);
    float degreesLeft = 0;
    oi_setWheels(50, -50);
    while (degreesLeft < degrees)
    {
        oi_update(sensor_data);
        degreesLeft = degreesLeft + sensor_data->angle;
    }

    oi_setWheels(0, 0);

    return degreesLeft;

}

