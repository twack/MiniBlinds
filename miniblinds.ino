//*****************************************************************************
/// Sketch:  mini_blinds.ino
/// Author:  twack@wackware.net
/// Version: 0.1
/// Date:    2013-10-11
/// Purpose: Operate Mini Blinds using Arduino and SmartThings Shield
///
/// Use License: Non-Profit Open Software License version 3.0 (NPOSL-3.0)
///              http://opensource.org/licenses/NPOSL-3.0
///
/// Note: This only works if your miniblinds move easily. The servo I used is
///       fairly torque-y, but could not budge my 6 ft blinds. OK on 4 footer.
///
//*****************************************************************************
///                               Parts List
///
/// Servo: Hitec HS-425BB 4.8 volt (the bigger the better as long as it fits)
///          Red Wire    to shield +5
///          Black Wire  to shield ground
///          Yellow Wire to Shield pin 10 (can use 9, but see servoPin notes)
///
/// Arduino Uno: Off the shelf
///
/// SmartThings Shield: Off the shelf
///
/// Other Parts Needed: Plastic or Metal to Mount Servo to Blind Frame
///                     Cheap Hotel Pen (body is used as a drive shaft)
///                     Hot Glue Gun and Glue
///                     Wires from Shield to Servo
///                     Wall Wort to Power Arduino
///
//*****************************************************************************
///                                Change Log
/// Version:    0.2
///   Date:     20131013
///   Change1:  Added debug print statements to moveblinds function
///
/// Version:    0.1
///   Date:     20131012
///   Change1:  Initial Release
///
//*****************************************************************************
/// Key variables:
///  1. "stepSize" - sets speed that servo moves. Use 1-5, slow to fast.
///  2. "openAngle" - sets angle of servo that equates to blinds being open.
///  3. "closedInsideAngle" - sets angle that equates to closed with the slat
///     edge closest to you pointing down.
///  4. "closedOutsideAngle" - sets angle that equates to closed with the slat
///     edge closest to you pointing up.
///  5. "servoPin" - Pin that drives the servo (9 or 10 must be used to the
///     servo library disables PWM on these pins).
///   
/// Issues:
///  1. Can't use the SmartThings LED as it causes eratic behavior with the 
///    servo. Servo would hop before moving and would drift if issuing repeated
///    moves to the same angle. Probably voltage spikes/drops or PWM.
//*****************************************************************************
#include <SoftwareSerial.h>
#include <SmartThings.h>
#include <Servo.h>  // servo library

#define PIN_THING_RX    3
#define PIN_THING_TX    2

SmartThingsCallout_t messageCallout;    // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  // constructor

bool isDebugEnabled;    // enable or disable debug

int stepSize;
int openAngle;
int closedInsideAngle;
int closedOutsideAngle;
Servo servo1;          // servo control object
int servoPin;

void setup()
{
  // setup default state of global variables
  stepSize = 1;      //sets speed of servo 1-5, 1 = slow (best), 5 = fast

  // these angles depend on your setup, test with servo on benchtop first and
  // understand what will work for you.
  openAngle =           90;  // middle of the servo throw is good for open
  closedInsideAngle =   0;   // might want to decrease the throw when testing
  closedOutsideAngle =  180; // might want to decrease the throw when testing

  servoPin = 10; //must be 9 or 10, see notes above

  isDebugEnabled = true;

  if (isDebugEnabled)
  { // setup debug serial port
    Serial.begin(9600);         // setup serial with a baud rate of 9600
    Serial.println("setup..");  // print out 'setup..' on start
  }

  openBlinds();
}

void loop()
{
  // run smartthing logic
  smartthing.run();
}

void move_blind(int newPosition)
{
  int currentPosition;  //need to read to compare to new position
  int tmpPosition;      //tmp var to use to drive servo in steps

  servo1.attach(servoPin);  //attach to the servo on this pin

  currentPosition = servo1.read(); //need to see which position we're in

  if (isDebugEnabled){
    Serial.print("Current Position: ");
    Serial.print(currentPosition);
    Serial.println("");

    Serial.print("New Position: ");
    Serial.print(newPosition);
    Serial.println("");
  }
  
  if(currentPosition > newPosition) //moving in a negative direction
    for(tmpPosition = currentPosition; tmpPosition >= newPosition; tmpPosition -= stepSize)
    {  
      servo1.write(tmpPosition);
      delay(20);
      if (isDebugEnabled)
        Serial.println(tmpPosition);
    }
  else  //moving in a positive direction
    for(tmpPosition = currentPosition; tmpPosition <= newPosition; tmpPosition += stepSize)
    {  
      servo1.write(tmpPosition);
      delay(20);
      if (isDebugEnabled)
        Serial.println(tmpPosition);      
    }
  servo1.detach(); //disconnect so spurious voltage does not move us
}

void openBlinds()
{
  move_blind(openAngle);

  smartthing.send("openedState:yes"); // send message to cloud
  smartthing.send("closedInsideState:no");
  smartthing.send("closedOutsideState:no");

  if (isDebugEnabled)
    Serial.println("Set to Open Position");
}

void closeOutside()
{
  move_blind(closedOutsideAngle);

  smartthing.send("closedOutsideState:yes");      
  smartthing.send("openedState:no"); // send message to cloud
  smartthing.send("closedInsideState:no");

  if (isDebugEnabled)
    Serial.println("Set to Closed Outside Position");
}

void closeInside()
{
  move_blind(closedInsideAngle);

  smartthing.send("closedInsideState:yes");
  smartthing.send("openedState:no"); // send message to cloud
  smartthing.send("closedOutsideState:no");

  if (isDebugEnabled)
    Serial.println("Set to Closed Inside Position");
}

void messageCallout(String message)
{
  // if debug is enabled print out the received message
  if (isDebugEnabled)
  {
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("' ");
  }

  // execute function based on parsed command
  if (message.equals("openBlinds"))
    openBlinds();
  else if (message.equals("closeOutside"))
    closeOutside();
  else if (message.equals("closeInside"))
    closeInside();
}
