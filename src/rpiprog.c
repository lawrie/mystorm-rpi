#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wiringPi.h>
#include <bcm2835.h>

#define CFG_SS   10
#define CFG_RST  5
#define CFG_DONE 6 
#define CFG_S    1
#define CFG_OE   3

int debug = 0;

int main(int argc, char **argv)
{
        if (argc != 2) {
		printf("Usage: sudo %s bin-file\n",argv[0]);
		return 1;
	}
	
    	if (!bcm2835_init())
    	{	
      		printf("bcm2835_init failed. Are you running as root?\n");
      		return 1;
    	}	

	unsigned char buffer[1];

	FILE *f = strcmp(argv[1], "-") ? fopen(argv[1], "rb") : stdin;

	if (f == NULL) {
		printf("Failed to open %s: %s\n", argv[1], strerror(errno));
		return 1;
	}
	
	// Discard 4 comment bytes
	for(int i=0;i<4;i++) fread(&buffer, 1, 1, f);
	
	wiringPiSetup();

	pinMode(CFG_RST,  OUTPUT);
	pinMode(CFG_DONE, INPUT);
	pinMode(CFG_S,    OUTPUT);
	pinMode(CFG_OE,   OUTPUT);
        pinMode(CFG_SS,   OUTPUT);

        //printf("DONE is %d\n", digitalRead(CFG_DONE));
        //fflush(stdout);
	
	// Enable Mux and select RPi header
    	digitalWrite(CFG_S,   HIGH);
	digitalWrite(CFG_OE,  LOW);
	
	// Reset the Ice40	
	digitalWrite(CFG_RST, LOW);
	digitalWrite(CFG_SS, LOW);
	usleep(2000);
	
	digitalWrite(CFG_RST, HIGH);

	for (int i = 100; digitalRead(CFG_DONE) == HIGH; i--) {
		if (i == 0) {
			printf("Config: DONE took too long\n");
			break;
		}
	}
	
	usleep(2000);

	//int status = system("echo 'r' >/dev/ttyAMA0");
	//printf("Result from sending reset command %d\n", status);
	
	//usleep(1000000);

    	if (!bcm2835_spi_begin())
    	{
      		printf("bcm2835_spi_begin failed. Are you running as root??\n");
      		return 1;
    	}

    	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      
    	bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                  
    	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
    	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                  
    	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	
	// Send the file
    	for (int i=0;;i++) {
		int rc = fread(&buffer, 1, 1, f);
		if (i <= 240) { // Display first few bytes
			if (i % 40 == 0) {
				if (debug) {
					printf("\n");
					fflush(stdout);
				}
			}	
                	if (debug) if (i < 240) printf("%02x", buffer[0]);
		}

		if (rc <= 0) {
			printf("Written %d bytes\n", i);
			fflush(stdout);
			break;
		}

    		uint8_t read_data = bcm2835_spi_transfer(buffer[0]);

 		//if (i < 240) printf("%02x", read_data);
                if (debug && i > 0 && i % 1024 == 0) {
			printf("Written %d bytes\n", i);
			fflush(stdout);
		}
    	}	
	
	//digitalWrite(CFG_SS, HIGH);
        //pinMode(CFG_SS, INPUT);

	// Finish the configuration	
	for (int i = 100; digitalRead(CFG_DONE) != HIGH; i--) {
		if (i == 0) {
			printf("Error: DONE not set\n");
			break;
		}
    		bcm2835_spi_transfer(0);
	}
  
	for (int i = 0; i < 7; i++) {
    		bcm2835_spi_transfer(0);
        }

	bcm2835_spi_end();
    	bcm2835_close();

	// Turn the LEDs back on
	system("echo 'l' >/dev/ttyAMA0");
    	//digitalWrite(CFG_S,LOW);
 	
        int result = digitalRead(CFG_DONE);
        if (result) printf("Configure successful\n");
	else printf("Configure failed\n");

	return 0;
}

