/**
 * lab6-interrupt_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"  // For scan sensors
#include "uart-interrupt.h"
#include "adc.h"

// Uncomment or add any include directives that you want to use
#include "open_interface.h"
#include "movement.h"
#include "button.h"

// Your code can use the global variables defined in uart-interrupt.c
// They are declared with the extern qualifier in uart-interrupt.h, which makes the variables visible to this file.


typedef struct
{
    int startingAngle;
    int endingAngle;
    int middleAngle;
    double linearWidth;
} object;


void control(oi_t *sensor_data)
{

    while (true)
    {
        char byte = uart_receive_nonBlocking();
        lcd_printf("%c", byte);

        if (byte == 'w')
        {
            move_forward(sensor_data, 100);
            byte = '\0';
        }
        else if (byte == 's')
        {
            backup(sensor_data, 100);
            byte = '\0';
        }
        else if (byte == 'a')
        {
            turn_left(sensor_data, 15);
            byte = '\0';
        }
        else if (byte == 'd')
        {
            turn_right(sensor_data, 15);
            byte = '\0';
        }
        else if (byte == 't')
        {
            cyBOT_Scan_t scanData;
            float sonarVals[360];
            short irVals[360];
            //
            int i = 0;
            // The objects could be stored in an array with the malloc, and free methods

            for (i = 0; i < 360; i++)
            {
                sonarVals[i] = 0;
                irVals[i] = 0;
            }

            //Header
            char str[40];
            int len = sprintf(str,
                              "\rDegrees\tPING Distance(cm)\tIR Value\r\n");
            uart_sendStr(str, len);

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

            while (!objectCount)
            {

                for (angle = 0; angle <= 180; angle += 1)
                {

                    cyBOT_Scan(angle, &scanData);
                    int IRsum = adcRead();
                    float SONARsum = adcDistance();
                    int RealIR = scanData.IR_raw_val;
                    float RealPing = scanData.sound_dist;
                    irVals[angle] = IRsum;
                    sonarVals[angle] = SONARsum;

                    lcd_printf("ADC: %d\t%.2lf cm \n Actual: %d\t%.2lf cm", IRsum, SONARsum, RealIR, RealPing);
                    timer_waitMillis(1000);

                    char scanData[21];
                    int len = sprintf(scanData, "%d\t%.2lf\t\t\t%d\r", angle,
                                      sonarVals[angle], irVals[angle]);

                    uart_sendStr(scanData, len);
                }

                for (i = 0; i <= 180; ++i)
                {
                    if (irVals[i] > 475 && ((i > 5 && i < 179) && !objectFound))
                    {
                        // Store the starting angle of the object and also set the objectFound flag to true
                        items[objectCount].startingAngle = i;
                        items[objectCount].endingAngle = i;
                        objectFound++;

                        // If there is currently being an object being looked at
                    }
                    else if (objectFound)
                    {
                        // Then if the IR value is still high enough, update the degreesFound, and the ending angle
                        if (irVals[i] > 850)
                        {
                            items[objectCount].endingAngle = i;
                            degreesFound++;
                            // If the IR value is not high enough
                        }
                        else
                        {
                            // Send that the end of item has been found, and clear the objectFound flag
                            objectFound = 0;
                            // If the total degrees of the object is less than 3, assume it is a bad reading and zero the values
                            if (degreesFound < 3)
                            {
                                items[objectCount].startingAngle = 0;
                                items[objectCount].endingAngle = 0;
                                degreesFound = 0;
                                // Otherwise, store the final angle value, and add one to the objectCount
                            }
                            else
                            {
                                items[objectCount].middleAngle =
                                        (items[objectCount].startingAngle
                                                + items[objectCount].endingAngle)
                                                / 2;

                                char temp[6];

                                int Length =
                                        sprintf(temp,
                                                "StartAngle: %d  EndAngle: %d  MidPoint: %d  Distance: %.2lf\r",
                                                items[objectCount].startingAngle,
                                                items[objectCount].endingAngle,
                                                items[objectCount].middleAngle,
                                                sonarVals[items[objectCount].middleAngle]);
                                uart_sendStr(temp, Length);

                                objectCount++;
                                degreesFound = 0;

                            }
                        }
                        // If there is an object currently being scanned, and the end of the scan happens
                        if (i == 179)
                        {
                            // Print that to putty, and clear the objectFound flag
                            uart_sendStr("End of scan with object\r", 25);
                            objectFound = 0;
                            // Then do a check if the object is less than 5 degrees clearing it if it is
                            if (degreesFound < 5)
                            {
                                items[objectCount].startingAngle = 0;
                                items[objectCount].endingAngle = 0;
                                degreesFound = 0;
                                // Otherwise, store the final angle value, and add one to the objectCount
                            }
                            else
                            {
                                items[objectCount].middleAngle =
                                        (items[objectCount].startingAngle
                                                + items[objectCount].endingAngle)
                                                / 2;

                                char temp[6];

                                int Length =
                                        sprintf(temp,
                                                "%d\t%d\t%d\t%.2lf\r",
                                                items[objectCount].startingAngle,
                                                items[objectCount].endingAngle,
                                                items[objectCount].middleAngle,
                                                sonarVals[items[objectCount].middleAngle]);
                                uart_sendStr(temp, Length);

                                objectCount++;
                                degreesFound = 0;

                            }
                        }
                    }

                    // At the end of the scan, print the number of items found
                    if (i == 179)
                    {
                        char itemCount[25];
                        int len = sprintf(itemCount, "Found %d items total\r",
                                          objectCount);
                        uart_sendStr(itemCount, len);
                    }
                }
                if (!objectCount)
                {
                    move_forward(sensor_data, 800);
                    continue;
                }
                else if (objectCount)
                {
                    break;
                }

            }

            for (i = 0; i < objectCount; i++)
            {
                // Find angular width
                float angle = items[i].middleAngle - items[i].startingAngle;
                float angleRads = (angle * 3.1415) / 180.0;
                float width = 2 * sonarVals[items[i].middleAngle]
                        * tan(angleRads);
                items[i].linearWidth = width;

                char dist[30];
                int len =
                        sprintf(dist,
                                "Item: %d Width: %.2lf Degrees: %d End Degree: %d Midpoint: %d\r",
                                i, items[i].linearWidth, items[i].startingAngle,
                                items[i].endingAngle, items[i].middleAngle);
                uart_sendStr(dist, len);
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
            // Then print the width, and item number of the smallest object to putty
            char dist[50];
            len = sprintf(dist, "Smallest Item: %d Width: %.2lf\r",
                          smallestIndex, items[smallestIndex].linearWidth);
            uart_sendStr(dist, len);

            if (items[smallestIndex].middleAngle <= 90)
            {
                // Calculate the drive distance by averaging the 3 sonar values at the center of the object
                int driveDistance =
                        ((sonarVals[items[smallestIndex].middleAngle]
                                + sonarVals[items[smallestIndex].middleAngle - 1]
                                + sonarVals[items[smallestIndex].middleAngle + 1])
                                / 3) - 8;

                turn_right(sensor_data, 90 - items[smallestIndex].middleAngle);
                Movebumpcheck(sensor_data, driveDistance * 10);
            }
            else
            {
                int driveDistance =
                        ((sonarVals[items[smallestIndex].middleAngle]
                                + sonarVals[items[smallestIndex].middleAngle - 1]
                                + sonarVals[items[smallestIndex].middleAngle + 1])
                                / 3) - 8;
                turn_left(sensor_data, items[smallestIndex].middleAngle - 90);
                Movebumpcheck(sensor_data, driveDistance * 10);
            }
            byte = '\0';

        }
        else if (byte == '`')
        {
            break;
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
    cyBOT_init_Scan(0b0111);
    adc_init();
    //cyBOT_SERVO_cal();
    right_calibration_value = 259000;
    left_calibration_value = 1235500;

    control(sensor_data);

    oi_free(sensor_data);
    return 0;


    }



