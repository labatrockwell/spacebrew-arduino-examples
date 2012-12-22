/*!
 *  Spacebrew Data Forwarder *
 * 
 *  This sketch was created to forward range, boolean, and string values received via serial to  
 *  Spacebrew. The Arduino running this sketch (the sbArduino) should be connected to the Arduino 
 *  that is capturing data (dataArduino) via the Serial port. Make sure to connect pin 0 on one
 *  Arduino to pin 1 on the other one, and vice versa.
 *
 *  This sketch will register spacebrew publish messages using the data in the pubNames and 
 *  pubTypes array. It will then automatically forward messages to spacebrew publish channels 
 *  by matching the index location of channel names in the pubNames array to those in the 
 *  pubIds array. 
 *
 *  This sketch also creates variations of the app's spacebrew name whenever it connects to spacebrew, 
 *  in order to address a current issue with the connection between the spacebrew server and Arduino
 *  clients.
 * 
 *  Check-out the readme for more information about the message protocol, and the limitations associted
 *  with this sketch.
 *
*/

#include <SPI.h>
#include <Spacebrew.h>
#include <Ethernet.h>
#include <WebSocketClient.h>

#define MSG_LEN       20      // maximum length of serial messages
#define MSG_END       '\n'    // message end character
#define VAL_LEN       4       // maximum length of each int value, used to parse integers between ids in serial msg
#define MAX_NAME_LEN  20      // maximum length of spacebrew activity name

/////////////////////////////////////////////////
// CUSTOMIZABLE CONSTANTS AND VARIABLES - START
// Update the contants and variables in this section to configure the spacebrew names and publication
// channels, and how the serial message ids map to spacebrew publication channels.
//////////////////////////////////////////////////

boolean debug =     true;  // flag that turns on, and off, the debug messages

#define PUB_STR       1       // number of string publication channels
#define PUB_RNG       1       // number of range publication channels
#define PUB_BOOL      1       // number of boolean publication channels
#define PUB_CNT       (PUB_STR + PUB_RNG + PUB_BOOL) // number of publish channels (or outlets)

// mac address of Arduino (just has to be a unique address, not important if it is actual address)
byte PROGMEM mac[] = { 0x90, 0xa2, 0xda, 0x00, 0x68, 0xf7 };  // mac address to be used to get ip address

char sbName [MAX_NAME_LEN + 1] = "check_in_stn_";   // base name of the app
int sbNameLen = 13;                                 // length of base name

// serial to spacebrew message conversion/parsing variables
char pubNames [PUB_CNT][11] =     {"range", "string", "boolean"};     // name of publish channels (outlets) 
char pubIds [PUB_CNT] =     {'R', 'S', 'B'};                          // char ids used in serial message to identify each value
enum SBType pubTypes [PUB_CNT] =  {SB_RANGE, SB_STRING, SB_BOOLEAN};  // type of publish channels (outlets)
int pubData [PUB_CNT] = {0,0,0};                                      // current state, also last published state

//////////////////////////////////////////////////
// CUSTOMIZABLE CONSTANTS AND VARIABLES - END 
//////////////////////////////////////////////////


// create spacebrew connection object, and vars to hold base name, and length of base name
Spacebrew sbConnection;

// serial message handling variables
char serialMsg [MSG_LEN];     // holds the current serial message
int serialMsgLen = 0;         // holds the current lenght of the serial message
boolean msgStarted = false;   // flag that identifies when a new message has started (valid id has been found)
boolean msgComplete = false;  // flag that identifies when a message is ready for processing

int curMsgPos = 0;      // holds the current position of serial message while processing 
char curId = '\0';      // holds the current id of the serial message being red

void setup()   {

  // Adapt the spacebrew app name to include an ID number
  randomSeed(analogRead(0));    // use randomSeed to influence generation of random numbers
  if (sbNameLen >= (MAX_NAME_LEN - VAL_LEN)) sbNameLen = MAX_NAME_LEN - 5;
  for (int i = sbNameLen; i < MAX_NAME_LEN; i++) {    
    if (i < (sbNameLen + 4)) sbName[i] = char(int(random(int('0'),int('9'))));
    else sbName[i] = '\0';
  }

  // connect to serial
  Serial.begin(9600);
  while (!Serial) {}    // for leonardos only
  if (debug) Serial.println(F("serial: connected"));
  resetMsg();

  // connect to internet
  if (Ethernet.begin(mac) == 0) {
    if (debug) Serial.println(F("ERROR: ethernet"));      
  } else {
    if (debug) { Serial.print(F("ethernet: connected\n  - ip: ")); Serial.println(Ethernet.localIP()); }
  }

  //connect to SB callback methods
  sbConnection.onOpen(onOpen);
  sbConnection.onClose(onClose);
  sbConnection.onError(onError);

  //register publishers and subscribers
  for (int i = 0; i < PUB_CNT; i++) sbConnection.addPublish(pubNames[i], pubTypes[i]);

  // connect to spacebrew
  sbConnection.connect("10.0.1.11", sbName, "check in station prototype");    
  if (debug) { Serial.print(F("spacebrew: attempting connection\n  - name: ")); Serial.println(sbName);}

  delay(1000);
  if (debug) Serial.println(F("set-up complete"));
}

void loop() {
  sbConnection.monitor();
  readSerial();
  processSerial();
}


/**
 * readSerial Reads data from the Serial port into the serialMsg array. Also identifies when a 
 *   valid message ID or a message end byte has been received, and updates the appropriate flags
 *   to start processing the data received.
 */
void readSerial() {
  // if serial port has data then process it
  if (Serial.available()) {
    while (Serial.available()){

      // read next character from the serial port
      serialMsg[serialMsgLen] = char(Serial.read());

      // check if current character is an publication ID
      for (int j = 0; j < PUB_CNT; j++) {
        if (serialMsg[serialMsgLen] == pubIds[j]) {
          msgStarted = true;
          if (debug) Serial.println(F("serialMsg started: "));
          // break;
          j = PUB_CNT;
        } 
      }

      // if message has started then process current character
      if (msgStarted) {
        if (debug) { Serial.print(F("reading msg started: "));Serial.println(serialMsgLen); }

        // check if current character is MSG_END char, if so, set msgComplete to true
        if (serialMsg[serialMsgLen] == MSG_END) {
          msgComplete = true;
          if (debug) { Serial.print(F("\nserialMsg completed: ")); Serial.println(serialMsg); }
          break;        
        } 

        // increment the serial length tracking var
        serialMsgLen ++;

        // if the message has exceeded the maximum length then reset it
        if (serialMsgLen >= MSG_LEN) {
          if (debug) { Serial.println(F("\nserialMsg reset")); }
          resetMsg();
        }
      }
    }
  }  
}


/**
 * processSerial Method that processes valid serial messages. Valid serial messages are initially identified
 *   by the presence of a serialId and an end char on the handleSerial method. This method routes further 
 *   processes the message with help from the data-type specific data processing methods.
 */
void processSerial() {
  if (msgStarted && msgComplete) {
    if (debug) { Serial.print(F("processing serial ")); Serial.println(serialMsg); }

    int serialMsgPos = 0;
    char curMsg [MSG_LEN + 1];
    curMsgPos = 0;
    curId = '\0';
    int pubIndex;

    for (bool readingVal = true; (serialMsgPos < serialMsgLen) && readingVal; serialMsgPos++) {

      if (curId == '\0') {
        // check if we have read the entire serial message
        if ( (serialMsgPos > 0) && (serialMsgPos < (serialMsgLen - 1)) ) {
          serialMsgPos--; // move the serialMsgPos back to capture the new message id
        } 

        // loop through each pubId to see if a valid id is available
        for (pubIndex = 0; pubIndex < PUB_CNT; pubIndex++) {
          // if the current char is a message ID then set the curId var
          if (serialMsg[serialMsgPos] == pubIds[pubIndex]) {
            curId = serialMsg[serialMsgPos];
            for (int k = 0; k < MSG_LEN; k++) curMsg[k] = '\0';
            curMsgPos = 0;
            if (debug) { Serial.print(F("id found ")); Serial.println(curId); }
            serialMsgPos++;
            break;
          }
        }
      }

      // if the a message ID has been set then process the message
      if (curId != '\0') {

        // Handle RANGE Messages
        if (pubTypes[pubIndex] == SB_RANGE) {
          readingVal = handleRange(curMsg, serialMsgPos, pubIndex);
        }

        // Handle STRING Messages
        else if (pubTypes[pubIndex] == SB_STRING) {
          readingVal = handleString(curMsg, serialMsgPos, pubIndex);
        }

        // Handle BOOLEAN Messages
        else if (pubTypes[pubIndex] == SB_BOOLEAN) {
          readingVal = handleBoolean(curMsg, serialMsgPos, pubIndex);
        }

      }

      // stop looping through pub names if the serial message has been read
      else {
        break;
      }

    }
    // reset the serial message and associated variables
    resetMsg();
  }
}

/**
 * handleRange Process range serial messages by first making sure that all chars fall within the acceptable
 *   value range, and then adds acceptable values to the curMsg array. The curMsg array is used to 
 *   convert a char array (aka string) to an integer. The conversion takes place when a non-number character 
 *   is received. The latest value is saved in the pubData array.
 * @param  curMsg Pointer to char array that is used to hold the chars that will be converted to an integer
 * @param  serialMsgPos Current position in the curMsg char array
 * @param  pubIndex Index of current spacebrew in the pubNames and pubData arrays
 * @return Returns true if still reading the message, and false if message is completed
 */
boolean handleRange(char * curMsg, int serialMsgPos, int pubIndex) {

  // if new character is a valid char then add it to the curMsg
  if (serialMsg[serialMsgPos] >= '0' && serialMsg[serialMsgPos] <= '9') {
    curMsg[curMsgPos] = serialMsg[serialMsgPos];
    curMsgPos++;
    if (debug) { Serial.print(F("[handleRange] adding to curMsg: ")); Serial.println(curMsg[curMsgPos]); }
    return true;
  }

  // otherwise, stop reading the message and send it via spacebrew
  else {
    if (curMsgPos > 0) {
      if (debug) { Serial.print(F("[handleRange] sending range: ")); Serial.println(curMsg); }
      pubData[pubIndex] = atoi(curMsg);    // convert string to integer
      sbConnection.send(pubNames[pubIndex], pubData[pubIndex]);
      curId = '\0';
    }
    return false;
  }
}


/**
 * handleString Process string serial messages by first making sure that all chars fall within the acceptable
 *   value range, and then adds acceptable values to the curMsg array. The curMsg array is used to hold
 *   the string message that is sent to Spacebrew
 * @param  curMsg Pointer to char array that is used to hold the string that will be sent to Spacebrew
 * @param  serialMsgPos Current position in the curMsg char array
 * @param  pubIndex Index of current spacebrew in the pubNames and pubData arrays
 * @return Returns true if still reading the message, and false if message is completed
 */
boolean handleString(char * curMsg, int serialMsgPos, int pubIndex) {
    if (debug) { Serial.print(F("[handleString] adding to curMsg: ")); Serial.println(int(serialMsg[serialMsgPos])); }
  // if new character is a valid char then add it to the curMsg
  if ( (serialMsg[serialMsgPos] >= '0' && serialMsg[serialMsgPos] <= '9') || 
       (serialMsg[serialMsgPos] >= 'a' && serialMsg[serialMsgPos] <= 'z') || 
       (serialMsg[serialMsgPos] == ',') || (serialMsg[serialMsgPos] == ':') || 
       (serialMsg[serialMsgPos] == ' ') ) 
  {
    if (debug) { Serial.print(F("'"));Serial.print(byte(curMsg[curMsgPos]));Serial.println(F("'")); }
    curMsg[curMsgPos] = serialMsg[serialMsgPos];
    curMsgPos++;
    if (debug) { Serial.print(F("[handleString] adding to curMsg: ")); Serial.println(curMsg[curMsgPos]); }
    return true;
  }

  // otherwise, stop reading the message and send it via spacebrew
  else {
    // if msg message was read, then send it off to spacebrew
    if (curMsgPos > 0) {
      if (debug) { Serial.print(F("[handleString] sending string: ")); Serial.println(curMsg); }
      sbConnection.send(pubNames[pubIndex], curMsg);
      curId = '\0';
    }
    return false;
  }  
}

/**
 * handleBoolean Process boolean serial messages by first making sure that all chars fall within the acceptable
 *   value range, and then adds acceptable values to the curMsg array. The curMsg array is used to 
 *   convert the char array to an integer. If the integer is a 0 then a false message is sent to spacebrew, 
 *   if the integer is 1 then a true message is sent to spacebrew. The latest value is saved in the pubData array.
 * @param  curMsg Pointer to char array that is used to hold the chars that will be converted to an boolean value
 * @param  serialMsgPos Current position in the curMsg char array
 * @param  pubIndex Index of current spacebrew in the pubNames and pubData arrays
 * @return Returns true if still reading the message, and false if message is completed
 */
boolean handleBoolean(char * curMsg, int serialMsgPos, int pubIndex) {
  // if new character is a valid char then add it to the curMsg
  if (serialMsg[serialMsgPos] >= '0' && serialMsg[serialMsgPos] <= '1' && curMsgPos == 0) {
    curMsg[curMsgPos] = serialMsg[serialMsgPos];
    curMsgPos++;
    if (debug) { Serial.print(F("[handleBoolean] adding to curMsg: ")); Serial.println(curMsg[curMsgPos]); }
    return true;
  }

  // otherwise, stop reading the message and send it via spacebrew
  else {
    // if msg message was read, then send it off to spacebrew
    if (curMsgPos > 0) {
      if (debug) { Serial.print(F("[handleBoolean] sending boolean: ")); Serial.println(curMsg); }
      pubData[pubIndex] = atoi(curMsg);    // convert string to integer
      sbConnection.send(pubNames[pubIndex], (pubData[pubIndex] == 0) ? (bool)false : (bool)true );
      curId = '\0';
    }
    return false;
  }  
}

/**
 * resetMsg Method that resets the serialMsg and all associated variables.
 */
void resetMsg() {
  for (int i = 0; i < MSG_LEN; i++) {
    serialMsg[i] = '\0';
  }
  msgStarted = false;
  msgComplete = false;
  serialMsgLen = 0;
  curMsgPos = 0;
}

/**
 * onOpen Callback method that is called when the connection to spacebrew is established
 */
void onOpen(){
  if (debug) Serial.println(F("Connected to SB"));
}

/**
 * onClose Callback method that is called when the connection to spacebrew is closed
 * @param code    
 * @param message 
 */
void onClose(int code, char* message){
  if (debug) Serial.println(F("Disconnected from SB"));
}

/**
 * onError Callback method that is called when the connection to spacebrew encounters an error
 * @param message Error message from spacebrew
 */
void onError(char* message){
  if (debug) Serial.println(F("SB error, msg:"));  
  if (debug) Serial.println(message);  
}

