#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hub75.pio.h"

#define DATA_BASE_PIN 2
#define PIN_A 8
#define PIN_B 9
#define PIN_C 10
#define PIN_D 11
#define CLK_PIN 12
#define LAT_PIN 13
#define OE_PIN  14
#define BUTTON_PIN 15

// --- Font Data (Keeping original font arrays) ---
const uint8_t font_T[7] = {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_o[7] = {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E};
const uint8_t font_w[7] = {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0A};
const uint8_t font_e[7] = {0x00, 0x00, 0x07, 0x04, 0x07, 0x04, 0x07};
const uint8_t font_r[7] = {0x00, 0x00, 0x06, 0x09, 0x08, 0x08, 0x08};
const uint8_t font_S[7] = {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E};
const uint8_t font_t[7] = {0x04, 0x04, 0x1F, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_a[7] = {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F};
const uint8_t font_k[8] = {0x08, 0x09, 0x0A, 0x0C, 0x0C, 0x0A, 0x09, 0x08};

// --- Title Pixel Helpers ---
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

// --- Animation Pixel Helpers ---
bool get_pixel_anim_line(int x, int y) {
    return (y >= 0 && y <= 32) && (x >= 59 && x <= 63);
}

// Updated block helper to accept custom X boundaries
bool get_pixel_block_custom(int x, int y, int b_y, int x_start, int x_end) {
    return (x >= x_start && x < x_end) && (y >= b_y && y < b_y + 6);
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

    // --- State Machine ---
    // 0: Title, 1: Col 1 Moving, 2: Col 1 Frozen + Col 2 Moving, 3: Both Frozen
    int app_state = 0; 
    bool last_button_state = true;
    absolute_time_t last_press_time = get_absolute_time();

    // Column 1 state
    int block1_y = 0, block1_dir = 1;
    // Column 2 state
    int block2_y = 0, block2_dir = 1;
    
    int frame_counter = 0;
    const int FRAME_DELAY = 15;

    while (true) {
        bool current_button_state = gpio_get(BUTTON_PIN);
        if (!current_button_state && last_button_state) {
            if (absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
                app_state = (app_state + 1) % 4; // Cycle through 4 states
                if (app_state == 0) {
                    block1_y = 0; block1_dir = 1;
                    block2_y = 0; block2_dir = 1;
                }
                last_press_time = get_absolute_time();
            }
        }
        last_button_state = current_button_state;

        // --- Animation Logic ---
        frame_counter++;
        if (frame_counter >= FRAME_DELAY) {
            frame_counter = 0;
            // Move Column 1 if active
            if (app_state == 1) {
                block1_y += block1_dir;
                if (block1_y >= 19) { block1_y = 19; block1_dir = -1; }
                if (block1_y <= 0)  { block1_y = 0;  block1_dir = 1; }
            }
            // Move Column 2 if active
            if (app_state == 2) {
                block2_y += block2_dir;
                if (block2_y >= 19) { block2_y = 19; block2_dir = -1; }
                if (block2_y <= 0)  { block2_y = 0;  block2_dir = 1; }
            }
        }

        // --- Refresh Display ---
        for (int row = 0; row < 16; row++) {
            for (int x = 63; x >= 0; x--) {
                uint32_t data = 0;

                if (app_state >= 1) { // Any animation state
                    // Static Side Rail
                    if (get_pixel_anim_line(x, row))      data |= 0b000001;
                    if (get_pixel_anim_line(x, row + 16)) data |= 0b001000;

                    // Draw Column 1 (x: 52-57)
                    if (get_pixel_block_custom(x, row,      block1_y, 52, 58)) data |= 0b000001;
                    if (get_pixel_block_custom(x, row + 16, block1_y, 52, 58)) data |= 0b001000;
                    if (get_pixel_block_custom(x, row,      block1_y + 7, 52, 58)) data |= 0b000001;
                    if (get_pixel_block_custom(x, row + 16, block1_y + 7, 52, 58)) data |= 0b001000;

                    // Draw Column 2 (x: 44-49) only if app_state >= 2
                    if (app_state >= 2) {
                        if (get_pixel_block_custom(x, row,      block2_y, 44, 50)) data |= 0b000001;
                        if (get_pixel_block_custom(x, row + 16, block2_y, 44, 50)) data |= 0b001000;
                        if (get_pixel_block_custom(x, row,      block2_y + 7, 44, 50)) data |= 0b000001;
                        if (get_pixel_block_custom(x, row + 16, block2_y + 7, 44, 50)) data |= 0b001000;
                    }
                } else {
                    // --- Title Screen ---
                    if (get_pixel_for_tower(x, row))               data |= 0b000001;
                    if (get_pixel_for_tower(x, row + 16))          data |= 0b001000;
                    if (get_pixel_for_stacker(x - 12, row))        data |= 0b000001;
                    if (get_pixel_for_stacker(x - 12, row + 16))   data |= 0b001000;
                    if (get_pixel_line_1(x, row))                  data |= 0b000001;
                    if (get_pixel_line_1(x, row + 16))             data |= 0b001000;
                    if (get_pixel_line_2(x, row))                  data |= 0b000001;
                    if (get_pixel_line_2(x, row + 16))             data |= 0b001000;
                    if (get_pixel_line_3(x, row))                  data |= 0b000001;
                    if (get_pixel_line_3(x, row + 16))             data |= 0b001000;
                    if (get_pixel_box_1(x, row))                   data |= 0b000001;
                    if (get_pixel_box_1(x, row + 16))              data |= 0b001000;
                    if (get_pixel_box_2(x, row))                   data |= 0b000001;
                    if (get_pixel_box_2(x, row + 16))              data |= 0b001000;
                    if (get_pixel_box_3(x, row))                   data |= 0b000001;
                    if (get_pixel_box_3(x, row + 16))              data |= 0b001000;
                    if (get_pixel_box_4(x, row))                   data |= 0b000001;
                    if (get_pixel_box_4(x, row + 16))              data |= 0b001000;
                    if (get_pixel_box_5(x, row))                   data |= 0b000001;
                    if (get_pixel_box_5(x, row + 16))              data |= 0b001000;
                    if (get_pixel_box_6(x, row))                   data |= 0b000001;
                    if (get_pixel_box_6(x, row + 16))              data |= 0b001000;
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
