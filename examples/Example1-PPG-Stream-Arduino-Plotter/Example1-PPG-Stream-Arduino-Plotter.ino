/*
  ProtoCentral MAX86150 Breakout Board

  https://github.com/protocentral/protocentral_max86150

  Wrriten by: Ashwin Whitchurch (support@protocentral.com)

  Outputs PPG values from the Red LED.

  Place your finger on the window of the sensor to see your PPG and heartbeat.

  Hardware Connections (Breakout
  board to Arduino):
  -5V = 5V
  -GND = GND
  -SDA = A4 (or SDA)
  -SCL = A5 (or SCL)
  -INT = Not connected

	See the output on the Arduino Plotter utlity by:
	1) Program the code to your Arduino
	2) Place your finger on the sensor
	3) In the Arduino IDE, Open Tools->'Serial Plotter'
	4) Make sure the drop down is set to 115200 baud
	5) See the PPG waveform

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "max86150.h"

MAX86150 max86150Sensor;

uint16_t ppgunsigned16;

void setup()
{
    Serial.begin(57600);
    Serial.println("MAX86150 PPG Streaming Example");

    // Initialize sensor
    if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
        Serial.println("MAX86150 was not found. Please check wiring/power. ");
        while (1);
    }

  	Serial.println(max86150Sensor.readPartID());

    max86150Sensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}

void loop()
{
    if(max86150Sensor.check()>0)
    {
				ppgunsigned16 = (uint16_t) (max86150Sensor.getFIFORed()>>2);
				Serial.println(ppgunsigned16);
    }
}
