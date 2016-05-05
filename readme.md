USB-C-Adapter-Tester is a device to test USB-C-to-USB-A/Micro-USB cables and adapters for conformity with the USB standard. Such USB-C adapters should use a 56k resistor connecting the 5 V line to the CC (Channel Configuration) line to signal to the downstream device (e.g., smartphone) that it should not draw more than standard USB power. However, some cables and adapters use smaller resistor values resulting in excessive current drawn by the downstream USB-C device, which might damage the upstream USB device providing power.

USB-C-Adapter-Tester consists of a hardware and a software part:

* circuit board (PCB) for connecting the USB-C cable/adapter under test 
* software based on the Arduino platform

Eagle files and PDFs for the hardware (schematics, board layout) can be found in folder `pcb`. The software can be found in folder `src`.

# Hardware

The tester hardware as depicted in the figures below consists of a microcontroller (Atmega 328p; same chip as the Arduino UNO) featuring an Analog-to-Digital Converter (ADC). The ADC measures the voltage drop along a 5.1 k resistor (actually, two separate 5.1k resistors on different channels of the ADC since USB-C features two CC lines so you can plug-in the USB-C cable either way). Knowing the resistance and the voltage drop measured by the ADC, the microcontroller calculates the current (ICC) flowing on the CC line. If ICC is within the specified range (80 uA +- 20 % according to the standard), an LED signaling a "good" cable is turned on from an GPIO pin. If it is outside the range, another LED signaling a "bad" cable is turned on.

![USB-C Tester Device](/img/usb-c-adapter-tester-1.jpg)
![USB-C Tester Device](/img/usb-c-adapter-tester-2.jpg)

The cable to be checked is connected to an USB host or charger to power the microcontroller. The good old Atmega 328p can be powered from 5V, which is the voltage provided by standard USB-A and Micro-USB. 

Since the internal voltage reference of the Atmega might not be very precise, an external 2.5 V voltage reference diode is used as reference for the ADC. 

As soon as the cable under test is plugged in, the microcontroller starts measuring the voltage drop along the 5.1k resistor using the ADC, translates the voltage to current (R = V/I), compares the measured current to the allowed current range (80 uA +- 20 %), and switches on one the corresponding "good" or "bad cable" LED.

# Programming the Microcontroller

The code is implemented for the Arduino platform. 

The device is programmed through an in-system programmer port (6 pin ISP) with the following standard layout:

    MISO <-- 1 2 --> VCC
     SCK <-- 3 4 --> MOSI
     RST <-- 5 6 --> GND

To prepare the Atmega, program the following fuses (note that "0" means 
that the fuse is programmed):

* CKSEL = 0010: use internal 8 MHz RC oscillator
* CKDIV8 = 1: 8 MHz system clock (do not divide the 8 MHz internal clock by 8)
* SPIEN = 0: enable serial programming
* BODLEVEL = 101: brown-out detection set to 2.7 V
* SUT = 00: lowest possible startup time (6 CK from power-down; since we use
  BOD, we do not need additional waiting time for the power source to come up)
    
Using avrdude, the command looks like this (you might need
to execute this command as root):

    $ avrdude -c usbasp -p m328p -U lfuse:w:0xc2:m -U hfuse:w:0xd9:m -U efuse:w:0x05:m

Make sure to compile your code for the correct board. In the repository, 
you will find a suitable board definition for the Arduino IDE (see folder 
`board_definition`). Copy the directory `atmega328_8MHz` into the folder 
`hardware` in your Arduino sketchbook. Then you should find and select the 
board  called `ATmega328P @ 8 MHz` under the menu item `Tools/Board`. 

Compiling generates a hex file that you need to program the Atmega. This hex 
file is a little hidden in the temporary build directory of the Arduino
IDE. If you use Linux  and Arduino IDE 1.6, have a look at the `/tmp` 
directory. After hitting the  compile button in the Arduino IDE, search for the
latest hex file called `usb-c-adapter-tester.cpp.hex` in a temporary directory named `/tmp/build...`. If you have found the hex file, you can flash it using avrdude (again, you might need to be root):

    $ avrdude -p m328p -c usbasp -v -U flash:w:/tmp/build6535101126624677692.tmp/usb-c-adapter-tester.cpp.hex
