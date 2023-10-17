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
                                   NOTE_G6,
                                   NOTE_F5, NOTE_E4, NOTE_F5, NOTE_E4,
                                   NOTE_D3,
                                   NOTE_G6, NOTE_F5, NOTE_E4, NOTE_F5,
                                   NOTE_E4,
                                   NOTE_F5, NOTE_G6, NOTE_F5, NOTE_E4,
                                   NOTE_F5,
                                   NOTE_E4, NOTE_D3, NOTE_G6, NOTE_E4,
                                   NOTE_F5,
                                   NOTE_E4, NOTE_F5, NOTE_G6, NOTE_F5,
                                   NOTE_E4,
                                   NOTE_F5, NOTE_E4, NOTE_D3, NOTE_G6,
                                   NOTE_F5,
                                   NOTE_E4, NOTE_F5 };
unsigned char duration[] = { 20, 20, 20, 60, 40, 20, 20, 20, 20, 40, 40, 20, 20,
                             20, 20, 40, 40, 20, 20, 20, 20, 40, 40, 40, 20, 20,
                             40, 40, 20, 20, 20, 20 };

int song_index = 0;
int num_notes = 38;
int len;

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
    float sonarVals[180] = { 0 };
    short irVals[180] = { 0 };

    char temp1[40] = "";
    char temp[180] = "";
    int i;
    int angle = 0;
    int objectCount = 0;

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
        if (angle == 90 || angle == 180)
        {

            sprintf(temp1, "/Scan angle %d", angle);
            len = strlen(temp1);
            uart_sendStr(temp1, len);
        }

        timer_waitMillis(80);

    }

    for (i = 0; i <= 180; ++i)
    {
        // Then if the IR value is still high enough, update the degreesFound, and the ending angle
        if (irVals[i] >= 500 && objectFound == 0)
        {
            items[objectCount].startingAngle = i;
            objectFound = 1;
            degreesFound = 0;
            // If the IR value is not high enough
        }
        if (irVals[i - 1] >= 500 && irVals[i] <= 500 && degreesFound > 0
                && objectFound == 1)
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
            }

        }
        else
        {
            degreesFound++;
        }
    }

    for (i = 0; i < objectCount; i++)
    {
        // Find angular width
        int angle = items[i].endingAngle - items[i].startingAngle;
        float distance = sonarVals[items[i].middleAngle];
        float angleRads = (angle * 3.1415) / 180.0;
        float width = (2 * distance) * (tan(angleRads / 2));
        items[i].linearWidth = width;
    }

    /**
     * sending object data to PuTTY to then be sent to pyhton for
     * plotting the objects on the GUI
     */
    int len;
    for (i = 0; i < objectCount; i++)
    {
        sprintf(temp, "%d   %.2lf   %.2lf\r", items[i].middleAngle,
                sonarVals[items[i].middleAngle], items[i].linearWidth);
        len = strlen(temp);
        uart_sendStr(temp, len);
    }

}

void control(oi_t *sensor_data)
{
    /**
     *
     *Switch Scan to use our servo code and not scan library
     *
     */
    int degreesTurned = 90;
    int len = 0;
    int turnKeyPress = 0;
    int tKeyPress = 0;
    char cyAngle[35] = { 0 };
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
                degreesTurned += 45;
                turnKeyPress = 1;
                command_flag = 0;
            }
            else if (command_byte == 'd')
            {
                turn_right(sensor_data, 45);
                degreesTurned -= 45;
                turnKeyPress = 1;
                command_flag = 0;
            }
            else if (command_byte == 'e')
            { //'e' and 'q' will be used to turn the cybot a finite amount for lining up entry into final goal
                turn_right(sensor_data, 1);
                degreesTurned = degreesTurned - 2;
                turnKeyPress = 1;
                command_flag = 0;
            }
            else if (command_byte == 'q')
            {
                turn_left(sensor_data, 1);
                degreesTurned = degreesTurned + 2;
                turnKeyPress = 1;
                command_flag = 0;
            }
            else if (command_byte == 't')
            {
                scan();
                tKeyPress = 1;
                command_flag = 0;
            }
            else if (command_byte == 'p')
            {
                oi_play_song(song_index);
                command_flag = 0;
            }

            if ((degreesTurned >= 270 || degreesTurned <= -90)
                    && (turnKeyPress != 0))
            {
                degreesTurned = 90;
                sprintf(cyAngle, "/Turned around Angle: %d", degreesTurned);
                len = strlen(cyAngle);
                turnKeyPress = 0;
                tKeyPress = 0;
                uart_sendStr(cyAngle, len);

            }
            else if ((degreesTurned < 270 || degreesTurned > -180)
                    && turnKeyPress != 0)
            {
                sprintf(cyAngle, "/Angle: %d", degreesTurned);
                len = strlen(cyAngle);
                turnKeyPress = 0;
                uart_sendStr(cyAngle, len);
            }
            if(tKeyPress == 1){
                degreesTurned = 90;
                tKeyPress = 0;
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

