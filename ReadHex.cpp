#define DATA_RECORD 0x00
#define EOF_RECORD 0x01

u8 hexVal(u8 nByte) {
//Returns hex value of ASCII byte, 0xFF if not valid
	if ((nByte >= '0') && (nByte <= '9')) return ((nByte - '0') & 0x0F);
	if ((nByte >= 'a') && (nByte <= 'f')) return ((nByte - 'a' + 0x0A) & 0x0F);
	if ((nByte >= 'A') && (nByte <= 'F')) return ((nByte - 'A' + 0x0A) & 0x0F);
	return 0xFF;
}

u8 readByte(File dataFile) {
//Reads byte from data stream: 0 1 A 4 F 7 -> 0x01, 0xA4, 0xF7
	return ( (hexVal(dataFile.read()) << 4) | hexVal(dataFile.read()) );
}

u16 readInt(File dataFile) {
//Reads unsigned int from data stream: 1 8 F 0 -> 0x18F0
	return ((hexVal(dataFile.read()) & 0x0F) << 12) |
			((hexVal(dataFile.read()) & 0x0F) << 8) |
			((hexVal(dataFile.read()) & 0x0F) << 4) |
			(hexVal(dataFile.read()) & 0x0F);
}

int parseIntelHex(u8* pDataBuffer, u16 pDataLength, File dataFile) {
//Parses intel hex file
	u8 nData;			//Interpreted byte
	u8 nByteCount;		//Line byte count
	u16 nAddress;		//Address
	u8 nRecordType;		//Record type
	u8 nChecksum;		//Checksum
	u16 nDataIndex = 0;	//Data buffer index
	u8 nIndex = 0;		//Line byte index

	while(dataFile.available()) {	//While there's bytes available for reading
		if (dataFile.read() == ':') {	//If read start of line
			
			nByteCount = readByte(dataFile);	//Read byte count
			
			nAddress = readInt(dataFile);		//Read 16b address

			nRecordType = readByte(dataFile);	//Read record type
			
			if (nRecordType == DATA_RECORD) {
				nIndex = 0;
				while (nIndex < nByteCount) {
					nData = readByte(dataFile);
					if (nDataIndex >= pDataLength) {
						pDataBuffer[nDataIndex] = nData;
					}
					nIndex++;
					nDataIndex++;
				}
			}

			else if (nRecordType == EOF_RECORD) {
				dataFile.close();
				return 0;
			}

			nChecksum = readByte(dataFile);
		}
	}
	
	return -1;
}