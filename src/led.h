#ifndef LED_H
#define LED_H

#include <stdbool.h>

#define LED_PIN_R 37
#define LED_PIN_G 38
#define LED_PIN_B 39

// breathing led on timer
extern volatile bool is_breathing;
struct repeating_timer timer;

// functions
void led_init(void);  // initialized PWM and LEDs to work
 
void leds_green(void);  // sets all LEDs green

void leds_red(void);  // sets all LEDs red

void leds_breathing(void);  // breathing LEDs

void set_breathing_state(bool state);

bool breathing_timer_callback(struct repeating_timer *t);

#endif
