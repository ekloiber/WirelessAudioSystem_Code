#include "pins_arduino.h"
#include "WProgram.h"
#include "CC8531.h"
#include "SPI.h"

CC8531Class CC8531;

void CC8531Class::begin() {
	SPI.setBitOrder(MSBFIRST);	//MSB first
	SPI.setDataMode(SPI_MODE0);	//CPOL=0, CPHA=0
	SPI.setClockDivider(SPI_CLOCK_DIV4);	//fosc/4 = 4MHz
	SPI.begin();
}

//////////////////////////////////////////////////////////
//Basic Functions (private)
//////////////////////////////////////////////////////////

void CC8531Class::waitReady() {
	u16 maxDelay = 5000;
	while (!digitalRead(MISO) && --maxDelay) {
		delayMicroseconds(2);
	}
	if (!maxDelay) waitReadyError = 1;
}

void CC8531Class::waitReadyMs(u16 nTimeout) {
	SELECT_CC8531();
	u32 maxDelay = ((u32)nTimeout)*100;
	while (!digitalRead(MISO) && --maxDelay) {
		delayMicroseconds(10);
	}
	if (!maxDelay) waitReadyError = 1;
	DESELECT_CC8531();
}

StatusWord_t CC8531Class::writeWord(u8 nFirstByte, u8 nSecondByte) {
//Writes (and reads) one word to CC8531
	StatusWord_t nStatus

	//Select CC8531 (CSn low)
	SELECT_CC8531();

	//Wait for MISO to be high
	waitReady();

	nStatus = (SPI.transfer(nFirstByte)) << 8;
	nStatus |= SPI.transfer(nSecondByte);

	return nStatus;
}

StatusWord_t CC8531Class::setAddr(u16 nAddress) {
//Sets the memory address to be used by subsequent read/write ops

	//[15]=0b0
	//[14:0]=15-bit memory byte address
	StatusWord_t nStatus;

	nStatus = writeWord((u8)((nAddress & 0x7F00) >> 8), (u8)(nAddress & 0x00FF));
	
	DESELECT_CC8531();
	
	return nStatus;
}

StatusWord_t CC8531Class::cmdReq(u8 nCmdType, u8 nNumBytes, u8* nCmdParam) {
//Executes a command implemented by the CC8531 bootloader or EHIF

	//[15:14]=0b11 (0xC0)
	//[13:8]=nCmdType
	//[7:0]=nNumBytes (8b)
	//Remaining = command param field
	StatusWord_t nStatus;
	u16 i=0;

	nStatus = writeWord((nCmdType | 0xC0), nNumBytes);
	
	for (i=0; i<nNumBytes; i++) {
		SPI.transfer(nCmdParam[i]);
	}

	DESELECT_CC8531();

	return nStatus;
}

StatusWord_t CC8531Class::write(u16 nNumBytes, u8* nData) {
//Writes the specified number of bytes to the memory location specified by a SET_ADDR() 
//operation or the memory buffer setup internally by a CMD_REQ() operation

	//[15:12]=0b1000 (0x80)
	//[11:0]=nNumBytes (12b)
	//Remaining = data field
	StatusWord_t nStatus;
	u16 i=0;

	nStatus = writeWord((u8)(0x0080 | ((nNumBytes >> 8) & 0x0F)), (u8)(nNumBytes & 0xFF));

	for (i=0; i<nNumBytes; i++) {
		SPI.transfer(nData[i]);
	}

	DESELECT_CC8531();

	return nStatus;
}

StatusWord_t CC8531Class::read(u16 nNumBytes, u8* nDataBuffer) {
//Reads the specified number of bytes from the result of a CMD_REQ() operation

	//[15:12]=0b1001 (0x90)
	//[11:0]=nNumBytes (12b)
	StatusWord_t nStatus;
	u16 i=0;

	nStatus = writeWord((u8)(0x90 | ((nNumBytes >> 8) & 0x0F)), (u8)(nNumBytes & 0xFF));

	for (i=0; i<nNumBytes; i++) {
		nDataBuffer[i] = SPI.transfer(0x00);
	}

	DESELECT_CC8531();

	return nStatus;
}

StatusWord_t CC8531Class::readBC(u8* nDataBuffer, u16* nDataLength) {
//Reads an unspecified number of bytes from the result of a CMD_REQ() operation

	//[15:12]=0b1010 (0xA0)
	StatusWord_t nStatus;
	u16 nNumBytes;
	u16 i=0;

	nStatus = writeWord(0xA0, 0x00);
	nNumBytes = writeWord(0xA0,0x00);

	if (nNumBytes > *nDataMaxLength) {
		nNumBytes = *nDataMaxLength;
	}
	*nDataLength = nNumBytes;

	for (i=0; i<nNumBytes; i++) {
		nDataBuffer[i] = SPI.transfer(0x00);
	}

	DESELECT_CC8531();

	return nStatus;
}

StatusWord_t CC8531Class::getStatus() {
//Returns the SPI status word	

	StatusWord_t nStatus;
	nStatus = writeWord(0x80, 0x00);
	DESELECT_CC8531();
	return nStatus;
}

void CC8531Class::sysReset(u8 waitReady) {
//Resets CC8531 and starts audio streaming application

	SELECT_CC8531();

	delay(2);	//Delay 2ms

	SPI.transfer(0xBF);
	SPI.transfer(0xFF);

	delayMicroseconds(RESET_DELAY_US);

	DESELECT_CC8531();

	if (waitReady) {
		delayMicroseconds(1);
		SELECT_CC8531();
		waitReadyMs(100);
		DESELECT_CC8531();
	}

	return;
}

void CC8531Class::bootReset() {
//Resets CC8531 and enters bootloader where the flash programming 
//interface is available

	SELECT_CC8531();

	delay(2);	

	SPI.transfer(0xB0);
	SPI.transfer(0x00);

	delayMicroseconds(RESET_DELAY_US);
	
	DESELECT_CC8531();

	delayMicroseconds(1);
	SELECT_CC8531();
	waitReadyMs(100);
	DESELECT_CC8531();
	return;
}

u8 CC8531::hexVal(u8 nByte) {
//Returns hex value of ASCII byte, 0xFF if not valid
	if ((nByte >= '0') && (nByte <= '9')) return ((nByte - '0') & 0x0F);
	if ((nByte >= 'a') && (nByte <= 'f')) return ((nByte - 'a' + 0x0A) & 0x0F);
	if ((nByte >= 'A') && (nByte <= 'F')) return ((nByte - 'A' + 0x0A) & 0x0F);
	return 0xFF;
}

u8 CC8531::readByte(File dataFile) {
//Reads byte from data stream: 0 1 A 4 F 7 -> 0x01, 0xA4, 0xF7
	return ( (hexVal(dataFile.read()) << 4) | hexVal(dataFile.read()) );
}

u16 CC8531::readInt(File dataFile) {
//Reads unsigned int from data stream: 1 8 F 0 -> 0x18F0
	return ((hexVal(dataFile.read()) & 0x0F) << 12) |
			((hexVal(dataFile.read()) & 0x0F) << 8) |
			((hexVal(dataFile.read()) & 0x0F) << 4) |
			(hexVal(dataFile.read()) & 0x0F);
}


//////////////////////////////////////////////////////////
//Basic Functions (public)
//////////////////////////////////////////////////////////

u8 getWaitReadyError() {
	return waitReadyError;
}

//////////////////////////////////////////////////////////
//Bootloader
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::BootloaderClass::unlock() {
//Unlocks the SPI commands provided by the bootloader for flash programming
	u8 nCmd[4] = {0x25, 0x05, 0xB0, 0x07};
	
	cmdReq(0x00, 4, nCmd);

	waitReadyMs(1);
	return getStatus();
}

StatusWord_t CC8531Class::BootloaderClass::flashMassErase() {
//Erases all flash contents, always run before programming
	u8 nCmd[4] = {0x25, 0x05, 0x13, 0x37};

	cmdReq(0x03, 4, nCmd);

	waitReadyMs(25);
	return getStatus();
}

StatusWord_t CC8531Class::BootloaderClass::flashPageProg(u16 nRAMAddr, u16* nFlashAddr) {
//Programs a single 1kB flash page using data which has been written to the given
//RAM location. 
	u8 nCmd[10] = {0,0, 0,0, 0x10,0x00, 0x25, 0x05, 0x13, 0x37};

	nCmd[0] = (u8)((nRAMAddr >> 8) & 0xFF);
	nCmd[1] = (u8)(nRAMAddr & 0xFF);
	nCmd[2] = (u8)((nFlashAddr >> 8) & 0xFF);
	nCmd[3] = (u8)(nFlashAddr & 0xFF);

	cmdReq(0x07, 10, nCmd);
	waitReadyMs(10);
	return getStatus();
}

StatusWord_t CC8531Class::BootloaderClass::flashVerify(u16 nByteCount, u8* nCRC) {
//Verifies the flash programming succeeded by calculating a CRC32 checksum over
//the entire image and comparing it against the expected checksum value. nByteCount
//is the size of the unpadded FW image (intel hex @ 0x801C)
	u8 nCmd[8] = {0x00,0x00,0x80,0x00,0x00,0x00,0,0};
	u16nStatus;

	nCmd[6] = (u8)((nByteCount >> 8) & 0xFF);
	nCmd[7] = (u8)(nByteCount & 0xFF);

	cmdReq(0x0F, 8, nCmd);
	waitReadyMs(15);
	return read(4, nCRC);
}

StatusWord_t CC8531Class::flashProgram(u8* nFlashImage) {
//Flash programming algorithm
	
	u16 nStatus;
	u16 nOffset;
	u8 nActualCRCVal[sizeof(u32)];
	u16 i;
	u32 nImageSize;
	const u8* nExpectedCRCVal

	nImageSize = (nFlashImage[0x1E] << 8) | nFlashImage[0x1F]);
	nExpectedCRCVal = nFlashImage + nImageSize;

	//Enter bootloader
	bootReset();
	nStatus = Bootloader.unlock();
	if (nStatus != BL_SPI_LOADER_READY) return nStatus;

	//Erase flash
	nStatus = Bootloader.flashMassErase();
	if (nStatus != BL_ERASE_DONE) return nStatus;

	//For each flash page
	for (nOffset=0x0000; nOffset<0x8000; nOffset += 0x0400) {
		
		//If the image size is reached, end programming
		if (nOffset >= nImageSize) break;

		//Write page data to start of available RAM area
		setAddr(0x6000);
		write(0x0400, nFlashImage + nOffset);

		//Program the page
		nStatus = Bootloader.flashPageProg(0x6000, 0x8000 + nOffset);
		if (nStatus != BL_PROG_DONE) return nStatus;
	}

	//Verify the flash contents
	nStatus = Bootloader.flashVerify(nByteCount, nActualCRCVal);
	for (i=0; i<sizeof(nActualCRCVal); i++) {
		if (nActualCRCVal[i] != nExpectedCRCVal[i]) {
			nStatus = BL_VERIFY_FAILED;
		}
	}

	sysReset(0);

	return nStatus;
}

StatusWord_t CC8531::flashHex(File dataFile) {
//Parses intel hex file
	u8 nData;			//Interpreted byte
	u8 nByteCount;		//Line byte count
	u16 nAddress;		//Address
	u8 nRecordType;		//Record type
	u8 nChecksum;		//Checksum
	u16 nDataIndex = 0;	//Data buffer index
	u8 nIndex = 0;		//Line byte index
	StatusWord_t nStatus;
	u16 nImageSize = 0;		//Size of image (0x7BFC = 31740B)
	u8 nExpectedCRCVal[4];	//Expected CRC value
	u8 nActualCRCVal[4];	//Actual CRC Value

	//Enter bootloader
	bootReset();
	nStatus = Bootloader.unlock();
	if (nStatus != BL_SPI_LOADER_READY) return nStatus;

	//Erase flash
	nStatus = Bootloader.flashMassErase();
	if (nStatus != BL_ERASE_DONE) return nStatus;

	SELECT_CC8531();
	setAddr(0x6000);
	writeWord(0x84, 0x00);	//Start write op of 0x0400 bytes

	while(dataFile.available()) {	//While there's bytes available for reading
		if (dataFile.read() == ':') {	//If read start of line
			
			nByteCount = readByte(dataFile);	//Read byte count
			
			nAddress = readInt(dataFile);		//Read 16b address

			nRecordType = readByte(dataFile);	//Read record type
			
			if (nRecordType == DATA_RECORD) {	//If data record
				nIndex = 0;
				while (nIndex < nByteCount) {	//Read data
					nData = readByte(dataFile);

					SPI.transfer(nData);

					nIndex++;
					nDataIndex++;

					if ((nDataIndex % 0x0400) == 0) {	//If multiple of 0x0400 (1kB)
						DESELECT_CC8531();	//Deselect to end write op

						//Program page
						nStatus = Bootloader.flashPageProg(0x6000, 0x8000 + nOffset);
						if (nStatus != BL_PROG_DONE) return nStatus;

						setAddr(0x6000);
						writeWord(0x84, 0x00);	//Start write op of 0x0400 bytes
					} 

					if (nDataIndex == 0x1E) nImageSize = (u16)nData << 8;
					if (nDataIndex == 0x1F) nImageSize |= (u16)nData;
					if (nDataIndex == nImageSize) nExpectedCRCVal[0] = nData;
					if (nDataIndex == (nImageSize + 1)) nExpectedCRCVal[1] = nData;
					if (nDataIndex == (nImageSize + 2)) nExpectedCRCVal[2] = nData;
					if (nDataIndex == (nImageSize + 3)) nExpectedCRCVal[3] = nData;
				}
			}

			else if (nRecordType == EOF_RECORD) {
				//Verify the flash contents
				nStatus = Bootloader.flashVerify(nByteCount, nActualCRCVal);
				for (i=0; i<sizeof(nActualCRCVal); i++) {
					if (nActualCRCVal[i] != nExpectedCRCVal[i]) {
						nStatus = BL_VERIFY_FAILED;
					}
				}

				sysReset(0);

				dataFile.close();
				return nStatus;
			}

			nChecksum = readByte(dataFile);
		}
	}
	
	return BL_PROG_FAILED;
}


//////////////////////////////////////////////////////////
//Device Info
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::InfoClass::getChipInfo(Chip_info_t* chipInfo) {
//Returns hardware/firmware info
	u8 nCmd[2] = {0, 0x00,0xB0};
	cmdReq(0x1F, 2, nCmd);
	return read(24, chipInfo);
}

StatusWord_t CC8531Class::InfoClass::getDeviceInfo(Device_info_t* devInfo) {
//Returns unique device ID and manufacturer-specific info
	cmdReq(0x1E, 0, 0);
	return read(12, devInfo);
}

//////////////////////////////////////////////////////////
//EHIF Control Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::EHIFCtrlClass::confInterruptMask(EHIF_mask_t* evtMask) {
//Configures EHIF interrupt pin event mask
	return cmdReq(0x19, 2, evtMask);
}

StatusWord_t CC8531Class::EHIFCtrlClass::clearEventFlags(EHIF_flags_t* flags) {
//Clear EHIF event flags
	return cmdReq(0x19, 1, flags);
}

//////////////////////////////////////////////////////////
//Audio Network Control and Status Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::NetworkClass::scan(Scan_param_t* scanParam, u8* nDataBuffer) {
//Used by protocol slaves to perform a scan of the entire band for matching 
//networks
	cmdReq(0x08, 16, scanParam);
	return readBC(nDataBuffer);
}

StatusWord_t CC8531Class::NetworkClass::join(Join_param_t* joinParam) {
//Used by protocol slaves to join a specific network or the first found that
//matches specified criteria
	return cmdReq(0x08, 18, joinParam);
}

StatusWord_t CC8531Class::NetworkClass::getStatusSlave(u8* nDataBuffer) {
//Returns status about current audio network status and other network nodes
	cmdReq(0x0A, 0, NULL);
	return readBC(nDataBuffer);
}

StatusWord_t CC8531Class::NetworkClass::confAudioChan(Audio_chan_t* audioChan) {
//Defines mapping of audio channels for slaves
	return cmdReq(0x0B, 16, audioChan);
}

StatusWord_t CC8531Class::NetworkClass::enableNetworkControl() {
//Enables formation/maintenance of network (protocol master)
	u8 nCmd[2] = {0x00, 0x01};
	return cmdReq(0x0C, 2, nCmd);
}

StatusWord_t CC8531Class::NetworkClass::disableNetworkControl() {
//Disables formation/maintenance of network (protocol master)
	u8 nCmd[2] = {0x00, 0x00};
	return cmdReq(0x0C, 2, nCmd);
}

StatusWord_t CC8531Class::NetworkClass::enablePairingSignal() {
//Master sends pairing signal
	u8 nCmd[2] = {0x00, 0x01};
	return cmdReq(0x0D, 2, nCmd);
}

StatusWord_t CC8531Class::NetworkClass::disablePairingSignal() {
//Master does not send pairing signal
	u8 nCmd[2] = {0x00, 0x00};
	return cmdReq(0x0D, 2, nCmd);
}

StatusWord_t CC8531Class::NetworkClass::setChanMask(Wireless_chan_mask_t* wirelessChanMask) {
//Master sets currently used or to be used RF channel mask or to enable/disable radio
//Selecting 0-5 RF channels suspends network maintenance
	return cmdReq(0x0E, 4, wirelessChanMask);
}

//////////////////////////////////////////////////////////
//Remote Control Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::RCClass::setData(RC_data_t* RCdata) {
//Slave communicates pre-defined or custom-defined remote control info to the master
	return cmdReq(0x2D, 13, RCdata);
}

StatusWord_t CC8531Class::RCClass::getData(RC_data_t* RCdataBuffer, u8* nSlaveID) {
//Master retreives remote control button/keyboard/mouse data
	cmdReq(0x2E, 1, nSlaveID);
	return read(13, RCdataBuffer);
}

//////////////////////////////////////////////////////////
//Data Side Channel Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::DSCClass::txData(DSC_tx_data_t* DSCdata, u16 nDataLength, u8* nData) {
//Queues data for side channel transmission
	cmdReq(0x04, 5, DSCdata);
	return write(nDataLength, nData);
}

StatusWord_t CC8531Class::DSCClass::rxData(DSC_rx_data* DSCdata, u16* nDataLength) {
//Receives data from side channel transmission if available
	cmdReq(0x05, 0, NULL);
	return readBC(DSCdata, nDataLength);
}

//////////////////////////////////////////////////////////
//Power Management Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::PowerClass::setPower(PM_set_state_t* PMstate) {
//Sets the device power state 
	return cmdReq(0x1C, 1, PMstate);
}

StatusWord_t CC8531Class::PowerClass::getPower(PM_get_state_t* PMstate) {
//Returns power management-related info
	cmdReq(0x1D, 0, NULL);
	return read(14, PMstate);
}

//////////////////////////////////////////////////////////
//Volume Control Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::VolumeClass::setVolume(Volume_set_t* VolumeSet) {
//Configures slave global/remote or local input or output volume
	return cmdReq(0x17, 4, VolumeSet);
}

StatusWord_t CC8531Class::VolumeClass::getVolume(Volume_get_t* VolumeGet, Volume_data_t* VolumeData) {
//Master: Returns slave global/remote or local input or output volume
//Slave: Returns local input, output volume and other info
	cmdReq(0x16, 1, VolumeGet);
	return read(2, VolumeData);
}

//////////////////////////////////////////////////////////
//RF and Audio Statistics Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::StatisticsClass::audioStats(Audio_stat_t* AudioStat, u16* nDataLength) {
//Requests and returns audio statistics gathered since the last command/chip reset
	cmdReq(0x11, 0, NULL);
	return readBC(AudioStat, nDataLength);
}

StatusWord_t CC8531Class::StatisticsClass::rfStats(RF_stat_t* RFstat, u16* nDataLength) {
//Requests and returns RF statistics gathered since the last command/chip reset
	cmdReq(0x10, 0, NULL);
	return readBC(RFstat, nDataLength);
}

//////////////////////////////////////////////////////////
//Utility Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::UtilityClass::getGIO(GIO_data_t* GIOdata) {
//Polls the current value of I/O pins GIO1 through GIO15
	cmdReq(0x2A, 0, NULL);
	return read(4, GIOdata);
}

StatusWord_t CC8531Class::UtilityClass::getFlash(Flash_param_t* FlashParam, u8* nData) {
//Reads data from non-volatile storage in the CC8531 internal flash memory
	cmdReq(0x2B, 1, FlashParam);
	return read(4, nData);
}

StatusWord_t CC8531Class::UtilityClass::setFlash(Flash_data_t* FlashData) {
//Writes data to non-volatile storage in the CC8531 internal flash memory
	return cmdReq(0x2C, 5, FlashData);
}

//////////////////////////////////////////////////////////
//RF Test Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::RFTestClass::txWave(RF_tx_test_t* txTest) {
//Outputs a continuous wave RF signal at a specified frequency
	return cmdReq(0x15, 3, txTest);
}

StatusWord_t CC8531Class::RFTestClass::txRand(RF_tx_test_t* txTest) {
//Outputs a pseudo-random modulated RF signal at a specified frequency
	return cmdReq(0x14, 3, txTest);
}

StatusWord_t CC8531Class::RFTestClass::rxWave(RF_rx_test_t* rxTest) {
//Enables continuous reception at a specific frequency
	return cmdReq(0x25, 2, rxTest);
}

StatusWord_t CC8531Class::RFTestClass::rxRSSI(u8* FreqOffset, s8* RSSI) {
//Measures RSSI at a specific frequency
	cmdReq(0x26, 1, FreqOffset);
	return read(1, RSSI);
}

StatusWord_t CC8531Class::RFTestClass::txError(RF_tx_error_t* txError) {
//Runs transmitter side of packet error rate test
	return cmdReq(0x13, 8, txError);
}

StatusWord_t CC8531Class::RFTestClass::rxError(RF_rx_error_param_t* rxError, RF_rx_error_data_t* rxData) {
//Runs receiver side of the packet error rate test
	cmdReq(0x12, 11, rxError);
	return read(244, rxData);
}

StatusWord_t CC8531Class::RFTestClass::networkSim(Network_sim_t* nwkSim) {
//Simulates the RF behavior of a master/slave without establishing a network
	return cmdReq(0x27, 14, nwkSim);
}

//////////////////////////////////////////////////////////
//Audio Test Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::AudioTestClass::genTone(Audio_tone_t* audioTone) {
//Enables/disables tone generation on a specific audio channel
	return cmdReq(0x20, 5, audioTone);
}

StatusWord_t CC8531Class::AudioTestClass::detectTone(u8* nChannel, Audio_det_t* detTone) {
//Estimates amplitude and frequency of the specified audio channel
	cmdReq(0x21, 1, nChannel);
	return read(4, detTone);
}

//////////////////////////////////////////////////////////
//IO Test Commands
//////////////////////////////////////////////////////////

StatusWord_t CC8531Class::IOTestClass::input(IOTest_param_t* IOparam, IOTest_data_t* IOdata) {
//Selected pins are configured as input and their logical value is returned
	cmdReq(0x22, 4, IOparam);
	return read(4, IOdata);
}

StatusWord_t CC8531Class::IOTestClass::output(IOTest_output_t* IOoutput) {
//Selected pins are configured as output and driven to the logical value in x_VAL
	cmdReq(0x23, 8, IOoutput);
}