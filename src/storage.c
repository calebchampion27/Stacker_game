#include "storage.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// code
void eeprom_init(void) {
  i2c_init(i2c0, 100000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);


}

void eeprom_write_score(int score) {
  uint8_t buffer[6];
    
  // address bytes 0 and 1
  buffer[0] = (HIGH_SCORE_ADDR >> 8) & 0xFF;  // high add
  buffer[1] = HIGH_SCORE_ADDR & 0xFF; // low add

  // 32 bit (4x4byte) data of score int
  buffer[2] = (score >> 24) & 0xFF;
  buffer[3] = (score >> 16) & 0xFF;
  buffer[4] = (score >> 8) & 0xFF;
  buffer[5] = score & 0xFF;

  // write 6 bytes to eeprom
  i2c_write_blocking(i2c0, EEPROM_ADDR, buffer, 6, false);
    
  sleep_ms(10); 
}

int eeprom_read_score(void) {
  uint8_t addr_buffer[2];
  addr_buffer[0] = (HIGH_SCORE_ADDR >> 8) & 0xFF; // high
  addr_buffer[1] = HIGH_SCORE_ADDR & 0xFF;  // low

  // find address
  i2c_write_blocking(i2c0, EEPROM_ADDR, addr_buffer, 2, true);

  // read from address
  uint8_t data_buffer[4];
  i2c_read_blocking(i2c0, EEPROM_ADDR, data_buffer, 4, false);

  // add together
  int score = (data_buffer[0] << 24) |
              (data_buffer[1] << 16) |
              (data_buffer[2] << 8)  |
               data_buffer[3];
                 
  return score;
}