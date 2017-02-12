// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <Adafruit_GFX.h> // Core graphics library
#include "UIobjects.h"// Custom library for User interface objects
#include <UTouch.h>
#include <TouchManager.h> // Library for filtering touchscreen data
#include <MatrixMath.h>  // Library for matrix calculations
#include <LinkedList.h>
#include "header.h"


// personBlock as an extension of Block
class personBlock : public Block
{
	public:
		personBlock(Screen* sc, int x, int y, int w, int h, sMngrMemFn func, Person* person):Block(sc, x, y, w, h, func){};
		String getText();
//		void draw();
	private:
		Person* person;
};

//personBlock::Block(Screen* sc, int x, int y, int w, int h, sMngrMemFn func, Person* person)
//{
//	this->person	= person;
//}

//personBlock::draw()
//{
//	Block::draw();
//	Serial.println(this->person->name);
//}


void setup(void)
{
	Serial.begin(9600);

	persons.add(new Person("Knor",1));
	persons.add(new Person("Kotser",2));
	persons.add(new Person("Trucker",3));
	persons.add(new Person("Dijntje",4));
	persons.add(new Person("'Rina",5));
	persons.add(new Person("Herrie",6));
	persons.add(new Person("JP",7));
	persons.add(new Person("OPT",8));
	persons.add(new Person("Bami",9));

	// Blocks
	sMngrMemFn donothing	= &screenManager::donothing;
	sMngrMemFn nextScreen	= &screenManager::nextScreen;
	sManager				= new screenManager();
	// Create screens and attach to manager
	for(int i = 0; i < NUMBER_SCREENS; i++)
	{
		screens[i] = new Screen( &lcd, NROWS, NCOLS );
		sManager->attach_screen(screens[i]);
	}

	int block_counter = 0;
	int s = 0;
	int r = 0;
	int c = 0;
	for(int i=0; i<persons.size(); i++)
	{
		s = (int) floor((double)block_counter/8);
		r = (int) floor((double)(block_counter %(s*8))/(NCOLS/2)) ;
		c = block_counter%(NCOLS/2);
		blocks[block_counter] = new personBlock(screens[0], c*2+1, r+1,	2,	1, donothing, persons.get(i));
		screens[s]->attach_block(blocks[block_counter]);
		block_counter ++;
	}
	// Navigation
	for(int s=0; s<NUMBER_SCREENS; s++)
	{
		for(int r=0; r<NROWS; r++)
		{
			//sMngrMemFn func;
			//if(r==4)
			//{
			//	func = donothing;
			//} else {
			//	func = nextScreen;
			//}
			blocks[block_counter] = new Block(screens[0], NCOLS, r+1,	1,	1, nextScreen);
			screens[s]->attach_block(blocks[block_counter]);
			block_counter ++;
		}
	}
	
	// Other
	lcd.begin(0x9488);
	lcd.setRotation(3);

	// Draw first screen
	sManager->refresh();

	// Initialize digitizer
	tScreen.InitTouch(0);
	tScreen.setPrecision(PREC_LOW);

	tManager	= new touchManager(&tScreen);
}

void loop(void)
{
	if (tScreen.dataAvailable() == true)
	{
		touchAction tAction = tManager->getAction();
		sManager->touch(tAction.location[0],tAction.location[1]-160);
	}
}
