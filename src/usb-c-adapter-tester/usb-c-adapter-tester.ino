/**
 * This file is part of USB-C-Adapter-Tester.
 * 
 * Copyright 2016 Frank Duerr
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define PIN_LED_OK 0
#define PIN_LED_NOTOK 1

#define PIN_CC1 A0
#define PIN_CC2 A1

// Resistance of shunt resistor for measuring current [Ohm]
const float R_shunt = 5100.0f;

// Reference voltage of ADC [V]
const float V_ref = 2.5f;

// According to the USB-C standard, "Default USB power" is signaled by 
// current I = 80 uA +- 20 % on CC line. This current range leads to the 
// following allowed voltage range measured along the shunt.
const float V_min = R_shunt*80e-6f * 0.8f;
const float V_max = R_shunt*80e-6f * 1.2f;

// ADC counts of 10 bit ADC corresponding to V_min (low threshold) 
// and V_max (high threshold) 
const uint16_t Low_threshold = (uint16_t) (((V_min/V_ref) * 1023.0f) + 0.5f);
const uint16_t High_threshold = (uint16_t) (((V_max/V_ref) * 1023.0f) + 0.5f);

void setup() 
{
    // Configure LED pins as output
    pinMode(PIN_LED_OK, OUTPUT);
    pinMode(PIN_LED_NOTOK, OUTPUT);
    digitalWrite(PIN_LED_OK, LOW);
    digitalWrite(PIN_LED_NOTOK, LOW);

    // Configure ADC.    
    // Use external voltage reference (2.5 V) for ADC.
    analogReference(EXTERNAL);
    // Perform dummy ADC reading for the following reason:
    // "The first ADC conversion result after switching reference 
    // voltage source may be inaccurate, and the user is advised 
    // to discard this result." [ATmega 48/88/168/326 datasheet]
    uint16_t dummy = analogRead(PIN_CC1);

    // To be really save, give some extra time to stabilize.
    delay(500);
}

void loop() 
{
    // Sample both CC pins
    uint16_t sampleCC1 = analogRead(PIN_CC1);
    uint16_t sampleCC2 = analogRead(PIN_CC2);
    
    // Only one CC pin is connected to host; the other is pulled low.
    // -> use the sample with higher value
    uint16_t sampleCC = sampleCC1 > sampleCC2 ? sampleCC1 : sampleCC2;

    if (sampleCC >= Low_threshold && sampleCC <= High_threshold) {
        digitalWrite(PIN_LED_OK, HIGH);
        digitalWrite(PIN_LED_NOTOK, LOW);
    } else {
        digitalWrite(PIN_LED_OK, LOW);
        digitalWrite(PIN_LED_NOTOK, HIGH);
    }
    
    // About 10 samples per second is more than enough.
    delay(100);
}
