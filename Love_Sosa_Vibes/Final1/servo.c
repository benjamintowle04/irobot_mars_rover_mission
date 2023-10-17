#include "servo.h"
#include "Timer.h"
#include <math.h>

void servomotor_init(void) {

    //Enable clocks
    SYSCTL_RCGCGPIO_R |= 0x2;
    while ((SYSCTL_PRGPIO_R & 0x2) == 0) {};
    SYSCTL_RCGCTIMER_R |= 0x2;
    while ((SYSCTL_PRTIMER_R & 0x2) == 0) {};

    //PB5 set to output, digital
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_DEN_R |= 0x20;

    //Enable alternate functions
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x700000;

    //Disable TIMER1
    TIMER1_CTL_R &= 0xFEFF;

    //set to PWM with periodic interval
    TIMER1_TBMR_R |= 0xA;
    TIMER1_TBMR_R &= 0xA;

    //Configure
    TIMER1_CFG_R |= 0x4;

    //Start value set to 20ms(prescale value)
    TIMER1_TBPR_R = 0x04;

    //Write count reg
    TIMER1_TBILR_R |= 0xE200;

    //Re-emable TIMER1
    TIMER1_CTL_R |= 0x100;
}

void servomotor_move(uint16_t degrees) {

    if (degrees >= 0 && degrees <= 180) {

        float fractDeg = (degrees / 105.0) - .05; //hopefully not servo related took to long to debug to get good value on CYBOT-08 in overflow lab
        float tempMinus20 = 20.0 - fractDeg;
        uint32_t degreeVal = tempMinus20 * 16000;

        uint16_t MATCHR = degreeVal & 0xFFFF;

        //Load match register with match value
        TIMER1_TBMATCHR_R = MATCHR;

        uint8_t PMR = (degreeVal >> 16);

        //Timer match prescale value
        TIMER1_TBPMR_R = PMR;
    }

}
