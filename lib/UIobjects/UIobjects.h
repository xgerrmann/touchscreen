#ifndef _UIobjects_
#define _UIobjects_

#include "../Adafruit_TFTLCD/Adafruit_TFTLCD.h"

// ## SCREEN ##################################################
class Screen{
	public:
		Screen();
		void draw( Adafruit_TFTLCD );
	private:
};

// Constructor
Screen::Screen( void )
{
	// Empty
}


// Draw
void Screen::draw(Adafruit_TFTLCD tft)
{
	tft.fillScreen(0x001F);
}

// ## BUTTON ##################################################
class Button{
	public:
		Button();
		void draw( Adafruit_TFTLCD );
	private:
		int		id;
		String	text;
};

// Constructor
Button::Button( void )
{
	// Empty
}

// Draw
void Button::draw(Adafruit_TFTLCD tft)
{
	
}

#endif
