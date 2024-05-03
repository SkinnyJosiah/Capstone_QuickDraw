//SD card initialization failed!

//Loading image 'OPTIONQD.bmp'
//File not found.

//Note: I have a function that reads the files. It does show that OptionQD is in the files.

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// Pin Definitions
#define SD_CS    4  // Chip select line for SD card
#define TFT_CS    10  // Chip select line for TFT display
#define TFT_DC     8  // Data/command line for TFT
#define TFT_RST  -1  // Reset line for TFT (or connect to +5V)
#define JOYSTICK   A0 // Analog pin for joystick input
#define BUTTON_P1  2  // Digital pin for Player 1 button
#define BUTTON_P2  6  // Digital pin for Player 2 button
#define RGB_LED_P1_RED    3  // Digital pin for Player 1 RGB LED (Red)
#define RGB_LED_P1_GREEN  4  // Digital pin for Player 1 RGB LED (Green)
#define RGB_LED_P1_BLUE   5  // Digital pin for Player 1 RGB LED (Blue)
#define RGB_LED_P2_RED    7  // Digital pin for Player 2 RGB LED (Red)
#define RGB_LED_P2_GREEN  8  // Digital pin for Player 2 RGB LED (Green)
#define RGB_LED_P2_BLUE   9  // Digital pin for Player 2 RGB LED (Blue)
#define BUFFPIXEL 20

// Initialize TFT display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, -1);

// Current menu state
enum MenuState {
  MENU_START,
  MENU_OPTIONS
};

MenuState currentMenu = MENU_START;

void setup() {
  Serial.begin(9600);
  
    // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  
  // List all files to ensure SD card is read correctly
  Serial.println("Files on SD card:");
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    Serial.println(entry.name());
    entry.close();
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

  // Initialize SD card
  if (!SD.begin()) {
    Serial.println("SD card initialized!");
    return;
  }
  else{
    Serial.println("SD card failed to initialize! SD.Begin has not begun.");
    return;
  }

  // Display initial menu
  displayStartMenu();
}

void loop() {
  // Check joystick input for menu navigation
  int joystickValue = analogRead(JOYSTICK);

  // Move between menus
  if (currentMenu == MENU_START) {
    // Check if joystick is moved down to switch to options menu
    if (joystickValue < 100) {
      currentMenu = MENU_OPTIONS;
      displayOptionsMenu();
      delay(200); // Delay to prevent multiple menu switches
    }
  } else if (currentMenu == MENU_OPTIONS) {
    // Check if joystick is moved up to switch to start menu
    if (joystickValue > 900) {
      currentMenu = MENU_START;
      displayStartMenu();
      delay(200); // Delay to prevent multiple menu switches
    }
  }

  // Handle selection
  if (joystickValue < 1000 && joystickValue > 900) {
    // Joystick button pressed
    if (currentMenu == MENU_START) {
      // Enter the game
      // Call function to start the game
      // For now, let's just print a message
      Serial.println("Entering menu...");
    } else if (currentMenu == MENU_OPTIONS) {
      // Enter options mode
      // Call function to enter options mode
      // For now, let's just print a message
      Serial.println("Entering options mode...");
    }
    delay(200); // Delay to prevent multiple selections
  }
}

void displayStartMenu() {
  // Display PlayQD.bmp on TFT screen
  // Implement joystick controls to navigate between PlayQD and OptionQD menus
  // Use joystick select button to enter the game or options
  tft.fillScreen(ST7735_BLACK); // Clear screen
  // Load and display PlayQD.bmp from SD card
  drawBitmap("PlayQD.bmp", 0, 0);
  
  Serial.println(" Play Menu Function Initialized.");
}

void displayOptionsMenu() {
  // Display OptionQD.bmp on TFT screen
  // Implement joystick controls to navigate between PlayQD and OptionQD menus
  // Use joystick select button to enter the game or options
  tft.fillScreen(ST7735_BLACK); // Clear screen
  // Load and display OptionQD.bmp from SD card
  drawBitmap("OptionQD.bmp", 0, 0);
  
  Serial.println(" Options Menu Function Initialized.");
}

void drawBitmap(const char *filename, int16_t x, uint16_t y) {
  
  uint8_t sdbuffer[3 * BUFFPIXEL]; // Pixel buffer (R+G+B per pixel)
  uint8_t buffidx = 0; // Current position in sdbuffer
  
  File bmpFile;
  int bmpWidth, bmpHeight;   // Width and height in pixels
  uint8_t bmpDepth;          // Bit depth (must be 24)
  uint32_t bmpImageoffset;   // Start of image data in file
  uint32_t rowSize;          // Not always = bmpWidth; may have padding
  boolean goodBmp = false;   // Set to true on valid header parse
  boolean flip = true;       // BMP is stored bottom-to-top
  int w, h, row, col;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= tft.width()) || (y >= tft.height())) return;
  
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found.");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    bmpImageoffset = read32(bmpFile); // Start of image data
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);

    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format
        rowSize = (bmpWidth * 3 + 3) & ~3; // BMP rows are padded to 4-byte boundary

        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (row = 0; row < h; row++) { // For each scanline...

          if (flip) // Bitmap stored bottom-to-top
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;

          if (bmpFile.position() != pos) { // Need to seek?
            bmpFile.seek(pos);
            buffidx = 0; // Force buffer reload
          }

          for (col = 0; col < w; col++) { // For each pixel...
            if (buffidx >= sizeof(sdbuffer)) { // Reload pixel data
              sdbuffer[0] = bmpFile.read();
              sdbuffer[1] = bmpFile.read();
              sdbuffer[2] = bmpFile.read();
              buffidx = 0;
            }

            // Push pixel color to TFT
            if (tft.getRotation() == 0) {
              tft.pushColor(tft.color565(sdbuffer[2], sdbuffer[1], sdbuffer[0]));
            } else {
              tft.pushColor(tft.color565(sdbuffer[0], sdbuffer[1], sdbuffer[2]));
            }
          }
        }
      }
    }
  }

  bmpFile.close();
  if (!goodBmp) Serial.println("BMP format not recognized.");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
