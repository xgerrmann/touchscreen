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

int screen_active;
