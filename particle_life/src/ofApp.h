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
	void interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float G, float radius, float viscosity, float probability);

	ofxPanel gui;

	ofxGuiGroup globalGroup;
	ofxGuiGroup rndGroup;
	ofxGuiGroup viscosityRGroup;
	ofxGuiGroup viscosityGGroup;
	ofxGuiGroup viscosityBGroup;
	ofxGuiGroup viscosityWGroup;
	ofxGuiGroup viscosityOGroup;
	ofxGuiGroup viscosityKGroup;
	ofxGuiGroup viscosityCGroup;
	ofxGuiGroup viscosityDGroup;
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

	ofxFloatSlider viscositySlider;
	ofxFloatSlider viscositySliderRR;
	ofxFloatSlider viscositySliderRG;
	ofxFloatSlider viscositySliderRB;
	ofxFloatSlider viscositySliderRW;
	ofxFloatSlider viscositySliderRO;
	ofxFloatSlider viscositySliderRK;
	ofxFloatSlider viscositySliderRC;
	ofxFloatSlider viscositySliderRD;

	ofxFloatSlider viscositySliderGR;
	ofxFloatSlider viscositySliderGG;
	ofxFloatSlider viscositySliderGB;
	ofxFloatSlider viscositySliderGW;
	ofxFloatSlider viscositySliderGO;
	ofxFloatSlider viscositySliderGK;
	ofxFloatSlider viscositySliderGC;
	ofxFloatSlider viscositySliderGD;

	ofxFloatSlider viscositySliderBR;
	ofxFloatSlider viscositySliderBG;
	ofxFloatSlider viscositySliderBB;
	ofxFloatSlider viscositySliderBW;
	ofxFloatSlider viscositySliderBO;
	ofxFloatSlider viscositySliderBK;
	ofxFloatSlider viscositySliderBC;
	ofxFloatSlider viscositySliderBD;

	ofxFloatSlider viscositySliderWR;
	ofxFloatSlider viscositySliderWG;
	ofxFloatSlider viscositySliderWB;
	ofxFloatSlider viscositySliderWW;
	ofxFloatSlider viscositySliderWO;
	ofxFloatSlider viscositySliderWK;
	ofxFloatSlider viscositySliderWC;
	ofxFloatSlider viscositySliderWD;

	ofxFloatSlider viscositySliderOR;
	ofxFloatSlider viscositySliderOG;
	ofxFloatSlider viscositySliderOB;
	ofxFloatSlider viscositySliderOW;
	ofxFloatSlider viscositySliderOO;
	ofxFloatSlider viscositySliderOK;
	ofxFloatSlider viscositySliderOC;
	ofxFloatSlider viscositySliderOD;

	ofxFloatSlider viscositySliderKR;
	ofxFloatSlider viscositySliderKG;
	ofxFloatSlider viscositySliderKB;
	ofxFloatSlider viscositySliderKW;
	ofxFloatSlider viscositySliderKO;
	ofxFloatSlider viscositySliderKK;
	ofxFloatSlider viscositySliderKC;
	ofxFloatSlider viscositySliderKD;

	ofxFloatSlider viscositySliderCR;
	ofxFloatSlider viscositySliderCG;
	ofxFloatSlider viscositySliderCB;
	ofxFloatSlider viscositySliderCW;
	ofxFloatSlider viscositySliderCO;
	ofxFloatSlider viscositySliderCK;
	ofxFloatSlider viscositySliderCC;
	ofxFloatSlider viscositySliderCD;

	ofxFloatSlider viscositySliderDR;
	ofxFloatSlider viscositySliderDG;
	ofxFloatSlider viscositySliderDB;
	ofxFloatSlider viscositySliderDW;
	ofxFloatSlider viscositySliderDO;
	ofxFloatSlider viscositySliderDK;
	ofxFloatSlider viscositySliderDC;
	ofxFloatSlider viscositySliderDD;

	ofxFloatSlider probabilitySlider;
	ofxFloatSlider probabilitySliderRR;
	ofxFloatSlider probabilitySliderRG;
	ofxFloatSlider probabilitySliderRB;
	ofxFloatSlider probabilitySliderRW;
	ofxFloatSlider probabilitySliderRO;
	ofxFloatSlider probabilitySliderRK;
	ofxFloatSlider probabilitySliderRC;
	ofxFloatSlider probabilitySliderRD;

	ofxFloatSlider probabilitySliderGR;
	ofxFloatSlider probabilitySliderGG;
	ofxFloatSlider probabilitySliderGB;
	ofxFloatSlider probabilitySliderGW;
	ofxFloatSlider probabilitySliderGO;
	ofxFloatSlider probabilitySliderGK;
	ofxFloatSlider probabilitySliderGC;
	ofxFloatSlider probabilitySliderGD;

	ofxFloatSlider probabilitySliderBR;
	ofxFloatSlider probabilitySliderBG;
	ofxFloatSlider probabilitySliderBB;
	ofxFloatSlider probabilitySliderBW;
	ofxFloatSlider probabilitySliderBO;
	ofxFloatSlider probabilitySliderBK;
	ofxFloatSlider probabilitySliderBC;
	ofxFloatSlider probabilitySliderBD;

	ofxFloatSlider probabilitySliderWR;
	ofxFloatSlider probabilitySliderWG;
	ofxFloatSlider probabilitySliderWB;
	ofxFloatSlider probabilitySliderWW;
	ofxFloatSlider probabilitySliderWO;
	ofxFloatSlider probabilitySliderWK;
	ofxFloatSlider probabilitySliderWC;
	ofxFloatSlider probabilitySliderWD;

	ofxFloatSlider probabilitySliderOR;
	ofxFloatSlider probabilitySliderOG;
	ofxFloatSlider probabilitySliderOB;
	ofxFloatSlider probabilitySliderOW;
	ofxFloatSlider probabilitySliderOO;
	ofxFloatSlider probabilitySliderOK;
	ofxFloatSlider probabilitySliderOC;
	ofxFloatSlider probabilitySliderOD;

	ofxFloatSlider probabilitySliderKR;
	ofxFloatSlider probabilitySliderKG;
	ofxFloatSlider probabilitySliderKB;
	ofxFloatSlider probabilitySliderKW;
	ofxFloatSlider probabilitySliderKO;
	ofxFloatSlider probabilitySliderKK;
	ofxFloatSlider probabilitySliderKC;
	ofxFloatSlider probabilitySliderKD;

	ofxFloatSlider probabilitySliderCR;
	ofxFloatSlider probabilitySliderCG;
	ofxFloatSlider probabilitySliderCB;
	ofxFloatSlider probabilitySliderCW;
	ofxFloatSlider probabilitySliderCO;
	ofxFloatSlider probabilitySliderCK;
	ofxFloatSlider probabilitySliderCC;
	ofxFloatSlider probabilitySliderCD;

	ofxFloatSlider probabilitySliderDR;
	ofxFloatSlider probabilitySliderDG;
	ofxFloatSlider probabilitySliderDB;
	ofxFloatSlider probabilitySliderDW;
	ofxFloatSlider probabilitySliderDO;
	ofxFloatSlider probabilitySliderDK;
	ofxFloatSlider probabilitySliderDC;
	ofxFloatSlider probabilitySliderDD;

	unsigned int probability = 90.00;
	unsigned int probabilityRR = 90.00;
	unsigned int probabilityRG = 95.00;
	unsigned int probabilityRB = 100.00;
	unsigned int probabilityRW = 80.00;
	unsigned int probabilityRO = 85.00;
	unsigned int probabilityRK = 70.00;
	unsigned int probabilityRC = 75.00;
	unsigned int probabilityRD = 65.00;

	unsigned int probabilityGR = 90.00;
	unsigned int probabilityGG = 95.00;
	unsigned int probabilityGB = 100.00;
	unsigned int probabilityGW = 80.00;
	unsigned int probabilityGO = 85.00;
	unsigned int probabilityGK = 70.00;
	unsigned int probabilityGC = 75.00;
	unsigned int probabilityGD = 65.00;

	unsigned int probabilityBR = 90.00;
	unsigned int probabilityBG = 95.00;
	unsigned int probabilityBB = 100.00;
	unsigned int probabilityBW = 80.00;
	unsigned int probabilityBO = 85.00;
	unsigned int probabilityBK = 70.00;
	unsigned int probabilityBC = 75.00;
	unsigned int probabilityBD = 65.00;

	unsigned int probabilityWR = 90.00;
	unsigned int probabilityWG = 95.00;
	unsigned int probabilityWB = 100.00;
	unsigned int probabilityWW = 80.00;
	unsigned int probabilityWO = 85.00;
	unsigned int probabilityWK = 70.00;
	unsigned int probabilityWC = 75.00;
	unsigned int probabilityWD = 65.00;

	unsigned int probabilityOR = 90.00;
	unsigned int probabilityOG = 95.00;
	unsigned int probabilityOB = 100.00;
	unsigned int probabilityOW = 80.00;
	unsigned int probabilityOO = 85.00;
	unsigned int probabilityOK = 70.00;
	unsigned int probabilityOC = 75.00;
	unsigned int probabilityOD = 65.00;

	unsigned int probabilityKR = 90.00;
	unsigned int probabilityKG = 95.00;
	unsigned int probabilityKB = 100.00;
	unsigned int probabilityKW = 80.00;
	unsigned int probabilityKO = 85.00;
	unsigned int probabilityKK = 70.00;
	unsigned int probabilityKC = 75.00;
	unsigned int probabilityKD = 65.00;

	unsigned int probabilityCR = 90.00;
	unsigned int probabilityCG = 95.00;
	unsigned int probabilityCB = 100.00;
	unsigned int probabilityCW = 80.00;
	unsigned int probabilityCO = 85.00;
	unsigned int probabilityCK = 70.00;
	unsigned int probabilityCC = 75.00;
	unsigned int probabilityCD = 65.00;

	unsigned int probabilityDR = 90.00;
	unsigned int probabilityDG = 95.00;
	unsigned int probabilityDB = 100.00;
	unsigned int probabilityDW = 80.00;
	unsigned int probabilityDO = 85.00;
	unsigned int probabilityDK = 70.00;
	unsigned int probabilityDC = 75.00;
	unsigned int probabilityDD = 65.00;

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

	ofxFloatSlider gravitySlider;
	ofxFloatSlider wallRepelSlider;

	ofxFloatSlider powerSliderRR;
	ofxFloatSlider powerSliderRG;
	ofxFloatSlider powerSliderRW;
	ofxFloatSlider powerSliderRB;
	ofxFloatSlider powerSliderRO;
	ofxFloatSlider powerSliderRK;
	ofxFloatSlider powerSliderRC;
	ofxFloatSlider powerSliderRD;

	float ppowerSliderRR = 0;
	float ppowerSliderRG = 0;
	float ppowerSliderRW = 0;
	float ppowerSliderRB = 0;
	float ppowerSliderRO = 0;
	float ppowerSliderRK = 0;
	float ppowerSliderRC = 0;
	float ppowerSliderRD = 0;

	ofxFloatSlider powerSliderGR;
	ofxFloatSlider powerSliderGG;
	ofxFloatSlider powerSliderGW;
	ofxFloatSlider powerSliderGB;
	ofxFloatSlider powerSliderGO;
	ofxFloatSlider powerSliderGK;
	ofxFloatSlider powerSliderGC;
	ofxFloatSlider powerSliderGD;

	float ppowerSliderGR = 0;
	float ppowerSliderGG = 0;
	float ppowerSliderGW = 0;
	float ppowerSliderGB = 0;
	float ppowerSliderGO = 0;
	float ppowerSliderGK = 0;
	float ppowerSliderGC = 0;
	float ppowerSliderGD = 0;

	ofxFloatSlider powerSliderWR;
	ofxFloatSlider powerSliderWG;
	ofxFloatSlider powerSliderWW;
	ofxFloatSlider powerSliderWB;
	ofxFloatSlider powerSliderWO;
	ofxFloatSlider powerSliderWK;
	ofxFloatSlider powerSliderWC;
	ofxFloatSlider powerSliderWD;

	float ppowerSliderWR = 0;
	float ppowerSliderWG = 0;
	float ppowerSliderWW = 0;
	float ppowerSliderWB = 0;
	float ppowerSliderWO = 0;
	float ppowerSliderWK = 0;
	float ppowerSliderWC = 0;
	float ppowerSliderWD = 0;

	ofxFloatSlider powerSliderBR;
	ofxFloatSlider powerSliderBG;
	ofxFloatSlider powerSliderBW;
	ofxFloatSlider powerSliderBB;
	ofxFloatSlider powerSliderBO;
	ofxFloatSlider powerSliderBK;
	ofxFloatSlider powerSliderBC;
	ofxFloatSlider powerSliderBD;

	float ppowerSliderBR = 0;
	float ppowerSliderBG = 0;
	float ppowerSliderBW = 0;
	float ppowerSliderBB = 0;
	float ppowerSliderBO = 0;
	float ppowerSliderBK = 0;
	float ppowerSliderBC = 0;
	float ppowerSliderBD = 0;

	ofxFloatSlider powerSliderOR;
	ofxFloatSlider powerSliderOG;
	ofxFloatSlider powerSliderOW;
	ofxFloatSlider powerSliderOB;
	ofxFloatSlider powerSliderOO;
	ofxFloatSlider powerSliderOK;
	ofxFloatSlider powerSliderOC;
	ofxFloatSlider powerSliderOD;

	float ppowerSliderOR = 0;
	float ppowerSliderOG = 0;
	float ppowerSliderOW = 0;
	float ppowerSliderOB = 0;
	float ppowerSliderOO = 0;
	float ppowerSliderOK = 0;
	float ppowerSliderOC = 0;
	float ppowerSliderOD = 0;

	ofxFloatSlider powerSliderKR;
	ofxFloatSlider powerSliderKG;
	ofxFloatSlider powerSliderKW;
	ofxFloatSlider powerSliderKB;
	ofxFloatSlider powerSliderKO;
	ofxFloatSlider powerSliderKK;
	ofxFloatSlider powerSliderKC;
	ofxFloatSlider powerSliderKD;

	float ppowerSliderKR = 0;
	float ppowerSliderKG = 0;
	float ppowerSliderKW = 0;
	float ppowerSliderKB = 0;
	float ppowerSliderKO = 0;
	float ppowerSliderKK = 0;
	float ppowerSliderKC = 0;
	float ppowerSliderKD = 0;

	ofxFloatSlider powerSliderCR;
	ofxFloatSlider powerSliderCG;
	ofxFloatSlider powerSliderCW;
	ofxFloatSlider powerSliderCB;
	ofxFloatSlider powerSliderCO;
	ofxFloatSlider powerSliderCK;
	ofxFloatSlider powerSliderCC;
	ofxFloatSlider powerSliderCD;

	float ppowerSliderCR = 0;
	float ppowerSliderCG = 0;
	float ppowerSliderCW = 0;
	float ppowerSliderCB = 0;
	float ppowerSliderCO = 0;
	float ppowerSliderCK = 0;
	float ppowerSliderCC = 0;
	float ppowerSliderCD = 0;

	ofxFloatSlider powerSliderDR;
	ofxFloatSlider powerSliderDG;
	ofxFloatSlider powerSliderDW;
	ofxFloatSlider powerSliderDB;
	ofxFloatSlider powerSliderDO;
	ofxFloatSlider powerSliderDK;
	ofxFloatSlider powerSliderDC;
	ofxFloatSlider powerSliderDD;

	float ppowerSliderDR = 0;
	float ppowerSliderDG = 0;
	float ppowerSliderDW = 0;
	float ppowerSliderDB = 0;
	float ppowerSliderDO = 0;
	float ppowerSliderDK = 0;
	float ppowerSliderDC = 0;
	float ppowerSliderDD = 0;

	ofxFloatSlider vSliderRR;
	ofxFloatSlider vSliderRG;
	ofxFloatSlider vSliderRW;
	ofxFloatSlider vSliderRB;
	ofxFloatSlider vSliderRO;
	ofxFloatSlider vSliderRK;
	ofxFloatSlider vSliderRC;
	ofxFloatSlider vSliderRD;

	float pvSliderRR = 0;
	float pvSliderRG = 0;
	float pvSliderRW = 0;
	float pvSliderRB = 0;
	float pvSliderRO = 0;
	float pvSliderRK = 0;
	float pvSliderRC = 0;
	float pvSliderRD = 0;

	ofxFloatSlider vSliderGR;
	ofxFloatSlider vSliderGG;
	ofxFloatSlider vSliderGW;
	ofxFloatSlider vSliderGB;
	ofxFloatSlider vSliderGO;
	ofxFloatSlider vSliderGK;
	ofxFloatSlider vSliderGC;
	ofxFloatSlider vSliderGD;

	float pvSliderGR = 0;
	float pvSliderGG = 0;
	float pvSliderGW = 0;
	float pvSliderGB = 0;
	float pvSliderGO = 0;
	float pvSliderGK = 0;
	float pvSliderGC = 0;
	float pvSliderGD = 0;

	ofxFloatSlider vSliderWR;
	ofxFloatSlider vSliderWG;
	ofxFloatSlider vSliderWW;
	ofxFloatSlider vSliderWB;
	ofxFloatSlider vSliderWO;
	ofxFloatSlider vSliderWK;
	ofxFloatSlider vSliderWC;
	ofxFloatSlider vSliderWD;

	float pvSliderWR = 0;
	float pvSliderWG = 0;
	float pvSliderWW = 0;
	float pvSliderWB = 0;
	float pvSliderWO = 0;
	float pvSliderWK = 0;
	float pvSliderWC = 0;
	float pvSliderWD = 0;

	ofxFloatSlider vSliderBR;
	ofxFloatSlider vSliderBG;
	ofxFloatSlider vSliderBW;
	ofxFloatSlider vSliderBB;
	ofxFloatSlider vSliderBO;
	ofxFloatSlider vSliderBK;
	ofxFloatSlider vSliderBC;
	ofxFloatSlider vSliderBD;

	float pvSliderBR = 0;
	float pvSliderBG = 0;
	float pvSliderBW = 0;
	float pvSliderBB = 0;
	float pvSliderBO = 0;
	float pvSliderBK = 0;
	float pvSliderBC = 0;
	float pvSliderBD = 0;

	ofxFloatSlider vSliderOR;
	ofxFloatSlider vSliderOG;
	ofxFloatSlider vSliderOW;
	ofxFloatSlider vSliderOB;
	ofxFloatSlider vSliderOO;
	ofxFloatSlider vSliderOK;
	ofxFloatSlider vSliderOC;
	ofxFloatSlider vSliderOD;

	float pvSliderOR = 0;
	float pvSliderOG = 0;
	float pvSliderOW = 0;
	float pvSliderOB = 0;
	float pvSliderOO = 0;
	float pvSliderOK = 0;
	float pvSliderOC = 0;
	float pvSliderOD = 0;

	ofxFloatSlider vSliderKR;
	ofxFloatSlider vSliderKG;
	ofxFloatSlider vSliderKW;
	ofxFloatSlider vSliderKB;
	ofxFloatSlider vSliderKO;
	ofxFloatSlider vSliderKK;
	ofxFloatSlider vSliderKC;
	ofxFloatSlider vSliderKD;

	float pvSliderKR = 0;
	float pvSliderKG = 0;
	float pvSliderKW = 0;
	float pvSliderKB = 0;
	float pvSliderKO = 0;
	float pvSliderKK = 0;
	float pvSliderKC = 0;
	float pvSliderKD = 0;

	ofxFloatSlider vSliderCR;
	ofxFloatSlider vSliderCG;
	ofxFloatSlider vSliderCW;
	ofxFloatSlider vSliderCB;
	ofxFloatSlider vSliderCO;
	ofxFloatSlider vSliderCK;
	ofxFloatSlider vSliderCC;
	ofxFloatSlider vSliderCD;

	float pvSliderCR = 0;
	float pvSliderCG = 0;
	float pvSliderCW = 0;
	float pvSliderCB = 0;
	float pvSliderCO = 0;
	float pvSliderCK = 0;
	float pvSliderCC = 0;
	float pvSliderCD = 0;

	ofxFloatSlider vSliderDR;
	ofxFloatSlider vSliderDG;
	ofxFloatSlider vSliderDW;
	ofxFloatSlider vSliderDB;
	ofxFloatSlider vSliderDO;
	ofxFloatSlider vSliderDK;
	ofxFloatSlider vSliderDC;
	ofxFloatSlider vSliderDD;

	float pvSliderDR = 0;
	float pvSliderDG = 0;
	float pvSliderDW = 0;
	float pvSliderDB = 0;
	float pvSliderDO = 0;
	float pvSliderDK = 0;
	float pvSliderDC = 0;
	float pvSliderDD = 0;

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

	float viscosityRR = 0.2F;
	float viscosityRG = 0.3F;
	float viscosityRW = 0.4F;
	float viscosityRB = 0.5F;
	float viscosityRO = 0.6F;
	float viscosityRK = 0.7F;
	float viscosityRC = 0.8F;
	float viscosityRD = 0.9F;

	float viscosityGR = 0.3F;
	float viscosityGG = 0.4F;
	float viscosityGW = 0.5F;
	float viscosityGB = 0.6F;
	float viscosityGO = 0.7F;
	float viscosityGK = 0.8F;
	float viscosityGC = 0.9F;
	float viscosityGD = 0.2F;

	float viscosityBR = 0.4F;
	float viscosityBG = 0.5F;
	float viscosityBW = 0.6F;
	float viscosityBB = 0.7F;
	float viscosityBO = 0.8F;
	float viscosityBK = 0.9F;
	float viscosityBC = 0.2F;
	float viscosityBD = 0.3F;

	float viscosityWR = 0.5F;
	float viscosityWG = 0.6F;
	float viscosityWW = 0.7F;
	float viscosityWB = 0.8F;
	float viscosityWO = 0.9F;
	float viscosityWK = 0.2F;
	float viscosityWC = 0.3F;
	float viscosityWD = 0.4F;

	float viscosityOR = 0.6F;
	float viscosityOG = 0.7F;
	float viscosityOW = 0.8F;
	float viscosityOB = 0.9F;
	float viscosityOO = 0.2F;
	float viscosityOK = 0.3F;
	float viscosityOC = 0.4F;
	float viscosityOD = 0.5F;

	float viscosityKR = 0.7F;
	float viscosityKG = 0.8F;
	float viscosityKW = 0.9F;
	float viscosityKB = 0.2F;
	float viscosityKO = 0.3F;
	float viscosityKK = 0.4F;
	float viscosityKC = 0.5F;
	float viscosityKD = 0.6F;

	float viscosityCR = 0.8F;
	float viscosityCG = 0.9F;
	float viscosityCW = 0.2F;
	float viscosityCB = 0.3F;
	float viscosityCO = 0.4F;
	float viscosityCK = 0.5F;
	float viscosityCC = 0.6F;
	float viscosityCD = 0.7F;

	float viscosityDR = 0.9F;
	float viscosityDG = 0.2F;
	float viscosityDW = 0.3F;
	float viscosityDB = 0.4F;
	float viscosityDO = 0.5F;
	float viscosityDK = 0.6F;
	float viscosityDC = 0.7F;
	float viscosityDD = 0.8F;

	float worldGravity = 0.0F;
	float forceVariance = 0.7F;
	float radiusVariance = 0.5F;
	float wallRepel = 20.0F;
};
