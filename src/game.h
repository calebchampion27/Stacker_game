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

#define MAX_LAYERS 8
#define BLOCK_HEIGHT 6
#define BLOCK_WIDTH 6
#define BLOCK_GAP 7

bool get_pixel_you_lost(int x, int y);

bool get_pixel_you_win(int x, int y);

bool get_pixel_for_tower(int x, int y);

bool get_pixel_for_stacker(int x, int y);

bool get_pixel_line_1(int x, int y);
bool get_pixel_line_2(int x, int y);
bool get_pixel_line_3(int x, int y);
bool get_pixel_box_1(int x, int y);
bool get_pixel_box_2(int x, int y);
bool get_pixel_box_3(int x, int y);
bool get_pixel_box_4(int x, int y);
bool get_pixel_box_5(int x, int y);
bool get_pixel_box_6(int x, int y);
bool get_pixel_anim_line(int x, int y);
bool get_pixel_mid_line(int x, int y);
bool get_pixel_line_14(int x, int y);

bool get_pixel_block_custom(int x, int y, int b_y, int x_start, int x_end);

int col_x_start(int col);

bool get_pixel_pair(int x, int y, int b_y, int xs, int col);

void init_hub75_pio(PIO pio, uint sm, uint offset);