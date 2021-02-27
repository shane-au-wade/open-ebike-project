#include <Servo.h>

// pin setup
const byte hallPower1 = 2; // hall effect power wire
const byte hallPower2 = 3; // second hall effect power
const byte throttleSensor = A2; // ADC to read throttle value
const byte pwmOut = A7; // ADC to send pwm single to motor controller
//const byte ledOut = D5;

// Variables 
bool engageThrottle = false;
int currTV = 0; // current throttle value
int prevTV = 0; // previous throttle value

float rps = 0.f;
float revs = 0.f;
float seconds = 1.f;

int hallCount = 1;
int inactiveCount = 0;

unsigned long prevTime;
unsigned long currTime;
const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second

int pwmVal = 0;

Servo esc;

void updateCurrTV()
{
  prevTV = currTV;
  currTV = analogRead(throttleSensor);
}

void setup() {
  // put your setup code here, to run once: 

Serial.begin(9600);

  pinMode(hallPower1, OUTPUT);
  pinMode(hallPower2, OUTPUT);
   
  digitalWrite(hallPower1, HIGH); 
  digitalWrite(hallPower2, HIGH); 
  
  pinMode(throttleSensor, INPUT); //Hall sensor

  //pinMode(pwmOut, OUTPUT);

  esc.attach(pwmOut);
  prevTime = millis();
  updateCurrTV();
}

void loop() {
  currTime = millis();  //get the current "time" (actually the number of milliseconds since the program started
  if (currTime - prevTime >= period)  //test whether the period has elapsed
  {
    if(engageThrottle)
    {
      seconds += 1.f;
    }
    prevTime = currTime;
  }

  
  if(prevTV < 500 && currTV > 500)
  {
    hallCount ++;
    inactiveCount = 0;
  }
  else if(prevTV > 500 && currTV < 500)
  {
    hallCount ++;
    inactiveCount = 0;
  }
  else if(engageThrottle)
  {
    inactiveCount++;
  }
  
  if(inactiveCount >= 1000)
  {
    revs = 0.f;
    seconds = 1.f;
    rps = 0.f;
    engageThrottle = false;
  }
  
  if(hallCount == 24)
  {
    revs += 1.f; 
    engageThrottle = true;
    rps = revs/seconds;
    hallCount = 1;
  }
    
   pwmVal = (int)(466.6667*rps + 833.3334);

//  Serial.print("revs: ");
//  Serial.print(revs);
//  Serial.print("   |   ");
//  Serial.print("seconds: ");
//  Serial.print(seconds);
//  Serial.print("   |   ");
//  Serial.print("rps: ");
//  Serial.print(rps);
//  Serial.print("   |   ");
//  Serial.print("i.c: ");
//  Serial.print(inactiveCount);
//  Serial.print("   |   ");
  Serial.print("pwm: ");
 
  if(engageThrottle)
  {
    Serial.println(pwmVal);
    esc.writeMicroseconds(pwmVal);
  }
  else
  {
    Serial.println("throttle disengaged");
    esc.writeMicroseconds(1300);
  }
  
  updateCurrTV();
}
