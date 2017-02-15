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

#define NUMBER_SCREENS 2
#define MAX_BLOCKS (2*NROWS*NCOLS)

Adafruit_TFTLCD lcd(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

UTouch tScreen( 6, 5, 4, 3, 2);

touchManager* tManager;

screenManager* sManager;

Screen* screens[NUMBER_SCREENS];

Block* blocks[MAX_BLOCKS];

// Define persons
struct Person
{
	public:
		int id;
		String name;
		int count;
		void add( void );
		int get_increment( void );
		Person(String name_tmp, int id_tmp, int count_tmp):id(id_tmp), name(name_tmp), count(count_tmp){};
	private:
		int increment = 0;
};

void Person::add( void )
{
	this->increment ++;
}

int Person::get_increment( void )
{
	return this->increment;
}

LinkedList<Person*> persons; // List of attached blocks


// personBlock as an extension of Block
class personBlock : public Block
{
	public:
		personBlock(Screen* sc, int x, int y, int w, int h, void(*func)(Block* block), Person* person);
		String getText();
		void clearText();
		void drawText();
		void draw();
		void clear();
		Person*	person;
	private:
		int text_size	= 2;
		int text_height	= text_size*7;
};

personBlock::personBlock(Screen* sc, int x, int y, int w, int h, void(*func)(Block* block), Person* person):Block(sc, x, y, w, h, func)
{
	this->person	= person;
}

void personBlock::draw()
{
	Block::draw();
	this->drawText();
}

void personBlock::clear()
{
	this->clearText();
	this->screen->lcd->drawRoundRect(xpos*screen->column_width+this->margin,ypos*screen->row_height+this->margin,screen->column_width*width-2*this->margin, screen->row_height*height-2*this->margin, this->radius, white);
}

void personBlock::drawText()
{
	this->screen->lcd->setTextSize(this->text_size);

	int text_x = this->xpos*this->screen->column_width + this->margin + this->padding;
	int text_y = this->ypos*this->screen->row_height + (int) (this->screen->row_height-this->text_height)/2;
	this->clearText();
	this->screen->lcd->setCursor(text_x, text_y);
	this->screen->lcd->println(this->person->name);

	int integer_length = (int) floor(log10(this->person->get_increment())) + 1;
	text_x = (this->xpos+2)*this->screen->column_width - this->padding - this->text_height - integer_length*this->text_height;
	this->screen->lcd->setCursor(text_x, text_y);
	if(this->person->get_increment()>0)
	{
		this->screen->lcd->print('+');
		this->screen->lcd->print(this->person->get_increment());
	}
}

void personBlock::clearText()
{
	int text_x = this->xpos*this->screen->column_width + this->margin + this->padding;
	int text_y = this->ypos*this->screen->row_height + (int) (this->screen->row_height-this->text_height)/2;
	
	int width = (this->xpos+this->width)*this->screen->column_width-text_x-this->margin-1;
	this->screen->lcd->fillRect(text_x,text_y,width,this->text_height,white);
}

// Button action callbacks
void donothing(Block* block)
{
	//Do nothing
}

void nextScreen(Block* block)
{
	sManager->nextScreen();
}

void increment(Block* block)
{
	static_cast<personBlock*>(block)->person->add();
	static_cast<personBlock*>(block)->drawText();
}


// personBlock as an extension of Block
class menuBlock : public Block
{
	public:
		menuBlock(Screen* sc, int x, int y, int w, int h, void(*func)(Block* block), void(*drawFunc)(Block* block));
		void draw();
		void(*drawFunc)(Block*);
		//void clear();
	private:
		int text_size	= 2;
		int text_height	= text_size*7;
};

menuBlock::menuBlock(Screen* sc, int x, int y, int w, int h, void(*func)(Block* block), void(*drawfunc)(Block* block)):Block(sc, x, y, w, h, func)
{
	this->drawFunc	= drawfunc;
}

void menuBlock::draw()
{
	Block::draw();
	//this->screen->lcd->fillRoundRect(xpos*screen->column_width+this->margin,ypos*screen->row_height+this->margin,screen->column_width*width-2*this->margin, screen->row_height*height-2*this->margin, this->radius, this->color);
	//this->drawFunc(this);
}

// Drawfunction for nextScreen button
void nxt_screen_button_draw(Block* block)
{
	//empty
}
