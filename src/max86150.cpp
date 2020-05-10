/***************************************************
  Arduino library written for the Maxim MAX86150 ECG and PPG integrated sensor

	Written by Ashwin Whitchurch, ProtoCentral Electronics (www.protocentral.com)

  Based on code written by Peter Jansen and Nathan Seidle (SparkFun) for the MAX30105 sensor
  BSD license, all text above must be included in any redistribution.
 *****************************************************/

#include "max86150.h"

static const uint8_t MAX86150_INTSTAT1 =		0x00;
static const uint8_t MAX86150_INTSTAT2 =		0x01;
static const uint8_t MAX86150_INTENABLE1 =		0x02;
static const uint8_t MAX86150_INTENABLE2 =		0x03;

static const uint8_t MAX86150_FIFOWRITEPTR = 	0x04;
static const uint8_t MAX86150_FIFOOVERFLOW = 	0x05;
static const uint8_t MAX86150_FIFOREADPTR = 	0x06;
static const uint8_t MAX86150_FIFODATA 		=		0x07;

static const uint8_t MAX86150_FIFOCONFIG = 		0x08;
static const uint8_t MAX86150_FIFOCONTROL1= 	0x09;
static const uint8_t MAX86150_FIFOCONTROL2 = 	0x0A;

static const uint8_t MAX86150_SYSCONTROL = 	0x0D;
static const uint8_t MAX86150_PPGCONFIG1 = 		0x0E;
static const uint8_t MAX86150_PPGCONFIG2 = 		0x0F;
static const uint8_t MAX86150_LED_PROX_AMP = 	0x10;

static const uint8_t MAX86150_LED1_PULSEAMP = 	0x11;
static const uint8_t MAX86150_LED2_PULSEAMP = 	0x12;
static const uint8_t MAX86150_LED_RANGE 		= 	0x14;
static const uint8_t MAX86150_LED_PILOT_PA 	= 	0x15;

static const uint8_t MAX86150_ECG_CONFIG1 	= 	0x3C;
static const uint8_t MAX86150_ECG_CONFIG3 	= 	0x3E;
static const uint8_t MAX86150_PROXINTTHRESH = 	0x10;

static const uint8_t MAX86150_PARTID = 			0xFF;

// MAX86150 Commands
static const uint8_t MAX86150_INT_A_FULL_MASK =		(byte)~0b10000000;
static const uint8_t MAX86150_INT_A_FULL_ENABLE = 	0x80;
static const uint8_t MAX86150_INT_A_FULL_DISABLE = 	0x00;

static const uint8_t MAX86150_INT_DATA_RDY_MASK = (byte)~0b01000000;
static const uint8_t MAX86150_INT_DATA_RDY_ENABLE =	0x40;
static const uint8_t MAX86150_INT_DATA_RDY_DISABLE = 0x00;

static const uint8_t MAX86150_INT_ALC_OVF_MASK = (byte)~0b00100000;
static const uint8_t MAX86150_INT_ALC_OVF_ENABLE = 	0x20;
static const uint8_t MAX86150_INT_ALC_OVF_DISABLE = 0x00;

static const uint8_t MAX86150_INT_PROX_INT_MASK = (byte)~0b00010000;
static const uint8_t MAX86150_INT_PROX_INT_ENABLE = 0x10;
static const uint8_t MAX86150_INT_PROX_INT_DISABLE = 0x00;

static const uint8_t MAX86150_SAMPLEAVG_MASK =	(byte)~0b11100000;
static const uint8_t MAX86150_SAMPLEAVG_1 = 	0x00;
static const uint8_t MAX86150_SAMPLEAVG_2 = 	0x20;
static const uint8_t MAX86150_SAMPLEAVG_4 = 	0x40;
static const uint8_t MAX86150_SAMPLEAVG_8 = 	0x60;
static const uint8_t MAX86150_SAMPLEAVG_16 = 	0x80;
static const uint8_t MAX86150_SAMPLEAVG_32 = 	0xA0;

static const uint8_t MAX86150_ROLLOVER_MASK = 	0xEF;
static const uint8_t MAX86150_ROLLOVER_ENABLE = 0x10;
static const uint8_t MAX86150_ROLLOVER_DISABLE = 0x00;

static const uint8_t MAX86150_A_FULL_MASK = 	0xF0;

static const uint8_t MAX86150_SHUTDOWN_MASK = 	0x7F;
static const uint8_t MAX86150_SHUTDOWN = 		0x80;
static const uint8_t MAX86150_WAKEUP = 			0x00;

static const uint8_t MAX86150_RESET_MASK = 		0xFE;
static const uint8_t MAX86150_RESET = 			0x01;

static const uint8_t MAX86150_MODE_MASK = 		0xF8;
static const uint8_t MAX86150_MODE_REDONLY = 	0x02;
static const uint8_t MAX86150_MODE_REDIRONLY = 	0x03;
static const uint8_t MAX86150_MODE_MULTILED = 	0x07;

static const uint8_t MAX86150_ADCRANGE_MASK = 	0x9F;
static const uint8_t MAX86150_ADCRANGE_2048 = 	0x00;
static const uint8_t MAX86150_ADCRANGE_4096 = 	0x20;
static const uint8_t MAX86150_ADCRANGE_8192 = 	0x40;
static const uint8_t MAX86150_ADCRANGE_16384 = 	0x60;

static const uint8_t MAX86150_SAMPLERATE_MASK = 0xE3;
static const uint8_t MAX86150_SAMPLERATE_50 = 	0x00;
static const uint8_t MAX86150_SAMPLERATE_100 = 	0x04;
static const uint8_t MAX86150_SAMPLERATE_200 = 	0x08;
static const uint8_t MAX86150_SAMPLERATE_400 = 	0x0C;
static const uint8_t MAX86150_SAMPLERATE_800 = 	0x10;
static const uint8_t MAX86150_SAMPLERATE_1000 = 0x14;
static const uint8_t MAX86150_SAMPLERATE_1600 = 0x18;
static const uint8_t MAX86150_SAMPLERATE_3200 = 0x1C;

static const uint8_t MAX86150_PULSEWIDTH_MASK = 0xFC;
static const uint8_t MAX86150_PULSEWIDTH_69 = 	0x00;
static const uint8_t MAX86150_PULSEWIDTH_118 = 	0x01;
static const uint8_t MAX86150_PULSEWIDTH_215 = 	0x02;
static const uint8_t MAX86150_PULSEWIDTH_411 = 	0x03;

static const uint8_t MAX86150_SLOT1_MASK = 		0xF0;
static const uint8_t MAX86150_SLOT2_MASK = 		0x0F;
static const uint8_t MAX86150_SLOT3_MASK = 		0xF0;
static const uint8_t MAX86150_SLOT4_MASK = 		0x0F;

static const uint8_t SLOT_NONE 				= 			0x00;
static const uint8_t SLOT_RED_LED 		= 			0x01;
static const uint8_t SLOT_IR_LED 			= 			0x02;
static const uint8_t SLOT_RED_PILOT 	=				0x09;
static const uint8_t SLOT_IR_PILOT 		= 			0x0A;
static const uint8_t SLOT_ECG					= 			0x0D;

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x1E;

MAX86150::MAX86150() {
  // Constructor
}

boolean MAX86150::begin(TwoWire &wirePort, uint32_t i2cSpeed, uint8_t i2caddr)
{
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  _i2cPort->begin();
  _i2cPort->setClock(i2cSpeed);

  _i2caddr = i2caddr;

  // Step 1: Initial Communication and Verification
  // Check that a MAX86150 is connected
  if (readPartID() != MAX_30105_EXPECTEDPARTID) {
    // Error -- Part ID read from MAX86150 does not match expected part ID.
    // This may mean there is a physical connectivity problem (broken wire, unpowered, etc).
    return false;
  }
  return true;
}

//
// Configuration
//

//Begin Interrupt configuration
uint8_t MAX86150::getINT1(void)
{
  return (readRegister8(_i2caddr, MAX86150_INTSTAT1));
}
uint8_t MAX86150::getINT2(void) {
  return (readRegister8(_i2caddr, MAX86150_INTSTAT2));
}

void MAX86150::enableAFULL(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_A_FULL_MASK, MAX86150_INT_A_FULL_ENABLE);
}
void MAX86150::disableAFULL(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_A_FULL_MASK, MAX86150_INT_A_FULL_DISABLE);
}

void MAX86150::enableDATARDY(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_DATA_RDY_MASK, MAX86150_INT_DATA_RDY_ENABLE);
}
void MAX86150::disableDATARDY(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_DATA_RDY_MASK, MAX86150_INT_DATA_RDY_DISABLE);
}

void MAX86150::enableALCOVF(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_ALC_OVF_MASK, MAX86150_INT_ALC_OVF_ENABLE);
}
void MAX86150::disableALCOVF(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_ALC_OVF_MASK, MAX86150_INT_ALC_OVF_DISABLE);
}

void MAX86150::enablePROXINT(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_PROX_INT_MASK, MAX86150_INT_PROX_INT_ENABLE);
}
void MAX86150::disablePROXINT(void) {
  bitMask(MAX86150_INTENABLE1, MAX86150_INT_PROX_INT_MASK, MAX86150_INT_PROX_INT_DISABLE);
}
//End Interrupt configuration

void MAX86150::softReset(void) {
  bitMask(MAX86150_SYSCONTROL, MAX86150_RESET_MASK, MAX86150_RESET);

  // Poll for bit to clear, reset is then complete
  // Timeout after 100ms
  unsigned long startTime = millis();
  while (millis() - startTime < 100)
  {
    uint8_t response = readRegister8(_i2caddr, MAX86150_SYSCONTROL);
    if ((response & MAX86150_RESET) == 0) break; //We're done!
    delay(1); //Let's not over burden the I2C bus
  }
}

void MAX86150::shutDown(void) {
  // Put IC into low power mode (datasheet pg. 19)
  // During shutdown the IC will continue to respond to I2C commands but will
  // not update with or take new readings (such as temperature)
  bitMask(MAX86150_SYSCONTROL, MAX86150_SHUTDOWN_MASK, MAX86150_SHUTDOWN);
}

void MAX86150::wakeUp(void) {
  // Pull IC out of low power mode (datasheet pg. 19)
  bitMask(MAX86150_SYSCONTROL, MAX86150_SHUTDOWN_MASK, MAX86150_WAKEUP);
}

void MAX86150::setLEDMode(uint8_t mode) {
  // Set which LEDs are used for sampling -- Red only, RED+IR only, or custom.
  // See datasheet, page 19
  //bitMask(MAX86150_PPGCONFIG1, MAX86150_MODE_MASK, mode);
}

void MAX86150::setADCRange(uint8_t adcRange) {
  // adcRange: one of MAX86150_ADCRANGE_2048, _4096, _8192, _16384
  //bitMask(MAX86150_PARTICLECONFIG, MAX86150_ADCRANGE_MASK, adcRange);
}

void MAX86150::setSampleRate(uint8_t sampleRate) {
  // sampleRate: one of MAX86150_SAMPLERATE_50, _100, _200, _400, _800, _1000, _1600, _3200
  //bitMask(MAX86150_PARTICLECONFIG, MAX86150_SAMPLERATE_MASK, sampleRate);
}

void MAX86150::setPulseWidth(uint8_t pulseWidth) {
  // pulseWidth: one of MAX86150_PULSEWIDTH_69, _188, _215, _411
  //bitMask(MAX86150_PPGCONFIG1, MAX86150_PULSEWIDTH_MASK, pulseWidth);
}

// NOTE: Amplitude values: 0x00 = 0mA, 0x7F = 25.4mA, 0xFF = 50mA (typical)
// See datasheet, page 21
void MAX86150::setPulseAmplitudeRed(uint8_t amplitude)
{
  writeRegister8(_i2caddr, MAX86150_LED2_PULSEAMP, amplitude);
}

void MAX86150::setPulseAmplitudeIR(uint8_t amplitude)
{
  writeRegister8(_i2caddr, MAX86150_LED1_PULSEAMP, amplitude);
}

void MAX86150::setPulseAmplitudeProximity(uint8_t amplitude) {
  writeRegister8(_i2caddr, MAX86150_LED_PROX_AMP, amplitude);
}

void MAX86150::setProximityThreshold(uint8_t threshMSB)
{
  // The threshMSB signifies only the 8 most significant-bits of the ADC count.
  writeRegister8(_i2caddr, MAX86150_PROXINTTHRESH, threshMSB);
}

//Given a slot number assign a thing to it
//Devices are SLOT_RED_LED or SLOT_RED_PILOT (proximity)
//Assigning a SLOT_RED_LED will pulse LED
//Assigning a SLOT_RED_PILOT will ??
void MAX86150::enableSlot(uint8_t slotNumber, uint8_t device)
{
	  uint8_t originalContents;

	  switch (slotNumber) {
	    case (1):
	      bitMask(MAX86150_FIFOCONTROL1, MAX86150_SLOT1_MASK, device);
	      break;
	    case (2):
	      bitMask(MAX86150_FIFOCONTROL1, MAX86150_SLOT2_MASK, device << 4);
	      break;
	    case (3):
	      bitMask(MAX86150_FIFOCONTROL2, MAX86150_SLOT3_MASK, device);
	      break;
	    case (4):
	      bitMask(MAX86150_FIFOCONTROL2, MAX86150_SLOT4_MASK, device << 4);
	      break;
	    default:
	      //Shouldn't be here!
	      break;
	  }
}

//Clears all slot assignments
void MAX86150::disableSlots(void)
{
  writeRegister8(_i2caddr, MAX86150_FIFOCONTROL1, 0);
  writeRegister8(_i2caddr, MAX86150_FIFOCONTROL2, 0);
}

//
// FIFO Configuration
//

void MAX86150::setFIFOAverage(uint8_t numberOfSamples)
{
  bitMask(MAX86150_FIFOCONFIG, MAX86150_SAMPLEAVG_MASK, numberOfSamples);
}

//Resets all points to start in a known state
void MAX86150::clearFIFO(void) {
  writeRegister8(_i2caddr, MAX86150_FIFOWRITEPTR, 0);
  writeRegister8(_i2caddr, MAX86150_FIFOOVERFLOW, 0);
  writeRegister8(_i2caddr, MAX86150_FIFOREADPTR, 0);
}

//Enable roll over if FIFO over flows
void MAX86150::enableFIFORollover(void) {
  bitMask(MAX86150_FIFOCONFIG, MAX86150_ROLLOVER_MASK, MAX86150_ROLLOVER_ENABLE);
}

//Disable roll over if FIFO over flows
void MAX86150::disableFIFORollover(void) {
  bitMask(MAX86150_FIFOCONFIG, MAX86150_ROLLOVER_MASK, MAX86150_ROLLOVER_DISABLE);
}

//Power on default is 32 samples
//Note it is reverse: 0x00 is 32 samples, 0x0F is 17 samples
void MAX86150::setFIFOAlmostFull(uint8_t numberOfSamples) {
  bitMask(MAX86150_FIFOCONFIG, MAX86150_A_FULL_MASK, numberOfSamples);
}

//Read the FIFO Write Pointer
uint8_t MAX86150::getWritePointer(void) {
  return (readRegister8(_i2caddr, MAX86150_FIFOWRITEPTR));
}

//Read the FIFO Read Pointer
uint8_t MAX86150::getReadPointer(void) {
  return (readRegister8(_i2caddr, MAX86150_FIFOREADPTR));
}

// Set the PROX_INT_THRESHold
void MAX86150::setPROXINTTHRESH(uint8_t val) {
  writeRegister8(_i2caddr, MAX86150_PROXINTTHRESH, val);
}

//
// Device ID and Revision
//
uint8_t MAX86150::readPartID() {
  return readRegister8(_i2caddr, MAX86150_PARTID);
}

//Setup the sensor
//The MAX86150 has many settings. By default we select:
// Sample Average = 4
// Mode = MultiLED
// ADC Range = 16384 (62.5pA per LSB)
// Sample rate = 50
//Use the default setup if you are just getting started with the MAX86150 sensor
void MAX86150::setup(byte powerLevel, byte sampleAverage, byte ledMode, int sampleRate, int pulseWidth, int adcRange)
{
		activeDevices=3;
		writeRegister8(_i2caddr,MAX86150_SYSCONTROL,0x01);
		delay(100);
		writeRegister8(_i2caddr,MAX86150_FIFOCONFIG,0x7F);

		//FIFO Configuration
		//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
		//The chip will average multiple samples of same type together if you wish
		if (sampleAverage == 1) setFIFOAverage(MAX86150_SAMPLEAVG_1); //No averaging per FIFO record
		else if (sampleAverage == 2) setFIFOAverage(MAX86150_SAMPLEAVG_2);
		else if (sampleAverage == 4) setFIFOAverage(MAX86150_SAMPLEAVG_4);
		else if (sampleAverage == 8) setFIFOAverage(MAX86150_SAMPLEAVG_8);
		else if (sampleAverage == 16) setFIFOAverage(MAX86150_SAMPLEAVG_16);
		else if (sampleAverage == 32) setFIFOAverage(MAX86150_SAMPLEAVG_32);
		else setFIFOAverage(MAX86150_SAMPLEAVG_4);

		uint16_t FIFOCode = 0x00;

	  FIFOCode = FIFOCode<<4 | 0x0009;// : FIFOCode;  //insert ECG front of ETI in FIFO
	  FIFOCode = FIFOCode<<8 | 0x0021;//) : FIFOCode; //insert Red(2) and IR (1) in front of ECG in FIFO


		//FIFO Control 1 = FD2|FD1, FIFO Control 2 = FD4|FD3

		writeRegister8(_i2caddr,MAX86150_FIFOCONTROL1,(0b00100001));
		//writeRegister8(_i2caddr,MAX86150_FIFOCONTROL1,(0b00001001));
		writeRegister8(_i2caddr,MAX86150_FIFOCONTROL2,(0b00001001));
		//writeRegister8(_i2caddr,MAX86150_FIFOCONTROL2,(0b00000000));
		//writeRegister8(_i2caddr,MAX86150_FIFOCONTROL1, (char)(FIFOCode & 0x00FF) );
		//writeRegister8(_i2caddr,MAX86150_FIFOCONTROL2, (char)(FIFOCode >>8) );

		writeRegister8(_i2caddr,MAX86150_PPGCONFIG1,0b11010001);
		//writeRegister8(_i2caddr,MAX86150_PPGCONFIG1,0b11100111);

		writeRegister8(_i2caddr,MAX86150_PPGCONFIG2, 0x06);
		writeRegister8(_i2caddr,MAX86150_LED_RANGE, 0x00 ); // PPG_ADC_RGE: 32768nA

		writeRegister8(_i2caddr,MAX86150_SYSCONTROL,0x04);//start FIFO

		writeRegister8(_i2caddr,MAX86150_ECG_CONFIG1,0b00000011);
		writeRegister8(_i2caddr,MAX86150_ECG_CONFIG3,0b00001101);

		setPulseAmplitudeRed(0xFF);
		setPulseAmplitudeIR(0xFF);

  //Multi-LED Mode Configuration, Enable the reading of the three LEDs
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  //enableSlot(1, SLOT_RED_LED);
  //if (ledMode > 1)
	//enableSlot(2, SLOT_IR_LED);
  //if (ledMode > 2)
	//enableSlot(3, SLOT_ECG);
  //enableSlot(1, SLOT_RED_PILOT);
  //enableSlot(2, SLOT_IR_PILOT);
  //enableSlot(3, SLOT_GREEN_PILOT);
  //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

  	clearFIFO(); //Reset the FIFO before we begin checking the sensor
}

//Tell caller how many samples are available
uint8_t MAX86150::available(void)
{
  int8_t numberOfSamples = sense.head - sense.tail;
  if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;

  return (numberOfSamples);
}

//Report the most recent red value
uint32_t MAX86150::getRed(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.red[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the most recent IR value
uint32_t MAX86150::getIR(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.IR[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the most recent Green value
int32_t MAX86150::getECG(void)
{
  //Check the sensor for new data for 250ms
  if(safeCheck(250))
    return (sense.ecg[sense.head]);
  else
    return(0); //Sensor failed to find new data
}

//Report the next Red value in the FIFO
uint32_t MAX86150::getFIFORed(void)
{
  return (sense.red[sense.tail]);
}

//Report the next IR value in the FIFO
uint32_t MAX86150::getFIFOIR(void)
{
  return (sense.IR[sense.tail]);
}

//Report the next Green value in the FIFO
int32_t MAX86150::getFIFOECG(void)
{
  return (sense.ecg[sense.tail]);
}

//Advance the tail
void MAX86150::nextSample(void)
{
  if(available()) //Only advance the tail if new data is available
  {
    sense.tail++;
    sense.tail %= STORAGE_SIZE; //Wrap condition
  }
}

//Polls the sensor for new data
//Call regularly
//If new data is available, it updates the head and tail in the main struct
//Returns number of new samples obtained
uint16_t MAX86150::check(void)
{
  //Read register FIDO_DATA in (3-byte * number of active LED) chunks
  //Until FIFO_RD_PTR = FIFO_WR_PTR

  byte readPointer = getReadPointer();
  byte writePointer = getWritePointer();

  int numberOfSamples = 0;

  //Do we have new data?
  if (readPointer != writePointer)
  {
    //Calculate the number of readings we need to get from sensor
    numberOfSamples = writePointer - readPointer;
    if (numberOfSamples < 0) numberOfSamples += 32; //Wrap condition

    //We now have the number of readings, now calc bytes to read
    //For this example we are just doing Red and IR (3 bytes each)
    int bytesLeftToRead = numberOfSamples * activeDevices * 3;

    //Get ready to read a burst of data from the FIFO register
    _i2cPort->beginTransmission(_i2caddr);
    _i2cPort->write(MAX86150_FIFODATA);
    _i2cPort->endTransmission();

    //We may need to read as many as 288 bytes so we read in blocks no larger than I2C_BUFFER_LENGTH
    //I2C_BUFFER_LENGTH changes based on the platform. 64 bytes for SAMD21, 32 bytes for Uno.
    //Wire.requestFrom() is limited to BUFFER_LENGTH which is 32 on the Uno
    while (bytesLeftToRead > 0)
    {
      int toGet = bytesLeftToRead;
      if (toGet > I2C_BUFFER_LENGTH)
      {
        //If toGet is 32 this is bad because we read 6 bytes (Red+IR * 3 = 6) at a time
        //32 % 6 = 2 left over. We don't want to request 32 bytes, we want to request 30.
        //32 % 9 (Red+IR+GREEN) = 5 left over. We want to request 27.

        toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (activeDevices * 3)); //Trim toGet to be a multiple of the samples we need to read
      }

      bytesLeftToRead -= toGet;

      //Request toGet number of bytes from sensor
      _i2cPort->requestFrom(_i2caddr, toGet);

      while (toGet > 0)
      {
        sense.head++; //Advance the head of the storage struct
        sense.head %= STORAGE_SIZE; //Wrap condition

        byte temp[sizeof(uint32_t)]; //Array of 4 bytes that we will convert into long
        uint32_t tempLong;

        //Burst read three bytes - RED
        temp[3] = 0;
        temp[2] = _i2cPort->read();
        temp[1] = _i2cPort->read();
        temp[0] = _i2cPort->read();

        //Convert array to long
        memcpy(&tempLong, temp, sizeof(tempLong));

				tempLong &= 0x7FFFF; //Zero out all but 18 bits

        sense.red[sense.head] = tempLong; //Store this reading into the sense array

        if (activeDevices > 1)
        {
          //Burst read three more bytes - IR
          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();

          //Convert array to long
          memcpy(&tempLong, temp, sizeof(tempLong));
					//Serial.println(tempLong);
				  tempLong &= 0x7FFFF; //Zero out all but 18 bits

				  sense.IR[sense.head] = tempLong;
        }

        if (activeDevices > 2)
        {
          //Burst read three more bytes - ECG
					int32_t tempLongSigned;

          temp[3] = 0;
          temp[2] = _i2cPort->read();
          temp[1] = _i2cPort->read();
          temp[0] = _i2cPort->read();
					//Serial.println(tempLong);
          //Convert array to long
          memcpy(&tempLongSigned, temp, sizeof(tempLongSigned));

		  		//tempLong &= 0x3FFFF; //Zero out all but 18 bits

          sense.ecg[sense.head] = tempLongSigned;
        }

        toGet -= activeDevices * 3;
      }
    } //End while (bytesLeftToRead > 0)
  } //End readPtr != writePtr
  return (numberOfSamples); //Let the world know how much new data we found
}

//Check for new data but give up after a certain amount of time
//Returns true if new data was found
//Returns false if new data was not found
bool MAX86150::safeCheck(uint8_t maxTimeToCheck)
{
  uint32_t markTime = millis();

  while(1)
  {
	if(millis() - markTime > maxTimeToCheck) return(false);

	if(check() == true) //We found new data!
	  return(true);

	delay(1);
  }
}

//Given a register, read it, mask it, and then set the thing
void MAX86150::bitMask(uint8_t reg, uint8_t mask, uint8_t thing)
{
  // Grab current register context
  uint8_t originalContents = readRegister8(_i2caddr, reg);

  // Zero-out the portions of the register we're interested in
  originalContents = originalContents & mask;

  // Change contents
  writeRegister8(_i2caddr, reg, originalContents | thing);
}

uint8_t MAX86150::readRegister8(uint8_t address, uint8_t reg) {

		uint8_t tempData = 0;
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->endTransmission(false);

  _i2cPort->requestFrom((uint8_t)address, (uint8_t)1); // Request 1 byte
  if (_i2cPort->available())
  {

    return(_i2cPort->read());
  }
  return (0); //Fail
}

void MAX86150::writeRegister8(uint8_t address, uint8_t reg, uint8_t value) {
  _i2cPort->beginTransmission(address);
  _i2cPort->write(reg);
  _i2cPort->write(value);
  _i2cPort->endTransmission();
}
