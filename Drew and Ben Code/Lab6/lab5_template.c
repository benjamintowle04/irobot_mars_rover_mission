
/**
 * lab5_template.c
 *
 * Template file for CprE 288 Lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 * @author Diane Rover, updated 2/25/2021, 2/17/2022
 */

#include "button.h"
#include "timer.h"
#include "lcd.h"

#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART1)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1
#include "uart-interrupt.h"
#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors

// Uncomment or add any include directives that you want to use
// #include "open_interface.h"
// #include "movement.h"
// #include "button.h"

#warning "Possible unimplemented functions"
#define REPLACEME 0

volatile char command_byte;
volatile int command_flag;


void scan_Objects(void) {
    int i;
    int j;
    int t;
    int c;
    cyBOT_Scan_t scan;
    float distance[181] = {0};   // All scan distance values stored in this array
    char data[12] = "";
    char header[30] = "";






    //Outputs scanning data in Putty
    sprintf(header, "\rDegrees\t\tPING Distance (cm)\n");
    for(j = 0; j <= sizeof(header); j+=1) {
        uart_sendChar(header[j]);
    }

    i = 0;  //Iterate through entire scan from 0-185
    c = 0;
    while(i <= 180) {
        cyBOT_Scan(i,  &scan);
        distance[c] = scan.sound_dist;

        sprintf(data, "\r%d\t\t\t%0.3f", i, scan.sound_dist);
        for(t = 0; t <= sizeof(data); t+=1) {
            uart_sendChar(data[t]);
            if(uart_receive() == 115) {
                return;
            }

        }
        uart_sendChar('\n');
        i++;
        c++;
}
}


int main(void) {
    button_init();
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();

  // initialize the cyBot UART1 before trying to use it

   //(Uncomment ME for UART init part of lab)
    cyBot_uart_init_clean();  // Clean UART1 initialization, before running your UART1 GPIO init code

    uart_init();
     // Initialize the scan
    cyBOT_init_Scan(0b0111);
    right_calibration_value = 280000; //Cybot 4
    left_calibration_value = 1261750; //Cybot 4
    // Remember servo calibration function and variables from Lab 3

    // YOUR CODE HERE


    while(1)
    {

      // YOUR CODE HERE
      if(uart_receive() == 'g') {
         scan_Objects();
      }

    }


}
