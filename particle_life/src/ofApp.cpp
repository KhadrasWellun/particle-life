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
std::vector<point> yellow;
std::vector<point> pink;
std::vector<point> magenta;
std::vector<point> aqua;
std::vector<point> teal;
std::vector<point> silver;
std::vector<point> firebrick;


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
	if (numberSliderD > 0) { dark = CreatePoints(numberSliderD, 10, 10, 10); }
	if (numberSliderY > 0) { yellow = CreatePoints(numberSliderY, 255, 255, 10); }
	if (numberSliderP > 0) { pink = CreatePoints(numberSliderP, 255, 192, 203); }
	if (numberSliderM > 0) { magenta = CreatePoints(numberSliderM, 255, 10, 255); }
	if (numberSliderA > 0) { aqua = CreatePoints(numberSliderA, 10, 255, 255); }
	if (numberSliderT > 0) { teal = CreatePoints(numberSliderT, 10, 128, 128); }
	if (numberSliderS > 0) { silver = CreatePoints(numberSliderS, 192, 192, 192); }
	if (numberSliderF > 0) { firebrick = CreatePoints(numberSliderF, 178, 34, 34); }
}


/**
 * @brief Generate initial simulation parameters
 */
void ofApp::random()
{
	numberSliderG = RandomInt(200, 1000);
	numberSliderR = RandomInt(200, 1000);
	numberSliderW = RandomInt(200, 1000);
	numberSliderB = RandomInt(200, 1000);
	numberSliderO = RandomInt(200, 1000);
	numberSliderK = RandomInt(200, 1000);
	numberSliderC = RandomInt(200, 1000);
	numberSliderD = RandomInt(200, 1000);
	numberSliderY = RandomInt(200, 1000);
	numberSliderP = RandomInt(200, 1000);
	numberSliderM = RandomInt(200, 1000);
	numberSliderA = RandomInt(200, 1000);
	numberSliderT = RandomInt(200, 1000);
	numberSliderS = RandomInt(200, 1000);
	numberSliderF = RandomInt(200, 1000);

	evoProbSlider = RandomFloat(0.1, 1.5);
	evoAmountSlider = RandomFloat(0.1, 3);

	viscositySlider = RandomFloat(0.0, 1.0);

	viscositySliderRR = RandomFloat(0.0, 1.0);
	viscositySliderRG = RandomFloat(0.0, 1.0);
	viscositySliderRW = RandomFloat(0.0, 1.0);
	viscositySliderRB = RandomFloat(0.0, 1.0);
	viscositySliderRO = RandomFloat(0.0, 1.0);
	viscositySliderRK = RandomFloat(0.0, 1.0);
	viscositySliderRC = RandomFloat(0.0, 1.0);
	viscositySliderRD = RandomFloat(0.0, 1.0);
	viscositySliderRY = RandomFloat(0.0, 1.0);
	viscositySliderRP = RandomFloat(0.0, 1.0);
	viscositySliderRM = RandomFloat(0.0, 1.0);
	viscositySliderRA = RandomFloat(0.0, 1.0);
	viscositySliderRT = RandomFloat(0.0, 1.0);
	viscositySliderRS = RandomFloat(0.0, 1.0);
	viscositySliderRF = RandomFloat(0.0, 1.0);

	viscositySliderGR = RandomFloat(0.0, 1.0);
	viscositySliderGG = RandomFloat(0.0, 1.0);
	viscositySliderGW = RandomFloat(0.0, 1.0);
	viscositySliderGB = RandomFloat(0.0, 1.0);
	viscositySliderGO = RandomFloat(0.0, 1.0);
	viscositySliderGK = RandomFloat(0.0, 1.0);
	viscositySliderGC = RandomFloat(0.0, 1.0);
	viscositySliderGD = RandomFloat(0.0, 1.0);
	viscositySliderGY = RandomFloat(0.0, 1.0);
	viscositySliderGP = RandomFloat(0.0, 1.0);
	viscositySliderGM = RandomFloat(0.0, 1.0);
	viscositySliderGA = RandomFloat(0.0, 1.0);
	viscositySliderGT = RandomFloat(0.0, 1.0);
	viscositySliderGS = RandomFloat(0.0, 1.0);
	viscositySliderGF = RandomFloat(0.0, 1.0);

	viscositySliderBR = RandomFloat(0.0, 1.0);
	viscositySliderBG = RandomFloat(0.0, 1.0);
	viscositySliderBW = RandomFloat(0.0, 1.0);
	viscositySliderBB = RandomFloat(0.0, 1.0);
	viscositySliderBO = RandomFloat(0.0, 1.0);
	viscositySliderBK = RandomFloat(0.0, 1.0);
	viscositySliderBC = RandomFloat(0.0, 1.0);
	viscositySliderBD = RandomFloat(0.0, 1.0);
	viscositySliderBY = RandomFloat(0.0, 1.0);
	viscositySliderBP = RandomFloat(0.0, 1.0);
	viscositySliderBM = RandomFloat(0.0, 1.0);
	viscositySliderBA = RandomFloat(0.0, 1.0);
	viscositySliderBT = RandomFloat(0.0, 1.0);
	viscositySliderBS = RandomFloat(0.0, 1.0);
	viscositySliderBF = RandomFloat(0.0, 1.0);

	viscositySliderWR = RandomFloat(0.0, 1.0);
	viscositySliderWG = RandomFloat(0.0, 1.0);
	viscositySliderWW = RandomFloat(0.0, 1.0);
	viscositySliderWB = RandomFloat(0.0, 1.0);
	viscositySliderWO = RandomFloat(0.0, 1.0);
	viscositySliderWK = RandomFloat(0.0, 1.0);
	viscositySliderWC = RandomFloat(0.0, 1.0);
	viscositySliderWD = RandomFloat(0.0, 1.0);
	viscositySliderWY = RandomFloat(0.0, 1.0);
	viscositySliderWP = RandomFloat(0.0, 1.0);
	viscositySliderWM = RandomFloat(0.0, 1.0);
	viscositySliderWA = RandomFloat(0.0, 1.0);
	viscositySliderWT = RandomFloat(0.0, 1.0);
	viscositySliderWS = RandomFloat(0.0, 1.0);
	viscositySliderWF = RandomFloat(0.0, 1.0);

	viscositySliderOR = RandomFloat(0.0, 1.0);
	viscositySliderOG = RandomFloat(0.0, 1.0);
	viscositySliderOW = RandomFloat(0.0, 1.0);
	viscositySliderOB = RandomFloat(0.0, 1.0);
	viscositySliderOO = RandomFloat(0.0, 1.0);
	viscositySliderOK = RandomFloat(0.0, 1.0);
	viscositySliderOC = RandomFloat(0.0, 1.0);
	viscositySliderOD = RandomFloat(0.0, 1.0);
	viscositySliderOY = RandomFloat(0.0, 1.0);
	viscositySliderOP = RandomFloat(0.0, 1.0);
	viscositySliderOM = RandomFloat(0.0, 1.0);
	viscositySliderOA = RandomFloat(0.0, 1.0);
	viscositySliderOT = RandomFloat(0.0, 1.0);
	viscositySliderOS = RandomFloat(0.0, 1.0);
	viscositySliderOF = RandomFloat(0.0, 1.0);

	viscositySliderKR = RandomFloat(0.0, 1.0);
	viscositySliderKG = RandomFloat(0.0, 1.0);
	viscositySliderKW = RandomFloat(0.0, 1.0);
	viscositySliderKB = RandomFloat(0.0, 1.0);
	viscositySliderKO = RandomFloat(0.0, 1.0);
	viscositySliderKK = RandomFloat(0.0, 1.0);
	viscositySliderKC = RandomFloat(0.0, 1.0);
	viscositySliderKD = RandomFloat(0.0, 1.0);
	viscositySliderKY = RandomFloat(0.0, 1.0);
	viscositySliderKP = RandomFloat(0.0, 1.0);
	viscositySliderKM = RandomFloat(0.0, 1.0);
	viscositySliderKA = RandomFloat(0.0, 1.0);
	viscositySliderKT = RandomFloat(0.0, 1.0);
	viscositySliderKS = RandomFloat(0.0, 1.0);
	viscositySliderKF = RandomFloat(0.0, 1.0);

	viscositySliderCR = RandomFloat(0.0, 1.0);
	viscositySliderCG = RandomFloat(0.0, 1.0);
	viscositySliderCW = RandomFloat(0.0, 1.0);
	viscositySliderCB = RandomFloat(0.0, 1.0);
	viscositySliderCO = RandomFloat(0.0, 1.0);
	viscositySliderCK = RandomFloat(0.0, 1.0);
	viscositySliderCC = RandomFloat(0.0, 1.0);
	viscositySliderCD = RandomFloat(0.0, 1.0);
	viscositySliderCY = RandomFloat(0.0, 1.0);
	viscositySliderCP = RandomFloat(0.0, 1.0);
	viscositySliderCM = RandomFloat(0.0, 1.0);
	viscositySliderCA = RandomFloat(0.0, 1.0);
	viscositySliderCT = RandomFloat(0.0, 1.0);
	viscositySliderCS = RandomFloat(0.0, 1.0);
	viscositySliderCF = RandomFloat(0.0, 1.0);

	viscositySliderDR = RandomFloat(0.0, 1.0);
	viscositySliderDG = RandomFloat(0.0, 1.0);
	viscositySliderDW = RandomFloat(0.0, 1.0);
	viscositySliderDB = RandomFloat(0.0, 1.0);
	viscositySliderDO = RandomFloat(0.0, 1.0);
	viscositySliderDK = RandomFloat(0.0, 1.0);
	viscositySliderDC = RandomFloat(0.0, 1.0);
	viscositySliderDD = RandomFloat(0.0, 1.0);
	viscositySliderDY = RandomFloat(0.0, 1.0);
	viscositySliderDP = RandomFloat(0.0, 1.0);
	viscositySliderDM = RandomFloat(0.0, 1.0);
	viscositySliderDA = RandomFloat(0.0, 1.0);
	viscositySliderDT = RandomFloat(0.0, 1.0);
	viscositySliderDS = RandomFloat(0.0, 1.0);
	viscositySliderDF = RandomFloat(0.0, 1.0);

	viscositySliderYR = RandomFloat(0.0, 1.0);
	viscositySliderYG = RandomFloat(0.0, 1.0);
	viscositySliderYW = RandomFloat(0.0, 1.0);
	viscositySliderYB = RandomFloat(0.0, 1.0);
	viscositySliderYO = RandomFloat(0.0, 1.0);
	viscositySliderYK = RandomFloat(0.0, 1.0);
	viscositySliderYC = RandomFloat(0.0, 1.0);
	viscositySliderYD = RandomFloat(0.0, 1.0);
	viscositySliderYY = RandomFloat(0.0, 1.0);
	viscositySliderYP = RandomFloat(0.0, 1.0);
	viscositySliderYM = RandomFloat(0.0, 1.0);
	viscositySliderYA = RandomFloat(0.0, 1.0);
	viscositySliderYT = RandomFloat(0.0, 1.0);
	viscositySliderYS = RandomFloat(0.0, 1.0);
	viscositySliderYF = RandomFloat(0.0, 1.0);

	viscositySliderPR = RandomFloat(0.0, 1.0);
	viscositySliderPG = RandomFloat(0.0, 1.0);
	viscositySliderPW = RandomFloat(0.0, 1.0);
	viscositySliderPB = RandomFloat(0.0, 1.0);
	viscositySliderPO = RandomFloat(0.0, 1.0);
	viscositySliderPK = RandomFloat(0.0, 1.0);
	viscositySliderPC = RandomFloat(0.0, 1.0);
	viscositySliderPD = RandomFloat(0.0, 1.0);
	viscositySliderPY = RandomFloat(0.0, 1.0);
	viscositySliderPP = RandomFloat(0.0, 1.0);
	viscositySliderPM = RandomFloat(0.0, 1.0);
	viscositySliderPA = RandomFloat(0.0, 1.0);
	viscositySliderPT = RandomFloat(0.0, 1.0);
	viscositySliderPS = RandomFloat(0.0, 1.0);
	viscositySliderPF = RandomFloat(0.0, 1.0);

	viscositySliderMR = RandomFloat(0.0, 1.0);
	viscositySliderMG = RandomFloat(0.0, 1.0);
	viscositySliderMW = RandomFloat(0.0, 1.0);
	viscositySliderMB = RandomFloat(0.0, 1.0);
	viscositySliderMO = RandomFloat(0.0, 1.0);
	viscositySliderMK = RandomFloat(0.0, 1.0);
	viscositySliderMC = RandomFloat(0.0, 1.0);
	viscositySliderMD = RandomFloat(0.0, 1.0);
	viscositySliderMY = RandomFloat(0.0, 1.0);
	viscositySliderMP = RandomFloat(0.0, 1.0);
	viscositySliderMM = RandomFloat(0.0, 1.0);
	viscositySliderMA = RandomFloat(0.0, 1.0);
	viscositySliderMT = RandomFloat(0.0, 1.0);
	viscositySliderMS = RandomFloat(0.0, 1.0);
	viscositySliderMF = RandomFloat(0.0, 1.0);

	viscositySliderAR = RandomFloat(0.0, 1.0);
	viscositySliderAG = RandomFloat(0.0, 1.0);
	viscositySliderAW = RandomFloat(0.0, 1.0);
	viscositySliderAB = RandomFloat(0.0, 1.0);
	viscositySliderAO = RandomFloat(0.0, 1.0);
	viscositySliderAK = RandomFloat(0.0, 1.0);
	viscositySliderAC = RandomFloat(0.0, 1.0);
	viscositySliderAD = RandomFloat(0.0, 1.0);
	viscositySliderAY = RandomFloat(0.0, 1.0);
	viscositySliderAP = RandomFloat(0.0, 1.0);
	viscositySliderAM = RandomFloat(0.0, 1.0);
	viscositySliderAA = RandomFloat(0.0, 1.0);
	viscositySliderAT = RandomFloat(0.0, 1.0);
	viscositySliderAS = RandomFloat(0.0, 1.0);
	viscositySliderAF = RandomFloat(0.0, 1.0);

	viscositySliderTR = RandomFloat(0.0, 1.0);
	viscositySliderTG = RandomFloat(0.0, 1.0);
	viscositySliderTW = RandomFloat(0.0, 1.0);
	viscositySliderTB = RandomFloat(0.0, 1.0);
	viscositySliderTO = RandomFloat(0.0, 1.0);
	viscositySliderTK = RandomFloat(0.0, 1.0);
	viscositySliderTC = RandomFloat(0.0, 1.0);
	viscositySliderTD = RandomFloat(0.0, 1.0);
	viscositySliderTY = RandomFloat(0.0, 1.0);
	viscositySliderTP = RandomFloat(0.0, 1.0);
	viscositySliderTM = RandomFloat(0.0, 1.0);
	viscositySliderTA = RandomFloat(0.0, 1.0);
	viscositySliderTT = RandomFloat(0.0, 1.0);
	viscositySliderTS = RandomFloat(0.0, 1.0);
	viscositySliderTF = RandomFloat(0.0, 1.0);

	viscositySliderSR = RandomFloat(0.0, 1.0);
	viscositySliderSG = RandomFloat(0.0, 1.0);
	viscositySliderSW = RandomFloat(0.0, 1.0);
	viscositySliderSB = RandomFloat(0.0, 1.0);
	viscositySliderSO = RandomFloat(0.0, 1.0);
	viscositySliderSK = RandomFloat(0.0, 1.0);
	viscositySliderSC = RandomFloat(0.0, 1.0);
	viscositySliderSD = RandomFloat(0.0, 1.0);
	viscositySliderSY = RandomFloat(0.0, 1.0);
	viscositySliderSP = RandomFloat(0.0, 1.0);
	viscositySliderSM = RandomFloat(0.0, 1.0);
	viscositySliderSA = RandomFloat(0.0, 1.0);
	viscositySliderST = RandomFloat(0.0, 1.0);
	viscositySliderSS = RandomFloat(0.0, 1.0);
	viscositySliderSF = RandomFloat(0.0, 1.0);

	viscositySliderFR = RandomFloat(0.0, 1.0);
	viscositySliderFG = RandomFloat(0.0, 1.0);
	viscositySliderFW = RandomFloat(0.0, 1.0);
	viscositySliderFB = RandomFloat(0.0, 1.0);
	viscositySliderFO = RandomFloat(0.0, 1.0);
	viscositySliderFK = RandomFloat(0.0, 1.0);
	viscositySliderFC = RandomFloat(0.0, 1.0);
	viscositySliderFD = RandomFloat(0.0, 1.0);
	viscositySliderFY = RandomFloat(0.0, 1.0);
	viscositySliderFP = RandomFloat(0.0, 1.0);
	viscositySliderFM = RandomFloat(0.0, 1.0);
	viscositySliderFA = RandomFloat(0.0, 1.0);
	viscositySliderFT = RandomFloat(0.0, 1.0);
	viscositySliderFS = RandomFloat(0.0, 1.0);
	viscositySliderFF = RandomFloat(0.0, 1.0);

	probabilitySlider = RandomFloat(0, 100);

	probabilitySliderRR = RandomFloat(0, 100);
	probabilitySliderRG = RandomFloat(0, 100);
	probabilitySliderRB = RandomFloat(0, 100);
	probabilitySliderRW = RandomFloat(0, 100);
	probabilitySliderRO = RandomFloat(0, 100);
	probabilitySliderRK = RandomFloat(0, 100);
	probabilitySliderRC = RandomFloat(0, 100);
	probabilitySliderRD = RandomFloat(0, 100);
	probabilitySliderRY = RandomFloat(0, 100);
	probabilitySliderRP = RandomFloat(0, 100);
	probabilitySliderRM = RandomFloat(0, 100);
	probabilitySliderRA = RandomFloat(0, 100);
	probabilitySliderRT = RandomFloat(0, 100);
	probabilitySliderRS = RandomFloat(0, 100);
	probabilitySliderRF = RandomFloat(0, 100);

	probabilitySliderGR = RandomFloat(0, 100);
	probabilitySliderGG = RandomFloat(0, 100);
	probabilitySliderGB = RandomFloat(0, 100);
	probabilitySliderGW = RandomFloat(0, 100);
	probabilitySliderGO = RandomFloat(0, 100);
	probabilitySliderGK = RandomFloat(0, 100);
	probabilitySliderGC = RandomFloat(0, 100);
	probabilitySliderGD = RandomFloat(0, 100);
	probabilitySliderGY = RandomFloat(0, 100);
	probabilitySliderGP = RandomFloat(0, 100);
	probabilitySliderGM = RandomFloat(0, 100);
	probabilitySliderGA = RandomFloat(0, 100);
	probabilitySliderGT = RandomFloat(0, 100);
	probabilitySliderGS = RandomFloat(0, 100);
	probabilitySliderGF = RandomFloat(0, 100);

	probabilitySliderBR = RandomFloat(0, 100);
	probabilitySliderBG = RandomFloat(0, 100);
	probabilitySliderBB = RandomFloat(0, 100);
	probabilitySliderBW = RandomFloat(0, 100);
	probabilitySliderBO = RandomFloat(0, 100);
	probabilitySliderBK = RandomFloat(0, 100);
	probabilitySliderBC = RandomFloat(0, 100);
	probabilitySliderBD = RandomFloat(0, 100);
	probabilitySliderBY = RandomFloat(0, 100);
	probabilitySliderBP = RandomFloat(0, 100);
	probabilitySliderBM = RandomFloat(0, 100);
	probabilitySliderBA = RandomFloat(0, 100);
	probabilitySliderBT = RandomFloat(0, 100);
	probabilitySliderBS = RandomFloat(0, 100);
	probabilitySliderBF = RandomFloat(0, 100);

	probabilitySliderWR = RandomFloat(0, 100);
	probabilitySliderWG = RandomFloat(0, 100);
	probabilitySliderWB = RandomFloat(0, 100);
	probabilitySliderWW = RandomFloat(0, 100);
	probabilitySliderWO = RandomFloat(0, 100);
	probabilitySliderWK = RandomFloat(0, 100);
	probabilitySliderWC = RandomFloat(0, 100);
	probabilitySliderWD = RandomFloat(0, 100);
	probabilitySliderWY = RandomFloat(0, 100);
	probabilitySliderWP = RandomFloat(0, 100);
	probabilitySliderWM = RandomFloat(0, 100);
	probabilitySliderWA = RandomFloat(0, 100);
	probabilitySliderWT = RandomFloat(0, 100);
	probabilitySliderWS = RandomFloat(0, 100);
	probabilitySliderWF = RandomFloat(0, 100);

	probabilitySliderOR = RandomFloat(0, 100);
	probabilitySliderOG = RandomFloat(0, 100);
	probabilitySliderOB = RandomFloat(0, 100);
	probabilitySliderOW = RandomFloat(0, 100);
	probabilitySliderOO = RandomFloat(0, 100);
	probabilitySliderOK = RandomFloat(0, 100);
	probabilitySliderOC = RandomFloat(0, 100);
	probabilitySliderOD = RandomFloat(0, 100);
	probabilitySliderOY = RandomFloat(0, 100);
	probabilitySliderOP = RandomFloat(0, 100);
	probabilitySliderOM = RandomFloat(0, 100);
	probabilitySliderOA = RandomFloat(0, 100);
	probabilitySliderOT = RandomFloat(0, 100);
	probabilitySliderOS = RandomFloat(0, 100);
	probabilitySliderOF = RandomFloat(0, 100);

	probabilitySliderKR = RandomFloat(0, 100);
	probabilitySliderKG = RandomFloat(0, 100);
	probabilitySliderKB = RandomFloat(0, 100);
	probabilitySliderKW = RandomFloat(0, 100);
	probabilitySliderKO = RandomFloat(0, 100);
	probabilitySliderKK = RandomFloat(0, 100);
	probabilitySliderKC = RandomFloat(0, 100);
	probabilitySliderKD = RandomFloat(0, 100);
	probabilitySliderKY = RandomFloat(0, 100);
	probabilitySliderKP = RandomFloat(0, 100);
	probabilitySliderKM = RandomFloat(0, 100);
	probabilitySliderKA = RandomFloat(0, 100);
	probabilitySliderKT = RandomFloat(0, 100);
	probabilitySliderKS = RandomFloat(0, 100);
	probabilitySliderKF = RandomFloat(0, 100);

	probabilitySliderCR = RandomFloat(0, 100);
	probabilitySliderCG = RandomFloat(0, 100);
	probabilitySliderCB = RandomFloat(0, 100);
	probabilitySliderCW = RandomFloat(0, 100);
	probabilitySliderCO = RandomFloat(0, 100);
	probabilitySliderCK = RandomFloat(0, 100);
	probabilitySliderCC = RandomFloat(0, 100);
	probabilitySliderCD = RandomFloat(0, 100);
	probabilitySliderCY = RandomFloat(0, 100);
	probabilitySliderCP = RandomFloat(0, 100);
	probabilitySliderCM = RandomFloat(0, 100);
	probabilitySliderCA = RandomFloat(0, 100);
	probabilitySliderCT = RandomFloat(0, 100);
	probabilitySliderCS = RandomFloat(0, 100);
	probabilitySliderCF = RandomFloat(0, 100);

	probabilitySliderDR = RandomFloat(0, 100);
	probabilitySliderDG = RandomFloat(0, 100);
	probabilitySliderDB = RandomFloat(0, 100);
	probabilitySliderDW = RandomFloat(0, 100);
	probabilitySliderDO = RandomFloat(0, 100);
	probabilitySliderDK = RandomFloat(0, 100);
	probabilitySliderDC = RandomFloat(0, 100);
	probabilitySliderDD = RandomFloat(0, 100);
	probabilitySliderDY = RandomFloat(0, 100);
	probabilitySliderDP = RandomFloat(0, 100);
	probabilitySliderDM = RandomFloat(0, 100);
	probabilitySliderDA = RandomFloat(0, 100);
	probabilitySliderDT = RandomFloat(0, 100);
	probabilitySliderDS = RandomFloat(0, 100);
	probabilitySliderDF = RandomFloat(0, 100);

	probabilitySliderYR = RandomFloat(0, 100);
	probabilitySliderYG = RandomFloat(0, 100);
	probabilitySliderYB = RandomFloat(0, 100);
	probabilitySliderYW = RandomFloat(0, 100);
	probabilitySliderYO = RandomFloat(0, 100);
	probabilitySliderYK = RandomFloat(0, 100);
	probabilitySliderYC = RandomFloat(0, 100);
	probabilitySliderYD = RandomFloat(0, 100);
	probabilitySliderYY = RandomFloat(0, 100);
	probabilitySliderYP = RandomFloat(0, 100);
	probabilitySliderYM = RandomFloat(0, 100);
	probabilitySliderYA = RandomFloat(0, 100);
	probabilitySliderYT = RandomFloat(0, 100);
	probabilitySliderYS = RandomFloat(0, 100);
	probabilitySliderYF = RandomFloat(0, 100);

	probabilitySliderPR = RandomFloat(0, 100);
	probabilitySliderPG = RandomFloat(0, 100);
	probabilitySliderPB = RandomFloat(0, 100);
	probabilitySliderPW = RandomFloat(0, 100);
	probabilitySliderPO = RandomFloat(0, 100);
	probabilitySliderPK = RandomFloat(0, 100);
	probabilitySliderPC = RandomFloat(0, 100);
	probabilitySliderPD = RandomFloat(0, 100);
	probabilitySliderPY = RandomFloat(0, 100);
	probabilitySliderPP = RandomFloat(0, 100);
	probabilitySliderPM = RandomFloat(0, 100);
	probabilitySliderPA = RandomFloat(0, 100);
	probabilitySliderPT = RandomFloat(0, 100);
	probabilitySliderPS = RandomFloat(0, 100);
	probabilitySliderPF = RandomFloat(0, 100);

	probabilitySliderMR = RandomFloat(0, 100);
	probabilitySliderMG = RandomFloat(0, 100);
	probabilitySliderMB = RandomFloat(0, 100);
	probabilitySliderMW = RandomFloat(0, 100);
	probabilitySliderMO = RandomFloat(0, 100);
	probabilitySliderMK = RandomFloat(0, 100);
	probabilitySliderMC = RandomFloat(0, 100);
	probabilitySliderMD = RandomFloat(0, 100);
	probabilitySliderMY = RandomFloat(0, 100);
	probabilitySliderMP = RandomFloat(0, 100);
	probabilitySliderMM = RandomFloat(0, 100);
	probabilitySliderMA = RandomFloat(0, 100);
	probabilitySliderMT = RandomFloat(0, 100);
	probabilitySliderMS = RandomFloat(0, 100);
	probabilitySliderMF = RandomFloat(0, 100);

	probabilitySliderAR = RandomFloat(0, 100);
	probabilitySliderAG = RandomFloat(0, 100);
	probabilitySliderAB = RandomFloat(0, 100);
	probabilitySliderAW = RandomFloat(0, 100);
	probabilitySliderAO = RandomFloat(0, 100);
	probabilitySliderAK = RandomFloat(0, 100);
	probabilitySliderAC = RandomFloat(0, 100);
	probabilitySliderAD = RandomFloat(0, 100);
	probabilitySliderAY = RandomFloat(0, 100);
	probabilitySliderAP = RandomFloat(0, 100);
	probabilitySliderAM = RandomFloat(0, 100);
	probabilitySliderAA = RandomFloat(0, 100);
	probabilitySliderAT = RandomFloat(0, 100);
	probabilitySliderAS = RandomFloat(0, 100);
	probabilitySliderAF = RandomFloat(0, 100);

	probabilitySliderTR = RandomFloat(0, 100);
	probabilitySliderTG = RandomFloat(0, 100);
	probabilitySliderTB = RandomFloat(0, 100);
	probabilitySliderTW = RandomFloat(0, 100);
	probabilitySliderTO = RandomFloat(0, 100);
	probabilitySliderTK = RandomFloat(0, 100);
	probabilitySliderTC = RandomFloat(0, 100);
	probabilitySliderTD = RandomFloat(0, 100);
	probabilitySliderTY = RandomFloat(0, 100);
	probabilitySliderTP = RandomFloat(0, 100);
	probabilitySliderTM = RandomFloat(0, 100);
	probabilitySliderTA = RandomFloat(0, 100);
	probabilitySliderTT = RandomFloat(0, 100);
	probabilitySliderTS = RandomFloat(0, 100);
	probabilitySliderTF = RandomFloat(0, 100);

	probabilitySliderSR = RandomFloat(0, 100);
	probabilitySliderSG = RandomFloat(0, 100);
	probabilitySliderSB = RandomFloat(0, 100);
	probabilitySliderSW = RandomFloat(0, 100);
	probabilitySliderSO = RandomFloat(0, 100);
	probabilitySliderSK = RandomFloat(0, 100);
	probabilitySliderSC = RandomFloat(0, 100);
	probabilitySliderSD = RandomFloat(0, 100);
	probabilitySliderSY = RandomFloat(0, 100);
	probabilitySliderSP = RandomFloat(0, 100);
	probabilitySliderSM = RandomFloat(0, 100);
	probabilitySliderSA = RandomFloat(0, 100);
	probabilitySliderST = RandomFloat(0, 100);
	probabilitySliderSS = RandomFloat(0, 100);
	probabilitySliderSF = RandomFloat(0, 100);

	probabilitySliderFR = RandomFloat(0, 100);
	probabilitySliderFG = RandomFloat(0, 100);
	probabilitySliderFB = RandomFloat(0, 100);
	probabilitySliderFW = RandomFloat(0, 100);
	probabilitySliderFO = RandomFloat(0, 100);
	probabilitySliderFK = RandomFloat(0, 100);
	probabilitySliderFC = RandomFloat(0, 100);
	probabilitySliderFD = RandomFloat(0, 100);
	probabilitySliderFY = RandomFloat(0, 100);
	probabilitySliderFP = RandomFloat(0, 100);
	probabilitySliderFM = RandomFloat(0, 100);
	probabilitySliderFA = RandomFloat(0, 100);
	probabilitySliderFT = RandomFloat(0, 100);
	probabilitySliderFS = RandomFloat(0, 100);
	probabilitySliderFF = RandomFloat(0, 100);

	// GREEN
	//numberSliderG = RandomFloat(0, 3000);
	powerSliderGG = RandomFloat(0, 100) * forceVariance;
	powerSliderGR = RandomFloat(0, 100) * forceVariance;
	powerSliderGW = RandomFloat(0, 100) * forceVariance;
	powerSliderGB = RandomFloat(0, 100) * forceVariance;
	powerSliderGO = RandomFloat(0, 100) * forceVariance;
	powerSliderGK = RandomFloat(0, 100) * forceVariance;
	powerSliderGC = RandomFloat(0, 100) * forceVariance;
	powerSliderGD = RandomFloat(0, 100) * forceVariance;
	powerSliderGY = RandomFloat(0, 100) * forceVariance;
	powerSliderGP = RandomFloat(0, 100) * forceVariance;
	powerSliderGM = RandomFloat(0, 100) * forceVariance;
	powerSliderGA = RandomFloat(0, 100) * forceVariance;
	powerSliderGT = RandomFloat(0, 100) * forceVariance;
	powerSliderGS = RandomFloat(0, 100) * forceVariance;
	powerSliderGF = RandomFloat(0, 100) * forceVariance;

	vSliderGG = RandomFloat(0, 500) * radiusVariance;
	vSliderGR = RandomFloat(0, 500) * radiusVariance;
	vSliderGW = RandomFloat(0, 500) * radiusVariance;
	vSliderGB = RandomFloat(0, 500) * radiusVariance;
	vSliderGO = RandomFloat(0, 500) * radiusVariance;
	vSliderGK = RandomFloat(0, 500) * radiusVariance;
	vSliderGC = RandomFloat(0, 500) * radiusVariance;
	vSliderGD = RandomFloat(0, 500) * radiusVariance;
	vSliderGY = RandomFloat(0, 500) * radiusVariance;
	vSliderGP = RandomFloat(0, 500) * radiusVariance;
	vSliderGM = RandomFloat(0, 500) * radiusVariance;
	vSliderGA = RandomFloat(0, 500) * radiusVariance;
	vSliderGT = RandomFloat(0, 500) * radiusVariance;
	vSliderGS = RandomFloat(0, 500) * radiusVariance;
	vSliderGF = RandomFloat(0, 500) * radiusVariance;

	// RED
	//numberSliderR = RandomFloat(0, 3000);
	powerSliderRR = RandomFloat(0, 100) * forceVariance;
	powerSliderRG = RandomFloat(0, 100) * forceVariance;
	powerSliderRW = RandomFloat(0, 100) * forceVariance;
	powerSliderRB = RandomFloat(0, 100) * forceVariance;
	powerSliderRO = RandomFloat(0, 100) * forceVariance;
	powerSliderRK = RandomFloat(0, 100) * forceVariance;
	powerSliderRC = RandomFloat(0, 100) * forceVariance;
	powerSliderRD = RandomFloat(0, 100) * forceVariance;
	powerSliderRY = RandomFloat(0, 100) * forceVariance;
	powerSliderRP = RandomFloat(0, 100) * forceVariance;
	powerSliderRM = RandomFloat(0, 100) * forceVariance;
	powerSliderRA = RandomFloat(0, 100) * forceVariance;
	powerSliderRT = RandomFloat(0, 100) * forceVariance;
	powerSliderRS = RandomFloat(0, 100) * forceVariance;
	powerSliderRF = RandomFloat(0, 100) * forceVariance;

	vSliderRG = RandomFloat(0, 500) * radiusVariance;
	vSliderRR = RandomFloat(0, 500) * radiusVariance;
	vSliderRW = RandomFloat(0, 500) * radiusVariance;
	vSliderRB = RandomFloat(0, 500) * radiusVariance;
	vSliderRO = RandomFloat(0, 500) * radiusVariance;
	vSliderRK = RandomFloat(0, 500) * radiusVariance;
	vSliderRC = RandomFloat(0, 500) * radiusVariance;
	vSliderRD = RandomFloat(0, 500) * radiusVariance;
	vSliderRY = RandomFloat(0, 500) * radiusVariance;
	vSliderRP = RandomFloat(0, 500) * radiusVariance;
	vSliderRM = RandomFloat(0, 500) * radiusVariance;
	vSliderRA = RandomFloat(0, 500) * radiusVariance;
	vSliderRT = RandomFloat(0, 500) * radiusVariance;
	vSliderRS = RandomFloat(0, 500) * radiusVariance;
	vSliderRF = RandomFloat(0, 500) * radiusVariance;

	// WHITE
	// numberSliderW = RandomFloat(0, 3000);
	powerSliderWW = RandomFloat(0, 100) * forceVariance;
	powerSliderWR = RandomFloat(0, 100) * forceVariance;
	powerSliderWG = RandomFloat(0, 100) * forceVariance;
	powerSliderWB = RandomFloat(0, 100) * forceVariance;
	powerSliderWO = RandomFloat(0, 100) * forceVariance;
	powerSliderWK = RandomFloat(0, 100) * forceVariance;
	powerSliderWC = RandomFloat(0, 100) * forceVariance;
	powerSliderWD = RandomFloat(0, 100) * forceVariance;
	powerSliderWY = RandomFloat(0, 100) * forceVariance;
	powerSliderWP = RandomFloat(0, 100) * forceVariance;
	powerSliderWM = RandomFloat(0, 100) * forceVariance;
	powerSliderWA = RandomFloat(0, 100) * forceVariance;
	powerSliderWT = RandomFloat(0, 100) * forceVariance;
	powerSliderWS = RandomFloat(0, 100) * forceVariance;
	powerSliderWF = RandomFloat(0, 100) * forceVariance;

	vSliderWG = RandomFloat(0, 500) * radiusVariance;
	vSliderWR = RandomFloat(0, 500) * radiusVariance;
	vSliderWW = RandomFloat(0, 500) * radiusVariance;
	vSliderWB = RandomFloat(0, 500) * radiusVariance;
	vSliderWO = RandomFloat(0, 500) * radiusVariance;
	vSliderWK = RandomFloat(0, 500) * radiusVariance;
	vSliderWC = RandomFloat(0, 500) * radiusVariance;
	vSliderWD = RandomFloat(0, 500) * radiusVariance;
	vSliderWY = RandomFloat(0, 500) * radiusVariance;
	vSliderWP = RandomFloat(0, 500) * radiusVariance;
	vSliderWM = RandomFloat(0, 500) * radiusVariance;
	vSliderWA = RandomFloat(0, 500) * radiusVariance;
	vSliderWT = RandomFloat(0, 500) * radiusVariance;
	vSliderWS = RandomFloat(0, 500) * radiusVariance;
	vSliderWF = RandomFloat(0, 500) * radiusVariance;

	// BLUE
	//numberSliderB = RandomFloat(0, 3000);
	powerSliderBB = RandomFloat(0, 100) * forceVariance;
	powerSliderBW = RandomFloat(0, 100) * forceVariance;
	powerSliderBR = RandomFloat(0, 100) * forceVariance;
	powerSliderBG = RandomFloat(0, 100) * forceVariance;
	powerSliderBO = RandomFloat(0, 100) * forceVariance;
	powerSliderBK = RandomFloat(0, 100) * forceVariance;
	powerSliderBC = RandomFloat(0, 100) * forceVariance;
	powerSliderBD = RandomFloat(0, 100) * forceVariance;
	powerSliderBY = RandomFloat(0, 100) * forceVariance;
	powerSliderBP = RandomFloat(0, 100) * forceVariance;
	powerSliderBM = RandomFloat(0, 100) * forceVariance;
	powerSliderBA = RandomFloat(0, 100) * forceVariance;
	powerSliderBT = RandomFloat(0, 100) * forceVariance;
	powerSliderBS = RandomFloat(0, 100) * forceVariance;
	powerSliderBF = RandomFloat(0, 100) * forceVariance;

	vSliderBG = RandomFloat(0, 500) * radiusVariance;
	vSliderBR = RandomFloat(0, 500) * radiusVariance;
	vSliderBW = RandomFloat(0, 500) * radiusVariance;
	vSliderBB = RandomFloat(0, 500) * radiusVariance;
	vSliderBO = RandomFloat(0, 500) * radiusVariance;
	vSliderBK = RandomFloat(0, 500) * radiusVariance;
	vSliderBC = RandomFloat(0, 500) * radiusVariance;
	vSliderBD = RandomFloat(0, 500) * radiusVariance;
	vSliderBY = RandomFloat(0, 500) * radiusVariance;
	vSliderBP = RandomFloat(0, 500) * radiusVariance;
	vSliderBM = RandomFloat(0, 500) * radiusVariance;
	vSliderBA = RandomFloat(0, 500) * radiusVariance;
	vSliderBT = RandomFloat(0, 500) * radiusVariance;
	vSliderBS = RandomFloat(0, 500) * radiusVariance;
	vSliderBF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderOY = RandomFloat(-100, 100) * forceVariance;
	powerSliderOP = RandomFloat(-100, 100) * forceVariance;
	powerSliderOM = RandomFloat(-100, 100) * forceVariance;
	powerSliderOA = RandomFloat(-100, 100) * forceVariance;
	powerSliderOT = RandomFloat(-100, 100) * forceVariance;
	powerSliderOS = RandomFloat(-100, 100) * forceVariance;
	powerSliderOF = RandomFloat(-100, 100) * forceVariance;

	vSliderOG = RandomFloat(0, 500) * radiusVariance;
	vSliderOR = RandomFloat(0, 500) * radiusVariance;
	vSliderOW = RandomFloat(0, 500) * radiusVariance;
	vSliderOB = RandomFloat(0, 500) * radiusVariance;
	vSliderOO = RandomFloat(0, 500) * radiusVariance;
	vSliderOK = RandomFloat(0, 500) * radiusVariance;
	vSliderOC = RandomFloat(0, 500) * radiusVariance;
	vSliderOD = RandomFloat(0, 500) * radiusVariance;
	vSliderOY = RandomFloat(0, 500) * radiusVariance;
	vSliderOP = RandomFloat(0, 500) * radiusVariance;
	vSliderOM = RandomFloat(0, 500) * radiusVariance;
	vSliderOA = RandomFloat(0, 500) * radiusVariance;
	vSliderOT = RandomFloat(0, 500) * radiusVariance;
	vSliderOS = RandomFloat(0, 500) * radiusVariance;
	vSliderOF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderKY = RandomFloat(-100, 100) * forceVariance;
	powerSliderKP = RandomFloat(-100, 100) * forceVariance;
	powerSliderKM = RandomFloat(-100, 100) * forceVariance;
	powerSliderKA = RandomFloat(-100, 100) * forceVariance;
	powerSliderKT = RandomFloat(-100, 100) * forceVariance;
	powerSliderKS = RandomFloat(-100, 100) * forceVariance;
	powerSliderKF = RandomFloat(-100, 100) * forceVariance;

	vSliderKG = RandomFloat(0, 500) * radiusVariance;
	vSliderKR = RandomFloat(0, 500) * radiusVariance;
	vSliderKW = RandomFloat(0, 500) * radiusVariance;
	vSliderKB = RandomFloat(0, 500) * radiusVariance;
	vSliderKO = RandomFloat(0, 500) * radiusVariance;
	vSliderKK = RandomFloat(0, 500) * radiusVariance;
	vSliderKC = RandomFloat(0, 500) * radiusVariance;
	vSliderKD = RandomFloat(0, 500) * radiusVariance;
	vSliderKY = RandomFloat(0, 500) * radiusVariance;
	vSliderKP = RandomFloat(0, 500) * radiusVariance;
	vSliderKM = RandomFloat(0, 500) * radiusVariance;
	vSliderKA = RandomFloat(0, 500) * radiusVariance;
	vSliderKT = RandomFloat(0, 500) * radiusVariance;
	vSliderKS = RandomFloat(0, 500) * radiusVariance;
	vSliderKF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderCY = RandomFloat(-100, 100) * forceVariance;
	powerSliderCP = RandomFloat(-100, 100) * forceVariance;
	powerSliderCM = RandomFloat(-100, 100) * forceVariance;
	powerSliderCA = RandomFloat(-100, 100) * forceVariance;
	powerSliderCT = RandomFloat(-100, 100) * forceVariance;
	powerSliderCS = RandomFloat(-100, 100) * forceVariance;
	powerSliderCF = RandomFloat(-100, 100) * forceVariance;

	vSliderCG = RandomFloat(0, 500) * radiusVariance;
	vSliderCR = RandomFloat(0, 500) * radiusVariance;
	vSliderCW = RandomFloat(0, 500) * radiusVariance;
	vSliderCB = RandomFloat(0, 500) * radiusVariance;
	vSliderCO = RandomFloat(0, 500) * radiusVariance;
	vSliderCK = RandomFloat(0, 500) * radiusVariance;
	vSliderCC = RandomFloat(0, 500) * radiusVariance;
	vSliderCD = RandomFloat(0, 500) * radiusVariance;
	vSliderCY = RandomFloat(0, 500) * radiusVariance;
	vSliderCP = RandomFloat(0, 500) * radiusVariance;
	vSliderCM = RandomFloat(0, 500) * radiusVariance;
	vSliderCA = RandomFloat(0, 500) * radiusVariance;
	vSliderCT = RandomFloat(0, 500) * radiusVariance;
	vSliderCS = RandomFloat(0, 500) * radiusVariance;
	vSliderCF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderDY = RandomFloat(-100, 100) * forceVariance;
	powerSliderDP = RandomFloat(-100, 100) * forceVariance;
	powerSliderDM = RandomFloat(-100, 100) * forceVariance;
	powerSliderDA = RandomFloat(-100, 100) * forceVariance;
	powerSliderDT = RandomFloat(-100, 100) * forceVariance;
	powerSliderDS = RandomFloat(-100, 100) * forceVariance;
	powerSliderDF = RandomFloat(-100, 100) * forceVariance;

	vSliderDG = RandomFloat(0, 500) * radiusVariance;
	vSliderDR = RandomFloat(0, 500) * radiusVariance;
	vSliderDW = RandomFloat(0, 500) * radiusVariance;
	vSliderDB = RandomFloat(0, 500) * radiusVariance;
	vSliderDO = RandomFloat(0, 500) * radiusVariance;
	vSliderDK = RandomFloat(0, 500) * radiusVariance;
	vSliderDC = RandomFloat(0, 500) * radiusVariance;
	vSliderDD = RandomFloat(0, 500) * radiusVariance;
	vSliderDY = RandomFloat(0, 500) * radiusVariance;
	vSliderDP = RandomFloat(0, 500) * radiusVariance;
	vSliderDM = RandomFloat(0, 500) * radiusVariance;
	vSliderDA = RandomFloat(0, 500) * radiusVariance;
	vSliderDT = RandomFloat(0, 500) * radiusVariance;
	vSliderDS = RandomFloat(0, 500) * radiusVariance;
	vSliderDF = RandomFloat(0, 500) * radiusVariance;

	// YELLOW
	//numberSliderY = RandomFloat(0, 3000);
	powerSliderYB = RandomFloat(-100, 0) * forceVariance;
	powerSliderYW = RandomFloat(-100, 0) * forceVariance;
	powerSliderYR = RandomFloat(-100, 0) * forceVariance;
	powerSliderYG = RandomFloat(-100, 0) * forceVariance;
	powerSliderYO = RandomFloat(-100, 0) * forceVariance;
	powerSliderYK = RandomFloat(-100, 0) * forceVariance;
	powerSliderYC = RandomFloat(-100, 0) * forceVariance;
	powerSliderYD = RandomFloat(-100, 0) * forceVariance;
	powerSliderYY = RandomFloat(-100, 0) * forceVariance;
	powerSliderYP = RandomFloat(-100, 0) * forceVariance;
	powerSliderYM = RandomFloat(-100, 0) * forceVariance;
	powerSliderYA = RandomFloat(-100, 0) * forceVariance;
	powerSliderYT = RandomFloat(-100, 0) * forceVariance;
	powerSliderYS = RandomFloat(-100, 0) * forceVariance;
	powerSliderYF = RandomFloat(-100, 0) * forceVariance;

	vSliderYG = RandomFloat(0, 500) * radiusVariance;
	vSliderYR = RandomFloat(0, 500) * radiusVariance;
	vSliderYW = RandomFloat(0, 500) * radiusVariance;
	vSliderYB = RandomFloat(0, 500) * radiusVariance;
	vSliderYO = RandomFloat(0, 500) * radiusVariance;
	vSliderYK = RandomFloat(0, 500) * radiusVariance;
	vSliderYC = RandomFloat(0, 500) * radiusVariance;
	vSliderYD = RandomFloat(0, 500) * radiusVariance;
	vSliderYY = RandomFloat(0, 500) * radiusVariance;
	vSliderYP = RandomFloat(0, 500) * radiusVariance;
	vSliderYM = RandomFloat(0, 500) * radiusVariance;
	vSliderYA = RandomFloat(0, 500) * radiusVariance;
	vSliderYT = RandomFloat(0, 500) * radiusVariance;
	vSliderYS = RandomFloat(0, 500) * radiusVariance;
	vSliderYF = RandomFloat(0, 500) * radiusVariance;

	// PINK
	//numberSliderP = RandomFloat(0, 3000);
	powerSliderPB = RandomFloat(-100, 0) * forceVariance;
	powerSliderPW = RandomFloat(-100, 0) * forceVariance;
	powerSliderPR = RandomFloat(-100, 0) * forceVariance;
	powerSliderPG = RandomFloat(-100, 0) * forceVariance;
	powerSliderPO = RandomFloat(-100, 0) * forceVariance;
	powerSliderPK = RandomFloat(-100, 0) * forceVariance;
	powerSliderPC = RandomFloat(-100, 0) * forceVariance;
	powerSliderPD = RandomFloat(-100, 0) * forceVariance;
	powerSliderPY = RandomFloat(-100, 0) * forceVariance;
	powerSliderPP = RandomFloat(-100, 0) * forceVariance;
	powerSliderPM = RandomFloat(-100, 0) * forceVariance;
	powerSliderPA = RandomFloat(-100, 0) * forceVariance;
	powerSliderPT = RandomFloat(-100, 0) * forceVariance;
	powerSliderPS = RandomFloat(-100, 0) * forceVariance;
	powerSliderPF = RandomFloat(-100, 0) * forceVariance;

	vSliderPG = RandomFloat(0, 500) * radiusVariance;
	vSliderPR = RandomFloat(0, 500) * radiusVariance;
	vSliderPW = RandomFloat(0, 500) * radiusVariance;
	vSliderPB = RandomFloat(0, 500) * radiusVariance;
	vSliderPO = RandomFloat(0, 500) * radiusVariance;
	vSliderPK = RandomFloat(0, 500) * radiusVariance;
	vSliderPC = RandomFloat(0, 500) * radiusVariance;
	vSliderPD = RandomFloat(0, 500) * radiusVariance;
	vSliderPY = RandomFloat(0, 500) * radiusVariance;
	vSliderPP = RandomFloat(0, 500) * radiusVariance;
	vSliderPM = RandomFloat(0, 500) * radiusVariance;
	vSliderPA = RandomFloat(0, 500) * radiusVariance;
	vSliderPT = RandomFloat(0, 500) * radiusVariance;
	vSliderPS = RandomFloat(0, 500) * radiusVariance;
	vSliderPF = RandomFloat(0, 500) * radiusVariance;

	// MAGENTA
	//numberSliderM = RandomFloat(0, 3000);
	powerSliderMB = RandomFloat(-100, 0) * forceVariance;
	powerSliderMW = RandomFloat(-100, 0) * forceVariance;
	powerSliderMR = RandomFloat(-100, 0) * forceVariance;
	powerSliderMG = RandomFloat(-100, 0) * forceVariance;
	powerSliderMO = RandomFloat(-100, 0) * forceVariance;
	powerSliderMK = RandomFloat(-100, 0) * forceVariance;
	powerSliderMC = RandomFloat(-100, 0) * forceVariance;
	powerSliderMD = RandomFloat(-100, 0) * forceVariance;
	powerSliderMY = RandomFloat(-100, 0) * forceVariance;
	powerSliderMP = RandomFloat(-100, 0) * forceVariance;
	powerSliderMM = RandomFloat(-100, 0) * forceVariance;
	powerSliderMA = RandomFloat(-100, 0) * forceVariance;
	powerSliderMT = RandomFloat(-100, 0) * forceVariance;
	powerSliderMS = RandomFloat(-100, 0) * forceVariance;
	powerSliderMF = RandomFloat(-100, 0) * forceVariance;

	vSliderMG = RandomFloat(0, 500) * radiusVariance;
	vSliderMR = RandomFloat(0, 500) * radiusVariance;
	vSliderMW = RandomFloat(0, 500) * radiusVariance;
	vSliderMB = RandomFloat(0, 500) * radiusVariance;
	vSliderMO = RandomFloat(0, 500) * radiusVariance;
	vSliderMK = RandomFloat(0, 500) * radiusVariance;
	vSliderMC = RandomFloat(0, 500) * radiusVariance;
	vSliderMD = RandomFloat(0, 500) * radiusVariance;
	vSliderMY = RandomFloat(0, 500) * radiusVariance;
	vSliderMP = RandomFloat(0, 500) * radiusVariance;
	vSliderMM = RandomFloat(0, 500) * radiusVariance;
	vSliderMA = RandomFloat(0, 500) * radiusVariance;
	vSliderMT = RandomFloat(0, 500) * radiusVariance;
	vSliderMS = RandomFloat(0, 500) * radiusVariance;
	vSliderMF = RandomFloat(0, 500) * radiusVariance;

	// AQUA
	//numberSliderA = RandomFloat(0, 3000);
	powerSliderAB = RandomFloat(-100, 0) * forceVariance;
	powerSliderAW = RandomFloat(-100, 0) * forceVariance;
	powerSliderAR = RandomFloat(-100, 0) * forceVariance;
	powerSliderAG = RandomFloat(-100, 0) * forceVariance;
	powerSliderAO = RandomFloat(-100, 0) * forceVariance;
	powerSliderAK = RandomFloat(-100, 0) * forceVariance;
	powerSliderAC = RandomFloat(-100, 0) * forceVariance;
	powerSliderAD = RandomFloat(-100, 0) * forceVariance;
	powerSliderAY = RandomFloat(-100, 0) * forceVariance;
	powerSliderAP = RandomFloat(-100, 0) * forceVariance;
	powerSliderAM = RandomFloat(-100, 0) * forceVariance;
	powerSliderAA = RandomFloat(-100, 0) * forceVariance;
	powerSliderAT = RandomFloat(-100, 0) * forceVariance;
	powerSliderAS = RandomFloat(-100, 0) * forceVariance;
	powerSliderAF = RandomFloat(-100, 0) * forceVariance;

	vSliderAG = RandomFloat(0, 500) * radiusVariance;
	vSliderAR = RandomFloat(0, 500) * radiusVariance;
	vSliderAW = RandomFloat(0, 500) * radiusVariance;
	vSliderAB = RandomFloat(0, 500) * radiusVariance;
	vSliderAO = RandomFloat(0, 500) * radiusVariance;
	vSliderAK = RandomFloat(0, 500) * radiusVariance;
	vSliderAC = RandomFloat(0, 500) * radiusVariance;
	vSliderAD = RandomFloat(0, 500) * radiusVariance;
	vSliderAY = RandomFloat(0, 500) * radiusVariance;
	vSliderAP = RandomFloat(0, 500) * radiusVariance;
	vSliderAM = RandomFloat(0, 500) * radiusVariance;
	vSliderAA = RandomFloat(0, 500) * radiusVariance;
	vSliderAT = RandomFloat(0, 500) * radiusVariance;
	vSliderAS = RandomFloat(0, 500) * radiusVariance;
	vSliderAF = RandomFloat(0, 500) * radiusVariance;

	// TEAL
	//numberSliderT = RandomFloat(0, 3000);
	powerSliderTB = RandomFloat(0, 100) * forceVariance;
	powerSliderTW = RandomFloat(0, 100) * forceVariance;
	powerSliderTR = RandomFloat(0, 100) * forceVariance;
	powerSliderTG = RandomFloat(0, 100) * forceVariance;
	powerSliderTO = RandomFloat(0, 100) * forceVariance;
	powerSliderTK = RandomFloat(0, 100) * forceVariance;
	powerSliderTC = RandomFloat(0, 100) * forceVariance;
	powerSliderTD = RandomFloat(0, 100) * forceVariance;
	powerSliderTY = RandomFloat(0, 100) * forceVariance;
	powerSliderTP = RandomFloat(0, 100) * forceVariance;
	powerSliderTM = RandomFloat(0, 100) * forceVariance;
	powerSliderTA = RandomFloat(0, 100) * forceVariance;
	powerSliderTT = RandomFloat(0, 100) * forceVariance;
	powerSliderTS = RandomFloat(0, 100) * forceVariance;
	powerSliderTF = RandomFloat(0, 100) * forceVariance;

	vSliderTG = RandomFloat(0, 500) * radiusVariance;
	vSliderTR = RandomFloat(0, 500) * radiusVariance;
	vSliderTW = RandomFloat(0, 500) * radiusVariance;
	vSliderTB = RandomFloat(0, 500) * radiusVariance;
	vSliderTO = RandomFloat(0, 500) * radiusVariance;
	vSliderTK = RandomFloat(0, 500) * radiusVariance;
	vSliderTC = RandomFloat(0, 500) * radiusVariance;
	vSliderTD = RandomFloat(0, 500) * radiusVariance;
	vSliderTY = RandomFloat(0, 500) * radiusVariance;
	vSliderTP = RandomFloat(0, 500) * radiusVariance;
	vSliderTM = RandomFloat(0, 500) * radiusVariance;
	vSliderTA = RandomFloat(0, 500) * radiusVariance;
	vSliderTT = RandomFloat(0, 500) * radiusVariance;
	vSliderTS = RandomFloat(0, 500) * radiusVariance;
	vSliderTF = RandomFloat(0, 500) * radiusVariance;

	// SILVER
	//numberSliderS = RandomFloat(0, 3000);
	powerSliderSB = RandomFloat(-100, 100) * forceVariance;
	powerSliderSW = RandomFloat(-100, 100) * forceVariance;
	powerSliderSR = RandomFloat(-100, 100) * forceVariance;
	powerSliderSG = RandomFloat(-100, 100) * forceVariance;
	powerSliderSO = RandomFloat(-100, 100) * forceVariance;
	powerSliderSK = RandomFloat(-100, 100) * forceVariance;
	powerSliderSC = RandomFloat(-100, 100) * forceVariance;
	powerSliderSD = RandomFloat(-100, 100) * forceVariance;
	powerSliderSY = RandomFloat(-100, 100) * forceVariance;
	powerSliderSP = RandomFloat(-100, 100) * forceVariance;
	powerSliderSM = RandomFloat(-100, 100) * forceVariance;
	powerSliderSA = RandomFloat(-100, 100) * forceVariance;
	powerSliderST = RandomFloat(-100, 100) * forceVariance;
	powerSliderSS = RandomFloat(-100, 100) * forceVariance;
	powerSliderSF = RandomFloat(-100, 100) * forceVariance;

	vSliderSG = RandomFloat(0, 500) * radiusVariance;
	vSliderSR = RandomFloat(0, 500) * radiusVariance;
	vSliderSW = RandomFloat(0, 500) * radiusVariance;
	vSliderSB = RandomFloat(0, 500) * radiusVariance;
	vSliderSO = RandomFloat(0, 500) * radiusVariance;
	vSliderSK = RandomFloat(0, 500) * radiusVariance;
	vSliderSC = RandomFloat(0, 500) * radiusVariance;
	vSliderSD = RandomFloat(0, 500) * radiusVariance;
	vSliderSY = RandomFloat(0, 500) * radiusVariance;
	vSliderSP = RandomFloat(0, 500) * radiusVariance;
	vSliderSM = RandomFloat(0, 500) * radiusVariance;
	vSliderSA = RandomFloat(0, 500) * radiusVariance;
	vSliderST = RandomFloat(0, 500) * radiusVariance;
	vSliderSS = RandomFloat(0, 500) * radiusVariance;
	vSliderSF = RandomFloat(0, 500) * radiusVariance;

	// FIREBRICK
	//numberSliderF = RandomFloat(0, 3000);
	powerSliderFB = RandomFloat(-100, 0) * forceVariance;
	powerSliderFW = RandomFloat(-100, 0) * forceVariance;
	powerSliderFR = RandomFloat(-100, 0) * forceVariance;
	powerSliderFG = RandomFloat(-100, 0) * forceVariance;
	powerSliderFO = RandomFloat(-100, 0) * forceVariance;
	powerSliderFK = RandomFloat(-100, 0) * forceVariance;
	powerSliderFC = RandomFloat(-100, 0) * forceVariance;
	powerSliderFD = RandomFloat(-100, 0) * forceVariance;
	powerSliderFY = RandomFloat(-100, 0) * forceVariance;
	powerSliderFP = RandomFloat(-100, 0) * forceVariance;
	powerSliderFM = RandomFloat(-100, 0) * forceVariance;
	powerSliderFA = RandomFloat(-100, 0) * forceVariance;
	powerSliderFT = RandomFloat(-100, 0) * forceVariance;
	powerSliderFS = RandomFloat(-100, 0) * forceVariance;
	powerSliderFF = RandomFloat(-100, 0) * forceVariance;

	vSliderFG = RandomFloat(0, 500) * radiusVariance;
	vSliderFR = RandomFloat(0, 500) * radiusVariance;
	vSliderFW = RandomFloat(0, 500) * radiusVariance;
	vSliderFB = RandomFloat(0, 500) * radiusVariance;
	vSliderFO = RandomFloat(0, 500) * radiusVariance;
	vSliderFK = RandomFloat(0, 500) * radiusVariance;
	vSliderFC = RandomFloat(0, 500) * radiusVariance;
	vSliderFD = RandomFloat(0, 500) * radiusVariance;
	vSliderFY = RandomFloat(0, 500) * radiusVariance;
	vSliderFP = RandomFloat(0, 500) * radiusVariance;
	vSliderFM = RandomFloat(0, 500) * radiusVariance;
	vSliderFA = RandomFloat(0, 500) * radiusVariance;
	vSliderFT = RandomFloat(0, 500) * radiusVariance;
	vSliderFS = RandomFloat(0, 500) * radiusVariance;
	vSliderFF = RandomFloat(0, 500) * radiusVariance;
}

void ofApp::monads() {
	numberSliderG = RandomInt(200, 1000);
	numberSliderR = RandomInt(200, 1000);
	numberSliderW = RandomInt(200, 1000);
	numberSliderB = RandomInt(200, 1000);
	numberSliderO = RandomInt(200, 1000);
	numberSliderK = RandomInt(200, 1000);
	numberSliderC = RandomInt(200, 1000);
	numberSliderD = RandomInt(200, 1000);
	numberSliderY = RandomInt(200, 1000);
	numberSliderP = RandomInt(200, 1000);
	numberSliderM = RandomInt(200, 1000);
	numberSliderA = RandomInt(200, 1000);
	numberSliderT = RandomInt(200, 1000);
	numberSliderS = RandomInt(200, 1000);
	numberSliderF = RandomInt(200, 1000);
}
void ofApp::rndvsc() {
	viscositySlider = RandomFloat(0.0, 1.0);

	viscositySliderRR = RandomFloat(0.0, 1.0);
	viscositySliderRG = RandomFloat(0.0, 1.0);
	viscositySliderRW = RandomFloat(0.0, 1.0);
	viscositySliderRB = RandomFloat(0.0, 1.0);
	viscositySliderRO = RandomFloat(0.0, 1.0);
	viscositySliderRK = RandomFloat(0.0, 1.0);
	viscositySliderRC = RandomFloat(0.0, 1.0);
	viscositySliderRD = RandomFloat(0.0, 1.0);
	viscositySliderRY = RandomFloat(0.0, 1.0);
	viscositySliderRP = RandomFloat(0.0, 1.0);
	viscositySliderRM = RandomFloat(0.0, 1.0);
	viscositySliderRA = RandomFloat(0.0, 1.0);
	viscositySliderRT = RandomFloat(0.0, 1.0);
	viscositySliderRS = RandomFloat(0.0, 1.0);
	viscositySliderRF = RandomFloat(0.0, 1.0);

	viscositySliderGR = RandomFloat(0.0, 1.0);
	viscositySliderGG = RandomFloat(0.0, 1.0);
	viscositySliderGW = RandomFloat(0.0, 1.0);
	viscositySliderGB = RandomFloat(0.0, 1.0);
	viscositySliderGO = RandomFloat(0.0, 1.0);
	viscositySliderGK = RandomFloat(0.0, 1.0);
	viscositySliderGC = RandomFloat(0.0, 1.0);
	viscositySliderGD = RandomFloat(0.0, 1.0);
	viscositySliderGY = RandomFloat(0.0, 1.0);
	viscositySliderGP = RandomFloat(0.0, 1.0);
	viscositySliderGM = RandomFloat(0.0, 1.0);
	viscositySliderGA = RandomFloat(0.0, 1.0);
	viscositySliderGT = RandomFloat(0.0, 1.0);
	viscositySliderGS = RandomFloat(0.0, 1.0);
	viscositySliderGF = RandomFloat(0.0, 1.0);

	viscositySliderBR = RandomFloat(0.0, 1.0);
	viscositySliderBG = RandomFloat(0.0, 1.0);
	viscositySliderBW = RandomFloat(0.0, 1.0);
	viscositySliderBB = RandomFloat(0.0, 1.0);
	viscositySliderBO = RandomFloat(0.0, 1.0);
	viscositySliderBK = RandomFloat(0.0, 1.0);
	viscositySliderBC = RandomFloat(0.0, 1.0);
	viscositySliderBD = RandomFloat(0.0, 1.0);
	viscositySliderBY = RandomFloat(0.0, 1.0);
	viscositySliderBP = RandomFloat(0.0, 1.0);
	viscositySliderBM = RandomFloat(0.0, 1.0);
	viscositySliderBA = RandomFloat(0.0, 1.0);
	viscositySliderBT = RandomFloat(0.0, 1.0);
	viscositySliderBS = RandomFloat(0.0, 1.0);
	viscositySliderBF = RandomFloat(0.0, 1.0);

	viscositySliderWR = RandomFloat(0.0, 1.0);
	viscositySliderWG = RandomFloat(0.0, 1.0);
	viscositySliderWW = RandomFloat(0.0, 1.0);
	viscositySliderWB = RandomFloat(0.0, 1.0);
	viscositySliderWO = RandomFloat(0.0, 1.0);
	viscositySliderWK = RandomFloat(0.0, 1.0);
	viscositySliderWC = RandomFloat(0.0, 1.0);
	viscositySliderWD = RandomFloat(0.0, 1.0);
	viscositySliderWY = RandomFloat(0.0, 1.0);
	viscositySliderWP = RandomFloat(0.0, 1.0);
	viscositySliderWM = RandomFloat(0.0, 1.0);
	viscositySliderWA = RandomFloat(0.0, 1.0);
	viscositySliderWT = RandomFloat(0.0, 1.0);
	viscositySliderWS = RandomFloat(0.0, 1.0);
	viscositySliderWF = RandomFloat(0.0, 1.0);

	viscositySliderOR = RandomFloat(0.0, 1.0);
	viscositySliderOG = RandomFloat(0.0, 1.0);
	viscositySliderOW = RandomFloat(0.0, 1.0);
	viscositySliderOB = RandomFloat(0.0, 1.0);
	viscositySliderOO = RandomFloat(0.0, 1.0);
	viscositySliderOK = RandomFloat(0.0, 1.0);
	viscositySliderOC = RandomFloat(0.0, 1.0);
	viscositySliderOD = RandomFloat(0.0, 1.0);
	viscositySliderOY = RandomFloat(0.0, 1.0);
	viscositySliderOP = RandomFloat(0.0, 1.0);
	viscositySliderOM = RandomFloat(0.0, 1.0);
	viscositySliderOA = RandomFloat(0.0, 1.0);
	viscositySliderOT = RandomFloat(0.0, 1.0);
	viscositySliderOS = RandomFloat(0.0, 1.0);
	viscositySliderOF = RandomFloat(0.0, 1.0);

	viscositySliderKR = RandomFloat(0.0, 1.0);
	viscositySliderKG = RandomFloat(0.0, 1.0);
	viscositySliderKW = RandomFloat(0.0, 1.0);
	viscositySliderKB = RandomFloat(0.0, 1.0);
	viscositySliderKO = RandomFloat(0.0, 1.0);
	viscositySliderKK = RandomFloat(0.0, 1.0);
	viscositySliderKC = RandomFloat(0.0, 1.0);
	viscositySliderKD = RandomFloat(0.0, 1.0);
	viscositySliderKY = RandomFloat(0.0, 1.0);
	viscositySliderKP = RandomFloat(0.0, 1.0);
	viscositySliderKM = RandomFloat(0.0, 1.0);
	viscositySliderKA = RandomFloat(0.0, 1.0);
	viscositySliderKT = RandomFloat(0.0, 1.0);
	viscositySliderKS = RandomFloat(0.0, 1.0);
	viscositySliderKF = RandomFloat(0.0, 1.0);

	viscositySliderCR = RandomFloat(0.0, 1.0);
	viscositySliderCG = RandomFloat(0.0, 1.0);
	viscositySliderCW = RandomFloat(0.0, 1.0);
	viscositySliderCB = RandomFloat(0.0, 1.0);
	viscositySliderCO = RandomFloat(0.0, 1.0);
	viscositySliderCK = RandomFloat(0.0, 1.0);
	viscositySliderCC = RandomFloat(0.0, 1.0);
	viscositySliderCD = RandomFloat(0.0, 1.0);
	viscositySliderCY = RandomFloat(0.0, 1.0);
	viscositySliderCP = RandomFloat(0.0, 1.0);
	viscositySliderCM = RandomFloat(0.0, 1.0);
	viscositySliderCA = RandomFloat(0.0, 1.0);
	viscositySliderCT = RandomFloat(0.0, 1.0);
	viscositySliderCS = RandomFloat(0.0, 1.0);
	viscositySliderCF = RandomFloat(0.0, 1.0);

	viscositySliderDR = RandomFloat(0.0, 1.0);
	viscositySliderDG = RandomFloat(0.0, 1.0);
	viscositySliderDW = RandomFloat(0.0, 1.0);
	viscositySliderDB = RandomFloat(0.0, 1.0);
	viscositySliderDO = RandomFloat(0.0, 1.0);
	viscositySliderDK = RandomFloat(0.0, 1.0);
	viscositySliderDC = RandomFloat(0.0, 1.0);
	viscositySliderDD = RandomFloat(0.0, 1.0);
	viscositySliderDY = RandomFloat(0.0, 1.0);
	viscositySliderDP = RandomFloat(0.0, 1.0);
	viscositySliderDM = RandomFloat(0.0, 1.0);
	viscositySliderDA = RandomFloat(0.0, 1.0);
	viscositySliderDT = RandomFloat(0.0, 1.0);
	viscositySliderDS = RandomFloat(0.0, 1.0);
	viscositySliderDF = RandomFloat(0.0, 1.0);

	viscositySliderYR = RandomFloat(0.0, 1.0);
	viscositySliderYG = RandomFloat(0.0, 1.0);
	viscositySliderYW = RandomFloat(0.0, 1.0);
	viscositySliderYB = RandomFloat(0.0, 1.0);
	viscositySliderYO = RandomFloat(0.0, 1.0);
	viscositySliderYK = RandomFloat(0.0, 1.0);
	viscositySliderYC = RandomFloat(0.0, 1.0);
	viscositySliderYD = RandomFloat(0.0, 1.0);
	viscositySliderYY = RandomFloat(0.0, 1.0);
	viscositySliderYP = RandomFloat(0.0, 1.0);
	viscositySliderYM = RandomFloat(0.0, 1.0);
	viscositySliderYA = RandomFloat(0.0, 1.0);
	viscositySliderYT = RandomFloat(0.0, 1.0);
	viscositySliderYS = RandomFloat(0.0, 1.0);
	viscositySliderYF = RandomFloat(0.0, 1.0);

	viscositySliderPR = RandomFloat(0.0, 1.0);
	viscositySliderPG = RandomFloat(0.0, 1.0);
	viscositySliderPW = RandomFloat(0.0, 1.0);
	viscositySliderPB = RandomFloat(0.0, 1.0);
	viscositySliderPO = RandomFloat(0.0, 1.0);
	viscositySliderPK = RandomFloat(0.0, 1.0);
	viscositySliderPC = RandomFloat(0.0, 1.0);
	viscositySliderPD = RandomFloat(0.0, 1.0);
	viscositySliderPY = RandomFloat(0.0, 1.0);
	viscositySliderPP = RandomFloat(0.0, 1.0);
	viscositySliderPM = RandomFloat(0.0, 1.0);
	viscositySliderPA = RandomFloat(0.0, 1.0);
	viscositySliderPT = RandomFloat(0.0, 1.0);
	viscositySliderPS = RandomFloat(0.0, 1.0);
	viscositySliderPF = RandomFloat(0.0, 1.0);

	viscositySliderMR = RandomFloat(0.0, 1.0);
	viscositySliderMG = RandomFloat(0.0, 1.0);
	viscositySliderMW = RandomFloat(0.0, 1.0);
	viscositySliderMB = RandomFloat(0.0, 1.0);
	viscositySliderMO = RandomFloat(0.0, 1.0);
	viscositySliderMK = RandomFloat(0.0, 1.0);
	viscositySliderMC = RandomFloat(0.0, 1.0);
	viscositySliderMD = RandomFloat(0.0, 1.0);
	viscositySliderMY = RandomFloat(0.0, 1.0);
	viscositySliderMP = RandomFloat(0.0, 1.0);
	viscositySliderMM = RandomFloat(0.0, 1.0);
	viscositySliderMA = RandomFloat(0.0, 1.0);
	viscositySliderMT = RandomFloat(0.0, 1.0);
	viscositySliderMS = RandomFloat(0.0, 1.0);
	viscositySliderMF = RandomFloat(0.0, 1.0);

	viscositySliderAR = RandomFloat(0.0, 1.0);
	viscositySliderAG = RandomFloat(0.0, 1.0);
	viscositySliderAW = RandomFloat(0.0, 1.0);
	viscositySliderAB = RandomFloat(0.0, 1.0);
	viscositySliderAO = RandomFloat(0.0, 1.0);
	viscositySliderAK = RandomFloat(0.0, 1.0);
	viscositySliderAC = RandomFloat(0.0, 1.0);
	viscositySliderAD = RandomFloat(0.0, 1.0);
	viscositySliderAY = RandomFloat(0.0, 1.0);
	viscositySliderAP = RandomFloat(0.0, 1.0);
	viscositySliderAM = RandomFloat(0.0, 1.0);
	viscositySliderAA = RandomFloat(0.0, 1.0);
	viscositySliderAT = RandomFloat(0.0, 1.0);
	viscositySliderAS = RandomFloat(0.0, 1.0);
	viscositySliderAF = RandomFloat(0.0, 1.0);

	viscositySliderTR = RandomFloat(0.0, 1.0);
	viscositySliderTG = RandomFloat(0.0, 1.0);
	viscositySliderTW = RandomFloat(0.0, 1.0);
	viscositySliderTB = RandomFloat(0.0, 1.0);
	viscositySliderTO = RandomFloat(0.0, 1.0);
	viscositySliderTK = RandomFloat(0.0, 1.0);
	viscositySliderTC = RandomFloat(0.0, 1.0);
	viscositySliderTD = RandomFloat(0.0, 1.0);
	viscositySliderTY = RandomFloat(0.0, 1.0);
	viscositySliderTP = RandomFloat(0.0, 1.0);
	viscositySliderTM = RandomFloat(0.0, 1.0);
	viscositySliderTA = RandomFloat(0.0, 1.0);
	viscositySliderTT = RandomFloat(0.0, 1.0);
	viscositySliderTS = RandomFloat(0.0, 1.0);
	viscositySliderTF = RandomFloat(0.0, 1.0);

	viscositySliderSR = RandomFloat(0.0, 1.0);
	viscositySliderSG = RandomFloat(0.0, 1.0);
	viscositySliderSW = RandomFloat(0.0, 1.0);
	viscositySliderSB = RandomFloat(0.0, 1.0);
	viscositySliderSO = RandomFloat(0.0, 1.0);
	viscositySliderSK = RandomFloat(0.0, 1.0);
	viscositySliderSC = RandomFloat(0.0, 1.0);
	viscositySliderSD = RandomFloat(0.0, 1.0);
	viscositySliderSY = RandomFloat(0.0, 1.0);
	viscositySliderSP = RandomFloat(0.0, 1.0);
	viscositySliderSM = RandomFloat(0.0, 1.0);
	viscositySliderSA = RandomFloat(0.0, 1.0);
	viscositySliderST = RandomFloat(0.0, 1.0);
	viscositySliderSS = RandomFloat(0.0, 1.0);
	viscositySliderSF = RandomFloat(0.0, 1.0);

	viscositySliderFR = RandomFloat(0.0, 1.0);
	viscositySliderFG = RandomFloat(0.0, 1.0);
	viscositySliderFW = RandomFloat(0.0, 1.0);
	viscositySliderFB = RandomFloat(0.0, 1.0);
	viscositySliderFO = RandomFloat(0.0, 1.0);
	viscositySliderFK = RandomFloat(0.0, 1.0);
	viscositySliderFC = RandomFloat(0.0, 1.0);
	viscositySliderFD = RandomFloat(0.0, 1.0);
	viscositySliderFY = RandomFloat(0.0, 1.0);
	viscositySliderFP = RandomFloat(0.0, 1.0);
	viscositySliderFM = RandomFloat(0.0, 1.0);
	viscositySliderFA = RandomFloat(0.0, 1.0);
	viscositySliderFT = RandomFloat(0.0, 1.0);
	viscositySliderFS = RandomFloat(0.0, 1.0);
	viscositySliderFF = RandomFloat(0.0, 1.0);
}
void ofApp::rndprob() {
	probabilitySlider = RandomFloat(0, 100);

	probabilitySliderRR = RandomFloat(0, 100);
	probabilitySliderRG = RandomFloat(0, 100);
	probabilitySliderRB = RandomFloat(0, 100);
	probabilitySliderRW = RandomFloat(0, 100);
	probabilitySliderRO = RandomFloat(0, 100);
	probabilitySliderRK = RandomFloat(0, 100);
	probabilitySliderRC = RandomFloat(0, 100);
	probabilitySliderRD = RandomFloat(0, 100);
	probabilitySliderRY = RandomFloat(0, 100);
	probabilitySliderRP = RandomFloat(0, 100);
	probabilitySliderRM = RandomFloat(0, 100);
	probabilitySliderRA = RandomFloat(0, 100);
	probabilitySliderRT = RandomFloat(0, 100);
	probabilitySliderRS = RandomFloat(0, 100);
	probabilitySliderRF = RandomFloat(0, 100);

	probabilitySliderGR = RandomFloat(0, 100);
	probabilitySliderGG = RandomFloat(0, 100);
	probabilitySliderGB = RandomFloat(0, 100);
	probabilitySliderGW = RandomFloat(0, 100);
	probabilitySliderGO = RandomFloat(0, 100);
	probabilitySliderGK = RandomFloat(0, 100);
	probabilitySliderGC = RandomFloat(0, 100);
	probabilitySliderGD = RandomFloat(0, 100);
	probabilitySliderGY = RandomFloat(0, 100);
	probabilitySliderGP = RandomFloat(0, 100);
	probabilitySliderGM = RandomFloat(0, 100);
	probabilitySliderGA = RandomFloat(0, 100);
	probabilitySliderGT = RandomFloat(0, 100);
	probabilitySliderGS = RandomFloat(0, 100);
	probabilitySliderGF = RandomFloat(0, 100);

	probabilitySliderBR = RandomFloat(0, 100);
	probabilitySliderBG = RandomFloat(0, 100);
	probabilitySliderBB = RandomFloat(0, 100);
	probabilitySliderBW = RandomFloat(0, 100);
	probabilitySliderBO = RandomFloat(0, 100);
	probabilitySliderBK = RandomFloat(0, 100);
	probabilitySliderBC = RandomFloat(0, 100);
	probabilitySliderBD = RandomFloat(0, 100);
	probabilitySliderBY = RandomFloat(0, 100);
	probabilitySliderBP = RandomFloat(0, 100);
	probabilitySliderBM = RandomFloat(0, 100);
	probabilitySliderBA = RandomFloat(0, 100);
	probabilitySliderBT = RandomFloat(0, 100);
	probabilitySliderBS = RandomFloat(0, 100);
	probabilitySliderBF = RandomFloat(0, 100);

	probabilitySliderWR = RandomFloat(0, 100);
	probabilitySliderWG = RandomFloat(0, 100);
	probabilitySliderWB = RandomFloat(0, 100);
	probabilitySliderWW = RandomFloat(0, 100);
	probabilitySliderWO = RandomFloat(0, 100);
	probabilitySliderWK = RandomFloat(0, 100);
	probabilitySliderWC = RandomFloat(0, 100);
	probabilitySliderWD = RandomFloat(0, 100);
	probabilitySliderWY = RandomFloat(0, 100);
	probabilitySliderWP = RandomFloat(0, 100);
	probabilitySliderWM = RandomFloat(0, 100);
	probabilitySliderWA = RandomFloat(0, 100);
	probabilitySliderWT = RandomFloat(0, 100);
	probabilitySliderWS = RandomFloat(0, 100);
	probabilitySliderWF = RandomFloat(0, 100);

	probabilitySliderOR = RandomFloat(0, 100);
	probabilitySliderOG = RandomFloat(0, 100);
	probabilitySliderOB = RandomFloat(0, 100);
	probabilitySliderOW = RandomFloat(0, 100);
	probabilitySliderOO = RandomFloat(0, 100);
	probabilitySliderOK = RandomFloat(0, 100);
	probabilitySliderOC = RandomFloat(0, 100);
	probabilitySliderOD = RandomFloat(0, 100);
	probabilitySliderOY = RandomFloat(0, 100);
	probabilitySliderOP = RandomFloat(0, 100);
	probabilitySliderOM = RandomFloat(0, 100);
	probabilitySliderOA = RandomFloat(0, 100);
	probabilitySliderOT = RandomFloat(0, 100);
	probabilitySliderOS = RandomFloat(0, 100);
	probabilitySliderOF = RandomFloat(0, 100);

	probabilitySliderKR = RandomFloat(0, 100);
	probabilitySliderKG = RandomFloat(0, 100);
	probabilitySliderKB = RandomFloat(0, 100);
	probabilitySliderKW = RandomFloat(0, 100);
	probabilitySliderKO = RandomFloat(0, 100);
	probabilitySliderKK = RandomFloat(0, 100);
	probabilitySliderKC = RandomFloat(0, 100);
	probabilitySliderKD = RandomFloat(0, 100);
	probabilitySliderKY = RandomFloat(0, 100);
	probabilitySliderKP = RandomFloat(0, 100);
	probabilitySliderKM = RandomFloat(0, 100);
	probabilitySliderKA = RandomFloat(0, 100);
	probabilitySliderKT = RandomFloat(0, 100);
	probabilitySliderKS = RandomFloat(0, 100);
	probabilitySliderKF = RandomFloat(0, 100);

	probabilitySliderCR = RandomFloat(0, 100);
	probabilitySliderCG = RandomFloat(0, 100);
	probabilitySliderCB = RandomFloat(0, 100);
	probabilitySliderCW = RandomFloat(0, 100);
	probabilitySliderCO = RandomFloat(0, 100);
	probabilitySliderCK = RandomFloat(0, 100);
	probabilitySliderCC = RandomFloat(0, 100);
	probabilitySliderCD = RandomFloat(0, 100);
	probabilitySliderCY = RandomFloat(0, 100);
	probabilitySliderCP = RandomFloat(0, 100);
	probabilitySliderCM = RandomFloat(0, 100);
	probabilitySliderCA = RandomFloat(0, 100);
	probabilitySliderCT = RandomFloat(0, 100);
	probabilitySliderCS = RandomFloat(0, 100);
	probabilitySliderCF = RandomFloat(0, 100);

	probabilitySliderDR = RandomFloat(0, 100);
	probabilitySliderDG = RandomFloat(0, 100);
	probabilitySliderDB = RandomFloat(0, 100);
	probabilitySliderDW = RandomFloat(0, 100);
	probabilitySliderDO = RandomFloat(0, 100);
	probabilitySliderDK = RandomFloat(0, 100);
	probabilitySliderDC = RandomFloat(0, 100);
	probabilitySliderDD = RandomFloat(0, 100);
	probabilitySliderDY = RandomFloat(0, 100);
	probabilitySliderDP = RandomFloat(0, 100);
	probabilitySliderDM = RandomFloat(0, 100);
	probabilitySliderDA = RandomFloat(0, 100);
	probabilitySliderDT = RandomFloat(0, 100);
	probabilitySliderDS = RandomFloat(0, 100);
	probabilitySliderDF = RandomFloat(0, 100);

	probabilitySliderYR = RandomFloat(0, 100);
	probabilitySliderYG = RandomFloat(0, 100);
	probabilitySliderYB = RandomFloat(0, 100);
	probabilitySliderYW = RandomFloat(0, 100);
	probabilitySliderYO = RandomFloat(0, 100);
	probabilitySliderYK = RandomFloat(0, 100);
	probabilitySliderYC = RandomFloat(0, 100);
	probabilitySliderYD = RandomFloat(0, 100);
	probabilitySliderYY = RandomFloat(0, 100);
	probabilitySliderYP = RandomFloat(0, 100);
	probabilitySliderYM = RandomFloat(0, 100);
	probabilitySliderYA = RandomFloat(0, 100);
	probabilitySliderYT = RandomFloat(0, 100);
	probabilitySliderYS = RandomFloat(0, 100);
	probabilitySliderYF = RandomFloat(0, 100);

	probabilitySliderPR = RandomFloat(0, 100);
	probabilitySliderPG = RandomFloat(0, 100);
	probabilitySliderPB = RandomFloat(0, 100);
	probabilitySliderPW = RandomFloat(0, 100);
	probabilitySliderPO = RandomFloat(0, 100);
	probabilitySliderPK = RandomFloat(0, 100);
	probabilitySliderPC = RandomFloat(0, 100);
	probabilitySliderPD = RandomFloat(0, 100);
	probabilitySliderPY = RandomFloat(0, 100);
	probabilitySliderPP = RandomFloat(0, 100);
	probabilitySliderPM = RandomFloat(0, 100);
	probabilitySliderPA = RandomFloat(0, 100);
	probabilitySliderPT = RandomFloat(0, 100);
	probabilitySliderPS = RandomFloat(0, 100);
	probabilitySliderPF = RandomFloat(0, 100);

	probabilitySliderMR = RandomFloat(0, 100);
	probabilitySliderMG = RandomFloat(0, 100);
	probabilitySliderMB = RandomFloat(0, 100);
	probabilitySliderMW = RandomFloat(0, 100);
	probabilitySliderMO = RandomFloat(0, 100);
	probabilitySliderMK = RandomFloat(0, 100);
	probabilitySliderMC = RandomFloat(0, 100);
	probabilitySliderMD = RandomFloat(0, 100);
	probabilitySliderMY = RandomFloat(0, 100);
	probabilitySliderMP = RandomFloat(0, 100);
	probabilitySliderMM = RandomFloat(0, 100);
	probabilitySliderMA = RandomFloat(0, 100);
	probabilitySliderMT = RandomFloat(0, 100);
	probabilitySliderMS = RandomFloat(0, 100);
	probabilitySliderMF = RandomFloat(0, 100);

	probabilitySliderAR = RandomFloat(0, 100);
	probabilitySliderAG = RandomFloat(0, 100);
	probabilitySliderAB = RandomFloat(0, 100);
	probabilitySliderAW = RandomFloat(0, 100);
	probabilitySliderAO = RandomFloat(0, 100);
	probabilitySliderAK = RandomFloat(0, 100);
	probabilitySliderAC = RandomFloat(0, 100);
	probabilitySliderAD = RandomFloat(0, 100);
	probabilitySliderAY = RandomFloat(0, 100);
	probabilitySliderAP = RandomFloat(0, 100);
	probabilitySliderAM = RandomFloat(0, 100);
	probabilitySliderAA = RandomFloat(0, 100);
	probabilitySliderAT = RandomFloat(0, 100);
	probabilitySliderAS = RandomFloat(0, 100);
	probabilitySliderAF = RandomFloat(0, 100);

	probabilitySliderTR = RandomFloat(0, 100);
	probabilitySliderTG = RandomFloat(0, 100);
	probabilitySliderTB = RandomFloat(0, 100);
	probabilitySliderTW = RandomFloat(0, 100);
	probabilitySliderTO = RandomFloat(0, 100);
	probabilitySliderTK = RandomFloat(0, 100);
	probabilitySliderTC = RandomFloat(0, 100);
	probabilitySliderTD = RandomFloat(0, 100);
	probabilitySliderTY = RandomFloat(0, 100);
	probabilitySliderTP = RandomFloat(0, 100);
	probabilitySliderTM = RandomFloat(0, 100);
	probabilitySliderTA = RandomFloat(0, 100);
	probabilitySliderTT = RandomFloat(0, 100);
	probabilitySliderTS = RandomFloat(0, 100);
	probabilitySliderTF = RandomFloat(0, 100);

	probabilitySliderSR = RandomFloat(0, 100);
	probabilitySliderSG = RandomFloat(0, 100);
	probabilitySliderSB = RandomFloat(0, 100);
	probabilitySliderSW = RandomFloat(0, 100);
	probabilitySliderSO = RandomFloat(0, 100);
	probabilitySliderSK = RandomFloat(0, 100);
	probabilitySliderSC = RandomFloat(0, 100);
	probabilitySliderSD = RandomFloat(0, 100);
	probabilitySliderSY = RandomFloat(0, 100);
	probabilitySliderSP = RandomFloat(0, 100);
	probabilitySliderSM = RandomFloat(0, 100);
	probabilitySliderSA = RandomFloat(0, 100);
	probabilitySliderST = RandomFloat(0, 100);
	probabilitySliderSS = RandomFloat(0, 100);
	probabilitySliderSF = RandomFloat(0, 100);

	probabilitySliderFR = RandomFloat(0, 100);
	probabilitySliderFG = RandomFloat(0, 100);
	probabilitySliderFB = RandomFloat(0, 100);
	probabilitySliderFW = RandomFloat(0, 100);
	probabilitySliderFO = RandomFloat(0, 100);
	probabilitySliderFK = RandomFloat(0, 100);
	probabilitySliderFC = RandomFloat(0, 100);
	probabilitySliderFD = RandomFloat(0, 100);
	probabilitySliderFY = RandomFloat(0, 100);
	probabilitySliderFP = RandomFloat(0, 100);
	probabilitySliderFM = RandomFloat(0, 100);
	probabilitySliderFA = RandomFloat(0, 100);
	probabilitySliderFT = RandomFloat(0, 100);
	probabilitySliderFS = RandomFloat(0, 100);
	probabilitySliderFF = RandomFloat(0, 100);
}
void ofApp::rndir() {
	// GREEN
	//numberSliderG = RandomFloat(0, 3000);
	powerSliderGG = RandomFloat(0, 100) * forceVariance;
	powerSliderGR = RandomFloat(0, 100) * forceVariance;
	powerSliderGW = RandomFloat(0, 100) * forceVariance;
	powerSliderGB = RandomFloat(0, 100) * forceVariance;
	powerSliderGO = RandomFloat(0, 100) * forceVariance;
	powerSliderGK = RandomFloat(0, 100) * forceVariance;
	powerSliderGC = RandomFloat(0, 100) * forceVariance;
	powerSliderGD = RandomFloat(0, 100) * forceVariance;
	powerSliderGY = RandomFloat(0, 100) * forceVariance;
	powerSliderGP = RandomFloat(0, 100) * forceVariance;
	powerSliderGM = RandomFloat(0, 100) * forceVariance;
	powerSliderGA = RandomFloat(0, 100) * forceVariance;
	powerSliderGT = RandomFloat(0, 100) * forceVariance;
	powerSliderGS = RandomFloat(0, 100) * forceVariance;
	powerSliderGF = RandomFloat(0, 100) * forceVariance;

	vSliderGG = RandomFloat(0, 500) * radiusVariance;
	vSliderGR = RandomFloat(0, 500) * radiusVariance;
	vSliderGW = RandomFloat(0, 500) * radiusVariance;
	vSliderGB = RandomFloat(0, 500) * radiusVariance;
	vSliderGO = RandomFloat(0, 500) * radiusVariance;
	vSliderGK = RandomFloat(0, 500) * radiusVariance;
	vSliderGC = RandomFloat(0, 500) * radiusVariance;
	vSliderGD = RandomFloat(0, 500) * radiusVariance;
	vSliderGY = RandomFloat(0, 500) * radiusVariance;
	vSliderGP = RandomFloat(0, 500) * radiusVariance;
	vSliderGM = RandomFloat(0, 500) * radiusVariance;
	vSliderGA = RandomFloat(0, 500) * radiusVariance;
	vSliderGT = RandomFloat(0, 500) * radiusVariance;
	vSliderGS = RandomFloat(0, 500) * radiusVariance;
	vSliderGF = RandomFloat(0, 500) * radiusVariance;

	// RED
	//numberSliderR = RandomFloat(0, 3000);
	powerSliderRR = RandomFloat(0, 100) * forceVariance;
	powerSliderRG = RandomFloat(0, 100) * forceVariance;
	powerSliderRW = RandomFloat(0, 100) * forceVariance;
	powerSliderRB = RandomFloat(0, 100) * forceVariance;
	powerSliderRO = RandomFloat(0, 100) * forceVariance;
	powerSliderRK = RandomFloat(0, 100) * forceVariance;
	powerSliderRC = RandomFloat(0, 100) * forceVariance;
	powerSliderRD = RandomFloat(0, 100) * forceVariance;
	powerSliderRY = RandomFloat(0, 100) * forceVariance;
	powerSliderRP = RandomFloat(0, 100) * forceVariance;
	powerSliderRM = RandomFloat(0, 100) * forceVariance;
	powerSliderRA = RandomFloat(0, 100) * forceVariance;
	powerSliderRT = RandomFloat(0, 100) * forceVariance;
	powerSliderRS = RandomFloat(0, 100) * forceVariance;
	powerSliderRF = RandomFloat(0, 100) * forceVariance;

	vSliderRG = RandomFloat(0, 500) * radiusVariance;
	vSliderRR = RandomFloat(0, 500) * radiusVariance;
	vSliderRW = RandomFloat(0, 500) * radiusVariance;
	vSliderRB = RandomFloat(0, 500) * radiusVariance;
	vSliderRO = RandomFloat(0, 500) * radiusVariance;
	vSliderRK = RandomFloat(0, 500) * radiusVariance;
	vSliderRC = RandomFloat(0, 500) * radiusVariance;
	vSliderRD = RandomFloat(0, 500) * radiusVariance;
	vSliderRY = RandomFloat(0, 500) * radiusVariance;
	vSliderRP = RandomFloat(0, 500) * radiusVariance;
	vSliderRM = RandomFloat(0, 500) * radiusVariance;
	vSliderRA = RandomFloat(0, 500) * radiusVariance;
	vSliderRT = RandomFloat(0, 500) * radiusVariance;
	vSliderRS = RandomFloat(0, 500) * radiusVariance;
	vSliderRF = RandomFloat(0, 500) * radiusVariance;

	// WHITE
	// numberSliderW = RandomFloat(0, 3000);
	powerSliderWW = RandomFloat(0, 100) * forceVariance;
	powerSliderWR = RandomFloat(0, 100) * forceVariance;
	powerSliderWG = RandomFloat(0, 100) * forceVariance;
	powerSliderWB = RandomFloat(0, 100) * forceVariance;
	powerSliderWO = RandomFloat(0, 100) * forceVariance;
	powerSliderWK = RandomFloat(0, 100) * forceVariance;
	powerSliderWC = RandomFloat(0, 100) * forceVariance;
	powerSliderWD = RandomFloat(0, 100) * forceVariance;
	powerSliderWY = RandomFloat(0, 100) * forceVariance;
	powerSliderWP = RandomFloat(0, 100) * forceVariance;
	powerSliderWM = RandomFloat(0, 100) * forceVariance;
	powerSliderWA = RandomFloat(0, 100) * forceVariance;
	powerSliderWT = RandomFloat(0, 100) * forceVariance;
	powerSliderWS = RandomFloat(0, 100) * forceVariance;
	powerSliderWF = RandomFloat(0, 100) * forceVariance;

	vSliderWG = RandomFloat(0, 500) * radiusVariance;
	vSliderWR = RandomFloat(0, 500) * radiusVariance;
	vSliderWW = RandomFloat(0, 500) * radiusVariance;
	vSliderWB = RandomFloat(0, 500) * radiusVariance;
	vSliderWO = RandomFloat(0, 500) * radiusVariance;
	vSliderWK = RandomFloat(0, 500) * radiusVariance;
	vSliderWC = RandomFloat(0, 500) * radiusVariance;
	vSliderWD = RandomFloat(0, 500) * radiusVariance;
	vSliderWY = RandomFloat(0, 500) * radiusVariance;
	vSliderWP = RandomFloat(0, 500) * radiusVariance;
	vSliderWM = RandomFloat(0, 500) * radiusVariance;
	vSliderWA = RandomFloat(0, 500) * radiusVariance;
	vSliderWT = RandomFloat(0, 500) * radiusVariance;
	vSliderWS = RandomFloat(0, 500) * radiusVariance;
	vSliderWF = RandomFloat(0, 500) * radiusVariance;

	// BLUE
	//numberSliderB = RandomFloat(0, 3000);
	powerSliderBB = RandomFloat(0, 100) * forceVariance;
	powerSliderBW = RandomFloat(0, 100) * forceVariance;
	powerSliderBR = RandomFloat(0, 100) * forceVariance;
	powerSliderBG = RandomFloat(0, 100) * forceVariance;
	powerSliderBO = RandomFloat(0, 100) * forceVariance;
	powerSliderBK = RandomFloat(0, 100) * forceVariance;
	powerSliderBC = RandomFloat(0, 100) * forceVariance;
	powerSliderBD = RandomFloat(0, 100) * forceVariance;
	powerSliderBY = RandomFloat(0, 100) * forceVariance;
	powerSliderBP = RandomFloat(0, 100) * forceVariance;
	powerSliderBM = RandomFloat(0, 100) * forceVariance;
	powerSliderBA = RandomFloat(0, 100) * forceVariance;
	powerSliderBT = RandomFloat(0, 100) * forceVariance;
	powerSliderBS = RandomFloat(0, 100) * forceVariance;
	powerSliderBF = RandomFloat(0, 100) * forceVariance;

	vSliderBG = RandomFloat(0, 500) * radiusVariance;
	vSliderBR = RandomFloat(0, 500) * radiusVariance;
	vSliderBW = RandomFloat(0, 500) * radiusVariance;
	vSliderBB = RandomFloat(0, 500) * radiusVariance;
	vSliderBO = RandomFloat(0, 500) * radiusVariance;
	vSliderBK = RandomFloat(0, 500) * radiusVariance;
	vSliderBC = RandomFloat(0, 500) * radiusVariance;
	vSliderBD = RandomFloat(0, 500) * radiusVariance;
	vSliderBY = RandomFloat(0, 500) * radiusVariance;
	vSliderBP = RandomFloat(0, 500) * radiusVariance;
	vSliderBM = RandomFloat(0, 500) * radiusVariance;
	vSliderBA = RandomFloat(0, 500) * radiusVariance;
	vSliderBT = RandomFloat(0, 500) * radiusVariance;
	vSliderBS = RandomFloat(0, 500) * radiusVariance;
	vSliderBF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderOY = RandomFloat(-100, 100) * forceVariance;
	powerSliderOP = RandomFloat(-100, 100) * forceVariance;
	powerSliderOM = RandomFloat(-100, 100) * forceVariance;
	powerSliderOA = RandomFloat(-100, 100) * forceVariance;
	powerSliderOT = RandomFloat(-100, 100) * forceVariance;
	powerSliderOS = RandomFloat(-100, 100) * forceVariance;
	powerSliderOF = RandomFloat(-100, 100) * forceVariance;

	vSliderOG = RandomFloat(0, 500) * radiusVariance;
	vSliderOR = RandomFloat(0, 500) * radiusVariance;
	vSliderOW = RandomFloat(0, 500) * radiusVariance;
	vSliderOB = RandomFloat(0, 500) * radiusVariance;
	vSliderOO = RandomFloat(0, 500) * radiusVariance;
	vSliderOK = RandomFloat(0, 500) * radiusVariance;
	vSliderOC = RandomFloat(0, 500) * radiusVariance;
	vSliderOD = RandomFloat(0, 500) * radiusVariance;
	vSliderOY = RandomFloat(0, 500) * radiusVariance;
	vSliderOP = RandomFloat(0, 500) * radiusVariance;
	vSliderOM = RandomFloat(0, 500) * radiusVariance;
	vSliderOA = RandomFloat(0, 500) * radiusVariance;
	vSliderOT = RandomFloat(0, 500) * radiusVariance;
	vSliderOS = RandomFloat(0, 500) * radiusVariance;
	vSliderOF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderKY = RandomFloat(-100, 100) * forceVariance;
	powerSliderKP = RandomFloat(-100, 100) * forceVariance;
	powerSliderKM = RandomFloat(-100, 100) * forceVariance;
	powerSliderKA = RandomFloat(-100, 100) * forceVariance;
	powerSliderKT = RandomFloat(-100, 100) * forceVariance;
	powerSliderKS = RandomFloat(-100, 100) * forceVariance;
	powerSliderKF = RandomFloat(-100, 100) * forceVariance;

	vSliderKG = RandomFloat(0, 500) * radiusVariance;
	vSliderKR = RandomFloat(0, 500) * radiusVariance;
	vSliderKW = RandomFloat(0, 500) * radiusVariance;
	vSliderKB = RandomFloat(0, 500) * radiusVariance;
	vSliderKO = RandomFloat(0, 500) * radiusVariance;
	vSliderKK = RandomFloat(0, 500) * radiusVariance;
	vSliderKC = RandomFloat(0, 500) * radiusVariance;
	vSliderKD = RandomFloat(0, 500) * radiusVariance;
	vSliderKY = RandomFloat(0, 500) * radiusVariance;
	vSliderKP = RandomFloat(0, 500) * radiusVariance;
	vSliderKM = RandomFloat(0, 500) * radiusVariance;
	vSliderKA = RandomFloat(0, 500) * radiusVariance;
	vSliderKT = RandomFloat(0, 500) * radiusVariance;
	vSliderKS = RandomFloat(0, 500) * radiusVariance;
	vSliderKF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderCY = RandomFloat(-100, 100) * forceVariance;
	powerSliderCP = RandomFloat(-100, 100) * forceVariance;
	powerSliderCM = RandomFloat(-100, 100) * forceVariance;
	powerSliderCA = RandomFloat(-100, 100) * forceVariance;
	powerSliderCT = RandomFloat(-100, 100) * forceVariance;
	powerSliderCS = RandomFloat(-100, 100) * forceVariance;
	powerSliderCF = RandomFloat(-100, 100) * forceVariance;

	vSliderCG = RandomFloat(0, 500) * radiusVariance;
	vSliderCR = RandomFloat(0, 500) * radiusVariance;
	vSliderCW = RandomFloat(0, 500) * radiusVariance;
	vSliderCB = RandomFloat(0, 500) * radiusVariance;
	vSliderCO = RandomFloat(0, 500) * radiusVariance;
	vSliderCK = RandomFloat(0, 500) * radiusVariance;
	vSliderCC = RandomFloat(0, 500) * radiusVariance;
	vSliderCD = RandomFloat(0, 500) * radiusVariance;
	vSliderCY = RandomFloat(0, 500) * radiusVariance;
	vSliderCP = RandomFloat(0, 500) * radiusVariance;
	vSliderCM = RandomFloat(0, 500) * radiusVariance;
	vSliderCA = RandomFloat(0, 500) * radiusVariance;
	vSliderCT = RandomFloat(0, 500) * radiusVariance;
	vSliderCS = RandomFloat(0, 500) * radiusVariance;
	vSliderCF = RandomFloat(0, 500) * radiusVariance;

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
	powerSliderDY = RandomFloat(-100, 100) * forceVariance;
	powerSliderDP = RandomFloat(-100, 100) * forceVariance;
	powerSliderDM = RandomFloat(-100, 100) * forceVariance;
	powerSliderDA = RandomFloat(-100, 100) * forceVariance;
	powerSliderDT = RandomFloat(-100, 100) * forceVariance;
	powerSliderDS = RandomFloat(-100, 100) * forceVariance;
	powerSliderDF = RandomFloat(-100, 100) * forceVariance;

	vSliderDG = RandomFloat(0, 500) * radiusVariance;
	vSliderDR = RandomFloat(0, 500) * radiusVariance;
	vSliderDW = RandomFloat(0, 500) * radiusVariance;
	vSliderDB = RandomFloat(0, 500) * radiusVariance;
	vSliderDO = RandomFloat(0, 500) * radiusVariance;
	vSliderDK = RandomFloat(0, 500) * radiusVariance;
	vSliderDC = RandomFloat(0, 500) * radiusVariance;
	vSliderDD = RandomFloat(0, 500) * radiusVariance;
	vSliderDY = RandomFloat(0, 500) * radiusVariance;
	vSliderDP = RandomFloat(0, 500) * radiusVariance;
	vSliderDM = RandomFloat(0, 500) * radiusVariance;
	vSliderDA = RandomFloat(0, 500) * radiusVariance;
	vSliderDT = RandomFloat(0, 500) * radiusVariance;
	vSliderDS = RandomFloat(0, 500) * radiusVariance;
	vSliderDF = RandomFloat(0, 500) * radiusVariance;

	// YELLOW
	//numberSliderY = RandomFloat(0, 3000);
	powerSliderYB = RandomFloat(-100, 0) * forceVariance;
	powerSliderYW = RandomFloat(-100, 0) * forceVariance;
	powerSliderYR = RandomFloat(-100, 0) * forceVariance;
	powerSliderYG = RandomFloat(-100, 0) * forceVariance;
	powerSliderYO = RandomFloat(-100, 0) * forceVariance;
	powerSliderYK = RandomFloat(-100, 0) * forceVariance;
	powerSliderYC = RandomFloat(-100, 0) * forceVariance;
	powerSliderYD = RandomFloat(-100, 0) * forceVariance;
	powerSliderYY = RandomFloat(-100, 0) * forceVariance;
	powerSliderYP = RandomFloat(-100, 0) * forceVariance;
	powerSliderYM = RandomFloat(-100, 0) * forceVariance;
	powerSliderYA = RandomFloat(-100, 0) * forceVariance;
	powerSliderYT = RandomFloat(-100, 0) * forceVariance;
	powerSliderYS = RandomFloat(-100, 0) * forceVariance;
	powerSliderYF = RandomFloat(-100, 0) * forceVariance;

	vSliderYG = RandomFloat(0, 500) * radiusVariance;
	vSliderYR = RandomFloat(0, 500) * radiusVariance;
	vSliderYW = RandomFloat(0, 500) * radiusVariance;
	vSliderYB = RandomFloat(0, 500) * radiusVariance;
	vSliderYO = RandomFloat(0, 500) * radiusVariance;
	vSliderYK = RandomFloat(0, 500) * radiusVariance;
	vSliderYC = RandomFloat(0, 500) * radiusVariance;
	vSliderYD = RandomFloat(0, 500) * radiusVariance;
	vSliderYY = RandomFloat(0, 500) * radiusVariance;
	vSliderYP = RandomFloat(0, 500) * radiusVariance;
	vSliderYM = RandomFloat(0, 500) * radiusVariance;
	vSliderYA = RandomFloat(0, 500) * radiusVariance;
	vSliderYT = RandomFloat(0, 500) * radiusVariance;
	vSliderYS = RandomFloat(0, 500) * radiusVariance;
	vSliderYF = RandomFloat(0, 500) * radiusVariance;

	// PINK
	//numberSliderP = RandomFloat(0, 3000);
	powerSliderPB = RandomFloat(-100, 0) * forceVariance;
	powerSliderPW = RandomFloat(-100, 0) * forceVariance;
	powerSliderPR = RandomFloat(-100, 0) * forceVariance;
	powerSliderPG = RandomFloat(-100, 0) * forceVariance;
	powerSliderPO = RandomFloat(-100, 0) * forceVariance;
	powerSliderPK = RandomFloat(-100, 0) * forceVariance;
	powerSliderPC = RandomFloat(-100, 0) * forceVariance;
	powerSliderPD = RandomFloat(-100, 0) * forceVariance;
	powerSliderPY = RandomFloat(-100, 0) * forceVariance;
	powerSliderPP = RandomFloat(-100, 0) * forceVariance;
	powerSliderPM = RandomFloat(-100, 0) * forceVariance;
	powerSliderPA = RandomFloat(-100, 0) * forceVariance;
	powerSliderPT = RandomFloat(-100, 0) * forceVariance;
	powerSliderPS = RandomFloat(-100, 0) * forceVariance;
	powerSliderPF = RandomFloat(-100, 0) * forceVariance;

	vSliderPG = RandomFloat(0, 500) * radiusVariance;
	vSliderPR = RandomFloat(0, 500) * radiusVariance;
	vSliderPW = RandomFloat(0, 500) * radiusVariance;
	vSliderPB = RandomFloat(0, 500) * radiusVariance;
	vSliderPO = RandomFloat(0, 500) * radiusVariance;
	vSliderPK = RandomFloat(0, 500) * radiusVariance;
	vSliderPC = RandomFloat(0, 500) * radiusVariance;
	vSliderPD = RandomFloat(0, 500) * radiusVariance;
	vSliderPY = RandomFloat(0, 500) * radiusVariance;
	vSliderPP = RandomFloat(0, 500) * radiusVariance;
	vSliderPM = RandomFloat(0, 500) * radiusVariance;
	vSliderPA = RandomFloat(0, 500) * radiusVariance;
	vSliderPT = RandomFloat(0, 500) * radiusVariance;
	vSliderPS = RandomFloat(0, 500) * radiusVariance;
	vSliderPF = RandomFloat(0, 500) * radiusVariance;

	// MAGENTA
	//numberSliderM = RandomFloat(0, 3000);
	powerSliderMB = RandomFloat(-100, 0) * forceVariance;
	powerSliderMW = RandomFloat(-100, 0) * forceVariance;
	powerSliderMR = RandomFloat(-100, 0) * forceVariance;
	powerSliderMG = RandomFloat(-100, 0) * forceVariance;
	powerSliderMO = RandomFloat(-100, 0) * forceVariance;
	powerSliderMK = RandomFloat(-100, 0) * forceVariance;
	powerSliderMC = RandomFloat(-100, 0) * forceVariance;
	powerSliderMD = RandomFloat(-100, 0) * forceVariance;
	powerSliderMY = RandomFloat(-100, 0) * forceVariance;
	powerSliderMP = RandomFloat(-100, 0) * forceVariance;
	powerSliderMM = RandomFloat(-100, 0) * forceVariance;
	powerSliderMA = RandomFloat(-100, 0) * forceVariance;
	powerSliderMT = RandomFloat(-100, 0) * forceVariance;
	powerSliderMS = RandomFloat(-100, 0) * forceVariance;
	powerSliderMF = RandomFloat(-100, 0) * forceVariance;

	vSliderMG = RandomFloat(0, 500) * radiusVariance;
	vSliderMR = RandomFloat(0, 500) * radiusVariance;
	vSliderMW = RandomFloat(0, 500) * radiusVariance;
	vSliderMB = RandomFloat(0, 500) * radiusVariance;
	vSliderMO = RandomFloat(0, 500) * radiusVariance;
	vSliderMK = RandomFloat(0, 500) * radiusVariance;
	vSliderMC = RandomFloat(0, 500) * radiusVariance;
	vSliderMD = RandomFloat(0, 500) * radiusVariance;
	vSliderMY = RandomFloat(0, 500) * radiusVariance;
	vSliderMP = RandomFloat(0, 500) * radiusVariance;
	vSliderMM = RandomFloat(0, 500) * radiusVariance;
	vSliderMA = RandomFloat(0, 500) * radiusVariance;
	vSliderMT = RandomFloat(0, 500) * radiusVariance;
	vSliderMS = RandomFloat(0, 500) * radiusVariance;
	vSliderMF = RandomFloat(0, 500) * radiusVariance;

	// AQUA
	//numberSliderA = RandomFloat(0, 3000);
	powerSliderAB = RandomFloat(-100, 0) * forceVariance;
	powerSliderAW = RandomFloat(-100, 0) * forceVariance;
	powerSliderAR = RandomFloat(-100, 0) * forceVariance;
	powerSliderAG = RandomFloat(-100, 0) * forceVariance;
	powerSliderAO = RandomFloat(-100, 0) * forceVariance;
	powerSliderAK = RandomFloat(-100, 0) * forceVariance;
	powerSliderAC = RandomFloat(-100, 0) * forceVariance;
	powerSliderAD = RandomFloat(-100, 0) * forceVariance;
	powerSliderAY = RandomFloat(-100, 0) * forceVariance;
	powerSliderAP = RandomFloat(-100, 0) * forceVariance;
	powerSliderAM = RandomFloat(-100, 0) * forceVariance;
	powerSliderAA = RandomFloat(-100, 0) * forceVariance;
	powerSliderAT = RandomFloat(-100, 0) * forceVariance;
	powerSliderAS = RandomFloat(-100, 0) * forceVariance;
	powerSliderAF = RandomFloat(-100, 0) * forceVariance;

	vSliderAG = RandomFloat(0, 500) * radiusVariance;
	vSliderAR = RandomFloat(0, 500) * radiusVariance;
	vSliderAW = RandomFloat(0, 500) * radiusVariance;
	vSliderAB = RandomFloat(0, 500) * radiusVariance;
	vSliderAO = RandomFloat(0, 500) * radiusVariance;
	vSliderAK = RandomFloat(0, 500) * radiusVariance;
	vSliderAC = RandomFloat(0, 500) * radiusVariance;
	vSliderAD = RandomFloat(0, 500) * radiusVariance;
	vSliderAY = RandomFloat(0, 500) * radiusVariance;
	vSliderAP = RandomFloat(0, 500) * radiusVariance;
	vSliderAM = RandomFloat(0, 500) * radiusVariance;
	vSliderAA = RandomFloat(0, 500) * radiusVariance;
	vSliderAT = RandomFloat(0, 500) * radiusVariance;
	vSliderAS = RandomFloat(0, 500) * radiusVariance;
	vSliderAF = RandomFloat(0, 500) * radiusVariance;

	// TEAL
	//numberSliderT = RandomFloat(0, 3000);
	powerSliderTB = RandomFloat(0, 100) * forceVariance;
	powerSliderTW = RandomFloat(0, 100) * forceVariance;
	powerSliderTR = RandomFloat(0, 100) * forceVariance;
	powerSliderTG = RandomFloat(0, 100) * forceVariance;
	powerSliderTO = RandomFloat(0, 100) * forceVariance;
	powerSliderTK = RandomFloat(0, 100) * forceVariance;
	powerSliderTC = RandomFloat(0, 100) * forceVariance;
	powerSliderTD = RandomFloat(0, 100) * forceVariance;
	powerSliderTY = RandomFloat(0, 100) * forceVariance;
	powerSliderTP = RandomFloat(0, 100) * forceVariance;
	powerSliderTM = RandomFloat(0, 100) * forceVariance;
	powerSliderTA = RandomFloat(0, 100) * forceVariance;
	powerSliderTT = RandomFloat(0, 100) * forceVariance;
	powerSliderTS = RandomFloat(0, 100) * forceVariance;
	powerSliderTF = RandomFloat(0, 100) * forceVariance;

	vSliderTG = RandomFloat(0, 500) * radiusVariance;
	vSliderTR = RandomFloat(0, 500) * radiusVariance;
	vSliderTW = RandomFloat(0, 500) * radiusVariance;
	vSliderTB = RandomFloat(0, 500) * radiusVariance;
	vSliderTO = RandomFloat(0, 500) * radiusVariance;
	vSliderTK = RandomFloat(0, 500) * radiusVariance;
	vSliderTC = RandomFloat(0, 500) * radiusVariance;
	vSliderTD = RandomFloat(0, 500) * radiusVariance;
	vSliderTY = RandomFloat(0, 500) * radiusVariance;
	vSliderTP = RandomFloat(0, 500) * radiusVariance;
	vSliderTM = RandomFloat(0, 500) * radiusVariance;
	vSliderTA = RandomFloat(0, 500) * radiusVariance;
	vSliderTT = RandomFloat(0, 500) * radiusVariance;
	vSliderTS = RandomFloat(0, 500) * radiusVariance;
	vSliderTF = RandomFloat(0, 500) * radiusVariance;

	// SILVER
	//numberSliderS = RandomFloat(0, 3000);
	powerSliderSB = RandomFloat(-100, 100) * forceVariance;
	powerSliderSW = RandomFloat(-100, 100) * forceVariance;
	powerSliderSR = RandomFloat(-100, 100) * forceVariance;
	powerSliderSG = RandomFloat(-100, 100) * forceVariance;
	powerSliderSO = RandomFloat(-100, 100) * forceVariance;
	powerSliderSK = RandomFloat(-100, 100) * forceVariance;
	powerSliderSC = RandomFloat(-100, 100) * forceVariance;
	powerSliderSD = RandomFloat(-100, 100) * forceVariance;
	powerSliderSY = RandomFloat(-100, 100) * forceVariance;
	powerSliderSP = RandomFloat(-100, 100) * forceVariance;
	powerSliderSM = RandomFloat(-100, 100) * forceVariance;
	powerSliderSA = RandomFloat(-100, 100) * forceVariance;
	powerSliderST = RandomFloat(-100, 100) * forceVariance;
	powerSliderSS = RandomFloat(-100, 100) * forceVariance;
	powerSliderSF = RandomFloat(-100, 100) * forceVariance;

	vSliderSG = RandomFloat(0, 500) * radiusVariance;
	vSliderSR = RandomFloat(0, 500) * radiusVariance;
	vSliderSW = RandomFloat(0, 500) * radiusVariance;
	vSliderSB = RandomFloat(0, 500) * radiusVariance;
	vSliderSO = RandomFloat(0, 500) * radiusVariance;
	vSliderSK = RandomFloat(0, 500) * radiusVariance;
	vSliderSC = RandomFloat(0, 500) * radiusVariance;
	vSliderSD = RandomFloat(0, 500) * radiusVariance;
	vSliderSY = RandomFloat(0, 500) * radiusVariance;
	vSliderSP = RandomFloat(0, 500) * radiusVariance;
	vSliderSM = RandomFloat(0, 500) * radiusVariance;
	vSliderSA = RandomFloat(0, 500) * radiusVariance;
	vSliderST = RandomFloat(0, 500) * radiusVariance;
	vSliderSS = RandomFloat(0, 500) * radiusVariance;
	vSliderSF = RandomFloat(0, 500) * radiusVariance;

	// FIREBRICK
	//numberSliderF = RandomFloat(0, 3000);
	powerSliderFB = RandomFloat(-100, 0) * forceVariance;
	powerSliderFW = RandomFloat(-100, 0) * forceVariance;
	powerSliderFR = RandomFloat(-100, 0) * forceVariance;
	powerSliderFG = RandomFloat(-100, 0) * forceVariance;
	powerSliderFO = RandomFloat(-100, 0) * forceVariance;
	powerSliderFK = RandomFloat(-100, 0) * forceVariance;
	powerSliderFC = RandomFloat(-100, 0) * forceVariance;
	powerSliderFD = RandomFloat(-100, 0) * forceVariance;
	powerSliderFY = RandomFloat(-100, 0) * forceVariance;
	powerSliderFP = RandomFloat(-100, 0) * forceVariance;
	powerSliderFM = RandomFloat(-100, 0) * forceVariance;
	powerSliderFA = RandomFloat(-100, 0) * forceVariance;
	powerSliderFT = RandomFloat(-100, 0) * forceVariance;
	powerSliderFS = RandomFloat(-100, 0) * forceVariance;
	powerSliderFF = RandomFloat(-100, 0) * forceVariance;

	vSliderFG = RandomFloat(0, 500) * radiusVariance;
	vSliderFR = RandomFloat(0, 500) * radiusVariance;
	vSliderFW = RandomFloat(0, 500) * radiusVariance;
	vSliderFB = RandomFloat(0, 500) * radiusVariance;
	vSliderFO = RandomFloat(0, 500) * radiusVariance;
	vSliderFK = RandomFloat(0, 500) * radiusVariance;
	vSliderFC = RandomFloat(0, 500) * radiusVariance;
	vSliderFD = RandomFloat(0, 500) * radiusVariance;
	vSliderFY = RandomFloat(0, 500) * radiusVariance;
	vSliderFP = RandomFloat(0, 500) * radiusVariance;
	vSliderFM = RandomFloat(0, 500) * radiusVariance;
	vSliderFA = RandomFloat(0, 500) * radiusVariance;
	vSliderFT = RandomFloat(0, 500) * radiusVariance;
	vSliderFS = RandomFloat(0, 500) * radiusVariance;
	vSliderFF = RandomFloat(0, 500) * radiusVariance;
}

/// this is a cheap and quick way to save and load parameters (openFramework have betters ways but requires some additional library setups) 
// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::saveSettings()
{
	const std::vector<float> settings = {
		powerSliderGG, powerSliderGR, powerSliderGW, powerSliderGB, powerSliderGO, powerSliderGK, powerSliderGC, powerSliderGD, powerSliderGY, powerSliderGP, powerSliderGM, powerSliderGA, powerSliderGT, powerSliderGS, powerSliderGF,
		vSliderGG, vSliderGR, vSliderGW, vSliderGB, vSliderGO, vSliderGK, vSliderGC, vSliderGD, vSliderGY, vSliderGP, vSliderGM, vSliderGA, vSliderGT, vSliderGS, vSliderGF,
		powerSliderRG, powerSliderRR, powerSliderRW, powerSliderRB, powerSliderRO, powerSliderRK, powerSliderRC, powerSliderRD, powerSliderRY, powerSliderRP, powerSliderRM, powerSliderRA, powerSliderRT, powerSliderRS, powerSliderRF,
		vSliderRG, vSliderRR, vSliderRW, vSliderRB, vSliderRO, vSliderRK, vSliderRC, vSliderRD, vSliderRY, vSliderRP, vSliderRM, vSliderRA, vSliderRT, vSliderRS, vSliderRF,
		powerSliderWG, powerSliderWR, powerSliderWW, powerSliderWB, powerSliderWO, powerSliderWK, powerSliderWC, powerSliderWD, powerSliderWY, powerSliderWP, powerSliderWM, powerSliderWA, powerSliderWT, powerSliderWS, powerSliderWF,
		vSliderWG, vSliderWR, vSliderWW, vSliderWB, vSliderWO, vSliderWK, vSliderWC, vSliderWD, vSliderWY, vSliderWP, vSliderWM, vSliderWA, vSliderWT, vSliderWS, vSliderWF,
		powerSliderBG, powerSliderBR, powerSliderBW, powerSliderBB, powerSliderBO, powerSliderBK, powerSliderBC, powerSliderBD, powerSliderBY, powerSliderBP, powerSliderBM, powerSliderBA, powerSliderBT, powerSliderBS, powerSliderBF,
		vSliderBG, vSliderBR, vSliderBW, vSliderBB, vSliderBO, vSliderBK, vSliderBC, vSliderBD, vSliderBY, vSliderBP, vSliderBM, vSliderBA, vSliderBT, vSliderBS, vSliderBF,
		powerSliderOG, powerSliderOR, powerSliderOW, powerSliderOB, powerSliderOO, powerSliderOK, powerSliderOC, powerSliderOD, powerSliderOY, powerSliderOP, powerSliderOM, powerSliderOA, powerSliderOT, powerSliderOS, powerSliderOF,
		vSliderOG, vSliderOR, vSliderOW, vSliderOB, vSliderOO, vSliderOK, vSliderOC, vSliderOD, vSliderOY, vSliderOP, vSliderOM, vSliderOA, vSliderOT, vSliderOS, vSliderOF,
		powerSliderKG, powerSliderKR, powerSliderKW, powerSliderKB, powerSliderKO, powerSliderKK, powerSliderKC, powerSliderKD, powerSliderKY, powerSliderKP, powerSliderKM, powerSliderKA, powerSliderKT, powerSliderKS, powerSliderKF,
		vSliderKG, vSliderKR, vSliderKW, vSliderKB, vSliderKO, vSliderKK, vSliderKC, vSliderKD, vSliderKY, vSliderKP, vSliderKM, vSliderKA, vSliderKT, vSliderKS, vSliderKF,
		powerSliderCG, powerSliderCR, powerSliderCW, powerSliderCB, powerSliderCO, powerSliderCK, powerSliderCC, powerSliderCD, powerSliderCY, powerSliderCP, powerSliderCM, powerSliderCA, powerSliderCT, powerSliderCS, powerSliderCF,
		vSliderCG, vSliderCR, vSliderCW, vSliderCB, vSliderCO, vSliderCK, vSliderCC, vSliderCD, vSliderCY, vSliderCP, vSliderCM, vSliderCA, vSliderCT, vSliderCS, vSliderCF,
		powerSliderDG, powerSliderDR, powerSliderDW, powerSliderDB, powerSliderDO, powerSliderDK, powerSliderDC, powerSliderDD, powerSliderDY, powerSliderDP, powerSliderDM, powerSliderDA, powerSliderDT, powerSliderDS, powerSliderDF,
		vSliderDG, vSliderDR, vSliderDW, vSliderDB, vSliderDO, vSliderDK, vSliderDC, vSliderDD, vSliderDY, vSliderDP, vSliderDM, vSliderDA, vSliderDT, vSliderDS, vSliderDF,
		powerSliderYG, powerSliderYR, powerSliderYW, powerSliderYB, powerSliderYO, powerSliderYK, powerSliderYC, powerSliderYD, powerSliderYY, powerSliderYP, powerSliderYM, powerSliderYA, powerSliderYT, powerSliderYS, powerSliderYF,
		vSliderYG, vSliderYR, vSliderYW, vSliderYB, vSliderYO, vSliderYK, vSliderYC, vSliderYD, vSliderYY, vSliderYP, vSliderYM, vSliderYA, vSliderYT, vSliderYS, vSliderYF,
		powerSliderPG, powerSliderPR, powerSliderPW, powerSliderPB, powerSliderPO, powerSliderPK, powerSliderPC, powerSliderPD, powerSliderPY, powerSliderPP, powerSliderPM, powerSliderPA, powerSliderPT, powerSliderPS, powerSliderPF,
		vSliderPG, vSliderPR, vSliderPW, vSliderPB, vSliderPO, vSliderPK, vSliderPC, vSliderPD, vSliderPY, vSliderPP, vSliderPM, vSliderPA, vSliderPT, vSliderPS, vSliderPF,
		powerSliderMG, powerSliderMR, powerSliderMW, powerSliderMB, powerSliderMO, powerSliderMK, powerSliderMC, powerSliderMD, powerSliderMY, powerSliderMP, powerSliderMM, powerSliderMA, powerSliderMT, powerSliderMS, powerSliderMF,
		vSliderMG, vSliderMR, vSliderMW, vSliderMB, vSliderMO, vSliderMK, vSliderMC, vSliderMD, vSliderMY, vSliderMP, vSliderMM, vSliderMA, vSliderMT, vSliderMS, vSliderMF,
		powerSliderAG, powerSliderAR, powerSliderAW, powerSliderAB, powerSliderAO, powerSliderAK, powerSliderAC, powerSliderAD, powerSliderAY, powerSliderAP, powerSliderAM, powerSliderAA, powerSliderAT, powerSliderAS, powerSliderAF,
		vSliderAG, vSliderAR, vSliderAW, vSliderAB, vSliderAO, vSliderAK, vSliderAC, vSliderAD, vSliderAY, vSliderAP, vSliderAM, vSliderAA, vSliderAT, vSliderAS, vSliderAF,
		powerSliderTG, powerSliderTR, powerSliderTW, powerSliderTB, powerSliderTO, powerSliderTK, powerSliderTC, powerSliderTD, powerSliderTY, powerSliderTP, powerSliderTM, powerSliderTA, powerSliderTT, powerSliderTS, powerSliderTF,
		vSliderTG, vSliderTR, vSliderTW, vSliderTB, vSliderTO, vSliderTK, vSliderTC, vSliderTD, vSliderTY, vSliderTP, vSliderTM, vSliderTA, vSliderTT, vSliderTS, vSliderTF,
		powerSliderSG, powerSliderSR, powerSliderSW, powerSliderSB, powerSliderSO, powerSliderSK, powerSliderSC, powerSliderSD, powerSliderSY, powerSliderSP, powerSliderSM, powerSliderSA, powerSliderST, powerSliderSS, powerSliderSF,
		vSliderSG, vSliderSR, vSliderSW, vSliderSB, vSliderSO, vSliderSK, vSliderSC, vSliderSD, vSliderSY, vSliderSP, vSliderSM, vSliderSA, vSliderST, vSliderSS, vSliderSF,
		powerSliderFG, powerSliderFR, powerSliderFW, powerSliderFB, powerSliderFO, powerSliderFK, powerSliderFC, powerSliderFD, powerSliderFY, powerSliderFP, powerSliderFM, powerSliderFA, powerSliderFT, powerSliderFS, powerSliderFF,
		vSliderFG, vSliderFR, vSliderFW, vSliderFB, vSliderFO, vSliderFK, vSliderFC, vSliderFD, vSliderFY, vSliderFP, vSliderFM, vSliderFA, vSliderFT, vSliderFS, vSliderFF,
		static_cast<float>(numberSliderG),
		static_cast<float>(numberSliderR),
		static_cast<float>(numberSliderW),
		static_cast<float>(numberSliderB),
		static_cast<float>(numberSliderO),
		static_cast<float>(numberSliderK),
		static_cast<float>(numberSliderC),
		static_cast<float>(numberSliderD),
		static_cast<float>(numberSliderY),
		static_cast<float>(numberSliderP),
		static_cast<float>(numberSliderM),
		static_cast<float>(numberSliderA),
		static_cast<float>(numberSliderT),
		static_cast<float>(numberSliderS),
		static_cast<float>(numberSliderF),
		viscositySlider,
		viscositySliderRR,
		viscositySliderRG,
		viscositySliderRB,
		viscositySliderRW,
		viscositySliderRO,
		viscositySliderRK,
		viscositySliderRC,
		viscositySliderRD,
		viscositySliderRY,
		viscositySliderRP,
		viscositySliderRM,
		viscositySliderRA,
		viscositySliderRT,
		viscositySliderRS,
		viscositySliderRF,
		viscositySliderGR,
		viscositySliderGG,
		viscositySliderGB,
		viscositySliderGW,
		viscositySliderGO,
		viscositySliderGK,
		viscositySliderGC,
		viscositySliderGD,
		viscositySliderGY,
		viscositySliderGP,
		viscositySliderGM,
		viscositySliderGA,
		viscositySliderGT,
		viscositySliderGS,
		viscositySliderGF,
		viscositySliderBR,
		viscositySliderBG,
		viscositySliderBB,
		viscositySliderBW,
		viscositySliderBO,
		viscositySliderBK,
		viscositySliderBC,
		viscositySliderBD,
		viscositySliderBY,
		viscositySliderBP,
		viscositySliderBM,
		viscositySliderBA,
		viscositySliderBT,
		viscositySliderBS,
		viscositySliderBF,
		viscositySliderWR,
		viscositySliderWG,
		viscositySliderWB,
		viscositySliderWW,
		viscositySliderWO,
		viscositySliderWK,
		viscositySliderWC,
		viscositySliderWD,
		viscositySliderWY,
		viscositySliderWP,
		viscositySliderWM,
		viscositySliderWA,
		viscositySliderWT,
		viscositySliderWS,
		viscositySliderWF,
		viscositySliderOR,
		viscositySliderOG,
		viscositySliderOB,
		viscositySliderOW,
		viscositySliderOO,
		viscositySliderOK,
		viscositySliderOC,
		viscositySliderOD,
		viscositySliderOY,
		viscositySliderOP,
		viscositySliderOM,
		viscositySliderOA,
		viscositySliderOT,
		viscositySliderOS,
		viscositySliderOF,
		viscositySliderKR,
		viscositySliderKG,
		viscositySliderKB,
		viscositySliderKW,
		viscositySliderKO,
		viscositySliderKK,
		viscositySliderKC,
		viscositySliderKD,
		viscositySliderKY,
		viscositySliderKP,
		viscositySliderKM,
		viscositySliderKA,
		viscositySliderKT,
		viscositySliderKS,
		viscositySliderKF,
		viscositySliderCR,
		viscositySliderCG,
		viscositySliderCB,
		viscositySliderCW,
		viscositySliderCO,
		viscositySliderCK,
		viscositySliderCC,
		viscositySliderCD,
		viscositySliderCY,
		viscositySliderCP,
		viscositySliderCM,
		viscositySliderCA,
		viscositySliderCT,
		viscositySliderCS,
		viscositySliderCF,
		viscositySliderDR,
		viscositySliderDG,
		viscositySliderDB,
		viscositySliderDW,
		viscositySliderDO,
		viscositySliderDK,
		viscositySliderDC,
		viscositySliderDD,
		viscositySliderDY,
		viscositySliderDP,
		viscositySliderDM,
		viscositySliderDA,
		viscositySliderDT,
		viscositySliderDS,
		viscositySliderDF,
		viscositySliderYR,
		viscositySliderYG,
		viscositySliderYB,
		viscositySliderYW,
		viscositySliderYO,
		viscositySliderYK,
		viscositySliderYC,
		viscositySliderYD,
		viscositySliderYY,
		viscositySliderYP,
		viscositySliderYM,
		viscositySliderYA,
		viscositySliderYT,
		viscositySliderYS,
		viscositySliderYF,
		viscositySliderPR,
		viscositySliderPG,
		viscositySliderPB,
		viscositySliderPW,
		viscositySliderPO,
		viscositySliderPK,
		viscositySliderPC,
		viscositySliderPD,
		viscositySliderPY,
		viscositySliderPP,
		viscositySliderPM,
		viscositySliderPA,
		viscositySliderPT,
		viscositySliderPS,
		viscositySliderPF,
		viscositySliderMR,
		viscositySliderMG,
		viscositySliderMB,
		viscositySliderMW,
		viscositySliderMO,
		viscositySliderMK,
		viscositySliderMC,
		viscositySliderMD,
		viscositySliderMY,
		viscositySliderMP,
		viscositySliderMM,
		viscositySliderMA,
		viscositySliderMT,
		viscositySliderMS,
		viscositySliderMF,
		viscositySliderAR,
		viscositySliderAG,
		viscositySliderAB,
		viscositySliderAW,
		viscositySliderAO,
		viscositySliderAK,
		viscositySliderAC,
		viscositySliderAD,
		viscositySliderAY,
		viscositySliderAP,
		viscositySliderAM,
		viscositySliderAA,
		viscositySliderAT,
		viscositySliderAS,
		viscositySliderAF,
		viscositySliderTR,
		viscositySliderTG,
		viscositySliderTB,
		viscositySliderTW,
		viscositySliderTO,
		viscositySliderTK,
		viscositySliderTC,
		viscositySliderTD,
		viscositySliderTY,
		viscositySliderTP,
		viscositySliderTM,
		viscositySliderTA,
		viscositySliderTT,
		viscositySliderTS,
		viscositySliderTF,
		viscositySliderSR,
		viscositySliderSG,
		viscositySliderSB,
		viscositySliderSW,
		viscositySliderSO,
		viscositySliderSK,
		viscositySliderSC,
		viscositySliderSD,
		viscositySliderSY,
		viscositySliderSP,
		viscositySliderSM,
		viscositySliderSA,
		viscositySliderST,
		viscositySliderSS,
		viscositySliderSF,
		viscositySliderFR,
		viscositySliderFG,
		viscositySliderFB,
		viscositySliderFW,
		viscositySliderFO,
		viscositySliderFK,
		viscositySliderFC,
		viscositySliderFD,
		viscositySliderFY,
		viscositySliderFP,
		viscositySliderFM,
		viscositySliderFA,
		viscositySliderFT,
		viscositySliderFS,
		viscositySliderFF,
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
		probabilitySliderRY,
		probabilitySliderRP,
		probabilitySliderRM,
		probabilitySliderRA,
		probabilitySliderRT,
		probabilitySliderRS,
		probabilitySliderRF,
		probabilitySliderGR,
		probabilitySliderGG,
		probabilitySliderGB,
		probabilitySliderGW,
		probabilitySliderGO,
		probabilitySliderGK,
		probabilitySliderGC,
		probabilitySliderGD,
		probabilitySliderGY,
		probabilitySliderGP,
		probabilitySliderGM,
		probabilitySliderGA,
		probabilitySliderGT,
		probabilitySliderGS,
		probabilitySliderGF,
		probabilitySliderBR,
		probabilitySliderBG,
		probabilitySliderBB,
		probabilitySliderBW,
		probabilitySliderBO,
		probabilitySliderBK,
		probabilitySliderBC,
		probabilitySliderBD,
		probabilitySliderBY,
		probabilitySliderBP,
		probabilitySliderBM,
		probabilitySliderBA,
		probabilitySliderBT,
		probabilitySliderBS,
		probabilitySliderBF,
		probabilitySliderWR,
		probabilitySliderWG,
		probabilitySliderWB,
		probabilitySliderWW,
		probabilitySliderWO,
		probabilitySliderWK,
		probabilitySliderWC,
		probabilitySliderWD,
		probabilitySliderWY,
		probabilitySliderWP,
		probabilitySliderWM,
		probabilitySliderWA,
		probabilitySliderWT,
		probabilitySliderWS,
		probabilitySliderWF,
		probabilitySliderOR,
		probabilitySliderOG,
		probabilitySliderOB,
		probabilitySliderOW,
		probabilitySliderOO,
		probabilitySliderOK,
		probabilitySliderOC,
		probabilitySliderOD,
		probabilitySliderOY,
		probabilitySliderOP,
		probabilitySliderOM,
		probabilitySliderOA,
		probabilitySliderOT,
		probabilitySliderOS,
		probabilitySliderOF,
		probabilitySliderKR,
		probabilitySliderKG,
		probabilitySliderKB,
		probabilitySliderKW,
		probabilitySliderKO,
		probabilitySliderKK,
		probabilitySliderKC,
		probabilitySliderKD,
		probabilitySliderKY,
		probabilitySliderKP,
		probabilitySliderKM,
		probabilitySliderKA,
		probabilitySliderKT,
		probabilitySliderKS,
		probabilitySliderKF,
		probabilitySliderCR,
		probabilitySliderCG,
		probabilitySliderCB,
		probabilitySliderCW,
		probabilitySliderCO,
		probabilitySliderCK,
		probabilitySliderCC,
		probabilitySliderCD,
		probabilitySliderCY,
		probabilitySliderCP,
		probabilitySliderCM,
		probabilitySliderCA,
		probabilitySliderCT,
		probabilitySliderCS,
		probabilitySliderCF,
		probabilitySliderDR,
		probabilitySliderDG,
		probabilitySliderDB,
		probabilitySliderDW,
		probabilitySliderDO,
		probabilitySliderDK,
		probabilitySliderDC,
		probabilitySliderDD,
		probabilitySliderDY,
		probabilitySliderDP,
		probabilitySliderDM,
		probabilitySliderDA,
		probabilitySliderDT,
		probabilitySliderDS,
		probabilitySliderDF,
		probabilitySliderYR,
		probabilitySliderYG,
		probabilitySliderYB,
		probabilitySliderYW,
		probabilitySliderYO,
		probabilitySliderYK,
		probabilitySliderYC,
		probabilitySliderYD,
		probabilitySliderYY,
		probabilitySliderYP,
		probabilitySliderYM,
		probabilitySliderYA,
		probabilitySliderYT,
		probabilitySliderYS,
		probabilitySliderYF,
		probabilitySliderPR,
		probabilitySliderPG,
		probabilitySliderPB,
		probabilitySliderPW,
		probabilitySliderPO,
		probabilitySliderPK,
		probabilitySliderPC,
		probabilitySliderPD,
		probabilitySliderPY,
		probabilitySliderPP,
		probabilitySliderPM,
		probabilitySliderPA,
		probabilitySliderPT,
		probabilitySliderPS,
		probabilitySliderPF,
		probabilitySliderMR,
		probabilitySliderMG,
		probabilitySliderMB,
		probabilitySliderMW,
		probabilitySliderMO,
		probabilitySliderMK,
		probabilitySliderMC,
		probabilitySliderMD,
		probabilitySliderMY,
		probabilitySliderMP,
		probabilitySliderMM,
		probabilitySliderMA,
		probabilitySliderMT,
		probabilitySliderMS,
		probabilitySliderMF,
		probabilitySliderAR,
		probabilitySliderAG,
		probabilitySliderAB,
		probabilitySliderAW,
		probabilitySliderAO,
		probabilitySliderAK,
		probabilitySliderAC,
		probabilitySliderAD,
		probabilitySliderAY,
		probabilitySliderAP,
		probabilitySliderAM,
		probabilitySliderAA,
		probabilitySliderAT,
		probabilitySliderAS,
		probabilitySliderAF,
		probabilitySliderTR,
		probabilitySliderTG,
		probabilitySliderTB,
		probabilitySliderTW,
		probabilitySliderTO,
		probabilitySliderTK,
		probabilitySliderTC,
		probabilitySliderTD,
		probabilitySliderTY,
		probabilitySliderTP,
		probabilitySliderTM,
		probabilitySliderTA,
		probabilitySliderTT,
		probabilitySliderTS,
		probabilitySliderTF,
		probabilitySliderSR,
		probabilitySliderSG,
		probabilitySliderSB,
		probabilitySliderSW,
		probabilitySliderSO,
		probabilitySliderSK,
		probabilitySliderSC,
		probabilitySliderSD,
		probabilitySliderSY,
		probabilitySliderSP,
		probabilitySliderSM,
		probabilitySliderSA,
		probabilitySliderST,
		probabilitySliderSS,
		probabilitySliderSF,
		probabilitySliderFR,
		probabilitySliderFG,
		probabilitySliderFB,
		probabilitySliderFW,
		probabilitySliderFO,
		probabilitySliderFK,
		probabilitySliderFC,
		probabilitySliderFD,
		probabilitySliderFY,
		probabilitySliderFP,
		probabilitySliderFM,
		probabilitySliderFA,
		probabilitySliderFT,
		probabilitySliderFS,
		probabilitySliderFF,
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

	if (p.size() < 919)
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
		powerSliderGY = p[8];
		powerSliderGP = p[9];
		powerSliderGM = p[10];
		powerSliderGA = p[11];
		powerSliderGT = p[12];
		powerSliderGS = p[13];
		powerSliderGF = p[14];
		vSliderGG = p[15];
		vSliderGR = p[16];
		vSliderGW = p[17];
		vSliderGB = p[18];
		vSliderGO = p[19];
		vSliderGK = p[20];
		vSliderGC = p[21];
		vSliderGD = p[22];
		vSliderGY = p[23];
		vSliderGP = p[24];
		vSliderGM = p[25];
		vSliderGA = p[26];
		vSliderGT = p[27];
		vSliderGS = p[28];
		vSliderGF = p[29];
		powerSliderRG = p[30];
		powerSliderRR = p[31];
		powerSliderRW = p[32];
		powerSliderRB = p[33];
		powerSliderRO = p[34];
		powerSliderRK = p[35];
		powerSliderRC = p[36];
		powerSliderRD = p[37];
		powerSliderRY = p[38];
		powerSliderRP = p[39];
		powerSliderRM = p[40];
		powerSliderRA = p[41];
		powerSliderRT = p[42];
		powerSliderRS = p[43];
		powerSliderRF = p[44];
		vSliderRG = p[45];
		vSliderRR = p[46];
		vSliderRW = p[47];
		vSliderRB = p[48];
		vSliderRO = p[49];
		vSliderRK = p[50];
		vSliderRC = p[51];
		vSliderRD = p[52];
		vSliderRY = p[53];
		vSliderRP = p[54];
		vSliderRM = p[55];
		vSliderRA = p[56];
		vSliderRT = p[57];
		vSliderRS = p[58];
		vSliderRF = p[59];
		powerSliderWG = p[60];
		powerSliderWR = p[61];
		powerSliderWW = p[62];
		powerSliderWB = p[63];
		powerSliderWO = p[64];
		powerSliderWK = p[65];
		powerSliderWC = p[66];
		powerSliderWD = p[67];
		powerSliderWY = p[68];
		powerSliderWP = p[69];
		powerSliderWM = p[70];
		powerSliderWA = p[71];
		powerSliderWT = p[72];
		powerSliderWS = p[73];
		powerSliderWF = p[74];
		vSliderWG = p[75];
		vSliderWR = p[76];
		vSliderWW = p[77];
		vSliderWB = p[78];
		vSliderWO = p[79];
		vSliderWK = p[80];
		vSliderWC = p[81];
		vSliderWD = p[82];
		vSliderWY = p[83];
		vSliderWP = p[84];
		vSliderWM = p[85];
		vSliderWA = p[86];
		vSliderWT = p[87];
		vSliderWS = p[88];
		vSliderWF = p[89];
		powerSliderBG = p[90];
		powerSliderBR = p[91];
		powerSliderBW = p[92];
		powerSliderBB = p[93];
		powerSliderBO = p[94];
		powerSliderBK = p[95];
		powerSliderBC = p[96];
		powerSliderBD = p[97];
		powerSliderBY = p[98];
		powerSliderBP = p[99];
		powerSliderBM = p[100];
		powerSliderBA = p[101];
		powerSliderBT = p[102];
		powerSliderBS = p[103];
		powerSliderBF = p[104];
		vSliderBG = p[105];
		vSliderBR = p[106];
		vSliderBW = p[107];
		vSliderBB = p[108];
		vSliderBO = p[109];
		vSliderBK = p[110];
		vSliderBC = p[111];
		vSliderBD = p[112];
		vSliderBY = p[113];
		vSliderBP = p[114];
		vSliderBM = p[115];
		vSliderBA = p[116];
		vSliderBT = p[117];
		vSliderBS = p[118];
		vSliderBF = p[119];
		powerSliderOG = p[120];
		powerSliderOR = p[121];
		powerSliderOW = p[122];
		powerSliderOB = p[123];
		powerSliderOO = p[124];
		powerSliderOK = p[125];
		powerSliderOC = p[126];
		powerSliderOD = p[127];
		powerSliderOY = p[128];
		powerSliderOP = p[129];
		powerSliderOM = p[130];
		powerSliderOA = p[131];
		powerSliderOT = p[132];
		powerSliderOS = p[133];
		powerSliderOF = p[134];
		vSliderOG = p[135];
		vSliderOR = p[136];
		vSliderOW = p[137];
		vSliderOB = p[138];
		vSliderOO = p[139];
		vSliderOK = p[140];
		vSliderOC = p[141];
		vSliderOD = p[142];
		vSliderOY = p[143];
		vSliderOP = p[144];
		vSliderOM = p[145];
		vSliderOA = p[146];
		vSliderOT = p[147];
		vSliderOS = p[148];
		vSliderOF = p[149];
		powerSliderKG = p[150];
		powerSliderKR = p[151];
		powerSliderKW = p[152];
		powerSliderKB = p[153];
		powerSliderKO = p[154];
		powerSliderKK = p[155];
		powerSliderKC = p[156];
		powerSliderKD = p[157];
		powerSliderKY = p[158];
		powerSliderKP = p[159];
		powerSliderKM = p[160];
		powerSliderKA = p[161];
		powerSliderKT = p[162];
		powerSliderKS = p[163];
		powerSliderKF = p[164];
		vSliderKG = p[165];
		vSliderKR = p[166];
		vSliderKW = p[167];
		vSliderKB = p[168];
		vSliderKO = p[168];
		vSliderKK = p[170];
		vSliderKC = p[171];
		vSliderKD = p[172];
		vSliderKY = p[173];
		vSliderKP = p[174];
		vSliderKM = p[175];
		vSliderKA = p[176];
		vSliderKT = p[177];
		vSliderKS = p[178];
		vSliderKF = p[179];
		powerSliderCG = p[180];
		powerSliderCR = p[181];
		powerSliderCW = p[182];
		powerSliderCB = p[183];
		powerSliderCO = p[184];
		powerSliderCK = p[185];
		powerSliderCC = p[186];
		powerSliderCD = p[187];
		powerSliderCY = p[188];
		powerSliderCP = p[189];
		powerSliderCM = p[190];
		powerSliderCA = p[191];
		powerSliderCT = p[192];
		powerSliderCS = p[193];
		powerSliderCF = p[194];
		vSliderCG = p[195];
		vSliderCR = p[196];
		vSliderCW = p[197];
		vSliderCB = p[198];
		vSliderCO = p[199];
		vSliderCK = p[200];
		vSliderCC = p[201];
		vSliderCD = p[202];
		vSliderCY = p[203];
		vSliderCP = p[204];
		vSliderCM = p[205];
		vSliderCA = p[206];
		vSliderCT = p[207];
		vSliderCS = p[208];
		vSliderCF = p[209];
		powerSliderDG = p[210];
		powerSliderDR = p[211];
		powerSliderDW = p[212];
		powerSliderDB = p[213];
		powerSliderDO = p[214];
		powerSliderDK = p[215];
		powerSliderDC = p[216];
		powerSliderDD = p[217];
		powerSliderDY = p[218];
		powerSliderDP = p[219];
		powerSliderDM = p[220];
		powerSliderDA = p[221];
		powerSliderDT = p[222];
		powerSliderDS = p[223];
		powerSliderDF = p[224];
		vSliderDG = p[225];
		vSliderDR = p[226];
		vSliderDW = p[227];
		vSliderDB = p[228];
		vSliderDO = p[229];
		vSliderDK = p[230];
		vSliderDC = p[231];
		vSliderDD = p[232];
		vSliderDY = p[233];
		vSliderDP = p[234];
		vSliderDM = p[235];
		vSliderDA = p[236];
		vSliderDT = p[237];
		vSliderDS = p[238];
		vSliderDF = p[239];
		powerSliderYG = p[240];
		powerSliderYR = p[241];
		powerSliderYW = p[242];
		powerSliderYB = p[243];
		powerSliderYO = p[244];
		powerSliderYK = p[245];
		powerSliderYC = p[246];
		powerSliderYD = p[247];
		powerSliderYY = p[248];
		powerSliderYP = p[249];
		powerSliderYM = p[250];
		powerSliderYA = p[251];
		powerSliderYT = p[252];
		powerSliderYS = p[253];
		powerSliderYF = p[254];
		vSliderYG = p[255];
		vSliderYR = p[256];
		vSliderYW = p[257];
		vSliderYB = p[258];
		vSliderYO = p[259];
		vSliderYK = p[260];
		vSliderYC = p[261];
		vSliderYD = p[262];
		vSliderYY = p[263];
		vSliderYP = p[264];
		vSliderYM = p[265];
		vSliderYA = p[266];
		vSliderYT = p[267];
		vSliderYS = p[268];
		vSliderYF = p[269];
		powerSliderPG = p[270];
		powerSliderPR = p[271];
		powerSliderPW = p[272];
		powerSliderPB = p[273];
		powerSliderPO = p[274];
		powerSliderPK = p[275];
		powerSliderPC = p[276];
		powerSliderPD = p[277];
		powerSliderPY = p[278];
		powerSliderPP = p[279];
		powerSliderPM = p[280];
		powerSliderPA = p[281];
		powerSliderPT = p[282];
		powerSliderPS = p[283];
		powerSliderPF = p[284];
		vSliderPG = p[285];
		vSliderPR = p[286];
		vSliderPW = p[287];
		vSliderPB = p[288];
		vSliderPO = p[289];
		vSliderPK = p[290];
		vSliderPC = p[291];
		vSliderPD = p[292];
		vSliderPY = p[293];
		vSliderPP = p[294];
		vSliderPM = p[295];
		vSliderPA = p[296];
		vSliderPT = p[297];
		vSliderPS = p[298];
		vSliderPF = p[299];
		powerSliderMG = p[300];
		powerSliderMR = p[301];
		powerSliderMW = p[302];
		powerSliderMB = p[303];
		powerSliderMO = p[304];
		powerSliderMK = p[305];
		powerSliderMC = p[306];
		powerSliderMD = p[307];
		powerSliderMY = p[308];
		powerSliderMP = p[309];
		powerSliderMM = p[310];
		powerSliderMA = p[311];
		powerSliderMT = p[312];
		powerSliderMS = p[313];
		powerSliderMF = p[314];
		vSliderMG = p[315];
		vSliderMR = p[316];
		vSliderMW = p[317];
		vSliderMB = p[318];
		vSliderMO = p[319];
		vSliderMK = p[320];
		vSliderMC = p[321];
		vSliderMD = p[322];
		vSliderMY = p[323];
		vSliderMP = p[324];
		vSliderMM = p[325];
		vSliderMA = p[326];
		vSliderMT = p[327];
		vSliderMS = p[328];
		vSliderMF = p[329];
		powerSliderAG = p[330];
		powerSliderAR = p[331];
		powerSliderAW = p[332];
		powerSliderAB = p[333];
		powerSliderAO = p[334];
		powerSliderAK = p[335];
		powerSliderAC = p[336];
		powerSliderAD = p[337];
		powerSliderAY = p[338];
		powerSliderAP = p[339];
		powerSliderAM = p[340];
		powerSliderAA = p[341];
		powerSliderAT = p[342];
		powerSliderAS = p[343];
		powerSliderAF = p[344];
		vSliderAG = p[345];
		vSliderAR = p[346];
		vSliderAW = p[347];
		vSliderAB = p[348];
		vSliderAO = p[349];
		vSliderAK = p[350];
		vSliderAC = p[351];
		vSliderAD = p[352];
		vSliderAY = p[353];
		vSliderAP = p[354];
		vSliderAM = p[355];
		vSliderAA = p[356];
		vSliderAT = p[357];
		vSliderAS = p[358];
		vSliderAF = p[359];
		powerSliderTG = p[360];
		powerSliderTR = p[361];
		powerSliderTW = p[362];
		powerSliderTB = p[363];
		powerSliderTO = p[364];
		powerSliderTK = p[365];
		powerSliderTC = p[366];
		powerSliderTD = p[367];
		powerSliderTY = p[368];
		powerSliderTP = p[369];
		powerSliderTM = p[370];
		powerSliderTA = p[371];
		powerSliderTT = p[372];
		powerSliderTS = p[373];
		powerSliderTF = p[374];
		vSliderTG = p[375];
		vSliderTR = p[376];
		vSliderTW = p[377];
		vSliderTB = p[378];
		vSliderTO = p[379];
		vSliderTK = p[380];
		vSliderTC = p[381];
		vSliderTD = p[382];
		vSliderTY = p[383];
		vSliderTP = p[384];
		vSliderTM = p[385];
		vSliderTA = p[386];
		vSliderTT = p[387];
		vSliderTS = p[388];
		vSliderTF = p[389];
		powerSliderSG = p[390];
		powerSliderSR = p[391];
		powerSliderSW = p[392];
		powerSliderSB = p[393];
		powerSliderSO = p[394];
		powerSliderSK = p[395];
		powerSliderSC = p[396];
		powerSliderSD = p[397];
		powerSliderSY = p[398];
		powerSliderSP = p[399];
		powerSliderSM = p[400];
		powerSliderSA = p[401];
		powerSliderST = p[402];
		powerSliderSS = p[403];
		powerSliderSF = p[404];
		vSliderSG = p[405];
		vSliderSR = p[406];
		vSliderSW = p[407];
		vSliderSB = p[408];
		vSliderSO = p[409];
		vSliderSK = p[410];
		vSliderSC = p[411];
		vSliderSD = p[412];
		vSliderSY = p[413];
		vSliderSP = p[414];
		vSliderSM = p[415];
		vSliderSA = p[416];
		vSliderST = p[417];
		vSliderSS = p[418];
		vSliderSF = p[419];
		powerSliderFG = p[420];
		powerSliderFR = p[421];
		powerSliderFW = p[422];
		powerSliderFB = p[423];
		powerSliderFO = p[424];
		powerSliderFK = p[425];
		powerSliderFC = p[426];
		powerSliderFD = p[427];
		powerSliderFY = p[428];
		powerSliderFP = p[429];
		powerSliderFM = p[430];
		powerSliderFA = p[431];
		powerSliderFT = p[432];
		powerSliderFS = p[433];
		powerSliderFF = p[434];
		vSliderFG = p[435];
		vSliderFR = p[436];
		vSliderFW = p[437];
		vSliderFB = p[438];
		vSliderFO = p[439];
		vSliderFK = p[440];
		vSliderFC = p[441];
		vSliderFD = p[442];
		vSliderFY = p[443];
		vSliderFP = p[444];
		vSliderFM = p[445];
		vSliderFA = p[446];
		vSliderFT = p[447];
		vSliderFS = p[448];
		vSliderFF = p[449];
		numberSliderG = static_cast<int>(p[450]);
		numberSliderR = static_cast<int>(p[451]);
		numberSliderW = static_cast<int>(p[452]);
		numberSliderB = static_cast<int>(p[453]);
		numberSliderO = static_cast<int>(p[454]);
		numberSliderK = static_cast<int>(p[455]);
		numberSliderC = static_cast<int>(p[456]);
		numberSliderD = static_cast<int>(p[457]);
		numberSliderY = static_cast<int>(p[458]);
		numberSliderP = static_cast<int>(p[459]);
		numberSliderM = static_cast<int>(p[460]);
		numberSliderA = static_cast<int>(p[461]);
		numberSliderT = static_cast<int>(p[462]);
		numberSliderS = static_cast<int>(p[463]);
		numberSliderF = static_cast<int>(p[464]);
		viscositySlider = p[465];
		viscositySliderRR = p[466];
		viscositySliderRG = p[467];
		viscositySliderRB = p[468];
		viscositySliderRW = p[469];
		viscositySliderRO = p[470];
		viscositySliderRK = p[471];
		viscositySliderRC = p[472];
		viscositySliderRD = p[473];
		viscositySliderRY = p[474];
		viscositySliderRP = p[475];
		viscositySliderRM = p[476];
		viscositySliderRA = p[477];
		viscositySliderRT = p[478];
		viscositySliderRS = p[479];
		viscositySliderRF = p[480];
		viscositySliderGR = p[481];
		viscositySliderGG = p[482];
		viscositySliderGB = p[483];
		viscositySliderGW = p[484];
		viscositySliderGO = p[485];
		viscositySliderGK = p[486];
		viscositySliderGC = p[487];
		viscositySliderGD = p[488];
		viscositySliderGY = p[489];
		viscositySliderGP = p[490];
		viscositySliderGM = p[491];
		viscositySliderGA = p[492];
		viscositySliderGT = p[493];
		viscositySliderGS = p[494];
		viscositySliderGF = p[495];
		viscositySliderBR = p[496];
		viscositySliderBG = p[497];
		viscositySliderBB = p[498];
		viscositySliderBW = p[499];
		viscositySliderBO = p[500];
		viscositySliderBK = p[501];
		viscositySliderBC = p[502];
		viscositySliderBD = p[503];
		viscositySliderBY = p[504];
		viscositySliderBP = p[505];
		viscositySliderBM = p[506];
		viscositySliderBA = p[507];
		viscositySliderBT = p[508];
		viscositySliderBS = p[509];
		viscositySliderBF = p[510];
		viscositySliderWR = p[511];
		viscositySliderWG = p[512];
		viscositySliderWB = p[513];
		viscositySliderWW = p[514];
		viscositySliderWO = p[515];
		viscositySliderWK = p[516];
		viscositySliderWC = p[517];
		viscositySliderWD = p[518];
		viscositySliderWY = p[519];
		viscositySliderWP = p[520];
		viscositySliderWM = p[521];
		viscositySliderWA = p[522];
		viscositySliderWT = p[523];
		viscositySliderWS = p[524];
		viscositySliderWF = p[525];
		viscositySliderOR = p[526];
		viscositySliderOG = p[527];
		viscositySliderOB = p[528];
		viscositySliderOW = p[529];
		viscositySliderOO = p[530];
		viscositySliderOK = p[531];
		viscositySliderOC = p[532];
		viscositySliderOD = p[533];
		viscositySliderOY = p[534];
		viscositySliderOP = p[535];
		viscositySliderOM = p[536];
		viscositySliderOA = p[537];
		viscositySliderOT = p[538];
		viscositySliderOS = p[539];
		viscositySliderOF = p[540];
		viscositySliderKR = p[541];
		viscositySliderKG = p[542];
		viscositySliderKB = p[543];
		viscositySliderKW = p[544];
		viscositySliderKO = p[545];
		viscositySliderKK = p[546];
		viscositySliderKC = p[547];
		viscositySliderKD = p[548];
		viscositySliderKY = p[549];
		viscositySliderKP = p[550];
		viscositySliderKM = p[551];
		viscositySliderKA = p[552];
		viscositySliderKT = p[553];
		viscositySliderKS = p[554];
		viscositySliderKF = p[555];
		viscositySliderCR = p[556];
		viscositySliderCG = p[557];
		viscositySliderCB = p[558];
		viscositySliderCW = p[559];
		viscositySliderCO = p[560];
		viscositySliderCK = p[561];
		viscositySliderCC = p[562];
		viscositySliderCD = p[563];
		viscositySliderCY = p[564];
		viscositySliderCP = p[565];
		viscositySliderCM = p[566];
		viscositySliderCA = p[567];
		viscositySliderCT = p[568];
		viscositySliderCS = p[569];
		viscositySliderCF = p[570];
		viscositySliderDR = p[571];
		viscositySliderDG = p[572];
		viscositySliderDB = p[573];
		viscositySliderDW = p[574];
		viscositySliderDO = p[575];
		viscositySliderDK = p[576];
		viscositySliderDC = p[577];
		viscositySliderDD = p[578];
		viscositySliderDY = p[579];
		viscositySliderDP = p[580];
		viscositySliderDM = p[581];
		viscositySliderDA = p[582];
		viscositySliderDT = p[583];
		viscositySliderDS = p[584];
		viscositySliderDF = p[585];
		viscositySliderYR = p[586];
		viscositySliderYG = p[587];
		viscositySliderYB = p[588];
		viscositySliderYW = p[589];
		viscositySliderYO = p[590];
		viscositySliderYK = p[591];
		viscositySliderYC = p[592];
		viscositySliderYD = p[593];
		viscositySliderYY = p[594];
		viscositySliderYP = p[595];
		viscositySliderYM = p[596];
		viscositySliderYA = p[597];
		viscositySliderYT = p[598];
		viscositySliderYS = p[599];
		viscositySliderYF = p[600];
		viscositySliderPR = p[601];
		viscositySliderPG = p[602];
		viscositySliderPB = p[603];
		viscositySliderPW = p[604];
		viscositySliderPO = p[605];
		viscositySliderPK = p[606];
		viscositySliderPC = p[607];
		viscositySliderPD = p[608];
		viscositySliderPY = p[609];
		viscositySliderPP = p[610];
		viscositySliderPM = p[611];
		viscositySliderPA = p[612];
		viscositySliderPT = p[613];
		viscositySliderPS = p[614];
		viscositySliderPF = p[615];
		viscositySliderMR = p[616];
		viscositySliderMG = p[617];
		viscositySliderMB = p[618];
		viscositySliderMW = p[619];
		viscositySliderMO = p[620];
		viscositySliderMK = p[621];
		viscositySliderMC = p[622];
		viscositySliderMD = p[623];
		viscositySliderMY = p[624];
		viscositySliderMP = p[625];
		viscositySliderMM = p[626];
		viscositySliderMA = p[627];
		viscositySliderMT = p[628];
		viscositySliderMS = p[629];
		viscositySliderMF = p[630];
		viscositySliderAR = p[631];
		viscositySliderAG = p[632];
		viscositySliderAB = p[633];
		viscositySliderAW = p[634];
		viscositySliderAO = p[635];
		viscositySliderAK = p[636];
		viscositySliderAC = p[637];
		viscositySliderAD = p[638];
		viscositySliderAY = p[639];
		viscositySliderAP = p[640];
		viscositySliderAM = p[641];
		viscositySliderAA = p[642];
		viscositySliderAT = p[643];
		viscositySliderAS = p[644];
		viscositySliderAF = p[645];
		viscositySliderTR = p[646];
		viscositySliderTG = p[647];
		viscositySliderTB = p[648];
		viscositySliderTW = p[649];
		viscositySliderTO = p[650];
		viscositySliderTK = p[651];
		viscositySliderTC = p[652];
		viscositySliderTD = p[653];
		viscositySliderTY = p[654];
		viscositySliderTP = p[655];
		viscositySliderTM = p[656];
		viscositySliderTA = p[657];
		viscositySliderTT = p[658];
		viscositySliderTS = p[659];
		viscositySliderTF = p[660];
		viscositySliderSR = p[661];
		viscositySliderSG = p[662];
		viscositySliderSB = p[663];
		viscositySliderSW = p[664];
		viscositySliderSO = p[665];
		viscositySliderSK = p[666];
		viscositySliderSC = p[667];
		viscositySliderSD = p[668];
		viscositySliderSY = p[669];
		viscositySliderSP = p[670];
		viscositySliderSM = p[671];
		viscositySliderSA = p[672];
		viscositySliderST = p[673];
		viscositySliderSS = p[674];
		viscositySliderSF = p[675];
		viscositySliderFR = p[676];
		viscositySliderFG = p[677];
		viscositySliderFB = p[678];
		viscositySliderFW = p[679];
		viscositySliderFO = p[680];
		viscositySliderFK = p[681];
		viscositySliderFC = p[682];
		viscositySliderFD = p[683];
		viscositySliderFY = p[684];
		viscositySliderFP = p[685];
		viscositySliderFM = p[686];
		viscositySliderFA = p[687];
		viscositySliderFT = p[688];
		viscositySliderFS = p[689];
		viscositySliderFF = p[690];
		evoProbSlider = p[691];
		evoAmountSlider = p[692];
		probabilitySlider = p[693];
		probabilitySliderRR = p[694];
		probabilitySliderRG = p[695];
		probabilitySliderRB = p[696];
		probabilitySliderRW = p[697];
		probabilitySliderRO = p[698];
		probabilitySliderRK = p[699];
		probabilitySliderRC = p[700];
		probabilitySliderRD = p[701];
		probabilitySliderRY = p[702];
		probabilitySliderRP = p[703];
		probabilitySliderRM = p[704];
		probabilitySliderRA = p[705];
		probabilitySliderRT = p[706];
		probabilitySliderRS = p[707];
		probabilitySliderRF = p[708];
		probabilitySliderGR = p[709];
		probabilitySliderGG = p[710];
		probabilitySliderGB = p[711];
		probabilitySliderGW = p[712];
		probabilitySliderGO = p[713];
		probabilitySliderGK = p[714];
		probabilitySliderGC = p[715];
		probabilitySliderGD = p[716];
		probabilitySliderGY = p[717];
		probabilitySliderGP = p[718];
		probabilitySliderGM = p[719];
		probabilitySliderGA = p[720];
		probabilitySliderGT = p[721];
		probabilitySliderGS = p[722];
		probabilitySliderGF = p[723];
		probabilitySliderBR = p[724];
		probabilitySliderBG = p[725];
		probabilitySliderBB = p[726];
		probabilitySliderBW = p[727];
		probabilitySliderBO = p[728];
		probabilitySliderBK = p[729];
		probabilitySliderBC = p[730];
		probabilitySliderBD = p[731];
		probabilitySliderBY = p[732];
		probabilitySliderBP = p[733];
		probabilitySliderBM = p[734];
		probabilitySliderBA = p[735];
		probabilitySliderBT = p[736];
		probabilitySliderBS = p[737];
		probabilitySliderBF = p[738];
		probabilitySliderWR = p[739];
		probabilitySliderWG = p[740];
		probabilitySliderWB = p[741];
		probabilitySliderWW = p[742];
		probabilitySliderWO = p[743];
		probabilitySliderWK = p[744];
		probabilitySliderWC = p[745];
		probabilitySliderWD = p[746];
		probabilitySliderWY = p[747];
		probabilitySliderWP = p[748];
		probabilitySliderWM = p[749];
		probabilitySliderWA = p[750];
		probabilitySliderWT = p[751];
		probabilitySliderWS = p[752];
		probabilitySliderWF = p[753];
		probabilitySliderOR = p[754];
		probabilitySliderOG = p[755];
		probabilitySliderOB = p[756];
		probabilitySliderOW = p[757];
		probabilitySliderOO = p[758];
		probabilitySliderOK = p[759];
		probabilitySliderOC = p[760];
		probabilitySliderOD = p[761];
		probabilitySliderOY = p[762];
		probabilitySliderOP = p[763];
		probabilitySliderOM = p[764];
		probabilitySliderOA = p[765];
		probabilitySliderOT = p[766];
		probabilitySliderOS = p[767];
		probabilitySliderOF = p[768];
		probabilitySliderKR = p[769];
		probabilitySliderKG = p[770];
		probabilitySliderKB = p[771];
		probabilitySliderKW = p[772];
		probabilitySliderKO = p[773];
		probabilitySliderKK = p[774];
		probabilitySliderKC = p[775];
		probabilitySliderKD = p[776];
		probabilitySliderKY = p[777];
		probabilitySliderKP = p[778];
		probabilitySliderKM = p[779];
		probabilitySliderKA = p[780];
		probabilitySliderKT = p[781];
		probabilitySliderKS = p[782];
		probabilitySliderKF = p[783];
		probabilitySliderCR = p[784];
		probabilitySliderCG = p[785];
		probabilitySliderCB = p[786];
		probabilitySliderCW = p[787];
		probabilitySliderCO = p[788];
		probabilitySliderCK = p[789];
		probabilitySliderCC = p[790];
		probabilitySliderCD = p[791];
		probabilitySliderCY = p[792];
		probabilitySliderCP = p[793];
		probabilitySliderCM = p[794];
		probabilitySliderCA = p[795];
		probabilitySliderCT = p[796];
		probabilitySliderCS = p[797];
		probabilitySliderCF = p[798];
		probabilitySliderDR = p[799];
		probabilitySliderDG = p[800];
		probabilitySliderDB = p[801];
		probabilitySliderDW = p[802];
		probabilitySliderDO = p[803];
		probabilitySliderDK = p[804];
		probabilitySliderDC = p[805];
		probabilitySliderDD = p[806];
		probabilitySliderDY = p[807];
		probabilitySliderDP = p[808];
		probabilitySliderDM = p[809];
		probabilitySliderDA = p[810];
		probabilitySliderDT = p[811];
		probabilitySliderDS = p[812];
		probabilitySliderDF = p[813];
		probabilitySliderYR = p[814];
		probabilitySliderYG = p[815];
		probabilitySliderYB = p[816];
		probabilitySliderYW = p[817];
		probabilitySliderYO = p[818];
		probabilitySliderYK = p[819];
		probabilitySliderYC = p[820];
		probabilitySliderYD = p[821];
		probabilitySliderYY = p[822];
		probabilitySliderYP = p[823];
		probabilitySliderYM = p[824];
		probabilitySliderYA = p[825];
		probabilitySliderYT = p[826];
		probabilitySliderYS = p[827];
		probabilitySliderYF = p[828];
		probabilitySliderPR = p[829];
		probabilitySliderPG = p[830];
		probabilitySliderPB = p[831];
		probabilitySliderPW = p[832];
		probabilitySliderPO = p[833];
		probabilitySliderPK = p[834];
		probabilitySliderPC = p[835];
		probabilitySliderPD = p[836];
		probabilitySliderPY = p[837];
		probabilitySliderPP = p[838];
		probabilitySliderPM = p[839];
		probabilitySliderPA = p[840];
		probabilitySliderPT = p[841];
		probabilitySliderPS = p[842];
		probabilitySliderPF = p[843];
		probabilitySliderMR = p[844];
		probabilitySliderMG = p[845];
		probabilitySliderMB = p[846];
		probabilitySliderMW = p[847];
		probabilitySliderMO = p[848];
		probabilitySliderMK = p[849];
		probabilitySliderMC = p[850];
		probabilitySliderMD = p[851];
		probabilitySliderMY = p[852];
		probabilitySliderMP = p[853];
		probabilitySliderMM = p[854];
		probabilitySliderMA = p[855];
		probabilitySliderMT = p[856];
		probabilitySliderMS = p[857];
		probabilitySliderMF = p[858];
		probabilitySliderAR = p[859];
		probabilitySliderAG = p[860];
		probabilitySliderAB = p[861];
		probabilitySliderAW = p[862];
		probabilitySliderAO = p[863];
		probabilitySliderAK = p[864];
		probabilitySliderAC = p[865];
		probabilitySliderAD = p[866];
		probabilitySliderAY = p[867];
		probabilitySliderAP = p[868];
		probabilitySliderAM = p[869];
		probabilitySliderAA = p[870];
		probabilitySliderAT = p[871];
		probabilitySliderAS = p[872];
		probabilitySliderAF = p[873];
		probabilitySliderTR = p[874];
		probabilitySliderTG = p[875];
		probabilitySliderTB = p[876];
		probabilitySliderTW = p[877];
		probabilitySliderTO = p[878];
		probabilitySliderTK = p[879];
		probabilitySliderTC = p[880];
		probabilitySliderTD = p[881];
		probabilitySliderTY = p[882];
		probabilitySliderTP = p[883];
		probabilitySliderTM = p[884];
		probabilitySliderTA = p[885];
		probabilitySliderTT = p[886];
		probabilitySliderTS = p[887];
		probabilitySliderTF = p[888];
		probabilitySliderSR = p[889];
		probabilitySliderSG = p[890];
		probabilitySliderSB = p[891];
		probabilitySliderSW = p[892];
		probabilitySliderSO = p[893];
		probabilitySliderSK = p[894];
		probabilitySliderSC = p[895];
		probabilitySliderSD = p[896];
		probabilitySliderSY = p[897];
		probabilitySliderSP = p[898];
		probabilitySliderSM = p[899];
		probabilitySliderSA = p[900];
		probabilitySliderST = p[901];
		probabilitySliderSS = p[902];
		probabilitySliderSF = p[903];
		probabilitySliderFR = p[904];
		probabilitySliderFG = p[905];
		probabilitySliderFB = p[906];
		probabilitySliderFW = p[907];
		probabilitySliderFO = p[908];
		probabilitySliderFK = p[909];
		probabilitySliderFC = p[910];
		probabilitySliderFD = p[911];
		probabilitySliderFY = p[912];
		probabilitySliderFP = p[913];
		probabilitySliderFM = p[914];
		probabilitySliderFA = p[915];
		probabilitySliderFT = p[916];
		probabilitySliderFS = p[917];
		probabilitySliderFF = p[918];
	}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - 15c225v225p version 1.5");
	ofSetVerticalSync(true);

	// Interface
	gui.setup("Settings");
	gui.loadFont("Arial", 10);
	gui.setWidthElements(250.0f);
	gui.add(fps.setup("FPS", "0"));
	gui.add(resetButton.setup("Restart"));
	gui.add(save.setup("Save Model"));
	gui.add(load.setup("Load Model"));
	gui.add(modelToggle.setup("Show Model", false));

	rndGroup.setup("Randomize");
	rndGroup.add(randomGeneral.setup("Randomize all parameters (space bar)"));
	rndGroup.add(randomCount.setup("Randomize particle count (q)"));
	rndGroup.add(randomVsc.setup("Randomize Viscosities (v)"));
	rndGroup.add(randomProb.setup("Randomize Probabilities (p)"));
	rndGroup.add(randomChoice.setup("Randomize IP (i)"));
	gui.add(&rndGroup);
	rndGroup.minimize();

	// Global Group
	globalGroup.setup("Global");
	globalGroup.add(evoToggle.setup("Evolve parameters", true));
	globalGroup.add(evoProbSlider.setup("evo chance%", evoChance, 0, 100));
	globalGroup.add(evoAmountSlider.setup("evo amount%%", evoAmount, 0, 100));
	globalGroup.add(probabilitySlider.setup("interaction prob%", probability, 1, 100));
	globalGroup.add(viscositySlider.setup("interaction viscosity", viscosity, 0, 1));
	globalGroup.add(motionBlurToggle.setup("Motion Blur", false));
	globalGroup.add(physicLabel.setup("physic (ms)", "0"));
	globalGroup.add(radiusToogle.setup("infinite radius", false));
	gui.add(&globalGroup);
	globalGroup.minimize();

	// GREEN
	greenGroup.setup("Green");
	greenGroup.add(numberSliderG.setup("Green", pnumberSliderG, 0, 10000));
	greenGroup.add(powerSliderGG.setup("green x green:", ppowerSliderGG, 0, 100));
	greenGroup.add(powerSliderGR.setup("green x red:", ppowerSliderGR, 0, 100));
	greenGroup.add(powerSliderGW.setup("green x white:", ppowerSliderGW, 0, 100));
	greenGroup.add(powerSliderGB.setup("green x blue:", ppowerSliderGB, 0, 100));
	greenGroup.add(powerSliderGO.setup("green x orange:", ppowerSliderGO, 0, 100));
	greenGroup.add(powerSliderGK.setup("green x khaki:", ppowerSliderGK, 0, 100));
	greenGroup.add(powerSliderGC.setup("green x crimson:", ppowerSliderGC, 0, 100));
	greenGroup.add(powerSliderGD.setup("green x dark:", ppowerSliderGD, 0, 100));
	greenGroup.add(powerSliderGY.setup("green x yellow:", ppowerSliderGY, 00, 100));
	greenGroup.add(powerSliderGP.setup("green x pink:", ppowerSliderGP, 0, 100));
	greenGroup.add(powerSliderGM.setup("green x magenta:", ppowerSliderGM, 00, 100));
	greenGroup.add(powerSliderGA.setup("green x aqua:", ppowerSliderGA, 0, 100));
	greenGroup.add(powerSliderGT.setup("green x teal:", ppowerSliderGT, 0, 100));
	greenGroup.add(powerSliderGS.setup("green x silver:", ppowerSliderGS, 0, 100));
	greenGroup.add(powerSliderGF.setup("green x firebrick:", ppowerSliderGF, 0, 100));
	greenGroup.add(vSliderGG.setup("radius g x g:", pvSliderGG, 0, 500));
	greenGroup.add(vSliderGR.setup("radius g x r:", pvSliderGR, 0, 500));
	greenGroup.add(vSliderGW.setup("radius g x w:", pvSliderGW, 0, 500));
	greenGroup.add(vSliderGB.setup("radius g x b:", pvSliderGB, 0, 500));
	greenGroup.add(vSliderGO.setup("radius g x o:", pvSliderGO, 0, 500));
	greenGroup.add(vSliderGK.setup("radius g x k:", pvSliderGK, 0, 500));
	greenGroup.add(vSliderGC.setup("radius g x c:", pvSliderGC, 0, 500));
	greenGroup.add(vSliderGD.setup("radius g x d:", pvSliderGD, 0, 500));
	greenGroup.add(vSliderGY.setup("radius g x y:", pvSliderGY, 0, 500));
	greenGroup.add(vSliderGP.setup("radius g x p:", pvSliderGP, 0, 500));
	greenGroup.add(vSliderGM.setup("radius g x m:", pvSliderGM, 0, 500));
	greenGroup.add(vSliderGA.setup("radius g x a:", pvSliderGA, 0, 500));
	greenGroup.add(vSliderGT.setup("radius g x t:", pvSliderGT, 0, 500));
	greenGroup.add(vSliderGS.setup("radius g x s:", pvSliderGS, 0, 500));
	greenGroup.add(vSliderGF.setup("radius g x f:", pvSliderGF, 0, 500));
	greenGroup.add(viscositySliderGR.setup("Viscosity GR", viscosity, 0, 1));
	greenGroup.add(viscositySliderGG.setup("Viscosity GG", viscosity, 0, 1));
	greenGroup.add(viscositySliderGB.setup("Viscosity GB", viscosity, 0, 1));
	greenGroup.add(viscositySliderGW.setup("Viscosity GW", viscosity, 0, 1));
	greenGroup.add(viscositySliderGO.setup("Viscosity GO", viscosity, 0, 1));
	greenGroup.add(viscositySliderGK.setup("Viscosity GK", viscosity, 0, 1));
	greenGroup.add(viscositySliderGC.setup("Viscosity GC", viscosity, 0, 1));
	greenGroup.add(viscositySliderGD.setup("Viscosity GD", viscosity, 0, 1));
	greenGroup.add(viscositySliderGY.setup("Viscosity GY", viscosity, 0, 1));
	greenGroup.add(viscositySliderGP.setup("Viscosity GP", viscosity, 0, 1));
	greenGroup.add(viscositySliderGM.setup("Viscosity GM", viscosity, 0, 1));
	greenGroup.add(viscositySliderGA.setup("Viscosity GA", viscosity, 0, 1));
	greenGroup.add(viscositySliderGT.setup("Viscosity GT", viscosity, 0, 1));
	greenGroup.add(viscositySliderGS.setup("Viscosity GS", viscosity, 0, 1));
	greenGroup.add(viscositySliderGF.setup("Viscosity GF", viscosity, 0, 1));
	greenGroup.add(probabilitySliderGR.setup("Interaction Probability GR", probabilityGR, 0, 100));
	greenGroup.add(probabilitySliderGG.setup("Interaction Probability GG", probabilityGG, 0, 100));
	greenGroup.add(probabilitySliderGB.setup("Interaction Probability GB", probabilityGB, 0, 100));
	greenGroup.add(probabilitySliderGW.setup("Interaction Probability GW", probabilityGW, 0, 100));
	greenGroup.add(probabilitySliderGO.setup("Interaction Probability GO", probabilityGO, 0, 100));
	greenGroup.add(probabilitySliderGK.setup("Interaction Probability GK", probabilityGK, 0, 100));
	greenGroup.add(probabilitySliderGC.setup("Interaction Probability GC", probabilityGC, 0, 100));
	greenGroup.add(probabilitySliderGD.setup("Interaction Probability GD", probabilityGD, 0, 100));
	greenGroup.add(probabilitySliderGY.setup("Interaction Probability GY", probabilityGY, 0, 100));
	greenGroup.add(probabilitySliderGP.setup("Interaction Probability GP", probabilityGP, 0, 100));
	greenGroup.add(probabilitySliderGM.setup("Interaction Probability GM", probabilityGM, 0, 100));
	greenGroup.add(probabilitySliderGA.setup("Interaction Probability GA", probabilityGA, 0, 100));
	greenGroup.add(probabilitySliderGT.setup("Interaction Probability GT", probabilityGT, 0, 100));
	greenGroup.add(probabilitySliderGS.setup("Interaction Probability GS", probabilityGS, 0, 100));
	greenGroup.add(probabilitySliderGF.setup("Interaction Probability GF", probabilityGF, 0, 100));
	greenGroup.minimize();
	gui.add(&greenGroup);

	// RED
	redGroup.setup("Red");
	redGroup.add(numberSliderR.setup("Red", pnumberSliderR, 0, 10000));
	redGroup.add(powerSliderRG.setup("red x green:", ppowerSliderRG, 0, 100));
	redGroup.add(powerSliderRR.setup("red x red:", ppowerSliderRR, 0, 100));
	redGroup.add(powerSliderRW.setup("red x white:", ppowerSliderRW, 0, 100));
	redGroup.add(powerSliderRB.setup("red x blue:", ppowerSliderRB, 0, 100));
	redGroup.add(powerSliderRO.setup("red x orange:", ppowerSliderRO, 0, 100));
	redGroup.add(powerSliderRK.setup("red x khaki:", ppowerSliderRK, 0, 100));
	redGroup.add(powerSliderRC.setup("red x crimson:", ppowerSliderRC, 0, 100));
	redGroup.add(powerSliderRD.setup("red x dark:", ppowerSliderRD, 0, 100));
	redGroup.add(powerSliderRY.setup("red x yellow:", ppowerSliderRY, 0, 100));
	redGroup.add(powerSliderRP.setup("red x pink:", ppowerSliderRP, 0, 100));
	redGroup.add(powerSliderRM.setup("red x magenta:", ppowerSliderRM, 0, 100));
	redGroup.add(powerSliderRA.setup("red x aqua:", ppowerSliderRA, 0, 100));
	redGroup.add(powerSliderRT.setup("red x teal:", ppowerSliderRT, 0, 100));
	redGroup.add(powerSliderRS.setup("red x silver:", ppowerSliderRS, 0, 100));
	redGroup.add(powerSliderRF.setup("red x firebrick:", ppowerSliderRF, 0, 100));
	redGroup.add(vSliderRG.setup("radius r x g:", pvSliderRG, 0, 500));
	redGroup.add(vSliderRR.setup("radius r x r:", pvSliderRR, 0, 500));
	redGroup.add(vSliderRW.setup("radius r x w:", pvSliderRW, 0, 500));
	redGroup.add(vSliderRB.setup("radius r x b:", pvSliderRB, 0, 500));
	redGroup.add(vSliderRO.setup("radius r x o:", pvSliderRO, 0, 500));
	redGroup.add(vSliderRK.setup("radius r x k:", pvSliderRK, 0, 500));
	redGroup.add(vSliderRC.setup("radius r x c:", pvSliderRC, 0, 500));
	redGroup.add(vSliderRD.setup("radius r x d:", pvSliderRD, 0, 500));
	redGroup.add(vSliderRY.setup("radius r x y:", pvSliderRY, 0, 500));
	redGroup.add(vSliderRP.setup("radius r x p:", pvSliderRP, 0, 500));
	redGroup.add(vSliderRM.setup("radius r x m:", pvSliderRM, 0, 500));
	redGroup.add(vSliderRA.setup("radius r x a:", pvSliderRA, 0, 500));
	redGroup.add(vSliderRT.setup("radius r x t:", pvSliderRT, 0, 500));
	redGroup.add(vSliderRS.setup("radius r x s:", pvSliderRS, 0, 500));
	redGroup.add(vSliderRF.setup("radius r x f:", pvSliderRF, 0, 500));
	redGroup.add(viscositySliderRR.setup("Viscosity RR", viscosity, 0, 1));
	redGroup.add(viscositySliderRG.setup("Viscosity RG", viscosity, 0, 1));
	redGroup.add(viscositySliderRB.setup("Viscosity RB", viscosity, 0, 1));
	redGroup.add(viscositySliderRW.setup("Viscosity RW", viscosity, 0, 1));
	redGroup.add(viscositySliderRO.setup("Viscosity RO", viscosity, 0, 1));
	redGroup.add(viscositySliderRK.setup("Viscosity RK", viscosity, 0, 1));
	redGroup.add(viscositySliderRC.setup("Viscosity RC", viscosity, 0, 1));
	redGroup.add(viscositySliderRD.setup("Viscosity RD", viscosity, 0, 1));
	redGroup.add(viscositySliderRY.setup("Viscosity RY", viscosity, 0, 1));
	redGroup.add(viscositySliderRP.setup("Viscosity RP", viscosity, 0, 1));
	redGroup.add(viscositySliderRM.setup("Viscosity RM", viscosity, 0, 1));
	redGroup.add(viscositySliderRA.setup("Viscosity RA", viscosity, 0, 1));
	redGroup.add(viscositySliderRT.setup("Viscosity RT", viscosity, 0, 1));
	redGroup.add(viscositySliderRS.setup("Viscosity RS", viscosity, 0, 1));
	redGroup.add(viscositySliderRF.setup("Viscosity RF", viscosity, 0, 1));
	redGroup.add(probabilitySliderRR.setup("Interaction Probability RR", probabilityRR, 0, 100));
	redGroup.add(probabilitySliderRG.setup("Interaction Probability RG", probabilityRG, 0, 100));
	redGroup.add(probabilitySliderRB.setup("Interaction Probability RB", probabilityRB, 0, 100));
	redGroup.add(probabilitySliderRW.setup("Interaction Probability RW", probabilityRW, 0, 100));
	redGroup.add(probabilitySliderRO.setup("Interaction Probability RO", probabilityRO, 0, 100));
	redGroup.add(probabilitySliderRK.setup("Interaction Probability RK", probabilityRK, 0, 100));
	redGroup.add(probabilitySliderRC.setup("Interaction Probability RC", probabilityRC, 0, 100));
	redGroup.add(probabilitySliderRD.setup("Interaction Probability RD", probabilityRD, 0, 100));
	redGroup.add(probabilitySliderRY.setup("Interaction Probability RY", probabilityRY, 0, 100));
	redGroup.add(probabilitySliderRP.setup("Interaction Probability RP", probabilityRP, 0, 100));
	redGroup.add(probabilitySliderRM.setup("Interaction Probability RM", probabilityRM, 0, 100));
	redGroup.add(probabilitySliderRA.setup("Interaction Probability RA", probabilityRA, 0, 100));
	redGroup.add(probabilitySliderRT.setup("Interaction Probability RT", probabilityRT, 0, 100));
	redGroup.add(probabilitySliderRS.setup("Interaction Probability RS", probabilityRS, 0, 100));
	redGroup.add(probabilitySliderRF.setup("Interaction Probability RF", probabilityRF, 0, 100));
	redGroup.minimize();
	gui.add(&redGroup);

	// BLUE
	blueGroup.setup("Blue");
	blueGroup.add(numberSliderB.setup("Blue", pnumberSliderB, 0, 10000));
	blueGroup.add(powerSliderBG.setup("blue x green:", ppowerSliderBG, 0, 100));
	blueGroup.add(powerSliderBR.setup("blue x red:", ppowerSliderBR, 0, 100));
	blueGroup.add(powerSliderBW.setup("blue x white:", ppowerSliderBW, 0, 100));
	blueGroup.add(powerSliderBB.setup("blue x blue:", ppowerSliderBB, 0, 100));
	blueGroup.add(powerSliderBO.setup("blue x orange:", ppowerSliderBO, 0, 100));
	blueGroup.add(powerSliderBK.setup("blue x khaki:", ppowerSliderBK, 0, 100));
	blueGroup.add(powerSliderBC.setup("blue x crimson:", ppowerSliderBC, 00, 100));
	blueGroup.add(powerSliderBD.setup("blue x dark:", ppowerSliderBD, 0, 100));
	blueGroup.add(powerSliderBY.setup("blue x yellow:", ppowerSliderBY, 0, 100));
	blueGroup.add(powerSliderBP.setup("blue x pink:", ppowerSliderBP, 0, 100));
	blueGroup.add(powerSliderBM.setup("blue x magenta:", ppowerSliderBM, 0, 100));
	blueGroup.add(powerSliderBA.setup("blue x aqua:", ppowerSliderBA, 0, 100));
	blueGroup.add(powerSliderBT.setup("blue x teal:", ppowerSliderBT, 0, 100));
	blueGroup.add(powerSliderBS.setup("blue x silver:", ppowerSliderBS, 0, 100));
	blueGroup.add(powerSliderBF.setup("blue x firebrick:", ppowerSliderBF, 0, 100));
	blueGroup.add(vSliderBG.setup("radius b x g:", pvSliderBG, 0, 500));
	blueGroup.add(vSliderBR.setup("radius b x r:", pvSliderBR, 0, 500));
	blueGroup.add(vSliderBW.setup("radius b x w:", pvSliderBW, 0, 500));
	blueGroup.add(vSliderBB.setup("radius b x b:", pvSliderBB, 0, 500));
	blueGroup.add(vSliderBO.setup("radius b x o:", pvSliderBO, 0, 500));
	blueGroup.add(vSliderBK.setup("radius b x k:", pvSliderBK, 0, 500));
	blueGroup.add(vSliderBC.setup("radius b x c:", pvSliderBC, 0, 500));
	blueGroup.add(vSliderBD.setup("radius b x d:", pvSliderBD, 0, 500));
	blueGroup.add(vSliderBY.setup("radius b x y:", pvSliderBY, 0, 500));
	blueGroup.add(vSliderBP.setup("radius b x p:", pvSliderBP, 0, 500));
	blueGroup.add(vSliderBM.setup("radius b x m:", pvSliderBM, 0, 500));
	blueGroup.add(vSliderBA.setup("radius b x a:", pvSliderBA, 0, 500));
	blueGroup.add(vSliderBT.setup("radius b x t:", pvSliderBT, 0, 500));
	blueGroup.add(vSliderBS.setup("radius b x s:", pvSliderBS, 0, 500));
	blueGroup.add(vSliderBF.setup("radius b x f:", pvSliderBF, 0, 500));
	blueGroup.add(viscositySliderBR.setup("Viscosity BR", viscosity, 0, 1));
	blueGroup.add(viscositySliderBG.setup("Viscosity BG", viscosity, 0, 1));
	blueGroup.add(viscositySliderBB.setup("Viscosity BB", viscosity, 0, 1));
	blueGroup.add(viscositySliderBW.setup("Viscosity BW", viscosity, 0, 1));
	blueGroup.add(viscositySliderBO.setup("Viscosity BO", viscosity, 0, 1));
	blueGroup.add(viscositySliderBK.setup("Viscosity BK", viscosity, 0, 1));
	blueGroup.add(viscositySliderBC.setup("Viscosity BC", viscosity, 0, 1));
	blueGroup.add(viscositySliderBD.setup("Viscosity BD", viscosity, 0, 1));
	blueGroup.add(viscositySliderBY.setup("Viscosity BY", viscosity, 0, 1));
	blueGroup.add(viscositySliderBP.setup("Viscosity BP", viscosity, 0, 1));
	blueGroup.add(viscositySliderBM.setup("Viscosity BM", viscosity, 0, 1));
	blueGroup.add(viscositySliderBA.setup("Viscosity BA", viscosity, 0, 1));
	blueGroup.add(viscositySliderBT.setup("Viscosity BT", viscosity, 0, 1));
	blueGroup.add(viscositySliderBS.setup("Viscosity BS", viscosity, 0, 1));
	blueGroup.add(viscositySliderBF.setup("Viscosity BF", viscosity, 0, 1));
	blueGroup.add(probabilitySliderBR.setup("Interaction Probability BR", probabilityBR, 0, 100));
	blueGroup.add(probabilitySliderBG.setup("Interaction Probability BG", probabilityBG, 0, 100));
	blueGroup.add(probabilitySliderBB.setup("Interaction Probability BB", probabilityBB, 0, 100));
	blueGroup.add(probabilitySliderBW.setup("Interaction Probability BW", probabilityBW, 0, 100));
	blueGroup.add(probabilitySliderBO.setup("Interaction Probability BO", probabilityBO, 0, 100));
	blueGroup.add(probabilitySliderBK.setup("Interaction Probability BK", probabilityBK, 0, 100));
	blueGroup.add(probabilitySliderBC.setup("Interaction Probability BC", probabilityBC, 0, 100));
	blueGroup.add(probabilitySliderBD.setup("Interaction Probability BD", probabilityBD, 0, 100));
	blueGroup.add(probabilitySliderBY.setup("Interaction Probability BY", probabilityBY, 0, 100));
	blueGroup.add(probabilitySliderBP.setup("Interaction Probability BP", probabilityBP, 0, 100));
	blueGroup.add(probabilitySliderBM.setup("Interaction Probability BM", probabilityBM, 0, 100));
	blueGroup.add(probabilitySliderBA.setup("Interaction Probability BA", probabilityBA, 0, 100));
	blueGroup.add(probabilitySliderBT.setup("Interaction Probability BT", probabilityBT, 0, 100));
	blueGroup.add(probabilitySliderBS.setup("Interaction Probability BS", probabilityBS, 0, 100));
	blueGroup.add(probabilitySliderBF.setup("Interaction Probability BF", probabilityBF, 0, 100));
	blueGroup.minimize();
	gui.add(&blueGroup);

	// WHITE
	whiteGroup.setup("White");
	whiteGroup.add(numberSliderW.setup("White", pnumberSliderW, 0, 10000));
	whiteGroup.add(powerSliderWG.setup("white x green:", ppowerSliderWG, 0, 100));
	whiteGroup.add(powerSliderWR.setup("white x red:", ppowerSliderWR, 0, 100));
	whiteGroup.add(powerSliderWW.setup("white x white:", ppowerSliderWW, 0, 100));
	whiteGroup.add(powerSliderWB.setup("white x blue:", ppowerSliderWB, 0, 100));
	whiteGroup.add(powerSliderWO.setup("white x orange:", ppowerSliderWO, 0, 100));
	whiteGroup.add(powerSliderWK.setup("white x khaki:", ppowerSliderWK, 0, 100));
	whiteGroup.add(powerSliderWC.setup("white x crimson:", ppowerSliderWC, 0, 100));
	whiteGroup.add(powerSliderWD.setup("white x dark:", ppowerSliderWD, 0, 100));
	whiteGroup.add(powerSliderWY.setup("white x yellow:", ppowerSliderWY, 0, 100));
	whiteGroup.add(powerSliderWP.setup("white x pink:", ppowerSliderWP, 0, 100));
	whiteGroup.add(powerSliderWM.setup("white x magenta:", ppowerSliderWM, 0, 100));
	whiteGroup.add(powerSliderWA.setup("white x aqua:", ppowerSliderWA, 0, 100));
	whiteGroup.add(powerSliderWT.setup("white x teal:", ppowerSliderWT, 0, 100));
	whiteGroup.add(powerSliderWS.setup("white x silver:", ppowerSliderWS, 0, 100));
	whiteGroup.add(powerSliderWF.setup("white x firebrick:", ppowerSliderWF, 0, 100));
	whiteGroup.add(vSliderWG.setup("radius w x g:", pvSliderWG, 0, 500));
	whiteGroup.add(vSliderWR.setup("radius w x r:", pvSliderWR, 0, 500));
	whiteGroup.add(vSliderWW.setup("radius w x w:", pvSliderWW, 0, 500));
	whiteGroup.add(vSliderWB.setup("radius w x b:", pvSliderWB, 0, 500));
	whiteGroup.add(vSliderWO.setup("radius w x o:", pvSliderWO, 0, 500));
	whiteGroup.add(vSliderWK.setup("radius w x k:", pvSliderWK, 0, 500));
	whiteGroup.add(vSliderWC.setup("radius w x c:", pvSliderWC, 0, 500));
	whiteGroup.add(vSliderWD.setup("radius w x d:", pvSliderWD, 0, 500));
	whiteGroup.add(vSliderWY.setup("radius w x y:", pvSliderWY, 0, 500));
	whiteGroup.add(vSliderWP.setup("radius w x p:", pvSliderWP, 0, 500));
	whiteGroup.add(vSliderWM.setup("radius w x m:", pvSliderWM, 0, 500));
	whiteGroup.add(vSliderWA.setup("radius w x a:", pvSliderWA, 0, 500));
	whiteGroup.add(vSliderWT.setup("radius w x t:", pvSliderWT, 0, 500));
	whiteGroup.add(vSliderWS.setup("radius w x s:", pvSliderWS, 0, 500));
	whiteGroup.add(vSliderWF.setup("radius w x f:", pvSliderWF, 0, 500));
	whiteGroup.add(viscositySliderWR.setup("Viscosity WR", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWG.setup("Viscosity WG", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWB.setup("Viscosity WB", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWW.setup("Viscosity WW", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWO.setup("Viscosity WO", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWK.setup("Viscosity WK", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWC.setup("Viscosity WC", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWD.setup("Viscosity WD", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWY.setup("Viscosity WY", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWP.setup("Viscosity WP", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWM.setup("Viscosity WM", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWA.setup("Viscosity WA", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWT.setup("Viscosity WT", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWS.setup("Viscosity WS", viscosity, 0, 1));
	whiteGroup.add(viscositySliderWF.setup("Viscosity WF", viscosity, 0, 1));
	whiteGroup.add(probabilitySliderWR.setup("Interaction Probability WR", probabilityWR, 0, 100));
	whiteGroup.add(probabilitySliderWG.setup("Interaction Probability WG", probabilityWG, 0, 100));
	whiteGroup.add(probabilitySliderWB.setup("Interaction Probability WB", probabilityWB, 0, 100));
	whiteGroup.add(probabilitySliderWW.setup("Interaction Probability WW", probabilityWW, 0, 100));
	whiteGroup.add(probabilitySliderWO.setup("Interaction Probability WO", probabilityWO, 0, 100));
	whiteGroup.add(probabilitySliderWK.setup("Interaction Probability WK", probabilityWK, 0, 100));
	whiteGroup.add(probabilitySliderWC.setup("Interaction Probability WC", probabilityWC, 0, 100));
	whiteGroup.add(probabilitySliderWD.setup("Interaction Probability WD", probabilityWD, 0, 100));
	whiteGroup.add(probabilitySliderWY.setup("Interaction Probability WY", probabilityWY, 0, 100));
	whiteGroup.add(probabilitySliderWP.setup("Interaction Probability WP", probabilityWP, 0, 100));
	whiteGroup.add(probabilitySliderWM.setup("Interaction Probability WM", probabilityWM, 0, 100));
	whiteGroup.add(probabilitySliderWA.setup("Interaction Probability WA", probabilityWA, 0, 100));
	whiteGroup.add(probabilitySliderWT.setup("Interaction Probability WT", probabilityWT, 0, 100));
	whiteGroup.add(probabilitySliderWS.setup("Interaction Probability WS", probabilityWS, 0, 100));
	whiteGroup.add(probabilitySliderWF.setup("Interaction Probability WF", probabilityWF, 0, 100));
	whiteGroup.minimize();
	gui.add(&whiteGroup);

	// ORANGE
	orangeGroup.setup("Orange");
	orangeGroup.add(numberSliderO.setup("Orange", pnumberSliderO, 0, 10000));
	orangeGroup.add(powerSliderOG.setup("orange x green:", ppowerSliderOG, -100, 100));
	orangeGroup.add(powerSliderOR.setup("orange x red:", ppowerSliderOR, -100, 100));
	orangeGroup.add(powerSliderOW.setup("orange x white:", ppowerSliderOW, -100, 100));
	orangeGroup.add(powerSliderOB.setup("orange x blue:", ppowerSliderOB, -100, 100));
	orangeGroup.add(powerSliderOO.setup("orange x orange:", ppowerSliderOO, -100, 100));
	orangeGroup.add(powerSliderOK.setup("orange x khaki:", ppowerSliderOK, -100, 100));
	orangeGroup.add(powerSliderOC.setup("orange x crimson:", ppowerSliderOC, -100, 100));
	orangeGroup.add(powerSliderOD.setup("orange x dark:", ppowerSliderOD, -100, 100));
	orangeGroup.add(powerSliderOY.setup("orange x yellow:", ppowerSliderOY, -100, 100));
	orangeGroup.add(powerSliderOP.setup("orange x pink:", ppowerSliderOP, -100, 100));
	orangeGroup.add(powerSliderOM.setup("orange x magenta:", ppowerSliderOM, -100, 100));
	orangeGroup.add(powerSliderOA.setup("orange x aqua:", ppowerSliderOA, -100, 100));
	orangeGroup.add(powerSliderOT.setup("orange x teal:", ppowerSliderOT, -100, 100));
	orangeGroup.add(powerSliderOS.setup("orange x silver:", ppowerSliderOS, -100, 100));
	orangeGroup.add(powerSliderOF.setup("orange x firebrick:", ppowerSliderOF, -100, 100));
	orangeGroup.add(vSliderOG.setup("radius o x g:", pvSliderOG, 0, 500));
	orangeGroup.add(vSliderOR.setup("radius o x r:", pvSliderOR, 0, 500));
	orangeGroup.add(vSliderOW.setup("radius o x w:", pvSliderOW, 0, 500));
	orangeGroup.add(vSliderOB.setup("radius o x b:", pvSliderOB, 0, 500));
	orangeGroup.add(vSliderOO.setup("radius o x o:", pvSliderOO, 0, 500));
	orangeGroup.add(vSliderOK.setup("radius o x k:", pvSliderOK, 0, 500));
	orangeGroup.add(vSliderOC.setup("radius o x c:", pvSliderOC, 0, 500));
	orangeGroup.add(vSliderOD.setup("radius o x d:", pvSliderOD, 0, 500));
	orangeGroup.add(vSliderOY.setup("radius o x y:", pvSliderOY, 0, 500));
	orangeGroup.add(vSliderOP.setup("radius o x p:", pvSliderOP, 0, 500));
	orangeGroup.add(vSliderOM.setup("radius o x m:", pvSliderOM, 0, 500));
	orangeGroup.add(vSliderOA.setup("radius o x a:", pvSliderOA, 0, 500));
	orangeGroup.add(vSliderOT.setup("radius o x t:", pvSliderOT, 0, 500));
	orangeGroup.add(vSliderOS.setup("radius o x s:", pvSliderOS, 0, 500));
	orangeGroup.add(vSliderOF.setup("radius o x f:", pvSliderOF, 0, 500));
	orangeGroup.add(viscositySliderOR.setup("Viscosity OR", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOG.setup("Viscosity OG", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOB.setup("Viscosity OB", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOW.setup("Viscosity OW", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOO.setup("Viscosity OO", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOK.setup("Viscosity OK", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOC.setup("Viscosity OC", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOD.setup("Viscosity OD", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOY.setup("Viscosity OY", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOP.setup("Viscosity OP", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOM.setup("Viscosity OM", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOA.setup("Viscosity OA", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOT.setup("Viscosity OT", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOS.setup("Viscosity OS", viscosity, 0, 1));
	orangeGroup.add(viscositySliderOF.setup("Viscosity OF", viscosity, 0, 1));
	orangeGroup.add(probabilitySliderOR.setup("Interaction Probability OR", probabilityOR, 0, 100));
	orangeGroup.add(probabilitySliderOG.setup("Interaction Probability OG", probabilityOG, 0, 100));
	orangeGroup.add(probabilitySliderOB.setup("Interaction Probability OB", probabilityOB, 0, 100));
	orangeGroup.add(probabilitySliderOW.setup("Interaction Probability OW", probabilityOW, 0, 100));
	orangeGroup.add(probabilitySliderOO.setup("Interaction Probability OO", probabilityOO, 0, 100));
	orangeGroup.add(probabilitySliderOK.setup("Interaction Probability OK", probabilityOK, 0, 100));
	orangeGroup.add(probabilitySliderOC.setup("Interaction Probability OC", probabilityOC, 0, 100));
	orangeGroup.add(probabilitySliderOD.setup("Interaction Probability OD", probabilityOD, 0, 100));
	orangeGroup.add(probabilitySliderOY.setup("Interaction Probability OY", probabilityOY, 0, 100));
	orangeGroup.add(probabilitySliderOP.setup("Interaction Probability OP", probabilityOP, 0, 100));
	orangeGroup.add(probabilitySliderOM.setup("Interaction Probability OM", probabilityOM, 0, 100));
	orangeGroup.add(probabilitySliderOA.setup("Interaction Probability OA", probabilityOA, 0, 100));
	orangeGroup.add(probabilitySliderOT.setup("Interaction Probability OT", probabilityOT, 0, 100));
	orangeGroup.add(probabilitySliderOS.setup("Interaction Probability OS", probabilityOS, 0, 100));
	orangeGroup.add(probabilitySliderOF.setup("Interaction Probability OF", probabilityOF, 0, 100));
	orangeGroup.minimize();
	gui.add(&orangeGroup);

	// KHAKI
	khakiGroup.setup("Khaki");
	khakiGroup.add(numberSliderK.setup("Khaki", pnumberSliderK, 0, 10000));
	khakiGroup.add(powerSliderKG.setup("khaki x green:", ppowerSliderKG, -100, 100));
	khakiGroup.add(powerSliderKR.setup("khaki x red:", ppowerSliderKR, -100, 100));
	khakiGroup.add(powerSliderKW.setup("khaki x white:", ppowerSliderKW, -100, 100));
	khakiGroup.add(powerSliderKB.setup("khaki x blue:", ppowerSliderKB, -100, 100));
	khakiGroup.add(powerSliderKO.setup("khaki x orange:", ppowerSliderKO, -100, 100));
	khakiGroup.add(powerSliderKK.setup("khaki x khaki:", ppowerSliderKK, -100, 100));
	khakiGroup.add(powerSliderKC.setup("khaki x crimson:", ppowerSliderKC, -100, 100));
	khakiGroup.add(powerSliderKD.setup("khaki x dark:", ppowerSliderKD, -100, 100));
	khakiGroup.add(powerSliderKY.setup("khaki x yellow:", ppowerSliderKY, -100, 100));
	khakiGroup.add(powerSliderKP.setup("khaki x pink:", ppowerSliderKP, -100, 100));
	khakiGroup.add(powerSliderKM.setup("khaki x magenta:", ppowerSliderKM, -100, 100));
	khakiGroup.add(powerSliderKA.setup("khaki x aqua:", ppowerSliderKA, -100, 100));
	khakiGroup.add(powerSliderKT.setup("khaki x teal:", ppowerSliderKT, -100, 100));
	khakiGroup.add(powerSliderKS.setup("khaki x silver:", ppowerSliderKS, -100, 100));
	khakiGroup.add(powerSliderKF.setup("khaki x firebrick:", ppowerSliderKF, -100, 100));
	khakiGroup.add(vSliderKG.setup("radius k x g:", pvSliderKG, 0, 500));
	khakiGroup.add(vSliderKR.setup("radius k x r:", pvSliderKR, 0, 500));
	khakiGroup.add(vSliderKW.setup("radius k x w:", pvSliderKW, 0, 500));
	khakiGroup.add(vSliderKB.setup("radius k x b:", pvSliderKB, 0, 500));
	khakiGroup.add(vSliderKO.setup("radius k x o:", pvSliderKO, 0, 500));
	khakiGroup.add(vSliderKK.setup("radius k x k:", pvSliderKK, 0, 500));
	khakiGroup.add(vSliderKC.setup("radius k x c:", pvSliderKC, 0, 500));
	khakiGroup.add(vSliderKD.setup("radius k x d:", pvSliderKD, 0, 500));
	khakiGroup.add(vSliderKY.setup("radius k x y:", pvSliderKY, 0, 500));
	khakiGroup.add(vSliderKP.setup("radius k x p:", pvSliderKP, 0, 500));
	khakiGroup.add(vSliderKM.setup("radius k x m:", pvSliderKM, 0, 500));
	khakiGroup.add(vSliderKA.setup("radius k x a:", pvSliderKA, 0, 500));
	khakiGroup.add(vSliderKT.setup("radius k x t:", pvSliderKT, 0, 500));
	khakiGroup.add(vSliderKS.setup("radius k x s:", pvSliderKS, 0, 500));
	khakiGroup.add(vSliderKF.setup("radius k x f:", pvSliderKF, 0, 500));
	khakiGroup.add(viscositySliderKR.setup("Viscosity KR", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKG.setup("Viscosity KG", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKB.setup("Viscosity KB", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKW.setup("Viscosity KW", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKO.setup("Viscosity KO", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKK.setup("Viscosity KK", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKC.setup("Viscosity KC", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKD.setup("Viscosity KD", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKY.setup("Viscosity KY", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKP.setup("Viscosity KP", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKM.setup("Viscosity KM", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKA.setup("Viscosity KA", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKT.setup("Viscosity KT", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKS.setup("Viscosity KS", viscosity, 0, 1));
	khakiGroup.add(viscositySliderKF.setup("Viscosity KF", viscosity, 0, 1));
	khakiGroup.add(probabilitySliderKR.setup("Interaction Probability KR", probabilityKR, 0, 100));
	khakiGroup.add(probabilitySliderKG.setup("Interaction Probability KG", probabilityKG, 0, 100));
	khakiGroup.add(probabilitySliderKB.setup("Interaction Probability KB", probabilityKB, 0, 100));
	khakiGroup.add(probabilitySliderKW.setup("Interaction Probability KW", probabilityKW, 0, 100));
	khakiGroup.add(probabilitySliderKO.setup("Interaction Probability KO", probabilityKO, 0, 100));
	khakiGroup.add(probabilitySliderKK.setup("Interaction Probability KK", probabilityKK, 0, 100));
	khakiGroup.add(probabilitySliderKC.setup("Interaction Probability KC", probabilityKC, 0, 100));
	khakiGroup.add(probabilitySliderKD.setup("Interaction Probability KD", probabilityKD, 0, 100));
	khakiGroup.add(probabilitySliderKY.setup("Interaction Probability KY", probabilityKY, 0, 100));
	khakiGroup.add(probabilitySliderKP.setup("Interaction Probability KP", probabilityKP, 0, 100));
	khakiGroup.add(probabilitySliderKM.setup("Interaction Probability KM", probabilityKM, 0, 100));
	khakiGroup.add(probabilitySliderKA.setup("Interaction Probability KA", probabilityKA, 0, 100));
	khakiGroup.add(probabilitySliderKT.setup("Interaction Probability KT", probabilityKT, 0, 100));
	khakiGroup.add(probabilitySliderKS.setup("Interaction Probability KS", probabilityKS, 0, 100));
	khakiGroup.add(probabilitySliderKF.setup("Interaction Probability KF", probabilityKF, 0, 100));
	khakiGroup.minimize();
	gui.add(&khakiGroup);

	// CRIMSON
	crimsonGroup.setup("Crimson");
	crimsonGroup.add(numberSliderC.setup("Crimson", pnumberSliderC, 0, 10000));
	crimsonGroup.add(powerSliderCG.setup("crimson x green:", ppowerSliderCG, -100, 100));
	crimsonGroup.add(powerSliderCR.setup("crimson x red:", ppowerSliderCR, -100, 100));
	crimsonGroup.add(powerSliderCW.setup("crimson x white:", ppowerSliderCW, -100, 100));
	crimsonGroup.add(powerSliderCB.setup("crimson x blue:", ppowerSliderCB, -100, 100));
	crimsonGroup.add(powerSliderCO.setup("crimson x orange:", ppowerSliderCO, -100, 100));
	crimsonGroup.add(powerSliderCK.setup("crimson x khaki:", ppowerSliderCK, -100, 100));
	crimsonGroup.add(powerSliderCC.setup("crimson x crimson:", ppowerSliderCC, -100, 100));
	crimsonGroup.add(powerSliderCD.setup("crimson x dark:", ppowerSliderCD, -100, 100));
	crimsonGroup.add(powerSliderCY.setup("crimson x yellow:", ppowerSliderCY, -100, 100));
	crimsonGroup.add(powerSliderCP.setup("crimson x pink:", ppowerSliderCP, -100, 100));
	crimsonGroup.add(powerSliderCM.setup("crimson x magenta:", ppowerSliderCM, -100, 100));
	crimsonGroup.add(powerSliderCA.setup("crimson x aqua:", ppowerSliderCA, -100, 100));
	crimsonGroup.add(powerSliderCT.setup("crimson x teal:", ppowerSliderCT, -100, 100));
	crimsonGroup.add(powerSliderCS.setup("crimson x silver:", ppowerSliderCS, -100, 100));
	crimsonGroup.add(powerSliderCF.setup("crimson x firebrick:", ppowerSliderCF, -100, 100));
	crimsonGroup.add(vSliderCG.setup("radius c x g:", pvSliderCG, 0, 500));
	crimsonGroup.add(vSliderCR.setup("radius c x r:", pvSliderCR, 0, 500));
	crimsonGroup.add(vSliderCW.setup("radius c x w:", pvSliderCW, 0, 500));
	crimsonGroup.add(vSliderCB.setup("radius c x b:", pvSliderCB, 0, 500));
	crimsonGroup.add(vSliderCO.setup("radius c x o:", pvSliderCO, 0, 500));
	crimsonGroup.add(vSliderCK.setup("radius c x k:", pvSliderCK, 0, 500));
	crimsonGroup.add(vSliderCC.setup("radius c x c:", pvSliderCC, 0, 500));
	crimsonGroup.add(vSliderCD.setup("radius c x d:", pvSliderCD, 0, 500));
	crimsonGroup.add(vSliderCY.setup("radius c x y:", pvSliderCY, 0, 500));
	crimsonGroup.add(vSliderCP.setup("radius c x p:", pvSliderCP, 0, 500));
	crimsonGroup.add(vSliderCM.setup("radius c x m:", pvSliderCM, 0, 500));
	crimsonGroup.add(vSliderCA.setup("radius c x a:", pvSliderCA, 0, 500));
	crimsonGroup.add(vSliderCT.setup("radius c x t:", pvSliderCT, 0, 500));
	crimsonGroup.add(vSliderCS.setup("radius c x s:", pvSliderCS, 0, 500));
	crimsonGroup.add(vSliderCF.setup("radius c x f:", pvSliderCF, 0, 500));
	crimsonGroup.add(viscositySliderCR.setup("Viscosity CR", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCG.setup("Viscosity CG", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCB.setup("Viscosity CB", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCW.setup("Viscosity CW", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCO.setup("Viscosity CO", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCK.setup("Viscosity CK", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCC.setup("Viscosity CC", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCD.setup("Viscosity CD", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCY.setup("Viscosity CY", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCP.setup("Viscosity CP", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCM.setup("Viscosity CM", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCA.setup("Viscosity CA", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCT.setup("Viscosity CT", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCS.setup("Viscosity CS", viscosity, 0, 1));
	crimsonGroup.add(viscositySliderCF.setup("Viscosity CF", viscosity, 0, 1));
	crimsonGroup.add(probabilitySliderCR.setup("Interaction Probability CR", probabilityCR, 0, 100));
	crimsonGroup.add(probabilitySliderCG.setup("Interaction Probability CG", probabilityCG, 0, 100));
	crimsonGroup.add(probabilitySliderCB.setup("Interaction Probability CB", probabilityCB, 0, 100));
	crimsonGroup.add(probabilitySliderCW.setup("Interaction Probability CW", probabilityCW, 0, 100));
	crimsonGroup.add(probabilitySliderCO.setup("Interaction Probability CO", probabilityCO, 0, 100));
	crimsonGroup.add(probabilitySliderCK.setup("Interaction Probability CK", probabilityCK, 0, 100));
	crimsonGroup.add(probabilitySliderCC.setup("Interaction Probability CC", probabilityCC, 0, 100));
	crimsonGroup.add(probabilitySliderCD.setup("Interaction Probability CD", probabilityCD, 0, 100));
	crimsonGroup.add(probabilitySliderCY.setup("Interaction Probability CY", probabilityCY, 0, 100));
	crimsonGroup.add(probabilitySliderCP.setup("Interaction Probability CP", probabilityCP, 0, 100));
	crimsonGroup.add(probabilitySliderCM.setup("Interaction Probability CM", probabilityCM, 0, 100));
	crimsonGroup.add(probabilitySliderCA.setup("Interaction Probability CA", probabilityCA, 0, 100));
	crimsonGroup.add(probabilitySliderCT.setup("Interaction Probability CT", probabilityCT, 0, 100));
	crimsonGroup.add(probabilitySliderCS.setup("Interaction Probability CS", probabilityCS, 0, 100));
	crimsonGroup.add(probabilitySliderCF.setup("Interaction Probability CF", probabilityCF, 0, 100));
	crimsonGroup.minimize();
	gui.add(&crimsonGroup);

	// DARK
	darkGroup.setup("Dark");
	darkGroup.add(numberSliderD.setup("Dark", pnumberSliderD, 0, 10000));
	darkGroup.add(powerSliderDG.setup("dark x green:", ppowerSliderDG, -100, 100));
	darkGroup.add(powerSliderDR.setup("dark x red:", ppowerSliderDR, -100, 100));
	darkGroup.add(powerSliderDW.setup("dark x white:", ppowerSliderDW, -100, 100));
	darkGroup.add(powerSliderDB.setup("dark x blue:", ppowerSliderDB, -100, 100));
	darkGroup.add(powerSliderDO.setup("dark x orange:", ppowerSliderDO, -100, 100));
	darkGroup.add(powerSliderDK.setup("dark x khaki:", ppowerSliderDK, -100, 100));
	darkGroup.add(powerSliderDC.setup("dark x crimson:", ppowerSliderDC, -100, 100));
	darkGroup.add(powerSliderDD.setup("dark x dark:", ppowerSliderDD, -100, 100));
	darkGroup.add(powerSliderDY.setup("dark x yellow:", ppowerSliderDY, -100, 100));
	darkGroup.add(powerSliderDP.setup("dark x pink:", ppowerSliderDP, -100, 100));
	darkGroup.add(powerSliderDM.setup("dark x magenta:", ppowerSliderDM, -100, 100));
	darkGroup.add(powerSliderDA.setup("dark x aqua:", ppowerSliderDA, -100, 100));
	darkGroup.add(powerSliderDT.setup("dark x teal:", ppowerSliderDT, -100, 100));
	darkGroup.add(powerSliderDS.setup("dark x silver:", ppowerSliderDS, -100, 100));
	darkGroup.add(powerSliderDF.setup("dark x firebrick:", ppowerSliderDF, -100, 100));
	darkGroup.add(vSliderDG.setup("radius d x g:", pvSliderDG, 0, 500));
	darkGroup.add(vSliderDR.setup("radius d x r:", pvSliderDR, 0, 500));
	darkGroup.add(vSliderDW.setup("radius d x w:", pvSliderDW, 0, 500));
	darkGroup.add(vSliderDB.setup("radius d x b:", pvSliderDB, 0, 500));
	darkGroup.add(vSliderDO.setup("radius d x o:", pvSliderDO, 0, 500));
	darkGroup.add(vSliderDK.setup("radius d x k:", pvSliderDK, 0, 500));
	darkGroup.add(vSliderDC.setup("radius d x c:", pvSliderDC, 0, 500));
	darkGroup.add(vSliderDD.setup("radius d x d:", pvSliderDD, 0, 500));
	darkGroup.add(vSliderDY.setup("radius d x y:", pvSliderDY, 0, 500));
	darkGroup.add(vSliderDP.setup("radius d x p:", pvSliderDP, 0, 500));
	darkGroup.add(vSliderDM.setup("radius d x m:", pvSliderDM, 0, 500));
	darkGroup.add(vSliderDA.setup("radius d x a:", pvSliderDA, 0, 500));
	darkGroup.add(vSliderDT.setup("radius d x t:", pvSliderDT, 0, 500));
	darkGroup.add(vSliderDS.setup("radius d x s:", pvSliderDS, 0, 500));
	darkGroup.add(vSliderDF.setup("radius d x f:", pvSliderDF, 0, 500));
	darkGroup.add(viscositySliderDR.setup("Viscosity DR", viscosity, 0, 1));
	darkGroup.add(viscositySliderDG.setup("Viscosity DG", viscosity, 0, 1));
	darkGroup.add(viscositySliderDB.setup("Viscosity DB", viscosity, 0, 1));
	darkGroup.add(viscositySliderDW.setup("Viscosity DW", viscosity, 0, 1));
	darkGroup.add(viscositySliderDO.setup("Viscosity DO", viscosity, 0, 1));
	darkGroup.add(viscositySliderDK.setup("Viscosity DK", viscosity, 0, 1));
	darkGroup.add(viscositySliderDC.setup("Viscosity DC", viscosity, 0, 1));
	darkGroup.add(viscositySliderDD.setup("Viscosity DD", viscosity, 0, 1));
	darkGroup.add(viscositySliderDY.setup("Viscosity DY", viscosity, 0, 1));
	darkGroup.add(viscositySliderDP.setup("Viscosity DP", viscosity, 0, 1));
	darkGroup.add(viscositySliderDM.setup("Viscosity DM", viscosity, 0, 1));
	darkGroup.add(viscositySliderDA.setup("Viscosity DA", viscosity, 0, 1));
	darkGroup.add(viscositySliderDT.setup("Viscosity DT", viscosity, 0, 1));
	darkGroup.add(viscositySliderDS.setup("Viscosity DS", viscosity, 0, 1));
	darkGroup.add(viscositySliderDF.setup("Viscosity DF", viscosity, 0, 1));
	darkGroup.add(probabilitySliderDR.setup("Interaction Probability DR", probabilityDR, 0, 100));
	darkGroup.add(probabilitySliderDG.setup("Interaction Probability DG", probabilityDG, 0, 100));
	darkGroup.add(probabilitySliderDB.setup("Interaction Probability DB", probabilityDB, 0, 100));
	darkGroup.add(probabilitySliderDW.setup("Interaction Probability DW", probabilityDW, 0, 100));
	darkGroup.add(probabilitySliderDO.setup("Interaction Probability DO", probabilityDO, 0, 100));
	darkGroup.add(probabilitySliderDK.setup("Interaction Probability DK", probabilityDK, 0, 100));
	darkGroup.add(probabilitySliderDC.setup("Interaction Probability DC", probabilityDC, 0, 100));
	darkGroup.add(probabilitySliderDD.setup("Interaction Probability DD", probabilityDD, 0, 100));
	darkGroup.add(probabilitySliderDY.setup("Interaction Probability DY", probabilityDY, 0, 100));
	darkGroup.add(probabilitySliderDP.setup("Interaction Probability DP", probabilityDP, 0, 100));
	darkGroup.add(probabilitySliderDM.setup("Interaction Probability DM", probabilityDM, 0, 100));
	darkGroup.add(probabilitySliderDA.setup("Interaction Probability DA", probabilityDA, 0, 100));
	darkGroup.add(probabilitySliderDT.setup("Interaction Probability DT", probabilityDT, 0, 100));
	darkGroup.add(probabilitySliderDS.setup("Interaction Probability DS", probabilityDS, 0, 100));
	darkGroup.add(probabilitySliderDF.setup("Interaction Probability DF", probabilityDF, 0, 100));
	darkGroup.minimize();
	gui.add(&darkGroup);

	// YELLOW
	yellowGroup.setup("Yellow");
	yellowGroup.add(numberSliderY.setup("Yellow", pnumberSliderY, 0, 10000));
	yellowGroup.add(powerSliderYG.setup("yellow x green:", ppowerSliderYG, -100, 0));
	yellowGroup.add(powerSliderYR.setup("yellow x red:", ppowerSliderYR, -100, 0));
	yellowGroup.add(powerSliderYW.setup("yellow x white:", ppowerSliderYW, -100, 0));
	yellowGroup.add(powerSliderYB.setup("yellow x blue:", ppowerSliderYB, -100, 0));
	yellowGroup.add(powerSliderYO.setup("yellow x orange:", ppowerSliderYO, -100, 0));
	yellowGroup.add(powerSliderYK.setup("yellow x khaki:", ppowerSliderYK, -100, 0));
	yellowGroup.add(powerSliderYC.setup("yellow x crimson:", ppowerSliderYC, -100, 0));
	yellowGroup.add(powerSliderYD.setup("yellow x dark:", ppowerSliderYD, -100, 0));
	yellowGroup.add(powerSliderYY.setup("yellow x yellow:", ppowerSliderYY, -100, 0));
	yellowGroup.add(powerSliderYP.setup("yellow x pink:", ppowerSliderYP, -100, 0));
	yellowGroup.add(powerSliderYM.setup("yellow x magenta:", ppowerSliderYM, -100, 0));
	yellowGroup.add(powerSliderYA.setup("yellow x aqua:", ppowerSliderYA, -100, 0));
	yellowGroup.add(powerSliderYT.setup("yellow x teal:", ppowerSliderYT, -100, 0));
	yellowGroup.add(powerSliderYS.setup("yellow x silver:", ppowerSliderYS, -100, 0));
	yellowGroup.add(powerSliderYF.setup("yellow x firebrick:", ppowerSliderYF, -100, 0));
	yellowGroup.add(vSliderYG.setup("radius y x g:", pvSliderYG, 0, 500));
	yellowGroup.add(vSliderYR.setup("radius y x r:", pvSliderYR, 0, 500));
	yellowGroup.add(vSliderYW.setup("radius y x w:", pvSliderYW, 0, 500));
	yellowGroup.add(vSliderYB.setup("radius y x b:", pvSliderYB, 0, 500));
	yellowGroup.add(vSliderYO.setup("radius y x o:", pvSliderYO, 0, 500));
	yellowGroup.add(vSliderYK.setup("radius y x k:", pvSliderYK, 0, 500));
	yellowGroup.add(vSliderYC.setup("radius y x c:", pvSliderYC, 0, 500));
	yellowGroup.add(vSliderYD.setup("radius y x d:", pvSliderYD, 0, 500));
	yellowGroup.add(vSliderYY.setup("radius y x y:", pvSliderYY, 0, 500));
	yellowGroup.add(vSliderYP.setup("radius y x p:", pvSliderYP, 0, 500));
	yellowGroup.add(vSliderYM.setup("radius y x m:", pvSliderYM, 0, 500));
	yellowGroup.add(vSliderYA.setup("radius y x a:", pvSliderYA, 0, 500));
	yellowGroup.add(vSliderYT.setup("radius y x t:", pvSliderYT, 0, 500));
	yellowGroup.add(vSliderYS.setup("radius y x s:", pvSliderYS, 0, 500));
	yellowGroup.add(vSliderYF.setup("radius y x f:", pvSliderYF, 0, 500));
	yellowGroup.add(viscositySliderYR.setup("Viscosity YR", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYG.setup("Viscosity YG", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYB.setup("Viscosity YB", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYW.setup("Viscosity YW", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYO.setup("Viscosity YO", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYK.setup("Viscosity YK", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYC.setup("Viscosity YC", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYD.setup("Viscosity YD", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYY.setup("Viscosity YY", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYP.setup("Viscosity YP", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYM.setup("Viscosity YM", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYA.setup("Viscosity YA", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYT.setup("Viscosity YT", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYS.setup("Viscosity YS", viscosity, 0, 1));
	yellowGroup.add(viscositySliderYF.setup("Viscosity YF", viscosity, 0, 1));
	yellowGroup.add(probabilitySliderYR.setup("Interaction Probability YR", probabilityYR, 0, 100));
	yellowGroup.add(probabilitySliderYG.setup("Interaction Probability YG", probabilityYG, 0, 100));
	yellowGroup.add(probabilitySliderYB.setup("Interaction Probability YB", probabilityYB, 0, 100));
	yellowGroup.add(probabilitySliderYW.setup("Interaction Probability YW", probabilityYW, 0, 100));
	yellowGroup.add(probabilitySliderYO.setup("Interaction Probability YO", probabilityYO, 0, 100));
	yellowGroup.add(probabilitySliderYK.setup("Interaction Probability YK", probabilityYK, 0, 100));
	yellowGroup.add(probabilitySliderYC.setup("Interaction Probability YC", probabilityYC, 0, 100));
	yellowGroup.add(probabilitySliderYD.setup("Interaction Probability YD", probabilityYD, 0, 100));
	yellowGroup.add(probabilitySliderYY.setup("Interaction Probability YY", probabilityYY, 0, 100));
	yellowGroup.add(probabilitySliderYP.setup("Interaction Probability YP", probabilityYP, 0, 100));
	yellowGroup.add(probabilitySliderYM.setup("Interaction Probability YM", probabilityYM, 0, 100));
	yellowGroup.add(probabilitySliderYA.setup("Interaction Probability YA", probabilityYA, 0, 100));
	yellowGroup.add(probabilitySliderYT.setup("Interaction Probability YT", probabilityYT, 0, 100));
	yellowGroup.add(probabilitySliderYS.setup("Interaction Probability YS", probabilityYS, 0, 100));
	yellowGroup.add(probabilitySliderYF.setup("Interaction Probability YF", probabilityYF, 0, 100));
	yellowGroup.minimize();
	gui.add(&yellowGroup);

	// PINK
	pinkGroup.setup("Pink");
	pinkGroup.add(numberSliderP.setup("Pink", pnumberSliderP, 0, 10000));
	pinkGroup.add(powerSliderPG.setup("Pink x green:", ppowerSliderPG, -100, 0));
	pinkGroup.add(powerSliderPR.setup("Pink x red:", ppowerSliderPR, -100, 0));
	pinkGroup.add(powerSliderPW.setup("Pink x white:", ppowerSliderPW, -100, 0));
	pinkGroup.add(powerSliderPB.setup("Pink x blue:", ppowerSliderPB, -100, 0));
	pinkGroup.add(powerSliderPO.setup("Pink x orange:", ppowerSliderPO, -100, 0));
	pinkGroup.add(powerSliderPK.setup("Pink x khaki:", ppowerSliderPK, -100, 0));
	pinkGroup.add(powerSliderPC.setup("Pink x crimson:", ppowerSliderPC, -100, 0));
	pinkGroup.add(powerSliderPD.setup("Pink x dark:", ppowerSliderPD, -100, 0));
	pinkGroup.add(powerSliderPY.setup("Pink x yellow:", ppowerSliderPY, -100, 0));
	pinkGroup.add(powerSliderPP.setup("Pink x pink:", ppowerSliderPP, -100, 0));
	pinkGroup.add(powerSliderPM.setup("Pink x magenta:", ppowerSliderPM, -100, 0));
	pinkGroup.add(powerSliderPA.setup("Pink x aqua:", ppowerSliderPA, -100, 0));
	pinkGroup.add(powerSliderPT.setup("Pink x teal:", ppowerSliderPT, -100, 0));
	pinkGroup.add(powerSliderPS.setup("Pink x silver:", ppowerSliderPS, -100, 0));
	pinkGroup.add(powerSliderPF.setup("Pink x firebrick:", ppowerSliderPF, -100, 0));
	pinkGroup.add(vSliderPG.setup("radius p x g:", pvSliderPG, 0, 500));
	pinkGroup.add(vSliderPR.setup("radius p x r:", pvSliderPR, 0, 500));
	pinkGroup.add(vSliderPW.setup("radius p x w:", pvSliderPW, 0, 500));
	pinkGroup.add(vSliderPB.setup("radius p x b:", pvSliderPB, 0, 500));
	pinkGroup.add(vSliderPO.setup("radius p x o:", pvSliderPO, 0, 500));
	pinkGroup.add(vSliderPK.setup("radius p x k:", pvSliderPK, 0, 500));
	pinkGroup.add(vSliderPC.setup("radius p x c:", pvSliderPC, 0, 500));
	pinkGroup.add(vSliderPD.setup("radius p x d:", pvSliderPD, 0, 500));
	pinkGroup.add(vSliderPY.setup("radius p x y:", pvSliderPY, 0, 500));
	pinkGroup.add(vSliderPP.setup("radius p x p:", pvSliderPP, 0, 500));
	pinkGroup.add(vSliderPM.setup("radius p x m:", pvSliderPM, 0, 500));
	pinkGroup.add(vSliderPA.setup("radius p x a:", pvSliderPA, 0, 500));
	pinkGroup.add(vSliderPT.setup("radius p x t:", pvSliderPT, 0, 500));
	pinkGroup.add(vSliderPS.setup("radius p x s:", pvSliderPS, 0, 500));
	pinkGroup.add(vSliderPF.setup("radius p x f:", pvSliderPF, 0, 500));
	pinkGroup.add(viscositySliderPR.setup("Viscosity PR", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPG.setup("Viscosity PG", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPB.setup("Viscosity PB", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPW.setup("Viscosity PW", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPO.setup("Viscosity PO", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPK.setup("Viscosity PK", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPC.setup("Viscosity PC", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPD.setup("Viscosity PD", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPY.setup("Viscosity PY", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPP.setup("Viscosity PP", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPM.setup("Viscosity PM", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPA.setup("Viscosity PA", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPT.setup("Viscosity PT", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPS.setup("Viscosity PS", viscosity, 0, 1));
	pinkGroup.add(viscositySliderPF.setup("Viscosity PF", viscosity, 0, 1));
	pinkGroup.add(probabilitySliderPR.setup("Interaction Probability PR", probabilityPR, 0, 100));
	pinkGroup.add(probabilitySliderPG.setup("Interaction Probability PG", probabilityPG, 0, 100));
	pinkGroup.add(probabilitySliderPB.setup("Interaction Probability PB", probabilityPB, 0, 100));
	pinkGroup.add(probabilitySliderPW.setup("Interaction Probability PW", probabilityPW, 0, 100));
	pinkGroup.add(probabilitySliderPO.setup("Interaction Probability PO", probabilityPO, 0, 100));
	pinkGroup.add(probabilitySliderPK.setup("Interaction Probability PK", probabilityPK, 0, 100));
	pinkGroup.add(probabilitySliderPC.setup("Interaction Probability PC", probabilityPC, 0, 100));
	pinkGroup.add(probabilitySliderPD.setup("Interaction Probability PD", probabilityPD, 0, 100));
	pinkGroup.add(probabilitySliderPY.setup("Interaction Probability PY", probabilityPY, 0, 100));
	pinkGroup.add(probabilitySliderPP.setup("Interaction Probability PP", probabilityPP, 0, 100));
	pinkGroup.add(probabilitySliderPM.setup("Interaction Probability PM", probabilityPM, 0, 100));
	pinkGroup.add(probabilitySliderPA.setup("Interaction Probability PA", probabilityPA, 0, 100));
	pinkGroup.add(probabilitySliderPT.setup("Interaction Probability PT", probabilityPT, 0, 100));
	pinkGroup.add(probabilitySliderPS.setup("Interaction Probability PS", probabilityPS, 0, 100));
	pinkGroup.add(probabilitySliderPF.setup("Interaction Probability PF", probabilityPF, 0, 100));
	pinkGroup.minimize();
	gui.add(&pinkGroup);

	// MAGENTA
	magentaGroup.setup("Magenta");
	magentaGroup.add(numberSliderM.setup("Magenta", pnumberSliderM, 0, 10000));
	magentaGroup.add(powerSliderMG.setup("magenta x green:", ppowerSliderMG, -100, 0));
	magentaGroup.add(powerSliderMR.setup("magenta x red:", ppowerSliderMR, -100, 0));
	magentaGroup.add(powerSliderMW.setup("magenta x white:", ppowerSliderMW, -100, 0));
	magentaGroup.add(powerSliderMB.setup("magenta x blue:", ppowerSliderMB, -100, 0));
	magentaGroup.add(powerSliderMO.setup("magenta x orange:", ppowerSliderMO, -100, 0));
	magentaGroup.add(powerSliderMK.setup("magenta x khaki:", ppowerSliderMK, -100, 0));
	magentaGroup.add(powerSliderMC.setup("magenta x crimson:", ppowerSliderMC, -100, 0));
	magentaGroup.add(powerSliderMD.setup("magenta x dark:", ppowerSliderMD, -100, 0));
	magentaGroup.add(powerSliderMY.setup("magenta x yellow:", ppowerSliderMY, -100, 0));
	magentaGroup.add(powerSliderMP.setup("magenta x pink:", ppowerSliderMP, -100, 0));
	magentaGroup.add(powerSliderMM.setup("magenta x magenta:", ppowerSliderMM, -100, 0));
	magentaGroup.add(powerSliderMA.setup("magenta x aqua:", ppowerSliderMA, -100, 0));
	magentaGroup.add(powerSliderMT.setup("magenta x teal:", ppowerSliderMT, -100, 0));
	magentaGroup.add(powerSliderMS.setup("magenta x silver:", ppowerSliderMS, -100, 0));
	magentaGroup.add(powerSliderMF.setup("magenta x firebrick:", ppowerSliderMF, -100, 0));
	magentaGroup.add(vSliderMG.setup("radius m x g:", pvSliderMG, 0, 500));
	magentaGroup.add(vSliderMR.setup("radius m x r:", pvSliderMR, 0, 500));
	magentaGroup.add(vSliderMW.setup("radius m x w:", pvSliderMW, 0, 500));
	magentaGroup.add(vSliderMB.setup("radius m x b:", pvSliderMB, 0, 500));
	magentaGroup.add(vSliderMO.setup("radius m x o:", pvSliderMO, 0, 500));
	magentaGroup.add(vSliderMK.setup("radius m x k:", pvSliderMK, 0, 500));
	magentaGroup.add(vSliderMC.setup("radius m x c:", pvSliderMC, 0, 500));
	magentaGroup.add(vSliderMD.setup("radius m x d:", pvSliderMD, 0, 500));
	magentaGroup.add(vSliderMY.setup("radius m x y:", pvSliderMY, 0, 500));
	magentaGroup.add(vSliderMP.setup("radius m x p:", pvSliderMP, 0, 500));
	magentaGroup.add(vSliderMM.setup("radius m x m:", pvSliderMM, 0, 500));
	magentaGroup.add(vSliderMA.setup("radius m x a:", pvSliderMA, 0, 500));
	magentaGroup.add(vSliderMT.setup("radius m x t:", pvSliderMT, 0, 500));
	magentaGroup.add(vSliderMS.setup("radius m x s:", pvSliderMS, 0, 500));
	magentaGroup.add(vSliderMF.setup("radius m x f:", pvSliderMF, 0, 500));
	magentaGroup.add(viscositySliderMR.setup("Viscosity MR", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMG.setup("Viscosity MG", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMB.setup("Viscosity MB", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMW.setup("Viscosity MW", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMO.setup("Viscosity MO", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMK.setup("Viscosity MK", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMC.setup("Viscosity MC", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMD.setup("Viscosity MD", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMY.setup("Viscosity MY", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMP.setup("Viscosity MP", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMM.setup("Viscosity MM", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMA.setup("Viscosity MA", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMT.setup("Viscosity MT", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMS.setup("Viscosity MS", viscosity, 0, 1));
	magentaGroup.add(viscositySliderMF.setup("Viscosity MF", viscosity, 0, 1));
	magentaGroup.add(probabilitySliderMR.setup("Interaction Probability MR", probabilityMR, 0, 100));
	magentaGroup.add(probabilitySliderMG.setup("Interaction Probability MG", probabilityMG, 0, 100));
	magentaGroup.add(probabilitySliderMB.setup("Interaction Probability MB", probabilityMB, 0, 100));
	magentaGroup.add(probabilitySliderMW.setup("Interaction Probability MW", probabilityMW, 0, 100));
	magentaGroup.add(probabilitySliderMO.setup("Interaction Probability MO", probabilityMO, 0, 100));
	magentaGroup.add(probabilitySliderMK.setup("Interaction Probability MK", probabilityMK, 0, 100));
	magentaGroup.add(probabilitySliderMC.setup("Interaction Probability MC", probabilityMC, 0, 100));
	magentaGroup.add(probabilitySliderMD.setup("Interaction Probability MD", probabilityMD, 0, 100));
	magentaGroup.add(probabilitySliderMY.setup("Interaction Probability MY", probabilityMY, 0, 100));
	magentaGroup.add(probabilitySliderMP.setup("Interaction Probability MP", probabilityMP, 0, 100));
	magentaGroup.add(probabilitySliderMM.setup("Interaction Probability MM", probabilityMM, 0, 100));
	magentaGroup.add(probabilitySliderMA.setup("Interaction Probability MA", probabilityMA, 0, 100));
	magentaGroup.add(probabilitySliderMT.setup("Interaction Probability MT", probabilityMT, 0, 100));
	magentaGroup.add(probabilitySliderMS.setup("Interaction Probability MS", probabilityMS, 0, 100));
	magentaGroup.add(probabilitySliderMF.setup("Interaction Probability MF", probabilityMF, 0, 100));
	magentaGroup.minimize();
	gui.add(&magentaGroup);

	// AQUA
	aquaGroup.setup("Aqua");
	aquaGroup.add(numberSliderA.setup("Aqua", pnumberSliderA, 0, 10000));
	aquaGroup.add(powerSliderAG.setup("aqua x green:", ppowerSliderAG, -100, 0));
	aquaGroup.add(powerSliderAR.setup("aqua x red:", ppowerSliderAR, -100, 0));
	aquaGroup.add(powerSliderAW.setup("aqua x white:", ppowerSliderAW, -100, 0));
	aquaGroup.add(powerSliderAB.setup("aqua x blue:", ppowerSliderAB, -100, 0));
	aquaGroup.add(powerSliderAO.setup("aqua x orange:", ppowerSliderAO, -100, 0));
	aquaGroup.add(powerSliderAK.setup("aqua x khaki:", ppowerSliderAK, -100, 0));
	aquaGroup.add(powerSliderAC.setup("aqua x crimson:", ppowerSliderAC, -100, 0));
	aquaGroup.add(powerSliderAD.setup("aqua x dark:", ppowerSliderAD, -100, 0));
	aquaGroup.add(powerSliderAY.setup("aqua x yellow:", ppowerSliderAY, -100, 0));
	aquaGroup.add(powerSliderAP.setup("aqua x pink:", ppowerSliderAP, -100, 0));
	aquaGroup.add(powerSliderAM.setup("aqua x magenta:", ppowerSliderAM, -100, 0));
	aquaGroup.add(powerSliderAA.setup("aqua x aqua:", ppowerSliderAA, -100, 0));
	aquaGroup.add(powerSliderAT.setup("aqua x teal:", ppowerSliderAT, -100, 0));
	aquaGroup.add(powerSliderAS.setup("aqua x silver:", ppowerSliderAS, -100, 0));
	aquaGroup.add(powerSliderAF.setup("aqua x firebrick:", ppowerSliderAF, -100, 0));
	aquaGroup.add(vSliderAG.setup("radius a x g:", pvSliderAG, 0, 500));
	aquaGroup.add(vSliderAR.setup("radius a x r:", pvSliderAR, 0, 500));
	aquaGroup.add(vSliderAW.setup("radius a x w:", pvSliderAW, 0, 500));
	aquaGroup.add(vSliderAB.setup("radius a x b:", pvSliderAB, 0, 500));
	aquaGroup.add(vSliderAO.setup("radius a x o:", pvSliderAO, 0, 500));
	aquaGroup.add(vSliderAK.setup("radius a x k:", pvSliderAK, 0, 500));
	aquaGroup.add(vSliderAC.setup("radius a x c:", pvSliderAC, 0, 500));
	aquaGroup.add(vSliderAD.setup("radius a x d:", pvSliderAD, 0, 500));
	aquaGroup.add(vSliderAY.setup("radius a x y:", pvSliderAY, 0, 500));
	aquaGroup.add(vSliderAP.setup("radius a x p:", pvSliderAP, 0, 500));
	aquaGroup.add(vSliderAM.setup("radius a x m:", pvSliderAM, 0, 500));
	aquaGroup.add(vSliderAA.setup("radius a x a:", pvSliderAA, 0, 500));
	aquaGroup.add(vSliderAT.setup("radius a x t:", pvSliderAT, 0, 500));
	aquaGroup.add(vSliderAS.setup("radius a x s:", pvSliderAS, 0, 500));
	aquaGroup.add(vSliderAF.setup("radius a x f:", pvSliderAF, 0, 500));
	aquaGroup.add(viscositySliderAR.setup("Viscosity AR", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAG.setup("Viscosity AG", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAB.setup("Viscosity AB", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAW.setup("Viscosity AW", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAO.setup("Viscosity AO", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAK.setup("Viscosity AK", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAC.setup("Viscosity AC", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAD.setup("Viscosity AD", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAY.setup("Viscosity AY", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAP.setup("Viscosity AP", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAM.setup("Viscosity AM", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAA.setup("Viscosity AA", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAT.setup("Viscosity AT", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAS.setup("Viscosity AS", viscosity, 0, 1));
	aquaGroup.add(viscositySliderAF.setup("Viscosity AF", viscosity, 0, 1));
	aquaGroup.add(probabilitySliderAR.setup("Interaction Probability AR", probabilityAR, 0, 100));
	aquaGroup.add(probabilitySliderAG.setup("Interaction Probability AG", probabilityAG, 0, 100));
	aquaGroup.add(probabilitySliderAB.setup("Interaction Probability AB", probabilityAB, 0, 100));
	aquaGroup.add(probabilitySliderAW.setup("Interaction Probability AW", probabilityAW, 0, 100));
	aquaGroup.add(probabilitySliderAO.setup("Interaction Probability AO", probabilityAO, 0, 100));
	aquaGroup.add(probabilitySliderAK.setup("Interaction Probability AK", probabilityAK, 0, 100));
	aquaGroup.add(probabilitySliderAC.setup("Interaction Probability AC", probabilityAC, 0, 100));
	aquaGroup.add(probabilitySliderAD.setup("Interaction Probability AD", probabilityAD, 0, 100));
	aquaGroup.add(probabilitySliderAY.setup("Interaction Probability AY", probabilityAY, 0, 100));
	aquaGroup.add(probabilitySliderAP.setup("Interaction Probability AP", probabilityAP, 0, 100));
	aquaGroup.add(probabilitySliderAM.setup("Interaction Probability AM", probabilityAM, 0, 100));
	aquaGroup.add(probabilitySliderAA.setup("Interaction Probability AA", probabilityAA, 0, 100));
	aquaGroup.add(probabilitySliderAT.setup("Interaction Probability AT", probabilityAT, 0, 100));
	aquaGroup.add(probabilitySliderAS.setup("Interaction Probability AS", probabilityAS, 0, 100));
	aquaGroup.add(probabilitySliderAF.setup("Interaction Probability AF", probabilityAF, 0, 100));
	aquaGroup.minimize();
	gui.add(&aquaGroup);

	// TEAL
	tealGroup.setup("Teal");
	tealGroup.add(numberSliderT.setup("Teal", pnumberSliderT, 0, 10000));
	tealGroup.add(powerSliderTG.setup("teal x green:", ppowerSliderTG, 0, 100));
	tealGroup.add(powerSliderTR.setup("teal x red:", ppowerSliderTR, 0, 100));
	tealGroup.add(powerSliderTW.setup("teal x white:", ppowerSliderTW, 0, 100));
	tealGroup.add(powerSliderTB.setup("teal x blue:", ppowerSliderTB, 0, 100));
	tealGroup.add(powerSliderTO.setup("teal x orange:", ppowerSliderTO, 0, 100));
	tealGroup.add(powerSliderTK.setup("teal x khaki:", ppowerSliderTK, 0, 100));
	tealGroup.add(powerSliderTC.setup("teal x crimson:", ppowerSliderTC, 0, 100));
	tealGroup.add(powerSliderTD.setup("teal x dark:", ppowerSliderTD, 0, 100));
	tealGroup.add(powerSliderTY.setup("teal x yellow:", ppowerSliderTY, 0, 100));
	tealGroup.add(powerSliderTP.setup("teal x pink:", ppowerSliderTP, 0, 100));
	tealGroup.add(powerSliderTM.setup("teal x magenta:", ppowerSliderTM, 0, 100));
	tealGroup.add(powerSliderTA.setup("teal x aqua:", ppowerSliderTA, 0, 100));
	tealGroup.add(powerSliderTT.setup("teal x teal:", ppowerSliderTT, 0, 100));
	tealGroup.add(powerSliderTS.setup("teal x silver:", ppowerSliderTS, 0, 100));
	tealGroup.add(powerSliderTF.setup("teal x firebrick:", ppowerSliderTF, 0, 100));
	tealGroup.add(vSliderTG.setup("radius t x g:", pvSliderTG, 0, 500));
	tealGroup.add(vSliderTR.setup("radius t x r:", pvSliderTR, 0, 500));
	tealGroup.add(vSliderTW.setup("radius t x w:", pvSliderTW, 0, 500));
	tealGroup.add(vSliderTB.setup("radius t x b:", pvSliderTB, 0, 500));
	tealGroup.add(vSliderTO.setup("radius t x o:", pvSliderTO, 0, 500));
	tealGroup.add(vSliderTK.setup("radius t x k:", pvSliderTK, 0, 500));
	tealGroup.add(vSliderTC.setup("radius t x c:", pvSliderTC, 0, 500));
	tealGroup.add(vSliderTD.setup("radius t x d:", pvSliderTD, 0, 500));
	tealGroup.add(vSliderTY.setup("radius t x y:", pvSliderTY, 0, 500));
	tealGroup.add(vSliderTP.setup("radius t x p:", pvSliderTP, 0, 500));
	tealGroup.add(vSliderTM.setup("radius t x m:", pvSliderTM, 0, 500));
	tealGroup.add(vSliderTA.setup("radius t x a:", pvSliderTA, 0, 500));
	tealGroup.add(vSliderTT.setup("radius t x t:", pvSliderTT, 0, 500));
	tealGroup.add(vSliderTS.setup("radius t x s:", pvSliderTS, 0, 500));
	tealGroup.add(vSliderTF.setup("radius t x f:", pvSliderTF, 0, 500));
	tealGroup.add(viscositySliderTR.setup("Viscosity TR", viscosity, 0, 1));
	tealGroup.add(viscositySliderTG.setup("Viscosity TG", viscosity, 0, 1));
	tealGroup.add(viscositySliderTB.setup("Viscosity TB", viscosity, 0, 1));
	tealGroup.add(viscositySliderTW.setup("Viscosity TW", viscosity, 0, 1));
	tealGroup.add(viscositySliderTO.setup("Viscosity TO", viscosity, 0, 1));
	tealGroup.add(viscositySliderTK.setup("Viscosity TK", viscosity, 0, 1));
	tealGroup.add(viscositySliderTC.setup("Viscosity TC", viscosity, 0, 1));
	tealGroup.add(viscositySliderTD.setup("Viscosity TD", viscosity, 0, 1));
	tealGroup.add(viscositySliderTY.setup("Viscosity TY", viscosity, 0, 1));
	tealGroup.add(viscositySliderTP.setup("Viscosity TP", viscosity, 0, 1));
	tealGroup.add(viscositySliderTM.setup("Viscosity TM", viscosity, 0, 1));
	tealGroup.add(viscositySliderTA.setup("Viscosity TA", viscosity, 0, 1));
	tealGroup.add(viscositySliderTT.setup("Viscosity TT", viscosity, 0, 1));
	tealGroup.add(viscositySliderTS.setup("Viscosity TS", viscosity, 0, 1));
	tealGroup.add(viscositySliderTF.setup("Viscosity TF", viscosity, 0, 1));
	tealGroup.add(probabilitySliderTR.setup("Interaction Probability TR", probabilityTR, 0, 100));
	tealGroup.add(probabilitySliderTG.setup("Interaction Probability TG", probabilityTG, 0, 100));
	tealGroup.add(probabilitySliderTB.setup("Interaction Probability TB", probabilityTB, 0, 100));
	tealGroup.add(probabilitySliderTW.setup("Interaction Probability TW", probabilityTW, 0, 100));
	tealGroup.add(probabilitySliderTO.setup("Interaction Probability TO", probabilityTO, 0, 100));
	tealGroup.add(probabilitySliderTK.setup("Interaction Probability TK", probabilityTK, 0, 100));
	tealGroup.add(probabilitySliderTC.setup("Interaction Probability TC", probabilityTC, 0, 100));
	tealGroup.add(probabilitySliderTD.setup("Interaction Probability TD", probabilityTD, 0, 100));
	tealGroup.add(probabilitySliderTY.setup("Interaction Probability TY", probabilityTY, 0, 100));
	tealGroup.add(probabilitySliderTP.setup("Interaction Probability TP", probabilityTP, 0, 100));
	tealGroup.add(probabilitySliderTM.setup("Interaction Probability TM", probabilityTM, 0, 100));
	tealGroup.add(probabilitySliderTA.setup("Interaction Probability TA", probabilityTA, 0, 100));
	tealGroup.add(probabilitySliderTT.setup("Interaction Probability TT", probabilityTT, 0, 100));
	tealGroup.add(probabilitySliderTS.setup("Interaction Probability TS", probabilityTS, 0, 100));
	tealGroup.add(probabilitySliderTF.setup("Interaction Probability TF", probabilityTF, 0, 100));
	tealGroup.minimize();
	gui.add(&tealGroup);

	// SILVER
	silverGroup.setup("Silver");
	silverGroup.add(numberSliderS.setup("Silver", pnumberSliderS, 0, 10000));
	silverGroup.add(powerSliderSG.setup("silver x green:", ppowerSliderSG, -100, 100));
	silverGroup.add(powerSliderSR.setup("silver x red:", ppowerSliderSR, -100, 100));
	silverGroup.add(powerSliderSW.setup("silver x white:", ppowerSliderSW, -100, 100));
	silverGroup.add(powerSliderSB.setup("silver x blue:", ppowerSliderSB, -100, 100));
	silverGroup.add(powerSliderSO.setup("silver x orange:", ppowerSliderSO, -100, 100));
	silverGroup.add(powerSliderSK.setup("silver x khaki:", ppowerSliderSK, -100, 100));
	silverGroup.add(powerSliderSC.setup("silver x crimson:", ppowerSliderSC, -100, 100));
	silverGroup.add(powerSliderSD.setup("silver x dark:", ppowerSliderSD, -100, 100));
	silverGroup.add(powerSliderSY.setup("silver x yellow:", ppowerSliderSY, -100, 100));
	silverGroup.add(powerSliderSP.setup("silver x pink:", ppowerSliderSP, -100, 100));
	silverGroup.add(powerSliderSM.setup("silver x magenta:", ppowerSliderSM, -100, 100));
	silverGroup.add(powerSliderSA.setup("silver x aqua:", ppowerSliderSA, -100, 100));
	silverGroup.add(powerSliderST.setup("silver x teal:", ppowerSliderST, -100, 100));
	silverGroup.add(powerSliderSS.setup("silver x silver:", ppowerSliderSS, -100, 100));
	silverGroup.add(powerSliderSF.setup("silver x firebrick:", ppowerSliderSF, -100, 100));
	silverGroup.add(vSliderSG.setup("radius s x g:", pvSliderSG, 0, 500));
	silverGroup.add(vSliderSR.setup("radius s x r:", pvSliderSR, 0, 500));
	silverGroup.add(vSliderSW.setup("radius s x w:", pvSliderSW, 0, 500));
	silverGroup.add(vSliderSB.setup("radius s x b:", pvSliderSB, 0, 500));
	silverGroup.add(vSliderSO.setup("radius s x o:", pvSliderSO, 0, 500));
	silverGroup.add(vSliderSK.setup("radius s x k:", pvSliderSK, 0, 500));
	silverGroup.add(vSliderSC.setup("radius s x c:", pvSliderSC, 0, 500));
	silverGroup.add(vSliderSD.setup("radius s x d:", pvSliderSD, 0, 500));
	silverGroup.add(vSliderSY.setup("radius s x y:", pvSliderSY, 0, 500));
	silverGroup.add(vSliderSP.setup("radius s x p:", pvSliderSP, 0, 500));
	silverGroup.add(vSliderSM.setup("radius s x m:", pvSliderSM, 0, 500));
	silverGroup.add(vSliderSA.setup("radius s x a:", pvSliderSA, 0, 500));
	silverGroup.add(vSliderST.setup("radius s x t:", pvSliderST, 0, 500));
	silverGroup.add(vSliderSS.setup("radius s x s:", pvSliderSS, 0, 500));
	silverGroup.add(vSliderSF.setup("radius s x f:", pvSliderSF, 0, 500));
	silverGroup.add(viscositySliderSR.setup("Viscosity SR", viscosity, 0, 1));
	silverGroup.add(viscositySliderSG.setup("Viscosity SG", viscosity, 0, 1));
	silverGroup.add(viscositySliderSB.setup("Viscosity SB", viscosity, 0, 1));
	silverGroup.add(viscositySliderSW.setup("Viscosity SW", viscosity, 0, 1));
	silverGroup.add(viscositySliderSO.setup("Viscosity SO", viscosity, 0, 1));
	silverGroup.add(viscositySliderSK.setup("Viscosity SK", viscosity, 0, 1));
	silverGroup.add(viscositySliderSC.setup("Viscosity SC", viscosity, 0, 1));
	silverGroup.add(viscositySliderSD.setup("Viscosity SD", viscosity, 0, 1));
	silverGroup.add(viscositySliderSY.setup("Viscosity SY", viscosity, 0, 1));
	silverGroup.add(viscositySliderSP.setup("Viscosity SP", viscosity, 0, 1));
	silverGroup.add(viscositySliderSM.setup("Viscosity SM", viscosity, 0, 1));
	silverGroup.add(viscositySliderSA.setup("Viscosity SA", viscosity, 0, 1));
	silverGroup.add(viscositySliderST.setup("Viscosity ST", viscosity, 0, 1));
	silverGroup.add(viscositySliderSS.setup("Viscosity SS", viscosity, 0, 1));
	silverGroup.add(viscositySliderSF.setup("Viscosity SF", viscosity, 0, 1));
	silverGroup.add(probabilitySliderSR.setup("Interaction Probability SR", probabilitySR, 0, 100));
	silverGroup.add(probabilitySliderSG.setup("Interaction Probability SG", probabilitySG, 0, 100));
	silverGroup.add(probabilitySliderSB.setup("Interaction Probability SB", probabilitySB, 0, 100));
	silverGroup.add(probabilitySliderSW.setup("Interaction Probability SW", probabilitySW, 0, 100));
	silverGroup.add(probabilitySliderSO.setup("Interaction Probability SO", probabilitySO, 0, 100));
	silverGroup.add(probabilitySliderSK.setup("Interaction Probability SK", probabilitySK, 0, 100));
	silverGroup.add(probabilitySliderSC.setup("Interaction Probability SC", probabilitySC, 0, 100));
	silverGroup.add(probabilitySliderSD.setup("Interaction Probability SD", probabilitySD, 0, 100));
	silverGroup.add(probabilitySliderSY.setup("Interaction Probability SY", probabilitySY, 0, 100));
	silverGroup.add(probabilitySliderSP.setup("Interaction Probability SP", probabilitySP, 0, 100));
	silverGroup.add(probabilitySliderSM.setup("Interaction Probability SM", probabilitySM, 0, 100));
	silverGroup.add(probabilitySliderSA.setup("Interaction Probability SA", probabilitySA, 0, 100));
	silverGroup.add(probabilitySliderST.setup("Interaction Probability ST", probabilityST, 0, 100));
	silverGroup.add(probabilitySliderSS.setup("Interaction Probability SS", probabilitySS, 0, 100));
	silverGroup.add(probabilitySliderSF.setup("Interaction Probability SF", probabilitySF, 0, 100));
	silverGroup.minimize();
	gui.add(&silverGroup);

	// FIREBRICK
	firebrickGroup.setup("Firebrick");
	firebrickGroup.add(numberSliderF.setup("Firebrick", pnumberSliderF, 0, 10000));
	firebrickGroup.add(powerSliderFG.setup("firebrick x green:", ppowerSliderFG, -100, 0));
	firebrickGroup.add(powerSliderFR.setup("firebrick x red:", ppowerSliderFR, -100, 0));
	firebrickGroup.add(powerSliderFW.setup("firebrick x white:", ppowerSliderFW, -100, 0));
	firebrickGroup.add(powerSliderFB.setup("firebrick x blue:", ppowerSliderFB, -100, 0));
	firebrickGroup.add(powerSliderFO.setup("firebrick x orange:", ppowerSliderFO, -100, 0));
	firebrickGroup.add(powerSliderFK.setup("firebrick x khaki:", ppowerSliderFK, -100, 0));
	firebrickGroup.add(powerSliderFC.setup("firebrick x crimson:", ppowerSliderFC, -100, 0));
	firebrickGroup.add(powerSliderFD.setup("firebrick x dark:", ppowerSliderFD, -100, 0));
	firebrickGroup.add(powerSliderFY.setup("firebrick x yellow:", ppowerSliderFY, -100, 0));
	firebrickGroup.add(powerSliderFP.setup("firebrick x pink:", ppowerSliderFP, -100, 0));
	firebrickGroup.add(powerSliderFM.setup("firebrick x magenta:", ppowerSliderFM, -100, 0));
	firebrickGroup.add(powerSliderFA.setup("firebrick x aqua:", ppowerSliderFA, -100, 0));
	firebrickGroup.add(powerSliderFT.setup("firebrick x teal:", ppowerSliderFT, -100, 0));
	firebrickGroup.add(powerSliderFS.setup("firebrick x silver:", ppowerSliderFS, -100, 0));
	firebrickGroup.add(powerSliderFF.setup("firebrick x firebrick:", ppowerSliderFF, -100, 0));
	firebrickGroup.add(vSliderFG.setup("radius f x g:", pvSliderFG, 0, 500));
	firebrickGroup.add(vSliderFR.setup("radius f x r:", pvSliderFR, 0, 500));
	firebrickGroup.add(vSliderFW.setup("radius f x w:", pvSliderFW, 0, 500));
	firebrickGroup.add(vSliderFB.setup("radius f x b:", pvSliderFB, 0, 500));
	firebrickGroup.add(vSliderFO.setup("radius f x o:", pvSliderFO, 0, 500));
	firebrickGroup.add(vSliderFK.setup("radius f x k:", pvSliderFK, 0, 500));
	firebrickGroup.add(vSliderFC.setup("radius f x c:", pvSliderFC, 0, 500));
	firebrickGroup.add(vSliderFD.setup("radius f x d:", pvSliderFD, 0, 500));
	firebrickGroup.add(vSliderFY.setup("radius f x y:", pvSliderFY, 0, 500));
	firebrickGroup.add(vSliderFP.setup("radius f x p:", pvSliderFP, 0, 500));
	firebrickGroup.add(vSliderFM.setup("radius f x m:", pvSliderFM, 0, 500));
	firebrickGroup.add(vSliderFA.setup("radius f x a:", pvSliderFA, 0, 500));
	firebrickGroup.add(vSliderFT.setup("radius f x t:", pvSliderFT, 0, 500));
	firebrickGroup.add(vSliderFS.setup("radius f x s:", pvSliderFS, 0, 500));
	firebrickGroup.add(vSliderFF.setup("radius f x f:", pvSliderFF, 0, 500));
	firebrickGroup.add(viscositySliderFR.setup("Viscosity FR", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFG.setup("Viscosity FG", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFB.setup("Viscosity FB", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFW.setup("Viscosity FW", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFO.setup("Viscosity FO", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFK.setup("Viscosity FK", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFC.setup("Viscosity FC", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFD.setup("Viscosity FD", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFY.setup("Viscosity FY", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFP.setup("Viscosity FP", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFM.setup("Viscosity FM", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFA.setup("Viscosity FA", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFT.setup("Viscosity FT", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFS.setup("Viscosity FS", viscosity, 0, 1));
	firebrickGroup.add(viscositySliderFF.setup("Viscosity FF", viscosity, 0, 1));
	firebrickGroup.add(probabilitySliderFR.setup("Interaction Probability FR", probabilityFR, 0, 100));
	firebrickGroup.add(probabilitySliderFG.setup("Interaction Probability FG", probabilityFG, 0, 100));
	firebrickGroup.add(probabilitySliderFB.setup("Interaction Probability FB", probabilityFB, 0, 100));
	firebrickGroup.add(probabilitySliderFW.setup("Interaction Probability FW", probabilityFW, 0, 100));
	firebrickGroup.add(probabilitySliderFO.setup("Interaction Probability FO", probabilityFO, 0, 100));
	firebrickGroup.add(probabilitySliderFK.setup("Interaction Probability FK", probabilityFK, 0, 100));
	firebrickGroup.add(probabilitySliderFC.setup("Interaction Probability FC", probabilityFC, 0, 100));
	firebrickGroup.add(probabilitySliderFD.setup("Interaction Probability FD", probabilityFD, 0, 100));
	firebrickGroup.add(probabilitySliderFY.setup("Interaction Probability FY", probabilityFY, 0, 100));
	firebrickGroup.add(probabilitySliderFP.setup("Interaction Probability FP", probabilityFP, 0, 100));
	firebrickGroup.add(probabilitySliderFM.setup("Interaction Probability FM", probabilityFM, 0, 100));
	firebrickGroup.add(probabilitySliderFA.setup("Interaction Probability FA", probabilityFA, 0, 100));
	firebrickGroup.add(probabilitySliderFT.setup("Interaction Probability FT", probabilityFT, 0, 100));
	firebrickGroup.add(probabilitySliderFS.setup("Interaction Probability FS", probabilityFS, 0, 100));
	firebrickGroup.add(probabilitySliderFF.setup("Interaction Probability FF", probabilityFF, 0, 100));
	firebrickGroup.minimize();
	gui.add(&firebrickGroup);

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
	probabilityRR = probabilitySliderRR;
	probabilityRG = probabilitySliderRG;
	probabilityRB = probabilitySliderRB;
	probabilityRW = probabilitySliderRW;
	probabilityRO = probabilitySliderRO;
	probabilityRK = probabilitySliderRK;
	probabilityRC = probabilitySliderRC;
	probabilityRD = probabilitySliderRD;
	probabilityRY = probabilitySliderRY;
	probabilityRP = probabilitySliderRP;
	probabilityRM = probabilitySliderRM;
	probabilityRA = probabilitySliderRA;
	probabilityRT = probabilitySliderRT;
	probabilityRS = probabilitySliderRS;
	probabilityRF = probabilitySliderRF;
	probabilityGR = probabilitySliderGR;
	probabilityGG = probabilitySliderGG;
	probabilityGB = probabilitySliderGB;
	probabilityGW = probabilitySliderGW;
	probabilityGO = probabilitySliderGO;
	probabilityGK = probabilitySliderGK;
	probabilityGC = probabilitySliderGC;
	probabilityGD = probabilitySliderGD;
	probabilityGY = probabilitySliderGY;
	probabilityGP = probabilitySliderGP;
	probabilityGM = probabilitySliderGM;
	probabilityGA = probabilitySliderGA;
	probabilityGT = probabilitySliderGT;
	probabilityGS = probabilitySliderGS;
	probabilityGF = probabilitySliderGF;
	probabilityBR = probabilitySliderBR;
	probabilityBG = probabilitySliderBG;
	probabilityBB = probabilitySliderBB;
	probabilityBW = probabilitySliderBW;
	probabilityBO = probabilitySliderBO;
	probabilityBK = probabilitySliderBK;
	probabilityBC = probabilitySliderBC;
	probabilityBD = probabilitySliderBD;
	probabilityBY = probabilitySliderBY;
	probabilityBP = probabilitySliderBP;
	probabilityBM = probabilitySliderBM;
	probabilityBA = probabilitySliderBA;
	probabilityBT = probabilitySliderBT;
	probabilityBS = probabilitySliderBS;
	probabilityBF = probabilitySliderBF;
	probabilityWR = probabilitySliderWR;
	probabilityWG = probabilitySliderWG;
	probabilityWB = probabilitySliderWB;
	probabilityWW = probabilitySliderWW;
	probabilityWO = probabilitySliderWO;
	probabilityWK = probabilitySliderWK;
	probabilityWC = probabilitySliderWC;
	probabilityWD = probabilitySliderWD;
	probabilityWY = probabilitySliderWY;
	probabilityWP = probabilitySliderWP;
	probabilityWM = probabilitySliderWM;
	probabilityWA = probabilitySliderWA;
	probabilityWT = probabilitySliderWT;
	probabilityWS = probabilitySliderWS;
	probabilityWF = probabilitySliderWF;
	probabilityOR = probabilitySliderOR;
	probabilityOG = probabilitySliderOG;
	probabilityOB = probabilitySliderOB;
	probabilityOW = probabilitySliderOW;
	probabilityOO = probabilitySliderOO;
	probabilityOK = probabilitySliderOK;
	probabilityOC = probabilitySliderOC;
	probabilityOD = probabilitySliderOD;
	probabilityOY = probabilitySliderOY;
	probabilityOP = probabilitySliderOP;
	probabilityOM = probabilitySliderOM;
	probabilityOA = probabilitySliderOA;
	probabilityOT = probabilitySliderOT;
	probabilityOS = probabilitySliderOS;
	probabilityOF = probabilitySliderOF;
	probabilityKR = probabilitySliderKR;
	probabilityKG = probabilitySliderKG;
	probabilityKB = probabilitySliderKB;
	probabilityKW = probabilitySliderKW;
	probabilityKO = probabilitySliderKO;
	probabilityKK = probabilitySliderKK;
	probabilityKC = probabilitySliderKC;
	probabilityKD = probabilitySliderKD;
	probabilityKY = probabilitySliderKY;
	probabilityKP = probabilitySliderKP;
	probabilityKM = probabilitySliderKM;
	probabilityKA = probabilitySliderKA;
	probabilityKT = probabilitySliderKT;
	probabilityKS = probabilitySliderKS;
	probabilityKF = probabilitySliderKF;
	probabilityCR = probabilitySliderCR;
	probabilityCG = probabilitySliderCG;
	probabilityCB = probabilitySliderCB;
	probabilityCW = probabilitySliderCW;
	probabilityCO = probabilitySliderCO;
	probabilityCK = probabilitySliderCK;
	probabilityCC = probabilitySliderCC;
	probabilityCD = probabilitySliderCD;
	probabilityCY = probabilitySliderCY;
	probabilityCP = probabilitySliderCP;
	probabilityCM = probabilitySliderCM;
	probabilityCA = probabilitySliderCA;
	probabilityCT = probabilitySliderCT;
	probabilityCS = probabilitySliderCS;
	probabilityCF = probabilitySliderCF;
	probabilityDR = probabilitySliderDR;
	probabilityDG = probabilitySliderDG;
	probabilityDB = probabilitySliderDB;
	probabilityDW = probabilitySliderDW;
	probabilityDO = probabilitySliderDO;
	probabilityDK = probabilitySliderDK;
	probabilityDC = probabilitySliderDC;
	probabilityDD = probabilitySliderDD;
	probabilityDY = probabilitySliderDY;
	probabilityDP = probabilitySliderDP;
	probabilityDM = probabilitySliderDM;
	probabilityDA = probabilitySliderDA;
	probabilityDT = probabilitySliderDT;
	probabilityDS = probabilitySliderDS;
	probabilityDF = probabilitySliderDF;
	probabilityYR = probabilitySliderYR;
	probabilityYG = probabilitySliderYG;
	probabilityYB = probabilitySliderYB;
	probabilityYW = probabilitySliderYW;
	probabilityYO = probabilitySliderYO;
	probabilityYK = probabilitySliderYK;
	probabilityYC = probabilitySliderYC;
	probabilityYD = probabilitySliderYD;
	probabilityYY = probabilitySliderYY;
	probabilityYP = probabilitySliderYP;
	probabilityYM = probabilitySliderYM;
	probabilityYA = probabilitySliderYA;
	probabilityYT = probabilitySliderYT;
	probabilityYS = probabilitySliderYS;
	probabilityYF = probabilitySliderYF;
	probabilityPR = probabilitySliderPR;
	probabilityPG = probabilitySliderPG;
	probabilityPB = probabilitySliderPB;
	probabilityPW = probabilitySliderPW;
	probabilityPO = probabilitySliderPO;
	probabilityPK = probabilitySliderPK;
	probabilityPC = probabilitySliderPC;
	probabilityPD = probabilitySliderPD;
	probabilityPY = probabilitySliderPY;
	probabilityPP = probabilitySliderPP;
	probabilityPM = probabilitySliderPM;
	probabilityPA = probabilitySliderPA;
	probabilityPT = probabilitySliderPT;
	probabilityPS = probabilitySliderPS;
	probabilityPF = probabilitySliderPF;
	probabilityMR = probabilitySliderMR;
	probabilityMG = probabilitySliderMG;
	probabilityMB = probabilitySliderMB;
	probabilityMW = probabilitySliderMW;
	probabilityMO = probabilitySliderMO;
	probabilityMK = probabilitySliderMK;
	probabilityMC = probabilitySliderMC;
	probabilityMD = probabilitySliderMD;
	probabilityMY = probabilitySliderMY;
	probabilityMP = probabilitySliderMP;
	probabilityMM = probabilitySliderMM;
	probabilityMA = probabilitySliderMA;
	probabilityMT = probabilitySliderMT;
	probabilityMS = probabilitySliderMS;
	probabilityMF = probabilitySliderMF;
	probabilityAR = probabilitySliderAR;
	probabilityAG = probabilitySliderAG;
	probabilityAB = probabilitySliderAB;
	probabilityAW = probabilitySliderAW;
	probabilityAO = probabilitySliderAO;
	probabilityAK = probabilitySliderAK;
	probabilityAC = probabilitySliderAC;
	probabilityAD = probabilitySliderAD;
	probabilityAY = probabilitySliderAY;
	probabilityAP = probabilitySliderAP;
	probabilityAM = probabilitySliderAM;
	probabilityAA = probabilitySliderAA;
	probabilityAT = probabilitySliderAT;
	probabilityAS = probabilitySliderAS;
	probabilityAF = probabilitySliderAF;
	probabilityTR = probabilitySliderTR;
	probabilityTG = probabilitySliderTG;
	probabilityTB = probabilitySliderTB;
	probabilityTW = probabilitySliderTW;
	probabilityTO = probabilitySliderTO;
	probabilityTK = probabilitySliderTK;
	probabilityTC = probabilitySliderTC;
	probabilityTD = probabilitySliderTD;
	probabilityTY = probabilitySliderTY;
	probabilityTP = probabilitySliderTP;
	probabilityTM = probabilitySliderTM;
	probabilityTA = probabilitySliderTA;
	probabilityTT = probabilitySliderTT;
	probabilityTS = probabilitySliderTS;
	probabilityTF = probabilitySliderTF;
	probabilitySR = probabilitySliderSR;
	probabilitySG = probabilitySliderSG;
	probabilitySB = probabilitySliderSB;
	probabilitySW = probabilitySliderSW;
	probabilitySO = probabilitySliderSO;
	probabilitySK = probabilitySliderSK;
	probabilitySC = probabilitySliderSC;
	probabilitySD = probabilitySliderSD;
	probabilitySY = probabilitySliderSY;
	probabilitySP = probabilitySliderSP;
	probabilitySM = probabilitySliderSM;
	probabilitySA = probabilitySliderSA;
	probabilityST = probabilitySliderST;
	probabilitySS = probabilitySliderSS;
	probabilitySF = probabilitySliderSF;
	probabilityFR = probabilitySliderFR;
	probabilityFG = probabilitySliderFG;
	probabilityFB = probabilitySliderFB;
	probabilityFW = probabilitySliderFW;
	probabilityFO = probabilitySliderFO;
	probabilityFK = probabilitySliderFK;
	probabilityFC = probabilitySliderFC;
	probabilityFD = probabilitySliderFD;
	probabilityFY = probabilitySliderFY;
	probabilityFP = probabilitySliderFP;
	probabilityFM = probabilitySliderFM;
	probabilityFA = probabilitySliderFA;
	probabilityFT = probabilitySliderFT;
	probabilityFS = probabilitySliderFS;
	probabilityFF = probabilitySliderFF;
	viscosity = viscositySlider;
	viscosityRR = viscositySliderRR;
	viscosityRG = viscositySliderRG;
	viscosityRW = viscositySliderRW;
	viscosityRB = viscositySliderRB;
	viscosityRO = viscositySliderRO;
	viscosityRK = viscositySliderRK;
	viscosityRC = viscositySliderRC;
	viscosityRD = viscositySliderRD;
	viscosityRY = viscositySliderRY;
	viscosityRP = viscositySliderRP;
	viscosityRM = viscositySliderRM;
	viscosityRA = viscositySliderRA;
	viscosityRT = viscositySliderRT;
	viscosityRS = viscositySliderRS;
	viscosityRF = viscositySliderRF;
	viscosityGR = viscositySliderGR;
	viscosityGG = viscositySliderGG;
	viscosityGW = viscositySliderGW;
	viscosityGB = viscositySliderGB;
	viscosityGO = viscositySliderGO;
	viscosityGK = viscositySliderGK;
	viscosityGC = viscositySliderGC;
	viscosityGD = viscositySliderGD;
	viscosityGY = viscositySliderGY;
	viscosityGP = viscositySliderGP;
	viscosityGM = viscositySliderGM;
	viscosityGA = viscositySliderGA;
	viscosityGT = viscositySliderGT;
	viscosityGS = viscositySliderGS;
	viscosityGF = viscositySliderGF;
	viscosityBR = viscositySliderBR;
	viscosityBG = viscositySliderBG;
	viscosityBW = viscositySliderBW;
	viscosityBB = viscositySliderBB;
	viscosityBO = viscositySliderBO;
	viscosityBK = viscositySliderBK;
	viscosityBC = viscositySliderBC;
	viscosityBD = viscositySliderBD;
	viscosityBY = viscositySliderBY;
	viscosityBP = viscositySliderBP;
	viscosityBM = viscositySliderBM;
	viscosityBA = viscositySliderBA;
	viscosityBT = viscositySliderBT;
	viscosityBS = viscositySliderBS;
	viscosityBF = viscositySliderBF;
	viscosityWR = viscositySliderWR;
	viscosityWG = viscositySliderWG;
	viscosityWW = viscositySliderWW;
	viscosityWB = viscositySliderWB;
	viscosityWO = viscositySliderWO;
	viscosityWK = viscositySliderWK;
	viscosityWC = viscositySliderWC;
	viscosityWD = viscositySliderWD;
	viscosityWY = viscositySliderWY;
	viscosityWP = viscositySliderWP;
	viscosityWM = viscositySliderWM;
	viscosityWA = viscositySliderWA;
	viscosityWT = viscositySliderWT;
	viscosityWS = viscositySliderWS;
	viscosityWF = viscositySliderWF;
	viscosityOR = viscositySliderOR;
	viscosityOG = viscositySliderOG;
	viscosityOW = viscositySliderOW;
	viscosityOB = viscositySliderOB;
	viscosityOO = viscositySliderOO;
	viscosityOK = viscositySliderOK;
	viscosityOC = viscositySliderOC;
	viscosityOD = viscositySliderOD;
	viscosityOY = viscositySliderOY;
	viscosityOP = viscositySliderOP;
	viscosityOM = viscositySliderOM;
	viscosityOA = viscositySliderOA;
	viscosityOT = viscositySliderOT;
	viscosityOS = viscositySliderOS;
	viscosityOF = viscositySliderOF;
	viscosityKR = viscositySliderKR;
	viscosityKG = viscositySliderKG;
	viscosityKW = viscositySliderKW;
	viscosityKB = viscositySliderKB;
	viscosityKO = viscositySliderKO;
	viscosityKK = viscositySliderKK;
	viscosityKC = viscositySliderKC;
	viscosityKD = viscositySliderKD;
	viscosityKY = viscositySliderKY;
	viscosityKP = viscositySliderKP;
	viscosityKM = viscositySliderKM;
	viscosityKA = viscositySliderKA;
	viscosityKT = viscositySliderKT;
	viscosityKS = viscositySliderKS;
	viscosityKF = viscositySliderKF;
	viscosityCR = viscositySliderCR;
	viscosityCG = viscositySliderCG;
	viscosityCW = viscositySliderCW;
	viscosityCB = viscositySliderCB;
	viscosityCO = viscositySliderCO;
	viscosityCK = viscositySliderCK;
	viscosityCC = viscositySliderCC;
	viscosityCD = viscositySliderCD;
	viscosityCY = viscositySliderCY;
	viscosityCP = viscositySliderCP;
	viscosityCM = viscositySliderCM;
	viscosityCA = viscositySliderCA;
	viscosityCT = viscositySliderCT;
	viscosityCS = viscositySliderCS;
	viscosityCF = viscositySliderCF;
	viscosityDR = viscositySliderDR;
	viscosityDG = viscositySliderDG;
	viscosityDW = viscositySliderDW;
	viscosityDB = viscositySliderDB;
	viscosityDO = viscositySliderDO;
	viscosityDK = viscositySliderDK;
	viscosityDC = viscositySliderDC;
	viscosityDD = viscositySliderDD;
	viscosityDY = viscositySliderDY;
	viscosityDP = viscositySliderDP;
	viscosityDM = viscositySliderDM;
	viscosityDA = viscositySliderDA;
	viscosityDT = viscositySliderDT;
	viscosityDS = viscositySliderDS;
	viscosityDF = viscositySliderDF;
	viscosityYR = viscositySliderYR;
	viscosityYG = viscositySliderYG;
	viscosityYW = viscositySliderYW;
	viscosityYB = viscositySliderYB;
	viscosityYO = viscositySliderYO;
	viscosityYK = viscositySliderYK;
	viscosityYC = viscositySliderYC;
	viscosityYD = viscositySliderYD;
	viscosityYY = viscositySliderYY;
	viscosityYP = viscositySliderYP;
	viscosityYM = viscositySliderYM;
	viscosityYA = viscositySliderYA;
	viscosityYT = viscositySliderYT;
	viscosityYS = viscositySliderYS;
	viscosityYF = viscositySliderYF;
	viscosityPR = viscositySliderPR;
	viscosityPG = viscositySliderPG;
	viscosityPW = viscositySliderPW;
	viscosityPB = viscositySliderPB;
	viscosityPO = viscositySliderPO;
	viscosityPK = viscositySliderPK;
	viscosityPC = viscositySliderPC;
	viscosityPD = viscositySliderPD;
	viscosityPY = viscositySliderPY;
	viscosityPP = viscositySliderPP;
	viscosityPM = viscositySliderPM;
	viscosityPA = viscositySliderPA;
	viscosityPT = viscositySliderPT;
	viscosityPS = viscositySliderPS;
	viscosityPF = viscositySliderPF;
	viscosityMR = viscositySliderMR;
	viscosityMG = viscositySliderMG;
	viscosityMW = viscositySliderMW;
	viscosityMB = viscositySliderMB;
	viscosityMO = viscositySliderMO;
	viscosityMK = viscositySliderMK;
	viscosityMC = viscositySliderMC;
	viscosityMD = viscositySliderMD;
	viscosityMY = viscositySliderMY;
	viscosityMP = viscositySliderMP;
	viscosityMM = viscositySliderMM;
	viscosityMA = viscositySliderMA;
	viscosityMT = viscositySliderMT;
	viscosityMS = viscositySliderMS;
	viscosityMF = viscositySliderMF;
	viscosityAR = viscositySliderAR;
	viscosityAG = viscositySliderAG;
	viscosityAW = viscositySliderAW;
	viscosityAB = viscositySliderAB;
	viscosityAO = viscositySliderAO;
	viscosityAK = viscositySliderAK;
	viscosityAC = viscositySliderAC;
	viscosityAD = viscositySliderAD;
	viscosityAY = viscositySliderAY;
	viscosityAP = viscositySliderAP;
	viscosityAM = viscositySliderAM;
	viscosityAA = viscositySliderAA;
	viscosityAT = viscositySliderAT;
	viscosityAS = viscositySliderAS;
	viscosityAF = viscositySliderAF;
	viscosityTR = viscositySliderTR;
	viscosityTG = viscositySliderTG;
	viscosityTW = viscositySliderTW;
	viscosityTB = viscositySliderTB;
	viscosityTO = viscositySliderTO;
	viscosityTK = viscositySliderTK;
	viscosityTC = viscositySliderTC;
	viscosityTD = viscositySliderTD;
	viscosityTY = viscositySliderTY;
	viscosityTP = viscositySliderTP;
	viscosityTM = viscositySliderTM;
	viscosityTA = viscositySliderTA;
	viscosityTT = viscositySliderTT;
	viscosityTS = viscositySliderTS;
	viscosityTF = viscositySliderTF;
	viscositySR = viscositySliderSR;
	viscositySG = viscositySliderSG;
	viscositySW = viscositySliderSW;
	viscositySB = viscositySliderSB;
	viscositySO = viscositySliderSO;
	viscositySK = viscositySliderSK;
	viscositySC = viscositySliderSC;
	viscositySD = viscositySliderSD;
	viscositySY = viscositySliderSY;
	viscositySP = viscositySliderSP;
	viscositySM = viscositySliderSM;
	viscositySA = viscositySliderSA;
	viscosityST = viscositySliderST;
	viscositySS = viscositySliderSS;
	viscositySF = viscositySliderSF;
	viscosityFR = viscositySliderFR;
	viscosityFG = viscositySliderFG;
	viscosityFW = viscositySliderFW;
	viscosityFB = viscositySliderFB;
	viscosityFO = viscositySliderFO;
	viscosityFK = viscositySliderFK;
	viscosityFC = viscositySliderFC;
	viscosityFD = viscositySliderFD;
	viscosityFY = viscositySliderFY;
	viscosityFP = viscositySliderFP;
	viscosityFM = viscositySliderFM;
	viscosityFA = viscositySliderFA;
	viscosityFT = viscositySliderFT;
	viscosityFS = viscositySliderFS;
	viscosityFF = viscositySliderFF;

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
		powerSliderRY = powerSliderRY + ((ofRandom(2.0F) - 1.0F) * (powerSliderRY.getMax() - powerSliderRY.getMin()) * (evoAmount / 100.0F));
		powerSliderRP = powerSliderRP + ((ofRandom(2.0F) - 1.0F) * (powerSliderRP.getMax() - powerSliderRP.getMin()) * (evoAmount / 100.0F));
		powerSliderRM = powerSliderRM + ((ofRandom(2.0F) - 1.0F) * (powerSliderRM.getMax() - powerSliderRM.getMin()) * (evoAmount / 100.0F));
		powerSliderRA = powerSliderRA + ((ofRandom(2.0F) - 1.0F) * (powerSliderRA.getMax() - powerSliderRA.getMin()) * (evoAmount / 100.0F));
		powerSliderRT = powerSliderRT + ((ofRandom(2.0F) - 1.0F) * (powerSliderRT.getMax() - powerSliderRT.getMin()) * (evoAmount / 100.0F));
		powerSliderRS = powerSliderRS + ((ofRandom(2.0F) - 1.0F) * (powerSliderRS.getMax() - powerSliderRS.getMin()) * (evoAmount / 100.0F));
		powerSliderRF = powerSliderRF + ((ofRandom(2.0F) - 1.0F) * (powerSliderRF.getMax() - powerSliderRF.getMin()) * (evoAmount / 100.0F));
		powerSliderGR = powerSliderGR + ((ofRandom(2.0F) - 1.0F) * (powerSliderGR.getMax() - powerSliderGR.getMin()) * (evoAmount / 100.0F));
		powerSliderGG = powerSliderGG + ((ofRandom(2.0F) - 1.0F) * (powerSliderGG.getMax() - powerSliderGG.getMin()) * (evoAmount / 100.0F));
		powerSliderGB = powerSliderGB + ((ofRandom(2.0F) - 1.0F) * (powerSliderGB.getMax() - powerSliderGB.getMin()) * (evoAmount / 100.0F));
		powerSliderGW = powerSliderGW + ((ofRandom(2.0F) - 1.0F) * (powerSliderGW.getMax() - powerSliderGW.getMin()) * (evoAmount / 100.0F));
		powerSliderGO = powerSliderGO + ((ofRandom(2.0F) - 1.0F) * (powerSliderGO.getMax() - powerSliderGO.getMin()) * (evoAmount / 100.0F));
		powerSliderGK = powerSliderGK + ((ofRandom(2.0F) - 1.0F) * (powerSliderGK.getMax() - powerSliderGK.getMin()) * (evoAmount / 100.0F));
		powerSliderGC = powerSliderGC + ((ofRandom(2.0F) - 1.0F) * (powerSliderGC.getMax() - powerSliderGC.getMin()) * (evoAmount / 100.0F));
		powerSliderGD = powerSliderGD + ((ofRandom(2.0F) - 1.0F) * (powerSliderGD.getMax() - powerSliderGD.getMin()) * (evoAmount / 100.0F));
		powerSliderGY = powerSliderGY + ((ofRandom(2.0F) - 1.0F) * (powerSliderGY.getMax() - powerSliderGY.getMin()) * (evoAmount / 100.0F));
		powerSliderGP = powerSliderGP + ((ofRandom(2.0F) - 1.0F) * (powerSliderGP.getMax() - powerSliderGP.getMin()) * (evoAmount / 100.0F));
		powerSliderGM = powerSliderGM + ((ofRandom(2.0F) - 1.0F) * (powerSliderGM.getMax() - powerSliderGM.getMin()) * (evoAmount / 100.0F));
		powerSliderGA = powerSliderGA + ((ofRandom(2.0F) - 1.0F) * (powerSliderGA.getMax() - powerSliderGA.getMin()) * (evoAmount / 100.0F));
		powerSliderGT = powerSliderGT + ((ofRandom(2.0F) - 1.0F) * (powerSliderGT.getMax() - powerSliderGT.getMin()) * (evoAmount / 100.0F));
		powerSliderGS = powerSliderGS + ((ofRandom(2.0F) - 1.0F) * (powerSliderGS.getMax() - powerSliderGS.getMin()) * (evoAmount / 100.0F));
		powerSliderGF = powerSliderGF + ((ofRandom(2.0F) - 1.0F) * (powerSliderGF.getMax() - powerSliderGF.getMin()) * (evoAmount / 100.0F));
		powerSliderBR = powerSliderBR + ((ofRandom(2.0F) - 1.0F) * (powerSliderBR.getMax() - powerSliderBR.getMin()) * (evoAmount / 100.0F));
		powerSliderBG = powerSliderBG + ((ofRandom(2.0F) - 1.0F) * (powerSliderBG.getMax() - powerSliderBG.getMin()) * (evoAmount / 100.0F));
		powerSliderBB = powerSliderBB + ((ofRandom(2.0F) - 1.0F) * (powerSliderBB.getMax() - powerSliderBB.getMin()) * (evoAmount / 100.0F));
		powerSliderBW = powerSliderBW + ((ofRandom(2.0F) - 1.0F) * (powerSliderBW.getMax() - powerSliderBW.getMin()) * (evoAmount / 100.0F));
		powerSliderBO = powerSliderBO + ((ofRandom(2.0F) - 1.0F) * (powerSliderBO.getMax() - powerSliderBO.getMin()) * (evoAmount / 100.0F));
		powerSliderBK = powerSliderBK + ((ofRandom(2.0F) - 1.0F) * (powerSliderBK.getMax() - powerSliderBK.getMin()) * (evoAmount / 100.0F));
		powerSliderBC = powerSliderBC + ((ofRandom(2.0F) - 1.0F) * (powerSliderBC.getMax() - powerSliderBC.getMin()) * (evoAmount / 100.0F));
		powerSliderBD = powerSliderBD + ((ofRandom(2.0F) - 1.0F) * (powerSliderBD.getMax() - powerSliderBD.getMin()) * (evoAmount / 100.0F));
		powerSliderBY = powerSliderBY + ((ofRandom(2.0F) - 1.0F) * (powerSliderBY.getMax() - powerSliderBY.getMin()) * (evoAmount / 100.0F));
		powerSliderBP = powerSliderBP + ((ofRandom(2.0F) - 1.0F) * (powerSliderBP.getMax() - powerSliderBP.getMin()) * (evoAmount / 100.0F));
		powerSliderBM = powerSliderBM + ((ofRandom(2.0F) - 1.0F) * (powerSliderBM.getMax() - powerSliderBM.getMin()) * (evoAmount / 100.0F));
		powerSliderBA = powerSliderBA + ((ofRandom(2.0F) - 1.0F) * (powerSliderBA.getMax() - powerSliderBA.getMin()) * (evoAmount / 100.0F));
		powerSliderBT = powerSliderBT + ((ofRandom(2.0F) - 1.0F) * (powerSliderBT.getMax() - powerSliderBT.getMin()) * (evoAmount / 100.0F));
		powerSliderBS = powerSliderBS + ((ofRandom(2.0F) - 1.0F) * (powerSliderBS.getMax() - powerSliderBS.getMin()) * (evoAmount / 100.0F));
		powerSliderBF = powerSliderBF + ((ofRandom(2.0F) - 1.0F) * (powerSliderBF.getMax() - powerSliderBF.getMin()) * (evoAmount / 100.0F));
		powerSliderWR = powerSliderWR + ((ofRandom(2.0F) - 1.0F) * (powerSliderWR.getMax() - powerSliderWR.getMin()) * (evoAmount / 100.0F));
		powerSliderWG = powerSliderWG + ((ofRandom(2.0F) - 1.0F) * (powerSliderWG.getMax() - powerSliderWG.getMin()) * (evoAmount / 100.0F));
		powerSliderWB = powerSliderWB + ((ofRandom(2.0F) - 1.0F) * (powerSliderWB.getMax() - powerSliderWB.getMin()) * (evoAmount / 100.0F));
		powerSliderWW = powerSliderWW + ((ofRandom(2.0F) - 1.0F) * (powerSliderWW.getMax() - powerSliderWW.getMin()) * (evoAmount / 100.0F));
		powerSliderWO = powerSliderWO + ((ofRandom(2.0F) - 1.0F) * (powerSliderWO.getMax() - powerSliderWO.getMin()) * (evoAmount / 100.0F));
		powerSliderWK = powerSliderWK + ((ofRandom(2.0F) - 1.0F) * (powerSliderWK.getMax() - powerSliderWK.getMin()) * (evoAmount / 100.0F));
		powerSliderWC = powerSliderWC + ((ofRandom(2.0F) - 1.0F) * (powerSliderWC.getMax() - powerSliderWC.getMin()) * (evoAmount / 100.0F));
		powerSliderWD = powerSliderWD + ((ofRandom(2.0F) - 1.0F) * (powerSliderWD.getMax() - powerSliderWD.getMin()) * (evoAmount / 100.0F));
		powerSliderWY = powerSliderWY + ((ofRandom(2.0F) - 1.0F) * (powerSliderWY.getMax() - powerSliderWY.getMin()) * (evoAmount / 100.0F));
		powerSliderWP = powerSliderWP + ((ofRandom(2.0F) - 1.0F) * (powerSliderWP.getMax() - powerSliderWP.getMin()) * (evoAmount / 100.0F));
		powerSliderWM = powerSliderWM + ((ofRandom(2.0F) - 1.0F) * (powerSliderWM.getMax() - powerSliderWM.getMin()) * (evoAmount / 100.0F));
		powerSliderWA = powerSliderWA + ((ofRandom(2.0F) - 1.0F) * (powerSliderWA.getMax() - powerSliderWA.getMin()) * (evoAmount / 100.0F));
		powerSliderWT = powerSliderWT + ((ofRandom(2.0F) - 1.0F) * (powerSliderWT.getMax() - powerSliderWT.getMin()) * (evoAmount / 100.0F));
		powerSliderWS = powerSliderWS + ((ofRandom(2.0F) - 1.0F) * (powerSliderWS.getMax() - powerSliderWS.getMin()) * (evoAmount / 100.0F));
		powerSliderWF = powerSliderWF + ((ofRandom(2.0F) - 1.0F) * (powerSliderWF.getMax() - powerSliderWF.getMin()) * (evoAmount / 100.0F));
		powerSliderOR = powerSliderOR + ((ofRandom(2.0F) - 1.0F) * (powerSliderOR.getMax() - powerSliderOR.getMin()) * (evoAmount / 100.0F));
		powerSliderOG = powerSliderOG + ((ofRandom(2.0F) - 1.0F) * (powerSliderOG.getMax() - powerSliderOG.getMin()) * (evoAmount / 100.0F));
		powerSliderOB = powerSliderOB + ((ofRandom(2.0F) - 1.0F) * (powerSliderOB.getMax() - powerSliderOB.getMin()) * (evoAmount / 100.0F));
		powerSliderOW = powerSliderOW + ((ofRandom(2.0F) - 1.0F) * (powerSliderOW.getMax() - powerSliderOW.getMin()) * (evoAmount / 100.0F));
		powerSliderOO = powerSliderOO + ((ofRandom(2.0F) - 1.0F) * (powerSliderOO.getMax() - powerSliderOO.getMin()) * (evoAmount / 100.0F));
		powerSliderOK = powerSliderOK + ((ofRandom(2.0F) - 1.0F) * (powerSliderOK.getMax() - powerSliderOK.getMin()) * (evoAmount / 100.0F));
		powerSliderOC = powerSliderOC + ((ofRandom(2.0F) - 1.0F) * (powerSliderOC.getMax() - powerSliderOC.getMin()) * (evoAmount / 100.0F));
		powerSliderOD = powerSliderOD + ((ofRandom(2.0F) - 1.0F) * (powerSliderOD.getMax() - powerSliderOD.getMin()) * (evoAmount / 100.0F));
		powerSliderOY = powerSliderOY + ((ofRandom(2.0F) - 1.0F) * (powerSliderOY.getMax() - powerSliderOY.getMin()) * (evoAmount / 100.0F));
		powerSliderOP = powerSliderOP + ((ofRandom(2.0F) - 1.0F) * (powerSliderOP.getMax() - powerSliderOP.getMin()) * (evoAmount / 100.0F));
		powerSliderOM = powerSliderOM + ((ofRandom(2.0F) - 1.0F) * (powerSliderOM.getMax() - powerSliderOM.getMin()) * (evoAmount / 100.0F));
		powerSliderOA = powerSliderOA + ((ofRandom(2.0F) - 1.0F) * (powerSliderOA.getMax() - powerSliderOA.getMin()) * (evoAmount / 100.0F));
		powerSliderOT = powerSliderOT + ((ofRandom(2.0F) - 1.0F) * (powerSliderOT.getMax() - powerSliderOT.getMin()) * (evoAmount / 100.0F));
		powerSliderOS = powerSliderOS + ((ofRandom(2.0F) - 1.0F) * (powerSliderOS.getMax() - powerSliderOS.getMin()) * (evoAmount / 100.0F));
		powerSliderOF = powerSliderOF + ((ofRandom(2.0F) - 1.0F) * (powerSliderOF.getMax() - powerSliderOF.getMin()) * (evoAmount / 100.0F));
		powerSliderKR = powerSliderKR + ((ofRandom(2.0F) - 1.0F) * (powerSliderKR.getMax() - powerSliderKR.getMin()) * (evoAmount / 100.0F));
		powerSliderKG = powerSliderKG + ((ofRandom(2.0F) - 1.0F) * (powerSliderKG.getMax() - powerSliderKG.getMin()) * (evoAmount / 100.0F));
		powerSliderKB = powerSliderKB + ((ofRandom(2.0F) - 1.0F) * (powerSliderKB.getMax() - powerSliderKB.getMin()) * (evoAmount / 100.0F));
		powerSliderKW = powerSliderKW + ((ofRandom(2.0F) - 1.0F) * (powerSliderKW.getMax() - powerSliderKW.getMin()) * (evoAmount / 100.0F));
		powerSliderKO = powerSliderKO + ((ofRandom(2.0F) - 1.0F) * (powerSliderKO.getMax() - powerSliderKO.getMin()) * (evoAmount / 100.0F));
		powerSliderKK = powerSliderKK + ((ofRandom(2.0F) - 1.0F) * (powerSliderKK.getMax() - powerSliderKK.getMin()) * (evoAmount / 100.0F));
		powerSliderKC = powerSliderKC + ((ofRandom(2.0F) - 1.0F) * (powerSliderKC.getMax() - powerSliderKC.getMin()) * (evoAmount / 100.0F));
		powerSliderKD = powerSliderKD + ((ofRandom(2.0F) - 1.0F) * (powerSliderKD.getMax() - powerSliderKD.getMin()) * (evoAmount / 100.0F));
		powerSliderKY = powerSliderKY + ((ofRandom(2.0F) - 1.0F) * (powerSliderKY.getMax() - powerSliderKY.getMin()) * (evoAmount / 100.0F));
		powerSliderKP = powerSliderKP + ((ofRandom(2.0F) - 1.0F) * (powerSliderKP.getMax() - powerSliderKP.getMin()) * (evoAmount / 100.0F));
		powerSliderKM = powerSliderKM + ((ofRandom(2.0F) - 1.0F) * (powerSliderKM.getMax() - powerSliderKM.getMin()) * (evoAmount / 100.0F));
		powerSliderKA = powerSliderKA + ((ofRandom(2.0F) - 1.0F) * (powerSliderKA.getMax() - powerSliderKA.getMin()) * (evoAmount / 100.0F));
		powerSliderKT = powerSliderKT + ((ofRandom(2.0F) - 1.0F) * (powerSliderKT.getMax() - powerSliderKT.getMin()) * (evoAmount / 100.0F));
		powerSliderKS = powerSliderKS + ((ofRandom(2.0F) - 1.0F) * (powerSliderKS.getMax() - powerSliderKS.getMin()) * (evoAmount / 100.0F));
		powerSliderKF = powerSliderKF + ((ofRandom(2.0F) - 1.0F) * (powerSliderKF.getMax() - powerSliderKF.getMin()) * (evoAmount / 100.0F));
		powerSliderCR = powerSliderCR + ((ofRandom(2.0F) - 1.0F) * (powerSliderCR.getMax() - powerSliderCR.getMin()) * (evoAmount / 100.0F));
		powerSliderCG = powerSliderCG + ((ofRandom(2.0F) - 1.0F) * (powerSliderCG.getMax() - powerSliderCG.getMin()) * (evoAmount / 100.0F));
		powerSliderCB = powerSliderCB + ((ofRandom(2.0F) - 1.0F) * (powerSliderCB.getMax() - powerSliderCB.getMin()) * (evoAmount / 100.0F));
		powerSliderCW = powerSliderCW + ((ofRandom(2.0F) - 1.0F) * (powerSliderCW.getMax() - powerSliderCW.getMin()) * (evoAmount / 100.0F));
		powerSliderCO = powerSliderCO + ((ofRandom(2.0F) - 1.0F) * (powerSliderCO.getMax() - powerSliderCO.getMin()) * (evoAmount / 100.0F));
		powerSliderCK = powerSliderCK + ((ofRandom(2.0F) - 1.0F) * (powerSliderCK.getMax() - powerSliderCK.getMin()) * (evoAmount / 100.0F));
		powerSliderCC = powerSliderCC + ((ofRandom(2.0F) - 1.0F) * (powerSliderCC.getMax() - powerSliderCC.getMin()) * (evoAmount / 100.0F));
		powerSliderCD = powerSliderCD + ((ofRandom(2.0F) - 1.0F) * (powerSliderCD.getMax() - powerSliderCD.getMin()) * (evoAmount / 100.0F));
		powerSliderCY = powerSliderCY + ((ofRandom(2.0F) - 1.0F) * (powerSliderCY.getMax() - powerSliderCY.getMin()) * (evoAmount / 100.0F));
		powerSliderCP = powerSliderCP + ((ofRandom(2.0F) - 1.0F) * (powerSliderCP.getMax() - powerSliderCP.getMin()) * (evoAmount / 100.0F));
		powerSliderCM = powerSliderCM + ((ofRandom(2.0F) - 1.0F) * (powerSliderCM.getMax() - powerSliderCM.getMin()) * (evoAmount / 100.0F));
		powerSliderCA = powerSliderCA + ((ofRandom(2.0F) - 1.0F) * (powerSliderCA.getMax() - powerSliderCA.getMin()) * (evoAmount / 100.0F));
		powerSliderCT = powerSliderCT + ((ofRandom(2.0F) - 1.0F) * (powerSliderCT.getMax() - powerSliderCT.getMin()) * (evoAmount / 100.0F));
		powerSliderCS = powerSliderCS + ((ofRandom(2.0F) - 1.0F) * (powerSliderCS.getMax() - powerSliderCS.getMin()) * (evoAmount / 100.0F));
		powerSliderCF = powerSliderCF + ((ofRandom(2.0F) - 1.0F) * (powerSliderCF.getMax() - powerSliderCF.getMin()) * (evoAmount / 100.0F));
		powerSliderDR = powerSliderDR + ((ofRandom(2.0F) - 1.0F) * (powerSliderDR.getMax() - powerSliderDR.getMin()) * (evoAmount / 100.0F));
		powerSliderDG = powerSliderDG + ((ofRandom(2.0F) - 1.0F) * (powerSliderDG.getMax() - powerSliderDG.getMin()) * (evoAmount / 100.0F));
		powerSliderDB = powerSliderDB + ((ofRandom(2.0F) - 1.0F) * (powerSliderDB.getMax() - powerSliderDB.getMin()) * (evoAmount / 100.0F));
		powerSliderDW = powerSliderDW + ((ofRandom(2.0F) - 1.0F) * (powerSliderDW.getMax() - powerSliderDW.getMin()) * (evoAmount / 100.0F));
		powerSliderDO = powerSliderDO + ((ofRandom(2.0F) - 1.0F) * (powerSliderDO.getMax() - powerSliderDO.getMin()) * (evoAmount / 100.0F));
		powerSliderDK = powerSliderDK + ((ofRandom(2.0F) - 1.0F) * (powerSliderDK.getMax() - powerSliderDK.getMin()) * (evoAmount / 100.0F));
		powerSliderDC = powerSliderDC + ((ofRandom(2.0F) - 1.0F) * (powerSliderDC.getMax() - powerSliderDC.getMin()) * (evoAmount / 100.0F));
		powerSliderDD = powerSliderDD + ((ofRandom(2.0F) - 1.0F) * (powerSliderDD.getMax() - powerSliderDD.getMin()) * (evoAmount / 100.0F));
		powerSliderDY = powerSliderDY + ((ofRandom(2.0F) - 1.0F) * (powerSliderDY.getMax() - powerSliderDY.getMin()) * (evoAmount / 100.0F));
		powerSliderDP = powerSliderDP + ((ofRandom(2.0F) - 1.0F) * (powerSliderDP.getMax() - powerSliderDP.getMin()) * (evoAmount / 100.0F));
		powerSliderDM = powerSliderDM + ((ofRandom(2.0F) - 1.0F) * (powerSliderDM.getMax() - powerSliderDM.getMin()) * (evoAmount / 100.0F));
		powerSliderDA = powerSliderDA + ((ofRandom(2.0F) - 1.0F) * (powerSliderDA.getMax() - powerSliderDA.getMin()) * (evoAmount / 100.0F));
		powerSliderDT = powerSliderDT + ((ofRandom(2.0F) - 1.0F) * (powerSliderDT.getMax() - powerSliderDT.getMin()) * (evoAmount / 100.0F));
		powerSliderDS = powerSliderDS + ((ofRandom(2.0F) - 1.0F) * (powerSliderDS.getMax() - powerSliderDS.getMin()) * (evoAmount / 100.0F));
		powerSliderDF = powerSliderDF + ((ofRandom(2.0F) - 1.0F) * (powerSliderDF.getMax() - powerSliderDF.getMin()) * (evoAmount / 100.0F));
		powerSliderYR = powerSliderYR + ((ofRandom(2.0F) - 1.0F) * (powerSliderYR.getMax() - powerSliderYR.getMin()) * (evoAmount / 100.0F));
		powerSliderYG = powerSliderYG + ((ofRandom(2.0F) - 1.0F) * (powerSliderYG.getMax() - powerSliderYG.getMin()) * (evoAmount / 100.0F));
		powerSliderYB = powerSliderYB + ((ofRandom(2.0F) - 1.0F) * (powerSliderYB.getMax() - powerSliderYB.getMin()) * (evoAmount / 100.0F));
		powerSliderYW = powerSliderYW + ((ofRandom(2.0F) - 1.0F) * (powerSliderYW.getMax() - powerSliderYW.getMin()) * (evoAmount / 100.0F));
		powerSliderYO = powerSliderYO + ((ofRandom(2.0F) - 1.0F) * (powerSliderYO.getMax() - powerSliderYO.getMin()) * (evoAmount / 100.0F));
		powerSliderYK = powerSliderYK + ((ofRandom(2.0F) - 1.0F) * (powerSliderYK.getMax() - powerSliderYK.getMin()) * (evoAmount / 100.0F));
		powerSliderYC = powerSliderYC + ((ofRandom(2.0F) - 1.0F) * (powerSliderYC.getMax() - powerSliderYC.getMin()) * (evoAmount / 100.0F));
		powerSliderYD = powerSliderYD + ((ofRandom(2.0F) - 1.0F) * (powerSliderYD.getMax() - powerSliderYD.getMin()) * (evoAmount / 100.0F));
		powerSliderYY = powerSliderYY + ((ofRandom(2.0F) - 1.0F) * (powerSliderYY.getMax() - powerSliderYY.getMin()) * (evoAmount / 100.0F));
		powerSliderYP = powerSliderYP + ((ofRandom(2.0F) - 1.0F) * (powerSliderYP.getMax() - powerSliderYP.getMin()) * (evoAmount / 100.0F));
		powerSliderYM = powerSliderYM + ((ofRandom(2.0F) - 1.0F) * (powerSliderYM.getMax() - powerSliderYM.getMin()) * (evoAmount / 100.0F));
		powerSliderYA = powerSliderYA + ((ofRandom(2.0F) - 1.0F) * (powerSliderYA.getMax() - powerSliderYA.getMin()) * (evoAmount / 100.0F));
		powerSliderYT = powerSliderYT + ((ofRandom(2.0F) - 1.0F) * (powerSliderYT.getMax() - powerSliderYT.getMin()) * (evoAmount / 100.0F));
		powerSliderYS = powerSliderYS + ((ofRandom(2.0F) - 1.0F) * (powerSliderYS.getMax() - powerSliderYS.getMin()) * (evoAmount / 100.0F));
		powerSliderYF = powerSliderYF + ((ofRandom(2.0F) - 1.0F) * (powerSliderYF.getMax() - powerSliderYF.getMin()) * (evoAmount / 100.0F));
		powerSliderPR = powerSliderPR + ((ofRandom(2.0F) - 1.0F) * (powerSliderPR.getMax() - powerSliderPR.getMin()) * (evoAmount / 100.0F));
		powerSliderPG = powerSliderPG + ((ofRandom(2.0F) - 1.0F) * (powerSliderPG.getMax() - powerSliderPG.getMin()) * (evoAmount / 100.0F));
		powerSliderPB = powerSliderPB + ((ofRandom(2.0F) - 1.0F) * (powerSliderPB.getMax() - powerSliderPB.getMin()) * (evoAmount / 100.0F));
		powerSliderPW = powerSliderPW + ((ofRandom(2.0F) - 1.0F) * (powerSliderPW.getMax() - powerSliderPW.getMin()) * (evoAmount / 100.0F));
		powerSliderPO = powerSliderPO + ((ofRandom(2.0F) - 1.0F) * (powerSliderPO.getMax() - powerSliderPO.getMin()) * (evoAmount / 100.0F));
		powerSliderPK = powerSliderPK + ((ofRandom(2.0F) - 1.0F) * (powerSliderPK.getMax() - powerSliderPK.getMin()) * (evoAmount / 100.0F));
		powerSliderPC = powerSliderPC + ((ofRandom(2.0F) - 1.0F) * (powerSliderPC.getMax() - powerSliderPC.getMin()) * (evoAmount / 100.0F));
		powerSliderPD = powerSliderPD + ((ofRandom(2.0F) - 1.0F) * (powerSliderPD.getMax() - powerSliderPD.getMin()) * (evoAmount / 100.0F));
		powerSliderPY = powerSliderPY + ((ofRandom(2.0F) - 1.0F) * (powerSliderPY.getMax() - powerSliderPY.getMin()) * (evoAmount / 100.0F));
		powerSliderPP = powerSliderPP + ((ofRandom(2.0F) - 1.0F) * (powerSliderPP.getMax() - powerSliderPP.getMin()) * (evoAmount / 100.0F));
		powerSliderPM = powerSliderPM + ((ofRandom(2.0F) - 1.0F) * (powerSliderPM.getMax() - powerSliderPM.getMin()) * (evoAmount / 100.0F));
		powerSliderPA = powerSliderPA + ((ofRandom(2.0F) - 1.0F) * (powerSliderPA.getMax() - powerSliderPA.getMin()) * (evoAmount / 100.0F));
		powerSliderPT = powerSliderPT + ((ofRandom(2.0F) - 1.0F) * (powerSliderPT.getMax() - powerSliderPT.getMin()) * (evoAmount / 100.0F));
		powerSliderPS = powerSliderPS + ((ofRandom(2.0F) - 1.0F) * (powerSliderPS.getMax() - powerSliderPS.getMin()) * (evoAmount / 100.0F));
		powerSliderPF = powerSliderPF + ((ofRandom(2.0F) - 1.0F) * (powerSliderPF.getMax() - powerSliderPF.getMin()) * (evoAmount / 100.0F));
		powerSliderMR = powerSliderMR + ((ofRandom(2.0F) - 1.0F) * (powerSliderMR.getMax() - powerSliderMR.getMin()) * (evoAmount / 100.0F));
		powerSliderMG = powerSliderMG + ((ofRandom(2.0F) - 1.0F) * (powerSliderMG.getMax() - powerSliderMG.getMin()) * (evoAmount / 100.0F));
		powerSliderMB = powerSliderMB + ((ofRandom(2.0F) - 1.0F) * (powerSliderMB.getMax() - powerSliderMB.getMin()) * (evoAmount / 100.0F));
		powerSliderMW = powerSliderMW + ((ofRandom(2.0F) - 1.0F) * (powerSliderMW.getMax() - powerSliderMW.getMin()) * (evoAmount / 100.0F));
		powerSliderMO = powerSliderMO + ((ofRandom(2.0F) - 1.0F) * (powerSliderMO.getMax() - powerSliderMO.getMin()) * (evoAmount / 100.0F));
		powerSliderMK = powerSliderMK + ((ofRandom(2.0F) - 1.0F) * (powerSliderMK.getMax() - powerSliderMK.getMin()) * (evoAmount / 100.0F));
		powerSliderMC = powerSliderMC + ((ofRandom(2.0F) - 1.0F) * (powerSliderMC.getMax() - powerSliderMC.getMin()) * (evoAmount / 100.0F));
		powerSliderMD = powerSliderMD + ((ofRandom(2.0F) - 1.0F) * (powerSliderMD.getMax() - powerSliderMD.getMin()) * (evoAmount / 100.0F));
		powerSliderMY = powerSliderMY + ((ofRandom(2.0F) - 1.0F) * (powerSliderMY.getMax() - powerSliderMY.getMin()) * (evoAmount / 100.0F));
		powerSliderMP = powerSliderMP + ((ofRandom(2.0F) - 1.0F) * (powerSliderMP.getMax() - powerSliderMP.getMin()) * (evoAmount / 100.0F));
		powerSliderMM = powerSliderMM + ((ofRandom(2.0F) - 1.0F) * (powerSliderMM.getMax() - powerSliderMM.getMin()) * (evoAmount / 100.0F));
		powerSliderMA = powerSliderMA + ((ofRandom(2.0F) - 1.0F) * (powerSliderMA.getMax() - powerSliderMA.getMin()) * (evoAmount / 100.0F));
		powerSliderMT = powerSliderMT + ((ofRandom(2.0F) - 1.0F) * (powerSliderMT.getMax() - powerSliderMT.getMin()) * (evoAmount / 100.0F));
		powerSliderMS = powerSliderMS + ((ofRandom(2.0F) - 1.0F) * (powerSliderMS.getMax() - powerSliderMS.getMin()) * (evoAmount / 100.0F));
		powerSliderMF = powerSliderMF + ((ofRandom(2.0F) - 1.0F) * (powerSliderMF.getMax() - powerSliderMF.getMin()) * (evoAmount / 100.0F));
		powerSliderAR = powerSliderAR + ((ofRandom(2.0F) - 1.0F) * (powerSliderAR.getMax() - powerSliderAR.getMin()) * (evoAmount / 100.0F));
		powerSliderAG = powerSliderAG + ((ofRandom(2.0F) - 1.0F) * (powerSliderAG.getMax() - powerSliderAG.getMin()) * (evoAmount / 100.0F));
		powerSliderAB = powerSliderAB + ((ofRandom(2.0F) - 1.0F) * (powerSliderAB.getMax() - powerSliderAB.getMin()) * (evoAmount / 100.0F));
		powerSliderAW = powerSliderAW + ((ofRandom(2.0F) - 1.0F) * (powerSliderAW.getMax() - powerSliderAW.getMin()) * (evoAmount / 100.0F));
		powerSliderAO = powerSliderAO + ((ofRandom(2.0F) - 1.0F) * (powerSliderAO.getMax() - powerSliderAO.getMin()) * (evoAmount / 100.0F));
		powerSliderAK = powerSliderAK + ((ofRandom(2.0F) - 1.0F) * (powerSliderAK.getMax() - powerSliderAK.getMin()) * (evoAmount / 100.0F));
		powerSliderAC = powerSliderAC + ((ofRandom(2.0F) - 1.0F) * (powerSliderAC.getMax() - powerSliderAC.getMin()) * (evoAmount / 100.0F));
		powerSliderAD = powerSliderAD + ((ofRandom(2.0F) - 1.0F) * (powerSliderAD.getMax() - powerSliderAD.getMin()) * (evoAmount / 100.0F));
		powerSliderAY = powerSliderAY + ((ofRandom(2.0F) - 1.0F) * (powerSliderAY.getMax() - powerSliderAY.getMin()) * (evoAmount / 100.0F));
		powerSliderAP = powerSliderAP + ((ofRandom(2.0F) - 1.0F) * (powerSliderAP.getMax() - powerSliderAP.getMin()) * (evoAmount / 100.0F));
		powerSliderAM = powerSliderAM + ((ofRandom(2.0F) - 1.0F) * (powerSliderAM.getMax() - powerSliderAM.getMin()) * (evoAmount / 100.0F));
		powerSliderAA = powerSliderAA + ((ofRandom(2.0F) - 1.0F) * (powerSliderAA.getMax() - powerSliderAA.getMin()) * (evoAmount / 100.0F));
		powerSliderAT = powerSliderAT + ((ofRandom(2.0F) - 1.0F) * (powerSliderAT.getMax() - powerSliderAT.getMin()) * (evoAmount / 100.0F));
		powerSliderAS = powerSliderAS + ((ofRandom(2.0F) - 1.0F) * (powerSliderAS.getMax() - powerSliderAS.getMin()) * (evoAmount / 100.0F));
		powerSliderAF = powerSliderAF + ((ofRandom(2.0F) - 1.0F) * (powerSliderAF.getMax() - powerSliderAF.getMin()) * (evoAmount / 100.0F));
		powerSliderTR = powerSliderTR + ((ofRandom(2.0F) - 1.0F) * (powerSliderTR.getMax() - powerSliderTR.getMin()) * (evoAmount / 100.0F));
		powerSliderTG = powerSliderTG + ((ofRandom(2.0F) - 1.0F) * (powerSliderTG.getMax() - powerSliderTG.getMin()) * (evoAmount / 100.0F));
		powerSliderTB = powerSliderTB + ((ofRandom(2.0F) - 1.0F) * (powerSliderTB.getMax() - powerSliderTB.getMin()) * (evoAmount / 100.0F));
		powerSliderTW = powerSliderTW + ((ofRandom(2.0F) - 1.0F) * (powerSliderTW.getMax() - powerSliderTW.getMin()) * (evoAmount / 100.0F));
		powerSliderTO = powerSliderTO + ((ofRandom(2.0F) - 1.0F) * (powerSliderTO.getMax() - powerSliderTO.getMin()) * (evoAmount / 100.0F));
		powerSliderTK = powerSliderTK + ((ofRandom(2.0F) - 1.0F) * (powerSliderTK.getMax() - powerSliderTK.getMin()) * (evoAmount / 100.0F));
		powerSliderTC = powerSliderTC + ((ofRandom(2.0F) - 1.0F) * (powerSliderTC.getMax() - powerSliderTC.getMin()) * (evoAmount / 100.0F));
		powerSliderTD = powerSliderTD + ((ofRandom(2.0F) - 1.0F) * (powerSliderTD.getMax() - powerSliderTD.getMin()) * (evoAmount / 100.0F));
		powerSliderTY = powerSliderTY + ((ofRandom(2.0F) - 1.0F) * (powerSliderTY.getMax() - powerSliderTY.getMin()) * (evoAmount / 100.0F));
		powerSliderTP = powerSliderTP + ((ofRandom(2.0F) - 1.0F) * (powerSliderTP.getMax() - powerSliderTP.getMin()) * (evoAmount / 100.0F));
		powerSliderTM = powerSliderTM + ((ofRandom(2.0F) - 1.0F) * (powerSliderTM.getMax() - powerSliderTM.getMin()) * (evoAmount / 100.0F));
		powerSliderTA = powerSliderTA + ((ofRandom(2.0F) - 1.0F) * (powerSliderTA.getMax() - powerSliderTA.getMin()) * (evoAmount / 100.0F));
		powerSliderTT = powerSliderTT + ((ofRandom(2.0F) - 1.0F) * (powerSliderTT.getMax() - powerSliderTT.getMin()) * (evoAmount / 100.0F));
		powerSliderTS = powerSliderTS + ((ofRandom(2.0F) - 1.0F) * (powerSliderTS.getMax() - powerSliderTS.getMin()) * (evoAmount / 100.0F));
		powerSliderTF = powerSliderTF + ((ofRandom(2.0F) - 1.0F) * (powerSliderTF.getMax() - powerSliderTF.getMin()) * (evoAmount / 100.0F));
		powerSliderSR = powerSliderSR + ((ofRandom(2.0F) - 1.0F) * (powerSliderSR.getMax() - powerSliderSR.getMin()) * (evoAmount / 100.0F));
		powerSliderSG = powerSliderSG + ((ofRandom(2.0F) - 1.0F) * (powerSliderSG.getMax() - powerSliderSG.getMin()) * (evoAmount / 100.0F));
		powerSliderSB = powerSliderSB + ((ofRandom(2.0F) - 1.0F) * (powerSliderSB.getMax() - powerSliderSB.getMin()) * (evoAmount / 100.0F));
		powerSliderSW = powerSliderSW + ((ofRandom(2.0F) - 1.0F) * (powerSliderSW.getMax() - powerSliderSW.getMin()) * (evoAmount / 100.0F));
		powerSliderSO = powerSliderSO + ((ofRandom(2.0F) - 1.0F) * (powerSliderSO.getMax() - powerSliderSO.getMin()) * (evoAmount / 100.0F));
		powerSliderSK = powerSliderSK + ((ofRandom(2.0F) - 1.0F) * (powerSliderSK.getMax() - powerSliderSK.getMin()) * (evoAmount / 100.0F));
		powerSliderSC = powerSliderSC + ((ofRandom(2.0F) - 1.0F) * (powerSliderSC.getMax() - powerSliderSC.getMin()) * (evoAmount / 100.0F));
		powerSliderSD = powerSliderSD + ((ofRandom(2.0F) - 1.0F) * (powerSliderSD.getMax() - powerSliderSD.getMin()) * (evoAmount / 100.0F));
		powerSliderSY = powerSliderSY + ((ofRandom(2.0F) - 1.0F) * (powerSliderSY.getMax() - powerSliderSY.getMin()) * (evoAmount / 100.0F));
		powerSliderSP = powerSliderSP + ((ofRandom(2.0F) - 1.0F) * (powerSliderSP.getMax() - powerSliderSP.getMin()) * (evoAmount / 100.0F));
		powerSliderSM = powerSliderSM + ((ofRandom(2.0F) - 1.0F) * (powerSliderSM.getMax() - powerSliderSM.getMin()) * (evoAmount / 100.0F));
		powerSliderSA = powerSliderSA + ((ofRandom(2.0F) - 1.0F) * (powerSliderSA.getMax() - powerSliderSA.getMin()) * (evoAmount / 100.0F));
		powerSliderST = powerSliderST + ((ofRandom(2.0F) - 1.0F) * (powerSliderST.getMax() - powerSliderST.getMin()) * (evoAmount / 100.0F));
		powerSliderSS = powerSliderSS + ((ofRandom(2.0F) - 1.0F) * (powerSliderSS.getMax() - powerSliderSS.getMin()) * (evoAmount / 100.0F));
		powerSliderSF = powerSliderSF + ((ofRandom(2.0F) - 1.0F) * (powerSliderSF.getMax() - powerSliderSF.getMin()) * (evoAmount / 100.0F));
		powerSliderFR = powerSliderFR + ((ofRandom(2.0F) - 1.0F) * (powerSliderFR.getMax() - powerSliderFR.getMin()) * (evoAmount / 100.0F));
		powerSliderFG = powerSliderFG + ((ofRandom(2.0F) - 1.0F) * (powerSliderFG.getMax() - powerSliderFG.getMin()) * (evoAmount / 100.0F));
		powerSliderFB = powerSliderFB + ((ofRandom(2.0F) - 1.0F) * (powerSliderFB.getMax() - powerSliderFB.getMin()) * (evoAmount / 100.0F));
		powerSliderFW = powerSliderFW + ((ofRandom(2.0F) - 1.0F) * (powerSliderFW.getMax() - powerSliderFW.getMin()) * (evoAmount / 100.0F));
		powerSliderFO = powerSliderFO + ((ofRandom(2.0F) - 1.0F) * (powerSliderFO.getMax() - powerSliderFO.getMin()) * (evoAmount / 100.0F));
		powerSliderFK = powerSliderFK + ((ofRandom(2.0F) - 1.0F) * (powerSliderFK.getMax() - powerSliderFK.getMin()) * (evoAmount / 100.0F));
		powerSliderFC = powerSliderFC + ((ofRandom(2.0F) - 1.0F) * (powerSliderFC.getMax() - powerSliderFC.getMin()) * (evoAmount / 100.0F));
		powerSliderFD = powerSliderFD + ((ofRandom(2.0F) - 1.0F) * (powerSliderFD.getMax() - powerSliderFD.getMin()) * (evoAmount / 100.0F));
		powerSliderFY = powerSliderFY + ((ofRandom(2.0F) - 1.0F) * (powerSliderFY.getMax() - powerSliderFY.getMin()) * (evoAmount / 100.0F));
		powerSliderFP = powerSliderFP + ((ofRandom(2.0F) - 1.0F) * (powerSliderFP.getMax() - powerSliderFP.getMin()) * (evoAmount / 100.0F));
		powerSliderFM = powerSliderFM + ((ofRandom(2.0F) - 1.0F) * (powerSliderFM.getMax() - powerSliderFM.getMin()) * (evoAmount / 100.0F));
		powerSliderFA = powerSliderFA + ((ofRandom(2.0F) - 1.0F) * (powerSliderFA.getMax() - powerSliderFA.getMin()) * (evoAmount / 100.0F));
		powerSliderFT = powerSliderFT + ((ofRandom(2.0F) - 1.0F) * (powerSliderFT.getMax() - powerSliderFT.getMin()) * (evoAmount / 100.0F));
		powerSliderFS = powerSliderFS + ((ofRandom(2.0F) - 1.0F) * (powerSliderFS.getMax() - powerSliderFS.getMin()) * (evoAmount / 100.0F));
		powerSliderFF = powerSliderFF + ((ofRandom(2.0F) - 1.0F) * (powerSliderFF.getMax() - powerSliderFF.getMin()) * (evoAmount / 100.0F));

		vSliderRR = vSliderRR + ((ofRandom(2.0F) - 1.0F) * (vSliderRR.getMax() - vSliderRR.getMin()) * (evoAmount / 100.0F));
		vSliderRG = vSliderRG + ((ofRandom(2.0F) - 1.0F) * (vSliderRG.getMax() - vSliderRG.getMin()) * (evoAmount / 100.0F));
		vSliderRB = vSliderRB + ((ofRandom(2.0F) - 1.0F) * (vSliderRB.getMax() - vSliderRB.getMin()) * (evoAmount / 100.0F));
		vSliderRW = vSliderRW + ((ofRandom(2.0F) - 1.0F) * (vSliderRW.getMax() - vSliderRW.getMin()) * (evoAmount / 100.0F));
		vSliderRO = vSliderRO + ((ofRandom(2.0F) - 1.0F) * (vSliderRO.getMax() - vSliderRO.getMin()) * (evoAmount / 100.0F));
		vSliderRK = vSliderRK + ((ofRandom(2.0F) - 1.0F) * (vSliderRK.getMax() - vSliderRK.getMin()) * (evoAmount / 100.0F));
		vSliderRC = vSliderRC + ((ofRandom(2.0F) - 1.0F) * (vSliderRC.getMax() - vSliderRC.getMin()) * (evoAmount / 100.0F));
		vSliderRD = vSliderRD + ((ofRandom(2.0F) - 1.0F) * (vSliderRD.getMax() - vSliderRD.getMin()) * (evoAmount / 100.0F));
		vSliderRY = vSliderRY + ((ofRandom(2.0F) - 1.0F) * (vSliderRY.getMax() - vSliderRY.getMin()) * (evoAmount / 100.0F));
		vSliderRP = vSliderRP + ((ofRandom(2.0F) - 1.0F) * (vSliderRP.getMax() - vSliderRP.getMin()) * (evoAmount / 100.0F));
		vSliderRM = vSliderRM + ((ofRandom(2.0F) - 1.0F) * (vSliderRM.getMax() - vSliderRM.getMin()) * (evoAmount / 100.0F));
		vSliderRA = vSliderRA + ((ofRandom(2.0F) - 1.0F) * (vSliderRA.getMax() - vSliderRA.getMin()) * (evoAmount / 100.0F));
		vSliderRT = vSliderRT + ((ofRandom(2.0F) - 1.0F) * (vSliderRT.getMax() - vSliderRT.getMin()) * (evoAmount / 100.0F));
		vSliderRS = vSliderRS + ((ofRandom(2.0F) - 1.0F) * (vSliderRS.getMax() - vSliderRS.getMin()) * (evoAmount / 100.0F));
		vSliderRF = vSliderRF + ((ofRandom(2.0F) - 1.0F) * (vSliderRF.getMax() - vSliderRF.getMin()) * (evoAmount / 100.0F));
		vSliderGR = vSliderGR + ((ofRandom(2.0F) - 1.0F) * (vSliderGR.getMax() - vSliderGR.getMin()) * (evoAmount / 100.0F));
		vSliderGG = vSliderGG + ((ofRandom(2.0F) - 1.0F) * (vSliderGG.getMax() - vSliderGG.getMin()) * (evoAmount / 100.0F));
		vSliderGB = vSliderGB + ((ofRandom(2.0F) - 1.0F) * (vSliderGB.getMax() - vSliderGB.getMin()) * (evoAmount / 100.0F));
		vSliderGW = vSliderGW + ((ofRandom(2.0F) - 1.0F) * (vSliderGW.getMax() - vSliderGW.getMin()) * (evoAmount / 100.0F));
		vSliderGO = vSliderGO + ((ofRandom(2.0F) - 1.0F) * (vSliderGO.getMax() - vSliderGO.getMin()) * (evoAmount / 100.0F));
		vSliderGK = vSliderGK + ((ofRandom(2.0F) - 1.0F) * (vSliderGK.getMax() - vSliderGK.getMin()) * (evoAmount / 100.0F));
		vSliderGC = vSliderGC + ((ofRandom(2.0F) - 1.0F) * (vSliderGC.getMax() - vSliderGC.getMin()) * (evoAmount / 100.0F));
		vSliderGD = vSliderGD + ((ofRandom(2.0F) - 1.0F) * (vSliderGD.getMax() - vSliderGD.getMin()) * (evoAmount / 100.0F));
		vSliderGY = vSliderGY + ((ofRandom(2.0F) - 1.0F) * (vSliderGY.getMax() - vSliderGY.getMin()) * (evoAmount / 100.0F));
		vSliderGP = vSliderGP + ((ofRandom(2.0F) - 1.0F) * (vSliderGP.getMax() - vSliderGP.getMin()) * (evoAmount / 100.0F));
		vSliderGM = vSliderGM + ((ofRandom(2.0F) - 1.0F) * (vSliderGM.getMax() - vSliderGM.getMin()) * (evoAmount / 100.0F));
		vSliderGA = vSliderGA + ((ofRandom(2.0F) - 1.0F) * (vSliderGA.getMax() - vSliderGA.getMin()) * (evoAmount / 100.0F));
		vSliderGT = vSliderGT + ((ofRandom(2.0F) - 1.0F) * (vSliderGT.getMax() - vSliderGT.getMin()) * (evoAmount / 100.0F));
		vSliderGS = vSliderGS + ((ofRandom(2.0F) - 1.0F) * (vSliderGS.getMax() - vSliderGS.getMin()) * (evoAmount / 100.0F));
		vSliderGF = vSliderGF + ((ofRandom(2.0F) - 1.0F) * (vSliderGF.getMax() - vSliderGF.getMin()) * (evoAmount / 100.0F));
		vSliderBR = vSliderBR + ((ofRandom(2.0F) - 1.0F) * (vSliderBR.getMax() - vSliderBR.getMin()) * (evoAmount / 100.0F));
		vSliderBG = vSliderBG + ((ofRandom(2.0F) - 1.0F) * (vSliderBG.getMax() - vSliderBG.getMin()) * (evoAmount / 100.0F));
		vSliderBB = vSliderBB + ((ofRandom(2.0F) - 1.0F) * (vSliderBB.getMax() - vSliderBB.getMin()) * (evoAmount / 100.0F));
		vSliderBW = vSliderBW + ((ofRandom(2.0F) - 1.0F) * (vSliderBW.getMax() - vSliderBW.getMin()) * (evoAmount / 100.0F));
		vSliderBO = vSliderBO + ((ofRandom(2.0F) - 1.0F) * (vSliderBO.getMax() - vSliderBO.getMin()) * (evoAmount / 100.0F));
		vSliderBK = vSliderBK + ((ofRandom(2.0F) - 1.0F) * (vSliderBK.getMax() - vSliderBK.getMin()) * (evoAmount / 100.0F));
		vSliderBC = vSliderBC + ((ofRandom(2.0F) - 1.0F) * (vSliderBC.getMax() - vSliderBC.getMin()) * (evoAmount / 100.0F));
		vSliderBD = vSliderBD + ((ofRandom(2.0F) - 1.0F) * (vSliderBD.getMax() - vSliderBD.getMin()) * (evoAmount / 100.0F));
		vSliderBY = vSliderBY + ((ofRandom(2.0F) - 1.0F) * (vSliderBY.getMax() - vSliderBY.getMin()) * (evoAmount / 100.0F));
		vSliderBP = vSliderBP + ((ofRandom(2.0F) - 1.0F) * (vSliderBP.getMax() - vSliderBP.getMin()) * (evoAmount / 100.0F));
		vSliderBM = vSliderBM + ((ofRandom(2.0F) - 1.0F) * (vSliderBM.getMax() - vSliderBM.getMin()) * (evoAmount / 100.0F));
		vSliderBA = vSliderBA + ((ofRandom(2.0F) - 1.0F) * (vSliderBA.getMax() - vSliderBA.getMin()) * (evoAmount / 100.0F));
		vSliderBT = vSliderBT + ((ofRandom(2.0F) - 1.0F) * (vSliderBT.getMax() - vSliderBT.getMin()) * (evoAmount / 100.0F));
		vSliderBS = vSliderBS + ((ofRandom(2.0F) - 1.0F) * (vSliderBS.getMax() - vSliderBS.getMin()) * (evoAmount / 100.0F));
		vSliderBF = vSliderBF + ((ofRandom(2.0F) - 1.0F) * (vSliderBF.getMax() - vSliderBF.getMin()) * (evoAmount / 100.0F));
		vSliderWR = vSliderWR + ((ofRandom(2.0F) - 1.0F) * (vSliderWR.getMax() - vSliderWR.getMin()) * (evoAmount / 100.0F));
		vSliderWG = vSliderWG + ((ofRandom(2.0F) - 1.0F) * (vSliderWG.getMax() - vSliderWG.getMin()) * (evoAmount / 100.0F));
		vSliderWB = vSliderWB + ((ofRandom(2.0F) - 1.0F) * (vSliderWB.getMax() - vSliderWB.getMin()) * (evoAmount / 100.0F));
		vSliderWW = vSliderWW + ((ofRandom(2.0F) - 1.0F) * (vSliderWW.getMax() - vSliderWW.getMin()) * (evoAmount / 100.0F));
		vSliderWO = vSliderWO + ((ofRandom(2.0F) - 1.0F) * (vSliderWO.getMax() - vSliderWO.getMin()) * (evoAmount / 100.0F));
		vSliderWK = vSliderWK + ((ofRandom(2.0F) - 1.0F) * (vSliderWK.getMax() - vSliderWK.getMin()) * (evoAmount / 100.0F));
		vSliderWC = vSliderWC + ((ofRandom(2.0F) - 1.0F) * (vSliderWC.getMax() - vSliderWC.getMin()) * (evoAmount / 100.0F));
		vSliderWD = vSliderWD + ((ofRandom(2.0F) - 1.0F) * (vSliderWD.getMax() - vSliderWD.getMin()) * (evoAmount / 100.0F));
		vSliderWY = vSliderWY + ((ofRandom(2.0F) - 1.0F) * (vSliderWY.getMax() - vSliderWY.getMin()) * (evoAmount / 100.0F));
		vSliderWP = vSliderWP + ((ofRandom(2.0F) - 1.0F) * (vSliderWP.getMax() - vSliderWP.getMin()) * (evoAmount / 100.0F));
		vSliderWM = vSliderWM + ((ofRandom(2.0F) - 1.0F) * (vSliderWM.getMax() - vSliderWM.getMin()) * (evoAmount / 100.0F));
		vSliderWA = vSliderWA + ((ofRandom(2.0F) - 1.0F) * (vSliderWA.getMax() - vSliderWA.getMin()) * (evoAmount / 100.0F));
		vSliderWT = vSliderWT + ((ofRandom(2.0F) - 1.0F) * (vSliderWT.getMax() - vSliderWT.getMin()) * (evoAmount / 100.0F));
		vSliderWS = vSliderWS + ((ofRandom(2.0F) - 1.0F) * (vSliderWS.getMax() - vSliderWS.getMin()) * (evoAmount / 100.0F));
		vSliderWF = vSliderWF + ((ofRandom(2.0F) - 1.0F) * (vSliderWF.getMax() - vSliderWF.getMin()) * (evoAmount / 100.0F));
		vSliderOR = vSliderOR + ((ofRandom(2.0F) - 1.0F) * (vSliderOR.getMax() - vSliderOR.getMin()) * (evoAmount / 100.0F));
		vSliderOG = vSliderOG + ((ofRandom(2.0F) - 1.0F) * (vSliderOG.getMax() - vSliderOG.getMin()) * (evoAmount / 100.0F));
		vSliderOB = vSliderOB + ((ofRandom(2.0F) - 1.0F) * (vSliderOB.getMax() - vSliderOB.getMin()) * (evoAmount / 100.0F));
		vSliderOW = vSliderOW + ((ofRandom(2.0F) - 1.0F) * (vSliderOW.getMax() - vSliderOW.getMin()) * (evoAmount / 100.0F));
		vSliderOO = vSliderOO + ((ofRandom(2.0F) - 1.0F) * (vSliderOO.getMax() - vSliderOO.getMin()) * (evoAmount / 100.0F));
		vSliderOK = vSliderOK + ((ofRandom(2.0F) - 1.0F) * (vSliderOK.getMax() - vSliderOK.getMin()) * (evoAmount / 100.0F));
		vSliderOC = vSliderOC + ((ofRandom(2.0F) - 1.0F) * (vSliderOC.getMax() - vSliderOC.getMin()) * (evoAmount / 100.0F));
		vSliderOD = vSliderOD + ((ofRandom(2.0F) - 1.0F) * (vSliderOD.getMax() - vSliderOD.getMin()) * (evoAmount / 100.0F));
		vSliderOY = vSliderOY + ((ofRandom(2.0F) - 1.0F) * (vSliderOY.getMax() - vSliderOY.getMin()) * (evoAmount / 100.0F));
		vSliderOP = vSliderOP + ((ofRandom(2.0F) - 1.0F) * (vSliderOP.getMax() - vSliderOP.getMin()) * (evoAmount / 100.0F));
		vSliderOM = vSliderOM + ((ofRandom(2.0F) - 1.0F) * (vSliderOM.getMax() - vSliderOM.getMin()) * (evoAmount / 100.0F));
		vSliderOA = vSliderOA + ((ofRandom(2.0F) - 1.0F) * (vSliderOA.getMax() - vSliderOA.getMin()) * (evoAmount / 100.0F));
		vSliderOT = vSliderOT + ((ofRandom(2.0F) - 1.0F) * (vSliderOT.getMax() - vSliderOT.getMin()) * (evoAmount / 100.0F));
		vSliderOS = vSliderOS + ((ofRandom(2.0F) - 1.0F) * (vSliderOS.getMax() - vSliderOS.getMin()) * (evoAmount / 100.0F));
		vSliderOF = vSliderOF + ((ofRandom(2.0F) - 1.0F) * (vSliderOF.getMax() - vSliderOF.getMin()) * (evoAmount / 100.0F));
		vSliderKR = vSliderKR + ((ofRandom(2.0F) - 1.0F) * (vSliderKR.getMax() - vSliderKR.getMin()) * (evoAmount / 100.0F));
		vSliderKG = vSliderKG + ((ofRandom(2.0F) - 1.0F) * (vSliderKG.getMax() - vSliderKG.getMin()) * (evoAmount / 100.0F));
		vSliderKB = vSliderKB + ((ofRandom(2.0F) - 1.0F) * (vSliderKB.getMax() - vSliderKB.getMin()) * (evoAmount / 100.0F));
		vSliderKW = vSliderKW + ((ofRandom(2.0F) - 1.0F) * (vSliderKW.getMax() - vSliderKW.getMin()) * (evoAmount / 100.0F));
		vSliderKO = vSliderKO + ((ofRandom(2.0F) - 1.0F) * (vSliderKO.getMax() - vSliderKO.getMin()) * (evoAmount / 100.0F));
		vSliderKK = vSliderKK + ((ofRandom(2.0F) - 1.0F) * (vSliderKK.getMax() - vSliderKK.getMin()) * (evoAmount / 100.0F));
		vSliderKC = vSliderKC + ((ofRandom(2.0F) - 1.0F) * (vSliderKC.getMax() - vSliderKC.getMin()) * (evoAmount / 100.0F));
		vSliderKD = vSliderKD + ((ofRandom(2.0F) - 1.0F) * (vSliderKD.getMax() - vSliderKD.getMin()) * (evoAmount / 100.0F));
		vSliderKY = vSliderKY + ((ofRandom(2.0F) - 1.0F) * (vSliderKY.getMax() - vSliderKY.getMin()) * (evoAmount / 100.0F));
		vSliderKP = vSliderKP + ((ofRandom(2.0F) - 1.0F) * (vSliderKP.getMax() - vSliderKP.getMin()) * (evoAmount / 100.0F));
		vSliderKM = vSliderKM + ((ofRandom(2.0F) - 1.0F) * (vSliderKM.getMax() - vSliderKM.getMin()) * (evoAmount / 100.0F));
		vSliderKA = vSliderKA + ((ofRandom(2.0F) - 1.0F) * (vSliderKA.getMax() - vSliderKA.getMin()) * (evoAmount / 100.0F));
		vSliderKT = vSliderKT + ((ofRandom(2.0F) - 1.0F) * (vSliderKT.getMax() - vSliderKT.getMin()) * (evoAmount / 100.0F));
		vSliderKS = vSliderKS + ((ofRandom(2.0F) - 1.0F) * (vSliderKS.getMax() - vSliderKS.getMin()) * (evoAmount / 100.0F));
		vSliderKF = vSliderKF + ((ofRandom(2.0F) - 1.0F) * (vSliderKF.getMax() - vSliderKF.getMin()) * (evoAmount / 100.0F));
		vSliderCR = vSliderCR + ((ofRandom(2.0F) - 1.0F) * (vSliderCR.getMax() - vSliderCR.getMin()) * (evoAmount / 100.0F));
		vSliderCG = vSliderCG + ((ofRandom(2.0F) - 1.0F) * (vSliderCG.getMax() - vSliderCG.getMin()) * (evoAmount / 100.0F));
		vSliderCB = vSliderCB + ((ofRandom(2.0F) - 1.0F) * (vSliderCB.getMax() - vSliderCB.getMin()) * (evoAmount / 100.0F));
		vSliderCW = vSliderCW + ((ofRandom(2.0F) - 1.0F) * (vSliderCW.getMax() - vSliderCW.getMin()) * (evoAmount / 100.0F));
		vSliderCO = vSliderCO + ((ofRandom(2.0F) - 1.0F) * (vSliderCO.getMax() - vSliderCO.getMin()) * (evoAmount / 100.0F));
		vSliderCK = vSliderCK + ((ofRandom(2.0F) - 1.0F) * (vSliderCK.getMax() - vSliderCK.getMin()) * (evoAmount / 100.0F));
		vSliderCC = vSliderCC + ((ofRandom(2.0F) - 1.0F) * (vSliderCC.getMax() - vSliderCC.getMin()) * (evoAmount / 100.0F));
		vSliderCD = vSliderCD + ((ofRandom(2.0F) - 1.0F) * (vSliderCD.getMax() - vSliderCD.getMin()) * (evoAmount / 100.0F));
		vSliderCY = vSliderCY + ((ofRandom(2.0F) - 1.0F) * (vSliderCY.getMax() - vSliderCY.getMin()) * (evoAmount / 100.0F));
		vSliderCP = vSliderCP + ((ofRandom(2.0F) - 1.0F) * (vSliderCP.getMax() - vSliderCP.getMin()) * (evoAmount / 100.0F));
		vSliderCM = vSliderCM + ((ofRandom(2.0F) - 1.0F) * (vSliderCM.getMax() - vSliderCM.getMin()) * (evoAmount / 100.0F));
		vSliderCA = vSliderCA + ((ofRandom(2.0F) - 1.0F) * (vSliderCA.getMax() - vSliderCA.getMin()) * (evoAmount / 100.0F));
		vSliderCT = vSliderCT + ((ofRandom(2.0F) - 1.0F) * (vSliderCT.getMax() - vSliderCT.getMin()) * (evoAmount / 100.0F));
		vSliderCS = vSliderCS + ((ofRandom(2.0F) - 1.0F) * (vSliderCS.getMax() - vSliderCS.getMin()) * (evoAmount / 100.0F));
		vSliderCF = vSliderCF + ((ofRandom(2.0F) - 1.0F) * (vSliderCF.getMax() - vSliderCF.getMin()) * (evoAmount / 100.0F));
		vSliderDR = vSliderDR + ((ofRandom(2.0F) - 1.0F) * (vSliderDR.getMax() - vSliderDR.getMin()) * (evoAmount / 100.0F));
		vSliderDG = vSliderDG + ((ofRandom(2.0F) - 1.0F) * (vSliderDG.getMax() - vSliderDG.getMin()) * (evoAmount / 100.0F));
		vSliderDB = vSliderDB + ((ofRandom(2.0F) - 1.0F) * (vSliderDB.getMax() - vSliderDB.getMin()) * (evoAmount / 100.0F));
		vSliderDW = vSliderDW + ((ofRandom(2.0F) - 1.0F) * (vSliderDW.getMax() - vSliderDW.getMin()) * (evoAmount / 100.0F));
		vSliderDO = vSliderDO + ((ofRandom(2.0F) - 1.0F) * (vSliderDO.getMax() - vSliderDO.getMin()) * (evoAmount / 100.0F));
		vSliderDK = vSliderDK + ((ofRandom(2.0F) - 1.0F) * (vSliderDK.getMax() - vSliderDK.getMin()) * (evoAmount / 100.0F));
		vSliderDC = vSliderDC + ((ofRandom(2.0F) - 1.0F) * (vSliderDC.getMax() - vSliderDC.getMin()) * (evoAmount / 100.0F));
		vSliderDD = vSliderDD + ((ofRandom(2.0F) - 1.0F) * (vSliderDD.getMax() - vSliderDD.getMin()) * (evoAmount / 100.0F));
		vSliderDY = vSliderDY + ((ofRandom(2.0F) - 1.0F) * (vSliderDY.getMax() - vSliderDY.getMin()) * (evoAmount / 100.0F));
		vSliderDP = vSliderDP + ((ofRandom(2.0F) - 1.0F) * (vSliderDP.getMax() - vSliderDP.getMin()) * (evoAmount / 100.0F));
		vSliderDM = vSliderDM + ((ofRandom(2.0F) - 1.0F) * (vSliderDM.getMax() - vSliderDM.getMin()) * (evoAmount / 100.0F));
		vSliderDA = vSliderDA + ((ofRandom(2.0F) - 1.0F) * (vSliderDA.getMax() - vSliderDA.getMin()) * (evoAmount / 100.0F));
		vSliderDT = vSliderDT + ((ofRandom(2.0F) - 1.0F) * (vSliderDT.getMax() - vSliderDT.getMin()) * (evoAmount / 100.0F));
		vSliderDS = vSliderDS + ((ofRandom(2.0F) - 1.0F) * (vSliderDS.getMax() - vSliderDS.getMin()) * (evoAmount / 100.0F));
		vSliderDF = vSliderDF + ((ofRandom(2.0F) - 1.0F) * (vSliderDF.getMax() - vSliderDF.getMin()) * (evoAmount / 100.0F));
		vSliderYR = vSliderYR + ((ofRandom(2.0F) - 1.0F) * (vSliderYR.getMax() - vSliderYR.getMin()) * (evoAmount / 100.0F));
		vSliderYG = vSliderYG + ((ofRandom(2.0F) - 1.0F) * (vSliderYG.getMax() - vSliderYG.getMin()) * (evoAmount / 100.0F));
		vSliderYB = vSliderYB + ((ofRandom(2.0F) - 1.0F) * (vSliderYB.getMax() - vSliderYB.getMin()) * (evoAmount / 100.0F));
		vSliderYW = vSliderYW + ((ofRandom(2.0F) - 1.0F) * (vSliderYW.getMax() - vSliderYW.getMin()) * (evoAmount / 100.0F));
		vSliderYO = vSliderYO + ((ofRandom(2.0F) - 1.0F) * (vSliderYO.getMax() - vSliderYO.getMin()) * (evoAmount / 100.0F));
		vSliderYK = vSliderYK + ((ofRandom(2.0F) - 1.0F) * (vSliderYK.getMax() - vSliderYK.getMin()) * (evoAmount / 100.0F));
		vSliderYC = vSliderYC + ((ofRandom(2.0F) - 1.0F) * (vSliderYC.getMax() - vSliderYC.getMin()) * (evoAmount / 100.0F));
		vSliderYD = vSliderYD + ((ofRandom(2.0F) - 1.0F) * (vSliderYD.getMax() - vSliderYD.getMin()) * (evoAmount / 100.0F));
		vSliderYY = vSliderYY + ((ofRandom(2.0F) - 1.0F) * (vSliderYY.getMax() - vSliderYY.getMin()) * (evoAmount / 100.0F));
		vSliderYP = vSliderYP + ((ofRandom(2.0F) - 1.0F) * (vSliderYP.getMax() - vSliderYP.getMin()) * (evoAmount / 100.0F));
		vSliderYM = vSliderYM + ((ofRandom(2.0F) - 1.0F) * (vSliderYM.getMax() - vSliderYM.getMin()) * (evoAmount / 100.0F));
		vSliderYA = vSliderYA + ((ofRandom(2.0F) - 1.0F) * (vSliderYA.getMax() - vSliderYA.getMin()) * (evoAmount / 100.0F));
		vSliderYT = vSliderYT + ((ofRandom(2.0F) - 1.0F) * (vSliderYT.getMax() - vSliderYT.getMin()) * (evoAmount / 100.0F));
		vSliderYS = vSliderYS + ((ofRandom(2.0F) - 1.0F) * (vSliderYS.getMax() - vSliderYS.getMin()) * (evoAmount / 100.0F));
		vSliderYF = vSliderYF + ((ofRandom(2.0F) - 1.0F) * (vSliderYF.getMax() - vSliderYF.getMin()) * (evoAmount / 100.0F));
		vSliderPR = vSliderPR + ((ofRandom(2.0F) - 1.0F) * (vSliderPR.getMax() - vSliderPR.getMin()) * (evoAmount / 100.0F));
		vSliderPG = vSliderPG + ((ofRandom(2.0F) - 1.0F) * (vSliderPG.getMax() - vSliderPG.getMin()) * (evoAmount / 100.0F));
		vSliderPB = vSliderPB + ((ofRandom(2.0F) - 1.0F) * (vSliderPB.getMax() - vSliderPB.getMin()) * (evoAmount / 100.0F));
		vSliderPW = vSliderPW + ((ofRandom(2.0F) - 1.0F) * (vSliderPW.getMax() - vSliderPW.getMin()) * (evoAmount / 100.0F));
		vSliderPO = vSliderPO + ((ofRandom(2.0F) - 1.0F) * (vSliderPO.getMax() - vSliderPO.getMin()) * (evoAmount / 100.0F));
		vSliderPK = vSliderPK + ((ofRandom(2.0F) - 1.0F) * (vSliderPK.getMax() - vSliderPK.getMin()) * (evoAmount / 100.0F));
		vSliderPC = vSliderPC + ((ofRandom(2.0F) - 1.0F) * (vSliderPC.getMax() - vSliderPC.getMin()) * (evoAmount / 100.0F));
		vSliderPD = vSliderPD + ((ofRandom(2.0F) - 1.0F) * (vSliderPD.getMax() - vSliderPD.getMin()) * (evoAmount / 100.0F));
		vSliderPY = vSliderPY + ((ofRandom(2.0F) - 1.0F) * (vSliderPY.getMax() - vSliderPY.getMin()) * (evoAmount / 100.0F));
		vSliderPP = vSliderPP + ((ofRandom(2.0F) - 1.0F) * (vSliderPP.getMax() - vSliderPP.getMin()) * (evoAmount / 100.0F));
		vSliderPM = vSliderPM + ((ofRandom(2.0F) - 1.0F) * (vSliderPM.getMax() - vSliderPM.getMin()) * (evoAmount / 100.0F));
		vSliderPA = vSliderPA + ((ofRandom(2.0F) - 1.0F) * (vSliderPA.getMax() - vSliderPA.getMin()) * (evoAmount / 100.0F));
		vSliderPT = vSliderPT + ((ofRandom(2.0F) - 1.0F) * (vSliderPT.getMax() - vSliderPT.getMin()) * (evoAmount / 100.0F));
		vSliderPS = vSliderPS + ((ofRandom(2.0F) - 1.0F) * (vSliderPS.getMax() - vSliderPS.getMin()) * (evoAmount / 100.0F));
		vSliderPF = vSliderPF + ((ofRandom(2.0F) - 1.0F) * (vSliderPF.getMax() - vSliderPF.getMin()) * (evoAmount / 100.0F));
		vSliderMR = vSliderMR + ((ofRandom(2.0F) - 1.0F) * (vSliderMR.getMax() - vSliderMR.getMin()) * (evoAmount / 100.0F));
		vSliderMG = vSliderMG + ((ofRandom(2.0F) - 1.0F) * (vSliderMG.getMax() - vSliderMG.getMin()) * (evoAmount / 100.0F));
		vSliderMB = vSliderMB + ((ofRandom(2.0F) - 1.0F) * (vSliderMB.getMax() - vSliderMB.getMin()) * (evoAmount / 100.0F));
		vSliderMW = vSliderMW + ((ofRandom(2.0F) - 1.0F) * (vSliderMW.getMax() - vSliderMW.getMin()) * (evoAmount / 100.0F));
		vSliderMO = vSliderMO + ((ofRandom(2.0F) - 1.0F) * (vSliderMO.getMax() - vSliderMO.getMin()) * (evoAmount / 100.0F));
		vSliderMK = vSliderMK + ((ofRandom(2.0F) - 1.0F) * (vSliderMK.getMax() - vSliderMK.getMin()) * (evoAmount / 100.0F));
		vSliderMC = vSliderMC + ((ofRandom(2.0F) - 1.0F) * (vSliderMC.getMax() - vSliderMC.getMin()) * (evoAmount / 100.0F));
		vSliderMD = vSliderMD + ((ofRandom(2.0F) - 1.0F) * (vSliderMD.getMax() - vSliderMD.getMin()) * (evoAmount / 100.0F));
		vSliderMY = vSliderMY + ((ofRandom(2.0F) - 1.0F) * (vSliderMY.getMax() - vSliderMY.getMin()) * (evoAmount / 100.0F));
		vSliderMP = vSliderMP + ((ofRandom(2.0F) - 1.0F) * (vSliderMP.getMax() - vSliderMP.getMin()) * (evoAmount / 100.0F));
		vSliderMM = vSliderMM + ((ofRandom(2.0F) - 1.0F) * (vSliderMM.getMax() - vSliderMM.getMin()) * (evoAmount / 100.0F));
		vSliderMA = vSliderMA + ((ofRandom(2.0F) - 1.0F) * (vSliderMA.getMax() - vSliderMA.getMin()) * (evoAmount / 100.0F));
		vSliderMT = vSliderMT + ((ofRandom(2.0F) - 1.0F) * (vSliderMT.getMax() - vSliderMT.getMin()) * (evoAmount / 100.0F));
		vSliderMS = vSliderMS + ((ofRandom(2.0F) - 1.0F) * (vSliderMS.getMax() - vSliderMS.getMin()) * (evoAmount / 100.0F));
		vSliderMF = vSliderMF + ((ofRandom(2.0F) - 1.0F) * (vSliderMF.getMax() - vSliderMF.getMin()) * (evoAmount / 100.0F));
		vSliderAR = vSliderAR + ((ofRandom(2.0F) - 1.0F) * (vSliderAR.getMax() - vSliderAR.getMin()) * (evoAmount / 100.0F));
		vSliderAG = vSliderAG + ((ofRandom(2.0F) - 1.0F) * (vSliderAG.getMax() - vSliderAG.getMin()) * (evoAmount / 100.0F));
		vSliderAB = vSliderAB + ((ofRandom(2.0F) - 1.0F) * (vSliderAB.getMax() - vSliderAB.getMin()) * (evoAmount / 100.0F));
		vSliderAW = vSliderAW + ((ofRandom(2.0F) - 1.0F) * (vSliderAW.getMax() - vSliderAW.getMin()) * (evoAmount / 100.0F));
		vSliderAO = vSliderAO + ((ofRandom(2.0F) - 1.0F) * (vSliderAO.getMax() - vSliderAO.getMin()) * (evoAmount / 100.0F));
		vSliderAK = vSliderAK + ((ofRandom(2.0F) - 1.0F) * (vSliderAK.getMax() - vSliderAK.getMin()) * (evoAmount / 100.0F));
		vSliderAC = vSliderAC + ((ofRandom(2.0F) - 1.0F) * (vSliderAC.getMax() - vSliderAC.getMin()) * (evoAmount / 100.0F));
		vSliderAD = vSliderAD + ((ofRandom(2.0F) - 1.0F) * (vSliderAD.getMax() - vSliderAD.getMin()) * (evoAmount / 100.0F));
		vSliderAY = vSliderAY + ((ofRandom(2.0F) - 1.0F) * (vSliderAY.getMax() - vSliderAY.getMin()) * (evoAmount / 100.0F));
		vSliderAP = vSliderAP + ((ofRandom(2.0F) - 1.0F) * (vSliderAP.getMax() - vSliderAP.getMin()) * (evoAmount / 100.0F));
		vSliderAM = vSliderAM + ((ofRandom(2.0F) - 1.0F) * (vSliderAM.getMax() - vSliderAM.getMin()) * (evoAmount / 100.0F));
		vSliderAA = vSliderAA + ((ofRandom(2.0F) - 1.0F) * (vSliderAA.getMax() - vSliderAA.getMin()) * (evoAmount / 100.0F));
		vSliderAT = vSliderAT + ((ofRandom(2.0F) - 1.0F) * (vSliderAT.getMax() - vSliderAT.getMin()) * (evoAmount / 100.0F));
		vSliderAS = vSliderAS + ((ofRandom(2.0F) - 1.0F) * (vSliderAS.getMax() - vSliderAS.getMin()) * (evoAmount / 100.0F));
		vSliderAF = vSliderAF + ((ofRandom(2.0F) - 1.0F) * (vSliderAF.getMax() - vSliderAF.getMin()) * (evoAmount / 100.0F));
		vSliderTR = vSliderTR + ((ofRandom(2.0F) - 1.0F) * (vSliderTR.getMax() - vSliderTR.getMin()) * (evoAmount / 100.0F));
		vSliderTG = vSliderTG + ((ofRandom(2.0F) - 1.0F) * (vSliderTG.getMax() - vSliderTG.getMin()) * (evoAmount / 100.0F));
		vSliderTB = vSliderTB + ((ofRandom(2.0F) - 1.0F) * (vSliderTB.getMax() - vSliderTB.getMin()) * (evoAmount / 100.0F));
		vSliderTW = vSliderTW + ((ofRandom(2.0F) - 1.0F) * (vSliderTW.getMax() - vSliderTW.getMin()) * (evoAmount / 100.0F));
		vSliderTO = vSliderTO + ((ofRandom(2.0F) - 1.0F) * (vSliderTO.getMax() - vSliderTO.getMin()) * (evoAmount / 100.0F));
		vSliderTK = vSliderTK + ((ofRandom(2.0F) - 1.0F) * (vSliderTK.getMax() - vSliderTK.getMin()) * (evoAmount / 100.0F));
		vSliderTC = vSliderTC + ((ofRandom(2.0F) - 1.0F) * (vSliderTC.getMax() - vSliderTC.getMin()) * (evoAmount / 100.0F));
		vSliderTD = vSliderTD + ((ofRandom(2.0F) - 1.0F) * (vSliderTD.getMax() - vSliderTD.getMin()) * (evoAmount / 100.0F));
		vSliderTY = vSliderTY + ((ofRandom(2.0F) - 1.0F) * (vSliderTY.getMax() - vSliderTY.getMin()) * (evoAmount / 100.0F));
		vSliderTP = vSliderTP + ((ofRandom(2.0F) - 1.0F) * (vSliderTP.getMax() - vSliderTP.getMin()) * (evoAmount / 100.0F));
		vSliderTM = vSliderTM + ((ofRandom(2.0F) - 1.0F) * (vSliderTM.getMax() - vSliderTM.getMin()) * (evoAmount / 100.0F));
		vSliderTA = vSliderTA + ((ofRandom(2.0F) - 1.0F) * (vSliderTA.getMax() - vSliderTA.getMin()) * (evoAmount / 100.0F));
		vSliderTT = vSliderTT + ((ofRandom(2.0F) - 1.0F) * (vSliderTT.getMax() - vSliderTT.getMin()) * (evoAmount / 100.0F));
		vSliderTS = vSliderTS + ((ofRandom(2.0F) - 1.0F) * (vSliderTS.getMax() - vSliderTS.getMin()) * (evoAmount / 100.0F));
		vSliderTF = vSliderTF + ((ofRandom(2.0F) - 1.0F) * (vSliderTF.getMax() - vSliderTF.getMin()) * (evoAmount / 100.0F));
		vSliderSR = vSliderSR + ((ofRandom(2.0F) - 1.0F) * (vSliderSR.getMax() - vSliderSR.getMin()) * (evoAmount / 100.0F));
		vSliderSG = vSliderSG + ((ofRandom(2.0F) - 1.0F) * (vSliderSG.getMax() - vSliderSG.getMin()) * (evoAmount / 100.0F));
		vSliderSB = vSliderSB + ((ofRandom(2.0F) - 1.0F) * (vSliderSB.getMax() - vSliderSB.getMin()) * (evoAmount / 100.0F));
		vSliderSW = vSliderSW + ((ofRandom(2.0F) - 1.0F) * (vSliderSW.getMax() - vSliderSW.getMin()) * (evoAmount / 100.0F));
		vSliderSO = vSliderSO + ((ofRandom(2.0F) - 1.0F) * (vSliderSO.getMax() - vSliderSO.getMin()) * (evoAmount / 100.0F));
		vSliderSK = vSliderSK + ((ofRandom(2.0F) - 1.0F) * (vSliderSK.getMax() - vSliderSK.getMin()) * (evoAmount / 100.0F));
		vSliderSC = vSliderSC + ((ofRandom(2.0F) - 1.0F) * (vSliderSC.getMax() - vSliderSC.getMin()) * (evoAmount / 100.0F));
		vSliderSD = vSliderSD + ((ofRandom(2.0F) - 1.0F) * (vSliderSD.getMax() - vSliderSD.getMin()) * (evoAmount / 100.0F));
		vSliderSY = vSliderSY + ((ofRandom(2.0F) - 1.0F) * (vSliderSY.getMax() - vSliderSY.getMin()) * (evoAmount / 100.0F));
		vSliderSP = vSliderSP + ((ofRandom(2.0F) - 1.0F) * (vSliderSP.getMax() - vSliderSP.getMin()) * (evoAmount / 100.0F));
		vSliderSM = vSliderSM + ((ofRandom(2.0F) - 1.0F) * (vSliderSM.getMax() - vSliderSM.getMin()) * (evoAmount / 100.0F));
		vSliderSA = vSliderSA + ((ofRandom(2.0F) - 1.0F) * (vSliderSA.getMax() - vSliderSA.getMin()) * (evoAmount / 100.0F));
		vSliderST = vSliderST + ((ofRandom(2.0F) - 1.0F) * (vSliderST.getMax() - vSliderST.getMin()) * (evoAmount / 100.0F));
		vSliderSS = vSliderSS + ((ofRandom(2.0F) - 1.0F) * (vSliderSS.getMax() - vSliderSS.getMin()) * (evoAmount / 100.0F));
		vSliderSF = vSliderSF + ((ofRandom(2.0F) - 1.0F) * (vSliderSF.getMax() - vSliderSF.getMin()) * (evoAmount / 100.0F));
		vSliderFR = vSliderFR + ((ofRandom(2.0F) - 1.0F) * (vSliderFR.getMax() - vSliderFR.getMin()) * (evoAmount / 100.0F));
		vSliderFG = vSliderFG + ((ofRandom(2.0F) - 1.0F) * (vSliderFG.getMax() - vSliderFG.getMin()) * (evoAmount / 100.0F));
		vSliderFB = vSliderFB + ((ofRandom(2.0F) - 1.0F) * (vSliderFB.getMax() - vSliderFB.getMin()) * (evoAmount / 100.0F));
		vSliderFW = vSliderFW + ((ofRandom(2.0F) - 1.0F) * (vSliderFW.getMax() - vSliderFW.getMin()) * (evoAmount / 100.0F));
		vSliderFO = vSliderFO + ((ofRandom(2.0F) - 1.0F) * (vSliderFO.getMax() - vSliderFO.getMin()) * (evoAmount / 100.0F));
		vSliderFK = vSliderFK + ((ofRandom(2.0F) - 1.0F) * (vSliderFK.getMax() - vSliderFK.getMin()) * (evoAmount / 100.0F));
		vSliderFC = vSliderFC + ((ofRandom(2.0F) - 1.0F) * (vSliderFC.getMax() - vSliderFC.getMin()) * (evoAmount / 100.0F));
		vSliderFD = vSliderFD + ((ofRandom(2.0F) - 1.0F) * (vSliderFD.getMax() - vSliderFD.getMin()) * (evoAmount / 100.0F));
		vSliderFY = vSliderFY + ((ofRandom(2.0F) - 1.0F) * (vSliderFY.getMax() - vSliderFY.getMin()) * (evoAmount / 100.0F));
		vSliderFP = vSliderFP + ((ofRandom(2.0F) - 1.0F) * (vSliderFP.getMax() - vSliderFP.getMin()) * (evoAmount / 100.0F));
		vSliderFM = vSliderFM + ((ofRandom(2.0F) - 1.0F) * (vSliderFM.getMax() - vSliderFM.getMin()) * (evoAmount / 100.0F));
		vSliderFA = vSliderFA + ((ofRandom(2.0F) - 1.0F) * (vSliderFA.getMax() - vSliderFA.getMin()) * (evoAmount / 100.0F));
		vSliderFT = vSliderFT + ((ofRandom(2.0F) - 1.0F) * (vSliderFT.getMax() - vSliderFT.getMin()) * (evoAmount / 100.0F));
		vSliderFS = vSliderFS + ((ofRandom(2.0F) - 1.0F) * (vSliderFS.getMax() - vSliderFS.getMin()) * (evoAmount / 100.0F));
		vSliderFF = vSliderFF + ((ofRandom(2.0F) - 1.0F) * (vSliderFF.getMax() - vSliderFF.getMin()) * (evoAmount / 100.0F));
		
		viscositySlider = viscositySlider + ((ofRandom(2.0F) - 1.0F) * (viscositySlider.getMax() - viscositySlider.getMin()) * (evoAmount / 100.0F));
		probabilitySlider = probabilitySlider + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider.getMax() - probabilitySlider.getMin()) * (evoAmount / 100.0F));

		viscositySliderRR = viscositySliderRR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRR.getMax() - viscositySliderRR.getMin()) * (evoAmount / 100.0F));
		viscositySliderRG = viscositySliderRG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRG.getMax() - viscositySliderRG.getMin()) * (evoAmount / 100.0F));
		viscositySliderRB = viscositySliderRB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRB.getMax() - viscositySliderRB.getMin()) * (evoAmount / 100.0F));
		viscositySliderRW = viscositySliderRW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRW.getMax() - viscositySliderRW.getMin()) * (evoAmount / 100.0F));
		viscositySliderRO = viscositySliderRO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRO.getMax() - viscositySliderRO.getMin()) * (evoAmount / 100.0F));
		viscositySliderRK = viscositySliderRK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRK.getMax() - viscositySliderRK.getMin()) * (evoAmount / 100.0F));
		viscositySliderRC = viscositySliderRC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRC.getMax() - viscositySliderRC.getMin()) * (evoAmount / 100.0F));
		viscositySliderRD = viscositySliderRD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRD.getMax() - viscositySliderRD.getMin()) * (evoAmount / 100.0F));
		viscositySliderRY = viscositySliderRY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRY.getMax() - viscositySliderRY.getMin()) * (evoAmount / 100.0F));
		viscositySliderRP = viscositySliderRP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRP.getMax() - viscositySliderRP.getMin()) * (evoAmount / 100.0F));
		viscositySliderRM = viscositySliderRM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRM.getMax() - viscositySliderRM.getMin()) * (evoAmount / 100.0F));
		viscositySliderRA = viscositySliderRA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRA.getMax() - viscositySliderRA.getMin()) * (evoAmount / 100.0F));
		viscositySliderRT = viscositySliderRT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRT.getMax() - viscositySliderRT.getMin()) * (evoAmount / 100.0F));
		viscositySliderRS = viscositySliderRS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRS.getMax() - viscositySliderRS.getMin()) * (evoAmount / 100.0F));
		viscositySliderRF = viscositySliderRF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderRF.getMax() - viscositySliderRF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderRR = probabilitySliderRR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRR.getMax() - probabilitySliderRR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRG = probabilitySliderRG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRG.getMax() - probabilitySliderRG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRB = probabilitySliderRB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRB.getMax() - probabilitySliderRB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRW = probabilitySliderRW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRW.getMax() - probabilitySliderRW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRO = probabilitySliderRO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRO.getMax() - probabilitySliderRO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRK = probabilitySliderRK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRK.getMax() - probabilitySliderRK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRC = probabilitySliderRC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRC.getMax() - probabilitySliderRC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRD = probabilitySliderRD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRD.getMax() - probabilitySliderRD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRY = probabilitySliderRY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRY.getMax() - probabilitySliderRY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRP = probabilitySliderRP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRP.getMax() - probabilitySliderRP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRM = probabilitySliderRM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRM.getMax() - probabilitySliderRM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRA = probabilitySliderRA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRA.getMax() - probabilitySliderRA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRT = probabilitySliderRT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRT.getMax() - probabilitySliderRT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRS = probabilitySliderRS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRS.getMax() - probabilitySliderRS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderRF = probabilitySliderRF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderRF.getMax() - probabilitySliderRF.getMin()) * (evoAmount / 100.0F));

		viscositySliderGR = viscositySliderGR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGR.getMax() - viscositySliderGR.getMin()) * (evoAmount / 100.0F));
		viscositySliderGG = viscositySliderGG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGG.getMax() - viscositySliderGG.getMin()) * (evoAmount / 100.0F));
		viscositySliderGB = viscositySliderGB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGB.getMax() - viscositySliderGB.getMin()) * (evoAmount / 100.0F));
		viscositySliderGW = viscositySliderGW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGW.getMax() - viscositySliderGW.getMin()) * (evoAmount / 100.0F));
		viscositySliderGO = viscositySliderGO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGO.getMax() - viscositySliderGO.getMin()) * (evoAmount / 100.0F));
		viscositySliderGK = viscositySliderGK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGK.getMax() - viscositySliderGK.getMin()) * (evoAmount / 100.0F));
		viscositySliderGC = viscositySliderGC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGC.getMax() - viscositySliderGC.getMin()) * (evoAmount / 100.0F));
		viscositySliderGD = viscositySliderGD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGD.getMax() - viscositySliderGD.getMin()) * (evoAmount / 100.0F));
		viscositySliderGY = viscositySliderGY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGY.getMax() - viscositySliderGY.getMin()) * (evoAmount / 100.0F));
		viscositySliderGP = viscositySliderGP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGP.getMax() - viscositySliderGP.getMin()) * (evoAmount / 100.0F));
		viscositySliderGM = viscositySliderGM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGM.getMax() - viscositySliderGM.getMin()) * (evoAmount / 100.0F));
		viscositySliderGA = viscositySliderGA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGA.getMax() - viscositySliderGA.getMin()) * (evoAmount / 100.0F));
		viscositySliderGT = viscositySliderGT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGT.getMax() - viscositySliderGT.getMin()) * (evoAmount / 100.0F));
		viscositySliderGS = viscositySliderGS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGS.getMax() - viscositySliderGS.getMin()) * (evoAmount / 100.0F));
		viscositySliderGF = viscositySliderGF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderGF.getMax() - viscositySliderGF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderGR = probabilitySliderGR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGR.getMax() - probabilitySliderGR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGG = probabilitySliderGG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGG.getMax() - probabilitySliderGG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGB = probabilitySliderGB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGB.getMax() - probabilitySliderGB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGW = probabilitySliderGW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGW.getMax() - probabilitySliderGW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGO = probabilitySliderGO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGO.getMax() - probabilitySliderGO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGK = probabilitySliderGK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGK.getMax() - probabilitySliderGK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGC = probabilitySliderGC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGC.getMax() - probabilitySliderGC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGD = probabilitySliderGD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGD.getMax() - probabilitySliderGD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGY = probabilitySliderGY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGY.getMax() - probabilitySliderGY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGP = probabilitySliderGP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGP.getMax() - probabilitySliderGP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGM = probabilitySliderGM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGM.getMax() - probabilitySliderGM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGA = probabilitySliderGA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGA.getMax() - probabilitySliderGA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGT = probabilitySliderGT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGT.getMax() - probabilitySliderGT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGS = probabilitySliderGS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGS.getMax() - probabilitySliderGS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderGF = probabilitySliderGF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderGF.getMax() - probabilitySliderGF.getMin()) * (evoAmount / 100.0F));

		viscositySliderBR = viscositySliderBR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBR.getMax() - viscositySliderBR.getMin()) * (evoAmount / 100.0F));
		viscositySliderBG = viscositySliderBG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBG.getMax() - viscositySliderBG.getMin()) * (evoAmount / 100.0F));
		viscositySliderBB = viscositySliderBB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBB.getMax() - viscositySliderBB.getMin()) * (evoAmount / 100.0F));
		viscositySliderBW = viscositySliderBW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBW.getMax() - viscositySliderBW.getMin()) * (evoAmount / 100.0F));
		viscositySliderBO = viscositySliderBO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBO.getMax() - viscositySliderBO.getMin()) * (evoAmount / 100.0F));
		viscositySliderBK = viscositySliderBK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBK.getMax() - viscositySliderBK.getMin()) * (evoAmount / 100.0F));
		viscositySliderBC = viscositySliderBC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBC.getMax() - viscositySliderBC.getMin()) * (evoAmount / 100.0F));
		viscositySliderBD = viscositySliderBD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBD.getMax() - viscositySliderBD.getMin()) * (evoAmount / 100.0F));
		viscositySliderBY = viscositySliderBY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBY.getMax() - viscositySliderBY.getMin()) * (evoAmount / 100.0F));
		viscositySliderBP = viscositySliderBP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBP.getMax() - viscositySliderBP.getMin()) * (evoAmount / 100.0F));
		viscositySliderBM = viscositySliderBM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBM.getMax() - viscositySliderBM.getMin()) * (evoAmount / 100.0F));
		viscositySliderBA = viscositySliderBA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBA.getMax() - viscositySliderBA.getMin()) * (evoAmount / 100.0F));
		viscositySliderBT = viscositySliderBT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBT.getMax() - viscositySliderBT.getMin()) * (evoAmount / 100.0F));
		viscositySliderBS = viscositySliderBS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBS.getMax() - viscositySliderBS.getMin()) * (evoAmount / 100.0F));
		viscositySliderBF = viscositySliderBF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderBF.getMax() - viscositySliderBF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderBR = probabilitySliderBR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBR.getMax() - probabilitySliderBR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBG = probabilitySliderBG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBG.getMax() - probabilitySliderBG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBB = probabilitySliderBB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBB.getMax() - probabilitySliderBB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBW = probabilitySliderBW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBW.getMax() - probabilitySliderBW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBO = probabilitySliderBO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBO.getMax() - probabilitySliderBO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBK = probabilitySliderBK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBK.getMax() - probabilitySliderBK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBC = probabilitySliderBC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBC.getMax() - probabilitySliderBC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBD = probabilitySliderBD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBD.getMax() - probabilitySliderBD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBY = probabilitySliderBY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBY.getMax() - probabilitySliderBY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBP = probabilitySliderBP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBP.getMax() - probabilitySliderBP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBM = probabilitySliderBM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBM.getMax() - probabilitySliderBM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBA = probabilitySliderBA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBA.getMax() - probabilitySliderBA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBT = probabilitySliderBT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBT.getMax() - probabilitySliderBT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBS = probabilitySliderBS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBS.getMax() - probabilitySliderBS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderBF = probabilitySliderBF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderBF.getMax() - probabilitySliderBF.getMin()) * (evoAmount / 100.0F));

		viscositySliderWR = viscositySliderWR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWR.getMax() - viscositySliderWR.getMin()) * (evoAmount / 100.0F));
		viscositySliderWG = viscositySliderWG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWG.getMax() - viscositySliderWG.getMin()) * (evoAmount / 100.0F));
		viscositySliderWB = viscositySliderWB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWB.getMax() - viscositySliderWB.getMin()) * (evoAmount / 100.0F));
		viscositySliderWW = viscositySliderWW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWW.getMax() - viscositySliderWW.getMin()) * (evoAmount / 100.0F));
		viscositySliderWO = viscositySliderWO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWO.getMax() - viscositySliderWO.getMin()) * (evoAmount / 100.0F));
		viscositySliderWK = viscositySliderWK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWK.getMax() - viscositySliderWK.getMin()) * (evoAmount / 100.0F));
		viscositySliderWC = viscositySliderWC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWC.getMax() - viscositySliderWC.getMin()) * (evoAmount / 100.0F));
		viscositySliderWD = viscositySliderWD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWD.getMax() - viscositySliderWD.getMin()) * (evoAmount / 100.0F));
		viscositySliderWY = viscositySliderWY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWY.getMax() - viscositySliderWY.getMin()) * (evoAmount / 100.0F));
		viscositySliderWP = viscositySliderWP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWP.getMax() - viscositySliderWP.getMin()) * (evoAmount / 100.0F));
		viscositySliderWM = viscositySliderWM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWM.getMax() - viscositySliderWM.getMin()) * (evoAmount / 100.0F));
		viscositySliderWA = viscositySliderWA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWA.getMax() - viscositySliderWA.getMin()) * (evoAmount / 100.0F));
		viscositySliderWT = viscositySliderWT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWT.getMax() - viscositySliderWT.getMin()) * (evoAmount / 100.0F));
		viscositySliderWS = viscositySliderWS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWS.getMax() - viscositySliderWS.getMin()) * (evoAmount / 100.0F));
		viscositySliderWF = viscositySliderWF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderWF.getMax() - viscositySliderWF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderWR = probabilitySliderWR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWR.getMax() - probabilitySliderWR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWG = probabilitySliderWG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWG.getMax() - probabilitySliderWG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWB = probabilitySliderWB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWB.getMax() - probabilitySliderWB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWW = probabilitySliderWW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWW.getMax() - probabilitySliderWW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWO = probabilitySliderWO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWO.getMax() - probabilitySliderWO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWK = probabilitySliderWK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWK.getMax() - probabilitySliderWK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWC = probabilitySliderWC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWC.getMax() - probabilitySliderWC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWD = probabilitySliderWD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWD.getMax() - probabilitySliderWD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWY = probabilitySliderWY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWY.getMax() - probabilitySliderWY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWP = probabilitySliderWP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWP.getMax() - probabilitySliderWP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWM = probabilitySliderWM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWM.getMax() - probabilitySliderWM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWA = probabilitySliderWA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWA.getMax() - probabilitySliderWA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWT = probabilitySliderWT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWT.getMax() - probabilitySliderWT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWS = probabilitySliderWS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWS.getMax() - probabilitySliderWS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderWF = probabilitySliderWF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderWF.getMax() - probabilitySliderWF.getMin()) * (evoAmount / 100.0F));

		viscositySliderOR = viscositySliderOR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOR.getMax() - viscositySliderOR.getMin()) * (evoAmount / 100.0F));
		viscositySliderOG = viscositySliderOG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOG.getMax() - viscositySliderOG.getMin()) * (evoAmount / 100.0F));
		viscositySliderOB = viscositySliderOB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOB.getMax() - viscositySliderOB.getMin()) * (evoAmount / 100.0F));
		viscositySliderOW = viscositySliderOW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOW.getMax() - viscositySliderOW.getMin()) * (evoAmount / 100.0F));
		viscositySliderOO = viscositySliderOO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOO.getMax() - viscositySliderOO.getMin()) * (evoAmount / 100.0F));
		viscositySliderOK = viscositySliderOK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOK.getMax() - viscositySliderOK.getMin()) * (evoAmount / 100.0F));
		viscositySliderOC = viscositySliderOC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOC.getMax() - viscositySliderOC.getMin()) * (evoAmount / 100.0F));
		viscositySliderOD = viscositySliderOD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOD.getMax() - viscositySliderOD.getMin()) * (evoAmount / 100.0F));
		viscositySliderOY = viscositySliderOY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOY.getMax() - viscositySliderOY.getMin()) * (evoAmount / 100.0F));
		viscositySliderOP = viscositySliderOP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOP.getMax() - viscositySliderOP.getMin()) * (evoAmount / 100.0F));
		viscositySliderOM = viscositySliderOM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOM.getMax() - viscositySliderOM.getMin()) * (evoAmount / 100.0F));
		viscositySliderOA = viscositySliderOA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOA.getMax() - viscositySliderOA.getMin()) * (evoAmount / 100.0F));
		viscositySliderOT = viscositySliderOT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOT.getMax() - viscositySliderOT.getMin()) * (evoAmount / 100.0F));
		viscositySliderOS = viscositySliderOS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOS.getMax() - viscositySliderOS.getMin()) * (evoAmount / 100.0F));
		viscositySliderOF = viscositySliderOF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderOF.getMax() - viscositySliderOF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderOR = probabilitySliderOR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOR.getMax() - probabilitySliderOR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOG = probabilitySliderOG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOG.getMax() - probabilitySliderOG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOB = probabilitySliderOB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOB.getMax() - probabilitySliderOB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOW = probabilitySliderOW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOW.getMax() - probabilitySliderOW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOO = probabilitySliderOO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOO.getMax() - probabilitySliderOO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOK = probabilitySliderOK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOK.getMax() - probabilitySliderOK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOC = probabilitySliderOC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOC.getMax() - probabilitySliderOC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOD = probabilitySliderOD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOD.getMax() - probabilitySliderOD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOY = probabilitySliderOY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOY.getMax() - probabilitySliderOY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOP = probabilitySliderOP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOP.getMax() - probabilitySliderOP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOM = probabilitySliderOM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOM.getMax() - probabilitySliderOM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOA = probabilitySliderOA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOA.getMax() - probabilitySliderOA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOT = probabilitySliderOT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOT.getMax() - probabilitySliderOT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOS = probabilitySliderOS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOS.getMax() - probabilitySliderOS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderOF = probabilitySliderOF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderOF.getMax() - probabilitySliderOF.getMin()) * (evoAmount / 100.0F));

		viscositySliderKR = viscositySliderKR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKR.getMax() - viscositySliderKR.getMin()) * (evoAmount / 100.0F));
		viscositySliderKG = viscositySliderKG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKG.getMax() - viscositySliderKG.getMin()) * (evoAmount / 100.0F));
		viscositySliderKB = viscositySliderKB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKB.getMax() - viscositySliderKB.getMin()) * (evoAmount / 100.0F));
		viscositySliderKW = viscositySliderKW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKW.getMax() - viscositySliderKW.getMin()) * (evoAmount / 100.0F));
		viscositySliderKO = viscositySliderKO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKO.getMax() - viscositySliderKO.getMin()) * (evoAmount / 100.0F));
		viscositySliderKK = viscositySliderKK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKK.getMax() - viscositySliderKK.getMin()) * (evoAmount / 100.0F));
		viscositySliderKC = viscositySliderKC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKC.getMax() - viscositySliderKC.getMin()) * (evoAmount / 100.0F));
		viscositySliderKD = viscositySliderKD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKD.getMax() - viscositySliderKD.getMin()) * (evoAmount / 100.0F));
		viscositySliderKY = viscositySliderKY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKY.getMax() - viscositySliderKY.getMin()) * (evoAmount / 100.0F));
		viscositySliderKP = viscositySliderKP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKP.getMax() - viscositySliderKP.getMin()) * (evoAmount / 100.0F));
		viscositySliderKM = viscositySliderKM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKM.getMax() - viscositySliderKM.getMin()) * (evoAmount / 100.0F));
		viscositySliderKA = viscositySliderKA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKA.getMax() - viscositySliderKA.getMin()) * (evoAmount / 100.0F));
		viscositySliderKT = viscositySliderKT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKT.getMax() - viscositySliderKT.getMin()) * (evoAmount / 100.0F));
		viscositySliderKS = viscositySliderKS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKS.getMax() - viscositySliderKS.getMin()) * (evoAmount / 100.0F));
		viscositySliderKF = viscositySliderKF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderKF.getMax() - viscositySliderKF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderKR = probabilitySliderKR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKR.getMax() - probabilitySliderKR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKG = probabilitySliderKG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKG.getMax() - probabilitySliderKG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKB = probabilitySliderKB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKB.getMax() - probabilitySliderKB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKW = probabilitySliderKW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKW.getMax() - probabilitySliderKW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKO = probabilitySliderKO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKO.getMax() - probabilitySliderKO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKK = probabilitySliderKK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKK.getMax() - probabilitySliderKK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKC = probabilitySliderKC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKC.getMax() - probabilitySliderKC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKD = probabilitySliderKD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKD.getMax() - probabilitySliderKD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKY = probabilitySliderKY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKY.getMax() - probabilitySliderKY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKP = probabilitySliderKP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKP.getMax() - probabilitySliderKP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKM = probabilitySliderKM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKM.getMax() - probabilitySliderKM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKA = probabilitySliderKA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKA.getMax() - probabilitySliderKA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKT = probabilitySliderKT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKT.getMax() - probabilitySliderKT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKS = probabilitySliderKS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKS.getMax() - probabilitySliderKS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderKF = probabilitySliderKF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderKF.getMax() - probabilitySliderKF.getMin()) * (evoAmount / 100.0F));

		viscositySliderCR = viscositySliderCR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCR.getMax() - viscositySliderCR.getMin()) * (evoAmount / 100.0F));
		viscositySliderCG = viscositySliderCG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCG.getMax() - viscositySliderCG.getMin()) * (evoAmount / 100.0F));
		viscositySliderCB = viscositySliderCB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCB.getMax() - viscositySliderCB.getMin()) * (evoAmount / 100.0F));
		viscositySliderCW = viscositySliderCW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCW.getMax() - viscositySliderCW.getMin()) * (evoAmount / 100.0F));
		viscositySliderCO = viscositySliderCO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCO.getMax() - viscositySliderCO.getMin()) * (evoAmount / 100.0F));
		viscositySliderCK = viscositySliderCK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCK.getMax() - viscositySliderCK.getMin()) * (evoAmount / 100.0F));
		viscositySliderCC = viscositySliderCC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCC.getMax() - viscositySliderCC.getMin()) * (evoAmount / 100.0F));
		viscositySliderCD = viscositySliderCD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCD.getMax() - viscositySliderCD.getMin()) * (evoAmount / 100.0F));
		viscositySliderCY = viscositySliderCY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCY.getMax() - viscositySliderCY.getMin()) * (evoAmount / 100.0F));
		viscositySliderCP = viscositySliderCP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCP.getMax() - viscositySliderCP.getMin()) * (evoAmount / 100.0F));
		viscositySliderCM = viscositySliderCM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCM.getMax() - viscositySliderCM.getMin()) * (evoAmount / 100.0F));
		viscositySliderCA = viscositySliderCA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCA.getMax() - viscositySliderCA.getMin()) * (evoAmount / 100.0F));
		viscositySliderCT = viscositySliderCT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCT.getMax() - viscositySliderCT.getMin()) * (evoAmount / 100.0F));
		viscositySliderCS = viscositySliderCS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCS.getMax() - viscositySliderCS.getMin()) * (evoAmount / 100.0F));
		viscositySliderCF = viscositySliderCF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderCF.getMax() - viscositySliderCF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderCR = probabilitySliderCR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCR.getMax() - probabilitySliderCR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCG = probabilitySliderCG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCG.getMax() - probabilitySliderCG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCB = probabilitySliderCB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCB.getMax() - probabilitySliderCB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCW = probabilitySliderCW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCW.getMax() - probabilitySliderCW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCO = probabilitySliderCO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCO.getMax() - probabilitySliderCO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCK = probabilitySliderCK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCK.getMax() - probabilitySliderCK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCC = probabilitySliderCC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCC.getMax() - probabilitySliderCC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCD = probabilitySliderCD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCD.getMax() - probabilitySliderCD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCY = probabilitySliderCY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCY.getMax() - probabilitySliderCY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCP = probabilitySliderCP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCP.getMax() - probabilitySliderCP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCM = probabilitySliderCM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCM.getMax() - probabilitySliderCM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCA = probabilitySliderCA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCA.getMax() - probabilitySliderCA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCT = probabilitySliderCT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCT.getMax() - probabilitySliderCT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCS = probabilitySliderCS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCS.getMax() - probabilitySliderCS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderCF = probabilitySliderCF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderCF.getMax() - probabilitySliderCF.getMin()) * (evoAmount / 100.0F));

		viscositySliderDR = viscositySliderDR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDR.getMax() - viscositySliderDR.getMin()) * (evoAmount / 100.0F));
		viscositySliderDG = viscositySliderDG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDG.getMax() - viscositySliderDG.getMin()) * (evoAmount / 100.0F));
		viscositySliderDB = viscositySliderDB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDB.getMax() - viscositySliderDB.getMin()) * (evoAmount / 100.0F));
		viscositySliderDW = viscositySliderDW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDW.getMax() - viscositySliderDW.getMin()) * (evoAmount / 100.0F));
		viscositySliderDO = viscositySliderDO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDO.getMax() - viscositySliderDO.getMin()) * (evoAmount / 100.0F));
		viscositySliderDK = viscositySliderDK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDK.getMax() - viscositySliderDK.getMin()) * (evoAmount / 100.0F));
		viscositySliderDC = viscositySliderDC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDC.getMax() - viscositySliderDC.getMin()) * (evoAmount / 100.0F));
		viscositySliderDD = viscositySliderDD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDD.getMax() - viscositySliderDD.getMin()) * (evoAmount / 100.0F));
		viscositySliderDY = viscositySliderDY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDY.getMax() - viscositySliderDY.getMin()) * (evoAmount / 100.0F));
		viscositySliderDP = viscositySliderDP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDP.getMax() - viscositySliderDP.getMin()) * (evoAmount / 100.0F));
		viscositySliderDM = viscositySliderDM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDM.getMax() - viscositySliderDM.getMin()) * (evoAmount / 100.0F));
		viscositySliderDA = viscositySliderDA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDA.getMax() - viscositySliderDA.getMin()) * (evoAmount / 100.0F));
		viscositySliderDT = viscositySliderDT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDT.getMax() - viscositySliderDT.getMin()) * (evoAmount / 100.0F));
		viscositySliderDS = viscositySliderDS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDS.getMax() - viscositySliderDS.getMin()) * (evoAmount / 100.0F));
		viscositySliderDF = viscositySliderDF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderDF.getMax() - viscositySliderDF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderDR = probabilitySliderDR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDR.getMax() - probabilitySliderDR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDG = probabilitySliderDG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDG.getMax() - probabilitySliderDG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDB = probabilitySliderDB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDB.getMax() - probabilitySliderDB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDW = probabilitySliderDW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDW.getMax() - probabilitySliderDW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDO = probabilitySliderDO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDO.getMax() - probabilitySliderDO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDK = probabilitySliderDK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDK.getMax() - probabilitySliderDK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDC = probabilitySliderDC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDC.getMax() - probabilitySliderDC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDD = probabilitySliderDD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDD.getMax() - probabilitySliderDD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDY = probabilitySliderDY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDY.getMax() - probabilitySliderDY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDP = probabilitySliderDP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDP.getMax() - probabilitySliderDP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDM = probabilitySliderDM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDM.getMax() - probabilitySliderDM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDA = probabilitySliderDA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDA.getMax() - probabilitySliderDA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDT = probabilitySliderDT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDT.getMax() - probabilitySliderDT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDS = probabilitySliderDS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDS.getMax() - probabilitySliderDS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderDF = probabilitySliderDF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderDF.getMax() - probabilitySliderDF.getMin()) * (evoAmount / 100.0F));

		viscositySliderYR = viscositySliderYR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYR.getMax() - viscositySliderYR.getMin()) * (evoAmount / 100.0F));
		viscositySliderYG = viscositySliderYG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYG.getMax() - viscositySliderYG.getMin()) * (evoAmount / 100.0F));
		viscositySliderYB = viscositySliderYB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYB.getMax() - viscositySliderYB.getMin()) * (evoAmount / 100.0F));
		viscositySliderYW = viscositySliderYW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYW.getMax() - viscositySliderYW.getMin()) * (evoAmount / 100.0F));
		viscositySliderYO = viscositySliderYO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYO.getMax() - viscositySliderYO.getMin()) * (evoAmount / 100.0F));
		viscositySliderYK = viscositySliderYK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYK.getMax() - viscositySliderYK.getMin()) * (evoAmount / 100.0F));
		viscositySliderYC = viscositySliderYC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYC.getMax() - viscositySliderYC.getMin()) * (evoAmount / 100.0F));
		viscositySliderYD = viscositySliderYD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYD.getMax() - viscositySliderYD.getMin()) * (evoAmount / 100.0F));
		viscositySliderYY = viscositySliderYY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYY.getMax() - viscositySliderYY.getMin()) * (evoAmount / 100.0F));
		viscositySliderYP = viscositySliderYP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYP.getMax() - viscositySliderYP.getMin()) * (evoAmount / 100.0F));
		viscositySliderYM = viscositySliderYM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYM.getMax() - viscositySliderYM.getMin()) * (evoAmount / 100.0F));
		viscositySliderYA = viscositySliderYA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYA.getMax() - viscositySliderYA.getMin()) * (evoAmount / 100.0F));
		viscositySliderYT = viscositySliderYT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYT.getMax() - viscositySliderYT.getMin()) * (evoAmount / 100.0F));
		viscositySliderYS = viscositySliderYS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYS.getMax() - viscositySliderYS.getMin()) * (evoAmount / 100.0F));
		viscositySliderYF = viscositySliderYF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderYF.getMax() - viscositySliderYF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderYR = probabilitySliderYR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYR.getMax() - probabilitySliderYR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYG = probabilitySliderYG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYG.getMax() - probabilitySliderYG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYB = probabilitySliderYB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYB.getMax() - probabilitySliderYB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYW = probabilitySliderYW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYW.getMax() - probabilitySliderYW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYO = probabilitySliderYO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYO.getMax() - probabilitySliderYO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYK = probabilitySliderYK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYK.getMax() - probabilitySliderYK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYC = probabilitySliderYC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYC.getMax() - probabilitySliderYC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYD = probabilitySliderYD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYD.getMax() - probabilitySliderYD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYY = probabilitySliderYY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYY.getMax() - probabilitySliderYY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYP = probabilitySliderYP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYP.getMax() - probabilitySliderYP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYM = probabilitySliderYM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYM.getMax() - probabilitySliderYM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYA = probabilitySliderYA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYA.getMax() - probabilitySliderYA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYT = probabilitySliderYT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYT.getMax() - probabilitySliderYT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYS = probabilitySliderYS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYS.getMax() - probabilitySliderYS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderYF = probabilitySliderYF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderYF.getMax() - probabilitySliderYF.getMin()) * (evoAmount / 100.0F));

		viscositySliderPR = viscositySliderPR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPR.getMax() - viscositySliderPR.getMin()) * (evoAmount / 100.0F));
		viscositySliderPG = viscositySliderPG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPG.getMax() - viscositySliderPG.getMin()) * (evoAmount / 100.0F));
		viscositySliderPB = viscositySliderPB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPB.getMax() - viscositySliderPB.getMin()) * (evoAmount / 100.0F));
		viscositySliderPW = viscositySliderPW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPW.getMax() - viscositySliderPW.getMin()) * (evoAmount / 100.0F));
		viscositySliderPO = viscositySliderPO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPO.getMax() - viscositySliderPO.getMin()) * (evoAmount / 100.0F));
		viscositySliderPK = viscositySliderPK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPK.getMax() - viscositySliderPK.getMin()) * (evoAmount / 100.0F));
		viscositySliderPC = viscositySliderPC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPC.getMax() - viscositySliderPC.getMin()) * (evoAmount / 100.0F));
		viscositySliderPD = viscositySliderPD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPD.getMax() - viscositySliderPD.getMin()) * (evoAmount / 100.0F));
		viscositySliderPY = viscositySliderPY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPY.getMax() - viscositySliderPY.getMin()) * (evoAmount / 100.0F));
		viscositySliderPP = viscositySliderPP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPP.getMax() - viscositySliderPP.getMin()) * (evoAmount / 100.0F));
		viscositySliderPM = viscositySliderPM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPM.getMax() - viscositySliderPM.getMin()) * (evoAmount / 100.0F));
		viscositySliderPA = viscositySliderPA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPA.getMax() - viscositySliderPA.getMin()) * (evoAmount / 100.0F));
		viscositySliderPT = viscositySliderPT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPT.getMax() - viscositySliderPT.getMin()) * (evoAmount / 100.0F));
		viscositySliderPS = viscositySliderPS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPS.getMax() - viscositySliderPS.getMin()) * (evoAmount / 100.0F));
		viscositySliderPF = viscositySliderPF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderPF.getMax() - viscositySliderPF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderPR = probabilitySliderPR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPR.getMax() - probabilitySliderPR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPG = probabilitySliderPG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPG.getMax() - probabilitySliderPG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPB = probabilitySliderPB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPB.getMax() - probabilitySliderPB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPW = probabilitySliderPW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPW.getMax() - probabilitySliderPW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPO = probabilitySliderPO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPO.getMax() - probabilitySliderPO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPK = probabilitySliderPK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPK.getMax() - probabilitySliderPK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPC = probabilitySliderPC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPC.getMax() - probabilitySliderPC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPD = probabilitySliderPD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPD.getMax() - probabilitySliderPD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPY = probabilitySliderPY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPY.getMax() - probabilitySliderPY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPP = probabilitySliderPP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPP.getMax() - probabilitySliderPP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPM = probabilitySliderPM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPM.getMax() - probabilitySliderPM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPA = probabilitySliderPA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPA.getMax() - probabilitySliderPA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPT = probabilitySliderPT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPT.getMax() - probabilitySliderPT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPS = probabilitySliderPS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPS.getMax() - probabilitySliderPS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderPF = probabilitySliderPF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderPF.getMax() - probabilitySliderPF.getMin()) * (evoAmount / 100.0F));

		viscositySliderMR = viscositySliderMR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMR.getMax() - viscositySliderMR.getMin()) * (evoAmount / 100.0F));
		viscositySliderMG = viscositySliderMG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMG.getMax() - viscositySliderMG.getMin()) * (evoAmount / 100.0F));
		viscositySliderMB = viscositySliderMB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMB.getMax() - viscositySliderMB.getMin()) * (evoAmount / 100.0F));
		viscositySliderMW = viscositySliderMW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMW.getMax() - viscositySliderMW.getMin()) * (evoAmount / 100.0F));
		viscositySliderMO = viscositySliderMO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMO.getMax() - viscositySliderMO.getMin()) * (evoAmount / 100.0F));
		viscositySliderMK = viscositySliderMK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMK.getMax() - viscositySliderMK.getMin()) * (evoAmount / 100.0F));
		viscositySliderMC = viscositySliderMC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMC.getMax() - viscositySliderMC.getMin()) * (evoAmount / 100.0F));
		viscositySliderMD = viscositySliderMD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMD.getMax() - viscositySliderMD.getMin()) * (evoAmount / 100.0F));
		viscositySliderMY = viscositySliderMY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMY.getMax() - viscositySliderMY.getMin()) * (evoAmount / 100.0F));
		viscositySliderMP = viscositySliderMP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMP.getMax() - viscositySliderMP.getMin()) * (evoAmount / 100.0F));
		viscositySliderMM = viscositySliderMM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMM.getMax() - viscositySliderMM.getMin()) * (evoAmount / 100.0F));
		viscositySliderMA = viscositySliderMA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMA.getMax() - viscositySliderMA.getMin()) * (evoAmount / 100.0F));
		viscositySliderMT = viscositySliderMT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMT.getMax() - viscositySliderMT.getMin()) * (evoAmount / 100.0F));
		viscositySliderMS = viscositySliderMS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMS.getMax() - viscositySliderMS.getMin()) * (evoAmount / 100.0F));
		viscositySliderMF = viscositySliderMF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderMF.getMax() - viscositySliderMF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderMR = probabilitySliderMR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMR.getMax() - probabilitySliderMR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMG = probabilitySliderMG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMG.getMax() - probabilitySliderMG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMB = probabilitySliderMB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMB.getMax() - probabilitySliderMB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMW = probabilitySliderMW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMW.getMax() - probabilitySliderMW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMO = probabilitySliderMO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMO.getMax() - probabilitySliderMO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMK = probabilitySliderMK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMK.getMax() - probabilitySliderMK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMC = probabilitySliderMC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMC.getMax() - probabilitySliderMC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMD = probabilitySliderMD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMD.getMax() - probabilitySliderMD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMY = probabilitySliderMY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMY.getMax() - probabilitySliderMY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMP = probabilitySliderMP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMP.getMax() - probabilitySliderMP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMM = probabilitySliderMM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMM.getMax() - probabilitySliderMM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMA = probabilitySliderMA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMA.getMax() - probabilitySliderMA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMT = probabilitySliderMT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMT.getMax() - probabilitySliderMT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMS = probabilitySliderMS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMS.getMax() - probabilitySliderMS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderMF = probabilitySliderMF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderMF.getMax() - probabilitySliderMF.getMin()) * (evoAmount / 100.0F));

		viscositySliderAR = viscositySliderAR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAR.getMax() - viscositySliderAR.getMin()) * (evoAmount / 100.0F));
		viscositySliderAG = viscositySliderAG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAG.getMax() - viscositySliderAG.getMin()) * (evoAmount / 100.0F));
		viscositySliderAB = viscositySliderAB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAB.getMax() - viscositySliderAB.getMin()) * (evoAmount / 100.0F));
		viscositySliderAW = viscositySliderAW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAW.getMax() - viscositySliderAW.getMin()) * (evoAmount / 100.0F));
		viscositySliderAO = viscositySliderAO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAO.getMax() - viscositySliderAO.getMin()) * (evoAmount / 100.0F));
		viscositySliderAK = viscositySliderAK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAK.getMax() - viscositySliderAK.getMin()) * (evoAmount / 100.0F));
		viscositySliderAC = viscositySliderAC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAC.getMax() - viscositySliderAC.getMin()) * (evoAmount / 100.0F));
		viscositySliderAD = viscositySliderAD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAD.getMax() - viscositySliderAD.getMin()) * (evoAmount / 100.0F));
		viscositySliderAY = viscositySliderAY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAY.getMax() - viscositySliderAY.getMin()) * (evoAmount / 100.0F));
		viscositySliderAP = viscositySliderAP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAP.getMax() - viscositySliderAP.getMin()) * (evoAmount / 100.0F));
		viscositySliderAM = viscositySliderAM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAM.getMax() - viscositySliderAM.getMin()) * (evoAmount / 100.0F));
		viscositySliderAA = viscositySliderAA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAA.getMax() - viscositySliderAA.getMin()) * (evoAmount / 100.0F));
		viscositySliderAT = viscositySliderAT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAT.getMax() - viscositySliderAT.getMin()) * (evoAmount / 100.0F));
		viscositySliderAS = viscositySliderAS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAS.getMax() - viscositySliderAS.getMin()) * (evoAmount / 100.0F));
		viscositySliderAF = viscositySliderAF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderAF.getMax() - viscositySliderAF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderAR = probabilitySliderAR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAR.getMax() - probabilitySliderAR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAG = probabilitySliderAG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAG.getMax() - probabilitySliderAG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAB = probabilitySliderAB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAB.getMax() - probabilitySliderAB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAW = probabilitySliderAW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAW.getMax() - probabilitySliderAW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAO = probabilitySliderAO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAO.getMax() - probabilitySliderAO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAK = probabilitySliderAK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAK.getMax() - probabilitySliderAK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAC = probabilitySliderAC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAC.getMax() - probabilitySliderAC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAD = probabilitySliderAD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAD.getMax() - probabilitySliderAD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAY = probabilitySliderAY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAY.getMax() - probabilitySliderAY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAP = probabilitySliderAP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAP.getMax() - probabilitySliderAP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAM = probabilitySliderAM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAM.getMax() - probabilitySliderAM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAA = probabilitySliderAA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAA.getMax() - probabilitySliderAA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAT = probabilitySliderAT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAT.getMax() - probabilitySliderAT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAS = probabilitySliderAS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAS.getMax() - probabilitySliderAS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderAF = probabilitySliderAF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderAF.getMax() - probabilitySliderAF.getMin()) * (evoAmount / 100.0F));

		viscositySliderTR = viscositySliderTR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTR.getMax() - viscositySliderTR.getMin()) * (evoAmount / 100.0F));
		viscositySliderTG = viscositySliderTG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTG.getMax() - viscositySliderTG.getMin()) * (evoAmount / 100.0F));
		viscositySliderTB = viscositySliderTB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTB.getMax() - viscositySliderTB.getMin()) * (evoAmount / 100.0F));
		viscositySliderTW = viscositySliderTW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTW.getMax() - viscositySliderTW.getMin()) * (evoAmount / 100.0F));
		viscositySliderTO = viscositySliderTO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTO.getMax() - viscositySliderTO.getMin()) * (evoAmount / 100.0F));
		viscositySliderTK = viscositySliderTK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTK.getMax() - viscositySliderTK.getMin()) * (evoAmount / 100.0F));
		viscositySliderTC = viscositySliderTC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTC.getMax() - viscositySliderTC.getMin()) * (evoAmount / 100.0F));
		viscositySliderTD = viscositySliderTD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTD.getMax() - viscositySliderTD.getMin()) * (evoAmount / 100.0F));
		viscositySliderTY = viscositySliderTY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTY.getMax() - viscositySliderTY.getMin()) * (evoAmount / 100.0F));
		viscositySliderTP = viscositySliderTP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTP.getMax() - viscositySliderTP.getMin()) * (evoAmount / 100.0F));
		viscositySliderTM = viscositySliderTM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTM.getMax() - viscositySliderTM.getMin()) * (evoAmount / 100.0F));
		viscositySliderTA = viscositySliderTA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTA.getMax() - viscositySliderTA.getMin()) * (evoAmount / 100.0F));
		viscositySliderTT = viscositySliderTT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTT.getMax() - viscositySliderTT.getMin()) * (evoAmount / 100.0F));
		viscositySliderTS = viscositySliderTS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTS.getMax() - viscositySliderTS.getMin()) * (evoAmount / 100.0F));
		viscositySliderTF = viscositySliderTF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderTF.getMax() - viscositySliderTF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderTR = probabilitySliderTR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTR.getMax() - probabilitySliderTR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTG = probabilitySliderTG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTG.getMax() - probabilitySliderTG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTB = probabilitySliderTB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTB.getMax() - probabilitySliderTB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTW = probabilitySliderTW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTW.getMax() - probabilitySliderTW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTO = probabilitySliderTO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTO.getMax() - probabilitySliderTO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTK = probabilitySliderTK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTK.getMax() - probabilitySliderTK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTC = probabilitySliderTC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTC.getMax() - probabilitySliderTC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTD = probabilitySliderTD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTD.getMax() - probabilitySliderTD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTY = probabilitySliderTY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTY.getMax() - probabilitySliderTY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTP = probabilitySliderTP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTP.getMax() - probabilitySliderTP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTM = probabilitySliderTM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTM.getMax() - probabilitySliderTM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTA = probabilitySliderTA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTA.getMax() - probabilitySliderTA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTT = probabilitySliderTT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTT.getMax() - probabilitySliderTT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTS = probabilitySliderTS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTS.getMax() - probabilitySliderTS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderTF = probabilitySliderTF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderTF.getMax() - probabilitySliderTF.getMin()) * (evoAmount / 100.0F));

		viscositySliderSR = viscositySliderSR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSR.getMax() - viscositySliderSR.getMin()) * (evoAmount / 100.0F));
		viscositySliderSG = viscositySliderSG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSG.getMax() - viscositySliderSG.getMin()) * (evoAmount / 100.0F));
		viscositySliderSB = viscositySliderSB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSB.getMax() - viscositySliderSB.getMin()) * (evoAmount / 100.0F));
		viscositySliderSW = viscositySliderSW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSW.getMax() - viscositySliderSW.getMin()) * (evoAmount / 100.0F));
		viscositySliderSO = viscositySliderSO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSO.getMax() - viscositySliderSO.getMin()) * (evoAmount / 100.0F));
		viscositySliderSK = viscositySliderSK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSK.getMax() - viscositySliderSK.getMin()) * (evoAmount / 100.0F));
		viscositySliderSC = viscositySliderSC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSC.getMax() - viscositySliderSC.getMin()) * (evoAmount / 100.0F));
		viscositySliderSD = viscositySliderSD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSD.getMax() - viscositySliderSD.getMin()) * (evoAmount / 100.0F));
		viscositySliderSY = viscositySliderSY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSY.getMax() - viscositySliderSY.getMin()) * (evoAmount / 100.0F));
		viscositySliderSP = viscositySliderSP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSP.getMax() - viscositySliderSP.getMin()) * (evoAmount / 100.0F));
		viscositySliderSM = viscositySliderSM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSM.getMax() - viscositySliderSM.getMin()) * (evoAmount / 100.0F));
		viscositySliderSA = viscositySliderSA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSA.getMax() - viscositySliderSA.getMin()) * (evoAmount / 100.0F));
		viscositySliderST = viscositySliderST + ((ofRandom(2.0F) - 1.0F) * (viscositySliderST.getMax() - viscositySliderST.getMin()) * (evoAmount / 100.0F));
		viscositySliderSS = viscositySliderSS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSS.getMax() - viscositySliderSS.getMin()) * (evoAmount / 100.0F));
		viscositySliderSF = viscositySliderSF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderSF.getMax() - viscositySliderSF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderSR = probabilitySliderSR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSR.getMax() - probabilitySliderSR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSG = probabilitySliderSG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSG.getMax() - probabilitySliderSG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSB = probabilitySliderSB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSB.getMax() - probabilitySliderSB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSW = probabilitySliderSW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSW.getMax() - probabilitySliderSW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSO = probabilitySliderSO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSO.getMax() - probabilitySliderSO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSK = probabilitySliderSK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSK.getMax() - probabilitySliderSK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSC = probabilitySliderSC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSC.getMax() - probabilitySliderSC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSD = probabilitySliderSD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSD.getMax() - probabilitySliderSD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSY = probabilitySliderSY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSY.getMax() - probabilitySliderSY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSP = probabilitySliderSP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSP.getMax() - probabilitySliderSP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSM = probabilitySliderSM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSM.getMax() - probabilitySliderSM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSA = probabilitySliderSA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSA.getMax() - probabilitySliderSA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderST = probabilitySliderST + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderST.getMax() - probabilitySliderST.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSS = probabilitySliderSS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSS.getMax() - probabilitySliderSS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderSF = probabilitySliderSF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderSF.getMax() - probabilitySliderSF.getMin()) * (evoAmount / 100.0F));

		viscositySliderFR = viscositySliderFR + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFR.getMax() - viscositySliderFR.getMin()) * (evoAmount / 100.0F));
		viscositySliderFG = viscositySliderFG + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFG.getMax() - viscositySliderFG.getMin()) * (evoAmount / 100.0F));
		viscositySliderFB = viscositySliderFB + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFB.getMax() - viscositySliderFB.getMin()) * (evoAmount / 100.0F));
		viscositySliderFW = viscositySliderFW + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFW.getMax() - viscositySliderFW.getMin()) * (evoAmount / 100.0F));
		viscositySliderFO = viscositySliderFO + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFO.getMax() - viscositySliderFO.getMin()) * (evoAmount / 100.0F));
		viscositySliderFK = viscositySliderFK + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFK.getMax() - viscositySliderFK.getMin()) * (evoAmount / 100.0F));
		viscositySliderFC = viscositySliderFC + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFC.getMax() - viscositySliderFC.getMin()) * (evoAmount / 100.0F));
		viscositySliderFD = viscositySliderFD + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFD.getMax() - viscositySliderFD.getMin()) * (evoAmount / 100.0F));
		viscositySliderFY = viscositySliderFY + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFY.getMax() - viscositySliderFY.getMin()) * (evoAmount / 100.0F));
		viscositySliderFP = viscositySliderFP + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFP.getMax() - viscositySliderFP.getMin()) * (evoAmount / 100.0F));
		viscositySliderFM = viscositySliderFM + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFM.getMax() - viscositySliderFM.getMin()) * (evoAmount / 100.0F));
		viscositySliderFA = viscositySliderFA + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFA.getMax() - viscositySliderFA.getMin()) * (evoAmount / 100.0F));
		viscositySliderFT = viscositySliderFT + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFT.getMax() - viscositySliderFT.getMin()) * (evoAmount / 100.0F));
		viscositySliderFS = viscositySliderFS + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFS.getMax() - viscositySliderFS.getMin()) * (evoAmount / 100.0F));
		viscositySliderFF = viscositySliderFF + ((ofRandom(2.0F) - 1.0F) * (viscositySliderFF.getMax() - viscositySliderFF.getMin()) * (evoAmount / 100.0F));

		probabilitySliderFR = probabilitySliderFR + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFR.getMax() - probabilitySliderFR.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFG = probabilitySliderFG + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFG.getMax() - probabilitySliderFG.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFB = probabilitySliderFB + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFB.getMax() - probabilitySliderFB.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFW = probabilitySliderFW + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFW.getMax() - probabilitySliderFW.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFO = probabilitySliderFO + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFO.getMax() - probabilitySliderFO.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFK = probabilitySliderFK + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFK.getMax() - probabilitySliderFK.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFC = probabilitySliderFC + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFC.getMax() - probabilitySliderFC.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFD = probabilitySliderFD + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFD.getMax() - probabilitySliderFD.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFY = probabilitySliderFY + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFY.getMax() - probabilitySliderFY.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFP = probabilitySliderFP + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFP.getMax() - probabilitySliderFP.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFM = probabilitySliderFM + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFM.getMax() - probabilitySliderFM.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFA = probabilitySliderFA + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFA.getMax() - probabilitySliderFA.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFT = probabilitySliderFT + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFT.getMax() - probabilitySliderFT.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFS = probabilitySliderFS + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFS.getMax() - probabilitySliderFS.getMin()) * (evoAmount / 100.0F));
		probabilitySliderFF = probabilitySliderFF + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderFF.getMax() - probabilitySliderFF.getMin()) * (evoAmount / 100.0F));

		if (probabilitySlider < 0.0F) probabilitySlider = 0;
		if (viscositySlider < 0.0F) viscositySlider = 0;
		if (probabilitySlider > 100.0F) probabilitySlider = 100.0F;
		if (viscositySlider > 1.0F) viscositySlider = 1.0F;

		if (powerSliderRR < 0.0F) powerSliderRR = 0;
		if (powerSliderRG < 0.0F) powerSliderRG = 0;
		if (powerSliderRB < 0.0F) powerSliderRB = 0;
		if (powerSliderRW < 0.0F) powerSliderRW = 0;
		if (powerSliderRO < 0.0F) powerSliderRO = 0;
		if (powerSliderRK < 0.0F) powerSliderRK = 0;
		if (powerSliderRC < 0.0F) powerSliderRC = 0;
		if (powerSliderRD < 0.0F) powerSliderRD = 0;
		if (powerSliderRY < 0.0F) powerSliderRY = 0;
		if (powerSliderRP < 0.0F) powerSliderRP = 0;
		if (powerSliderRM < 0.0F) powerSliderRM = 0;
		if (powerSliderRA < 0.0F) powerSliderRA = 0;
		if (powerSliderRT < 0.0F) powerSliderRT = 0;
		if (powerSliderRS < 0.0F) powerSliderRS = 0;
		if (powerSliderRF < 0.0F) powerSliderRF = 0;
		if (powerSliderRR > 100.0F) powerSliderRR = 100.0F;
		if (powerSliderRG > 100.0F) powerSliderRG = 100.0F;
		if (powerSliderRB > 100.0F) powerSliderRB = 100.0F;
		if (powerSliderRW > 100.0F) powerSliderRW = 100.0F;
		if (powerSliderRO > 100.0F) powerSliderRO = 100.0F;
		if (powerSliderRK > 100.0F) powerSliderRK = 100.0F;
		if (powerSliderRC > 100.0F) powerSliderRC = 100.0F;
		if (powerSliderRD > 100.0F) powerSliderRD = 100.0F;
		if (powerSliderRY > 100.0F) powerSliderRY = 100.0F;
		if (powerSliderRP > 100.0F) powerSliderRP = 100.0F;
		if (powerSliderRM > 100.0F) powerSliderRM = 100.0F;
		if (powerSliderRA > 100.0F) powerSliderRA = 100.0F;
		if (powerSliderRT > 100.0F) powerSliderRT = 100.0F;
		if (powerSliderRS > 100.0F) powerSliderRS = 100.0F;
		if (powerSliderRF > 100.0F) powerSliderRF = 100.0F;
		if (vSliderRR < 0.0F) vSliderRR = 0;
		if (vSliderRG < 0.0F) vSliderRG = 0;
		if (vSliderRB < 0.0F) vSliderRB = 0;
		if (vSliderRW < 0.0F) vSliderRW = 0;
		if (vSliderRO < 0.0F) vSliderRO = 0;
		if (vSliderRK < 0.0F) vSliderRK = 0;
		if (vSliderRC < 0.0F) vSliderRC = 0;
		if (vSliderRD < 0.0F) vSliderRD = 0;
		if (vSliderRY < 0.0F) vSliderRY = 0;
		if (vSliderRP < 0.0F) vSliderRP = 0;
		if (vSliderRM < 0.0F) vSliderRM = 0;
		if (vSliderRA < 0.0F) vSliderRA = 0;
		if (vSliderRT < 0.0F) vSliderRT = 0;
		if (vSliderRS < 0.0F) vSliderRS = 0;
		if (vSliderRF < 0.0F) vSliderRF = 0;
		if (vSliderRR > 500.0F) vSliderRR = 500.0F;
		if (vSliderRG > 500.0F) vSliderRG = 500.0F;
		if (vSliderRB > 500.0F) vSliderRB = 500.0F;
		if (vSliderRW > 500.0F) vSliderRW = 500.0F;
		if (vSliderRO > 500.0F) vSliderRO = 500.0F;
		if (vSliderRK > 500.0F) vSliderRK = 500.0F;
		if (vSliderRC > 500.0F) vSliderRC = 500.0F;
		if (vSliderRD > 500.0F) vSliderRD = 500.0F;
		if (vSliderRY > 500.0F) vSliderRY = 500.0F;
		if (vSliderRP > 500.0F) vSliderRP = 500.0F;
		if (vSliderRM > 500.0F) vSliderRM = 500.0F;
		if (vSliderRA > 500.0F) vSliderRA = 500.0F;
		if (vSliderRT > 500.0F) vSliderRT = 500.0F;
		if (vSliderRS > 500.0F) vSliderRS = 500.0F;
		if (vSliderRF > 500.0F) vSliderRF = 500.0F;
		if (probabilitySliderRR < 0.0F) probabilitySliderRR = 0;
		if (probabilitySliderRG < 0.0F) probabilitySliderRG = 0;
		if (probabilitySliderRB < 0.0F) probabilitySliderRB = 0;
		if (probabilitySliderRW < 0.0F) probabilitySliderRW = 0;
		if (probabilitySliderRO < 0.0F) probabilitySliderRO = 0;
		if (probabilitySliderRK < 0.0F) probabilitySliderRK = 0;
		if (probabilitySliderRC < 0.0F) probabilitySliderRC = 0;
		if (probabilitySliderRD < 0.0F) probabilitySliderRD = 0;
		if (probabilitySliderRY < 0.0F) probabilitySliderRY = 0;
		if (probabilitySliderRP < 0.0F) probabilitySliderRP = 0;
		if (probabilitySliderRM < 0.0F) probabilitySliderRM = 0;
		if (probabilitySliderRA < 0.0F) probabilitySliderRA = 0;
		if (probabilitySliderRT < 0.0F) probabilitySliderRT = 0;
		if (probabilitySliderRS < 0.0F) probabilitySliderRS = 0;
		if (probabilitySliderRF < 0.0F) probabilitySliderRF = 0;
		if (probabilitySliderRR > 100.0F) probabilitySliderRR = 100.0F;
		if (probabilitySliderRG > 100.0F) probabilitySliderRG = 100.0F;
		if (probabilitySliderRB > 100.0F) probabilitySliderRB = 100.0F;
		if (probabilitySliderRW > 100.0F) probabilitySliderRW = 100.0F;
		if (probabilitySliderRO > 100.0F) probabilitySliderRO = 100.0F;
		if (probabilitySliderRK > 100.0F) probabilitySliderRK = 100.0F;
		if (probabilitySliderRC > 100.0F) probabilitySliderRC = 100.0F;
		if (probabilitySliderRD > 100.0F) probabilitySliderRD = 100.0F;
		if (probabilitySliderRY > 100.0F) probabilitySliderRY = 100.0F;
		if (probabilitySliderRP > 100.0F) probabilitySliderRP = 100.0F;
		if (probabilitySliderRM > 100.0F) probabilitySliderRM = 100.0F;
		if (probabilitySliderRA > 100.0F) probabilitySliderRA = 100.0F;
		if (probabilitySliderRT > 100.0F) probabilitySliderRT = 100.0F;
		if (probabilitySliderRS > 100.0F) probabilitySliderRS = 100.0F;
		if (probabilitySliderRF > 100.0F) probabilitySliderRF = 100.0F;
		if (viscositySliderRR < 0.0F) viscositySliderRR = 0;
		if (viscositySliderRG < 0.0F) viscositySliderRG = 0;
		if (viscositySliderRB < 0.0F) viscositySliderRB = 0;
		if (viscositySliderRW < 0.0F) viscositySliderRW = 0;
		if (viscositySliderRO < 0.0F) viscositySliderRO = 0;
		if (viscositySliderRK < 0.0F) viscositySliderRK = 0;
		if (viscositySliderRC < 0.0F) viscositySliderRC = 0;
		if (viscositySliderRD < 0.0F) viscositySliderRD = 0;
		if (viscositySliderRY < 0.0F) viscositySliderRY = 0;
		if (viscositySliderRP < 0.0F) viscositySliderRP = 0;
		if (viscositySliderRM < 0.0F) viscositySliderRM = 0;
		if (viscositySliderRA < 0.0F) viscositySliderRA = 0;
		if (viscositySliderRT < 0.0F) viscositySliderRT = 0;
		if (viscositySliderRS < 0.0F) viscositySliderRS = 0;
		if (viscositySliderRF < 0.0F) viscositySliderRF = 0;
		if (viscositySliderRR > 1.0F) viscositySliderRR = 1.0F;
		if (viscositySliderRG > 1.0F) viscositySliderRG = 1.0F;
		if (viscositySliderRB > 1.0F) viscositySliderRB = 1.0F;
		if (viscositySliderRW > 1.0F) viscositySliderRW = 1.0F;
		if (viscositySliderRO > 1.0F) viscositySliderRO = 1.0F;
		if (viscositySliderRK > 1.0F) viscositySliderRK = 1.0F;
		if (viscositySliderRC > 1.0F) viscositySliderRC = 1.0F;
		if (viscositySliderRD > 1.0F) viscositySliderRD = 1.0F;
		if (viscositySliderRY > 1.0F) viscositySliderRY = 1.0F;
		if (viscositySliderRP > 1.0F) viscositySliderRP = 1.0F;
		if (viscositySliderRM > 1.0F) viscositySliderRM = 1.0F;
		if (viscositySliderRA > 1.0F) viscositySliderRA = 1.0F;
		if (viscositySliderRT > 1.0F) viscositySliderRT = 1.0F;
		if (viscositySliderRS > 1.0F) viscositySliderRS = 1.0F;
		if (viscositySliderRF > 1.0F) viscositySliderRF = 1.0F;

		if (powerSliderGR < 0.0F) powerSliderGR = 0;
		if (powerSliderGG < 0.0F) powerSliderGG = 0;
		if (powerSliderGB < 0.0F) powerSliderGB = 0;
		if (powerSliderGW < 0.0F) powerSliderGW = 0;
		if (powerSliderGO < 0.0F) powerSliderGO = 0;
		if (powerSliderGK < 0.0F) powerSliderGK = 0;
		if (powerSliderGC < 0.0F) powerSliderGC = 0;
		if (powerSliderGD < 0.0F) powerSliderGD = 0;
		if (powerSliderGY < 0.0F) powerSliderGY = 0;
		if (powerSliderGP < 0.0F) powerSliderGP = 0;
		if (powerSliderGM < 0.0F) powerSliderGM = 0;
		if (powerSliderGA < 0.0F) powerSliderGA = 0;
		if (powerSliderGT < 0.0F) powerSliderGT = 0;
		if (powerSliderGS < 0.0F) powerSliderGS = 0;
		if (powerSliderGF < 0.0F) powerSliderGF = 0;
		if (powerSliderGR > 100.0F) powerSliderGR = 100.0F;
		if (powerSliderGG > 100.0F) powerSliderGG = 100.0F;
		if (powerSliderGB > 100.0F) powerSliderGB = 100.0F;
		if (powerSliderGW > 100.0F) powerSliderGW = 100.0F;
		if (powerSliderGO > 100.0F) powerSliderGO = 100.0F;
		if (powerSliderGK > 100.0F) powerSliderGK = 100.0F;
		if (powerSliderGC > 100.0F) powerSliderGC = 100.0F;
		if (powerSliderGD > 100.0F) powerSliderGD = 100.0F;
		if (powerSliderGY > 100.0F) powerSliderGY = 100.0F;
		if (powerSliderGP > 100.0F) powerSliderGP = 100.0F;
		if (powerSliderGM > 100.0F) powerSliderGM = 100.0F;
		if (powerSliderGA > 100.0F) powerSliderGA = 100.0F;
		if (powerSliderGT > 100.0F) powerSliderGT = 100.0F;
		if (powerSliderGS > 100.0F) powerSliderGS = 100.0F;
		if (powerSliderGF > 100.0F) powerSliderGF = 100.0F;
		if (vSliderGR < 0.0F) vSliderGR = 0;
		if (vSliderGG < 0.0F) vSliderGG = 0;
		if (vSliderGB < 0.0F) vSliderGB = 0;
		if (vSliderGW < 0.0F) vSliderGW = 0;
		if (vSliderGO < 0.0F) vSliderGO = 0;
		if (vSliderGK < 0.0F) vSliderGK = 0;
		if (vSliderGC < 0.0F) vSliderGC = 0;
		if (vSliderGD < 0.0F) vSliderGD = 0;
		if (vSliderGY < 0.0F) vSliderGY = 0;
		if (vSliderGP < 0.0F) vSliderGP = 0;
		if (vSliderGM < 0.0F) vSliderGM = 0;
		if (vSliderGA < 0.0F) vSliderGA = 0;
		if (vSliderGT < 0.0F) vSliderGT = 0;
		if (vSliderGS < 0.0F) vSliderGS = 0;
		if (vSliderGF < 0.0F) vSliderGF = 0;
		if (vSliderGR > 500.0F) vSliderGR = 500.0F;
		if (vSliderGG > 500.0F) vSliderGG = 500.0F;
		if (vSliderGB > 500.0F) vSliderGB = 500.0F;
		if (vSliderGW > 500.0F) vSliderGW = 500.0F;
		if (vSliderGO > 500.0F) vSliderGO = 500.0F;
		if (vSliderGK > 500.0F) vSliderGK = 500.0F;
		if (vSliderGC > 500.0F) vSliderGC = 500.0F;
		if (vSliderGD > 500.0F) vSliderGD = 500.0F;
		if (vSliderGY > 500.0F) vSliderGY = 500.0F;
		if (vSliderGP > 500.0F) vSliderGP = 500.0F;
		if (vSliderGM > 500.0F) vSliderGM = 500.0F;
		if (vSliderGA > 500.0F) vSliderGA = 500.0F;
		if (vSliderGT > 500.0F) vSliderGT = 500.0F;
		if (vSliderGS > 500.0F) vSliderGS = 500.0F;
		if (vSliderGF > 500.0F) vSliderGF = 500.0F;
		if (probabilitySliderGR < 0.0F) probabilitySliderGR = 0;
		if (probabilitySliderGG < 0.0F) probabilitySliderGG = 0;
		if (probabilitySliderGB < 0.0F) probabilitySliderGB = 0;
		if (probabilitySliderGW < 0.0F) probabilitySliderGW = 0;
		if (probabilitySliderGO < 0.0F) probabilitySliderGO = 0;
		if (probabilitySliderGK < 0.0F) probabilitySliderGK = 0;
		if (probabilitySliderGC < 0.0F) probabilitySliderGC = 0;
		if (probabilitySliderGD < 0.0F) probabilitySliderGD = 0;
		if (probabilitySliderGY < 0.0F) probabilitySliderGY = 0;
		if (probabilitySliderGP < 0.0F) probabilitySliderGP = 0;
		if (probabilitySliderGM < 0.0F) probabilitySliderGM = 0;
		if (probabilitySliderGA < 0.0F) probabilitySliderGA = 0;
		if (probabilitySliderGT < 0.0F) probabilitySliderGT = 0;
		if (probabilitySliderGS < 0.0F) probabilitySliderGS = 0;
		if (probabilitySliderGF < 0.0F) probabilitySliderGF = 0;
		if (probabilitySliderGR > 100.0F) probabilitySliderGR = 100.0F;
		if (probabilitySliderGG > 100.0F) probabilitySliderGG = 100.0F;
		if (probabilitySliderGB > 100.0F) probabilitySliderGB = 100.0F;
		if (probabilitySliderGW > 100.0F) probabilitySliderGW = 100.0F;
		if (probabilitySliderGO > 100.0F) probabilitySliderGO = 100.0F;
		if (probabilitySliderGK > 100.0F) probabilitySliderGK = 100.0F;
		if (probabilitySliderGC > 100.0F) probabilitySliderGC = 100.0F;
		if (probabilitySliderGD > 100.0F) probabilitySliderGD = 100.0F;
		if (probabilitySliderGY > 100.0F) probabilitySliderGY = 100.0F;
		if (probabilitySliderGP > 100.0F) probabilitySliderGP = 100.0F;
		if (probabilitySliderGM > 100.0F) probabilitySliderGM = 100.0F;
		if (probabilitySliderGA > 100.0F) probabilitySliderGA = 100.0F;
		if (probabilitySliderGT > 100.0F) probabilitySliderGT = 100.0F;
		if (probabilitySliderGS > 100.0F) probabilitySliderGS = 100.0F;
		if (probabilitySliderGF > 100.0F) probabilitySliderGF = 100.0F;
		if (viscositySliderGR < 0.0F) viscositySliderGR = 0;
		if (viscositySliderGG < 0.0F) viscositySliderGG = 0;
		if (viscositySliderGB < 0.0F) viscositySliderGB = 0;
		if (viscositySliderGW < 0.0F) viscositySliderGW = 0;
		if (viscositySliderGO < 0.0F) viscositySliderGO = 0;
		if (viscositySliderGK < 0.0F) viscositySliderGK = 0;
		if (viscositySliderGC < 0.0F) viscositySliderGC = 0;
		if (viscositySliderGD < 0.0F) viscositySliderGD = 0;
		if (viscositySliderGY < 0.0F) viscositySliderGY = 0;
		if (viscositySliderGP < 0.0F) viscositySliderGP = 0;
		if (viscositySliderGM < 0.0F) viscositySliderGM = 0;
		if (viscositySliderGA < 0.0F) viscositySliderGA = 0;
		if (viscositySliderGT < 0.0F) viscositySliderGT = 0;
		if (viscositySliderGS < 0.0F) viscositySliderGS = 0;
		if (viscositySliderGF < 0.0F) viscositySliderGF = 0;
		if (viscositySliderGR > 1.0F) viscositySliderGR = 1.0F;
		if (viscositySliderGG > 1.0F) viscositySliderGG = 1.0F;
		if (viscositySliderGB > 1.0F) viscositySliderGB = 1.0F;
		if (viscositySliderGW > 1.0F) viscositySliderGW = 1.0F;
		if (viscositySliderGO > 1.0F) viscositySliderGO = 1.0F;
		if (viscositySliderGK > 1.0F) viscositySliderGK = 1.0F;
		if (viscositySliderGC > 1.0F) viscositySliderGC = 1.0F;
		if (viscositySliderGD > 1.0F) viscositySliderGD = 1.0F;
		if (viscositySliderGY > 1.0F) viscositySliderGY = 1.0F;
		if (viscositySliderGP > 1.0F) viscositySliderGP = 1.0F;
		if (viscositySliderGM > 1.0F) viscositySliderGM = 1.0F;
		if (viscositySliderGA > 1.0F) viscositySliderGA = 1.0F;
		if (viscositySliderGT > 1.0F) viscositySliderGT = 1.0F;
		if (viscositySliderGS > 1.0F) viscositySliderGS = 1.0F;
		if (viscositySliderGF > 1.0F) viscositySliderGF = 1.0F;

		if (powerSliderBR < 0.0F) powerSliderBR = 0;
		if (powerSliderBG < 0.0F) powerSliderBG = 0;
		if (powerSliderBB < 0.0F) powerSliderBB = 0;
		if (powerSliderBW < 0.0F) powerSliderBW = 0;
		if (powerSliderBO < 0.0F) powerSliderBO = 0;
		if (powerSliderBK < 0.0F) powerSliderBK = 0;
		if (powerSliderBC < 0.0F) powerSliderBC = 0;
		if (powerSliderBD < 0.0F) powerSliderBD = 0;
		if (powerSliderBY < 0.0F) powerSliderBY = 0;
		if (powerSliderBP < 0.0F) powerSliderBP = 0;
		if (powerSliderBM < 0.0F) powerSliderBM = 0;
		if (powerSliderBA < 0.0F) powerSliderBA = 0;
		if (powerSliderBT < 0.0F) powerSliderBT = 0;
		if (powerSliderBS < 0.0F) powerSliderBS = 0;
		if (powerSliderBF < 0.0F) powerSliderBF = 0;
		if (powerSliderBR > 100.0F) powerSliderBR = 100.0F;
		if (powerSliderBG > 100.0F) powerSliderBG = 100.0F;
		if (powerSliderBB > 100.0F) powerSliderBB = 100.0F;
		if (powerSliderBW > 100.0F) powerSliderBW = 100.0F;
		if (powerSliderBO > 100.0F) powerSliderBO = 100.0F;
		if (powerSliderBK > 100.0F) powerSliderBK = 100.0F;
		if (powerSliderBC > 100.0F) powerSliderBC = 100.0F;
		if (powerSliderBD > 100.0F) powerSliderBD = 100.0F;
		if (powerSliderBY > 100.0F) powerSliderBY = 100.0F;
		if (powerSliderBP > 100.0F) powerSliderBP = 100.0F;
		if (powerSliderBM > 100.0F) powerSliderBM = 100.0F;
		if (powerSliderBA > 100.0F) powerSliderBA = 100.0F;
		if (powerSliderBT > 100.0F) powerSliderBT = 100.0F;
		if (powerSliderBS > 100.0F) powerSliderBS = 100.0F;
		if (powerSliderBF > 100.0F) powerSliderBF = 100.0F;
		if (vSliderBR < 0.0F) vSliderBR = 0;
		if (vSliderBG < 0.0F) vSliderBG = 0;
		if (vSliderBB < 0.0F) vSliderBB = 0;
		if (vSliderBW < 0.0F) vSliderBW = 0;
		if (vSliderBO < 0.0F) vSliderBO = 0;
		if (vSliderBK < 0.0F) vSliderBK = 0;
		if (vSliderBC < 0.0F) vSliderBC = 0;
		if (vSliderBD < 0.0F) vSliderBD = 0;
		if (vSliderBY < 0.0F) vSliderBY = 0;
		if (vSliderBP < 0.0F) vSliderBP = 0;
		if (vSliderBM < 0.0F) vSliderBM = 0;
		if (vSliderBA < 0.0F) vSliderBA = 0;
		if (vSliderBT < 0.0F) vSliderBT = 0;
		if (vSliderBS < 0.0F) vSliderBS = 0;
		if (vSliderBF < 0.0F) vSliderBF = 0;
		if (vSliderBR > 500.0F) vSliderBR = 500.0F;
		if (vSliderBG > 500.0F) vSliderBG = 500.0F;
		if (vSliderBB > 500.0F) vSliderBB = 500.0F;
		if (vSliderBW > 500.0F) vSliderBW = 500.0F;
		if (vSliderBO > 500.0F) vSliderBO = 500.0F;
		if (vSliderBK > 500.0F) vSliderBK = 500.0F;
		if (vSliderBC > 500.0F) vSliderBC = 500.0F;
		if (vSliderBD > 500.0F) vSliderBD = 500.0F;
		if (vSliderBY > 500.0F) vSliderBY = 500.0F;
		if (vSliderBP > 500.0F) vSliderBP = 500.0F;
		if (vSliderBM > 500.0F) vSliderBM = 500.0F;
		if (vSliderBA > 500.0F) vSliderBA = 500.0F;
		if (vSliderBT > 500.0F) vSliderBT = 500.0F;
		if (vSliderBS > 500.0F) vSliderBS = 500.0F;
		if (vSliderBF > 500.0F) vSliderBF = 500.0F;
		if (probabilitySliderBR < 0.0F) probabilitySliderBR = 0;
		if (probabilitySliderBG < 0.0F) probabilitySliderBG = 0;
		if (probabilitySliderBB < 0.0F) probabilitySliderBB = 0;
		if (probabilitySliderBW < 0.0F) probabilitySliderBW = 0;
		if (probabilitySliderBO < 0.0F) probabilitySliderBO = 0;
		if (probabilitySliderBK < 0.0F) probabilitySliderBK = 0;
		if (probabilitySliderBC < 0.0F) probabilitySliderBC = 0;
		if (probabilitySliderBD < 0.0F) probabilitySliderBD = 0;
		if (probabilitySliderBY < 0.0F) probabilitySliderBY = 0;
		if (probabilitySliderBP < 0.0F) probabilitySliderBP = 0;
		if (probabilitySliderBM < 0.0F) probabilitySliderBM = 0;
		if (probabilitySliderBA < 0.0F) probabilitySliderBA = 0;
		if (probabilitySliderBT < 0.0F) probabilitySliderBT = 0;
		if (probabilitySliderBS < 0.0F) probabilitySliderBS = 0;
		if (probabilitySliderBF < 0.0F) probabilitySliderBF = 0;
		if (probabilitySliderBR > 100.0F) probabilitySliderBR = 100.0F;
		if (probabilitySliderBG > 100.0F) probabilitySliderBG = 100.0F;
		if (probabilitySliderBB > 100.0F) probabilitySliderBB = 100.0F;
		if (probabilitySliderBW > 100.0F) probabilitySliderBW = 100.0F;
		if (probabilitySliderBO > 100.0F) probabilitySliderBO = 100.0F;
		if (probabilitySliderBK > 100.0F) probabilitySliderBK = 100.0F;
		if (probabilitySliderBC > 100.0F) probabilitySliderBC = 100.0F;
		if (probabilitySliderBD > 100.0F) probabilitySliderBD = 100.0F;
		if (probabilitySliderBY > 100.0F) probabilitySliderBY = 100.0F;
		if (probabilitySliderBP > 100.0F) probabilitySliderBP = 100.0F;
		if (probabilitySliderBM > 100.0F) probabilitySliderBM = 100.0F;
		if (probabilitySliderBA > 100.0F) probabilitySliderBA = 100.0F;
		if (probabilitySliderBT > 100.0F) probabilitySliderBT = 100.0F;
		if (probabilitySliderBS > 100.0F) probabilitySliderBS = 100.0F;
		if (probabilitySliderBF > 100.0F) probabilitySliderBF = 100.0F;
		if (viscositySliderBR < 0.0F) viscositySliderBR = 0;
		if (viscositySliderBG < 0.0F) viscositySliderBG = 0;
		if (viscositySliderBB < 0.0F) viscositySliderBB = 0;
		if (viscositySliderBW < 0.0F) viscositySliderBW = 0;
		if (viscositySliderBO < 0.0F) viscositySliderBO = 0;
		if (viscositySliderBK < 0.0F) viscositySliderBK = 0;
		if (viscositySliderBC < 0.0F) viscositySliderBC = 0;
		if (viscositySliderBD < 0.0F) viscositySliderBD = 0;
		if (viscositySliderBY < 0.0F) viscositySliderBY = 0;
		if (viscositySliderBP < 0.0F) viscositySliderBP = 0;
		if (viscositySliderBM < 0.0F) viscositySliderBM = 0;
		if (viscositySliderBA < 0.0F) viscositySliderBA = 0;
		if (viscositySliderBT < 0.0F) viscositySliderBT = 0;
		if (viscositySliderBS < 0.0F) viscositySliderBS = 0;
		if (viscositySliderBF < 0.0F) viscositySliderBF = 0;
		if (viscositySliderBR > 1.0F) viscositySliderBR = 1.0F;
		if (viscositySliderBG > 1.0F) viscositySliderBG = 1.0F;
		if (viscositySliderBB > 1.0F) viscositySliderBB = 1.0F;
		if (viscositySliderBW > 1.0F) viscositySliderBW = 1.0F;
		if (viscositySliderBO > 1.0F) viscositySliderBO = 1.0F;
		if (viscositySliderBK > 1.0F) viscositySliderBK = 1.0F;
		if (viscositySliderBC > 1.0F) viscositySliderBC = 1.0F;
		if (viscositySliderBD > 1.0F) viscositySliderBD = 1.0F;
		if (viscositySliderBY > 1.0F) viscositySliderBY = 1.0F;
		if (viscositySliderBP > 1.0F) viscositySliderBP = 1.0F;
		if (viscositySliderBM > 1.0F) viscositySliderBM = 1.0F;
		if (viscositySliderBA > 1.0F) viscositySliderBA = 1.0F;
		if (viscositySliderBT > 1.0F) viscositySliderBT = 1.0F;
		if (viscositySliderBS > 1.0F) viscositySliderBS = 1.0F;
		if (viscositySliderBF > 1.0F) viscositySliderBF = 1.0F;

		if (powerSliderWR < 0.0F) powerSliderWR = 0;
		if (powerSliderWG < 0.0F) powerSliderWG = 0;
		if (powerSliderWB < 0.0F) powerSliderWB = 0;
		if (powerSliderWW < 0.0F) powerSliderWW = 0;
		if (powerSliderWO < 0.0F) powerSliderWO = 0;
		if (powerSliderWK < 0.0F) powerSliderWK = 0;
		if (powerSliderWC < 0.0F) powerSliderWC = 0;
		if (powerSliderWD < 0.0F) powerSliderWD = 0;
		if (powerSliderWY < 0.0F) powerSliderWY = 0;
		if (powerSliderWP < 0.0F) powerSliderWP = 0;
		if (powerSliderWM < 0.0F) powerSliderWM = 0;
		if (powerSliderWA < 0.0F) powerSliderWA = 0;
		if (powerSliderWT < 0.0F) powerSliderWT = 0;
		if (powerSliderWS < 0.0F) powerSliderWS = 0;
		if (powerSliderWF < 0.0F) powerSliderWF = 0;
		if (powerSliderWR > 100.0F) powerSliderWR = 100.0F;
		if (powerSliderWG > 100.0F) powerSliderWG = 100.0F;
		if (powerSliderWB > 100.0F) powerSliderWB = 100.0F;
		if (powerSliderWW > 100.0F) powerSliderWW = 100.0F;
		if (powerSliderWO > 100.0F) powerSliderWO = 100.0F;
		if (powerSliderWK > 100.0F) powerSliderWK = 100.0F;
		if (powerSliderWC > 100.0F) powerSliderWC = 100.0F;
		if (powerSliderWD > 100.0F) powerSliderWD = 100.0F;
		if (powerSliderWY > 100.0F) powerSliderWY = 100.0F;
		if (powerSliderWP > 100.0F) powerSliderWP = 100.0F;
		if (powerSliderWM > 100.0F) powerSliderWM = 100.0F;
		if (powerSliderWA > 100.0F) powerSliderWA = 100.0F;
		if (powerSliderWT > 100.0F) powerSliderWT = 100.0F;
		if (powerSliderWS > 100.0F) powerSliderWS = 100.0F;
		if (powerSliderWF > 100.0F) powerSliderWF = 100.0F;
		if (vSliderWR < 0.0F) vSliderWR = 0;
		if (vSliderWG < 0.0F) vSliderWG = 0;
		if (vSliderWB < 0.0F) vSliderWB = 0;
		if (vSliderWW < 0.0F) vSliderWW = 0;
		if (vSliderWO < 0.0F) vSliderWO = 0;
		if (vSliderWK < 0.0F) vSliderWK = 0;
		if (vSliderWC < 0.0F) vSliderWC = 0;
		if (vSliderWD < 0.0F) vSliderWD = 0;
		if (vSliderWY < 0.0F) vSliderWY = 0;
		if (vSliderWP < 0.0F) vSliderWP = 0;
		if (vSliderWM < 0.0F) vSliderWM = 0;
		if (vSliderWA < 0.0F) vSliderWA = 0;
		if (vSliderWT < 0.0F) vSliderWT = 0;
		if (vSliderWS < 0.0F) vSliderWS = 0;
		if (vSliderWF < 0.0F) vSliderWF = 0;
		if (vSliderWR > 500.0F) vSliderWR = 500.0F;
		if (vSliderWG > 500.0F) vSliderWG = 500.0F;
		if (vSliderWB > 500.0F) vSliderWB = 500.0F;
		if (vSliderWW > 500.0F) vSliderWW = 500.0F;
		if (vSliderWO > 500.0F) vSliderWO = 500.0F;
		if (vSliderWK > 500.0F) vSliderWK = 500.0F;
		if (vSliderWC > 500.0F) vSliderWC = 500.0F;
		if (vSliderWD > 500.0F) vSliderWD = 500.0F;
		if (vSliderWY > 500.0F) vSliderWY = 500.0F;
		if (vSliderWP > 500.0F) vSliderWP = 500.0F;
		if (vSliderWM > 500.0F) vSliderWM = 500.0F;
		if (vSliderWA > 500.0F) vSliderWA = 500.0F;
		if (vSliderWT > 500.0F) vSliderWT = 500.0F;
		if (vSliderWS > 500.0F) vSliderWS = 500.0F;
		if (vSliderWF > 500.0F) vSliderWF = 500.0F;
		if (probabilitySliderWR < 0.0F) probabilitySliderWR = 0;
		if (probabilitySliderWG < 0.0F) probabilitySliderWG = 0;
		if (probabilitySliderWB < 0.0F) probabilitySliderWB = 0;
		if (probabilitySliderWW < 0.0F) probabilitySliderWW = 0;
		if (probabilitySliderWO < 0.0F) probabilitySliderWO = 0;
		if (probabilitySliderWK < 0.0F) probabilitySliderWK = 0;
		if (probabilitySliderWC < 0.0F) probabilitySliderWC = 0;
		if (probabilitySliderWD < 0.0F) probabilitySliderWD = 0;
		if (probabilitySliderWY < 0.0F) probabilitySliderWY = 0;
		if (probabilitySliderWP < 0.0F) probabilitySliderWP = 0;
		if (probabilitySliderWM < 0.0F) probabilitySliderWM = 0;
		if (probabilitySliderWA < 0.0F) probabilitySliderWA = 0;
		if (probabilitySliderWT < 0.0F) probabilitySliderWT = 0;
		if (probabilitySliderWS < 0.0F) probabilitySliderWS = 0;
		if (probabilitySliderWF < 0.0F) probabilitySliderWF = 0;
		if (probabilitySliderWR > 100.0F) probabilitySliderWR = 100.0F;
		if (probabilitySliderWG > 100.0F) probabilitySliderWG = 100.0F;
		if (probabilitySliderWB > 100.0F) probabilitySliderWB = 100.0F;
		if (probabilitySliderWW > 100.0F) probabilitySliderWW = 100.0F;
		if (probabilitySliderWO > 100.0F) probabilitySliderWO = 100.0F;
		if (probabilitySliderWK > 100.0F) probabilitySliderWK = 100.0F;
		if (probabilitySliderWC > 100.0F) probabilitySliderWC = 100.0F;
		if (probabilitySliderWD > 100.0F) probabilitySliderWD = 100.0F;
		if (probabilitySliderWY > 100.0F) probabilitySliderWY = 100.0F;
		if (probabilitySliderWP > 100.0F) probabilitySliderWP = 100.0F;
		if (probabilitySliderWM > 100.0F) probabilitySliderWM = 100.0F;
		if (probabilitySliderWA > 100.0F) probabilitySliderWA = 100.0F;
		if (probabilitySliderWT > 100.0F) probabilitySliderWT = 100.0F;
		if (probabilitySliderWS > 100.0F) probabilitySliderWS = 100.0F;
		if (probabilitySliderWF > 100.0F) probabilitySliderWF = 100.0F;
		if (viscositySliderWR < 0.0F) viscositySliderWR = 0;
		if (viscositySliderWG < 0.0F) viscositySliderWG = 0;
		if (viscositySliderWB < 0.0F) viscositySliderWB = 0;
		if (viscositySliderWW < 0.0F) viscositySliderWW = 0;
		if (viscositySliderWO < 0.0F) viscositySliderWO = 0;
		if (viscositySliderWK < 0.0F) viscositySliderWK = 0;
		if (viscositySliderWC < 0.0F) viscositySliderWC = 0;
		if (viscositySliderWD < 0.0F) viscositySliderWD = 0;
		if (viscositySliderWY < 0.0F) viscositySliderWY = 0;
		if (viscositySliderWP < 0.0F) viscositySliderWP = 0;
		if (viscositySliderWM < 0.0F) viscositySliderWM = 0;
		if (viscositySliderWA < 0.0F) viscositySliderWA = 0;
		if (viscositySliderWT < 0.0F) viscositySliderWT = 0;
		if (viscositySliderWS < 0.0F) viscositySliderWS = 0;
		if (viscositySliderWF < 0.0F) viscositySliderWF = 0;
		if (viscositySliderWR > 1.0F) viscositySliderWR = 1.0F;
		if (viscositySliderWG > 1.0F) viscositySliderWG = 1.0F;
		if (viscositySliderWB > 1.0F) viscositySliderWB = 1.0F;
		if (viscositySliderWW > 1.0F) viscositySliderWW = 1.0F;
		if (viscositySliderWO > 1.0F) viscositySliderWO = 1.0F;
		if (viscositySliderWK > 1.0F) viscositySliderWK = 1.0F;
		if (viscositySliderWC > 1.0F) viscositySliderWC = 1.0F;
		if (viscositySliderWD > 1.0F) viscositySliderWD = 1.0F;
		if (viscositySliderWY > 1.0F) viscositySliderWY = 1.0F;
		if (viscositySliderWP > 1.0F) viscositySliderWP = 1.0F;
		if (viscositySliderWM > 1.0F) viscositySliderWM = 1.0F;
		if (viscositySliderWA > 1.0F) viscositySliderWA = 1.0F;
		if (viscositySliderWT > 1.0F) viscositySliderWT = 1.0F;
		if (viscositySliderWS > 1.0F) viscositySliderWS = 1.0F;
		if (viscositySliderWF > 1.0F) viscositySliderWF = 1.0F;

		if (powerSliderOR < -100.0F) powerSliderOR = -100.0F;
		if (powerSliderOG < -100.0F) powerSliderOG = -100.0F;
		if (powerSliderOB < -100.0F) powerSliderOB = -100.0F;
		if (powerSliderOW < -100.0F) powerSliderOW = -100.0F;
		if (powerSliderOO < -100.0F) powerSliderOO = -100.0F;
		if (powerSliderOK < -100.0F) powerSliderOK = -100.0F;
		if (powerSliderOC < -100.0F) powerSliderOC = -100.0F;
		if (powerSliderOD < -100.0F) powerSliderOD = -100.0F;
		if (powerSliderOY < -100.0F) powerSliderOY = -100.0F;
		if (powerSliderOP < -100.0F) powerSliderOP = -100.0F;
		if (powerSliderOM < -100.0F) powerSliderOM = -100.0F;
		if (powerSliderOA < -100.0F) powerSliderOA = -100.0F;
		if (powerSliderOT < -100.0F) powerSliderOT = -100.0F;
		if (powerSliderOS < -100.0F) powerSliderOS = -100.0F;
		if (powerSliderOF < -100.0F) powerSliderOF = -100.0F;
		if (powerSliderOR > 100.0F) powerSliderOR = 100.0F;
		if (powerSliderOG > 100.0F) powerSliderOG = 100.0F;
		if (powerSliderOB > 100.0F) powerSliderOB = 100.0F;
		if (powerSliderOW > 100.0F) powerSliderOW = 100.0F;
		if (powerSliderOO > 100.0F) powerSliderOO = 100.0F;
		if (powerSliderOK > 100.0F) powerSliderOK = 100.0F;
		if (powerSliderOC > 100.0F) powerSliderOC = 100.0F;
		if (powerSliderOD > 100.0F) powerSliderOD = 100.0F;
		if (powerSliderOY > 100.0F) powerSliderOY = 100.0F;
		if (powerSliderOP > 100.0F) powerSliderOP = 100.0F;
		if (powerSliderOM > 100.0F) powerSliderOM = 100.0F;
		if (powerSliderOA > 100.0F) powerSliderOA = 100.0F;
		if (powerSliderOT > 100.0F) powerSliderOT = 100.0F;
		if (powerSliderOS > 100.0F) powerSliderOS = 100.0F;
		if (powerSliderOF > 100.0F) powerSliderOF = 100.0F;
		if (vSliderOR < 0.0F) vSliderOR = 0;
		if (vSliderOG < 0.0F) vSliderOG = 0;
		if (vSliderOB < 0.0F) vSliderOB = 0;
		if (vSliderOW < 0.0F) vSliderOW = 0;
		if (vSliderOO < 0.0F) vSliderOO = 0;
		if (vSliderOK < 0.0F) vSliderOK = 0;
		if (vSliderOC < 0.0F) vSliderOC = 0;
		if (vSliderOD < 0.0F) vSliderOD = 0;
		if (vSliderOY < 0.0F) vSliderOY = 0;
		if (vSliderOP < 0.0F) vSliderOP = 0;
		if (vSliderOM < 0.0F) vSliderOM = 0;
		if (vSliderOA < 0.0F) vSliderOA = 0;
		if (vSliderOT < 0.0F) vSliderOT = 0;
		if (vSliderOS < 0.0F) vSliderOS = 0;
		if (vSliderOF < 0.0F) vSliderOF = 0;
		if (vSliderOR > 500.0F) vSliderOR = 500.0F;
		if (vSliderOG > 500.0F) vSliderOG = 500.0F;
		if (vSliderOB > 500.0F) vSliderOB = 500.0F;
		if (vSliderOW > 500.0F) vSliderOW = 500.0F;
		if (vSliderOO > 500.0F) vSliderOO = 500.0F;
		if (vSliderOK > 500.0F) vSliderOK = 500.0F;
		if (vSliderOC > 500.0F) vSliderOC = 500.0F;
		if (vSliderOD > 500.0F) vSliderOD = 500.0F;
		if (vSliderOY > 500.0F) vSliderOY = 500.0F;
		if (vSliderOP > 500.0F) vSliderOP = 500.0F;
		if (vSliderOM > 500.0F) vSliderOM = 500.0F;
		if (vSliderOA > 500.0F) vSliderOA = 500.0F;
		if (vSliderOT > 500.0F) vSliderOT = 500.0F;
		if (vSliderOS > 500.0F) vSliderOS = 500.0F;
		if (vSliderOF > 500.0F) vSliderOF = 500.0F;
		if (probabilitySliderOR < 0.0F) probabilitySliderOR = 0;
		if (probabilitySliderOG < 0.0F) probabilitySliderOG = 0;
		if (probabilitySliderOB < 0.0F) probabilitySliderOB = 0;
		if (probabilitySliderOW < 0.0F) probabilitySliderOW = 0;
		if (probabilitySliderOO < 0.0F) probabilitySliderOO = 0;
		if (probabilitySliderOK < 0.0F) probabilitySliderOK = 0;
		if (probabilitySliderOC < 0.0F) probabilitySliderOC = 0;
		if (probabilitySliderOD < 0.0F) probabilitySliderOD = 0;
		if (probabilitySliderOY < 0.0F) probabilitySliderOY = 0;
		if (probabilitySliderOP < 0.0F) probabilitySliderOP = 0;
		if (probabilitySliderOM < 0.0F) probabilitySliderOM = 0;
		if (probabilitySliderOA < 0.0F) probabilitySliderOA = 0;
		if (probabilitySliderOT < 0.0F) probabilitySliderOT = 0;
		if (probabilitySliderOS < 0.0F) probabilitySliderOS = 0;
		if (probabilitySliderOF < 0.0F) probabilitySliderOF = 0;
		if (probabilitySliderOR > 100.0F) probabilitySliderOR = 100.0F;
		if (probabilitySliderOG > 100.0F) probabilitySliderOG = 100.0F;
		if (probabilitySliderOB > 100.0F) probabilitySliderOB = 100.0F;
		if (probabilitySliderOW > 100.0F) probabilitySliderOW = 100.0F;
		if (probabilitySliderOO > 100.0F) probabilitySliderOO = 100.0F;
		if (probabilitySliderOK > 100.0F) probabilitySliderOK = 100.0F;
		if (probabilitySliderOC > 100.0F) probabilitySliderOC = 100.0F;
		if (probabilitySliderOD > 100.0F) probabilitySliderOD = 100.0F;
		if (probabilitySliderOY > 100.0F) probabilitySliderOY = 100.0F;
		if (probabilitySliderOP > 100.0F) probabilitySliderOP = 100.0F;
		if (probabilitySliderOM > 100.0F) probabilitySliderOM = 100.0F;
		if (probabilitySliderOA > 100.0F) probabilitySliderOA = 100.0F;
		if (probabilitySliderOT > 100.0F) probabilitySliderOT = 100.0F;
		if (probabilitySliderOS > 100.0F) probabilitySliderOS = 100.0F;
		if (probabilitySliderOF > 100.0F) probabilitySliderOF = 100.0F;
		if (viscositySliderOR < 0.0F) viscositySliderOR = 0;
		if (viscositySliderOG < 0.0F) viscositySliderOG = 0;
		if (viscositySliderOB < 0.0F) viscositySliderOB = 0;
		if (viscositySliderOW < 0.0F) viscositySliderOW = 0;
		if (viscositySliderOO < 0.0F) viscositySliderOO = 0;
		if (viscositySliderOK < 0.0F) viscositySliderOK = 0;
		if (viscositySliderOC < 0.0F) viscositySliderOC = 0;
		if (viscositySliderOD < 0.0F) viscositySliderOD = 0;
		if (viscositySliderOY < 0.0F) viscositySliderOY = 0;
		if (viscositySliderOP < 0.0F) viscositySliderOP = 0;
		if (viscositySliderOM < 0.0F) viscositySliderOM = 0;
		if (viscositySliderOA < 0.0F) viscositySliderOA = 0;
		if (viscositySliderOT < 0.0F) viscositySliderOT = 0;
		if (viscositySliderOS < 0.0F) viscositySliderOS = 0;
		if (viscositySliderOF < 0.0F) viscositySliderOF = 0;
		if (viscositySliderOR > 1.0F) viscositySliderOR = 1.0F;
		if (viscositySliderOG > 1.0F) viscositySliderOG = 1.0F;
		if (viscositySliderOB > 1.0F) viscositySliderOB = 1.0F;
		if (viscositySliderOW > 1.0F) viscositySliderOW = 1.0F;
		if (viscositySliderOO > 1.0F) viscositySliderOO = 1.0F;
		if (viscositySliderOK > 1.0F) viscositySliderOK = 1.0F;
		if (viscositySliderOC > 1.0F) viscositySliderOC = 1.0F;
		if (viscositySliderOD > 1.0F) viscositySliderOD = 1.0F;
		if (viscositySliderOY > 1.0F) viscositySliderOY = 1.0F;
		if (viscositySliderOP > 1.0F) viscositySliderOP = 1.0F;
		if (viscositySliderOM > 1.0F) viscositySliderOM = 1.0F;
		if (viscositySliderOA > 1.0F) viscositySliderOA = 1.0F;
		if (viscositySliderOT > 1.0F) viscositySliderOT = 1.0F;
		if (viscositySliderOS > 1.0F) viscositySliderOS = 1.0F;
		if (viscositySliderOF > 1.0F) viscositySliderOF = 1.0F;

		if (powerSliderKR < -100.0F) powerSliderKR = -100.0F;
		if (powerSliderKG < -100.0F) powerSliderKG = -100.0F;
		if (powerSliderKB < -100.0F) powerSliderKB = -100.0F;
		if (powerSliderKW < -100.0F) powerSliderKW = -100.0F;
		if (powerSliderKO < -100.0F) powerSliderKO = -100.0F;
		if (powerSliderKK < -100.0F) powerSliderKK = -100.0F;
		if (powerSliderKC < -100.0F) powerSliderKC = -100.0F;
		if (powerSliderKD < -100.0F) powerSliderKD = -100.0F;
		if (powerSliderKY < -100.0F) powerSliderKY = -100.0F;
		if (powerSliderKP < -100.0F) powerSliderKP = -100.0F;
		if (powerSliderKM < -100.0F) powerSliderKM = -100.0F;
		if (powerSliderKA < -100.0F) powerSliderKA = -100.0F;
		if (powerSliderKT < -100.0F) powerSliderKT = -100.0F;
		if (powerSliderKS < -100.0F) powerSliderKS = -100.0F;
		if (powerSliderKF < -100.0F) powerSliderKF = -100.0F;
		if (powerSliderKR > 100.0F) powerSliderKR = 100.0F;
		if (powerSliderKG > 100.0F) powerSliderKG = 100.0F;
		if (powerSliderKB > 100.0F) powerSliderKB = 100.0F;
		if (powerSliderKW > 100.0F) powerSliderKW = 100.0F;
		if (powerSliderKO > 100.0F) powerSliderKO = 100.0F;
		if (powerSliderKK > 100.0F) powerSliderKK = 100.0F;
		if (powerSliderKC > 100.0F) powerSliderKC = 100.0F;
		if (powerSliderKD > 100.0F) powerSliderKD = 100.0F;
		if (powerSliderKY > 100.0F) powerSliderKY = 100.0F;
		if (powerSliderKP > 100.0F) powerSliderKP = 100.0F;
		if (powerSliderKM > 100.0F) powerSliderKM = 100.0F;
		if (powerSliderKA > 100.0F) powerSliderKA = 100.0F;
		if (powerSliderKT > 100.0F) powerSliderKT = 100.0F;
		if (powerSliderKS > 100.0F) powerSliderKS = 100.0F;
		if (powerSliderKF > 100.0F) powerSliderKF = 100.0F;
		if (vSliderKR < 0.0F) vSliderKR = 0;
		if (vSliderKG < 0.0F) vSliderKG = 0;
		if (vSliderKB < 0.0F) vSliderKB = 0;
		if (vSliderKW < 0.0F) vSliderKW = 0;
		if (vSliderKO < 0.0F) vSliderKO = 0;
		if (vSliderKK < 0.0F) vSliderKK = 0;
		if (vSliderKC < 0.0F) vSliderKC = 0;
		if (vSliderKD < 0.0F) vSliderKD = 0;
		if (vSliderKY < 0.0F) vSliderKY = 0;
		if (vSliderKP < 0.0F) vSliderKP = 0;
		if (vSliderKM < 0.0F) vSliderKM = 0;
		if (vSliderKA < 0.0F) vSliderKA = 0;
		if (vSliderKT < 0.0F) vSliderKT = 0;
		if (vSliderKS < 0.0F) vSliderKS = 0;
		if (vSliderKF < 0.0F) vSliderKF = 0;
		if (vSliderKR > 500.0F) vSliderKR = 500.0F;
		if (vSliderKG > 500.0F) vSliderKG = 500.0F;
		if (vSliderKB > 500.0F) vSliderKB = 500.0F;
		if (vSliderKW > 500.0F) vSliderKW = 500.0F;
		if (vSliderKO > 500.0F) vSliderKO = 500.0F;
		if (vSliderKK > 500.0F) vSliderKK = 500.0F;
		if (vSliderKC > 500.0F) vSliderKC = 500.0F;
		if (vSliderKD > 500.0F) vSliderKD = 500.0F;
		if (vSliderKY > 500.0F) vSliderKY = 500.0F;
		if (vSliderKP > 500.0F) vSliderKP = 500.0F;
		if (vSliderKM > 500.0F) vSliderKM = 500.0F;
		if (vSliderKA > 500.0F) vSliderKA = 500.0F;
		if (vSliderKT > 500.0F) vSliderKT = 500.0F;
		if (vSliderKS > 500.0F) vSliderKS = 500.0F;
		if (vSliderKF > 500.0F) vSliderKF = 500.0F;
		if (probabilitySliderKR < 0.0F) probabilitySliderKR = 0;
		if (probabilitySliderKG < 0.0F) probabilitySliderKG = 0;
		if (probabilitySliderKB < 0.0F) probabilitySliderKB = 0;
		if (probabilitySliderKW < 0.0F) probabilitySliderKW = 0;
		if (probabilitySliderKO < 0.0F) probabilitySliderKO = 0;
		if (probabilitySliderKK < 0.0F) probabilitySliderKK = 0;
		if (probabilitySliderKC < 0.0F) probabilitySliderKC = 0;
		if (probabilitySliderKD < 0.0F) probabilitySliderKD = 0;
		if (probabilitySliderKY < 0.0F) probabilitySliderKY = 0;
		if (probabilitySliderKP < 0.0F) probabilitySliderKP = 0;
		if (probabilitySliderKM < 0.0F) probabilitySliderKM = 0;
		if (probabilitySliderKA < 0.0F) probabilitySliderKA = 0;
		if (probabilitySliderKT < 0.0F) probabilitySliderKT = 0;
		if (probabilitySliderKS < 0.0F) probabilitySliderKS = 0;
		if (probabilitySliderKF < 0.0F) probabilitySliderKF = 0;
		if (probabilitySliderKR > 100.0F) probabilitySliderKR = 100.0F;
		if (probabilitySliderKG > 100.0F) probabilitySliderKG = 100.0F;
		if (probabilitySliderKB > 100.0F) probabilitySliderKB = 100.0F;
		if (probabilitySliderKW > 100.0F) probabilitySliderKW = 100.0F;
		if (probabilitySliderKO > 100.0F) probabilitySliderKO = 100.0F;
		if (probabilitySliderKK > 100.0F) probabilitySliderKK = 100.0F;
		if (probabilitySliderKC > 100.0F) probabilitySliderKC = 100.0F;
		if (probabilitySliderKD > 100.0F) probabilitySliderKD = 100.0F;
		if (probabilitySliderKY > 100.0F) probabilitySliderKY = 100.0F;
		if (probabilitySliderKP > 100.0F) probabilitySliderKP = 100.0F;
		if (probabilitySliderKM > 100.0F) probabilitySliderKM = 100.0F;
		if (probabilitySliderKA > 100.0F) probabilitySliderKA = 100.0F;
		if (probabilitySliderKT > 100.0F) probabilitySliderKT = 100.0F;
		if (probabilitySliderKS > 100.0F) probabilitySliderKS = 100.0F;
		if (probabilitySliderKF > 100.0F) probabilitySliderKF = 100.0F;
		if (viscositySliderKR < 0.0F) viscositySliderKR = 0;
		if (viscositySliderKG < 0.0F) viscositySliderKG = 0;
		if (viscositySliderKB < 0.0F) viscositySliderKB = 0;
		if (viscositySliderKW < 0.0F) viscositySliderKW = 0;
		if (viscositySliderKO < 0.0F) viscositySliderKO = 0;
		if (viscositySliderKK < 0.0F) viscositySliderKK = 0;
		if (viscositySliderKC < 0.0F) viscositySliderKC = 0;
		if (viscositySliderKD < 0.0F) viscositySliderKD = 0;
		if (viscositySliderKY < 0.0F) viscositySliderKY = 0;
		if (viscositySliderKP < 0.0F) viscositySliderKP = 0;
		if (viscositySliderKM < 0.0F) viscositySliderKM = 0;
		if (viscositySliderKA < 0.0F) viscositySliderKA = 0;
		if (viscositySliderKT < 0.0F) viscositySliderKT = 0;
		if (viscositySliderKS < 0.0F) viscositySliderKS = 0;
		if (viscositySliderKF < 0.0F) viscositySliderKF = 0;
		if (viscositySliderKR > 1.0F) viscositySliderKR = 1.0F;
		if (viscositySliderKG > 1.0F) viscositySliderKG = 1.0F;
		if (viscositySliderKB > 1.0F) viscositySliderKB = 1.0F;
		if (viscositySliderKW > 1.0F) viscositySliderKW = 1.0F;
		if (viscositySliderKO > 1.0F) viscositySliderKO = 1.0F;
		if (viscositySliderKK > 1.0F) viscositySliderKK = 1.0F;
		if (viscositySliderKC > 1.0F) viscositySliderKC = 1.0F;
		if (viscositySliderKD > 1.0F) viscositySliderKD = 1.0F;
		if (viscositySliderKY > 1.0F) viscositySliderKY = 1.0F;
		if (viscositySliderKP > 1.0F) viscositySliderKP = 1.0F;
		if (viscositySliderKM > 1.0F) viscositySliderKM = 1.0F;
		if (viscositySliderKA > 1.0F) viscositySliderKA = 1.0F;
		if (viscositySliderKT > 1.0F) viscositySliderKT = 1.0F;
		if (viscositySliderKS > 1.0F) viscositySliderKS = 1.0F;
		if (viscositySliderKF > 1.0F) viscositySliderKF = 1.0F;

		if (powerSliderCR < -100.0F) powerSliderCR = -100.0F;
		if (powerSliderCG < -100.0F) powerSliderCG = -100.0F;
		if (powerSliderCB < -100.0F) powerSliderCB = -100.0F;
		if (powerSliderCW < -100.0F) powerSliderCW = -100.0F;
		if (powerSliderCO < -100.0F) powerSliderCO = -100.0F;
		if (powerSliderCK < -100.0F) powerSliderCK = -100.0F;
		if (powerSliderCC < -100.0F) powerSliderCC = -100.0F;
		if (powerSliderCD < -100.0F) powerSliderCD = -100.0F;
		if (powerSliderCY < -100.0F) powerSliderCY = -100.0F;
		if (powerSliderCP < -100.0F) powerSliderCP = -100.0F;
		if (powerSliderCM < -100.0F) powerSliderCM = -100.0F;
		if (powerSliderCA < -100.0F) powerSliderCA = -100.0F;
		if (powerSliderCT < -100.0F) powerSliderCT = -100.0F;
		if (powerSliderCS < -100.0F) powerSliderCS = -100.0F;
		if (powerSliderCF < -100.0F) powerSliderCF = -100.0F;
		if (powerSliderCR > 100.0F) powerSliderCR = 100.0F;
		if (powerSliderCG > 100.0F) powerSliderCG = 100.0F;
		if (powerSliderCB > 100.0F) powerSliderCB = 100.0F;
		if (powerSliderCW > 100.0F) powerSliderCW = 100.0F;
		if (powerSliderCO > 100.0F) powerSliderCO = 100.0F;
		if (powerSliderCK > 100.0F) powerSliderCK = 100.0F;
		if (powerSliderCC > 100.0F) powerSliderCC = 100.0F;
		if (powerSliderCD > 100.0F) powerSliderCD = 100.0F;
		if (powerSliderCY > 100.0F) powerSliderCY = 100.0F;
		if (powerSliderCP > 100.0F) powerSliderCP = 100.0F;
		if (powerSliderCM > 100.0F) powerSliderCM = 100.0F;
		if (powerSliderCA > 100.0F) powerSliderCA = 100.0F;
		if (powerSliderCT > 100.0F) powerSliderCT = 100.0F;
		if (powerSliderCS > 100.0F) powerSliderCS = 100.0F;
		if (powerSliderCF > 100.0F) powerSliderCF = 100.0F;
		if (vSliderCR < 0.0F) vSliderCR = 0;
		if (vSliderCG < 0.0F) vSliderCG = 0;
		if (vSliderCB < 0.0F) vSliderCB = 0;
		if (vSliderCW < 0.0F) vSliderCW = 0;
		if (vSliderCO < 0.0F) vSliderCO = 0;
		if (vSliderCK < 0.0F) vSliderCK = 0;
		if (vSliderCC < 0.0F) vSliderCC = 0;
		if (vSliderCD < 0.0F) vSliderCD = 0;
		if (vSliderCY < 0.0F) vSliderCY = 0;
		if (vSliderCP < 0.0F) vSliderCP = 0;
		if (vSliderCM < 0.0F) vSliderCM = 0;
		if (vSliderCA < 0.0F) vSliderCA = 0;
		if (vSliderCT < 0.0F) vSliderCT = 0;
		if (vSliderCS < 0.0F) vSliderCS = 0;
		if (vSliderCF < 0.0F) vSliderCF = 0;
		if (vSliderCR > 500.0F) vSliderCR = 500.0F;
		if (vSliderCG > 500.0F) vSliderCG = 500.0F;
		if (vSliderCB > 500.0F) vSliderCB = 500.0F;
		if (vSliderCW > 500.0F) vSliderCW = 500.0F;
		if (vSliderCO > 500.0F) vSliderCO = 500.0F;
		if (vSliderCK > 500.0F) vSliderCK = 500.0F;
		if (vSliderCC > 500.0F) vSliderCC = 500.0F;
		if (vSliderCD > 500.0F) vSliderCD = 500.0F;
		if (vSliderCY > 500.0F) vSliderCY = 500.0F;
		if (vSliderCP > 500.0F) vSliderCP = 500.0F;
		if (vSliderCM > 500.0F) vSliderCM = 500.0F;
		if (vSliderCA > 500.0F) vSliderCA = 500.0F;
		if (vSliderCT > 500.0F) vSliderCT = 500.0F;
		if (vSliderCS > 500.0F) vSliderCS = 500.0F;
		if (vSliderCF > 500.0F) vSliderCF = 500.0F;
		if (probabilitySliderCR < 0.0F) probabilitySliderCR = 0;
		if (probabilitySliderCG < 0.0F) probabilitySliderCG = 0;
		if (probabilitySliderCB < 0.0F) probabilitySliderCB = 0;
		if (probabilitySliderCW < 0.0F) probabilitySliderCW = 0;
		if (probabilitySliderCO < 0.0F) probabilitySliderCO = 0;
		if (probabilitySliderCK < 0.0F) probabilitySliderCK = 0;
		if (probabilitySliderCC < 0.0F) probabilitySliderCC = 0;
		if (probabilitySliderCD < 0.0F) probabilitySliderCD = 0;
		if (probabilitySliderCY < 0.0F) probabilitySliderCY = 0;
		if (probabilitySliderCP < 0.0F) probabilitySliderCP = 0;
		if (probabilitySliderCM < 0.0F) probabilitySliderCM = 0;
		if (probabilitySliderCA < 0.0F) probabilitySliderCA = 0;
		if (probabilitySliderCT < 0.0F) probabilitySliderCT = 0;
		if (probabilitySliderCS < 0.0F) probabilitySliderCS = 0;
		if (probabilitySliderCF < 0.0F) probabilitySliderCF = 0;
		if (probabilitySliderCR > 100.0F) probabilitySliderCR = 100.0F;
		if (probabilitySliderCG > 100.0F) probabilitySliderCG = 100.0F;
		if (probabilitySliderCB > 100.0F) probabilitySliderCB = 100.0F;
		if (probabilitySliderCW > 100.0F) probabilitySliderCW = 100.0F;
		if (probabilitySliderCO > 100.0F) probabilitySliderCO = 100.0F;
		if (probabilitySliderCK > 100.0F) probabilitySliderCK = 100.0F;
		if (probabilitySliderCC > 100.0F) probabilitySliderCC = 100.0F;
		if (probabilitySliderCD > 100.0F) probabilitySliderCD = 100.0F;
		if (probabilitySliderCY > 100.0F) probabilitySliderCY = 100.0F;
		if (probabilitySliderCP > 100.0F) probabilitySliderCP = 100.0F;
		if (probabilitySliderCM > 100.0F) probabilitySliderCM = 100.0F;
		if (probabilitySliderCA > 100.0F) probabilitySliderCA = 100.0F;
		if (probabilitySliderCT > 100.0F) probabilitySliderCT = 100.0F;
		if (probabilitySliderCS > 100.0F) probabilitySliderCS = 100.0F;
		if (probabilitySliderCF > 100.0F) probabilitySliderCF = 100.0F;
		if (viscositySliderCR < 0.0F) viscositySliderCR = 0;
		if (viscositySliderCG < 0.0F) viscositySliderCG = 0;
		if (viscositySliderCB < 0.0F) viscositySliderCB = 0;
		if (viscositySliderCW < 0.0F) viscositySliderCW = 0;
		if (viscositySliderCO < 0.0F) viscositySliderCO = 0;
		if (viscositySliderCK < 0.0F) viscositySliderCK = 0;
		if (viscositySliderCC < 0.0F) viscositySliderCC = 0;
		if (viscositySliderCD < 0.0F) viscositySliderCD = 0;
		if (viscositySliderCY < 0.0F) viscositySliderCY = 0;
		if (viscositySliderCP < 0.0F) viscositySliderCP = 0;
		if (viscositySliderCM < 0.0F) viscositySliderCM = 0;
		if (viscositySliderCA < 0.0F) viscositySliderCA = 0;
		if (viscositySliderCT < 0.0F) viscositySliderCT = 0;
		if (viscositySliderCS < 0.0F) viscositySliderCS = 0;
		if (viscositySliderCF < 0.0F) viscositySliderCF = 0;
		if (viscositySliderCR > 1.0F) viscositySliderCR = 1.0F;
		if (viscositySliderCG > 1.0F) viscositySliderCG = 1.0F;
		if (viscositySliderCB > 1.0F) viscositySliderCB = 1.0F;
		if (viscositySliderCW > 1.0F) viscositySliderCW = 1.0F;
		if (viscositySliderCO > 1.0F) viscositySliderCO = 1.0F;
		if (viscositySliderCK > 1.0F) viscositySliderCK = 1.0F;
		if (viscositySliderCC > 1.0F) viscositySliderCC = 1.0F;
		if (viscositySliderCD > 1.0F) viscositySliderCD = 1.0F;
		if (viscositySliderCY > 1.0F) viscositySliderCY = 1.0F;
		if (viscositySliderCP > 1.0F) viscositySliderCP = 1.0F;
		if (viscositySliderCM > 1.0F) viscositySliderCM = 1.0F;
		if (viscositySliderCA > 1.0F) viscositySliderCA = 1.0F;
		if (viscositySliderCT > 1.0F) viscositySliderCT = 1.0F;
		if (viscositySliderCS > 1.0F) viscositySliderCS = 1.0F;
		if (viscositySliderCF > 1.0F) viscositySliderCF = 1.0F;

		if (powerSliderDR < -100.0F) powerSliderDR = -100.0F;
		if (powerSliderDG < -100.0F) powerSliderDG = -100.0F;
		if (powerSliderDB < -100.0F) powerSliderDB = -100.0F;
		if (powerSliderDW < -100.0F) powerSliderDW = -100.0F;
		if (powerSliderDO < -100.0F) powerSliderDO = -100.0F;
		if (powerSliderDK < -100.0F) powerSliderDK = -100.0F;
		if (powerSliderDC < -100.0F) powerSliderDC = -100.0F;
		if (powerSliderDD < -100.0F) powerSliderDD = -100.0F;
		if (powerSliderDY < -100.0F) powerSliderDY = -100.0F;
		if (powerSliderDP < -100.0F) powerSliderDP = -100.0F;
		if (powerSliderDM < -100.0F) powerSliderDM = -100.0F;
		if (powerSliderDA < -100.0F) powerSliderDA = -100.0F;
		if (powerSliderDT < -100.0F) powerSliderDT = -100.0F;
		if (powerSliderDS < -100.0F) powerSliderDS = -100.0F;
		if (powerSliderDF < -100.0F) powerSliderDF = -100.0F;
		if (powerSliderDR > 100.0F) powerSliderDR = 100.0F;
		if (powerSliderDG > 100.0F) powerSliderDG = 100.0F;
		if (powerSliderDB > 100.0F) powerSliderDB = 100.0F;
		if (powerSliderDW > 100.0F) powerSliderDW = 100.0F;
		if (powerSliderDO > 100.0F) powerSliderDO = 100.0F;
		if (powerSliderDK > 100.0F) powerSliderDK = 100.0F;
		if (powerSliderDC > 100.0F) powerSliderDC = 100.0F;
		if (powerSliderDD > 100.0F) powerSliderDD = 100.0F;
		if (powerSliderDY > 100.0F) powerSliderDY = 100.0F;
		if (powerSliderDP > 100.0F) powerSliderDP = 100.0F;
		if (powerSliderDM > 100.0F) powerSliderDM = 100.0F;
		if (powerSliderDA > 100.0F) powerSliderDA = 100.0F;
		if (powerSliderDT > 100.0F) powerSliderDT = 100.0F;
		if (powerSliderDS > 100.0F) powerSliderDS = 100.0F;
		if (powerSliderDF > 100.0F) powerSliderDF = 100.0F;
		if (vSliderDR < 0.0F) vSliderDR = 0;
		if (vSliderDG < 0.0F) vSliderDG = 0;
		if (vSliderDB < 0.0F) vSliderDB = 0;
		if (vSliderDW < 0.0F) vSliderDW = 0;
		if (vSliderDO < 0.0F) vSliderDO = 0;
		if (vSliderDK < 0.0F) vSliderDK = 0;
		if (vSliderDC < 0.0F) vSliderDC = 0;
		if (vSliderDD < 0.0F) vSliderDD = 0;
		if (vSliderDY < 0.0F) vSliderDY = 0;
		if (vSliderDP < 0.0F) vSliderDP = 0;
		if (vSliderDM < 0.0F) vSliderDM = 0;
		if (vSliderDA < 0.0F) vSliderDA = 0;
		if (vSliderDT < 0.0F) vSliderDT = 0;
		if (vSliderDS < 0.0F) vSliderDS = 0;
		if (vSliderDF < 0.0F) vSliderDF = 0;
		if (vSliderDR > 500.0F) vSliderDR = 500.0F;
		if (vSliderDG > 500.0F) vSliderDG = 500.0F;
		if (vSliderDB > 500.0F) vSliderDB = 500.0F;
		if (vSliderDW > 500.0F) vSliderDW = 500.0F;
		if (vSliderDO > 500.0F) vSliderDO = 500.0F;
		if (vSliderDK > 500.0F) vSliderDK = 500.0F;
		if (vSliderDC > 500.0F) vSliderDC = 500.0F;
		if (vSliderDD > 500.0F) vSliderDD = 500.0F;
		if (vSliderDY > 500.0F) vSliderDY = 500.0F;
		if (vSliderDP > 500.0F) vSliderDP = 500.0F;
		if (vSliderDM > 500.0F) vSliderDM = 500.0F;
		if (vSliderDA > 500.0F) vSliderDA = 500.0F;
		if (vSliderDT > 500.0F) vSliderDT = 500.0F;
		if (vSliderDS > 500.0F) vSliderDS = 500.0F;
		if (vSliderDF > 500.0F) vSliderDF = 500.0F;
		if (probabilitySliderDR < 0.0F) probabilitySliderDR = 0;
		if (probabilitySliderDG < 0.0F) probabilitySliderDG = 0;
		if (probabilitySliderDB < 0.0F) probabilitySliderDB = 0;
		if (probabilitySliderDW < 0.0F) probabilitySliderDW = 0;
		if (probabilitySliderDO < 0.0F) probabilitySliderDO = 0;
		if (probabilitySliderDK < 0.0F) probabilitySliderDK = 0;
		if (probabilitySliderDC < 0.0F) probabilitySliderDC = 0;
		if (probabilitySliderDD < 0.0F) probabilitySliderDD = 0;
		if (probabilitySliderDY < 0.0F) probabilitySliderDY = 0;
		if (probabilitySliderDP < 0.0F) probabilitySliderDP = 0;
		if (probabilitySliderDM < 0.0F) probabilitySliderDM = 0;
		if (probabilitySliderDA < 0.0F) probabilitySliderDA = 0;
		if (probabilitySliderDT < 0.0F) probabilitySliderDT = 0;
		if (probabilitySliderDS < 0.0F) probabilitySliderDS = 0;
		if (probabilitySliderDF < 0.0F) probabilitySliderDF = 0;
		if (probabilitySliderDR > 100.0F) probabilitySliderDR = 100.0F;
		if (probabilitySliderDG > 100.0F) probabilitySliderDG = 100.0F;
		if (probabilitySliderDB > 100.0F) probabilitySliderDB = 100.0F;
		if (probabilitySliderDW > 100.0F) probabilitySliderDW = 100.0F;
		if (probabilitySliderDO > 100.0F) probabilitySliderDO = 100.0F;
		if (probabilitySliderDK > 100.0F) probabilitySliderDK = 100.0F;
		if (probabilitySliderDC > 100.0F) probabilitySliderDC = 100.0F;
		if (probabilitySliderDD > 100.0F) probabilitySliderDD = 100.0F;
		if (probabilitySliderDY > 100.0F) probabilitySliderDY = 100.0F;
		if (probabilitySliderDP > 100.0F) probabilitySliderDP = 100.0F;
		if (probabilitySliderDM > 100.0F) probabilitySliderDM = 100.0F;
		if (probabilitySliderDA > 100.0F) probabilitySliderDA = 100.0F;
		if (probabilitySliderDT > 100.0F) probabilitySliderDT = 100.0F;
		if (probabilitySliderDS > 100.0F) probabilitySliderDS = 100.0F;
		if (probabilitySliderDF > 100.0F) probabilitySliderDF = 100.0F;
		if (viscositySliderDR < 0.0F) viscositySliderDR = 0;
		if (viscositySliderDG < 0.0F) viscositySliderDG = 0;
		if (viscositySliderDB < 0.0F) viscositySliderDB = 0;
		if (viscositySliderDW < 0.0F) viscositySliderDW = 0;
		if (viscositySliderDO < 0.0F) viscositySliderDO = 0;
		if (viscositySliderDK < 0.0F) viscositySliderDK = 0;
		if (viscositySliderDC < 0.0F) viscositySliderDC = 0;
		if (viscositySliderDD < 0.0F) viscositySliderDD = 0;
		if (viscositySliderDY < 0.0F) viscositySliderDY = 0;
		if (viscositySliderDP < 0.0F) viscositySliderDP = 0;
		if (viscositySliderDM < 0.0F) viscositySliderDM = 0;
		if (viscositySliderDA < 0.0F) viscositySliderDA = 0;
		if (viscositySliderDT < 0.0F) viscositySliderDT = 0;
		if (viscositySliderDS < 0.0F) viscositySliderDS = 0;
		if (viscositySliderDF < 0.0F) viscositySliderDF = 0;
		if (viscositySliderDR > 1.0F) viscositySliderDR = 1.0F;
		if (viscositySliderDG > 1.0F) viscositySliderDG = 1.0F;
		if (viscositySliderDB > 1.0F) viscositySliderDB = 1.0F;
		if (viscositySliderDW > 1.0F) viscositySliderDW = 1.0F;
		if (viscositySliderDO > 1.0F) viscositySliderDO = 1.0F;
		if (viscositySliderDK > 1.0F) viscositySliderDK = 1.0F;
		if (viscositySliderDC > 1.0F) viscositySliderDC = 1.0F;
		if (viscositySliderDD > 1.0F) viscositySliderDD = 1.0F;
		if (viscositySliderDY > 1.0F) viscositySliderDY = 1.0F;
		if (viscositySliderDP > 1.0F) viscositySliderDP = 1.0F;
		if (viscositySliderDM > 1.0F) viscositySliderDM = 1.0F;
		if (viscositySliderDA > 1.0F) viscositySliderDA = 1.0F;
		if (viscositySliderDT > 1.0F) viscositySliderDT = 1.0F;
		if (viscositySliderDS > 1.0F) viscositySliderDS = 1.0F;
		if (viscositySliderDF > 1.0F) viscositySliderDF = 1.0F;

		if (powerSliderYR < -100.0F) powerSliderYR = -100.0F;
		if (powerSliderYG < -100.0F) powerSliderYG = -100.0F;
		if (powerSliderYB < -100.0F) powerSliderYB = -100.0F;
		if (powerSliderYW < -100.0F) powerSliderYW = -100.0F;
		if (powerSliderYO < -100.0F) powerSliderYO = -100.0F;
		if (powerSliderYK < -100.0F) powerSliderYK = -100.0F;
		if (powerSliderYC < -100.0F) powerSliderYC = -100.0F;
		if (powerSliderYD < -100.0F) powerSliderYD = -100.0F;
		if (powerSliderYY < -100.0F) powerSliderYY = -100.0F;
		if (powerSliderYP < -100.0F) powerSliderYP = -100.0F;
		if (powerSliderYM < -100.0F) powerSliderYM = -100.0F;
		if (powerSliderYA < -100.0F) powerSliderYA = -100.0F;
		if (powerSliderYT < -100.0F) powerSliderYT = -100.0F;
		if (powerSliderYS < -100.0F) powerSliderYS = -100.0F;
		if (powerSliderYF < -100.0F) powerSliderYF = -100.0F;
		if (powerSliderYR > 0.0F) powerSliderYR = 0.0F;
		if (powerSliderYG > 0.0F) powerSliderYG = 0.0F;
		if (powerSliderYB > 0.0F) powerSliderYB = 0.0F;
		if (powerSliderYW > 0.0F) powerSliderYW = 0.0F;
		if (powerSliderYO > 0.0F) powerSliderYO = 0.0F;
		if (powerSliderYK > 0.0F) powerSliderYK = 0.0F;
		if (powerSliderYC > 0.0F) powerSliderYC = 0.0F;
		if (powerSliderYD > 0.0F) powerSliderYD = 0.0F;
		if (powerSliderYY > 0.0F) powerSliderYY = 0.0F;
		if (powerSliderYP > 0.0F) powerSliderYP = 0.0F;
		if (powerSliderYM > 0.0F) powerSliderYM = 0.0F;
		if (powerSliderYA > 0.0F) powerSliderYA = 0.0F;
		if (powerSliderYT > 0.0F) powerSliderYT = 0.0F;
		if (powerSliderYS > 0.0F) powerSliderYS = 0.0F;
		if (powerSliderYF > 0.0F) powerSliderYF = 0.0F;
		if (vSliderYR < 0.0F) vSliderYR = 0;
		if (vSliderYG < 0.0F) vSliderYG = 0;
		if (vSliderYB < 0.0F) vSliderYB = 0;
		if (vSliderYW < 0.0F) vSliderYW = 0;
		if (vSliderYO < 0.0F) vSliderYO = 0;
		if (vSliderYK < 0.0F) vSliderYK = 0;
		if (vSliderYC < 0.0F) vSliderYC = 0;
		if (vSliderYD < 0.0F) vSliderYD = 0;
		if (vSliderYY < 0.0F) vSliderYY = 0;
		if (vSliderYP < 0.0F) vSliderYP = 0;
		if (vSliderYM < 0.0F) vSliderYM = 0;
		if (vSliderYA < 0.0F) vSliderYA = 0;
		if (vSliderYT < 0.0F) vSliderYT = 0;
		if (vSliderYS < 0.0F) vSliderYS = 0;
		if (vSliderYF < 0.0F) vSliderYF = 0;
		if (vSliderYR > 500.0F) vSliderYR = 500.0F;
		if (vSliderYG > 500.0F) vSliderYG = 500.0F;
		if (vSliderYB > 500.0F) vSliderYB = 500.0F;
		if (vSliderYW > 500.0F) vSliderYW = 500.0F;
		if (vSliderYO > 500.0F) vSliderYO = 500.0F;
		if (vSliderYK > 500.0F) vSliderYK = 500.0F;
		if (vSliderYC > 500.0F) vSliderYC = 500.0F;
		if (vSliderYD > 500.0F) vSliderYD = 500.0F;
		if (vSliderYY > 500.0F) vSliderYY = 500.0F;
		if (vSliderYP > 500.0F) vSliderYP = 500.0F;
		if (vSliderYM > 500.0F) vSliderYM = 500.0F;
		if (vSliderYA > 500.0F) vSliderYA = 500.0F;
		if (vSliderYT > 500.0F) vSliderYT = 500.0F;
		if (vSliderYS > 500.0F) vSliderYS = 500.0F;
		if (vSliderYF > 500.0F) vSliderYF = 500.0F;
		if (probabilitySliderYR < 0.0F) probabilitySliderYR = 0;
		if (probabilitySliderYG < 0.0F) probabilitySliderYG = 0;
		if (probabilitySliderYB < 0.0F) probabilitySliderYB = 0;
		if (probabilitySliderYW < 0.0F) probabilitySliderYW = 0;
		if (probabilitySliderYO < 0.0F) probabilitySliderYO = 0;
		if (probabilitySliderYK < 0.0F) probabilitySliderYK = 0;
		if (probabilitySliderYC < 0.0F) probabilitySliderYC = 0;
		if (probabilitySliderYD < 0.0F) probabilitySliderYD = 0;
		if (probabilitySliderYY < 0.0F) probabilitySliderYY = 0;
		if (probabilitySliderYP < 0.0F) probabilitySliderYP = 0;
		if (probabilitySliderYM < 0.0F) probabilitySliderYM = 0;
		if (probabilitySliderYA < 0.0F) probabilitySliderYA = 0;
		if (probabilitySliderYT < 0.0F) probabilitySliderYT = 0;
		if (probabilitySliderYS < 0.0F) probabilitySliderYS = 0;
		if (probabilitySliderYF < 0.0F) probabilitySliderYF = 0;
		if (probabilitySliderYR > 100.0F) probabilitySliderYR = 100.0F;
		if (probabilitySliderYG > 100.0F) probabilitySliderYG = 100.0F;
		if (probabilitySliderYB > 100.0F) probabilitySliderYB = 100.0F;
		if (probabilitySliderYW > 100.0F) probabilitySliderYW = 100.0F;
		if (probabilitySliderYO > 100.0F) probabilitySliderYO = 100.0F;
		if (probabilitySliderYK > 100.0F) probabilitySliderYK = 100.0F;
		if (probabilitySliderYC > 100.0F) probabilitySliderYC = 100.0F;
		if (probabilitySliderYD > 100.0F) probabilitySliderYD = 100.0F;
		if (probabilitySliderYY > 100.0F) probabilitySliderYY = 100.0F;
		if (probabilitySliderYP > 100.0F) probabilitySliderYP = 100.0F;
		if (probabilitySliderYM > 100.0F) probabilitySliderYM = 100.0F;
		if (probabilitySliderYA > 100.0F) probabilitySliderYA = 100.0F;
		if (probabilitySliderYT > 100.0F) probabilitySliderYT = 100.0F;
		if (probabilitySliderYS > 100.0F) probabilitySliderYS = 100.0F;
		if (probabilitySliderYF > 100.0F) probabilitySliderYF = 100.0F;
		if (viscositySliderYR < 0.0F) viscositySliderYR = 0;
		if (viscositySliderYG < 0.0F) viscositySliderYG = 0;
		if (viscositySliderYB < 0.0F) viscositySliderYB = 0;
		if (viscositySliderYW < 0.0F) viscositySliderYW = 0;
		if (viscositySliderYO < 0.0F) viscositySliderYO = 0;
		if (viscositySliderYK < 0.0F) viscositySliderYK = 0;
		if (viscositySliderYC < 0.0F) viscositySliderYC = 0;
		if (viscositySliderYD < 0.0F) viscositySliderYD = 0;
		if (viscositySliderYY < 0.0F) viscositySliderYY = 0;
		if (viscositySliderYP < 0.0F) viscositySliderYP = 0;
		if (viscositySliderYM < 0.0F) viscositySliderYM = 0;
		if (viscositySliderYA < 0.0F) viscositySliderYA = 0;
		if (viscositySliderYT < 0.0F) viscositySliderYT = 0;
		if (viscositySliderYS < 0.0F) viscositySliderYS = 0;
		if (viscositySliderYF < 0.0F) viscositySliderYF = 0;
		if (viscositySliderYR > 1.0F) viscositySliderYR = 1.0F;
		if (viscositySliderYG > 1.0F) viscositySliderYG = 1.0F;
		if (viscositySliderYB > 1.0F) viscositySliderYB = 1.0F;
		if (viscositySliderYW > 1.0F) viscositySliderYW = 1.0F;
		if (viscositySliderYO > 1.0F) viscositySliderYO = 1.0F;
		if (viscositySliderYK > 1.0F) viscositySliderYK = 1.0F;
		if (viscositySliderYC > 1.0F) viscositySliderYC = 1.0F;
		if (viscositySliderYD > 1.0F) viscositySliderYD = 1.0F;
		if (viscositySliderYY > 1.0F) viscositySliderYY = 1.0F;
		if (viscositySliderYP > 1.0F) viscositySliderYP = 1.0F;
		if (viscositySliderYM > 1.0F) viscositySliderYM = 1.0F;
		if (viscositySliderYA > 1.0F) viscositySliderYA = 1.0F;
		if (viscositySliderYT > 1.0F) viscositySliderYT = 1.0F;
		if (viscositySliderYS > 1.0F) viscositySliderYS = 1.0F;
		if (viscositySliderYF > 1.0F) viscositySliderYF = 1.0F;

		if (powerSliderPR < -100.0F) powerSliderPR = -100.0F;
		if (powerSliderPG < -100.0F) powerSliderPG = -100.0F;
		if (powerSliderPB < -100.0F) powerSliderPB = -100.0F;
		if (powerSliderPW < -100.0F) powerSliderPW = -100.0F;
		if (powerSliderPO < -100.0F) powerSliderPO = -100.0F;
		if (powerSliderPK < -100.0F) powerSliderPK = -100.0F;
		if (powerSliderPC < -100.0F) powerSliderPC = -100.0F;
		if (powerSliderPD < -100.0F) powerSliderPD = -100.0F;
		if (powerSliderPY < -100.0F) powerSliderPY = -100.0F;
		if (powerSliderPP < -100.0F) powerSliderPP = -100.0F;
		if (powerSliderPM < -100.0F) powerSliderPM = -100.0F;
		if (powerSliderPA < -100.0F) powerSliderPA = -100.0F;
		if (powerSliderPT < -100.0F) powerSliderPT = -100.0F;
		if (powerSliderPS < -100.0F) powerSliderPS = -100.0F;
		if (powerSliderPF < -100.0F) powerSliderPF = -100.0F;
		if (powerSliderPR > 0.0F) powerSliderPR = 0.0F;
		if (powerSliderPG > 0.0F) powerSliderPG = 0.0F;
		if (powerSliderPB > 0.0F) powerSliderPB = 0.0F;
		if (powerSliderPW > 0.0F) powerSliderPW = 0.0F;
		if (powerSliderPO > 0.0F) powerSliderPO = 0.0F;
		if (powerSliderPK > 0.0F) powerSliderPK = 0.0F;
		if (powerSliderPC > 0.0F) powerSliderPC = 0.0F;
		if (powerSliderPD > 0.0F) powerSliderPD = 0.0F;
		if (powerSliderPY > 0.0F) powerSliderPY = 0.0F;
		if (powerSliderPP > 0.0F) powerSliderPP = 0.0F;
		if (powerSliderPM > 0.0F) powerSliderPM = 0.0F;
		if (powerSliderPA > 0.0F) powerSliderPA = 0.0F;
		if (powerSliderPT > 0.0F) powerSliderPT = 0.0F;
		if (powerSliderPS > 0.0F) powerSliderPS = 0.0F;
		if (powerSliderPF > 0.0F) powerSliderPF = 0.0F;
		if (vSliderPR < 0.0F) vSliderPR = 0;
		if (vSliderPG < 0.0F) vSliderPG = 0;
		if (vSliderPB < 0.0F) vSliderPB = 0;
		if (vSliderPW < 0.0F) vSliderPW = 0;
		if (vSliderPO < 0.0F) vSliderPO = 0;
		if (vSliderPK < 0.0F) vSliderPK = 0;
		if (vSliderPC < 0.0F) vSliderPC = 0;
		if (vSliderPD < 0.0F) vSliderPD = 0;
		if (vSliderPY < 0.0F) vSliderPY = 0;
		if (vSliderPP < 0.0F) vSliderPP = 0;
		if (vSliderPM < 0.0F) vSliderPM = 0;
		if (vSliderPA < 0.0F) vSliderPA = 0;
		if (vSliderPT < 0.0F) vSliderPT = 0;
		if (vSliderPS < 0.0F) vSliderPS = 0;
		if (vSliderPF < 0.0F) vSliderPF = 0;
		if (vSliderPR > 500.0F) vSliderPR = 500.0F;
		if (vSliderPG > 500.0F) vSliderPG = 500.0F;
		if (vSliderPB > 500.0F) vSliderPB = 500.0F;
		if (vSliderPW > 500.0F) vSliderPW = 500.0F;
		if (vSliderPO > 500.0F) vSliderPO = 500.0F;
		if (vSliderPK > 500.0F) vSliderPK = 500.0F;
		if (vSliderPC > 500.0F) vSliderPC = 500.0F;
		if (vSliderPD > 500.0F) vSliderPD = 500.0F;
		if (vSliderPY > 500.0F) vSliderPY = 500.0F;
		if (vSliderPP > 500.0F) vSliderPP = 500.0F;
		if (vSliderPM > 500.0F) vSliderPM = 500.0F;
		if (vSliderPA > 500.0F) vSliderPA = 500.0F;
		if (vSliderPT > 500.0F) vSliderPT = 500.0F;
		if (vSliderPS > 500.0F) vSliderPS = 500.0F;
		if (vSliderPF > 500.0F) vSliderPF = 500.0F;
		if (probabilitySliderPR < 0.0F) probabilitySliderPR = 0;
		if (probabilitySliderPG < 0.0F) probabilitySliderPG = 0;
		if (probabilitySliderPB < 0.0F) probabilitySliderPB = 0;
		if (probabilitySliderPW < 0.0F) probabilitySliderPW = 0;
		if (probabilitySliderPO < 0.0F) probabilitySliderPO = 0;
		if (probabilitySliderPK < 0.0F) probabilitySliderPK = 0;
		if (probabilitySliderPC < 0.0F) probabilitySliderPC = 0;
		if (probabilitySliderPD < 0.0F) probabilitySliderPD = 0;
		if (probabilitySliderPY < 0.0F) probabilitySliderPY = 0;
		if (probabilitySliderPP < 0.0F) probabilitySliderPP = 0;
		if (probabilitySliderPM < 0.0F) probabilitySliderPM = 0;
		if (probabilitySliderPA < 0.0F) probabilitySliderPA = 0;
		if (probabilitySliderPT < 0.0F) probabilitySliderPT = 0;
		if (probabilitySliderPS < 0.0F) probabilitySliderPS = 0;
		if (probabilitySliderPF < 0.0F) probabilitySliderPF = 0;
		if (probabilitySliderPR > 100.0F) probabilitySliderPR = 100.0F;
		if (probabilitySliderPG > 100.0F) probabilitySliderPG = 100.0F;
		if (probabilitySliderPB > 100.0F) probabilitySliderPB = 100.0F;
		if (probabilitySliderPW > 100.0F) probabilitySliderPW = 100.0F;
		if (probabilitySliderPO > 100.0F) probabilitySliderPO = 100.0F;
		if (probabilitySliderPK > 100.0F) probabilitySliderPK = 100.0F;
		if (probabilitySliderPC > 100.0F) probabilitySliderPC = 100.0F;
		if (probabilitySliderPD > 100.0F) probabilitySliderPD = 100.0F;
		if (probabilitySliderPY > 100.0F) probabilitySliderPY = 100.0F;
		if (probabilitySliderPP > 100.0F) probabilitySliderPP = 100.0F;
		if (probabilitySliderPM > 100.0F) probabilitySliderPM = 100.0F;
		if (probabilitySliderPA > 100.0F) probabilitySliderPA = 100.0F;
		if (probabilitySliderPT > 100.0F) probabilitySliderPT = 100.0F;
		if (probabilitySliderPS > 100.0F) probabilitySliderPS = 100.0F;
		if (probabilitySliderPF > 100.0F) probabilitySliderPF = 100.0F;
		if (viscositySliderPR < 0.0F) viscositySliderPR = 0;
		if (viscositySliderPG < 0.0F) viscositySliderPG = 0;
		if (viscositySliderPB < 0.0F) viscositySliderPB = 0;
		if (viscositySliderPW < 0.0F) viscositySliderPW = 0;
		if (viscositySliderPO < 0.0F) viscositySliderPO = 0;
		if (viscositySliderPK < 0.0F) viscositySliderPK = 0;
		if (viscositySliderPC < 0.0F) viscositySliderPC = 0;
		if (viscositySliderPD < 0.0F) viscositySliderPD = 0;
		if (viscositySliderPY < 0.0F) viscositySliderPY = 0;
		if (viscositySliderPP < 0.0F) viscositySliderPP = 0;
		if (viscositySliderPM < 0.0F) viscositySliderPM = 0;
		if (viscositySliderPA < 0.0F) viscositySliderPA = 0;
		if (viscositySliderPT < 0.0F) viscositySliderPT = 0;
		if (viscositySliderPS < 0.0F) viscositySliderPS = 0;
		if (viscositySliderPF < 0.0F) viscositySliderPF = 0;
		if (viscositySliderPR > 1.0F) viscositySliderPR = 1.0F;
		if (viscositySliderPG > 1.0F) viscositySliderPG = 1.0F;
		if (viscositySliderPB > 1.0F) viscositySliderPB = 1.0F;
		if (viscositySliderPW > 1.0F) viscositySliderPW = 1.0F;
		if (viscositySliderPO > 1.0F) viscositySliderPO = 1.0F;
		if (viscositySliderPK > 1.0F) viscositySliderPK = 1.0F;
		if (viscositySliderPC > 1.0F) viscositySliderPC = 1.0F;
		if (viscositySliderPD > 1.0F) viscositySliderPD = 1.0F;
		if (viscositySliderPY > 1.0F) viscositySliderPY = 1.0F;
		if (viscositySliderPP > 1.0F) viscositySliderPP = 1.0F;
		if (viscositySliderPM > 1.0F) viscositySliderPM = 1.0F;
		if (viscositySliderPA > 1.0F) viscositySliderPA = 1.0F;
		if (viscositySliderPT > 1.0F) viscositySliderPT = 1.0F;
		if (viscositySliderPS > 1.0F) viscositySliderPS = 1.0F;
		if (viscositySliderPF > 1.0F) viscositySliderPF = 1.0F;

		if (powerSliderMR < -100.0F) powerSliderMR = -100.0F;
		if (powerSliderMG < -100.0F) powerSliderMG = -100.0F;
		if (powerSliderMB < -100.0F) powerSliderMB = -100.0F;
		if (powerSliderMW < -100.0F) powerSliderMW = -100.0F;
		if (powerSliderMO < -100.0F) powerSliderMO = -100.0F;
		if (powerSliderMK < -100.0F) powerSliderMK = -100.0F;
		if (powerSliderMC < -100.0F) powerSliderMC = -100.0F;
		if (powerSliderMD < -100.0F) powerSliderMD = -100.0F;
		if (powerSliderMY < -100.0F) powerSliderMY = -100.0F;
		if (powerSliderMP < -100.0F) powerSliderMP = -100.0F;
		if (powerSliderMM < -100.0F) powerSliderMM = -100.0F;
		if (powerSliderMA < -100.0F) powerSliderMA = -100.0F;
		if (powerSliderMT < -100.0F) powerSliderMT = -100.0F;
		if (powerSliderMS < -100.0F) powerSliderMS = -100.0F;
		if (powerSliderMF < -100.0F) powerSliderMF = -100.0F;
		if (powerSliderMR > 0.0F) powerSliderMR = 0.0F;
		if (powerSliderMG > 0.0F) powerSliderMG = 0.0F;
		if (powerSliderMB > 0.0F) powerSliderMB = 0.0F;
		if (powerSliderMW > 0.0F) powerSliderMW = 0.0F;
		if (powerSliderMO > 0.0F) powerSliderMO = 0.0F;
		if (powerSliderMK > 0.0F) powerSliderMK = 0.0F;
		if (powerSliderMC > 0.0F) powerSliderMC = 0.0F;
		if (powerSliderMD > 0.0F) powerSliderMD = 0.0F;
		if (powerSliderMY > 0.0F) powerSliderMY = 0.0F;
		if (powerSliderMP > 0.0F) powerSliderMP = 0.0F;
		if (powerSliderMM > 0.0F) powerSliderMM = 0.0F;
		if (powerSliderMA > 0.0F) powerSliderMA = 0.0F;
		if (powerSliderMT > 0.0F) powerSliderMT = 0.0F;
		if (powerSliderMS > 0.0F) powerSliderMS = 0.0F;
		if (powerSliderMF > 0.0F) powerSliderMF = 0.0F;
		if (vSliderMR < 0.0F) vSliderMR = 0;
		if (vSliderMG < 0.0F) vSliderMG = 0;
		if (vSliderMB < 0.0F) vSliderMB = 0;
		if (vSliderMW < 0.0F) vSliderMW = 0;
		if (vSliderMO < 0.0F) vSliderMO = 0;
		if (vSliderMK < 0.0F) vSliderMK = 0;
		if (vSliderMC < 0.0F) vSliderMC = 0;
		if (vSliderMD < 0.0F) vSliderMD = 0;
		if (vSliderMY < 0.0F) vSliderMY = 0;
		if (vSliderMP < 0.0F) vSliderMP = 0;
		if (vSliderMM < 0.0F) vSliderMM = 0;
		if (vSliderMA < 0.0F) vSliderMA = 0;
		if (vSliderMT < 0.0F) vSliderMT = 0;
		if (vSliderMS < 0.0F) vSliderMS = 0;
		if (vSliderMF < 0.0F) vSliderMF = 0;
		if (vSliderMR > 500.0F) vSliderMR = 500.0F;
		if (vSliderMG > 500.0F) vSliderMG = 500.0F;
		if (vSliderMB > 500.0F) vSliderMB = 500.0F;
		if (vSliderMW > 500.0F) vSliderMW = 500.0F;
		if (vSliderMO > 500.0F) vSliderMO = 500.0F;
		if (vSliderMK > 500.0F) vSliderMK = 500.0F;
		if (vSliderMC > 500.0F) vSliderMC = 500.0F;
		if (vSliderMD > 500.0F) vSliderMD = 500.0F;
		if (vSliderMY > 500.0F) vSliderMY = 500.0F;
		if (vSliderMP > 500.0F) vSliderMP = 500.0F;
		if (vSliderMM > 500.0F) vSliderMM = 500.0F;
		if (vSliderMA > 500.0F) vSliderMA = 500.0F;
		if (vSliderMT > 500.0F) vSliderMT = 500.0F;
		if (vSliderMS > 500.0F) vSliderMS = 500.0F;
		if (vSliderMF > 500.0F) vSliderMF = 500.0F;
		if (probabilitySliderMR < 0.0F) probabilitySliderMR = 0;
		if (probabilitySliderMG < 0.0F) probabilitySliderMG = 0;
		if (probabilitySliderMB < 0.0F) probabilitySliderMB = 0;
		if (probabilitySliderMW < 0.0F) probabilitySliderMW = 0;
		if (probabilitySliderMO < 0.0F) probabilitySliderMO = 0;
		if (probabilitySliderMK < 0.0F) probabilitySliderMK = 0;
		if (probabilitySliderMC < 0.0F) probabilitySliderMC = 0;
		if (probabilitySliderMD < 0.0F) probabilitySliderMD = 0;
		if (probabilitySliderMY < 0.0F) probabilitySliderMY = 0;
		if (probabilitySliderMP < 0.0F) probabilitySliderMP = 0;
		if (probabilitySliderMM < 0.0F) probabilitySliderMM = 0;
		if (probabilitySliderMA < 0.0F) probabilitySliderMA = 0;
		if (probabilitySliderMT < 0.0F) probabilitySliderMT = 0;
		if (probabilitySliderMS < 0.0F) probabilitySliderMS = 0;
		if (probabilitySliderMF < 0.0F) probabilitySliderMF = 0;
		if (probabilitySliderMR > 100.0F) probabilitySliderMR = 100.0F;
		if (probabilitySliderMG > 100.0F) probabilitySliderMG = 100.0F;
		if (probabilitySliderMB > 100.0F) probabilitySliderMB = 100.0F;
		if (probabilitySliderMW > 100.0F) probabilitySliderMW = 100.0F;
		if (probabilitySliderMO > 100.0F) probabilitySliderMO = 100.0F;
		if (probabilitySliderMK > 100.0F) probabilitySliderMK = 100.0F;
		if (probabilitySliderMC > 100.0F) probabilitySliderMC = 100.0F;
		if (probabilitySliderMD > 100.0F) probabilitySliderMD = 100.0F;
		if (probabilitySliderMY > 100.0F) probabilitySliderMY = 100.0F;
		if (probabilitySliderMP > 100.0F) probabilitySliderMP = 100.0F;
		if (probabilitySliderMM > 100.0F) probabilitySliderMM = 100.0F;
		if (probabilitySliderMA > 100.0F) probabilitySliderMA = 100.0F;
		if (probabilitySliderMT > 100.0F) probabilitySliderMT = 100.0F;
		if (probabilitySliderMS > 100.0F) probabilitySliderMS = 100.0F;
		if (probabilitySliderMF > 100.0F) probabilitySliderMF = 100.0F;
		if (viscositySliderMR < 0.0F) viscositySliderMR = 0;
		if (viscositySliderMG < 0.0F) viscositySliderMG = 0;
		if (viscositySliderMB < 0.0F) viscositySliderMB = 0;
		if (viscositySliderMW < 0.0F) viscositySliderMW = 0;
		if (viscositySliderMO < 0.0F) viscositySliderMO = 0;
		if (viscositySliderMK < 0.0F) viscositySliderMK = 0;
		if (viscositySliderMC < 0.0F) viscositySliderMC = 0;
		if (viscositySliderMD < 0.0F) viscositySliderMD = 0;
		if (viscositySliderMY < 0.0F) viscositySliderMY = 0;
		if (viscositySliderMP < 0.0F) viscositySliderMP = 0;
		if (viscositySliderMM < 0.0F) viscositySliderMM = 0;
		if (viscositySliderMA < 0.0F) viscositySliderMA = 0;
		if (viscositySliderMT < 0.0F) viscositySliderMT = 0;
		if (viscositySliderMS < 0.0F) viscositySliderMS = 0;
		if (viscositySliderMF < 0.0F) viscositySliderMF = 0;
		if (viscositySliderMR > 1.0F) viscositySliderMR = 1.0F;
		if (viscositySliderMG > 1.0F) viscositySliderMG = 1.0F;
		if (viscositySliderMB > 1.0F) viscositySliderMB = 1.0F;
		if (viscositySliderMW > 1.0F) viscositySliderMW = 1.0F;
		if (viscositySliderMO > 1.0F) viscositySliderMO = 1.0F;
		if (viscositySliderMK > 1.0F) viscositySliderMK = 1.0F;
		if (viscositySliderMC > 1.0F) viscositySliderMC = 1.0F;
		if (viscositySliderMD > 1.0F) viscositySliderMD = 1.0F;
		if (viscositySliderMY > 1.0F) viscositySliderMY = 1.0F;
		if (viscositySliderMP > 1.0F) viscositySliderMP = 1.0F;
		if (viscositySliderMM > 1.0F) viscositySliderMM = 1.0F;
		if (viscositySliderMA > 1.0F) viscositySliderMA = 1.0F;
		if (viscositySliderMT > 1.0F) viscositySliderMT = 1.0F;
		if (viscositySliderMS > 1.0F) viscositySliderMS = 1.0F;
		if (viscositySliderMF > 1.0F) viscositySliderMF = 1.0F;

		if (powerSliderAR < -100.0F) powerSliderAR = -100.0F;
		if (powerSliderAG < -100.0F) powerSliderAG = -100.0F;
		if (powerSliderAB < -100.0F) powerSliderAB = -100.0F;
		if (powerSliderAW < -100.0F) powerSliderAW = -100.0F;
		if (powerSliderAO < -100.0F) powerSliderAO = -100.0F;
		if (powerSliderAK < -100.0F) powerSliderAK = -100.0F;
		if (powerSliderAC < -100.0F) powerSliderAC = -100.0F;
		if (powerSliderAD < -100.0F) powerSliderAD = -100.0F;
		if (powerSliderAY < -100.0F) powerSliderAY = -100.0F;
		if (powerSliderAP < -100.0F) powerSliderAP = -100.0F;
		if (powerSliderAM < -100.0F) powerSliderAM = -100.0F;
		if (powerSliderAA < -100.0F) powerSliderAA = -100.0F;
		if (powerSliderAT < -100.0F) powerSliderAT = -100.0F;
		if (powerSliderAS < -100.0F) powerSliderAS = -100.0F;
		if (powerSliderAF < -100.0F) powerSliderAF = -100.0F;
		if (powerSliderAR > 0.0F) powerSliderAR = 0.0F;
		if (powerSliderAG > 0.0F) powerSliderAG = 0.0F;
		if (powerSliderAB > 0.0F) powerSliderAB = 0.0F;
		if (powerSliderAW > 0.0F) powerSliderAW = 0.0F;
		if (powerSliderAO > 0.0F) powerSliderAO = 0.0F;
		if (powerSliderAK > 0.0F) powerSliderAK = 0.0F;
		if (powerSliderAC > 0.0F) powerSliderAC = 0.0F;
		if (powerSliderAD > 0.0F) powerSliderAD = 0.0F;
		if (powerSliderAY > 0.0F) powerSliderAY = 0.0F;
		if (powerSliderAP > 0.0F) powerSliderAP = 0.0F;
		if (powerSliderAM > 0.0F) powerSliderAM = 0.0F;
		if (powerSliderAA > 0.0F) powerSliderAA = 0.0F;
		if (powerSliderAT > 0.0F) powerSliderAT = 0.0F;
		if (powerSliderAS > 0.0F) powerSliderAS = 0.0F;
		if (powerSliderAF > 0.0F) powerSliderAF = 0.0F;
		if (vSliderAR < 0.0F) vSliderAR = 0;
		if (vSliderAG < 0.0F) vSliderAG = 0;
		if (vSliderAB < 0.0F) vSliderAB = 0;
		if (vSliderAW < 0.0F) vSliderAW = 0;
		if (vSliderAO < 0.0F) vSliderAO = 0;
		if (vSliderAK < 0.0F) vSliderAK = 0;
		if (vSliderAC < 0.0F) vSliderAC = 0;
		if (vSliderAD < 0.0F) vSliderAD = 0;
		if (vSliderAY < 0.0F) vSliderAY = 0;
		if (vSliderAP < 0.0F) vSliderAP = 0;
		if (vSliderAM < 0.0F) vSliderAM = 0;
		if (vSliderAA < 0.0F) vSliderAA = 0;
		if (vSliderAT < 0.0F) vSliderAT = 0;
		if (vSliderAS < 0.0F) vSliderAS = 0;
		if (vSliderAF < 0.0F) vSliderAF = 0;
		if (vSliderAR > 500.0F) vSliderAR = 500.0F;
		if (vSliderAG > 500.0F) vSliderAG = 500.0F;
		if (vSliderAB > 500.0F) vSliderAB = 500.0F;
		if (vSliderAW > 500.0F) vSliderAW = 500.0F;
		if (vSliderAO > 500.0F) vSliderAO = 500.0F;
		if (vSliderAK > 500.0F) vSliderAK = 500.0F;
		if (vSliderAC > 500.0F) vSliderAC = 500.0F;
		if (vSliderAD > 500.0F) vSliderAD = 500.0F;
		if (vSliderAY > 500.0F) vSliderAY = 500.0F;
		if (vSliderAP > 500.0F) vSliderAP = 500.0F;
		if (vSliderAM > 500.0F) vSliderAM = 500.0F;
		if (vSliderAA > 500.0F) vSliderAA = 500.0F;
		if (vSliderAT > 500.0F) vSliderAT = 500.0F;
		if (vSliderAS > 500.0F) vSliderAS = 500.0F;
		if (vSliderAF > 500.0F) vSliderAF = 500.0F;
		if (probabilitySliderAR < 0.0F) probabilitySliderAR = 0;
		if (probabilitySliderAG < 0.0F) probabilitySliderAG = 0;
		if (probabilitySliderAB < 0.0F) probabilitySliderAB = 0;
		if (probabilitySliderAW < 0.0F) probabilitySliderAW = 0;
		if (probabilitySliderAO < 0.0F) probabilitySliderAO = 0;
		if (probabilitySliderAK < 0.0F) probabilitySliderAK = 0;
		if (probabilitySliderAC < 0.0F) probabilitySliderAC = 0;
		if (probabilitySliderAD < 0.0F) probabilitySliderAD = 0;
		if (probabilitySliderAY < 0.0F) probabilitySliderAY = 0;
		if (probabilitySliderAP < 0.0F) probabilitySliderAP = 0;
		if (probabilitySliderAM < 0.0F) probabilitySliderAM = 0;
		if (probabilitySliderAA < 0.0F) probabilitySliderAA = 0;
		if (probabilitySliderAT < 0.0F) probabilitySliderAT = 0;
		if (probabilitySliderAS < 0.0F) probabilitySliderAS = 0;
		if (probabilitySliderAF < 0.0F) probabilitySliderAF = 0;
		if (probabilitySliderAR > 100.0F) probabilitySliderAR = 100.0F;
		if (probabilitySliderAG > 100.0F) probabilitySliderAG = 100.0F;
		if (probabilitySliderAB > 100.0F) probabilitySliderAB = 100.0F;
		if (probabilitySliderAW > 100.0F) probabilitySliderAW = 100.0F;
		if (probabilitySliderAO > 100.0F) probabilitySliderAO = 100.0F;
		if (probabilitySliderAK > 100.0F) probabilitySliderAK = 100.0F;
		if (probabilitySliderAC > 100.0F) probabilitySliderAC = 100.0F;
		if (probabilitySliderAD > 100.0F) probabilitySliderAD = 100.0F;
		if (probabilitySliderAY > 100.0F) probabilitySliderAY = 100.0F;
		if (probabilitySliderAP > 100.0F) probabilitySliderAP = 100.0F;
		if (probabilitySliderAM > 100.0F) probabilitySliderAM = 100.0F;
		if (probabilitySliderAA > 100.0F) probabilitySliderAA = 100.0F;
		if (probabilitySliderAT > 100.0F) probabilitySliderAT = 100.0F;
		if (probabilitySliderAS > 100.0F) probabilitySliderAS = 100.0F;
		if (probabilitySliderAF > 100.0F) probabilitySliderAF = 100.0F;
		if (viscositySliderAR < 0.0F) viscositySliderAR = 0;
		if (viscositySliderAG < 0.0F) viscositySliderAG = 0;
		if (viscositySliderAB < 0.0F) viscositySliderAB = 0;
		if (viscositySliderAW < 0.0F) viscositySliderAW = 0;
		if (viscositySliderAO < 0.0F) viscositySliderAO = 0;
		if (viscositySliderAK < 0.0F) viscositySliderAK = 0;
		if (viscositySliderAC < 0.0F) viscositySliderAC = 0;
		if (viscositySliderAD < 0.0F) viscositySliderAD = 0;
		if (viscositySliderAY < 0.0F) viscositySliderAY = 0;
		if (viscositySliderAP < 0.0F) viscositySliderAP = 0;
		if (viscositySliderAM < 0.0F) viscositySliderAM = 0;
		if (viscositySliderAA < 0.0F) viscositySliderAA = 0;
		if (viscositySliderAT < 0.0F) viscositySliderAT = 0;
		if (viscositySliderAS < 0.0F) viscositySliderAS = 0;
		if (viscositySliderAF < 0.0F) viscositySliderAF = 0;
		if (viscositySliderAR > 1.0F) viscositySliderAR = 1.0F;
		if (viscositySliderAG > 1.0F) viscositySliderAG = 1.0F;
		if (viscositySliderAB > 1.0F) viscositySliderAB = 1.0F;
		if (viscositySliderAW > 1.0F) viscositySliderAW = 1.0F;
		if (viscositySliderAO > 1.0F) viscositySliderAO = 1.0F;
		if (viscositySliderAK > 1.0F) viscositySliderAK = 1.0F;
		if (viscositySliderAC > 1.0F) viscositySliderAC = 1.0F;
		if (viscositySliderAD > 1.0F) viscositySliderAD = 1.0F;
		if (viscositySliderAY > 1.0F) viscositySliderAY = 1.0F;
		if (viscositySliderAP > 1.0F) viscositySliderAP = 1.0F;
		if (viscositySliderAM > 1.0F) viscositySliderAM = 1.0F;
		if (viscositySliderAA > 1.0F) viscositySliderAA = 1.0F;
		if (viscositySliderAT > 1.0F) viscositySliderAT = 1.0F;
		if (viscositySliderAS > 1.0F) viscositySliderAS = 1.0F;
		if (viscositySliderAF > 1.0F) viscositySliderAF = 1.0F;

		if (powerSliderTR < 0.0F) powerSliderTR = 0;
		if (powerSliderTG < 0.0F) powerSliderTG = 0;
		if (powerSliderTB < 0.0F) powerSliderTB = 0;
		if (powerSliderTW < 0.0F) powerSliderTW = 0;
		if (powerSliderTO < 0.0F) powerSliderTO = 0;
		if (powerSliderTK < 0.0F) powerSliderTK = 0;
		if (powerSliderTC < 0.0F) powerSliderTC = 0;
		if (powerSliderTD < 0.0F) powerSliderTD = 0;
		if (powerSliderTY < 0.0F) powerSliderTY = 0;
		if (powerSliderTP < 0.0F) powerSliderTP = 0;
		if (powerSliderTM < 0.0F) powerSliderTM = 0;
		if (powerSliderTA < 0.0F) powerSliderTA = 0;
		if (powerSliderTT < 0.0F) powerSliderTT = 0;
		if (powerSliderTS < 0.0F) powerSliderTS = 0;
		if (powerSliderTF < 0.0F) powerSliderTF = 0;
		if (powerSliderTR > 100.0F) powerSliderTR = 100.0F;
		if (powerSliderTG > 100.0F) powerSliderTG = 100.0F;
		if (powerSliderTB > 100.0F) powerSliderTB = 100.0F;
		if (powerSliderTW > 100.0F) powerSliderTW = 100.0F;
		if (powerSliderTO > 100.0F) powerSliderTO = 100.0F;
		if (powerSliderTK > 100.0F) powerSliderTK = 100.0F;
		if (powerSliderTC > 100.0F) powerSliderTC = 100.0F;
		if (powerSliderTD > 100.0F) powerSliderTD = 100.0F;
		if (powerSliderTY > 100.0F) powerSliderTY = 100.0F;
		if (powerSliderTP > 100.0F) powerSliderTP = 100.0F;
		if (powerSliderTM > 100.0F) powerSliderTM = 100.0F;
		if (powerSliderTA > 100.0F) powerSliderTA = 100.0F;
		if (powerSliderTT > 100.0F) powerSliderTT = 100.0F;
		if (powerSliderTS > 100.0F) powerSliderTS = 100.0F;
		if (powerSliderTF > 100.0F) powerSliderTF = 100.0F;
		if (vSliderTR < 0.0F) vSliderTR = 0;
		if (vSliderTG < 0.0F) vSliderTG = 0;
		if (vSliderTB < 0.0F) vSliderTB = 0;
		if (vSliderTW < 0.0F) vSliderTW = 0;
		if (vSliderTO < 0.0F) vSliderTO = 0;
		if (vSliderTK < 0.0F) vSliderTK = 0;
		if (vSliderTC < 0.0F) vSliderTC = 0;
		if (vSliderTD < 0.0F) vSliderTD = 0;
		if (vSliderTY < 0.0F) vSliderTY = 0;
		if (vSliderTP < 0.0F) vSliderTP = 0;
		if (vSliderTM < 0.0F) vSliderTM = 0;
		if (vSliderTA < 0.0F) vSliderTA = 0;
		if (vSliderTT < 0.0F) vSliderTT = 0;
		if (vSliderTS < 0.0F) vSliderTS = 0;
		if (vSliderTF < 0.0F) vSliderTF = 0;
		if (vSliderTR > 500.0F) vSliderTR = 500.0F;
		if (vSliderTG > 500.0F) vSliderTG = 500.0F;
		if (vSliderTB > 500.0F) vSliderTB = 500.0F;
		if (vSliderTW > 500.0F) vSliderTW = 500.0F;
		if (vSliderTO > 500.0F) vSliderTO = 500.0F;
		if (vSliderTK > 500.0F) vSliderTK = 500.0F;
		if (vSliderTC > 500.0F) vSliderTC = 500.0F;
		if (vSliderTD > 500.0F) vSliderTD = 500.0F;
		if (vSliderTY > 500.0F) vSliderTY = 500.0F;
		if (vSliderTP > 500.0F) vSliderTP = 500.0F;
		if (vSliderTM > 500.0F) vSliderTM = 500.0F;
		if (vSliderTA > 500.0F) vSliderTA = 500.0F;
		if (vSliderTT > 500.0F) vSliderTT = 500.0F;
		if (vSliderTS > 500.0F) vSliderTS = 500.0F;
		if (vSliderTF > 500.0F) vSliderTF = 500.0F;
		if (probabilitySliderTR < 0.0F) probabilitySliderTR = 0;
		if (probabilitySliderTG < 0.0F) probabilitySliderTG = 0;
		if (probabilitySliderTB < 0.0F) probabilitySliderTB = 0;
		if (probabilitySliderTW < 0.0F) probabilitySliderTW = 0;
		if (probabilitySliderTO < 0.0F) probabilitySliderTO = 0;
		if (probabilitySliderTK < 0.0F) probabilitySliderTK = 0;
		if (probabilitySliderTC < 0.0F) probabilitySliderTC = 0;
		if (probabilitySliderTD < 0.0F) probabilitySliderTD = 0;
		if (probabilitySliderTY < 0.0F) probabilitySliderTY = 0;
		if (probabilitySliderTP < 0.0F) probabilitySliderTP = 0;
		if (probabilitySliderTM < 0.0F) probabilitySliderTM = 0;
		if (probabilitySliderTA < 0.0F) probabilitySliderTA = 0;
		if (probabilitySliderTT < 0.0F) probabilitySliderTT = 0;
		if (probabilitySliderTS < 0.0F) probabilitySliderTS = 0;
		if (probabilitySliderTF < 0.0F) probabilitySliderTF = 0;
		if (probabilitySliderTR > 100.0F) probabilitySliderTR = 100.0F;
		if (probabilitySliderTG > 100.0F) probabilitySliderTG = 100.0F;
		if (probabilitySliderTB > 100.0F) probabilitySliderTB = 100.0F;
		if (probabilitySliderTW > 100.0F) probabilitySliderTW = 100.0F;
		if (probabilitySliderTO > 100.0F) probabilitySliderTO = 100.0F;
		if (probabilitySliderTK > 100.0F) probabilitySliderTK = 100.0F;
		if (probabilitySliderTC > 100.0F) probabilitySliderTC = 100.0F;
		if (probabilitySliderTD > 100.0F) probabilitySliderTD = 100.0F;
		if (probabilitySliderTY > 100.0F) probabilitySliderTY = 100.0F;
		if (probabilitySliderTP > 100.0F) probabilitySliderTP = 100.0F;
		if (probabilitySliderTM > 100.0F) probabilitySliderTM = 100.0F;
		if (probabilitySliderTA > 100.0F) probabilitySliderTA = 100.0F;
		if (probabilitySliderTT > 100.0F) probabilitySliderTT = 100.0F;
		if (probabilitySliderTS > 100.0F) probabilitySliderTS = 100.0F;
		if (probabilitySliderTF > 100.0F) probabilitySliderTF = 100.0F;
		if (viscositySliderTR < 0.0F) viscositySliderTR = 0;
		if (viscositySliderTG < 0.0F) viscositySliderTG = 0;
		if (viscositySliderTB < 0.0F) viscositySliderTB = 0;
		if (viscositySliderTW < 0.0F) viscositySliderTW = 0;
		if (viscositySliderTO < 0.0F) viscositySliderTO = 0;
		if (viscositySliderTK < 0.0F) viscositySliderTK = 0;
		if (viscositySliderTC < 0.0F) viscositySliderTC = 0;
		if (viscositySliderTD < 0.0F) viscositySliderTD = 0;
		if (viscositySliderTY < 0.0F) viscositySliderTY = 0;
		if (viscositySliderTP < 0.0F) viscositySliderTP = 0;
		if (viscositySliderTM < 0.0F) viscositySliderTM = 0;
		if (viscositySliderTA < 0.0F) viscositySliderTA = 0;
		if (viscositySliderTT < 0.0F) viscositySliderTT = 0;
		if (viscositySliderTS < 0.0F) viscositySliderTS = 0;
		if (viscositySliderTF < 0.0F) viscositySliderTF = 0;
		if (viscositySliderTR > 1.0F) viscositySliderTR = 1.0F;
		if (viscositySliderTG > 1.0F) viscositySliderTG = 1.0F;
		if (viscositySliderTB > 1.0F) viscositySliderTB = 1.0F;
		if (viscositySliderTW > 1.0F) viscositySliderTW = 1.0F;
		if (viscositySliderTO > 1.0F) viscositySliderTO = 1.0F;
		if (viscositySliderTK > 1.0F) viscositySliderTK = 1.0F;
		if (viscositySliderTC > 1.0F) viscositySliderTC = 1.0F;
		if (viscositySliderTD > 1.0F) viscositySliderTD = 1.0F;
		if (viscositySliderTY > 1.0F) viscositySliderTY = 1.0F;
		if (viscositySliderTP > 1.0F) viscositySliderTP = 1.0F;
		if (viscositySliderTM > 1.0F) viscositySliderTM = 1.0F;
		if (viscositySliderTA > 1.0F) viscositySliderTA = 1.0F;
		if (viscositySliderTT > 1.0F) viscositySliderTT = 1.0F;
		if (viscositySliderTS > 1.0F) viscositySliderTS = 1.0F;
		if (viscositySliderTF > 1.0F) viscositySliderTF = 1.0F;

		if (powerSliderSR < -100.0F) powerSliderSR = -100.0F;
		if (powerSliderSG < -100.0F) powerSliderSG = -100.0F;
		if (powerSliderSB < -100.0F) powerSliderSB = -100.0F;
		if (powerSliderSW < -100.0F) powerSliderSW = -100.0F;
		if (powerSliderSO < -100.0F) powerSliderSO = -100.0F;
		if (powerSliderSK < -100.0F) powerSliderSK = -100.0F;
		if (powerSliderSC < -100.0F) powerSliderSC = -100.0F;
		if (powerSliderSD < -100.0F) powerSliderSD = -100.0F;
		if (powerSliderSY < -100.0F) powerSliderSY = -100.0F;
		if (powerSliderSP < -100.0F) powerSliderSP = -100.0F;
		if (powerSliderSM < -100.0F) powerSliderSM = -100.0F;
		if (powerSliderSA < -100.0F) powerSliderSA = -100.0F;
		if (powerSliderST < -100.0F) powerSliderST = -100.0F;
		if (powerSliderSS < -100.0F) powerSliderSS = -100.0F;
		if (powerSliderSF < -100.0F) powerSliderSF = -100.0F;
		if (powerSliderSR > 100.0F) powerSliderSR = 100.0F;
		if (powerSliderSG > 100.0F) powerSliderSG = 100.0F;
		if (powerSliderSB > 100.0F) powerSliderSB = 100.0F;
		if (powerSliderSW > 100.0F) powerSliderSW = 100.0F;
		if (powerSliderSO > 100.0F) powerSliderSO = 100.0F;
		if (powerSliderSK > 100.0F) powerSliderSK = 100.0F;
		if (powerSliderSC > 100.0F) powerSliderSC = 100.0F;
		if (powerSliderSD > 100.0F) powerSliderSD = 100.0F;
		if (powerSliderSY > 100.0F) powerSliderSY = 100.0F;
		if (powerSliderSP > 100.0F) powerSliderSP = 100.0F;
		if (powerSliderSM > 100.0F) powerSliderSM = 100.0F;
		if (powerSliderSA > 100.0F) powerSliderSA = 100.0F;
		if (powerSliderST > 100.0F) powerSliderST = 100.0F;
		if (powerSliderSS > 100.0F) powerSliderSS = 100.0F;
		if (powerSliderSF > 100.0F) powerSliderSF = 100.0F;
		if (vSliderSR < 0.0F) vSliderSR = 0;
		if (vSliderSG < 0.0F) vSliderSG = 0;
		if (vSliderSB < 0.0F) vSliderSB = 0;
		if (vSliderSW < 0.0F) vSliderSW = 0;
		if (vSliderSO < 0.0F) vSliderSO = 0;
		if (vSliderSK < 0.0F) vSliderSK = 0;
		if (vSliderSC < 0.0F) vSliderSC = 0;
		if (vSliderSD < 0.0F) vSliderSD = 0;
		if (vSliderSY < 0.0F) vSliderSY = 0;
		if (vSliderSP < 0.0F) vSliderSP = 0;
		if (vSliderSM < 0.0F) vSliderSM = 0;
		if (vSliderSA < 0.0F) vSliderSA = 0;
		if (vSliderST < 0.0F) vSliderST = 0;
		if (vSliderSS < 0.0F) vSliderSS = 0;
		if (vSliderSF < 0.0F) vSliderSF = 0;
		if (vSliderSR > 500.0F) vSliderSR = 500.0F;
		if (vSliderSG > 500.0F) vSliderSG = 500.0F;
		if (vSliderSB > 500.0F) vSliderSB = 500.0F;
		if (vSliderSW > 500.0F) vSliderSW = 500.0F;
		if (vSliderSO > 500.0F) vSliderSO = 500.0F;
		if (vSliderSK > 500.0F) vSliderSK = 500.0F;
		if (vSliderSC > 500.0F) vSliderSC = 500.0F;
		if (vSliderSD > 500.0F) vSliderSD = 500.0F;
		if (vSliderSY > 500.0F) vSliderSY = 500.0F;
		if (vSliderSP > 500.0F) vSliderSP = 500.0F;
		if (vSliderSM > 500.0F) vSliderSM = 500.0F;
		if (vSliderSA > 500.0F) vSliderSA = 500.0F;
		if (vSliderST > 500.0F) vSliderST = 500.0F;
		if (vSliderSS > 500.0F) vSliderSS = 500.0F;
		if (vSliderSF > 500.0F) vSliderSF = 500.0F;
		if (probabilitySliderSR < 0.0F) probabilitySliderSR = 0;
		if (probabilitySliderSG < 0.0F) probabilitySliderSG = 0;
		if (probabilitySliderSB < 0.0F) probabilitySliderSB = 0;
		if (probabilitySliderSW < 0.0F) probabilitySliderSW = 0;
		if (probabilitySliderSO < 0.0F) probabilitySliderSO = 0;
		if (probabilitySliderSK < 0.0F) probabilitySliderSK = 0;
		if (probabilitySliderSC < 0.0F) probabilitySliderSC = 0;
		if (probabilitySliderSD < 0.0F) probabilitySliderSD = 0;
		if (probabilitySliderSY < 0.0F) probabilitySliderSY = 0;
		if (probabilitySliderSP < 0.0F) probabilitySliderSP = 0;
		if (probabilitySliderSM < 0.0F) probabilitySliderSM = 0;
		if (probabilitySliderSA < 0.0F) probabilitySliderSA = 0;
		if (probabilitySliderST < 0.0F) probabilitySliderST = 0;
		if (probabilitySliderSS < 0.0F) probabilitySliderSS = 0;
		if (probabilitySliderSF < 0.0F) probabilitySliderSF = 0;
		if (probabilitySliderSR > 100.0F) probabilitySliderSR = 100.0F;
		if (probabilitySliderSG > 100.0F) probabilitySliderSG = 100.0F;
		if (probabilitySliderSB > 100.0F) probabilitySliderSB = 100.0F;
		if (probabilitySliderSW > 100.0F) probabilitySliderSW = 100.0F;
		if (probabilitySliderSO > 100.0F) probabilitySliderSO = 100.0F;
		if (probabilitySliderSK > 100.0F) probabilitySliderSK = 100.0F;
		if (probabilitySliderSC > 100.0F) probabilitySliderSC = 100.0F;
		if (probabilitySliderSD > 100.0F) probabilitySliderSD = 100.0F;
		if (probabilitySliderSY > 100.0F) probabilitySliderSY = 100.0F;
		if (probabilitySliderSP > 100.0F) probabilitySliderSP = 100.0F;
		if (probabilitySliderSM > 100.0F) probabilitySliderSM = 100.0F;
		if (probabilitySliderSA > 100.0F) probabilitySliderSA = 100.0F;
		if (probabilitySliderST > 100.0F) probabilitySliderST = 100.0F;
		if (probabilitySliderSS > 100.0F) probabilitySliderSS = 100.0F;
		if (probabilitySliderSF > 100.0F) probabilitySliderSF = 100.0F;
		if (viscositySliderSR < 0.0F) viscositySliderSR = 0;
		if (viscositySliderSG < 0.0F) viscositySliderSG = 0;
		if (viscositySliderSB < 0.0F) viscositySliderSB = 0;
		if (viscositySliderSW < 0.0F) viscositySliderSW = 0;
		if (viscositySliderSO < 0.0F) viscositySliderSO = 0;
		if (viscositySliderSK < 0.0F) viscositySliderSK = 0;
		if (viscositySliderSC < 0.0F) viscositySliderSC = 0;
		if (viscositySliderSD < 0.0F) viscositySliderSD = 0;
		if (viscositySliderSY < 0.0F) viscositySliderSY = 0;
		if (viscositySliderSP < 0.0F) viscositySliderSP = 0;
		if (viscositySliderSM < 0.0F) viscositySliderSM = 0;
		if (viscositySliderSA < 0.0F) viscositySliderSA = 0;
		if (viscositySliderST < 0.0F) viscositySliderST = 0;
		if (viscositySliderSS < 0.0F) viscositySliderSS = 0;
		if (viscositySliderSF < 0.0F) viscositySliderSF = 0;
		if (viscositySliderSR > 1.0F) viscositySliderSR = 1.0F;
		if (viscositySliderSG > 1.0F) viscositySliderSG = 1.0F;
		if (viscositySliderSB > 1.0F) viscositySliderSB = 1.0F;
		if (viscositySliderSW > 1.0F) viscositySliderSW = 1.0F;
		if (viscositySliderSO > 1.0F) viscositySliderSO = 1.0F;
		if (viscositySliderSK > 1.0F) viscositySliderSK = 1.0F;
		if (viscositySliderSC > 1.0F) viscositySliderSC = 1.0F;
		if (viscositySliderSD > 1.0F) viscositySliderSD = 1.0F;
		if (viscositySliderSY > 1.0F) viscositySliderSY = 1.0F;
		if (viscositySliderSP > 1.0F) viscositySliderSP = 1.0F;
		if (viscositySliderSM > 1.0F) viscositySliderSM = 1.0F;
		if (viscositySliderSA > 1.0F) viscositySliderSA = 1.0F;
		if (viscositySliderST > 1.0F) viscositySliderST = 1.0F;
		if (viscositySliderSS > 1.0F) viscositySliderSS = 1.0F;
		if (viscositySliderSF > 1.0F) viscositySliderSF = 1.0F;

		if (powerSliderFR < -100.0F) powerSliderFR = -100.0F;
		if (powerSliderFG < -100.0F) powerSliderFG = -100.0F;
		if (powerSliderFB < -100.0F) powerSliderFB = -100.0F;
		if (powerSliderFW < -100.0F) powerSliderFW = -100.0F;
		if (powerSliderFO < -100.0F) powerSliderFO = -100.0F;
		if (powerSliderFK < -100.0F) powerSliderFK = -100.0F;
		if (powerSliderFC < -100.0F) powerSliderFC = -100.0F;
		if (powerSliderFD < -100.0F) powerSliderFD = -100.0F;
		if (powerSliderFY < -100.0F) powerSliderFY = -100.0F;
		if (powerSliderFP < -100.0F) powerSliderFP = -100.0F;
		if (powerSliderFM < -100.0F) powerSliderFM = -100.0F;
		if (powerSliderFA < -100.0F) powerSliderFA = -100.0F;
		if (powerSliderFT < -100.0F) powerSliderFT = -100.0F;
		if (powerSliderFS < -100.0F) powerSliderFS = -100.0F;
		if (powerSliderFF < -100.0F) powerSliderFF = -100.0F;
		if (powerSliderFR > 0.0F) powerSliderFR = 0.0F;
		if (powerSliderFG > 0.0F) powerSliderFG = 0.0F;
		if (powerSliderFB > 0.0F) powerSliderFB = 0.0F;
		if (powerSliderFW > 0.0F) powerSliderFW = 0.0F;
		if (powerSliderFO > 0.0F) powerSliderFO = 0.0F;
		if (powerSliderFK > 0.0F) powerSliderFK = 0.0F;
		if (powerSliderFC > 0.0F) powerSliderFC = 0.0F;
		if (powerSliderFD > 0.0F) powerSliderFD = 0.0F;
		if (powerSliderFY > 0.0F) powerSliderFY = 0.0F;
		if (powerSliderFP > 0.0F) powerSliderFP = 0.0F;
		if (powerSliderFM > 0.0F) powerSliderFM = 0.0F;
		if (powerSliderFA > 0.0F) powerSliderFA = 0.0F;
		if (powerSliderFT > 0.0F) powerSliderFT = 0.0F;
		if (powerSliderFS > 0.0F) powerSliderFS = 0.0F;
		if (powerSliderFF > 0.0F) powerSliderFF = 0.0F;
		if (vSliderFR < 0.0F) vSliderFR = 0;
		if (vSliderFG < 0.0F) vSliderFG = 0;
		if (vSliderFB < 0.0F) vSliderFB = 0;
		if (vSliderFW < 0.0F) vSliderFW = 0;
		if (vSliderFO < 0.0F) vSliderFO = 0;
		if (vSliderFK < 0.0F) vSliderFK = 0;
		if (vSliderFC < 0.0F) vSliderFC = 0;
		if (vSliderFD < 0.0F) vSliderFD = 0;
		if (vSliderFY < 0.0F) vSliderFY = 0;
		if (vSliderFP < 0.0F) vSliderFP = 0;
		if (vSliderFM < 0.0F) vSliderFM = 0;
		if (vSliderFA < 0.0F) vSliderFA = 0;
		if (vSliderFT < 0.0F) vSliderFT = 0;
		if (vSliderFS < 0.0F) vSliderFS = 0;
		if (vSliderFF < 0.0F) vSliderFF = 0;
		if (vSliderFR > 500.0F) vSliderFR = 500.0F;
		if (vSliderFG > 500.0F) vSliderFG = 500.0F;
		if (vSliderFB > 500.0F) vSliderFB = 500.0F;
		if (vSliderFW > 500.0F) vSliderFW = 500.0F;
		if (vSliderFO > 500.0F) vSliderFO = 500.0F;
		if (vSliderFK > 500.0F) vSliderFK = 500.0F;
		if (vSliderFC > 500.0F) vSliderFC = 500.0F;
		if (vSliderFD > 500.0F) vSliderFD = 500.0F;
		if (vSliderFY > 500.0F) vSliderFY = 500.0F;
		if (vSliderFP > 500.0F) vSliderFP = 500.0F;
		if (vSliderFM > 500.0F) vSliderFM = 500.0F;
		if (vSliderFA > 500.0F) vSliderFA = 500.0F;
		if (vSliderFT > 500.0F) vSliderFT = 500.0F;
		if (vSliderFS > 500.0F) vSliderFS = 500.0F;
		if (vSliderFF > 500.0F) vSliderFF = 500.0F;
		if (probabilitySliderFR < 0.0F) probabilitySliderFR = 0;
		if (probabilitySliderFG < 0.0F) probabilitySliderFG = 0;
		if (probabilitySliderFB < 0.0F) probabilitySliderFB = 0;
		if (probabilitySliderFW < 0.0F) probabilitySliderFW = 0;
		if (probabilitySliderFO < 0.0F) probabilitySliderFO = 0;
		if (probabilitySliderFK < 0.0F) probabilitySliderFK = 0;
		if (probabilitySliderFC < 0.0F) probabilitySliderFC = 0;
		if (probabilitySliderFD < 0.0F) probabilitySliderFD = 0;
		if (probabilitySliderFY < 0.0F) probabilitySliderFY = 0;
		if (probabilitySliderFP < 0.0F) probabilitySliderFP = 0;
		if (probabilitySliderFM < 0.0F) probabilitySliderFM = 0;
		if (probabilitySliderFA < 0.0F) probabilitySliderFA = 0;
		if (probabilitySliderFT < 0.0F) probabilitySliderFT = 0;
		if (probabilitySliderFS < 0.0F) probabilitySliderFS = 0;
		if (probabilitySliderFF < 0.0F) probabilitySliderFF = 0;
		if (probabilitySliderFR > 100.0F) probabilitySliderFR = 100.0F;
		if (probabilitySliderFG > 100.0F) probabilitySliderFG = 100.0F;
		if (probabilitySliderFB > 100.0F) probabilitySliderFB = 100.0F;
		if (probabilitySliderFW > 100.0F) probabilitySliderFW = 100.0F;
		if (probabilitySliderFO > 100.0F) probabilitySliderFO = 100.0F;
		if (probabilitySliderFK > 100.0F) probabilitySliderFK = 100.0F;
		if (probabilitySliderFC > 100.0F) probabilitySliderFC = 100.0F;
		if (probabilitySliderFD > 100.0F) probabilitySliderFD = 100.0F;
		if (probabilitySliderFY > 100.0F) probabilitySliderFY = 100.0F;
		if (probabilitySliderFP > 100.0F) probabilitySliderFP = 100.0F;
		if (probabilitySliderFM > 100.0F) probabilitySliderFM = 100.0F;
		if (probabilitySliderFA > 100.0F) probabilitySliderFA = 100.0F;
		if (probabilitySliderFT > 100.0F) probabilitySliderFT = 100.0F;
		if (probabilitySliderFS > 100.0F) probabilitySliderFS = 100.0F;
		if (probabilitySliderFF > 100.0F) probabilitySliderFF = 100.0F;
		if (viscositySliderFR < 0.0F) viscositySliderFR = 0;
		if (viscositySliderFG < 0.0F) viscositySliderFG = 0;
		if (viscositySliderFB < 0.0F) viscositySliderFB = 0;
		if (viscositySliderFW < 0.0F) viscositySliderFW = 0;
		if (viscositySliderFO < 0.0F) viscositySliderFO = 0;
		if (viscositySliderFK < 0.0F) viscositySliderFK = 0;
		if (viscositySliderFC < 0.0F) viscositySliderFC = 0;
		if (viscositySliderFD < 0.0F) viscositySliderFD = 0;
		if (viscositySliderFY < 0.0F) viscositySliderFY = 0;
		if (viscositySliderFP < 0.0F) viscositySliderFP = 0;
		if (viscositySliderFM < 0.0F) viscositySliderFM = 0;
		if (viscositySliderFA < 0.0F) viscositySliderFA = 0;
		if (viscositySliderFT < 0.0F) viscositySliderFT = 0;
		if (viscositySliderFS < 0.0F) viscositySliderFS = 0;
		if (viscositySliderFF < 0.0F) viscositySliderFF = 0;
		if (viscositySliderFR > 1.0F) viscositySliderFR = 1.0F;
		if (viscositySliderFG > 1.0F) viscositySliderFG = 1.0F;
		if (viscositySliderFB > 1.0F) viscositySliderFB = 1.0F;
		if (viscositySliderFW > 1.0F) viscositySliderFW = 1.0F;
		if (viscositySliderFO > 1.0F) viscositySliderFO = 1.0F;
		if (viscositySliderFK > 1.0F) viscositySliderFK = 1.0F;
		if (viscositySliderFC > 1.0F) viscositySliderFC = 1.0F;
		if (viscositySliderFD > 1.0F) viscositySliderFD = 1.0F;
		if (viscositySliderFY > 1.0F) viscositySliderFY = 1.0F;
		if (viscositySliderFP > 1.0F) viscositySliderFP = 1.0F;
		if (viscositySliderFM > 1.0F) viscositySliderFM = 1.0F;
		if (viscositySliderFA > 1.0F) viscositySliderFA = 1.0F;
		if (viscositySliderFT > 1.0F) viscositySliderFT = 1.0F;
		if (viscositySliderFS > 1.0F) viscositySliderFS = 1.0F;
		if (viscositySliderFF > 1.0F) viscositySliderFF = 1.0F;
	}

	if (numberSliderR > 0)
	{
		interaction(&red, &red, powerSliderRR, vSliderRR, viscosityRR, probabilityRR);
		if (numberSliderG > 0) interaction(&red, &green, powerSliderRG, vSliderRG, viscosityRG, probabilityRG);
		if (numberSliderB > 0) interaction(&red, &blue, powerSliderRB, vSliderRB, viscosityRB, probabilityRB);
		if (numberSliderW > 0) interaction(&red, &white, powerSliderRW, vSliderRW, viscosityRW, probabilityRW);
		if (numberSliderO > 0) interaction(&red, &orange, powerSliderRO, vSliderRO, viscosityRO, probabilityRO);
		if (numberSliderK > 0) interaction(&red, &khaki, powerSliderRK, vSliderRK, viscosityRK, probabilityRK);
		if (numberSliderC > 0) interaction(&red, &crimson, powerSliderRC, vSliderRC, viscosityRC, probabilityRC);
		if (numberSliderD > 0) interaction(&red, &dark, powerSliderRD, vSliderRD, viscosityRD, probabilityRD);
		if (numberSliderY > 0) interaction(&red, &yellow, powerSliderRY, vSliderRY, viscosityRY, probabilityRY);
		if (numberSliderP > 0) interaction(&red, &pink, powerSliderRP, vSliderRP, viscosityRP, probabilityRP);
		if (numberSliderM > 0) interaction(&red, &magenta, powerSliderRM, vSliderRM, viscosityRM, probabilityRM);
		if (numberSliderA > 0) interaction(&red, &aqua, powerSliderRA, vSliderRA, viscosityRA, probabilityRA);
		if (numberSliderT > 0) interaction(&red, &teal, powerSliderRT, vSliderRT, viscosityRT, probabilityRT);
		if (numberSliderS > 0) interaction(&red, &silver, powerSliderRS, vSliderRS, viscosityRS, probabilityRS);
		if (numberSliderF > 0) interaction(&red, &firebrick, powerSliderRF, vSliderRF, viscosityRF, probabilityRF);
	}

	if (numberSliderG > 0)
	{
		interaction(&green, &green, powerSliderGG, vSliderGG, viscosityGG, probabilityGG);
		if (numberSliderR > 0) interaction(&green, &red, powerSliderGR, vSliderGR, viscosityGR, probabilityGR);
		if (numberSliderB > 0) interaction(&green, &blue, powerSliderGB, vSliderGB, viscosityGB, probabilityGB);
		if (numberSliderW > 0) interaction(&green, &white, powerSliderGW, vSliderGW, viscosityGW, probabilityGW);
		if (numberSliderO > 0) interaction(&green, &orange, powerSliderGO, vSliderGO, viscosityGO, probabilityGO);
		if (numberSliderK > 0) interaction(&green, &khaki, powerSliderGK, vSliderGK, viscosityGK, probabilityGK);
		if (numberSliderC > 0) interaction(&green, &crimson, powerSliderGC, vSliderGC, viscosityGC, probabilityGC);
		if (numberSliderD > 0) interaction(&green, &dark, powerSliderGD, vSliderGD, viscosityGD, probabilityGD);
		if (numberSliderY > 0) interaction(&green, &yellow, powerSliderGY, vSliderGY, viscosityGY, probabilityGY);
		if (numberSliderP > 0) interaction(&green, &pink, powerSliderGP, vSliderGP, viscosityGP, probabilityGP);
		if (numberSliderM > 0) interaction(&green, &magenta, powerSliderGM, vSliderGM, viscosityGM, probabilityGM);
		if (numberSliderA > 0) interaction(&green, &aqua, powerSliderGA, vSliderGA, viscosityGA, probabilityGA);
		if (numberSliderT > 0) interaction(&green, &teal, powerSliderGT, vSliderGT, viscosityGT, probabilityGT);
		if (numberSliderS > 0) interaction(&green, &silver, powerSliderGS, vSliderGS, viscosityGS, probabilityGS);
		if (numberSliderF > 0) interaction(&green, &firebrick, powerSliderGF, vSliderGF, viscosityGF, probabilityGF);
	}

	if (numberSliderB > 0)
	{
		interaction(&blue, &blue, powerSliderBB, vSliderBB, viscosityBB, probabilityBB);
		if (numberSliderR > 0) interaction(&blue, &red, powerSliderBR, vSliderBR, viscosityBR, probabilityBR);
		if (numberSliderG > 0) interaction(&blue, &green, powerSliderBG, vSliderBG, viscosityBG, probabilityBG);
		if (numberSliderW > 0) interaction(&blue, &white, powerSliderBW, vSliderBW, viscosityBW, probabilityBW);
		if (numberSliderO > 0) interaction(&blue, &orange, powerSliderBO, vSliderBO, viscosityBO, probabilityBO);
		if (numberSliderK > 0) interaction(&blue, &khaki, powerSliderBK, vSliderBK, viscosityBK, probabilityBK);
		if (numberSliderC > 0) interaction(&blue, &crimson, powerSliderBC, vSliderBC, viscosityBC, probabilityBC);
		if (numberSliderD > 0) interaction(&blue, &dark, powerSliderBD, vSliderBD, viscosityBD, probabilityBD);
		if (numberSliderY > 0) interaction(&blue, &yellow, powerSliderBY, vSliderBY, viscosityBY, probabilityBY);
		if (numberSliderP > 0) interaction(&blue, &pink, powerSliderBP, vSliderBP, viscosityBP, probabilityBP);
		if (numberSliderM > 0) interaction(&blue, &magenta, powerSliderBM, vSliderBM, viscosityBM, probabilityBM);
		if (numberSliderA > 0) interaction(&blue, &aqua, powerSliderBA, vSliderBA, viscosityBA, probabilityBA);
		if (numberSliderT > 0) interaction(&blue, &teal, powerSliderBT, vSliderBT, viscosityBT, probabilityBT);
		if (numberSliderS > 0) interaction(&blue, &silver, powerSliderBS, vSliderBS, viscosityBS, probabilityBS);
		if (numberSliderF > 0) interaction(&blue, &firebrick, powerSliderBF, vSliderBF, viscosityBF, probabilityBF);
	}

	if (numberSliderW > 0)
	{
		interaction(&white, &white, powerSliderWW, vSliderWW, viscosityWW, probabilityWW);
		if (numberSliderR > 0) interaction(&white, &red, powerSliderWR, vSliderWR, viscosityWR, probabilityWR);
		if (numberSliderG > 0) interaction(&white, &green, powerSliderWG, vSliderWG, viscosityWG, probabilityWG);
		if (numberSliderB > 0) interaction(&white, &blue, powerSliderWB, vSliderWB, viscosityWB, probabilityWB);
		if (numberSliderO > 0) interaction(&white, &orange, powerSliderWO, vSliderWO, viscosityWO, probabilityWO);
		if (numberSliderK > 0) interaction(&white, &khaki, powerSliderWK, vSliderWK, viscosityWK, probabilityWK);
		if (numberSliderC > 0) interaction(&white, &crimson, powerSliderWC, vSliderWC, viscosityWC, probabilityWC);
		if (numberSliderD > 0) interaction(&white, &dark, powerSliderWD, vSliderWD, viscosityWD, probabilityWD);
		if (numberSliderY > 0) interaction(&white, &yellow, powerSliderWY, vSliderWY, viscosityWY, probabilityWY);
		if (numberSliderP > 0) interaction(&white, &pink, powerSliderWP, vSliderWP, viscosityWP, probabilityWP);
		if (numberSliderM > 0) interaction(&white, &magenta, powerSliderWM, vSliderWM, viscosityWM, probabilityWM);
		if (numberSliderA > 0) interaction(&white, &aqua, powerSliderWA, vSliderWA, viscosityWA, probabilityWA);
		if (numberSliderT > 0) interaction(&white, &teal, powerSliderWT, vSliderWT, viscosityWT, probabilityWT);
		if (numberSliderS > 0) interaction(&white, &silver, powerSliderWS, vSliderWS, viscosityWS, probabilityWS);
		if (numberSliderF > 0) interaction(&white, &firebrick, powerSliderWF, vSliderWF, viscosityWF, probabilityWF);
	}

	if (numberSliderO > 0)
	{
		interaction(&orange, &orange, powerSliderOO, vSliderOO, viscosityOO, probabilityOO);
		if (numberSliderR > 0) interaction(&orange, &red, powerSliderOR, vSliderOR, viscosityOR, probabilityOR);
		if (numberSliderG > 0) interaction(&orange, &green, powerSliderOG, vSliderOG, viscosityOG, probabilityOG);
		if (numberSliderB > 0) interaction(&orange, &blue, powerSliderOB, vSliderOB, viscosityOB, probabilityOB);
		if (numberSliderW > 0) interaction(&orange, &white, powerSliderOW, vSliderOW, viscosityOW, probabilityOW);
		if (numberSliderK > 0) interaction(&orange, &khaki, powerSliderOK, vSliderOK, viscosityOK, probabilityOK);
		if (numberSliderC > 0) interaction(&orange, &crimson, powerSliderOC, vSliderOC, viscosityOC, probabilityOC);
		if (numberSliderD > 0) interaction(&orange, &dark, powerSliderOD, vSliderOD, viscosityOD, probabilityOD);
		if (numberSliderY > 0) interaction(&orange, &yellow, powerSliderOY, vSliderOY, viscosityOY, probabilityOY);
		if (numberSliderP > 0) interaction(&orange, &pink, powerSliderOP, vSliderOP, viscosityOP, probabilityOP);
		if (numberSliderM > 0) interaction(&orange, &magenta, powerSliderOM, vSliderOM, viscosityOM, probabilityOM);
		if (numberSliderA > 0) interaction(&orange, &aqua, powerSliderOA, vSliderOA, viscosityOA, probabilityOA);
		if (numberSliderT > 0) interaction(&orange, &teal, powerSliderOT, vSliderOT, viscosityOT, probabilityOT);
		if (numberSliderS > 0) interaction(&orange, &silver, powerSliderOS, vSliderOS, viscosityOS, probabilityOS);
		if (numberSliderF > 0) interaction(&orange, &firebrick, powerSliderOF, vSliderOF, viscosityOF, probabilityOF);
	}

	if (numberSliderK > 0)
	{
		interaction(&khaki, &khaki, powerSliderKK, vSliderKK, viscosityKK, probabilityKK);
		if (numberSliderR > 0) interaction(&khaki, &red, powerSliderKR, vSliderKR, viscosityKR, probabilityKR);
		if (numberSliderG > 0) interaction(&khaki, &green, powerSliderKG, vSliderKG, viscosityKG, probabilityKG);
		if (numberSliderB > 0) interaction(&khaki, &blue, powerSliderKB, vSliderKB, viscosityKB, probabilityKB);
		if (numberSliderW > 0) interaction(&khaki, &white, powerSliderKW, vSliderKW, viscosityKW, probabilityKW);
		if (numberSliderO > 0) interaction(&khaki, &orange, powerSliderKO, vSliderKO, viscosityKO, probabilityKO);
		if (numberSliderC > 0) interaction(&khaki, &crimson, powerSliderKC, vSliderKC, viscosityKC, probabilityKC);
		if (numberSliderD > 0) interaction(&khaki, &dark, powerSliderKD, vSliderKD, viscosityKD, probabilityKD);
		if (numberSliderY > 0) interaction(&khaki, &yellow, powerSliderKY, vSliderKY, viscosityKY, probabilityKY);
		if (numberSliderP > 0) interaction(&khaki, &pink, powerSliderKP, vSliderKP, viscosityKP, probabilityKP);
		if (numberSliderM > 0) interaction(&khaki, &magenta, powerSliderKM, vSliderKM, viscosityKM, probabilityKM);
		if (numberSliderA > 0) interaction(&khaki, &aqua, powerSliderKA, vSliderKA, viscosityKA, probabilityKA);
		if (numberSliderT > 0) interaction(&khaki, &teal, powerSliderKT, vSliderKT, viscosityKT, probabilityKT);
		if (numberSliderS > 0) interaction(&khaki, &silver, powerSliderKS, vSliderKS, viscosityKS, probabilityKS);
		if (numberSliderF > 0) interaction(&khaki, &firebrick, powerSliderKF, vSliderKF, viscosityKF, probabilityKF);
	}

	if (numberSliderC > 0)
	{
		interaction(&crimson, &crimson, powerSliderCC, vSliderCC, viscosityCC, probabilityCC);
		if (numberSliderR > 0) interaction(&crimson, &red, powerSliderCR, vSliderCR, viscosityCR, probabilityCR);
		if (numberSliderG > 0) interaction(&crimson, &green, powerSliderCG, vSliderCG, viscosityCG, probabilityCG);
		if (numberSliderB > 0) interaction(&crimson, &blue, powerSliderCB, vSliderCB, viscosityCB, probabilityCB);
		if (numberSliderW > 0) interaction(&crimson, &white, powerSliderCW, vSliderCW, viscosityCW, probabilityCW);
		if (numberSliderO > 0) interaction(&crimson, &orange, powerSliderCO, vSliderCO, viscosityCO, probabilityCO);
		if (numberSliderK > 0) interaction(&crimson, &khaki, powerSliderCK, vSliderCK, viscosityCK, probabilityCK);
		if (numberSliderD > 0) interaction(&crimson, &dark, powerSliderCD, vSliderCD, viscosityCD, probabilityCD);
		if (numberSliderY > 0) interaction(&crimson, &yellow, powerSliderCY, vSliderCY, viscosityCY, probabilityCY);
		if (numberSliderP > 0) interaction(&crimson, &pink, powerSliderCP, vSliderCP, viscosityCP, probabilityCP);
		if (numberSliderM > 0) interaction(&crimson, &magenta, powerSliderCM, vSliderCM, viscosityCM, probabilityCM);
		if (numberSliderA > 0) interaction(&crimson, &aqua, powerSliderCA, vSliderCA, viscosityCA, probabilityCA);
		if (numberSliderT > 0) interaction(&crimson, &teal, powerSliderCT, vSliderCT, viscosityCT, probabilityCT);
		if (numberSliderS > 0) interaction(&crimson, &silver, powerSliderCS, vSliderCS, viscosityCS, probabilityCS);
		if (numberSliderF > 0) interaction(&crimson, &firebrick, powerSliderCF, vSliderCF, viscosityCF, probabilityCF);
	}

	if (numberSliderD > 0)
	{
		interaction(&dark, &dark, powerSliderDD, vSliderDD, viscosityDD, probabilityDD);
		if (numberSliderR > 0) interaction(&dark, &red, powerSliderDR, vSliderDR, viscosityDR, probabilityDR);
		if (numberSliderG > 0) interaction(&dark, &green, powerSliderDG, vSliderDG, viscosityDG, probabilityDG);
		if (numberSliderB > 0) interaction(&dark, &blue, powerSliderDB, vSliderDB, viscosityDB, probabilityDB);
		if (numberSliderW > 0) interaction(&dark, &white, powerSliderDW, vSliderDW, viscosityDW, probabilityDW);
		if (numberSliderO > 0) interaction(&dark, &orange, powerSliderDO, vSliderDO, viscosityDO, probabilityDO);
		if (numberSliderK > 0) interaction(&dark, &khaki, powerSliderDK, vSliderDK, viscosityDK, probabilityDK);
		if (numberSliderC > 0) interaction(&dark, &crimson, powerSliderDC, vSliderDC, viscosityDC, probabilityDC);
		if (numberSliderY > 0) interaction(&dark, &yellow, powerSliderDY, vSliderDY, viscosityDY, probabilityDY);
		if (numberSliderP > 0) interaction(&dark, &pink, powerSliderDP, vSliderDP, viscosityDP, probabilityDP);
		if (numberSliderM > 0) interaction(&dark, &magenta, powerSliderDM, vSliderDM, viscosityDM, probabilityDM);
		if (numberSliderA > 0) interaction(&dark, &aqua, powerSliderDA, vSliderDA, viscosityDA, probabilityDA);
		if (numberSliderT > 0) interaction(&dark, &teal, powerSliderDT, vSliderDT, viscosityDT, probabilityDT);
		if (numberSliderS > 0) interaction(&dark, &silver, powerSliderDS, vSliderDS, viscosityDS, probabilityDS);
		if (numberSliderF > 0) interaction(&dark, &firebrick, powerSliderDF, vSliderDF, viscosityDF, probabilityDF);
	}

	if (numberSliderY > 0)
	{
		interaction(&yellow, &yellow, powerSliderYY, vSliderYY, viscosityYY, probabilityYY);
		if (numberSliderR > 0) interaction(&yellow, &red, powerSliderYR, vSliderYR, viscosityYR, probabilityYR);
		if (numberSliderG > 0) interaction(&yellow, &green, powerSliderYG, vSliderYG, viscosityYG, probabilityYG);
		if (numberSliderB > 0) interaction(&yellow, &blue, powerSliderYB, vSliderYB, viscosityYB, probabilityYB);
		if (numberSliderW > 0) interaction(&yellow, &white, powerSliderYW, vSliderYW, viscosityYW, probabilityYW);
		if (numberSliderO > 0) interaction(&yellow, &orange, powerSliderYO, vSliderYO, viscosityYO, probabilityYO);
		if (numberSliderK > 0) interaction(&yellow, &khaki, powerSliderYK, vSliderYK, viscosityYK, probabilityYK);
		if (numberSliderC > 0) interaction(&yellow, &crimson, powerSliderYC, vSliderYC, viscosityYC, probabilityYC);
		if (numberSliderD > 0) interaction(&yellow, &dark, powerSliderYD, vSliderYD, viscosityYD, probabilityYD);
		if (numberSliderP > 0) interaction(&yellow, &pink, powerSliderYP, vSliderYP, viscosityYP, probabilityYP);
		if (numberSliderM > 0) interaction(&yellow, &magenta, powerSliderYM, vSliderYM, viscosityYM, probabilityYM);
		if (numberSliderA > 0) interaction(&yellow, &aqua, powerSliderYA, vSliderYA, viscosityYA, probabilityYA);
		if (numberSliderT > 0) interaction(&yellow, &teal, powerSliderYT, vSliderYT, viscosityYT, probabilityYT);
		if (numberSliderS > 0) interaction(&yellow, &silver, powerSliderYS, vSliderYS, viscosityYS, probabilityYS);
		if (numberSliderF > 0) interaction(&yellow, &firebrick, powerSliderYF, vSliderYF, viscosityYF, probabilityYF);
	}

	if (numberSliderP > 0)
	{
		interaction(&pink, &pink, powerSliderPP, vSliderPP, viscosityPP, probabilityPP);
		if (numberSliderR > 0) interaction(&pink, &red, powerSliderPR, vSliderPR, viscosityPR, probabilityPR);
		if (numberSliderG > 0) interaction(&pink, &green, powerSliderPG, vSliderPG, viscosityPG, probabilityPG);
		if (numberSliderB > 0) interaction(&pink, &blue, powerSliderPB, vSliderPB, viscosityPB, probabilityPB);
		if (numberSliderW > 0) interaction(&pink, &white, powerSliderPW, vSliderPW, viscosityPW, probabilityPW);
		if (numberSliderO > 0) interaction(&pink, &orange, powerSliderPO, vSliderPO, viscosityPO, probabilityPO);
		if (numberSliderK > 0) interaction(&pink, &khaki, powerSliderPK, vSliderPK, viscosityPK, probabilityPK);
		if (numberSliderC > 0) interaction(&pink, &crimson, powerSliderPC, vSliderPC, viscosityPC, probabilityPC);
		if (numberSliderD > 0) interaction(&pink, &dark, powerSliderPD, vSliderPD, viscosityPD, probabilityPD);
		if (numberSliderY > 0) interaction(&pink, &yellow, powerSliderPY, vSliderPY, viscosityPY, probabilityPY);
		if (numberSliderM > 0) interaction(&pink, &magenta, powerSliderPM, vSliderPM, viscosityPM, probabilityPM);
		if (numberSliderA > 0) interaction(&pink, &aqua, powerSliderPA, vSliderPA, viscosityPA, probabilityPA);
		if (numberSliderT > 0) interaction(&pink, &teal, powerSliderPT, vSliderPT, viscosityPT, probabilityPT);
		if (numberSliderS > 0) interaction(&pink, &silver, powerSliderPS, vSliderPS, viscosityPS, probabilityPS);
		if (numberSliderF > 0) interaction(&pink, &firebrick, powerSliderPF, vSliderPF, viscosityPF, probabilityPF);
	}

	if (numberSliderM > 0)
	{
		interaction(&magenta, &magenta, powerSliderMM, vSliderMM, viscosityMM, probabilityMM);
		if (numberSliderR > 0) interaction(&magenta, &red, powerSliderMR, vSliderMR, viscosityMR, probabilityMR);
		if (numberSliderG > 0) interaction(&magenta, &green, powerSliderMG, vSliderMG, viscosityMG, probabilityMG);
		if (numberSliderB > 0) interaction(&magenta, &blue, powerSliderMB, vSliderMB, viscosityMB, probabilityMB);
		if (numberSliderW > 0) interaction(&magenta, &white, powerSliderMW, vSliderMW, viscosityMW, probabilityMW);
		if (numberSliderO > 0) interaction(&magenta, &orange, powerSliderMO, vSliderMO, viscosityMO, probabilityMO);
		if (numberSliderK > 0) interaction(&magenta, &khaki, powerSliderMK, vSliderMK, viscosityMK, probabilityMK);
		if (numberSliderC > 0) interaction(&magenta, &crimson, powerSliderMC, vSliderMC, viscosityMC, probabilityMC);
		if (numberSliderD > 0) interaction(&magenta, &dark, powerSliderMD, vSliderMD, viscosityMD, probabilityMD);
		if (numberSliderY > 0) interaction(&magenta, &yellow, powerSliderMY, vSliderMY, viscosityMY, probabilityMY);
		if (numberSliderP > 0) interaction(&magenta, &pink, powerSliderMP, vSliderMP, viscosityMP, probabilityMP);
		if (numberSliderA > 0) interaction(&magenta, &aqua, powerSliderMA, vSliderMA, viscosityMA, probabilityMA);
		if (numberSliderT > 0) interaction(&magenta, &teal, powerSliderMT, vSliderMT, viscosityMT, probabilityMT);
		if (numberSliderS > 0) interaction(&magenta, &silver, powerSliderMS, vSliderMS, viscosityMS, probabilityMS);
		if (numberSliderF > 0) interaction(&magenta, &firebrick, powerSliderMF, vSliderMF, viscosityMF, probabilityMF);
	}

	if (numberSliderA > 0)
	{
		interaction(&aqua, &aqua, powerSliderAA, vSliderAA, viscosityAA, probabilityAA);
		if (numberSliderR > 0) interaction(&aqua, &red, powerSliderAR, vSliderAR, viscosityAR, probabilityAR);
		if (numberSliderG > 0) interaction(&aqua, &green, powerSliderAG, vSliderAG, viscosityAG, probabilityAG);
		if (numberSliderB > 0) interaction(&aqua, &blue, powerSliderAB, vSliderAB, viscosityAB, probabilityAB);
		if (numberSliderW > 0) interaction(&aqua, &white, powerSliderAW, vSliderAW, viscosityAW, probabilityAW);
		if (numberSliderO > 0) interaction(&aqua, &orange, powerSliderAO, vSliderAO, viscosityAO, probabilityAO);
		if (numberSliderK > 0) interaction(&aqua, &khaki, powerSliderAK, vSliderAK, viscosityAK, probabilityAK);
		if (numberSliderC > 0) interaction(&aqua, &crimson, powerSliderAC, vSliderAC, viscosityAC, probabilityAC);
		if (numberSliderD > 0) interaction(&aqua, &dark, powerSliderAD, vSliderAD, viscosityAD, probabilityAD);
		if (numberSliderY > 0) interaction(&aqua, &yellow, powerSliderAY, vSliderAY, viscosityAY, probabilityAY);
		if (numberSliderP > 0) interaction(&aqua, &pink, powerSliderAP, vSliderAP, viscosityAP, probabilityAP);
		if (numberSliderM > 0) interaction(&aqua, &magenta, powerSliderAM, vSliderAM, viscosityAM, probabilityAM);
		if (numberSliderT > 0) interaction(&aqua, &teal, powerSliderAT, vSliderAT, viscosityAT, probabilityAT);
		if (numberSliderS > 0) interaction(&aqua, &silver, powerSliderAS, vSliderAS, viscosityAS, probabilityAS);
		if (numberSliderF > 0) interaction(&aqua, &firebrick, powerSliderAF, vSliderAF, viscosityAF, probabilityAF);
	}

	if (numberSliderT > 0)
	{
		interaction(&teal, &teal, powerSliderTT, vSliderTT, viscosityTT, probabilityTT);
		if (numberSliderR > 0) interaction(&teal, &red, powerSliderTR, vSliderTR, viscosityTR, probabilityTR);
		if (numberSliderG > 0) interaction(&teal, &green, powerSliderTG, vSliderTG, viscosityTG, probabilityTG);
		if (numberSliderB > 0) interaction(&teal, &blue, powerSliderTB, vSliderTB, viscosityTB, probabilityTB);
		if (numberSliderW > 0) interaction(&teal, &white, powerSliderTW, vSliderTW, viscosityTW, probabilityTW);
		if (numberSliderO > 0) interaction(&teal, &orange, powerSliderTO, vSliderTO, viscosityTO, probabilityTO);
		if (numberSliderK > 0) interaction(&teal, &khaki, powerSliderTK, vSliderTK, viscosityTK, probabilityTK);
		if (numberSliderC > 0) interaction(&teal, &crimson, powerSliderTC, vSliderTC, viscosityTC, probabilityTC);
		if (numberSliderD > 0) interaction(&teal, &dark, powerSliderTD, vSliderTD, viscosityTD, probabilityTD);
		if (numberSliderY > 0) interaction(&teal, &yellow, powerSliderTY, vSliderTY, viscosityTY, probabilityTY);
		if (numberSliderP > 0) interaction(&teal, &pink, powerSliderTP, vSliderTP, viscosityTP, probabilityTP);
		if (numberSliderM > 0) interaction(&teal, &magenta, powerSliderTM, vSliderTM, viscosityTM, probabilityTM);
		if (numberSliderA > 0) interaction(&teal, &aqua, powerSliderTA, vSliderTA, viscosityTA, probabilityTA);
		if (numberSliderS > 0) interaction(&teal, &silver, powerSliderTS, vSliderTS, viscosityTS, probabilityTS);
		if (numberSliderF > 0) interaction(&teal, &firebrick, powerSliderTF, vSliderTF, viscosityTF, probabilityTF);
	}

	if (numberSliderS > 0)
	{
		interaction(&silver, &silver, powerSliderSS, vSliderSS, viscositySS, probabilitySS);
		if (numberSliderR > 0) interaction(&silver, &red, powerSliderSR, vSliderSR, viscositySR, probabilitySR);
		if (numberSliderG > 0) interaction(&silver, &green, powerSliderSG, vSliderSG, viscositySG, probabilitySG);
		if (numberSliderB > 0) interaction(&silver, &blue, powerSliderSB, vSliderSB, viscositySB, probabilitySB);
		if (numberSliderW > 0) interaction(&silver, &white, powerSliderSW, vSliderSW, viscositySW, probabilitySW);
		if (numberSliderO > 0) interaction(&silver, &orange, powerSliderSO, vSliderSO, viscositySO, probabilitySO);
		if (numberSliderK > 0) interaction(&silver, &khaki, powerSliderSK, vSliderSK, viscositySK, probabilitySK);
		if (numberSliderC > 0) interaction(&silver, &crimson, powerSliderSC, vSliderSC, viscositySC, probabilitySC);
		if (numberSliderD > 0) interaction(&silver, &dark, powerSliderSD, vSliderSD, viscositySD, probabilitySD);
		if (numberSliderY > 0) interaction(&silver, &yellow, powerSliderSY, vSliderSY, viscositySY, probabilitySY);
		if (numberSliderP > 0) interaction(&silver, &pink, powerSliderSP, vSliderSP, viscositySP, probabilitySP);
		if (numberSliderM > 0) interaction(&silver, &magenta, powerSliderSM, vSliderSM, viscositySM, probabilitySM);
		if (numberSliderA > 0) interaction(&silver, &aqua, powerSliderSA, vSliderSA, viscositySA, probabilitySA);
		if (numberSliderT > 0) interaction(&silver, &teal, powerSliderST, vSliderST, viscosityST, probabilityST);
		if (numberSliderF > 0) interaction(&silver, &firebrick, powerSliderSF, vSliderSF, viscositySF, probabilitySF);
	}

	if (numberSliderF > 0)
	{
		interaction(&firebrick, &firebrick, powerSliderFF, vSliderFF, viscosityFF, probabilityFF);
		if (numberSliderR > 0) interaction(&firebrick, &red, powerSliderFR, vSliderFR, viscosityFR, probabilityFR);
		if (numberSliderG > 0) interaction(&firebrick, &green, powerSliderFG, vSliderFG, viscosityFG, probabilityFG);
		if (numberSliderB > 0) interaction(&firebrick, &blue, powerSliderFB, vSliderFB, viscosityFB, probabilityFB);
		if (numberSliderW > 0) interaction(&firebrick, &white, powerSliderFW, vSliderFW, viscosityFW, probabilityFW);
		if (numberSliderO > 0) interaction(&firebrick, &orange, powerSliderFO, vSliderFO, viscosityFO, probabilityFO);
		if (numberSliderK > 0) interaction(&firebrick, &khaki, powerSliderFK, vSliderFK, viscosityFK, probabilityFK);
		if (numberSliderC > 0) interaction(&firebrick, &crimson, powerSliderFC, vSliderFC, viscosityFC, probabilityFC);
		if (numberSliderD > 0) interaction(&firebrick, &dark, powerSliderFD, vSliderFD, viscosityFD, probabilityFD);
		if (numberSliderY > 0) interaction(&firebrick, &yellow, powerSliderFY, vSliderFY, viscosityFY, probabilityFY);
		if (numberSliderP > 0) interaction(&firebrick, &pink, powerSliderFP, vSliderFP, viscosityFP, probabilityFP);
		if (numberSliderM > 0) interaction(&firebrick, &magenta, powerSliderFM, vSliderFM, viscosityFM, probabilityFM);
		if (numberSliderA > 0) interaction(&firebrick, &aqua, powerSliderFA, vSliderFA, viscosityFA, probabilityFA);
		if (numberSliderT > 0) interaction(&firebrick, &teal, powerSliderFT, vSliderFT, viscosityFT, probabilityFT);
		if (numberSliderS > 0) interaction(&firebrick, &silver, powerSliderFS, vSliderFS, viscosityFS, probabilityFS);
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
	if (resetButton) 
	{ 
		restart(); 
	}
	if (randomGeneral)
	{
		random();
		restart();
	}
	if (randomCount)
	{
		monads();
		restart();
	}
	if (randomVsc)
	{
		rndvsc();
	}
	if (randomProb)
	{
		rndprob();
	}
	if (randomChoice)
	{
		rndir();
	}
	if (numberSliderW > 0) { Draw(&white); }
	if (numberSliderR > 0) { Draw(&red); }
	if (numberSliderG > 0) { Draw(&green); }
	if (numberSliderB > 0) { Draw(&blue); }
	if (numberSliderO > 0) { Draw(&orange); }
	if (numberSliderK > 0) { Draw(&khaki); }
	if (numberSliderC > 0) { Draw(&crimson); }
	if (numberSliderD > 0) { Draw(&dark); }
	if (numberSliderY > 0) { Draw(&yellow); }
	if (numberSliderP > 0) { Draw(&pink); }
	if (numberSliderM > 0) { Draw(&magenta); }
	if (numberSliderA > 0) { Draw(&aqua); }
	if (numberSliderT > 0) { Draw(&teal); }
	if (numberSliderS > 0) { Draw(&silver); }
	if (numberSliderF > 0) { Draw(&firebrick); }
	if (numberSliderR < 0.0F) numberSliderR = 0;
	if (numberSliderG < 0.0F) numberSliderG = 0;
	if (numberSliderB < 0.0F) numberSliderB = 0;
	if (numberSliderW < 0.0F) numberSliderW = 0;
	if (numberSliderO < 0.0F) numberSliderO = 0;
	if (numberSliderK < 0.0F) numberSliderK = 0;
	if (numberSliderC < 0.0F) numberSliderC = 0;
	if (numberSliderD < 0.0F) numberSliderD = 0;
	if (numberSliderY < 0.0F) numberSliderY = 0;
	if (numberSliderP < 0.0F) numberSliderP = 0;
	if (numberSliderM < 0.0F) numberSliderM = 0;
	if (numberSliderA < 0.0F) numberSliderA = 0;
	if (numberSliderT < 0.0F) numberSliderT = 0;
	if (numberSliderS < 0.0F) numberSliderS = 0;
	if (numberSliderF < 0.0F) numberSliderF = 0;

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
	if (key == 'v')
	{
		rndvsc();
	}
	if (key == 'p')
	{
		rndprob();
	}
	if (key == 'i')
	{
		rndir();
	}
}
