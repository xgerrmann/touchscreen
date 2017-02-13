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
		void draw();
		Person*	person;
};

personBlock::personBlock(Screen* sc, int x, int y, int w, int h, void(*func)(Block* block), Person* person):Block(sc, x, y, w, h, func)
{
	this->person	= person;
}

void personBlock::draw()
{
	Block::draw();
	int x_margin	= 5;
	int txt_size	= 2;
	int txt_height	= txt_size*7;
	this->screen->lcd->setTextSize(txt_size);

	int text_x = this->xpos*this->screen->column_width + x_margin;
	int text_y = this->ypos*this->screen->row_height + (int) (this->screen->row_height-txt_height)/2;
	this->screen->lcd->setCursor(text_x, text_y);
	this->screen->lcd->println(this->person->name);

	int integer_length = (int) floor(log10(this->person->get_increment())) + 1;
	text_x = (this->xpos+2)*this->screen->column_width - x_margin - txt_height - integer_length*txt_height;
	this->screen->lcd->setCursor(text_x, text_y);
	if(this->person->get_increment()>0)
	{
		this->screen->lcd->print('+');
		this->screen->lcd->print(this->person->get_increment());
	}
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
	block->draw();
}
