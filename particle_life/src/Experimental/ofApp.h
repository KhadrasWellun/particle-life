#pragma once

#include "ofMain.h"
#include "ofxGui.h"
//#include <sdlt/sdlt.h>


// use a SOA that represents a group of particles with the same color
struct colorGroup {
	std::vector<ofVec2f> pos;
	std::vector<ofVec2f> vel;
	ofColor color;
};


class ofApp final : public ofBaseApp
{
public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyPressed(int key) override;
	void restart();
	void random();
	void saveSettings();
	void loadSettings();
	void interaction(colorGroup&Group1, const colorGroup&Group2, const float G, const float radius, const float viscosity, const float probability, bool boundsToggle) const noexcept;

	static float RandomFloat(const float a, const float b) { return a + (ofRandomuf() * (b - a)); }

	void Draw(colorGroup group)
	{
		ofSetColor(group.color);
		vbo.updateVertexData(group.pos.data(), group.pos.size());
		vbo.draw(GL_POINTS, 0, group.pos.size());

	}
	
	ofxPanel gui;
	ofVbo vbo;

	colorGroup green;
	colorGroup red;
	colorGroup white;
	colorGroup yellow;

	int cntFps = 0;
	clock_t now, lastTime, delta;
	clock_t lastTime_draw, delta_draw;
	clock_t physic_begin, physic_delta;

#pragma region guigroup
	ofxGuiGroup globalGroup;
	ofxGuiGroup qtyGroup;
	ofxGuiGroup redGroup;
	ofxGuiGroup greenGroup;
	ofxGuiGroup yellowGroup;
	ofxGuiGroup whiteGroup;
#pragma endregion guigroup

	ofxButton resetButton;
	ofxButton selectButton;
	ofxButton randomChoice;
	ofxButton save;
	ofxButton load;

	ofxToggle boundsToggle;
	ofxToggle modelToggle;

#pragma region some experimental features
	ofxGuiGroup expGroup;
	ofxToggle evoToggle;
	ofxFloatSlider evoProbSlider;
	ofxFloatSlider evoAmountSlider;
	float evoChance = 1;
	float evoAmount = 1;
	ofxToggle radiusToogle;
	ofxLabel physicLabel;
	ofxToggle motionBlurToggle;

	

#pragma endregion some experimental features

#pragma region slider
	ofxIntSlider numberSliderR;
	ofxIntSlider numberSliderG;
	ofxIntSlider numberSliderW;
	ofxIntSlider numberSliderY;

	ofxFloatSlider viscoSlider;
	ofxFloatSlider gravitySlider;
	ofxFloatSlider wallRepelSlider;

	ofxFloatSlider powerSliderRR;
	ofxFloatSlider powerSliderRG;
	ofxFloatSlider powerSliderRW;
	ofxFloatSlider powerSliderRY;
	
	ofxFloatSlider powerSliderGR;
	ofxFloatSlider powerSliderGG;
	ofxFloatSlider powerSliderGW;
	ofxFloatSlider powerSliderGY;
	
	ofxFloatSlider powerSliderWR;
	ofxFloatSlider powerSliderWG;
	ofxFloatSlider powerSliderWW;
	ofxFloatSlider powerSliderWY;
	
	ofxFloatSlider powerSliderYR;
	ofxFloatSlider powerSliderYG;
	ofxFloatSlider powerSliderYW;
	ofxFloatSlider powerSliderYY;
	
	ofxFloatSlider vSliderRR;
	ofxFloatSlider vSliderRG;
	ofxFloatSlider vSliderRW;
	ofxFloatSlider vSliderRY;
	
	ofxFloatSlider vSliderGR;
	ofxFloatSlider vSliderGG;
	ofxFloatSlider vSliderGW;
	ofxFloatSlider vSliderGY;
	
	ofxFloatSlider vSliderWR;
	ofxFloatSlider vSliderWG;
	ofxFloatSlider vSliderWW;
	ofxFloatSlider vSliderWY;

	ofxFloatSlider vSliderYR;
	ofxFloatSlider vSliderYG;
	ofxFloatSlider vSliderYW;
	ofxFloatSlider vSliderYY;

	ofxFloatSlider probabilitySlider;
	float probability = 100;

	ofxFloatSlider probabilityRRSlider;
	float probabilityRR = 100;
	ofxFloatSlider probabilityRGSlider;
	float probabilityRG = 100;
	ofxFloatSlider probabilityRYSlider;
	float probabilityRY = 100;
	ofxFloatSlider probabilityRWSlider;
	float probabilityRW = 100;

	ofxFloatSlider probabilityGRSlider;
	float probabilityGR = 100;
	ofxFloatSlider probabilityGGSlider;
	float probabilityGG = 100;
	ofxFloatSlider probabilityGYSlider;
	float probabilityGY = 100;
	ofxFloatSlider probabilityGWSlider;
	float probabilityGW = 100;

	ofxFloatSlider probabilityYRSlider;
	float probabilityYR = 100;
	ofxFloatSlider probabilityYGSlider;
	float probabilityYG = 100;
	ofxFloatSlider probabilityYYSlider;
	float probabilityYY = 100;
	ofxFloatSlider probabilityYWSlider;
	float probabilityYW = 100;

	ofxFloatSlider probabilityWRSlider;
	float probabilityWR = 100;
	ofxFloatSlider probabilityWGSlider;
	float probabilityWG = 100;
	ofxFloatSlider probabilityWYSlider;
	float probabilityWY = 100;
	ofxFloatSlider probabilityWWSlider;
	float probabilityWW = 100;

	ofxFloatSlider viscositySlider;
	float viscosity = 0.5F;

	ofxFloatSlider viscosityRRSlider;
	float viscosityRR = 0.5F;
	ofxFloatSlider viscosityRGSlider;
	float viscosityRG = 0.5F;
	ofxFloatSlider viscosityRYSlider;
	float viscosityRY = 0.5F;
	ofxFloatSlider viscosityRWSlider;
	float viscosityRW = 0.5F;

	ofxFloatSlider viscosityGRSlider;
	float viscosityGR = 0.5F;
	ofxFloatSlider viscosityGGSlider;
	float viscosityGG = 0.5F;
	ofxFloatSlider viscosityGYSlider;
	float viscosityGY = 0.5F;
	ofxFloatSlider viscosityGWSlider;
	float viscosityGW = 0.5F;

	ofxFloatSlider viscosityYRSlider;
	float viscosityYR = 0.5F;
	ofxFloatSlider viscosityYGSlider;
	float viscosityYG = 0.5F;
	ofxFloatSlider viscosityYYSlider;
	float viscosityYY = 0.5F;
	ofxFloatSlider viscosityYWSlider;
	float viscosityYW = 0.5F;

	ofxFloatSlider viscosityWRSlider;
	float viscosityWR = 0.5F;
	ofxFloatSlider viscosityWGSlider;
	float viscosityWG = 0.5F;
	ofxFloatSlider viscosityWYSlider;
	float viscosityWY = 0.5F;
	ofxFloatSlider viscosityWWSlider;
	float viscosityWW = 0.5F;

	vector<ofxFloatSlider*> powersliders = {
		&powerSliderRR, &powerSliderRG, &powerSliderRY, &powerSliderRW,
		&powerSliderGR, &powerSliderGG, &powerSliderGY, &powerSliderGW,
		&powerSliderYR, &powerSliderYG, &powerSliderYY, &powerSliderYW,
		&powerSliderWR, &powerSliderWG, &powerSliderWY, &powerSliderWW,
	};

	vector<ofxFloatSlider*> vsliders = {
		&vSliderRR, &vSliderRG, &vSliderRY, &vSliderRW,
		&vSliderGR, &vSliderGG, &vSliderGY, &vSliderGW,
		&vSliderYR, &vSliderYG, &vSliderYY, &vSliderYW,
		&vSliderWR, &vSliderWG, &vSliderWY, &vSliderWW,
	};

	vector<ofxFloatSlider*> viscositysliders = {
		&viscosityRRSlider, &viscosityRGSlider, &viscosityRYSlider, &viscosityRWSlider,
		&viscosityGRSlider, &viscosityGGSlider, &viscosityGYSlider, &viscosityGWSlider,
		&viscosityYRSlider, &viscosityYGSlider, &viscosityYYSlider, &viscosityYWSlider,
		&viscosityWRSlider, &viscosityWGSlider, &viscosityWYSlider, &viscosityWWSlider,
	};

	vector<ofxFloatSlider*> probabilitysliders = {
		&probabilityRRSlider, &probabilityRGSlider, &probabilityRYSlider, &probabilityRWSlider,
		&probabilityGRSlider, &probabilityGGSlider, &probabilityGYSlider, &probabilityGWSlider,
		&probabilityYRSlider, &probabilityYGSlider, &probabilityYYSlider, &probabilityYWSlider,
		&probabilityWRSlider, &probabilityWGSlider, &probabilityWYSlider, &probabilityWWSlider,
	};

#pragma endregion slider

#pragma region slider values
	unsigned int pnumberSliderR = 1000;
	unsigned int pnumberSliderG = 1000;
	unsigned int pnumberSliderW = 1000;
	unsigned int pnumberSliderY = 1000;

	float ppowerSliderRR = 0;
	float ppowerSliderRG = 0;
	float ppowerSliderRW = 0;
	float ppowerSliderRY = 0;
	
	float ppowerSliderGR = 0;
	float ppowerSliderGG = 0;
	float ppowerSliderGW = 0;
	float ppowerSliderGY = 0;

	float ppowerSliderWR = 0;
	float ppowerSliderWG = 0;
	float ppowerSliderWW = 0;
	float ppowerSliderWY = 0;

	float ppowerSliderYR = 0;
	float ppowerSliderYG = 0;
	float ppowerSliderYW = 0;
	float ppowerSliderYY = 0;

	float pvSliderRR = 180;
	float pvSliderRG = 180;
	float pvSliderRW = 180;
	float pvSliderRY = 180;

	float pvSliderGR = 180;
	float pvSliderGG = 180;
	float pvSliderGW = 180;
	float pvSliderGY = 180;

	float pvSliderWR = 180;
	float pvSliderWG = 180;
	float pvSliderWW = 180;
	float pvSliderWY = 180;

	float pvSliderYR = 180;
	float pvSliderYG = 180;
	float pvSliderYW = 180;
	float pvSliderYY = 180;

	float pviscositySliderRR = 0;
	float pviscositySliderRG = 0;
	float pviscositySliderRW = 0;
	float pviscositySliderRY = 0;

	float pviscositySliderGR = 0;
	float pviscositySliderGG = 0;
	float pviscositySliderGW = 0;
	float pviscositySliderGY = 0;

	float pviscositySliderWR = 0;
	float pviscositySliderWG = 0;
	float pviscositySliderWW = 0;
	float pviscositySliderWY = 0;

	float pviscositySliderYR = 0;
	float pviscositySliderYG = 0;
	float pviscositySliderYW = 0;
	float pviscositySliderYY = 0;

	float pprobabilitySliderRR = 100;
	float pprobabilitySliderRG = 100;
	float pprobabilitySliderRW = 100;
	float pprobabilitySliderRY = 100;

	float pprobabilitySliderGR = 100;
	float pprobabilitySliderGG = 100;
	float pprobabilitySliderGW = 100;
	float pprobabilitySliderGY = 100;

	float pprobabilitySliderWR = 100;
	float pprobabilitySliderWG = 100;
	float pprobabilitySliderWW = 100;
	float pprobabilitySliderWY = 100;

	float pprobabilitySliderYR = 100;
	float pprobabilitySliderYG = 100;
	float pprobabilitySliderYW = 100;
	float pprobabilitySliderYY = 100;

#pragma endregion slider values

	ofxLabel labelG;
	ofxLabel labelR;
	ofxLabel labelW;
	ofxLabel labelY;
	
	ofxLabel aboutL1;
	ofxLabel aboutL2;
	ofxLabel aboutL3;
	ofxLabel fps;

	std::string fps_text;
	std::string physic_text;


	// simulation bounds
	unsigned int boundWidth = 1600;
	unsigned int boundHeight = 900;

	
	float worldGravity = 0.0F;
	float forceVariance = 0.8F;
	float radiusVariance = 0.6F;
	float probVariance = 0.1F;
	float viscoVariance = 0.1F;
	float wallRepel = 10.0F;
};
