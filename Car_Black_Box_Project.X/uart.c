#include <xc.h>
#include "uart.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    SPEN = 1;
    
    /* Continuous Reception Enable Bit */
    CREN = 1;
    BRGH = 1;
    /* Baud Rate Setting Register */
    SPBRG = (FOSC / (16 * baud)) - 1;
}

void putchar(unsigned char data)
{
    TXREG = data;           //load data to register
    do
    {
        continue;
    }while(TXIF != 1);      //wait until transfer is done/until transmission flag becomes 1
    TXIF = 0;               //reset the transmission flag
}

void puts(const char *s)
{
    while (*s)
    {                   //SENDING EVERY CHARACTER OF STRING THROUGH PUTCHAR
        putchar(*s++);
    }
}