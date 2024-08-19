

// libraries to be included
#include <Wire.h> // I2C communication
#include <TM1637TinyDisplay.h> // 7-seg display   Link: https://github.com/jasonacox/TM1637TinyDisplay


// new with DST
#include "RTClib.h" 
#include "DST_RTC.h"   // Link:  https://github.com/andydoro/DST_RTC


// rotary encoder
#include <ClickEncoder.h> // Link: https://github.com/soligen2010/encoder
#include <TimerOne.h>


// Instantiation and pins configurations

// display
// Pin 3 - > CLK
// Pin 2 - > DIO

TM1637TinyDisplay display(3,2); // schwarze version umgekehrt!

//RTC

RTC_DS3231 rtc; // clock object

DST_RTC dst_rtc; // DST object


// RTC global variables
byte currentSec;
byte lastSec;
byte hourOffset = 1; // DE: UTC+1 // GR: UTC+2
//new
const char rulesDST[] = "EU";   // EU DST rules

// DateTime now = myRTC.now();
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// encoder
// Encoder & Button
int16_t oldEncPos, encPos;
// uint8_t buttonState;

#define pinA 7
#define pinB 6
#define pinSw 8 //switch
#define STEPS 4


ClickEncoder encoder(pinA, pinB, pinSw, STEPS);
// encoder values
// 0 open, 1 closed, 2 pressed, 3 held, 4 released, 5 clicked, 6 double clicked


// encoder timer
void timerIsr() 
{
  encoder.service();
}

uint64_t timer;


void setup() {
  // put your setup code here, to run once:
  
  // Serial begin
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  


  // code to set the rtc time manually for testing
  // myRTC.setYear(23);
  // myRTC.setMonth(12);
  // myRTC.setDate(12);
  // myRTC.setHour(19);
  // myRTC.setMinute(10);
  // myRTC.setDoW(4);
  
  

  // check RTC

  //if(myRTC.lostPower()) Serial.println("rtc lost power");
  // Encoder setup 
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  // encoder.setAccelerationEnabled(true);
  oldEncPos = -1;

  //display init
  display.begin();
  // 
  display.showString("HELLO");
  Serial.println("Hello");
  Serial.println("nano is running");
  delay(1000);
  display.clear();
  delay(500);
  //timer init
  timer = millis();

  //display.showNumber(myRTC.getHour(h12Flag, pmFlag)*100 + myRTC.getMinute());
  //
  DateTime standardTime = rtc.now();
  //Serial.println("Standard Time");


  DateTime theTime = dst_rtc.calculateTime(standardTime); // takes into account DST
  printTheTime(theTime);
  display.showNumber(theTime.hour()*100 + theTime.minute(),true);
  //byte temp = myRTC.getTemperature();
  byte temp = rtc.getTemperature();
  //lastSec = myRTC.getSecond();
  standardTime = rtc.now();
  lastSec = standardTime.second();
  //int hBright = myRTC.getHour(h12Flag, pmFlag);
  int hBright = theTime.hour();
  // Serial.print("Brightness: ");
  // Serial.println(hBright);
  if (hBright > 6 && hBright < 9)
  display.setBrightness(3);
  else if (hBright > 8 && hBright < 15)
  display.setBrightness(5);
  else if (hBright > 14 && hBright < 18)
  display.setBrightness(4);
  else display.setBrightness(0);
  Serial.println("End of setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //display the time
  DateTime standardTime = rtc.now();
  DateTime theTime = dst_rtc.calculateTime(standardTime);
  while (encoder.getButton() != 3)
  {
    //int time = myRTC.getHour(h12Flag, pmFlag)*100 + myRTC.getMinute();
    standardTime = rtc.now();
    //Serial.println("Standard Time");
    //printTheTime(standardTime);
    theTime = dst_rtc.calculateTime(standardTime); // takes into account DST
    int time = theTime.hour()*100 + theTime.minute();
    //Serial.println(standardTime.hour());
    //Serial.println(standardTime.year());

    
    //get current second
    currentSec = standardTime.second();
    // wait for the second to change
    while (currentSec == lastSec)
    {
      delay(250);
      //currentSec = myRTC.getSecond();
      standardTime = rtc.now();
      currentSec = standardTime.second(); 
      //Serial.println(currentSec);     
      if (encoder.getButton() == 5)
      {
        Serial.println();
        Serial.print("Temperature: ");
        //Serial.print(myRTC.getTemperature());
        Serial.print(rtc.getTemperature());
        Serial.println(" C");
        Serial.println();
        display.clear();

        int temp = rtc.getTemperature();
        // display.showString("\xB0", 1, 3);
        display.showString("\xB0", 1, 2);
        display.showString("C", 1, 3);
        // display.showNumber(temp, false, 3, 0);
        display.showNumber(temp, false, 2, 0);
        delay(2000);
        //display.schowString("");
      }
    }
    //lastSec = myRTC.getSecond(); // update last sec
    standardTime = rtc.now();
    lastSec = standardTime.second();
    //Serial.println(lastSec);
    //Serial.println(time);
    // display the time
    if (lastSec%2)
    {
      // display time colon off
      display.showNumberDec(time, 0, true);
    }
    else
    {
      //display time, colon on
      display.showNumberDec(time, 0b01000000, true);
    }

    if(theTime.minute() == 0)
    {
      //int hBright = myRTC.getHour(h12Flag, pmFlag);
      int hBright = theTime.hour();
      // Serial.print("Brightness: ");
      // Serial.println(hBright);
      if (hBright > 6 && hBright < 9)
      display.setBrightness(3);
      else if (hBright > 8 && hBright < 15)
      display.setBrightness(5);
      else if (hBright > 14 && hBright < 18)
      display.setBrightness(4);
      else display.setBrightness(0);
    }
 
    //buttonState = encoder.getButton();
  }
  //exiting while loop

  // SETUP
  standardTime = rtc.now();
  theTime = dst_rtc.calculateTime(standardTime);
  Serial.println("entering setup");
  // button was long pressed, set the date / time
  display.clear();
  // display an animation
  // Demo Horizontal Level Meter
  for (int count = 0; count < 2; count++) {
    for (int x = 0; x <= 100; x = x + 10) {
      display.showLevel(x, false);
      delay(20);
    }
    for (int x = 100; x >= 0; x = x - 10) {
      display.showLevel(x, false);
      delay(20);
    }
  }
  display.clear();
  display.showString("SET DATE");
  delay(1000);
  display.showString("YEAR");
  delay(1000);

  //int param = standardTime.year();
  int year = theTime.year();
  display.showNumber(year);
  Serial.println("set date");
  Serial.println("set the year");
  
  // encPos = myRTC.getYear(); // set the encoder to the current year 
  encPos = year;
  oldEncPos = encPos; // update the old position value for the first loop
  // as long as the button wasn't clicked
  // read encoder, set new value, update the display 
  while (encoder.getButton() != 5) // encoder button is not beeing clicked
  {
     // todo!colon blinking
    // set the new values

    if (encoder.getButton() == 3) goto bailout;
    encPos += encoder.getValue();
    if (encPos != oldEncPos) // encoder active - value has been changed
    {
      // check boundaries
      // if (encPos > 99)
      // {
      //   encPos = 0;
      // }
      // if (encPos < 0)
      // {
      //   encPos = 99;
      // }
      encPos = constrain(encPos,2000,2099);
      // update 7-segment display
      display.showNumber(encPos);
      //tm1637.dispNumber(date); 
      oldEncPos = encPos; // update old encoder position
      //Serial.println(encPos+2000);
    }
    delay(10); //stability
  } // end of while loop - year setting
  // set the rtc
  //myRTC.setYear(encPos);
  year = encPos;
  rtc.adjust(DateTime(year,theTime.month(),theTime.day(),theTime.hour(),theTime.minute(),theTime.second()));
  //  rtc.adjust(DateTime(theTime.year(),theTime.month(),theTime.day(),theTime.hour(),theTime.minute(),theTime.second()));
  display.showNumber(year);
  Serial.print("year set to: ");
  Serial.print(year);
  Serial.println(" encoder");
  standardTime = rtc.now(); // update the time from the rtc
  theTime = dst_rtc.calculateTime(standardTime);
  Serial.print(standardTime.year());
  Serial.println(" rtc");

  Serial.println("set the month");
  // set the month
  display.showString("MONTH");
  delay(500);
 //encPos = myRTC.getMonth(century); // set the encoder to the current month
  encPos = standardTime.month(); // set the encoder to the current month
  display.showNumber(encPos);
  oldEncPos = encPos; // update the old position value for the first loop

  while (encoder.getButton() != 5)
  {
    if (encoder.getButton() == 3) goto bailout;
    encPos += encoder.getValue();
    if (encPos != oldEncPos) // encoder active - value has been changed
    {
      // check boundaries
      // if (encPos > 12)
      // {
      //   encPos = 1;
      // }
      // if (encPos < 1)
      // {
      //   encPos = 12;
      // }
      encPos = constrain(encPos,1,12);
      // update 7-segment display
      display.showNumber(encPos,false);
      oldEncPos = encPos; // update old encoder position
      Serial.println(encPos);
    }
  }
  // set the rtc
  //myRTC.setMonth(encPos);
  // rtc.adjust(DateTime(theTime.year(),encPos,standardTime.day(),standardTime.hour(),standardTime.minute(),standardTime.second()));
  rtc.adjust(DateTime(theTime.year(),encPos,theTime.day(),theTime.hour(),theTime.minute(),theTime.second()));
  standardTime = rtc.now(); // update the time from the rtc
  theTime = dst_rtc.calculateTime(standardTime);
  display.showNumber(encPos);
  Serial.println("month set to: ");
  Serial.print(encPos);
  Serial.println(" encoder");
  Serial.print(standardTime.month());
  Serial.println(" rtc");
  //standardTime = rtc.now(); // update the time from the rtc

  Serial.print("set the date");

  // set the date
  display.showString("DATE");
  delay(2000);
  //encPos = myRTC.getDate(); // set the encoder to the current date
  encPos = standardTime.day(); // set the encoder to the current date
  display.showNumber(encPos);
  oldEncPos = encPos; // update the old position value for the first loop
  while (encoder.getButton() != 5) // encoder button is not beeing clicked
  {
    if (encoder.getButton() == 3) goto bailout; 
    // set the new values
    encPos += encoder.getValue();
    if (encPos != oldEncPos) // encoder active - value has been changed
    {
      // check boundaries
      //toDO: constrain the date according to the current month  rtc.daysInMonth(now.year(), now.month()));
      // if (encPos > 31)
      // {
      //   encPos = 1;
      // }
      // if (encPos < 1)
      // {
      //   encPos = 31;
      // }
      // constrain day, based on current month
      //encPos = constrain(encPos, 1, numberOfDaysInMonth(myRTC.getYear(), myRTC.getMonth(century))); // Constrain day based on current month
      encPos = constrain(encPos, 1, numberOfDaysInMonth(standardTime.year(), standardTime.month())); // Constrain day based on current month
      // update 7-segment display
      display.showNumber(encPos,false);
      oldEncPos = encPos; // update old encoder position
      Serial.println(encPos);
    }
  }
  // set the rtc
  //myRTC.setDate(encPos);
  // rtc.adjust(DateTime(standardTime.year(),standardTime.month(),encPos,standardTime.hour(),standardTime.minute(),standardTime.second()));
  rtc.adjust(DateTime(theTime.year(),theTime.month(),encPos,theTime.hour(),theTime.minute(),theTime.second()));
  
  standardTime = rtc.now(); // update the time from the rtc
  theTime = dst_rtc.calculateTime(standardTime);
  display.showNumber(encPos);  
  Serial.println("date set to: ");
  Serial.print(encPos);
  Serial.println(" encoder");
  Serial.print(standardTime.day());
  Serial.println(" rtc");
  

  display.showString("TIME");
  Serial.println("set time");
  delay(2000);
  Serial.println("set the hour");
  // set the hour
  display.showString("Hour");
  delay(2000);
  encPos = standardTime.hour(); // set the encoder to the current hour
  display.showNumber(encPos);
  oldEncPos = encPos; // update the old position value for the first loop
  while (encoder.getButton() != 5) // encoder button is not beeing clicked
  {
    
    if (encoder.getButton() == 3) goto bailout;
    // set the new values
    encPos += encoder.getValue();
    if (encPos != oldEncPos) // encoder active - value has been changed
    {
      //check boundaries
      if (encPos > 23)
      {
        encPos = 0;
      }
      if (encPos < 0)
      {
        encPos = 23;
      }
      //encPos = constrain(encPos,1,23);
      // update 7-segment display
      display.showNumber(encPos,false);
      oldEncPos = encPos; // update old encoder position
      Serial.println(encPos);
    }
  }
  // set the rtc
  if(!dst_rtc.checkDST(standardTime)){
    rtc.adjust(DateTime(theTime.year(),theTime.month(),theTime.day(),encPos-hourOffset,theTime.minute(),theTime.second()));
  }
  else{
    rtc.adjust(DateTime(theTime.year(),theTime.month(),theTime.day(),encPos-(hourOffset+1),theTime.minute(),theTime.second()));
  }
  
  //rtc.adjust(DateTime(theTime.year(),theTime.month(),theTime.day(),encPos-hourOffset,theTime.minute(),theTime.second()));
  standardTime = rtc.now(); // update the time from the rtc
  theTime = dst_rtc.calculateTime(standardTime);
  display.showNumber(encPos);  
  Serial.println("hour set to: ");
  Serial.print(encPos);
  Serial.println(" encoder");
  Serial.print(standardTime.hour());
  Serial.println(" rtc");
  

  Serial.println("set the minute");

  display.showString("Minute");
  delay(500);
  //encPos = myRTC.getMinute(); // set the encoder to the current min
  encPos = standardTime.minute();
  //DateTime testTime = rtc.now();
  
  display.showNumber(encPos);
  oldEncPos = encPos; // update the old position value for the first loop
  while (encoder.getButton() != 5) // encoder button is not beeing clicked
  {
    // todo! colon blinking
    // set the new values
    encPos += encoder.getValue();
    if (encPos != oldEncPos) // encoder active - value has been changed
    {
      encPos = constrain(encPos,0,59);
      // update 7-segment display
      display.showNumber(encPos,false);
      oldEncPos = encPos; // update old encoder position
      Serial.println(encPos);
    }
  }
    // set the rtc
  //myRTC.setMinute(encPos);
  //check if DST is active, if so, set the rtc hour to encpos - 1
  rtc.adjust(DateTime(theTime.year(),theTime.month(),theTime.day(),theTime.hour(),encPos,theTime.second()));
  standardTime = rtc.now(); // update the time from the rtc
  theTime = dst_rtc.calculateTime(standardTime);
  display.showNumber(encPos);  
  Serial.print("minute set to:");
  Serial.println(encPos);

  bailout:
  display.showString("done");
  delay(2000);
  // standardTime = rtc.now();
   // Serial.println(standardTime.hour());
    // Serial.println(standardTime.year());
}

uint8_t numberOfDaysInMonth( uint16_t year, uint8_t month )
{
	if ( month == 2 )  
		return ( (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0) ) ? 29 : 28;
		
	else if ( month == 4 || month == 6 || month == 9 || month == 11 )  
  		return 30;
  		
	return 31;
}

// print time to serial
void printTheTime(DateTime theTimeP) {
  Serial.print(theTimeP.year(), DEC);
  Serial.print('/');
  Serial.print(theTimeP.month(), DEC);
  Serial.print('/');
  Serial.print(theTimeP.day(), DEC);
  Serial.print(' ');
  Serial.print(theTimeP.hour(), DEC);
  Serial.print(':');
  Serial.print(theTimeP.minute(), DEC);
  Serial.print(':');
  Serial.print(theTimeP.second(), DEC);
  Serial.println();
}