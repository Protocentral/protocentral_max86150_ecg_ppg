/*
  ProtoCentral MAX86150 Breakout Board

  https://github.com/protocentral/protocentral_max86150

  Written by: Ashwin Whitchurch (support@protocentral.com)

  Example 3: Outputs ECG and PPG values

  Use only with the ProtoCentral OpenView application
  
  Place your finger on the two ECG pads to see your ECG waveform in realtime

  Hardware Connections (Breakout
  board to Arduino):
  -5V = 5V
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

  See the output on the Arduino Plotter utlity by:
  1) Program the code to your Arduino
  2) Place your left hand finger and the right hand finger on the two ECG electrode pads
  3) Open the ProtoCentral Openview GUI
  4) See you ECG and PPG on the screen

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "max86150.h"

MAX86150 max86150Sensor;


#define debug Serial      //Uncomment this line if you're using an Uno or ESP
int i, j;
int16_t ecgsigned16,redunsigned16,IRunsinged16;
 

uint8_t data_len=20;
uint8_t DataPacket[100];


void setup()
{
    delay(2000);
    debug.begin(115200);
    debug.println("MAX86150 Basic Readings Example");

    // Initialize sensor
    if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
        debug.println("MAX86150 was not found. Please check wiring/power. ");
        while (1);
    }

    max86150Sensor.setup(); //Configure sensor
}

void loop()
{
    if(max86150Sensor.check()>0)
    {
        ecgsigned16 = (int16_t) (max86150Sensor.getECG());
        redunsigned16 = (int16_t)(max86150Sensor.getRed());
        IRunsinged16 = (int16_t)(max86150Sensor.getIR());
     
        DataPacket[0] = 0x0A;
        DataPacket[1] = 0xFA;
        DataPacket[2] = (uint8_t) (data_len);
        DataPacket[3] = (uint8_t) (data_len>>8);
        DataPacket[4] = 0x02;
        DataPacket[5] = ecgsigned16;
        DataPacket[6] = ecgsigned16>>8;
        DataPacket[7] = 0 ;
        DataPacket[8] = 0;
        DataPacket[9] = redunsigned16;  // spo2 ir
        DataPacket[10] =redunsigned16>>8;
        DataPacket[11] =0;
        DataPacket[12] =0;
        DataPacket[13] =IRunsinged16;  // spo2 ir
        DataPacket[14] =IRunsinged16>>8;
        DataPacket[15] =0;
        DataPacket[16] =0;
        DataPacket[17] = 0;      // temperature
        DataPacket[18] = 0;
        DataPacket[19] = 0; //QRS_Heart_Rate;   // calculated from ads1292r
        DataPacket[20] = 0;
        DataPacket[21] =  0;   // systolic pressure
        DataPacket[22] =  80;     //diastolic pressure
        DataPacket[23] =  0;  // from BP module
        DataPacket[24] =    0;  // leadstatus
        DataPacket[25] = 0x00;
        DataPacket[26] = 0x0b;
    
        for(int i=0; i<27; i++) // transmit the data
        {
          Serial.write(DataPacket[i]);
        }
    } 
}
