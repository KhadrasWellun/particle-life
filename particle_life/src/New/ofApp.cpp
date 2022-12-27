#include "ofApp.h"
#include "ofUtils.h"

#include <iostream>
#include <algorithm>
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
void ofApp::interaction(std::vector<point>* Group1, const std::vector<point>* Group2, const float G, const float Gradius, const float Gprobability, const float A, const float Aradius, const float Aprobability, const float viscosity)
{
	const float g = G;	//Gravity coefficient
	const float a = A;	//Anti-Gravity coefficient
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
			if (rd() % 100 < Gprobability && rd() % 100 < Aprobability) {
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
					using namespace std;
					
					// cout << std::min(Gradius, Aradius) << "\n";
					if ((r <= min(Gradius, Aradius) || radius_toggle) && r != 0.0F)
					{
						fx += (dx / r);
						fy += (dy / r);
						//Calculate new velocity
						p1.vx = ((p1.vx + (fx * (g + a))) * (1 - viscosity));
						p1.vy = ((p1.vx + (fx * (g + a))) * (1 - viscosity) + worldGravity);
						
					}
					
					if ((min(Gradius, Aradius) < r <= max(Gradius, Aradius) || radius_toggle) && r != 0.0F)
					{
						fx += (dx / r);
						fy += (dy / r);
						if (Gradius <= Aradius)
						{
							//Calculate new velocity
							p1.vx = ((p1.vx + (fx * a)) * (1 - viscosity));
							p1.vy = ((p1.vx + (fx * a)) * (1 - viscosity) + worldGravity);
							//Update position based on velocity
							p1.x += p1.vx;
							p1.y += p1.vy;
						}
						if (Gradius > Aradius)
						{
							//Calculate new velocity
							p1.vx = ((p1.vx + (fx * g)) * (1 - viscosity));
							p1.vy = ((p1.vx + (fx * g)) * (1 - viscosity) + worldGravity);
							
						}
					}				
				}
				
				
					
				
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
void ofApp::random()
{
	numberSliderG = RandomInt(200, 2000);
	numberSliderR = RandomInt(200, 2000);
	numberSliderW = RandomInt(200, 2000);
	numberSliderB = RandomInt(200, 2000);
	numberSliderO = RandomInt(200, 2000);
	numberSliderK = RandomInt(200, 2000);
	numberSliderC = RandomInt(200, 2000);
	numberSliderD = RandomInt(200, 2000);

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

	probabilitySlider_a_RR = RandomFloat(0, 100);
	probabilitySlider_a_RG = RandomFloat(0, 100);
	probabilitySlider_a_RB = RandomFloat(0, 100);
	probabilitySlider_a_RW = RandomFloat(0, 100);
	probabilitySlider_a_RO = RandomFloat(0, 100);
	probabilitySlider_a_RK = RandomFloat(0, 100);
	probabilitySlider_a_RC = RandomFloat(0, 100);
	probabilitySlider_a_RD = RandomFloat(0, 100);

	probabilitySlider_r_RR = RandomFloat(0, 100);
	probabilitySlider_r_RG = RandomFloat(0, 100);
	probabilitySlider_r_RB = RandomFloat(0, 100);
	probabilitySlider_r_RW = RandomFloat(0, 100);
	probabilitySlider_r_RO = RandomFloat(0, 100);
	probabilitySlider_r_RK = RandomFloat(0, 100);
	probabilitySlider_r_RC = RandomFloat(0, 100);
	probabilitySlider_r_RD = RandomFloat(0, 100);

	probabilitySlider_a_GR = RandomFloat(0, 100);
	probabilitySlider_a_GG = RandomFloat(0, 100);
	probabilitySlider_a_GB = RandomFloat(0, 100);
	probabilitySlider_a_GW = RandomFloat(0, 100);
	probabilitySlider_a_GO = RandomFloat(0, 100);
	probabilitySlider_a_GK = RandomFloat(0, 100);
	probabilitySlider_a_GC = RandomFloat(0, 100);
	probabilitySlider_a_GD = RandomFloat(0, 100);

	probabilitySlider_r_GR = RandomFloat(0, 100);
	probabilitySlider_r_GG = RandomFloat(0, 100);
	probabilitySlider_r_GB = RandomFloat(0, 100);
	probabilitySlider_r_GW = RandomFloat(0, 100);
	probabilitySlider_r_GO = RandomFloat(0, 100);
	probabilitySlider_r_GK = RandomFloat(0, 100);
	probabilitySlider_r_GC = RandomFloat(0, 100);
	probabilitySlider_r_GD = RandomFloat(0, 100);

	probabilitySlider_a_BR = RandomFloat(0, 100);
	probabilitySlider_a_BG = RandomFloat(0, 100);
	probabilitySlider_a_BB = RandomFloat(0, 100);
	probabilitySlider_a_BW = RandomFloat(0, 100);
	probabilitySlider_a_BO = RandomFloat(0, 100);
	probabilitySlider_a_BK = RandomFloat(0, 100);
	probabilitySlider_a_BC = RandomFloat(0, 100);
	probabilitySlider_a_BD = RandomFloat(0, 100);

	probabilitySlider_r_BR = RandomFloat(0, 100);
	probabilitySlider_r_BG = RandomFloat(0, 100);
	probabilitySlider_r_BB = RandomFloat(0, 100);
	probabilitySlider_r_BW = RandomFloat(0, 100);
	probabilitySlider_r_BO = RandomFloat(0, 100);
	probabilitySlider_r_BK = RandomFloat(0, 100);
	probabilitySlider_r_BC = RandomFloat(0, 100);
	probabilitySlider_r_BD = RandomFloat(0, 100);

	probabilitySlider_a_WR = RandomFloat(0, 100);
	probabilitySlider_a_WG = RandomFloat(0, 100);
	probabilitySlider_a_WB = RandomFloat(0, 100);
	probabilitySlider_a_WW = RandomFloat(0, 100);
	probabilitySlider_a_WO = RandomFloat(0, 100);
	probabilitySlider_a_WK = RandomFloat(0, 100);
	probabilitySlider_a_WC = RandomFloat(0, 100);
	probabilitySlider_a_WD = RandomFloat(0, 100);

	probabilitySlider_r_WR = RandomFloat(0, 100);
	probabilitySlider_r_WG = RandomFloat(0, 100);
	probabilitySlider_r_WB = RandomFloat(0, 100);
	probabilitySlider_r_WW = RandomFloat(0, 100);
	probabilitySlider_r_WO = RandomFloat(0, 100);
	probabilitySlider_r_WK = RandomFloat(0, 100);
	probabilitySlider_r_WC = RandomFloat(0, 100);
	probabilitySlider_r_WD = RandomFloat(0, 100);

	probabilitySlider_a_OR = RandomFloat(0, 100);
	probabilitySlider_a_OG = RandomFloat(0, 100);
	probabilitySlider_a_OB = RandomFloat(0, 100);
	probabilitySlider_a_OW = RandomFloat(0, 100);
	probabilitySlider_a_OO = RandomFloat(0, 100);
	probabilitySlider_a_OK = RandomFloat(0, 100);
	probabilitySlider_a_OC = RandomFloat(0, 100);
	probabilitySlider_a_OD = RandomFloat(0, 100);

	probabilitySlider_r_OR = RandomFloat(0, 100);
	probabilitySlider_r_OG = RandomFloat(0, 100);
	probabilitySlider_r_OB = RandomFloat(0, 100);
	probabilitySlider_r_OW = RandomFloat(0, 100);
	probabilitySlider_r_OO = RandomFloat(0, 100);
	probabilitySlider_r_OK = RandomFloat(0, 100);
	probabilitySlider_r_OC = RandomFloat(0, 100);
	probabilitySlider_r_OD = RandomFloat(0, 100);

	probabilitySlider_a_KR = RandomFloat(0, 100);
	probabilitySlider_a_KG = RandomFloat(0, 100);
	probabilitySlider_a_KB = RandomFloat(0, 100);
	probabilitySlider_a_KW = RandomFloat(0, 100);
	probabilitySlider_a_KO = RandomFloat(0, 100);
	probabilitySlider_a_KK = RandomFloat(0, 100);
	probabilitySlider_a_KC = RandomFloat(0, 100);
	probabilitySlider_a_KD = RandomFloat(0, 100);

	probabilitySlider_r_KR = RandomFloat(0, 100);
	probabilitySlider_r_KG = RandomFloat(0, 100);
	probabilitySlider_r_KB = RandomFloat(0, 100);
	probabilitySlider_r_KW = RandomFloat(0, 100);
	probabilitySlider_r_KO = RandomFloat(0, 100);
	probabilitySlider_r_KK = RandomFloat(0, 100);
	probabilitySlider_r_KC = RandomFloat(0, 100);
	probabilitySlider_r_KD = RandomFloat(0, 100);

	probabilitySlider_a_CR = RandomFloat(0, 100);
	probabilitySlider_a_CG = RandomFloat(0, 100);
	probabilitySlider_a_CB = RandomFloat(0, 100);
	probabilitySlider_a_CW = RandomFloat(0, 100);
	probabilitySlider_a_CO = RandomFloat(0, 100);
	probabilitySlider_a_CK = RandomFloat(0, 100);
	probabilitySlider_a_CC = RandomFloat(0, 100);
	probabilitySlider_a_CD = RandomFloat(0, 100);

	probabilitySlider_r_CR = RandomFloat(0, 100);
	probabilitySlider_r_CG = RandomFloat(0, 100);
	probabilitySlider_r_CB = RandomFloat(0, 100);
	probabilitySlider_r_CW = RandomFloat(0, 100);
	probabilitySlider_r_CO = RandomFloat(0, 100);
	probabilitySlider_r_CK = RandomFloat(0, 100);
	probabilitySlider_r_CC = RandomFloat(0, 100);
	probabilitySlider_r_CD = RandomFloat(0, 100);

	probabilitySlider_a_DR = RandomFloat(0, 100);
	probabilitySlider_a_DG = RandomFloat(0, 100);
	probabilitySlider_a_DB = RandomFloat(0, 100);
	probabilitySlider_a_DW = RandomFloat(0, 100);
	probabilitySlider_a_DO = RandomFloat(0, 100);
	probabilitySlider_a_DK = RandomFloat(0, 100);
	probabilitySlider_a_DC = RandomFloat(0, 100);
	probabilitySlider_a_DD = RandomFloat(0, 100);

	probabilitySlider_r_DR = RandomFloat(0, 100);
	probabilitySlider_r_DG = RandomFloat(0, 100);
	probabilitySlider_r_DB = RandomFloat(0, 100);
	probabilitySlider_r_DW = RandomFloat(0, 100);
	probabilitySlider_r_DO = RandomFloat(0, 100);
	probabilitySlider_r_DK = RandomFloat(0, 100);
	probabilitySlider_r_DC = RandomFloat(0, 100);
	probabilitySlider_r_DD = RandomFloat(0, 100);

	// GREEN
	//numberSliderG = RandomFloat(0, 3000);
	powerSlider_a_GG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_GG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_GG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_GG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GD = RandomFloat(0, 500) * radiusVarianceR;

	// RED
	//numberSliderR = RandomFloat(0, 3000);
	powerSlider_a_RG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_RG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_RG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_RG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RD = RandomFloat(0, 500) * radiusVarianceR;

	// BLUE
	//numberSliderB = RandomFloat(0, 3000);
	powerSlider_a_BG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_BG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_BG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_BG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BD = RandomFloat(0, 500) * radiusVarianceR;

	// WHITE
	// numberSliderW = RandomFloat(0, 3000);
	powerSlider_a_WG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_WG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_WG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_WG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WD = RandomFloat(0, 500) * radiusVarianceR;

	// ORANGE
	//numberSliderO = RandomFloat(0, 3000);
	powerSlider_a_OG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_OG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_OG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_OG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OD = RandomFloat(0, 500) * radiusVarianceR;

	// KHAKI
	//numberSliderK = RandomFloat(0, 3000);
	powerSlider_a_KG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_KG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_KG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_KG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KD = RandomFloat(0, 500) * radiusVarianceR;

	// CRIMSON
	//numberSliderC = RandomFloat(0, 3000);
	powerSlider_a_CG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_CG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_CG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_CG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CD = RandomFloat(0, 500) * radiusVarianceR;

	// DARK
	//numberSliderD = RandomFloat(0, 3000);
	powerSlider_a_DG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_DG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_DG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_DG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DD = RandomFloat(0, 500) * radiusVarianceR;
}
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
void ofApp::rndvsc() {
	viscoSlider = RandomFloat(0.4, 0.9);
	viscoSliderR = RandomFloat(0.4, 0.9);
	viscoSliderG = RandomFloat(0.4, 0.9);
	viscoSliderW = RandomFloat(0.4, 0.9);
	viscoSliderB = RandomFloat(0.4, 0.9);
	viscoSliderO = RandomFloat(0.4, 0.9);
	viscoSliderK = RandomFloat(0.4, 0.9);
	viscoSliderC = RandomFloat(0.4, 0.9);
	viscoSliderD = RandomFloat(0.4, 0.9);
}
void ofApp::rndprob() {
	probabilitySlider = RandomFloat(30, 100);

	probabilitySlider_a_RR = RandomFloat(0, 100);
	probabilitySlider_a_RG = RandomFloat(0, 100);
	probabilitySlider_a_RB = RandomFloat(0, 100);
	probabilitySlider_a_RW = RandomFloat(0, 100);
	probabilitySlider_a_RO = RandomFloat(0, 100);
	probabilitySlider_a_RK = RandomFloat(0, 100);
	probabilitySlider_a_RC = RandomFloat(0, 100);
	probabilitySlider_a_RD = RandomFloat(0, 100);

	probabilitySlider_r_RR = RandomFloat(0, 100);
	probabilitySlider_r_RG = RandomFloat(0, 100);
	probabilitySlider_r_RB = RandomFloat(0, 100);
	probabilitySlider_r_RW = RandomFloat(0, 100);
	probabilitySlider_r_RO = RandomFloat(0, 100);
	probabilitySlider_r_RK = RandomFloat(0, 100);
	probabilitySlider_r_RC = RandomFloat(0, 100);
	probabilitySlider_r_RD = RandomFloat(0, 100);

	probabilitySlider_a_GR = RandomFloat(0, 100);
	probabilitySlider_a_GG = RandomFloat(0, 100);
	probabilitySlider_a_GB = RandomFloat(0, 100);
	probabilitySlider_a_GW = RandomFloat(0, 100);
	probabilitySlider_a_GO = RandomFloat(0, 100);
	probabilitySlider_a_GK = RandomFloat(0, 100);
	probabilitySlider_a_GC = RandomFloat(0, 100);
	probabilitySlider_a_GD = RandomFloat(0, 100);

	probabilitySlider_r_GR = RandomFloat(0, 100);
	probabilitySlider_r_GG = RandomFloat(0, 100);
	probabilitySlider_r_GB = RandomFloat(0, 100);
	probabilitySlider_r_GW = RandomFloat(0, 100);
	probabilitySlider_r_GO = RandomFloat(0, 100);
	probabilitySlider_r_GK = RandomFloat(0, 100);
	probabilitySlider_r_GC = RandomFloat(0, 100);
	probabilitySlider_r_GD = RandomFloat(0, 100);

	probabilitySlider_a_BR = RandomFloat(0, 100);
	probabilitySlider_a_BG = RandomFloat(0, 100);
	probabilitySlider_a_BB = RandomFloat(0, 100);
	probabilitySlider_a_BW = RandomFloat(0, 100);
	probabilitySlider_a_BO = RandomFloat(0, 100);
	probabilitySlider_a_BK = RandomFloat(0, 100);
	probabilitySlider_a_BC = RandomFloat(0, 100);
	probabilitySlider_a_BD = RandomFloat(0, 100);

	probabilitySlider_r_BR = RandomFloat(0, 100);
	probabilitySlider_r_BG = RandomFloat(0, 100);
	probabilitySlider_r_BB = RandomFloat(0, 100);
	probabilitySlider_r_BW = RandomFloat(0, 100);
	probabilitySlider_r_BO = RandomFloat(0, 100);
	probabilitySlider_r_BK = RandomFloat(0, 100);
	probabilitySlider_r_BC = RandomFloat(0, 100);
	probabilitySlider_r_BD = RandomFloat(0, 100);

	probabilitySlider_a_WR = RandomFloat(0, 100);
	probabilitySlider_a_WG = RandomFloat(0, 100);
	probabilitySlider_a_WB = RandomFloat(0, 100);
	probabilitySlider_a_WW = RandomFloat(0, 100);
	probabilitySlider_a_WO = RandomFloat(0, 100);
	probabilitySlider_a_WK = RandomFloat(0, 100);
	probabilitySlider_a_WC = RandomFloat(0, 100);
	probabilitySlider_a_WD = RandomFloat(0, 100);

	probabilitySlider_r_WR = RandomFloat(0, 100);
	probabilitySlider_r_WG = RandomFloat(0, 100);
	probabilitySlider_r_WB = RandomFloat(0, 100);
	probabilitySlider_r_WW = RandomFloat(0, 100);
	probabilitySlider_r_WO = RandomFloat(0, 100);
	probabilitySlider_r_WK = RandomFloat(0, 100);
	probabilitySlider_r_WC = RandomFloat(0, 100);
	probabilitySlider_r_WD = RandomFloat(0, 100);

	probabilitySlider_a_OR = RandomFloat(0, 100);
	probabilitySlider_a_OG = RandomFloat(0, 100);
	probabilitySlider_a_OB = RandomFloat(0, 100);
	probabilitySlider_a_OW = RandomFloat(0, 100);
	probabilitySlider_a_OO = RandomFloat(0, 100);
	probabilitySlider_a_OK = RandomFloat(0, 100);
	probabilitySlider_a_OC = RandomFloat(0, 100);
	probabilitySlider_a_OD = RandomFloat(0, 100);

	probabilitySlider_r_OR = RandomFloat(0, 100);
	probabilitySlider_r_OG = RandomFloat(0, 100);
	probabilitySlider_r_OB = RandomFloat(0, 100);
	probabilitySlider_r_OW = RandomFloat(0, 100);
	probabilitySlider_r_OO = RandomFloat(0, 100);
	probabilitySlider_r_OK = RandomFloat(0, 100);
	probabilitySlider_r_OC = RandomFloat(0, 100);
	probabilitySlider_r_OD = RandomFloat(0, 100);

	probabilitySlider_a_KR = RandomFloat(0, 100);
	probabilitySlider_a_KG = RandomFloat(0, 100);
	probabilitySlider_a_KB = RandomFloat(0, 100);
	probabilitySlider_a_KW = RandomFloat(0, 100);
	probabilitySlider_a_KO = RandomFloat(0, 100);
	probabilitySlider_a_KK = RandomFloat(0, 100);
	probabilitySlider_a_KC = RandomFloat(0, 100);
	probabilitySlider_a_KD = RandomFloat(0, 100);

	probabilitySlider_r_KR = RandomFloat(0, 100);
	probabilitySlider_r_KG = RandomFloat(0, 100);
	probabilitySlider_r_KB = RandomFloat(0, 100);
	probabilitySlider_r_KW = RandomFloat(0, 100);
	probabilitySlider_r_KO = RandomFloat(0, 100);
	probabilitySlider_r_KK = RandomFloat(0, 100);
	probabilitySlider_r_KC = RandomFloat(0, 100);
	probabilitySlider_r_KD = RandomFloat(0, 100);

	probabilitySlider_a_CR = RandomFloat(0, 100);
	probabilitySlider_a_CG = RandomFloat(0, 100);
	probabilitySlider_a_CB = RandomFloat(0, 100);
	probabilitySlider_a_CW = RandomFloat(0, 100);
	probabilitySlider_a_CO = RandomFloat(0, 100);
	probabilitySlider_a_CK = RandomFloat(0, 100);
	probabilitySlider_a_CC = RandomFloat(0, 100);
	probabilitySlider_a_CD = RandomFloat(0, 100);

	probabilitySlider_r_CR = RandomFloat(0, 100);
	probabilitySlider_r_CG = RandomFloat(0, 100);
	probabilitySlider_r_CB = RandomFloat(0, 100);
	probabilitySlider_r_CW = RandomFloat(0, 100);
	probabilitySlider_r_CO = RandomFloat(0, 100);
	probabilitySlider_r_CK = RandomFloat(0, 100);
	probabilitySlider_r_CC = RandomFloat(0, 100);
	probabilitySlider_r_CD = RandomFloat(0, 100);

	probabilitySlider_a_DR = RandomFloat(0, 100);
	probabilitySlider_a_DG = RandomFloat(0, 100);
	probabilitySlider_a_DB = RandomFloat(0, 100);
	probabilitySlider_a_DW = RandomFloat(0, 100);
	probabilitySlider_a_DO = RandomFloat(0, 100);
	probabilitySlider_a_DK = RandomFloat(0, 100);
	probabilitySlider_a_DC = RandomFloat(0, 100);
	probabilitySlider_a_DD = RandomFloat(0, 100);

	probabilitySlider_r_DR = RandomFloat(0, 100);
	probabilitySlider_r_DG = RandomFloat(0, 100);
	probabilitySlider_r_DB = RandomFloat(0, 100);
	probabilitySlider_r_DW = RandomFloat(0, 100);
	probabilitySlider_r_DO = RandomFloat(0, 100);
	probabilitySlider_r_DK = RandomFloat(0, 100);
	probabilitySlider_r_DC = RandomFloat(0, 100);
	probabilitySlider_r_DD = RandomFloat(0, 100);
}
void ofApp::rndir() {
	// GREEN
	//numberSliderG = RandomFloat(0, 3000);
	powerSlider_a_GG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_GD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_GG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_GD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_GG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_GD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_GG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_GD = RandomFloat(0, 500) * radiusVarianceR;

	// RED
	//numberSliderR = RandomFloat(0, 3000);
	powerSlider_a_RG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_RD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_RG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_RD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_RG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_RD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_RG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_RD = RandomFloat(0, 500) * radiusVarianceR;

	// BLUE
	//numberSliderB = RandomFloat(0, 3000);
	powerSlider_a_BG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_BD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_BG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_BD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_BG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_BD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_BG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_BD = RandomFloat(0, 500) * radiusVarianceR;

	// WHITE
	// numberSliderW = RandomFloat(0, 3000);
	powerSlider_a_WG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_WD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_WG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_WD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_WG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_WD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_WG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_WD = RandomFloat(0, 500) * radiusVarianceR;

	// ORANGE
	//numberSliderO = RandomFloat(0, 3000);
	powerSlider_a_OG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_OD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_OG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_OD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_OG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_OD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_OG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_OD = RandomFloat(0, 500) * radiusVarianceR;

	// KHAKI
	//numberSliderK = RandomFloat(0, 3000);
	powerSlider_a_KG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_KD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_KG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_KD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_KG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_KD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_KG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_KD = RandomFloat(0, 500) * radiusVarianceR;

	// CRIMSON
	//numberSliderC = RandomFloat(0, 3000);
	powerSlider_a_CG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_CD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_CG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_CD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_CG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_CD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_CG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_CD = RandomFloat(0, 500) * radiusVarianceR;

	// DARK
	//numberSliderD = RandomFloat(0, 3000);
	powerSlider_a_DG = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DR = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DB = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DW = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DO = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DK = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DC = RandomFloat(0, 100) * forceVarianceA;
	powerSlider_a_DD = RandomFloat(0, 100) * forceVarianceA;

	vSlider_a_DG = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DR = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DB = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DW = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DO = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DK = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DC = RandomFloat(0, 500) * radiusVarianceA;
	vSlider_a_DD = RandomFloat(0, 500) * radiusVarianceA;

	powerSlider_r_DG = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DR = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DB = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DW = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DO = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DK = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DC = RandomFloat(-100, 0) * forceVarianceR;
	powerSlider_r_DD = RandomFloat(-100, 0) * forceVarianceR;

	vSlider_r_DG = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DR = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DB = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DW = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DO = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DK = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DC = RandomFloat(0, 500) * radiusVarianceR;
	vSlider_r_DD = RandomFloat(0, 500) * radiusVarianceR;
}

/// this is a cheap and quick way to save and load parameters (openFramework have betters ways but requires some additional library setups) 
// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::saveSettings()
{
	const std::vector<float> settings = {
		powerSlider_a_GG, powerSlider_a_GR, powerSlider_a_GB, powerSlider_a_GW, powerSlider_a_GO, powerSlider_a_GK, powerSlider_a_GC, powerSlider_a_GD,
		vSlider_a_GG, vSlider_a_GR, vSlider_a_GB, vSlider_a_GW, vSlider_a_GO, vSlider_a_GK, vSlider_a_GC, vSlider_a_GD,
		powerSlider_r_GG, powerSlider_r_GR, powerSlider_r_GB, powerSlider_r_GW, powerSlider_r_GO, powerSlider_r_GK, powerSlider_r_GC, powerSlider_r_GD,
		vSlider_r_GG, vSlider_r_GR, vSlider_r_GB, vSlider_r_GW, vSlider_r_GO, vSlider_r_GK, vSlider_r_GC, vSlider_r_GD,
		powerSlider_a_RG, powerSlider_a_RR, powerSlider_a_RB, powerSlider_a_RW, powerSlider_a_RO, powerSlider_a_RK, powerSlider_a_RC, powerSlider_a_RD,
		vSlider_a_RG, vSlider_a_RR, vSlider_a_RB, vSlider_a_RW, vSlider_a_RO, vSlider_a_RK, vSlider_a_RC, vSlider_a_RD,
		powerSlider_r_RG, powerSlider_r_RR, powerSlider_r_RB, powerSlider_r_RW, powerSlider_r_RO, powerSlider_r_RK, powerSlider_r_RC, powerSlider_r_RD,
		vSlider_r_RG, vSlider_r_RR, vSlider_r_RB, vSlider_r_RW, vSlider_r_RO, vSlider_r_RK, vSlider_r_RC, vSlider_r_RD,
		powerSlider_a_BG, powerSlider_a_BR, powerSlider_a_BB, powerSlider_a_BW, powerSlider_a_BO, powerSlider_a_BK, powerSlider_a_BC, powerSlider_a_BD,
		vSlider_a_BG, vSlider_a_BR, vSlider_a_BB, vSlider_a_BW, vSlider_a_BO, vSlider_a_BK, vSlider_a_BC, vSlider_a_BD,
		powerSlider_r_BG, powerSlider_r_BR, powerSlider_r_BB, powerSlider_r_BW, powerSlider_r_BO, powerSlider_r_BK, powerSlider_r_BC, powerSlider_r_BD,
		vSlider_r_BG, vSlider_r_BR, vSlider_r_BB, vSlider_r_BW, vSlider_r_BO, vSlider_r_BK, vSlider_r_BC, vSlider_r_BD,
		powerSlider_a_WG, powerSlider_a_WR, powerSlider_a_WB, powerSlider_a_WW, powerSlider_a_WO, powerSlider_a_WK, powerSlider_a_WC, powerSlider_a_WD,
		vSlider_a_WG, vSlider_a_WR, vSlider_a_WB, vSlider_a_WW, vSlider_a_WO, vSlider_a_WK, vSlider_a_WC, vSlider_a_WD,
		powerSlider_r_WG, powerSlider_r_WR, powerSlider_r_WB, powerSlider_r_WW, powerSlider_r_WO, powerSlider_r_WK, powerSlider_r_WC, powerSlider_r_WD,
		vSlider_r_WG, vSlider_r_WR, vSlider_r_WB, vSlider_r_WW, vSlider_r_WO, vSlider_r_WK, vSlider_r_WC, vSlider_r_WD,
		powerSlider_a_OG, powerSlider_a_OR, powerSlider_a_OB, powerSlider_a_OW, powerSlider_a_OO, powerSlider_a_OK, powerSlider_a_OC, powerSlider_a_OD,
		vSlider_a_OG, vSlider_a_OR, vSlider_a_OB, vSlider_a_OW, vSlider_a_OO, vSlider_a_OK, vSlider_a_OC, vSlider_a_OD,
		powerSlider_r_OG, powerSlider_r_OR, powerSlider_r_OB, powerSlider_r_OW, powerSlider_r_OO, powerSlider_r_OK, powerSlider_r_OC, powerSlider_r_OD,
		vSlider_r_OG, vSlider_r_OR, vSlider_r_OB, vSlider_r_OW, vSlider_r_OO, vSlider_r_OK, vSlider_r_OC, vSlider_r_OD,
		powerSlider_a_KG, powerSlider_a_KR, powerSlider_a_KB, powerSlider_a_KW, powerSlider_a_KO, powerSlider_a_KK, powerSlider_a_KC, powerSlider_a_KD,
		vSlider_a_KG, vSlider_a_KR, vSlider_a_KB, vSlider_a_KW, vSlider_a_KO, vSlider_a_KK, vSlider_a_KC, vSlider_a_KD,
		powerSlider_r_KG, powerSlider_r_KR, powerSlider_r_KB, powerSlider_r_KW, powerSlider_r_KO, powerSlider_r_KK, powerSlider_r_KC, powerSlider_r_KD,
		vSlider_r_KG, vSlider_r_KR, vSlider_r_KB, vSlider_r_KW, vSlider_r_KO, vSlider_r_KK, vSlider_r_KC, vSlider_r_KD,
		powerSlider_a_CG, powerSlider_a_CR, powerSlider_a_CB, powerSlider_a_CW, powerSlider_a_CO, powerSlider_a_CK, powerSlider_a_CC, powerSlider_a_CD,
		vSlider_a_CG, vSlider_a_CR, vSlider_a_CB, vSlider_a_CW, vSlider_a_CO, vSlider_a_CK, vSlider_a_CC, vSlider_a_CD,
		powerSlider_r_CG, powerSlider_r_CR, powerSlider_r_CB, powerSlider_r_CW, powerSlider_r_CO, powerSlider_r_CK, powerSlider_r_CC, powerSlider_r_CD,
		vSlider_r_CG, vSlider_r_CR, vSlider_r_CB, vSlider_r_CW, vSlider_r_CO, vSlider_r_CK, vSlider_r_CC, vSlider_r_CD,
		powerSlider_a_DG, powerSlider_a_DR, powerSlider_a_DB, powerSlider_a_DW, powerSlider_a_DO, powerSlider_a_DK, powerSlider_a_DC, powerSlider_a_DD,
		vSlider_a_DG, vSlider_a_DR, vSlider_a_DB, vSlider_a_DW, vSlider_a_DO, vSlider_a_DK, vSlider_a_DC, vSlider_a_DD,
		powerSlider_r_DG, powerSlider_r_DR, powerSlider_r_DB, powerSlider_r_DW, powerSlider_r_DO, powerSlider_r_DK, powerSlider_r_DC, powerSlider_r_DD,
		vSlider_r_DG, vSlider_r_DR, vSlider_r_DB, vSlider_r_DW, vSlider_r_DO, vSlider_r_DK, vSlider_r_DC, vSlider_r_DD,
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
		probabilitySlider_a_RR,
		probabilitySlider_a_RG,
		probabilitySlider_a_RB,
		probabilitySlider_a_RW,
		probabilitySlider_a_RO,
		probabilitySlider_a_RK,
		probabilitySlider_a_RC,
		probabilitySlider_a_RD,
		probabilitySlider_r_RR,
		probabilitySlider_r_RG,
		probabilitySlider_r_RB,
		probabilitySlider_r_RW,
		probabilitySlider_r_RO,
		probabilitySlider_r_RK,
		probabilitySlider_r_RC,
		probabilitySlider_r_RD,
		probabilitySlider_a_GR,
		probabilitySlider_a_GG,
		probabilitySlider_a_GB,
		probabilitySlider_a_GW,
		probabilitySlider_a_GO,
		probabilitySlider_a_GK,
		probabilitySlider_a_GC,
		probabilitySlider_a_GD,
		probabilitySlider_r_GR,
		probabilitySlider_r_GG,
		probabilitySlider_r_GB,
		probabilitySlider_r_GW,
		probabilitySlider_r_GO,
		probabilitySlider_r_GK,
		probabilitySlider_r_GC,
		probabilitySlider_r_GD,
		probabilitySlider_a_BR,
		probabilitySlider_a_BG,
		probabilitySlider_a_BB,
		probabilitySlider_a_BW,
		probabilitySlider_a_BO,
		probabilitySlider_a_BK,
		probabilitySlider_a_BC,
		probabilitySlider_a_BD,
		probabilitySlider_r_BR,
		probabilitySlider_r_BG,
		probabilitySlider_r_BB,
		probabilitySlider_r_BW,
		probabilitySlider_r_BO,
		probabilitySlider_r_BK,
		probabilitySlider_r_BC,
		probabilitySlider_r_BD,
		probabilitySlider_a_WR,
		probabilitySlider_a_WG,
		probabilitySlider_a_WB,
		probabilitySlider_a_WW,
		probabilitySlider_a_WO,
		probabilitySlider_a_WK,
		probabilitySlider_a_WC,
		probabilitySlider_a_WD,
		probabilitySlider_r_WR,
		probabilitySlider_r_WG,
		probabilitySlider_r_WB,
		probabilitySlider_r_WW,
		probabilitySlider_r_WO,
		probabilitySlider_r_WK,
		probabilitySlider_r_WC,
		probabilitySlider_r_WD,
		probabilitySlider_a_OR,
		probabilitySlider_a_OG,
		probabilitySlider_a_OB,
		probabilitySlider_a_OW,
		probabilitySlider_a_OO,
		probabilitySlider_a_OK,
		probabilitySlider_a_OC,
		probabilitySlider_a_OD,
		probabilitySlider_r_OR,
		probabilitySlider_r_OG,
		probabilitySlider_r_OB,
		probabilitySlider_r_OW,
		probabilitySlider_r_OO,
		probabilitySlider_r_OK,
		probabilitySlider_r_OC,
		probabilitySlider_r_OD,
		probabilitySlider_a_KR,
		probabilitySlider_a_KG,
		probabilitySlider_a_KB,
		probabilitySlider_a_KW,
		probabilitySlider_a_KO,
		probabilitySlider_a_KK,
		probabilitySlider_a_KC,
		probabilitySlider_a_KD,
		probabilitySlider_r_KR,
		probabilitySlider_r_KG,
		probabilitySlider_r_KB,
		probabilitySlider_r_KW,
		probabilitySlider_r_KO,
		probabilitySlider_r_KK,
		probabilitySlider_r_KC,
		probabilitySlider_r_KD,
		probabilitySlider_a_CR,
		probabilitySlider_a_CG,
		probabilitySlider_a_CB,
		probabilitySlider_a_CW,
		probabilitySlider_a_CO,
		probabilitySlider_a_CK,
		probabilitySlider_a_CC,
		probabilitySlider_a_CD,
		probabilitySlider_r_CR,
		probabilitySlider_r_CG,
		probabilitySlider_r_CB,
		probabilitySlider_r_CW,
		probabilitySlider_r_CO,
		probabilitySlider_r_CK,
		probabilitySlider_r_CC,
		probabilitySlider_r_CD,
		probabilitySlider_a_DR,
		probabilitySlider_a_DG,
		probabilitySlider_a_DB,
		probabilitySlider_a_DW,
		probabilitySlider_a_DO,
		probabilitySlider_a_DK,
		probabilitySlider_a_DC,
		probabilitySlider_a_DD,
		probabilitySlider_r_DR,
		probabilitySlider_r_DG,
		probabilitySlider_r_DB,
		probabilitySlider_r_DW,
		probabilitySlider_r_DO,
		probabilitySlider_r_DK,
		probabilitySlider_r_DC,
		probabilitySlider_r_DD,
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

	if (p.size() < 404)
	{
		// better checks needed
		ofSystemAlertDialog("Could not read the file!");
	}
	else
	{
		powerSlider_a_GG = p[0];
		powerSlider_a_GR = p[1];
		powerSlider_a_GW = p[2];
		powerSlider_a_GB = p[3];
		powerSlider_a_GO = p[4];
		powerSlider_a_GK = p[5];
		powerSlider_a_GC = p[6];
		powerSlider_a_GD = p[7];
		vSlider_a_GG = p[8];
		vSlider_a_GR = p[9];
		vSlider_a_GW = p[10];
		vSlider_a_GB = p[11];
		vSlider_a_GO = p[12];
		vSlider_a_GK = p[13];
		vSlider_a_GC = p[14];
		vSlider_a_GD = p[15];
		powerSlider_r_GG = p[16];
		powerSlider_r_GR = p[17];
		powerSlider_r_GW = p[18];
		powerSlider_r_GB = p[19];
		powerSlider_r_GO = p[20];
		powerSlider_r_GK = p[21];
		powerSlider_r_GC = p[22];
		powerSlider_r_GD = p[23];
		vSlider_r_GG = p[24];
		vSlider_r_GR = p[25];
		vSlider_r_GW = p[26];
		vSlider_r_GB = p[27];
		vSlider_r_GO = p[28];
		vSlider_r_GK = p[29];
		vSlider_r_GC = p[30];
		vSlider_r_GD = p[31];
		powerSlider_a_RG = p[32];
		powerSlider_a_RR = p[33];
		powerSlider_a_RW = p[34];
		powerSlider_a_RB = p[35];
		powerSlider_a_RO = p[36];
		powerSlider_a_RK = p[37];
		powerSlider_a_RC = p[38];
		powerSlider_a_RD = p[39];
		vSlider_a_RG = p[40];
		vSlider_a_RR = p[41];
		vSlider_a_RW = p[42];
		vSlider_a_RB = p[43];
		vSlider_a_RO = p[44];
		vSlider_a_RK = p[45];
		vSlider_a_RC = p[46];
		vSlider_a_RD = p[47];
		powerSlider_r_RG = p[48];
		powerSlider_r_RR = p[49];
		powerSlider_r_RW = p[50];
		powerSlider_r_RB = p[51];
		powerSlider_r_RO = p[52];
		powerSlider_r_RK = p[53];
		powerSlider_r_RC = p[54];
		powerSlider_r_RD = p[55];
		vSlider_r_RG = p[56];
		vSlider_r_RR = p[57];
		vSlider_r_RW = p[58];
		vSlider_r_RB = p[59];
		vSlider_r_RO = p[60];
		vSlider_r_RK = p[61];
		vSlider_r_RC = p[62];
		vSlider_r_RD = p[63];
		powerSlider_a_BG = p[64];
		powerSlider_a_BR = p[65];
		powerSlider_a_BW = p[66];
		powerSlider_a_BB = p[67];
		powerSlider_a_BO = p[68];
		powerSlider_a_BK = p[69];
		powerSlider_a_BC = p[70];
		powerSlider_a_BD = p[71];
		vSlider_a_BG = p[72];
		vSlider_a_BR = p[73];
		vSlider_a_BW = p[74];
		vSlider_a_BB = p[75];
		vSlider_a_BO = p[76];
		vSlider_a_BK = p[77];
		vSlider_a_BC = p[78];
		vSlider_a_BD = p[79];
		powerSlider_r_BG = p[80];
		powerSlider_r_BR = p[81];
		powerSlider_r_BW = p[82];
		powerSlider_r_BB = p[83];
		powerSlider_r_BO = p[84];
		powerSlider_r_BK = p[85];
		powerSlider_r_BC = p[86];
		powerSlider_r_BD = p[87];
		vSlider_r_BG = p[88];
		vSlider_r_BR = p[89];
		vSlider_r_BW = p[90];
		vSlider_r_BB = p[91];
		vSlider_r_BO = p[92];
		vSlider_r_BK = p[93];
		vSlider_r_BC = p[94];
		vSlider_r_BD = p[95];
		powerSlider_a_WG = p[96];
		powerSlider_a_WR = p[97];
		powerSlider_a_WW = p[98];
		powerSlider_a_WB = p[99];
		powerSlider_a_WO = p[100];
		powerSlider_a_WK = p[101];
		powerSlider_a_WC = p[102];
		powerSlider_a_WD = p[103];
		vSlider_a_WG = p[104];
		vSlider_a_WR = p[105];
		vSlider_a_WW = p[106];
		vSlider_a_WB = p[107];
		vSlider_a_WO = p[108];
		vSlider_a_WK = p[109];
		vSlider_a_WC = p[110];
		vSlider_a_WD = p[111];
		powerSlider_r_WG = p[112];
		powerSlider_r_WR = p[113];
		powerSlider_r_WW = p[114];
		powerSlider_r_WB = p[115];
		powerSlider_r_WO = p[116];
		powerSlider_r_WK = p[117];
		powerSlider_r_WC = p[118];
		powerSlider_r_WD = p[119];
		vSlider_r_WG = p[120];
		vSlider_r_WR = p[121];
		vSlider_r_WW = p[122];
		vSlider_r_WB = p[123];
		vSlider_r_WO = p[124];
		vSlider_r_WK = p[125];
		vSlider_r_WC = p[126];
		vSlider_r_WD = p[127];
		powerSlider_a_OG = p[128];
		powerSlider_a_OR = p[129];
		powerSlider_a_OW = p[130];
		powerSlider_a_OB = p[131];
		powerSlider_a_OO = p[132];
		powerSlider_a_OK = p[133];
		powerSlider_a_OC = p[134];
		powerSlider_a_OD = p[135];
		vSlider_a_OG = p[136];
		vSlider_a_OR = p[137];
		vSlider_a_OW = p[138];
		vSlider_a_OB = p[139];
		vSlider_a_OO = p[140];
		vSlider_a_OK = p[141];
		vSlider_a_OC = p[142];
		vSlider_a_OD = p[143];
		powerSlider_r_OG = p[144];
		powerSlider_r_OR = p[145];
		powerSlider_r_OW = p[146];
		powerSlider_r_OB = p[147];
		powerSlider_r_OO = p[148];
		powerSlider_r_OK = p[149];
		powerSlider_r_OC = p[150];
		powerSlider_r_OD = p[151];
		vSlider_r_OG = p[152];
		vSlider_r_OR = p[153];
		vSlider_r_OW = p[154];
		vSlider_r_OB = p[155];
		vSlider_r_OO = p[156];
		vSlider_r_OK = p[157];
		vSlider_r_OC = p[158];
		vSlider_r_OD = p[159];
		powerSlider_a_KG = p[160];
		powerSlider_a_KR = p[161];
		powerSlider_a_KW = p[162];
		powerSlider_a_KB = p[163];
		powerSlider_a_KO = p[164];
		powerSlider_a_KK = p[165];
		powerSlider_a_KC = p[166];
		powerSlider_a_KD = p[167];
		vSlider_a_KG = p[168];
		vSlider_a_KR = p[169];
		vSlider_a_KW = p[170];
		vSlider_a_KB = p[171];
		vSlider_a_KO = p[172];
		vSlider_a_KK = p[173];
		vSlider_a_KC = p[174];
		vSlider_a_KD = p[175];
		powerSlider_r_KG = p[176];
		powerSlider_r_KR = p[177];
		powerSlider_r_KW = p[178];
		powerSlider_r_KB = p[179];
		powerSlider_r_KO = p[180];
		powerSlider_r_KK = p[181];
		powerSlider_r_KC = p[182];
		powerSlider_r_KD = p[183];
		vSlider_r_KG = p[184];
		vSlider_r_KR = p[185];
		vSlider_r_KW = p[186];
		vSlider_r_KB = p[187];
		vSlider_r_KO = p[188];
		vSlider_r_KK = p[189];
		vSlider_r_KC = p[190];
		vSlider_r_KD = p[191];
		powerSlider_a_CG = p[192];
		powerSlider_a_CR = p[193];
		powerSlider_a_CW = p[194];
		powerSlider_a_CB = p[195];
		powerSlider_a_CO = p[196];
		powerSlider_a_CK = p[197];
		powerSlider_a_CC = p[198];
		powerSlider_a_CD = p[199];
		vSlider_a_CG = p[200];
		vSlider_a_CR = p[201];
		vSlider_a_CW = p[202];
		vSlider_a_CB = p[203];
		vSlider_a_CO = p[204];
		vSlider_a_CK = p[205];
		vSlider_a_CC = p[206];
		vSlider_a_CD = p[207];
		powerSlider_r_CG = p[208];
		powerSlider_r_CR = p[209];
		powerSlider_r_CW = p[210];
		powerSlider_r_CB = p[211];
		powerSlider_r_CO = p[212];
		powerSlider_r_CK = p[213];
		powerSlider_r_CC = p[214];
		powerSlider_r_CD = p[215];
		vSlider_r_CG = p[216];
		vSlider_r_CR = p[217];
		vSlider_r_CW = p[218];
		vSlider_r_CB = p[219];
		vSlider_r_CO = p[220];
		vSlider_r_CK = p[221];
		vSlider_r_CC = p[222];
		vSlider_r_CD = p[223];
		powerSlider_a_DG = p[224];
		powerSlider_a_DR = p[225];
		powerSlider_a_DW = p[226];
		powerSlider_a_DB = p[227];
		powerSlider_a_DO = p[228];
		powerSlider_a_DK = p[229];
		powerSlider_a_DC = p[230];
		powerSlider_a_DD = p[231];
		vSlider_a_DG = p[232];
		vSlider_a_DR = p[233];
		vSlider_a_DW = p[234];
		vSlider_a_DB = p[235];
		vSlider_a_DO = p[236];
		vSlider_a_DK = p[237];
		vSlider_a_DC = p[238];
		vSlider_a_DD = p[239];
		powerSlider_r_DG = p[240];
		powerSlider_r_DR = p[241];
		powerSlider_r_DW = p[242];
		powerSlider_r_DB = p[243];
		powerSlider_r_DO = p[244];
		powerSlider_r_DK = p[245];
		powerSlider_r_DC = p[246];
		powerSlider_r_DD = p[247];
		vSlider_r_DG = p[248];
		vSlider_r_DR = p[249];
		vSlider_r_DW = p[250];
		vSlider_r_DB = p[251];
		vSlider_r_DO = p[252];
		vSlider_r_DK = p[253];
		vSlider_r_DC = p[254];
		vSlider_r_DD = p[255];
		numberSliderG = static_cast<int>(p[256]);
		numberSliderR = static_cast<int>(p[257]);
		numberSliderW = static_cast<int>(p[258]);
		numberSliderB = static_cast<int>(p[259]);
		numberSliderO = static_cast<int>(p[260]);
		numberSliderK = static_cast<int>(p[261]);
		numberSliderC = static_cast<int>(p[262]);
		numberSliderD = static_cast<int>(p[263]);
		viscoSlider = p[264];
		viscoSliderR = p[265];
		viscoSliderG = p[266];
		viscoSliderB = p[267];
		viscoSliderW = p[268];
		viscoSliderO = p[269];
		viscoSliderK = p[270];
		viscoSliderC = p[271];
		viscoSliderD = p[272];
		evoProbSlider = p[273];
		evoAmountSlider = p[274];
		probabilitySlider = p[275];
		probabilitySlider_a_RR = p[276];
		probabilitySlider_a_RG = p[277];
		probabilitySlider_a_RB = p[278];
		probabilitySlider_a_RW = p[279];
		probabilitySlider_a_RO = p[280];
		probabilitySlider_a_RK = p[281];
		probabilitySlider_a_RC = p[282];
		probabilitySlider_a_RD = p[283];
		probabilitySlider_r_RR = p[284];
		probabilitySlider_r_RG = p[285];
		probabilitySlider_r_RB = p[286];
		probabilitySlider_r_RW = p[287];
		probabilitySlider_r_RO = p[288];
		probabilitySlider_r_RK = p[289];
		probabilitySlider_r_RC = p[290];
		probabilitySlider_r_RD = p[291];
		probabilitySlider_a_GR = p[292];
		probabilitySlider_a_GG = p[293];
		probabilitySlider_a_GB = p[294];
		probabilitySlider_a_GW = p[295];
		probabilitySlider_a_GO = p[296];
		probabilitySlider_a_GK = p[297];
		probabilitySlider_a_GC = p[298];
		probabilitySlider_a_GD = p[299];
		probabilitySlider_r_GR = p[300];
		probabilitySlider_r_GG = p[301];
		probabilitySlider_r_GB = p[302];
		probabilitySlider_r_GW = p[303];
		probabilitySlider_r_GO = p[304];
		probabilitySlider_r_GK = p[305];
		probabilitySlider_r_GC = p[306];
		probabilitySlider_r_GD = p[307];
		probabilitySlider_a_BR = p[308];
		probabilitySlider_a_BG = p[309];
		probabilitySlider_a_BB = p[310];
		probabilitySlider_a_BW = p[311];
		probabilitySlider_a_BO = p[312];
		probabilitySlider_a_BK = p[313];
		probabilitySlider_a_BC = p[314];
		probabilitySlider_a_BD = p[315];
		probabilitySlider_r_BR = p[316];
		probabilitySlider_r_BG = p[317];
		probabilitySlider_r_BB = p[318];
		probabilitySlider_r_BW = p[319];
		probabilitySlider_r_BO = p[320];
		probabilitySlider_r_BK = p[321];
		probabilitySlider_r_BC = p[322];
		probabilitySlider_r_BD = p[323];
		probabilitySlider_a_WR = p[324];
		probabilitySlider_a_WG = p[325];
		probabilitySlider_a_WB = p[326];
		probabilitySlider_a_WW = p[327];
		probabilitySlider_a_WO = p[328];
		probabilitySlider_a_WK = p[329];
		probabilitySlider_a_WC = p[330];
		probabilitySlider_a_WD = p[331];
		probabilitySlider_r_WR = p[332];
		probabilitySlider_r_WG = p[333];
		probabilitySlider_r_WB = p[334];
		probabilitySlider_r_WW = p[335];
		probabilitySlider_r_WO = p[336];
		probabilitySlider_r_WK = p[337];
		probabilitySlider_r_WC = p[338];
		probabilitySlider_r_WD = p[339];
		probabilitySlider_a_OR = p[340];
		probabilitySlider_a_OG = p[341];
		probabilitySlider_a_OB = p[342];
		probabilitySlider_a_OW = p[343];
		probabilitySlider_a_OO = p[344];
		probabilitySlider_a_OK = p[345];
		probabilitySlider_a_OC = p[346];
		probabilitySlider_a_OD = p[347];
		probabilitySlider_r_OR = p[348];
		probabilitySlider_r_OG = p[349];
		probabilitySlider_r_OB = p[350];
		probabilitySlider_r_OW = p[351];
		probabilitySlider_r_OO = p[352];
		probabilitySlider_r_OK = p[353];
		probabilitySlider_r_OC = p[354];
		probabilitySlider_r_OD = p[355];
		probabilitySlider_a_KR = p[356];
		probabilitySlider_a_KG = p[357];
		probabilitySlider_a_KB = p[358];
		probabilitySlider_a_KW = p[359];
		probabilitySlider_a_KO = p[360];
		probabilitySlider_a_KK = p[361];
		probabilitySlider_a_KC = p[362];
		probabilitySlider_a_KD = p[363];
		probabilitySlider_r_KR = p[364];
		probabilitySlider_r_KG = p[365];
		probabilitySlider_r_KB = p[366];
		probabilitySlider_r_KW = p[367];
		probabilitySlider_r_KO = p[368];
		probabilitySlider_r_KK = p[369];
		probabilitySlider_r_KC = p[370];
		probabilitySlider_r_KD = p[371];
		probabilitySlider_a_CR = p[372];
		probabilitySlider_a_CG = p[373];
		probabilitySlider_a_CB = p[374];
		probabilitySlider_a_CW = p[375];
		probabilitySlider_a_CO = p[376];
		probabilitySlider_a_CK = p[377];
		probabilitySlider_a_CC = p[378];
		probabilitySlider_a_CD = p[379];
		probabilitySlider_r_CR = p[380];
		probabilitySlider_r_CG = p[381];
		probabilitySlider_r_CB = p[382];
		probabilitySlider_r_CW = p[383];
		probabilitySlider_r_CO = p[384];
		probabilitySlider_r_CK = p[385];
		probabilitySlider_r_CC = p[386];
		probabilitySlider_r_CD = p[387];
		probabilitySlider_a_DR = p[388];
		probabilitySlider_a_DG = p[389];
		probabilitySlider_a_DB = p[390];
		probabilitySlider_a_DW = p[391];
		probabilitySlider_a_DO = p[392];
		probabilitySlider_a_DK = p[393];
		probabilitySlider_a_DC = p[394];
		probabilitySlider_a_DD = p[395];
		probabilitySlider_r_DR = p[396];
		probabilitySlider_r_DG = p[397];
		probabilitySlider_r_DB = p[398];
		probabilitySlider_r_DW = p[399];
		probabilitySlider_r_DO = p[400];
		probabilitySlider_r_DK = p[401];
		probabilitySlider_r_DC = p[402];
		probabilitySlider_r_DD = p[403];
	}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - 8c2i2r8v128p4e 1.5");
	ofSetVerticalSync(false);

	// Interface
	gui.setup("Settings");
	gui.loadFont("Arial", 12);
	gui.setWidthElements(300.0f);

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
	probabilityRGroup.add(probabilitySlider_a_RR.setup("Interaction Probability RR", probability_a_RR, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RG.setup("Interaction Probability RG", probability_a_RG, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RB.setup("Interaction Probability RB", probability_a_RB, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RW.setup("Interaction Probability RW", probability_a_RW, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RO.setup("Interaction Probability RO", probability_a_RO, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RK.setup("Interaction Probability RK", probability_a_RK, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RC.setup("Interaction Probability RC", probability_a_RC, 0, 100));
	probabilityRGroup.add(probabilitySlider_a_RD.setup("Interaction Probability RD", probability_a_RD, 0, 100));

	probabilityRGroup.add(probabilitySlider_r_RR.setup("Interaction Probability RR", probability_r_RR, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RG.setup("Interaction Probability RG", probability_r_RG, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RB.setup("Interaction Probability RB", probability_r_RB, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RW.setup("Interaction Probability RW", probability_r_RW, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RO.setup("Interaction Probability RO", probability_r_RO, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RK.setup("Interaction Probability RK", probability_r_RK, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RC.setup("Interaction Probability RC", probability_r_RC, 0, 100));
	probabilityRGroup.add(probabilitySlider_r_RD.setup("Interaction Probability RD", probability_r_RD, 0, 100));
	gui.add(&probabilityRGroup);
	probabilityRGroup.minimize();

	// ProbabilityG Group
	probabilityGGroup.setup("Interaction Probability % Green");
	probabilityGGroup.add(probabilitySlider_a_GR.setup("Interaction Probability GR", probability_a_GR, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GG.setup("Interaction Probability GG", probability_a_GG, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GB.setup("Interaction Probability GB", probability_a_GB, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GW.setup("Interaction Probability GW", probability_a_GW, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GO.setup("Interaction Probability GO", probability_a_GO, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GK.setup("Interaction Probability GK", probability_a_GK, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GC.setup("Interaction Probability GC", probability_a_GC, 0, 100));
	probabilityGGroup.add(probabilitySlider_a_GD.setup("Interaction Probability GD", probability_a_GD, 0, 100));

	probabilityGGroup.add(probabilitySlider_r_GR.setup("Interaction Probability GR", probability_r_GR, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GG.setup("Interaction Probability GG", probability_r_GG, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GB.setup("Interaction Probability GB", probability_r_GB, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GW.setup("Interaction Probability GW", probability_r_GW, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GO.setup("Interaction Probability GO", probability_r_GO, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GK.setup("Interaction Probability GK", probability_r_GK, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GC.setup("Interaction Probability GC", probability_r_GC, 0, 100));
	probabilityGGroup.add(probabilitySlider_r_GD.setup("Interaction Probability GD", probability_r_GD, 0, 100));
	gui.add(&probabilityGGroup);
	probabilityGGroup.minimize();

	// ProbabilityB Group
	probabilityBGroup.setup("Interaction Probability % Blue");
	probabilityBGroup.add(probabilitySlider_a_BR.setup("Interaction Probability BR", probability_a_BR, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BG.setup("Interaction Probability BG", probability_a_BG, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BB.setup("Interaction Probability BB", probability_a_BB, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BW.setup("Interaction Probability BW", probability_a_BW, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BO.setup("Interaction Probability BO", probability_a_BO, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BK.setup("Interaction Probability BK", probability_a_BK, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BC.setup("Interaction Probability BC", probability_a_BC, 0, 100));
	probabilityBGroup.add(probabilitySlider_a_BD.setup("Interaction Probability BD", probability_a_BD, 0, 100));

	probabilityBGroup.add(probabilitySlider_r_BR.setup("Interaction Probability BR", probability_r_BR, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BG.setup("Interaction Probability BG", probability_r_BG, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BB.setup("Interaction Probability BB", probability_r_BB, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BW.setup("Interaction Probability BW", probability_r_BW, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BO.setup("Interaction Probability BO", probability_r_BO, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BK.setup("Interaction Probability BK", probability_r_BK, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BC.setup("Interaction Probability BC", probability_r_BC, 0, 100));
	probabilityBGroup.add(probabilitySlider_r_BD.setup("Interaction Probability BD", probability_r_BD, 0, 100));
	gui.add(&probabilityBGroup);
	probabilityBGroup.minimize();

	// ProbabilityW Group
	probabilityWGroup.setup("Interaction Probability % White");
	probabilityWGroup.add(probabilitySlider_a_WR.setup("Interaction Probability WR", probability_a_WR, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WG.setup("Interaction Probability WG", probability_a_WG, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WB.setup("Interaction Probability WB", probability_a_WB, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WW.setup("Interaction Probability WW", probability_a_WW, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WO.setup("Interaction Probability WO", probability_a_WO, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WK.setup("Interaction Probability WK", probability_a_WK, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WC.setup("Interaction Probability WC", probability_a_WC, 0, 100));
	probabilityWGroup.add(probabilitySlider_a_WD.setup("Interaction Probability WD", probability_a_WD, 0, 100));

	probabilityWGroup.add(probabilitySlider_r_WR.setup("Interaction Probability WR", probability_r_WR, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WG.setup("Interaction Probability WG", probability_r_WG, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WB.setup("Interaction Probability WB", probability_r_WB, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WW.setup("Interaction Probability WW", probability_r_WW, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WO.setup("Interaction Probability WO", probability_r_WO, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WK.setup("Interaction Probability WK", probability_r_WK, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WC.setup("Interaction Probability WC", probability_r_WC, 0, 100));
	probabilityWGroup.add(probabilitySlider_r_WD.setup("Interaction Probability WD", probability_r_WD, 0, 100));
	gui.add(&probabilityWGroup);
	probabilityWGroup.minimize();

	// ProbabilityO Group
	probabilityOGroup.setup("Interaction Probability % Orange");
	probabilityOGroup.add(probabilitySlider_a_OR.setup("Interaction Probability OR", probability_a_OR, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OG.setup("Interaction Probability OG", probability_a_OG, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OB.setup("Interaction Probability OB", probability_a_OB, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OW.setup("Interaction Probability OW", probability_a_OW, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OO.setup("Interaction Probability OO", probability_a_OO, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OK.setup("Interaction Probability OK", probability_a_OK, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OC.setup("Interaction Probability OC", probability_a_OC, 0, 100));
	probabilityOGroup.add(probabilitySlider_a_OD.setup("Interaction Probability OD", probability_a_OD, 0, 100));

	probabilityOGroup.add(probabilitySlider_r_OR.setup("Interaction Probability OR", probability_r_OR, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OG.setup("Interaction Probability OG", probability_r_OG, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OB.setup("Interaction Probability OB", probability_r_OB, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OW.setup("Interaction Probability OW", probability_r_OW, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OO.setup("Interaction Probability OO", probability_r_OO, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OK.setup("Interaction Probability OK", probability_r_OK, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OC.setup("Interaction Probability OC", probability_r_OC, 0, 100));
	probabilityOGroup.add(probabilitySlider_r_OD.setup("Interaction Probability OD", probability_r_OD, 0, 100));
	gui.add(&probabilityOGroup);
	probabilityOGroup.minimize();

	// ProbabilityK Group
	probabilityKGroup.setup("Interaction Probability % Khaki");
	probabilityKGroup.add(probabilitySlider_a_KR.setup("Interaction Probability KR", probability_a_KR, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KG.setup("Interaction Probability KG", probability_a_KG, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KB.setup("Interaction Probability KB", probability_a_KB, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KW.setup("Interaction Probability KW", probability_a_KW, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KO.setup("Interaction Probability KO", probability_a_KO, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KK.setup("Interaction Probability KK", probability_a_KK, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KC.setup("Interaction Probability KC", probability_a_KC, 0, 100));
	probabilityKGroup.add(probabilitySlider_a_KD.setup("Interaction Probability KD", probability_a_KD, 0, 100));

	probabilityKGroup.add(probabilitySlider_r_KR.setup("Interaction Probability KR", probability_r_KR, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KG.setup("Interaction Probability KG", probability_r_KG, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KB.setup("Interaction Probability KB", probability_r_KB, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KW.setup("Interaction Probability KW", probability_r_KW, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KO.setup("Interaction Probability KO", probability_r_KO, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KK.setup("Interaction Probability KK", probability_r_KK, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KC.setup("Interaction Probability KC", probability_r_KC, 0, 100));
	probabilityKGroup.add(probabilitySlider_r_KD.setup("Interaction Probability KD", probability_r_KD, 0, 100));
	gui.add(&probabilityKGroup);
	probabilityKGroup.minimize();

	// ProbabilityC Group
	probabilityCGroup.setup("Interaction Probability % Crimson");
	probabilityCGroup.add(probabilitySlider_a_CR.setup("Interaction Probability CR", probability_a_CR, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CG.setup("Interaction Probability CG", probability_a_CG, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CB.setup("Interaction Probability CB", probability_a_CB, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CW.setup("Interaction Probability CW", probability_a_CW, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CO.setup("Interaction Probability CO", probability_a_CO, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CK.setup("Interaction Probability CK", probability_a_CK, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CC.setup("Interaction Probability CC", probability_a_CC, 0, 100));
	probabilityCGroup.add(probabilitySlider_a_CD.setup("Interaction Probability CD", probability_a_CD, 0, 100));

	probabilityCGroup.add(probabilitySlider_r_CR.setup("Interaction Probability CR", probability_r_CR, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CG.setup("Interaction Probability CG", probability_r_CG, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CB.setup("Interaction Probability CB", probability_r_CB, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CW.setup("Interaction Probability CW", probability_r_CW, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CO.setup("Interaction Probability CO", probability_r_CO, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CK.setup("Interaction Probability CK", probability_r_CK, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CC.setup("Interaction Probability CC", probability_r_CC, 0, 100));
	probabilityCGroup.add(probabilitySlider_r_CD.setup("Interaction Probability CD", probability_r_CD, 0, 100));
	gui.add(&probabilityCGroup);
	probabilityCGroup.minimize();

	// ProbabilityD Group
	probabilityDGroup.setup("Interaction Probability % Dark");
	probabilityDGroup.add(probabilitySlider_a_DR.setup("Interaction Probability DR", probability_a_DR, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DG.setup("Interaction Probability DG", probability_a_DG, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DB.setup("Interaction Probability DB", probability_a_DB, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DW.setup("Interaction Probability DW", probability_a_DW, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DO.setup("Interaction Probability DO", probability_a_DO, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DK.setup("Interaction Probability DK", probability_a_DK, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DC.setup("Interaction Probability DC", probability_a_DC, 0, 100));
	probabilityDGroup.add(probabilitySlider_a_DD.setup("Interaction Probability DD", probability_a_DD, 0, 100));

	probabilityDGroup.add(probabilitySlider_r_DR.setup("Interaction Probability DR", probability_r_DR, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DG.setup("Interaction Probability DG", probability_r_DG, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DB.setup("Interaction Probability DB", probability_r_DB, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DW.setup("Interaction Probability DW", probability_r_DW, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DO.setup("Interaction Probability DO", probability_r_DO, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DK.setup("Interaction Probability DK", probability_r_DK, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DC.setup("Interaction Probability DC", probability_r_DC, 0, 100));
	probabilityDGroup.add(probabilitySlider_r_DD.setup("Interaction Probability DD", probability_r_DD, 0, 100));
	gui.add(&probabilityDGroup);
	probabilityDGroup.minimize();

	// GREEN
	greenGroup.setup("Green");

	greenGroup.add(powerSlider_a_GG.setup("green x green:", ppowerSlider_a_GG, 0, 100));
	greenGroup.add(powerSlider_a_GR.setup("green x red:", ppowerSlider_a_GR, 0, 100));
	greenGroup.add(powerSlider_a_GW.setup("green x white:", ppowerSlider_a_GW, 0, 100));
	greenGroup.add(powerSlider_a_GB.setup("green x blue:", ppowerSlider_a_GB, 0, 100));
	greenGroup.add(powerSlider_a_GO.setup("green x orange:", ppowerSlider_a_GO, 0, 100));
	greenGroup.add(powerSlider_a_GK.setup("green x khaki:", ppowerSlider_a_GK, 0, 100));
	greenGroup.add(powerSlider_a_GC.setup("green x crimson:", ppowerSlider_a_GC, 0, 100));
	greenGroup.add(powerSlider_a_GD.setup("green x dark:", ppowerSlider_a_GD, 0, 100));

	greenGroup.add(vSlider_a_GG.setup("radius g x g:", pvSlider_a_GG, 0, 500));
	greenGroup.add(vSlider_a_GR.setup("radius g x r:", pvSlider_a_GR, 0, 500));
	greenGroup.add(vSlider_a_GW.setup("radius g x w:", pvSlider_a_GW, 0, 500));
	greenGroup.add(vSlider_a_GB.setup("radius g x b:", pvSlider_a_GB, 0, 500));
	greenGroup.add(vSlider_a_GO.setup("radius g x o:", pvSlider_a_GO, 0, 500));
	greenGroup.add(vSlider_a_GK.setup("radius g x k:", pvSlider_a_GK, 0, 500));
	greenGroup.add(vSlider_a_GC.setup("radius g x c:", pvSlider_a_GC, 0, 500));
	greenGroup.add(vSlider_a_GD.setup("radius g x d:", pvSlider_a_GD, 0, 500));

	greenGroup.add(powerSlider_r_GG.setup("green x green:", ppowerSlider_r_GG, -100, 0));
	greenGroup.add(powerSlider_r_GR.setup("green x red:", ppowerSlider_r_GR, -100, 0));
	greenGroup.add(powerSlider_r_GW.setup("green x white:", ppowerSlider_r_GW, -100, 0));
	greenGroup.add(powerSlider_r_GB.setup("green x blue:", ppowerSlider_r_GB, -100, 0));
	greenGroup.add(powerSlider_r_GO.setup("green x orange:", ppowerSlider_r_GO, -100, 0));
	greenGroup.add(powerSlider_r_GK.setup("green x khaki:", ppowerSlider_r_GK, -100, 0));
	greenGroup.add(powerSlider_r_GC.setup("green x crimson:", ppowerSlider_r_GC, -100, 0));
	greenGroup.add(powerSlider_r_GD.setup("green x dark:", ppowerSlider_r_GD, -100, 0));

	greenGroup.add(vSlider_r_GG.setup("radius g x g:", pvSlider_r_GG, 0, 500));
	greenGroup.add(vSlider_r_GR.setup("radius g x r:", pvSlider_r_GR, 0, 500));
	greenGroup.add(vSlider_r_GW.setup("radius g x w:", pvSlider_r_GW, 0, 500));
	greenGroup.add(vSlider_r_GB.setup("radius g x b:", pvSlider_r_GB, 0, 500));
	greenGroup.add(vSlider_r_GO.setup("radius g x o:", pvSlider_r_GO, 0, 500));
	greenGroup.add(vSlider_r_GK.setup("radius g x k:", pvSlider_r_GK, 0, 500));
	greenGroup.add(vSlider_r_GC.setup("radius g x c:", pvSlider_r_GC, 0, 500));
	greenGroup.add(vSlider_r_GD.setup("radius g x d:", pvSlider_r_GD, 0, 500));

	greenGroup.minimize();
	gui.add(&greenGroup);

	// RED
	redGroup.setup("Red");

	redGroup.add(powerSlider_a_RR.setup("red x red:", ppowerSlider_a_RR, 0, 100));
	redGroup.add(powerSlider_a_RG.setup("red x green:", ppowerSlider_a_RG, 0, 100));
	redGroup.add(powerSlider_a_RW.setup("red x white:", ppowerSlider_a_RW, 0, 100));
	redGroup.add(powerSlider_a_RB.setup("red x blue:", ppowerSlider_a_RB, 0, 100));
	redGroup.add(powerSlider_a_RO.setup("red x orange:", ppowerSlider_a_RO, 0, 100));
	redGroup.add(powerSlider_a_RK.setup("red x khaki:", ppowerSlider_a_RK, 0, 100));
	redGroup.add(powerSlider_a_RC.setup("red x crimson:", ppowerSlider_a_RC,0, 100));
	redGroup.add(powerSlider_a_RD.setup("red x dark:", ppowerSlider_a_RD, 0, 100));

	redGroup.add(vSlider_a_RG.setup("radius r x g:", pvSlider_a_RG, 0, 500));
	redGroup.add(vSlider_a_RR.setup("radius r x r:", pvSlider_a_RR, 0, 500));
	redGroup.add(vSlider_a_RW.setup("radius r x w:", pvSlider_a_RW, 0, 500));
	redGroup.add(vSlider_a_RB.setup("radius r x b:", pvSlider_a_RB, 0, 500));
	redGroup.add(vSlider_a_RO.setup("radius r x o:", pvSlider_a_RO, 0, 500));
	redGroup.add(vSlider_a_RK.setup("radius r x k:", pvSlider_a_RK, 0, 500));
	redGroup.add(vSlider_a_RC.setup("radius r x c:", pvSlider_a_RC, 0, 500));
	redGroup.add(vSlider_a_RD.setup("radius r x d:", pvSlider_a_RD, 0, 500));

	redGroup.add(powerSlider_r_RR.setup("red x red:", ppowerSlider_r_RR, -100, 0));
	redGroup.add(powerSlider_r_RG.setup("red x green:", ppowerSlider_r_RG, -100, 0));
	redGroup.add(powerSlider_r_RW.setup("red x white:", ppowerSlider_r_RW, -100, 0));
	redGroup.add(powerSlider_r_RB.setup("red x blue:", ppowerSlider_r_RB, -100, 0));
	redGroup.add(powerSlider_r_RO.setup("red x orange:", ppowerSlider_r_RO, -100, 0));
	redGroup.add(powerSlider_r_RK.setup("red x khaki:", ppowerSlider_r_RK, -100, 0));
	redGroup.add(powerSlider_r_RC.setup("red x crimson:", ppowerSlider_r_RC, -100, 0));
	redGroup.add(powerSlider_r_RD.setup("red x dark:", ppowerSlider_r_RD, -100, 0));

	redGroup.add(vSlider_r_RG.setup("radius r x g:", pvSlider_r_RG, 0, 500));
	redGroup.add(vSlider_r_RR.setup("radius r x r:", pvSlider_r_RR, 0, 500));
	redGroup.add(vSlider_r_RW.setup("radius r x w:", pvSlider_r_RW, 0, 500));
	redGroup.add(vSlider_r_RB.setup("radius r x b:", pvSlider_r_RB, 0, 500));
	redGroup.add(vSlider_r_RO.setup("radius r x o:", pvSlider_r_RO, 0, 500));
	redGroup.add(vSlider_r_RK.setup("radius r x k:", pvSlider_r_RK, 0, 500));
	redGroup.add(vSlider_r_RC.setup("radius r x c:", pvSlider_r_RC, 0, 500));
	redGroup.add(vSlider_r_RD.setup("radius r x d:", pvSlider_r_RD, 0, 500));

	redGroup.minimize();
	gui.add(&redGroup);

	// WHITE
	whiteGroup.setup("White");

	whiteGroup.add(powerSlider_a_WW.setup("white x white:", ppowerSlider_a_WW, 0, 100));
	whiteGroup.add(powerSlider_a_WR.setup("white x red:", ppowerSlider_a_WR, 0, 100));
	whiteGroup.add(powerSlider_a_WG.setup("white x green:", ppowerSlider_a_WG, 0, 100));
	whiteGroup.add(powerSlider_a_WB.setup("white x blue:", ppowerSlider_a_WB, 0, 100));
	whiteGroup.add(powerSlider_a_WO.setup("white x orange:", ppowerSlider_a_WO, 0, 100));
	whiteGroup.add(powerSlider_a_WK.setup("white x khaki:", ppowerSlider_a_WK, 0, 100));
	whiteGroup.add(powerSlider_a_WC.setup("white x crimson:", ppowerSlider_a_WC, 0, 100));
	whiteGroup.add(powerSlider_a_WD.setup("white x dark:", ppowerSlider_a_WD, 0, 100));

	whiteGroup.add(vSlider_a_WG.setup("radius w x g:", pvSlider_a_WG, 0, 500));
	whiteGroup.add(vSlider_a_WR.setup("radius w x r:", pvSlider_a_WR, 0, 500));
	whiteGroup.add(vSlider_a_WW.setup("radius w x w:", pvSlider_a_WW, 0, 500));
	whiteGroup.add(vSlider_a_WB.setup("radius w x b:", pvSlider_a_WB, 0, 500));
	whiteGroup.add(vSlider_a_WO.setup("radius w x o:", pvSlider_a_WO, 0, 500));
	whiteGroup.add(vSlider_a_WK.setup("radius w x k:", pvSlider_a_WK, 0, 500));
	whiteGroup.add(vSlider_a_WC.setup("radius w x c:", pvSlider_a_WC, 0, 500));
	whiteGroup.add(vSlider_a_WD.setup("radius w x d:", pvSlider_a_WD, 0, 500));

	whiteGroup.add(powerSlider_r_WW.setup("white x white:", ppowerSlider_r_WW, -100, 0));
	whiteGroup.add(powerSlider_r_WR.setup("white x red:", ppowerSlider_r_WR, -100, 0));
	whiteGroup.add(powerSlider_r_WG.setup("white x green:", ppowerSlider_r_WG, -100, 0));
	whiteGroup.add(powerSlider_r_WB.setup("white x blue:", ppowerSlider_r_WB, -100, 0));
	whiteGroup.add(powerSlider_r_WO.setup("white x orange:", ppowerSlider_r_WO, -100, 0));
	whiteGroup.add(powerSlider_r_WK.setup("white x khaki:", ppowerSlider_r_WK, -100, 0));
	whiteGroup.add(powerSlider_r_WC.setup("white x crimson:", ppowerSlider_r_WC, -100, 0));
	whiteGroup.add(powerSlider_r_WD.setup("white x dark:", ppowerSlider_r_WD, -100, 0));

	whiteGroup.add(vSlider_r_WG.setup("radius w x g:", pvSlider_r_WG, 0, 500));
	whiteGroup.add(vSlider_r_WR.setup("radius w x r:", pvSlider_r_WR, 0, 500));
	whiteGroup.add(vSlider_r_WW.setup("radius w x w:", pvSlider_r_WW, 0, 500));
	whiteGroup.add(vSlider_r_WB.setup("radius w x b:", pvSlider_r_WB, 0, 500));
	whiteGroup.add(vSlider_r_WO.setup("radius w x o:", pvSlider_r_WO, 0, 500));
	whiteGroup.add(vSlider_r_WK.setup("radius w x k:", pvSlider_r_WK, 0, 500));
	whiteGroup.add(vSlider_r_WC.setup("radius w x c:", pvSlider_r_WC, 0, 500));
	whiteGroup.add(vSlider_r_WD.setup("radius w x d:", pvSlider_r_WD, 0, 500));

	whiteGroup.minimize();
	gui.add(&whiteGroup);

	// BLUE
	blueGroup.setup("Blue");

	blueGroup.add(powerSlider_a_BB.setup("blue x blue:", ppowerSlider_a_BB, 0, 100));
	blueGroup.add(powerSlider_a_BW.setup("blue x white:", ppowerSlider_a_BW, 0, 100));
	blueGroup.add(powerSlider_a_BR.setup("blue x red:", ppowerSlider_a_BR, 0, 100));
	blueGroup.add(powerSlider_a_BG.setup("blue x green:", ppowerSlider_a_BG, 0, 100));
	blueGroup.add(powerSlider_a_BO.setup("blue x orange:", ppowerSlider_a_BO, 0, 100));
	blueGroup.add(powerSlider_a_BK.setup("blue x khaki:", ppowerSlider_a_BK, 0, 100));
	blueGroup.add(powerSlider_a_BC.setup("blue x crimson:", ppowerSlider_a_BC, 0, 100));
	blueGroup.add(powerSlider_a_BD.setup("blue x dark:", ppowerSlider_a_BD, 0, 100));

	blueGroup.add(vSlider_a_BG.setup("radius b x g:", pvSlider_a_BG, 0, 500));
	blueGroup.add(vSlider_a_BR.setup("radius b x r:", pvSlider_a_BR, 0, 500));
	blueGroup.add(vSlider_a_BW.setup("radius b x w:", pvSlider_a_BW, 0, 500));
	blueGroup.add(vSlider_a_BB.setup("radius b x b:", pvSlider_a_BB, 0, 500));
	blueGroup.add(vSlider_a_BO.setup("radius b x o:", pvSlider_a_BO, 0, 500));
	blueGroup.add(vSlider_a_BK.setup("radius b x k:", pvSlider_a_BK, 0, 500));
	blueGroup.add(vSlider_a_BC.setup("radius b x c:", pvSlider_a_BC, 0, 500));
	blueGroup.add(vSlider_a_BD.setup("radius b x d:", pvSlider_a_BD, 0, 500));

	blueGroup.add(powerSlider_r_BB.setup("blue x blue:", ppowerSlider_r_BB, -100, 0));
	blueGroup.add(powerSlider_r_BW.setup("blue x white:", ppowerSlider_r_BW, -100, 0));
	blueGroup.add(powerSlider_r_BR.setup("blue x red:", ppowerSlider_r_BR, -100, 0));
	blueGroup.add(powerSlider_r_BG.setup("blue x green:", ppowerSlider_r_BG, -100, 0));
	blueGroup.add(powerSlider_r_BO.setup("blue x orange:", ppowerSlider_r_BO, -100, 0));
	blueGroup.add(powerSlider_r_BK.setup("blue x khaki:", ppowerSlider_r_BK, -100, 0));
	blueGroup.add(powerSlider_r_BC.setup("blue x crimson:", ppowerSlider_r_BC, -100, 0));
	blueGroup.add(powerSlider_r_BD.setup("blue x dark:", ppowerSlider_r_BD, -100, 0));

	blueGroup.add(vSlider_r_BG.setup("radius b x g:", pvSlider_r_BG, 0, 500));
	blueGroup.add(vSlider_r_BR.setup("radius b x r:", pvSlider_r_BR, 0, 500));
	blueGroup.add(vSlider_r_BW.setup("radius b x w:", pvSlider_r_BW, 0, 500));
	blueGroup.add(vSlider_r_BB.setup("radius b x b:", pvSlider_r_BB, 0, 500));
	blueGroup.add(vSlider_r_BO.setup("radius b x o:", pvSlider_r_BO, 0, 500));
	blueGroup.add(vSlider_r_BK.setup("radius b x k:", pvSlider_r_BK, 0, 500));
	blueGroup.add(vSlider_r_BC.setup("radius b x c:", pvSlider_r_BC, 0, 500));
	blueGroup.add(vSlider_r_BD.setup("radius b x d:", pvSlider_r_BD, 0, 500));

	blueGroup.minimize();
	gui.add(&blueGroup);

	// ORANGE
	orangeGroup.setup("Orange");

	orangeGroup.add(powerSlider_a_OB.setup("orange x blue:", ppowerSlider_a_OB, 0, 100));
	orangeGroup.add(powerSlider_a_OW.setup("orange x white:", ppowerSlider_a_OW, 0, 100));
	orangeGroup.add(powerSlider_a_OR.setup("orange x red:", ppowerSlider_a_OR, 0, 100));
	orangeGroup.add(powerSlider_a_OG.setup("orange x green:", ppowerSlider_a_OG, 0, 100));
	orangeGroup.add(powerSlider_a_OO.setup("orange x orange:", ppowerSlider_a_OO, 0, 100));
	orangeGroup.add(powerSlider_a_OK.setup("orange x khaki:", ppowerSlider_a_OK, 0, 100));
	orangeGroup.add(powerSlider_a_OC.setup("orange x crimson:", ppowerSlider_a_OC, 0, 100));
	orangeGroup.add(powerSlider_a_OD.setup("orange x dark:", ppowerSlider_a_OD, 0, 100));

	orangeGroup.add(vSlider_a_OG.setup("radius o x g:", pvSlider_a_OG, 0, 500));
	orangeGroup.add(vSlider_a_OR.setup("radius o x r:", pvSlider_a_OR, 0, 500));
	orangeGroup.add(vSlider_a_OW.setup("radius o x w:", pvSlider_a_OW, 0, 500));
	orangeGroup.add(vSlider_a_OB.setup("radius o x b:", pvSlider_a_OB, 0, 500));
	orangeGroup.add(vSlider_a_OO.setup("radius o x o:", pvSlider_a_OO, 0, 500));
	orangeGroup.add(vSlider_a_OK.setup("radius o x k:", pvSlider_a_OK, 0, 500));
	orangeGroup.add(vSlider_a_OC.setup("radius o x c:", pvSlider_a_OC, 0, 500));
	orangeGroup.add(vSlider_a_OD.setup("radius o x d:", pvSlider_a_OD, 0, 500));

	orangeGroup.add(powerSlider_r_OB.setup("orange x blue:", ppowerSlider_r_OB, -100, 0));
	orangeGroup.add(powerSlider_r_OW.setup("orange x white:", ppowerSlider_r_OW, -100, 0));
	orangeGroup.add(powerSlider_r_OR.setup("orange x red:", ppowerSlider_r_OR, -100, 0));
	orangeGroup.add(powerSlider_r_OG.setup("orange x green:", ppowerSlider_r_OG, -100, 0));
	orangeGroup.add(powerSlider_r_OO.setup("orange x orange:", ppowerSlider_r_OO, -100, 0));
	orangeGroup.add(powerSlider_r_OK.setup("orange x khaki:", ppowerSlider_r_OK, -100, 0));
	orangeGroup.add(powerSlider_r_OC.setup("orange x crimson:", ppowerSlider_r_OC, -100, 0));
	orangeGroup.add(powerSlider_r_OD.setup("orange x dark:", ppowerSlider_r_OD, -100, 0));

	orangeGroup.add(vSlider_r_OG.setup("radius o x g:", pvSlider_r_OG, 0, 500));
	orangeGroup.add(vSlider_r_OR.setup("radius o x r:", pvSlider_r_OR, 0, 500));
	orangeGroup.add(vSlider_r_OW.setup("radius o x w:", pvSlider_r_OW, 0, 500));
	orangeGroup.add(vSlider_r_OB.setup("radius o x b:", pvSlider_r_OB, 0, 500));
	orangeGroup.add(vSlider_r_OO.setup("radius o x o:", pvSlider_r_OO, 0, 500));
	orangeGroup.add(vSlider_r_OK.setup("radius o x k:", pvSlider_r_OK, 0, 500));
	orangeGroup.add(vSlider_r_OC.setup("radius o x c:", pvSlider_r_OC, 0, 500));
	orangeGroup.add(vSlider_r_OD.setup("radius o x d:", pvSlider_r_OD, 0, 500));

	orangeGroup.minimize();
	gui.add(&orangeGroup);

	// KHAKI
	khakiGroup.setup("Khaki");

	khakiGroup.add(powerSlider_a_KB.setup("khaki x blue:", ppowerSlider_a_KB, 0, 100));
	khakiGroup.add(powerSlider_a_KW.setup("khaki x white:", ppowerSlider_a_KW, 0, 100));
	khakiGroup.add(powerSlider_a_KR.setup("khaki x red:", ppowerSlider_a_KR, 0, 100));
	khakiGroup.add(powerSlider_a_KG.setup("khaki x green:", ppowerSlider_a_KG, 0, 100));
	khakiGroup.add(powerSlider_a_KO.setup("khaki x orange:", ppowerSlider_a_KO, 0, 100));
	khakiGroup.add(powerSlider_a_KK.setup("khaki x khaki:", ppowerSlider_a_KK, 0, 100));
	khakiGroup.add(powerSlider_a_KC.setup("khaki x crimson:", ppowerSlider_a_KC, 0, 100));
	khakiGroup.add(powerSlider_a_KD.setup("khaki x dark:", ppowerSlider_a_KD, 0, 100));

	khakiGroup.add(vSlider_a_KG.setup("radius k x g:", pvSlider_a_KG, 0, 500));
	khakiGroup.add(vSlider_a_KR.setup("radius k x r:", pvSlider_a_KR, 0, 500));
	khakiGroup.add(vSlider_a_KW.setup("radius k x w:", pvSlider_a_KW, 0, 500));
	khakiGroup.add(vSlider_a_KB.setup("radius k x b:", pvSlider_a_KB, 0, 500));
	khakiGroup.add(vSlider_a_KO.setup("radius k x o:", pvSlider_a_KO, 0, 500));
	khakiGroup.add(vSlider_a_KK.setup("radius k x k:", pvSlider_a_KK, 0, 500));
	khakiGroup.add(vSlider_a_KC.setup("radius k x c:", pvSlider_a_KC, 0, 500));
	khakiGroup.add(vSlider_a_KD.setup("radius k x d:", pvSlider_a_KD, 0, 500));

	khakiGroup.add(powerSlider_r_KB.setup("khaki x blue:", ppowerSlider_r_KB, -100, 0));
	khakiGroup.add(powerSlider_r_KW.setup("khaki x white:", ppowerSlider_r_KW, -100, 0));
	khakiGroup.add(powerSlider_r_KR.setup("khaki x red:", ppowerSlider_r_KR, -100, 0));
	khakiGroup.add(powerSlider_r_KG.setup("khaki x green:", ppowerSlider_r_KG, -100, 0));
	khakiGroup.add(powerSlider_r_KO.setup("khaki x orange:", ppowerSlider_r_KO, -100, 0));
	khakiGroup.add(powerSlider_r_KK.setup("khaki x khaki:", ppowerSlider_r_KK, -100, 0));
	khakiGroup.add(powerSlider_r_KC.setup("khaki x crimson:", ppowerSlider_r_KC, -100, 0));
	khakiGroup.add(powerSlider_r_KD.setup("khaki x dark:", ppowerSlider_r_KD, -100, 0));

	khakiGroup.add(vSlider_r_KG.setup("radius k x g:", pvSlider_r_KG, 0, 500));
	khakiGroup.add(vSlider_r_KR.setup("radius k x r:", pvSlider_r_KR, 0, 500));
	khakiGroup.add(vSlider_r_KW.setup("radius k x w:", pvSlider_r_KW, 0, 500));
	khakiGroup.add(vSlider_r_KB.setup("radius k x b:", pvSlider_r_KB, 0, 500));
	khakiGroup.add(vSlider_r_KO.setup("radius k x o:", pvSlider_r_KO, 0, 500));
	khakiGroup.add(vSlider_r_KK.setup("radius k x k:", pvSlider_r_KK, 0, 500));
	khakiGroup.add(vSlider_r_KC.setup("radius k x c:", pvSlider_r_KC, 0, 500));
	khakiGroup.add(vSlider_r_KD.setup("radius k x d:", pvSlider_r_KD, 0, 500));

	khakiGroup.minimize();
	gui.add(&khakiGroup);

	// CRIMSON
	crimsonGroup.setup("Crimson");

	crimsonGroup.add(powerSlider_a_CB.setup("crimson x blue:", ppowerSlider_a_CB, 0, 100));
	crimsonGroup.add(powerSlider_a_CW.setup("crimson x white:", ppowerSlider_a_CW, 0, 100));
	crimsonGroup.add(powerSlider_a_CR.setup("crimson x red:", ppowerSlider_a_CR, 0, 100));
	crimsonGroup.add(powerSlider_a_CG.setup("crimson x green:", ppowerSlider_a_CG, 0, 100));
	crimsonGroup.add(powerSlider_a_CO.setup("crimson x orange:", ppowerSlider_a_CO, 0, 100));
	crimsonGroup.add(powerSlider_a_CK.setup("crimson x khaki:", ppowerSlider_a_CK, 0, 100));
	crimsonGroup.add(powerSlider_a_CC.setup("crimson x crimson:", ppowerSlider_a_CC, 0, 100));
	crimsonGroup.add(powerSlider_a_CD.setup("crimson x dark:", ppowerSlider_a_CD, 0, 100));

	crimsonGroup.add(vSlider_a_CG.setup("radius c x g:", pvSlider_a_CG, 0, 500));
	crimsonGroup.add(vSlider_a_CR.setup("radius c x r:", pvSlider_a_CR, 0, 500));
	crimsonGroup.add(vSlider_a_CW.setup("radius c x w:", pvSlider_a_CW, 0, 500));
	crimsonGroup.add(vSlider_a_CB.setup("radius c x b:", pvSlider_a_CB, 0, 500));
	crimsonGroup.add(vSlider_a_CO.setup("radius c x o:", pvSlider_a_CO, 0, 500));
	crimsonGroup.add(vSlider_a_CK.setup("radius c x k:", pvSlider_a_CK, 0, 500));
	crimsonGroup.add(vSlider_a_CC.setup("radius c x c:", pvSlider_a_CC, 0, 500));
	crimsonGroup.add(vSlider_a_CD.setup("radius c x d:", pvSlider_a_CD, 0, 500));

	crimsonGroup.add(powerSlider_r_CB.setup("crimson x blue:", ppowerSlider_r_CB, -100, 0));
	crimsonGroup.add(powerSlider_r_CW.setup("crimson x white:", ppowerSlider_r_CW, -100, 0));
	crimsonGroup.add(powerSlider_r_CR.setup("crimson x red:", ppowerSlider_r_CR, -100, 0));
	crimsonGroup.add(powerSlider_r_CG.setup("crimson x green:", ppowerSlider_r_CG, -100, 0));
	crimsonGroup.add(powerSlider_r_CO.setup("crimson x orange:", ppowerSlider_r_CO, -100, 0));
	crimsonGroup.add(powerSlider_r_CK.setup("crimson x khaki:", ppowerSlider_r_CK, -100, 0));
	crimsonGroup.add(powerSlider_r_CC.setup("crimson x crimson:", ppowerSlider_r_CC, -100, 0));
	crimsonGroup.add(powerSlider_r_CD.setup("crimson x dark:", ppowerSlider_r_CD, -100, 0));

	crimsonGroup.add(vSlider_r_CG.setup("radius c x g:", pvSlider_r_CG, 0, 500));
	crimsonGroup.add(vSlider_r_CR.setup("radius c x r:", pvSlider_r_CR, 0, 500));
	crimsonGroup.add(vSlider_r_CW.setup("radius c x w:", pvSlider_r_CW, 0, 500));
	crimsonGroup.add(vSlider_r_CB.setup("radius c x b:", pvSlider_r_CB, 0, 500));
	crimsonGroup.add(vSlider_r_CO.setup("radius c x o:", pvSlider_r_CO, 0, 500));
	crimsonGroup.add(vSlider_r_CK.setup("radius c x k:", pvSlider_r_CK, 0, 500));
	crimsonGroup.add(vSlider_r_CC.setup("radius c x c:", pvSlider_r_CC, 0, 500));
	crimsonGroup.add(vSlider_r_CD.setup("radius c x d:", pvSlider_r_CD, 0, 500));

	crimsonGroup.minimize();
	gui.add(&crimsonGroup);

	// DARK
	darkGroup.setup("Dark");

	darkGroup.add(powerSlider_a_DB.setup("dark x blue:", ppowerSlider_a_DB, 0, 100));
	darkGroup.add(powerSlider_a_DW.setup("dark x white:", ppowerSlider_a_DW, 0, 100));
	darkGroup.add(powerSlider_a_DR.setup("dark x red:", ppowerSlider_a_DR, 0, 100));
	darkGroup.add(powerSlider_a_DG.setup("dark x green:", ppowerSlider_a_DG, 0, 100));
	darkGroup.add(powerSlider_a_DO.setup("dark x orange:", ppowerSlider_a_DO, 0, 100));
	darkGroup.add(powerSlider_a_DK.setup("dark x khaki:", ppowerSlider_a_DK, 0, 100));
	darkGroup.add(powerSlider_a_DC.setup("dark x crimson:", ppowerSlider_a_DC, 0, 100));
	darkGroup.add(powerSlider_a_DD.setup("dark x dark:", ppowerSlider_a_DD, 0, 100));

	darkGroup.add(vSlider_a_DG.setup("radius d x g:", pvSlider_a_DG, 0, 500));
	darkGroup.add(vSlider_a_DR.setup("radius d x r:", pvSlider_a_DR, 0, 500));
	darkGroup.add(vSlider_a_DW.setup("radius d x w:", pvSlider_a_DW, 0, 500));
	darkGroup.add(vSlider_a_DB.setup("radius d x b:", pvSlider_a_DB, 0, 500));
	darkGroup.add(vSlider_a_DO.setup("radius d x o:", pvSlider_a_DO, 0, 500));
	darkGroup.add(vSlider_a_DK.setup("radius d x k:", pvSlider_a_DK, 0, 500));
	darkGroup.add(vSlider_a_DC.setup("radius d x c:", pvSlider_a_DC, 0, 500));
	darkGroup.add(vSlider_a_DD.setup("radius d x d:", pvSlider_a_DD, 0, 500));

	darkGroup.add(powerSlider_r_DB.setup("dark x blue:", ppowerSlider_r_DB, -100, 0));
	darkGroup.add(powerSlider_r_DW.setup("dark x white:", ppowerSlider_r_DW, -100, 0));
	darkGroup.add(powerSlider_r_DR.setup("dark x red:", ppowerSlider_r_DR, -100, 0));
	darkGroup.add(powerSlider_r_DG.setup("dark x green:", ppowerSlider_r_DG, -100, 0));
	darkGroup.add(powerSlider_r_DO.setup("dark x orange:", ppowerSlider_r_DO, -100, 0));
	darkGroup.add(powerSlider_r_DK.setup("dark x khaki:", ppowerSlider_r_DK, -100, 0));
	darkGroup.add(powerSlider_r_DC.setup("dark x crimson:", ppowerSlider_r_DC, -100, 0));
	darkGroup.add(powerSlider_r_DD.setup("dark x dark:", ppowerSlider_r_DD, -100, 0));

	darkGroup.add(vSlider_r_DG.setup("radius d x g:", pvSlider_r_DG, 0, 500));
	darkGroup.add(vSlider_r_DR.setup("radius d x r:", pvSlider_r_DR, 0, 500));
	darkGroup.add(vSlider_r_DW.setup("radius d x w:", pvSlider_r_DW, 0, 500));
	darkGroup.add(vSlider_r_DB.setup("radius d x b:", pvSlider_r_DB, 0, 500));
	darkGroup.add(vSlider_r_DO.setup("radius d x o:", pvSlider_r_DO, 0, 500));
	darkGroup.add(vSlider_r_DK.setup("radius d x k:", pvSlider_r_DK, 0, 500));
	darkGroup.add(vSlider_r_DC.setup("radius d x c:", pvSlider_r_DC, 0, 500));
	darkGroup.add(vSlider_r_DD.setup("radius d x d:", pvSlider_r_DD, 0, 500));

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
		powerSlider_a_RR = powerSlider_a_RR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RR.getMax() - powerSlider_a_RR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RG = powerSlider_a_RG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RG.getMax() - powerSlider_a_RG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RB = powerSlider_a_RB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RB.getMax() - powerSlider_a_RB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RW = powerSlider_a_RW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RW.getMax() - powerSlider_a_RW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RO = powerSlider_a_RO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RO.getMax() - powerSlider_a_RO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RK = powerSlider_a_RK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RK.getMax() - powerSlider_a_RK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RC = powerSlider_a_RC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RC.getMax() - powerSlider_a_RC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_RD = powerSlider_a_RD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_RD.getMax() - powerSlider_a_RD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_RR = powerSlider_r_RR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RR.getMax() - powerSlider_r_RR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RG = powerSlider_r_RG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RG.getMax() - powerSlider_r_RG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RB = powerSlider_r_RB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RB.getMax() - powerSlider_r_RB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RW = powerSlider_r_RW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RW.getMax() - powerSlider_r_RW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RO = powerSlider_r_RO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RO.getMax() - powerSlider_r_RO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RK = powerSlider_r_RK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RK.getMax() - powerSlider_r_RK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RC = powerSlider_r_RC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RC.getMax() - powerSlider_r_RC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_RD = powerSlider_r_RD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_RD.getMax() - powerSlider_r_RD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_GR = powerSlider_a_GR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GR.getMax() - powerSlider_a_GR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GG = powerSlider_a_GG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GG.getMax() - powerSlider_a_GG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GB = powerSlider_a_GB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GB.getMax() - powerSlider_a_GB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GW = powerSlider_a_GW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GW.getMax() - powerSlider_a_GW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GO = powerSlider_a_GO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GO.getMax() - powerSlider_a_GO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GK = powerSlider_a_GK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GK.getMax() - powerSlider_a_GK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GC = powerSlider_a_GC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GC.getMax() - powerSlider_a_GC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_GD = powerSlider_a_GD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_GD.getMax() - powerSlider_a_GD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_GR = powerSlider_r_GR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GR.getMax() - powerSlider_r_GR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GG = powerSlider_r_GG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GG.getMax() - powerSlider_r_GG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GB = powerSlider_r_GB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GB.getMax() - powerSlider_r_GB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GW = powerSlider_r_GW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GW.getMax() - powerSlider_r_GW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GO = powerSlider_r_GO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GO.getMax() - powerSlider_r_GO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GK = powerSlider_r_GK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GK.getMax() - powerSlider_r_GK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GC = powerSlider_r_GC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GC.getMax() - powerSlider_r_GC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_GD = powerSlider_r_GD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_GD.getMax() - powerSlider_r_GD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_BR = powerSlider_a_BR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BR.getMax() - powerSlider_a_BR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BG = powerSlider_a_BG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BG.getMax() - powerSlider_a_BG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BB = powerSlider_a_BB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BB.getMax() - powerSlider_a_BB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BW = powerSlider_a_BW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BW.getMax() - powerSlider_a_BW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BO = powerSlider_a_BO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BO.getMax() - powerSlider_a_BO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BK = powerSlider_a_BK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BK.getMax() - powerSlider_a_BK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BC = powerSlider_a_BC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BC.getMax() - powerSlider_a_BC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_BD = powerSlider_a_BD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_BD.getMax() - powerSlider_a_BD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_BR = powerSlider_r_BR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BR.getMax() - powerSlider_r_BR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BG = powerSlider_r_BG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BG.getMax() - powerSlider_r_BG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BB = powerSlider_r_BB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BB.getMax() - powerSlider_r_BB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BW = powerSlider_r_BW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BW.getMax() - powerSlider_r_BW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BO = powerSlider_r_BO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BO.getMax() - powerSlider_r_BO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BK = powerSlider_r_BK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BK.getMax() - powerSlider_r_BK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BC = powerSlider_r_BC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BC.getMax() - powerSlider_r_BC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_BD = powerSlider_r_BD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_BD.getMax() - powerSlider_r_BD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_WR = powerSlider_a_WR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WR.getMax() - powerSlider_a_WR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WG = powerSlider_a_WG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WG.getMax() - powerSlider_a_WG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WB = powerSlider_a_WB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WB.getMax() - powerSlider_a_WB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WW = powerSlider_a_WW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WW.getMax() - powerSlider_a_WW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WO = powerSlider_a_WO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WO.getMax() - powerSlider_a_WO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WK = powerSlider_a_WK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WK.getMax() - powerSlider_a_WK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WC = powerSlider_a_WC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WC.getMax() - powerSlider_a_WC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_WD = powerSlider_a_WD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_WD.getMax() - powerSlider_a_WD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_WR = powerSlider_r_WR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WR.getMax() - powerSlider_r_WR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WG = powerSlider_r_WG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WG.getMax() - powerSlider_r_WG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WB = powerSlider_r_WB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WB.getMax() - powerSlider_r_WB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WW = powerSlider_r_WW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WW.getMax() - powerSlider_r_WW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WO = powerSlider_r_WO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WO.getMax() - powerSlider_r_WO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WK = powerSlider_r_WK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WK.getMax() - powerSlider_r_WK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WC = powerSlider_r_WC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WC.getMax() - powerSlider_r_WC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_WD = powerSlider_r_WD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_WD.getMax() - powerSlider_r_WD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_OR = powerSlider_a_OR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OR.getMax() - powerSlider_a_OR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OG = powerSlider_a_OG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OG.getMax() - powerSlider_a_OG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OB = powerSlider_a_OB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OB.getMax() - powerSlider_a_OB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OW = powerSlider_a_OW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OW.getMax() - powerSlider_a_OW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OO = powerSlider_a_OO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OO.getMax() - powerSlider_a_OO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OK = powerSlider_a_OK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OK.getMax() - powerSlider_a_OK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OC = powerSlider_a_OC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OC.getMax() - powerSlider_a_OC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_OD = powerSlider_a_OD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_OD.getMax() - powerSlider_a_OD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_OR = powerSlider_r_OR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OR.getMax() - powerSlider_r_OR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OG = powerSlider_r_OG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OG.getMax() - powerSlider_r_OG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OB = powerSlider_r_OB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OB.getMax() - powerSlider_r_OB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OW = powerSlider_r_OW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OW.getMax() - powerSlider_r_OW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OO = powerSlider_r_OO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OO.getMax() - powerSlider_r_OO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OK = powerSlider_r_OK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OK.getMax() - powerSlider_r_OK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OC = powerSlider_r_OC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OC.getMax() - powerSlider_r_OC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_OD = powerSlider_r_OD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_OD.getMax() - powerSlider_r_OD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_KR = powerSlider_a_KR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KR.getMax() - powerSlider_a_KR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KG = powerSlider_a_KG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KG.getMax() - powerSlider_a_KG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KB = powerSlider_a_KB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KB.getMax() - powerSlider_a_KB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KW = powerSlider_a_KW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KW.getMax() - powerSlider_a_KW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KO = powerSlider_a_KO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KO.getMax() - powerSlider_a_KO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KK = powerSlider_a_KK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KK.getMax() - powerSlider_a_KK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KC = powerSlider_a_KC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KC.getMax() - powerSlider_a_KC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_KD = powerSlider_a_KD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_KD.getMax() - powerSlider_a_KD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_KR = powerSlider_r_KR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KR.getMax() - powerSlider_r_KR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KG = powerSlider_r_KG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KG.getMax() - powerSlider_r_KG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KB = powerSlider_r_KB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KB.getMax() - powerSlider_r_KB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KW = powerSlider_r_KW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KW.getMax() - powerSlider_r_KW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KO = powerSlider_r_KO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KO.getMax() - powerSlider_r_KO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KK = powerSlider_r_KK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KK.getMax() - powerSlider_r_KK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KC = powerSlider_r_KC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KC.getMax() - powerSlider_r_KC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_KD = powerSlider_r_KD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_KD.getMax() - powerSlider_r_KD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_CR = powerSlider_a_CR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CR.getMax() - powerSlider_a_CR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CG = powerSlider_a_CG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CG.getMax() - powerSlider_a_CG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CB = powerSlider_a_CB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CB.getMax() - powerSlider_a_CB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CW = powerSlider_a_CW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CW.getMax() - powerSlider_a_CW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CO = powerSlider_a_CO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CO.getMax() - powerSlider_a_CO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CK = powerSlider_a_CK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CK.getMax() - powerSlider_a_CK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CC = powerSlider_a_CC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CC.getMax() - powerSlider_a_CC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_CD = powerSlider_a_CD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_CD.getMax() - powerSlider_a_CD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_CR = powerSlider_r_CR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CR.getMax() - powerSlider_r_CR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CG = powerSlider_r_CG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CG.getMax() - powerSlider_r_CG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CB = powerSlider_r_CB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CB.getMax() - powerSlider_r_CB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CW = powerSlider_r_CW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CW.getMax() - powerSlider_r_CW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CO = powerSlider_r_CO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CO.getMax() - powerSlider_r_CO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CK = powerSlider_r_CK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CK.getMax() - powerSlider_r_CK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CC = powerSlider_r_CC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CC.getMax() - powerSlider_r_CC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_CD = powerSlider_r_CD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_CD.getMax() - powerSlider_r_CD.getMin()) * (evoAmount / 100.0F));

		powerSlider_a_DR = powerSlider_a_DR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DR.getMax() - powerSlider_a_DR.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DG = powerSlider_a_DG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DG.getMax() - powerSlider_a_DG.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DB = powerSlider_a_DB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DB.getMax() - powerSlider_a_DB.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DW = powerSlider_a_DW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DW.getMax() - powerSlider_a_DW.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DO = powerSlider_a_DO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DO.getMax() - powerSlider_a_DO.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DK = powerSlider_a_DK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DK.getMax() - powerSlider_a_DK.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DC = powerSlider_a_DC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DC.getMax() - powerSlider_a_DC.getMin()) * (evoAmount / 100.0F));
		powerSlider_a_DD = powerSlider_a_DD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_a_DD.getMax() - powerSlider_a_DD.getMin()) * (evoAmount / 100.0F));

		powerSlider_r_DR = powerSlider_r_DR + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DR.getMax() - powerSlider_r_DR.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DG = powerSlider_r_DG + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DG.getMax() - powerSlider_r_DG.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DB = powerSlider_r_DB + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DB.getMax() - powerSlider_r_DB.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DW = powerSlider_r_DW + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DW.getMax() - powerSlider_r_DW.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DO = powerSlider_r_DO + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DO.getMax() - powerSlider_r_DO.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DK = powerSlider_r_DK + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DK.getMax() - powerSlider_r_DK.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DC = powerSlider_r_DC + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DC.getMax() - powerSlider_r_DC.getMin()) * (evoAmount / 100.0F));
		powerSlider_r_DD = powerSlider_r_DD + ((ofRandom(2.0F) - 1.0F) * (powerSlider_r_DD.getMax() - powerSlider_r_DD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_RR = vSlider_a_RR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RR.getMax() - vSlider_a_RR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RG = vSlider_a_RG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RG.getMax() - vSlider_a_RG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RB = vSlider_a_RB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RB.getMax() - vSlider_a_RB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RW = vSlider_a_RW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RW.getMax() - vSlider_a_RW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RO = vSlider_a_RO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RO.getMax() - vSlider_a_RO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RK = vSlider_a_RK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RK.getMax() - vSlider_a_RK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RC = vSlider_a_RC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RC.getMax() - vSlider_a_RC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_RD = vSlider_a_RD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_RD.getMax() - vSlider_a_RD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_RR = vSlider_r_RR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RR.getMax() - vSlider_r_RR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RG = vSlider_r_RG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RG.getMax() - vSlider_r_RG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RB = vSlider_r_RB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RB.getMax() - vSlider_r_RB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RW = vSlider_r_RW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RW.getMax() - vSlider_r_RW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RO = vSlider_r_RO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RO.getMax() - vSlider_r_RO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RK = vSlider_r_RK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RK.getMax() - vSlider_r_RK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RC = vSlider_r_RC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RC.getMax() - vSlider_r_RC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_RD = vSlider_r_RD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_RD.getMax() - vSlider_r_RD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_GR = vSlider_a_GR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GR.getMax() - vSlider_a_GR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GG = vSlider_a_GG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GG.getMax() - vSlider_a_GG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GB = vSlider_a_GB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GB.getMax() - vSlider_a_GB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GW = vSlider_a_GW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GW.getMax() - vSlider_a_GW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GO = vSlider_a_GO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GO.getMax() - vSlider_a_GO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GK = vSlider_a_GK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GK.getMax() - vSlider_a_GK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GC = vSlider_a_GC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GC.getMax() - vSlider_a_GC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_GD = vSlider_a_GD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_GD.getMax() - vSlider_a_GD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_GR = vSlider_r_GR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GR.getMax() - vSlider_r_GR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GG = vSlider_r_GG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GG.getMax() - vSlider_r_GG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GB = vSlider_r_GB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GB.getMax() - vSlider_r_GB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GW = vSlider_r_GW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GW.getMax() - vSlider_r_GW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GO = vSlider_r_GO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GO.getMax() - vSlider_r_GO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GK = vSlider_r_GK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GK.getMax() - vSlider_r_GK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GC = vSlider_r_GC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GC.getMax() - vSlider_r_GC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_GD = vSlider_r_GD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_GD.getMax() - vSlider_r_GD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_BR = vSlider_a_BR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BR.getMax() - vSlider_a_BR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BG = vSlider_a_BG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BG.getMax() - vSlider_a_BG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BB = vSlider_a_BB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BB.getMax() - vSlider_a_BB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BW = vSlider_a_BW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BW.getMax() - vSlider_a_BW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BO = vSlider_a_BO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BO.getMax() - vSlider_a_BO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BK = vSlider_a_BK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BK.getMax() - vSlider_a_BK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BC = vSlider_a_BC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BC.getMax() - vSlider_a_BC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_BD = vSlider_a_BD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_BD.getMax() - vSlider_a_BD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_BR = vSlider_r_BR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BR.getMax() - vSlider_r_BR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BG = vSlider_r_BG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BG.getMax() - vSlider_r_BG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BB = vSlider_r_BB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BB.getMax() - vSlider_r_BB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BW = vSlider_r_BW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BW.getMax() - vSlider_r_BW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BO = vSlider_r_BO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BO.getMax() - vSlider_r_BO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BK = vSlider_r_BK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BK.getMax() - vSlider_r_BK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BC = vSlider_r_BC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BC.getMax() - vSlider_r_BC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_BD = vSlider_r_BD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_BD.getMax() - vSlider_r_BD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_WR = vSlider_a_WR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WR.getMax() - vSlider_a_WR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WG = vSlider_a_WG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WG.getMax() - vSlider_a_WG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WB = vSlider_a_WB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WB.getMax() - vSlider_a_WB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WW = vSlider_a_WW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WW.getMax() - vSlider_a_WW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WO = vSlider_a_WO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WO.getMax() - vSlider_a_WO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WK = vSlider_a_WK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WK.getMax() - vSlider_a_WK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WC = vSlider_a_WC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WC.getMax() - vSlider_a_WC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_WD = vSlider_a_WD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_WD.getMax() - vSlider_a_WD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_WR = vSlider_r_WR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WR.getMax() - vSlider_r_WR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WG = vSlider_r_WG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WG.getMax() - vSlider_r_WG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WB = vSlider_r_WB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WB.getMax() - vSlider_r_WB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WW = vSlider_r_WW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WW.getMax() - vSlider_r_WW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WO = vSlider_r_WO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WO.getMax() - vSlider_r_WO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WK = vSlider_r_WK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WK.getMax() - vSlider_r_WK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WC = vSlider_r_WC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WC.getMax() - vSlider_r_WC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_WD = vSlider_r_WD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_WD.getMax() - vSlider_r_WD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_OR = vSlider_a_OR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OR.getMax() - vSlider_a_OR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OG = vSlider_a_OG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OG.getMax() - vSlider_a_OG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OB = vSlider_a_OB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OB.getMax() - vSlider_a_OB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OW = vSlider_a_OW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OW.getMax() - vSlider_a_OW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OO = vSlider_a_OO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OO.getMax() - vSlider_a_OO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OK = vSlider_a_OK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OK.getMax() - vSlider_a_OK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OC = vSlider_a_OC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OC.getMax() - vSlider_a_OC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_OD = vSlider_a_OD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_OD.getMax() - vSlider_a_OD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_OR = vSlider_r_OR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OR.getMax() - vSlider_r_OR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OG = vSlider_r_OG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OG.getMax() - vSlider_r_OG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OB = vSlider_r_OB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OB.getMax() - vSlider_r_OB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OW = vSlider_r_OW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OW.getMax() - vSlider_r_OW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OO = vSlider_r_OO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OO.getMax() - vSlider_r_OO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OK = vSlider_r_OK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OK.getMax() - vSlider_r_OK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OC = vSlider_r_OC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OC.getMax() - vSlider_r_OC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_OD = vSlider_r_OD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_OD.getMax() - vSlider_r_OD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_KR = vSlider_a_KR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KR.getMax() - vSlider_a_KR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KG = vSlider_a_KG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KG.getMax() - vSlider_a_KG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KB = vSlider_a_KB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KB.getMax() - vSlider_a_KB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KW = vSlider_a_KW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KW.getMax() - vSlider_a_KW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KO = vSlider_a_KO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KO.getMax() - vSlider_a_KO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KK = vSlider_a_KK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KK.getMax() - vSlider_a_KK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KC = vSlider_a_KC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KC.getMax() - vSlider_a_KC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_KD = vSlider_a_KD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_KD.getMax() - vSlider_a_KD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_KR = vSlider_r_KR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KR.getMax() - vSlider_r_KR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KG = vSlider_r_KG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KG.getMax() - vSlider_r_KG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KB = vSlider_r_KB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KB.getMax() - vSlider_r_KB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KW = vSlider_r_KW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KW.getMax() - vSlider_r_KW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KO = vSlider_r_KO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KO.getMax() - vSlider_r_KO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KK = vSlider_r_KK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KK.getMax() - vSlider_r_KK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KC = vSlider_r_KC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KC.getMax() - vSlider_r_KC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_KD = vSlider_r_KD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_KD.getMax() - vSlider_r_KD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_CR = vSlider_a_CR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CR.getMax() - vSlider_a_CR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CG = vSlider_a_CG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CG.getMax() - vSlider_a_CG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CB = vSlider_a_CB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CB.getMax() - vSlider_a_CB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CW = vSlider_a_CW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CW.getMax() - vSlider_a_CW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CO = vSlider_a_CO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CO.getMax() - vSlider_a_CO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CK = vSlider_a_CK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CK.getMax() - vSlider_a_CK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CC = vSlider_a_CC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CC.getMax() - vSlider_a_CC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_CD = vSlider_a_CD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_CD.getMax() - vSlider_a_CD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_CR = vSlider_r_CR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CR.getMax() - vSlider_r_CR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CG = vSlider_r_CG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CG.getMax() - vSlider_r_CG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CB = vSlider_r_CB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CB.getMax() - vSlider_r_CB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CW = vSlider_r_CW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CW.getMax() - vSlider_r_CW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CO = vSlider_r_CO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CO.getMax() - vSlider_r_CO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CK = vSlider_r_CK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CK.getMax() - vSlider_r_CK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CC = vSlider_r_CC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CC.getMax() - vSlider_r_CC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_CD = vSlider_r_CD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_CD.getMax() - vSlider_r_CD.getMin()) * (evoAmount / 100.0F));

		vSlider_a_DR = vSlider_a_DR + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DR.getMax() - vSlider_a_DR.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DG = vSlider_a_DG + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DG.getMax() - vSlider_a_DG.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DB = vSlider_a_DB + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DB.getMax() - vSlider_a_DB.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DW = vSlider_a_DW + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DW.getMax() - vSlider_a_DW.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DO = vSlider_a_DO + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DO.getMax() - vSlider_a_DO.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DK = vSlider_a_DK + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DK.getMax() - vSlider_a_DK.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DC = vSlider_a_DC + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DC.getMax() - vSlider_a_DC.getMin()) * (evoAmount / 100.0F));
		vSlider_a_DD = vSlider_a_DD + ((ofRandom(2.0F) - 1.0F) * (vSlider_a_DD.getMax() - vSlider_a_DD.getMin()) * (evoAmount / 100.0F));

		vSlider_r_DR = vSlider_r_DR + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DR.getMax() - vSlider_r_DR.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DG = vSlider_r_DG + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DG.getMax() - vSlider_r_DG.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DB = vSlider_r_DB + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DB.getMax() - vSlider_r_DB.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DW = vSlider_r_DW + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DW.getMax() - vSlider_r_DW.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DO = vSlider_r_DO + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DO.getMax() - vSlider_r_DO.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DK = vSlider_r_DK + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DK.getMax() - vSlider_r_DK.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DC = vSlider_r_DC + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DC.getMax() - vSlider_r_DC.getMin()) * (evoAmount / 100.0F));
		vSlider_r_DD = vSlider_r_DD + ((ofRandom(2.0F) - 1.0F) * (vSlider_r_DD.getMax() - vSlider_r_DD.getMin()) * (evoAmount / 100.0F));
		
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

		probabilitySlider_a_RR = probabilitySlider_a_RR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RR.getMax() - probabilitySlider_a_RR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RG = probabilitySlider_a_RG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RG.getMax() - probabilitySlider_a_RG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RB = probabilitySlider_a_RB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RB.getMax() - probabilitySlider_a_RB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RW = probabilitySlider_a_RW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RW.getMax() - probabilitySlider_a_RW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RO = probabilitySlider_a_RO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RO.getMax() - probabilitySlider_a_RO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RK = probabilitySlider_a_RK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RK.getMax() - probabilitySlider_a_RK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RC = probabilitySlider_a_RC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RC.getMax() - probabilitySlider_a_RC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_RD = probabilitySlider_a_RD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_RD.getMax() - probabilitySlider_a_RD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_RR = probabilitySlider_r_RR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RR.getMax() - probabilitySlider_r_RR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RG = probabilitySlider_r_RG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RG.getMax() - probabilitySlider_r_RG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RB = probabilitySlider_r_RB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RB.getMax() - probabilitySlider_r_RB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RW = probabilitySlider_r_RW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RW.getMax() - probabilitySlider_r_RW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RO = probabilitySlider_r_RO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RO.getMax() - probabilitySlider_r_RO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RK = probabilitySlider_r_RK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RK.getMax() - probabilitySlider_r_RK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RC = probabilitySlider_r_RC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RC.getMax() - probabilitySlider_r_RC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_RD = probabilitySlider_r_RD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_RD.getMax() - probabilitySlider_r_RD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_GR = probabilitySlider_a_GR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GR.getMax() - probabilitySlider_a_GR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GG = probabilitySlider_a_GG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GG.getMax() - probabilitySlider_a_GG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GB = probabilitySlider_a_GB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GB.getMax() - probabilitySlider_a_GB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GW = probabilitySlider_a_GW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GW.getMax() - probabilitySlider_a_GW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GO = probabilitySlider_a_GO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GO.getMax() - probabilitySlider_a_GO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GK = probabilitySlider_a_GK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GK.getMax() - probabilitySlider_a_GK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GC = probabilitySlider_a_GC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GC.getMax() - probabilitySlider_a_GC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_GD = probabilitySlider_a_GD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_GD.getMax() - probabilitySlider_a_GD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_GR = probabilitySlider_r_GR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GR.getMax() - probabilitySlider_r_GR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GG = probabilitySlider_r_GG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GG.getMax() - probabilitySlider_r_GG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GB = probabilitySlider_r_GB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GB.getMax() - probabilitySlider_r_GB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GW = probabilitySlider_r_GW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GW.getMax() - probabilitySlider_r_GW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GO = probabilitySlider_r_GO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GO.getMax() - probabilitySlider_r_GO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GK = probabilitySlider_r_GK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GK.getMax() - probabilitySlider_r_GK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GC = probabilitySlider_r_GC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GC.getMax() - probabilitySlider_r_GC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_GD = probabilitySlider_r_GD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_GD.getMax() - probabilitySlider_r_GD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_BR = probabilitySlider_a_BR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BR.getMax() - probabilitySlider_a_BR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BG = probabilitySlider_a_BG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BG.getMax() - probabilitySlider_a_BG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BB = probabilitySlider_a_BB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BB.getMax() - probabilitySlider_a_BB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BW = probabilitySlider_a_BW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BW.getMax() - probabilitySlider_a_BW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BO = probabilitySlider_a_BO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BO.getMax() - probabilitySlider_a_BO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BK = probabilitySlider_a_BK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BK.getMax() - probabilitySlider_a_BK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BC = probabilitySlider_a_BC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BC.getMax() - probabilitySlider_a_BC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_BD = probabilitySlider_a_BD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_BD.getMax() - probabilitySlider_a_BD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_BR = probabilitySlider_r_BR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BR.getMax() - probabilitySlider_r_BR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BG = probabilitySlider_r_BG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BG.getMax() - probabilitySlider_r_BG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BB = probabilitySlider_r_BB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BB.getMax() - probabilitySlider_r_BB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BW = probabilitySlider_r_BW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BW.getMax() - probabilitySlider_r_BW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BO = probabilitySlider_r_BO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BO.getMax() - probabilitySlider_r_BO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BK = probabilitySlider_r_BK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BK.getMax() - probabilitySlider_r_BK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BC = probabilitySlider_r_BC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BC.getMax() - probabilitySlider_r_BC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_BD = probabilitySlider_r_BD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_BD.getMax() - probabilitySlider_r_BD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_WR = probabilitySlider_a_WR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WR.getMax() - probabilitySlider_a_WR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WG = probabilitySlider_a_WG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WG.getMax() - probabilitySlider_a_WG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WB = probabilitySlider_a_WB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WB.getMax() - probabilitySlider_a_WB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WW = probabilitySlider_a_WW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WW.getMax() - probabilitySlider_a_WW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WO = probabilitySlider_a_WO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WO.getMax() - probabilitySlider_a_WO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WK = probabilitySlider_a_WK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WK.getMax() - probabilitySlider_a_WK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WC = probabilitySlider_a_WC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WC.getMax() - probabilitySlider_a_WC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_WD = probabilitySlider_a_WD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_WD.getMax() - probabilitySlider_a_WD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_WR = probabilitySlider_r_WR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WR.getMax() - probabilitySlider_r_WR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WG = probabilitySlider_r_WG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WG.getMax() - probabilitySlider_r_WG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WB = probabilitySlider_r_WB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WB.getMax() - probabilitySlider_r_WB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WW = probabilitySlider_r_WW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WW.getMax() - probabilitySlider_r_WW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WO = probabilitySlider_r_WO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WO.getMax() - probabilitySlider_r_WO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WK = probabilitySlider_r_WK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WK.getMax() - probabilitySlider_r_WK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WC = probabilitySlider_r_WC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WC.getMax() - probabilitySlider_r_WC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_WD = probabilitySlider_r_WD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_WD.getMax() - probabilitySlider_r_WD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_OR = probabilitySlider_a_OR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OR.getMax() - probabilitySlider_a_OR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OG = probabilitySlider_a_OG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OG.getMax() - probabilitySlider_a_OG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OB = probabilitySlider_a_OB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OB.getMax() - probabilitySlider_a_OB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OW = probabilitySlider_a_OW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OW.getMax() - probabilitySlider_a_OW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OO = probabilitySlider_a_OO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OO.getMax() - probabilitySlider_a_OO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OK = probabilitySlider_a_OK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OK.getMax() - probabilitySlider_a_OK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OC = probabilitySlider_a_OC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OC.getMax() - probabilitySlider_a_OC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_OD = probabilitySlider_a_OD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_OD.getMax() - probabilitySlider_a_OD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_OR = probabilitySlider_r_OR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OR.getMax() - probabilitySlider_r_OR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OG = probabilitySlider_r_OG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OG.getMax() - probabilitySlider_r_OG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OB = probabilitySlider_r_OB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OB.getMax() - probabilitySlider_r_OB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OW = probabilitySlider_r_OW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OW.getMax() - probabilitySlider_r_OW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OO = probabilitySlider_r_OO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OO.getMax() - probabilitySlider_r_OO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OK = probabilitySlider_r_OK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OK.getMax() - probabilitySlider_r_OK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OC = probabilitySlider_r_OC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OC.getMax() - probabilitySlider_r_OC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_OD = probabilitySlider_r_OD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_OD.getMax() - probabilitySlider_r_OD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_KR = probabilitySlider_a_KR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KR.getMax() - probabilitySlider_a_KR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KG = probabilitySlider_a_KG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KG.getMax() - probabilitySlider_a_KG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KB = probabilitySlider_a_KB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KB.getMax() - probabilitySlider_a_KB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KW = probabilitySlider_a_KW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KW.getMax() - probabilitySlider_a_KW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KO = probabilitySlider_a_KO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KO.getMax() - probabilitySlider_a_KO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KK = probabilitySlider_a_KK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KK.getMax() - probabilitySlider_a_KK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KC = probabilitySlider_a_KC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KC.getMax() - probabilitySlider_a_KC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_KD = probabilitySlider_a_KD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_KD.getMax() - probabilitySlider_a_KD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_KR = probabilitySlider_r_KR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KR.getMax() - probabilitySlider_r_KR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KG = probabilitySlider_r_KG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KG.getMax() - probabilitySlider_r_KG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KB = probabilitySlider_r_KB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KB.getMax() - probabilitySlider_r_KB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KW = probabilitySlider_r_KW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KW.getMax() - probabilitySlider_r_KW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KO = probabilitySlider_r_KO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KO.getMax() - probabilitySlider_r_KO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KK = probabilitySlider_r_KK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KK.getMax() - probabilitySlider_r_KK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KC = probabilitySlider_r_KC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KC.getMax() - probabilitySlider_r_KC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_KD = probabilitySlider_r_KD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_KD.getMax() - probabilitySlider_r_KD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_CR = probabilitySlider_a_CR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CR.getMax() - probabilitySlider_a_CR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CG = probabilitySlider_a_CG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CG.getMax() - probabilitySlider_a_CG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CB = probabilitySlider_a_CB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CB.getMax() - probabilitySlider_a_CB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CW = probabilitySlider_a_CW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CW.getMax() - probabilitySlider_a_CW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CO = probabilitySlider_a_CO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CO.getMax() - probabilitySlider_a_CO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CK = probabilitySlider_a_CK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CK.getMax() - probabilitySlider_a_CK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CC = probabilitySlider_a_CC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CC.getMax() - probabilitySlider_a_CC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_CD = probabilitySlider_a_CD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_CD.getMax() - probabilitySlider_a_CD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_CR = probabilitySlider_r_CR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CR.getMax() - probabilitySlider_r_CR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CG = probabilitySlider_r_CG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CG.getMax() - probabilitySlider_r_CG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CB = probabilitySlider_r_CB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CB.getMax() - probabilitySlider_r_CB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CW = probabilitySlider_r_CW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CW.getMax() - probabilitySlider_r_CW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CO = probabilitySlider_r_CO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CO.getMax() - probabilitySlider_r_CO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CK = probabilitySlider_r_CK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CK.getMax() - probabilitySlider_r_CK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CC = probabilitySlider_r_CC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CC.getMax() - probabilitySlider_r_CC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_CD = probabilitySlider_r_CD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_CD.getMax() - probabilitySlider_r_CD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_a_DR = probabilitySlider_a_DR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DR.getMax() - probabilitySlider_a_DR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DG = probabilitySlider_a_DG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DG.getMax() - probabilitySlider_a_DG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DB = probabilitySlider_a_DB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DB.getMax() - probabilitySlider_a_DB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DW = probabilitySlider_a_DW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DW.getMax() - probabilitySlider_a_DW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DO = probabilitySlider_a_DO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DO.getMax() - probabilitySlider_a_DO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DK = probabilitySlider_a_DK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DK.getMax() - probabilitySlider_a_DK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DC = probabilitySlider_a_DC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DC.getMax() - probabilitySlider_a_DC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_a_DD = probabilitySlider_a_DD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_a_DD.getMax() - probabilitySlider_a_DD.getMin()) * (evoAmount / 100.0F));

		probabilitySlider_r_DR = probabilitySlider_r_DR + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DR.getMax() - probabilitySlider_r_DR.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DG = probabilitySlider_r_DG + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DG.getMax() - probabilitySlider_r_DG.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DB = probabilitySlider_r_DB + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DB.getMax() - probabilitySlider_r_DB.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DW = probabilitySlider_r_DW + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DW.getMax() - probabilitySlider_r_DW.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DO = probabilitySlider_r_DO + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DO.getMax() - probabilitySlider_r_DO.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DK = probabilitySlider_r_DK + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DK.getMax() - probabilitySlider_r_DK.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DC = probabilitySlider_r_DC + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DC.getMax() - probabilitySlider_r_DC.getMin()) * (evoAmount / 100.0F));
		probabilitySlider_r_DD = probabilitySlider_r_DD + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider_r_DD.getMax() - probabilitySlider_r_DD.getMin()) * (evoAmount / 100.0F));

		if (vSlider_a_RR < 0.0F) vSlider_a_RR = 0;
		if (vSlider_a_RG < 0.0F) vSlider_a_RG = 0;
		if (vSlider_a_RB < 0.0F) vSlider_a_RB = 0;
		if (vSlider_a_RW < 0.0F) vSlider_a_RW = 0;
		if (vSlider_a_RO < 0.0F) vSlider_a_RO = 0;
		if (vSlider_a_RK < 0.0F) vSlider_a_RK = 0;
		if (vSlider_a_RC < 0.0F) vSlider_a_RC = 0;
		if (vSlider_a_RD < 0.0F) vSlider_a_RD = 0;

		if (vSlider_r_RR < 0.0F) vSlider_r_RR = 0;
		if (vSlider_r_RG < 0.0F) vSlider_r_RG = 0;
		if (vSlider_r_RB < 0.0F) vSlider_r_RB = 0;
		if (vSlider_r_RW < 0.0F) vSlider_r_RW = 0;
		if (vSlider_r_RO < 0.0F) vSlider_r_RO = 0;
		if (vSlider_r_RK < 0.0F) vSlider_r_RK = 0;
		if (vSlider_r_RC < 0.0F) vSlider_r_RC = 0;
		if (vSlider_r_RD < 0.0F) vSlider_r_RD = 0;

		if (vSlider_a_GR < 0.0F) vSlider_a_GR = 0;
		if (vSlider_a_GG < 0.0F) vSlider_a_GG = 0;
		if (vSlider_a_GB < 0.0F) vSlider_a_GB = 0;
		if (vSlider_a_GW < 0.0F) vSlider_a_GW = 0;
		if (vSlider_a_GO < 0.0F) vSlider_a_GO = 0;
		if (vSlider_a_GK < 0.0F) vSlider_a_GK = 0;
		if (vSlider_a_GC < 0.0F) vSlider_a_GC = 0;
		if (vSlider_a_GD < 0.0F) vSlider_a_GD = 0;

		if (vSlider_r_GR < 0.0F) vSlider_r_GR = 0;
		if (vSlider_r_GG < 0.0F) vSlider_r_GG = 0;
		if (vSlider_r_GB < 0.0F) vSlider_r_GB = 0;
		if (vSlider_r_GW < 0.0F) vSlider_r_GW = 0;
		if (vSlider_r_GO < 0.0F) vSlider_r_GO = 0;
		if (vSlider_r_GK < 0.0F) vSlider_r_GK = 0;
		if (vSlider_r_GC < 0.0F) vSlider_r_GC = 0;
		if (vSlider_r_GD < 0.0F) vSlider_r_GD = 0;

		if (vSlider_a_BR < 0.0F) vSlider_a_BR = 0;
		if (vSlider_a_BG < 0.0F) vSlider_a_BG = 0;
		if (vSlider_a_BB < 0.0F) vSlider_a_BB = 0;
		if (vSlider_a_BW < 0.0F) vSlider_a_BW = 0;
		if (vSlider_a_BO < 0.0F) vSlider_a_BO = 0;
		if (vSlider_a_BK < 0.0F) vSlider_a_BK = 0;
		if (vSlider_a_BC < 0.0F) vSlider_a_BC = 0;
		if (vSlider_a_BD < 0.0F) vSlider_a_BD = 0;

		if (vSlider_r_BR < 0.0F) vSlider_r_BR = 0;
		if (vSlider_r_BG < 0.0F) vSlider_r_BG = 0;
		if (vSlider_r_BB < 0.0F) vSlider_r_BB = 0;
		if (vSlider_r_BW < 0.0F) vSlider_r_BW = 0;
		if (vSlider_r_BO < 0.0F) vSlider_r_BO = 0;
		if (vSlider_r_BK < 0.0F) vSlider_r_BK = 0;
		if (vSlider_r_BC < 0.0F) vSlider_r_BC = 0;
		if (vSlider_r_BD < 0.0F) vSlider_r_BD = 0;

		if (vSlider_a_WR < 0.0F) vSlider_a_WR = 0;
		if (vSlider_a_WG < 0.0F) vSlider_a_WG = 0;
		if (vSlider_a_WB < 0.0F) vSlider_a_WB = 0;
		if (vSlider_a_WW < 0.0F) vSlider_a_WW = 0;
		if (vSlider_a_WO < 0.0F) vSlider_a_WO = 0;
		if (vSlider_a_WK < 0.0F) vSlider_a_WK = 0;
		if (vSlider_a_WC < 0.0F) vSlider_a_WC = 0;
		if (vSlider_a_WD < 0.0F) vSlider_a_WD = 0;

		if (vSlider_r_WR < 0.0F) vSlider_r_WR = 0;
		if (vSlider_r_WG < 0.0F) vSlider_r_WG = 0;
		if (vSlider_r_WB < 0.0F) vSlider_r_WB = 0;
		if (vSlider_r_WW < 0.0F) vSlider_r_WW = 0;
		if (vSlider_r_WO < 0.0F) vSlider_r_WO = 0;
		if (vSlider_r_WK < 0.0F) vSlider_r_WK = 0;
		if (vSlider_r_WC < 0.0F) vSlider_r_WC = 0;
		if (vSlider_r_WD < 0.0F) vSlider_r_WD = 0;

		if (vSlider_a_OR < 0.0F) vSlider_a_OR = 0;
		if (vSlider_a_OG < 0.0F) vSlider_a_OG = 0;
		if (vSlider_a_OB < 0.0F) vSlider_a_OB = 0;
		if (vSlider_a_OW < 0.0F) vSlider_a_OW = 0;
		if (vSlider_a_OO < 0.0F) vSlider_a_OO = 0;
		if (vSlider_a_OK < 0.0F) vSlider_a_OK = 0;
		if (vSlider_a_OC < 0.0F) vSlider_a_OC = 0;
		if (vSlider_a_OD < 0.0F) vSlider_a_OD = 0;

		if (vSlider_r_OR < 0.0F) vSlider_r_OR = 0;
		if (vSlider_r_OG < 0.0F) vSlider_r_OG = 0;
		if (vSlider_r_OB < 0.0F) vSlider_r_OB = 0;
		if (vSlider_r_OW < 0.0F) vSlider_r_OW = 0;
		if (vSlider_r_OO < 0.0F) vSlider_r_OO = 0;
		if (vSlider_r_OK < 0.0F) vSlider_r_OK = 0;
		if (vSlider_r_OC < 0.0F) vSlider_r_OC = 0;
		if (vSlider_r_OD < 0.0F) vSlider_r_OD = 0;

		if (vSlider_a_KR < 0.0F) vSlider_a_KR = 0;
		if (vSlider_a_KG < 0.0F) vSlider_a_KG = 0;
		if (vSlider_a_KB < 0.0F) vSlider_a_KB = 0;
		if (vSlider_a_KW < 0.0F) vSlider_a_KW = 0;
		if (vSlider_a_KO < 0.0F) vSlider_a_KO = 0;
		if (vSlider_a_KK < 0.0F) vSlider_a_KK = 0;
		if (vSlider_a_KC < 0.0F) vSlider_a_KC = 0;
		if (vSlider_a_KD < 0.0F) vSlider_a_KD = 0;

		if (vSlider_r_KR < 0.0F) vSlider_r_KR = 0;
		if (vSlider_r_KG < 0.0F) vSlider_r_KG = 0;
		if (vSlider_r_KB < 0.0F) vSlider_r_KB = 0;
		if (vSlider_r_KW < 0.0F) vSlider_r_KW = 0;
		if (vSlider_r_KO < 0.0F) vSlider_r_KO = 0;
		if (vSlider_r_KK < 0.0F) vSlider_r_KK = 0;
		if (vSlider_r_KC < 0.0F) vSlider_r_KC = 0;
		if (vSlider_r_KD < 0.0F) vSlider_r_KD = 0;

		if (vSlider_a_CR < 0.0F) vSlider_a_CR = 0;
		if (vSlider_a_CG < 0.0F) vSlider_a_CG = 0;
		if (vSlider_a_CB < 0.0F) vSlider_a_CB = 0;
		if (vSlider_a_CW < 0.0F) vSlider_a_CW = 0;
		if (vSlider_a_CO < 0.0F) vSlider_a_CO = 0;
		if (vSlider_a_CK < 0.0F) vSlider_a_CK = 0;
		if (vSlider_a_CC < 0.0F) vSlider_a_CC = 0;
		if (vSlider_a_CD < 0.0F) vSlider_a_CD = 0;

		if (vSlider_r_CR < 0.0F) vSlider_r_CR = 0;
		if (vSlider_r_CG < 0.0F) vSlider_r_CG = 0;
		if (vSlider_r_CB < 0.0F) vSlider_r_CB = 0;
		if (vSlider_r_CW < 0.0F) vSlider_r_CW = 0;
		if (vSlider_r_CO < 0.0F) vSlider_r_CO = 0;
		if (vSlider_r_CK < 0.0F) vSlider_r_CK = 0;
		if (vSlider_r_CC < 0.0F) vSlider_r_CC = 0;
		if (vSlider_r_CD < 0.0F) vSlider_r_CD = 0;

		if (vSlider_a_DR < 0.0F) vSlider_a_DR = 0;
		if (vSlider_a_DG < 0.0F) vSlider_a_DG = 0;
		if (vSlider_a_DB < 0.0F) vSlider_a_DB = 0;
		if (vSlider_a_DW < 0.0F) vSlider_a_DW = 0;
		if (vSlider_a_DO < 0.0F) vSlider_a_DO = 0;
		if (vSlider_a_DK < 0.0F) vSlider_a_DK = 0;
		if (vSlider_a_DC < 0.0F) vSlider_a_DC = 0;
		if (vSlider_a_DD < 0.0F) vSlider_a_DD = 0;

		if (vSlider_r_DR < 0.0F) vSlider_r_DR = 0;
		if (vSlider_r_DG < 0.0F) vSlider_r_DG = 0;
		if (vSlider_r_DB < 0.0F) vSlider_r_DB = 0;
		if (vSlider_r_DW < 0.0F) vSlider_r_DW = 0;
		if (vSlider_r_DO < 0.0F) vSlider_r_DO = 0;
		if (vSlider_r_DK < 0.0F) vSlider_r_DK = 0;
		if (vSlider_r_DC < 0.0F) vSlider_r_DC = 0;
		if (vSlider_r_DD < 0.0F) vSlider_r_DD = 0;

		if (probabilitySlider < 0.0F) probabilitySlider = 0;

		if (probabilitySlider_a_RR < 0.0F) probabilitySlider_a_RR = 0;
		if (probabilitySlider_a_RG < 0.0F) probabilitySlider_a_RG = 0;
		if (probabilitySlider_a_RB < 0.0F) probabilitySlider_a_RB = 0;
		if (probabilitySlider_a_RW < 0.0F) probabilitySlider_a_RW = 0;
		if (probabilitySlider_a_RO < 0.0F) probabilitySlider_a_RO = 0;
		if (probabilitySlider_a_RK < 0.0F) probabilitySlider_a_RK = 0;
		if (probabilitySlider_a_RC < 0.0F) probabilitySlider_a_RC = 0;
		if (probabilitySlider_a_RD < 0.0F) probabilitySlider_a_RD = 0;

		if (probabilitySlider_r_RR < 0.0F) probabilitySlider_r_RR = 0;
		if (probabilitySlider_r_RG < 0.0F) probabilitySlider_r_RG = 0;
		if (probabilitySlider_r_RB < 0.0F) probabilitySlider_r_RB = 0;
		if (probabilitySlider_r_RW < 0.0F) probabilitySlider_r_RW = 0;
		if (probabilitySlider_r_RO < 0.0F) probabilitySlider_r_RO = 0;
		if (probabilitySlider_r_RK < 0.0F) probabilitySlider_r_RK = 0;
		if (probabilitySlider_r_RC < 0.0F) probabilitySlider_r_RC = 0;
		if (probabilitySlider_r_RD < 0.0F) probabilitySlider_r_RD = 0;

		if (probabilitySlider_a_GR < 0.0F) probabilitySlider_a_GR = 0;
		if (probabilitySlider_a_GG < 0.0F) probabilitySlider_a_GG = 0;
		if (probabilitySlider_a_GB < 0.0F) probabilitySlider_a_GB = 0;
		if (probabilitySlider_a_GW < 0.0F) probabilitySlider_a_GW = 0;
		if (probabilitySlider_a_GO < 0.0F) probabilitySlider_a_GO = 0;
		if (probabilitySlider_a_GK < 0.0F) probabilitySlider_a_GK = 0;
		if (probabilitySlider_a_GC < 0.0F) probabilitySlider_a_GC = 0;
		if (probabilitySlider_a_GD < 0.0F) probabilitySlider_a_GD = 0;

		if (probabilitySlider_r_GR < 0.0F) probabilitySlider_r_GR = 0;
		if (probabilitySlider_r_GG < 0.0F) probabilitySlider_r_GG = 0;
		if (probabilitySlider_r_GB < 0.0F) probabilitySlider_r_GB = 0;
		if (probabilitySlider_r_GW < 0.0F) probabilitySlider_r_GW = 0;
		if (probabilitySlider_r_GO < 0.0F) probabilitySlider_r_GO = 0;
		if (probabilitySlider_r_GK < 0.0F) probabilitySlider_r_GK = 0;
		if (probabilitySlider_r_GC < 0.0F) probabilitySlider_r_GC = 0;
		if (probabilitySlider_r_GD < 0.0F) probabilitySlider_r_GD = 0;

		if (probabilitySlider_a_BR < 0.0F) probabilitySlider_a_BR = 0;
		if (probabilitySlider_a_BG < 0.0F) probabilitySlider_a_BG = 0;
		if (probabilitySlider_a_BB < 0.0F) probabilitySlider_a_BB = 0;
		if (probabilitySlider_a_BW < 0.0F) probabilitySlider_a_BW = 0;
		if (probabilitySlider_a_BO < 0.0F) probabilitySlider_a_BO = 0;
		if (probabilitySlider_a_BK < 0.0F) probabilitySlider_a_BK = 0;
		if (probabilitySlider_a_BC < 0.0F) probabilitySlider_a_BC = 0;
		if (probabilitySlider_a_BD < 0.0F) probabilitySlider_a_BD = 0;

		if (probabilitySlider_r_BR < 0.0F) probabilitySlider_r_BR = 0;
		if (probabilitySlider_r_BG < 0.0F) probabilitySlider_r_BG = 0;
		if (probabilitySlider_r_BB < 0.0F) probabilitySlider_r_BB = 0;
		if (probabilitySlider_r_BW < 0.0F) probabilitySlider_r_BW = 0;
		if (probabilitySlider_r_BO < 0.0F) probabilitySlider_r_BO = 0;
		if (probabilitySlider_r_BK < 0.0F) probabilitySlider_r_BK = 0;
		if (probabilitySlider_r_BC < 0.0F) probabilitySlider_r_BC = 0;
		if (probabilitySlider_r_BD < 0.0F) probabilitySlider_r_BD = 0;

		if (probabilitySlider_a_WR < 0.0F) probabilitySlider_a_WR = 0;
		if (probabilitySlider_a_WG < 0.0F) probabilitySlider_a_WG = 0;
		if (probabilitySlider_a_WB < 0.0F) probabilitySlider_a_WB = 0;
		if (probabilitySlider_a_WW < 0.0F) probabilitySlider_a_WW = 0;
		if (probabilitySlider_a_WO < 0.0F) probabilitySlider_a_WO = 0;
		if (probabilitySlider_a_WK < 0.0F) probabilitySlider_a_WK = 0;
		if (probabilitySlider_a_WC < 0.0F) probabilitySlider_a_WC = 0;
		if (probabilitySlider_a_WD < 0.0F) probabilitySlider_a_WD = 0;

		if (probabilitySlider_r_WR < 0.0F) probabilitySlider_r_WR = 0;
		if (probabilitySlider_r_WG < 0.0F) probabilitySlider_r_WG = 0;
		if (probabilitySlider_r_WB < 0.0F) probabilitySlider_r_WB = 0;
		if (probabilitySlider_r_WW < 0.0F) probabilitySlider_r_WW = 0;
		if (probabilitySlider_r_WO < 0.0F) probabilitySlider_r_WO = 0;
		if (probabilitySlider_r_WK < 0.0F) probabilitySlider_r_WK = 0;
		if (probabilitySlider_r_WC < 0.0F) probabilitySlider_r_WC = 0;
		if (probabilitySlider_r_WD < 0.0F) probabilitySlider_r_WD = 0;

		if (probabilitySlider_a_OR < 0.0F) probabilitySlider_a_OR = 0;
		if (probabilitySlider_a_OG < 0.0F) probabilitySlider_a_OG = 0;
		if (probabilitySlider_a_OB < 0.0F) probabilitySlider_a_OB = 0;
		if (probabilitySlider_a_OW < 0.0F) probabilitySlider_a_OW = 0;
		if (probabilitySlider_a_OO < 0.0F) probabilitySlider_a_OO = 0;
		if (probabilitySlider_a_OK < 0.0F) probabilitySlider_a_OK = 0;
		if (probabilitySlider_a_OC < 0.0F) probabilitySlider_a_OC = 0;
		if (probabilitySlider_a_OD < 0.0F) probabilitySlider_a_OD = 0;

		if (probabilitySlider_r_OR < 0.0F) probabilitySlider_r_OR = 0;
		if (probabilitySlider_r_OG < 0.0F) probabilitySlider_r_OG = 0;
		if (probabilitySlider_r_OB < 0.0F) probabilitySlider_r_OB = 0;
		if (probabilitySlider_r_OW < 0.0F) probabilitySlider_r_OW = 0;
		if (probabilitySlider_r_OO < 0.0F) probabilitySlider_r_OO = 0;
		if (probabilitySlider_r_OK < 0.0F) probabilitySlider_r_OK = 0;
		if (probabilitySlider_r_OC < 0.0F) probabilitySlider_r_OC = 0;
		if (probabilitySlider_r_OD < 0.0F) probabilitySlider_r_OD = 0;

		if (probabilitySlider_a_KR < 0.0F) probabilitySlider_a_KR = 0;
		if (probabilitySlider_a_KG < 0.0F) probabilitySlider_a_KG = 0;
		if (probabilitySlider_a_KB < 0.0F) probabilitySlider_a_KB = 0;
		if (probabilitySlider_a_KW < 0.0F) probabilitySlider_a_KW = 0;
		if (probabilitySlider_a_KO < 0.0F) probabilitySlider_a_KO = 0;
		if (probabilitySlider_a_KK < 0.0F) probabilitySlider_a_KK = 0;
		if (probabilitySlider_a_KC < 0.0F) probabilitySlider_a_KC = 0;
		if (probabilitySlider_a_KD < 0.0F) probabilitySlider_a_KD = 0;

		if (probabilitySlider_r_KR < 0.0F) probabilitySlider_r_KR = 0;
		if (probabilitySlider_r_KG < 0.0F) probabilitySlider_r_KG = 0;
		if (probabilitySlider_r_KB < 0.0F) probabilitySlider_r_KB = 0;
		if (probabilitySlider_r_KW < 0.0F) probabilitySlider_r_KW = 0;
		if (probabilitySlider_r_KO < 0.0F) probabilitySlider_r_KO = 0;
		if (probabilitySlider_r_KK < 0.0F) probabilitySlider_r_KK = 0;
		if (probabilitySlider_r_KC < 0.0F) probabilitySlider_r_KC = 0;
		if (probabilitySlider_r_KD < 0.0F) probabilitySlider_r_KD = 0;

		if (probabilitySlider_a_CR < 0.0F) probabilitySlider_a_CR = 0;
		if (probabilitySlider_a_CG < 0.0F) probabilitySlider_a_CG = 0;
		if (probabilitySlider_a_CB < 0.0F) probabilitySlider_a_CB = 0;
		if (probabilitySlider_a_CW < 0.0F) probabilitySlider_a_CW = 0;
		if (probabilitySlider_a_CO < 0.0F) probabilitySlider_a_CO = 0;
		if (probabilitySlider_a_CK < 0.0F) probabilitySlider_a_CK = 0;
		if (probabilitySlider_a_CC < 0.0F) probabilitySlider_a_CC = 0;
		if (probabilitySlider_a_CD < 0.0F) probabilitySlider_a_CD = 0;

		if (probabilitySlider_r_CR < 0.0F) probabilitySlider_r_CR = 0;
		if (probabilitySlider_r_CG < 0.0F) probabilitySlider_r_CG = 0;
		if (probabilitySlider_r_CB < 0.0F) probabilitySlider_r_CB = 0;
		if (probabilitySlider_r_CW < 0.0F) probabilitySlider_r_CW = 0;
		if (probabilitySlider_r_CO < 0.0F) probabilitySlider_r_CO = 0;
		if (probabilitySlider_r_CK < 0.0F) probabilitySlider_r_CK = 0;
		if (probabilitySlider_r_CC < 0.0F) probabilitySlider_r_CC = 0;
		if (probabilitySlider_r_CD < 0.0F) probabilitySlider_r_CD = 0;

		if (probabilitySlider_a_DR < 0.0F) probabilitySlider_a_DR = 0;
		if (probabilitySlider_a_DG < 0.0F) probabilitySlider_a_DG = 0;
		if (probabilitySlider_a_DB < 0.0F) probabilitySlider_a_DB = 0;
		if (probabilitySlider_a_DW < 0.0F) probabilitySlider_a_DW = 0;
		if (probabilitySlider_a_DO < 0.0F) probabilitySlider_a_DO = 0;
		if (probabilitySlider_a_DK < 0.0F) probabilitySlider_a_DK = 0;
		if (probabilitySlider_a_DC < 0.0F) probabilitySlider_a_DC = 0;
		if (probabilitySlider_a_DD < 0.0F) probabilitySlider_a_DD = 0;

		if (probabilitySlider_r_DR < 0.0F) probabilitySlider_r_DR = 0;
		if (probabilitySlider_r_DG < 0.0F) probabilitySlider_r_DG = 0;
		if (probabilitySlider_r_DB < 0.0F) probabilitySlider_r_DB = 0;
		if (probabilitySlider_r_DW < 0.0F) probabilitySlider_r_DW = 0;
		if (probabilitySlider_r_DO < 0.0F) probabilitySlider_r_DO = 0;
		if (probabilitySlider_r_DK < 0.0F) probabilitySlider_r_DK = 0;
		if (probabilitySlider_r_DC < 0.0F) probabilitySlider_r_DC = 0;
		if (probabilitySlider_r_DD < 0.0F) probabilitySlider_r_DD = 0;

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
		

		if (vSlider_a_RR > 500.0F) vSlider_a_RR = 500.0F;
		if (vSlider_a_RG > 500.0F) vSlider_a_RG = 500.0F;
		if (vSlider_a_RB > 500.0F) vSlider_a_RB = 500.0F;
		if (vSlider_a_RW > 500.0F) vSlider_a_RW = 500.0F;
		if (vSlider_a_RO > 500.0F) vSlider_a_RO = 500.0F;
		if (vSlider_a_RK > 500.0F) vSlider_a_RK = 500.0F;
		if (vSlider_a_RC > 500.0F) vSlider_a_RC = 500.0F;
		if (vSlider_a_RD > 500.0F) vSlider_a_RD = 500.0F;

		if (vSlider_r_RR > 500.0F) vSlider_r_RR = 500.0F;
		if (vSlider_r_RG > 500.0F) vSlider_r_RG = 500.0F;
		if (vSlider_r_RB > 500.0F) vSlider_r_RB = 500.0F;
		if (vSlider_r_RW > 500.0F) vSlider_r_RW = 500.0F;
		if (vSlider_r_RO > 500.0F) vSlider_r_RO = 500.0F;
		if (vSlider_r_RK > 500.0F) vSlider_r_RK = 500.0F;
		if (vSlider_r_RC > 500.0F) vSlider_r_RC = 500.0F;
		if (vSlider_r_RD > 500.0F) vSlider_r_RD = 500.0F;

		if (vSlider_a_GR > 500.0F) vSlider_a_GR = 500.0F;
		if (vSlider_a_GG > 500.0F) vSlider_a_GG = 500.0F;
		if (vSlider_a_GB > 500.0F) vSlider_a_GB = 500.0F;
		if (vSlider_a_GW > 500.0F) vSlider_a_GW = 500.0F;
		if (vSlider_a_GO > 500.0F) vSlider_a_GO = 500.0F;
		if (vSlider_a_GK > 500.0F) vSlider_a_GK = 500.0F;
		if (vSlider_a_GC > 500.0F) vSlider_a_GC = 500.0F;
		if (vSlider_a_GD > 500.0F) vSlider_a_GD = 500.0F;

		if (vSlider_r_GR > 500.0F) vSlider_r_GR = 500.0F;
		if (vSlider_r_GG > 500.0F) vSlider_r_GG = 500.0F;
		if (vSlider_r_GB > 500.0F) vSlider_r_GB = 500.0F;
		if (vSlider_r_GW > 500.0F) vSlider_r_GW = 500.0F;
		if (vSlider_r_GO > 500.0F) vSlider_r_GO = 500.0F;
		if (vSlider_r_GK > 500.0F) vSlider_r_GK = 500.0F;
		if (vSlider_r_GC > 500.0F) vSlider_r_GC = 500.0F;
		if (vSlider_r_GD > 500.0F) vSlider_r_GD = 500.0F;

		if (vSlider_a_BR > 500.0F) vSlider_a_BR = 500.0F;
		if (vSlider_a_BG > 500.0F) vSlider_a_BG = 500.0F;
		if (vSlider_a_BB > 500.0F) vSlider_a_BB = 500.0F;
		if (vSlider_a_BW > 500.0F) vSlider_a_BW = 500.0F;
		if (vSlider_a_BO > 500.0F) vSlider_a_BO = 500.0F;
		if (vSlider_a_BK > 500.0F) vSlider_a_BK = 500.0F;
		if (vSlider_a_BC > 500.0F) vSlider_a_BC = 500.0F;
		if (vSlider_a_BD > 500.0F) vSlider_a_BD = 500.0F;

		if (vSlider_r_BR > 500.0F) vSlider_r_BR = 500.0F;
		if (vSlider_r_BG > 500.0F) vSlider_r_BG = 500.0F;
		if (vSlider_r_BB > 500.0F) vSlider_r_BB = 500.0F;
		if (vSlider_r_BW > 500.0F) vSlider_r_BW = 500.0F;
		if (vSlider_r_BO > 500.0F) vSlider_r_BO = 500.0F;
		if (vSlider_r_BK > 500.0F) vSlider_r_BK = 500.0F;
		if (vSlider_r_BC > 500.0F) vSlider_r_BC = 500.0F;
		if (vSlider_r_BD > 500.0F) vSlider_r_BD = 500.0F;

		if (vSlider_a_WR > 500.0F) vSlider_a_WR = 500.0F;
		if (vSlider_a_WG > 500.0F) vSlider_a_WG = 500.0F;
		if (vSlider_a_WB > 500.0F) vSlider_a_WB = 500.0F;
		if (vSlider_a_WW > 500.0F) vSlider_a_WW = 500.0F;
		if (vSlider_a_WO > 500.0F) vSlider_a_WO = 500.0F;
		if (vSlider_a_WK > 500.0F) vSlider_a_WK = 500.0F;
		if (vSlider_a_WC > 500.0F) vSlider_a_WC = 500.0F;
		if (vSlider_a_WD > 500.0F) vSlider_a_WD = 500.0F;

		if (vSlider_r_WR > 500.0F) vSlider_r_WR = 500.0F;
		if (vSlider_r_WG > 500.0F) vSlider_r_WG = 500.0F;
		if (vSlider_r_WB > 500.0F) vSlider_r_WB = 500.0F;
		if (vSlider_r_WW > 500.0F) vSlider_r_WW = 500.0F;
		if (vSlider_r_WO > 500.0F) vSlider_r_WO = 500.0F;
		if (vSlider_r_WK > 500.0F) vSlider_r_WK = 500.0F;
		if (vSlider_r_WC > 500.0F) vSlider_r_WC = 500.0F;
		if (vSlider_r_WD > 500.0F) vSlider_r_WD = 500.0F;

		if (vSlider_a_OR > 500.0F) vSlider_a_OR = 500.0F;
		if (vSlider_a_OG > 500.0F) vSlider_a_OG = 500.0F;
		if (vSlider_a_OB > 500.0F) vSlider_a_OB = 500.0F;
		if (vSlider_a_OW > 500.0F) vSlider_a_OW = 500.0F;
		if (vSlider_a_OO > 500.0F) vSlider_a_OO = 500.0F;
		if (vSlider_a_OK > 500.0F) vSlider_a_OK = 500.0F;
		if (vSlider_a_OC > 500.0F) vSlider_a_OC = 500.0F;
		if (vSlider_a_OD > 500.0F) vSlider_a_OD = 500.0F;

		if (vSlider_r_OR > 500.0F) vSlider_r_OR = 500.0F;
		if (vSlider_r_OG > 500.0F) vSlider_r_OG = 500.0F;
		if (vSlider_r_OB > 500.0F) vSlider_r_OB = 500.0F;
		if (vSlider_r_OW > 500.0F) vSlider_r_OW = 500.0F;
		if (vSlider_r_OO > 500.0F) vSlider_r_OO = 500.0F;
		if (vSlider_r_OK > 500.0F) vSlider_r_OK = 500.0F;
		if (vSlider_r_OC > 500.0F) vSlider_r_OC = 500.0F;
		if (vSlider_r_OD > 500.0F) vSlider_r_OD = 500.0F;

		if (vSlider_a_KR > 500.0F) vSlider_a_KR = 500.0F;
		if (vSlider_a_KG > 500.0F) vSlider_a_KG = 500.0F;
		if (vSlider_a_KB > 500.0F) vSlider_a_KB = 500.0F;
		if (vSlider_a_KW > 500.0F) vSlider_a_KW = 500.0F;
		if (vSlider_a_KO > 500.0F) vSlider_a_KO = 500.0F;
		if (vSlider_a_KK > 500.0F) vSlider_a_KK = 500.0F;
		if (vSlider_a_KC > 500.0F) vSlider_a_KC = 500.0F;
		if (vSlider_a_KD > 500.0F) vSlider_a_KD = 500.0F;

		if (vSlider_r_KR > 500.0F) vSlider_r_KR = 500.0F;
		if (vSlider_r_KG > 500.0F) vSlider_r_KG = 500.0F;
		if (vSlider_r_KB > 500.0F) vSlider_r_KB = 500.0F;
		if (vSlider_r_KW > 500.0F) vSlider_r_KW = 500.0F;
		if (vSlider_r_KO > 500.0F) vSlider_r_KO = 500.0F;
		if (vSlider_r_KK > 500.0F) vSlider_r_KK = 500.0F;
		if (vSlider_r_KC > 500.0F) vSlider_r_KC = 500.0F;
		if (vSlider_r_KD > 500.0F) vSlider_r_KD = 500.0F;

		if (vSlider_a_CR > 500.0F) vSlider_a_CR = 500.0F;
		if (vSlider_a_CG > 500.0F) vSlider_a_CG = 500.0F;
		if (vSlider_a_CB > 500.0F) vSlider_a_CB = 500.0F;
		if (vSlider_a_CW > 500.0F) vSlider_a_CW = 500.0F;
		if (vSlider_a_CO > 500.0F) vSlider_a_CO = 500.0F;
		if (vSlider_a_CK > 500.0F) vSlider_a_CK = 500.0F;
		if (vSlider_a_CC > 500.0F) vSlider_a_CC = 500.0F;
		if (vSlider_a_CD > 500.0F) vSlider_a_CD = 500.0F;

		if (vSlider_r_CR > 500.0F) vSlider_r_CR = 500.0F;
		if (vSlider_r_CG > 500.0F) vSlider_r_CG = 500.0F;
		if (vSlider_r_CB > 500.0F) vSlider_r_CB = 500.0F;
		if (vSlider_r_CW > 500.0F) vSlider_r_CW = 500.0F;
		if (vSlider_r_CO > 500.0F) vSlider_r_CO = 500.0F;
		if (vSlider_r_CK > 500.0F) vSlider_r_CK = 500.0F;
		if (vSlider_r_CC > 500.0F) vSlider_r_CC = 500.0F;
		if (vSlider_r_CD > 500.0F) vSlider_r_CD = 500.0F;

		if (vSlider_a_DR > 500.0F) vSlider_a_DR = 500.0F;
		if (vSlider_a_DG > 500.0F) vSlider_a_DG = 500.0F;
		if (vSlider_a_DB > 500.0F) vSlider_a_DB = 500.0F;
		if (vSlider_a_DW > 500.0F) vSlider_a_DW = 500.0F;
		if (vSlider_a_DO > 500.0F) vSlider_a_DO = 500.0F;
		if (vSlider_a_DK > 500.0F) vSlider_a_DK = 500.0F;
		if (vSlider_a_DC > 500.0F) vSlider_a_DC = 500.0F;
		if (vSlider_a_DD > 500.0F) vSlider_a_DD = 500.0F;

		if (vSlider_r_DR > 500.0F) vSlider_r_DR = 500.0F;
		if (vSlider_r_DG > 500.0F) vSlider_r_DG = 500.0F;
		if (vSlider_r_DB > 500.0F) vSlider_r_DB = 500.0F;
		if (vSlider_r_DW > 500.0F) vSlider_r_DW = 500.0F;
		if (vSlider_r_DO > 500.0F) vSlider_r_DO = 500.0F;
		if (vSlider_r_DK > 500.0F) vSlider_r_DK = 500.0F;
		if (vSlider_r_DC > 500.0F) vSlider_r_DC = 500.0F;
		if (vSlider_r_DD > 500.0F) vSlider_r_DD = 500.0F;

		if (probabilitySlider > 100.0F) probabilitySlider = 100.0F;

		if (probabilitySlider_a_RR > 100.0F) probabilitySlider_a_RR = 100.0F;
		if (probabilitySlider_a_RG > 100.0F) probabilitySlider_a_RG = 100.0F;
		if (probabilitySlider_a_RB > 100.0F) probabilitySlider_a_RB = 100.0F;
		if (probabilitySlider_a_RW > 100.0F) probabilitySlider_a_RW = 100.0F;
		if (probabilitySlider_a_RO > 100.0F) probabilitySlider_a_RO = 100.0F;
		if (probabilitySlider_a_RK > 100.0F) probabilitySlider_a_RK = 100.0F;
		if (probabilitySlider_a_RC > 100.0F) probabilitySlider_a_RC = 100.0F;
		if (probabilitySlider_a_RD > 100.0F) probabilitySlider_a_RD = 100.0F;

		if (probabilitySlider_r_RR > 100.0F) probabilitySlider_r_RR = 100.0F;
		if (probabilitySlider_r_RG > 100.0F) probabilitySlider_r_RG = 100.0F;
		if (probabilitySlider_r_RB > 100.0F) probabilitySlider_r_RB = 100.0F;
		if (probabilitySlider_r_RW > 100.0F) probabilitySlider_r_RW = 100.0F;
		if (probabilitySlider_r_RO > 100.0F) probabilitySlider_r_RO = 100.0F;
		if (probabilitySlider_r_RK > 100.0F) probabilitySlider_r_RK = 100.0F;
		if (probabilitySlider_r_RC > 100.0F) probabilitySlider_r_RC = 100.0F;
		if (probabilitySlider_r_RD > 100.0F) probabilitySlider_r_RD = 100.0F;

		if (probabilitySlider_a_GR > 100.0F) probabilitySlider_a_GR = 100.0F;
		if (probabilitySlider_a_GG > 100.0F) probabilitySlider_a_GG = 100.0F;
		if (probabilitySlider_a_GB > 100.0F) probabilitySlider_a_GB = 100.0F;
		if (probabilitySlider_a_GW > 100.0F) probabilitySlider_a_GW = 100.0F;
		if (probabilitySlider_a_GO > 100.0F) probabilitySlider_a_GO = 100.0F;
		if (probabilitySlider_a_GK > 100.0F) probabilitySlider_a_GK = 100.0F;
		if (probabilitySlider_a_GC > 100.0F) probabilitySlider_a_GC = 100.0F;
		if (probabilitySlider_a_GD > 100.0F) probabilitySlider_a_GD = 100.0F;

		if (probabilitySlider_r_GR > 100.0F) probabilitySlider_r_GR = 100.0F;
		if (probabilitySlider_r_GG > 100.0F) probabilitySlider_r_GG = 100.0F;
		if (probabilitySlider_r_GB > 100.0F) probabilitySlider_r_GB = 100.0F;
		if (probabilitySlider_r_GW > 100.0F) probabilitySlider_r_GW = 100.0F;
		if (probabilitySlider_r_GO > 100.0F) probabilitySlider_r_GO = 100.0F;
		if (probabilitySlider_r_GK > 100.0F) probabilitySlider_r_GK = 100.0F;
		if (probabilitySlider_r_GC > 100.0F) probabilitySlider_r_GC = 100.0F;
		if (probabilitySlider_r_GD > 100.0F) probabilitySlider_r_GD = 100.0F;

		if (probabilitySlider_a_BR > 100.0F) probabilitySlider_a_BR = 100.0F;
		if (probabilitySlider_a_BG > 100.0F) probabilitySlider_a_BG = 100.0F;
		if (probabilitySlider_a_BB > 100.0F) probabilitySlider_a_BB = 100.0F;
		if (probabilitySlider_a_BW > 100.0F) probabilitySlider_a_BW = 100.0F;
		if (probabilitySlider_a_BO > 100.0F) probabilitySlider_a_BO = 100.0F;
		if (probabilitySlider_a_BK > 100.0F) probabilitySlider_a_BK = 100.0F;
		if (probabilitySlider_a_BC > 100.0F) probabilitySlider_a_BC = 100.0F;
		if (probabilitySlider_a_BD > 100.0F) probabilitySlider_a_BD = 100.0F;

		if (probabilitySlider_r_BR > 100.0F) probabilitySlider_r_BR = 100.0F;
		if (probabilitySlider_r_BG > 100.0F) probabilitySlider_r_BG = 100.0F;
		if (probabilitySlider_r_BB > 100.0F) probabilitySlider_r_BB = 100.0F;
		if (probabilitySlider_r_BW > 100.0F) probabilitySlider_r_BW = 100.0F;
		if (probabilitySlider_r_BO > 100.0F) probabilitySlider_r_BO = 100.0F;
		if (probabilitySlider_r_BK > 100.0F) probabilitySlider_r_BK = 100.0F;
		if (probabilitySlider_r_BC > 100.0F) probabilitySlider_r_BC = 100.0F;
		if (probabilitySlider_r_BD > 100.0F) probabilitySlider_r_BD = 100.0F;

		if (probabilitySlider_a_WR > 100.0F) probabilitySlider_a_WR = 100.0F;
		if (probabilitySlider_a_WG > 100.0F) probabilitySlider_a_WG = 100.0F;
		if (probabilitySlider_a_WB > 100.0F) probabilitySlider_a_WB = 100.0F;
		if (probabilitySlider_a_WW > 100.0F) probabilitySlider_a_WW = 100.0F;
		if (probabilitySlider_a_WO > 100.0F) probabilitySlider_a_WO = 100.0F;
		if (probabilitySlider_a_WK > 100.0F) probabilitySlider_a_WK = 100.0F;
		if (probabilitySlider_a_WC > 100.0F) probabilitySlider_a_WC = 100.0F;
		if (probabilitySlider_a_WD > 100.0F) probabilitySlider_a_WD = 100.0F;

		if (probabilitySlider_r_WR > 100.0F) probabilitySlider_r_WR = 100.0F;
		if (probabilitySlider_r_WG > 100.0F) probabilitySlider_r_WG = 100.0F;
		if (probabilitySlider_r_WB > 100.0F) probabilitySlider_r_WB = 100.0F;
		if (probabilitySlider_r_WW > 100.0F) probabilitySlider_r_WW = 100.0F;
		if (probabilitySlider_r_WO > 100.0F) probabilitySlider_r_WO = 100.0F;
		if (probabilitySlider_r_WK > 100.0F) probabilitySlider_r_WK = 100.0F;
		if (probabilitySlider_r_WC > 100.0F) probabilitySlider_r_WC = 100.0F;
		if (probabilitySlider_r_WD > 100.0F) probabilitySlider_r_WD = 100.0F;

		if (probabilitySlider_a_OR > 100.0F) probabilitySlider_a_OR = 100.0F;
		if (probabilitySlider_a_OG > 100.0F) probabilitySlider_a_OG = 100.0F;
		if (probabilitySlider_a_OB > 100.0F) probabilitySlider_a_OB = 100.0F;
		if (probabilitySlider_a_OW > 100.0F) probabilitySlider_a_OW = 100.0F;
		if (probabilitySlider_a_OO > 100.0F) probabilitySlider_a_OO = 100.0F;
		if (probabilitySlider_a_OK > 100.0F) probabilitySlider_a_OK = 100.0F;
		if (probabilitySlider_a_OC > 100.0F) probabilitySlider_a_OC = 100.0F;
		if (probabilitySlider_a_OD > 100.0F) probabilitySlider_a_OD = 100.0F;

		if (probabilitySlider_r_OR > 100.0F) probabilitySlider_r_OR = 100.0F;
		if (probabilitySlider_r_OG > 100.0F) probabilitySlider_r_OG = 100.0F;
		if (probabilitySlider_r_OB > 100.0F) probabilitySlider_r_OB = 100.0F;
		if (probabilitySlider_r_OW > 100.0F) probabilitySlider_r_OW = 100.0F;
		if (probabilitySlider_r_OO > 100.0F) probabilitySlider_r_OO = 100.0F;
		if (probabilitySlider_r_OK > 100.0F) probabilitySlider_r_OK = 100.0F;
		if (probabilitySlider_r_OC > 100.0F) probabilitySlider_r_OC = 100.0F;
		if (probabilitySlider_r_OD > 100.0F) probabilitySlider_r_OD = 100.0F;

		if (probabilitySlider_a_KR > 100.0F) probabilitySlider_a_KR = 100.0F;
		if (probabilitySlider_a_KG > 100.0F) probabilitySlider_a_KG = 100.0F;
		if (probabilitySlider_a_KB > 100.0F) probabilitySlider_a_KB = 100.0F;
		if (probabilitySlider_a_KW > 100.0F) probabilitySlider_a_KW = 100.0F;
		if (probabilitySlider_a_KO > 100.0F) probabilitySlider_a_KO = 100.0F;
		if (probabilitySlider_a_KK > 100.0F) probabilitySlider_a_KK = 100.0F;
		if (probabilitySlider_a_KC > 100.0F) probabilitySlider_a_KC = 100.0F;
		if (probabilitySlider_a_KD > 100.0F) probabilitySlider_a_KD = 100.0F;

		if (probabilitySlider_r_KR > 100.0F) probabilitySlider_r_KR = 100.0F;
		if (probabilitySlider_r_KG > 100.0F) probabilitySlider_r_KG = 100.0F;
		if (probabilitySlider_r_KB > 100.0F) probabilitySlider_r_KB = 100.0F;
		if (probabilitySlider_r_KW > 100.0F) probabilitySlider_r_KW = 100.0F;
		if (probabilitySlider_r_KO > 100.0F) probabilitySlider_r_KO = 100.0F;
		if (probabilitySlider_r_KK > 100.0F) probabilitySlider_r_KK = 100.0F;
		if (probabilitySlider_r_KC > 100.0F) probabilitySlider_r_KC = 100.0F;
		if (probabilitySlider_r_KD > 100.0F) probabilitySlider_r_KD = 100.0F;

		if (probabilitySlider_a_CR > 100.0F) probabilitySlider_a_CR = 100.0F;
		if (probabilitySlider_a_CG > 100.0F) probabilitySlider_a_CG = 100.0F;
		if (probabilitySlider_a_CB > 100.0F) probabilitySlider_a_CB = 100.0F;
		if (probabilitySlider_a_CW > 100.0F) probabilitySlider_a_CW = 100.0F;
		if (probabilitySlider_a_CO > 100.0F) probabilitySlider_a_CO = 100.0F;
		if (probabilitySlider_a_CK > 100.0F) probabilitySlider_a_CK = 100.0F;
		if (probabilitySlider_a_CC > 100.0F) probabilitySlider_a_CC = 100.0F;
		if (probabilitySlider_a_CD > 100.0F) probabilitySlider_a_CD = 100.0F;

		if (probabilitySlider_r_CR > 100.0F) probabilitySlider_r_CR = 100.0F;
		if (probabilitySlider_r_CG > 100.0F) probabilitySlider_r_CG = 100.0F;
		if (probabilitySlider_r_CB > 100.0F) probabilitySlider_r_CB = 100.0F;
		if (probabilitySlider_r_CW > 100.0F) probabilitySlider_r_CW = 100.0F;
		if (probabilitySlider_r_CO > 100.0F) probabilitySlider_r_CO = 100.0F;
		if (probabilitySlider_r_CK > 100.0F) probabilitySlider_r_CK = 100.0F;
		if (probabilitySlider_r_CC > 100.0F) probabilitySlider_r_CC = 100.0F;
		if (probabilitySlider_r_CD > 100.0F) probabilitySlider_r_CD = 100.0F;

		if (probabilitySlider_a_DR > 100.0F) probabilitySlider_a_DR = 100.0F;
		if (probabilitySlider_a_DG > 100.0F) probabilitySlider_a_DG = 100.0F;
		if (probabilitySlider_a_DB > 100.0F) probabilitySlider_a_DB = 100.0F;
		if (probabilitySlider_a_DW > 100.0F) probabilitySlider_a_DW = 100.0F;
		if (probabilitySlider_a_DO > 100.0F) probabilitySlider_a_DO = 100.0F;
		if (probabilitySlider_a_DK > 100.0F) probabilitySlider_a_DK = 100.0F;
		if (probabilitySlider_a_DC > 100.0F) probabilitySlider_a_DC = 100.0F;
		if (probabilitySlider_a_DD > 100.0F) probabilitySlider_a_DD = 100.0F;

		if (probabilitySlider_r_DR > 100.0F) probabilitySlider_r_DR = 100.0F;
		if (probabilitySlider_r_DG > 100.0F) probabilitySlider_r_DG = 100.0F;
		if (probabilitySlider_r_DB > 100.0F) probabilitySlider_r_DB = 100.0F;
		if (probabilitySlider_r_DW > 100.0F) probabilitySlider_r_DW = 100.0F;
		if (probabilitySlider_r_DO > 100.0F) probabilitySlider_r_DO = 100.0F;
		if (probabilitySlider_r_DK > 100.0F) probabilitySlider_r_DK = 100.0F;
		if (probabilitySlider_r_DC > 100.0F) probabilitySlider_r_DC = 100.0F;
		if (probabilitySlider_r_DD > 100.0F) probabilitySlider_r_DD = 100.0F;

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
		interaction(&red, &red, powerSlider_a_RR, vSlider_a_RR, probability_a_RR, powerSlider_r_RR, vSlider_r_RR, probability_r_RR, viscosityR);
		if (numberSliderG > 0) 
			interaction(&red, &green, powerSlider_a_RG, vSlider_a_RG, probability_a_RG, powerSlider_r_RG, vSlider_r_RG, probability_r_RG, viscosityR);
		if (numberSliderB > 0) 
			interaction(&red, &blue, powerSlider_a_RB, vSlider_a_RB, probability_a_RB, powerSlider_r_RB, vSlider_r_RB, probability_r_RB, viscosityR);
		if (numberSliderW > 0) 
			interaction(&red, &white, powerSlider_a_RW, vSlider_a_RW, probability_a_RW, powerSlider_r_RW, vSlider_r_RW, probability_r_RW, viscosityR);
		if (numberSliderO > 0) 
			interaction(&red, &orange, powerSlider_a_RO, vSlider_a_RO, probability_a_RO, powerSlider_r_RO, vSlider_r_RO, probability_r_RO, viscosityR);
		if (numberSliderK > 0) 
			interaction(&red, &khaki, powerSlider_a_RK, vSlider_a_RK, probability_a_RK, powerSlider_r_RK, vSlider_r_RK, probability_r_RK, viscosityR);
		if (numberSliderC > 0) 
			interaction(&red, &crimson, powerSlider_a_RC, vSlider_a_RC, probability_a_RC, powerSlider_r_RC, vSlider_r_RC, probability_r_RC, viscosityR);
		if (numberSliderD > 0) 
			interaction(&red, &dark, powerSlider_a_RD, vSlider_a_RD, probability_a_RD, powerSlider_r_RD, vSlider_r_RD, probability_r_RD, viscosityR);
	}

	if (numberSliderG > 0)
	{
		interaction(&green, &green, powerSlider_a_GG, vSlider_a_GG, probability_a_GG, powerSlider_r_GG, vSlider_r_GG, probability_r_GG, viscosityG);
		if (numberSliderR > 0)
			interaction(&green, &red, powerSlider_a_GR, vSlider_a_GR, probability_a_GR, powerSlider_r_GR, vSlider_r_GR, probability_r_GR, viscosityG);
		if (numberSliderB > 0)
			interaction(&green, &blue, powerSlider_a_GB, vSlider_a_GB, probability_a_GB, powerSlider_r_GB, vSlider_r_GB, probability_r_GB, viscosityG);
		if (numberSliderW > 0)
			interaction(&green, &white, powerSlider_a_GW, vSlider_a_GW, probability_a_GW, powerSlider_r_GW, vSlider_r_GW, probability_r_GW, viscosityG);
		if (numberSliderO > 0)
			interaction(&green, &orange, powerSlider_a_GO, vSlider_a_GO, probability_a_GO, powerSlider_r_GO, vSlider_r_GO, probability_r_GO, viscosityG);
		if (numberSliderK > 0)
			interaction(&green, &khaki, powerSlider_a_GK, vSlider_a_GK, probability_a_GK, powerSlider_r_GK, vSlider_r_GK, probability_r_GK, viscosityG);
		if (numberSliderC > 0)
			interaction(&green, &crimson, powerSlider_a_GC, vSlider_a_GC, probability_a_GC, powerSlider_r_GC, vSlider_r_GC, probability_r_GC, viscosityG);
		if (numberSliderD > 0)
			interaction(&green, &dark, powerSlider_a_GD, vSlider_a_GD, probability_a_GD, powerSlider_r_GD, vSlider_r_GD, probability_r_GD, viscosityG);
	}

	if (numberSliderB > 0)
	{
		interaction(&blue, &blue, powerSlider_a_BB, vSlider_a_BB, probability_a_BB, powerSlider_r_BB, vSlider_r_BB, probability_r_BB, viscosityB);
		if (numberSliderR > 0)
			interaction(&blue, &red, powerSlider_a_BR, vSlider_a_BR, probability_a_BR, powerSlider_r_BR, vSlider_r_BR, probability_r_BR, viscosityB);
		if (numberSliderG > 0)
			interaction(&blue, &green, powerSlider_a_BG, vSlider_a_BG, probability_a_BG, powerSlider_r_BG, vSlider_r_BG, probability_r_BG, viscosityB);
		if (numberSliderW > 0)
			interaction(&blue, &white, powerSlider_a_BW, vSlider_a_BW, probability_a_BW, powerSlider_r_BW, vSlider_r_BW, probability_r_BW, viscosityB);
		if (numberSliderO > 0)
			interaction(&blue, &orange, powerSlider_a_BO, vSlider_a_BO, probability_a_BO, powerSlider_r_BO, vSlider_r_BO, probability_r_BO, viscosityB);
		if (numberSliderK > 0)
			interaction(&blue, &khaki, powerSlider_a_BK, vSlider_a_BK, probability_a_BK, powerSlider_r_BK, vSlider_r_BK, probability_r_BK, viscosityB);
		if (numberSliderC > 0)
			interaction(&blue, &crimson, powerSlider_a_BC, vSlider_a_BC, probability_a_BC, powerSlider_r_BC, vSlider_r_BC, probability_r_BC, viscosityB);
		if (numberSliderD > 0)
			interaction(&blue, &dark, powerSlider_a_BD, vSlider_a_BD, probability_a_BD, powerSlider_r_BD, vSlider_r_BD, probability_r_BD, viscosityB);
	}

	if (numberSliderW > 0)
	{
		interaction(&white, &white, powerSlider_a_WW, vSlider_a_WW, probability_a_WW, powerSlider_r_WW, vSlider_r_WW, probability_r_WW, viscosityW);
		if (numberSliderR > 0)
			interaction(&white, &red, powerSlider_a_WR, vSlider_a_WR, probability_a_WR, powerSlider_r_WR, vSlider_r_WR, probability_r_WR, viscosityW);
		if (numberSliderG > 0)
			interaction(&white, &green, powerSlider_a_WG, vSlider_a_WG, probability_a_WG, powerSlider_r_WG, vSlider_r_WG, probability_r_WG, viscosityW);
		if (numberSliderB > 0)
			interaction(&white, &blue, powerSlider_a_WB, vSlider_a_WB, probability_a_WB, powerSlider_r_WB, vSlider_r_WB, probability_r_WB, viscosityW);
		if (numberSliderO > 0)
			interaction(&white, &orange, powerSlider_a_WO, vSlider_a_WO, probability_a_WO, powerSlider_r_WO, vSlider_r_WO, probability_r_WO, viscosityW);
		if (numberSliderK > 0)
			interaction(&white, &khaki, powerSlider_a_WK, vSlider_a_WK, probability_a_WK, powerSlider_r_WK, vSlider_r_WK, probability_r_WK, viscosityW);
		if (numberSliderC > 0)
			interaction(&white, &crimson, powerSlider_a_WC, vSlider_a_WC, probability_a_WC, powerSlider_r_WC, vSlider_r_WC, probability_r_WC, viscosityW);
		if (numberSliderD > 0)
			interaction(&white, &dark, powerSlider_a_WD, vSlider_a_WD, probability_a_WD, powerSlider_r_WD, vSlider_r_WD, probability_r_WD, viscosityW);
	}

	if (numberSliderO > 0)
	{
		interaction(&orange, &orange, powerSlider_a_OO, vSlider_a_OO, probability_a_OO, powerSlider_r_OO, vSlider_r_OO, probability_r_OO, viscosityO);
		if (numberSliderR > 0)
			interaction(&orange, &red, powerSlider_a_OR, vSlider_a_OR, probability_a_OR, powerSlider_r_OR, vSlider_r_OR, probability_r_OR, viscosityO);
		if (numberSliderG > 0)
			interaction(&orange, &green, powerSlider_a_OG, vSlider_a_OG, probability_a_OG, powerSlider_r_OG, vSlider_r_OG, probability_r_OG, viscosityO);
		if (numberSliderB > 0)
			interaction(&orange, &blue, powerSlider_a_OB, vSlider_a_OB, probability_a_OB, powerSlider_r_OB, vSlider_r_OB, probability_r_OB, viscosityO);
		if (numberSliderW > 0)
			interaction(&orange, &white, powerSlider_a_OW, vSlider_a_OW, probability_a_OW, powerSlider_r_OW, vSlider_r_OW, probability_r_OW, viscosityO);
		if (numberSliderK > 0)
			interaction(&orange, &khaki, powerSlider_a_OK, vSlider_a_OK, probability_a_OK, powerSlider_r_OK, vSlider_r_OK, probability_r_OK, viscosityO);
		if (numberSliderC > 0)
			interaction(&orange, &crimson, powerSlider_a_OC, vSlider_a_OC, probability_a_OC, powerSlider_r_OC, vSlider_r_OC, probability_r_OC, viscosityO);
		if (numberSliderD > 0)
			interaction(&orange, &dark, powerSlider_a_OD, vSlider_a_OD, probability_a_OD, powerSlider_r_OD, vSlider_r_OD, probability_r_OD, viscosityO);
	}

	if (numberSliderK > 0)
	{
		interaction(&khaki, &khaki, powerSlider_a_KK, vSlider_a_KK, probability_a_KK, powerSlider_r_KK, vSlider_r_KK, probability_r_KK, viscosityK);
		if (numberSliderR > 0)
			interaction(&khaki, &red, powerSlider_a_KR, vSlider_a_KR, probability_a_KR, powerSlider_r_KR, vSlider_r_KR, probability_r_KR, viscosityK);
		if (numberSliderG > 0)
			interaction(&khaki, &green, powerSlider_a_KG, vSlider_a_KG, probability_a_KG, powerSlider_r_KG, vSlider_r_KG, probability_r_KG, viscosityK);
		if (numberSliderB > 0)
			interaction(&khaki, &blue, powerSlider_a_KB, vSlider_a_KB, probability_a_KB, powerSlider_r_KB, vSlider_r_KB, probability_r_KB, viscosityK);
		if (numberSliderW > 0)
			interaction(&khaki, &white, powerSlider_a_KW, vSlider_a_KW, probability_a_KW, powerSlider_r_KW, vSlider_r_KW, probability_r_KW, viscosityK);
		if (numberSliderO > 0)
			interaction(&khaki, &orange, powerSlider_a_KO, vSlider_a_KO, probability_a_KO, powerSlider_r_KO, vSlider_r_KO, probability_r_KO, viscosityK);
		if (numberSliderC > 0)
			interaction(&khaki, &crimson, powerSlider_a_KC, vSlider_a_KC, probability_a_KC, powerSlider_r_KC, vSlider_r_KC, probability_r_KC, viscosityK);
		if (numberSliderD > 0)
			interaction(&khaki, &dark, powerSlider_a_KD, vSlider_a_KD, probability_a_KD, powerSlider_r_KD, vSlider_r_KD, probability_r_KD, viscosityK);
	}

	if (numberSliderC > 0)
	{
		interaction(&crimson, &crimson, powerSlider_a_CC, vSlider_a_CC, probability_a_CC, powerSlider_r_CC, vSlider_r_CC, probability_r_CC, viscosityC);
		if (numberSliderR > 0)
			interaction(&crimson, &red, powerSlider_a_CR, vSlider_a_CR, probability_a_CR, powerSlider_r_CR, vSlider_r_CR, probability_r_CR, viscosityC);
		if (numberSliderG > 0)
			interaction(&crimson, &green, powerSlider_a_CG, vSlider_a_CG, probability_a_CG, powerSlider_r_CG, vSlider_r_CG, probability_r_CG, viscosityC);
		if (numberSliderB > 0)
			interaction(&crimson, &blue, powerSlider_a_CB, vSlider_a_CB, probability_a_CB, powerSlider_r_CB, vSlider_r_CB, probability_r_CB, viscosityC);
		if (numberSliderW > 0)
			interaction(&crimson, &white, powerSlider_a_CW, vSlider_a_CW, probability_a_CW, powerSlider_r_CW, vSlider_r_CW, probability_r_CW, viscosityC);
		if (numberSliderO > 0)
			interaction(&crimson, &orange, powerSlider_a_CO, vSlider_a_CO, probability_a_CO, powerSlider_r_CO, vSlider_r_CO, probability_r_CO, viscosityC);
		if (numberSliderK > 0)
			interaction(&crimson, &khaki, powerSlider_a_CK, vSlider_a_CK, probability_a_CK, powerSlider_r_CK, vSlider_r_CK, probability_r_CK, viscosityC);
		if (numberSliderD > 0)
			interaction(&crimson, &dark, powerSlider_a_CD, vSlider_a_CD, probability_a_CD, powerSlider_r_CD, vSlider_r_CD, probability_r_CD, viscosityC);
	}

	if (numberSliderD > 0)
	{
		interaction(&dark, &dark, powerSlider_a_DD, vSlider_a_DD, probability_a_DD, powerSlider_r_DD, vSlider_r_DD, probability_r_DD, viscosityD);
		if (numberSliderR > 0)
			interaction(&dark, &red, powerSlider_a_DR, vSlider_a_DR, probability_a_DR, powerSlider_r_DR, vSlider_r_DR, probability_r_DR, viscosityD);
		if (numberSliderG > 0)
			interaction(&dark, &green, powerSlider_a_DG, vSlider_a_DG, probability_a_DG, powerSlider_r_DG, vSlider_r_DG, probability_r_DG, viscosityD);
		if (numberSliderB > 0)
			interaction(&dark, &blue, powerSlider_a_DB, vSlider_a_DB, probability_a_DB, powerSlider_r_DB, vSlider_r_DB, probability_r_DB, viscosityD);
		if (numberSliderW > 0)
			interaction(&dark, &white, powerSlider_a_DW, vSlider_a_DW, probability_a_DW, powerSlider_r_DW, vSlider_r_DW, probability_r_DW, viscosityD);
		if (numberSliderO > 0)
			interaction(&dark, &orange, powerSlider_a_DO, vSlider_a_DO, probability_a_DO, powerSlider_r_DO, vSlider_r_DO, probability_r_DO, viscosityD);
		if (numberSliderK > 0)
			interaction(&dark, &khaki, powerSlider_a_DK, vSlider_a_DK, probability_a_DK, powerSlider_r_DK, vSlider_r_DK, probability_r_DK, viscosityD);
		if (numberSliderC > 0)
			interaction(&dark, &crimson, powerSlider_a_DC, vSlider_a_DC, probability_a_DC, powerSlider_r_DC, vSlider_r_DC, probability_r_DC, viscosityD);
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
		restart();
	}
	if (randomProb)
	{
		rndprob();
		restart();
	}
	if (randomChoice)
	{
		rndir();
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
	//if (modelToggle == true)
	//{
	//	ofSetColor(0, 0, 0);
	//	ofDrawCircle(xshift, yshift, 150);
	//	ofSetLineWidth(5);
	//	ofSetColor(150.0F - powerSliderGR, 150.0F + powerSliderGR, 150);
	//	ofDrawLine(p1x, p1y - 10, p2x, p2y - 10);
	//	ofSetColor(150.0F - powerSliderRG, 150.0F + powerSliderRG, 150);
	//	ofDrawLine(p1x, p1y + 10, p2x, p2y + 10);
	//	ofSetColor(150.0F - powerSliderGW, 150.0F + powerSliderGW, 150);
	//	ofDrawLine(p3x, p3y - 10, p1x, p1y - 10);
	//	ofSetColor(150.0F - powerSliderWG, 150.0F + powerSliderWG, 150);
	//	ofDrawLine(p3x, p3y + 10, p1x, p1y + 10);
	//	ofSetColor(150.0F - powerSliderGB, 150.0F + powerSliderGB, 150);
	//	ofDrawLine(p4x - 10, p4y, p1x - 10, p1y);
	//	ofSetColor(150.0F - powerSliderBG, 150.0F + powerSliderBG, 150);
	//	ofDrawLine(p4x + 10, p4y, p1x + 10, p1y);
	//	ofSetColor(150.0F - powerSliderRW, 150.0F + powerSliderRW, 150);
	//	ofDrawLine(p2x - 10, p2y, p3x - 10, p3y);
	//	ofSetColor(150.0F - powerSliderWR, 150.0F + powerSliderWR, 150);
	//	ofDrawLine(p2x + 10, p2y, p3x + 10, p3y);
	//	ofSetColor(150.0F - powerSliderRB, 150.0F + powerSliderRB, 150);
	//	ofDrawLine(p2x, p2y - 10, p4x, p4y - 10);
	//	ofSetColor(150.0F - powerSliderBR, 150.0F + powerSliderBR, 150);
	//	ofDrawLine(p2x, p2y + 10, p4x, p4y + 10);
	//	ofSetColor(150.0F - powerSliderWB, 150.0F + powerSliderWB, 150);
	//	ofDrawLine(p3x, p3y - 10, p4x, p4y - 10);
	//	ofSetColor(150.0F - powerSliderBW, 150.0F + powerSliderBW, 150);
	//	ofDrawLine(p3x, p3y + 10, p4x, p4y + 10);
	//	ofNoFill();
	//	ofSetColor(150.0F - powerSliderGG, 150.0F + powerSliderGG, 150);
	//	ofDrawCircle(p1x - 20, p1y - 20, rr + 20);
	//	ofSetColor(150.0F - powerSliderRR, 150.0F + powerSliderRR, 150);
	//	ofDrawCircle(p2x + 20, p2y - 20, rr + 20);
	//	ofSetColor(150.0F - powerSliderWW, 150.0F + powerSliderWW, 150);
	//	ofDrawCircle(p3x + 20, p3y + 20, rr + 20);
	//	ofSetColor(150.0F - powerSliderBB, 150.0F + powerSliderBB, 150);
	//	ofDrawCircle(p4x - 20, p4y + 20, rr + 20);

	//	ofFill();
	//	ofSetColor(100, 250, 10);
	//	ofDrawCircle(p1x, p1y, rr);
	//	ofSetColor(250, 10, 100);
	//	ofDrawCircle(p2x, p2y, rr);
	//	ofSetColor(250, 250, 250);
	//	ofDrawCircle(p3x, p3y, rr);
	//	ofSetColor(100, 100, 250);
	//	ofDrawCircle(p4x, p4y, rr);
	//}
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
