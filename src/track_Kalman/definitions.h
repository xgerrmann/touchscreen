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


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

UTouch  myTouch( 6, 5, 4, 3, 2);

class trackFilter{
	private:
		// Variables
		float	A[16];
		float	A_t[16];
		float	R[4];
		float	C[8];
		float	Q[16];
		float	P[16];
		//int width_max, height_max;
		float	dt;
		float loc[4]; // [x,y,dx/dt,dy/dt]
		float Pscale = 100;
		float Qscale = 0.001;

		// Procedures
		void setConstantMatrices();
		void setP();
		void setLoc(float location[4]);
	public:
		// Variables

		// Procedures
		trackFilter(int width, int height, float d_t);
		void reset(float measurement[2]);
		void setMatrices();
		void update(float measurement[2], float location[2]);
};

// Constructor
trackFilter::trackFilter(int width, int height, float d_t)
{
	// Set time step
	this->dt = d_t;
	
	// Initialize matrices
	this->setConstantMatrices();
	this->setP();
}

// setLoc
void trackFilter::setLoc(float loc[4])
{
	Matrix.Copy(loc,4,1,this->loc);
}

// setMatrices
void trackFilter::setConstantMatrices()
{
	float R_tmp[4] = {0.2845,0.0045,0.0045,0.0455};
	float C_tmp[8] = {1,0,0,0,
					0,1,0,0};
	
	float Q_tmp[16] = {this->Qscale,0,0,0,
						0,this->Qscale,0,0,
						0,0,this->Qscale,0,
						0,0,0,this->Qscale};
	// System matrix
	float A_tmp[16]  = {	1,	0,	this->dt,	0,
							0,	1,	0,			this->dt,
							0,	0,	1,			0,
							0,	0,	0,			1};
	Matrix.Copy(R_tmp,2,2,this->R);
	Matrix.Copy(C_tmp,2,4,this->C);
	Matrix.Copy(Q_tmp,4,4,this->Q);
	Matrix.Copy(A_tmp,4,4,this->A);

	// Also store the transpose of A
	Matrix.Transpose(this->A,4,4,this->A_t);
}

// P matrix
void trackFilter::setP()
{
	float P_tmp[16] = {this->Pscale,0,0,0,
						0,this->Pscale,0,0,
						0,0,this->Pscale,0,
						0,0,0,this->Pscale};
	Matrix.Copy(P_tmp,4,4,this->P);
}

// Reset
void trackFilter::reset(float measurement[2])
{
	// Initial position estimate
	float loc[4] = {measurement[0], measurement[1], 0, 0};
	this->setLoc(loc);
	this->setP();
}

// Updater
void trackFilter::update(float meas[2], float loc_out[2])
{
	// meas:	measurement [2x1]
	// loc_out:	location	[2x1]
	
	// PP = A*P*A' + Q
	float AP[16];	Matrix.Multiply(this->A,this->P,4,4,4,AP);	// [4x4]
	float APA[16];	Matrix.Multiply(AP,this->A_t,4,4,4,APA);	// 4x4
	float PP[16];	Matrix.Add(APA,this->Q,4,4,PP);				// 4x4
	
	// New Kalman Gain
	// K = PP*C'*inv(C*PP*C'+R)
	float C_t[8];		Matrix.Transpose(this->C,2,4,C_t);			// [2x4] -> [4x2]
	float PPC_t[8];		Matrix.Multiply(PP,C_t,4,4,2,PPC_t);		// [4x2]
	float CPPC_t[4];	Matrix.Multiply(this->C,PPC_t,2,4,2,CPPC_t);// [2x2]
	float M1[4];		Matrix.Add(CPPC_t,R,2,2,M1);				// [2x2]
	Matrix.Invert(M1,2);											// [2x2]
	float K[8];			Matrix.Multiply(PPC_t,M1,4,2,2,K);			// [4x2]
	
	// Location update
	//location(new) = (location + K*(measurement - C*location))';
	float Cloc [2];		Matrix.Multiply(this->C,this->loc,2,4,1,Cloc);	// [2x1]
	float M3 [2];		Matrix.Subtract(meas, Cloc,2,1,M3);		// [2x1]
	float M4 [4];		Matrix.Multiply(K,M3,4,2,1,M4);			// [4x1]
	float loc [4];		Matrix.Add(this->loc,M4,4,1,loc);	// (pos + K*(meas - C*pos));
	this->setLoc(loc);
	
	// New Covariance matrix
	// P = (eye(4)-K*C)*PP
	float M6 [16]; Matrix.Multiply(K,this->C,4,2,4,M6);	// [4x4]
	float M7 [16] = {1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					0,0,0,1};		// [4x4]
	float M8 [16]; Matrix.Subtract(M7,M6,4,4,M8);	// [4x4]
	Matrix.Multiply(M8,PP,4,4,4,this->P);			// [4x4]
	
	// Output
	Matrix.Copy(this->loc,2,1,loc_out);
}
