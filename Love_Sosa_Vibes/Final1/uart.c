/*
*
*   uart.c
*
*
*
*   @author Andrew Vick
*   @date 2/28/2023
*/

//#include <inc/tm4c123gh6pm.h>
//#include <stdint.h>
//#include "uart.h"
//
//void uart_init(void){
//	//TODO
//  //enable clock to GPIO port B
//  SYSCTL_RCGCGPIO_R |= 0b000010;
//
//  //enable clock to UART1
//  SYSCTL_RCGCUART_R |= 0b000010;
//
//  //wait for GPIOB and UART1 peripherals to be ready
//  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
//  while ((SYSCTL_PRUART_R & 0x02) == 0) {};
//
//  //enable alternate functions on port B pins
//  GPIO_PORTB_AFSEL_R |= (1<<0)|(1<<1);
//
//  //enable digital functionality on port B pins
//  GPIO_PORTB_DEN_R |= (1<<0)|(1<<1);
//
//  //enable UART1 Rx and Tx on port B pins
//  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB0_U1RX | GPIO_PCTL_PB1_U1TX;
//
//  //calculate baud rate
//  uint16_t iBRD = 0x8; //use equations
//  uint16_t fBRD = 0x2C; //use equations
//
//  //turn off UART1 while setting it up
//  UART1_CTL_R &= ~(UART_CTL_UARTEN);
//
//  //set baud rate
//  //note: to take effect, there must be a write to LCRH after these assignments
//  UART1_IBRD_R = iBRD;
//  UART1_FBRD_R = fBRD;
//
//  //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
//  //note: this write to LCRH must be after the BRD assignments
//  UART1_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
//
//  //use system clock as source
//  //note from the datasheet UARTCCC register description:
//  //field is 0 (system clock) by default on reset
//  //Good to be explicit in your code
//  UART1_CC_R = UART_CC_CS_SYSCLK;
//
//  //re-enable UART1 and also enable RX, TX (three bits)
//  //note from the datasheet UARTCTL register description:
//  //RX and TX are enabled by default on reset
//  //Good to be explicit in your code
//  //Be careful to not clear RX and TX enable bits
//  //(either preserve if already set or set them)
//  UART1_CTL_R |= UART_CTL_UARTEN | UART_CTL_RXE | UART_CTL_TXE;
//
//}
//
//void uart_sendChar(char data){
//	//TODO
//    while(UART1_FR_R & 0x20){}
//    UART1_DR_R = data;
//}
//
//char uart_receive(void){
//	//TODO
//    char data = 0;
//    while(UART1_FR_R & UART_FR_RXFE){}
//    data = (char) (UART1_DR_R & 0xFF);
//    return data;
//}
//
//char uart_receive_nonBlocking(void){
//    //TODO
////    char data = 0;
////    while(UART1_FR_R & UART_FR_RXFE){}//change this line
////    data = (char) (UART1_DR_R & 0xFF);
////    return data;
//
//    char data = 0;
//    if(UART1_FR_R & UART_FR_RXFE){
//        return 0;
//    }else{
//        data = (char) (UART1_DR_R & 0xFF);
//        return data;
//    }
//
//}
//
//void uart_sendStr(const char *data){
//	//TODO for reference see lcd_puts from lcd.c file
//}
