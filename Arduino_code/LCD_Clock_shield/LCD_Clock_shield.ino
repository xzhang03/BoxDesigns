#include "RTClib.h"
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_PCF8574 lcd(0x27);  

// Initialize rtc
RTC_PCF8523 rtc;

// RTC variables
DateTime now;

byte nMonth, nDate, nHour, nMin, nSec;
float nTemp;

// Schedule varaibles
byte LightOn = 1;
byte LightOff = 13;
byte RoomLightOn = 6;
byte RoomLightOff = 18;

// Pin varaibles
byte LightPin1 = 2;
byte LightPin2 = 3;
byte LightPin3 = 4;
byte ButtonPin = 12;
byte LightPMW = 0; // 0 = On, 255 = Off;

// Display variables
byte Display2_switch = 10; // Switch to Display 2 if at the top X seconds of each minute

// Debug variables
float t0;

void setup() {
  // Start the I2C interface
  Wire.begin();

  // Initialize rtc
  Serial.begin(9600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // Force updating time (uncomment to do)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  // Initialize LCD.
  lcd.begin(16, 2); // initialize the lcd
  lcd.setBacklight(255);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello! Oh, it's");
  lcd.setCursor(0, 1);
  lcd.print("you again...");

  // Start the serial interface
  Serial.begin(9600);

  // Set up pins
  pinMode(LightPin1, OUTPUT);
  pinMode(LightPin2, OUTPUT);
  pinMode(LightPin3, OUTPUT);
  
  pinMode(ButtonPin, INPUT);
}

void loop() {
  // Log cycle start time
  t0 = millis();

  // Time stamp 
  now = rtc.now();
  
  // Get data from RTC
  nMonth = now.month();
  nDate = now.day();
  nHour = now.hour();
  nMin = now.minute();
  nSec = now.second();

  // Serial
  // New line for serial
  Serial.println("");
  
  // Serial Date
  Serial.print(nMonth);
  Serial.print("/");
  Serial.print(nDate);
  Serial.print(" ");

  // Serial Time
  Serial.print(nHour);
  Serial.print(":");
  Serial.print(nMin);
  Serial.print(":");
  Serial.println(nSec);

  // Light on/off
  if (nHour >= LightOn && nHour< LightOff){
    // Light on
    Serial.print("Light ON (");
    Serial.print(LightOn);
    Serial.print(",");
    Serial.print(LightOff);
    Serial.println(")");
  }
  else{
    // Light off
    Serial.print("Light OFF (");
    Serial.print(LightOn);
    Serial.print(",");
    Serial.print(LightOff);
    Serial.println(")");
  }
  
  // Room info
  Serial.print("RoomLight ");
  if (nHour >= RoomLightOn && nHour< RoomLightOff){
    Serial.print("On (");
  }
  else{
    Serial.print("Off (");
  }
  Serial.print(RoomLightOn);
  Serial.print(",");
  Serial.print(RoomLightOff);
  Serial.println(")");

  // The actual light
  // Light on/off
  if (nHour >= LightOn && nHour< LightOff){
    // Lights on
    analogWrite(LightPin1, LightPMW);
    analogWrite(LightPin2, LightPMW);
    analogWrite(LightPin3, LightPMW);
  }
  else{
    // Lights off
    analogWrite(LightPin1, 255);
    analogWrite(LightPin2, 255);
    analogWrite(LightPin3, 255);
  }

  
  // LCD
  if (nSec > Display2_switch){
    // Display 1
        
    // LCD Date
    lcd.setCursor(0, 0);
    lcd.print(nMonth);
    lcd.print("/");
    lcd.print(nDate);
    lcd.print(" ");
    
    // LCD Time
    lcd.print(nHour);
    lcd.print(":");
    lcd.print(nMin);
    lcd.print("        ");


    // Light on/off
    if (nHour >= LightOn && nHour< LightOff){
      // Light on
      // lcd
      lcd.setCursor(0, 1);
      lcd.print("Light ON (");
      lcd.print(LightOn);
      lcd.print(",");
      lcd.print(LightOff);
      lcd.print(")");
      lcd.print("       ");
    }
    else{
      // Light off
      // lcd
      lcd.setCursor(0, 1);
      lcd.print("Light OFF (");
      lcd.print(LightOn);
      lcd.print(",");
      lcd.print(LightOff);
      lcd.print(") ");
    }
  }
  else{
    // Display 2
    // Room program
    if(nHour >= RoomLightOn && nHour< RoomLightOff){
      lcd.setCursor(0, 0);
      lcd.print("RoomL ON (");
      lcd.print(RoomLightOn);
      lcd.print(",");
      lcd.print(RoomLightOff);
      lcd.print(") ");
    }
    else{
      lcd.setCursor(0, 0);
      lcd.print("RoomL OFF (");
      lcd.print(RoomLightOn);
      lcd.print(",");
      lcd.print(RoomLightOff);
      lcd.print(")");
    }

    // Wipe out second row
    lcd.setCursor(0, 1);
    lcd.print("                ");

  }

  // Adjust backlight to room light status
  if(nHour >= RoomLightOn && nHour< RoomLightOff){
    // Turn backlight on
    lcd.setBacklight(255);
  }
  else{

    // Turn backlight off
    lcd.setBacklight(0);
    /*
    // Turn backlight off, unless the button is being pushed
    if (digitalRead(ButtonPin) == HIGH){
      lcd.setBacklight(255);
      Serial.println("Button pushed.");
    }
    else{
      lcd.setBacklight(0);     
    }
    */

  }

  // Log cycle end time
  Serial.print("Arduino cycle time: ");
  Serial.print(millis() - t0);
  Serial.println(" ms.");
  
  delay(10000);
}
