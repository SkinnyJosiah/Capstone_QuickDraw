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
#define BUFFPIXEL 20

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Current menu state
enum MenuState {
  MENU_START,
  MENU_OPTIONS
};

MenuState currentMenu = MENU_START;

void setup(void) {
  Serial.begin(9600);
  
  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Display available files on SD card
  //Serial.println("Files on SD card:");
  //File root = SD.open("/");
  //while (true) {
    //File entry = root.openNextFile();
    //if (!entry) break;
    //Serial.println(entry.name());
    //entry.close();
  //}

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

// Function to read joystick value
JoystickDirection readJoystick(void) {
  float a = analogRead(JOYSTICK);
  
  a *= 5.0;
  a /= 1024.0;
  
  Serial.print("Joystick read analog = ");
  Serial.println(a);
  if (a < 0.2) return JOYSTICK_DOWN;
  if (a < 1.0) return JOYSTICK_RIGHT;
  if (a < 1.5) return JOYSTICK_SELECT;
  if (a < 2.0) return JOYSTICK_UP;
  if (a < 3.2) return JOYSTICK_LEFT;
  else return JOYSTICK_NONE;
}

void loop() {

  JoystickDirection j = readJoystick();
  switch (j) {
    case JOYSTICK_UP:
  if (currentMenu == MENU_OPTIONS) {
    // Check if joystick is moved up to switch to start menu
    currentMenu = MENU_START;
    displayStartMenu();
    delay(200); // Delay to prevent multiple menu switches
  }
  break;
    case JOYSTICK_DOWN:
      if (currentMenu == MENU_OPTIONS) {
        // Check if joystick is moved down to switch to start menu
        currentMenu = MENU_START;
        displayStartMenu();
        delay(200); // Delay to prevent multiple menu switches
      }
      break;
    case JOYSTICK_SELECT:
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
      break;
    default:
      // Handle other joystick directions or none
      break;
  }
  delay(500);
}

void displayStartMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("PLYQD.BMP", 0, 0); // Display PLAYQD.bmp
  Serial.println(" Play Menu Function Initialized.");
}

void displayOptionsMenu() {
  tft.fillScreen(ST7735_BLACK); // Clear screen
  drawBitmap("OPTQD.BMP", 0, 0); // Display OPTQD.bmp
  Serial.println(" Options Menu Function Initialized.");
}

void drawBitmap(char *filename, uint16_t x, uint16_t y) {

  //Serial.print("Attempting to open file: ");
  //Serial.println(filename);

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  //filename = "OPTQD.bmp";
  
  Serial.println();
  Serial.print("Loading image '");
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.print("File not found in drawBMP");
    return;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println("BMP signature OK");
    Serial.print("File size: "); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data

    Serial.print("Image Offset: ");
    Serial.println(bmpImageoffset);
    Serial.print("Header size: "); 
    Serial.println(read32(bmpFile));
    
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) { // # planes -- must be '1'
      uint8_t bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print("Bit Depth: ");
      Serial.println(bmpDepth);
      
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        Serial.println("BMP format is 24-bit and uncompressed");
        
        boolean goodBmp = true; // Supported BMP format
        Serial.print("Width: ");
        Serial.println(bmpWidth);
        Serial.print("Height: ");
        Serial.println(bmpHeight);
        
        rowSize = (bmpWidth * 3 + 3) & ~3; // BMP rows are padded to 4-byte boundary

        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        w = bmpWidth;
        h = bmpHeight;
        if ((x+w-1) >= tft.width()) w = tft.width() - x;
        if ((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite();
        tft.setAddrWindow(x, y, w, h);

        for (row = 0; row<h; row++) { // For each scanline...
          if (flip) // Bitmap stored bottom-to-top
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;

          if (bmpFile.position() != pos) { // Need to seek?
            tft.endWrite();
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col = 0; col<w; col++) { // For each pixel...
            if (buffidx>=sizeof(sdbuffer)) { // Buffer empty?
              bmpFile.read(sdbuffer,sizeof(sdbuffer));
              buffidx=0; // Set index to beginning
              tft.startWrite();
            }

            // Convert pixel from BMP to TFT format
            b=sdbuffer[buffidx++];
            g=sdbuffer[buffidx++];
            r=sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          }
        }
        tft.endWrite();
        Serial.print("Loaded in ");
        Serial.print(millis() - startTime);
        Serial.println(" ms");
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
