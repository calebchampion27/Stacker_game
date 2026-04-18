#include "led.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// variables
int duty_cycle = 0;
int color = 0;
int dir = 0;

void led_init(void) {
  // set gpio pins to PWM
  gpio_set_function(LED_PIN_R, GPIO_FUNC_PWM);
  gpio_set_function(LED_PIN_G, GPIO_FUNC_PWM);
  gpio_set_function(LED_PIN_B, GPIO_FUNC_PWM);

  // PWM clock div to 150
  uint slice_r = pwm_gpio_to_slice_num(LED_PIN_R); 
  uint slice_g = pwm_gpio_to_slice_num(LED_PIN_G);
  uint slice_b = pwm_gpio_to_slice_num(LED_PIN_B);

  // div clock by 150
  pwm_set_clkdiv(slice_r, 150.0);
  pwm_set_clkdiv(slice_g, 150.0);
  pwm_set_clkdiv(slice_b, 150.0);

  // set to wrap at 0 to 100 duty cycle
  pwm_set_wrap(slice_r, 255);
  pwm_set_wrap(slice_g, 255);
  pwm_set_wrap(slice_b, 255);

  // set all LEDs to off
  pwm_set_chan_level(slice_r, pwm_gpio_to_channel(LED_PIN_R), 255);
  pwm_set_chan_level(slice_g, pwm_gpio_to_channel(LED_PIN_G), 255);
  pwm_set_chan_level(slice_b, pwm_gpio_to_channel(LED_PIN_B), 255);

  // enable PWM signal
  pwm_set_enabled(slice_r, true);
  pwm_set_enabled(slice_g, true);
  pwm_set_enabled(slice_b, true);
}

void leds_green(void) {
  pwm_set_gpio_level(LED_PIN_R, 255);
  pwm_set_gpio_level(LED_PIN_G, 0);
  pwm_set_gpio_level(LED_PIN_B, 255);
}

void leds_red(void) {
  pwm_set_gpio_level(LED_PIN_R, 0);
  pwm_set_gpio_level(LED_PIN_G, 255);
  pwm_set_gpio_level(LED_PIN_B, 255);
}

void leds_breathing(void) {
  uint slice_num = pwm_gpio_to_slice_num(LED_PIN_R);
    // logic
    if (dir == 0 && duty_cycle == 100) {
        color = (color + 1) % 3;
        dir = 1;
    }
    else if (dir == 1 && duty_cycle == 0) {
        dir = 0;
    }

    // update duty cycle 
    if (dir == 0) {
        duty_cycle++;
    }
    else {
        duty_cycle--;
    }

    // PWM level
    uint32_t period = pwm_hw->slice[slice_num].top;
    uint32_t level = (duty_cycle * period) / 100;

    pwm_set_gpio_level(LED_PIN_R, (color == 0) ? level : period);
    pwm_set_gpio_level(LED_PIN_G, (color == 1) ? level : period);
    pwm_set_gpio_level(LED_PIN_B, (color == 2) ? level : period);
}