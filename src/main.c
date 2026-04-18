#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "display.h"
#include "led.h"
#include "storage.h"
#include "controls.h"

// global game states
typedef enum {
    STATE_INIT,
    STATE_MENU,
    STATE_PLAYING,
    STATE_LEVEL_SUCCESS,
    STATE_GAME_OVER
} GameState;

int main() {
    stdio_init_all();

    // initialize peripherals
    display_init();  // pio for LED screen
    led_init();  // pwm for RGB leds
    eeprom_init();  // i2c setup
    adc_volume_init();  // adc & gpio pins setup for volume/ potentiometer

    // initial variables 
    GameState current_state = STATE_INIT;
    int high_score = 0;
    int current_score = 0;
    int current_volume = 0;
    bool won_game = false;

    // main loop
    while (true) {
        // always read potentiometer & button
        current_volume = read_potentiometer();
        current_volume = (current_volume * 100) / 4095;

        bool button_pressed = read_button();



        // testing
        current_state = STATE_MENU;





        
        switch (current_state) {
            case STATE_INIT:  // store high score, go to menu
                current_state = STATE_MENU;
                break;

            case STATE_MENU:  // show high score, get ready to play
                leds_breathing();
                if (button_pressed) {
                    current_score = 0;
                    current_state = STATE_PLAYING;
                }
                break;

            case STATE_PLAYING:                
                if (button_pressed) {
                    if (won_game) {  // check if success
                        current_state = STATE_LEVEL_SUCCESS;
                    } else {
                        current_state = STATE_GAME_OVER;
                    }

                    // move another block back and fourth
                }
                break;

            case STATE_LEVEL_SUCCESS:  // set LEDs green, play music, wait, then go to menu
                leds_green();
                current_score++;

                sleep_ms(50000);  // sleep 5 sec
                current_state = STATE_MENU;
                break;

            case STATE_GAME_OVER:  // set LEDs to red, compare high scores, wait, then go to menu
                leds_red();
                if (current_score > high_score) {
                    high_score = current_score;
                }

                sleep_ms(50000);  // sleep 5 sec
                current_state = STATE_MENU;
                break;
        }
        
        sleep_ms(10);
    }
    return 0;
}