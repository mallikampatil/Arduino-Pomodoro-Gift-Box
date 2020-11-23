#include <Servo.h>
#include <Wire.h>
/* code for LCD and 3 Buttons */

// include the library code:
#include <LiquidCrystal.h>

// initialize the numbers of the digital pins the LCD pins are connected to
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

/* Servo */ 
Servo projectServo; 
const int pServo = 9; 
const int pResistor = A0; 
const int buzzer_pin = 10; 

/* for buzzer tune */

struct MusicStruct {
  int A = 550;
  int As = 582;
  int B = 617;
  int C = 654;
  int Cs = 693;
  int D = 734;
  int Ds = 777;
  int E = 824;
  int F = 873;
  int Fs = 925;
  int G = 980;
  int Gs = 1003;
  int A2 = 1100;
  int A2s = 1165;
  int B2 = 1234;
  int C3 = 1308;
  int C3s = 1385;
  int D3 = 1555;
}Music;

struct LengthStruct {
  float half = 0.5;
  float one = 1.0;
  float one_half = 1.5;
  float two = 2.0;
  float two_half = 2.5;
}Length;

int tempo = 400;

const int upBtn = 2;     // the number of the 'up' pushbutton pin
const int downBtn = 7;     // the number of the 'down' pushbutton pin
const int enterBtn = 8;     // the number of the 'enter' pushbutton pin

const int ledPin =  13;      // the number of the LED pin

// variables for reading the pushbutton status
int upBtnStatus = 0;         
int downBtnStatus = 0; 
int enterBtnStatus = 0; 

// variable to store current option displayed on screen
int currOption = 0;

// the option user chose
int chosenOption = 0;

int minutes = -1;
int seconds = -1;

int pResisVal = 0; // to store value of photoresistor

bool timerUp = false; // to be used in countDown function

unsigned long previousMillis = 0;    // stores the last time button was updated

const long interval = 1000;          // interval for checking button state

void setup() {

  Wire.begin(0x8); //I2C Slave on addr 0x8        
  Wire.onReceive(receiveEvent);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  Serial.begin(9600);

  projectServo.attach(pServo); 
  pinMode(pResistor, INPUT); 
  pinMode(buzzer_pin, OUTPUT); 
  
  // initialize the pushbutton pins as an input:
  pinMode(upBtn, INPUT);
  pinMode(downBtn, INPUT);
  pinMode(enterBtn, INPUT);

  pinMode(ledPin, OUTPUT);

  // turn LED off:
  digitalWrite(ledPin, LOW);

  openBox();
  lcd.clear();

  // set cursor position to first row
  lcd.setCursor(0,0);

  lcd.print("Pick timer type:");
  Serial.println("Pick timer type:");

  // set cursor position to second row
  lcd.setCursor(0,1);

  lcd.print("Option 1");
  Serial.println("Option 1");
  currOption = 1;
  
  // call function to pick timer setting:
  chosenOption = chooseTimerOption();
  Serial.print("ChosenOption: ");
  Serial.println(chosenOption);

  if (chosenOption == 1) {
    pomodoroCycle(1, 1, 1);
  }
  else if (chosenOption == 2) {
    pomodoroCycle(30, 5, 30);
  }
  else if (chosenOption == 3) {
    pomodoroCycle(50, 10, 30);
  }
}



void receiveEvent(int event) { //Waiting for master. 
  char data = ' ';
  while (Wire.available()) { 
    data = Wire.read(); 
  }
  int degree = (c - '0')*10;  //0x39 => 9*10 => 90 (int)
  projectServo.write(degree);
}

void loop() {

  
  
}

int chooseTimerOption() {

  while (chosenOption == 0) {

      
      // set cursor position to first row
      lcd.setCursor(0,0);
    
      lcd.print("Pick timer type:");
      Serial.println("Pick timer type:");
    
      // set cursor position to second row
      lcd.setCursor(0,1);
    
      //check button status
      upBtnStatus = digitalRead(upBtn);
      Serial.print("upBtnStatus: ");
      Serial.println(upBtnStatus);
      downBtnStatus = digitalRead(downBtn);
      Serial.print("downBtnStatus: ");
      Serial.println(downBtnStatus);
      enterBtnStatus = digitalRead(enterBtn);
      Serial.print("enterBtnStatus: ");
      Serial.println(enterBtnStatus);
    
      if (enterBtnStatus == LOW) { // enter was NOT pressed
        
        if (currOption == 1) {
          if (upBtnStatus == HIGH) {
            lcd.print("Option 3");
            Serial.println("Option 3");
            currOption = 3;
          }
          else if (downBtnStatus == HIGH) {
            lcd.print("Option 2");
            Serial.println("Option 2");
            currOption = 2;
          }
        }
    
        else if (currOption == 2) {
          if (upBtnStatus == HIGH) {
            lcd.print("Option 1");
            Serial.println("Option 1");
            currOption = 1;
          }
          else if (downBtnStatus == HIGH) {
            lcd.print("Option 3");
            Serial.println("Option 3");
            currOption = 3;
          }
        }
    
        else if (currOption == 3) {
          if (upBtnStatus == HIGH) {
            lcd.print("Option 2");
            Serial.println("Option 2");
            currOption = 2;
          }
          else if (downBtnStatus == HIGH) {
            lcd.print("Option 1");
            Serial.println("Option 1");
            currOption = 1;
          }
        }
        
      }
      else { // enter was pressed
    
        // set cursor position to first row
        lcd.setCursor(0,0);
    
        lcd.print("Chosen option:");
        Serial.println("Chosen option:");
      
        // set cursor position to second row
        lcd.setCursor(0,1);
      
        //display chosen option and return it:
        if (currOption == 1) {
          lcd.print("Option 1");
          Serial.println("Option 1");
          chosenOption = 1;
          return 1;
        }
        else if (currOption == 2) {
          lcd.print("Option 2");
          Serial.println("Option 2");
          chosenOption = 2;
          return 2;
        }
        else if (currOption == 3) {
          lcd.print("Option 3");
          Serial.println("Option 3");
          chosenOption = 3;
          return 3;
        }
        lcd.clear(); 
      }

      delay(500);
        
  }
  
}


// full Pomodoro cycle
void pomodoroCycle(int workTime, int shortBreak, int longBreak) {

    int cycleCount = 1;
    
    if (!boxClosed()) // If box is not closed 
    {
      closeBox();
    }
    
    while (cycleCount <= 4)
    {
        lcd.clear();
        
        countDown(workTime, 0, "Work Cycle");  // timer for work time

        if (timerUp) {
            timerUp = false;    // reset timerUp variable for next run
            digitalWrite(ledPin, HIGH); // turn LED on
            playTune();
            openBox(); 
        }

        if (cycleCount < 4) {
            countDown(shortBreak, 0, "Short Break");  // timer for short break time
            
            if (timerUp) {
              timerUp = false;    // reset timerUp variable for next run
              playTune();
            }
        }
        else //done with work and short break cycles
          break;

        cycleCount++;
        digitalWrite(ledPin, LOW);  // turn LED off
        closeBox();
    }

    countDown(longBreak, 0, "Long Break");  // timer for long break time
            
    if (timerUp) {
      timerUp = false;    // reset timerUp variable for next run

      lcd.clear();
      lcd.print("CYCLE DONE!");
      
      Serial.println("CYCLE DONE!");

      // set cursor position to second row
      lcd.setCursor(0,1);
      
      lcd.print("PRESS RESET...");
      
      digitalWrite(ledPin, LOW);  // turn LED off
      
      playTune();
      
    }

    // full cycle done

}


bool boxClosed() {
    pResisVal = analogRead(pResistor);   //get value of photoresistor
    if (pResisVal < 20)                       //If value represents “dark”
      return true;
    else
      return false; 
}

void closeBox() {
  projectServo.write(110);
}

void openBox() {
  projectServo.write(-90);
}

bool countDown(int mins, int secs, String infoMessage) {

  minutes = mins;
  seconds = secs;

  displayTime(infoMessage);

  while (minutes != 0 || seconds != 0) {

      unsigned long currentMillis = millis();  // for time so we can use elapsed time 
                                           // instead of delay()

      // time elapsed between the current time and the previous time 
      // so we can know if button status changed
      unsigned long timeSpent = currentMillis - previousMillis;
    
      if (timeSpent >= interval) {
        // save the last time 
        previousMillis = currentMillis;
      }
    
      if (timeSpent == 1000) {
          
          if ( minutes == 0 && seconds == 1) {
              seconds = 0;
          }
          else if (seconds == 0) {
            seconds = 59;
            minutes--;
          }
          else {  
            seconds--;
          } 
    
          if (minutes == 0 && seconds == 0) {
            lcd.clear();
            lcd.print("TIME UPPPPP!");
            
            Serial.println("TIME UPPPPP!");
      
            timerUp = true;
            return timerUp;
          }
          else
            displayTime(infoMessage);
      }
    
  }
   
}

String buildTimeString(int minutes, int seconds) {

  String timeString = "";

  //add minutes to string
  String minutesString = String(minutes);
  if (minutesString.length() < 2)
    timeString += (String(0) + minutesString + ":");
  else
    timeString += (minutesString + ":");

  //add seconds to string
  String secondsString = String(seconds);
  if (secondsString.length() < 2)
    timeString += (String(0) + secondsString);
  else
    timeString += secondsString;

  return timeString;
    
}

void displayTime(String infoMessage){
  
  lcd.clear();

  //String timeStr = buildTimeString(minute(), second());
  String timeStr = buildTimeString(minutes, seconds);
  
  Serial.println(timeStr);
  Serial.println();

  // set cursor position to first row
  lcd.setCursor(0,0);
  //print info message on LCD
  lcd.print(infoMessage);

  // set cursor position to second row
  lcd.setCursor(0,1);
  //print Time string on LCD
  lcd.print(timeStr);


}

void setTone(int pin, int note, int duration) {
  tone(pin, note, duration);
  delay(duration);
  noTone(pin);
}

void playTune() {
  setTone(buzzer_pin, Music.B, tempo * Length.one);
  setTone(buzzer_pin, Music.E, tempo * Length.one_half);
  setTone(buzzer_pin, Music.G, tempo * Length.half);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.E, tempo * Length.two);
  setTone(buzzer_pin, Music.B2, tempo * Length.one);
  setTone(buzzer_pin, Music.A2, tempo * Length.two_half);
  setTone(buzzer_pin, Music.Fs, tempo * Length.two_half);
  
  setTone(buzzer_pin, Music.E, tempo * Length.one_half);
  setTone(buzzer_pin, Music.G, tempo * Length.half);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.Ds, tempo * Length.two);
  setTone(buzzer_pin, Music.F, tempo * Length.one);
  setTone(buzzer_pin, Music.B, tempo * Length.two_half);
}
