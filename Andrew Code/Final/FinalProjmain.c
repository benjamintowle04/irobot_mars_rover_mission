/**
 * Final Project Main
 * @author Andrew Vick
 * 4/17/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"

#include "uart-interrupt.h" //used for sending and receiving through PuTTy
#include "adc.h"    //Better PING and IR values
#include "open_interface.h" //CyBot API
#include "movement.h"   //Controls the movement of the cyBot and object checks
#include "servo.h"
#include "ping_template.h"

//Note data for love sosa and timing of notes
#define NOTE_E4 52
#define NOTE_G6 67
#define NOTE_F5 66
#define NOTE_D3 50
unsigned char Love_SosaNotes[] = { NOTE_E4, NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5,
                                   NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5, NOTE_E4,
                                   NOTE_D3, NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5,
                                   NOTE_E4, NOTE_F5, NOTE_G6, NOTE_F5, NOTE_E4,
                                   NOTE_F5, NOTE_E4, NOTE_D3, NOTE_G6, NOTE_E4,
                                   NOTE_F5, NOTE_E4, NOTE_F5, NOTE_G6, NOTE_F5,
                                   NOTE_E4, NOTE_F5, NOTE_E4, NOTE_D3, NOTE_G6,
                                   NOTE_F5, NOTE_E4, NOTE_F5 }; // NOTE_E4, NOTE_F5,NOTE_G6, NOTE_F5,NOTE_E4, NOTE_F5, NOTE_E4, NOTE_D3, NOTE_D3, NOTE_G6, NOTE_F5,NOTE_E4, NOTE_F5}; //end of thrid segment
unsigned char duration[] = { 20, 20, 20, 60, 40, 20, 20, 20, 20, 40, 40, 20, 20,
                             20, 20, 40, 40, 20, 20, 20, 20, 40, 40, 40, 20, 20,
                             40, 40, 20, 20, 20, 20 }; //, 49, 49, 20, 20, 20, 20, 49, 20, 20, 20, 20, 20, 20};

int song_index = 0;
int num_notes = 38; //51

typedef struct
{
    int startingAngle;
    int endingAngle;
    int middleAngle;
    double linearWidth;
    float distance;
} object;

void scan()
{
    float sonarVals[180] = {0};
    short irVals[180] = {0};

    char temp[40] = "";
    int i;
    int x;
    int angle = 0;
    int objectCount = 0;
    char data[180] = {0};

    object items[15];
    for (i = 0; i < 15; i++)
    {
        items[i].startingAngle = 0;
        items[i].endingAngle = 0;
        items[i].middleAngle = 0;
        items[i].linearWidth = 0;

    }

    int objectFound = 0;
    int degreesFound = 0;
    for (angle = 0; angle <= 180; angle += 1)
    {

        servomotor_move(angle);
        int IRsum = adcRead();
        float SONARsum = ping_getDistance();
        irVals[angle] = IRsum;
        sonarVals[angle] = SONARsum;
        if (angle == 90)
        {

            int len = sprintf(temp, "/Scan 50 percent done\r", angle);
            uart_sendStr(temp, len);
        }

          timer_waitMillis(100);
//        int len = sprintf(data, "%d\t\t%.2lf\t\t%d\r",angle, SONARsum, IRsum);
//        uart_sendStr(data, len);

    }

    for (i = 0; i <= 180; ++i)
    {
        // Then if the IR value is still high enough, update the degreesFound, and the ending angle
        if (irVals[i] >= 500 && objectFound == 0)
        {
            items[objectCount].startingAngle = i;
            //items[objectCount].endingAngle = i;
            objectFound = 1;
            degreesFound = 0;
            // If the IR value is not high enough
        }
        if (irVals[i - 1] >= 500 && irVals[i] <= 500 && degreesFound > 0 && objectFound == 1)
        {
            if (degreesFound < 4)
            {
                objectFound = 0;
                items[objectCount].startingAngle = 0;
                items[objectCount].endingAngle = 0;
                degreesFound = 0;
                // Otherwise, store the final angle value, and add one to the objectCount
            }
            else if (degreesFound > 5)
            {
                objectFound = 0;
                items[objectCount].endingAngle = i;
                items[objectCount].middleAngle =
                        (items[objectCount].startingAngle
                                + items[objectCount].endingAngle) / 2;

                objectCount++;
                degreesFound = 0;
//                if (items[objectCount].startingAngle == 0
//                        && items[objectCount].endingAngle == 0)
//                {
//                    objectFound = 0;
//                    items[objectCount].startingAngle = 0;
//                    items[objectCount].endingAngle = 0;
//                    degreesFound = 0;
//                    objectCount--;
//                }
            }

        }
        else{
            degreesFound++;
        }
//        if ((sonarVals[i - 1] - sonarVals[i]) > 50 && !objectFound && sonarVals[i] < 100.0) //Implement object detection falling int degreesFound < 5 since only looking for starting edge so degreesFound will always be 1 when hitting the edge of an object need to implement object found
//        {
//            items[objectCount].startingAngle = i;
//            items[objectCount].endingAngle = i;
//            degreesFound++;
//            objectFound = 1;
//        }
//        if ((sonarVals[i - 1] - sonarVals[i]) < -5 && degreesFound > 0 && objectFound) //Object ended
//        {
//
//            items[objectCount].endingAngle = i;
//
//            if (items[objectCount].endingAngle
//                    - items[objectCount].startingAngle > 5)
//            {
//                objectFound = 0;
//                items[objectCount].middleAngle =
//                        (items[objectCount].startingAngle
//                                + items[objectCount].endingAngle) / 2;
//
//                objectCount++;
//                degreesFound = 0;
//            }
//            else
//            {
//                objectFound = 0;
//                items[objectCount].startingAngle = 0;
//                items[objectCount].endingAngle = 0;
//                degreesFound = 0;
//            }
//
//        }

        // If there is an object currently being scanned, and the end of the scan happens
//        if (i == 179)
//        {
//            objectFound = 0;
//            // Then do a check if the object is less than 5 degrees clearing it if it is
//            if (degreesFound < 5)
//            {
//                items[objectCount].startingAngle = 0;
//                items[objectCount].endingAngle = 0;
//                degreesFound = 0;
//                // Otherwise, store the final angle value, and add one to the objectCount
//            }
//            else
//            {
//                items[objectCount].middleAngle =
//                        (items[objectCount].startingAngle
//                                + items[objectCount].endingAngle) / 2;
//
//                objectCount++;
//                degreesFound = 0;
//
//            }
//        }
    }

    for (i = 0; i < objectCount; i++)
    {
        // Find angular width
        int angle = items[i].endingAngle - items[i].startingAngle;
        float distance = sonarVals[items[i].middleAngle];
        float angleRads = (angle * 3.1415) / 180.0;
        float width = (2*distance)*(tan(angleRads/2));
        items[i].linearWidth = width;
    }

    /**
     * sending object data to PuTTY to then be sent to pyhton for
     * plotting the objects on the GUI
     */
    int len;
    for (i = 0; i < objectCount; i++)
    {
        sprintf(temp, "%d   %.2lf   %.2lf\r",items[i].middleAngle, sonarVals[items[i].middleAngle], items[i].linearWidth);
        len = strlen(temp);
        uart_sendStr(temp, len);
    }

    // After all the math is done, go through the array again to find the smallest object and store it's index
    i = 0;
    int smallestIndex = 0;
    for (i = 1; i < objectCount; i++)
    {
        if (items[i].linearWidth < items[smallestIndex].linearWidth)
        {
            smallestIndex = i;
        }
    }

}

void control(oi_t *sensor_data)
{
    /**
     *
     *Switch Scan to use our servo code and not scan library
     *
     */

    while (true)
    {
        if (command_flag == 1)
        {
            if (command_byte == 'w')
            {

                move_forward_checks(100, sensor_data); //moves forward correctly with the correct distance given(bug: if i call it multiple time i.e. hitting 'w' after movement is complete eventually it'll move for longer than suppose to)
                command_flag = 0;
            }
            else if (command_byte == 's')
            {
                backup(sensor_data, 80, 1);
                command_flag = 0;
            }
            else if (command_byte == 'a') //'a' and 'd' are used to turn the cybot in 15 degree chunks
            {
                turn_left(sensor_data, 45);
                command_flag = 0;
            }
            else if (command_byte == 'd')
            {
                turn_right(sensor_data, 45);
                command_flag = 0;
            }
            else if (command_byte == 'e')
            { //'e' and 'q' will be used to turn the cybot a finite amount for lining up entry into final goal
                turn_right(sensor_data, 1);
                command_flag = 0;
            }
            else if (command_byte == 'q')
            {
                turn_left(sensor_data, 1);
                command_flag = 0;
            }
            else if (command_byte == 't')
            {
                scan();
                command_flag = 0;
            }
            else if (command_byte == 'p')
            {
                oi_play_song(song_index);
                command_flag = 0;
            }

        }
        else
        {
            continue;
        }
    }
}

int main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    uart_interrupt_init();
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    servomotor_init();
    adc_init();
    ping_init();

    servomotor_move(90);
    oi_loadSong(song_index, num_notes, Love_SosaNotes, duration); //Loads love sosa

    control(sensor_data);

    oi_free(sensor_data);
    return 0;

}

