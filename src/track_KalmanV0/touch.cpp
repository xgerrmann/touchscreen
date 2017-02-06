// Paint example specifically for the TFTLCD breakout board.
// If using the Arduino shield, use the tftpaint_shield.pde sketch instead!
// DOES NOT CURRENTLY WORK ON ARDUINO LEONARDO

#include <Adafruit_GFX.h> // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <MatrixMath.h>  // Library for matrix calculations
//#include <TouchScreen.h>
#include <UTouch.h>


#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7

// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 22
//   D1 connects to digital pin 23
//   D2 connects to digital pin 24
//   D3 connects to digital pin 25
//   D4 connects to digital pin 26
//   D5 connects to digital pin 27
//   D6 connects to digital pin 28
//   D7 connects to digital pin 29

// For the Arduino Due, use digital pins 33 through 40
// (on the 2-row header at the end of the board).
//   D0 connects to digital pin 33
//   D1 connects to digital pin 34
//   D2 connects to digital pin 35
//   D3 connects to digital pin 36
//   D4 connects to digital pin 37
//   D5 connects to digital pin 38
//   D6 connects to digital pin 39
//   D7 connects to digital pin 40

//#define YP A1  // must be an analog pin, use "An" notation!
//#define XM A2  // must be an analog pin, use "An" notation!
//#define YM 7   // can be a digital pin
//#define XP 6   // can be a digital pin
/*
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
*/
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//#define LCD_CS A3
//#define LCD_CD A2
//#define LCD_WR A1
//#define LCD_RD A0
// optional
//#define LCD_RESET A4

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


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

UTouch  myTouch( 6, 5, 4, 3, 2);

#define BOXSIZE 40
#define PENRADIUS 1

//// MatrixMath operator overloading
//LiquidCrystal& LiquidCrystal::operator<< (const char str[]) {  //there are a half dozen overloaded versions of print
// LiquidCrystal::print(str);
// return *this;
// }
class trackFilter{
	private:
		int N;
		int N2;
		float	A[16];
		float	R[4];
		float	H[8];
		float	Q[16];
		float	P[16];
		int width_max, height_max;
		int		dt;
		float loc[4]; // [x,y,dx/dt,dy/dt]
	public:
		// Variables

		// Procedures
		trackFilter(int width, int height, int frequency);
		void update(float measurement[2], float location[2]);
};

// Constructor
trackFilter::trackFilter(int width, int height, int frequency)
{
	float R_tmp[4] = {0.5,0.5,0.5,0.5};
	float H_tmp[8] = {1,0,0,0,
					0,1,0,0};

	float scale = 0.01;
	float Q_tmp[16] = {scale,0,0,0,
						0,scale,0,0,
						0,0,scale,0,
						0,0,0,scale};
	scale = 100;
	float P_tmp[16] = {scale,0,0,0,
						0,scale,0,0,
						0,0,scale,0,
						0,0,0,scale};
	// Initial position estimate
	float x = width/2;
	float y = height/2;
	float loc_tmp[4] = {x,y,0,0};
	// System matrix
	float dt = 1/((double)frequency);
	float A_tmp[16]  = {	1,	0,	dt,	0,
							0,	1,	0,	dt,
							0,	0,	1,	0,
							0,	0,	0,	 1};
	Matrix.Copy(A_tmp,4,4,this->A);
	Matrix.Copy(R_tmp,2,2,this->R);
	Matrix.Copy(H_tmp,2,4,this->H);
	Matrix.Copy(Q_tmp,4,4,this->Q);
	Matrix.Copy(P_tmp,4,4,this->P);
	Matrix.Copy(loc_tmp,4,1,this->loc);
}

// Updater
void trackFilter::update(float meas[2], float loc_out[2])
{
	// meas:	measurement [2x1]
	// loc_out:	location	[2x1]
	
	// PP = A*P*A' + Q
	float AP[16];	Matrix.Multiply(this->A,this->P,4,4,4,AP);	// [4x4]
	float A_t[16];	Matrix.Transpose(this->A,4,4,A_t);	// TODO: store as internal variable (because it is constant).
	float APA[16];	Matrix.Multiply(AP,A_t,4,4,4,APA);	//4x4
	float PP[16];	Matrix.Add(APA,this->Q,4,4,PP);		// 4x4
	
	// New Kalman Gain
	// K = PP*H'*inv(H*PP*H'+R)
	float H_t[8];		Matrix.Transpose(this->H,2,4,H_t);			// [2x4] -> [4x2]
	float PPH_t[8];		Matrix.Multiply(PP,H_t,4,4,2,PPH_t);		// [4x2]
	float HPPH_t[4];	Matrix.Multiply(this->H,PPH_t,2,4,2,HPPH_t);// [2x2]
	float M1[4];		Matrix.Add(HPPH_t,R,2,2,M1);				// [2x2]
	Matrix.Invert(M1,2);											// [2x2]
	float K[8];			Matrix.Multiply(PPH_t,M1,4,2,2,K);	// [4x2]
	
	// Location update
	//location(new) = (location + K*(measurement - H*location))';
	float Hloc [2];		Matrix.Multiply(this->H,this->loc,2,4,1,Hloc);	// [2x1]
	float M3 [2];		Matrix.Subtract(meas, Hloc,2,1,M3);		// [2x1]
	float M4 [4];		Matrix.Multiply(K,M3,4,2,1,M4);			// [4x1]
	float loc_tmp [4];	Matrix.Add(this->loc,M4,4,1,loc_tmp);	// (pos + K*(meas - H*pos));
	Matrix.Copy(loc_tmp,4,1,this->loc);							// [2x1] -> write to fist two states
	
	// New Covariance matrix
	// P = (eye(4)-K*H)*PP
	float M6 [16]; Matrix.Multiply(K,this->H,4,2,4,M6);	// [4x4]
	float M7 [16] = {1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					0,0,0,1};		// [4x4]
	float M8 [16]; Matrix.Subtract(M7,M6,4,4,M8);	// [4x4]
	Matrix.Multiply(M8,PP,4,4,4,this->P);			// [4x4]
	
	// Output
	Matrix.Copy(this->loc,2,1,loc_out);
}


trackFilter* filter;

void setup(void) 
{
	Serial.begin(9600);
	
	tft.reset();
	
	myTouch.InitTouch();
	//myTouch.setPrecision(PREC_MEDIUM);
	myTouch.setPrecision(PREC_LOW);
	
	uint16_t identifier = 0x9488 ;
	
	tft.begin(identifier);
	
	tft.fillScreen(BLACK);
	
	tft.setCursor(280, 10);
	tft.setTextColor(WHITE);
	tft.setTextSize(3);
	filter = new trackFilter(420,380,10);
}

void loop()
{
int x_meas, y_meas, x_filt, y_filt;
	//tft.drawRect(150,200,150,120,WHITE);
	float measurement[2]= {};
	float location[2]	= {};
	while (myTouch.dataAvailable() == true)
	{
		int t0 = millis();
		myTouch.read();
		y_meas = myTouch.getX();
		x_meas = myTouch.getY();
		if ((x_meas!=-1) and (y_meas!=-1))
		{
			y_meas=480-y_meas;
			
			measurement[0] = (float) x_meas;
			measurement[1] = (float) y_meas;
			filter->update(measurement,location);
			x_filt	= location[0];
			y_filt	= location[1];
			
			tft.fillRect(0,0,51,100,BLACK);
			tft.setCursor(0,0);
			tft.setTextColor(WHITE);
			tft.println(x_meas);
			tft.println(y_meas);
			
			tft.setTextColor(WHITE);
			tft.println(x_filt);
			tft.println(y_filt);
			
			if (((y_meas-PENRADIUS) > BOXSIZE) && ((y_meas+PENRADIUS) < tft.height())) {
				tft.fillCircle(x_meas, y_meas, PENRADIUS, RED);
				tft.fillCircle(x_filt, y_filt, PENRADIUS, BLUE);
			}
		}
		Serial.println(millis()-t0);
	}
}
