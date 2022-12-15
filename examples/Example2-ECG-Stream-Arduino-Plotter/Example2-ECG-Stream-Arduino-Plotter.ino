/*
	ProtoCentral MAX86150 Breakout Board

	https://github.com/protocentral/protocentral_max86150

	Written by: Ashwin Whitchurch (support@protocentral.com)

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
#include <FIR.h>

MAX86150 max86150Sensor;

int16_t ecgsigned16;
int16_t redunsigned16;

FIR<long, 13> fir;

/*

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 200 Hz

fixed point precision: 16 bits

* 0 Hz - 1 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

* 2 Hz - 25 Hz
  gain = 1
  desired ripple = 10 dB
  actual ripple = n/a

* 26 Hz - 100 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = n/a

*/
long coef[13] = {
	-364,
	-103,
	-42,
	60,
	173,
	262,
	295,
	262,
	173,
	60,
	-42,
	-103,
	-364};

void setup()
{
	Serial.begin(57600);
	Serial.println("MAX86150 Basic Reading Example");

	// Set the coefficients
  fir.setFilterCoeffs(coef);

	// Initialize sensor
	if (max86150Sensor.begin(Wire, I2C_SPEED_FAST) == false)
	{
		Serial.println("MAX86150 was not found. Please check wiring/power. ");
		while (1)
			;
	}

	max86150Sensor.setup(); // Configure sensor
}

void loop()
{
	if (max86150Sensor.check() > 0)
	{
		ecgsigned16 = (int16_t)(max86150Sensor.getECG() >> 2);
		Serial.println(fir.processReading(ecgsigned16));
	}
}
