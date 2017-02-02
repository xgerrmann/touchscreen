#ifndef _UIobjects_
#define _UIobjects_

#include "../Adafruit_TFTLCD/Adafruit_TFTLCD.h"

// ## SCREEN ##################################################
class Screen
{
	public:
		// Variables
		int row_height, column_width;
		Adafruit_TFTLCD* lcd;
		
		// Functions
		Screen(Adafruit_TFTLCD* lcd, int rows, int columns);
		void draw();
	private:
		// Variables
		int rows, columns;
};

// Constructor
Screen::Screen( Adafruit_TFTLCD* display, int r, int c )
{
	this->lcd			= display;
	this->rows			= r;
	this->columns		= c;

	this->column_width	= (int) display->height()/c;
	this->row_height	= (int) display->width()/r;
}

// Draw
void Screen::draw()
{
	lcd->fillScreen(0x001F);
}

// ## BLOCK ##################################################
class Block
{
	public:
		// Variables
		
		// Functions
		Block( Screen* screen, int xpos, int ypos, int width, int height, void* action);
		void	draw();
		void	(*action)( void ); // function pointer
	private:
		// Variables
		int		xpos, ypos, width, height;
		Screen*	screen;
};

// Constructor
Block::Block( Screen* sc, int x, int y, int w, int h , void* func)
{
	this->screen=	sc;
	this->xpos	=	x-1; ypos = y-1; width = w; height = h;
	this->action=	func;
}

// Draw
void Block::draw()
{
	this->screen->lcd->fillRect(xpos*screen->column_width,ypos*screen->row_height,screen->column_width*width, screen->row_height*height, 0xF81F);
	this->screen->lcd->drawRect(xpos*screen->column_width,ypos*screen->row_height,screen->column_width*width, screen->row_height*height, 0xFFFF);
}
#endif
