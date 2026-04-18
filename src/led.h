#ifndef LED_H
#define LED_H

#define LED_PIN_R 37
#define LED_PIN_G 38
#define LED_PIN_B 39

// functions
void led_init(void);  // initialized PWM and LEDs to work
 
void leds_green(void);  // sets all LEDs green

void leds_red(void);  // sets all LEDs red

void leds_breathing(void);  // breathing LEDs

#endif
