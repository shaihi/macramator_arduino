#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* setup related to the motor of the knife that cuts */
const int ENA_PIN = 9; // the Arduino pin connected to the EN1 pin L298N
const int IN1_PIN = 6; // the Arduino pin connected to the IN1 pin L298N
const int IN2_PIN = 5; // the Arduino pin connected to the IN2 pin L298N

/* pin of the rolling motor */
const int motorPin = 9; // Connect the white wire to this pin

// Initialize the I2C LCD.
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Button pin assignments.
const int increaseButton = 2;
const int decreaseButton = 3;
const int confirmButton = 4;
const int cancelButton = 5;

int piecesToCut = 1;
int pieceLength = 5;
bool selectingPieces = true;



void displayPiecesQuestion() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pieces to cut?");
  updateDisplay();
}

void displayLengthQuestion() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Length (CM)?");
  updateDisplay();
}

void updateDisplay() {
  lcd.setCursor(0, 1);
  lcd.print("                "); // Clear the second line.
  lcd.setCursor(0, 1);
  if (selectingPieces) {
    lcd.print(piecesToCut);
  } else {
    lcd.print(pieceLength);
  }
}

void performCut(){
  // extend the actuator
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, HIGH);

  delay(3000); // // Run for 3 second. motor stops automatically when fully extended
  
  // Run the motor in reverse
  digitalWrite(ENA_PIN, HIGH);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  delay(3000); // Run for 3 second. motor stops automatically when fully retracted
}

void runRolling(int length){
  analogWrite(motorPin, 100);
  delay(1000*length); // Run for 1 second times the length we want (1 second is 1 cm)
  
  // Stop the motor
  analogWrite(motorPin, 0)
}

void beginCut(int pieces, int length) {
  for (int i=0; i<pieces;i++){
    runRolling(length);
    performCut();
  }
}



void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT); 

     // Set the motor control pin as output
  pinMode(motorPin, OUTPUT);

  // Initialize the LCD and button pins.
  lcd.begin();
  pinMode(increaseButton, INPUT_PULLUP);
  pinMode(decreaseButton, INPUT_PULLUP);
  pinMode(confirmButton, INPUT_PULLUP);
  pinMode(cancelButton, INPUT_PULLUP);

  displayPiecesQuestion();

}

void loop() {
  if (digitalRead(increaseButton) == LOW) {
    if (selectingPieces) {
      piecesToCut++;
    } else {
      pieceLength += 5;
    }
    updateDisplay();
    delay(200); // Debounce delay.
  }

  if (digitalRead(decreaseButton) == LOW) {
    if (selectingPieces && piecesToCut > 1) {
      piecesToCut--;
    } else if (!selectingPieces && pieceLength > 5) {
      pieceLength -= 5;
    }
    updateDisplay();
    delay(200); // Debounce delay.
  }

  if (digitalRead(confirmButton) == LOW) {
    if (selectingPieces) {
      selectingPieces = false;
      displayLengthQuestion();
    } else {
      beginCut(piecesToCut, pieceLength);
    }
    delay(200); // Debounce delay.
  }

  if (digitalRead(cancelButton) == LOW) {
    selectingPieces = true;
    displayPiecesQuestion();
    delay(200); // Debounce delay.
  }
}
