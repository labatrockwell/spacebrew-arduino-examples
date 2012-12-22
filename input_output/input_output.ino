/*
Developed by the LAB at Rockwell Group

See "basic protoshield setup.PNG" in the examples directory
to see how this example is expecting the Arduino to be wired up.
I have the Ethernet shield sitting on top of the arduino, and then the
Sparkfun ProtoShield on top of that.

You must remember to include the Ethernet library, SPI library, 
and the WebSocketClient library 
(accessible here: https://github.com/labatrockwell/ArduinoWebsocketClient)
in order to use the Spacebrew library.

visit http://docs.spacebrew.cc/ for more info about Spacebrew!

putting the "toty" back in "Prototyping"
*/


#include <SPI.h>
#include <Spacebrew.h>
#include <Ethernet.h>
#include <WebSocketClient.h>

uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
Spacebrew spacebrewConnection;
int lastAnalog = -1;
int lastButton = LOW;

const int buttonPin = 7;
const int digitalLedPin = 8;
const int analogLedPin = 9;
const int analogPin = 0;

void setup() {
  //connect to spacebrew library info
  spacebrewConnection.onOpen(onOpen);
  spacebrewConnection.onClose(onClose);
  spacebrewConnection.onError(onError);
  
  //connect to message callbacks
  spacebrewConnection.onBooleanMessage(onBooleanMessage);
  spacebrewConnection.onStringMessage(onStringMessage);
  spacebrewConnection.onRangeMessage(onRangeMessage);
  
  //register publishers and subscribers
  spacebrewConnection.addPublish("Analog", SB_RANGE);
  spacebrewConnection.addPublish("Button", SB_BOOLEAN);
  spacebrewConnection.addPublish("Parrot", SB_STRING);
  spacebrewConnection.addSubscribe("Blink LED", SB_BOOLEAN);
  spacebrewConnection.addSubscribe("Fade LED", SB_RANGE);
  spacebrewConnection.addSubscribe("Parrot", SB_STRING);
  
  //connect to the spacebrew server
  Ethernet.begin(mac);
  spacebrewConnection.connect("lab-macbookpro-02.rockwellgroup.com", "Rename_Me", "Arduino Input and Output Test");
  
  pinMode(digitalLedPin, OUTPUT);
  pinMode(analogLedPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop() {
  //let the spacebrew library check for any incoming messages
  //and re-connect to the server if necessary
  spacebrewConnection.monitor();
  
  int analogIn = analogRead(analogPin);
  if (analogIn != lastAnalog){
    //send a range via spacebrew
    spacebrewConnection.send("Analog", analogIn);
    lastAnalog = analogIn;
  }
  
  int buttonIn = digitalRead(buttonPin);
  if (buttonIn != lastButton){
    //send the button state via spacebrew
    spacebrewConnection.send("Button", buttonIn == LOW);
    lastButton = buttonIn;
  }
}

void onBooleanMessage(char *name, bool value){
  //turn the 'digital' LED on and off based on the incoming boolean
  digitalWrite(digitalLedPin, value ? HIGH : LOW);
}

void onStringMessage(char *name, char* message){
  //repeat back whatever was sent
  spacebrewConnection.send("Parrot", message);
}

void onRangeMessage(char *name, int value){
  //use the range input to control the brightness of the 'analog' LED
  analogWrite(analogLedPin, map(value, 0, 1024, 0, 255));
}

void onOpen(){
  //send a message when we get connected!
  spacebrewConnection.send("Parrot", "Hello Spacebrew");
}

void onClose(int code, char* message){
  //turn everything off if we get disconnected
  analogWrite(analogLedPin, 0);
  digitalWrite(digitalLedPin, LOW);
}

void onError(char* message){}
