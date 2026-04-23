#include <game.h>

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
const uint8_t font_L[7]  = {0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0F};
const uint8_t font_s[7]  = {0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x0E};
const uint8_t font_ex[7] = {0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}; 
const uint8_t font_W[7]  = {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A};
const uint8_t font_i[7]  = {0x04, 0x00, 0x04, 0x04, 0x04, 0x04, 0x04};
const uint8_t font_n[7]  = {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11};

// --- Font Data for High Score (0-8) ---
const uint8_t font_numbers[9][7] = {
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}, // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}, // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F}, // 2
    {0x0E, 0x11, 0x01, 0x06, 0x01, 0x11, 0x0E}, // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}, // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}, // 5
    {0x0E, 0x11, 0x10, 0x1E, 0x11, 0x11, 0x0E}, // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}, // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}  // 8
};

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

bool get_pixel_high_score(int x, int y, int score) {
    int rx = y, ry = x;
    
    if (score < 0) score = 0;
    if (score > 8) score = 8;

    int start_y = 40; 
    int start_x = 4;  

    if (ry >= start_y && ry <= start_y + 6) {
        int row_idx = ry - start_y;
        if (rx >= start_x && rx <= start_x + 4) {
            return (font_numbers[score][row_idx] >> (4 - (rx - start_x))) & 1;
        }
    }
    return false;
}