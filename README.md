# progthingzumo
First Assignment

# README

## Task List

### Task 1  
Complete  

### Task 2  
I had issues gets the sensors to reliably find the lines.

### Task 3 
Complete

### Task 4
This task is about half complete. The Zumo know its about to search a room and can find objects in the room. 
It does not remeber where they are however.

### Task 5
The Zumo can detect object in rooms and corridors, and knows the difference betweeen them.
The Zumo does not make note of where the corridor objects are found.

### Task 6
This task partially works. A route* is built up as the Zumo goes through the maze.
The bad line detection combined with simple code controlling how the zumo turns makes 
competing this task unreliable.

### Task 7
I implemented a GUI using processing. I did not manage to get a satisfactory map working.
The GUI is used a forward controller for the zumo and displays messages also.

## Libraries Used

In this assignment I used:

ZumoBuzzer,
ZumoMotors,
Pushbutton,
QTRSensors,
ZumoReflectanceSensorArray,
Standard Template Library by Andy Brown
NewPing,
Processing,
ControlP5

### Overview of the code

The general idea was to have the Zumo build a list of the different states 
it would be in as it completed the course. To do this I needed more flexibility than
arrays would give me. I settled on using a STL library that has access to a deque.
By recording a collection of states my plan was to have all the important states recorded
until the end and then have the recorded states played back. This way the rooms with
objects in them would be visisted and empty rooms could be skipped.

The solution I came to for moving the Zumo was flawed. It was too simple. Relying on
sending different amounts of power to the wheels over varying amounts of times led to
unreliable results. Once calibrated, it would also be difficult for this Zumo the run
a different course. If I had more time I would look into a gyroscope based solution
which could be calibrated for different environments.

I wanted to have a GUI to control the Zumo. I researched using processing and node.js.
I chose processing because it seems to be a fairly simple to use tool that is powerful.
However procssing combined with my approach for building the path the zumo took made it difficult
to produce a real time map of the zumo movements. 

The Ultrasonic sensor scans on a timer. Every 5 seconds in scans for object and will send a message if it
finds a object. The reason for this was in the case an object was in the corridor. Combined with my state approach
the Zumo would only record a room if an object were inside, this way the return trip would miss the empty rooms.

