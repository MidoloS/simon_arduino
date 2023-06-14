#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_LiquidCrystal.h>



#define END_OF_COLOR -1
#define LCD_COLUMNS 16
#define LCD_ROWS 2
#define LCD_ADDRESS 0x20
#define KEYPAD_NUM_COLUMNS 4
#define KEYPAD_NUM_ROWS 4

// Keypad config
char keymap[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLUMNS] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte ROW_PINS[KEYPAD_NUM_ROWS] = {9,8,7,6};
byte COL_PINS[KEYPAD_NUM_COLUMNS]= {5,4,3,2};

// Valid options
char VALID_RESET_GAME_OPTION[1] = {'#'};
char VALID_PLAYER_OPTION[2] = {'1', '2'};
char VALID_DIFFICULTY_OPTION[3] = {'A', 'B', 'C'};

// Game variables
int currentPlayer = 0;
int color_secuence[99];
int totalPlayers;

// Difficult variables
int maxColors = 5;
int player_tries[2];

Adafruit_LiquidCrystal lcd(0);
Keypad myKeypad= Keypad(makeKeymap(keymap), ROW_PINS, COL_PINS, KEYPAD_NUM_ROWS, KEYPAD_NUM_COLUMNS);

void display(String line1 = "", String line2 = "") {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
  lcd.setCursor(0,0);
}

int nroES = 0;
void nextLevel() {
  color_secuence[nroES] = random(maxColors);
  nroES++;
  color_secuence[nroES] = -1;
  sendColors();
}

void displayLevel() {
display(String("J: J") + String(currentPlayer == 0 ? 1 : 2) + " NroES: " + String(nroES), "Reintentos: " + String(player_tries[currentPlayer]));
}

char keypadInput() {
  char keypressed = myKeypad.getKey();
  while (keypressed == NO_KEY) {
    keypressed = myKeypad.getKey();

  }
  return keypressed;
}

char askFor(char* validChars, int arraySize) {
  char keypressed = keypadInput();
  Serial.print("key: ");
  Serial.println(keypressed);
  Serial.print("arraySize: ");
  Serial.println(strlen(validChars));
  bool isValid = false;

  while (!isValid) {
    for (int i = 0; i < arraySize; i++) {
      Serial.print("valid: ");
  		Serial.println(validChars[i]);
      if (keypressed == validChars[i]) {
        isValid = true;
        break;
      }
    }
    if (!isValid) {
      keypressed = keypadInput();
    }
  }
  return keypressed;
}

void setup() {
  	randomSeed(analogRead(0));
  	Serial.begin(9600);
  	Wire.begin();
   	lcd.begin(LCD_COLUMNS, LCD_ROWS);
}

void sendColors() {
  int i = 0;
  while(color_secuence[i] != -1) {
    Wire.beginTransmission(8);
    Wire.write(9);
    Wire.endTransmission(); 
    Wire.beginTransmission(8);
    Wire.write(color_secuence[i]);
    delay(800);
    Wire.endTransmission();
    Wire.beginTransmission(8);
    delay(1200);
    Wire.write(9);
    Wire.endTransmission(); 
    i++;
  }
  Wire.endTransmission(); 
}

void difficultConfig(char dif) {
  switch (dif) {
    case 'A':
    	maxColors = 3;
    	player_tries[0] = 3;
        player_tries[1] = 3;
    break;
    case 'B':
    	maxColors = 4;
    	player_tries[0] = 2;
        player_tries[1] = 2;
    break;
    case 'C':
    	maxColors = 5;
    	player_tries[0] = 1;
        player_tries[1] = 1;
    break;
  }
}

void playerConfig(char p) {
  switch (p) {
    case '1':
    	totalPlayers = 1;
    break;
    case '2':
    	totalPlayers = 2;
    break;
  }
}

int askForColor() {
  int btn = -1;
  int button = 1;
  while(!(btn >= 0 && btn <= 4)) {
  	Wire.requestFrom(8, 1);
  	while (Wire.available()) {
      	btn = Wire.read();
    }
    
  }
  
  while(button != 9) {
      Wire.requestFrom(8, 1);
      while (Wire.available()) {
          button = Wire.read();
        	Serial.print("button: ");
      Serial.println(button);
      }
    }
  Wire.beginTransmission(8);
  Wire.write(9);
  Wire.endTransmission(); 
    
    return btn;
}

bool checkColor() {
  int btn = -1;
  for (int i = 0; i < nroES; i++) {
    btn = askForColor();
    Serial.print("btn: ");
    Serial.print(btn);
    Serial.print("color_secuence: ");
    Serial.print(color_secuence[i]);
    Serial.println("");
    if (color_secuence[i] != btn) {
      return false;
    }
    delay(500);
  }
  return true;
}

void loop() {
  char finished;
  char continue_key = '#';
  char players;
  char difficult;
  char keypressed;
  
  
  while(continue_key == '#') {
    finished = false;
    continue_key = '-';
    
    display("Nro Jugadores", "1.Uno - 2.Dos");
  	players = askFor(VALID_PLAYER_OPTION, 2);
    playerConfig(players);
  
  	display("Nivel: A.Bajo", "B.Medio C.Alto");
  	difficult = askFor(VALID_DIFFICULTY_OPTION, 3);
    difficultConfig(difficult);
    
    while (!finished) {
      nextLevel();
      Serial.print("player: ");
      Serial.println(currentPlayer);
      displayLevel();
      bool correct = checkColor();
      
      if (!correct) {
        player_tries[currentPlayer]--;
      }
      
      if (player_tries[currentPlayer] <= 0) {
        if (totalPlayers == 1) {
          display("FIN DEL JUEGO!", "Puntaje: " + String(nroES - 1));
        } else {
          display("GANADOR J" + String(currentPlayer == 0 ? 2 : 1) + "!", "Puntaje: " + String(nroES - 1));
        }
        finished = true;
      }
          if (totalPlayers == 2) {
      currentPlayer = currentPlayer == 0 ? 1 : 0;
    }
    }

    // reset game variables
    currentPlayer = 0;
    nroES = 0;
    for (int i = 0; i < 99; i++) {
      color_secuence[i] = 9;
    }
   	continue_key = askFor(VALID_RESET_GAME_OPTION, 1);
  }
  
  
  
}