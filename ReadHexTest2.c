#include <stdio.h>

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int
#define bool unsigned char
#define true 1
#define false 0

#define DATA_RECORD 0x00
#define EOF_RECORD 0x01

char read(FILE *pF) {
	char nResult;
	fscanf(pF, "%c", &nResult);
//	printf("%c\n", nResult);
	return nResult;
}

u8 hexVal(char nChar) {
	u8 nByte = (u8)nChar;
//Returns hex value of ASCII byte, 0xFF if not valid
	if ((nByte >= '0') && (nByte <= '9')) return ((nByte - '0') & 0x0F);
	if ((nByte >= 'a') && (nByte <= 'f')) return ((nByte - 'a' + 0x0A) & 0x0F);
	if ((nByte >= 'A') && (nByte <= 'F')) return ((nByte - 'A' + 0x0A) & 0x0F);
	return 0xFF;
}

u8 readByte(FILE *pF) {
	return ( (hexVal(read(pF)) << 4) | hexVal(read(pF)) );
}

u16 readInt(FILE *pF) {
	return ((hexVal(read(pF)) & 0x0F) << 12) |
			((hexVal(read(pF)) & 0x0F) << 8) |
			((hexVal(read(pF)) & 0x0F) << 4) |
			(hexVal(read(pF)) & 0x0F);
}

int parseIntelHex(FILE *pFile) {
	u8 nData;	//Interpreted byte
	u8 nByteCount;	//Line byte count
	u16 nAddress;	//Address
	u8 nRecordType;	//Record type
	u8 nChecksum;	//Checksum
	u8 nIndex = 0;	//Line index
	u16 nLineCount = 1;

	printf("Entering Parse Loop\n");

	while(!feof(pFile)) {	//While there's bytes available for reading
		if (read(pFile) == ':') {	//If read start of line
			printf("Line %u:\n", nLineCount);
			nLineCount++;

			nByteCount = readByte(pFile);
			printf("Byte count = %#hhX\n", nByteCount);

			nAddress = readInt(pFile);
			printf("Address = %#X\n", nAddress);

			nRecordType = readByte(pFile);
			printf("Record type = %#hhX\n", nRecordType);

			if (nRecordType == DATA_RECORD) {
				nIndex = 0;
				printf("Data = ");
				while (nIndex < nByteCount) {
					nData = readByte(pFile);
					printf("%hhX ", nData);
					nIndex++;
				}
				printf("\n");
			}

			else if (nRecordType == EOF_RECORD) {
				printf("EOF\n");
			}

			nChecksum = readByte(pFile);
			printf("Checksum = %#hhX\n\n", nChecksum);
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