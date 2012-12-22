Spacebrew Data Forwarder
========================

Overview  
--------  
  
This sketch was created to forward range, boolean, and string values received via serial to Spacebrew. The Arduino running this sketch (the sbArduino) should be connected to the Arduino that is capturing data (dataArduino) via the Serial port. Make sure to connect pin 0 on one Arduino to pin 1 on the other one, and vice versa.  
  
This sketch will register spacebrew publish messages using the data in the pubNames and pubTypes array. It will then automatically forward messages to spacebrew publish channels by matching the index location of channel names in the pubNames array to those in the pubIds array.   
  
This sketch also creates variations of the app's spacebrew name whenever it connects to spacebrew, in order to address a current issue with the connection between the spacebrew server and Arduino clients.  
  
Copyright (C) 2012 LAB at Rockwell Group http://lab.rockwellgroup.com  
  
@filename    spacebrew_data_forwarder.ino  
@author      Julio Terra (LAB at Rockwell Group)  
@modified    12/17/2012  
@version     0.0.1  
  
Serial Message Protocol  
-----------------------  

Here is a description of the serial message format that is supported by this sketch. Note that all serial messages are sent as ASCII encoded text.  

``` 
single payload message: 
1 char serial ID | up to x number of data chars | 1 end char

double payload message: 
1 char serial ID | up to x number of data chars | 1 char serial ID | up to x number of data chars : 1 end char   
```

The ID for each message is defined in the pubIds array. Serial ids can only include upper case alpha characters. Data chars can vary in length, depending on message type and content. Message size is limited to MAX_NAME_LEN. the data chars can only include lower case alpha and numeric characters. The End char is new line char ('\n').


How to set-up the sketch
------------------------  
### 1. Update Constants  
The first step is to update the constants that define the number of each type of publication channel
* ```PUB_RNG defines the total number of "range" publication channels.```  
* ```PUB_STR defines the total number of "string" publication channels.```
* ```PUB_BOOL defines the total number of "boolean" publication channels.```
  
The following two constants will only need to be updated occasionally, but you should know their purpose
* ```MSG_LEN defines the max size of the incoming serial message```  
    * Defaults to 20, can be updated to support longer strings though you will run into SRAM issues if you increase too much  
* ```VAL_LEN defines the max number of data chars allowed for each range value message.```   
    * Defaults to 4, supports 10-bit range (0 to 1023)  
  
### 2. Update Spacebrew Name
The second step is to update the Spacebrew application base name, and then update the sbNameLen variable with the length of the Spacebrew application base name. The app will automatically generate a unique 4 digit number that will be appended to the end of the base name.
* ```sbName holds the base name of this spacebrew app.```
* ```sbNameLen holds the length of the spacebrew name.```
      
### 3. Update Spacebrew Pub Arrays
The last step is to set-up the arrays that map the publication channel names to their data type and associate serial message ids. All of these arrays need to be of the same length.
* ```pubNames holds the name of each spacebrew publication channel.```  
* ```pubIds holds the one-char ID used to parse data received from the serial port.```  
* ```pubTypes holds the data type of each publication channel.```  
      
