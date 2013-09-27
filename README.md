SolarPower
==========

Using an Adafruit INA219 breakout board and a TIP120 transistor to monitor and control solar systems.

Voltage and current are logged to an SPI flash chip every 10 seconds.
A button is used in conjunction with a TIP120 transitor to turn on and off high-voltage devices.

I used a Moteino from lowpowerlab.com to create this project! 

Requires the following arduino libraries:

Adafruit_INA219

SPIFlash

SPI

This sketch is based on the Adafruit_INA219 and SPIFlash example sketches.
