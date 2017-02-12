#ifndef _UIobjects_
#define _UIobjects_

#define MAX_ARRAY_LENGTH 20

#include "../LinkedList/LinkedList.h"
#include "../Adafruit_TFTLCD/Adafruit_TFTLCD.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
// Forward declarations.
// Dereferencing of member access not possible until the actual classes are defined.
class Block;
class Screen;
class screenManager;

typedef void (screenManager::*sMngrMemFn)();

// ## SCREENMANAGER
class screenManager
{
	private:
		int screen_active = 0;
		LinkedList<Screen*> screens; // List of attached screens
	public:
		screenManager( void );
		void nextScreen();
		void donothing();
		void attach_screen(Screen* screen);
		void refresh( void );
		void touch( int x, int y );
};

// ## SCREEN ##################################################
class Screen
{
	public:
		// Variables
		int row_height, column_width;
		Adafruit_TFTLCD* lcd;
		LinkedList<Block*> blocks; // List of attached blocks
		
		// Functions
		Screen( void );
		Screen(Adafruit_TFTLCD* lcd, int rows, int columns);
		void draw();
		void attach_block(Block* block);
		void touch( int x, int y );
		screenManager* sManager;
	private:
		// Variables
		int rows, columns;
};

// ## BLOCK ##################################################
class Block
{
	public:
		// Variables
		
		// Functions
		Block( Screen* screen, int xpos, int ypos, int width, int height, sMngrMemFn action);
		void	draw();
		sMngrMemFn	action; // function pointer
		bool	inRegion( int x, int y );
	private:
		// Variables
		int		xpos, ypos, width, height; //xpos and ypos indicate the row and column in the grid system. With and height indicate the spanning number of columns and rows respectively.
		Screen*	screen;
};
#endif
