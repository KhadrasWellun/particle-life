#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#define GRID_DIV 4

/*
 * for collision detection :
 * if (distance(x center, x line) < radius) then intersect 
 */

struct point
{
	point(float _x, float _y, const int _r, const int _g, const int _b) : x(_x), y(_y), r(_r), g(_g), b(_b) {}

	//Position
	float x;
	float y;

	//Velocity
	float vx = 0;
	float vy = 0;

	//Color
	const int r;
	const int g;
	const int b;

	int gridId = -1;

	void draw() const
	{
		ofSetColor(r, g, b, 100); //set particle color + some alpha
		ofDrawCircle(x, y, 2.0F); //draw a point at x,y coordinates, the size of a 2 pixels
	}
};

struct grid
{
	const int gridSize = GRID_DIV * GRID_DIV; // must be a power of 2
};

//---------------------------------------------CONFIGURE GUI---------------------------------------------//
class ofApp final : public ofBaseApp
{
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;
	void restart();
	void random();
	void monads();
	void rndir();
	void rndprob();
	void rndvsc();
	void saveSettings();
	void loadSettings();
	void interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float G, float Gradius, float Gprobability, float A, float Aradius, float Aprobability, float viscosity);
	
	ofxPanel gui;

	ofxGuiGroup globalGroup;
	ofxGuiGroup rndGroup;
	ofxGuiGroup viscosityGroup;
	ofxGuiGroup probabilityRGroup;
	ofxGuiGroup probabilityGGroup;
	ofxGuiGroup probabilityBGroup;
	ofxGuiGroup probabilityWGroup;
	ofxGuiGroup probabilityOGroup;
	ofxGuiGroup probabilityKGroup;
	ofxGuiGroup probabilityCGroup;
	ofxGuiGroup probabilityDGroup;
	ofxGuiGroup qtyGroup;
	ofxGuiGroup redGroup;
	ofxGuiGroup greenGroup;
	ofxGuiGroup blueGroup;
	ofxGuiGroup whiteGroup;
	ofxGuiGroup orangeGroup;
	ofxGuiGroup khakiGroup;
	ofxGuiGroup crimsonGroup;
	ofxGuiGroup darkGroup;


	ofxButton resetButton;
	ofxButton selectButton;
	ofxButton randomGeneral;
	ofxButton randomCount;
	ofxButton randomChoice;
	ofxButton randomProb;
	ofxButton randomVsc;
	ofxButton save;
	ofxButton load;

	ofxToggle boundsToggle;
	ofxToggle modelToggle;
	ofxToggle motionBlurToggle;

	// some experimental stuff here
	ofxGuiGroup expGroup;
	ofxToggle evoToggle;
	ofxFloatSlider evoProbSlider;
	ofxFloatSlider evoAmountSlider;
	float evoChance = 0.1;
	float evoAmount = 0.5;
	ofxToggle radiusToogle;
	ofxLabel physicLabel;
	//end of experimental

	ofxFloatSlider probabilitySlider;

	ofxFloatSlider probabilitySlider_a_RR;
	ofxFloatSlider probabilitySlider_a_RG;
	ofxFloatSlider probabilitySlider_a_RB;
	ofxFloatSlider probabilitySlider_a_RW;
	ofxFloatSlider probabilitySlider_a_RO;
	ofxFloatSlider probabilitySlider_a_RK;
	ofxFloatSlider probabilitySlider_a_RC;
	ofxFloatSlider probabilitySlider_a_RD;

	ofxFloatSlider probabilitySlider_r_RR;
	ofxFloatSlider probabilitySlider_r_RG;
	ofxFloatSlider probabilitySlider_r_RB;
	ofxFloatSlider probabilitySlider_r_RW;
	ofxFloatSlider probabilitySlider_r_RO;
	ofxFloatSlider probabilitySlider_r_RK;
	ofxFloatSlider probabilitySlider_r_RC;
	ofxFloatSlider probabilitySlider_r_RD;

	ofxFloatSlider probabilitySlider_a_GR;
	ofxFloatSlider probabilitySlider_a_GG;
	ofxFloatSlider probabilitySlider_a_GB;
	ofxFloatSlider probabilitySlider_a_GW;
	ofxFloatSlider probabilitySlider_a_GO;
	ofxFloatSlider probabilitySlider_a_GK;
	ofxFloatSlider probabilitySlider_a_GC;
	ofxFloatSlider probabilitySlider_a_GD;

	ofxFloatSlider probabilitySlider_r_GR;
	ofxFloatSlider probabilitySlider_r_GG;
	ofxFloatSlider probabilitySlider_r_GB;
	ofxFloatSlider probabilitySlider_r_GW;
	ofxFloatSlider probabilitySlider_r_GO;
	ofxFloatSlider probabilitySlider_r_GK;
	ofxFloatSlider probabilitySlider_r_GC;
	ofxFloatSlider probabilitySlider_r_GD;

	ofxFloatSlider probabilitySlider_a_BR;
	ofxFloatSlider probabilitySlider_a_BG;
	ofxFloatSlider probabilitySlider_a_BB;
	ofxFloatSlider probabilitySlider_a_BW;
	ofxFloatSlider probabilitySlider_a_BO;
	ofxFloatSlider probabilitySlider_a_BK;
	ofxFloatSlider probabilitySlider_a_BC;
	ofxFloatSlider probabilitySlider_a_BD;

	ofxFloatSlider probabilitySlider_r_BR;
	ofxFloatSlider probabilitySlider_r_BG;
	ofxFloatSlider probabilitySlider_r_BB;
	ofxFloatSlider probabilitySlider_r_BW;
	ofxFloatSlider probabilitySlider_r_BO;
	ofxFloatSlider probabilitySlider_r_BK;
	ofxFloatSlider probabilitySlider_r_BC;
	ofxFloatSlider probabilitySlider_r_BD;

	ofxFloatSlider probabilitySlider_a_WR;
	ofxFloatSlider probabilitySlider_a_WG;
	ofxFloatSlider probabilitySlider_a_WB;
	ofxFloatSlider probabilitySlider_a_WW;
	ofxFloatSlider probabilitySlider_a_WO;
	ofxFloatSlider probabilitySlider_a_WK;
	ofxFloatSlider probabilitySlider_a_WC;
	ofxFloatSlider probabilitySlider_a_WD;

	ofxFloatSlider probabilitySlider_r_WR;
	ofxFloatSlider probabilitySlider_r_WG;
	ofxFloatSlider probabilitySlider_r_WB;
	ofxFloatSlider probabilitySlider_r_WW;
	ofxFloatSlider probabilitySlider_r_WO;
	ofxFloatSlider probabilitySlider_r_WK;
	ofxFloatSlider probabilitySlider_r_WC;
	ofxFloatSlider probabilitySlider_r_WD;

	ofxFloatSlider probabilitySlider_a_OR;
	ofxFloatSlider probabilitySlider_a_OG;
	ofxFloatSlider probabilitySlider_a_OB;
	ofxFloatSlider probabilitySlider_a_OW;
	ofxFloatSlider probabilitySlider_a_OO;
	ofxFloatSlider probabilitySlider_a_OK;
	ofxFloatSlider probabilitySlider_a_OC;
	ofxFloatSlider probabilitySlider_a_OD;

	ofxFloatSlider probabilitySlider_r_OR;
	ofxFloatSlider probabilitySlider_r_OG;
	ofxFloatSlider probabilitySlider_r_OB;
	ofxFloatSlider probabilitySlider_r_OW;
	ofxFloatSlider probabilitySlider_r_OO;
	ofxFloatSlider probabilitySlider_r_OK;
	ofxFloatSlider probabilitySlider_r_OC;
	ofxFloatSlider probabilitySlider_r_OD;

	ofxFloatSlider probabilitySlider_a_KR;
	ofxFloatSlider probabilitySlider_a_KG;
	ofxFloatSlider probabilitySlider_a_KB;
	ofxFloatSlider probabilitySlider_a_KW;
	ofxFloatSlider probabilitySlider_a_KO;
	ofxFloatSlider probabilitySlider_a_KK;
	ofxFloatSlider probabilitySlider_a_KC;
	ofxFloatSlider probabilitySlider_a_KD;

	ofxFloatSlider probabilitySlider_r_KR;
	ofxFloatSlider probabilitySlider_r_KG;
	ofxFloatSlider probabilitySlider_r_KB;
	ofxFloatSlider probabilitySlider_r_KW;
	ofxFloatSlider probabilitySlider_r_KO;
	ofxFloatSlider probabilitySlider_r_KK;
	ofxFloatSlider probabilitySlider_r_KC;
	ofxFloatSlider probabilitySlider_r_KD;

	ofxFloatSlider probabilitySlider_a_CR;
	ofxFloatSlider probabilitySlider_a_CG;
	ofxFloatSlider probabilitySlider_a_CB;
	ofxFloatSlider probabilitySlider_a_CW;
	ofxFloatSlider probabilitySlider_a_CO;
	ofxFloatSlider probabilitySlider_a_CK;
	ofxFloatSlider probabilitySlider_a_CC;
	ofxFloatSlider probabilitySlider_a_CD;

	ofxFloatSlider probabilitySlider_r_CR;
	ofxFloatSlider probabilitySlider_r_CG;
	ofxFloatSlider probabilitySlider_r_CB;
	ofxFloatSlider probabilitySlider_r_CW;
	ofxFloatSlider probabilitySlider_r_CO;
	ofxFloatSlider probabilitySlider_r_CK;
	ofxFloatSlider probabilitySlider_r_CC;
	ofxFloatSlider probabilitySlider_r_CD;

	ofxFloatSlider probabilitySlider_a_DR;
	ofxFloatSlider probabilitySlider_a_DG;
	ofxFloatSlider probabilitySlider_a_DB;
	ofxFloatSlider probabilitySlider_a_DW;
	ofxFloatSlider probabilitySlider_a_DO;
	ofxFloatSlider probabilitySlider_a_DK;
	ofxFloatSlider probabilitySlider_a_DC;
	ofxFloatSlider probabilitySlider_a_DD;

	ofxFloatSlider probabilitySlider_r_DR;
	ofxFloatSlider probabilitySlider_r_DG;
	ofxFloatSlider probabilitySlider_r_DB;
	ofxFloatSlider probabilitySlider_r_DW;
	ofxFloatSlider probabilitySlider_r_DO;
	ofxFloatSlider probabilitySlider_r_DK;
	ofxFloatSlider probabilitySlider_r_DC;
	ofxFloatSlider probabilitySlider_r_DD;

	unsigned int probability = 90.00;

	unsigned int probability_a_RR = 90.00;
	unsigned int probability_a_RG = 95.00;
	unsigned int probability_a_RB = 100.00;
	unsigned int probability_a_RW = 80.00;
	unsigned int probability_a_RO = 85.00;
	unsigned int probability_a_RK = 70.00;
	unsigned int probability_a_RC = 75.00;
	unsigned int probability_a_RD = 65.00;

	unsigned int probability_r_RR = 90.00;
	unsigned int probability_r_RG = 95.00;
	unsigned int probability_r_RB = 100.00;
	unsigned int probability_r_RW = 80.00;
	unsigned int probability_r_RO = 85.00;
	unsigned int probability_r_RK = 70.00;
	unsigned int probability_r_RC = 75.00;
	unsigned int probability_r_RD = 65.00;

	unsigned int probability_a_GR = 90.00;
	unsigned int probability_a_GG = 95.00;
	unsigned int probability_a_GB = 100.00;
	unsigned int probability_a_GW = 80.00;
	unsigned int probability_a_GO = 85.00;
	unsigned int probability_a_GK = 70.00;
	unsigned int probability_a_GC = 75.00;
	unsigned int probability_a_GD = 65.00;

	unsigned int probability_r_GR = 90.00;
	unsigned int probability_r_GG = 95.00;
	unsigned int probability_r_GB = 100.00;
	unsigned int probability_r_GW = 80.00;
	unsigned int probability_r_GO = 85.00;
	unsigned int probability_r_GK = 70.00;
	unsigned int probability_r_GC = 75.00;
	unsigned int probability_r_GD = 65.00;

	unsigned int probability_a_BR = 90.00;
	unsigned int probability_a_BG = 95.00;
	unsigned int probability_a_BB = 100.00;
	unsigned int probability_a_BW = 80.00;
	unsigned int probability_a_BO = 85.00;
	unsigned int probability_a_BK = 70.00;
	unsigned int probability_a_BC = 75.00;
	unsigned int probability_a_BD = 65.00;

	unsigned int probability_r_BR = 90.00;
	unsigned int probability_r_BG = 95.00;
	unsigned int probability_r_BB = 100.00;
	unsigned int probability_r_BW = 80.00;
	unsigned int probability_r_BO = 85.00;
	unsigned int probability_r_BK = 70.00;
	unsigned int probability_r_BC = 75.00;
	unsigned int probability_r_BD = 65.00;

	unsigned int probability_a_WR = 90.00;
	unsigned int probability_a_WG = 95.00;
	unsigned int probability_a_WB = 100.00;
	unsigned int probability_a_WW = 80.00;
	unsigned int probability_a_WO = 85.00;
	unsigned int probability_a_WK = 70.00;
	unsigned int probability_a_WC = 75.00;
	unsigned int probability_a_WD = 65.00;

	unsigned int probability_r_WR = 90.00;
	unsigned int probability_r_WG = 95.00;
	unsigned int probability_r_WB = 100.00;
	unsigned int probability_r_WW = 80.00;
	unsigned int probability_r_WO = 85.00;
	unsigned int probability_r_WK = 70.00;
	unsigned int probability_r_WC = 75.00;
	unsigned int probability_r_WD = 65.00;

	unsigned int probability_a_OR = 90.00;
	unsigned int probability_a_OG = 95.00;
	unsigned int probability_a_OB = 100.00;
	unsigned int probability_a_OW = 80.00;
	unsigned int probability_a_OO = 85.00;
	unsigned int probability_a_OK = 70.00;
	unsigned int probability_a_OC = 75.00;
	unsigned int probability_a_OD = 65.00;

	unsigned int probability_r_OR = 90.00;
	unsigned int probability_r_OG = 95.00;
	unsigned int probability_r_OB = 100.00;
	unsigned int probability_r_OW = 80.00;
	unsigned int probability_r_OO = 85.00;
	unsigned int probability_r_OK = 70.00;
	unsigned int probability_r_OC = 75.00;
	unsigned int probability_r_OD = 65.00;

	unsigned int probability_a_KR = 90.00;
	unsigned int probability_a_KG = 95.00;
	unsigned int probability_a_KB = 100.00;
	unsigned int probability_a_KW = 80.00;
	unsigned int probability_a_KO = 85.00;
	unsigned int probability_a_KK = 70.00;
	unsigned int probability_a_KC = 75.00;
	unsigned int probability_a_KD = 65.00;

	unsigned int probability_r_KR = 90.00;
	unsigned int probability_r_KG = 95.00;
	unsigned int probability_r_KB = 100.00;
	unsigned int probability_r_KW = 80.00;
	unsigned int probability_r_KO = 85.00;
	unsigned int probability_r_KK = 70.00;
	unsigned int probability_r_KC = 75.00;
	unsigned int probability_r_KD = 65.00;

	unsigned int probability_a_CR = 90.00;
	unsigned int probability_a_CG = 95.00;
	unsigned int probability_a_CB = 100.00;
	unsigned int probability_a_CW = 80.00;
	unsigned int probability_a_CO = 85.00;
	unsigned int probability_a_CK = 70.00;
	unsigned int probability_a_CC = 75.00;
	unsigned int probability_a_CD = 65.00;

	unsigned int probability_r_CR = 90.00;
	unsigned int probability_r_CG = 95.00;
	unsigned int probability_r_CB = 100.00;
	unsigned int probability_r_CW = 80.00;
	unsigned int probability_r_CO = 85.00;
	unsigned int probability_r_CK = 70.00;
	unsigned int probability_r_CC = 75.00;
	unsigned int probability_r_CD = 65.00;

	unsigned int probability_a_DR = 90.00;
	unsigned int probability_a_DG = 95.00;
	unsigned int probability_a_DB = 100.00;
	unsigned int probability_a_DW = 80.00;
	unsigned int probability_a_DO = 85.00;
	unsigned int probability_a_DK = 70.00;
	unsigned int probability_a_DC = 75.00;
	unsigned int probability_a_DD = 65.00;

	unsigned int probability_r_DR = 90.00;
	unsigned int probability_r_DG = 95.00;
	unsigned int probability_r_DB = 100.00;
	unsigned int probability_r_DW = 80.00;
	unsigned int probability_r_DO = 85.00;
	unsigned int probability_r_DK = 70.00;
	unsigned int probability_r_DC = 75.00;
	unsigned int probability_r_DD = 65.00;

	ofxIntSlider numberSliderR;
	ofxIntSlider numberSliderG;
	ofxIntSlider numberSliderW;
	ofxIntSlider numberSliderB;
	ofxIntSlider numberSliderO;
	ofxIntSlider numberSliderK;
	ofxIntSlider numberSliderC;
	ofxIntSlider numberSliderD;

	int pnumberSliderR = 1000;
	int pnumberSliderG = 1000;
	int pnumberSliderW = 1000;
	int pnumberSliderB = 1000;
	int pnumberSliderO = 1000;
	int pnumberSliderK = 1000;
	int pnumberSliderC = 1000;
	int pnumberSliderD = 1000;

	ofxFloatSlider viscoSlider;
	ofxFloatSlider viscoSliderR;
	ofxFloatSlider viscoSliderG;
	ofxFloatSlider viscoSliderW;
	ofxFloatSlider viscoSliderB;
	ofxFloatSlider viscoSliderO;
	ofxFloatSlider viscoSliderK;
	ofxFloatSlider viscoSliderC;
	ofxFloatSlider viscoSliderD;

	ofxFloatSlider gravitySlider;
	ofxFloatSlider wallRepelSlider;

	ofxFloatSlider powerSlider_a_RR;
	ofxFloatSlider powerSlider_a_RG;
	ofxFloatSlider powerSlider_a_RW;
	ofxFloatSlider powerSlider_a_RB;
	ofxFloatSlider powerSlider_a_RO;
	ofxFloatSlider powerSlider_a_RK;
	ofxFloatSlider powerSlider_a_RC;
	ofxFloatSlider powerSlider_a_RD;

	float ppowerSlider_a_RR = 0;
	float ppowerSlider_a_RG = 0;
	float ppowerSlider_a_RW = 0;
	float ppowerSlider_a_RB = 0;
	float ppowerSlider_a_RO = 0;
	float ppowerSlider_a_RK = 0;
	float ppowerSlider_a_RC = 0;
	float ppowerSlider_a_RD = 0;

	ofxFloatSlider powerSlider_r_RR;
	ofxFloatSlider powerSlider_r_RG;
	ofxFloatSlider powerSlider_r_RW;
	ofxFloatSlider powerSlider_r_RB;
	ofxFloatSlider powerSlider_r_RO;
	ofxFloatSlider powerSlider_r_RK;
	ofxFloatSlider powerSlider_r_RC;
	ofxFloatSlider powerSlider_r_RD;

	float ppowerSlider_r_RR = 0;
	float ppowerSlider_r_RG = 0;
	float ppowerSlider_r_RW = 0;
	float ppowerSlider_r_RB = 0;
	float ppowerSlider_r_RO = 0;
	float ppowerSlider_r_RK = 0;
	float ppowerSlider_r_RC = 0;
	float ppowerSlider_r_RD = 0;

	ofxFloatSlider powerSlider_a_GR;
	ofxFloatSlider powerSlider_a_GG;
	ofxFloatSlider powerSlider_a_GW;
	ofxFloatSlider powerSlider_a_GB;
	ofxFloatSlider powerSlider_a_GO;
	ofxFloatSlider powerSlider_a_GK;
	ofxFloatSlider powerSlider_a_GC;
	ofxFloatSlider powerSlider_a_GD;

	float ppowerSlider_a_GR = 0;
	float ppowerSlider_a_GG = 0;
	float ppowerSlider_a_GW = 0;
	float ppowerSlider_a_GB = 0;
	float ppowerSlider_a_GO = 0;
	float ppowerSlider_a_GK = 0;
	float ppowerSlider_a_GC = 0;
	float ppowerSlider_a_GD = 0;

	ofxFloatSlider powerSlider_r_GR;
	ofxFloatSlider powerSlider_r_GG;
	ofxFloatSlider powerSlider_r_GW;
	ofxFloatSlider powerSlider_r_GB;
	ofxFloatSlider powerSlider_r_GO;
	ofxFloatSlider powerSlider_r_GK;
	ofxFloatSlider powerSlider_r_GC;
	ofxFloatSlider powerSlider_r_GD;

	float ppowerSlider_r_GR = 0;
	float ppowerSlider_r_GG = 0;
	float ppowerSlider_r_GW = 0;
	float ppowerSlider_r_GB = 0;
	float ppowerSlider_r_GO = 0;
	float ppowerSlider_r_GK = 0;
	float ppowerSlider_r_GC = 0;
	float ppowerSlider_r_GD = 0;

	ofxFloatSlider powerSlider_a_WR;
	ofxFloatSlider powerSlider_a_WG;
	ofxFloatSlider powerSlider_a_WW;
	ofxFloatSlider powerSlider_a_WB;
	ofxFloatSlider powerSlider_a_WO;
	ofxFloatSlider powerSlider_a_WK;
	ofxFloatSlider powerSlider_a_WC;
	ofxFloatSlider powerSlider_a_WD;

	float ppowerSlider_a_WR = 0;
	float ppowerSlider_a_WG = 0;
	float ppowerSlider_a_WW = 0;
	float ppowerSlider_a_WB = 0;
	float ppowerSlider_a_WO = 0;
	float ppowerSlider_a_WK = 0;
	float ppowerSlider_a_WC = 0;
	float ppowerSlider_a_WD = 0;

	ofxFloatSlider powerSlider_r_WR;
	ofxFloatSlider powerSlider_r_WG;
	ofxFloatSlider powerSlider_r_WW;
	ofxFloatSlider powerSlider_r_WB;
	ofxFloatSlider powerSlider_r_WO;
	ofxFloatSlider powerSlider_r_WK;
	ofxFloatSlider powerSlider_r_WC;
	ofxFloatSlider powerSlider_r_WD;

	float ppowerSlider_r_WR = 0;
	float ppowerSlider_r_WG = 0;
	float ppowerSlider_r_WW = 0;
	float ppowerSlider_r_WB = 0;
	float ppowerSlider_r_WO = 0;
	float ppowerSlider_r_WK = 0;
	float ppowerSlider_r_WC = 0;
	float ppowerSlider_r_WD = 0;

	ofxFloatSlider powerSlider_a_BR;
	ofxFloatSlider powerSlider_a_BG;
	ofxFloatSlider powerSlider_a_BW;
	ofxFloatSlider powerSlider_a_BB;
	ofxFloatSlider powerSlider_a_BO;
	ofxFloatSlider powerSlider_a_BK;
	ofxFloatSlider powerSlider_a_BC;
	ofxFloatSlider powerSlider_a_BD;

	float ppowerSlider_a_BR = 0;
	float ppowerSlider_a_BG = 0;
	float ppowerSlider_a_BW = 0;
	float ppowerSlider_a_BB = 0;
	float ppowerSlider_a_BO = 0;
	float ppowerSlider_a_BK = 0;
	float ppowerSlider_a_BC = 0;
	float ppowerSlider_a_BD = 0;

	ofxFloatSlider powerSlider_r_BR;
	ofxFloatSlider powerSlider_r_BG;
	ofxFloatSlider powerSlider_r_BW;
	ofxFloatSlider powerSlider_r_BB;
	ofxFloatSlider powerSlider_r_BO;
	ofxFloatSlider powerSlider_r_BK;
	ofxFloatSlider powerSlider_r_BC;
	ofxFloatSlider powerSlider_r_BD;

	float ppowerSlider_r_BR = 0;
	float ppowerSlider_r_BG = 0;
	float ppowerSlider_r_BW = 0;
	float ppowerSlider_r_BB = 0;
	float ppowerSlider_r_BO = 0;
	float ppowerSlider_r_BK = 0;
	float ppowerSlider_r_BC = 0;
	float ppowerSlider_r_BD = 0;

	ofxFloatSlider powerSlider_a_OR;
	ofxFloatSlider powerSlider_a_OG;
	ofxFloatSlider powerSlider_a_OW;
	ofxFloatSlider powerSlider_a_OB;
	ofxFloatSlider powerSlider_a_OO;
	ofxFloatSlider powerSlider_a_OK;
	ofxFloatSlider powerSlider_a_OC;
	ofxFloatSlider powerSlider_a_OD;

	float ppowerSlider_a_OR = 0;
	float ppowerSlider_a_OG = 0;
	float ppowerSlider_a_OW = 0;
	float ppowerSlider_a_OB = 0;
	float ppowerSlider_a_OO = 0;
	float ppowerSlider_a_OK = 0;
	float ppowerSlider_a_OC = 0;
	float ppowerSlider_a_OD = 0;

	ofxFloatSlider powerSlider_r_OR;
	ofxFloatSlider powerSlider_r_OG;
	ofxFloatSlider powerSlider_r_OW;
	ofxFloatSlider powerSlider_r_OB;
	ofxFloatSlider powerSlider_r_OO;
	ofxFloatSlider powerSlider_r_OK;
	ofxFloatSlider powerSlider_r_OC;
	ofxFloatSlider powerSlider_r_OD;

	float ppowerSlider_r_OR = 0;
	float ppowerSlider_r_OG = 0;
	float ppowerSlider_r_OW = 0;
	float ppowerSlider_r_OB = 0;
	float ppowerSlider_r_OO = 0;
	float ppowerSlider_r_OK = 0;
	float ppowerSlider_r_OC = 0;
	float ppowerSlider_r_OD = 0;

	ofxFloatSlider powerSlider_a_KR;
	ofxFloatSlider powerSlider_a_KG;
	ofxFloatSlider powerSlider_a_KW;
	ofxFloatSlider powerSlider_a_KB;
	ofxFloatSlider powerSlider_a_KO;
	ofxFloatSlider powerSlider_a_KK;
	ofxFloatSlider powerSlider_a_KC;
	ofxFloatSlider powerSlider_a_KD;

	float ppowerSlider_a_KR = 0;
	float ppowerSlider_a_KG = 0;
	float ppowerSlider_a_KW = 0;
	float ppowerSlider_a_KB = 0;
	float ppowerSlider_a_KO = 0;
	float ppowerSlider_a_KK = 0;
	float ppowerSlider_a_KC = 0;
	float ppowerSlider_a_KD = 0;

	ofxFloatSlider powerSlider_r_KR;
	ofxFloatSlider powerSlider_r_KG;
	ofxFloatSlider powerSlider_r_KW;
	ofxFloatSlider powerSlider_r_KB;
	ofxFloatSlider powerSlider_r_KO;
	ofxFloatSlider powerSlider_r_KK;
	ofxFloatSlider powerSlider_r_KC;
	ofxFloatSlider powerSlider_r_KD;

	float ppowerSlider_r_KR = 0;
	float ppowerSlider_r_KG = 0;
	float ppowerSlider_r_KW = 0;
	float ppowerSlider_r_KB = 0;
	float ppowerSlider_r_KO = 0;
	float ppowerSlider_r_KK = 0;
	float ppowerSlider_r_KC = 0;
	float ppowerSlider_r_KD = 0;

	ofxFloatSlider powerSlider_a_CR;
	ofxFloatSlider powerSlider_a_CG;
	ofxFloatSlider powerSlider_a_CW;
	ofxFloatSlider powerSlider_a_CB;
	ofxFloatSlider powerSlider_a_CO;
	ofxFloatSlider powerSlider_a_CK;
	ofxFloatSlider powerSlider_a_CC;
	ofxFloatSlider powerSlider_a_CD;

	float ppowerSlider_a_CR = 0;
	float ppowerSlider_a_CG = 0;
	float ppowerSlider_a_CW = 0;
	float ppowerSlider_a_CB = 0;
	float ppowerSlider_a_CO = 0;
	float ppowerSlider_a_CK = 0;
	float ppowerSlider_a_CC = 0;
	float ppowerSlider_a_CD = 0;

	ofxFloatSlider powerSlider_r_CR;
	ofxFloatSlider powerSlider_r_CG;
	ofxFloatSlider powerSlider_r_CW;
	ofxFloatSlider powerSlider_r_CB;
	ofxFloatSlider powerSlider_r_CO;
	ofxFloatSlider powerSlider_r_CK;
	ofxFloatSlider powerSlider_r_CC;
	ofxFloatSlider powerSlider_r_CD;

	float ppowerSlider_r_CR = 0;
	float ppowerSlider_r_CG = 0;
	float ppowerSlider_r_CW = 0;
	float ppowerSlider_r_CB = 0;
	float ppowerSlider_r_CO = 0;
	float ppowerSlider_r_CK = 0;
	float ppowerSlider_r_CC = 0;
	float ppowerSlider_r_CD = 0;

	ofxFloatSlider powerSlider_a_DR;
	ofxFloatSlider powerSlider_a_DG;
	ofxFloatSlider powerSlider_a_DW;
	ofxFloatSlider powerSlider_a_DB;
	ofxFloatSlider powerSlider_a_DO;
	ofxFloatSlider powerSlider_a_DK;
	ofxFloatSlider powerSlider_a_DC;
	ofxFloatSlider powerSlider_a_DD;

	float ppowerSlider_a_DR = 0;
	float ppowerSlider_a_DG = 0;
	float ppowerSlider_a_DW = 0;
	float ppowerSlider_a_DB = 0;
	float ppowerSlider_a_DO = 0;
	float ppowerSlider_a_DK = 0;
	float ppowerSlider_a_DC = 0;
	float ppowerSlider_a_DD = 0;

	ofxFloatSlider powerSlider_r_DR;
	ofxFloatSlider powerSlider_r_DG;
	ofxFloatSlider powerSlider_r_DW;
	ofxFloatSlider powerSlider_r_DB;
	ofxFloatSlider powerSlider_r_DO;
	ofxFloatSlider powerSlider_r_DK;
	ofxFloatSlider powerSlider_r_DC;
	ofxFloatSlider powerSlider_r_DD;

	float ppowerSlider_r_DR = 0;
	float ppowerSlider_r_DG = 0;
	float ppowerSlider_r_DW = 0;
	float ppowerSlider_r_DB = 0;
	float ppowerSlider_r_DO = 0;
	float ppowerSlider_r_DK = 0;
	float ppowerSlider_r_DC = 0;
	float ppowerSlider_r_DD = 0;

	ofxFloatSlider vSlider_a_RR;
	ofxFloatSlider vSlider_a_RG;
	ofxFloatSlider vSlider_a_RW;
	ofxFloatSlider vSlider_a_RB;
	ofxFloatSlider vSlider_a_RO;
	ofxFloatSlider vSlider_a_RK;
	ofxFloatSlider vSlider_a_RC;
	ofxFloatSlider vSlider_a_RD;

	float pvSlider_a_RR = 0;
	float pvSlider_a_RG = 0;
	float pvSlider_a_RW = 0;
	float pvSlider_a_RB = 0;
	float pvSlider_a_RO = 0;
	float pvSlider_a_RK = 0;
	float pvSlider_a_RC = 0;
	float pvSlider_a_RD = 0;

	ofxFloatSlider vSlider_r_RR;
	ofxFloatSlider vSlider_r_RG;
	ofxFloatSlider vSlider_r_RW;
	ofxFloatSlider vSlider_r_RB;
	ofxFloatSlider vSlider_r_RO;
	ofxFloatSlider vSlider_r_RK;
	ofxFloatSlider vSlider_r_RC;
	ofxFloatSlider vSlider_r_RD;

	float pvSlider_r_RR = 0;
	float pvSlider_r_RG = 0;
	float pvSlider_r_RW = 0;
	float pvSlider_r_RB = 0;
	float pvSlider_r_RO = 0;
	float pvSlider_r_RK = 0;
	float pvSlider_r_RC = 0;
	float pvSlider_r_RD = 0;

	ofxFloatSlider vSlider_a_GR;
	ofxFloatSlider vSlider_a_GG;
	ofxFloatSlider vSlider_a_GW;
	ofxFloatSlider vSlider_a_GB;
	ofxFloatSlider vSlider_a_GO;
	ofxFloatSlider vSlider_a_GK;
	ofxFloatSlider vSlider_a_GC;
	ofxFloatSlider vSlider_a_GD;

	float pvSlider_a_GR = 0;
	float pvSlider_a_GG = 0;
	float pvSlider_a_GW = 0;
	float pvSlider_a_GB = 0;
	float pvSlider_a_GO = 0;
	float pvSlider_a_GK = 0;
	float pvSlider_a_GC = 0;
	float pvSlider_a_GD = 0;

	ofxFloatSlider vSlider_r_GR;
	ofxFloatSlider vSlider_r_GG;
	ofxFloatSlider vSlider_r_GW;
	ofxFloatSlider vSlider_r_GB;
	ofxFloatSlider vSlider_r_GO;
	ofxFloatSlider vSlider_r_GK;
	ofxFloatSlider vSlider_r_GC;
	ofxFloatSlider vSlider_r_GD;

	float pvSlider_r_GR = 0;
	float pvSlider_r_GG = 0;
	float pvSlider_r_GW = 0;
	float pvSlider_r_GB = 0;
	float pvSlider_r_GO = 0;
	float pvSlider_r_GK = 0;
	float pvSlider_r_GC = 0;
	float pvSlider_r_GD = 0;

	ofxFloatSlider vSlider_a_WR;
	ofxFloatSlider vSlider_a_WG;
	ofxFloatSlider vSlider_a_WW;
	ofxFloatSlider vSlider_a_WB;
	ofxFloatSlider vSlider_a_WO;
	ofxFloatSlider vSlider_a_WK;
	ofxFloatSlider vSlider_a_WC;
	ofxFloatSlider vSlider_a_WD;

	float pvSlider_a_WR = 0;
	float pvSlider_a_WG = 0;
	float pvSlider_a_WW = 0;
	float pvSlider_a_WB = 0;
	float pvSlider_a_WO = 0;
	float pvSlider_a_WK = 0;
	float pvSlider_a_WC = 0;
	float pvSlider_a_WD = 0;

	ofxFloatSlider vSlider_r_WR;
	ofxFloatSlider vSlider_r_WG;
	ofxFloatSlider vSlider_r_WW;
	ofxFloatSlider vSlider_r_WB;
	ofxFloatSlider vSlider_r_WO;
	ofxFloatSlider vSlider_r_WK;
	ofxFloatSlider vSlider_r_WC;
	ofxFloatSlider vSlider_r_WD;

	float pvSlider_r_WR = 0;
	float pvSlider_r_WG = 0;
	float pvSlider_r_WW = 0;
	float pvSlider_r_WB = 0;
	float pvSlider_r_WO = 0;
	float pvSlider_r_WK = 0;
	float pvSlider_r_WC = 0;
	float pvSlider_r_WD = 0;

	ofxFloatSlider vSlider_a_BR;
	ofxFloatSlider vSlider_a_BG;
	ofxFloatSlider vSlider_a_BW;
	ofxFloatSlider vSlider_a_BB;
	ofxFloatSlider vSlider_a_BO;
	ofxFloatSlider vSlider_a_BK;
	ofxFloatSlider vSlider_a_BC;
	ofxFloatSlider vSlider_a_BD;

	float pvSlider_a_BR = 0;
	float pvSlider_a_BG = 0;
	float pvSlider_a_BW = 0;
	float pvSlider_a_BB = 0;
	float pvSlider_a_BO = 0;
	float pvSlider_a_BK = 0;
	float pvSlider_a_BC = 0;
	float pvSlider_a_BD = 0;

	ofxFloatSlider vSlider_r_BR;
	ofxFloatSlider vSlider_r_BG;
	ofxFloatSlider vSlider_r_BW;
	ofxFloatSlider vSlider_r_BB;
	ofxFloatSlider vSlider_r_BO;
	ofxFloatSlider vSlider_r_BK;
	ofxFloatSlider vSlider_r_BC;
	ofxFloatSlider vSlider_r_BD;

	float pvSlider_r_BR = 0;
	float pvSlider_r_BG = 0;
	float pvSlider_r_BW = 0;
	float pvSlider_r_BB = 0;
	float pvSlider_r_BO = 0;
	float pvSlider_r_BK = 0;
	float pvSlider_r_BC = 0;
	float pvSlider_r_BD = 0;

	ofxFloatSlider vSlider_a_OR;
	ofxFloatSlider vSlider_a_OG;
	ofxFloatSlider vSlider_a_OW;
	ofxFloatSlider vSlider_a_OB;
	ofxFloatSlider vSlider_a_OO;
	ofxFloatSlider vSlider_a_OK;
	ofxFloatSlider vSlider_a_OC;
	ofxFloatSlider vSlider_a_OD;

	float pvSlider_a_OR = 0;
	float pvSlider_a_OG = 0;
	float pvSlider_a_OW = 0;
	float pvSlider_a_OB = 0;
	float pvSlider_a_OO = 0;
	float pvSlider_a_OK = 0;
	float pvSlider_a_OC = 0;
	float pvSlider_a_OD = 0;

	ofxFloatSlider vSlider_r_OR;
	ofxFloatSlider vSlider_r_OG;
	ofxFloatSlider vSlider_r_OW;
	ofxFloatSlider vSlider_r_OB;
	ofxFloatSlider vSlider_r_OO;
	ofxFloatSlider vSlider_r_OK;
	ofxFloatSlider vSlider_r_OC;
	ofxFloatSlider vSlider_r_OD;

	float pvSlider_r_OR = 0;
	float pvSlider_r_OG = 0;
	float pvSlider_r_OW = 0;
	float pvSlider_r_OB = 0;
	float pvSlider_r_OO = 0;
	float pvSlider_r_OK = 0;
	float pvSlider_r_OC = 0;
	float pvSlider_r_OD = 0;

	ofxFloatSlider vSlider_a_KR;
	ofxFloatSlider vSlider_a_KG;
	ofxFloatSlider vSlider_a_KW;
	ofxFloatSlider vSlider_a_KB;
	ofxFloatSlider vSlider_a_KO;
	ofxFloatSlider vSlider_a_KK;
	ofxFloatSlider vSlider_a_KC;
	ofxFloatSlider vSlider_a_KD;

	float pvSlider_a_KR = 0;
	float pvSlider_a_KG = 0;
	float pvSlider_a_KW = 0;
	float pvSlider_a_KB = 0;
	float pvSlider_a_KO = 0;
	float pvSlider_a_KK = 0;
	float pvSlider_a_KC = 0;
	float pvSlider_a_KD = 0;

	ofxFloatSlider vSlider_r_KR;
	ofxFloatSlider vSlider_r_KG;
	ofxFloatSlider vSlider_r_KW;
	ofxFloatSlider vSlider_r_KB;
	ofxFloatSlider vSlider_r_KO;
	ofxFloatSlider vSlider_r_KK;
	ofxFloatSlider vSlider_r_KC;
	ofxFloatSlider vSlider_r_KD;

	float pvSlider_r_KR = 0;
	float pvSlider_r_KG = 0;
	float pvSlider_r_KW = 0;
	float pvSlider_r_KB = 0;
	float pvSlider_r_KO = 0;
	float pvSlider_r_KK = 0;
	float pvSlider_r_KC = 0;
	float pvSlider_r_KD = 0;

	ofxFloatSlider vSlider_a_CR;
	ofxFloatSlider vSlider_a_CG;
	ofxFloatSlider vSlider_a_CW;
	ofxFloatSlider vSlider_a_CB;
	ofxFloatSlider vSlider_a_CO;
	ofxFloatSlider vSlider_a_CK;
	ofxFloatSlider vSlider_a_CC;
	ofxFloatSlider vSlider_a_CD;

	float pvSlider_a_CR = 0;
	float pvSlider_a_CG = 0;
	float pvSlider_a_CW = 0;
	float pvSlider_a_CB = 0;
	float pvSlider_a_CO = 0;
	float pvSlider_a_CK = 0;
	float pvSlider_a_CC = 0;
	float pvSlider_a_CD = 0;

	ofxFloatSlider vSlider_r_CR;
	ofxFloatSlider vSlider_r_CG;
	ofxFloatSlider vSlider_r_CW;
	ofxFloatSlider vSlider_r_CB;
	ofxFloatSlider vSlider_r_CO;
	ofxFloatSlider vSlider_r_CK;
	ofxFloatSlider vSlider_r_CC;
	ofxFloatSlider vSlider_r_CD;

	float pvSlider_r_CR = 0;
	float pvSlider_r_CG = 0;
	float pvSlider_r_CW = 0;
	float pvSlider_r_CB = 0;
	float pvSlider_r_CO = 0;
	float pvSlider_r_CK = 0;
	float pvSlider_r_CC = 0;
	float pvSlider_r_CD = 0;

	ofxFloatSlider vSlider_a_DR;
	ofxFloatSlider vSlider_a_DG;
	ofxFloatSlider vSlider_a_DW;
	ofxFloatSlider vSlider_a_DB;
	ofxFloatSlider vSlider_a_DO;
	ofxFloatSlider vSlider_a_DK;
	ofxFloatSlider vSlider_a_DC;
	ofxFloatSlider vSlider_a_DD;

	float pvSlider_a_DR = 0;
	float pvSlider_a_DG = 0;
	float pvSlider_a_DW = 0;
	float pvSlider_a_DB = 0;
	float pvSlider_a_DO = 0;
	float pvSlider_a_DK = 0;
	float pvSlider_a_DC = 0;
	float pvSlider_a_DD = 0;

	ofxFloatSlider vSlider_r_DR;
	ofxFloatSlider vSlider_r_DG;
	ofxFloatSlider vSlider_r_DW;
	ofxFloatSlider vSlider_r_DB;
	ofxFloatSlider vSlider_r_DO;
	ofxFloatSlider vSlider_r_DK;
	ofxFloatSlider vSlider_r_DC;
	ofxFloatSlider vSlider_r_DD;

	float pvSlider_r_DR = 0;
	float pvSlider_r_DG = 0;
	float pvSlider_r_DW = 0;
	float pvSlider_r_DB = 0;
	float pvSlider_r_DO = 0;
	float pvSlider_r_DK = 0;
	float pvSlider_r_DC = 0;
	float pvSlider_r_DD = 0;

	ofxLabel labelG;
	ofxLabel labelR;
	ofxLabel labelW;
	ofxLabel labelB;
	ofxLabel labelO;
	ofxLabel labelK;
	ofxLabel labelC;
	ofxLabel labelD;
	ofxLabel aboutL1;
	ofxLabel aboutL2;
	ofxLabel aboutL3;
	ofxLabel fps;

	// simulation bounds
	int boundWidth = 1600;
	int boundHeight = 900;

	float viscosity = 0.5F;
	float viscosityR = 0.2F;
	float viscosityG = 0.3F;
	float viscosityW = 0.4F;
	float viscosityB = 0.5F;
	float viscosityO = 0.6F;
	float viscosityK = 0.7F;
	float viscosityC = 0.8F;
	float viscosityD = 0.9F;

	float forceVarianceA = 0.7F;
	float radiusVarianceA = 0.5F;
	float forceVarianceR = 0.8F;
	float radiusVarianceR = 0.6F;

	float wallRepel = 20.0F;
	float worldGravity = 0.0F;
};
