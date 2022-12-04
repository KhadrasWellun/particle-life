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
	void saveSettings();
	void loadSettings();
	void interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float G, float radius, float viscosity);

	ofxPanel gui;

	ofxGuiGroup globalGroup;
	ofxGuiGroup viscosityGroup;
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
	ofxButton randomChoice;
	ofxButton randomCount;
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
	unsigned int probability = 90.00;

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
	float viscosityR = 0.2F;
	float viscosityG = 0.3F;
	float viscosityW = 0.4F;
	float viscosityB = 0.5F;
	float viscosityO = 0.6F;
	float viscosityK = 0.7F;
	float viscosityC = 0.8F;
	float viscosityD = 0.9F;

	float worldGravity = 0.0F;
	float forceVariance = 0.7F;
	float radiusVariance = 0.5F;
	float wallRepel = 20.0F;
};
