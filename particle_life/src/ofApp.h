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
		ofDrawCircle(x, y, 2.25F); //draw a point at x,y coordinates, the size of a 2.25 pixels
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
	void rndrel();
	void monads();
	void rndir();
	void rndprob();
	void rndvsc();
	void freeze();
	void saveSettings();
	void loadSettings();
	void interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float attractForce, float repelForce, float attractRadius, float repelRadius, float attractViscosity, float repelViscosity, float attractProbability, float repelProbability);;
	
	ofxPanel gui;
	
	
	ofxGuiGroup evolveGroup;
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
	ofxButton randomRelations;
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
	ofxFloatSlider AttractEvoProbSlider;
	ofxFloatSlider AttractEvoAmountSlider;
	ofxFloatSlider ProbAttractEvoProbSlider;
	ofxFloatSlider ProbAttractEvoAmountSlider;
	ofxFloatSlider ViscoAttractEvoProbSlider;
	ofxFloatSlider ViscoAttractEvoAmountSlider;
	ofxFloatSlider RepelEvoProbSlider;
	ofxFloatSlider RepelEvoAmountSlider;
	ofxFloatSlider ProbRepelEvoProbSlider;
	ofxFloatSlider ProbRepelEvoAmountSlider;
	ofxFloatSlider ViscoRepelEvoProbSlider;
	ofxFloatSlider ViscoRepelEvoAmountSlider;
	float AttractEvoChance = 0.1;
	float AttractEvoAmount = 0.3;
	float ProbAttractEvoChance = 0.1;
	float ProbAttractEvoAmount = 0.3;
	float ViscoAttractEvoChance = 0.1;
	float ViscoAttractEvoAmount = 0.3;
	float RepelEvoChance = 0.1;
	float RepelEvoAmount = 0.3;
	float ProbRepelEvoChance = 0.1;
	float ProbRepelEvoAmount = 0.3;
	float ViscoRepelEvoChance = 0.1;
	float ViscoRepelEvoAmount = 0.3;
	ofxFloatSlider minAttractPowerSlider;
	ofxFloatSlider maxAttractPowerSlider;
	float minAttP = -50.0;
	float maxAttP = 50.0;
	ofxFloatSlider minRepelPowerSlider;
	ofxFloatSlider maxRepelPowerSlider;
	float minRepP = -50.0;
	float maxRepP = 50.0;
	ofxFloatSlider minAttractRangeSlider;
	ofxFloatSlider maxAttractRangeSlider;
	float minAttR = 0.0;
	float maxAttR = 100.0;
	ofxFloatSlider minRepelRangeSlider;
	ofxFloatSlider maxRepelRangeSlider;
	float minRepR = 0.0;
	float maxRepR = 100.0;
	ofxFloatSlider minAttractViscoSlider;
	ofxFloatSlider maxAttractViscoSlider;
	float minAttV = 0.0;
	float maxAttV = 1.0;
	ofxFloatSlider minRepelViscoSlider;
	ofxFloatSlider maxRepelViscoSlider;
	float minRepV = 0.0;
	float maxRepV = 1.0;
	ofxFloatSlider minAttractProbSlider;
	ofxFloatSlider maxAttractProbSlider;
	float minAttI = 0.0;
	float maxAttI = 100.0;
	ofxFloatSlider minRepelProbSlider;
	ofxFloatSlider maxRepelProbSlider;
	float minRepI = 0.0;
	float maxRepI = 100.0;
	ofxToggle radiusToogle;
	ofxLabel physicLabel;
	
	ofxFloatSlider viscositySlider;
	float viscosity = 0.7F;
	ofxFloatSlider probabilitySlider;
	float probability = 100.00;

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

	int pnumberSliderα = 700;
	int pnumberSliderβ = 700;
	int pnumberSliderδ = 700;
	int pnumberSliderγ = 700;
	int pnumberSliderε = 700;
	int pnumberSliderζ = 700;
	int pnumberSliderη = 700;
	int pnumberSliderθ = 700;

	//Alpha particle sliders

	ofxFloatSlider AttractPowerSliderαα;
	float pAttractPowerSliderαα = 0;
	ofxFloatSlider AttractDistanceSliderαα;
	float pAttractDistanceSliderαα = 0;
	ofxFloatSlider ProbAttractSliderαα;
	float ProbAttractαα = 100.00;
	ofxFloatSlider ViscosityAttractSliderαα;
	float ViscosityAttractαα = 0.7F;

	ofxFloatSlider RepelPowerSliderαα;
	float pRepelPowerSliderαα = 0;
	ofxFloatSlider RepelDistanceSliderαα;
	float pRepelDistanceSliderαα = 0;
	ofxFloatSlider ProbRepelSliderαα;
	float ProbRepelαα = 100.00;
	ofxFloatSlider ViscosityRepelSliderαα;
	float ViscosityRepelαα = 0.7F;

	ofxFloatSlider AttractPowerSliderαβ;
	float pAttractPowerSliderαβ = 0;
	ofxFloatSlider AttractDistanceSliderαβ;
	float pAttractDistanceSliderαβ = 0;
	ofxFloatSlider ProbAttractSliderαβ;
	float ProbAttractαβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderαβ;
	float ViscosityAttractαβ = 0.7F;

	ofxFloatSlider RepelPowerSliderαβ;
	float pRepelPowerSliderαβ = 0;
	ofxFloatSlider RepelDistanceSliderαβ;
	float pRepelDistanceSliderαβ = 0;
	ofxFloatSlider ProbRepelSliderαβ;
	float ProbRepelαβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderαβ;
	float ViscosityRepelαβ = 0.7F;

	ofxFloatSlider AttractPowerSliderαγ;
	float pAttractPowerSliderαγ = 0;
	ofxFloatSlider AttractDistanceSliderαγ;
	float pAttractDistanceSliderαγ = 0;
	ofxFloatSlider ProbAttractSliderαγ;
	float ProbAttractαγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderαγ;
	float ViscosityAttractαγ = 0.7F;

	ofxFloatSlider RepelPowerSliderαγ;
	float pRepelPowerSliderαγ = 0;
	ofxFloatSlider RepelDistanceSliderαγ;
	float pRepelDistanceSliderαγ = 0;
	ofxFloatSlider ProbRepelSliderαγ;
	float ProbRepelαγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderαγ;
	float ViscosityRepelαγ = 0.7F;

	ofxFloatSlider AttractPowerSliderαδ;
	float pAttractPowerSliderαδ = 0;
	ofxFloatSlider AttractDistanceSliderαδ;
	float pAttractDistanceSliderαδ = 0;
	ofxFloatSlider ProbAttractSliderαδ;
	float ProbAttractαδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderαδ;
	float ViscosityAttractαδ = 0.7F;

	ofxFloatSlider RepelPowerSliderαδ;
	float pRepelPowerSliderαδ = 0;
	ofxFloatSlider RepelDistanceSliderαδ;
	float pRepelDistanceSliderαδ = 0;
	ofxFloatSlider ProbRepelSliderαδ;
	float ProbRepelαδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderαδ;
	float ViscosityRepelαδ = 0.7F;

	ofxFloatSlider AttractPowerSliderαε;
	float pAttractPowerSliderαε = 0;
	ofxFloatSlider AttractDistanceSliderαε;
	float pAttractDistanceSliderαε = 0;
	ofxFloatSlider ProbAttractSliderαε;
	float ProbAttractαε = 100.00;
	ofxFloatSlider ViscosityAttractSliderαε;
	float ViscosityAttractαε = 0.7F;

	ofxFloatSlider RepelPowerSliderαε;
	float pRepelPowerSliderαε = 0;
	ofxFloatSlider RepelDistanceSliderαε;
	float pRepelDistanceSliderαε = 0;
	ofxFloatSlider ProbRepelSliderαε;
	float ProbRepelαε = 100.00;
	ofxFloatSlider ViscosityRepelSliderαε;
	float ViscosityRepelαε = 0.7F;

	ofxFloatSlider AttractPowerSliderαζ;
	float pAttractPowerSliderαζ = 0;
	ofxFloatSlider AttractDistanceSliderαζ;
	float pAttractDistanceSliderαζ = 0;
	ofxFloatSlider ProbAttractSliderαζ;
	float ProbAttractαζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderαζ;
	float ViscosityAttractαζ = 0.7F;

	ofxFloatSlider RepelPowerSliderαζ;
	float pRepelPowerSliderαζ = 0;
	ofxFloatSlider RepelDistanceSliderαζ;
	float pRepelDistanceSliderαζ = 0;
	ofxFloatSlider ProbRepelSliderαζ;
	float ProbRepelαζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderαζ;
	float ViscosityRepelαζ = 0.7F;

	ofxFloatSlider AttractPowerSliderαη;
	float pAttractPowerSliderαη = 0;
	ofxFloatSlider AttractDistanceSliderαη;
	float pAttractDistanceSliderαη = 0;
	ofxFloatSlider ProbAttractSliderαη;
	float ProbAttractαη = 100.00;
	ofxFloatSlider ViscosityAttractSliderαη;
	float ViscosityAttractαη = 0.7F;

	ofxFloatSlider RepelPowerSliderαη;
	float pRepelPowerSliderαη = 0;
	ofxFloatSlider RepelDistanceSliderαη;
	float pRepelDistanceSliderαη = 0;
	ofxFloatSlider ProbRepelSliderαη;
	float ProbRepelαη = 100.00;
	ofxFloatSlider ViscosityRepelSliderαη;
	float ViscosityRepelαη = 0.7F;

	ofxFloatSlider AttractPowerSliderαθ;
	float pAttractPowerSliderαθ = 0;
	ofxFloatSlider AttractDistanceSliderαθ;
	float pAttractDistanceSliderαθ = 0;
	ofxFloatSlider ProbAttractSliderαθ;
	float ProbAttractαθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderαθ;
	float ViscosityAttractαθ = 0.7F;

	ofxFloatSlider RepelPowerSliderαθ;
	float pRepelPowerSliderαθ = 0;
	ofxFloatSlider RepelDistanceSliderαθ;
	float pRepelDistanceSliderαθ = 0;
	ofxFloatSlider ProbRepelSliderαθ;
	float ProbRepelαθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderαθ;
	float ViscosityRepelαθ = 0.7F;

	//Beta particle sliders

	ofxFloatSlider AttractPowerSliderβα;
	float pAttractPowerSliderβα = 0;
	ofxFloatSlider AttractDistanceSliderβα;
	float pAttractDistanceSliderβα = 0;
	ofxFloatSlider ProbAttractSliderβα;
	float ProbAttractβα = 100.00;
	ofxFloatSlider ViscosityAttractSliderβα;
	float ViscosityAttractβα = 0.7F;

	ofxFloatSlider RepelPowerSliderβα;
	float pRepelPowerSliderβα = 0;
	ofxFloatSlider RepelDistanceSliderβα;
	float pRepelDistanceSliderβα = 0;
	ofxFloatSlider ProbRepelSliderβα;
	float ProbRepelβα = 100.00;
	ofxFloatSlider ViscosityRepelSliderβα;
	float ViscosityRepelβα = 0.7F;

	ofxFloatSlider AttractPowerSliderββ;
	float pAttractPowerSliderββ = 0;
	ofxFloatSlider AttractDistanceSliderββ;
	float pAttractDistanceSliderββ = 0;
	ofxFloatSlider ProbAttractSliderββ;
	float ProbAttractββ = 100.00;
	ofxFloatSlider ViscosityAttractSliderββ;
	float ViscosityAttractββ = 0.7F;

	ofxFloatSlider RepelPowerSliderββ;
	float pRepelPowerSliderββ = 0;
	ofxFloatSlider RepelDistanceSliderββ;
	float pRepelDistanceSliderββ = 0;
	ofxFloatSlider ProbRepelSliderββ;
	float ProbRepelββ = 100.00;
	ofxFloatSlider ViscosityRepelSliderββ;
	float ViscosityRepelββ = 0.7F;

	ofxFloatSlider AttractPowerSliderβγ;
	float pAttractPowerSliderβγ = 0;
	ofxFloatSlider AttractDistanceSliderβγ;
	float pAttractDistanceSliderβγ = 0;
	ofxFloatSlider ProbAttractSliderβγ;
	float ProbAttractβγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderβγ;
	float ViscosityAttractβγ = 0.7F;

	ofxFloatSlider RepelPowerSliderβγ;
	float pRepelPowerSliderβγ = 0;
	ofxFloatSlider RepelDistanceSliderβγ;
	float pRepelDistanceSliderβγ = 0;
	ofxFloatSlider ProbRepelSliderβγ;
	float ProbRepelβγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderβγ;
	float ViscosityRepelβγ = 0.7F;

	ofxFloatSlider AttractPowerSliderβδ;
	float pAttractPowerSliderβδ = 0;
	ofxFloatSlider AttractDistanceSliderβδ;
	float pAttractDistanceSliderβδ = 0;
	ofxFloatSlider ProbAttractSliderβδ;
	float ProbAttractβδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderβδ;
	float ViscosityAttractβδ = 0.7F;

	ofxFloatSlider RepelPowerSliderβδ;
	float pRepelPowerSliderβδ = 0;
	ofxFloatSlider RepelDistanceSliderβδ;
	float pRepelDistanceSliderβδ = 0;
	ofxFloatSlider ProbRepelSliderβδ;
	float ProbRepelβδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderβδ;
	float ViscosityRepelβδ = 0.7F;

	ofxFloatSlider AttractPowerSliderβε;
	float pAttractPowerSliderβε = 0;
	ofxFloatSlider AttractDistanceSliderβε;
	float pAttractDistanceSliderβε = 0;
	ofxFloatSlider ProbAttractSliderβε;
	float ProbAttractβε = 100.00;
	ofxFloatSlider ViscosityAttractSliderβε;
	float ViscosityAttractβε = 0.7F;

	ofxFloatSlider RepelPowerSliderβε;
	float pRepelPowerSliderβε = 0;
	ofxFloatSlider RepelDistanceSliderβε;
	float pRepelDistanceSliderβε = 0;
	ofxFloatSlider ProbRepelSliderβε;
	float ProbRepelβε = 100.00;
	ofxFloatSlider ViscosityRepelSliderβε;
	float ViscosityRepelβε = 0.7F;

	ofxFloatSlider AttractPowerSliderβζ;
	float pAttractPowerSliderβζ = 0;
	ofxFloatSlider AttractDistanceSliderβζ;
	float pAttractDistanceSliderβζ = 0;
	ofxFloatSlider ProbAttractSliderβζ;
	float ProbAttractβζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderβζ;
	float ViscosityAttractβζ = 0.7F;

	ofxFloatSlider RepelPowerSliderβζ;
	float pRepelPowerSliderβζ = 0;
	ofxFloatSlider RepelDistanceSliderβζ;
	float pRepelDistanceSliderβζ = 0;
	ofxFloatSlider ProbRepelSliderβζ;
	float ProbRepelβζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderβζ;
	float ViscosityRepelβζ = 0.7F;

	ofxFloatSlider AttractPowerSliderβη;
	float pAttractPowerSliderβη = 0;
	ofxFloatSlider AttractDistanceSliderβη;
	float pAttractDistanceSliderβη = 0;
	ofxFloatSlider ProbAttractSliderβη;
	float ProbAttractβη = 100.00;
	ofxFloatSlider ViscosityAttractSliderβη;
	float ViscosityAttractβη = 0.7F;

	ofxFloatSlider RepelPowerSliderβη;
	float pRepelPowerSliderβη = 0;
	ofxFloatSlider RepelDistanceSliderβη;
	float pRepelDistanceSliderβη = 0;
	ofxFloatSlider ProbRepelSliderβη;
	float ProbRepelβη = 100.00;
	ofxFloatSlider ViscosityRepelSliderβη;
	float ViscosityRepelβη = 0.7F;

	ofxFloatSlider AttractPowerSliderβθ;
	float pAttractPowerSliderβθ = 0;
	ofxFloatSlider AttractDistanceSliderβθ;
	float pAttractDistanceSliderβθ = 0;
	ofxFloatSlider ProbAttractSliderβθ;
	float ProbAttractβθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderβθ;
	float ViscosityAttractβθ = 0.7F;

	ofxFloatSlider RepelPowerSliderβθ;
	float pRepelPowerSliderβθ = 0;
	ofxFloatSlider RepelDistanceSliderβθ;
	float pRepelDistanceSliderβθ = 0;
	ofxFloatSlider ProbRepelSliderβθ;
	float ProbRepelβθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderβθ;
	float ViscosityRepelβθ = 0.7F;

	// Gamma particle sliders

	ofxFloatSlider AttractPowerSliderγα;
	float pAttractPowerSliderγα = 0;
	ofxFloatSlider AttractDistanceSliderγα;
	float pAttractDistanceSliderγα = 0;
	ofxFloatSlider ProbAttractSliderγα;
	float ProbAttractγα = 100.00;
	ofxFloatSlider ViscosityAttractSliderγα;
	float ViscosityAttractγα = 0.7F;

	ofxFloatSlider RepelPowerSliderγα;
	float pRepelPowerSliderγα = 0;
	ofxFloatSlider RepelDistanceSliderγα;
	float pRepelDistanceSliderγα = 0;
	ofxFloatSlider ProbRepelSliderγα;
	float ProbRepelγα = 100.00;
	ofxFloatSlider ViscosityRepelSliderγα;
	float ViscosityRepelγα = 0.7F;

	ofxFloatSlider AttractPowerSliderγβ;
	float pAttractPowerSliderγβ = 0;
	ofxFloatSlider AttractDistanceSliderγβ;
	float pAttractDistanceSliderγβ = 0;
	ofxFloatSlider ProbAttractSliderγβ;
	float ProbAttractγβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderγβ;
	float ViscosityAttractγβ = 0.7F;

	ofxFloatSlider RepelPowerSliderγβ;
	float pRepelPowerSliderγβ = 0;
	ofxFloatSlider RepelDistanceSliderγβ;
	float pRepelDistanceSliderγβ = 0;
	ofxFloatSlider ProbRepelSliderγβ;
	float ProbRepelγβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderγβ;
	float ViscosityRepelγβ = 0.7F;

	ofxFloatSlider AttractPowerSliderγγ;
	float pAttractPowerSliderγγ = 0;
	ofxFloatSlider AttractDistanceSliderγγ;
	float pAttractDistanceSliderγγ = 0;
	ofxFloatSlider ProbAttractSliderγγ;
	float ProbAttractγγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderγγ;
	float ViscosityAttractγγ = 0.7F;

	ofxFloatSlider RepelPowerSliderγγ;
	float pRepelPowerSliderγγ = 0;
	ofxFloatSlider RepelDistanceSliderγγ;
	float pRepelDistanceSliderγγ = 0;
	ofxFloatSlider ProbRepelSliderγγ;
	float ProbRepelγγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderγγ;
	float ViscosityRepelγγ = 0.7F;

	ofxFloatSlider AttractPowerSliderγδ;
	float pAttractPowerSliderγδ = 0;
	ofxFloatSlider AttractDistanceSliderγδ;
	float pAttractDistanceSliderγδ = 0;
	ofxFloatSlider ProbAttractSliderγδ;
	float ProbAttractγδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderγδ;
	float ViscosityAttractγδ = 0.7F;

	ofxFloatSlider RepelPowerSliderγδ;
	float pRepelPowerSliderγδ = 0;
	ofxFloatSlider RepelDistanceSliderγδ;
	float pRepelDistanceSliderγδ = 0;
	ofxFloatSlider ProbRepelSliderγδ;
	float ProbRepelγδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderγδ;
	float ViscosityRepelγδ = 0.7F;

	ofxFloatSlider AttractPowerSliderγε;
	float pAttractPowerSliderγε = 0;
	ofxFloatSlider AttractDistanceSliderγε;
	float pAttractDistanceSliderγε = 0;
	ofxFloatSlider ProbAttractSliderγε;
	float ProbAttractγε = 100.00;
	ofxFloatSlider ViscosityAttractSliderγε;
	float ViscosityAttractγε = 0.7F;

	ofxFloatSlider RepelPowerSliderγε;
	float pRepelPowerSliderγε = 0;
	ofxFloatSlider RepelDistanceSliderγε;
	float pRepelDistanceSliderγε = 0;
	ofxFloatSlider ProbRepelSliderγε;
	float ProbRepelγε = 100.00;
	ofxFloatSlider ViscosityRepelSliderγε;
	float ViscosityRepelγε = 0.7F;

	ofxFloatSlider AttractPowerSliderγζ;
	float pAttractPowerSliderγζ = 0;
	ofxFloatSlider AttractDistanceSliderγζ;
	float pAttractDistanceSliderγζ = 0;
	ofxFloatSlider ProbAttractSliderγζ;
	float ProbAttractγζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderγζ;
	float ViscosityAttractγζ = 0.7F;

	ofxFloatSlider RepelPowerSliderγζ;
	float pRepelPowerSliderγζ = 0;
	ofxFloatSlider RepelDistanceSliderγζ;
	float pRepelDistanceSliderγζ = 0;
	ofxFloatSlider ProbRepelSliderγζ;
	float ProbRepelγζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderγζ;
	float ViscosityRepelγζ = 0.7F;

	ofxFloatSlider AttractPowerSliderγη;
	float pAttractPowerSliderγη = 0;
	ofxFloatSlider AttractDistanceSliderγη;
	float pAttractDistanceSliderγη = 0;
	ofxFloatSlider ProbAttractSliderγη;
	float ProbAttractγη = 100.00;
	ofxFloatSlider ViscosityAttractSliderγη;
	float ViscosityAttractγη = 0.7F;

	ofxFloatSlider RepelPowerSliderγη;
	float pRepelPowerSliderγη = 0;
	ofxFloatSlider RepelDistanceSliderγη;
	float pRepelDistanceSliderγη = 0;
	ofxFloatSlider ProbRepelSliderγη;
	float ProbRepelγη = 100.00;
	ofxFloatSlider ViscosityRepelSliderγη;
	float ViscosityRepelγη = 0.7F;

	ofxFloatSlider AttractPowerSliderγθ;
	float pAttractPowerSliderγθ = 0;
	ofxFloatSlider AttractDistanceSliderγθ;
	float pAttractDistanceSliderγθ = 0;
	ofxFloatSlider ProbAttractSliderγθ;
	float ProbAttractγθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderγθ;
	float ViscosityAttractγθ = 0.7F;

	ofxFloatSlider RepelPowerSliderγθ;
	float pRepelPowerSliderγθ = 0;
	ofxFloatSlider RepelDistanceSliderγθ;
	float pRepelDistanceSliderγθ = 0;
	ofxFloatSlider ProbRepelSliderγθ;
	float ProbRepelγθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderγθ;
	float ViscosityRepelγθ = 0.7F;

	// Delta particle sliders

	ofxFloatSlider AttractPowerSliderδα;
	float pAttractPowerSliderδα = 0;
	ofxFloatSlider AttractDistanceSliderδα;
	float pAttractDistanceSliderδα = 0;
	ofxFloatSlider ProbAttractSliderδα;
	float ProbAttractδα = 100.00;
	ofxFloatSlider ViscosityAttractSliderδα;
	float ViscosityAttractδα = 0.7F;

	ofxFloatSlider RepelPowerSliderδα;
	float pRepelPowerSliderδα = 0;
	ofxFloatSlider RepelDistanceSliderδα;
	float pRepelDistanceSliderδα = 0;
	ofxFloatSlider ProbRepelSliderδα;
	float ProbRepelδα = 100.00;
	ofxFloatSlider ViscosityRepelSliderδα;
	float ViscosityRepelδα = 0.7F;

	ofxFloatSlider AttractPowerSliderδβ;
	float pAttractPowerSliderδβ = 0;
	ofxFloatSlider AttractDistanceSliderδβ;
	float pAttractDistanceSliderδβ = 0;
	ofxFloatSlider ProbAttractSliderδβ;
	float ProbAttractδβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderδβ;
	float ViscosityAttractδβ = 0.7F;

	ofxFloatSlider RepelPowerSliderδβ;
	float pRepelPowerSliderδβ = 0;
	ofxFloatSlider RepelDistanceSliderδβ;
	float pRepelDistanceSliderδβ = 0;
	ofxFloatSlider ProbRepelSliderδβ;
	float ProbRepelδβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderδβ;
	float ViscosityRepelδβ = 0.7F;

	ofxFloatSlider AttractPowerSliderδγ;
	float pAttractPowerSliderδγ = 0;
	ofxFloatSlider AttractDistanceSliderδγ;
	float pAttractDistanceSliderδγ = 0;
	ofxFloatSlider ProbAttractSliderδγ;
	float ProbAttractδγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderδγ;
	float ViscosityAttractδγ = 0.7F;

	ofxFloatSlider RepelPowerSliderδγ;
	float pRepelPowerSliderδγ = 0;
	ofxFloatSlider RepelDistanceSliderδγ;
	float pRepelDistanceSliderδγ = 0;
	ofxFloatSlider ProbRepelSliderδγ;
	float ProbRepelδγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderδγ;
	float ViscosityRepelδγ = 0.7F;

	ofxFloatSlider AttractPowerSliderδδ;
	float pAttractPowerSliderδδ = 0;
	ofxFloatSlider AttractDistanceSliderδδ;
	float pAttractDistanceSliderδδ = 0;
	ofxFloatSlider ProbAttractSliderδδ;
	float ProbAttractδδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderδδ;
	float ViscosityAttractδδ = 0.7F;

	ofxFloatSlider RepelPowerSliderδδ;
	float pRepelPowerSliderδδ = 0;
	ofxFloatSlider RepelDistanceSliderδδ;
	float pRepelDistanceSliderδδ = 0;
	ofxFloatSlider ProbRepelSliderδδ;
	float ProbRepelδδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderδδ;
	float ViscosityRepelδδ = 0.7F;

	ofxFloatSlider AttractPowerSliderδε;
	float pAttractPowerSliderδε = 0;
	ofxFloatSlider AttractDistanceSliderδε;
	float pAttractDistanceSliderδε = 0;
	ofxFloatSlider ProbAttractSliderδε;
	float ProbAttractδε = 100.00;
	ofxFloatSlider ViscosityAttractSliderδε;
	float ViscosityAttractδε = 0.7F;

	ofxFloatSlider RepelPowerSliderδε;
	float pRepelPowerSliderδε = 0;
	ofxFloatSlider RepelDistanceSliderδε;
	float pRepelDistanceSliderδε = 0;
	ofxFloatSlider ProbRepelSliderδε;
	float ProbRepelδε = 100.00;
	ofxFloatSlider ViscosityRepelSliderδε;
	float ViscosityRepelδε = 0.7F;

	ofxFloatSlider AttractPowerSliderδζ;
	float pAttractPowerSliderδζ = 0;
	ofxFloatSlider AttractDistanceSliderδζ;
	float pAttractDistanceSliderδζ = 0;
	ofxFloatSlider ProbAttractSliderδζ;
	float ProbAttractδζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderδζ;
	float ViscosityAttractδζ = 0.7F;

	ofxFloatSlider RepelPowerSliderδζ;
	float pRepelPowerSliderδζ = 0;
	ofxFloatSlider RepelDistanceSliderδζ;
	float pRepelDistanceSliderδζ = 0;
	ofxFloatSlider ProbRepelSliderδζ;
	float ProbRepelδζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderδζ;
	float ViscosityRepelδζ = 0.7F;

	ofxFloatSlider AttractPowerSliderδη;
	float pAttractPowerSliderδη = 0;
	ofxFloatSlider AttractDistanceSliderδη;
	float pAttractDistanceSliderδη = 0;
	ofxFloatSlider ProbAttractSliderδη;
	float ProbAttractδη = 100.00;
	ofxFloatSlider ViscosityAttractSliderδη;
	float ViscosityAttractδη = 0.7F;

	ofxFloatSlider RepelPowerSliderδη;
	float pRepelPowerSliderδη = 0;
	ofxFloatSlider RepelDistanceSliderδη;
	float pRepelDistanceSliderδη = 0;
	ofxFloatSlider ProbRepelSliderδη;
	float ProbRepelδη = 100.00;
	ofxFloatSlider ViscosityRepelSliderδη;
	float ViscosityRepelδη = 0.7F;

	ofxFloatSlider AttractPowerSliderδθ;
	float pAttractPowerSliderδθ = 0;
	ofxFloatSlider AttractDistanceSliderδθ;
	float pAttractDistanceSliderδθ = 0;
	ofxFloatSlider ProbAttractSliderδθ;
	float ProbAttractδθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderδθ;
	float ViscosityAttractδθ = 0.7F;

	ofxFloatSlider RepelPowerSliderδθ;
	float pRepelPowerSliderδθ = 0;
	ofxFloatSlider RepelDistanceSliderδθ;
	float pRepelDistanceSliderδθ = 0;
	ofxFloatSlider ProbRepelSliderδθ;
	float ProbRepelδθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderδθ;
	float ViscosityRepelδθ = 0.7F;

	// Epsilon particle sliders

	ofxFloatSlider AttractPowerSliderεα;
	float pAttractPowerSliderεα = 0;
	ofxFloatSlider AttractDistanceSliderεα;
	float pAttractDistanceSliderεα = 0;
	ofxFloatSlider ProbAttractSliderεα;
	float ProbAttractεα = 100.00;
	ofxFloatSlider ViscosityAttractSliderεα;
	float ViscosityAttractεα = 0.7F;

	ofxFloatSlider RepelPowerSliderεα;
	float pRepelPowerSliderεα = 0;
	ofxFloatSlider RepelDistanceSliderεα;
	float pRepelDistanceSliderεα = 0;
	ofxFloatSlider ProbRepelSliderεα;
	float ProbRepelεα = 100.00;
	ofxFloatSlider ViscosityRepelSliderεα;
	float ViscosityRepelεα = 0.7F;

	ofxFloatSlider AttractPowerSliderεβ;
	float pAttractPowerSliderεβ = 0;
	ofxFloatSlider AttractDistanceSliderεβ;
	float pAttractDistanceSliderεβ = 0;
	ofxFloatSlider ProbAttractSliderεβ;
	float ProbAttractεβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderεβ;
	float ViscosityAttractεβ = 0.7F;

	ofxFloatSlider RepelPowerSliderεβ;
	float pRepelPowerSliderεβ = 0;
	ofxFloatSlider RepelDistanceSliderεβ;
	float pRepelDistanceSliderεβ = 0;
	ofxFloatSlider ProbRepelSliderεβ;
	float ProbRepelεβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderεβ;
	float ViscosityRepelεβ = 0.7F;

	ofxFloatSlider AttractPowerSliderεγ;
	float pAttractPowerSliderεγ = 0;
	ofxFloatSlider AttractDistanceSliderεγ;
	float pAttractDistanceSliderεγ = 0;
	ofxFloatSlider ProbAttractSliderεγ;
	float ProbAttractεγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderεγ;
	float ViscosityAttractεγ = 0.7F;

	ofxFloatSlider RepelPowerSliderεγ;
	float pRepelPowerSliderεγ = 0;
	ofxFloatSlider RepelDistanceSliderεγ;
	float pRepelDistanceSliderεγ = 0;
	ofxFloatSlider ProbRepelSliderεγ;
	float ProbRepelεγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderεγ;
	float ViscosityRepelεγ = 0.7F;

	ofxFloatSlider AttractPowerSliderεδ;
	float pAttractPowerSliderεδ = 0;
	ofxFloatSlider AttractDistanceSliderεδ;
	float pAttractDistanceSliderεδ = 0;
	ofxFloatSlider ProbAttractSliderεδ;
	float ProbAttractεδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderεδ;
	float ViscosityAttractεδ = 0.7F;

	ofxFloatSlider RepelPowerSliderεδ;
	float pRepelPowerSliderεδ = 0;
	ofxFloatSlider RepelDistanceSliderεδ;
	float pRepelDistanceSliderεδ = 0;
	ofxFloatSlider ProbRepelSliderεδ;
	float ProbRepelεδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderεδ;
	float ViscosityRepelεδ = 0.7F;

	ofxFloatSlider AttractPowerSliderεε;
	float pAttractPowerSliderεε = 0;
	ofxFloatSlider AttractDistanceSliderεε;
	float pAttractDistanceSliderεε = 0;
	ofxFloatSlider ProbAttractSliderεε;
	float ProbAttractεε = 100.00;
	ofxFloatSlider ViscosityAttractSliderεε;
	float ViscosityAttractεε = 0.7F;

	ofxFloatSlider RepelPowerSliderεε;
	float pRepelPowerSliderεε = 0;
	ofxFloatSlider RepelDistanceSliderεε;
	float pRepelDistanceSliderεε = 0;
	ofxFloatSlider ProbRepelSliderεε;
	float ProbRepelεε = 100.00;
	ofxFloatSlider ViscosityRepelSliderεε;
	float ViscosityRepelεε = 0.7F;

	ofxFloatSlider AttractPowerSliderεζ;
	float pAttractPowerSliderεζ = 0;
	ofxFloatSlider AttractDistanceSliderεζ;
	float pAttractDistanceSliderεζ = 0;
	ofxFloatSlider ProbAttractSliderεζ;
	float ProbAttractεζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderεζ;
	float ViscosityAttractεζ = 0.7F;

	ofxFloatSlider RepelPowerSliderεζ;
	float pRepelPowerSliderεζ = 0;
	ofxFloatSlider RepelDistanceSliderεζ;
	float pRepelDistanceSliderεζ = 0;
	ofxFloatSlider ProbRepelSliderεζ;
	float ProbRepelεζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderεζ;
	float ViscosityRepelεζ = 0.7F;

	ofxFloatSlider AttractPowerSliderεη;
	float pAttractPowerSliderεη = 0;
	ofxFloatSlider AttractDistanceSliderεη;
	float pAttractDistanceSliderεη = 0;
	ofxFloatSlider ProbAttractSliderεη;
	float ProbAttractεη = 100.00;
	ofxFloatSlider ViscosityAttractSliderεη;
	float ViscosityAttractεη = 0.7F;

	ofxFloatSlider RepelPowerSliderεη;
	float pRepelPowerSliderεη = 0;
	ofxFloatSlider RepelDistanceSliderεη;
	float pRepelDistanceSliderεη = 0;
	ofxFloatSlider ProbRepelSliderεη;
	float ProbRepelεη = 100.00;
	ofxFloatSlider ViscosityRepelSliderεη;
	float ViscosityRepelεη = 0.7F;

	ofxFloatSlider AttractPowerSliderεθ;
	float pAttractPowerSliderεθ = 0;
	ofxFloatSlider AttractDistanceSliderεθ;
	float pAttractDistanceSliderεθ = 0;
	ofxFloatSlider ProbAttractSliderεθ;
	float ProbAttractεθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderεθ;
	float ViscosityAttractεθ = 0.7F;

	ofxFloatSlider RepelPowerSliderεθ;
	float pRepelPowerSliderεθ = 0;
	ofxFloatSlider RepelDistanceSliderεθ;
	float pRepelDistanceSliderεθ = 0;
	ofxFloatSlider ProbRepelSliderεθ;
	float ProbRepelεθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderεθ;
	float ViscosityRepelεθ = 0.7F;

	// Zeta particle sliders

	ofxFloatSlider AttractPowerSliderζα;
	float pAttractPowerSliderζα = 0;
	ofxFloatSlider AttractDistanceSliderζα;
	float pAttractDistanceSliderζα = 0;
	ofxFloatSlider ProbAttractSliderζα;
	float ProbAttractζα = 100.00;
	ofxFloatSlider ViscosityAttractSliderζα;
	float ViscosityAttractζα = 0.7F;

	ofxFloatSlider RepelPowerSliderζα;
	float pRepelPowerSliderζα = 0;
	ofxFloatSlider RepelDistanceSliderζα;
	float pRepelDistanceSliderζα = 0;
	ofxFloatSlider ProbRepelSliderζα;
	float ProbRepelζα = 100.00;
	ofxFloatSlider ViscosityRepelSliderζα;
	float ViscosityRepelζα = 0.7F;

	ofxFloatSlider AttractPowerSliderζβ;
	float pAttractPowerSliderζβ = 0;
	ofxFloatSlider AttractDistanceSliderζβ;
	float pAttractDistanceSliderζβ = 0;
	ofxFloatSlider ProbAttractSliderζβ;
	float ProbAttractζβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderζβ;
	float ViscosityAttractζβ = 0.7F;

	ofxFloatSlider RepelPowerSliderζβ;
	float pRepelPowerSliderζβ = 0;
	ofxFloatSlider RepelDistanceSliderζβ;
	float pRepelDistanceSliderζβ = 0;
	ofxFloatSlider ProbRepelSliderζβ;
	float ProbRepelζβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderζβ;
	float ViscosityRepelζβ = 0.7F;

	ofxFloatSlider AttractPowerSliderζγ;
	float pAttractPowerSliderζγ = 0;
	ofxFloatSlider AttractDistanceSliderζγ;
	float pAttractDistanceSliderζγ = 0;
	ofxFloatSlider ProbAttractSliderζγ;
	float ProbAttractζγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderζγ;
	float ViscosityAttractζγ = 0.7F;

	ofxFloatSlider RepelPowerSliderζγ;
	float pRepelPowerSliderζγ = 0;
	ofxFloatSlider RepelDistanceSliderζγ;
	float pRepelDistanceSliderζγ = 0;
	ofxFloatSlider ProbRepelSliderζγ;
	float ProbRepelζγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderζγ;
	float ViscosityRepelζγ = 0.7F;

	ofxFloatSlider AttractPowerSliderζδ;
	float pAttractPowerSliderζδ = 0;
	ofxFloatSlider AttractDistanceSliderζδ;
	float pAttractDistanceSliderζδ = 0;
	ofxFloatSlider ProbAttractSliderζδ;
	float ProbAttractζδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderζδ;
	float ViscosityAttractζδ = 0.7F;

	ofxFloatSlider RepelPowerSliderζδ;
	float pRepelPowerSliderζδ = 0;
	ofxFloatSlider RepelDistanceSliderζδ;
	float pRepelDistanceSliderζδ = 0;
	ofxFloatSlider ProbRepelSliderζδ;
	float ProbRepelζδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderζδ;
	float ViscosityRepelζδ = 0.7F;

	ofxFloatSlider AttractPowerSliderζε;
	float pAttractPowerSliderζε = 0;
	ofxFloatSlider AttractDistanceSliderζε;
	float pAttractDistanceSliderζε = 0;
	ofxFloatSlider ProbAttractSliderζε;
	float ProbAttractζε = 100.00;
	ofxFloatSlider ViscosityAttractSliderζε;
	float ViscosityAttractζε = 0.7F;

	ofxFloatSlider RepelPowerSliderζε;
	float pRepelPowerSliderζε = 0;
	ofxFloatSlider RepelDistanceSliderζε;
	float pRepelDistanceSliderζε = 0;
	ofxFloatSlider ProbRepelSliderζε;
	float ProbRepelζε = 100.00;
	ofxFloatSlider ViscosityRepelSliderζε;
	float ViscosityRepelζε = 0.7F;

	ofxFloatSlider AttractPowerSliderζζ;
	float pAttractPowerSliderζζ = 0;
	ofxFloatSlider AttractDistanceSliderζζ;
	float pAttractDistanceSliderζζ = 0;
	ofxFloatSlider ProbAttractSliderζζ;
	float ProbAttractζζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderζζ;
	float ViscosityAttractζζ = 0.7F;

	ofxFloatSlider RepelPowerSliderζζ;
	float pRepelPowerSliderζζ = 0;
	ofxFloatSlider RepelDistanceSliderζζ;
	float pRepelDistanceSliderζζ = 0;
	ofxFloatSlider ProbRepelSliderζζ;
	float ProbRepelζζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderζζ;
	float ViscosityRepelζζ = 0.7F;

	ofxFloatSlider AttractPowerSliderζη;
	float pAttractPowerSliderζη = 0;
	ofxFloatSlider AttractDistanceSliderζη;
	float pAttractDistanceSliderζη = 0;
	ofxFloatSlider ProbAttractSliderζη;
	float ProbAttractζη = 100.00;
	ofxFloatSlider ViscosityAttractSliderζη;
	float ViscosityAttractζη = 0.7F;

	ofxFloatSlider RepelPowerSliderζη;
	float pRepelPowerSliderζη = 0;
	ofxFloatSlider RepelDistanceSliderζη;
	float pRepelDistanceSliderζη = 0;
	ofxFloatSlider ProbRepelSliderζη;
	float ProbRepelζη = 100.00;
	ofxFloatSlider ViscosityRepelSliderζη;
	float ViscosityRepelζη = 0.7F;

	ofxFloatSlider AttractPowerSliderζθ;
	float pAttractPowerSliderζθ = 0;
	ofxFloatSlider AttractDistanceSliderζθ;
	float pAttractDistanceSliderζθ = 0;
	ofxFloatSlider ProbAttractSliderζθ;
	float ProbAttractζθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderζθ;
	float ViscosityAttractζθ = 0.7F;

	ofxFloatSlider RepelPowerSliderζθ;
	float pRepelPowerSliderζθ = 0;
	ofxFloatSlider RepelDistanceSliderζθ;
	float pRepelDistanceSliderζθ = 0;
	ofxFloatSlider ProbRepelSliderζθ;
	float ProbRepelζθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderζθ;
	float ViscosityRepelζθ = 0.7F;

	// Etha particle sliders

	ofxFloatSlider AttractPowerSliderηα;
	float pAttractPowerSliderηα = 0;
	ofxFloatSlider AttractDistanceSliderηα;
	float pAttractDistanceSliderηα = 0;
	ofxFloatSlider ProbAttractSliderηα;
	float ProbAttractηα = 100.00;
	ofxFloatSlider ViscosityAttractSliderηα;
	float ViscosityAttractηα = 0.7F;

	ofxFloatSlider RepelPowerSliderηα;
	float pRepelPowerSliderηα = 0;
	ofxFloatSlider RepelDistanceSliderηα;
	float pRepelDistanceSliderηα = 0;
	ofxFloatSlider ProbRepelSliderηα;
	float ProbRepelηα = 100.00;
	ofxFloatSlider ViscosityRepelSliderηα;
	float ViscosityRepelηα = 0.7F;

	ofxFloatSlider AttractPowerSliderηβ;
	float pAttractPowerSliderηβ = 0;
	ofxFloatSlider AttractDistanceSliderηβ;
	float pAttractDistanceSliderηβ = 0;
	ofxFloatSlider ProbAttractSliderηβ;
	float ProbAttractηβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderηβ;
	float ViscosityAttractηβ = 0.7F;

	ofxFloatSlider RepelPowerSliderηβ;
	float pRepelPowerSliderηβ = 0;
	ofxFloatSlider RepelDistanceSliderηβ;
	float pRepelDistanceSliderηβ = 0;
	ofxFloatSlider ProbRepelSliderηβ;
	float ProbRepelηβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderηβ;
	float ViscosityRepelηβ = 0.7F;

	ofxFloatSlider AttractPowerSliderηγ;
	float pAttractPowerSliderηγ = 0;
	ofxFloatSlider AttractDistanceSliderηγ;
	float pAttractDistanceSliderηγ = 0;
	ofxFloatSlider ProbAttractSliderηγ;
	float ProbAttractηγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderηγ;
	float ViscosityAttractηγ = 0.7F;

	ofxFloatSlider RepelPowerSliderηγ;
	float pRepelPowerSliderηγ = 0;
	ofxFloatSlider RepelDistanceSliderηγ;
	float pRepelDistanceSliderηγ = 0;
	ofxFloatSlider ProbRepelSliderηγ;
	float ProbRepelηγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderηγ;
	float ViscosityRepelηγ = 0.7F;

	ofxFloatSlider AttractPowerSliderηδ;
	float pAttractPowerSliderηδ = 0;
	ofxFloatSlider AttractDistanceSliderηδ;
	float pAttractDistanceSliderηδ = 0;
	ofxFloatSlider ProbAttractSliderηδ;
	float ProbAttractηδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderηδ;
	float ViscosityAttractηδ = 0.7F;

	ofxFloatSlider RepelPowerSliderηδ;
	float pRepelPowerSliderηδ = 0;
	ofxFloatSlider RepelDistanceSliderηδ;
	float pRepelDistanceSliderηδ = 0;
	ofxFloatSlider ProbRepelSliderηδ;
	float ProbRepelηδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderηδ;
	float ViscosityRepelηδ = 0.7F;

	ofxFloatSlider AttractPowerSliderηε;
	float pAttractPowerSliderηε = 0;
	ofxFloatSlider AttractDistanceSliderηε;
	float pAttractDistanceSliderηε = 0;
	ofxFloatSlider ProbAttractSliderηε;
	float ProbAttractηε = 100.00;
	ofxFloatSlider ViscosityAttractSliderηε;
	float ViscosityAttractηε = 0.7F;

	ofxFloatSlider RepelPowerSliderηε;
	float pRepelPowerSliderηε = 0;
	ofxFloatSlider RepelDistanceSliderηε;
	float pRepelDistanceSliderηε = 0;
	ofxFloatSlider ProbRepelSliderηε;
	float ProbRepelηε = 100.00;
	ofxFloatSlider ViscosityRepelSliderηε;
	float ViscosityRepelηε = 0.7F;

	ofxFloatSlider AttractPowerSliderηζ;
	float pAttractPowerSliderηζ = 0;
	ofxFloatSlider AttractDistanceSliderηζ;
	float pAttractDistanceSliderηζ = 0;
	ofxFloatSlider ProbAttractSliderηζ;
	float ProbAttractηζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderηζ;
	float ViscosityAttractηζ = 0.7F;

	ofxFloatSlider RepelPowerSliderηζ;
	float pRepelPowerSliderηζ = 0;
	ofxFloatSlider RepelDistanceSliderηζ;
	float pRepelDistanceSliderηζ = 0;
	ofxFloatSlider ProbRepelSliderηζ;
	float ProbRepelηζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderηζ;
	float ViscosityRepelηζ = 0.7F;

	ofxFloatSlider AttractPowerSliderηη;
	float pAttractPowerSliderηη = 0;
	ofxFloatSlider AttractDistanceSliderηη;
	float pAttractDistanceSliderηη = 0;
	ofxFloatSlider ProbAttractSliderηη;
	float ProbAttractηη = 100.00;
	ofxFloatSlider ViscosityAttractSliderηη;
	float ViscosityAttractηη = 0.7F;

	ofxFloatSlider RepelPowerSliderηη;
	float pRepelPowerSliderηη = 0;
	ofxFloatSlider RepelDistanceSliderηη;
	float pRepelDistanceSliderηη = 0;
	ofxFloatSlider ProbRepelSliderηη;
	float ProbRepelηη = 100.00;
	ofxFloatSlider ViscosityRepelSliderηη;
	float ViscosityRepelηη = 0.7F;

	ofxFloatSlider AttractPowerSliderηθ;
	float pAttractPowerSliderηθ = 0;
	ofxFloatSlider AttractDistanceSliderηθ;
	float pAttractDistanceSliderηθ = 0;
	ofxFloatSlider ProbAttractSliderηθ;
	float ProbAttractηθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderηθ;
	float ViscosityAttractηθ = 0.7F;

	ofxFloatSlider RepelPowerSliderηθ;
	float pRepelPowerSliderηθ = 0;
	ofxFloatSlider RepelDistanceSliderηθ;
	float pRepelDistanceSliderηθ = 0;
	ofxFloatSlider ProbRepelSliderηθ;
	float ProbRepelηθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderηθ;
	float ViscosityRepelηθ = 0.7F;

	// Tetha particle sliders

	ofxFloatSlider AttractPowerSliderθα;
	float pAttractPowerSliderθα = 0;
	ofxFloatSlider AttractDistanceSliderθα;
	float pAttractDistanceSliderθα = 0;
	ofxFloatSlider ProbAttractSliderθα;
	float ProbAttractθα = 100.00;
	ofxFloatSlider ViscosityAttractSliderθα;
	float ViscosityAttractθα = 0.7F;

	ofxFloatSlider RepelPowerSliderθα;
	float pRepelPowerSliderθα = 0;
	ofxFloatSlider RepelDistanceSliderθα;
	float pRepelDistanceSliderθα = 0;
	ofxFloatSlider ProbRepelSliderθα;
	float ProbRepelθα = 100.00;
	ofxFloatSlider ViscosityRepelSliderθα;
	float ViscosityRepelθα = 0.7F;

	ofxFloatSlider AttractPowerSliderθβ;
	float pAttractPowerSliderθβ = 0;
	ofxFloatSlider AttractDistanceSliderθβ;
	float pAttractDistanceSliderθβ = 0;
	ofxFloatSlider ProbAttractSliderθβ;
	float ProbAttractθβ = 100.00;
	ofxFloatSlider ViscosityAttractSliderθβ;
	float ViscosityAttractθβ = 0.7F;

	ofxFloatSlider RepelPowerSliderθβ;
	float pRepelPowerSliderθβ = 0;
	ofxFloatSlider RepelDistanceSliderθβ;
	float pRepelDistanceSliderθβ = 0;
	ofxFloatSlider ProbRepelSliderθβ;
	float ProbRepelθβ = 100.00;
	ofxFloatSlider ViscosityRepelSliderθβ;
	float ViscosityRepelθβ = 0.7F;

	ofxFloatSlider AttractPowerSliderθγ;
	float pAttractPowerSliderθγ = 0;
	ofxFloatSlider AttractDistanceSliderθγ;
	float pAttractDistanceSliderθγ = 0;
	ofxFloatSlider ProbAttractSliderθγ;
	float ProbAttractθγ = 100.00;
	ofxFloatSlider ViscosityAttractSliderθγ;
	float ViscosityAttractθγ = 0.7F;

	ofxFloatSlider RepelPowerSliderθγ;
	float pRepelPowerSliderθγ = 0;
	ofxFloatSlider RepelDistanceSliderθγ;
	float pRepelDistanceSliderθγ = 0;
	ofxFloatSlider ProbRepelSliderθγ;
	float ProbRepelθγ = 100.00;
	ofxFloatSlider ViscosityRepelSliderθγ;
	float ViscosityRepelθγ = 0.7F;

	ofxFloatSlider AttractPowerSliderθδ;
	float pAttractPowerSliderθδ = 0;
	ofxFloatSlider AttractDistanceSliderθδ;
	float pAttractDistanceSliderθδ = 0;
	ofxFloatSlider ProbAttractSliderθδ;
	float ProbAttractθδ = 100.00;
	ofxFloatSlider ViscosityAttractSliderθδ;
	float ViscosityAttractθδ = 0.7F;

	ofxFloatSlider RepelPowerSliderθδ;
	float pRepelPowerSliderθδ = 0;
	ofxFloatSlider RepelDistanceSliderθδ;
	float pRepelDistanceSliderθδ = 0;
	ofxFloatSlider ProbRepelSliderθδ;
	float ProbRepelθδ = 100.00;
	ofxFloatSlider ViscosityRepelSliderθδ;
	float ViscosityRepelθδ = 0.7F;

	ofxFloatSlider AttractPowerSliderθε;
	float pAttractPowerSliderθε = 0;
	ofxFloatSlider AttractDistanceSliderθε;
	float pAttractDistanceSliderθε = 0;
	ofxFloatSlider ProbAttractSliderθε;
	float ProbAttractθε = 100.00;
	ofxFloatSlider ViscosityAttractSliderθε;
	float ViscosityAttractθε = 0.7F;

	ofxFloatSlider RepelPowerSliderθε;
	float pRepelPowerSliderθε = 0;
	ofxFloatSlider RepelDistanceSliderθε;
	float pRepelDistanceSliderθε = 0;
	ofxFloatSlider ProbRepelSliderθε;
	float ProbRepelθε = 100.00;
	ofxFloatSlider ViscosityRepelSliderθε;
	float ViscosityRepelθε = 0.7F;

	ofxFloatSlider AttractPowerSliderθζ;
	float pAttractPowerSliderθζ = 0;
	ofxFloatSlider AttractDistanceSliderθζ;
	float pAttractDistanceSliderθζ = 0;
	ofxFloatSlider ProbAttractSliderθζ;
	float ProbAttractθζ = 100.00;
	ofxFloatSlider ViscosityAttractSliderθζ;
	float ViscosityAttractθζ = 0.7F;

	ofxFloatSlider RepelPowerSliderθζ;
	float pRepelPowerSliderθζ = 0;
	ofxFloatSlider RepelDistanceSliderθζ;
	float pRepelDistanceSliderθζ = 0;
	ofxFloatSlider ProbRepelSliderθζ;
	float ProbRepelθζ = 100.00;
	ofxFloatSlider ViscosityRepelSliderθζ;
	float ViscosityRepelθζ = 0.7F;

	ofxFloatSlider AttractPowerSliderθη;
	float pAttractPowerSliderθη = 0;
	ofxFloatSlider AttractDistanceSliderθη;
	float pAttractDistanceSliderθη = 0;
	ofxFloatSlider ProbAttractSliderθη;
	float ProbAttractθη = 100.00;
	ofxFloatSlider ViscosityAttractSliderθη;
	float ViscosityAttractθη = 0.7F;

	ofxFloatSlider RepelPowerSliderθη;
	float pRepelPowerSliderθη = 0;
	ofxFloatSlider RepelDistanceSliderθη;
	float pRepelDistanceSliderθη = 0;
	ofxFloatSlider ProbRepelSliderθη;
	float ProbRepelθη = 100.00;
	ofxFloatSlider ViscosityRepelSliderθη;
	float ViscosityRepelθη = 0.7F;

	ofxFloatSlider AttractPowerSliderθθ;
	float pAttractPowerSliderθθ = 0;
	ofxFloatSlider AttractDistanceSliderθθ;
	float pAttractDistanceSliderθθ = 0;
	ofxFloatSlider ProbAttractSliderθθ;
	float ProbAttractθθ = 100.00;
	ofxFloatSlider ViscosityAttractSliderθθ;
	float ViscosityAttractθθ = 0.7F;

	ofxFloatSlider RepelPowerSliderθθ;
	float pRepelPowerSliderθθ = 0;
	ofxFloatSlider RepelDistanceSliderθθ;
	float pRepelDistanceSliderθθ = 0;
	ofxFloatSlider ProbRepelSliderθθ;
	float ProbRepelθθ = 100.00;
	ofxFloatSlider ViscosityRepelSliderθθ;
	float ViscosityRepelθθ = 0.7F;

	ofxLabel labelβ;
	ofxLabel labelα;
	ofxLabel labelγ;
	ofxLabel labelδ;
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
	float worldGravity = 0.0F;
	float AttractForceVariance = 0.5F;
	float AttractRadiusVariance = 0.5F;
	float RepelForceVariance = 0.5F;
	float RepelRadiusVariance = 0.5F;
	float wallRepel = 20.0F;

	vector<ofxFloatSlider*> AttractPowerSliders = {
		&AttractPowerSliderαα, &AttractPowerSliderαβ, &AttractPowerSliderαγ, &AttractPowerSliderαδ, &AttractPowerSliderαε, &AttractPowerSliderαζ, &AttractPowerSliderαη, &AttractPowerSliderαθ,
		&AttractPowerSliderβα, &AttractPowerSliderββ, &AttractPowerSliderβγ, &AttractPowerSliderβδ, &AttractPowerSliderβε, &AttractPowerSliderβζ, &AttractPowerSliderβη, &AttractPowerSliderβθ,
		&AttractPowerSliderγα, &AttractPowerSliderγβ, &AttractPowerSliderγγ, &AttractPowerSliderγδ, &AttractPowerSliderγε, &AttractPowerSliderγζ, &AttractPowerSliderγη, &AttractPowerSliderγθ,
		&AttractPowerSliderδα, &AttractPowerSliderδβ, &AttractPowerSliderδγ, &AttractPowerSliderδδ, &AttractPowerSliderδε, &AttractPowerSliderδζ, &AttractPowerSliderδη, &AttractPowerSliderδθ,
		&AttractPowerSliderεα, &AttractPowerSliderεβ, &AttractPowerSliderεγ, &AttractPowerSliderεδ, &AttractPowerSliderεε, &AttractPowerSliderεζ, &AttractPowerSliderεη, &AttractPowerSliderεθ,
		&AttractPowerSliderζα, &AttractPowerSliderζβ, &AttractPowerSliderζγ, &AttractPowerSliderζδ, &AttractPowerSliderζε, &AttractPowerSliderζζ, &AttractPowerSliderζη, &AttractPowerSliderζθ,
		&AttractPowerSliderηα, &AttractPowerSliderηβ, &AttractPowerSliderηγ, &AttractPowerSliderηδ, &AttractPowerSliderηε, &AttractPowerSliderηζ, &AttractPowerSliderηη, &AttractPowerSliderηθ,
		&AttractPowerSliderθα, &AttractPowerSliderθβ, &AttractPowerSliderθγ, &AttractPowerSliderθδ, &AttractPowerSliderθε, &AttractPowerSliderθζ, &AttractPowerSliderθη, &AttractPowerSliderθθ,
	};
	vector<ofxFloatSlider*> AttractDistanceSliders = {
		&AttractDistanceSliderαα, &AttractDistanceSliderαβ, &AttractDistanceSliderαγ, &AttractDistanceSliderαδ, &AttractDistanceSliderαε, &AttractDistanceSliderαζ, &AttractDistanceSliderαη, &AttractDistanceSliderαθ,
		&AttractDistanceSliderβα, &AttractDistanceSliderββ, &AttractDistanceSliderβγ, &AttractDistanceSliderβδ, &AttractDistanceSliderβε, &AttractDistanceSliderβζ, &AttractDistanceSliderβη, &AttractDistanceSliderβθ,
		&AttractDistanceSliderγα, &AttractDistanceSliderγβ, &AttractDistanceSliderγγ, &AttractDistanceSliderγδ, &AttractDistanceSliderγε, &AttractDistanceSliderγζ, &AttractDistanceSliderγη, &AttractDistanceSliderγθ,
		&AttractDistanceSliderδα, &AttractDistanceSliderδβ, &AttractDistanceSliderδγ, &AttractDistanceSliderδδ, &AttractDistanceSliderδε, &AttractDistanceSliderδζ, &AttractDistanceSliderδη, &AttractDistanceSliderδθ,
		&AttractDistanceSliderεα, &AttractDistanceSliderεβ, &AttractDistanceSliderεγ, &AttractDistanceSliderεδ, &AttractDistanceSliderεε, &AttractDistanceSliderεζ, &AttractDistanceSliderεη, &AttractDistanceSliderεθ,
		&AttractDistanceSliderζα, &AttractDistanceSliderζβ, &AttractDistanceSliderζγ, &AttractDistanceSliderζδ, &AttractDistanceSliderζε, &AttractDistanceSliderζζ, &AttractDistanceSliderζη, &AttractDistanceSliderζθ,
		&AttractDistanceSliderηα, &AttractDistanceSliderηβ, &AttractDistanceSliderηγ, &AttractDistanceSliderηδ, &AttractDistanceSliderηε, &AttractDistanceSliderηζ, &AttractDistanceSliderηη, &AttractDistanceSliderηθ,
		&AttractDistanceSliderθα, &AttractDistanceSliderθβ, &AttractDistanceSliderθγ, &AttractDistanceSliderθδ, &AttractDistanceSliderθε, &AttractDistanceSliderθζ, &AttractDistanceSliderθη, &AttractDistanceSliderθθ,
	};
	vector<ofxFloatSlider*> RepelPowerSliders = {
		&RepelPowerSliderαα, &RepelPowerSliderαβ, &RepelPowerSliderαγ, &RepelPowerSliderαδ, &RepelPowerSliderαε, &RepelPowerSliderαζ, &RepelPowerSliderαη, &RepelPowerSliderαθ,
		&RepelPowerSliderβα, &RepelPowerSliderββ, &RepelPowerSliderβγ, &RepelPowerSliderβδ, &RepelPowerSliderβε, &RepelPowerSliderβζ, &RepelPowerSliderβη, &RepelPowerSliderβθ,
		&RepelPowerSliderγα, &RepelPowerSliderγβ, &RepelPowerSliderγγ, &RepelPowerSliderγδ, &RepelPowerSliderγε, &RepelPowerSliderγζ, &RepelPowerSliderγη, &RepelPowerSliderγθ,
		&RepelPowerSliderδα, &RepelPowerSliderδβ, &RepelPowerSliderδγ, &RepelPowerSliderδδ, &RepelPowerSliderδε, &RepelPowerSliderδζ, &RepelPowerSliderδη, &RepelPowerSliderδθ,
		&RepelPowerSliderεα, &RepelPowerSliderεβ, &RepelPowerSliderεγ, &RepelPowerSliderεδ, &RepelPowerSliderεε, &RepelPowerSliderεζ, &RepelPowerSliderεη, &RepelPowerSliderεθ,
		&RepelPowerSliderζα, &RepelPowerSliderζβ, &RepelPowerSliderζγ, &RepelPowerSliderζδ, &RepelPowerSliderζε, &RepelPowerSliderζζ, &RepelPowerSliderζη, &RepelPowerSliderζθ,
		&RepelPowerSliderηα, &RepelPowerSliderηβ, &RepelPowerSliderηγ, &RepelPowerSliderηδ, &RepelPowerSliderηε, &RepelPowerSliderηζ, &RepelPowerSliderηη, &RepelPowerSliderηθ,
		&RepelPowerSliderθα, &RepelPowerSliderθβ, &RepelPowerSliderθγ, &RepelPowerSliderθδ, &RepelPowerSliderθε, &RepelPowerSliderθζ, &RepelPowerSliderθη, &RepelPowerSliderθθ,
	};
	vector<ofxFloatSlider*> RepelDistanceSliders = {
		&RepelDistanceSliderαα, &RepelDistanceSliderαβ, &RepelDistanceSliderαγ, &RepelDistanceSliderαδ, &RepelDistanceSliderαε, &RepelDistanceSliderαζ, &RepelDistanceSliderαη, &RepelDistanceSliderαθ,
		&RepelDistanceSliderβα, &RepelDistanceSliderββ, &RepelDistanceSliderβγ, &RepelDistanceSliderβδ, &RepelDistanceSliderβε, &RepelDistanceSliderβζ, &RepelDistanceSliderβη, &RepelDistanceSliderβθ,
		&RepelDistanceSliderγα, &RepelDistanceSliderγβ, &RepelDistanceSliderγγ, &RepelDistanceSliderγδ, &RepelDistanceSliderγε, &RepelDistanceSliderγζ, &RepelDistanceSliderγη, &RepelDistanceSliderγθ,
		&RepelDistanceSliderδα, &RepelDistanceSliderδβ, &RepelDistanceSliderδγ, &RepelDistanceSliderδδ, &RepelDistanceSliderδε, &RepelDistanceSliderδζ, &RepelDistanceSliderδη, &RepelDistanceSliderδθ,
		&RepelDistanceSliderεα, &RepelDistanceSliderεβ, &RepelDistanceSliderεγ, &RepelDistanceSliderεδ, &RepelDistanceSliderεε, &RepelDistanceSliderεζ, &RepelDistanceSliderεη, &RepelDistanceSliderεθ,
		&RepelDistanceSliderζα, &RepelDistanceSliderζβ, &RepelDistanceSliderζγ, &RepelDistanceSliderζδ, &RepelDistanceSliderζε, &RepelDistanceSliderζζ, &RepelDistanceSliderζη, &RepelDistanceSliderζθ,
		&RepelDistanceSliderηα, &RepelDistanceSliderηβ, &RepelDistanceSliderηγ, &RepelDistanceSliderηδ, &RepelDistanceSliderηε, &RepelDistanceSliderηζ, &RepelDistanceSliderηη, &RepelDistanceSliderηθ,
		&RepelDistanceSliderθα, &RepelDistanceSliderθβ, &RepelDistanceSliderθγ, &RepelDistanceSliderθδ, &RepelDistanceSliderθε, &RepelDistanceSliderθζ, &RepelDistanceSliderθη, &RepelDistanceSliderθθ,
	};
	vector<ofxFloatSlider*> ViscosityAttractSliders = {
		&ViscosityAttractSliderαα, &ViscosityAttractSliderαβ, &ViscosityAttractSliderαγ, &ViscosityAttractSliderαδ, &ViscosityAttractSliderαε, &ViscosityAttractSliderαζ, &ViscosityAttractSliderαη, &ViscosityAttractSliderαθ,
		&ViscosityAttractSliderβα, &ViscosityAttractSliderββ, &ViscosityAttractSliderβγ, &ViscosityAttractSliderβδ, &ViscosityAttractSliderβε, &ViscosityAttractSliderβζ, &ViscosityAttractSliderβη, &ViscosityAttractSliderβθ,
		&ViscosityAttractSliderγα, &ViscosityAttractSliderγβ, &ViscosityAttractSliderγγ, &ViscosityAttractSliderγδ, &ViscosityAttractSliderγε, &ViscosityAttractSliderγζ, &ViscosityAttractSliderγη, &ViscosityAttractSliderγθ,
		&ViscosityAttractSliderδα, &ViscosityAttractSliderδβ, &ViscosityAttractSliderδγ, &ViscosityAttractSliderδδ, &ViscosityAttractSliderδε, &ViscosityAttractSliderδζ, &ViscosityAttractSliderδη, &ViscosityAttractSliderδθ,
		&ViscosityAttractSliderεα, &ViscosityAttractSliderεβ, &ViscosityAttractSliderεγ, &ViscosityAttractSliderεδ, &ViscosityAttractSliderεε, &ViscosityAttractSliderεζ, &ViscosityAttractSliderεη, &ViscosityAttractSliderεθ,
		&ViscosityAttractSliderζα, &ViscosityAttractSliderζβ, &ViscosityAttractSliderζγ, &ViscosityAttractSliderζδ, &ViscosityAttractSliderζε, &ViscosityAttractSliderζζ, &ViscosityAttractSliderζη, &ViscosityAttractSliderζθ,
		&ViscosityAttractSliderηα, &ViscosityAttractSliderηβ, &ViscosityAttractSliderηγ, &ViscosityAttractSliderηδ, &ViscosityAttractSliderηε, &ViscosityAttractSliderηζ, &ViscosityAttractSliderηη, &ViscosityAttractSliderηθ,
		&ViscosityAttractSliderθα, &ViscosityAttractSliderθβ, &ViscosityAttractSliderθγ, &ViscosityAttractSliderθδ, &ViscosityAttractSliderθε, &ViscosityAttractSliderθζ, &ViscosityAttractSliderθη, &ViscosityAttractSliderθθ,
	};
	vector<ofxFloatSlider*> ProbAttractSliders = {
		&ProbAttractSliderαα, &ProbAttractSliderαβ, &ProbAttractSliderαγ, &ProbAttractSliderαδ, &ProbAttractSliderαε, &ProbAttractSliderαζ, &ProbAttractSliderαη, &ProbAttractSliderαθ,
		&ProbAttractSliderβα, &ProbAttractSliderββ, &ProbAttractSliderβγ, &ProbAttractSliderβδ, &ProbAttractSliderβε, &ProbAttractSliderβζ, &ProbAttractSliderβη, &ProbAttractSliderβθ,
		&ProbAttractSliderγα, &ProbAttractSliderγβ, &ProbAttractSliderγγ, &ProbAttractSliderγδ, &ProbAttractSliderγε, &ProbAttractSliderγζ, &ProbAttractSliderγη, &ProbAttractSliderγθ,
		&ProbAttractSliderδα, &ProbAttractSliderδβ, &ProbAttractSliderδγ, &ProbAttractSliderδδ, &ProbAttractSliderδε, &ProbAttractSliderδζ, &ProbAttractSliderδη, &ProbAttractSliderδθ,
		&ProbAttractSliderεα, &ProbAttractSliderεβ, &ProbAttractSliderεγ, &ProbAttractSliderεδ, &ProbAttractSliderεε, &ProbAttractSliderεζ, &ProbAttractSliderεη, &ProbAttractSliderεθ,
		&ProbAttractSliderζα, &ProbAttractSliderζβ, &ProbAttractSliderζγ, &ProbAttractSliderζδ, &ProbAttractSliderζε, &ProbAttractSliderζζ, &ProbAttractSliderζη, &ProbAttractSliderζθ,
		&ProbAttractSliderηα, &ProbAttractSliderηβ, &ProbAttractSliderηγ, &ProbAttractSliderηδ, &ProbAttractSliderηε, &ProbAttractSliderηζ, &ProbAttractSliderηη, &ProbAttractSliderηθ,
		&ProbAttractSliderθα, &ProbAttractSliderθβ, &ProbAttractSliderθγ, &ProbAttractSliderθδ, &ProbAttractSliderθε, &ProbAttractSliderθζ, &ProbAttractSliderθη, &ProbAttractSliderθθ,
	};
	vector<ofxFloatSlider*> ViscosityRepelSliders = {
		&ViscosityRepelSliderαα, &ViscosityRepelSliderαβ, &ViscosityRepelSliderαγ, &ViscosityRepelSliderαδ, &ViscosityRepelSliderαε, &ViscosityRepelSliderαζ, &ViscosityRepelSliderαη, &ViscosityRepelSliderαθ,
		&ViscosityRepelSliderβα, &ViscosityRepelSliderββ, &ViscosityRepelSliderβγ, &ViscosityRepelSliderβδ, &ViscosityRepelSliderβε, &ViscosityRepelSliderβζ, &ViscosityRepelSliderβη, &ViscosityRepelSliderβθ,
		&ViscosityRepelSliderγα, &ViscosityRepelSliderγβ, &ViscosityRepelSliderγγ, &ViscosityRepelSliderγδ, &ViscosityRepelSliderγε, &ViscosityRepelSliderγζ, &ViscosityRepelSliderγη, &ViscosityRepelSliderγθ,
		&ViscosityRepelSliderδα, &ViscosityRepelSliderδβ, &ViscosityRepelSliderδγ, &ViscosityRepelSliderδδ, &ViscosityRepelSliderδε, &ViscosityRepelSliderδζ, &ViscosityRepelSliderδη, &ViscosityRepelSliderδθ,
		&ViscosityRepelSliderεα, &ViscosityRepelSliderεβ, &ViscosityRepelSliderεγ, &ViscosityRepelSliderεδ, &ViscosityRepelSliderεε, &ViscosityRepelSliderεζ, &ViscosityRepelSliderεη, &ViscosityRepelSliderεθ,
		&ViscosityRepelSliderζα, &ViscosityRepelSliderζβ, &ViscosityRepelSliderζγ, &ViscosityRepelSliderζδ, &ViscosityRepelSliderζε, &ViscosityRepelSliderζζ, &ViscosityRepelSliderζη, &ViscosityRepelSliderζθ,
		&ViscosityRepelSliderηα, &ViscosityRepelSliderηβ, &ViscosityRepelSliderηγ, &ViscosityRepelSliderηδ, &ViscosityRepelSliderηε, &ViscosityRepelSliderηζ, &ViscosityRepelSliderηη, &ViscosityRepelSliderηθ,
		&ViscosityRepelSliderθα, &ViscosityRepelSliderθβ, &ViscosityRepelSliderθγ, &ViscosityRepelSliderθδ, &ViscosityRepelSliderθε, &ViscosityRepelSliderθζ, &ViscosityRepelSliderθη, &ViscosityRepelSliderθθ,
	};
	vector<ofxFloatSlider*> ProbRepelSliders = {
		&ProbRepelSliderαα, &ProbRepelSliderαβ, &ProbRepelSliderαγ, &ProbRepelSliderαδ, &ProbRepelSliderαε, &ProbRepelSliderαζ, &ProbRepelSliderαη, &ProbRepelSliderαθ,
		&ProbRepelSliderβα, &ProbRepelSliderββ, &ProbRepelSliderβγ, &ProbRepelSliderβδ, &ProbRepelSliderβε, &ProbRepelSliderβζ, &ProbRepelSliderβη, &ProbRepelSliderβθ,
		&ProbRepelSliderγα, &ProbRepelSliderγβ, &ProbRepelSliderγγ, &ProbRepelSliderγδ, &ProbRepelSliderγε, &ProbRepelSliderγζ, &ProbRepelSliderγη, &ProbRepelSliderγθ,
		&ProbRepelSliderδα, &ProbRepelSliderδβ, &ProbRepelSliderδγ, &ProbRepelSliderδδ, &ProbRepelSliderδε, &ProbRepelSliderδζ, &ProbRepelSliderδη, &ProbRepelSliderδθ,
		&ProbRepelSliderεα, &ProbRepelSliderεβ, &ProbRepelSliderεγ, &ProbRepelSliderεδ, &ProbRepelSliderεε, &ProbRepelSliderεζ, &ProbRepelSliderεη, &ProbRepelSliderεθ,
		&ProbRepelSliderζα, &ProbRepelSliderζβ, &ProbRepelSliderζγ, &ProbRepelSliderζδ, &ProbRepelSliderζε, &ProbRepelSliderζζ, &ProbRepelSliderζη, &ProbRepelSliderζθ,
		&ProbRepelSliderηα, &ProbRepelSliderηβ, &ProbRepelSliderηγ, &ProbRepelSliderηδ, &ProbRepelSliderηε, &ProbRepelSliderηζ, &ProbRepelSliderηη, &ProbRepelSliderηθ,
		&ProbRepelSliderθα, &ProbRepelSliderθβ, &ProbRepelSliderθγ, &ProbRepelSliderθδ, &ProbRepelSliderθε, &ProbRepelSliderθζ, &ProbRepelSliderθη, &ProbRepelSliderθθ,
	};
};
