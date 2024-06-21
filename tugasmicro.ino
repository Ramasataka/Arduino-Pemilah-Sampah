#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address might be 0x3F or 0x27, 16 characters and 2 lines

Servo servo1;
Servo servo2;  // New servo for button control
const int trigPin = 12;
const int echoPin = 11;
const int buttonPin = 3; // Button pin
const int servoPin2 = 6; // Servo pin for button control

long duration;
int distance = 0;
int potPin = A0; // input pin
int soil = 0;
int fsoil;
bool buttonState = LOW;
bool lastButtonState = LOW;
bool servoPosition = LOW; // LOW means 0 degrees, HIGH means 90 degrees

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Project Arduino");
  lcd.setCursor(0, 1);
  lcd.print("PEMILAH SAMPAH");
  delay(2000); // Delay to show initial message for 2 seconds
  lcd.clear();
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT);
  
  servo1.attach(8);
  servo2.attach(servoPin2); // Attach new servo to pin 6
  servo1.write(90); // Set servo to neutral position
  servo2.write(0); // Initialize new servo to 0 degrees
  
  Serial.println("Setup complete");
}

void moveServoSlowly(Servo &servo, int startPos, int endPos, int stepDelay) {
  if (startPos < endPos) {
    for (int pos = startPos; pos <= endPos; pos++) {
      servo.write(pos);
      delay(stepDelay);
    }
  } else {
    for (int pos = startPos; pos >= endPos; pos--) {
      servo.write(pos);
      delay(stepDelay);
    }
  }
}

void loop() {
  int soil = 0;
  distance = 0;

  // Distance measurement
  for (int i = 0; i < 2; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(7);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance += duration * 0.034 / 2;
    delay(10);
  }
  
  distance = distance / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 15 && distance > 1) {
    delay(1000);

    for (int i = 0; i < 3; i++) {
      soil = analogRead(potPin);
      soil = constrain(soil, 485, 1023);
      fsoil += map(soil, 485, 1023, 100, 0);
      delay(75);
    }

    fsoil = fsoil / 3;
    Serial.print("Soil moisture: ");
    Serial.print(fsoil);
    Serial.println("%");
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("KALKULASI SAMPAH");
    lcd.clear();

    if (fsoil > 3) {
      delay(1000);
      Serial.println("WET - Trash is wet");
      lcd.setCursor(0, 0);
      lcd.print("SAMPAH INI");
      lcd.setCursor(0, 1);
      lcd.print("BASAH");
      moveServoSlowly(servo1, 90, 180, 15); // Move servo1 to 180 degrees slowly
      delay(3000);
    } else {
      delay(1000);
      Serial.println("DRY - Trash is dry");
      lcd.setCursor(0, 0);
      lcd.print("SAMPAH INI");
      lcd.setCursor(0, 1);
      lcd.print("KERING");
      moveServoSlowly(servo1, 90, 0, 15); // Move servo1 to 0 degrees slowly
      delay(3000);
    }

    moveServoSlowly(servo1, servo1.read(), 90, 15); // Reset servo to neutral position slowly
    lcd.setCursor(0, 0);
    lcd.print("Project Arduino");
    lcd.setCursor(0, 1);
    lcd.print("PEMILAH SAMPAH");
  }

  // Button and servo control
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      if (servoPosition == LOW) {
        moveServoSlowly(servo2, servo2.read(), 180, 15); // Move servo2 to 180 degrees slowly
        servoPosition = HIGH;
        Serial.println("Button pressed: Servo moved to 180 degrees");
           lcd.setCursor(0, 1);
            lcd.print("Tempat Sampah Dibuka");
      } else {
        moveServoSlowly(servo2, servo2.read(), 0, 15); // Move servo2 to 0 degrees slowly
        servoPosition = LOW;
        Serial.println("Button pressed: Servo moved to 0 degrees");
        lcd.print("Tempat Sampah Dibuka");
      }
      delay(200); // Debounce delay
    }
  }
  lastButtonState = buttonState;

  distance = 0;
  fsoil = 0;
  delay(1000);
}
