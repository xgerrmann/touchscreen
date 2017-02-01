#ifndef _UIobjects_
#define _UIobjects_

#include "../Adafruit_TFTLCD/Adafruit_TFTLCD.h"

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


// Draw function
void Screen::draw(Adafruit_TFTLCD tft)
{
	tft.fillScreen(0x001F);
}

#endif
