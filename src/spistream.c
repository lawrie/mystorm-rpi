#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {

    if (!bcm2835_init()) {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }

    if (!bcm2835_spi_begin()) {
      printf("bcm2835_spi_begin failed. Are you running as root??\n");
      return 1;
    }
    
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);              
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                     
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);   
  
    unsigned char buffer[1];
 
    for(;;) {
      int rc = fread(&buffer, 1, 1, stdin); 
      if (rc < 0) break;
      bcm2835_spi_transfer(buffer[0]);
    }
 
    bcm2835_spi_end();
    bcm2835_close();
    return 0;
}

