#include "controls.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

adc_volume_init(void) {
  // potentiometer adc
  adc_init();
  adc_gpio_init(POTENTIOMETER_PIN);
  adc_select_input(INPUT_SLICE); 

  // button init
  gpio_set_function(BUTTON_PIN, GPIO_FUNC_SIO);  // SIO function to gpio button
  gpio_set_dir(BUTTON_PIN, false);
}

read_potentiometer(void) {
  return adc_read();  // single shot
}

read_button(void) {
  bool button_press;
  button_press = gpio_get(BUTTON_PIN);

  return button_press;
}