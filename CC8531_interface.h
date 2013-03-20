//MSB first
//CPOL=0
//CPHA=0
//SPI.SetBitOrder(MSB_FIRST)
//SPI.setDataMode(SPI_MODE0)

#define SYS_RESET() SPI.transfer(0xBFFF)
#define BOOT_RESET() SPI.transfer(0xB000)
#define GET_STATUS() SPI.transfer(0x8000)

u16 SET_ADDR(u16 nAddress){
	return SPI.transfer(nAddress & ~(1<<15))
}

u16 CMD_REQ(u8 nCmdType, u8 nNumBytes, u8* nCmdParam){
	u8 i=0;
	u16 status=0;
	SPI.transfer((u16)nNumBytes | ((u16)(nCmdType & 0x1F) << 8) | 0x03 << 14)
	for (i=0;i<nNumBytes;i++) {
		status = SPI.transfer(nCmdParam[i]);
	}
	return status;
}

u16 READ(u16 nNumBytes){
	return SPI.transfer((nNumBytes & 0xFFF) | 0x09 << 12);
}

u16 WRITE(u16 nNumBytes, u8 nData) {
	SPI.transfer((nNumBytes & 0xFFF) | 0x08 << 12);
	return SPI.transfer(nData);
}

u16 WRITE_ARRAY(u16 nNumBytes, u8* nData) {
	u8 i=0;
	u16 status=0;

	for (i=0;i<nNumBytes;i++) {
		status = SPI.transfer(nData[i]);
	}
	return status;
}
