#include "led_blinky.h"

void led_blinky(void *pvParameters){
  pinMode(LED_GPIO, OUTPUT);
  
  while(1) {                        
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(50);
    digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    vTaskDelay(50);
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(50);
    digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    vTaskDelay(50);
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(50);
    digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    vTaskDelay(50);
    digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
    vTaskDelay(50);
    digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
    vTaskDelay(1000);
  }
}

void turn_off_led(void *pv){
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LOW);  // turn the LED OFF
  vTaskDelete(NULL);
}

void turn_on_led1(void *pv){
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
  vTaskDelete(NULL);
}

void turn_on_led2(void *pv){
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, HIGH);  // turn the LED ON
  vTaskDelete(NULL);
}