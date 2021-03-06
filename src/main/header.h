#ifndef __HEAD_MAIN__

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// Communication pins
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// LCD pins
#define LCD_CS 40 // Chip Select goes to Analog 3
#define LCD_CD 38 // Command/Data goes to Analog 2
#define LCD_WR 39 // LCD Write goes to Analog 1
#define LCD_RD 42 // LCD Read goes to Analog 0
#define LCD_RESET 41 // Can alternately just connect to Arduino's

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define EVER  true

// Define single screen
#define NROWS	4
#define NCOLS	5

#define NITEMS 3

//#define NUMBER_SCREENS 2
//#define MAX_BLOCKS (2*NROWS*NCOLS)

Adafruit_TFTLCD lcd(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

UTouch tScreen( 6, 5, 4, 3, 2);

touchManager* tManager;

screenManager* sManager;

Screen** screens;

Block** blocks;

struct Person;
struct Product;

LinkedList<Person*> persons; // List of attached blocks
LinkedList<Product*> products; // List of attached blocks

class itemManager;

itemManager* iManager;

int SCREENS_P		;
int SCREENS_O		;
int SCREENS_TOTAL	;
int MAX_BLOCKS		;
int N_BLOCKS		;

//###### Product
struct Product
{
	public:
		Product(String product_name, int product_id);
		int getID();
		String getName();
	private:
		int id;
		String name;
};

Product::Product(String product_name, int product_id)
{
	id	= product_id;
	name= product_name;
}

String Product::getName(){
	return name;
}

int Product::getID(){
	return id;
}

//#######  Person
struct Person
{
	public:
		int id;
		char name[20];
		int count;
		void add( void );
		void reset();
		int getIncrement( void );
		Person(char* name_tmp, int id_tmp, int count_tmp);
	private:
		int increment = 0;
};
Person::Person(char* name_tmp, int id_tmp, int count_tmp)
{
	id		= id_tmp;
	strcpy(name,name_tmp);
	count	= count_tmp;
}

void Person::add( void )
{
	this->increment ++;
}

void Person::reset( void )
{
	this->increment = 0;
}

int Person::getIncrement( void )
{
	return this->increment;
}

//
class itemManager
{
	public:
		itemManager();
		int getItem( void ){return item;};
		void nextItem( void ){ item = (item +1 )%products.size();};
	private:
		int item = 0;
		int n_items;
};

itemManager::itemManager()
{
	n_items = products.size();
}

//####### personBlock derived from Block
class personBlock : public Block
{
	public:
		personBlock(Screen* sc, int x, int y, int w, int h, Person* pers) : person(pers), Block(sc, x, y, w, h){};
		String getText();
		void clearText();
		void drawText(uint16_t);
		void drawText(){drawText(black);};
		void draw();
		void clear();
		Person*	person;
	private:
		int text_size	= 2;
		int text_height	= text_size*7;
};

void personBlock::draw()
{
	Block::draw();
	this->drawText(black);
}

void personBlock::clear()
{
	this->drawText(white);
	this->screen->lcd->drawRoundRect(xpos*screen->column_width+this->margin,ypos*screen->row_height+this->margin,screen->column_width*width-2*this->margin, screen->row_height*height-2*this->margin, this->radius, white);
}

void personBlock::drawText(uint16_t color)
{
	this->screen->lcd->setTextSize(this->text_size);
	this->screen->lcd->setTextColor(color);

	int text_x = this->xpos*this->screen->column_width + this->margin + this->padding;
	int text_y = this->ypos*this->screen->row_height + (int) (this->screen->row_height-this->text_height)/2;
	this->clearText();
	this->screen->lcd->setCursor(text_x, text_y);
	this->screen->lcd->println(this->person->name);

	int integer_length = (int) floor(log10(this->person->getIncrement())) + 1;
	text_x = (this->xpos+2)*this->screen->column_width - this->padding - this->text_height - integer_length*this->text_height;
	this->screen->lcd->setCursor(text_x, text_y);
	if(this->person->getIncrement()>0)
	{
		this->screen->lcd->print('+');
		this->screen->lcd->print(this->person->getIncrement());
	}
	// Set textcolor to default
	this->screen->lcd->setTextColor(black);
}

void personBlock::clearText()
{
	int text_x = this->xpos*this->screen->column_width + this->margin + this->padding;
	int text_y = this->ypos*this->screen->row_height + (int) (this->screen->row_height-this->text_height)/2;
	
	int width = (this->xpos+this->width)*this->screen->column_width-text_x-this->margin-1;
	this->screen->lcd->fillRect(text_x,text_y,width,this->text_height,white);
}

//######## Block action Callbacks
void nextScreen(Block* block)
{
	static int index = 0;
	index = (index +1 )%2;
	sManager->drawScreen(index);
}

void nextItem(Block* block)
{
	iManager->nextItem();
	Serial.print(iManager->getItem());
	// TODO:redraw screen
}
void increment(Block* block)
{
	static_cast<personBlock*>(block)->person->add();
	static_cast<personBlock*>(block)->drawText();
}

void resetIncrements()
{
	for(int i = 0; i<persons.size(); i++)
	{
		persons.get(i)->reset();
	}
}

void regular_cancel(Block* block)
{
	// Reset all increments
	resetIncrements();
	// ReDraw blocks in current screen with nonzero increment
	sManager->refresh();
	
}

void dialog_cancel(Block* block)
{
	block->getScreen()->clear();
	// Reset all increments
	resetIncrements();
	// Draw first screen
	sManager->drawScreen(0);
}

// Drawfunction for nextScreen button
void fillDraw(Block* block)
{
	Screen* screen = block->getScreen();
	screen->lcd->fillRoundRect(block->xpos*screen->column_width+block->margin,block->ypos*screen->row_height+block->margin,screen->column_width*block->width-2*block->margin, screen->row_height*block->height-2*block->margin, block->radius, block->color);
}

// DialogScreen
void dialogScreen(Block* block)
{
	sManager->drawScreen(2);
}

void drawList(Block* block, uint16_t color)
{
	Screen* screen			= sManager->screens.get(2);
	Adafruit_TFTLCD* lcd	= screen->lcd;
	lcd->setTextColor(color);
	int x0 = block->xpos*screen->column_width+block->padding;
	int y0 = block->ypos*screen->row_height+block->padding;
	int offset = 150;
	int line_height = 17;
	int p = 0;
	for(int i = 0; i<persons.size(); i++)
	{
		Person* person = persons.get(i);
		if(person->getIncrement()>0)
		{
			lcd->setCursor(x0,y0+p*line_height);
			lcd->print(person->name);
			lcd->setCursor(x0+offset,y0+p*line_height);
			lcd->print(person->getIncrement());
			p++;
		}
	}

	for(int i = 0; i<persons.size(); i++)
	{
		Person* person = persons.get(i);
		if(person->getIncrement()>0)
		{
		}
	}
	
	// Reset text color to default
	lcd->setTextColor(black);
}

// Drawfunction for dialog text block
void drawList(Block* block)
{
	drawList(block, black);
}

// Drawfunction for dialog text block
void clearList(Block* block)
{
	drawList(block, white);
}

// Drawfunction for item block
void drawItemBlock(Block* block)
{
	Serial.print(products.get(iManager->getItem())->getName());
}

void addPersons()
{
	persons.add(new Person("Knor",1,20));
	persons.add(new Person("Kotser",2,12));
	persons.add(new Person("Trucker",3,15));
	persons.add(new Person("Dijntje",4,3));
	persons.add(new Person("'Rina",5,2));
	persons.add(new Person("Herrie",6,5));
	persons.add(new Person("JP",7,8));
	persons.add(new Person("OPT",8,3));
	persons.add(new Person("Bami",9,2));
}

void addProducts()
{
	products.add(new Product("Bier",1));
	products.add(new Product("Rode wijn",2));
	products.add(new Product("Witte wijn",3));
}

#endif
