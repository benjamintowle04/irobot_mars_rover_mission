#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <inc/tm4c123gh6pm.h>
#include "driverlib/interrupt.h"

void servomotor_init(void);

void servomotor_move(uint16_t degrees);

#endif

