#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>
#include <stdbool.h>

#define POTENTIOMETER_PIN 45
#define INPUT_SLICE_POT 5 // for potentiometer
#define SOUND_PIN 25

// code
void adc_volume_init(void);

// read potentiometer adc value
uint16_t read_potentiometer(void);

// start sound at given frequency
void start_continuous_tone(int frequency);

// volume control
void set_volume(int frequency, uint16_t volume_level);

// turn off sound
void stop_tone(void);

#endif