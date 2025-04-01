#include "EEprom.h"
#include "i2c.h"

unsigned char ext_eeprom_24C02_read(unsigned char addr) {
//    declare local var
    unsigned char data;
//sending start bit in I2C bus
    i2c_start();
//    sending slave address byte with write bit LSB
    i2c_write(SLAVE_WRITE_EEPROM);
//    writing address to I2C line to read data from that address
    i2c_write(addr);
//    restart I2C bus
    i2c_rep_start();
//    sending slave address byte with read bit LSB
    i2c_write(SLAVE_READ_EEPROM);
    
//    read data from SSPBUFF
    data = i2c_read(0);
//    sending stop bit to make ready the I2C bus for further transmissions
    i2c_stop();

    return data;
}

//function to write from ext eeprom
void ext_eeprom_24C02_byte_write(unsigned char addr, char data) {
//    send start bit in I2C bus
    i2c_start();
//    sending slave address byte with write bit LSB
    i2c_write(SLAVE_WRITE_EEPROM);
//    writing the address to the slave to write the data in that address
    i2c_write(addr);
//    writing data to the slave
    i2c_write(data);
//    sending stop bit to make ready the I2C bus for further transmissions
    i2c_stop();
}

//write a string to eeprom
void ext_eeprom_24C02_str_write(unsigned char addr, char *str) {
    while (*str != '\0')
    {
        ext_eeprom_24C02_byte_write(addr, *str);
        addr++;
        str++;
    }
}
