/**
 * Final Project Main
 * @author Andrew Vick
 * 4/17/2023
 */

#include <stdio.h>
#include <stdlib.h>
#include "Timer.h"
#include "lcd.h"
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

//typedef struct
//{
//    int startingAngle;
//    int endingAngle;
//    int middleAngle;
//    double linearWidth;
//} object;

typedef struct object {
    float distance;
    int angle;
    int width;
    int objNum;
    int ir_dist;
    float lin_width;
} obj;

float calcAverage(float values[]) {
    int i;
    float sum = 0.0;
    for(i=0; i < sizeof(values); i++) {
        sum += values[i];
    }
    return sum/(sizeof(values));
}

int calcMedian(int values[]) {
    int n = sizeof(values);

    if(n%2 == 0) {
        return values[n/2];
    }

    else {
        return values[n/2 + 1];
    }
}

void scan_Objects() {
    int r;
    int k;
    int c;
    int x;
    float pi = 3.14159265359;


    float* distance = (float*) malloc(100 * sizeof(float));   // All scan distance values stored in this array
    int* ir_distance = (int*) malloc(100 * sizeof(int));  //All scanned ir_distances
    float* obj_dist = (float*) malloc(100 * sizeof(float));  //Distance Values of 1 object stored in this array
    float* obj_IRdist = (float*) malloc(100 * sizeof(float));  //IR Distance Values of 1 object stored in this array
    int* obj_angle = (int*) malloc(100 * sizeof(int));
    char* data = (char*)malloc(100 * sizeof(char));
    int num = 0;

    //empty the struct array initially
    obj* items = (obj*) malloc(50 * sizeof(obj));




    r = 0;  //Iterate through struct objects
    k = 0;  //Iterate through angle and distance arrays in struct object
    c = 0;
    x = 0;
    while(c <= 180) {
        servomotor_move(c);
        distance[c] = ping_getDistance();
        ir_distance[c] = adcRead();


       //When we first encounter an object
        if(c > 0 && ir_distance[c] > 700 && ir_distance[c] > 650) {
                obj_dist[k] = ping_getDistance();
                obj_angle[k] = c;
                obj_IRdist[k] = adcRead();
                k++;
        }

        //When object is finished
        if(c > 0 && (ir_distance[c] < 700 && ir_distance[c-1] > 700) && k > 0) {
            if(k <= 5) {
                k=0;
                while(obj_dist[x] != 0) {
                    obj_dist[x] = 0;
                    obj_angle[x] = 0;
                    x++;
                }
               x=0;
            }
            if(k > 5) {
                items[r].objNum = r + 1;
                items[r].width = abs(obj_angle[k-1] - obj_angle[0]);
                items[r].angle = calcMedian(obj_angle);
                items[r].distance = calcAverage(obj_dist);
                items[r].ir_dist =  calcAverage(obj_IRdist);
                items[r].lin_width = (tan((items[r].angle * (pi/180.0))/2)*2*items[r].distance)/2;
                num=r;
                r++;
                k=0;
                while(obj_dist[x] != 0) {
                    obj_dist[x] = 0;
                    obj_angle[x] = 0;
                    x++;
                }
                x=0;
            }
        }

        c++;

    }
    int i = 0;
    for( i = 0; i < r; i++) {
        timer_waitMillis(200);
        int len = sprintf(data, "%d %0.2f  %0.2f\r", items[i].angle, items[i].distance, items[i].lin_width);
        uart_sendStr(data, len);

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

            lcd_printf("%c", command_byte);

            if (command_byte == 'w')
            {

                move_forward_checks(200, sensor_data); //moves forward correctly with the correct distance given(bug: if i call it multiple time i.e. hitting 'w' after movement is complete eventually it'll move for longer than suppose to)
                command_flag = 0;
            }
            else if (command_byte == 's')
            {
                backup(sensor_data, 80);
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
                scan_Objects();
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
    lcd_init();
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    uart_interrupt_init();
    adc_init();
    ping_init();
    servomotor_init();
    servomotor_move(90);
    oi_loadSong(song_index, num_notes, Love_SosaNotes, duration); //Loads love sosa

    control(sensor_data);

    oi_free(sensor_data);
    return 0;

}






//void scan()
//{
//    float sonarVals[180];
//    short irVals[180];
//
//    char *temp;
//
//    int i = 0;
//    // The objects could be stored in an array with the malloc, and free methods
//
//    for (i = 0; i <= 180; i++)
//    {
//        sonarVals[i] = 0;
//        irVals[i] = 0;
//    }
//
//    int angle = 0;
//
//    int objectCount = 0;
//
//    object items[15];
//    for (i = 0; i < 15; i++)
//    {
//        items[i].startingAngle = 0;
//        items[i].endingAngle = 0;
//        items[i].middleAngle = 0;
//        items[i].linearWidth = 0;
//
//    }
//
//    int objectFound = 0;
//    int degreesFound = 0;
//    for (angle = 0; angle <= 180; angle += 1)
//    {
//
//        servomotor_move(angle);
//        int IRsum = adcRead();
//        float SONARsum = ping_getDistance();
//        irVals[angle] = IRsum;
//        sonarVals[angle] = SONARsum;
//        timer_waitMillis(100);
////        int len = sprintf(data, "%d\t\t%.2lf\t\t%d\r",angle, SONARsum, IRsum);
////        uart_sendStr(data, len);
//
//    }
//
//    for (i = 0; i <= 180; ++i)
//    {
////        if (irVals[i] > 475 && ((i > 5 && i < 179) && !objectFound) || (sonarVals[i-1] - sonarVals[i] >= 100) && ((i>5 && i <179) && !objectFound))
////        {
////            // Store the starting angle of the object and also set the objectFound flag to true
////            items[objectCount].startingAngle = i;
////            items[objectCount].endingAngle = i;
////            objectFound++;
////
////            // If there is currently being an object being looked at
////        }
//        //else if (objectFound)
//        //{
//        // Then if the IR value is still high enough, update the degreesFound, and the ending angle
//        if (irVals[i] > 570)
//        {
//            items[objectCount].startingAngle = i;
//            items[objectCount].endingAngle = i;
//            degreesFound++;
//            // If the IR value is not high enough
//        }
//        if (irVals[i - 1] > 570 && irVals[i] < 570 && degreesFound > 0)
//        {
//            if (degreesFound < 5)
//            {
//                objectFound = 0;
//                items[objectCount].startingAngle = 0;
//                items[objectCount].endingAngle = 0;
//                degreesFound = 0;
//                // Otherwise, store the final angle value, and add one to the objectCount
//            }
//            else if (degreesFound > 5)
//            {
//                objectFound = 0;
//                items[objectCount].endingAngle = i;
//                items[objectCount].middleAngle =
//                        (items[objectCount].startingAngle
//                                + items[objectCount].endingAngle) / 2;
//
//                objectCount++;
//                degreesFound = 0;
//            }
//        }
//        if ((sonarVals[i - 1] - sonarVals[i]) > 10 && !objectFound && sonarVals[i] < 100.0) //Implement object detection falling int degreesFound < 5 since only looking for starting edge so degreesFound will always be 1 when hitting the edge of an object need to implement object found
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
////            if (degreesFound < 5)
////            {
////                objectFound = 0;
////                items[objectCount].startingAngle = 0;
////                items[objectCount].endingAngle = 0;
////                degreesFound = 0;
////                // Otherwise, store the final angle value, and add one to the objectCount
////            }
////            else if (degreesFound > 5)
////            {
////                objectFound = 0;
////                items[objectCount].middleAngle =
////                        (items[objectCount].startingAngle
////                                + items[objectCount].endingAngle) / 2;
////
////                objectCount++;
////                degreesFound = 0;
////            }
//
//        }
//
////            else
////            {
////                // Send that the end of item has been found, and clear the objectFound flag
////                objectFound = 0;
////                // If the total degrees of the object is less than 3, assume it is a bad reading and zero the values
////                if (degreesFound < 5)
////                {
////                    items[objectCount].startingAngle = 0;
////                    items[objectCount].endingAngle = 0;
////                    degreesFound = 0;
////                    // Otherwise, store the final angle value, and add one to the objectCount
////                }
////                else
////                {
////                    items[objectCount].middleAngle =
////                            (items[objectCount].startingAngle
////                                    + items[objectCount].endingAngle) / 2;
////
////                    objectCount++;
////                    degreesFound = 0;
////
////                }
////            }
//        // If there is an object currently being scanned, and the end of the scan happens
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
//    }
//
//    for (i = 0; i < objectCount; i++)
//    {
//        // Find angular width
//        float angle = items[i].endingAngle - items[i].startingAngle;
//        float angleRads = (angle * 3.1415) / 180.0;
//        float width = 2 * sonarVals[items[i].middleAngle] * tan(angleRads); //(2 * 3.1415926 * (items[objectCount]))*((angle)/2)/360.0;
//        items[i].linearWidth = width;
//    }
//
//    /**
//     * sending object data to PuTTY to then be sent to pyhton for
//     * plotting the objects on the GUI
//     */
//    temp = (char*) malloc(50 * sizeof(char));
//    for (i = 0; i < objectCount; i++)
//    {
//        int len = sprintf(temp, "%d   %.2lf   %.2lf\r",items[i].middleAngle, sonarVals[items[i].middleAngle], items[i].linearWidth);
//
//        uart_sendStr(temp, len);
//    }
//
//    // After all the math is done, go through the array again to find the smallest object and store it's index
//    i = 0;
//    int smallestIndex = 0;
//    for (i = 1; i < objectCount; i++)
//    {
//        if (items[i].linearWidth < items[smallestIndex].linearWidth)
//        {
//            smallestIndex = i;
//        }
//    }
//
//}
