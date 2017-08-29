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

// ## ESP settings for Wifi and GET request
char ssid[]		= "Reetro 21";            // your network SSID (name)
char pass[]		= "JKHj5igu";        // your network password

//char ssid[]		= "GeenGedoe";            // your network SSID (name)
//char pass[]		= "Epi1005122533#";        // your network password

char server[]	= "api.reetro21.nl";
byte serverPORT	= 80;

// Reference to hardwareserial
HardwareSerial& esp = Serial1;

//StaticJsonBuffer<1024> jsonBuffer;
DynamicJsonBuffer jsonBuffer;

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

void nextItem(Block* block)
{
	iManager->nextItem();
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

// Drawfunction for nextItem button
void drawNextItemBlock(Block* block)
{
	Screen* screen = block->getScreen();
	screen->lcd->drawRoundRect(block->xpos*screen->column_width+block->margin,block->ypos*screen->row_height+block->margin,screen->column_width*block->width-2*block->margin, screen->row_height*block->height-2*block->margin, block->radius, block->color);
	int x0 = block->xpos*screen->column_width+block->padding;
	int y0 = block->ypos*screen->row_height+block->padding;
	screen->lcd->setCursor(x0,y0);
	int item_index	= iManager->getItem();
	Product* product= products.get(item_index);
	screen->lcd->print(product->getName());
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
	int item_index	= iManager->getItem();
	Product* product= products.get(item_index);
	Serial.print(product->getName());
}

// ########################################33
// ESP
// ########################################33
void reset(){
	esp.println("AT+RST");
	delay(1000);
	if(esp.find("OK")) Serial.println("Module Reset");
}

int connectWifi(char* ssid, char* password) {
	String cmd = "AT+CWJAP=\"" +String(ssid)+"\",\"" + String(password) + "\"";
	Serial.println("SSID:.... "+String(ssid)+"\r\nPassword: "+String(password));
	esp.println(cmd);
	Serial.print("Connecting to Wifi");
	int count =0;
	while(!esp.find("OK")){
		Serial.write('.');
		count++;
		if(count > 10) return 1;
	};
	Serial.write('\r');
	Serial.write('\n');
	Serial.println("Connected!");
	return 0;
}

void httpGET(String uri, char* response)
{
//	uri: e.g "/products"
	esp.println("AT+CIPSTART=\"TCP\",\"" + String(server) + "\"," + String(serverPORT));
	Serial.print("Setting up TCP connection");
	while( !esp.find("OK")) {
		Serial.write(".");
	}
	delay(1000);
	Serial.println("\r\nTCP connection ready\r\n");
	String request =
	"GET " + uri + " HTTP/0.9\r\n" +
	"Host: " + server + "\r\n"
	"Authorization: Basic eGFuZGVyQGdlcnJtYW5uLm5sOmxhc2VycGljbw==\r\n\r\n";
	Serial.println("Send GET request:\r\n----------");
	Serial.print(request);
	Serial.println("----------");
	String sendCMD = "AT+CIPSEND=";
	esp.print(sendCMD);
	esp.println(request.length());
	if(esp.find(">"))
	{
		Serial.println("Sending");
		esp.print(request);
		if( esp.find("SEND OK"))
		{
			int length = 0;
			while( esp.find('+IPD,')){
				length = esp.parseInt(); esp.find(':'); // get length and remove ':'
				char message[length];
				esp.readBytesUntil('?',message,length);
				strncat(response,message,length);
			}
		}
	}
	Serial.println("Close connection");
	esp.println("AT+CIPCLOSE");
}

getProducts()
{
	Serial.println("Retrieving products");
	const char response[4000] = "";
	httpGET("/products",response);
	char* json = strchr(response,'[');
	Serial.println(json);
	
	Serial.println("Received (stripped) Json:");
	Serial.println(json);
	JsonArray& productArray = jsonBuffer.parseArray(json);
	if (!productArray.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	productArray.prettyPrintTo(Serial);

	// Place in Products
	Serial.println("\r\nProducts found:");
	for(JsonArray::iterator it1=productArray.begin(); it1!=productArray.end(); ++it1)
	{
		JsonObject& prod= *it1;
		int id			= NULL;
		char name[20]	= "";
		for(JsonObject::iterator it2=prod.begin(); it2!=prod.end(); ++it2)
		{
			if(strcmp(it2->key,"id")==0)
			{
				id = it2->value.as<int>();
			} else if(strcmp(it2->key,"name")==0) {
				strcpy(name,it2->value);
			} else {
				Serial.print("No key match");
			}
		}
		Serial.println("Id: "+String(id)+", name: "+String(name));
		products.add(new Product(name,id));
	}
	// TODO: sort by id
}

getPersons()
{
	const char response[4000] = "";
	httpGET("/housemates",response);
	char* json = strchr(response,'[');
	Serial.println(json);
	
	Serial.println("Received (stripped) Json:");
	Serial.println(json);
	JsonArray& productArray = jsonBuffer.parseArray(json);
	if (!productArray.success())
	{
		Serial.println("parseObject() failed");
		return;
	}
	productArray.prettyPrintTo(Serial);

	// Place in Persons
	Serial.println("\r\nPersons found:");
	for(JsonArray::iterator it1=productArray.begin(); it1!=productArray.end(); ++it1)
	{
		JsonObject& pers= *it1;
		int id			= NULL;
		char name[20]	= "";
		int quantity	= 0;
		for(JsonObject::iterator it2=pers.begin(); it2!=pers.end(); ++it2)
		{
			if(strcmp(it2->key,"id")==0)					// First element
			{
				id		= it2->value.as<int>();
			} else if(strcmp(it2->key,"quantity")==0) {	// Second element
				quantity= it2->value.as<int>();
			} else if(strcmp(it2->key,"name")==0){			// Last (third) element
				strcpy(name,it2->value);
			} else {
				Serial.print("No key match");
			}
		}
		Serial.println("Id: "+String(id)+", name: "+String(name)+", quantity: "+String(quantity));
		persons.add(new Person(name,id,quantity));
	}
	//TODO: sort by quantity
}
