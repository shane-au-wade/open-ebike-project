// task variables for multithreading
TaskHandle_t Task1;

#define LED 2

//screen setup
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <VescUart.h>
VescUart UART;

#define RXD2 16
#define TXD2 17

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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
// pin setup
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

 void testdrawroundrect(void) {
    display.clearDisplay();
  
    for(int16_t i=0; i<display.height()/2-2; i+=2) {
      display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
        display.height()/4, SSD1306_WHITE);
      display.display();
      delay(1);
    }
    delay(500);
}

void testfillroundrect(void) {
    display.clearDisplay();
  
    for(int16_t i=0; i<display.height()/2-2; i+=2) {
      // The INVERSE color is used so round-rects alternate white/black
      display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
        display.height()/4, SSD1306_INVERSE);
      display.display();
      delay(1);
    }
    delay(500);
}


void setup() {
     // usb serial setup 
      
     
      Serial.begin(115200); //pc output
    
      Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // vesc input/output

        // initialize all the readings to 0:
        for (int thisReading = 0; thisReading < numReadings; thisReading++) {
          readings[thisReading] = 0;
        }
     
     // throttle setup
      pinMode(throttleSensor, INPUT); //Hall sensor

      pinMode(LED,OUTPUT);
      
      prevTime = millis();
      updateCurrTV();
    
    //screen stup
      while (!Serial2) {;}
      UART.setSerialPort(&Serial2);
      
      // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
      }
      
      // Clear the buffer
      display.clearDisplay();
      // Show initial display buffer contents on the screen --
      testdrawroundrect(); // Draw rounded rectangles (outlines)
      testfillroundrect(); // Draw rounded rectangles (filled)
      display.clearDisplay();
      //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
        xTaskCreatePinnedToCore(
                          Task1code,   /* Task function. */
                          "Task1",     /* name of task. */
                          10000,       /* Stack size of task */
                          NULL,        /* parameter of the task */
                          1,           /* priority of the task */
                          &Task1,      /* Task handle to keep track of created task */
                          0);          /* pin task to core 0 */                  
        delay(10); 

}


//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

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
        Serial.println("throttle disengaged");
        //esc.writeMicroseconds(1300);
         UART.nunchuck.valueY = 127;
      }

      UART.setNunchuckValues();
      updateCurrTV();
      delay(1);
    
     // end throttle code
  } 
}


void loop() {
    // display code
     if(UART.getVescValues())
     {
             display.clearDisplay();
           // display.setTextSize(2); // Draw 2X-scale text
           
            display.setRotation(3);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
          
            
          //  display.println(F("Battery:"));
          
          display.drawBitmap(
              0,
              0,
              bike_bmp, BIKE_WIDTH, BIKE_HEIGHT, 1);
              
            display.setTextSize(2); // Draw 2X-scale text
            display.setCursor(0, 24);

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
            
            
            display.println(batteryAverage, 1);
            
            //display.setCursor(24, 50);
            display.println(F("v"));
            display.display();      // Show initial text
            delay(100);
     } else {
      delay(500);
     }
 
 // end display code
   
 }
