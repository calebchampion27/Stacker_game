#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "led.h"
#include "storage.h"
#include "controls.h"
#include <game.h>

volatile bool is_breathing = true;

int main() {
    stdio_init_all();

    // initialize peripherals
    led_init();        // pwm for RGB leds
    eeprom_init();     // i2c setup
    adc_volume_init(); // adc & gpio pins setup for volume/ potentiometer
    int high_score = eeprom_read_score();
    bool reset_high = true;
    add_repeating_timer_ms(10, breathing_timer_callback, NULL, &timer); // set timer for LEDs
    uint16_t current_volume = 0;
    

    // display
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &hub75_program);
    uint sm = pio_claim_unused_sm(pio, true);
    init_hub75_pio(pio, sm, offset);

    uint8_t ctrl[] = {PIN_A, PIN_B, PIN_C, PIN_D, LAT_PIN, OE_PIN};
    for (int i = 0; i < 6; i++) {
        gpio_init(ctrl[i]);
        gpio_set_dir(ctrl[i], GPIO_OUT);
        gpio_put(ctrl[i], 0);
    }

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    int app_state = 0;
    bool last_button_state = true;
    absolute_time_t last_press_time = get_absolute_time();
    absolute_time_t state_start_time = 0;

    int frozen_y[MAX_LAYERS];
    for (int i = 0; i < MAX_LAYERS; i++) frozen_y[i] = -1;

    int num_frozen    = 0;
    int active_y      = 0;
    int active_dir    = 1;
    int frame_counter = 0;
    int frame_delay   = 30;

    #define STACK_TOLERANCE 2

while (true) {
    reset_high = gpio_get(RESET_BUTTON);
    if (!reset_high) {  // check for reset for eeprom
        eeprom_write_score(0);
        high_score = 0;
    }
    // check for volume adjestment at every loop
    current_volume = read_potentiometer();

        // won or lost logic
        if (app_state == 2 || app_state == 3) {
            if (app_state == 2) set_volume(500, current_volume);
            if (app_state == 3) set_volume(150, current_volume);

            // turn off lights and sound if 5 seconds
            if (absolute_time_diff_us(state_start_time, get_absolute_time()) >= 5000000LL) {
                stop_tone();
                is_breathing = true;
                app_state = 0;

                // save to eeprom if above high score
                if (num_frozen > high_score) {
                    high_score = num_frozen;
                    eeprom_write_score(high_score);
                }
            }
        }

        // button logic
        bool current_button_state = gpio_get(BUTTON_PIN);
        if (!current_button_state && last_button_state) {
            if (absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
                last_press_time = get_absolute_time();

                if (app_state == 0) {  // start game
                    app_state = 1;
                    is_breathing = false;
                    num_frozen = 0;
                    for (int i = 0; i < MAX_LAYERS; i++) frozen_y[i] = -1;
                    active_y = 0;
                    active_dir = 1;
                    frame_delay = 30;
                    frame_counter = 0;

                } else if (app_state == 1) {  // move onto next block
                    bool is_phase_start = (num_frozen == 0);

                    if (!is_phase_start) {
                        int target_y = frozen_y[num_frozen - 1];
                        int y_diff = active_y - target_y;
                        if (y_diff < 0) y_diff = -y_diff;

                        bool valid_stack = (y_diff <= STACK_TOLERANCE);

                        // Special Bridge Logic for Phase Transition
                        if (num_frozen == 4 && !valid_stack) {
                            int y_diff_alt = active_y - (target_y + BLOCK_GAP);
                            if (y_diff_alt < 0) y_diff_alt = -y_diff_alt;
                            if (y_diff_alt <= STACK_TOLERANCE) valid_stack = true;
                        }

                        if (!valid_stack) {  // lost game
                            app_state = 3;
                            state_start_time = get_absolute_time();
                            leds_red();
                            start_continuous_tone(150);
                            last_button_state = current_button_state;
                            continue;
                        }
                    }

                    frozen_y[num_frozen] = active_y;
                    num_frozen++;

                    frame_delay = (frame_delay * 75) / 100;
                    if (frame_delay < 1) frame_delay = 1;

                    if (num_frozen >= MAX_LAYERS) {  // win game
                        app_state = 2; 
                        state_start_time = get_absolute_time();
                        leds_green();
                        start_continuous_tone(500);
                    } else {
                        active_y = 0;
                        active_dir = 1;
                        frame_counter = 0;
                    }

                } else if (app_state == 2 || app_state == 3) {
                    // skip win or lose screen by pressing button again
                    stop_tone();
                    is_breathing = true;
                    if (num_frozen > high_score) {  // update high score if needed
                        high_score = num_frozen;
                        eeprom_write_score(high_score);
                    }
                    app_state = 0;
                }
            }
        }
        last_button_state = current_button_state;

        // play screen logic
        if (app_state == 1) {
            frame_counter++;
            if (frame_counter >= frame_delay) {
                frame_counter = 0;
                active_y += active_dir;
                int y_limit = (num_frozen < 4) ? 19 : 26;
                if (active_y >= y_limit) {
                    active_y = y_limit;
                    active_dir = -1;
                }
                if (active_y <= 0) {
                    active_y = 0;
                    active_dir = 1;
                }
            }
        }

        // screen display logic
        for (int row = 0; row < 16; row++) {
            for (int x = 63; x >= 0; x--) {
                uint32_t data = 0;
                if (app_state == 0) {
                    if (get_pixel_for_tower(x, row))                  data |= 0b000001;
                    if (get_pixel_for_tower(x, row + 16))             data |= 0b001000;
                    if (get_pixel_for_stacker(x - 12, row))           data |= 0b000001;
                    if (get_pixel_for_stacker(x - 12, row + 16))      data |= 0b001000;
                    if (get_pixel_line_1(x, row))                     data |= 0b000001;
                    if (get_pixel_line_1(x, row + 16))                data |= 0b001000;
                    if (get_pixel_line_2(x, row))                     data |= 0b000001;
                    if (get_pixel_line_2(x, row + 16))                data |= 0b001000;
                    if (get_pixel_line_3(x, row))                     data |= 0b000001;
                    if (get_pixel_line_3(x, row + 16))                data |= 0b001000;
                    if (get_pixel_box_1(x, row))                      data |= 0b000001;
                    if (get_pixel_box_1(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_box_2(x, row))                      data |= 0b000001;
                    if (get_pixel_box_2(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_box_3(x, row))                      data |= 0b000001;
                    if (get_pixel_box_3(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_box_4(x, row))                      data |= 0b000001;
                    if (get_pixel_box_4(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_box_5(x, row))                      data |= 0b000001;
                    if (get_pixel_box_5(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_box_6(x, row))                      data |= 0b000001;
                    if (get_pixel_box_6(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_for_tower(x, row))                  data |= 0b000001;
                    if (get_pixel_for_tower(x, row + 16))             data |= 0b001000;
                    if (get_pixel_for_stacker(x - 12, row))           data |= 0b000001;
                    if (get_pixel_for_stacker(x - 12, row + 16))      data |= 0b001000;
                    if (get_pixel_box_6(x, row))                      data |= 0b000001;
                    if (get_pixel_box_6(x, row + 16))                 data |= 0b001000;
                    if (get_pixel_high_score(x, row, high_score))      data |= 0b000001;
                    if (get_pixel_high_score(x, row + 16, high_score)) data |= 0b001000;
                } else if (app_state == 3) {
                    if (get_pixel_you_lost(x, row))      data |= 0b000001;
                    if (get_pixel_you_lost(x, row + 16)) data |= 0b001000;
                } else if (app_state == 2) {
                    if (get_pixel_you_win(x, row))       data |= 0b000001;
                    if (get_pixel_you_win(x, row + 16))  data |= 0b001000;
                } else {
                    if (get_pixel_anim_line(x, row))      data |= 0b000001;
                    if (get_pixel_anim_line(x, row + 16)) data |= 0b001000;
                    if (get_pixel_mid_line(x, row))       data |= 0b000001;
                    if (get_pixel_mid_line(x, row + 16))  data |= 0b001000;
                    if (get_pixel_line_14(x, row))        data |= 0b000001;
                    if (get_pixel_line_14(x, row + 16))   data |= 0b001000;

                    for (int i = 0; i < num_frozen; i++) {
                        int xs = col_x_start(i);
                        if (get_pixel_pair(x, row,      frozen_y[i], xs, i)) data |= 0b000001;
                        if (get_pixel_pair(x, row + 16, frozen_y[i], xs, i)) data |= 0b001000;
                    }
                    if (app_state == 1) {
                        int xs = col_x_start(num_frozen);
                        if (get_pixel_pair(x, row,      active_y, xs, num_frozen)) data |= 0b000001;
                        if (get_pixel_pair(x, row + 16, active_y, xs, num_frozen)) data |= 0b001000;
                    }
                }
                pio_sm_put_blocking(pio, sm, data);
            }
            while (!pio_sm_is_tx_fifo_empty(pio, sm));
            gpio_put(OE_PIN, 1);
            gpio_put(PIN_A, (row >> 0) & 1);
            gpio_put(PIN_B, (row >> 1) & 1);
            gpio_put(PIN_C, (row >> 2) & 1);
            gpio_put(PIN_D, (row >> 3) & 1);
            gpio_put(LAT_PIN, 1);
            sleep_us(1);
            gpio_put(LAT_PIN, 0);
            gpio_put(OE_PIN, 0);
            sleep_us(400);
        }
    }
}