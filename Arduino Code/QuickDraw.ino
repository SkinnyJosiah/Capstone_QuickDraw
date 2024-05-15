#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

enum JoystickDirection {
  JOYSTICK_NONE,
  JOYSTICK_UP,
  JOYSTICK_DOWN,
  JOYSTICK_LEFT,
  JOYSTICK_RIGHT,
  JOYSTICK_SELECT
};

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// Pin Definitions
#define SD_CS    4   // Chip select line for SD card
#define TFT_CS   10  // Chip select line for TFT display
#define TFT_DC    8  // Data/command line for TFT
#define TFT_RST  -1  // Reset line for TFT (or connect to +5V)
#define JOYSTICK A0  // Analog pin for joystick input
#define BUTTON_P1 2  // Digital pin for Player 1 button
#define BUTTON_P2 6  // Digital pin for Player 2 button
#define RGB_LED_P1_RED    3  // Digital pin for Player 1 RGB LED (Red)
#define RGB_LED_P1_GREEN  4  // Digital pin for Player 1 RGB LED (Green)
#define RGB_LED_P1_BLUE   5  // Digital pin for Player 1 RGB LED (Blue)
#define RGB_LED_P2_RED    7  // Digital pin for Player 2 RGB LED (Red)
#define RGB_LED_P2_GREEN  8  // Digital pin for Player 2 RGB LED (Green)
#define RGB_LED_P2_BLUE   9  // Digital pin for Player 2 RGB LED (Blue)
#define BUFFPIXEL 30

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Current menu state
enum MenuState {
  MENU_START,
  MENU_OPTIONS_P1,
  MENU_OPTIONS_P2,
  MENU_GAME
};

// Define a new state to represent the "waiting for select" state after displaying options
enum OptionsState {
  OPTIONS_DISPLAY,
  OPTIONS_WAIT_SELECT
};

OptionsState optionsState = OPTIONS_DISPLAY;


MenuState currentMenu = MENU_START;

// Player lives
int player1Lives = 3;
int player2Lives = 3;

// RGB LED Colors
enum LedColor {
  RED,
  BLUE,
  GREEN,
  PURPLE,
  YELLOW
};

LedColor player1Color = RED;
LedColor player2Color = BLUE;

void setup(void) {
  Serial.begin(9600);
  
  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Initialize TFT display
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab

  // Initialize button pins
  pinMode(BUTTON_P1, INPUT_PULLUP);
  pinMode(BUTTON_P2, INPUT_PULLUP);

  // Initialize RGB LED pins
  pinMode(RGB_LED_P1_RED, OUTPUT);
  pinMode(RGB_LED_P1_GREEN, OUTPUT);
  pinMode(RGB_LED_P1_BLUE, OUTPUT);
  pinMode(RGB_LED_P2_RED, OUTPUT);
  pinMode(RGB_LED_P2_GREEN, OUTPUT);
  pinMode(RGB_LED_P2_BLUE, OUTPUT);

  // Ensure joystick is in neutral state
  while (readJoystick() != JOYSTICK_NONE) {
    delay(100); // Wait for joystick to be released
  }

  // Display initial menu
  displayStartMenu();
}

// Function to read joystick value
JoystickDirection readJoystick(void) {
  float a = analogRead(3);
  a *= 5.0;
  a /= 1024.0;
  
  if (a < 0.2) return JOYSTICK_DOWN;
  if (a < 1.0) return JOYSTICK_RIGHT;
  if (a < 1.5) return JOYSTICK_SELECT;
  if (a < 2.0) return JOYSTICK_UP;
  if (a < 3.2) return JOYSTICK_LEFT;
  return JOYSTICK_NONE;
}


unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; // milliseconds

void loop() {
  JoystickDirection j = readJoystick();
  unsigned long currentTime = millis();
  
  if (currentTime - lastDebounceTime > debounceDelay) {
    switch (currentMenu) {
      case MENU_START:
        handleStartMenu(j);
        break;
      case MENU_OPTIONS_P1:
      case MENU_OPTIONS_P2:
        handleOptionsMenu(j);
        break;
      case MENU_GAME:
        handleGameLogic();
        break;
    }
    lastDebounceTime = currentTime;
  }
}

void handleStartMenu(JoystickDirection j) {
  switch (j) {
    case JOYSTICK_UP:
      // No action needed for up in start menu
      break;
    case JOYSTICK_DOWN:
      currentMenu = MENU_OPTIONS_P1;
      displayOptionsMenu();
      delay(200);
      break;
    case JOYSTICK_SELECT:
      currentMenu = MENU_GAME;
      startGame();
      delay(200);
      break;
    default:
      break;
  }
}

void handleOptionsMenu(JoystickDirection j) {
  switch (j) {
    case JOYSTICK_UP:
      if (currentMenu == MENU_OPTIONS_P2) {
        currentMenu = MENU_OPTIONS_P1;
        displayOptionsMenu();
        delay(200);
      }
      break;
    case JOYSTICK_DOWN:
      if (currentMenu == MENU_OPTIONS_P1) {
        currentMenu = MENU_OPTIONS_P2;
        displayOptionsMenu();
        delay(200);
      }
      break;
    case JOYSTICK_LEFT:
      currentMenu = MENU_START;
      displayStartMenu();
      delay(200);
      break;
    case JOYSTICK_SELECT:
      // Only transition to the player-specific options menu when select button is pressed
      if (currentMenu == MENU_OPTIONS_P1 || currentMenu == MENU_OPTIONS_P2) {
        currentMenu = MENU_GAME;
        startGame();
        delay(200);
      }
      break;
    default:
      break;
  }
}

void startGame() {
  displayLives(); // Display initial lives
  delay(3000);    // Delay to show initial lives screen for 3 seconds
  displayStandoff(); // Display standoff screen
  delay(3000);    // Delay to show standoff screen for 3 seconds before starting the game logic
  // Start game logic here
}

void displayStandoff() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("STNDOF.BMP", 0, 0); // Display standoffQD.bmp
}

void resetGame() {
  // Placeholder for resetting the game state
  // This function should reset all game-related variables and prepare for a new game
  
  // Reset player lives to their initial values
  player1Lives = 3;
  player2Lives = 3;

  // Clear any screen displays or visual effects
  clearScreen();
  
  // Display the start menu to allow players to start a new game
  currentMenu = MENU_START;
  displayStartMenu();
}

void checkWinConditions() {
  // Placeholder for checking win conditions
  // This function should check if any player has lost all their lives
  // If so, it should display the appropriate win screen and reset the game

  if (player1Lives <= 0) {
    // Display player 2 win screen
    displayWinScreen("P2WinENDQD.BMP");
    delay(5000); // Delay to display win screen (placeholder)
    resetGame(); // Placeholder function to reset the game
  } else if (player2Lives <= 0) {
    // Display player 1 win screen
    displayWinScreen("P1WinENDQD.BMP");
    delay(5000); // Delay to display win screen (placeholder)
    resetGame(); // Placeholder function to reset the game
  }
}

void handleGameLogic() {
  JoystickDirection j = readJoystick();

  // Placeholder for game logic
  // Here you can implement the game sequence including sound queues and button press handling

  // For now, let's simulate some game progression
  delay(2000); // Wait for 2 seconds (placeholder for displaying standoff screen)

  // Display standoff screen
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("standoffQD.BMP", 0, 0); // Display standoff screen image
  delay(2000); // Display standoff screen for 2 seconds (placeholder)

  // Simulate game progress by reducing player lives (placeholders)
  player1Lives--;
  player2Lives--;

  // Check win conditions and display appropriate screens
  checkWinConditions();

  // Check for button presses
  if (digitalRead(BUTTON_P1) == LOW || digitalRead(BUTTON_P2) == LOW) {
    // Simulate sound queue detection
    bool correctQueue = true; // For now, assume all queues are correct

    // Check if the button press corresponds to a correct sound queue
    if (correctQueue) {
      // Display visuals for correct response
      // For now, let's just display a black screen to indicate where to fire the gun
      displayGunFire();

      // Drain a life from the opposing player
      if (digitalRead(BUTTON_P1) == LOW) {
        player2Lives--;
      } else {
        player1Lives--;
      }
    } else {
      // Display visuals for incorrect response (Parrot image)
      displayParrot();
    }

    // Skip next turn for the player who pressed the button
    delay(2000); // Delay to display visuals
    clearScreen(); // Clear screen after displaying visuals
  }
}


void displayGunFire() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  // Display black screen or gun firing animation
}

void displayParrot() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("PARROT.BMP", 0, 0); // Display Parrot image
}

void clearScreen() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
}

void displayWinScreen(const char* filename) {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap(filename, 0, 0); // Display win screen image
}

void displayScreen(const char* filename) {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap(filename, 0, 0); // Display appropriate screen image
}

void displayStartMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("PLYQD.BMP", 0, 0); // Display PLAYQD.bmp
  Serial.println("Play Menu Function Initialized.");
}

void displayOptionsMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  if (currentMenu == MENU_OPTIONS_P1) {
    drawBitmap("P1OPT.BMP", 0, 0); // Display P1OPTIONSQD.bmp
  } else if (currentMenu == MENU_OPTIONS_P2) {
    drawBitmap("p2OPT.BMP", 0, 0); // Display P2OPTIONSQD.bmp
  }
  Serial.println("Options Menu Function Initialized.");
}

void displayLives() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("LSQD.BMP", 0, 0); // Display LivesStartQD.bmp
  Serial.println("Lives Displayed.");
}

void changeLedColor() {
  if (currentMenu == MENU_OPTIONS_P1) {
    player1Color = static_cast<LedColor>((player1Color + 1) % 5);
    setLedColor(RGB_LED_P1_RED, RGB_LED_P1_GREEN, RGB_LED_P1_BLUE, player1Color);
  } else if (currentMenu == MENU_OPTIONS_P2) {
    player2Color = static_cast<LedColor>((player2Color + 1) % 5);
    setLedColor(RGB_LED_P2_RED, RGB_LED_P2_GREEN, RGB_LED_P2_BLUE, player2Color);
  }
}

void setLedColor(int redPin, int greenPin, int bluePin, LedColor color) {
  switch (color) {
    case RED:
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 0);
      break;
    case BLUE:
      analogWrite(redPin, 0);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 255);
      break;
    case GREEN:
      analogWrite(redPin, 0);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
      break;
    case PURPLE:
      analogWrite(redPin, 255);
      analogWrite(greenPin, 0);
      analogWrite(bluePin, 255);
      break;
    case YELLOW:
      analogWrite(redPin, 255);
      analogWrite(greenPin, 255);
      analogWrite(bluePin, 0);
      break;
  }
}

void drawBitmap(char *filename, uint16_t x, uint16_t y) {
  File bmpFile;
  int bmpWidth, bmpHeight;
  uint8_t bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  uint8_t sdbuffer[3*BUFFPIXEL];
  uint8_t buffidx = sizeof(sdbuffer);
  boolean goodBmp = false;
  boolean flip = true;
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();
  
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return;
  }

  if(read16(bmpFile) == 0x4D42) {
    read32(bmpFile);
    read32(bmpFile);
    bmpImageoffset = read32(bmpFile);
    read32(bmpFile);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) {
        goodBmp = true;
        rowSize = (bmpWidth * 3 + 3) & ~3;
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
        for (row=0; row<h; row++) {
          if(flip)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) {
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer);
          }
          for (col=0; col<w; col++) {
            if (buffidx >= sizeof(sdbuffer)) {
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;
            }
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }
        }
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      }
    }
  }
  
  bmpFile.close();
  if(!goodBmp) Serial.println("BMP format not recognized.");
}

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}
