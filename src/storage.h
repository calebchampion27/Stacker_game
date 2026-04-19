#ifndef STORAGE_H
#define STORAGE_H

#define I2C_SDA 28
#define I2C_SCL 29
#define HIGH_SCORE_ADDR 0x0000
#define EEPROM_ADDR 0x50

void eeprom_init(void);

void eeprom_write_score(int);

int eeprom_read_score(void);

#endif