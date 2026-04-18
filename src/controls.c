#include "controls.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

// initialize PWM, ADC, and button
void adc_volume_init(void) {
  // potentiometer adc
  adc_init();
  adc_gpio_init(POTENTIOMETER_PIN);
  adc_select_input(INPUT_SLICE_POT); 

  // sound init
  gpio_set_function(SOUND_PIN, GPIO_FUNC_PWM);

  // button init
  gpio_set_function(BUTTON_PIN, GPIO_FUNC_SIO);  // SIO function to gpio button
  gpio_set_dir(BUTTON_PIN, false);
}

// read potentiometer adc value
uint16_t read_potentiometer(void) {
  return adc_read();  // single shot
}

// start sound at given frequency
void start_continuous_tone(int frequency) {
  uint slice_num = pwm_gpio_to_slice_num(SOUND_PIN);
  uint32_t clock_freq = clock_get_hz(clk_sys);
  uint32_t divider = 16;
  uint32_t wrap = clock_freq / (divider * frequency);
    
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, divider);
  pwm_config_set_wrap(&config, wrap);
  pwm_init(slice_num, &config, true);
}

// volume control
void set_volume(int frequency, uint16_t volume_level) {
  uint32_t clock_freq = clock_get_hz(clk_sys);
  uint32_t wrap = clock_freq / (16 * frequency);
    
  // duty cycle
  uint32_t duty_cycle = (volume_level * (wrap / 2)) / 4095;
  pwm_set_gpio_level(SOUND_PIN, duty_cycle);
}

// turn off sound
void stop_tone(void) {
  pwm_set_gpio_level(SOUND_PIN, 0);
}

// read button value
bool read_button(void) {
  return gpio_get(BUTTON_PIN);
}