Spacebrew Arduino Examples  
==========================  
  
This is a collection of sketches that use the Spacebrew library for Arduino. A network connection is required for these sketches to connect to Spacebrew. Therefore, these sketches and the Spacebrew library can only run on Ethernet Arduinos and Arduinos with an Ethernet shield.   
  
Get the collection by clicking the DOWNLOADS button in the top right corner. Then copy the sketches into your Arduino sketches folder. You also need to make sure that you have the following two Arduino libraries:  
* Spacebrew library for Arduino: download from github at https://github.com/labatrockwell/spacebrew-arduino-library  
* WebSocketClient library for Arduino: donwload from github at https://github.com/labatrockwell/ArduinoWebsocketClient  
   
Button  
------  
Sketch that connects an Arduino, with button and an led, to Spacebrew. A publication channel sends out boolean values that represent the switch's state, while the boolean values received via the subscription channel is used to turn on and off the led.  
  
Input_Output  
------------  
Sketch that connects an Arduino, with a button, an analog input (such as a pot or photoresistor), and two leds, to Spacebrew. This sketch features three publication and three subscription channels.  
* The publication channels include a boolean channel for the button state, a range channel for the analog input state, and a string that parrots back string messages received from Spacebrew.  
* The subscription channels include a boolean channel to blink one of the leds, a range channel to set the brightness of the second led, and a string channel that parrots back messages to Spacebrew.  
  
Spacebrew Forwarder  
-------------------  
The spacebrew library requires a decent amount of SRAM and sometimes conflicts with the libraries required to connect the arduino to select components, such as the Adafruit PN532 NFC/RFID Shield (http://adafruit.com/products/789) and the Sparkfun MPR121 Capacitive Touch Sensor Breakout Board (https://www.sparkfun.com/products/9695).  
  
That's why we've created a two arduino set-up, where one arduino captures data from sensors and components, and then sends the data via serial to another ethernet-enabled arduino, which forwards it to Spacebrew.  Check-out the readme file in the spacebrew_forwader directory for more details about these example.  
  
License  
=======  
  
The MIT License (MIT)  
Copyright © 2012 LAB at Rockwell Group, http://www.rockwellgroup.com/lab  
  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  