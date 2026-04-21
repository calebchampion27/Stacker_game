#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "display.h"
#include "led.h"
#include "storage.h"
#include "controls.h"

// global game states
typedef enum
{
    STATE_INIT,
    STATE_MENU,
    STATE_PLAYING,
    STATE_LEVEL_SUCCESS,
    STATE_GAME_OVER
} GameState;

// breathing led on timer
volatile bool is_breathing = true;

int main()
{
    stdio_init_all();

    // initialize peripherals
    display_init();    // pio for LED screen
    led_init();        // pwm for RGB leds
    eeprom_init();     // i2c setup
    adc_volume_init(); // adc & gpio pins setup for volume/ potentiometer

    // initial variables & states
    GameState current_state = STATE_INIT;
    int high_score = eeprom_read_score();
    int current_score = 0;
    uint16_t current_volume = 0;
    uint32_t state_start_time = 0;
    int won_game = 0; // 0 = no continue game, 1 = yes, 2 = no end game
    bool last_button_state = false;

    // set breathing light
    add_repeating_timer_ms(10, breathing_timer_callback, NULL, &timer); // set timer for LEDs


    // main loop
    while (true)
    {
        // always read potentiometer & button
        current_volume = read_potentiometer();
        bool current_button_state = read_button();
        bool button_pressed = (current_button_state == true && last_button_state == false);
        last_button_state = current_button_state;

        

        switch (current_state)
        {
            case STATE_INIT: // store high score, go to menu
                current_state = STATE_MENU;
                break;

            case STATE_MENU: // show high score, get ready to play
                if (button_pressed)
                {
                    current_score = 0;
                    current_state = STATE_PLAYING;
                }
                break;

            case STATE_PLAYING:
                if (button_pressed)
                {
                    if (won_game == 1)
                    { // check if success
                        current_state = STATE_LEVEL_SUCCESS;
                        is_breathing = false;
                        leds_green();

                        // turn on sound
                        start_continuous_tone(500);
                        set_volume(500, current_volume);
                        state_start_time = to_ms_since_boot(get_absolute_time());
                    }
                    else if (won_game == 2)
                    {
                        current_state = STATE_GAME_OVER;
                        is_breathing = false;
                        leds_red();

                        // turn on sound
                        start_continuous_tone(150); // low pitch
                        set_volume(150, current_volume);
                        state_start_time = to_ms_since_boot(get_absolute_time());
                    }
                    else
                    { // game is still playing (not won or lost)
                        // move another block back and fourth
                        current_state = STATE_PLAYING;
                    }
                }
                break;

            case STATE_LEVEL_SUCCESS: // set LEDs green, play music, wait, then go to menu
                // turn off music and lights after 10 sec
                set_volume(523, current_volume);

                if (to_ms_since_boot(get_absolute_time()) - state_start_time >= 5000)
                {
                    stop_tone();
                    is_breathing = true;
                    current_state = STATE_MENU;

                    if (current_score > high_score) {
                        high_score = current_score;
                        eeprom_write_score(current_score);
                    }
                }
                break;

            case STATE_GAME_OVER: // set LEDs to red, wait, then go to menu
                set_volume(200, current_volume);

                if (to_ms_since_boot(get_absolute_time()) - state_start_time >= 5000) {
                    stop_tone();
                    is_breathing = true;
                    current_state = STATE_MENU;

                    if (current_score > high_score) {
                        high_score = current_score;
                        eeprom_write_score(current_score);
                    }
                }
                    break;
        }

        sleep_ms(10);
    }
    return 0;
}