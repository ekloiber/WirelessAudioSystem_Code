#ifndef _CODEC_H_INCLUDED
#define _CODEC_H_INCLUDED

#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>

#define u8 uint8_t
#define s8 int8_t
#define u16 uint16_t
#define s16 int16_t
#define u32 uint32_t

#define SS_CODEC A0
#define RESET_CODEC 9

#define SELECT_CODEC() digitalWrite(SS_CODEC, LOW)
#define DESELECT_CODEC() digitalWrite(SS_CODEC, HIGH)

class CODECClass {
private:
	static void write(u8, u8);
	static u8 read(u8);
public:	
	static void on();
	static void begin();

	static void hardReset();
	static void softReset();

	static void configure();

	static void lineOutOn();
	static void lineOutOff();

	static void headphoneOn();
	static void headphoneOff();

	static void feedthroughOn();
	static void feedthroughOff();
	static void setFeedthroughVolume();

	static void ADCOn();
	static void ADCOff();

	static void DACOn();
	static void DACOff();

	static void setDACVolume(s8);
	static void setDriverVolume(s8);
	static void setADCVolume(s8);
	static void setMICPGAVolume(s8);
};

extern CODECClass CODEC;

#endif