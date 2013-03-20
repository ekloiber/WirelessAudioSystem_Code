#include "CODEC.h"

CODECClass CODEC;

#define PAGE_SELECT(x) write(0x00, x)

void CODECClass::begin() {
	SPI.setBitOrder(MSBFIRST);	//MSB first
	SPI.setDataMode(SPI_MODE1);	//CPOL=0, CPHA=1
	SPI.setClockDivider(SPI_CLOCK_DIV4);	//fosc/4 = 4MHz
	SPI.begin();
}

void CODECClass::on() {
	digitalWrite(RESET_CODEC, LOW);
}

void CODECClass::off() {
	digitalWrite(RESET_CODEC, HIGH);
}

void CODECClass::write(u8 nAddress, u8 nData) {
	SELECT_CODEC();
	// 7-bit address, LSB is 0
	SPI.transfer((nAddress << 1) & (~0x01));
	SPI.transfer(nData);
	DESELECT_CODEC();
	return;
}

u8 CODECClass::read(u8 nAddress) {
	u8 nData;
	SELECT_CODEC();
	//7-bit address, LSB is 1
	SPI.transfer((nAddress << 1) | 0x01);
	nData = SPI.transfer(0x00);	
	DESELECT_CODEC();
	return nData;
}

void CODECClass::hardReset() {
	digitalWrite(RESET_CODEC, LOW);
	delayMicroseconds(1);
	digitalWrite(RESET_CODEC, HIGH);
	delay(2);
	return;
}

void CODECClass::softReset() {
	PAGE_SELECT(0x00);
	write(0x01, 0x01);
	return;
}

void CODECClass::configure() {
	hardReset();
	PAGE_SELECT(0x00);
	softReset();

	//Clock
	write(0x0B, 0x81);	//NDAC on, div = 1
	write(0x0C, 0x82);	//MDAC on, div = 2
	write(0x0D, 0x00);	//DAC OSR = 128
	write(0x0E, 0x80);	// ^
	write(0x12, 0x81);	//NADC on, div = 1
	write(0x13, 0x82);	//NADC on, div = 2
	write(0x14, 0x80);	//ADC OSR = 128

	//Interface
	write(0x1B, 0x00);	//I2S, 16b, WCLK & BCLK inputs

	//Processing Block
	write(0x3C, 0x08);	//DAC: PRB_P8
	write(0x3D, 0x01);	//ADC: PRB_R1

	//Analog PS
	PAGE_SELECT(0x01);
	write(0x01, 0x08);	//Disable AVDD/DVDD connection
	write(0x02, 0x01);	//Enable AVDD LDO
	write(0x0A, 0x3B);	//Output common mode is 1.65V, Full chip CM is 0.9V, HP powered from LDO (1.8-3.6V)

	//Power up
	write(0x47, 0x31);	//Analog inputs power up in 3ms
	write(0x7B, 0x01);	//Reference powers up in 40ms
	write(0x14, 0x65);	//Headphone powers up in 50ms, charges for 5 time constants @ 6k ohms

	//DAC routing
	write(0x0C, 0x08);	//Left DAC -> Left headphone
	write(0x0D, 0x08);	//Right DAC -> Right headphone
	write(0x0E, 0x08);	//Left DAC -> Left line out
	write(0x0F, 0x08);	//Right DAC -> Right line out

	//ADC routing
	write(0x34, 0xC0);	//IN1L -> MICPGA L+, 40k
	write(0x36, 0xC0);	//CM -> MICPGA L-, 40k
	write(0x37, 0xC0);	//IN1R -> MICPGA R+, 40k
	write(0x39, 0xC0);	//CM -> MICPGA R-, 40k
	
	//1Hz filter L
	PAGE_SELECT(0x08);
	write(0x18, 0x7F);
	write(0x19, 0xFF);
	write(0x1A, 0x00);
	write(0x1C, 0x80);
	write(0x1D, 0x01);
	write(0x1E, 0x00);
	write(0x20, 0x7F);
	write(0x21, 0xFC);
	write(0x22, 0x00);

	//1Hz filter R
	PAGE_SELECT(0x09);
	write(0x20, 0x7F);
	write(0x21, 0xFF);
	write(0x22, 0x00);
	write(0x24, 0x80);
	write(0x25, 0x01);
	write(0x26, 0x00);
	write(0x28, 0x7F);
	write(0x29, 0xFC);
	write(0x2A, 0x00);

	return;
}

void CODECClass::lineOutOn() {
	u8 nReg;
	PAGE_SELECT(0x01);
	nReg = read(0x09);	//Read output driver register
	write(0x12, 0x00);	//Unmute left line out
	write(0x13, 0x00);	//Unmute right line out
	write(0x09, nReg | 0x0C);	//Power on line out
	DACOn();
	return;
}

void CODECClass::lineOutOff() {
	u8 nReg;
	PAGE_SELECT(0x01);
	nReg = read(0x09);
	write(0x12, 0x40);	//Mute left line out
	write(0x13, 0x40);	//Mute right line out
	write(0x09, nReg & 0xF3);	//Power off line out
	DACOff();
	return;
}

void CODECClass::headphoneOn() {
	u8 nReg;
	PAGE_SELECT(0x01);
	nReg = read(0x09);	//Read output driver register
	write(0x10, 0x00);	//Unmute left headphone
	write(0x11, 0x00);	//Unmute right headphone
	write(0x09, nReg | 0x30);	//Power on headphone
	DACOn();
	return;
}

void CODECClass::headphoneOff() {
	u8 nReg;
	PAGE_SELECT(0x01);
	nReg = read(0x09);
	write(0x10, 0x40);	//Mute left headphone
	write(0x11, 0x40);	//Mute right headphone
	write(0x09, nReg & 0xCF);	//Power off headphone
	DACOff();
	return;
}

void CODECClass::feedthroughOn() {
	PAGE_SELECT(0x01);
	write(0x0C, 0x04);	//IN1L -> HPL
	write(0x0D, 0x04);	//IN1R -> HPR
	return;
}

void CODECClass::feedthroughOff() {
	PAGE_SELECT(0x01);	
	write(0x0C, 0x08);	//Left DAC -> HPL
	write(0x0D, 0x08);	//Right DAC -> HPR
	return;
}

void CODECClass::setFeedthroughVolume(s8 nDBTimesTwo) {
	nDBTimesTwo = -nDBTimesTwo;
	if (nDBTimesTwo > 116) nDBTimesTwo = 116;	//Between +116 and 0 dB
	if (nDBTimesTwo < 0) nDBTimesTwo = 0;	// --> -72.3 to 0 dB (kinda close...)
	PAGE_SELECT(0x00);
	nDBTimesTwo &= 0x7F;
	write(0x22, nDBTimesTwo);
	write(0x23, nDBTimesTwo);
}

void CODECClass::ADCOn() {
	PAGE_SELECT(0x00);
	write(0x52, 0x00);	//Unmutes left and right ADC
	write(0x51, 0xC0);	//Powers on ADC
	return;
}

void CODECClass::ADCOff() {
	PAGE_SELECT(0x00);
	write(0x52, 0x88);	//Mutes left and right ADC
	write(0x51, 0x00);	//Powers off ADC
	return;
}

void CODECClass::DACOn() {
	PAGE_SELECT(0x00);
	write(0x3F, 0xD6);	//Powers on DAC
	return;
}

void CODECClass::DACOff() {
	PAGE_SELECT(0x00);
	write(0x3F, 0x00);	//Powers off DAC
}

void CODECClass::setDACVolume(s8 nDBTimesTwo) {
	if (nDBTimesTwo > 48) nDBTimesTwo = 48;	//Between +48 and -127 dB
	if (nDBTimesTwo < -127) nDBTimesTwo = -127;	// --> +24 to -63.5dB
	PAGE_SELECT(0x00);
	write(0x41, nDBTimesTwo);
	write(0x42, nDBTimesTwo);
	return;
}

void CODECClass::setDriverVolume(s8 nDB) {
	if (nDB > 29) nDB = 29;	//Between +29 and -6 dB
	if (nDB < -6) nDB = -6;
	if (nDB < 0) nDB &= 0x3F;	//translate s8 --> s6
	nDB &= 0x7F;	// Ensure not muted
	PAGE_SELECT(0x01);
	write(0x12, nDB);	//Set line out volume
	write(0x13, nDB);

	write(0x10, nDB);	//Set headphone volume
	write(0x11, nDB);
	return;
}

void CODECClass::setADCVolume(s8 nDBTimesTwo) {
	if (nDBTimesTwo > 40) nDBTimesTwo = 40;	//Between +40 and -24 dB
	if (nDBTimesTwo < -24) nDBTimesTwo = -24;	// --> +20 to -12dB
	PAGE_SELECT(0x00);
	nDBTimesTwo &= 0x7F;
	write(0x41, nDBTimesTwo);
	write(0x42, nDBTimesTwo);
	return;
}

void CODECClass::setMICPGAVolume(s8 nDBTimesTwo) {
	if (nDBTimesTwo > 95) nDBTimesTwo = 95;	//Between +95 and 0 dB
	if (nDBTimesTwo < 0) nDBTimesTwo = 0;	// --> +47.5 to 0 dB
	PAGE_SELECT(0x00);
	nDBTimesTwo &= 0x7F;
	write(0x3B, nDBTimesTwo);
	write(0x3C, nDBTimesTwo);
	return;
}
