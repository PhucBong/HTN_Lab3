#ifndef __LED_BLINKY__
#define __LED_BLINKY__
#include <Arduino.h>
#include "global.h"
#define LED_GPIO 48
#define NEO_PIN 45


void led_blinky(void *pvParameters);
void turn_off_led(void *pv);
void turn_on_led1(void *pv);
void turn_on_led2(void *pv);


#endif