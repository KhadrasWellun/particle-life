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
	void freeze();
	void rndDark();
	void attractDark();
	void repulseDark();
	void saveSettings();
	void loadSettings();
	void interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float G, float radius, float viscosity, float probability);

	ofxPanel gui;

	ofxGuiGroup globalGroup;
	ofxGuiGroup rndGroup;
	ofxGuiGroup qtyGroup;
	ofxGuiGroup alphaGroup;
	ofxGuiGroup bethaGroup;
	ofxGuiGroup gammaGroup;
	ofxGuiGroup eltaGroup;
	ofxGuiGroup epsilonGroup;
	ofxGuiGroup zetaGroup;
	ofxGuiGroup etaGroup;
	ofxGuiGroup tetaGroup;

	ofxGuiGroup MinMaxGroup;

	ofxButton resetButton;
	ofxButton freezeButton;
	ofxButton attractDarkButton;
	ofxButton repulseDarkButton;
	ofxButton selectButton;
	ofxButton save;
	ofxButton load;

	ofxButton randomGeneral;
	ofxButton randomCount;
	ofxButton randomChoice;
	ofxButton randomProb;
	ofxButton randomVsc;
	ofxButton rndDarkButton;

	ofxToggle boundsToggle;
	ofxToggle modelToggle;
	ofxToggle motionBlurToggle;

	// some experimental stuff here
	ofxGuiGroup expGroup;
	ofxToggle evoToggle;
	ofxFloatSlider InteractionEvoProbSlider;
	ofxFloatSlider InteractionEvoAmountSlider;
	ofxFloatSlider ProbabilityEvoProbSlider;
	ofxFloatSlider ProbabilityEvoAmountSlider;
	ofxFloatSlider ViscosityEvoProbSlider;
	ofxFloatSlider ViscosityEvoAmountSlider;
	float InterEvoChance = 0.3;
	float InterEvoAmount = 1.0;
	float ProbEvoChance = 0.1;
	float ProbEvoAmount = 0.3;
	float ViscoEvoChance = 0.1;
	float ViscoEvoAmount = 0.3;
	ofxFloatSlider minPowerSlider;
	ofxFloatSlider maxPowerSlider;
	float minP = -100.0;
	float maxP = 100.0;
	ofxFloatSlider minRangeSlider;
	ofxFloatSlider maxRangeSlider;
	float minR = 0.0;
	float maxR = 500.0;
	ofxFloatSlider minViscoSlider;
	ofxFloatSlider maxViscoSlider;
	float minV = 0.0;
	float maxV = 1.0;
	ofxFloatSlider minProbSlider;
	ofxFloatSlider maxProbSlider;
	float minI = 0.0;
	float maxI = 100.0;
	ofxToggle radiusToogle;
	ofxLabel physicLabel;
	//end of experimental

	ofxFloatSlider viscositySlider;

	ofxFloatSlider viscositySliderαα;
	ofxFloatSlider viscositySliderαβ;
	ofxFloatSlider viscositySliderαγ;
	ofxFloatSlider viscositySliderαδ;
	ofxFloatSlider viscositySliderαε;
	ofxFloatSlider viscositySliderαζ;
	ofxFloatSlider viscositySliderαη;
	
	ofxFloatSlider viscositySliderβα;
	ofxFloatSlider viscositySliderββ;
	ofxFloatSlider viscositySliderβγ;
	ofxFloatSlider viscositySliderβδ;
	ofxFloatSlider viscositySliderβε;
	ofxFloatSlider viscositySliderβζ;
	ofxFloatSlider viscositySliderβη;

	ofxFloatSlider viscositySliderγα;
	ofxFloatSlider viscositySliderγβ;
	ofxFloatSlider viscositySliderγγ;
	ofxFloatSlider viscositySliderγδ;
	ofxFloatSlider viscositySliderγε;
	ofxFloatSlider viscositySliderγζ;
	ofxFloatSlider viscositySliderγη;

	ofxFloatSlider viscositySliderδα;
	ofxFloatSlider viscositySliderδβ;
	ofxFloatSlider viscositySliderδγ;
	ofxFloatSlider viscositySliderδδ;
	ofxFloatSlider viscositySliderδε;
	ofxFloatSlider viscositySliderδζ;
	ofxFloatSlider viscositySliderδη;

	ofxFloatSlider viscositySliderεα;
	ofxFloatSlider viscositySliderεβ;
	ofxFloatSlider viscositySliderεγ;
	ofxFloatSlider viscositySliderεδ;
	ofxFloatSlider viscositySliderεε;
	ofxFloatSlider viscositySliderεζ;
	ofxFloatSlider viscositySliderεη;

	ofxFloatSlider viscositySliderζα;
	ofxFloatSlider viscositySliderζβ;
	ofxFloatSlider viscositySliderζγ;
	ofxFloatSlider viscositySliderζδ;
	ofxFloatSlider viscositySliderζε;
	ofxFloatSlider viscositySliderζζ;
	ofxFloatSlider viscositySliderζη;

	ofxFloatSlider viscositySliderηα;
	ofxFloatSlider viscositySliderηβ;
	ofxFloatSlider viscositySliderηγ;
	ofxFloatSlider viscositySliderηδ;
	ofxFloatSlider viscositySliderηε;
	ofxFloatSlider viscositySliderηζ;
	ofxFloatSlider viscositySliderηη;

	ofxFloatSlider viscositySliderαθ;
	ofxFloatSlider viscositySliderβθ;
	ofxFloatSlider viscositySliderγθ;
	ofxFloatSlider viscositySliderδθ;
	ofxFloatSlider viscositySliderεθ;
	ofxFloatSlider viscositySliderζθ;
	ofxFloatSlider viscositySliderηθ;
	ofxFloatSlider viscositySliderθα;
	ofxFloatSlider viscositySliderθβ;
	ofxFloatSlider viscositySliderθγ;
	ofxFloatSlider viscositySliderθδ;
	ofxFloatSlider viscositySliderθε;
	ofxFloatSlider viscositySliderθζ;
	ofxFloatSlider viscositySliderθη;
	ofxFloatSlider viscositySliderθθ;
	
	ofxFloatSlider probabilitySlider;

	ofxFloatSlider probabilitySliderαα;
	ofxFloatSlider probabilitySliderαβ;
	ofxFloatSlider probabilitySliderαγ;
	ofxFloatSlider probabilitySliderαδ;
	ofxFloatSlider probabilitySliderαε;
	ofxFloatSlider probabilitySliderαζ;
	ofxFloatSlider probabilitySliderαη;

	ofxFloatSlider probabilitySliderβα;
	ofxFloatSlider probabilitySliderββ;
	ofxFloatSlider probabilitySliderβγ;
	ofxFloatSlider probabilitySliderβδ;
	ofxFloatSlider probabilitySliderβε;
	ofxFloatSlider probabilitySliderβζ;
	ofxFloatSlider probabilitySliderβη;

	ofxFloatSlider probabilitySliderγα;
	ofxFloatSlider probabilitySliderγβ;
	ofxFloatSlider probabilitySliderγγ;
	ofxFloatSlider probabilitySliderγδ;
	ofxFloatSlider probabilitySliderγε;
	ofxFloatSlider probabilitySliderγζ;
	ofxFloatSlider probabilitySliderγη;

	ofxFloatSlider probabilitySliderδα;
	ofxFloatSlider probabilitySliderδβ;
	ofxFloatSlider probabilitySliderδγ;
	ofxFloatSlider probabilitySliderδδ;
	ofxFloatSlider probabilitySliderδε;
	ofxFloatSlider probabilitySliderδζ;
	ofxFloatSlider probabilitySliderδη;

	ofxFloatSlider probabilitySliderεα;
	ofxFloatSlider probabilitySliderεβ;
	ofxFloatSlider probabilitySliderεγ;
	ofxFloatSlider probabilitySliderεδ;
	ofxFloatSlider probabilitySliderεε;
	ofxFloatSlider probabilitySliderεζ;
	ofxFloatSlider probabilitySliderεη;

	ofxFloatSlider probabilitySliderζα;
	ofxFloatSlider probabilitySliderζβ;
	ofxFloatSlider probabilitySliderζγ;
	ofxFloatSlider probabilitySliderζδ;
	ofxFloatSlider probabilitySliderζε;
	ofxFloatSlider probabilitySliderζζ;
	ofxFloatSlider probabilitySliderζη;

	ofxFloatSlider probabilitySliderηα;
	ofxFloatSlider probabilitySliderηβ;
	ofxFloatSlider probabilitySliderηγ;
	ofxFloatSlider probabilitySliderηδ;
	ofxFloatSlider probabilitySliderηε;
	ofxFloatSlider probabilitySliderηζ;
	ofxFloatSlider probabilitySliderηη;

	ofxFloatSlider probabilitySliderαθ;
	ofxFloatSlider probabilitySliderβθ;
	ofxFloatSlider probabilitySliderγθ;
	ofxFloatSlider probabilitySliderδθ;
	ofxFloatSlider probabilitySliderεθ;
	ofxFloatSlider probabilitySliderζθ;
	ofxFloatSlider probabilitySliderηθ;
	ofxFloatSlider probabilitySliderθα;
	ofxFloatSlider probabilitySliderθβ;
	ofxFloatSlider probabilitySliderθγ;
	ofxFloatSlider probabilitySliderθδ;
	ofxFloatSlider probabilitySliderθε;
	ofxFloatSlider probabilitySliderθζ;
	ofxFloatSlider probabilitySliderθη;
	ofxFloatSlider probabilitySliderθθ;

	float probability = 100.00;

	float probabilityαα = 100.00;
	float probabilityαβ = 100.00;
	float probabilityαγ = 100.00;
	float probabilityαδ = 100.00;
	float probabilityαε = 100.00;
	float probabilityαζ = 100.00;
	float probabilityαη = 100.00;

	float probabilityβα = 100.00;
	float probabilityββ = 100.00;
	float probabilityβγ = 100.00;
	float probabilityβδ = 100.00;
	float probabilityβε = 100.00;
	float probabilityβζ = 100.00;
	float probabilityβη = 100.00;

	float probabilityγα = 100.00;
	float probabilityγβ = 100.00;
	float probabilityγγ = 100.00;
	float probabilityγδ = 100.00;
	float probabilityγε = 100.00;
	float probabilityγζ = 100.00;
	float probabilityγη = 100.00;

	float probabilityδα = 100.00;
	float probabilityδβ = 100.00;
	float probabilityδγ = 100.00;
	float probabilityδδ = 100.00;
	float probabilityδε = 100.00;
	float probabilityδζ = 100.00;
	float probabilityδη = 100.00;

	float probabilityεα = 100.00;
	float probabilityεβ = 100.00;
	float probabilityεγ = 100.00;
	float probabilityεδ = 100.00;
	float probabilityεε = 100.00;
	float probabilityεζ = 100.00;
	float probabilityεη = 100.00;

	float probabilityζα = 100.00;
	float probabilityζβ = 100.00;
	float probabilityζγ = 100.00;
	float probabilityζδ = 100.00;
	float probabilityζε = 100.00;
	float probabilityζζ = 100.00;
	float probabilityζη = 100.00;

	float probabilityηα = 100.00;
	float probabilityηβ = 100.00;
	float probabilityηγ = 100.00;
	float probabilityηδ = 100.00;
	float probabilityηε = 100.00;
	float probabilityηζ = 100.00;
	float probabilityηη = 100.00;

	float probabilityαθ = 0;
	float probabilityβθ = 0;
	float probabilityγθ = 0;
	float probabilityδθ = 0;
	float probabilityεθ = 0;
	float probabilityζθ = 0;
	float probabilityηθ = 0;
	float probabilityθα = 30.00;
	float probabilityθβ = 30.00;
	float probabilityθγ = 30.00;
	float probabilityθδ = 30.00;
	float probabilityθε = 30.00;
	float probabilityθζ = 30.00;
	float probabilityθη = 30.00;
	float probabilityθθ = 0;

	ofxFloatSlider gravitySlider;
	ofxFloatSlider wallRepelSlider;

	ofxIntSlider numberSliderα;
	ofxIntSlider numberSliderβ;
	ofxIntSlider numberSliderδ;
	ofxIntSlider numberSliderγ;
	ofxIntSlider numberSliderε;
	ofxIntSlider numberSliderζ;
	ofxIntSlider numberSliderη;
	ofxIntSlider numberSliderθ;

	int pnumberSliderα = 1500;
	int pnumberSliderβ = 1500;
	int pnumberSliderδ = 1500;
	int pnumberSliderγ = 1500;
	int pnumberSliderε = 1500;
	int pnumberSliderζ = 1500;
	int pnumberSliderη = 1500;
	int pnumberSliderθ = 2500;

	ofxFloatSlider powerSliderαα;
	ofxFloatSlider powerSliderαβ;
	ofxFloatSlider powerSliderαδ;
	ofxFloatSlider powerSliderαγ;
	ofxFloatSlider powerSliderαε;
	ofxFloatSlider powerSliderαζ;
	ofxFloatSlider powerSliderαη;

	float ppowerSliderαα = 0;
	float ppowerSliderαβ = 0;
	float ppowerSliderαδ = 0;
	float ppowerSliderαγ = 0;
	float ppowerSliderαε = 0;
	float ppowerSliderαζ = 0;
	float ppowerSliderαη = 0;

	ofxFloatSlider powerSliderβα;
	ofxFloatSlider powerSliderββ;
	ofxFloatSlider powerSliderβδ;
	ofxFloatSlider powerSliderβγ;
	ofxFloatSlider powerSliderβε;
	ofxFloatSlider powerSliderβζ;
	ofxFloatSlider powerSliderβη;

	float ppowerSliderβα = 0;
	float ppowerSliderββ = 0;
	float ppowerSliderβδ = 0;
	float ppowerSliderβγ = 0;
	float ppowerSliderβε = 0;
	float ppowerSliderβζ = 0;
	float ppowerSliderβη = 0;

	ofxFloatSlider powerSliderδα;
	ofxFloatSlider powerSliderδβ;
	ofxFloatSlider powerSliderδδ;
	ofxFloatSlider powerSliderδγ;
	ofxFloatSlider powerSliderδε;
	ofxFloatSlider powerSliderδζ;
	ofxFloatSlider powerSliderδη;

	float ppowerSliderδα = 0;
	float ppowerSliderδβ = 0;
	float ppowerSliderδδ = 0;
	float ppowerSliderδγ = 0;
	float ppowerSliderδε = 0;
	float ppowerSliderδζ = 0;
	float ppowerSliderδη = 0;

	ofxFloatSlider powerSliderγα;
	ofxFloatSlider powerSliderγβ;
	ofxFloatSlider powerSliderγδ;
	ofxFloatSlider powerSliderγγ;
	ofxFloatSlider powerSliderγε;
	ofxFloatSlider powerSliderγζ;
	ofxFloatSlider powerSliderγη;

	float ppowerSliderγα = 0;
	float ppowerSliderγβ = 0;
	float ppowerSliderγδ = 0;
	float ppowerSliderγγ = 0;
	float ppowerSliderγε = 0;
	float ppowerSliderγζ = 0;
	float ppowerSliderγη = 0;

	ofxFloatSlider powerSliderεα;
	ofxFloatSlider powerSliderεβ;
	ofxFloatSlider powerSliderεδ;
	ofxFloatSlider powerSliderεγ;
	ofxFloatSlider powerSliderεε;
	ofxFloatSlider powerSliderεζ;
	ofxFloatSlider powerSliderεη;

	float ppowerSliderεα = 0;
	float ppowerSliderεβ = 0;
	float ppowerSliderεδ = 0;
	float ppowerSliderεγ = 0;
	float ppowerSliderεε = 0;
	float ppowerSliderεζ = 0;
	float ppowerSliderεη = 0;

	ofxFloatSlider powerSliderζα;
	ofxFloatSlider powerSliderζβ;
	ofxFloatSlider powerSliderζδ;
	ofxFloatSlider powerSliderζγ;
	ofxFloatSlider powerSliderζε;
	ofxFloatSlider powerSliderζζ;
	ofxFloatSlider powerSliderζη;

	float ppowerSliderζα = 0;
	float ppowerSliderζβ = 0;
	float ppowerSliderζδ = 0;
	float ppowerSliderζγ = 0;
	float ppowerSliderζε = 0;
	float ppowerSliderζζ = 0;
	float ppowerSliderζη = 0;

	ofxFloatSlider powerSliderηα;
	ofxFloatSlider powerSliderηβ;
	ofxFloatSlider powerSliderηδ;
	ofxFloatSlider powerSliderηγ;
	ofxFloatSlider powerSliderηε;
	ofxFloatSlider powerSliderηζ;
	ofxFloatSlider powerSliderηη;

	float ppowerSliderηα = 0;
	float ppowerSliderηβ = 0;
	float ppowerSliderηδ = 0;
	float ppowerSliderηγ = 0;
	float ppowerSliderηε = 0;
	float ppowerSliderηζ = 0;
	float ppowerSliderηη = 0;

	ofxFloatSlider powerSliderαθ;
	ofxFloatSlider powerSliderβθ;
	ofxFloatSlider powerSliderδθ;
	ofxFloatSlider powerSliderγθ;
	ofxFloatSlider powerSliderεθ;
	ofxFloatSlider powerSliderζθ;
	ofxFloatSlider powerSliderηθ;
	ofxFloatSlider powerSliderθα;
	ofxFloatSlider powerSliderθβ;
	ofxFloatSlider powerSliderθδ;
	ofxFloatSlider powerSliderθγ;
	ofxFloatSlider powerSliderθε;
	ofxFloatSlider powerSliderθζ;
	ofxFloatSlider powerSliderθη;
	ofxFloatSlider powerSliderθθ;

	float ppowerSliderαθ = 0;
	float ppowerSliderβθ = 0;
	float ppowerSliderδθ = 0;
	float ppowerSliderγθ = 0;
	float ppowerSliderεθ = 0;
	float ppowerSliderζθ = 0;
	float ppowerSliderηθ = 0;
	float ppowerSliderθα = 0;
	float ppowerSliderθβ = 0;
	float ppowerSliderθδ = 0;
	float ppowerSliderθγ = 0;
	float ppowerSliderθε = 0;
	float ppowerSliderθζ = 0;
	float ppowerSliderθη = 0;
	float ppowerSliderθθ = 0;

	ofxFloatSlider vSliderαα;
	ofxFloatSlider vSliderαβ;
	ofxFloatSlider vSliderαδ;
	ofxFloatSlider vSliderαγ;
	ofxFloatSlider vSliderαε;
	ofxFloatSlider vSliderαζ;
	ofxFloatSlider vSliderαη;

	float pvSliderαα = 0;
	float pvSliderαβ = 0;
	float pvSliderαδ = 0;
	float pvSliderαγ = 0;
	float pvSliderαε = 0;
	float pvSliderαζ = 0;
	float pvSliderαη = 0;

	ofxFloatSlider vSliderβα;
	ofxFloatSlider vSliderββ;
	ofxFloatSlider vSliderβδ;
	ofxFloatSlider vSliderβγ;
	ofxFloatSlider vSliderβε;
	ofxFloatSlider vSliderβζ;
	ofxFloatSlider vSliderβη;

	float pvSliderβα = 0;
	float pvSliderββ = 0;
	float pvSliderβδ = 0;
	float pvSliderβγ = 0;
	float pvSliderβε = 0;
	float pvSliderβζ = 0;
	float pvSliderβη = 0;

	ofxFloatSlider vSliderδα;
	ofxFloatSlider vSliderδβ;
	ofxFloatSlider vSliderδδ;
	ofxFloatSlider vSliderδγ;
	ofxFloatSlider vSliderδε;
	ofxFloatSlider vSliderδζ;
	ofxFloatSlider vSliderδη;

	float pvSliderδα = 0;
	float pvSliderδβ = 0;
	float pvSliderδδ = 0;
	float pvSliderδγ = 0;
	float pvSliderδε = 0;
	float pvSliderδζ = 0;
	float pvSliderδη = 0;

	ofxFloatSlider vSliderγα;
	ofxFloatSlider vSliderγβ;
	ofxFloatSlider vSliderγδ;
	ofxFloatSlider vSliderγγ;
	ofxFloatSlider vSliderγε;
	ofxFloatSlider vSliderγζ;
	ofxFloatSlider vSliderγη;

	float pvSliderγα = 0;
	float pvSliderγβ = 0;
	float pvSliderγδ = 0;
	float pvSliderγγ = 0;
	float pvSliderγε = 0;
	float pvSliderγζ = 0;
	float pvSliderγη = 0;

	ofxFloatSlider vSliderεα;
	ofxFloatSlider vSliderεβ;
	ofxFloatSlider vSliderεδ;
	ofxFloatSlider vSliderεγ;
	ofxFloatSlider vSliderεε;
	ofxFloatSlider vSliderεζ;
	ofxFloatSlider vSliderεη;

	float pvSliderεα = 0;
	float pvSliderεβ = 0;
	float pvSliderεδ = 0;
	float pvSliderεγ = 0;
	float pvSliderεε = 0;
	float pvSliderεζ = 0;
	float pvSliderεη = 0;

	ofxFloatSlider vSliderζα;
	ofxFloatSlider vSliderζβ;
	ofxFloatSlider vSliderζδ;
	ofxFloatSlider vSliderζγ;
	ofxFloatSlider vSliderζε;
	ofxFloatSlider vSliderζζ;
	ofxFloatSlider vSliderζη;

	float pvSliderζα = 0;
	float pvSliderζβ = 0;
	float pvSliderζδ = 0;
	float pvSliderζγ = 0;
	float pvSliderζε = 0;
	float pvSliderζζ = 0;
	float pvSliderζη = 0;

	ofxFloatSlider vSliderηα;
	ofxFloatSlider vSliderηβ;
	ofxFloatSlider vSliderηδ;
	ofxFloatSlider vSliderηγ;
	ofxFloatSlider vSliderηε;
	ofxFloatSlider vSliderηζ;
	ofxFloatSlider vSliderηη;

	float pvSliderηα = 0;
	float pvSliderηβ = 0;
	float pvSliderηδ = 0;
	float pvSliderηγ = 0;
	float pvSliderηε = 0;
	float pvSliderηζ = 0;
	float pvSliderηη = 0;

	ofxFloatSlider vSliderαθ;
	ofxFloatSlider vSliderβθ;
	ofxFloatSlider vSliderδθ;
	ofxFloatSlider vSliderγθ;
	ofxFloatSlider vSliderεθ;
	ofxFloatSlider vSliderζθ;
	ofxFloatSlider vSliderηθ;
	ofxFloatSlider vSliderθα;
	ofxFloatSlider vSliderθβ;
	ofxFloatSlider vSliderθδ;
	ofxFloatSlider vSliderθγ;
	ofxFloatSlider vSliderθε;
	ofxFloatSlider vSliderθζ;
	ofxFloatSlider vSliderθη;
	ofxFloatSlider vSliderθθ;

	float pvSliderαθ = 0;
	float pvSliderβθ = 0;
	float pvSliderδθ = 0;
	float pvSliderγθ = 0;
	float pvSliderεθ = 0;
	float pvSliderζθ = 0;
	float pvSliderηθ = 0;
	float pvSliderθα = 300;
	float pvSliderθβ = 300;
	float pvSliderθδ = 300;
	float pvSliderθγ = 300;
	float pvSliderθε = 300;
	float pvSliderθζ = 300;
	float pvSliderθη = 300;
	float pvSliderθθ = 0;

	ofxLabel labelβ;
	ofxLabel labelα;
	ofxLabel labelδ;
	ofxLabel labelγ;
	ofxLabel labelε;
	ofxLabel labelζ;
	ofxLabel labelη;
	ofxLabel labelθ;
	ofxLabel aboutL1;
	ofxLabel aboutL2;
	ofxLabel aboutL3;
	ofxLabel fps;

	// simulation bounds
	int boundWidth = 1600;
	int boundHeight = 900;

	float viscosity = 0.7F;

	float viscosityαα = 0.7F;
	float viscosityαβ = 0.7F;
	float viscosityαδ = 0.7F;
	float viscosityαγ = 0.7F;
	float viscosityαε = 0.7F;
	float viscosityαζ = 0.7F;
	float viscosityαη = 0.7F;

	float viscosityβα = 0.7F;
	float viscosityββ = 0.7F;
	float viscosityβδ = 0.7F;
	float viscosityβγ = 0.7F;
	float viscosityβε = 0.7F;
	float viscosityβζ = 0.7F;
	float viscosityβη = 0.7F;

	float viscosityγα = 0.7F;
	float viscosityγβ = 0.7F;
	float viscosityγδ = 0.7F;
	float viscosityγγ = 0.7F;
	float viscosityγε = 0.7F;
	float viscosityγζ = 0.7F;
	float viscosityγη = 0.7F;

	float viscosityδα = 0.7F;
	float viscosityδβ = 0.7F;
	float viscosityδδ = 0.7F;
	float viscosityδγ = 0.7F;
	float viscosityδε = 0.7F;
	float viscosityδζ = 0.7F;
	float viscosityδη = 0.7F;

	float viscosityεα = 0.7F;
	float viscosityεβ = 0.7F;
	float viscosityεδ = 0.7F;
	float viscosityεγ = 0.7F;
	float viscosityεε = 0.7F;
	float viscosityεζ = 0.7F;
	float viscosityεη = 0.7F;

	float viscosityζα = 0.7F;
	float viscosityζβ = 0.7F;
	float viscosityζδ = 0.7F;
	float viscosityζγ = 0.7F;
	float viscosityζε = 0.7F;
	float viscosityζζ = 0.7F;
	float viscosityζη = 0.7F;

	float viscosityηα = 0.7F;
	float viscosityηβ = 0.7F;
	float viscosityηδ = 0.7F;
	float viscosityηγ = 0.7F;
	float viscosityηε = 0.7F;
	float viscosityηζ = 0.7F;
	float viscosityηη = 0.7F;

	float viscosityαθ = 0.0;
	float viscosityβθ = 0.0;
	float viscosityγθ = 0.0;
	float viscosityδθ = 0.0;
	float viscosityεθ = 0.0;
	float viscosityζθ = 0.0;
	float viscosityηθ = 0.0;
	float viscosityθα = 0.3F;
	float viscosityθβ = 0.3F;
	float viscosityθγ = 0.3F;
	float viscosityθδ = 0.3F;
	float viscosityθε = 0.3F;
	float viscosityθζ = 0.3F;
	float viscosityθη = 0.3F;
	float viscosityθθ = 0.0;

	float worldGravity = 0.0F;
	float forceVariance = 0.7F;
	float radiusVariance = 0.5F;
	float wallRepel = 20.0F;

	vector<ofxFloatSlider*> powersliders = {
		&powerSliderαα, &powerSliderαβ, &powerSliderαγ, &powerSliderαδ,
		&powerSliderβα, &powerSliderββ, &powerSliderβγ, &powerSliderβδ,
		&powerSliderγα, &powerSliderγβ, &powerSliderγγ, &powerSliderγδ,
		&powerSliderδα, &powerSliderδβ, &powerSliderδγ, &powerSliderδδ,
		&powerSliderεε, &powerSliderεζ, &powerSliderεη, &powerSliderεθ,
		&powerSliderζε, &powerSliderζζ, &powerSliderζη, &powerSliderζθ,
		&powerSliderηε, &powerSliderηζ, &powerSliderηη, &powerSliderηθ,
		&powerSliderθε, &powerSliderθζ, &powerSliderθη, &powerSliderθθ,
	};
	vector<ofxFloatSlider*> vsliders = {
		&vSliderαα, &vSliderαβ, &vSliderαγ, &vSliderαδ,
		&vSliderβα, &vSliderββ, &vSliderβγ, &vSliderβδ,
		&vSliderγα, &vSliderγβ, &vSliderγγ, &vSliderγδ,
		&vSliderδα, &vSliderδβ, &vSliderδγ, &vSliderδδ,
		&vSliderεε, &vSliderεζ, &vSliderεη, &vSliderεθ,
		&vSliderζε, &vSliderζζ, &vSliderζη, &vSliderζθ,
		&vSliderηε, &vSliderηζ, &vSliderηη, &vSliderηθ,
		&vSliderθε, &vSliderθζ, &vSliderθη, &vSliderθθ,
	};
	vector<ofxFloatSlider*> viscositysliders = {
		&viscositySliderαα, &viscositySliderαβ, &viscositySliderαγ, &viscositySliderαδ,
		&viscositySliderβα, &viscositySliderββ, &viscositySliderβγ, &viscositySliderβδ,
		&viscositySliderγα, &viscositySliderγβ, &viscositySliderγγ, &viscositySliderγδ,
		&viscositySliderδα, &viscositySliderδβ, &viscositySliderδγ, &viscositySliderδδ,
		&viscositySliderεε, &viscositySliderεζ, &viscositySliderεη, &viscositySliderεθ,
		&viscositySliderζε, &viscositySliderζζ, &viscositySliderζη, &viscositySliderζθ,
		&viscositySliderηε, &viscositySliderηζ, &viscositySliderηη, &viscositySliderηθ,
		&viscositySliderθε, &viscositySliderθζ, &viscositySliderθη, &viscositySliderθθ,
	};
	vector<ofxFloatSlider*> probabilitysliders = {
		&probabilitySliderαα, &probabilitySliderαβ, &probabilitySliderαγ, &probabilitySliderαδ,
		&probabilitySliderβα, &probabilitySliderββ, &probabilitySliderβγ, &probabilitySliderβδ,
		&probabilitySliderγα, &probabilitySliderγβ, &probabilitySliderγγ, &probabilitySliderγδ,
		&probabilitySliderδα, &probabilitySliderδβ, &probabilitySliderδγ, &probabilitySliderδδ,
		&probabilitySliderεε, &probabilitySliderεζ, &probabilitySliderεη, &probabilitySliderεθ,
		&probabilitySliderζε, &probabilitySliderζζ, &probabilitySliderζη, &probabilitySliderζθ,
		&probabilitySliderηε, &probabilitySliderηζ, &probabilitySliderηη, &probabilitySliderηθ,
		&probabilitySliderθε, &probabilitySliderθζ, &probabilitySliderθη, &probabilitySliderθθ,
	};
};
