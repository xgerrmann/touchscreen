// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

//#include <Arduino.h>
#include "Adafruit_TFTLCD.h"	// Hardware-specific library
#include "UIobjects.h"			// Custom library for User interface objects
#include "UTouch.h"				// Digitizer lobrary

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

// Digitizer touch library
UTouch  myTouch( 6, 5, 4, 3, 2);

// Define single screen
#define NROWS	4
#define NCOLS	5

#define NUMBER_SCREENS 2
#define MAX_BLOCKS (2*NROWS*NCOLS)

int screen_active;

// Create array of screens for easy access
Screen*	screens[NUMBER_SCREENS];
Block*	blocks[MAX_BLOCKS];

void tmpfunc( void )
{
	screens[0]->draw();
}

void donothing( void ){}

void setup(void)
{
	// Setup Blocks
	for(int i = 0; i < NUMBER_SCREENS; i++){
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
	}
	
	// Screen 1
	blocks[0] = new Block(screens[0], NCOLS,	1,			1,	NROWS/2,	&donothing);
	blocks[1] = new Block(screens[0], NCOLS,	NROWS/2+1,	1,	NROWS/2,	&donothing);
	blocks[2] = new Block(screens[0], 3,	4,	2,	1,	&donothing);
	blocks[3] = new Block(screens[0], 1,	1,	2,	1,	&donothing);
	blocks[4] = new Block(screens[0], 3,	1,	2,	1,	&donothing);
	blocks[5] = new Block(screens[0], 1,	3,	2,	1,	&tmpfunc);
	
	// Screen 2
	Screen screen2( &lcd, NROWS, NCOLS );
	blocks[6] = new Block(screens[1], NCOLS,	1,			1,	NROWS/2,	&donothing);
	blocks[7] = new Block(screens[1], NCOLS,	NROWS/2+1,	1,	NROWS/2,	&donothing);
	blocks[8] = new Block(screens[1], 3,	4,	2,	1,	&donothing);
	

	// Other
	Serial.begin(9600);
	uint16_t identifier = 0x9488;
	lcd.begin(identifier);
	lcd.setRotation(3);

	// Attach screen 1
	screens[0]->attach_block(blocks[0]);
	screens[0]->attach_block(blocks[1]);
	screens[0]->attach_block(blocks[2]);
	screens[0]->attach_block(blocks[3]);
	screens[0]->attach_block(blocks[4]);
	screens[0]->attach_block(blocks[5]);
	
	// Attach screen 2
	screens[1]->attach_block(blocks[6]);
	screens[1]->attach_block(blocks[7]);
	screens[1]->attach_block(blocks[8]);
	
	// Draw first screen
	screens[0]->draw();
	screen_active = 0;

	// Initialize digitizer
	myTouch.InitTouch(0);
	myTouch.setPrecision(PREC_LOW);
}

void loop(void)
{
	long x,y;
	while (myTouch.dataAvailable() == true)
	{
		myTouch.read();
		y = lcd.height()-myTouch.getX();
		x = myTouch.getY();
		screens[screen_active]->touch(x,y);
	}
}
