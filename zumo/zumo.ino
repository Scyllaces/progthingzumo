#include <ZumoBuzzer.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#include <ZumoReflectanceSensorArray.h>
#include <pnew.cpp>
#include <iterator>
#include <deque>
#include <NewPing.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 10
 
#define LED 13
 
// this might need to be tuned for different lighting conditions, surfaces, etc.
#define QTR_THRESHOLD  70 // microseconds
#define REVERSE_SPEED     100 // 0 is stopped, 400 is full speed
#define TURN_SPEED        100
#define FORWARD_SPEED     50
#define REVERSE_DURATION  400 // ms
#define TURN_DURATION     300 // ms
#define NUM_SENSORS 6
#define THRESHOLD_NEAR_LINE 200
#define THRESHOLD_ON_LINE 200
#define LEFT_IR_SNSR 0
#define RIGHT_IR_SNSR 5

/*A type to show the different states of the zumo*/
typedef enum _ROBOT_STATE
{
  WAIT,
  FORWARD,
  REVERSE,
  LEFT,
  RIGHT,
  SEARCH_ROOM,
  RETURN_TO_BASE
} ROBOT_STATE;

//An data structure for each individual zumo instruction
struct ZumoInstruction {
  _ROBOT_STATE state;
  int attribute;
};

//The collection of zumo instructions
std::deque<ZumoInstruction> zumoPath;
ZumoInstruction lastFeature;
_ROBOT_STATE currentState;
ZumoBuzzer buzzer;
ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON); // pushbutton on pin 12
unsigned int sensor_values[NUM_SENSORS];
ZumoReflectanceSensorArray reflectanceSensors(QTR_NO_EMITTER_PIN);
int incomingByte;      // a variable to read incoming serial data into
bool returnToBase = false;
bool inSearchMode = false;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
int roomNumber = 1;
String lastTurn = "";
unsigned int pingSpeed = 5000; 
unsigned long pingTimer;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  // initialize the LED pin as an output:
  pinMode(LED, OUTPUT);
  pingTimer = millis();

  sendMessage("ZUMO ONLINE");

  currentState = WAIT;
  buzzer.play(">g32>>c32>e32>>d32");
  button.waitForButton();
  delay(2000);

  calibrateSensor();

  buzzer.play(">a32>>c32>g32>>d32");
  button.waitForButton();
  delay(2000);
}

void loop() {
  //check for input from serial
  readFromSerialAndSetState();
 
  //perform current state
  performState();
  
  //reset variable, globals as needed
  if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  }

}

void echoCheck() 
{
  if (sonar.check_timer()) {
    if (inSearchMode)
    {
      sendMessage("Object was found in room " + roomNumber - 1);
    }
    else
    {
      sendMessage("Object was found in corridor" );
    }
  }
}

void performState()
{
  switch (currentState)
  {
    case FORWARD:
      driveForward();
      checkIfOnWall();
      if(!returnToBase || !inSearchMode)
      {
        AddInstructionToPath(FORWARD, 0);
      }
      break;

    case LEFT:
      turnZumoLeft();
      lastTurn = "left";
      if(!returnToBase || !inSearchMode)
      {
        AddInstructionToPath(LEFT, 0);
        currentState = WAIT;
      }
      break;

    case RIGHT:
      turnZumoRight();
      lastTurn = "right";
      if(!returnToBase || !inSearchMode)
      {
        AddInstructionToPath(RIGHT, 0);
        currentState = WAIT;
      }      
      break;

    case REVERSE:
      reverseZumo();
      if(!returnToBase || !inSearchMode)
      {
        AddInstructionToPath(REVERSE, 0);
      }
      break;
      
    case SEARCH_ROOM:  
      if(!inSearchMode)
      {
        searchRoomRoutine();
      }   
      currentState = WAIT;
      break;
       
    case RETURN_TO_BASE:
      returnToBase = true;
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
      delay(4000);
      executeZumoReturn();
      
      break;
      
    case WAIT:
      forceStop();
      break;
  }
}

void searchRoomRoutine()
{
  String roomNumberString = String(roomNumber);
  roomNumber++;
  sendMessage("Zumo has entered room " + roomNumberString + " on the " + lastTurn + " side");
}

void forceStop()
{
  motors.setSpeeds(0, 0);
}

void executeZumoReturn()
{
  int pathSize = zumoPath.size();
  int i;
  for (i = 0; i < pathSize; i++)
  {
    currentState = zumoPath[i].state;
    performState();
  }
  currentState = WAIT;
}

void AddInstructionToPath(_ROBOT_STATE state, int attribute)
{
  ZumoInstruction instruction = {state, attribute};
  zumoPath.push_front(instruction);
}

void calibrateSensor()
{
  digitalWrite(13, HIGH);
  //Calibrate zumo sensor array
  unsigned long startTime = millis();
  while(millis() - startTime < 10000)
  {
    reflectanceSensors.calibrate();
  }
  digitalWrite(13, LOW);
}

void readFromSerialAndSetState() 
{
  // see if there's incoming serial data:
  if (Serial.available() > 0) {
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    if (incomingByte == ' ') {
      currentState = WAIT;
    }
    if (incomingByte == 'w') {
      currentState = FORWARD;
    }
    if (incomingByte == 's') {
      currentState = REVERSE;
    }
    if (incomingByte == 'a') {
      currentState = LEFT;
    }
    if (incomingByte == 'd') {
      currentState = RIGHT;
    }
    if (incomingByte == 'c') {
      currentState = FORWARD;
      inSearchMode = false;
    }
    if (incomingByte == 'r') {
      currentState = SEARCH_ROOM;
    }
    if (incomingByte == 'e') {
      currentState = RETURN_TO_BASE;
    }
  }
}

void sendMessage(String message)
{
  Serial.print(message);
  Serial.print("\n");
}

void driveForward()
{
  reflectanceSensors.read(sensor_values);
  
  if (sensor_values[0] < QTR_THRESHOLD)
  {
    // if leftmost sensor detects line, reverse and turn to the right
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (sensor_values[5] < QTR_THRESHOLD)
  {
    // if rightmost sensor detects line, reverse and turn to the left
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // otherwise, go straight
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(0, 0);
  }
}

void reverseZumo()
{
    motors.setSpeeds(-FORWARD_SPEED, -FORWARD_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(0, 0);
}

void checkIfOnWall()
{
  reflectanceSensors.read(sensor_values);
  if ((sensor_values[LEFT_IR_SNSR] >= THRESHOLD_ON_LINE) &&
               (sensor_values[RIGHT_IR_SNSR] >= THRESHOLD_ON_LINE))
   {
      sendMessage("Zumo has reached a wall");
      motors.setSpeeds(-FORWARD_SPEED, -FORWARD_SPEED);
      delay(REVERSE_DURATION);
      motors.setSpeeds(0, 0);
      currentState = WAIT;
   }
}

void turnZumoLeft()
{
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(0, 0);
}

void turnZumoRight()
{
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(0, 0);
}

