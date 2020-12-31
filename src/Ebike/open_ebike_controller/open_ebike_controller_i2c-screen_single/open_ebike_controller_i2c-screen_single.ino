#define LED 2

/*
Vesc UART connection 
*/
#include <VescUart.h>
VescUart UART;
#define RXD2 16
#define TXD2 17

/*
I2C oled screen config
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET,  800000, 100000);

/*
icon bitmaps
*/
#define lightIconHeight 18
#define lightIconWidth 27
const unsigned char lightIcon [] PROGMEM = {
0x07, 0xfe, 0x00, 0x00, 
0x0f, 0xff, 0x7f, 0xc0, 
0x1c, 0x03, 0x00, 0x00, 
0x38, 0x01, 0x80, 0x00, 
0x70, 0x01, 0xbf, 0xe0, 
0xe0, 0x01, 0x80, 0x00, 
0xc0, 0x01, 0x80, 0x00, 
0xc0, 0x01, 0x9f, 0xe0, 
0xc0, 0x01, 0x80, 0x00, 
0xc0, 0x01, 0x80, 0x00, 
0xc0, 0x01, 0x9f, 0xe0, 
0xc0, 0x01, 0x80, 0x00, 
0xe0, 0x01, 0x80, 0x00, 
0x70, 0x01, 0xbf, 0xe0, 
0x38, 0x01, 0x80, 0x00, 
0x1c, 0x03, 0x00, 0x00, 
0x0f, 0xff, 0x7f, 0xc0, 
0x07, 0xfe, 0x00, 0x00
};

#define stopIconHeight 20
#define stopIconWidth 28
const unsigned char stopIcon [] PROGMEM ={
  0x02, 0x3f, 0xc4, 0x00, 
0x0c, 0xff, 0xf3, 0x00, 
0x1b, 0xe0, 0x7d, 0x80, 
0x33, 0x80, 0x1c, 0xc0, 
0x77, 0x0f, 0x0e, 0xe0, 
0x66, 0x0f, 0x06, 0x60, 
0xee, 0x0f, 0x07, 0x70, 
0xcc, 0x0f, 0x03, 0x30, 
0xcc, 0x0f, 0x03, 0x30, 
0xcc, 0x0f, 0x03, 0x30, 
0xcc, 0x0f, 0x03, 0x30, 
0xcc, 0x06, 0x03, 0x30, 
0xcc, 0x00, 0x03, 0x30, 
0xee, 0x00, 0x07, 0x70, 
0x66, 0x0f, 0x06, 0x60, 
0x77, 0x0f, 0x0e, 0xe0, 
0x33, 0x80, 0x1c, 0xc0, 
0x1b, 0xe0, 0x7d, 0x80, 
0x0c, 0xff, 0xf3, 0x00, 
0x02, 0x3f, 0xc4, 0x00
};

#define endStopHeight 4
#define endStopWidth 5
const unsigned char leftEndStop [] PROGMEM = {
  0x18, 
  0x38, 
  0x78, 
  0xf8
};

const unsigned char rightEndStop [] PROGMEM = {
  0xc0, 
  0xe0, 
  0xf0, 
  0xf8
};

#define throttleCursorHeight 4
#define throttleCursorWidth 1
const unsigned char throttleCursor [] PROGMEM = {
  0xf0
};

// pin setup
//const byte hallPower1 = 2; // hall effect power wire
//const byte hallPower2 = 3; // second hall effect power
#define throttleSensor  36 // ADC to read throttle value
//const byte pwmOut = A7; // ADC to send pwm single to motor controller
//const byte ledOut = D5;

// Variables 
int currTV = 0; // current throttle value
int prevTV = 0; // previous throttle value


int currIndex = 0; // variable used to draw throttle indicator
int prevIndex = 0; // variable used to draw throttle indicator

int pwmVal = 0;

/*
TickSensorInput() variables
*/
  bool engageThrottle = false;
  
  float rps = 0.f;
  float revs = 0.f;
  float seconds = 1.f;
  
  int hallCount = 1;
  int inactiveCount = 0;

  unsigned long prevTime;
  unsigned long currTime;
  const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second

/*
tickScreenUpdate() variables
*/
  float _speed = 0;
  // battery voltage smoothing variables 
  // value to determine the size of the readings array.
  const int numReadings = 10;
  float readings[numReadings];      // the readings from the analog input
  int readIndex = 0;              // the index of the current reading
  float total = 0;                  // the running total
  float batteryAverage = 0;                // the average


/*
update current throttle value
*/
void updateCurrTV()
{
  prevTV = currTV;
  currTV = analogRead(throttleSensor);
}

void drawStartAnimation(void) {
  // draw concentric rount rectangle
  display.clearDisplay();
  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(500);

  // draw concentric filled round rectangle
  display.clearDisplay();
    for(int16_t i=0; i<display.height()/2-2; i+=2) {
      // The INVERSE color is used so round-rects alternate white/black
      display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
        display.height()/4, SSD1306_INVERSE);
      display.display();
      delay(1);
    }
    delay(250);
  //
  display.clearDisplay();
}

void drawStartUI(void) {
  // draw mph
  display.setTextSize(1);
  display.setCursor(108,32);
  display.print(F("MPH")); 

  display.setTextSize(2);
  display.setCursor(50, 42);
  display.print(F("NRM"));

  display.drawBitmap(
    4,
    0,
    lightIcon, lightIconWidth,lightIconHeight, 1
  );

  display.drawBitmap(
    8,
    60,
    leftEndStop, endStopWidth,endStopHeight, 1
  );

  display.drawBitmap(
    116,
    60,
    rightEndStop, endStopWidth,endStopHeight, 1
  );

  display.drawBitmap(
    14,
    60,
    throttleCursor, throttleCursorWidth, throttleCursorHeight, 1
  );
  
  display.display();      // Show initial text
}

void setup() {
  Serial.begin(115200); //pc output
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // vesc input/output

   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
     }
  
  pinMode(throttleSensor, INPUT); // Hall sensor for PAS sensor
  pinMode(LED,OUTPUT); // blue onboard LED setup
  
    //screen stup
    // executes on core 0 and binds to core 0
    while (!Serial2) {;} // infinite loop until Serial2 is initialized
    UART.setSerialPort(&Serial2);

    // tickSensorInput intial variable setup
    prevTime = millis(); // throttle algorithim time value setup
    updateCurrTV();

    // initialize all battery readings to 0
    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
    }
    //Serial.print("Screen Update Task running on core ");
    //Serial.println(xPortGetCoreID());

    
    //Serial.println( display.height());
    //Serial.println( display.width());
    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    drawStartAnimation();
    drawStartUI();
}


int sysCounter = 1;
// open ebike controller operating system
void loop() { 
  
  tickSensorInput();

  if(sysCounter % 100 == 0){
     tickScreenUpdate();
     
     if(sysCounter == 100000) {
      sysCounter = 1;
     }
  }  
  sysCounter++;
}

void tickSensorInput() {

     // throttle code
      currTime = millis();  //get the current "time" (actually the number of milliseconds since the program started
      if (currTime - prevTime >= period)  //test whether the period has elapsed
      {
        if(engageThrottle)
        {
          seconds += (currTime - prevTime) * 0.001;
          //Serial.println(seconds);
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
      
      if(inactiveCount >= 75)
      {
        revs = 0.f;
        seconds = 1.f;
        rps = 0.f;
        hallCount = 1;
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
    /*
      old transformations
      pwmVal = (int)(466.6667*rps + 833.3334);
      pwmVal = (int)(178.78*rps - 41.8);
    */  
       
        pwmVal = (int)(128*rps + 60);
    /*
      Serial.print("revs: ");
      Serial.print(revs);
      Serial.print("   |   ");
      Serial.print("seconds: ");
      Serial.print(seconds);
      Serial.print("   |   ");
      Serial.print("rps: ");
      Serial.print(rps);
      Serial.print("   |   ");
      Serial.print("i.c: ");
      Serial.print(inactiveCount);
      Serial.print("   |   ");
      Serial.print("pwm: ");
    */
    
     if(pwmVal > 254) {
      pwmVal = 254;
     }
     
      if(engageThrottle) {
        //Serial.println(pwmVal);
        // Serial.print("rps: ");
        // Serial.print(rps);
        // Serial.println();
        // esc.writeMicroseconds(pwmVal);
        UART.nunchuck.valueY = pwmVal;
      } else {
        // Serial.println(0);
        //esc.writeMicroseconds(1300);
        pwmVal = 127;
        UART.nunchuck.valueY = pwmVal;
      }

      UART.setNunchuckValues();
      updateCurrTV();
    
     // end throttle code
}

void tickScreenUpdate() { 
     // display code
     if( UART.getVescValues() )
     {
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

            // step 1 access pwm value transform its value from a scale of 0-100
            // then draw that many throttleCursors in a row
             //Serial.println(pwmVal);
            currIndex = (int)(pwmVal*0.787401)-100;
            //Serial.println(currIndex);

            if(currIndex - prevIndex > 0) {
              //draw from prevIndex to currIndex the normal throttle cursor
              for(int index = prevIndex; index <= currIndex; ++index) {
               display.drawBitmap(
                  16 + index,
                  60,
                  throttleCursor, throttleCursorWidth, throttleCursorHeight, 1
                );
              }
            } else if(currIndex - prevIndex < 0) {
              //draw from currIndex to prevIndex the inverse throttle cursor
              for(int index = currIndex; index <= prevIndex; ++index) {
               display.drawBitmap(
                  16 + index,
                  60,
                  throttleCursor, throttleCursorWidth, throttleCursorHeight, 0
                );
              }
            } 

            // update indexs
            prevIndex = currIndex;

            //calculate speed 
            // Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter x (motor pulley / wheelpulley) 
            //data.tachometerAbs
            //_speed = UART.data.rpm/10 * 60 * 0.00138193;
            //wheel diameter = 28.25in
            _speed = (UART.data.rpm/14)*(3.142 * 28.25)*0.00001578282;
            if(_speed > 99) {
              _speed = 99;
            }

            // draw voltage
            display.setTextSize(2);
            display.setCursor(4, 24);
            display.print(batteryAverage, 1);
            display.print(F("v"));
      
            // draw speedo
            display.setTextSize(4);  
            display.setCursor(80, 0);
            display.print(_speed, 0);
            display.display();      // Show initial text
            // Serial.println(UART.data.rpm);
            // Serial.println(UART.data.inpVoltage);
            // Serial.println(UART.data.ampHours);
            // Serial.println(UART.data.tachometerAbs);
     } else { 
       // do nothing 
     }
 // end display code
}
