#ifndef _UIobjects_
#define _UIobjects_

#define MAX_ARRAY_LENGTH 20

#include "../LinkedList/LinkedList.h"
#include "../Adafruit_TFTLCD/Adafruit_TFTLCD.h"

class Block; // Forward declaration. Dereferencing of member access not possible untill the Block class is defined.

// ## SCREEN ##################################################
class Screen
{
	public:
		// Variables
		int row_height, column_width;
		Adafruit_TFTLCD* lcd;
		LinkedList<Block*> blocks; // Array of attached blocks
		
		// Functions
		Screen( void );
		Screen(Adafruit_TFTLCD* lcd, int rows, int columns);
		void draw();
		void attach_block(Block* block);
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
		Block( Screen* screen, int xpos, int ypos, int width, int height, void (*action)(void));
		void	draw();
		void	(*action)( void ); // function pointer
	private:
		// Variables
		int		xpos, ypos, width, height;
		Screen*	screen;
};

#endif
