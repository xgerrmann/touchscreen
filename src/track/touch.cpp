// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
//#include <TouchScreen.h>
#include <UTouch.h>


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

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
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

//#define YP A1  // must be an analog pin, use "An" notation!
//#define XM A2  // must be an analog pin, use "An" notation!
//#define YM 7   // can be a digital pin
//#define XP 6   // can be a digital pin
/*
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
*/
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//#define LCD_CS A3
//#define LCD_CD A2
//#define LCD_WR A1
//#define LCD_RD A0
// optional
//#define LCD_RESET A4

#define LCD_CS 40 // Chip Select goes to Analog 3
#define LCD_CD 38 // Command/Data goes to Analog 2
#define LCD_WR 39 // LCD Write goes to Analog 1
#define LCD_RD 42 // LCD Read goes to Analog 0
#define LCD_RESET 41 // Can alternately just connect to Arduino's



// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

UTouch  myTouch( 6, 5, 4, 3, 2);

#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void) {
	
	Serial.begin(9600);
	
	tft.reset();
	
	myTouch.InitTouch();
	//myTouch.setPrecision(PREC_MEDIUM);
	myTouch.setPrecision(PREC_LOW);
	
	uint16_t identifier = 0x9488 ;
	
	tft.begin(identifier);
	
	tft.fillScreen(BLACK);
	
	currentcolor = RED;
	
	tft.setCursor(280, 10);
	//tft.setRotation(90);
	tft.setTextColor(WHITE);
	tft.setTextSize(1);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

void loop()
{
	long x, y, x_old, y_old;
	tft.setTextSize(3);
	tft.drawRect(150,200,150,120,WHITE);
	while (myTouch.dataAvailable() == true)
	{
		myTouch.read();
		y = myTouch.getX();
		x = myTouch.getY();
		x = myTouch.getY();
		if ((x!=-1) and (y!=-1))
		{
			y=480-y;
			
			//tft.println("X = "); tft.println(x);
			//tft.println("\tY = "); tft.println(y);
			//tft.println("\tPressure = "); tft.println(z);
			
			if ( (y>200) && (x>150) )
			{
				tft.fillScreen(BLACK);
			}
			// scale from 0->1023 to tft.width
			//x = map(x, TS_MINX, TS_MAXX, tft.width(), 0);
			//y = map(y, TS_MINY, TS_MAXY, tft.height(), 0);
		
			tft.fillRect(0,0,51,50,BLACK);
			tft.setCursor(0,0);
			tft.setTextColor(WHITE);
			tft.println(x);
			tft.println(y);
			x_old = x;
			y_old = y;
			
			if (((y-PENRADIUS) > BOXSIZE) && ((y+PENRADIUS) < tft.height())) {
			  tft.fillCircle(x, y, PENRADIUS, currentcolor);
			}
			
		}
	}
}
