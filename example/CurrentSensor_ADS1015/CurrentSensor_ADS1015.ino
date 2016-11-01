/*
current_sense_bridge example code

This code demonstrates how to read data from ADS1015 ADC on the current_sense_bridge board.

You need to install ADS1X15 Arduino library written by Adafruit Industries
https://github.com/adafruit/Adafruit_ADS1X15
Significant portion of this code was taken from the example included in the library.

This example also utilizes timer library by Simon Monk and et al.
https://github.com/JChristensen/Timer

Author: Shunya Sato

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/



#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "Timer.h"

Timer t;
//Adafruit_ADS1115 ads;
Adafruit_ADS1015 ads;

// const float multiplier = 3.0F; /* ADS1015  @ 2/3x gain +/- 6.144V (12-bit results) */
const float multiplier = 2.0F; /* ADS1015  @ 1x gain   +/- 4.096V (12-bit results) */
// const float multiplier = 1.0F; /* ADS1015  @ 2x gain   +/- 2.048V (12-bit results) */
// const float multiplier = 0.5F; /* ADS1015  @ 4x gain   +/- 1.024V (12-bit results) */
// const float multiplier = 0.25F; /* ADS1015  @ 8x gain   +/- 0.512V (12-bit results) */
// const float multiplier = 0.125F; /* ADS1015  @ 16x gain  +/- 0.256V (12-bit results) */


void setup(){
  Serial.begin(115200);          //  setup serial
  //Serial.println("Getting single-ended readings from AIN0..3");
  //Serial.println("ADC Range: +/- 6.144V (1 bit = 3mV/ADS1015, 0.1875mV/ADS1115)");

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!

  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  ads.begin();
  //ads.begin(4,5);
  /* Use this to set data rate for the 16-bit version (optional)*/
  //ads.setSPS(ADS1115_DR_860SPS);                      // for ADS1115 fastest samples per second is 860 (default is 128)

  t.every(100, readADC);
}

void loop(){
  t.update();
}

void readADC(){
  int16_t adc0;
  int16_t adc1;

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);

//  Serial.print(millis());
//  Serial.print(", ");
//  Serial.print(adc0);
//  Serial.print(", ");
//  Serial.print(adc0 * multiplier);
//  Serial.print(", ");
//  Serial.print(adc1);
//  Serial.print(", ");
  Serial.println(adc1 * multiplier);
  //delay(100);
}
