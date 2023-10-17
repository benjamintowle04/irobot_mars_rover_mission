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

#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors


#warning "Possible unimplemented functions"
#define REPLACEME 0



int main(void) {
    button_init();
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();

  // initialize the cyBot UART1 before trying to use it

   //(Uncomment ME for UART init part of lab)
    cyBot_uart_init_clean();  // Clean UART1 initialization, before running your UART1 GPIO init code

    uart_init();

    // Complete this code for configuring the GPIO PORTB part of UART1 initialization (your UART1 GPIO init code)
      SYSCTL_RCGCGPIO_R |= 0x02;
        while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
          GPIO_PORTB_DEN_R |= 0x03;
          GPIO_PORTB_AFSEL_R |= 0x03;
          GPIO_PORTB_PCTL_R &= 0xFFFFFF11;     // Force 0's in the desired locations
          GPIO_PORTB_PCTL_R |= 0x00000011;     // Force 1's in the desired locations
         // Or see the notes for a coding alternative to assign a value to the PCTL field

    // (Uncomment ME for UART init part of lab)
         cyBot_uart_init_last_half();  // Complete the UART device configuration


        // Initialize the scan
       cyBOT_init_Scan(0x00000007);
        // Remember servo calibration function and variables from Lab 3

    // YOUR CODE HERE

    int i = 0;
    int t;
    int num = 0;
    char data_arr[20] = {0};
    while(1)
    {
        data_arr[i] = uart_receive();
        uart_sendChar(data_arr[i]);
        i++;
        if(i == 20 || (int) data_arr[i-1] == 13) {
            if((int) data_arr[i-1] == 13) {
                uart_sendChar('\n');
                uart_sendChar('\r');
                for(t = 0; t < i-1; t++) {
                    lcd_putc(data_arr[t]);
                }
            }else {
                for(t = 0; t < i; t++) {
                    lcd_putc(data_arr[t]);
                }
            }
            while(data_arr[num] != 0) {
                data_arr[num] = 0;
                num++;
            }
            num = 0;

            if(i < 9) {
                lcd_putc('0' + (i -1));
            }
            else {
                if(i < 20) {
                    lcd_putc('0' + 1);
                    lcd_putc('0' + (i-11));
                }else {
                    lcd_putc('0' + 2);
                    lcd_putc('0' + 0);
                }

            }

            i = 0;
            data_arr[i] = uart_receive();
            if(data_arr[i] == 109) {
                i = 0;
                break;
            }else {
                uart_sendChar(data_arr[i]);
                i++;
            }



        }
      // YOUR CODE HERE


    }

    char str[] = "\n\rDone\n\r";
    uart_sendStr(str);

}

