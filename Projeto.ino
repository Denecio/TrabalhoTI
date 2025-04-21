#include <TimeLib.h>

const int leds[] = {2, 3, 4, 5};
#define trigPin 7
#define echoPin 8
#define buzzerPin 9

String pills[] = {"00:00","10:00","16:00","22:54"};
int index = -1;

float distance;

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

void setup() {
  for(int i = 0; i < sizeof(leds); i++){
    pinMode(leds[i], OUTPUT);
  }

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600); // Start Serial Monitor

  setSyncProvider(requestSync);  //set function to call when sync required
}

void loop() {

  if (Serial.available()) {
    processSyncMessage();
  }

  if (timeStatus()!= timeNotSet) {
    const String time = String(String(hour())+":"+String(minute()));
    // Serial.println(time);

    for(int i = 0; i < sizeof(leds); i++){
      if(time == pills[i]){
        index = i;
      }
    }

    distance = calcDist();

    if(index != -1 && distance > 10)
      setAlarm(leds[index]);
  }
}

void setAlarm(int led){
  digitalWrite(led, HIGH);
  tone(buzzerPin, 4000, 20);
}

float calcDist(){
  long duration;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 100000);

  return (duration*0.034)/2; 
}

// These functions are from the example code illustrating Time library set through serial port messages.
// The whole repository can be seen here https://github.com/PaulStoffregen/Time/tree/master

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage() {
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync(){
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
