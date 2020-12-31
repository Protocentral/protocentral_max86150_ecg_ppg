/*
  ProtoCentral MAX86150 Breakout Board

  https://github.com/protocentral/protocentral_max86150

  Wrriten by: Ashwin Whitchurch (support@protocentral.com)

  Plots ppg and ECG signals through protocentral openview GUI
  GUI URL: https://github.com/Protocentral/protocentral_openview.git

  Place your finger on the window of the sensor to see your PPG and heartbeat.

  Hardware Connections (Breakout
  board to Arduino):
  -5V = 5V
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "max86150.h"

#define CES_CMDIF_PKT_START_1   0x0A
#define CES_CMDIF_PKT_START_2   0xFA
#define CES_CMDIF_TYPE_DATA     0x02
#define CES_CMDIF_PKT_STOP      0x0B
#define DATA_LEN                6
#define ZERO                    0

volatile char DataPacket[16];
const char DataPacketFooter[2] = {ZERO, CES_CMDIF_PKT_STOP};
const char DataPacketHeader[5] = {CES_CMDIF_PKT_START_1, CES_CMDIF_PKT_START_2, DATA_LEN, ZERO, CES_CMDIF_TYPE_DATA};

MAX86150 max86150Sensor;

#define debug Serial //Uncomment this line if you're using an Uno or ESP
//#define debug SerialUSB //Uncomment this line if you're using a SAMD21

uint16_t irunsigned16;
uint16_t redunsigned16;
int16_t  ecgsigned16;


void sendDataThroughUart(){

  DataPacket[0] = ecgsigned16;
  DataPacket[1] = ecgsigned16 >> 8;

  DataPacket[2] = irunsigned16;
  DataPacket[3] = irunsigned16 >> 8;

  DataPacket[4] = redunsigned16;
  DataPacket[5] = redunsigned16 >> 8;

  //send packet header
  for(int i=0; i<5; i++){

    Serial.write(DataPacketHeader[i]);
  }

  //send sensor data
  for(int i=0; i<DATA_LEN; i++){

    Serial.write(DataPacket[i]);
  }

  //send packet footer
  for(int i=0; i<2; i++){

    Serial.write(DataPacketFooter[i]);
  }
}

void setup()
{
    debug.begin(57600);
    debug.println("MAX86150 PPG Streaming Example");

    // Initialize sensor
    if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
        debug.println("MAX86150 was not found. Please check wiring/power. ");
        while (1);
    }

  	debug.println(max86150Sensor.readPartID());

    max86150Sensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}

void loop()
{
    if(max86150Sensor.check()>0)
    {
				irunsigned16  = (uint16_t) (max86150Sensor.getFIFOIR()>>2);
				redunsigned16 = (uint16_t) (max86150Sensor.getFIFORed()>>2);
        ecgsigned16   = (int16_t)  (max86150Sensor.getFIFOECG()>>2);

        sendDataThroughUart();
    }
}
