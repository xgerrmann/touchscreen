#include "UIobjects.h"

// ## SCREEN ##################################################
// Constructor
Screen::Screen( void )
{
	//this->lcd			= NULL;
	//this->rows			= NULL;
	//this->columns		= NULL;

	//this->column_width	= NULL;
	//this->row_height	= NULL;
}

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
	lcd->fillScreen(white);
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

// Touch Handler
void Screen::touch( int x, int y)
{
	this->lcd->fillCircle(x, y, 2, black);
	for(int i=0; i<blocks.size(); i++)
	{
		Block* block = blocks.get(i);
		if(block->inRegion( x, y ))
		{
			block->action(block);
		}
	}
}

// ## BLOCK ##################################################
// Constructor
Block::Block( Screen* sc, int x, int y, int w, int h , void(*func)(Block* block))
{
	this->screen=	sc;
	this->xpos	=	x-1; ypos = y-1; width = w; height = h;
	this->action=	func;
}

// Draw
void Block::draw()
{
	//this->screen->lcd->fillRect(xpos*screen->column_width,ypos*screen->row_height,screen->column_width*width, screen->row_height*height, 0xF81F);
	//this->screen->lcd->drawRect(xpos*screen->column_width,ypos*screen->row_height,screen->column_width*width, screen->row_height*height, 0xFFFF);
	this->screen->lcd->drawRoundRect(xpos*screen->column_width+this->margin,ypos*screen->row_height+this->margin,screen->column_width*width-2*this->margin, screen->row_height*height-2*this->margin, this->radius, info_color);
}

bool Block::inRegion( int x, int y)
{
	bool cond0 = xpos*screen->column_width<=x;
	bool cond1 = (xpos+this->width)*screen->column_width>x;
	bool cond2 = ypos*screen->row_height<=y;
	bool cond3 = (ypos+this->height)*screen->row_height*this->height>y;
	if( cond0 && cond1 && cond2 && cond3)
	{
		return true;
	}
	return false;
}

// ## SCREENMANAGER ##################################################
// Constructor
screenManager::screenManager()
{
	// Empty constructor
}

void screenManager::nextScreen()
{
	this->screen_active = (this->screen_active+1)%(this->screens.size());
	this->screens.get(screen_active)->draw();
}

// Refresh
void screenManager::refresh( void )
{
	this->screens.get(this->screen_active)->draw();
}

// Attach Screen
void screenManager::attach_screen(Screen* screen)
{
	this->screens.add(screen);
	screen->sManager = this;
}

// Touch
void screenManager::touch(int x, int y)
{
	this->screens.get(this->screen_active)->touch(x,y);
}
