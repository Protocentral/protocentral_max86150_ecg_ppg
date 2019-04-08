/*
	ProtoCentral MAX86150 Breakout Board

	https://github.com/protocentral/protocentral_max86150

	Wrriten by: Ashwin Whitchurch (support@protocentral.com)

	Outputs PPG values from the Red LED.

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
	3) In the Arduino IDE, Open Tools->'Serial Plotter'
	4) Make sure the drop down is set to 115200 baud
	5) See your ECG and heartbeat

  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
*/

#include <Wire.h>
#include "max86150.h"

MAX86150 max86150Sensor;

#define debug Serial //Uncomment this line if you're using an Uno or ESP
//#define debug SerialUSB //Uncomment this line if you're using a SAMD21

void setup()
{
    debug.begin(9600);
    debug.println("MAX86150 Basic Readings Example");

    // Initialize sensor
    if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
    {
        debug.println("MAX86150 was not found. Please check wiring/power. ");
        while (1);
    }

    max86150Sensor.setup(); //Configure sensor. Use 6.4mA for LED drive
}

void loop()
{
    if(max86150Sensor.check()>0)
    {
        debug.println(max86150Sensor.getFIFOECG());
    }
    delayMicroseconds(500);
}
