#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hub75.pio.h"
#include <stdio.h>

#define DATA_BASE_PIN 2
#define PIN_A 8
#define PIN_B 9
#define PIN_C 10
#define PIN_D 11
#define CLK_PIN 12
#define LAT_PIN 13
#define OE_PIN  14
#define BUTTON_PIN 15

#define MAX_LAYERS 8
#define BLOCK_HEIGHT 6
#define BLOCK_WIDTH 6
#define BLOCK_GAP 7

// --- Font Data ---
const uint8_t font_T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_o[7] = {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E};
const uint8_t font_w[7] = {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0A};
const uint8_t font_e[7] = {0x00, 0x00, 0x07, 0x04, 0x07, 0x04, 0x07};
const uint8_t font_r[7] = {0x00, 0x00, 0x06, 0x09, 0x08, 0x08, 0x08};
const uint8_t font_S[7] = {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E};
const uint8_t font_t[7] = {0x04, 0x04, 0x1F, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_a[7] = {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F};
const uint8_t font_k[8] = {0x08, 0x09, 0x0A, 0x0C, 0x0C, 0x0A, 0x09, 0x08};

const uint8_t font_Y[7]  = {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_u[7]  = {0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x0E};
const uint8_t font_L[7]  = {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0F}; // Corrected L
const uint8_t font_s[7]  = {0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x0E};
const uint8_t font_ex[7] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}; 
const uint8_t font_W[7]  = {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A};
const uint8_t font_i[7]  = {0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_n[7]  = {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11};

// --- Rendering Helpers ---

bool get_pixel_you_lost(int x, int y) {
    int rx = y, ry = x;
    if (ry == 17 && (rx >= 0 && rx <= 31)) return true;
    if (ry == 41 && (rx >= 0 && rx <= 31)) return true;
    if (ry >= 19 && ry <= 25) {
        int row_idx = ry - 19; 
        if (rx >= 2  && rx < 7)  return (font_Y[row_idx] >> (6  - rx)) & 1;
        if (rx >= 8  && rx < 13) return (font_o[row_idx] >> (12 - rx)) & 1;
        if (rx >= 14 && rx < 19) return (font_u[row_idx] >> (18 - rx)) & 1;
    }
    if (ry >= 33 && ry <= 39) {
        int row_idx = ry - 33;
        if (rx >= 2  && rx < 7)  return (font_L[row_idx]  >> (6  - rx)) & 1;
        if (rx >= 8  && rx < 13) return (font_o[row_idx]  >> (12 - rx)) & 1;
        if (rx >= 14 && rx < 19) return (font_s[row_idx]  >> (18 - rx)) & 1;
        if (rx >= 20 && rx < 25) return (font_t[row_idx]  >> (24 - rx)) & 1;
        if (rx >= 26 && rx < 31) return (font_ex[row_idx] >> (30 - rx)) & 1;
    }
    return false;
}

bool get_pixel_you_win(int x, int y) {
    int rx = y, ry = x;
    if (ry == 17 && (rx >= 0 && rx <= 31)) return true;
    if (ry == 41 && (rx >= 0 && rx <= 31)) return true;
    if (ry >= 19 && ry <= 25) {
        int row_idx = ry - 19; 
        if (rx >= 2  && rx < 7)  return (font_Y[row_idx] >> (6  - rx)) & 1;
        if (rx >= 8  && rx < 13) return (font_o[row_idx] >> (12 - rx)) & 1;
        if (rx >= 14 && rx < 19) return (font_u[row_idx] >> (18 - rx)) & 1;
    }
    if (ry >= 33 && ry <= 39) {
        int row_idx = ry - 33;
        if (rx >= 2  && rx < 7)  return (font_W[row_idx]  >> (6  - rx)) & 1;
        if (rx >= 8  && rx < 13) return (font_i[row_idx]  >> (12 - rx)) & 1;
        if (rx >= 14 && rx < 19) return (font_n[row_idx]  >> (18 - rx)) & 1;
        if (rx >= 20 && rx < 25) return (font_ex[row_idx] >> (24 - rx)) & 1;
    }
    return false;
}

bool get_pixel_for_tower(int x, int y) {
    int rx = y, ry = x;
    if (ry < 4 || ry > 10) return false;
    int row_idx = ry - 4;
    if (rx >= 2  && rx < 7)  return (font_T[row_idx] >> (6  - rx)) & 1;
    if (rx >= 7  && rx < 12) return (font_o[row_idx] >> (11 - rx)) & 1;
    if (rx >= 14 && rx < 19) return (font_w[row_idx] >> (18 - rx)) & 1;
    if (rx >= 19 && rx < 24) return (font_e[row_idx] >> (23 - rx)) & 1;
    if (rx >= 25 && rx < 30) return (font_r[row_idx] >> (29 - rx)) & 1;
    return false;
}

bool get_pixel_for_stacker(int x, int y) {
    int rx = y, ry = x;
    if (ry < 4 || ry > 10) return false;
    int row_idx = ry - 4;
    if (rx >= 2  && rx < 7)  return (font_S[row_idx] >> (6  - rx)) & 1;
    if (rx >= 7  && rx < 12) return (font_t[row_idx] >> (11 - rx)) & 1;
    if (rx >= 12 && rx < 17) return (font_a[row_idx] >> (16 - rx)) & 1;
    if (rx >= 17 && rx < 22) return (font_k[row_idx] >> (21 - rx)) & 1;
    if (rx >= 21 && rx < 26) return (font_e[row_idx] >> (25 - rx)) & 1;
    if (rx >= 26 && rx < 31) return (font_r[row_idx] >> (30 - rx)) & 1;
    return false;
}

bool get_pixel_line_1(int x, int y) { return (y >= 4 && y <= 28) && (x == 12); }
bool get_pixel_line_2(int x, int y) { return (y >= 4 && y <= 28) && (x == 24); }
bool get_pixel_line_3(int x, int y) { return (y >= 4 && y <= 26) && (x == 58); }
bool get_pixel_box_1(int x, int y) { return (x >= 50 && x <= 56) && (y >= 4  && y <= 10); }
bool get_pixel_box_2(int x, int y) { return (x >= 50 && x <= 56) && (y >= 12 && y <= 18); }
bool get_pixel_box_3(int x, int y) { return (x >= 50 && x <= 56) && (y >= 20 && y <= 26); }
bool get_pixel_box_4(int x, int y) { return (x >= 42 && x <= 48) && (y >= 20 && y <= 26); }
bool get_pixel_box_5(int x, int y) { return (x >= 42 && x <= 48) && (y >= 12 && y <= 18); }
bool get_pixel_box_6(int x, int y) { return (x >= 34 && x <= 40) && (y >= 12 && y <= 18); }
bool get_pixel_anim_line(int x, int y) { return (y >= 0 && y <= 32) && (x >= 59 && x <= 63); }
bool get_pixel_mid_line(int x, int y)  { return (y >= 0 && y <= 32) && (x == 29); }
bool get_pixel_line_14(int x, int y)   { return (y >= 0 && y <= 32) && (x == 7); }

bool get_pixel_block_custom(int x, int y, int b_y, int x_start, int x_end) {
    return (x >= x_start && x < x_end) && (y >= b_y && y < b_y + BLOCK_HEIGHT);
}

int col_x_start(int col) {
    int xs = 52 - col * (BLOCK_WIDTH + 1);
    if (col == 4) xs -= 1;
    if (col == 5) xs -= 1;
    if (col == 6) xs -= 1;
    if (col == 7) xs -= 2;
    if (xs < 0) xs = 0;
    return xs;
}

bool get_pixel_pair(int x, int y, int b_y, int xs, int col) {
    if (get_pixel_block_custom(x, y, b_y, xs, xs + BLOCK_WIDTH)) return true;
    if (col < 4 && get_pixel_block_custom(x, y, b_y + BLOCK_GAP, xs, xs + BLOCK_WIDTH)) return true;
    return false;
}

void init_hub75_pio(PIO pio, uint sm, uint offset) {
    pio_sm_config c = hub75_program_get_default_config(offset);
    sm_config_set_out_pins(&c, DATA_BASE_PIN, 6);
    for (int i = 0; i < 6; i++) pio_gpio_init(pio, DATA_BASE_PIN + i);
    pio_sm_set_consecutive_pindirs(pio, sm, DATA_BASE_PIN, 6, true);
    sm_config_set_sideset_pins(&c, CLK_PIN);
    pio_gpio_init(pio, CLK_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, CLK_PIN, 1, true);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

int main() {
    stdio_init_all();
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
    absolute_time_t lost_screen_start;

    int frozen_y[MAX_LAYERS];
    for (int i = 0; i < MAX_LAYERS; i++) frozen_y[i] = -1;

    int num_frozen    = 0;
    int active_y      = 0;
    int active_dir    = 1;
    int frame_counter = 0;
    int frame_delay   = 30;

    #define STACK_TOLERANCE 0

    while (true) {
        if (app_state == 3) {
            if (absolute_time_diff_us(lost_screen_start, get_absolute_time()) >= 2000000LL) {
                app_state = 0;
            }
        }

        bool current_button_state = gpio_get(BUTTON_PIN);
        if (!current_button_state && last_button_state) {
            if (absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
                last_press_time = get_absolute_time();

                if (app_state == 0) {
                    app_state     = 1;
                    num_frozen    = 0;
                    for (int i = 0; i < MAX_LAYERS; i++) frozen_y[i] = -1;
                    active_y      = 0;
                    active_dir    = 1;
                    frame_delay   = 30;
                    frame_counter = 0;

                } else if (app_state == 1) {
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

                        if (!valid_stack) {
                            app_state         = 3;
                            lost_screen_start = get_absolute_time();
                            num_frozen    = 0;
                            for (int i = 0; i < MAX_LAYERS; i++) frozen_y[i] = -1;
                            active_y      = 0;
                            active_dir    = 1;
                            frame_delay   = 30;
                            frame_counter = 0;
                            last_button_state = current_button_state;
                            continue;
                        }
                    }

                    frozen_y[num_frozen] = active_y;
                    num_frozen++;

                    frame_delay = (frame_delay * 75) / 100;
                    if (frame_delay < 1) frame_delay = 1;

                    if (num_frozen >= MAX_LAYERS) {
                        app_state = 2; // Win State
                    } else {
                        active_y      = 0;
                        active_dir    = 1;
                        frame_counter = 0;
                    }

                } else if (app_state == 2) {
                    app_state = 0;
                }
            }
        }
        last_button_state = current_button_state;

        if (app_state == 1) {
            frame_counter++;
            if (frame_counter >= frame_delay) {
                frame_counter = 0;
                active_y += active_dir;
                int y_limit = (num_frozen < 4) ? 19 : 26;
                if (active_y >= y_limit) {
                    active_y   = y_limit;
                    active_dir = -1;
                }
                if (active_y <= 0) {
                    active_y   = 0;
                    active_dir = 1;
                }
            }
        }

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
