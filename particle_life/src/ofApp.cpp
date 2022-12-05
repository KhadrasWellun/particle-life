#include "ofApp.h"
#include "ofUtils.h"

#include <iostream>
#include <vector>
#include <random>

// parameters for GUI
constexpr float xshift = 400;
constexpr float yshift = 80;
constexpr float anchor = 0;
constexpr float length = 70;
constexpr float p1x = anchor + xshift;
constexpr float p1y = anchor + yshift;
constexpr float p2x = anchor + length + xshift;
constexpr float p2y = anchor + yshift;
constexpr float p3x = anchor + length + xshift;
constexpr float p3y = anchor + length + yshift;
constexpr float p4x = anchor + xshift;
constexpr float p4y = anchor + length + yshift;
constexpr float rr = 8;

//int countThresh = 0;
std::string fps_text;
std::string physic_text;

//Simulation parameters
int cntFps = 0;
clock_t now, lastTime, delta;
clock_t physic_begin, physic_delta;

//Particle groups by color
std::vector<point> green;
std::vector<point> red;
std::vector<point> white;
std::vector<point> blue;
std::vector<point> orange;
std::vector<point> khaki;
std::vector<point> crimson;
std::vector<point> dark;


//Subdivison grid
grid subdiv;

/**
 * @brief Return a random float in range [a,b]
 *
 * @param a lower bound
 * @param b higher bound
 * @return random float
 */
inline float RandomFloat(const float a, const float b)
{
	const float diff = b - a;
	const float r = ofRandomuf() * diff;
	return a + r;
}
/**
 * @brief Return a random int in range [c,d]
 *
 * @param c lower bound
 * @param d higher bound
 * @return random int e
 */
inline int RandomInt(const int c, const int d)
{
	const int e = ofRandom(c, d);
	return e;

}

/**
 * @brief Draw all point from a given group of vector<point>*
 *
 * @param points a group of point
 */
void Draw(const std::vector<point>* points)
{
	for (auto& point : *points) point.draw();
}

/**
 * @brief Generate a number of single colored points randomly distributed on canvas
 *
 * @param num number of point to generate
 * @param r red
 * @param g green
 * @param b blue
 * @param o orange
 * @param k khaki
 * @param c crimson
 * @param d dark
 * @return a group of random point
 */
std::vector<point> CreatePoints(const int num, const int r, const int g, const int b)
{
	std::vector<point> points;
	points.reserve(num);
	for (auto i = 0; i < num; i++)
	{
		int x = static_cast<int>(ofRandomWidth());
		int y = static_cast<int>(ofRandomHeight());
		points.emplace_back(x, y, r, g, b);
	}
	return points;
}

/**
 * @brief Interaction between 2 particle groups
 * @param Group1 the group that will be modified by the interaction
 * @param Group2 the interacting group (its value won't be modified)
 * @param G gravity coefficient
 * @param radius radius of interaction
 */
void ofApp::interaction(std::vector<point>* Group1, const std::vector<point>* Group2, const float G, const float radius, const float viscosity, const float probability)
{
	const float g = G / -100;	//Gravity coefficient
	const auto group1size = Group1->size();
	const auto group2size = Group2->size();
	const bool radius_toggle = radiusToogle;

	boundHeight = ofGetHeight();
	boundWidth = ofGetWidth();

#pragma omp parallel
	{
		std::random_device rd;
#pragma omp for
		for (auto i = 0; i < group1size; i++)
		{
			if (rd() % 100 < probability) {
				auto& p1 = (*Group1)[i];
				float fx = 0;
				float fy = 0;

				//This inner loop is, of course, where most of the CPU time is spent. Everything else is cheap
				for (auto j = 0; j < group2size; j++)
				{
					const auto& p2 = (*Group2)[j];

					// you don't need sqrt to compare distance. (you need it to compute the actual distance however)
					const auto dx = p1.x - p2.x;
					const auto dy = p1.y - p2.y;
					const auto r = dx * dx + dy * dy;

					//Calculate the force in given bounds. 
					if ((r < radius * radius || radius_toggle) && r != 0.0F)
					{
						fx += (dx / std::sqrt(dx * dx + dy * dy));
						fy += (dy / std::sqrt(dx * dx + dy * dy));
					}
				}
				
					//Calculate new velocity
					p1.vx = (p1.vx + (fx * g)) * (1 - viscosity);
					p1.vy = (p1.vy + (fy * g)) * (1 - viscosity) + worldGravity;
				
				// Wall Repel
				if (wallRepel > 0.0F)
				{
					if (p1.x < wallRepel) p1.vx += (wallRepel - p1.x) * 0.1;
					if (p1.y < wallRepel) p1.vy += (wallRepel - p1.y) * 0.1;
					if (p1.x > boundWidth - wallRepel) p1.vx += (boundWidth - wallRepel - p1.x) * 0.1;
					if (p1.y > boundHeight - wallRepel) p1.vy += (boundHeight - wallRepel - p1.y) * 0.1;
				}

				//Update position based on velocity
				p1.x += p1.vx;
				p1.y += p1.vy;

				//Checking for canvas bounds
				if (boundsToggle)
				{
					if (p1.x < 0)
					{
						p1.vx *= -1;
						p1.x = 0;
					}
					if (p1.x > boundWidth)
					{
						p1.vx *= -1;
						p1.x = boundWidth;
					}
					if (p1.y < 0)
					{
						p1.vy *= -1;
						p1.y = 0;
					}
					if (p1.y > boundHeight)
					{
						p1.vy *= -1;
						p1.y = boundHeight;
					}
				}
			}
		}
	}
}

/* omp end parallel */

/**
 * @brief Generate new sets of points
 */
void ofApp::restart()
{
	if (numberSliderG > 0) { green = CreatePoints(numberSliderG, 10, 250, 10); }
	if (numberSliderR > 0) { red = CreatePoints(numberSliderR, 250, 10, 10); }
	if (numberSliderW > 0) { white = CreatePoints(numberSliderW, 250, 250, 250); }
	if (numberSliderB > 0) { blue = CreatePoints(numberSliderB, 10, 10, 250); }
	if (numberSliderO > 0) { orange = CreatePoints(numberSliderO, 250, 125, 10); }
	if (numberSliderK > 0) { khaki = CreatePoints(numberSliderK, 240, 230, 140); }
	if (numberSliderC > 0) { crimson = CreatePoints(numberSliderC, 220, 20, 60); }
	if (numberSliderD > 0) { dark = CreatePoints(numberSliderC, 10, 10, 10); }
}


/**
 * @brief Generate initial simulation parameters
 */
void ofApp::monads() {
	numberSliderG = RandomInt(200, 2000);
	numberSliderR = RandomInt(200, 2000);
	numberSliderW = RandomInt(200, 2000);
	numberSliderB = RandomInt(200, 2000);
	numberSliderO = RandomInt(200, 2000);
	numberSliderK = RandomInt(200, 2000);
	numberSliderC = RandomInt(200, 2000);
	numberSliderD = RandomInt(200, 2000);
}
void ofApp::random()
{
	evoProbSlider = RandomFloat(0.1, 1.5);
	evoAmountSlider = RandomFloat(0.1, 3);
	viscoSlider = RandomFloat(0.4, 0.9);
	viscoSliderR = RandomFloat(0.4, 0.9);
	viscoSliderG = RandomFloat(0.4, 0.9);
	viscoSliderW = RandomFloat(0.4, 0.9);
	viscoSliderB = RandomFloat(0.4, 0.9);
	viscoSliderO = RandomFloat(0.4, 0.9);
	viscoSliderK = RandomFloat(0.4, 0.9);
	viscoSliderC = RandomFloat(0.4, 0.9);
	viscoSliderD = RandomFloat(0.4, 0.9);

	probabilitySlider = RandomFloat(30, 100);

	probabilitySliderRR = RandomFloat(30, 100);
	probabilitySliderRG = RandomFloat(30, 100);
	probabilitySliderRB = RandomFloat(30, 100);
	probabilitySliderRW = RandomFloat(30, 100);
	probabilitySliderRO = RandomFloat(30, 100);
	probabilitySliderRK = RandomFloat(30, 100);
	probabilitySliderRC = RandomFloat(30, 100);
	probabilitySliderRD = RandomFloat(30, 100);

	probabilitySliderGR = RandomFloat(30, 100);
	probabilitySliderGG = RandomFloat(30, 100);
	probabilitySliderGB = RandomFloat(30, 100);
	probabilitySliderGW = RandomFloat(30, 100);
	probabilitySliderGO = RandomFloat(30, 100);
	probabilitySliderGK = RandomFloat(30, 100);
	probabilitySliderGC = RandomFloat(30, 100);
	probabilitySliderGD = RandomFloat(30, 100);

	probabilitySliderBR = RandomFloat(30, 100);
	probabilitySliderBG = RandomFloat(30, 100);
	probabilitySliderBB = RandomFloat(30, 100);
	probabilitySliderBW = RandomFloat(30, 100);
	probabilitySliderBO = RandomFloat(30, 100);
	probabilitySliderBK = RandomFloat(30, 100);
	probabilitySliderBC = RandomFloat(30, 100);
	probabilitySliderBD = RandomFloat(30, 100);

	probabilitySliderWR = RandomFloat(30, 100);
	probabilitySliderWG = RandomFloat(30, 100);
	probabilitySliderWB = RandomFloat(30, 100);
	probabilitySliderWW = RandomFloat(30, 100);
	probabilitySliderWO = RandomFloat(30, 100);
	probabilitySliderWK = RandomFloat(30, 100);
	probabilitySliderWC = RandomFloat(30, 100);
	probabilitySliderWD = RandomFloat(30, 100);

	probabilitySliderOR = RandomFloat(30, 100);
	probabilitySliderOG = RandomFloat(30, 100);
	probabilitySliderOB = RandomFloat(30, 100);
	probabilitySliderOW = RandomFloat(30, 100);
	probabilitySliderOO = RandomFloat(30, 100);
	probabilitySliderOK = RandomFloat(30, 100);
	probabilitySliderOC = RandomFloat(30, 100);
	probabilitySliderOD = RandomFloat(30, 100);

	probabilitySliderKR = RandomFloat(30, 100);
	probabilitySliderKG = RandomFloat(30, 100);
	probabilitySliderKB = RandomFloat(30, 100);
	probabilitySliderKW = RandomFloat(30, 100);
	probabilitySliderKO = RandomFloat(30, 100);
	probabilitySliderKK = RandomFloat(30, 100);
	probabilitySliderKC = RandomFloat(30, 100);
	probabilitySliderKD = RandomFloat(30, 100);

	probabilitySliderCR = RandomFloat(30, 100);
	probabilitySliderCG = RandomFloat(30, 100);
	probabilitySliderCB = RandomFloat(30, 100);
	probabilitySliderCW = RandomFloat(30, 100);
	probabilitySliderCO = RandomFloat(30, 100);
	probabilitySliderCK = RandomFloat(30, 100);
	probabilitySliderCC = RandomFloat(30, 100);
	probabilitySliderCD = RandomFloat(30, 100);

	probabilitySliderDR = RandomFloat(30, 100);
	probabilitySliderDG = RandomFloat(30, 100);
	probabilitySliderDB = RandomFloat(30, 100);
	probabilitySliderDW = RandomFloat(30, 100);
	probabilitySliderDO = RandomFloat(30, 100);
	probabilitySliderDK = RandomFloat(30, 100);
	probabilitySliderDC = RandomFloat(30, 100);
	probabilitySliderDD = RandomFloat(30, 100);

	// GREEN
	//numberSliderG = RandomFloat(0, 3000);
	powerSliderGG = RandomFloat(-100, 100) * forceVariance;
	powerSliderGR = RandomFloat(-100, 100) * forceVariance;
	powerSliderGW = RandomFloat(-100, 100) * forceVariance;
	powerSliderGB = RandomFloat(-100, 100) * forceVariance;
	powerSliderGO = RandomFloat(-100, 100) * forceVariance;
	powerSliderGK = RandomFloat(-100, 100) * forceVariance;
	powerSliderGC = RandomFloat(-100, 100) * forceVariance;
	powerSliderGD = RandomFloat(-100, 100) * forceVariance;

	vSliderGG = RandomFloat(0, 200) * radiusVariance;
	vSliderGR = RandomFloat(0, 200) * radiusVariance;
	vSliderGW = RandomFloat(0, 200) * radiusVariance;
	vSliderGB = RandomFloat(0, 200) * radiusVariance;
	vSliderGO = RandomFloat(0, 200) * radiusVariance;
	vSliderGK = RandomFloat(0, 200) * radiusVariance;
	vSliderGC = RandomFloat(0, 200) * radiusVariance;
	vSliderGD = RandomFloat(0, 200) * radiusVariance;

	// RED
	//numberSliderR = RandomFloat(0, 3000);
	powerSliderRR = RandomFloat(-100, 100) * forceVariance;
	powerSliderRG = RandomFloat(-100, 100) * forceVariance;
	powerSliderRW = RandomFloat(-100, 100) * forceVariance;
	powerSliderRB = RandomFloat(-100, 100) * forceVariance;
	powerSliderRO = RandomFloat(-100, 100) * forceVariance;
	powerSliderRK = RandomFloat(-100, 100) * forceVariance;
	powerSliderRC = RandomFloat(-100, 100) * forceVariance;
	powerSliderRD = RandomFloat(-100, 100) * forceVariance;

	vSliderRG = RandomFloat(0, 200) * radiusVariance;
	vSliderRR = RandomFloat(0, 200) * radiusVariance;
	vSliderRW = RandomFloat(0, 200) * radiusVariance;
	vSliderRB = RandomFloat(0, 200) * radiusVariance;
	vSliderRO = RandomFloat(0, 200) * radiusVariance;
	vSliderRK = RandomFloat(0, 200) * radiusVariance;
	vSliderRC = RandomFloat(0, 200) * radiusVariance;
	vSliderRD = RandomFloat(0, 200) * radiusVariance;

	// WHITE
	// numberSliderW = RandomFloat(0, 3000);
	powerSliderWW = RandomFloat(-100, 100) * forceVariance;
	powerSliderWR = RandomFloat(-100, 100) * forceVariance;
	powerSliderWG = RandomFloat(-100, 100) * forceVariance;
	powerSliderWB = RandomFloat(-100, 100) * forceVariance;
	powerSliderWO = RandomFloat(-100, 100) * forceVariance;
	powerSliderWK = RandomFloat(-100, 100) * forceVariance;
	powerSliderWC = RandomFloat(-100, 100) * forceVariance;
	powerSliderWD = RandomFloat(-100, 100) * forceVariance;

	vSliderWG = RandomFloat(0, 200) * radiusVariance;
	vSliderWR = RandomFloat(0, 200) * radiusVariance;
	vSliderWW = RandomFloat(0, 200) * radiusVariance;
	vSliderWB = RandomFloat(0, 200) * radiusVariance;
	vSliderWO = RandomFloat(0, 200) * radiusVariance;
	vSliderWK = RandomFloat(0, 200) * radiusVariance;
	vSliderWC = RandomFloat(0, 200) * radiusVariance;
	vSliderWD = RandomFloat(0, 200) * radiusVariance;

	// BLUE
	//numberSliderB = RandomFloat(0, 3000);
	powerSliderBB = RandomFloat(-100, 100) * forceVariance;
	powerSliderBW = RandomFloat(-100, 100) * forceVariance;
	powerSliderBR = RandomFloat(-100, 100) * forceVariance;
	powerSliderBG = RandomFloat(-100, 100) * forceVariance;
	powerSliderBO = RandomFloat(-100, 100) * forceVariance;
	powerSliderBK = RandomFloat(-100, 100) * forceVariance;
	powerSliderBC = RandomFloat(-100, 100) * forceVariance;
	powerSliderBD = RandomFloat(-100, 100) * forceVariance;

	vSliderBG = RandomFloat(0, 200) * radiusVariance;
	vSliderBR = RandomFloat(0, 200) * radiusVariance;
	vSliderBW = RandomFloat(0, 200) * radiusVariance;
	vSliderBB = RandomFloat(0, 200) * radiusVariance;
	vSliderBO = RandomFloat(0, 200) * radiusVariance;
	vSliderBK = RandomFloat(0, 200) * radiusVariance;
	vSliderBC = RandomFloat(0, 200) * radiusVariance;
	vSliderBD = RandomFloat(0, 200) * radiusVariance;

	// ORANGE
	//numberSliderO = RandomFloat(0, 3000);
	powerSliderOB = RandomFloat(-100, 100) * forceVariance;
	powerSliderOW = RandomFloat(-100, 100) * forceVariance;
	powerSliderOR = RandomFloat(-100, 100) * forceVariance;
	powerSliderOG = RandomFloat(-100, 100) * forceVariance;
	powerSliderOO = RandomFloat(-100, 100) * forceVariance;
	powerSliderOK = RandomFloat(-100, 100) * forceVariance;
	powerSliderOC = RandomFloat(-100, 100) * forceVariance;
	powerSliderOD = RandomFloat(-100, 100) * forceVariance;

	vSliderOG = RandomFloat(0, 200) * radiusVariance;
	vSliderOR = RandomFloat(0, 200) * radiusVariance;
	vSliderOW = RandomFloat(0, 200) * radiusVariance;
	vSliderOB = RandomFloat(0, 200) * radiusVariance;
	vSliderOO = RandomFloat(0, 200) * radiusVariance;
	vSliderOK = RandomFloat(0, 200) * radiusVariance;
	vSliderOC = RandomFloat(0, 200) * radiusVariance;
	vSliderOD = RandomFloat(0, 200) * radiusVariance;

	// KHAKI
	//numberSliderK = RandomFloat(0, 3000);
	powerSliderKB = RandomFloat(-100, 100) * forceVariance;
	powerSliderKW = RandomFloat(-100, 100) * forceVariance;
	powerSliderKR = RandomFloat(-100, 100) * forceVariance;
	powerSliderKG = RandomFloat(-100, 100) * forceVariance;
	powerSliderKO = RandomFloat(-100, 100) * forceVariance;
	powerSliderKK = RandomFloat(-100, 100) * forceVariance;
	powerSliderKC = RandomFloat(-100, 100) * forceVariance;
	powerSliderKD = RandomFloat(-100, 100) * forceVariance;

	vSliderKG = RandomFloat(0, 200) * radiusVariance;
	vSliderKR = RandomFloat(0, 200) * radiusVariance;
	vSliderKW = RandomFloat(0, 200) * radiusVariance;
	vSliderKB = RandomFloat(0, 200) * radiusVariance;
	vSliderKO = RandomFloat(0, 200) * radiusVariance;
	vSliderKK = RandomFloat(0, 200) * radiusVariance;
	vSliderKC = RandomFloat(0, 200) * radiusVariance;
	vSliderKD = RandomFloat(0, 200) * radiusVariance;

	// CRIMSON
	//numberSliderC = RandomFloat(0, 3000);
	powerSliderCB = RandomFloat(-100, 100) * forceVariance;
	powerSliderCW = RandomFloat(-100, 100) * forceVariance;
	powerSliderCR = RandomFloat(-100, 100) * forceVariance;
	powerSliderCG = RandomFloat(-100, 100) * forceVariance;
	powerSliderCO = RandomFloat(-100, 100) * forceVariance;
	powerSliderCK = RandomFloat(-100, 100) * forceVariance;
	powerSliderCC = RandomFloat(-100, 100) * forceVariance;
	powerSliderCD = RandomFloat(-100, 100) * forceVariance;

	vSliderCG = RandomFloat(0, 200) * radiusVariance;
	vSliderCR = RandomFloat(0, 200) * radiusVariance;
	vSliderCW = RandomFloat(0, 200) * radiusVariance;
	vSliderCB = RandomFloat(0, 200) * radiusVariance;
	vSliderCO = RandomFloat(0, 200) * radiusVariance;
	vSliderCK = RandomFloat(0, 200) * radiusVariance;
	vSliderCC = RandomFloat(0, 200) * radiusVariance;
	vSliderCD = RandomFloat(0, 200) * radiusVariance;

	// DARK
	//numberSliderD = RandomFloat(0, 3000);
	powerSliderDB = RandomFloat(-100, 100) * forceVariance;
	powerSliderDW = RandomFloat(-100, 100) * forceVariance;
	powerSliderDR = RandomFloat(-100, 100) * forceVariance;
	powerSliderDG = RandomFloat(-100, 100) * forceVariance;
	powerSliderDO = RandomFloat(-100, 100) * forceVariance;
	powerSliderDK = RandomFloat(-100, 100) * forceVariance;
	powerSliderDC = RandomFloat(-100, 100) * forceVariance;
	powerSliderDD = RandomFloat(-100, 100) * forceVariance;

	vSliderDG = RandomFloat(0, 200) * radiusVariance;
	vSliderDR = RandomFloat(0, 200) * radiusVariance;
	vSliderDW = RandomFloat(0, 200) * radiusVariance;
	vSliderDB = RandomFloat(0, 200) * radiusVariance;
	vSliderDO = RandomFloat(0, 200) * radiusVariance;
	vSliderDK = RandomFloat(0, 200) * radiusVariance;
	vSliderDC = RandomFloat(0, 200) * radiusVariance;
	vSliderDD = RandomFloat(0, 200) * radiusVariance;
}

/// this is a cheap and quick way to save and load parameters (openFramework have betters ways but requires some additional library setups) 
// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::saveSettings()
{
	const std::vector<float> settings = {
		powerSliderGG, powerSliderGR, powerSliderGW, powerSliderGB, powerSliderGO, powerSliderGK, powerSliderGC, powerSliderGD,
		vSliderGG, vSliderGR, vSliderGW, vSliderGB, vSliderGO, vSliderGK, vSliderGC, vSliderGD,
		powerSliderRG, powerSliderRR, powerSliderRW, powerSliderRB, powerSliderRO, powerSliderRK, powerSliderRC, powerSliderRD,
		vSliderRG, vSliderRR, vSliderRW, vSliderRB, vSliderRO, vSliderRK, vSliderRC, vSliderRD,
		powerSliderWG, powerSliderWR, powerSliderWW, powerSliderWB, powerSliderWO, powerSliderWK, powerSliderWC, powerSliderWD,
		vSliderWG, vSliderWR, vSliderWW, vSliderWB, vSliderWO, vSliderWK, vSliderWC, vSliderWD,
		powerSliderBG, powerSliderBR, powerSliderBW, powerSliderBB, powerSliderBO, powerSliderBK, powerSliderBC, powerSliderBD,
		vSliderBG, vSliderBR, vSliderBW, vSliderBB, vSliderBO, vSliderBK, vSliderBC, vSliderBD,
		powerSliderOG, powerSliderOR, powerSliderOW, powerSliderOB, powerSliderOO, powerSliderOK, powerSliderOC, powerSliderOD,
		vSliderOG, vSliderOR, vSliderOW, vSliderOB, vSliderOO, vSliderOK, vSliderOC, vSliderOD,
		powerSliderKG, powerSliderKR, powerSliderKW, powerSliderKB, powerSliderKO, powerSliderKK, powerSliderKC, powerSliderKD,
		vSliderKG, vSliderKR, vSliderKW, vSliderKB, vSliderKO, vSliderKK, vSliderKC, vSliderKD,
		powerSliderCG, powerSliderCR, powerSliderCW, powerSliderCB, powerSliderCO, powerSliderCK, powerSliderCC, powerSliderCD,
		vSliderCG, vSliderCR, vSliderCW, vSliderCB, vSliderCO, vSliderCK, vSliderCC, vSliderCD,
		powerSliderDG, powerSliderDR, powerSliderDW, powerSliderDB, powerSliderDO, powerSliderDK, powerSliderDC, powerSliderDD,
		vSliderDG, vSliderDR, vSliderDW, vSliderDB, vSliderDO, vSliderDK, vSliderDC, vSliderDD,
		static_cast<float>(numberSliderG),
		static_cast<float>(numberSliderR),
		static_cast<float>(numberSliderW),
		static_cast<float>(numberSliderB),
		static_cast<float>(numberSliderO),
		static_cast<float>(numberSliderK),
		static_cast<float>(numberSliderC),
		static_cast<float>(numberSliderD),
		viscoSlider,
		viscoSliderR,
		viscoSliderG,
		viscoSliderB,
		viscoSliderW,
		viscoSliderO,
		viscoSliderK,
		viscoSliderC,
		viscoSliderD,
		evoProbSlider,
		evoAmountSlider,
		probabilitySlider,
		probabilitySliderRR,
		probabilitySliderRG,
		probabilitySliderRB,
		probabilitySliderRW,
		probabilitySliderRO,
		probabilitySliderRK,
		probabilitySliderRC,
		probabilitySliderRD,
		probabilitySliderGR,
		probabilitySliderGG,
		probabilitySliderGB,
		probabilitySliderGW,
		probabilitySliderGO,
		probabilitySliderGK,
		probabilitySliderGC,
		probabilitySliderGD,
		probabilitySliderBR,
		probabilitySliderBG,
		probabilitySliderBB,
		probabilitySliderBW,
		probabilitySliderBO,
		probabilitySliderBK,
		probabilitySliderBC,
		probabilitySliderBD,
		probabilitySliderWR,
		probabilitySliderWG,
		probabilitySliderWB,
		probabilitySliderWW,
		probabilitySliderWO,
		probabilitySliderWK,
		probabilitySliderWC,
		probabilitySliderWD,
		probabilitySliderOR,
		probabilitySliderOG,
		probabilitySliderOB,
		probabilitySliderOW,
		probabilitySliderOO,
		probabilitySliderOK,
		probabilitySliderOC,
		probabilitySliderOD,
		probabilitySliderKR,
		probabilitySliderKG,
		probabilitySliderKB,
		probabilitySliderKW,
		probabilitySliderKO,
		probabilitySliderKK,
		probabilitySliderKC,
		probabilitySliderKD,
		probabilitySliderCR,
		probabilitySliderCG,
		probabilitySliderCB,
		probabilitySliderCW,
		probabilitySliderCO,
		probabilitySliderCK,
		probabilitySliderCC,
		probabilitySliderCD,
		probabilitySliderDR,
		probabilitySliderDG,
		probabilitySliderDB,
		probabilitySliderDW,
		probabilitySliderDO,
		probabilitySliderDK,
		probabilitySliderDC,
		probabilitySliderDD,
	};

	std::string save_path;
	ofFileDialogResult result = ofSystemSaveDialog("model.txt", "Save");
	if (result.bSuccess)
	{
		save_path = result.getPath();
	}
	else
	{
		ofSystemAlertDialog("Could not Save Model!");
	}
	ofstream myfile(save_path);
	if (myfile.is_open())
	{
		for (int i = 0; i < settings.size(); i++)
		{
			myfile << settings[i] << " ";
		}
		myfile.close();
		std::cout << "file saved successfully";
	}
	else
	{
		std::cout << "unable to save file!";
	}
}

// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::loadSettings()
{
	std::string load_path;
	std::string text;
	ofFileDialogResult result = ofSystemLoadDialog("Load file", false, load_path);
	if (result.bSuccess)
	{
		load_path = result.getPath();
		std::ifstream t(load_path);
		std::stringstream loaded;
		loaded << t.rdbuf();
		text = loaded.str();
	}
	else
	{
		ofSystemAlertDialog("Could not Load the File!");
	}

	// split text by space and convert them to floats
	string word;
	std::vector<float> p;
	for (auto x : text)
	{
		if (x == ' ')
		{
			p.push_back(std::stof(word));
			word = "";
		}
		else
		{
			word += x;
		}
	}

	if (p.size() < 180)
	{
		// better checks needed
		ofSystemAlertDialog("Could not read the file!");
	}
	else
	{
		powerSliderGG = p[0];
		powerSliderGR = p[1];
		powerSliderGW = p[2];
		powerSliderGB = p[3];
		powerSliderGO = p[4];
		powerSliderGK = p[5];
		powerSliderGC = p[6];
		powerSliderGD = p[7];
		vSliderGG = p[8];
		vSliderGR = p[9];
		vSliderGW = p[10];
		vSliderGB = p[11];
		vSliderGO = p[12];
		vSliderGK = p[13];
		vSliderGC = p[14];
		vSliderGD = p[15];
		powerSliderRG = p[16];
		powerSliderRR = p[17];
		powerSliderRW = p[18];
		powerSliderRB = p[19];
		powerSliderRO = p[20];
		powerSliderRK = p[21];
		powerSliderRC = p[22];
		powerSliderRD = p[23];
		vSliderRG = p[24];
		vSliderRR = p[25];
		vSliderRW = p[26];
		vSliderRB = p[27];
		vSliderRO = p[28];
		vSliderRK = p[29];
		vSliderRC = p[30];
		vSliderRD = p[31];
		powerSliderWG = p[32];
		powerSliderWR = p[33];
		powerSliderWW = p[34];
		powerSliderWB = p[35];
		powerSliderWO = p[36];
		powerSliderWK = p[37];
		powerSliderWC = p[38];
		powerSliderWD = p[39];
		vSliderWG = p[40];
		vSliderWR = p[41];
		vSliderWW = p[42];
		vSliderWB = p[43];
		vSliderWO = p[44];
		vSliderWK = p[45];
		vSliderWC = p[46];
		vSliderWD = p[47];
		powerSliderBG = p[48];
		powerSliderBR = p[49];
		powerSliderBW = p[50];
		powerSliderBB = p[51];
		powerSliderBO = p[52];
		powerSliderBK = p[53];
		powerSliderBC = p[54];
		powerSliderBD = p[55];
		vSliderBG = p[56];
		vSliderBR = p[57];
		vSliderBW = p[58];
		vSliderBB = p[59];
		vSliderBO = p[60];
		vSliderBK = p[61];
		vSliderBC = p[62];
		vSliderBD = p[63];
		powerSliderOG = p[64];
		powerSliderOR = p[65];
		powerSliderOW = p[66];
		powerSliderOB = p[67];
		powerSliderOO = p[68];
		powerSliderOK = p[69];
		powerSliderOC = p[70];
		powerSliderOD = p[71];
		vSliderOG = p[72];
		vSliderOR = p[73];
		vSliderOW = p[74];
		vSliderOB = p[75];
		vSliderOO = p[76];
		vSliderOK = p[77];
		vSliderOC = p[78];
		vSliderOD = p[79];
		powerSliderDG = p[80];
		powerSliderDR = p[81];
		powerSliderDW = p[82];
		powerSliderDB = p[83];
		powerSliderDO = p[84];
		powerSliderDK = p[85];
		powerSliderDC = p[86];
		powerSliderDD = p[87];
		vSliderDG = p[88];
		vSliderDR = p[89];
		vSliderDW = p[90];
		vSliderDB = p[91];
		vSliderDO = p[92];
		vSliderDK = p[93];
		vSliderDC = p[94];
		vSliderDD = p[95];
		numberSliderG = static_cast<int>(p[96]);
		numberSliderR = static_cast<int>(p[97]);
		numberSliderW = static_cast<int>(p[98]);
		numberSliderB = static_cast<int>(p[99]);
		numberSliderO = static_cast<int>(p[100]);
		numberSliderK = static_cast<int>(p[101]);
		numberSliderC = static_cast<int>(p[102]);
		numberSliderD = static_cast<int>(p[103]);
		viscoSlider = p[104];
		viscoSliderR = p[105];
		viscoSliderG = p[106];
		viscoSliderB = p[107];
		viscoSliderW = p[108];
		viscoSliderO = p[109];
		viscoSliderK = p[110];
		viscoSliderC = p[111];
		viscoSliderD = p[112];
		evoProbSlider = p[113];
		evoAmountSlider = p[114];
		probabilitySlider = p[115];
		probabilitySliderRR = p[116];
		probabilitySliderRG = p[117];
		probabilitySliderRB = p[118];
		probabilitySliderRW = p[119];
		probabilitySliderRO = p[120];
		probabilitySliderRK = p[121];
		probabilitySliderRC = p[122];
		probabilitySliderRD = p[123];
		probabilitySliderGR = p[124];
		probabilitySliderGG = p[125];
		probabilitySliderGB = p[126];
		probabilitySliderGW = p[127];
		probabilitySliderGO = p[128];
		probabilitySliderGK = p[129];
		probabilitySliderGC = p[130];
		probabilitySliderGD = p[131];
		probabilitySliderBR = p[132];
		probabilitySliderBG = p[133];
		probabilitySliderBB = p[134];
		probabilitySliderBW = p[135];
		probabilitySliderBO = p[136];
		probabilitySliderBK = p[137];
		probabilitySliderBC = p[138];
		probabilitySliderBD = p[139];
		probabilitySliderWR = p[140];
		probabilitySliderWG = p[141];
		probabilitySliderWB = p[142];
		probabilitySliderWW = p[143];
		probabilitySliderWO = p[144];
		probabilitySliderWK = p[145];
		probabilitySliderWC = p[146];
		probabilitySliderWD = p[147];
		probabilitySliderOR = p[148];
		probabilitySliderOG = p[149];
		probabilitySliderOB = p[150];
		probabilitySliderOW = p[151];
		probabilitySliderOO = p[152];
		probabilitySliderOK = p[153];
		probabilitySliderOC = p[154];
		probabilitySliderOD = p[155];
		probabilitySliderKR = p[156];
		probabilitySliderKG = p[157];
		probabilitySliderKB = p[158];
		probabilitySliderKW = p[159];
		probabilitySliderKO = p[160];
		probabilitySliderKK = p[161];
		probabilitySliderKC = p[162];
		probabilitySliderKD = p[163];
		probabilitySliderCR = p[164];
		probabilitySliderCG = p[165];
		probabilitySliderCB = p[166];
		probabilitySliderCW = p[167];
		probabilitySliderCO = p[168];
		probabilitySliderCK = p[169];
		probabilitySliderCC = p[170];
		probabilitySliderCD = p[171];
		probabilitySliderDR = p[172];
		probabilitySliderDG = p[173];
		probabilitySliderDB = p[174];
		probabilitySliderDW = p[175];
		probabilitySliderDO = p[176];
		probabilitySliderDK = p[177];
		probabilitySliderDC = p[178];
		probabilitySliderDD = p[179];
	}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - Eight Colors version 1.3");
	ofSetVerticalSync(false);

	// Interface
	gui.setup("Settings");
	gui.loadFont("Arial", 12);
	gui.setWidthElements(300.0f);

	gui.add(fps.setup("FPS", "0"));

	gui.add(resetButton.setup("Restart"));
	gui.add(randomChoice.setup("Randomize (space bar)"));
	gui.add(randomCount.setup("Randomize particle count (q)"));
	gui.add(save.setup("Save Model"));
	gui.add(load.setup("Load Model"));
	gui.add(modelToggle.setup("Show Model", false));
	

	// Global Group
	globalGroup.setup("Global");
	globalGroup.add(evoToggle.setup("Evolve parameters", true));
	globalGroup.add(evoProbSlider.setup("evo chance%", evoChance, 0, 100));
	globalGroup.add(evoAmountSlider.setup("evo amount%%", evoAmount, 0, 100));
	globalGroup.add(probabilitySlider.setup("interaction prob%", probability, 1, 100));
	globalGroup.add(motionBlurToggle.setup("Motion Blur", false));
	globalGroup.add(physicLabel.setup("physic (ms)", "0"));
	globalGroup.add(radiusToogle.setup("infinite radius", false));
	gui.add(&globalGroup);
	globalGroup.minimize();

	// Quantity Group
	qtyGroup.setup("Quantity (require restart/randomize)");
	qtyGroup.add(numberSliderG.setup("Green", pnumberSliderG, 0, 10000));
	qtyGroup.add(numberSliderR.setup("Red", pnumberSliderR, 0, 10000));
	qtyGroup.add(numberSliderW.setup("White", pnumberSliderW, 0, 10000));
	qtyGroup.add(numberSliderB.setup("Blue", pnumberSliderB, 0, 10000));
	qtyGroup.add(numberSliderO.setup("Orange", pnumberSliderO, 0, 10000));
	qtyGroup.add(numberSliderK.setup("Khaki", pnumberSliderK, 0, 10000));
	qtyGroup.add(numberSliderC.setup("Crimson", pnumberSliderC, 0, 10000));
	qtyGroup.add(numberSliderD.setup("Dark", pnumberSliderD, 0, 10000));
	gui.add(&qtyGroup);
	qtyGroup.minimize();

	// Viscosity Group
	viscosityGroup.setup("Viscosity Parameters");
	viscosityGroup.add(viscoSlider.setup("Viscosity/Friction", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderR.setup("Viscosity Red", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderG.setup("Viscosity Green", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderB.setup("Viscosity Blue", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderW.setup("Viscosity White", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderO.setup("Viscosity Orange", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderK.setup("Viscosity Khaki", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderC.setup("Viscosity Crimson", viscosity, 0, 1));
	viscosityGroup.add(viscoSliderD.setup("Viscosity Dark", viscosity, 0, 1));
	gui.add(&viscosityGroup);
	viscosityGroup.minimize();

	// ProbabilityR Group
	probabilityRGroup.setup("Interaction Probability % Red");
	probabilityRGroup.add(probabilitySliderRR.setup("Interaction Probability RR", probabilityRR, 0, 100));
	probabilityRGroup.add(probabilitySliderRG.setup("Interaction Probability RG", probabilityRG, 0, 100));
	probabilityRGroup.add(probabilitySliderRB.setup("Interaction Probability RB", probabilityRB, 0, 100));
	probabilityRGroup.add(probabilitySliderRW.setup("Interaction Probability RW", probabilityRW, 0, 100));
	probabilityRGroup.add(probabilitySliderRO.setup("Interaction Probability RO", probabilityRO, 0, 100));
	probabilityRGroup.add(probabilitySliderRK.setup("Interaction Probability RK", probabilityRK, 0, 100));
	probabilityRGroup.add(probabilitySliderRC.setup("Interaction Probability RC", probabilityRC, 0, 100));
	probabilityRGroup.add(probabilitySliderRD.setup("Interaction Probability RD", probabilityRD, 0, 100));
	gui.add(&probabilityRGroup);
	probabilityRGroup.minimize();

	// ProbabilityG Group
	probabilityGGroup.setup("Interaction Probability % Green");
	probabilityGGroup.add(probabilitySliderGR.setup("Interaction Probability GR", probabilityGR, 0, 100));
	probabilityGGroup.add(probabilitySliderGG.setup("Interaction Probability GG", probabilityGG, 0, 100));
	probabilityGGroup.add(probabilitySliderGB.setup("Interaction Probability GB", probabilityGB, 0, 100));
	probabilityGGroup.add(probabilitySliderGW.setup("Interaction Probability GW", probabilityGW, 0, 100));
	probabilityGGroup.add(probabilitySliderGO.setup("Interaction Probability GO", probabilityGO, 0, 100));
	probabilityGGroup.add(probabilitySliderGK.setup("Interaction Probability GK", probabilityGK, 0, 100));
	probabilityGGroup.add(probabilitySliderGC.setup("Interaction Probability GC", probabilityGC, 0, 100));
	probabilityGGroup.add(probabilitySliderGD.setup("Interaction Probability GD", probabilityGD, 0, 100));
	gui.add(&probabilityGGroup);
	probabilityGGroup.minimize();

	// ProbabilityB Group
	probabilityBGroup.setup("Interaction Probability % Blue");
	probabilityBGroup.add(probabilitySliderBR.setup("Interaction Probability BR", probabilityBR, 0, 100));
	probabilityBGroup.add(probabilitySliderBG.setup("Interaction Probability BG", probabilityBG, 0, 100));
	probabilityBGroup.add(probabilitySliderBB.setup("Interaction Probability BB", probabilityBB, 0, 100));
	probabilityBGroup.add(probabilitySliderBW.setup("Interaction Probability BW", probabilityBW, 0, 100));
	probabilityBGroup.add(probabilitySliderBO.setup("Interaction Probability BO", probabilityBO, 0, 100));
	probabilityBGroup.add(probabilitySliderBK.setup("Interaction Probability BK", probabilityBK, 0, 100));
	probabilityBGroup.add(probabilitySliderBC.setup("Interaction Probability BC", probabilityBC, 0, 100));
	probabilityBGroup.add(probabilitySliderBD.setup("Interaction Probability BD", probabilityBD, 0, 100));
	gui.add(&probabilityBGroup);
	probabilityBGroup.minimize();

	// ProbabilityW Group
	probabilityWGroup.setup("Interaction Probability % White");
	probabilityWGroup.add(probabilitySliderWR.setup("Interaction Probability WR", probabilityWR, 0, 100));
	probabilityWGroup.add(probabilitySliderWG.setup("Interaction Probability WG", probabilityWG, 0, 100));
	probabilityWGroup.add(probabilitySliderWB.setup("Interaction Probability WB", probabilityWB, 0, 100));
	probabilityWGroup.add(probabilitySliderWW.setup("Interaction Probability WW", probabilityWW, 0, 100));
	probabilityWGroup.add(probabilitySliderWO.setup("Interaction Probability WO", probabilityWO, 0, 100));
	probabilityWGroup.add(probabilitySliderWK.setup("Interaction Probability WK", probabilityWK, 0, 100));
	probabilityWGroup.add(probabilitySliderWC.setup("Interaction Probability WC", probabilityWC, 0, 100));
	probabilityWGroup.add(probabilitySliderWD.setup("Interaction Probability WD", probabilityWD, 0, 100));
	gui.add(&probabilityWGroup);
	probabilityWGroup.minimize();

	// ProbabilityO Group
	probabilityOGroup.setup("Interaction Probability % Orange");
	probabilityOGroup.add(probabilitySliderOR.setup("Interaction Probability OR", probabilityOR, 0, 100));
	probabilityOGroup.add(probabilitySliderOG.setup("Interaction Probability OG", probabilityOG, 0, 100));
	probabilityOGroup.add(probabilitySliderOB.setup("Interaction Probability OB", probabilityOB, 0, 100));
	probabilityOGroup.add(probabilitySliderOW.setup("Interaction Probability OW", probabilityOW, 0, 100));
	probabilityOGroup.add(probabilitySliderOO.setup("Interaction Probability OO", probabilityOO, 0, 100));
	probabilityOGroup.add(probabilitySliderOK.setup("Interaction Probability OK", probabilityOK, 0, 100));
	probabilityOGroup.add(probabilitySliderOC.setup("Interaction Probability OC", probabilityOC, 0, 100));
	probabilityOGroup.add(probabilitySliderOD.setup("Interaction Probability OD", probabilityOD, 0, 100));
	gui.add(&probabilityOGroup);
	probabilityOGroup.minimize();

	// ProbabilityK Group
	probabilityKGroup.setup("Interaction Probability % Khaki");
	probabilityKGroup.add(probabilitySliderKR.setup("Interaction Probability KR", probabilityKR, 0, 100));
	probabilityKGroup.add(probabilitySliderKG.setup("Interaction Probability KG", probabilityKG, 0, 100));
	probabilityKGroup.add(probabilitySliderKB.setup("Interaction Probability KB", probabilityKB, 0, 100));
	probabilityKGroup.add(probabilitySliderKW.setup("Interaction Probability KW", probabilityKW, 0, 100));
	probabilityKGroup.add(probabilitySliderKO.setup("Interaction Probability KO", probabilityKO, 0, 100));
	probabilityKGroup.add(probabilitySliderKK.setup("Interaction Probability KK", probabilityKK, 0, 100));
	probabilityKGroup.add(probabilitySliderKC.setup("Interaction Probability KC", probabilityKC, 0, 100));
	probabilityKGroup.add(probabilitySliderKD.setup("Interaction Probability KD", probabilityKD, 0, 100));
	gui.add(&probabilityKGroup);
	probabilityKGroup.minimize();

	// ProbabilityC Group
	probabilityCGroup.setup("Interaction Probability % Crimson");
	probabilityCGroup.add(probabilitySliderCR.setup("Interaction Probability CR", probabilityCR, 0, 100));
	probabilityCGroup.add(probabilitySliderCG.setup("Interaction Probability CG", probabilityCG, 0, 100));
	probabilityCGroup.add(probabilitySliderCB.setup("Interaction Probability CB", probabilityCB, 0, 100));
	probabilityCGroup.add(probabilitySliderCW.setup("Interaction Probability CW", probabilityCW, 0, 100));
	probabilityCGroup.add(probabilitySliderCO.setup("Interaction Probability CO", probabilityCO, 0, 100));
	probabilityCGroup.add(probabilitySliderCK.setup("Interaction Probability CK", probabilityCK, 0, 100));
	probabilityCGroup.add(probabilitySliderCC.setup("Interaction Probability CC", probabilityCC, 0, 100));
	probabilityCGroup.add(probabilitySliderCD.setup("Interaction Probability CD", probabilityCD, 0, 100));
	gui.add(&probabilityCGroup);
	probabilityCGroup.minimize();

	// ProbabilityD Group
	probabilityDGroup.setup("Interaction Probability % Dark");
	probabilityDGroup.add(probabilitySliderDR.setup("Interaction Probability DR", probabilityDR, 0, 100));
	probabilityDGroup.add(probabilitySliderDG.setup("Interaction Probability DG", probabilityDG, 0, 100));
	probabilityDGroup.add(probabilitySliderDB.setup("Interaction Probability DB", probabilityDB, 0, 100));
	probabilityDGroup.add(probabilitySliderDW.setup("Interaction Probability DW", probabilityDW, 0, 100));
	probabilityDGroup.add(probabilitySliderDO.setup("Interaction Probability DO", probabilityDO, 0, 100));
	probabilityDGroup.add(probabilitySliderDK.setup("Interaction Probability DK", probabilityDK, 0, 100));
	probabilityDGroup.add(probabilitySliderDC.setup("Interaction Probability DC", probabilityDC, 0, 100));
	probabilityDGroup.add(probabilitySliderDD.setup("Interaction Probability DD", probabilityDD, 0, 100));
	gui.add(&probabilityDGroup);
	probabilityDGroup.minimize();

	// GREEN
	greenGroup.setup("Green");
	greenGroup.add(powerSliderGG.setup("green x green:", ppowerSliderGG, -100, 100));
	greenGroup.add(powerSliderGR.setup("green x red:", ppowerSliderGR, -100, 100));
	greenGroup.add(powerSliderGW.setup("green x white:", ppowerSliderGW, -100, 100));
	greenGroup.add(powerSliderGB.setup("green x blue:", ppowerSliderGB, -100, 100));
	greenGroup.add(powerSliderGO.setup("green x orange:", ppowerSliderGO, -100, 100));
	greenGroup.add(powerSliderGK.setup("green x khaki:", ppowerSliderGK, -100, 100));
	greenGroup.add(powerSliderGC.setup("green x crimson:", ppowerSliderGC, -100, 100));
	greenGroup.add(powerSliderGD.setup("green x dark:", ppowerSliderGD, -100, 100));

	greenGroup.add(vSliderGG.setup("radius g x g:", pvSliderGG, 0, 500));
	greenGroup.add(vSliderGR.setup("radius g x r:", pvSliderGR, 0, 500));
	greenGroup.add(vSliderGW.setup("radius g x w:", pvSliderGW, 0, 500));
	greenGroup.add(vSliderGB.setup("radius g x b:", pvSliderGB, 0, 500));
	greenGroup.add(vSliderGO.setup("radius g x o:", pvSliderGO, 0, 500));
	greenGroup.add(vSliderGK.setup("radius g x k:", pvSliderGK, 0, 500));
	greenGroup.add(vSliderGC.setup("radius g x c:", pvSliderGC, 0, 500));
	greenGroup.add(vSliderGD.setup("radius g x d:", pvSliderGD, 0, 500));

	greenGroup.minimize();
	gui.add(&greenGroup);

	// RED
	redGroup.setup("Red");
	redGroup.add(powerSliderRR.setup("red x red:", ppowerSliderRR, -100, 100));
	redGroup.add(powerSliderRG.setup("red x green:", ppowerSliderRG, -100, 100));
	redGroup.add(powerSliderRW.setup("red x white:", ppowerSliderRW, -100, 100));
	redGroup.add(powerSliderRB.setup("red x blue:", ppowerSliderRB, -100, 100));
	redGroup.add(powerSliderRO.setup("red x orange:", ppowerSliderRO, -100, 100));
	redGroup.add(powerSliderRK.setup("red x khaki:", ppowerSliderRK, -100, 100));
	redGroup.add(powerSliderRC.setup("red x crimson:", ppowerSliderRC, -100, 100));
	redGroup.add(powerSliderRD.setup("red x dark:", ppowerSliderRD, -100, 100));

	redGroup.add(vSliderRG.setup("radius r x g:", pvSliderRG, 0, 500));
	redGroup.add(vSliderRR.setup("radius r x r:", pvSliderRR, 0, 500));
	redGroup.add(vSliderRW.setup("radius r x w:", pvSliderRW, 0, 500));
	redGroup.add(vSliderRB.setup("radius r x b:", pvSliderRB, 0, 500));
	redGroup.add(vSliderRO.setup("radius r x o:", pvSliderRO, 0, 500));
	redGroup.add(vSliderRK.setup("radius r x k:", pvSliderRK, 0, 500));
	redGroup.add(vSliderRC.setup("radius r x c:", pvSliderRC, 0, 500));
	redGroup.add(vSliderRD.setup("radius r x d:", pvSliderRD, 0, 500));

	redGroup.minimize();
	gui.add(&redGroup);

	// WHITE
	whiteGroup.setup("White");
	whiteGroup.add(powerSliderWW.setup("white x white:", ppowerSliderWW, -100, 100));
	whiteGroup.add(powerSliderWR.setup("white x red:", ppowerSliderWR, -100, 100));
	whiteGroup.add(powerSliderWG.setup("white x green:", ppowerSliderWG, -100, 100));
	whiteGroup.add(powerSliderWB.setup("white x blue:", ppowerSliderWB, -100, 100));
	whiteGroup.add(powerSliderWO.setup("white x orange:", ppowerSliderWO, -100, 100));
	whiteGroup.add(powerSliderWK.setup("white x khaki:", ppowerSliderWK, -100, 100));
	whiteGroup.add(powerSliderWC.setup("white x crimson:", ppowerSliderWC, -100, 100));
	whiteGroup.add(powerSliderWD.setup("white x dark:", ppowerSliderWD, -100, 100));

	whiteGroup.add(vSliderWG.setup("radius w x g:", pvSliderWG, 0, 500));
	whiteGroup.add(vSliderWR.setup("radius w x r:", pvSliderWR, 0, 500));
	whiteGroup.add(vSliderWW.setup("radius w x w:", pvSliderWW, 0, 500));
	whiteGroup.add(vSliderWB.setup("radius w x b:", pvSliderWB, 0, 500));
	whiteGroup.add(vSliderWO.setup("radius w x o:", pvSliderWO, 0, 500));
	whiteGroup.add(vSliderWK.setup("radius w x k:", pvSliderWK, 0, 500));
	whiteGroup.add(vSliderWC.setup("radius w x c:", pvSliderWC, 0, 500));
	whiteGroup.add(vSliderWD.setup("radius w x d:", pvSliderWD, 0, 500));

	whiteGroup.minimize();
	gui.add(&whiteGroup);

	// BLUE
	blueGroup.setup("Blue");
	blueGroup.add(powerSliderBB.setup("blue x blue:", ppowerSliderBB, -100, 100));
	blueGroup.add(powerSliderBW.setup("blue x white:", ppowerSliderBW, -100, 100));
	blueGroup.add(powerSliderBR.setup("blue x red:", ppowerSliderBR, -100, 100));
	blueGroup.add(powerSliderBG.setup("blue x green:", ppowerSliderBG, -100, 100));
	blueGroup.add(powerSliderBO.setup("blue x orange:", ppowerSliderBO, -100, 100));
	blueGroup.add(powerSliderBK.setup("blue x khaki:", ppowerSliderBK, -100, 100));
	blueGroup.add(powerSliderBC.setup("blue x crimson:", ppowerSliderBC, -100, 100));
	blueGroup.add(powerSliderBD.setup("blue x dark:", ppowerSliderBD, -100, 100));

	blueGroup.add(vSliderBG.setup("radius b x g:", pvSliderBG, 0, 500));
	blueGroup.add(vSliderBR.setup("radius b x r:", pvSliderBR, 0, 500));
	blueGroup.add(vSliderBW.setup("radius b x w:", pvSliderBW, 0, 500));
	blueGroup.add(vSliderBB.setup("radius b x b:", pvSliderBB, 0, 500));
	blueGroup.add(vSliderBO.setup("radius b x o:", pvSliderBO, 0, 500));
	blueGroup.add(vSliderBK.setup("radius b x k:", pvSliderBK, 0, 500));
	blueGroup.add(vSliderBC.setup("radius b x c:", pvSliderBC, 0, 500));
	blueGroup.add(vSliderBD.setup("radius b x d:", pvSliderBD, 0, 500));

	blueGroup.minimize();
	gui.add(&blueGroup);

	// ORANGE
	orangeGroup.setup("Orange");
	orangeGroup.add(powerSliderOB.setup("orange x blue:", ppowerSliderOB, -100, 100));
	orangeGroup.add(powerSliderOW.setup("orange x white:", ppowerSliderOW, -100, 100));
	orangeGroup.add(powerSliderOR.setup("orange x red:", ppowerSliderOR, -100, 100));
	orangeGroup.add(powerSliderOG.setup("orange x green:", ppowerSliderOG, -100, 100));
	orangeGroup.add(powerSliderOO.setup("orange x orange:", ppowerSliderOO, -100, 100));
	orangeGroup.add(powerSliderOK.setup("orange x khaki:", ppowerSliderOK, -100, 100));
	orangeGroup.add(powerSliderOC.setup("orange x crimson:", ppowerSliderOC, -100, 100));
	orangeGroup.add(powerSliderOD.setup("orange x dark:", ppowerSliderOD, -100, 100));

	orangeGroup.add(vSliderOG.setup("radius o x g:", pvSliderOG, 0, 500));
	orangeGroup.add(vSliderOR.setup("radius o x r:", pvSliderOR, 0, 500));
	orangeGroup.add(vSliderOW.setup("radius o x w:", pvSliderOW, 0, 500));
	orangeGroup.add(vSliderOB.setup("radius o x b:", pvSliderOB, 0, 500));
	orangeGroup.add(vSliderOO.setup("radius o x o:", pvSliderOO, 0, 500));
	orangeGroup.add(vSliderOK.setup("radius o x k:", pvSliderOK, 0, 500));
	orangeGroup.add(vSliderOC.setup("radius o x c:", pvSliderOC, 0, 500));
	orangeGroup.add(vSliderOD.setup("radius o x d:", pvSliderOD, 0, 500));

	orangeGroup.minimize();
	gui.add(&orangeGroup);

	// KHAKI
	khakiGroup.setup("Khaki");
	khakiGroup.add(powerSliderKB.setup("khaki x blue:", ppowerSliderKB, -100, 100));
	khakiGroup.add(powerSliderKW.setup("khaki x white:", ppowerSliderKW, -100, 100));
	khakiGroup.add(powerSliderKR.setup("khaki x red:", ppowerSliderKR, -100, 100));
	khakiGroup.add(powerSliderKG.setup("khaki x green:", ppowerSliderKG, -100, 100));
	khakiGroup.add(powerSliderKO.setup("khaki x orange:", ppowerSliderKO, -100, 100));
	khakiGroup.add(powerSliderKK.setup("khaki x khaki:", ppowerSliderKK, -100, 100));
	khakiGroup.add(powerSliderKC.setup("khaki x crimson:", ppowerSliderKC, -100, 100));
	khakiGroup.add(powerSliderKD.setup("khaki x dark:", ppowerSliderKD, -100, 100));

	khakiGroup.add(vSliderKG.setup("radius k x g:", pvSliderKG, 0, 500));
	khakiGroup.add(vSliderKR.setup("radius k x r:", pvSliderKR, 0, 500));
	khakiGroup.add(vSliderKW.setup("radius k x w:", pvSliderKW, 0, 500));
	khakiGroup.add(vSliderKB.setup("radius k x b:", pvSliderKB, 0, 500));
	khakiGroup.add(vSliderKO.setup("radius k x o:", pvSliderKO, 0, 500));
	khakiGroup.add(vSliderKK.setup("radius k x k:", pvSliderKK, 0, 500));
	khakiGroup.add(vSliderKC.setup("radius k x c:", pvSliderKC, 0, 500));
	khakiGroup.add(vSliderKD.setup("radius k x d:", pvSliderKD, 0, 500));

	khakiGroup.minimize();
	gui.add(&khakiGroup);

	// CRIMSON
	crimsonGroup.setup("Crimson");
	crimsonGroup.add(powerSliderCB.setup("crimson x blue:", ppowerSliderCB, -100, 100));
	crimsonGroup.add(powerSliderCW.setup("crimson x white:", ppowerSliderCW, -100, 100));
	crimsonGroup.add(powerSliderCR.setup("crimson x red:", ppowerSliderCR, -100, 100));
	crimsonGroup.add(powerSliderCG.setup("crimson x green:", ppowerSliderCG, -100, 100));
	crimsonGroup.add(powerSliderCO.setup("crimson x orange:", ppowerSliderCO, -100, 100));
	crimsonGroup.add(powerSliderCK.setup("crimson x khaki:", ppowerSliderCK, -100, 100));
	crimsonGroup.add(powerSliderCC.setup("crimson x crimson:", ppowerSliderCC, -100, 100));
	crimsonGroup.add(powerSliderCD.setup("crimson x dark:", ppowerSliderCD, -100, 100));

	crimsonGroup.add(vSliderCG.setup("radius c x g:", pvSliderCG, 0, 500));
	crimsonGroup.add(vSliderCR.setup("radius c x r:", pvSliderCR, 0, 500));
	crimsonGroup.add(vSliderCW.setup("radius c x w:", pvSliderCW, 0, 500));
	crimsonGroup.add(vSliderCB.setup("radius c x b:", pvSliderCB, 0, 500));
	crimsonGroup.add(vSliderCO.setup("radius c x o:", pvSliderCO, 0, 500));
	crimsonGroup.add(vSliderCK.setup("radius c x k:", pvSliderCK, 0, 500));
	crimsonGroup.add(vSliderCC.setup("radius c x c:", pvSliderCC, 0, 500));
	crimsonGroup.add(vSliderCD.setup("radius c x d:", pvSliderCD, 0, 500));

	crimsonGroup.minimize();
	gui.add(&crimsonGroup);

	// DARK
	darkGroup.setup("Dark");
	darkGroup.add(powerSliderDB.setup("dark x blue:", ppowerSliderDB, -100, 100));
	darkGroup.add(powerSliderDW.setup("dark x white:", ppowerSliderDW, -100, 100));
	darkGroup.add(powerSliderDR.setup("dark x red:", ppowerSliderDR, -100, 100));
	darkGroup.add(powerSliderDG.setup("dark x green:", ppowerSliderDG, -100, 100));
	darkGroup.add(powerSliderDO.setup("dark x orange:", ppowerSliderDO, -100, 100));
	darkGroup.add(powerSliderDK.setup("dark x khaki:", ppowerSliderDK, -100, 100));
	darkGroup.add(powerSliderDC.setup("dark x crimson:", ppowerSliderDC, -100, 100));
	darkGroup.add(powerSliderDD.setup("dark x dark:", ppowerSliderDD, -100, 100));

	darkGroup.add(vSliderDG.setup("radius d x g:", pvSliderDG, 0, 500));
	darkGroup.add(vSliderDR.setup("radius d x r:", pvSliderDR, 0, 500));
	darkGroup.add(vSliderDW.setup("radius d x w:", pvSliderDW, 0, 500));
	darkGroup.add(vSliderDB.setup("radius d x b:", pvSliderDB, 0, 500));
	darkGroup.add(vSliderDO.setup("radius d x o:", pvSliderDO, 0, 500));
	darkGroup.add(vSliderDK.setup("radius d x k:", pvSliderDK, 0, 500));
	darkGroup.add(vSliderDC.setup("radius d x c:", pvSliderDC, 0, 500));
	darkGroup.add(vSliderDD.setup("radius d x d:", pvSliderDD, 0, 500));

	darkGroup.minimize();
	gui.add(&darkGroup);

	expGroup.setup("Experimental");
	expGroup.add(wallRepelSlider.setup("Wall Repel", wallRepel, 0, 100));
	expGroup.add(gravitySlider.setup("Gravity", worldGravity, -1, 1));
	expGroup.add(boundsToggle.setup("Bounded", false));
	expGroup.minimize();
	gui.add(&expGroup);

	ofSetBackgroundAuto(false);
	ofEnableAlphaBlending();

	restart();
}

//------------------------------Update simulation with sliders values------------------------------
void ofApp::update()
{
	physic_begin = clock();
	probability = probabilitySlider;
	viscosity = viscoSlider;
	viscosityR = viscoSliderR;
	viscosityG = viscoSliderG;
	viscosityW = viscoSliderW;
	viscosityB = viscoSliderB;
	viscosityO = viscoSliderO;
	viscosityK = viscoSliderK;
	viscosityC = viscoSliderC;
	viscosityD = viscoSliderD;

	worldGravity = gravitySlider;
	wallRepel = wallRepelSlider;
	evoChance = evoProbSlider;
	evoAmount = evoAmountSlider;

	if (evoToggle && ofRandom(1.0F) < (evoChance / 100.0F))
	{
		powerSliderRR = powerSliderRR + ((ofRandom(2.0F) - 1.0F) * (powerSliderRR.getMax() - powerSliderRR.getMin()) * (evoAmount / 100.0F));
		powerSliderRG = powerSliderRG + ((ofRandom(2.0F) - 1.0F) * (powerSliderRG.getMax() - powerSliderRG.getMin()) * (evoAmount / 100.0F));
		powerSliderRB = powerSliderRB + ((ofRandom(2.0F) - 1.0F) * (powerSliderRB.getMax() - powerSliderRB.getMin()) * (evoAmount / 100.0F));
		powerSliderRW = powerSliderRW + ((ofRandom(2.0F) - 1.0F) * (powerSliderRW.getMax() - powerSliderRW.getMin()) * (evoAmount / 100.0F));
		powerSliderRO = powerSliderRO + ((ofRandom(2.0F) - 1.0F) * (powerSliderRO.getMax() - powerSliderRO.getMin()) * (evoAmount / 100.0F));
		powerSliderRK = powerSliderRK + ((ofRandom(2.0F) - 1.0F) * (powerSliderRK.getMax() - powerSliderRK.getMin()) * (evoAmount / 100.0F));
		powerSliderRC = powerSliderRC + ((ofRandom(2.0F) - 1.0F) * (powerSliderRC.getMax() - powerSliderRC.getMin()) * (evoAmount / 100.0F));
		powerSliderRD = powerSliderRD + ((ofRandom(2.0F) - 1.0F) * (powerSliderRD.getMax() - powerSliderRD.getMin()) * (evoAmount / 100.0F));
		powerSliderGR = powerSliderGR + ((ofRandom(2.0F) - 1.0F) * (powerSliderGR.getMax() - powerSliderGR.getMin()) * (evoAmount / 100.0F));
		powerSliderGG = powerSliderGG + ((ofRandom(2.0F) - 1.0F) * (powerSliderGG.getMax() - powerSliderGG.getMin()) * (evoAmount / 100.0F));
		powerSliderGB = powerSliderGB + ((ofRandom(2.0F) - 1.0F) * (powerSliderGB.getMax() - powerSliderGB.getMin()) * (evoAmount / 100.0F));
		powerSliderGW = powerSliderGW + ((ofRandom(2.0F) - 1.0F) * (powerSliderGW.getMax() - powerSliderGW.getMin()) * (evoAmount / 100.0F));
		powerSliderGO = powerSliderGO + ((ofRandom(2.0F) - 1.0F) * (powerSliderGO.getMax() - powerSliderGO.getMin()) * (evoAmount / 100.0F));
		powerSliderGK = powerSliderGK + ((ofRandom(2.0F) - 1.0F) * (powerSliderGK.getMax() - powerSliderGK.getMin()) * (evoAmount / 100.0F));
		powerSliderGC = powerSliderGC + ((ofRandom(2.0F) - 1.0F) * (powerSliderGC.getMax() - powerSliderGC.getMin()) * (evoAmount / 100.0F));
		powerSliderGD = powerSliderGD + ((ofRandom(2.0F) - 1.0F) * (powerSliderGD.getMax() - powerSliderGD.getMin()) * (evoAmount / 100.0F));
		powerSliderBR = powerSliderBR + ((ofRandom(2.0F) - 1.0F) * (powerSliderBR.getMax() - powerSliderBR.getMin()) * (evoAmount / 100.0F));
		powerSliderBG = powerSliderBG + ((ofRandom(2.0F) - 1.0F) * (powerSliderBG.getMax() - powerSliderBG.getMin()) * (evoAmount / 100.0F));
		powerSliderBB = powerSliderBB + ((ofRandom(2.0F) - 1.0F) * (powerSliderBB.getMax() - powerSliderBB.getMin()) * (evoAmount / 100.0F));
		powerSliderBW = powerSliderBW + ((ofRandom(2.0F) - 1.0F) * (powerSliderBW.getMax() - powerSliderBW.getMin()) * (evoAmount / 100.0F));
		powerSliderBO = powerSliderBO + ((ofRandom(2.0F) - 1.0F) * (powerSliderBO.getMax() - powerSliderBO.getMin()) * (evoAmount / 100.0F));
		powerSliderBK = powerSliderBK + ((ofRandom(2.0F) - 1.0F) * (powerSliderBK.getMax() - powerSliderBK.getMin()) * (evoAmount / 100.0F));
		powerSliderBC = powerSliderBC + ((ofRandom(2.0F) - 1.0F) * (powerSliderBC.getMax() - powerSliderBC.getMin()) * (evoAmount / 100.0F));
		powerSliderBD = powerSliderBD + ((ofRandom(2.0F) - 1.0F) * (powerSliderBD.getMax() - powerSliderBD.getMin()) * (evoAmount / 100.0F));
		powerSliderWR = powerSliderWR + ((ofRandom(2.0F) - 1.0F) * (powerSliderWR.getMax() - powerSliderWR.getMin()) * (evoAmount / 100.0F));
		powerSliderWG = powerSliderWG + ((ofRandom(2.0F) - 1.0F) * (powerSliderWG.getMax() - powerSliderWG.getMin()) * (evoAmount / 100.0F));
		powerSliderWB = powerSliderWB + ((ofRandom(2.0F) - 1.0F) * (powerSliderWB.getMax() - powerSliderWB.getMin()) * (evoAmount / 100.0F));
		powerSliderWW = powerSliderWW + ((ofRandom(2.0F) - 1.0F) * (powerSliderWW.getMax() - powerSliderWW.getMin()) * (evoAmount / 100.0F));
		powerSliderWO = powerSliderWO + ((ofRandom(2.0F) - 1.0F) * (powerSliderWO.getMax() - powerSliderWO.getMin()) * (evoAmount / 100.0F));
		powerSliderWK = powerSliderWK + ((ofRandom(2.0F) - 1.0F) * (powerSliderWK.getMax() - powerSliderWK.getMin()) * (evoAmount / 100.0F));
		powerSliderWC = powerSliderWC + ((ofRandom(2.0F) - 1.0F) * (powerSliderWC.getMax() - powerSliderWC.getMin()) * (evoAmount / 100.0F));
		powerSliderWD = powerSliderWD + ((ofRandom(2.0F) - 1.0F) * (powerSliderWD.getMax() - powerSliderWD.getMin()) * (evoAmount / 100.0F));
		powerSliderOR = powerSliderOR + ((ofRandom(2.0F) - 1.0F) * (powerSliderOR.getMax() - powerSliderOR.getMin()) * (evoAmount / 100.0F));
		powerSliderOG = powerSliderOG + ((ofRandom(2.0F) - 1.0F) * (powerSliderOG.getMax() - powerSliderOG.getMin()) * (evoAmount / 100.0F));
		powerSliderOB = powerSliderOB + ((ofRandom(2.0F) - 1.0F) * (powerSliderOB.getMax() - powerSliderOB.getMin()) * (evoAmount / 100.0F));
		powerSliderOW = powerSliderOW + ((ofRandom(2.0F) - 1.0F) * (powerSliderOW.getMax() - powerSliderOW.getMin()) * (evoAmount / 100.0F));
		powerSliderOO = powerSliderOO + ((ofRandom(2.0F) - 1.0F) * (powerSliderOO.getMax() - powerSliderOO.getMin()) * (evoAmount / 100.0F));
		powerSliderOK = powerSliderOK + ((ofRandom(2.0F) - 1.0F) * (powerSliderOK.getMax() - powerSliderOK.getMin()) * (evoAmount / 100.0F));
		powerSliderOC = powerSliderOC + ((ofRandom(2.0F) - 1.0F) * (powerSliderOC.getMax() - powerSliderOC.getMin()) * (evoAmount / 100.0F));
		powerSliderOD = powerSliderOD + ((ofRandom(2.0F) - 1.0F) * (powerSliderOD.getMax() - powerSliderOD.getMin()) * (evoAmount / 100.0F));
		powerSliderKR = powerSliderKR + ((ofRandom(2.0F) - 1.0F) * (powerSliderKR.getMax() - powerSliderKR.getMin()) * (evoAmount / 100.0F));
		powerSliderKG = powerSliderKG + ((ofRandom(2.0F) - 1.0F) * (powerSliderKG.getMax() - powerSliderKG.getMin()) * (evoAmount / 100.0F));
		powerSliderKB = powerSliderKB + ((ofRandom(2.0F) - 1.0F) * (powerSliderKB.getMax() - powerSliderKB.getMin()) * (evoAmount / 100.0F));
		powerSliderKW = powerSliderKW + ((ofRandom(2.0F) - 1.0F) * (powerSliderKW.getMax() - powerSliderKW.getMin()) * (evoAmount / 100.0F));
		powerSliderKO = powerSliderKO + ((ofRandom(2.0F) - 1.0F) * (powerSliderKO.getMax() - powerSliderKO.getMin()) * (evoAmount / 100.0F));
		powerSliderKK = powerSliderKK + ((ofRandom(2.0F) - 1.0F) * (powerSliderKK.getMax() - powerSliderKK.getMin()) * (evoAmount / 100.0F));
		powerSliderKC = powerSliderKC + ((ofRandom(2.0F) - 1.0F) * (powerSliderKC.getMax() - powerSliderKC.getMin()) * (evoAmount / 100.0F));
		powerSliderKD = powerSliderKD + ((ofRandom(2.0F) - 1.0F) * (powerSliderKD.getMax() - powerSliderKD.getMin()) * (evoAmount / 100.0F));
		powerSliderCR = powerSliderCR + ((ofRandom(2.0F) - 1.0F) * (powerSliderCR.getMax() - powerSliderCR.getMin()) * (evoAmount / 100.0F));
		powerSliderCG = powerSliderCG + ((ofRandom(2.0F) - 1.0F) * (powerSliderCG.getMax() - powerSliderCG.getMin()) * (evoAmount / 100.0F));
		powerSliderCB = powerSliderCB + ((ofRandom(2.0F) - 1.0F) * (powerSliderCB.getMax() - powerSliderCB.getMin()) * (evoAmount / 100.0F));
		powerSliderCW = powerSliderCW + ((ofRandom(2.0F) - 1.0F) * (powerSliderCW.getMax() - powerSliderCW.getMin()) * (evoAmount / 100.0F));
		powerSliderCO = powerSliderCO + ((ofRandom(2.0F) - 1.0F) * (powerSliderCO.getMax() - powerSliderCO.getMin()) * (evoAmount / 100.0F));
		powerSliderCK = powerSliderCK + ((ofRandom(2.0F) - 1.0F) * (powerSliderCK.getMax() - powerSliderCK.getMin()) * (evoAmount / 100.0F));
		powerSliderCC = powerSliderCC + ((ofRandom(2.0F) - 1.0F) * (powerSliderCC.getMax() - powerSliderCC.getMin()) * (evoAmount / 100.0F));
		powerSliderCD = powerSliderCD + ((ofRandom(2.0F) - 1.0F) * (powerSliderCD.getMax() - powerSliderCD.getMin()) * (evoAmount / 100.0F));
		powerSliderDR = powerSliderDR + ((ofRandom(2.0F) - 1.0F) * (powerSliderDR.getMax() - powerSliderDR.getMin()) * (evoAmount / 100.0F));
		powerSliderDG = powerSliderDG + ((ofRandom(2.0F) - 1.0F) * (powerSliderDG.getMax() - powerSliderDG.getMin()) * (evoAmount / 100.0F));
		powerSliderDB = powerSliderDB + ((ofRandom(2.0F) - 1.0F) * (powerSliderDB.getMax() - powerSliderDB.getMin()) * (evoAmount / 100.0F));
		powerSliderDW = powerSliderDW + ((ofRandom(2.0F) - 1.0F) * (powerSliderDW.getMax() - powerSliderDW.getMin()) * (evoAmount / 100.0F));
		powerSliderDO = powerSliderDO + ((ofRandom(2.0F) - 1.0F) * (powerSliderDO.getMax() - powerSliderDO.getMin()) * (evoAmount / 100.0F));
		powerSliderDK = powerSliderDK + ((ofRandom(2.0F) - 1.0F) * (powerSliderDK.getMax() - powerSliderDK.getMin()) * (evoAmount / 100.0F));
		powerSliderDC = powerSliderDC + ((ofRandom(2.0F) - 1.0F) * (powerSliderDC.getMax() - powerSliderDC.getMin()) * (evoAmount / 100.0F));
		powerSliderDD = powerSliderDD + ((ofRandom(2.0F) - 1.0F) * (powerSliderDD.getMax() - powerSliderDD.getMin()) * (evoAmount / 100.0F));

		vSliderRR = vSliderRR + ((ofRandom(2.0F) - 1.0F) * (vSliderRR.getMax() - vSliderRR.getMin()) * (evoAmount / 100.0F));
		vSliderRG = vSliderRG + ((ofRandom(2.0F) - 1.0F) * (vSliderRG.getMax() - vSliderRG.getMin()) * (evoAmount / 100.0F));
		vSliderRB = vSliderRB + ((ofRandom(2.0F) - 1.0F) * (vSliderRB.getMax() - vSliderRB.getMin()) * (evoAmount / 100.0F));
		vSliderRW = vSliderRW + ((ofRandom(2.0F) - 1.0F) * (vSliderRW.getMax() - vSliderRW.getMin()) * (evoAmount / 100.0F));
		vSliderRO = vSliderRO + ((ofRandom(2.0F) - 1.0F) * (vSliderRO.getMax() - vSliderRO.getMin()) * (evoAmount / 100.0F));
		vSliderRK = vSliderRK + ((ofRandom(2.0F) - 1.0F) * (vSliderRK.getMax() - vSliderRK.getMin()) * (evoAmount / 100.0F));
		vSliderRC = vSliderRC + ((ofRandom(2.0F) - 1.0F) * (vSliderRC.getMax() - vSliderRC.getMin()) * (evoAmount / 100.0F));
		vSliderRD = vSliderRD + ((ofRandom(2.0F) - 1.0F) * (vSliderRD.getMax() - vSliderRD.getMin()) * (evoAmount / 100.0F));
		vSliderGR = vSliderGR + ((ofRandom(2.0F) - 1.0F) * (vSliderGR.getMax() - vSliderGR.getMin()) * (evoAmount / 100.0F));
		vSliderGG = vSliderGG + ((ofRandom(2.0F) - 1.0F) * (vSliderGG.getMax() - vSliderGG.getMin()) * (evoAmount / 100.0F));
		vSliderGB = vSliderGB + ((ofRandom(2.0F) - 1.0F) * (vSliderGB.getMax() - vSliderGB.getMin()) * (evoAmount / 100.0F));
		vSliderGW = vSliderGW + ((ofRandom(2.0F) - 1.0F) * (vSliderGW.getMax() - vSliderGW.getMin()) * (evoAmount / 100.0F));
		vSliderGO = vSliderGO + ((ofRandom(2.0F) - 1.0F) * (vSliderGO.getMax() - vSliderGO.getMin()) * (evoAmount / 100.0F));
		vSliderGK = vSliderGK + ((ofRandom(2.0F) - 1.0F) * (vSliderGK.getMax() - vSliderGK.getMin()) * (evoAmount / 100.0F));
		vSliderGC = vSliderGC + ((ofRandom(2.0F) - 1.0F) * (vSliderGC.getMax() - vSliderGC.getMin()) * (evoAmount / 100.0F));
		vSliderGD = vSliderGD + ((ofRandom(2.0F) - 1.0F) * (vSliderGD.getMax() - vSliderGD.getMin()) * (evoAmount / 100.0F));
		vSliderBR = vSliderBR + ((ofRandom(2.0F) - 1.0F) * (vSliderBR.getMax() - vSliderBR.getMin()) * (evoAmount / 100.0F));
		vSliderBG = vSliderBG + ((ofRandom(2.0F) - 1.0F) * (vSliderBG.getMax() - vSliderBG.getMin()) * (evoAmount / 100.0F));
		vSliderBB = vSliderBB + ((ofRandom(2.0F) - 1.0F) * (vSliderBB.getMax() - vSliderBB.getMin()) * (evoAmount / 100.0F));
		vSliderBW = vSliderBW + ((ofRandom(2.0F) - 1.0F) * (vSliderBW.getMax() - vSliderBW.getMin()) * (evoAmount / 100.0F));
		vSliderBO = vSliderBO + ((ofRandom(2.0F) - 1.0F) * (vSliderBO.getMax() - vSliderBO.getMin()) * (evoAmount / 100.0F));
		vSliderBK = vSliderBK + ((ofRandom(2.0F) - 1.0F) * (vSliderBK.getMax() - vSliderBK.getMin()) * (evoAmount / 100.0F));
		vSliderBC = vSliderBC + ((ofRandom(2.0F) - 1.0F) * (vSliderBC.getMax() - vSliderBC.getMin()) * (evoAmount / 100.0F));
		vSliderBD = vSliderBD + ((ofRandom(2.0F) - 1.0F) * (vSliderBD.getMax() - vSliderBD.getMin()) * (evoAmount / 100.0F));
		vSliderWR = vSliderWR + ((ofRandom(2.0F) - 1.0F) * (vSliderWR.getMax() - vSliderWR.getMin()) * (evoAmount / 100.0F));
		vSliderWG = vSliderWG + ((ofRandom(2.0F) - 1.0F) * (vSliderWG.getMax() - vSliderWG.getMin()) * (evoAmount / 100.0F));
		vSliderWB = vSliderWB + ((ofRandom(2.0F) - 1.0F) * (vSliderWB.getMax() - vSliderWB.getMin()) * (evoAmount / 100.0F));
		vSliderWW = vSliderWW + ((ofRandom(2.0F) - 1.0F) * (vSliderWW.getMax() - vSliderWW.getMin()) * (evoAmount / 100.0F));
		vSliderWO = vSliderWO + ((ofRandom(2.0F) - 1.0F) * (vSliderWO.getMax() - vSliderWO.getMin()) * (evoAmount / 100.0F));
		vSliderWK = vSliderWK + ((ofRandom(2.0F) - 1.0F) * (vSliderWK.getMax() - vSliderWK.getMin()) * (evoAmount / 100.0F));
		vSliderWC = vSliderWC + ((ofRandom(2.0F) - 1.0F) * (vSliderWC.getMax() - vSliderWC.getMin()) * (evoAmount / 100.0F));
		vSliderWD = vSliderWD + ((ofRandom(2.0F) - 1.0F) * (vSliderWD.getMax() - vSliderWD.getMin()) * (evoAmount / 100.0F));
		vSliderOR = vSliderOR + ((ofRandom(2.0F) - 1.0F) * (vSliderOR.getMax() - vSliderOR.getMin()) * (evoAmount / 100.0F));
		vSliderOG = vSliderOG + ((ofRandom(2.0F) - 1.0F) * (vSliderOG.getMax() - vSliderOG.getMin()) * (evoAmount / 100.0F));
		vSliderOB = vSliderOB + ((ofRandom(2.0F) - 1.0F) * (vSliderOB.getMax() - vSliderOB.getMin()) * (evoAmount / 100.0F));
		vSliderOW = vSliderOW + ((ofRandom(2.0F) - 1.0F) * (vSliderOW.getMax() - vSliderOW.getMin()) * (evoAmount / 100.0F));
		vSliderOO = vSliderOO + ((ofRandom(2.0F) - 1.0F) * (vSliderOO.getMax() - vSliderOO.getMin()) * (evoAmount / 100.0F));
		vSliderOK = vSliderOK + ((ofRandom(2.0F) - 1.0F) * (vSliderOK.getMax() - vSliderOK.getMin()) * (evoAmount / 100.0F));
		vSliderOC = vSliderOC + ((ofRandom(2.0F) - 1.0F) * (vSliderOC.getMax() - vSliderOC.getMin()) * (evoAmount / 100.0F));
		vSliderOD = vSliderOD + ((ofRandom(2.0F) - 1.0F) * (vSliderOD.getMax() - vSliderOD.getMin()) * (evoAmount / 100.0F));
		vSliderKR = vSliderKR + ((ofRandom(2.0F) - 1.0F) * (vSliderKR.getMax() - vSliderKR.getMin()) * (evoAmount / 100.0F));
		vSliderKG = vSliderKG + ((ofRandom(2.0F) - 1.0F) * (vSliderKG.getMax() - vSliderKG.getMin()) * (evoAmount / 100.0F));
		vSliderKB = vSliderKB + ((ofRandom(2.0F) - 1.0F) * (vSliderKB.getMax() - vSliderKB.getMin()) * (evoAmount / 100.0F));
		vSliderKW = vSliderKW + ((ofRandom(2.0F) - 1.0F) * (vSliderKW.getMax() - vSliderKW.getMin()) * (evoAmount / 100.0F));
		vSliderKO = vSliderKO + ((ofRandom(2.0F) - 1.0F) * (vSliderKO.getMax() - vSliderKO.getMin()) * (evoAmount / 100.0F));
		vSliderKK = vSliderKK + ((ofRandom(2.0F) - 1.0F) * (vSliderKK.getMax() - vSliderKK.getMin()) * (evoAmount / 100.0F));
		vSliderKC = vSliderKC + ((ofRandom(2.0F) - 1.0F) * (vSliderKC.getMax() - vSliderKC.getMin()) * (evoAmount / 100.0F));
		vSliderKD = vSliderKD + ((ofRandom(2.0F) - 1.0F) * (vSliderKD.getMax() - vSliderKD.getMin()) * (evoAmount / 100.0F));
		vSliderCR = vSliderCR + ((ofRandom(2.0F) - 1.0F) * (vSliderCR.getMax() - vSliderCR.getMin()) * (evoAmount / 100.0F));
		vSliderCG = vSliderCG + ((ofRandom(2.0F) - 1.0F) * (vSliderCG.getMax() - vSliderCG.getMin()) * (evoAmount / 100.0F));
		vSliderCB = vSliderCB + ((ofRandom(2.0F) - 1.0F) * (vSliderCB.getMax() - vSliderCB.getMin()) * (evoAmount / 100.0F));
		vSliderCW = vSliderCW + ((ofRandom(2.0F) - 1.0F) * (vSliderCW.getMax() - vSliderCW.getMin()) * (evoAmount / 100.0F));
		vSliderCO = vSliderCO + ((ofRandom(2.0F) - 1.0F) * (vSliderCO.getMax() - vSliderCO.getMin()) * (evoAmount / 100.0F));
		vSliderCK = vSliderCK + ((ofRandom(2.0F) - 1.0F) * (vSliderCK.getMax() - vSliderCK.getMin()) * (evoAmount / 100.0F));
		vSliderCC = vSliderCC + ((ofRandom(2.0F) - 1.0F) * (vSliderCC.getMax() - vSliderCC.getMin()) * (evoAmount / 100.0F));
		vSliderCD = vSliderCD + ((ofRandom(2.0F) - 1.0F) * (vSliderCD.getMax() - vSliderCD.getMin()) * (evoAmount / 100.0F));
		vSliderDR = vSliderDR + ((ofRandom(2.0F) - 1.0F) * (vSliderDR.getMax() - vSliderDR.getMin()) * (evoAmount / 100.0F));
		vSliderDG = vSliderDG + ((ofRandom(2.0F) - 1.0F) * (vSliderDG.getMax() - vSliderDG.getMin()) * (evoAmount / 100.0F));
		vSliderDB = vSliderDB + ((ofRandom(2.0F) - 1.0F) * (vSliderDB.getMax() - vSliderDB.getMin()) * (evoAmount / 100.0F));
		vSliderDW = vSliderDW + ((ofRandom(2.0F) - 1.0F) * (vSliderDW.getMax() - vSliderDW.getMin()) * (evoAmount / 100.0F));
		vSliderDO = vSliderDO + ((ofRandom(2.0F) - 1.0F) * (vSliderDO.getMax() - vSliderDO.getMin()) * (evoAmount / 100.0F));
		vSliderDK = vSliderDK + ((ofRandom(2.0F) - 1.0F) * (vSliderDK.getMax() - vSliderDK.getMin()) * (evoAmount / 100.0F));
		vSliderDC = vSliderDC + ((ofRandom(2.0F) - 1.0F) * (vSliderDC.getMax() - vSliderDC.getMin()) * (evoAmount / 100.0F));
		vSliderDD = vSliderDD + ((ofRandom(2.0F) - 1.0F) * (vSliderDD.getMax() - vSliderDD.getMin()) * (evoAmount / 100.0F));
		
		viscoSlider = viscoSlider + ((ofRandom(2.0F) - 1.0F) * (viscoSlider.getMax() - viscoSlider.getMin()) * (evoAmount / 100.0F));
		viscoSliderR = viscoSliderR + ((ofRandom(2.0F) - 1.0F) * (viscoSliderR.getMax() - viscoSliderR.getMin()) * (evoAmount / 100.0F));
		viscoSliderG = viscoSliderG + ((ofRandom(2.0F) - 1.0F) * (viscoSliderG.getMax() - viscoSliderG.getMin()) * (evoAmount / 100.0F));
		viscoSliderB = viscoSliderB + ((ofRandom(2.0F) - 1.0F) * (viscoSliderB.getMax() - viscoSliderB.getMin()) * (evoAmount / 100.0F));
		viscoSliderW = viscoSliderW + ((ofRandom(2.0F) - 1.0F) * (viscoSliderW.getMax() - viscoSliderW.getMin()) * (evoAmount / 100.0F));
		viscoSliderO = viscoSliderO + ((ofRandom(2.0F) - 1.0F) * (viscoSliderO.getMax() - viscoSliderO.getMin()) * (evoAmount / 100.0F));
		viscoSliderK = viscoSliderK + ((ofRandom(2.0F) - 1.0F) * (viscoSliderK.getMax() - viscoSliderK.getMin()) * (evoAmount / 100.0F));
		viscoSliderC = viscoSliderC + ((ofRandom(2.0F) - 1.0F) * (viscoSliderC.getMax() - viscoSliderC.getMin()) * (evoAmount / 100.0F));
		viscoSliderD = viscoSliderD + ((ofRandom(2.0F) - 1.0F) * (viscoSliderD.getMax() - viscoSliderD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider = probabilitySlider + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider.getMax() - probabilitySlider.getMin()) * (evoAmount / 100.0F));

		probabilitySliderRR = probabilitySliderRR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRR.getMax() - probabilitySliderRR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRG = probabilitySliderRG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRG.getMax() - probabilitySliderRG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRB = probabilitySliderRB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRB.getMax() - probabilitySliderRB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRW = probabilitySliderRW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRW.getMax() - probabilitySliderRW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRO = probabilitySliderRO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRO.getMax() - probabilitySliderRO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRK = probabilitySliderRK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRK.getMax() - probabilitySliderRK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRC = probabilitySliderRC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRC.getMax() - probabilitySliderRC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRD = probabilitySliderRD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRD.getMax() - probabilitySliderRD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderGR = probabilitySliderGR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGR.getMax() - probabilitySliderGR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGG = probabilitySliderGG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGG.getMax() - probabilitySliderGG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGB = probabilitySliderGB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGB.getMax() - probabilitySliderGB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGW = probabilitySliderGW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGW.getMax() - probabilitySliderGW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGO = probabilitySliderGO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGO.getMax() - probabilitySliderGO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGK = probabilitySliderGK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGK.getMax() - probabilitySliderGK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGC = probabilitySliderGC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGC.getMax() - probabilitySliderGC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGD = probabilitySliderGD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGD.getMax() - probabilitySliderGD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderBR = probabilitySliderBR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBR.getMax() - probabilitySliderBR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBG = probabilitySliderBG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBG.getMax() - probabilitySliderBG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBB = probabilitySliderBB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBB.getMax() - probabilitySliderBB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBW = probabilitySliderBW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBW.getMax() - probabilitySliderBW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBO = probabilitySliderBO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBO.getMax() - probabilitySliderBO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBK = probabilitySliderBK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBK.getMax() - probabilitySliderBK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBC = probabilitySliderBC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBC.getMax() - probabilitySliderBC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBD = probabilitySliderBD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBD.getMax() - probabilitySliderBD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderWR = probabilitySliderWR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWR.getMax() - probabilitySliderWR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWG = probabilitySliderWG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWG.getMax() - probabilitySliderWG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWB = probabilitySliderWB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWB.getMax() - probabilitySliderWB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWW = probabilitySliderWW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWW.getMax() - probabilitySliderWW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWO = probabilitySliderWO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWO.getMax() - probabilitySliderWO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWK = probabilitySliderWK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWK.getMax() - probabilitySliderWK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWC = probabilitySliderWC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWC.getMax() - probabilitySliderWC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWD = probabilitySliderWD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWD.getMax() - probabilitySliderWD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderOR = probabilitySliderOR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOR.getMax() - probabilitySliderOR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOG = probabilitySliderOG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOG.getMax() - probabilitySliderOG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOB = probabilitySliderOB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOB.getMax() - probabilitySliderOB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOW = probabilitySliderOW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOW.getMax() - probabilitySliderOW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOO = probabilitySliderOO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOO.getMax() - probabilitySliderOO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOK = probabilitySliderOK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOK.getMax() - probabilitySliderOK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOC = probabilitySliderOC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOC.getMax() - probabilitySliderOC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOD = probabilitySliderOD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOD.getMax() - probabilitySliderOD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderKR = probabilitySliderKR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKR.getMax() - probabilitySliderKR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKG = probabilitySliderKG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKG.getMax() - probabilitySliderKG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKB = probabilitySliderKB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKB.getMax() - probabilitySliderKB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKW = probabilitySliderKW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKW.getMax() - probabilitySliderKW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKO = probabilitySliderKO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKO.getMax() - probabilitySliderKO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKK = probabilitySliderKK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKK.getMax() - probabilitySliderKK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKC = probabilitySliderKC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKC.getMax() - probabilitySliderKC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKD = probabilitySliderKD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKD.getMax() - probabilitySliderKD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderCR = probabilitySliderCR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCR.getMax() - probabilitySliderCR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCG = probabilitySliderCG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCG.getMax() - probabilitySliderCG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCB = probabilitySliderCB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCB.getMax() - probabilitySliderCB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCW = probabilitySliderCW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCW.getMax() - probabilitySliderCW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCO = probabilitySliderCO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCO.getMax() - probabilitySliderCO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCK = probabilitySliderCK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCK.getMax() - probabilitySliderCK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCC = probabilitySliderCC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCC.getMax() - probabilitySliderCC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCD = probabilitySliderCD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCD.getMax() - probabilitySliderCD.getMin()) * (evoAmount / 100.0F));

		probabilitySliderDR = probabilitySliderDR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDR.getMax() - probabilitySliderDR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDG = probabilitySliderDG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDG.getMax() - probabilitySliderDG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDB = probabilitySliderDB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDB.getMax() - probabilitySliderDB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDW = probabilitySliderDW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDW.getMax() - probabilitySliderDW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDO = probabilitySliderDO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDO.getMax() - probabilitySliderDO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDK = probabilitySliderDK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDK.getMax() - probabilitySliderDK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDC = probabilitySliderDC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDC.getMax() - probabilitySliderDC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDD = probabilitySliderDD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDD.getMax() - probabilitySliderDD.getMin()) * (evoAmount / 100.0F));

		//numberSliderG = numberSliderG + ((ofRandom(2.0F) - 1.0F) * (numberSliderG.getMax() - numberSliderG.getMin()) * (evoAmount / 100.0F))
		//numberSliderR = numberSliderR + ((ofRandom(2.0F) - 1.0F) * (numberSliderR.getMax() - numberSliderR.getMin()) * (evoAmount / 100.0F));
		//numberSliderW = numberSliderW + ((ofRandom(2.0F) - 1.0F) * (numberSliderW.getMax() - numberSliderW.getMin()) * (evoAmount / 100.0F));
		//numberSliderB = numberSliderB + ((ofRandom(2.0F) - 1.0F) * (numberSliderB.getMax() - numberSliderB.getMin()) * (evoAmount / 100.0F));
		//numberSliderO = numberSliderO + ((ofRandom(2.0F) - 1.0F) * (numberSliderO.getMax() - numberSliderO.getMin()) * (evoAmount / 100.0F));
		//numberSliderK = numberSliderK + ((ofRandom(2.0F) - 1.0F) * (numberSliderK.getMax() - numberSliderK.getMin()) * (evoAmount / 100.0F));
		//numberSliderC = numberSliderC + ((ofRandom(2.0F) - 1.0F) * (numberSliderC.getMax() - numberSliderC.getMin()) * (evoAmount / 100.0F));
		//numberSliderD = numberSliderD + ((ofRandom(2.0F) - 1.0F) * (numberSliderD.getMax() - numberSliderD.getMin()) * (evoAmount / 100.0F));

		if (vSliderRR < 0.0F) vSliderRR = 0;
		if (vSliderRG < 0.0F) vSliderRG = 0;
		if (vSliderRB < 0.0F) vSliderRB = 0;
		if (vSliderRW < 0.0F) vSliderRW = 0;
		if (vSliderRO < 0.0F) vSliderRO = 0;
		if (vSliderRK < 0.0F) vSliderRK = 0;
		if (vSliderRC < 0.0F) vSliderRC = 0;
		if (vSliderRD < 0.0F) vSliderRD = 0;
		if (vSliderGR < 0.0F) vSliderGR = 0;
		if (vSliderGG < 0.0F) vSliderGG = 0;
		if (vSliderGB < 0.0F) vSliderGB = 0;
		if (vSliderGW < 0.0F) vSliderGW = 0;
		if (vSliderGO < 0.0F) vSliderGO = 0;
		if (vSliderGK < 0.0F) vSliderGK = 0;
		if (vSliderGC < 0.0F) vSliderGC = 0;
		if (vSliderGD < 0.0F) vSliderGD = 0;
		if (vSliderBR < 0.0F) vSliderBR = 0;
		if (vSliderBG < 0.0F) vSliderBG = 0;
		if (vSliderBB < 0.0F) vSliderBB = 0;
		if (vSliderBW < 0.0F) vSliderBW = 0;
		if (vSliderBO < 0.0F) vSliderBO = 0;
		if (vSliderBK < 0.0F) vSliderBK = 0;
		if (vSliderBC < 0.0F) vSliderBC = 0;
		if (vSliderBD < 0.0F) vSliderBD = 0;
		if (vSliderWR < 0.0F) vSliderWR = 0;
		if (vSliderWG < 0.0F) vSliderWG = 0;
		if (vSliderWB < 0.0F) vSliderWB = 0;
		if (vSliderWW < 0.0F) vSliderWW = 0;
		if (vSliderWO < 0.0F) vSliderWO = 0;
		if (vSliderWK < 0.0F) vSliderWK = 0;
		if (vSliderWC < 0.0F) vSliderWC = 0;
		if (vSliderWD < 0.0F) vSliderWD = 0;
		if (vSliderOR < 0.0F) vSliderOR = 0;
		if (vSliderOG < 0.0F) vSliderOG = 0;
		if (vSliderOB < 0.0F) vSliderOB = 0;
		if (vSliderOW < 0.0F) vSliderOW = 0;
		if (vSliderOO < 0.0F) vSliderOO = 0;
		if (vSliderOK < 0.0F) vSliderOK = 0;
		if (vSliderOC < 0.0F) vSliderOC = 0;
		if (vSliderOD < 0.0F) vSliderOD = 0;
		if (vSliderKR < 0.0F) vSliderKR = 0;
		if (vSliderKG < 0.0F) vSliderKG = 0;
		if (vSliderKB < 0.0F) vSliderKB = 0;
		if (vSliderKW < 0.0F) vSliderKW = 0;
		if (vSliderKO < 0.0F) vSliderKO = 0;
		if (vSliderKK < 0.0F) vSliderKK = 0;
		if (vSliderKC < 0.0F) vSliderKC = 0;
		if (vSliderKD < 0.0F) vSliderKD = 0;
		if (vSliderCR < 0.0F) vSliderCR = 0;
		if (vSliderCG < 0.0F) vSliderCG = 0;
		if (vSliderCB < 0.0F) vSliderCB = 0;
		if (vSliderCW < 0.0F) vSliderCW = 0;
		if (vSliderCO < 0.0F) vSliderCO = 0;
		if (vSliderCK < 0.0F) vSliderCK = 0;
		if (vSliderCC < 0.0F) vSliderCC = 0;
		if (vSliderCD < 0.0F) vSliderCD = 0;
		if (vSliderDR < 0.0F) vSliderDR = 0;
		if (vSliderDG < 0.0F) vSliderDG = 0;
		if (vSliderDB < 0.0F) vSliderDB = 0;
		if (vSliderDW < 0.0F) vSliderDW = 0;
		if (vSliderDO < 0.0F) vSliderDO = 0;
		if (vSliderDK < 0.0F) vSliderDK = 0;
		if (vSliderDC < 0.0F) vSliderDC = 0;
		if (vSliderDD < 0.0F) vSliderDD = 0;

		if (probabilitySlider < 0.0F) probabilitySlider = 0;

		if (probabilitySliderRR < 0.0F) probabilitySliderRR = 0;
		if (probabilitySliderRG < 0.0F) probabilitySliderRG = 0;
		if (probabilitySliderRB < 0.0F) probabilitySliderRB = 0;
		if (probabilitySliderRW < 0.0F) probabilitySliderRW = 0;
		if (probabilitySliderRO < 0.0F) probabilitySliderRO = 0;
		if (probabilitySliderRK < 0.0F) probabilitySliderRK = 0;
		if (probabilitySliderRC < 0.0F) probabilitySliderRC = 0;
		if (probabilitySliderRD < 0.0F) probabilitySliderRD = 0;

		if (probabilitySliderGR < 0.0F) probabilitySliderGR = 0;
		if (probabilitySliderGG < 0.0F) probabilitySliderGG = 0;
		if (probabilitySliderGB < 0.0F) probabilitySliderGB = 0;
		if (probabilitySliderGW < 0.0F) probabilitySliderGW = 0;
		if (probabilitySliderGO < 0.0F) probabilitySliderGO = 0;
		if (probabilitySliderGK < 0.0F) probabilitySliderGK = 0;
		if (probabilitySliderGC < 0.0F) probabilitySliderGC = 0;
		if (probabilitySliderGD < 0.0F) probabilitySliderGD = 0;

		if (probabilitySliderBR < 0.0F) probabilitySliderBR = 0;
		if (probabilitySliderBG < 0.0F) probabilitySliderBG = 0;
		if (probabilitySliderBB < 0.0F) probabilitySliderBB = 0;
		if (probabilitySliderBW < 0.0F) probabilitySliderBW = 0;
		if (probabilitySliderBO < 0.0F) probabilitySliderBO = 0;
		if (probabilitySliderBK < 0.0F) probabilitySliderBK = 0;
		if (probabilitySliderBC < 0.0F) probabilitySliderBC = 0;
		if (probabilitySliderBD < 0.0F) probabilitySliderBD = 0;

		if (probabilitySliderWR < 0.0F) probabilitySliderWR = 0;
		if (probabilitySliderWG < 0.0F) probabilitySliderWG = 0;
		if (probabilitySliderWB < 0.0F) probabilitySliderWB = 0;
		if (probabilitySliderWW < 0.0F) probabilitySliderWW = 0;
		if (probabilitySliderWO < 0.0F) probabilitySliderWO = 0;
		if (probabilitySliderWK < 0.0F) probabilitySliderWK = 0;
		if (probabilitySliderWC < 0.0F) probabilitySliderWC = 0;
		if (probabilitySliderWD < 0.0F) probabilitySliderWD = 0;

		if (probabilitySliderOR < 0.0F) probabilitySliderOR = 0;
		if (probabilitySliderOG < 0.0F) probabilitySliderOG = 0;
		if (probabilitySliderOB < 0.0F) probabilitySliderOB = 0;
		if (probabilitySliderOW < 0.0F) probabilitySliderOW = 0;
		if (probabilitySliderOO < 0.0F) probabilitySliderOO = 0;
		if (probabilitySliderOK < 0.0F) probabilitySliderOK = 0;
		if (probabilitySliderOC < 0.0F) probabilitySliderOC = 0;
		if (probabilitySliderOD < 0.0F) probabilitySliderOD = 0;

		if (probabilitySliderKR < 0.0F) probabilitySliderKR = 0;
		if (probabilitySliderKG < 0.0F) probabilitySliderKG = 0;
		if (probabilitySliderKB < 0.0F) probabilitySliderKB = 0;
		if (probabilitySliderKW < 0.0F) probabilitySliderKW = 0;
		if (probabilitySliderKO < 0.0F) probabilitySliderKO = 0;
		if (probabilitySliderKK < 0.0F) probabilitySliderKK = 0;
		if (probabilitySliderKC < 0.0F) probabilitySliderKC = 0;
		if (probabilitySliderKD < 0.0F) probabilitySliderKD = 0;

		if (probabilitySliderCR < 0.0F) probabilitySliderCR = 0;
		if (probabilitySliderCG < 0.0F) probabilitySliderCG = 0;
		if (probabilitySliderCB < 0.0F) probabilitySliderCB = 0;
		if (probabilitySliderCW < 0.0F) probabilitySliderCW = 0;
		if (probabilitySliderCO < 0.0F) probabilitySliderCO = 0;
		if (probabilitySliderCK < 0.0F) probabilitySliderCK = 0;
		if (probabilitySliderCC < 0.0F) probabilitySliderCC = 0;
		if (probabilitySliderCD < 0.0F) probabilitySliderCD = 0;

		if (probabilitySliderDR < 0.0F) probabilitySliderDR = 0;
		if (probabilitySliderDG < 0.0F) probabilitySliderDG = 0;
		if (probabilitySliderDB < 0.0F) probabilitySliderDB = 0;
		if (probabilitySliderDW < 0.0F) probabilitySliderDW = 0;
		if (probabilitySliderDO < 0.0F) probabilitySliderDO = 0;
		if (probabilitySliderDK < 0.0F) probabilitySliderDK = 0;
		if (probabilitySliderDC < 0.0F) probabilitySliderDC = 0;
		if (probabilitySliderDD < 0.0F) probabilitySliderDD = 0;

		if (viscoSlider < 0.0F) viscoSlider = 0;

		if (viscoSliderR < 0.0F) viscoSliderR = 0;
		if (viscoSliderG < 0.0F) viscoSliderG = 0;
		if (viscoSliderB < 0.0F) viscoSliderB = 0;
		if (viscoSliderW < 0.0F) viscoSliderW = 0;
		if (viscoSliderO < 0.0F) viscoSliderO = 0;
		if (viscoSliderK < 0.0F) viscoSliderK = 0;
		if (viscoSliderC < 0.0F) viscoSliderC = 0;
		if (viscoSliderD < 0.0F) viscoSliderD = 0;
		
		//if (numberSliderG < 0.0F) numberSliderG = 0;
		//if (numberSliderR < 0.0F) numberSliderR = 0;
		//if (numberSliderW < 0.0F) numberSliderW = 0;
		//if (numberSliderB < 0.0F) numberSliderB = 0;
		//if (numberSliderO < 0.0F) numberSliderO = 0;
		//if (numberSliderK < 0.0F) numberSliderK = 0;
		//if (numberSliderC < 0.0F) numberSliderC = 0;
		//if (numberSliderD < 0.0F) numberSliderD = 0;
		

		if (vSliderRR > 200.0F) vSliderRR = 200.0F;
		if (vSliderRG > 200.0F) vSliderRG = 200.0F;
		if (vSliderRB > 200.0F) vSliderRB = 200.0F;
		if (vSliderRW > 200.0F) vSliderRW = 200.0F;
		if (vSliderRO > 200.0F) vSliderRO = 200.0F;
		if (vSliderRK > 200.0F) vSliderRK = 200.0F;
		if (vSliderRC > 200.0F) vSliderRC = 200.0F;
		if (vSliderRD > 200.0F) vSliderRD = 200.0F;
		if (vSliderGR > 200.0F) vSliderGR = 200.0F;
		if (vSliderGG > 200.0F) vSliderGG = 200.0F;
		if (vSliderGB > 200.0F) vSliderGB = 200.0F;
		if (vSliderGW > 200.0F) vSliderGW = 200.0F;
		if (vSliderGO > 200.0F) vSliderGO = 200.0F;
		if (vSliderGK > 200.0F) vSliderGK = 200.0F;
		if (vSliderGC > 200.0F) vSliderGC = 200.0F;
		if (vSliderGD > 200.0F) vSliderGD = 200.0F;
		if (vSliderBR > 200.0F) vSliderBR = 200.0F;
		if (vSliderBG > 200.0F) vSliderBG = 200.0F;
		if (vSliderBB > 200.0F) vSliderBB = 200.0F;
		if (vSliderBW > 200.0F) vSliderBW = 200.0F;
		if (vSliderBO > 200.0F) vSliderBO = 200.0F;
		if (vSliderBK > 200.0F) vSliderBK = 200.0F;
		if (vSliderBC > 200.0F) vSliderBC = 200.0F;
		if (vSliderBD > 200.0F) vSliderBD = 200.0F;
		if (vSliderWR > 200.0F) vSliderWR = 200.0F;
		if (vSliderWG > 200.0F) vSliderWG = 200.0F;
		if (vSliderWB > 200.0F) vSliderWB = 200.0F;
		if (vSliderWW > 200.0F) vSliderWW = 200.0F;
		if (vSliderWO > 200.0F) vSliderWO = 200.0F;
		if (vSliderWK > 200.0F) vSliderWK = 200.0F;
		if (vSliderWC > 200.0F) vSliderWC = 200.0F;
		if (vSliderWD > 200.0F) vSliderWD = 200.0F;
		if (vSliderOR > 200.0F) vSliderOR = 200.0F;
		if (vSliderOG > 200.0F) vSliderOG = 200.0F;
		if (vSliderOB > 200.0F) vSliderOB = 200.0F;
		if (vSliderOW > 200.0F) vSliderOW = 200.0F;
		if (vSliderOO > 200.0F) vSliderOO = 200.0F;
		if (vSliderOK > 200.0F) vSliderOK = 200.0F;
		if (vSliderOC > 200.0F) vSliderOC = 200.0F;
		if (vSliderOD > 200.0F) vSliderOD = 200.0F;
		if (vSliderKR > 200.0F) vSliderKR = 200.0F;
		if (vSliderKG > 200.0F) vSliderKG = 200.0F;
		if (vSliderKB > 200.0F) vSliderKB = 200.0F;
		if (vSliderKW > 200.0F) vSliderKW = 200.0F;
		if (vSliderKO > 200.0F) vSliderKO = 200.0F;
		if (vSliderKK > 200.0F) vSliderKK = 200.0F;
		if (vSliderKC > 200.0F) vSliderKC = 200.0F;
		if (vSliderKD > 200.0F) vSliderKD = 200.0F;
		if (vSliderCR > 200.0F) vSliderCR = 200.0F;
		if (vSliderCG > 200.0F) vSliderCG = 200.0F;
		if (vSliderCB > 200.0F) vSliderCB = 200.0F;
		if (vSliderCW > 200.0F) vSliderCW = 200.0F;
		if (vSliderCO > 200.0F) vSliderCO = 200.0F;
		if (vSliderCK > 200.0F) vSliderCK = 200.0F;
		if (vSliderCC > 200.0F) vSliderCC = 200.0F;
		if (vSliderCD > 200.0F) vSliderCD = 200.0F;
		if (vSliderDR > 200.0F) vSliderDR = 200.0F;
		if (vSliderDG > 200.0F) vSliderDG = 200.0F;
		if (vSliderDB > 200.0F) vSliderDB = 200.0F;
		if (vSliderDW > 200.0F) vSliderDW = 200.0F;
		if (vSliderDO > 200.0F) vSliderDO = 200.0F;
		if (vSliderDK > 200.0F) vSliderDK = 200.0F;
		if (vSliderDC > 200.0F) vSliderDC = 200.0F;
		if (vSliderDD > 200.0F) vSliderDD = 200.0F;

		if (probabilitySlider > 100.0F) probabilitySlider = 100.0F;

		if (probabilitySliderRR > 100.0F) probabilitySliderRR = 100.0F;
		if (probabilitySliderRG > 100.0F) probabilitySliderRG = 100.0F;
		if (probabilitySliderRB > 100.0F) probabilitySliderRB = 100.0F;
		if (probabilitySliderRW > 100.0F) probabilitySliderRW = 100.0F;
		if (probabilitySliderRO > 100.0F) probabilitySliderRO = 100.0F;
		if (probabilitySliderRK > 100.0F) probabilitySliderRK = 100.0F;
		if (probabilitySliderRC > 100.0F) probabilitySliderRC = 100.0F;
		if (probabilitySliderRD > 100.0F) probabilitySliderRD = 100.0F;

		if (probabilitySliderGR > 100.0F) probabilitySliderGR = 100.0F;
		if (probabilitySliderGG > 100.0F) probabilitySliderGG = 100.0F;
		if (probabilitySliderGB > 100.0F) probabilitySliderGB = 100.0F;
		if (probabilitySliderGW > 100.0F) probabilitySliderGW = 100.0F;
		if (probabilitySliderGO > 100.0F) probabilitySliderGO = 100.0F;
		if (probabilitySliderGK > 100.0F) probabilitySliderGK = 100.0F;
		if (probabilitySliderGC > 100.0F) probabilitySliderGC = 100.0F;
		if (probabilitySliderGD > 100.0F) probabilitySliderGD = 100.0F;

		if (probabilitySliderBR > 100.0F) probabilitySliderBR = 100.0F;
		if (probabilitySliderBG > 100.0F) probabilitySliderBG = 100.0F;
		if (probabilitySliderBB > 100.0F) probabilitySliderBB = 100.0F;
		if (probabilitySliderBW > 100.0F) probabilitySliderBW = 100.0F;
		if (probabilitySliderBO > 100.0F) probabilitySliderBO = 100.0F;
		if (probabilitySliderBK > 100.0F) probabilitySliderBK = 100.0F;
		if (probabilitySliderBC > 100.0F) probabilitySliderBC = 100.0F;
		if (probabilitySliderBD > 100.0F) probabilitySliderBD = 100.0F;

		if (probabilitySliderWR > 100.0F) probabilitySliderWR = 100.0F;
		if (probabilitySliderWG > 100.0F) probabilitySliderWG = 100.0F;
		if (probabilitySliderWB > 100.0F) probabilitySliderWB = 100.0F;
		if (probabilitySliderWW > 100.0F) probabilitySliderWW = 100.0F;
		if (probabilitySliderWO > 100.0F) probabilitySliderWO = 100.0F;
		if (probabilitySliderWK > 100.0F) probabilitySliderWK = 100.0F;
		if (probabilitySliderWC > 100.0F) probabilitySliderWC = 100.0F;
		if (probabilitySliderWD > 100.0F) probabilitySliderWD = 100.0F;

		if (probabilitySliderOR > 100.0F) probabilitySliderOR = 100.0F;
		if (probabilitySliderOG > 100.0F) probabilitySliderOG = 100.0F;
		if (probabilitySliderOB > 100.0F) probabilitySliderOB = 100.0F;
		if (probabilitySliderOW > 100.0F) probabilitySliderOW = 100.0F;
		if (probabilitySliderOO > 100.0F) probabilitySliderOO = 100.0F;
		if (probabilitySliderOK > 100.0F) probabilitySliderOK = 100.0F;
		if (probabilitySliderOC > 100.0F) probabilitySliderOC = 100.0F;
		if (probabilitySliderOD > 100.0F) probabilitySliderOD = 100.0F;

		if (probabilitySliderKR > 100.0F) probabilitySliderKR = 100.0F;
		if (probabilitySliderKG > 100.0F) probabilitySliderKG = 100.0F;
		if (probabilitySliderKB > 100.0F) probabilitySliderKB = 100.0F;
		if (probabilitySliderKW > 100.0F) probabilitySliderKW = 100.0F;
		if (probabilitySliderKO > 100.0F) probabilitySliderKO = 100.0F;
		if (probabilitySliderKK > 100.0F) probabilitySliderKK = 100.0F;
		if (probabilitySliderKC > 100.0F) probabilitySliderKC = 100.0F;
		if (probabilitySliderKD > 100.0F) probabilitySliderKD = 100.0F;

		if (probabilitySliderCR > 100.0F) probabilitySliderCR = 100.0F;
		if (probabilitySliderCG > 100.0F) probabilitySliderCG = 100.0F;
		if (probabilitySliderCB > 100.0F) probabilitySliderCB = 100.0F;
		if (probabilitySliderCW > 100.0F) probabilitySliderCW = 100.0F;
		if (probabilitySliderCO > 100.0F) probabilitySliderCO = 100.0F;
		if (probabilitySliderCK > 100.0F) probabilitySliderCK = 100.0F;
		if (probabilitySliderCC > 100.0F) probabilitySliderCC = 100.0F;
		if (probabilitySliderCD > 100.0F) probabilitySliderCD = 100.0F;

		if (probabilitySliderDR > 100.0F) probabilitySliderDR = 100.0F;
		if (probabilitySliderDG > 100.0F) probabilitySliderDG = 100.0F;
		if (probabilitySliderDB > 100.0F) probabilitySliderDB = 100.0F;
		if (probabilitySliderDW > 100.0F) probabilitySliderDW = 100.0F;
		if (probabilitySliderDO > 100.0F) probabilitySliderDO = 100.0F;
		if (probabilitySliderDK > 100.0F) probabilitySliderDK = 100.0F;
		if (probabilitySliderDC > 100.0F) probabilitySliderDC = 100.0F;
		if (probabilitySliderDD > 100.0F) probabilitySliderDD = 100.0F;

		if (viscoSlider > 1.0F) viscoSlider = 1.0F;

		if (viscoSliderR > 1.0F) viscoSliderR = 1.0F;
		if (viscoSliderG > 1.0F) viscoSliderG = 1.0F;
		if (viscoSliderB > 1.0F) viscoSliderB = 1.0F;
		if (viscoSliderW > 1.0F) viscoSliderW = 1.0F;
		if (viscoSliderO > 1.0F) viscoSliderO = 1.0F;
		if (viscoSliderK > 1.0F) viscoSliderK = 1.0F;
		if (viscoSliderC > 1.0F) viscoSliderC = 1.0F;
		if (viscoSliderD > 1.0F) viscoSliderD = 1.0F;

		//if (numberSliderG > 2000.0F) numberSliderG = 2000.0F;
		//if (numberSliderR > 2000.0F) numberSliderR = 2000.0F;
		//if (numberSliderW > 2000.0F) numberSliderW = 2000.0F;
		//if (numberSliderB > 2000.0F) numberSliderB = 2000.0F;
		//if (numberSliderO > 2000.0F) numberSliderO = 2000.0F;
		//if (numberSliderK > 2000.0F) numberSliderK = 2000.0F;
		//if (numberSliderC > 2000.0F) numberSliderC = 2000.0F;
		//if (numberSliderD > 2000.0F) numberSliderD = 2000.0F;
		
	}

	if (numberSliderR > 0)
	{
		interaction(&red, &red, powerSliderRR, vSliderRR, viscosityR, probabilityRR);
		if (numberSliderG > 0) interaction(&red, &green, powerSliderRG, vSliderRG, viscosityR, probabilityRG);
		if (numberSliderB > 0) interaction(&red, &blue, powerSliderRB, vSliderRB, viscosityR, probabilityRB);
		if (numberSliderW > 0) interaction(&red, &white, powerSliderRW, vSliderRW, viscosityR, probabilityRW);
		if (numberSliderO > 0) interaction(&red, &orange, powerSliderRO, vSliderRO, viscosityR, probabilityRO);
		if (numberSliderK > 0) interaction(&red, &khaki, powerSliderRK, vSliderRK, viscosityR, probabilityRK);
		if (numberSliderC > 0) interaction(&red, &crimson, powerSliderRC, vSliderRC, viscosityR, probabilityRC);
		if (numberSliderD > 0) interaction(&red, &dark, powerSliderRD, vSliderRD, viscosityR, probabilityRD);
	}

	if (numberSliderG > 0)
	{
		if (numberSliderR > 0) interaction(&green, &red, powerSliderGR, vSliderGR, viscosityG, probabilityGR);
		interaction(&green, &green, powerSliderGG, vSliderGG, viscosityG, probabilityGG);
		if (numberSliderB > 0) interaction(&green, &blue, powerSliderGB, vSliderGB, viscosityG, probabilityGB);
		if (numberSliderW > 0) interaction(&green, &white, powerSliderGW, vSliderGW, viscosityG, probabilityGW);
		if (numberSliderO > 0) interaction(&green, &orange, powerSliderGO, vSliderGO, viscosityG, probabilityGO);
		if (numberSliderK > 0) interaction(&green, &khaki, powerSliderGK, vSliderGK, viscosityG, probabilityGK);
		if (numberSliderC > 0) interaction(&green, &crimson, powerSliderGC, vSliderGC, viscosityG, probabilityGC);
		if (numberSliderD > 0) interaction(&green, &dark, powerSliderGD, vSliderGD, viscosityG, probabilityGD);
	}

	if (numberSliderB > 0)
	{
		if (numberSliderR > 0) interaction(&blue, &red, powerSliderBR, vSliderBR, viscosityB, probabilityBR);
		if (numberSliderG > 0) interaction(&blue, &green, powerSliderBG, vSliderBG, viscosityB, probabilityBG);
		interaction(&blue, &blue, powerSliderBB, vSliderBB, viscosityB, probabilityBB);
		if (numberSliderW > 0) interaction(&blue, &white, powerSliderBW, vSliderBW, viscosityB, probabilityBW);
		if (numberSliderO > 0) interaction(&blue, &orange, powerSliderBO, vSliderBO, viscosityB, probabilityBO);
		if (numberSliderK > 0) interaction(&blue, &khaki, powerSliderBK, vSliderBK, viscosityB, probabilityBK);
		if (numberSliderC > 0) interaction(&blue, &crimson, powerSliderBC, vSliderBC, viscosityB, probabilityBC);
		if (numberSliderD > 0) interaction(&blue, &dark, powerSliderBD, vSliderBD, viscosityB, probabilityBD);
	}

	if (numberSliderW > 0)
	{
		if (numberSliderR > 0) interaction(&white, &red, powerSliderWR, vSliderWR, viscosityW, probabilityWR);
		if (numberSliderG > 0) interaction(&white, &green, powerSliderWG, vSliderWG, viscosityW, probabilityWG);
		if (numberSliderB > 0) interaction(&white, &blue, powerSliderWB, vSliderWB, viscosityW, probabilityWB);
		interaction(&white, &white, powerSliderWW, vSliderWW, viscosityW, probabilityWW);
		if (numberSliderO > 0) interaction(&white, &orange, powerSliderWO, vSliderWO, viscosityW, probabilityWO);
		if (numberSliderK > 0) interaction(&white, &khaki, powerSliderWK, vSliderWK, viscosityW, probabilityWK);
		if (numberSliderC > 0) interaction(&white, &crimson, powerSliderWC, vSliderWC, viscosityW, probabilityWC);
		if (numberSliderD > 0) interaction(&white, &dark, powerSliderWD, vSliderWD, viscosityW, probabilityWD);
	}

	if (numberSliderO > 0)
	{
		if (numberSliderR > 0) interaction(&orange, &red, powerSliderOR, vSliderOR, viscosityO, probabilityOR);
		if (numberSliderG > 0) interaction(&orange, &green, powerSliderOG, vSliderOG, viscosityO, probabilityOR);
		if (numberSliderB > 0) interaction(&orange, &blue, powerSliderOB, vSliderOB, viscosityO, probabilityOB);
		if (numberSliderW > 0) interaction(&orange, &white, powerSliderOW, vSliderOW, viscosityO, probabilityOW);
		interaction(&orange, &orange, powerSliderOO, vSliderOO, viscosityO, probabilityOO);
		if (numberSliderK > 0) interaction(&orange, &khaki, powerSliderOK, vSliderOK, viscosityO, probabilityOK);
		if (numberSliderC > 0) interaction(&orange, &crimson, powerSliderOC, vSliderOC, viscosityO, probabilityOC);
		if (numberSliderD > 0) interaction(&orange, &dark, powerSliderOD, vSliderOD, viscosityO, probabilityOD);
	}

	if (numberSliderK > 0)
	{
		if (numberSliderR > 0) interaction(&khaki, &red, powerSliderKR, vSliderKR, viscosityK, probabilityKR);
		if (numberSliderG > 0) interaction(&khaki, &green, powerSliderKG, vSliderKG, viscosityK, probabilityKG);
		if (numberSliderB > 0) interaction(&khaki, &blue, powerSliderKB, vSliderKB, viscosityK, probabilityKB);
		if (numberSliderW > 0) interaction(&khaki, &white, powerSliderKW, vSliderKW, viscosityK, probabilityKW);
		if (numberSliderO > 0) interaction(&khaki, &orange, powerSliderKO, vSliderKO, viscosityK, probabilityKO);
		interaction(&khaki, &khaki, powerSliderKK, vSliderKK, viscosityK, probabilityKK);
		if (numberSliderC > 0) interaction(&khaki, &crimson, powerSliderKC, vSliderKC, viscosityK, probabilityKC);
		if (numberSliderD > 0) interaction(&khaki, &dark, powerSliderKD, vSliderKD, viscosityK, probabilityKD);
	}

	if (numberSliderC > 0)
	{
		if (numberSliderR > 0) interaction(&crimson, &red, powerSliderCR, vSliderCR, viscosityC, probabilityCR);
		if (numberSliderG > 0) interaction(&crimson, &green, powerSliderCG, vSliderCG, viscosityC, probabilityCG);
		if (numberSliderB > 0) interaction(&crimson, &blue, powerSliderCB, vSliderCB, viscosityC, probabilityCB);
		if (numberSliderW > 0) interaction(&crimson, &white, powerSliderCW, vSliderCW, viscosityC, probabilityCW);
		if (numberSliderO > 0) interaction(&crimson, &orange, powerSliderCO, vSliderCO, viscosityC, probabilityCO);
		if (numberSliderK > 0) interaction(&crimson, &khaki, powerSliderCK, vSliderCK, viscosityC, probabilityCK);
		interaction(&crimson, &crimson, powerSliderCC, vSliderCC, viscosityC, probabilityCC);
		if (numberSliderD > 0) interaction(&crimson, &dark, powerSliderCD, vSliderCD, viscosityC, probabilityCD);
	}

	if (numberSliderD > 0)
	{
		if (numberSliderR > 0) interaction(&dark, &red, powerSliderDR, vSliderDR, viscosityD, probabilityDR);
		if (numberSliderG > 0) interaction(&dark, &green, powerSliderDG, vSliderDG, viscosityD, probabilityDG);
		if (numberSliderB > 0) interaction(&dark, &blue, powerSliderDB, vSliderDB, viscosityD, probabilityDB);
		if (numberSliderW > 0) interaction(&dark, &white, powerSliderDW, vSliderDW, viscosityD, probabilityDW);
		if (numberSliderO > 0) interaction(&dark, &orange, powerSliderDO, vSliderDO, viscosityD, probabilityDO);
		if (numberSliderK > 0) interaction(&dark, &khaki, powerSliderDK, vSliderDK, viscosityD, probabilityDK);
		if (numberSliderC > 0) interaction(&dark, &crimson, powerSliderDC, vSliderDC, viscosityD, probabilityDC);
		interaction(&dark, &dark, powerSliderDD, vSliderDD, viscosityD, probabilityDD);
	}

	if (save) { saveSettings(); }
	if (load) { loadSettings(); }
	physic_delta = clock() - physic_begin;
}

//--------------------------------------------------------------
void ofApp::draw()
{
	if (motionBlurToggle)
	{
		ofSetColor(0, 0, 0, 64);
		ofDrawRectangle(0, 0, boundWidth, boundHeight);
	}
	else
	{
		ofClear(0);
	}

	//fps counter
	cntFps++;
	now = clock();
	delta = now - lastTime;

	//Time step
	if (delta >= 1000)
	{
		lastTime = now;
		fps.setup("FPS", to_string(static_cast<int>((1000 / static_cast<float>(delta)) * cntFps)));
		physicLabel.setup("physics (ms)", to_string(physic_delta));

		cntFps = 0;
	}

	//Check for GUI interaction
	if (resetButton) { restart(); }
	if (randomChoice)
	{
		random();
		restart();
	}
	if (randomCount)
	{
		monads();
		restart();
	}
	if (numberSliderW > 0) { Draw(&white); }
	if (numberSliderR > 0) { Draw(&red); }
	if (numberSliderG > 0) { Draw(&green); }
	if (numberSliderB > 0) { Draw(&blue); }
	if (numberSliderO > 0) { Draw(&orange); }
	if (numberSliderK > 0) { Draw(&khaki); }
	if (numberSliderC > 0) { Draw(&crimson); }
	if (numberSliderD > 0) { Draw(&dark); }
	if (numberSliderW < 0.0F) numberSliderW = 0;
	if (numberSliderR < 0.0F) numberSliderW = 0;
	if (numberSliderG < 0.0F) numberSliderW = 0;
	if (numberSliderB < 0.0F) numberSliderW = 0;
	if (numberSliderO < 0.0F) numberSliderW = 0;
	if (numberSliderK < 0.0F) numberSliderW = 0;
	if (numberSliderC < 0.0F) numberSliderW = 0;
	if (numberSliderD < 0.0F) numberSliderW = 0;

	//Draw GUI
	if (modelToggle == true)
	{
		ofSetColor(0, 0, 0);
		ofDrawCircle(xshift, yshift, 150);

		ofSetLineWidth(5);
		ofSetColor(150.0F - powerSliderGR, 150.0F + powerSliderGR, 150);
		ofDrawLine(p1x, p1y - 10, p2x, p2y - 10);
		ofSetColor(150.0F - powerSliderRG, 150.0F + powerSliderRG, 150);
		ofDrawLine(p1x, p1y + 10, p2x, p2y + 10);
		ofSetColor(150.0F - powerSliderGW, 150.0F + powerSliderGW, 150);
		ofDrawLine(p3x, p3y - 10, p1x, p1y - 10);
		ofSetColor(150.0F - powerSliderWG, 150.0F + powerSliderWG, 150);
		ofDrawLine(p3x, p3y + 10, p1x, p1y + 10);

		ofSetColor(150.0F - powerSliderGB, 150.0F + powerSliderGB, 150);
		ofDrawLine(p4x - 10, p4y, p1x - 10, p1y);
		ofSetColor(150.0F - powerSliderBG, 150.0F + powerSliderBG, 150);
		ofDrawLine(p4x + 10, p4y, p1x + 10, p1y);

		ofSetColor(150.0F - powerSliderRW, 150.0F + powerSliderRW, 150);
		ofDrawLine(p2x - 10, p2y, p3x - 10, p3y);
		ofSetColor(150.0F - powerSliderWR, 150.0F + powerSliderWR, 150);
		ofDrawLine(p2x + 10, p2y, p3x + 10, p3y);

		ofSetColor(150.0F - powerSliderRB, 150.0F + powerSliderRB, 150);
		ofDrawLine(p2x, p2y - 10, p4x, p4y - 10);
		ofSetColor(150.0F - powerSliderBR, 150.0F + powerSliderBR, 150);
		ofDrawLine(p2x, p2y + 10, p4x, p4y + 10);

		ofSetColor(150.0F - powerSliderWB, 150.0F + powerSliderWB, 150);
		ofDrawLine(p3x, p3y - 10, p4x, p4y - 10);
		ofSetColor(150.0F - powerSliderBW, 150.0F + powerSliderBW, 150);
		ofDrawLine(p3x, p3y + 10, p4x, p4y + 10);

		ofNoFill();
		ofSetColor(150.0F - powerSliderGG, 150.0F + powerSliderGG, 150);
		ofDrawCircle(p1x - 20, p1y - 20, rr + 20);
		ofSetColor(150.0F - powerSliderRR, 150.0F + powerSliderRR, 150);
		ofDrawCircle(p2x + 20, p2y - 20, rr + 20);
		ofSetColor(150.0F - powerSliderWW, 150.0F + powerSliderWW, 150);
		ofDrawCircle(p3x + 20, p3y + 20, rr + 20);
		ofSetColor(150.0F - powerSliderBB, 150.0F + powerSliderBB, 150);
		ofDrawCircle(p4x - 20, p4y + 20, rr + 20);

		ofFill();
		ofSetColor(100, 250, 10);
		ofDrawCircle(p1x, p1y, rr);
		ofSetColor(250, 10, 100);
		ofDrawCircle(p2x, p2y, rr);
		ofSetColor(250, 250, 250);
		ofDrawCircle(p3x, p3y, rr);
		ofSetColor(100, 100, 250);
		ofDrawCircle(p4x, p4y, rr);
	}
	gui.draw();
}

void ofApp::keyPressed(int key)
{
	if (key == ' ')
	{
		random();
		restart();
	}
	if (key == 'q')
	{
		monads();
		restart();
	}
}
