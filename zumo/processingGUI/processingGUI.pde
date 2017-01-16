import processing.serial.*;
import controlP5.*;

ControlP5 cp5;
Serial myPort;  // Create object from Serial class
int val;        // Data received from the serial port
PFont myFont; 
String messages = "";

void setup() 
{
  size(800, 800);
  background(255);
  cp5 = new ControlP5(this);

  String portName = "COM4";
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n');
  myFont = loadFont("Verdana-48.vlw");
  
  cp5.addButton("Forward")
     .setBroadcast(false)
     .setValue(0)
     .setPosition(150,50)
     .setSize(100,100)
     .setBroadcast(true)
     ;
     
   cp5.addButton("Backwards")
   .setBroadcast(false)
   .setValue(0)
   .setPosition(150,150)
   .setSize(100,100)
   .setBroadcast(true)
   ;
   
   cp5.addButton("Left")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(50,150)
     .setSize(100,100)
     .setBroadcast(true)
     ;
   
   cp5.addButton("Right")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(250,150)
     .setSize(100,100)
     .setBroadcast(true)
     ;
     
   cp5.addButton("Stop")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(50,300)
     .setSize(300,100)
     .setBroadcast(true)
     ;
     
   cp5.addButton("Complete")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(50,450)
     .setSize(100,100)
     .setBroadcast(true)
     ;
     
   cp5.addButton("Search Room")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(150,550)
     .setSize(100,100)
     .setBroadcast(true)
     ;
     
   cp5.addButton("Return to Base")
   .setBroadcast(false)
     .setValue(0)
     .setPosition(250,650)
     .setSize(100,100)
     .setBroadcast(true)
     ;
}

void draw() {
  background(255);
  fill(0);
  text(messages, 500, 100);
}

public void Forward(int theValue) {
  myPort.write("w");
}

public void Backwards(int theValue) {
  myPort.write("s");
}

public void Left(int theValue) {
  myPort.write("l");
}

public void Right(int theValue) {
  myPort.write("r");
}

public void Stop(int theValue) {
  myPort.write(" ");
}

public void Complete(int theValue) {
  myPort.write("c");
}

public void Search_Room(int theValue) {
  myPort.write("r");
}

public void Return_To_Base(int theValue) {
  myPort.write("e");
}

void serialEvent(Serial myPort) {
  // read the serial buffer:
  String myString = myPort.readStringUntil('\n');
  if (myString != null) {
    messages = messages + myString + "\n";
  }
}


