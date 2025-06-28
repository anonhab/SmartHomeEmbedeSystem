#include <Servo.h>
#include <RFID.h>
#include <Keypad.h>
#include "DS1302.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define RST_PIN 9
#define SS_PIN 10

String rfidCard;
static boolean statt=false;
RFID rfid(SS_PIN, RST_PIN);
String card  = "99 11 101 15";
String card2 = "243 122 230 246";
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
int soundPin = A0;
byte rowPins[4] = {6, 7, 8,  };
byte colPins[4] = {3, 4, 5, A1}; // Connect the fourth column pin to A1
int servoPin = A2; // Define the pin for the servo
String password = "7777";
String enteredPassword = ""; // Variable to store entered password
int position = 0;
  int wrongAttempts = 0; // Variable to count the wrong attempts.
int maxAttempts = 1; // Maximum allowed wrong attempts.

Servo myServo;
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4); // MAPPING THE KEYPAD.
LiquidCrystal_I2C lcd(0x27, 16, 2); // Initialize the LCD with the I2C address and dimensions

void setup() {
   myServo.attach(A2);
  SPI.begin();
  rfid.init();
  
  pinMode(2,OUTPUT);
   myServo.write(0);
  lcd.begin(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.setCursor(0, 0);
}

void displayTime() {
  char *timearray = "readTime()";   
  lcd.clear();
  float Vout = analogRead(A3);
  float Temp = (Vout * 5.0 * 100.0) / 1024.0; // Temperature calculation adjusted
  lcd.setCursor(0, 0); 
  lcd.print("Temp: ");
  lcd.print(Temp);
  lcd.setCursor(0, 1); 
  lcd.print("Time:"); 
  lcd.print(timearray); 
}

void loop() {
  int soundValue = analogRead(soundPin);
  Serial.println(soundValue);
  if(statt==true){
  key();
  }
   String card = "99 11 101 15";
   if (wrongAttempts >= maxAttempts) {
        lcd.clear();
        lcd.print("Max attempts!");
        delay(3000);
        lcd.clear();
         wrongAttempts = 0;
      } 
  if (soundValue > 20 && wrongAttempts < maxAttempts && statt==false  ) {
    statt=true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sound Detected");
     lcd.setCursor(0, 1);
    lcd.print("use key or card");
    delay(5000);
    if (keyOrCard()) {
      unlockDoor();
      statt=false;
      digitalWrite(2,HIGH); // Unlock the door if key or card is presented
      delay(5000); // Wait for 5 seconds to ensure door is fully open
      lockDoor(); // Lock the door after delay

    }
     
    else {
      lcd.clear();
      lcd.print("no key or card");
      delay(2000); // Wait for 2 seconds before clearing the LCD
      lcd.clear();
      
     // statt=false;
    }
  }
}
void key(){
  char pressedKey = keypad.getKey();
  if (pressedKey != NO_KEY) {
    //stat=true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter Password:");
    lcd.setCursor(0, 1);
    lcd.print("Pass: ");
    lcd.print(enteredPassword);

     if (pressedKey == '*' || pressedKey == '#') {
      enteredPassword = ""; // Clear entered password if * or # is pressed
    } else {
      enteredPassword += pressedKey; // Append pressed key to entered password
    }
    

    if(enteredPassword.length() == 4) {
      if (enteredPassword == password) {
      
        lcd.clear();
        lcd.print("Correct password!");
        lcd.setCursor(0, 1);
        lcd.print("Unlocking....");
        delay(2000);
        statt=false;
        digitalWrite(2,HIGH);
        unlockDoor();
        //stat=false;
        delay(2000);
        lockDoor() ;
        lcd.clear();
        enteredPassword = ""; // Clear entered password after correct entry
      } else {
         wrongAttempts++;
        lcd.clear();
        lcd.print("Incorrect pass");
        //stat=false;
        statt=false;
      
        delay(2000);
        lcd.clear();
        enteredPassword = ""; 
      }

      if (wrongAttempts >= maxAttempts) {
      lcd.clear();
      lcd.print("Max attempts!");
      delay(3000);
      lcd.clear();
      wrongAttempts = 0;
      }
    }

    if (position > 4) {
      enteredPassword = ""; // Clear entered password if more than 4 digits are entered
      lcd.setCursor(0, 1);
      lcd.print("                "); // Clear the LCD
      lcd.setCursor(0, 1);
      lcd.print(enteredPassword); // Display empty password on LCD
    }
  }
}
bool keyOrCard() {
  
  if (rfid.isCard()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card detecting...");
    if (rfid.readCardSerial()) {
      rfidCard = String(rfid.serNum[0]) + " " + String(rfid.serNum[1]) + " " + String(rfid.serNum[2]) + " " + String(rfid.serNum[3]);

      if (rfidCard == card) {
        lcd.setCursor(0, 1);
        lcd.print("Unlocking....");
        delay(2000);
        return true; // Return true if valid card is detected
      }
     
      if (rfidCard ==  card2) {
        
        lcd.setCursor(0, 1);
        lcd.print("invalid card");
        wrongAttempts++;
        delay(2000);
        statt=false;
      }

      
    }
    rfid.halt();
  }

  return false; 
}

void unlockDoor() {
  lcd.setCursor(0, 1);
  lcd.print("Unlocking...");
  myServo.write(180); // Rotate the servo to unlock the door
}

void lockDoor() {
  delay(5000); // Wait for 5 seconds to ensure door is fully open
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Locking...");
  myServo.write(0); // Return the servo to the locked position
}
