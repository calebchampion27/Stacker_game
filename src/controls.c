#include "controls.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "hardware/adc.h"
#include "hardware/gpio.h"

void adc_volume_init(void) {
  // potentiometer adc
  adc_init();
  adc_gpio_init(POTENTIOMETER_PIN);
  adc_select_input(INPUT_SLICE); 

  // button init
  gpio_set_function(BUTTON_PIN, GPIO_FUNC_SIO);  // SIO function to gpio button
  gpio_set_dir(BUTTON_PIN, false);
}

uint16_t read_potentiometer(void) {
  return adc_read();  // single shot
}

bool read_button(void) {
  return gpio_get(BUTTON_PIN);
}