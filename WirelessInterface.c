#include <SPI.h>
#include <SD.h>
#include <CC8531.h>
#include <CODEC.h>

/*
Function		Pin		Arduino Pin

MOSI			PB3		11
MISO			PB4		12
SCK				PB5		13

SS_CC8531		PC2		A2
RESET_CC8531	PC3		A3

MISO_SD			PD5		5
SCLK_SD			PD6		6
MOSI_SD			PD7		7
SS_SD			PB0		8
SD IN			PD2		2

SS_CODEC		PC0		A0
RESET_CODEC		PB1		9

ON_RX			PC4		A4

GIO				PD3		3

Rx				PD0		0
Tx				PD1		1

NC				PB2		10
NC				PD4		4
*/

#define u8 uint8_t
#define u16 uint16_t

#define SS_CC8531 A2
#define SS_CODEC A0
#define SS_SD A1
#define SD_IN 2

#define RESET_CC8531 A3
#define RESET_CODEC 9

#define MISO2_PIN 5
#define SCK2_PIN 6
#define MOSI2_PIN 7
#define SS_SD 8
#define SS2_PIN 8

#define ON_RX A4

#define	LED 3

#define SOFTWARE_SPI	//Define software SPI for SD card

#define SELECT_CC8531() fastDigitalWrite(SS_CC8531, LOW)
#define DESELECT_CC8531() fastDigitalWrite(SS_CC8531, HIGH)
#define SELECT_CODEC() fastDigitalWrite(SS_CODEC, LOW)
#define DESELECT_CODEC() fastDigitalWrite(SS_CODEC, HIGH)
#define SELECT_SD() fastDigitalWrite(SS_SD, LOW)
#define DESELECT_SD() fastDigitalWrite(SS_SD, HIGH)

/*
D0	D1	D2	Status	Limit
0	0	0	Off		100mA
0	0	1	On		100mA
0	1	0	Off		500mA
0	1	1	On		500mA
1	0	0	Off		1A
1	0	1	On		1A
1	1	0	Off		2.5mA
1	1	1	Off		500uA
*/


////////////////////////////////////////
// Configure
////////////////////////////////////////

#define DEBOUNCE_SAMPLES 10		//Samples to debounce switch
#define USE_CODEC 1	//Using codec?
#define MASTER_PRODID 1		//Product ID of master image
#define SLAVE_PRODID 2		//Product ID of slave image

////////////////////////////////////////
// Global variables
////////////////////////////////////////

static bool bCC8531Programmed = false;	//Was CC8531 already programmed?
static bool bMaster;

////////////////////////////////////////
// Functions
////////////////////////////////////////

bool debounced(u8 nPin) {
	for (i=0; i<DEBOUNCE_SAMPLES; i++) {
		if (!digitalRead(nPin)) return false;
	}
	return true;
}

StatusWord_t programCC8531() {
	Device_info_t devInfo;
	File dataFile;
	StatusWord_t nStatus;

	CC8531.Info.getDeviceInfo(&devInfo);	//Check to see existing product ID
	if (bMaster) {
		if (devInfo.PROD_ID != MASTER_PRODID) {
			SD.begin(SS_SD);
			dataFile = SD.open("master.hex");
			CC8531.flashHex(dataFile);
			bCC8531Programmed = true;
		}
	}
	else {
		if (devInfo.PROD_ID != SLAVE_PRODID) {
			SD.begin(SS_SD);
			dataFile = SD.open("slave.hex");
			CC8531.flashHex(dataFile);
			bCC8531Programmed = true;
		}
	}
}

////////////////////////////////////////

void setup() {
	Serial.begin(9600);

	//Set some pins & sheeeeeeiiiiit
	pinMode(SS_CC8531, OUTPUT);
	pinMode(SS_CC8531, OUTPUT);

	pinMode(SS_SD, OUTPUT);
	pinMode(SPI_MISO_PIN, OUTPUT);
	pinMode(SPI_SCK_PIN, OUTPUT);
	pinMode(SPI_MOSI_PIN, OUTPUT);

	pinMode(RESET_CC8531, OUTPUT);
	pinMode(RESET_CODEC, OUTPUT);
	
	pinMode(SD_IN, INPUT);

	pinMode(ON_RX, INPUT);

	pinMode(LED, OUTPUT);

	//Deselect all SPI
	DESELECT_SD();
	DESELECT_CODEC();
	DESELECT_CC8531();

	//Determine if master or slave
	if(debounced(ON_RX)) bMaster = false;
	else bMaster = false;

	//Program CODEC
	if (USE_CODEC) {
		CODEC.begin();
		CODEC.configure();
		CODEC.lineOutOn();
		CODEC.headphoneOn();
		CODEC.ADCOn();
		//Set all volume to 0dB
		CODEC.setDACVolume(0);
		CODEC.setDriverVolume(0);
		CODEC.setADCVolume(0);
		CODEC.setMICPGAVolume(0);

		//If master, enable feedthrough, set volume to 0dB
		if (bMaster) {
			CODEC.feedthroughOn();
			CODEC.setFeedthroughVolume(0);
		}
	}

	CC8531.begin();
}

////////////////////////////////////////

void loop() {
	

}

