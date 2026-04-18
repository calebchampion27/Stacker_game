#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>
#include <stdbool.h>

#define POTENTIOMETER_PIN 45
#define INPUT_SLICE 5
#define BUTTON_PIN 20

// code
void adc_volume_init(void);

uint16_t read_potentiometer(void);

bool read_button(void);


#endif