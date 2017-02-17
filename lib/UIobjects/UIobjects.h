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

//typedef void (screenManager::*sMngrMemFn)();

// COLORS (Bootstrap php)
#define white				0xFFFF
#define black				0x0000
#define danger_color		0xFA28
#define danger_color_dark	0xC800
#define warning_color		0xFDC6
#define warning_color_dark	0xFC40
#define success_color		0x064A
#define success_color_dark	0x03E6
#define info_color			0x35BC
#define info_color_dark		0x04D9

// ## SCREENMANAGER
class screenManager
{
	private:
		int screen_active = 0;
	public:
		screenManager( void );
		void attach_screen(Screen* screen);
		void refresh( void );
		void refresh( int index );
		void touch( int x, int y );
		Screen* drawScreen(int index);
		// TODO: move to private and make setters and getters for functions acting on screens
		LinkedList<Screen*> screens;	// List of attached screens
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
		void clear();
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
		Block( Screen* screen, int xpos, int ypos, int width, int height, void(*action)(Block* block));
		virtual void	draw();
		virtual void	clear();
		void(*action)(Block*); // function pointer
		bool	inRegion( int x, int y );
		int radius	= 10;
		int margin	= 2;
		int padding	= 5;
		uint16_t color = info_color;
		void setColor(uint16_t color);
		// TODO move to protected and make get functions
		int		xpos, ypos, width, height; //xpos and ypos indicate the row and column in the grid system. With and height indicate the spanning number of columns and rows respectively.
		Screen* getScreen();
	protected: // Allows variable access in derived classes
		// Variables
		Screen*	screen;
};
#endif
