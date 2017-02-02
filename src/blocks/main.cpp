// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

//#include <Arduino.h>
#include "Adafruit_TFTLCD.h"	// Hardware-specific library
#include "UIobjects.h"			// Custom library for User interface objects
//#include <Adafruit_GFX.h>		// Core graphics library

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).

//#define LCD_CS A3 // Chip Select goes to Analog 3
//#define LCD_CD A2 // Command/Data goes to Analog 2
//#define LCD_WR A1 // LCD Write goes to Analog 1
//#define LCD_RD A0 // LCD Read goes to Analog 0
//#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define LCD_CS 40 // Chip Select goes to Analog 3
#define LCD_CD 38 // Command/Data goes to Analog 2
#define LCD_WR 39 // LCD Write goes to Analog 1
#define LCD_RD 42 // LCD Read goes to Analog 0
#define LCD_RESET 41 // Can alternately just connect to Arduino's reset pin


// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK	0x0000
#define	BLUE	0x001F
#define	RED		0xF800
#define	GREEN	0x07E0
#define CYAN	0x07FF
#define MAGENTA	0xF81F
#define YELLOW	0xFFE0
#define WHITE	0xFFFF

Adafruit_TFTLCD lcd( LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET );
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

// Define single screen
#define NROWS	4
#define NCOLS	10
Screen screen( &lcd, NROWS, NCOLS );

// Define several blocks
Block block1(&screen, 1,	1,		1,	1);
Block block2(&screen, 2,	2,		1,	1);
Block block3(&screen, NCOLS,NROWS,	1,	2);
Block block4(&screen, 1,	NROWS,	1,	1);

void setup(void)
{
	Serial.begin(9600);
	uint16_t identifier = 0x9488;
	lcd.begin(identifier);
	lcd.setRotation(3);
}

void loop(void)
{
	//sc.draw(tft);
	//delay(1000);
	lcd.fillScreen(WHITE);
	block1.draw();
	block2.draw();
	block3.draw();
	block4.draw();
	delay(1000);
}
