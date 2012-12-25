Spacebrew Data Forwarder
========================

Overview  
--------  
The spacebrew library requires a decent amount of SRAM and sometimes conflicts with libraries required to connect the arduino to select components, such as the Adafruit PN532 NFC/RFID Shield (http://adafruit.com/products/789) and the Sparkfun MPR121 Capacitive Touch Sensor Breakout Board (https://www.sparkfun.com/products/9695).  
  
That's why we've created a two arduino set-up, where one arduino captures data from sensors and components, and then sends the data to another ethernet-enabled arduino, which forwards it to Spacebrew. The two sketches in this folder work together to capture data and forward that data to Spacebrew:  
* The first Arduino is connected to a button. It runs a sketch that sends three serial messages whenever the button is pressed. The first message is a boolen true value; the second is an integer that reflects how many times the button has been pressed, and the last message isa string that holds a short sentence. 
* The second Arduino is connected to Spacebrew via ethernet. It runs a sketch that forwards the messages received via serial to the appropriate Spacebrew publication channels: the first is a boolean channel; the second is a range channel; and the last is a string channel.  
  
Copyright (C) 2012 LAB at Rockwell Group http://lab.rockwellgroup.com  
  
@author      Julio Terra (LAB at Rockwell Group)  
@modified    12/23/2012  
@version     1.0.0  
  
  
Set Up  
-------  
  
###1. Load and Test Sketches
A. Load the spacebrew_forwader sketch onto an ethernet-enabled and ethernet-connected Arduino. Test it out by following these stpes:
* First, make sure that the sketch is able to connect to Spacebrew. By default it is configured to connect to the shared Spacebrew server at ```ec2-184-72-140-184.compute-1.amazonaws.com```. 
	* Check the hosted web-amin to confirm that your Arduino shows up - http://spacebrew.cc/master/spacebrew/admin/admin.html?server=ec2-184-72-140-184.compute-1.amazonaws.com 
	* To change the Spacebrew server then follow the instructions in the Spacebrew_Forwarder Sketch Set-Up below
* Next, test the message forwarding capabilities by sending the sketch a message via serial. Use one of the sample messages from the list below below.  
	* On Spacebrew, data being sent out from each publication channel is visualized by an animation on the round icon that serves as the connection point between publication and subscription channels.
  
Sample Serial Messages:  
```
B1\n
R100\n
Sthis is a test\n   
B1R800\n

Note: When using the sample serial messages leave off the '\n' and set your serial monitor to append NL and CR 
      chars to the end of each message (I think this is the default option on the Arduino IDE serial monitor). 
```

B. Load the data_capture sketch onto an Arduino that is connected to a button, as outlined in the diagram in the image file spacebrew_forwarder_hookup.png. Test out the sketch by following these steps:
* Open a serial monitor to connect to the Arduino running the data capture sketch. Press the button and read the serial messages that are received. They should correspond to the serial messages described below (not inclusive of the commented out section). 
	* If no messages are being received make sure the switch is hooked up properly. 
	* If getting unreadable data make sure serial monitor is set to proper baud rate (57600). 
  
Expected Serial Messages:  
```
Sthat felt good 	// S followed by a static string
B1 					// B followed by 1 or 0
R1 					// R followed by number of times button has been pressed
```

####2. Connecting Arduinos  
Follow the diagram in the image file spacebrew_forwarder_hookup.png to hook-up the Arduino with a button to the one that is connected to Spacebrew via ethernet.  
  

![diagram for hooking-up Arduinos](https://raw.github.com/labatrockwell/spacebrew-arduino-examples/master/spacebrew_forwarder/spacebrew_forwarder_hookup.png)
    
Spacebrew_Forwarder Sketch Set-Up
---------------------------------------------  

### 1. Update Spacebrew Server Host  
This is the only update required to get the example sketch to work. You need to configure the hostname of the computer running the Spacebrew server to which the Arduino will connect (line 113). If you are running the Spacebrew server on your local computer you will need to find your computer's IP address in order to connect to the Spacebrew server from the Arduino ("localhost" will not work).
  
```  
sbConnection.connect("HOSTNAME", sbName, "spacebrew forwarder example");
```   

example:   
```   
sbConnection.connect("10.0.1.11", sbName, "spacebrew forwarder example");
```    

###Advanced Customization Steps
These steps explain how to customize this sketch to change the number, name and type of the Spacebrew publication channels, and the associated mapped serial message ids.

### 2. Update Constants  
The first step is to update the constants that define the number of each type of publication channel
* ```PUB_RNG defines the total number of "range" publication channels.```  
* ```PUB_STR defines the total number of "string" publication channels.```
* ```PUB_BOOL defines the total number of "boolean" publication channels.```
  
The following two constants will only need to be updated occasionally, but you should know their purpose
* ```MSG_LEN defines the max size of the incoming serial message```  
    * Defaults to 20, can be updated to support longer strings 
* ```MAX_NAME_LEN defines the max size of the spacebrew name```  
    * Defaults to 20, can be updated to support longer strings 
* ```VAL_LEN defines the max number of data chars allowed for each range value message.```   
    * Defaults to 4, supports 10-bit range (0 to 1023)  

### 3. Update Spacebrew Name
The second step is to update the Spacebrew application base name, and then update the sbNameLen variable with the length of the Spacebrew application base name. The app will automatically generate a unique 4 digit number that will be appended to the end of the base name.
* ```sbName holds the base name of this spacebrew app.```
* ```sbNameLen holds the length of the spacebrew name.```
      
### 4. Update Spacebrew Pub Arrays
The last step is to set-up the arrays that map the publication channel names to their data type and associate serial message ids. All of these arrays need to be of the same length. 
* ```pubNames holds the name of each spacebrew publication channel.```  
* ```pubIds holds the one-char ID used to parse data received from the serial port.```  
* ```pubTypes holds the data type of each publication channel.```  
  

Serial Message Protocol  
-----------------------  

Here is a description of the serial message format that is supported by this sketch. Note that all serial messages are sent as ASCII encoded text.  

message protocol:
``` 
single payload message: 
1 char serial ID | up to x number of data chars | 1 end char

double payload message: 
1 char serial ID | up to x number of data chars | 1 char serial ID | up to x number of data chars : 1 end char   
```  

The ID for each message is defined in the pubIds array. Serial ids can only include upper case alpha characters. Data chars can vary in length, depending on message type and content. Message size is limited to MAX_NAME_LEN. the data chars can only include lower case alpha and numeric characters. The End char is new line char ('\n'). 


Sample Serial Messages  
----------------------  
  
Here are some sample serial messages that can be used test the spacebrew_forwarder sketch - make sure to leave off the '\n' and set your serial monitor to append NL and CR chars to the end of each message (I think this is the default option on the Arduino IDE serial monitor).  
      
sample messages:  
```
B1\n
R100\n
Sthis is a test\n   
B1R800\n
```

License  
=======  
  
The MIT License (MIT)  
Copyright © 2012 LAB at Rockwell Group, http://www.rockwellgroup.com/lab  
  
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:  
  
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.  
  
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.  
