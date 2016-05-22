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
float SessionDistance = 0;                                      // Real-Time distance
float SessionSpeed = 0;                                         // Sum of the session speed to calculate session average speed
bool RunningState = false;                                      // Running session state true-false boolean
retained float Distance;                                        // Absolute distace
float Speed;                                                    // Average Speed
retained float SumSpeed;                                        // To sum all speeds
int Counter = 0;                                                // Spin counter
retained int Sessions;                                          // Absolute session counter
unsigned long CurrentTime;               
float ElapsedTime;
float WaitTime;
float FirstTime;
int MaxTime = 20;                                               // Maximum time between spins in Seconds
long LastTime = 0;
float WheelLength = 0.43;                                       // The length of your wheel in Meters  


/********************
ThingSpeak configuration
********************/


unsigned long myChannelNumber = 118200;                         // Your thingspeak channel number
const char * myWriteAPIKey = "CPCMSS99N7EXQH4V";                // Your thingspeak channel write api key

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
    ElapsedTime = (CurrentTime - LastTime) / 1000.0;               // Elapsed time in seconds
    
    if (ElapsedTime > MaxTime) {                                // If elapsed time is more than max time, the session is over.
        
        RunningState = false;                                   // Stop runnning session
      
        Sessions ++;                                            // Calculate overall session, speed and distances
        
        SessionDistance = Counter*WheelLength;                  // Calculate session distance
        Distance += SessionDistance;                            // Calculate total distance
        
        SessionSpeed = SessionDistance/(LastTime-FirstTime);    // Calculate session speed
        
        SumSpeed += SessionSpeed;                               // Sum of all speeeds
        Speed = SumSpeed / Sessions;                            // Average Speed of all sessions
        
        ThingSpeak.setField(1,Distance);                        //Add Session Distance data to field 1
        ThingSpeak.setField(2,Speed);                           //Add Average Speed data point to field 2
        ThingSpeak.setField(3,SessionDistance);                 //Add Session Distance data point to fiel 3
        ThingSpeak.setField(4,SessionSpeed);                    //Add Session speed data point to field 4
        
        
        ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey); // Writte all the fields together
        
        Particle.publish("Ham", String(SessionDistance), PRIVATE);           // Publish to IFTTT the session distance and speed

        Counter = 0;                                            // Reset Session values
        SessionDistance = 0;
        SessionSpeed = 0;
      
        }
    }
}

/********************
   Count interrupt
********************/

void count() {

CurrentTime = millis();                                         // Update time
RunningState = true;                                            // Ham is running
Counter ++;                                                     // Count the spin
if (Counter == 1){
    FirstTime = CurrentTime;
}
LastTime = CurrentTime;                                         // Update last time
}
