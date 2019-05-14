#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_PCF8574 lcd(0x27);  

// Initialize rtc
DS3231 Clock;
// RTC variables
bool Century=false;
bool PM;
bool h12;
byte nMonth, nDate, nHour, nMin, nSec;
float nTemp;

// Schedule varaibles
byte LightOn = 6;
byte LightOff = 14;
byte RoomLightOn = 6;
byte RoomLightOff = 18;

// Pin varaibles
byte LightPin = 3;
byte ButtonPin = 12;
byte LightPMW = 230; // 0 = On, 255 = Off;

// Display variables
byte Display2_switch = 10; // Switch to Display 2 if at the top X seconds of each minute

// Debug variables
float t0;

void setup() {
  // Start the I2C interface
  Wire.begin();
  
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
  pinMode(LightPin, OUTPUT);
  pinMode(ButtonPin, INPUT);
}

void loop() {
  // Log cycle start time
  t0 = millis();
  
  // Get data from RTC
  nMonth = Clock.getMonth(Century);
  nDate = Clock.getDate();
  nHour = Clock.getHour(h12, PM);
  nMin = Clock.getMinute();
  nSec = Clock.getSecond();
  nTemp = Clock.getTemperature();

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
  Serial.print(nSec);
  Serial.print(" ");

  // Serial Temperature
  Serial.println(nTemp,1);

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
  Serial.print("RoomLight (");
  Serial.print(RoomLightOn);
  Serial.print(",");
  Serial.print(RoomLightOff);
  Serial.println(")");

  // Osillator status
  if (Clock.oscillatorCheck()) {
    Serial.println("Oscillator OK");
  } 
  else {
    Serial.println("Oscillator BAD");
  }

  // The actual light
  // Light on/off
  if (nHour >= LightOn && nHour< LightOff){
    analogWrite(LightPin, LightPMW);
  }
  else{
    analogWrite(LightPin, 255);
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
    lcd.print(" ");
  
    // LCD temperature
    lcd.print(nTemp,1);
    lcd.print(" ");

    // Light on/off
    if (nHour >= LightOn && nHour< LightOff){
      // Light on
      // lcd
      lcd.setCursor(0, 1);
      lcd.print("Light ON (");
      lcd.print(LightOn);
      lcd.print(",");
      lcd.print(LightOff);
      lcd.print(") ");
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
    lcd.setCursor(0, 0);
    lcd.print("RoomLight (");
    lcd.print(RoomLightOn);
    lcd.print(",");
    lcd.print(RoomLightOff);
    lcd.print(")");

    // Oscillator status
    lcd.setCursor(0, 1);
    if (Clock.oscillatorCheck()) {
      lcd.print("Oscillator OK   ");
    } 
    else {
      lcd.print("Oscillator BAD  ");
    }
  }
  
  

  // Adjust backlight to room light status
  if(nHour >= RoomLightOn && nHour< RoomLightOff){
    // Turn backlight on
    lcd.setBacklight(255);
  }
  else{
    // Turn backlight off, unless the button is being pushed
    if (digitalRead(ButtonPin) == HIGH){
      lcd.setBacklight(255);
      Serial.println("Button pushed.");
    }
    else{
      lcd.setBacklight(0);     
    }

  }

  // Log cycle end time
  Serial.print("Arduino cycle time: ");
  Serial.print(millis() - t0);
  Serial.println(" ms.");
  
  delay(10000);
}
