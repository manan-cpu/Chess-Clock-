
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define pins for LEDs and buzzers
#define LED1_PIN 12    // LED for White timer active
#define LED2_PIN 13    // LED for Black timer active
#define BUZZER1_PIN 10 // Buzzer for White
#define BUZZER2_PIN 11 // Buzzer for Black

// Define pins for buttons
#define White 4        // White button for Player 1
#define Black 3        // Black button for Player 2 / Start game
#define RESET 5        // Reset button
#define L 9            // Select White timer
#define R 8            // Select Black timer
#define U 7            // Increase time
#define D 6            // Decrease time

// Initialize the LCD with I2C address (e.g., 0x27) and size 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables for timers (in milliseconds)
unsigned long timerWhite = 600000; // 10 minutes for White
unsigned long timerBlack = 600000; // 10 minutes for Black
unsigned long previousMillis = 0;
bool timerWhiteActive = false;     // White timer status
bool timerBlackActive = false;     // Black timer status (initially inactive)
bool gameStarted = false;          // Tracks if the game has started
bool selectWhite = false;          // Tracks if White timer is selected for adjustment
bool selectBlack = false;          // Tracks if Black timer is selected for adjustment
bool gameEnded = false;            // Tracks if the game has ended

// Turn counters
int turnCountWhite = 0;
int turnCountBlack = 0;

// Time interval for speeding up timer for testing
unsigned long timeSpeedFactor = 50; // 50ms per loop counts as 1 second

void setup() {
  // Set up LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("White 0 Black 0");

  // Set up LED and buzzer pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER1_PIN, OUTPUT);
  pinMode(BUZZER2_PIN, OUTPUT);

  // Set up button pins with external pull-down resistors
  pinMode(White, INPUT);
  pinMode(Black, INPUT);
  pinMode(RESET, INPUT);
  pinMode(L, INPUT);
  pinMode(R, INPUT);
  pinMode(U, INPUT);
  pinMode(D, INPUT);

  // Initial state of LEDs
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  // Initialize Serial Monitor for debugging
  Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if the reset button is pressed
  if (digitalRead(RESET) == HIGH) {
    resetTimers();
    delay(300); // Debounce delay
  }

  // If the game has ended, do nothing further in the loop
  if (gameEnded) {
    return;
  }

  // Start the game with the black button, activating white timer
  if (digitalRead(Black) == HIGH && !gameStarted) {
    gameStarted = true;
    timerWhiteActive = true;
    timerBlackActive = false;
    previousMillis = currentMillis;
    digitalWrite(LED1_PIN, HIGH);  // White timer LED on
    delay(300); // Debounce delay
  }

  // Toggle selection of White timer with L button
  if (digitalRead(L) == HIGH && !gameStarted) {
    selectWhite = !selectWhite;  // Toggle White selection
    selectBlack = false;         // Ensure Black is deselected
    digitalWrite(LED1_PIN, selectWhite ? HIGH : LOW); // Update LED1 based on selection
    delay(300); // Debounce delay
  }

  // Toggle selection of Black timer with R button
  if (digitalRead(R) == HIGH && !gameStarted) {
    selectBlack = !selectBlack;  // Toggle Black selection
    selectWhite = false;         // Ensure White is deselected
    digitalWrite(LED2_PIN, selectBlack ? HIGH : LOW); // Update LED2 based on selection
    delay(300); // Debounce delay
  }

  // Increase time with U button if a timer is selected
  if (digitalRead(U) == HIGH && !gameStarted) {
    if (selectWhite) timerWhite += 60000;  // Increase White timer by 1 minute
    if (selectBlack) timerBlack += 60000;  // Increase Black timer by 1 minute
    delay(300); // Debounce delay
  }

  // Decrease time with D button if a timer is selected
  if (digitalRead(D) == HIGH && !gameStarted) {
    if (selectWhite && timerWhite > 60000) timerWhite -= 60000; // Decrease White timer
    if (selectBlack && timerBlack > 60000) timerBlack -= 60000; // Decrease Black timer
    delay(300); // Debounce delay
  }

  // Switch between white and black timers when buttons are pressed
  if (digitalRead(White) == HIGH && timerWhiteActive && gameStarted) {
    timerWhiteActive = false;
    timerBlackActive = true;
    previousMillis = currentMillis;
    digitalWrite(LED1_PIN, LOW);   // White LED off
    digitalWrite(LED2_PIN, HIGH);  // Black LED on
    turnCountWhite++;              // Increment White turn count
    delay(300); // Debounce delay
  }

  if (digitalRead(Black) == HIGH && timerBlackActive && gameStarted) {
    timerBlackActive = false;
    timerWhiteActive = true;
    previousMillis = currentMillis;
    digitalWrite(LED1_PIN, HIGH);  // White LED on
    digitalWrite(LED2_PIN, LOW);   // Black LED off
    turnCountBlack++;              // Increment Black turn count
    delay(300); // Debounce delay
  }

  // Update timers based on active state
  if (currentMillis - previousMillis >= timeSpeedFactor) {
    previousMillis = currentMillis;

    // Decrease white timer if active
    if (timerWhiteActive && timerWhite > 0) {
      timerWhite -= 1000; // Decrease by 1 second
    }
    // Decrease black timer if active
    if (timerBlackActive && timerBlack > 0) {
      timerBlack -= 1000; // Decrease by 1 second
    }
  }

  // Check if any timer has reached zero to end the game
  if (timerWhite == 0) {
    endGame("Black WINNER!!!");
  } else if (timerBlack == 0) {
    endGame("White WINNER!!!");
  }

  // Display both timers and turn counts on the LCD
  displayTimers();
}

// Function to end the game and display the winner
void endGame(const char* winnerMessage) {
  // Turn off LEDs and buzzers
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);
  digitalWrite(BUZZER1_PIN, LOW);
  digitalWrite(BUZZER2_PIN, LOW);

  // Display winner message on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(winnerMessage);

  gameEnded = true; // Mark game as ended
}

// Function to reset both timers and turn counts
void resetTimers() {
  timerWhite = 600000;        // Reset to 10 minutes
  timerBlack = 600000;
  timerWhiteActive = false;   // Reset both timers to inactive
  timerBlackActive = false;
  gameStarted = false;
  selectWhite = false;
  selectBlack = false;
  gameEnded = false;
  turnCountWhite = 0;         // Reset turn counts
  turnCountBlack = 0;
  digitalWrite(LED1_PIN, LOW); // Turn off both LEDs
  digitalWrite(LED2_PIN, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("White 0 Black 0");
}

// Function to display both timers and turn counts on the LCD
void displayTimers() {
  int minutesWhite = timerWhite / 60000;
  int secondsWhite = (timerWhite % 60000) / 1000;
  int minutesBlack = timerBlack / 60000;
  int secondsBlack = (timerBlack % 60000) / 1000;

  lcd.setCursor(0, 0);
  lcd.print("White ");
  lcd.print(turnCountWhite);
  lcd.print(" Black ");
  lcd.print(turnCountBlack);

  lcd.setCursor(0, 1);  // Set cursor to the second line
  if (minutesWhite < 10) lcd.print("0");
  lcd.print(minutesWhite);
  lcd.print(":");
  if (secondsWhite < 10) lcd.print("0");
  lcd.print(secondsWhite);
  lcd.print("  ");
  if (minutesBlack < 10) lcd.print("0");
  lcd.print(minutesBlack);
  lcd.print(":");
  if (secondsBlack < 10) lcd.print("0");
  lcd.print(secondsBlack);
}


