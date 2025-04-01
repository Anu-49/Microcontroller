# 1 "EEprom.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 285 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.00/packs/Microchip/PIC16Fxxx_DFP/1.3.42/xc8\\pic\\include/language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "EEprom.c" 2
# 1 "./EEprom.h" 1






unsigned char ext_eeprom_24C02_read(unsigned char addr);
void ext_eeprom_24C02_byte_write(unsigned char addr, char data);
void ext_eeprom_24C02_str_write(unsigned char addr, char *str);
# 2 "EEprom.c" 2
# 1 "./i2c.h" 1
# 10 "./i2c.h"
void init_i2c(unsigned long baud);
void i2c_start(void);
void i2c_rep_start(void);
void i2c_stop(void);
unsigned char i2c_read(unsigned char ack);
int i2c_write(unsigned char data);
# 3 "EEprom.c" 2

unsigned char ext_eeprom_24C02_read(unsigned char addr) {

    unsigned char data;

    i2c_start();

    i2c_write(0b10100000);

    i2c_write(addr);

    i2c_rep_start();

    i2c_write(0b10100001);


    data = i2c_read(0);

    i2c_stop();

    return data;
}


void ext_eeprom_24C02_byte_write(unsigned char addr, char data) {

    i2c_start();

    i2c_write(0b10100000);

    i2c_write(addr);

    i2c_write(data);

    i2c_stop();
}


void ext_eeprom_24C02_str_write(unsigned char addr, char *str) {
    while (*str != '\0')
    {
        ext_eeprom_24C02_byte_write(addr, *str);
        addr++;
        str++;
    }
}
