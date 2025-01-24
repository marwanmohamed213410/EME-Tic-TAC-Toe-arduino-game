#include <LiquidCrystal.h>

// Function Prototypes
void check_button(void);                // Scans the button matrix to mark the current move
void restart(void);                     // Restarts the game by resetting the game state
int winner_or_loser(void);              // Checks for a winner; returns 1 for Player 1, 2 for Player 2, or 0 for no winner
void check_win_conditions(void);        // Checks for a winner or tie and handles the result
void flash_winner(int winner);          // Flashes LEDs for the winning player
bool tie(void);                         // Checks if the game is a tie (no empty spaces)
void flash_tie(void);                   // Flashes all LEDs in case of a tie
void update_leds(void);                 // Updates the LEDs to reflect the current game state
void chooseColorAndStarter(void);       // Handles player color and starter selection
void full_reset(void);                  // Full reset of the game (including player color and starter)
void play_tune_start(void);             // Play a tune when the game starts
void play_tune_win(void);               // Play a tune when someone wins
void play_tune_tie(void);               // Play a tune when there's a tie


const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int button1Pin = 2;
const int button2Pin = 3;
const int resetButtonPin = 4;

int player1Color = 0;
bool player1Starts = true;
bool gameReady = false;


const int buzzerPin = 6;


int row_pb[] = {2, 3, 4};
int col_pb[] = {5, 6, 7};
int row_leds[] = {8, 9, 10};
int col_leds[] = {11, 12, 13, A0, A1, A2};

int gameState[3][3];
bool isPlayer1Turn = true;
bool gameEnded = false;

void setup() {
  lcd.begin(16, 2);
  
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  
  pinMode(buzzerPin, OUTPUT);
  
  lcd.clear();
  lcd.print("Player 1: Color");
  lcd.setCursor(0, 1);
  lcd.print("Blue or Red?");
  
  for (int i = 0; i < 3; i++) {
    pinMode(row_pb[i], INPUT_PULLUP);
    pinMode(row_leds[i], OUTPUT);
  }
  
  for (int i = 0; i < 6; i++) {
    pinMode(i < 3 ? col_pb[i] : col_leds[i - 3], OUTPUT);
  }
}

void loop() {
  if (digitalRead(resetButtonPin) == LOW) {
    full_reset();
    delay(300);
  }
  
  if (!gameReady) {
    chooseColorAndStarter();
  } else {
    if (!gameEnded) {
      check_button();
      update_leds();
      check_win_conditions();
    }
  }
}


void chooseColorAndStarter() {
  static bool player1ChoseColor = false;
  
  if (!player1ChoseColor) {
    if (digitalRead(button1Pin) == LOW) {
      player1Color = (player1Color == 1) ? 2 : 1;
      lcd.clear();
      lcd.print("Player 1: ");
      lcd.print((player1Color == 1) ? "Blue" : "Red");
      delay(300);
    }
    
    if (digitalRead(button2Pin) == LOW) {
      player1ChoseColor = true;
      lcd.clear();
      lcd.print("Player 2: Who");
      lcd.setCursor(0, 1);
      lcd.print("starts?");
      delay(300);
    }
  } else {
    if (digitalRead(button1Pin) == LOW) {
      player1Starts = !player1Starts;
      
      lcd.clear();
      lcd.print((player1Starts ? "Player 1" : "Player 2"));
      lcd.print(" starts");
      delay(300);
    }
  
    if (digitalRead(button2Pin) == LOW) {
      gameReady = true;
      lcd.clear();
      lcd.print("Start Game");
      play_tune_start();
      delay(1000);
      restart();
    }
  }
}

void restart(void) {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      gameState[row][col] = 0;
    }
  }
  isPlayer1Turn = player1Starts;
  gameEnded = false;                
}


void full_reset(void) {
  player1Color = 0;
  player1Starts = true;
  gameReady = false;
  gameEnded = false;
  isPlayer1Turn = true;

  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      gameState[row][col] = 0;
    }
  }
  lcd.clear();
  lcd.print("Player 1: Color");
  lcd.setCursor(0, 1);
  lcd.print("Blue or Red?");
}

void check_win_conditions() {
  int winner = winner_or_loser();

  if (winner != 0) {
    lcd.clear();
    lcd.print("Player ");
    lcd.print(winner);
    lcd.print(" wins!");
    
    flash_winner(winner);
    play_tune_win();
    gameEnded = true;
    delay(2000);
    full_reset();
  } else if (tie()) {

    lcd.clear();
    lcd.print("It's a tie!");
    
    flash_tie();
    play_tune_tie();
    gameEnded = true;
    delay(2000);
    full_reset();
  }
}

void flash_winner(int winner) {
  // Flash the LEDs for the winner
  for (int i = 0; i < 2; i++) {
    for (int row = 0; row < 3; row++) {
      digitalWrite(row_leds[row], HIGH);
      for (int col = 0; col < 3; col++) {
        if (gameState[row][col] == winner) {
          digitalWrite(col_leds[col + (winner == 2 ? 3 : 0)], LOW);
        }
      }
      delay(500);
      digitalWrite(row_leds[row], LOW);
    }
    delay(500);
  }
}

void play_tune_start(void) {
  tone(buzzerPin, 1000);
  delay(500);
  noTone(buzzerPin);
}

void play_tune_win(void) {
  for (int i = 0; i < 3; i++) {
    tone(buzzerPin, 1000);
    delay(300);
    noTone(buzzerPin);
    delay(300);
  }
}

void play_tune_tie(void) {
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 3; j++) {
      tone(buzzerPin, 500 + (j * 200));
      delay(300);
      noTone(buzzerPin);
    }
    delay(300);
  }
}

void update_leds(void) {
  for (int row = 0; row < 3; row++) {
    digitalWrite(row_leds[row], HIGH);
    for (int col = 0; col < 3; col++) {
      if (gameState[row][col] == 1) {
        digitalWrite(col_leds[col], LOW);
      } else if (gameState[row][col] == 2) {
        digitalWrite(col_leds[col + 3], LOW);
      } else {
        digitalWrite(col_leds[col], HIGH);
        digitalWrite(col_leds[col + 3], HIGH);
      }
    }
    delay(1);
    digitalWrite(row_leds[row], LOW);
  }
}

void check_button(void) {
  for (int row = 0; row < 3; row++) {
    digitalWrite(row_pb[row], LOW);
    for (int col = 0; col < 3; col++) {
      if (digitalRead(col_pb[col]) == LOW && gameState[row][col] == 0) {
        gameState[row][col] = isPlayer1Turn ? 1 : 2;
        isPlayer1Turn = !isPlayer1Turn;
        delay(300);
      }
    }
    digitalWrite(row_pb[row], HIGH);
  }
}

int winner_or_loser(void) {
  for (int i = 0; i < 3; i++) {
    if (gameState[i][0] == gameState[i][1] && gameState[i][1] == gameState[i][2] && gameState[i][0] != 0)
      return gameState[i][0];
    if (gameState[0][i] == gameState[1][i] && gameState[1][i] == gameState[2][i] && gameState[0][i] != 0)
      return gameState[0][i];
  }
  if (gameState[0][0] == gameState[1][1] && gameState[1][1] == gameState[2][2] && gameState[0][0] != 0)
    return gameState[0][0];
  if (gameState[0][2] == gameState[1][1] && gameState[1][1] == gameState[2][0] && gameState[0][2] != 0)
    return gameState[0][2];
  return 0;
}

bool tie(void) {
  for (int row = 0; row < 3; row++) {
    for (int col = 0; col < 3; col++) {
      if (gameState[row][col] == 0) return false;
    }
  }
  return true;
}

void flash_tie(void) {
  for (int i = 0; i < 2; i++) {
    for (int row = 0; row < 3; row++) {
      digitalWrite(row_leds[row], HIGH);
      for (int col = 0; col < 6; col++) {
        digitalWrite(col_leds[col], LOW);
      }
      delay(500);
      digitalWrite(row_leds[row], LOW);
    }
    delay(500);
  }
}