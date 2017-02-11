#include <TouchManager.h>

//########################################
//############### FILTER #################
//########################################

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

//########################################
//############### MANAGER ################
//########################################
touchManager::touchManager( UTouch* Tscreen)
{
	// Attach screen
	this->tScreen = Tscreen;
	
	// Initialize filter
	this->filter	= new trackFilter(420,380,this->dt);
}

touchManager::touchManager( UTouch* Tscreen, Adafruit_TFTLCD* LCD)
{
	// Attach screen
	this->tScreen = Tscreen;
	
	// Initialize filter
	this->filter	= new trackFilter(420,380,this->dt);

	// Connect display
	this->lcd		= LCD;
}

touchAction touchManager::getAction()
{
	int x_meas, y_meas;
	float x_filt, y_filt;
	float x_filt_prev, y_filt_prev;
	float measurement[2]= {};
	float location[2]	= {};
	int t_elapsed		= 0;	// [ms]
	int t_start_touch	= 0;	// [ms]
	int t_start_loop	= 0;
	int t_end_loop		= 0;
	bool filter_init	= false;
	t_start_touch = millis(); // Timer for the duration of the touch action
	
	for(EVER;;)
	{
		if(!this->tScreen->dataAvailable())
		{
			if((millis()-t_end_loop)>this->max_interval_ms)
			{
				break; // Break while loop, touch action is interrupted
			}
			continue; // Restart while loop
		}
		t_start_loop = millis();
	
		// Reset the filter for each NEW touch action
		if(!filter_init)
		{
			// Reset filter for current touch action
			filter->reset(measurement);
			filter_init = true;
		}
		tScreen->read();
		y_meas = tScreen->getX();
		x_meas = tScreen->getY();
		if ((x_meas!=-1) and (y_meas!=-1))
		{
			y_meas=480-y_meas;
			
			measurement[0] = (float) x_meas;
			measurement[1] = (float) y_meas;
			filter->update(measurement,location);
			x_filt_prev = x_filt;
			y_filt_prev = y_filt;
			x_filt	= (float) location[0];
			y_filt	= (float) location[1];
			if(this->lcd)
			{
				this->lcd->fillCircle(x_filt,y_filt,1,0x001F);
				this->lcd->fillCircle(x_meas,y_meas,1,0xF800);
			}
		}
		t_end_loop	= millis();
		t_elapsed	= t_end_loop-t_start_loop;
		// Check if data is available for the next loop
	}
	touchAction tAction;
	tAction.location[0] = x_filt_prev;
	tAction.location[1] = y_filt_prev; // Return second last filtered point (releas introduces noise).
	tAction.type	= 1;
	return tAction;
}
