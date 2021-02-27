
// task variables for multithreading
TaskHandle_t Task1;
TaskHandle_t Task2;

// Vesc UART definition
#include <VescUart.h>
VescUart UART;
#define RXD2 16
#define TXD2 17

// LED definition
#define LED 2

//screen setup
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#define SCREEN_WIDTH 96 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define clear() fillScreen(0)
#define show endWrite
/*
SD1331 Pin      Arduino ESP8266   rPi
1 GND
2 VCC
3 SCL/SCK/CLK/D0  13  GPIO14/D5 GPIO11/SPI0-SCLK  
4 SDA/MOSI/D1   11  GPIO13/D7 GPIO10/SPI0-MOSI  
5 RES/RST   9 GPIO15/D8 GPIO24      
6 DC/A0 (data)    8 GPIO05/D1 GPIO23      
7 CS      10  GPIO04/D2 GPIO08    
*/
#define sclk 18
#define mosi 23
#define cs   9 // slave selct
#define rst  5
#define dc   19
//#pragma message "Using HWSPI"
//Adafruit_SSD1331 display = Adafruit_SSD1331(&SPI, cs, dc, rst);
Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rst);


// Graphics information
#define LED_BLACK    0

#define LED_RED_VERYLOW   (3 <<  11)
#define LED_RED_LOW     (7 <<  11)
#define LED_RED_MEDIUM    (15 << 11)
#define LED_RED_HIGH    (31 << 11)

#define LED_GREEN_VERYLOW (1 <<  5)   
#define LED_GREEN_LOW     (15 << 5)  
#define LED_GREEN_MEDIUM  (31 << 5)  
#define LED_GREEN_HIGH    (63 << 5)  

#define LED_BLUE_VERYLOW  3
#define LED_BLUE_LOW    7
#define LED_BLUE_MEDIUM   15
#define LED_BLUE_HIGH     31

#define LED_ORANGE_VERYLOW  (LED_RED_VERYLOW + LED_GREEN_VERYLOW)
#define LED_ORANGE_LOW    (LED_RED_LOW     + LED_GREEN_LOW)
#define LED_ORANGE_MEDIUM (LED_RED_MEDIUM  + LED_GREEN_MEDIUM)
#define LED_ORANGE_HIGH   (LED_RED_HIGH    + LED_GREEN_HIGH)

#define LED_PURPLE_VERYLOW  (LED_RED_VERYLOW + LED_BLUE_VERYLOW)
#define LED_PURPLE_LOW    (LED_RED_LOW     + LED_BLUE_LOW)
#define LED_PURPLE_MEDIUM (LED_RED_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_PURPLE_HIGH   (LED_RED_HIGH    + LED_BLUE_HIGH)

#define LED_CYAN_VERYLOW  (LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_CYAN_LOW    (LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_CYAN_MEDIUM   (LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_CYAN_HIGH   (LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define LED_WHITE_VERYLOW (LED_RED_VERYLOW + LED_GREEN_VERYLOW + LED_BLUE_VERYLOW)
#define LED_WHITE_LOW   (LED_RED_LOW     + LED_GREEN_LOW     + LED_BLUE_LOW)
#define LED_WHITE_MEDIUM  (LED_RED_MEDIUM  + LED_GREEN_MEDIUM  + LED_BLUE_MEDIUM)
#define LED_WHITE_HIGH    (LED_RED_HIGH    + LED_GREEN_HIGH    + LED_BLUE_HIGH)

#define BIKE_HEIGHT   32
#define BIKE_WIDTH    32
const unsigned char bike_bmp [] PROGMEM = {
  // 'bike, 32x32px
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x3e, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x01, 0xe0, 0x00, 0x10, 0x01, 0x20, 
  0x00, 0x3f, 0xff, 0x60, 0x00, 0x57, 0xf9, 0x00, 0x00, 0x97, 0xf3, 0x00, 0x00, 0x10, 0x04, 0x00, 
  0x1f, 0x10, 0x09, 0xf0, 0x31, 0x94, 0x13, 0x18, 0x60, 0xd2, 0x26, 0x0c, 0x40, 0x52, 0x44, 0x04, 
  0x44, 0x4f, 0x84, 0x44, 0x40, 0x42, 0x04, 0x04, 0x60, 0xc2, 0x06, 0x0c, 0x31, 0x81, 0x03, 0x18, 
  0x1f, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// end screen setup

// Hall Effect Input setup
//const byte hallPower1 = 2; // hall effect power wire
//const byte hallPower2 = 3; // second hall effect power
const byte throttleSensor = 36; // ADC to read throttle value
//const byte pwmOut = A7; // ADC to send pwm single to motor controller
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

//battery voltage smoothing variables 

  // value to determine the size of the readings array.
  const int numReadings = 10;
  float readings[numReadings];      // the readings from the analog input
  int readIndex = 0;              // the index of the current reading
  float total = 0;                  // the running total
  float batteryAverage = 0;                // the average

int pwmVal = 0;

//Servo esc;

void updateCurrTV()
{
  prevTV = currTV;
  currTV = analogRead(throttleSensor);
}


void startDisplay(void) {
  // display start animation
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, LED_CYAN_HIGH);
    display.show();
    delay(70);
  }
  display.clear(); 
  display.setCursor(0, 0);
  display.drawBitmap(0,0,
      bike_bmp, BIKE_WIDTH, BIKE_HEIGHT, LED_WHITE_HIGH);
  delay(500);
}

void setup() {

// throttle setup
  pinMode(throttleSensor, INPUT); //Hall sensor
  
// led output setup
  pinMode(LED,OUTPUT);
    
// usb serial setup 
  Serial.begin(115200); //pc output

// vesc input/output
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 
    
// vesc uart setup
  while (!Serial2) {;}
  UART.setSerialPort(&Serial2);
  
//create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    pasAlgorithim,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    2,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */    

  xTaskCreatePinnedToCore(
                    screenUpdates,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 0 */    
}

void loop() { /* empty loop function */ } 


//Task1code: PAS and Throttle Algorithim
void pasAlgorithim( void * pvParameters ){
  (void) pvParameters;
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

 // PAS algorithim initial setup
  prevTime = millis();
  updateCurrTV();

  for(;;){
     // throttle code
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
        digitalWrite(LED,HIGH);
      }
      else if(prevTV > 500 && currTV < 500)
      {
        hallCount ++;
        inactiveCount = 0;
        digitalWrite(LED,HIGH);
      }
      else if(engageThrottle)
      {
        inactiveCount++;
      }
      
      if(inactiveCount >= 130)
      {
        revs = 0.f;
        seconds = 1.f;
        rps = 0.f;
        engageThrottle = false;
        digitalWrite(LED,LOW);
      }
      
      if(hallCount == 24)
      {
        revs += 1.f; 
        engageThrottle = true;
        rps = revs/seconds;
        hallCount = 1;
      }
        
       //pwmVal = (int)(466.6667*rps + 833.3334);
       //pwmVal = (int)(178.78*rps - 41.8);
       
       pwmVal = (int)(130*rps + 60);
    
    //  Serial.print("revs: ");
    //  Serial.print(revs);
    //  Serial.print("   |   ");
    //  Serial.print("seconds: ");
    //  Serial.print(seconds);
    //  Serial.print("   |   ");
//      Serial.print("rps: ");
//      Serial.print(rps);
    //  Serial.print("   |   ");
    //  Serial.print("i.c: ");
    //  Serial.print(inactiveCount);
    //  Serial.print("   |   ");
     // Serial.print("pwm: ");
     if(pwmVal > 254) {
      pwmVal = 254;
     }
   
      if(engageThrottle)
      {
          Serial.println(pwmVal);
//         Serial.print("rps: ");
//         Serial.print(rps);
//         Serial.println();
        //esc.writeMicroseconds(pwmVal);
        UART.nunchuck.valueY = pwmVal;
      }
      else
      {
        //Serial.println("throttle disengaged");
        //esc.writeMicroseconds(1300);
         UART.nunchuck.valueY = 127;
      }

      UART.setNunchuckValues();
      updateCurrTV();
      vTaskDelay(1);
    
     // end throttle code
  } 
}

// Task2: screen updates
void screenUpdates( void * pvParameters ){
  (void) pvParameters;
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  //screen setup
  display.begin(); 
  display.setFont();
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextWrap(false);
  display.setTextColor(LED_PURPLE_HIGH, LED_BLACK);
  display.setAddrWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  display.clear();
  startDisplay();

// initialize Battery V measurments to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
  
   for(;;){
        // display code
     if( UART.getVescValues() ){
        // perform averaging
                  // subtract the last reading:
        total = total - readings[readIndex];
        // read from the sensor:
        readings[readIndex] = UART.data.inpVoltage;
        // add the reading to the total:
        total = total + readings[readIndex];
        // advance to the next position in the array:
        readIndex = readIndex + 1;
      
        // if we're at the end of the array...
        if (readIndex >= numReadings) {
          // ...wrap around to the beginning:
          readIndex = 0;
        }
      
        // calculate the average:
        batteryAverage = total / numReadings;

        // update display
        
        display.setCursor(0, 24);
        display.print(batteryAverage, 1);
        display.println(F("v"));
        display.show(); // Show initial text 
        vTaskDelay(100);
     } else {
      vTaskDelay(500);
     } 
   }
 // end display code
 }
