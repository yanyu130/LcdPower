#ifndef __LED_H
#define __LED_H

#include "stm8s.h"
#include "prohead.h"

#define LED_High   GPIO_WriteHigh(GPIOC, GPIO_PIN_4)
#define LED_Low    GPIO_WriteLow(GPIOC, GPIO_PIN_4)




void LedGPIO_init();

//LEDøÿ÷∆  ‰»Î1 µ„¡¡; 0 œ®√;
void Driver_LEDCont(bool flag);


#endif