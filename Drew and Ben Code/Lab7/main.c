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

extern volatile char command_byte;
extern volatile int command_flag;



typedef struct object {
    float distance;
    int angle;
    int width;
    int objNum;
    int ir_dist;
    float lin_width;
} obj;



int objDetected(float distance, float tol) {
    return distance <= tol;
}

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


void outputMessage(void) {
   char data = ' ';
   data = uart_receive();
   lcd_printf("%c", data);
   uart_sendChar(data);
}


void buttonMove(void) {
       while(true) {
           outputMessage();
           oi_t *sensor_data = oi_alloc();
           oi_init(sensor_data);
           oi_update(sensor_data);

           if(uart_receive() == 119) {
               moveForward(sensor_data, 100);
           }
           else if (uart_receive() == 115) {
               moveBackward(sensor_data, 100);
           }

           else if (uart_receive() == 97) {
               turnLeft(sensor_data, 90);
           }

           else if (uart_receive() == 100) {
               turnRight(sensor_data, 90);
           }

           else {
               oi_setWheels(0,0);
           }
       }
}


void scan_Objects(oi_t*sensor_dataT) {
    int i;
    int j;
    int t;
    int r;
    int k;
    int u;
    int c;
    int x;
    float pi = 3.14159265359;

    cyBOT_Scan_t scan;
    //int degree;
    float distance[181] = {0};   // All scan distance values stored in this array
    int ir_distance[181] = {0};  //All scanned ir_distances
    float obj_dist[181] = {0};  //Distance Values of 1 object stored in this array
    float obj_IRdist[181] = {0};  //IR Distance Values of 1 object stored in this array
    int obj_angle[181] = {0};
    char data[19] = "";
    char header[43] = "";
    char smallestWidth[115] = "";
    int smallestIndex;
    int num = 0;
    int angle = 0;

    //empty the struct array initially
    obj items[num];



    //Outputs scanning data in Putty
    sprintf(header, "\rDegrees\t\tPING Distance (cm)\t\tIR Distance\n");
    for(j = 0; header[j]; j+=1) {
        uart_sendChar(header[j]);
    }

    i = 0;  //Iterate through entire scan from 0-185
    r = 0;  //Iterate through struct objects
    k = 0;  //Iterate through angle and distance arrays in struct object
    c = 0;
    x = 0;
    while(c <= 180) {
        cyBOT_Scan(i,  &scan);
        distance[c] = scan.sound_dist;
        ir_distance[c] = scan.IR_raw_val;

        sprintf(data, "\r%d\t\t\t%0.3f\t\t\t%d", i, scan.sound_dist, scan.IR_raw_val);
        for(t = 0; data[t]; t+=1) {
            uart_sendChar(data[t]);
        }
        uart_sendChar('\n');



       //When we first encounter an object
        if(c > 0 && ir_distance[c] > 700 && ir_distance[c] > 650) {
                obj_dist[k] = scan.sound_dist;
                obj_angle[k] = i;
                obj_IRdist[k] = scan.IR_raw_val;
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
                items[r].lin_width = (2*pi*(items[r].distance)*(((float)items[r].width /2)/360.0));
                num=r;
                r++;
                lcd_printf("%d", r);
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
        i+=1;

    }



    //Find the object with the smallest width
    int q;

    int min = items[0].width;
    smallestIndex = 0;

      if(r > 1) {
          uart_sendChar('h');
          uart_sendChar('r');
        q=0;
        while (q < r) {
            if (items[q].width > 0 && items[q].width < min) {
                min = items[q].width;
                smallestIndex = q;
            }
            q++;
        }
      }


    //Output the object with the smallest width
    sprintf(smallestWidth, "\n\rThe object with the smallest width is Object # %d with an linear width of %f cm and an angle of %d degrees", items[smallestIndex].objNum, items[smallestIndex].lin_width, items[smallestIndex].angle);
    for (u = 0; smallestWidth[u]; u++) {
        uart_sendChar(smallestWidth[u]);
    }
    uart_sendChar('\n');



    //Facing the cyBot towards the smallest object
    //uart_sendChar('h');
    obj smallestObject = items[smallestIndex];
    angle = smallestObject.angle;
    //uart_sendChar('r');
    if (angle < 90) {
        cyBOT_Scan(smallestObject.angle+5,  &scan);
        turnRight(sensor_dataT, 90 - smallestObject.angle-5);
        if(bumper(sensor_dataT) == 0) {
            moveForward(sensor_dataT, smallestObject.distance*10 - 30);
            uart_sendChar('t');
        }else {
            //bump_Check(sensor_dataT,150);
            uart_sendChar('n');
            scan_Objects(sensor_dataT);

        }
    }else if(angle > 90) {
        uart_sendChar('b');
        cyBOT_Scan(smallestObject.angle+5,  &scan);
        turnLeft(sensor_dataT, 180 - smallestObject.angle+5);
        if(bumper(sensor_dataT) == 0) {
            moveForward(sensor_dataT, smallestObject.distance*10 - 30);
            uart_sendChar('P');
        }else {
            //bump_Check(sensor_dataT,150);
            uart_sendChar('n');
            scan_Objects(sensor_dataT);

        }
    }




}

/**
 * main.c
 */
int main(void)
{
    lcd_init();
    timer_init();
   //cyBot_uart_init();
   // cyBot_uart_init_clean();
    uart_init();
    cyBOT_init_Scan(0b0111);
    oi_t*sensor_dataT = oi_alloc();
    oi_init(sensor_dataT);
    oi_update(sensor_dataT);


    //cyBOT_SERVRO_cal_t Cal;
    //Cal = cyBOT_SERVO_cal();
    right_calibration_value = 274750; //bot 9            //280000; //Cybot 4
    left_calibration_value = 1256500; //bot 9       //1261750; //Cybot 4


    //buttonMove();
    while(1) {
       if(command_flag == 1) {
            if(command_byte == 'm') {
                scan_Objects(sensor_dataT);
                uart_sendChar('m');
                command_flag = 0;
            }
        }
    }

    oi_free(sensor_dataT);


    return 0;

}


