#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>

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
#define BUFFPIXEL 20

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

  // Display available files on SD card
  Serial.println("Files on SD card:");
  File root = SD.open("/");
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    Serial.println(entry.name());
    entry.close();
  }

  // Attempt to open the image file
  Serial.print("Attempting to open OPTQD.BMP...");
  File bmpFile = SD.open("OPTQD.BMP");
  if (!bmpFile) {
    Serial.println("Failed to open OPTQD.BMP");
    return;
  }
  
  Serial.println("Successfully opened OPTQD.BMP");

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
      displayStartMenu();
      // For now, let's just print a message
      Serial.println("Entering menu...");
    } else if (currentMenu == MENU_OPTIONS) {
      // Enter options mode
      displayOptionsMenu();
      // For now, let's just print a message
      Serial.println("Entering options mode...");
    }
    delay(200); // Delay to prevent multiple selections
  }
}

void displayStartMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("PLAYQD.BMP", 0, 0); // Display PLAYQD.bmp
  Serial.println(" Play Menu Function Initialized.");
}

void displayOptionsMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("OPTQD.BMP", 0, 0); // Display OPTQD.bmp
  Serial.println(" Options Menu Function Initialized.");
}

void drawBitmap(const char *filename, int16_t x, uint16_t y) {
  Serial.print("Attempting to open file: ");
  Serial.println(filename);

  uint8_t sdbuffer[3 * BUFFPIXEL];
  uint8_t buffidx = 0;
  
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.print("File not found: ");
    Serial.println(filename);
    return;
  }
  
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println("BMP signature OK");

    uint32_t bmpImageoffset = read32(bmpFile); // Start of image data
    int bmpWidth = read32(bmpFile);
    int bmpHeight = read32(bmpFile);

    Serial.print("Image Offset: ");
    Serial.println(bmpImageoffset);
    Serial.print("Width: ");
    Serial.println(bmpWidth);
    Serial.print("Height: ");
    Serial.println(bmpHeight);

    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      uint8_t bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: ");
      Serial.println(bmpDepth);
      
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        Serial.println("BMP format is 24-bit and uncompressed");
        
        boolean goodBmp = true; // Supported BMP format
        uint32_t rowSize = (bmpWidth * 3 + 3) & ~3; // BMP rows are padded to 4-byte boundary

        boolean flip = true;
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        int w = bmpWidth;
        int h = bmpHeight;
        if ((x + w - 1) >= tft.width()) w = tft.width() - x;
        if ((y + h - 1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

        for (int row = 0; row < h; row++) { // For each scanline...
          uint32_t pos;
          if (flip) // Bitmap stored bottom-to-top
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;

          if (bmpFile.position() != pos) { // Need to seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (int col = 0; col < w; col++) { // For each pixel...
            if (buffidx >= sizeof(sdbuffer)) { // Buffer empty?
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            uint8_t b = sdbuffer[buffidx++];
            uint8_t g = sdbuffer[buffidx++];
            uint8_t r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r, g, b));
          }
        }
      } else {
        Serial.println("BMP format not recognized or not 24-bit uncompressed");
      }
    } else {
      Serial.println("BMP planes not 1");
    }
  } else {
    Serial.println("BMP signature not found");
  }

  bmpFile.close();
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
