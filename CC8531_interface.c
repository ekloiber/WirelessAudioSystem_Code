/*
	Function		Pin
	MOSI			PB3
	MISO			PB4
	SCK				PB5

	SS_CC8531		PC2
	RESET_CC8531	PC3

	SS_SD			PC1
	SD IN			PD2

	SS_CODEC		PC0

	D2				PD5
	D1				PD6
	D0				PD7

	ON_RX			PC4
	ON_TX			PC5

	GIO				PD3
*/

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define DD_MOSI DDB3
#define DD_MISO DDB4
#define DD_SCK DDB5
#define P_MOSI PB3
#define P_MISO PB4
#define P_SCK PB5

#define PORT_SS PORTC
#define SS_CC8531 PC2
#define SS_CODEC PC0
#define SS_SD PC1

#define _BV(x) (1<<x)
	
#define SPI_DIV _BV(SPR0)

#define SELECT_CC8531 (PORT_SS &= ~_BV(SS_CC8531))
#define DESELECT_CC8531 (PORT_SS |= _BV(SS_CC8531))
#define SELECT_CODEC (PORT_SS &= ~_BV(SS_CODEC))
#define DESELECT_CODEC (PORT_SS |= _BV(SS_CODEC))
#define SELECT_SD (PORT_SS &= ~_BV(SS_SD))
#define DESELECT_SD (PORT_SS |= _BV(SS_SD))

/*
		SPI Clock Divider 

	SPI2X	SPR1	SPR0	SCK Divider
	0		0		0		4
	0		0		1		16
	0		1		0		64
	0		1		1		128
	1		0		0		2
	1		0		1		8
	1		1		0		32
	1		1		1		64
*/

void SPI_Init(void) {
	//Set MOSI and SCK output
	DDR_SPI |= (_BV(DD_MOSI) | _BV(DD_SCK));
	//Enable SPI master, set clock divider
	//(MSB first, CPOL, CPHA = 0)
	SPCR = _BV(SPE) | _BV(MSTR) | (SPI_DIV);
}

void SPI_WRITE(u8 nData) {
	SPDR = nData;
	while(!(SPSR & _BV(SPIF)))
		;
	return;
}

void SPI_READ() {
	return SPDR;
}