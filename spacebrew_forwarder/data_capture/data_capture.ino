/*!
 *  Spacebrew Data Forwarder || Data Capture Sketch *
 * 
 *  This sketch was created to work with the spacebrew forwarder example sketch. It sends a range,
 *  a boolean and a string value via Serial to an Arduino that is running the spacebrew_forwarder
 *  sketch. The arduino running the spacebrew_forwarder sketch connects to spacebrew and forwards
 *  the values to spacebrew. 
 *  
 *  The spacebrew_forward sketch must be running on an Ethernet Arduino or an Arduino connected to
 *  an Ethernet shield. When connecting the serial port of both arduinos make sure to connect pin 0 
 *  on one Arduino to pin 1 on the other one.
 *
 *  Check-out the readme for information about license and the spacebrew_forwarder message protocol.
 *
 *  Visit http://docs.spacebrew.cc/ for more info about Spacebrew!
 *
 *  @filename    data_capture.ino  
 *  @author      Julio Terra from LAB at Rockwell Group  
 *  @modified    12/21/2012  
 *  @version     1.0.0  
 *
 */
const int buttonPin = 7;
int buttonLast = LOW;
int buttonState = LOW;
int buttonCount = 0;

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);
}

void loop() {
  int buttonIn = digitalRead(buttonPin);
  if (buttonIn != lastButton){
    if (buttonIn == LOW) {
      buttonState = HIGH;
      buttonCount++;
      sendData();
    }
    else {
      buttonState = LOW;
    }
    lastButton = buttonIn;
  }
}

void sendData() {
  Serial.print("B");
  Serial.println(buttonState);
  Serial.print("R");
  Serial.println(buttonCount);
  Serial.print("S");
  Serial.println(F("that felt good"));
}

