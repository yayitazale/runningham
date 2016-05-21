/********************
  Library include
********************/

// This #include statement was automatically added by the Particle IDE.
#include "ThingSpeak/ThingSpeak.h"


STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));         // Startup SRAM 



/********************
Variable declaration
********************/

const int Hall = D1;                                            // Hall sensor to D0 pin
float RTdistance = 0;                                           // Real-Time distance
float RTspeed = 0;                                              // Real-Time Speed
float SessionSpeed = 0;                                         // Sum of the session speed to calculate session average speed
float AVSessionSpeed = 0;                                       // Average of the session speed
bool RunningState = false;                                      // Running session state true-false boolean
retained float Distance;                                        // Absolute distace
retained float Speed;                                           // Absolute Speed sum to calculate average speed
retained float AVDistance;                                      // Average Distance
retained float AVSpeed;                                         // Average Speed
int Counter = 0;                                                // Spin counter
retained int Sessions;                                          // Absolute session counter
unsigned long CurrentTime;               
float ElapsedTime;
int MaxTime = 20;                                               // Maximum time between spins in Seconds
long LastTime = 0;
float WheelLength = 0.43;                                       // The length of your wheel in Meters  


/********************
ThingSpeak configuration
********************/


unsigned long myChannelNumber = **YourChannelNumber**;                         // Your thingspeak channel number
const char * myWriteAPIKey = **YourAPIkey**;                // Your thingspeak channel write api key

TCPClient client;                                               // Client to stream is TCP as we use a core.



/********************
     Pin setup
********************/

void setup() {
    

pinMode(Hall, INPUT_PULLDOWN);                                    // Hall effect as input
attachInterrupt(Hall, count, RISING);                           // Attach the "count" interrupt when hall input pin is rising

ThingSpeak.begin(client);                                       // Start thingspeak api

}


/********************
     Main loop
********************/

void loop() {
    

if (RunningState == true) {                                     // Check if Ham is running;
    
    CurrentTime = millis();                                     // Update time
    ElapsedTime = (CurrentTime - LastTime) / 1000.0;            // Elapsed time in seconds
    
    if (ElapsedTime > MaxTime) {                                // If elapsed time is more than max time, the session is over.
        
        RunningState = false;                                   // Stop runnning session
      
        Sessions ++;                                            // Calculate overall session, speed and distances
        Speed += AVSessionSpeed;
        Distance += RTdistance;
      
        AVSpeed = Speed / Sessions;                             // Calcculate Average Speed and Distance
        AVDistance = Distance / Sessions;
        
        ThingSpeak.setField(1,Distance);                        //Add Session Distance data to field 1
        ThingSpeak.setField(2,AVSpeed);                         //Add Average Speed data point to field 2
        ThingSpeak.setField(3,AVDistance);                      //Add Average Distance data point to fiel 3
        ThingSpeak.setField(4,AVSessionSpeed);                  //Add Session Speed data point to field 4
        
        
        
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Writte all the fields together
        
        Particle.publish(String(RTdistance), String(AVSessionSpeed), PRIVATE);           // Publish to IFTTT the session distance and speed

        delay(20000);                                           // ThingSpeak will only accept updates every 15 seconds. 
    
        Counter = 0;                                            // Reset Session values
        RTdistance = 0;
        RTspeed = 0;
        SessionSpeed = 0;
        LastTime = 0;
      
        }
    }
}

/********************
   Count interrupt
********************/

void count() {

CurrentTime = millis();                                         // Update time
RunningState = true;                                            // Ham is running
ElapsedTime = (CurrentTime - LastTime) / 1000.0;                // Elapsed time in seconds

 if (ElapsedTime > 0) {                                
     
     RTspeed = WheelLength / ElapsedTime;                       // Calculate Real time spead
     
     }
     
Counter ++;                                                     // Count the spin
SessionSpeed += RTspeed;                                        // Sum every spin speed to session speed
AVSessionSpeed = SessionSpeed / Counter;                        // Calculate session average speed                           
RTdistance += WheelLength;                                      // Real-time speed calculator
LastTime = CurrentTime;                                         // Update last time

}
