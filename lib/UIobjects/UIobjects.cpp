#include "UIobjects.h"

// ## SCREEN ##################################################
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
	for(int i =0; i<blocks.size(); i++)
	{
		Block* block = blocks.get(i);
		block->draw();
	}
}

// Attach Block
void Screen::attach_block(Block* block)
{
	blocks.add(block);
}

// ## BLOCK ##################################################
// Constructor
Block::Block( Screen* sc, int x, int y, int w, int h , void (*func)(void))
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
