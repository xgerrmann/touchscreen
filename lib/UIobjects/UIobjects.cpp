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

	// Clear screen
	this->lcd->fillScreen(white);
}

// Draw
void Screen::draw()
{
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
	for(int i=0; i<blocks.size(); i++)
	{
		Block* block = blocks.get(i);
		if(block->inRegion( x, y ))
		{
			if(block->action != NULL)
			{
				block->action(block);
			}
		}
	}
}

// Clear
void Screen::clear()
{
	for(int i=0; i<this->blocks.size(); i++)
	{
		this->blocks.get(i)->clear();
	}
}

// ## BLOCK ##################################################
// Constructor
Block::Block( Screen* sc, int x, int y, int w, int h)
{
	this->screen=	sc;
	this->xpos	=	x-1; ypos = y-1; width = w; height = h;
}

// Draw
void Block::draw()
{
	if(this->drawfun != NULL)
	{
		// Use alternative drawfunction if given.
		this->drawfun(this);
	} else {
		// Default draw function
		this->screen->lcd->drawRoundRect(xpos*screen->column_width+this->margin,ypos*screen->row_height+this->margin,screen->column_width*width-2*this->margin, screen->row_height*height-2*this->margin, this->radius, this->color);
	}
}

// Draw
void Block::clear()
{
	if(this->clearfun != NULL)
	{
		// Use alternative drawfunction if given.
		this->clearfun(this);
	} else {
		// Default draw function
		// Completely clear the area of the block
		this->screen->lcd->fillRect(xpos*screen->column_width,ypos*screen->row_height,screen->column_width*width, screen->row_height*height, white);
	}
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

// setColor
void Block::setColor(uint16_t color_tmp)
{
	this->color = color_tmp;
}

// getScreen
Screen* Block::getScreen()
{
	return this->screen;
}
// ## SCREENMANAGER ##################################################
// Constructor
screenManager::screenManager()
{
	// Empty constructor
}

// Refresh
void screenManager::refresh( void )
{
	//this->screens.get(this->screen_active)->lcd->fillScreen(white);
	//this->screens.get(this->screen_active)->draw();
	this->drawScreen(this->screen_active);
}

// Refresh with screenname given
void screenManager::refresh( int index )
{
	this->screen_active = index;
	this->screens.get(index)->lcd->fillScreen(white);
	this->screens.get(index)->draw();
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

// drawScreen
Screen* screenManager::drawScreen(int index)
{
	// Clear current screen
	this->screens.get(this->screen_active)->clear();
	
	// Draw new screen and set internal state
	this->screens.get(index)->draw();
	this->screen_active = index;
}
