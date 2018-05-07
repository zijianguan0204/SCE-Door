/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 *
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 *
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 *
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 *
 * @license Released into the public domain.
 *
 */

#include "MFRC522.h"
#include <esp_log.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_spi_flash.h"
#include "rom/spi_flash.h"
#include <string.h>
#include <stdlib.h>

static const char LOG_TAG[] = "DumpInfo";

MFRC522 mfrc522;  // Create MFRC522 instance

static uint32_t g_wbuf[SPI_FLASH_SEC_SIZE / 4];
static uint32_t g_rbuf[SPI_FLASH_SEC_SIZE / 4];
		//Why static????


void dumpInfo() {
	//printf("Starting dump");
	mfrc522.PCD_Init();		// Init MFRC522
	printf("Init");
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	printf("Dump to serial");
	ESP_LOGD(LOG_TAG, "Scan PICC to see UID, SAK, type, and data blocks...");
	printf("Logd");

	uint32_t base_addr = 0x200000;

	while(1) {
		
		// Look for new cards
		if ( ! mfrc522.PICC_IsNewCardPresent()) 	continue;

		// Select one of the cards
		if ( ! mfrc522.PICC_ReadCardSerial()) 		continue;

		// Dump debug info about the card; PICC_HaltA() is automatically called
		mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));
		int i;
		i = mfrc522.uid.size;
		printf("The serial number size is %i\n", i);

		
		int j;
		for (int k = 0; k<sizeof(mfrc522.uid.uidByte); k++)
		{
			j = mfrc522.uid.uidByte[k];
			//g_wbuf |= (j <<(k*8) );	//Only works for 4 bytes
			g_wbuf[k] = j;

			printf("%i", j);
		}
		

		cout <<"Store the data ";
		for(int i = 0; i < sizeof(g_wbuf);i++)
		{
			cout << g_wbuf[i];
		}
		cout <<" " << " starting from base address 200000"<<endl;
		//printf("\n Storing %zu at %x\n", g_wbuf[0], base_addr);
        spi_flash_write(base_addr, g_wbuf, sizeof(g_wbuf));
        
        spi_flash_read(base_addr, g_rbuf, sizeof(g_rbuf));
        printf("reading %zu from flash\n", g_rbuf);

        if(g_wbuf[0] != g_rbuf[0]){
        	printf("Data stored at address not equal to UID");
        }else {

        	printf("UID successfully stored %zu at address at %x\n", g_wbuf[0],base_addr);
     		base_addr += SPI_FLASH_SEC_SIZE;
    	}


	}
}
