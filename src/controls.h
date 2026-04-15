#ifndef CONTROLS_H
#define CONTROLS_H
#include <stdbool.h>

#define POTENTIOMETER_PIN 45
#define INPUT_SLICE 5
#define BUTTON_PIN 20

// code
void adc_volume_init(void);

int read_potentiometer(void);

bool read_button(void);


#endif