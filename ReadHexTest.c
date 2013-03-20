#include <stdio.h>

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define bool char
#define true 1
#define false 0

#define DATA_RECORD 0x00
#define EOF_RECORD 0x01

u8 read(FILE *pF) {
	char nResult;
	fscanf(pF, "%c", &nResult);
	//printf("%c\n", nResult);
	return nResult;
}

u8 hexVal(char nByte) {
//Returns hex value of ASCII byte, 0xFF if not valid
	if ((nByte >= '0') && (nByte <= '9')) return (u8)(nByte - '0');
	if ((nByte >= 'a') && (nByte <= 'f')) return (u8)(nByte - 'a' + 0x0A);
	if ((nByte >= 'A') && (nByte <= 'F')) return (u8)(nByte - 'A' + 0x0A);
	return 0xFF;
}

int parseIntelHex(FILE *pFile) {
	u8 nReadData;	//Raw byte
	u8 nHexVal;	//Interpreted nibble
	u8 nData;	//Interpreted byte
	u8 nByteCount;	//Line byte count
	u16 nAddress;	//Address
	u8 nRecordType;	//Record type
	u8 nChecksum;	//Checksum
	u16 nDataIndex = 0;	//Data buffer index
	u8 nIndex = 0;	//Line index
	u16 nLineCount = 0;

	bool bByteCount = false;
	bool bAddress = false;
	bool bRecordType = false;
	bool bData = false;
	bool bChecksum = false;
	printf("Entering Parse Loop\n");

	while(!feof(pFile)) {	//While there's bytes available for reading
		nReadData = read(pFile);
		nHexVal = hexVal(nReadData);

		if (nReadData == ':') {
			bByteCount = true;	//: indicates start of line, move to bytecount
			nIndex = 0;
			printf("Line %u:\n", nLineCount);
			nLineCount++;
		}
		
		else if (nHexVal != 0xFF) {	//If valid hex value
			//Read byte count
			if (bByteCount) {	//If at the start of the line, read bytecount
				//Byte count = MSB << 8 | LSB
				nByteCount = ((nHexVal & 0x0F) << 8) | hexVal(read(pFile));
				bByteCount = false;
				bAddress = true;
				printf("Byte count = %#x\n", nByteCount);
			}

			//Read address
			else if (bAddress) {	//If at the start of the address
				//Address = MSB << 24 | 2SB << 16 | 3SB << 8 | LSB
				nAddress = (((u16)(nHexVal) & 0x0F) << 8) |
							((u16)(hexVal(read(pFile))) & 0x0F);
				bAddress = false;
				bRecordType = true;
				printf("Address = %#x\n", nAddress);
			}

			//Read record type
			else if (bRecordType) {
				nRecordType = ((nHexVal & 0x0F) << 8) | hexVal(read(pFile));
				bRecordType = false;
				bData = true;

				if (nRecordType == EOF_RECORD) {
					printf("Record type = EOF\n");
					return 0;
				}
				printf("Record type = data\n");
			}

			//Read data
			else if (bData && (nRecordType == DATA_RECORD)) {
				nIndex = 0;
				while (nIndex < nByteCount) {
					//if (nDataIndex >= *pDataLength) return -1;
					nData = ((nHexVal & 0x0F) << 8) | hexVal(read(pFile));
					//*(pDataBuffer + nDataIndex) = nData;
					nDataIndex++;
					nIndex++;
					printf("%x", nData);
				}
				printf("\n");
				bData = false;
				bChecksum = true;
			}

			//Read checksum
			else if (bChecksum) {
				nChecksum = ((nHexVal & 0x0F) << 8) | hexVal(read(pFile));
				printf("Checksum = %#x\n", nChecksum);
				bChecksum = false;
			} 
		}
	}
	printf("Exiting Parse Loop\n");
	return 0;
}

int main(void) {
	FILE *pFile;
	pFile = fopen("test.hex","r");
	parseIntelHex(pFile);
	fclose(pFile);
	return 0;
}