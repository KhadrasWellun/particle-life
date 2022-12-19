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
float minP = -200;
float maxP = 200;
float minR = 0;
float maxR = 500;
clock_t now, lastTime, delta;
clock_t physic_begin, physic_delta;

//Particle groups by color
std::vector<point> alpha;
std::vector<point> beta;
std::vector<point> elta;
std::vector<point> gamma;
std::vector<point> epsilon;
std::vector<point> zeta;
std::vector<point> eta;


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
	if (numberSliderα > 0) { alpha = CreatePoints(numberSliderα, 199, 36, 177); }
	if (numberSliderβ > 0) { beta = CreatePoints(numberSliderβ, 77, 77, 255); }
	if (numberSliderγ > 0) { gamma = CreatePoints(numberSliderγ, 224, 231, 34); }
	if (numberSliderδ > 0) { elta = CreatePoints(numberSliderδ, 255, 173, 0); }
	if (numberSliderε > 0) { epsilon = CreatePoints(numberSliderε, 210, 39, 48); }
	if (numberSliderζ > 0) { zeta = CreatePoints(numberSliderζ, 219, 62, 177); }
	if (numberSliderη > 0) { eta = CreatePoints(numberSliderη, 68, 214, 44); }
}


/**
 * @brief Generate initial simulation parameters
 */
void ofApp::random()
{
	InteractionEvoProbSlider = RandomFloat(0.1, 1.5);
	InteractionEvoAmountSlider = RandomFloat(0.1, 3);
	ProbabilityEvoProbSlider = RandomFloat(0.1, 1.5);
	ProbabilityEvoAmountSlider = RandomFloat(0.1, 3);
	ViscosityEvoProbSlider = RandomFloat(0.1, 1.5);
	ViscosityEvoAmountSlider = RandomFloat(0.1, 3);

	numberSliderβ = RandomInt(500, 2500);
	numberSliderα = RandomInt(500, 2500);
	numberSliderδ = RandomInt(500, 2500);
	numberSliderγ = RandomInt(500, 2500);
	numberSliderε = RandomInt(500, 2500);
	numberSliderζ = RandomInt(500, 2500);
	numberSliderη = RandomInt(500, 2500);

	viscositySlider = RandomFloat(0.0, 1.0);

	viscositySliderαα = RandomFloat(0.0, 1.0);
	viscositySliderαβ = RandomFloat(0.0, 1.0);
	viscositySliderαδ = RandomFloat(0.0, 1.0);
	viscositySliderαγ = RandomFloat(0.0, 1.0);
	viscositySliderαε = RandomFloat(0.0, 1.0);
	viscositySliderαζ = RandomFloat(0.0, 1.0);
	viscositySliderαη = RandomFloat(0.0, 1.0);

	viscositySliderβα = RandomFloat(0.0, 1.0);
	viscositySliderββ = RandomFloat(0.0, 1.0);
	viscositySliderβδ = RandomFloat(0.0, 1.0);
	viscositySliderβγ = RandomFloat(0.0, 1.0);
	viscositySliderβε = RandomFloat(0.0, 1.0);
	viscositySliderβζ = RandomFloat(0.0, 1.0);
	viscositySliderβη = RandomFloat(0.0, 1.0);

	viscositySliderγα = RandomFloat(0.0, 1.0);
	viscositySliderγβ = RandomFloat(0.0, 1.0);
	viscositySliderγδ = RandomFloat(0.0, 1.0);
	viscositySliderγγ = RandomFloat(0.0, 1.0);
	viscositySliderγε = RandomFloat(0.0, 1.0);
	viscositySliderγζ = RandomFloat(0.0, 1.0);
	viscositySliderγη = RandomFloat(0.0, 1.0);

	viscositySliderδα = RandomFloat(0.0, 1.0);
	viscositySliderδβ = RandomFloat(0.0, 1.0);
	viscositySliderδδ = RandomFloat(0.0, 1.0);
	viscositySliderδγ = RandomFloat(0.0, 1.0);
	viscositySliderδε = RandomFloat(0.0, 1.0);
	viscositySliderδζ = RandomFloat(0.0, 1.0);
	viscositySliderδη = RandomFloat(0.0, 1.0);

	viscositySliderεα = RandomFloat(0.0, 1.0);
	viscositySliderεβ = RandomFloat(0.0, 1.0);
	viscositySliderεδ = RandomFloat(0.0, 1.0);
	viscositySliderεγ = RandomFloat(0.0, 1.0);
	viscositySliderεε = RandomFloat(0.0, 1.0);
	viscositySliderεζ = RandomFloat(0.0, 1.0);
	viscositySliderεη = RandomFloat(0.0, 1.0);

	viscositySliderζα = RandomFloat(0.0, 1.0);
	viscositySliderζβ = RandomFloat(0.0, 1.0);
	viscositySliderζδ = RandomFloat(0.0, 1.0);
	viscositySliderζγ = RandomFloat(0.0, 1.0);
	viscositySliderζε = RandomFloat(0.0, 1.0);
	viscositySliderζζ = RandomFloat(0.0, 1.0);
	viscositySliderζη = RandomFloat(0.0, 1.0);

	viscositySliderηα = RandomFloat(0.0, 1.0);
	viscositySliderηβ = RandomFloat(0.0, 1.0);
	viscositySliderηδ = RandomFloat(0.0, 1.0);
	viscositySliderηγ = RandomFloat(0.0, 1.0);
	viscositySliderηε = RandomFloat(0.0, 1.0);
	viscositySliderηζ = RandomFloat(0.0, 1.0);
	viscositySliderηη = RandomFloat(0.0, 1.0);

	probabilitySlider = RandomFloat(0, 100);

	probabilitySliderαα = RandomFloat(0, 100);
	probabilitySliderαβ = RandomFloat(0, 100);
	probabilitySliderαγ = RandomFloat(0, 100);
	probabilitySliderαδ = RandomFloat(0, 100);
	probabilitySliderαε = RandomFloat(0, 100);
	probabilitySliderαζ = RandomFloat(0, 100);
	probabilitySliderαη = RandomFloat(0, 100);

	probabilitySliderβα = RandomFloat(0, 100);
	probabilitySliderββ = RandomFloat(0, 100);
	probabilitySliderβγ = RandomFloat(0, 100);
	probabilitySliderβδ = RandomFloat(0, 100);
	probabilitySliderβε = RandomFloat(0, 100);
	probabilitySliderβζ = RandomFloat(0, 100);
	probabilitySliderβη = RandomFloat(0, 100);

	probabilitySliderγα = RandomFloat(0, 100);
	probabilitySliderγβ = RandomFloat(0, 100);
	probabilitySliderγγ = RandomFloat(0, 100);
	probabilitySliderγδ = RandomFloat(0, 100);
	probabilitySliderγε = RandomFloat(0, 100);
	probabilitySliderγζ = RandomFloat(0, 100);
	probabilitySliderγη = RandomFloat(0, 100);

	probabilitySliderδα = RandomFloat(0, 100);
	probabilitySliderδβ = RandomFloat(0, 100);
	probabilitySliderδγ = RandomFloat(0, 100);
	probabilitySliderδδ = RandomFloat(0, 100);
	probabilitySliderδε = RandomFloat(0, 100);
	probabilitySliderδζ = RandomFloat(0, 100);
	probabilitySliderδη = RandomFloat(0, 100);

	probabilitySliderεα = RandomFloat(0, 100);
	probabilitySliderεβ = RandomFloat(0, 100);
	probabilitySliderεγ = RandomFloat(0, 100);
	probabilitySliderεδ = RandomFloat(0, 100);
	probabilitySliderεε = RandomFloat(0, 100);
	probabilitySliderεζ = RandomFloat(0, 100);
	probabilitySliderεη = RandomFloat(0, 100);

	probabilitySliderζα = RandomFloat(0, 100);
	probabilitySliderζβ = RandomFloat(0, 100);
	probabilitySliderζγ = RandomFloat(0, 100);
	probabilitySliderζδ = RandomFloat(0, 100);
	probabilitySliderζε = RandomFloat(0, 100);
	probabilitySliderζζ = RandomFloat(0, 100);
	probabilitySliderζη = RandomFloat(0, 100);

	probabilitySliderηα = RandomFloat(0, 100);
	probabilitySliderηβ = RandomFloat(0, 100);
	probabilitySliderηγ = RandomFloat(0, 100);
	probabilitySliderηδ = RandomFloat(0, 100);
	probabilitySliderηε = RandomFloat(0, 100);
	probabilitySliderηζ = RandomFloat(0, 100);
	probabilitySliderηη = RandomFloat(0, 100);

	// βαEEN
	//numberSliderβ = RandomFloat(minR, 3000);
	powerSliderββ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβη = RandomFloat(minP, maxP) * forceVariance;

	vSliderββ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβη = RandomFloat(minR, maxR) * radiusVariance;

	// αED
	//numberSliderα = RandomFloat(minR, 3000);
	powerSliderαα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαη = RandomFloat(minP, maxP) * forceVariance;

	vSliderαβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαη = RandomFloat(minR, maxR) * radiusVariance;

	// δHITE
	// numberSliderδ = RandomFloat(minR, 3000);
	powerSliderδδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδη = RandomFloat(minP, maxP) * forceVariance;

	vSliderδβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδη = RandomFloat(minR, maxR) * radiusVariance;

	// γLUE
	//numberSliderγ = RandomFloat(minR, 3000);
	powerSliderγγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγη = RandomFloat(minP, maxP) * forceVariance;

	vSliderγβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγη = RandomFloat(minR, maxR) * radiusVariance;

	// εαANβE
	//numberSliderε = RandomFloat(minR, 3000);
	powerSliderεγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεη = RandomFloat(minP, maxP) * forceVariance;

	vSliderεβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεη = RandomFloat(minR, maxR) * radiusVariance;

	// ζHAζI
	//numberSliderζ = RandomFloat(minR, 3000);
	powerSliderζγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζη = RandomFloat(minP, maxP) * forceVariance;

	vSliderζβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζη = RandomFloat(minR, maxR) * radiusVariance;

	// ηαIMSεN
	//numberSliderη = RandomFloat(minR, 3000);
	powerSliderηγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηη = RandomFloat(minP, maxP) * forceVariance;

	vSliderηβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηη = RandomFloat(minR, maxR) * radiusVariance;
}

void ofApp::monads() {
	numberSliderβ = RandomInt(500, 2500);
	numberSliderα = RandomInt(500, 2500);
	numberSliderδ = RandomInt(500, 2500);
	numberSliderγ = RandomInt(500, 2500);
	numberSliderε = RandomInt(500, 2500);
	numberSliderζ = RandomInt(500, 2500);
	numberSliderη = RandomInt(500, 2500);
}
void ofApp::rndvsc() {
	viscositySlider = RandomFloat(0.0, 1.0);

	viscositySliderαα = RandomFloat(0.0, 1.0);
	viscositySliderαβ = RandomFloat(0.0, 1.0);
	viscositySliderαδ = RandomFloat(0.0, 1.0);
	viscositySliderαγ = RandomFloat(0.0, 1.0);
	viscositySliderαε = RandomFloat(0.0, 1.0);
	viscositySliderαζ = RandomFloat(0.0, 1.0);
	viscositySliderαη = RandomFloat(0.0, 1.0);

	viscositySliderβα = RandomFloat(0.0, 1.0);
	viscositySliderββ = RandomFloat(0.0, 1.0);
	viscositySliderβδ = RandomFloat(0.0, 1.0);
	viscositySliderβγ = RandomFloat(0.0, 1.0);
	viscositySliderβε = RandomFloat(0.0, 1.0);
	viscositySliderβζ = RandomFloat(0.0, 1.0);
	viscositySliderβη = RandomFloat(0.0, 1.0);

	viscositySliderγα = RandomFloat(0.0, 1.0);
	viscositySliderγβ = RandomFloat(0.0, 1.0);
	viscositySliderγδ = RandomFloat(0.0, 1.0);
	viscositySliderγγ = RandomFloat(0.0, 1.0);
	viscositySliderγε = RandomFloat(0.0, 1.0);
	viscositySliderγζ = RandomFloat(0.0, 1.0);
	viscositySliderγη = RandomFloat(0.0, 1.0);

	viscositySliderδα = RandomFloat(0.0, 1.0);
	viscositySliderδβ = RandomFloat(0.0, 1.0);
	viscositySliderδδ = RandomFloat(0.0, 1.0);
	viscositySliderδγ = RandomFloat(0.0, 1.0);
	viscositySliderδε = RandomFloat(0.0, 1.0);
	viscositySliderδζ = RandomFloat(0.0, 1.0);
	viscositySliderδη = RandomFloat(0.0, 1.0);

	viscositySliderεα = RandomFloat(0.0, 1.0);
	viscositySliderεβ = RandomFloat(0.0, 1.0);
	viscositySliderεδ = RandomFloat(0.0, 1.0);
	viscositySliderεγ = RandomFloat(0.0, 1.0);
	viscositySliderεε = RandomFloat(0.0, 1.0);
	viscositySliderεζ = RandomFloat(0.0, 1.0);
	viscositySliderεη = RandomFloat(0.0, 1.0);

	viscositySliderζα = RandomFloat(0.0, 1.0);
	viscositySliderζβ = RandomFloat(0.0, 1.0);
	viscositySliderζδ = RandomFloat(0.0, 1.0);
	viscositySliderζγ = RandomFloat(0.0, 1.0);
	viscositySliderζε = RandomFloat(0.0, 1.0);
	viscositySliderζζ = RandomFloat(0.0, 1.0);
	viscositySliderζη = RandomFloat(0.0, 1.0);

	viscositySliderηα = RandomFloat(0.0, 1.0);
	viscositySliderηβ = RandomFloat(0.0, 1.0);
	viscositySliderηδ = RandomFloat(0.0, 1.0);
	viscositySliderηγ = RandomFloat(0.0, 1.0);
	viscositySliderηε = RandomFloat(0.0, 1.0);
	viscositySliderηζ = RandomFloat(0.0, 1.0);
	viscositySliderηη = RandomFloat(0.0, 1.0);
}
void ofApp::rndprob() {
	probabilitySlider = RandomFloat(0, 100);

	probabilitySliderαα = RandomFloat(0, 100);
	probabilitySliderαβ = RandomFloat(0, 100);
	probabilitySliderαγ = RandomFloat(0, 100);
	probabilitySliderαδ = RandomFloat(0, 100);
	probabilitySliderαε = RandomFloat(0, 100);
	probabilitySliderαζ = RandomFloat(0, 100);
	probabilitySliderαη = RandomFloat(0, 100);

	probabilitySliderβα = RandomFloat(0, 100);
	probabilitySliderββ = RandomFloat(0, 100);
	probabilitySliderβγ = RandomFloat(0, 100);
	probabilitySliderβδ = RandomFloat(0, 100);
	probabilitySliderβε = RandomFloat(0, 100);
	probabilitySliderβζ = RandomFloat(0, 100);
	probabilitySliderβη = RandomFloat(0, 100);

	probabilitySliderγα = RandomFloat(0, 100);
	probabilitySliderγβ = RandomFloat(0, 100);
	probabilitySliderγγ = RandomFloat(0, 100);
	probabilitySliderγδ = RandomFloat(0, 100);
	probabilitySliderγε = RandomFloat(0, 100);
	probabilitySliderγζ = RandomFloat(0, 100);
	probabilitySliderγη = RandomFloat(0, 100);

	probabilitySliderδα = RandomFloat(0, 100);
	probabilitySliderδβ = RandomFloat(0, 100);
	probabilitySliderδγ = RandomFloat(0, 100);
	probabilitySliderδδ = RandomFloat(0, 100);
	probabilitySliderδε = RandomFloat(0, 100);
	probabilitySliderδζ = RandomFloat(0, 100);
	probabilitySliderδη = RandomFloat(0, 100);

	probabilitySliderεα = RandomFloat(0, 100);
	probabilitySliderεβ = RandomFloat(0, 100);
	probabilitySliderεγ = RandomFloat(0, 100);
	probabilitySliderεδ = RandomFloat(0, 100);
	probabilitySliderεε = RandomFloat(0, 100);
	probabilitySliderεζ = RandomFloat(0, 100);
	probabilitySliderεη = RandomFloat(0, 100);

	probabilitySliderζα = RandomFloat(0, 100);
	probabilitySliderζβ = RandomFloat(0, 100);
	probabilitySliderζγ = RandomFloat(0, 100);
	probabilitySliderζδ = RandomFloat(0, 100);
	probabilitySliderζε = RandomFloat(0, 100);
	probabilitySliderζζ = RandomFloat(0, 100);
	probabilitySliderζη = RandomFloat(0, 100);

	probabilitySliderηα = RandomFloat(0, 100);
	probabilitySliderηβ = RandomFloat(0, 100);
	probabilitySliderηγ = RandomFloat(0, 100);
	probabilitySliderηδ = RandomFloat(0, 100);
	probabilitySliderηε = RandomFloat(0, 100);
	probabilitySliderηζ = RandomFloat(0, 100);
	probabilitySliderηη = RandomFloat(0, 100);
}
void ofApp::rndir() {
	// βαEEN
	//numberSliderβ = RandomFloat(minR, 3000);
	powerSliderββ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβη = RandomFloat(minP, maxP) * forceVariance;

	vSliderββ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderβη = RandomFloat(minR, maxR) * radiusVariance;

	// αED
	//numberSliderα = RandomFloat(minR, 3000);
	powerSliderαα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαη = RandomFloat(minP, maxP) * forceVariance;

	vSliderαβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαη = RandomFloat(minR, maxR) * radiusVariance;

	// δHITE
	// numberSliderδ = RandomFloat(minR, 3000);
	powerSliderδδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδη = RandomFloat(minP, maxP) * forceVariance;

	vSliderδβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδη = RandomFloat(minR, maxR) * radiusVariance;

	// γLUE
	//numberSliderγ = RandomFloat(minR, 3000);
	powerSliderγγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderγη = RandomFloat(minP, maxP) * forceVariance;

	vSliderγβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγη = RandomFloat(minR, maxR) * radiusVariance;

	// εαANβE
	//numberSliderε = RandomFloat(minR, 3000);
	powerSliderεγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεη = RandomFloat(minP, maxP) * forceVariance;

	vSliderεβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεη = RandomFloat(minR, maxR) * radiusVariance;

	// ζHAζI
	//numberSliderζ = RandomFloat(minR, 3000);
	powerSliderζγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζη = RandomFloat(minP, maxP) * forceVariance;

	vSliderζβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζη = RandomFloat(minR, maxR) * radiusVariance;

	// ηαIMSεN
	//numberSliderη = RandomFloat(minR, 3000);
	powerSliderηγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηη = RandomFloat(minP, maxP) * forceVariance;

	vSliderηβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηη = RandomFloat(minR, maxR) * radiusVariance;
}

/// this is a cheap and quick way to save and load parameters (openFramework have betters ways but requires some additional library setups) 
// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::saveSettings()
{
	const std::vector<float> settings = {
		powerSliderββ, powerSliderβα, powerSliderβδ, powerSliderβγ, powerSliderβε, powerSliderβζ, powerSliderβη,
		vSliderββ, vSliderβα, vSliderβδ, vSliderβγ, vSliderβε, vSliderβζ, vSliderβη,
		powerSliderαβ, powerSliderαα, powerSliderαδ, powerSliderαγ, powerSliderαε, powerSliderαζ, powerSliderαη,
		vSliderαβ, vSliderαα, vSliderαδ, vSliderαγ, vSliderαε, vSliderαζ, vSliderαη,
		powerSliderδβ, powerSliderδα, powerSliderδδ, powerSliderδγ, powerSliderδε, powerSliderδζ, powerSliderδη,
		vSliderδβ, vSliderδα, vSliderδδ, vSliderδγ, vSliderδε, vSliderδζ, vSliderδη,
		powerSliderγβ, powerSliderγα, powerSliderγδ, powerSliderγγ, powerSliderγε, powerSliderγζ, powerSliderγη,
		vSliderγβ, vSliderγα, vSliderγδ, vSliderγγ, vSliderγε, vSliderγζ, vSliderγη,
		powerSliderεβ, powerSliderεα, powerSliderεδ, powerSliderεγ, powerSliderεε, powerSliderεζ, powerSliderεη,
		vSliderεβ, vSliderεα, vSliderεδ, vSliderεγ, vSliderεε, vSliderεζ, vSliderεη,
		powerSliderζβ, powerSliderζα, powerSliderζδ, powerSliderζγ, powerSliderζε, powerSliderζζ, powerSliderζη,
		vSliderζβ, vSliderζα, vSliderζδ, vSliderζγ, vSliderζε, vSliderζζ, vSliderζη,
		powerSliderηβ, powerSliderηα, powerSliderηδ, powerSliderηγ, powerSliderηε, powerSliderηζ, powerSliderηη,
		vSliderηβ, vSliderηα, vSliderηδ, vSliderηγ, vSliderηε, vSliderηζ, vSliderηη,
		static_cast<float>(numberSliderβ),
		static_cast<float>(numberSliderα),
		static_cast<float>(numberSliderδ),
		static_cast<float>(numberSliderγ),
		static_cast<float>(numberSliderε),
		static_cast<float>(numberSliderζ),
		static_cast<float>(numberSliderη),
		viscositySlider,
		viscositySliderαα,
		viscositySliderαβ,
		viscositySliderαγ,
		viscositySliderαδ,
		viscositySliderαε,
		viscositySliderαζ,
		viscositySliderαη,
		viscositySliderβα,
		viscositySliderββ,
		viscositySliderβγ,
		viscositySliderβδ,
		viscositySliderβε,
		viscositySliderβζ,
		viscositySliderβη,
		viscositySliderγα,
		viscositySliderγβ,
		viscositySliderγγ,
		viscositySliderγδ,
		viscositySliderγε,
		viscositySliderγζ,
		viscositySliderγη,
		viscositySliderδα,
		viscositySliderδβ,
		viscositySliderδγ,
		viscositySliderδδ,
		viscositySliderδε,
		viscositySliderδζ,
		viscositySliderδη,
		viscositySliderεα,
		viscositySliderεβ,
		viscositySliderεγ,
		viscositySliderεδ,
		viscositySliderεε,
		viscositySliderεζ,
		viscositySliderεη,
		viscositySliderζα,
		viscositySliderζβ,
		viscositySliderζγ,
		viscositySliderζδ,
		viscositySliderζε,
		viscositySliderζζ,
		viscositySliderζη,
		viscositySliderηα,
		viscositySliderηβ,
		viscositySliderηγ,
		viscositySliderηδ,
		viscositySliderηε,
		viscositySliderηζ,
		viscositySliderηη,
		InteractionEvoProbSlider,
		InteractionEvoAmountSlider,
		probabilitySlider,
		probabilitySliderαα,
		probabilitySliderαβ,
		probabilitySliderαγ,
		probabilitySliderαδ,
		probabilitySliderαε,
		probabilitySliderαζ,
		probabilitySliderαη,
		probabilitySliderβα,
		probabilitySliderββ,
		probabilitySliderβγ,
		probabilitySliderβδ,
		probabilitySliderβε,
		probabilitySliderβζ,
		probabilitySliderβη,
		probabilitySliderγα,
		probabilitySliderγβ,
		probabilitySliderγγ,
		probabilitySliderγδ,
		probabilitySliderγε,
		probabilitySliderγζ,
		probabilitySliderγη,
		probabilitySliderδα,
		probabilitySliderδβ,
		probabilitySliderδγ,
		probabilitySliderδδ,
		probabilitySliderδε,
		probabilitySliderδζ,
		probabilitySliderδη,
		probabilitySliderεα,
		probabilitySliderεβ,
		probabilitySliderεγ,
		probabilitySliderεδ,
		probabilitySliderεε,
		probabilitySliderεζ,
		probabilitySliderεη,
		probabilitySliderζα,
		probabilitySliderζβ,
		probabilitySliderζγ,
		probabilitySliderζδ,
		probabilitySliderζε,
		probabilitySliderζζ,
		probabilitySliderζη,
		probabilitySliderηα,
		probabilitySliderηβ,
		probabilitySliderηγ,
		probabilitySliderηδ,
		probabilitySliderηε,
		probabilitySliderηζ,
		probabilitySliderηη,
		minPowerSlider,
		maxPowerSlider,
		minRangeSlider,
		maxRangeSlider,
		ProbabilityEvoProbSlider,
		ProbabilityEvoAmountSlider,
		ViscosityEvoProbSlider,
		ViscosityEvoAmountSlider,
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

	if (p.size() < 215)
	{
		// better checks needed
		ofSystemAlertDialog("Could not read the file!");
	}
	else
	{
		powerSliderββ = p[0];
		powerSliderβα = p[1];
		powerSliderβδ = p[2];
		powerSliderβγ = p[3];
		powerSliderβε = p[4];
		powerSliderβζ = p[5];
		powerSliderβη = p[6];
		vSliderββ = p[7];
		vSliderβα = p[8];
		vSliderβδ = p[9];
		vSliderβγ = p[10];
		vSliderβε = p[11];
		vSliderβζ = p[12];
		vSliderβη = p[13];
		powerSliderαβ = p[14];
		powerSliderαα = p[15];
		powerSliderαδ = p[16];
		powerSliderαγ = p[17];
		powerSliderαε = p[18];
		powerSliderαζ = p[19];
		powerSliderαη = p[20];
		vSliderαβ = p[21];
		vSliderαα = p[22];
		vSliderαδ = p[23];
		vSliderαγ = p[24];
		vSliderαε = p[25];
		vSliderαζ = p[26];
		vSliderαη = p[27];
		powerSliderδβ = p[28];
		powerSliderδα = p[29];
		powerSliderδδ = p[30];
		powerSliderδγ = p[31];
		powerSliderδε = p[32];
		powerSliderδζ = p[33];
		powerSliderδη = p[34];
		vSliderδβ = p[35];
		vSliderδα = p[36];
		vSliderδδ = p[37];
		vSliderδγ = p[38];
		vSliderδε = p[39];
		vSliderδζ = p[40];
		vSliderδη = p[41];
		powerSliderγβ = p[42];
		powerSliderγα = p[43];
		powerSliderγδ = p[44];
		powerSliderγγ = p[45];
		powerSliderγε = p[46];
		powerSliderγζ = p[47];
		powerSliderγη = p[48];
		vSliderγβ = p[49];
		vSliderγα = p[50];
		vSliderγδ = p[51];
		vSliderγγ = p[52];
		vSliderγε = p[53];
		vSliderγζ = p[54];
		vSliderγη = p[55];
		powerSliderεβ = p[56];
		powerSliderεα = p[57];
		powerSliderεδ = p[58];
		powerSliderεγ = p[59];
		powerSliderεε = p[60];
		powerSliderεζ = p[61];
		powerSliderεη = p[62];
		vSliderεβ = p[63];
		vSliderεα = p[64];
		vSliderεδ = p[65];
		vSliderεγ = p[66];
		vSliderεε = p[67];
		vSliderεζ = p[68];
		vSliderεη = p[69];
		powerSliderζβ = p[70];
		powerSliderζα = p[71];
		powerSliderζδ = p[72];
		powerSliderζγ = p[73];
		powerSliderζε = p[74];
		powerSliderζζ = p[75];
		powerSliderζη = p[76];
		vSliderζβ = p[77];
		vSliderζα = p[78];
		vSliderζδ = p[79];
		vSliderζγ = p[80];
		vSliderζε = p[81];
		vSliderζζ = p[82];
		vSliderζη = p[83];
		powerSliderηβ = p[84];
		powerSliderηα = p[85];
		powerSliderηδ = p[86];
		powerSliderηγ = p[87];
		powerSliderηε = p[88];
		powerSliderηζ = p[89];
		powerSliderηη = p[90];
		vSliderηβ = p[91];
		vSliderηα = p[92];
		vSliderηδ = p[93];
		vSliderηγ = p[94];
		vSliderηε = p[95];
		vSliderηζ = p[96];
		vSliderηη = p[97];
		numberSliderβ = static_cast<int>(p[98]);
		numberSliderα = static_cast<int>(p[99]);
		numberSliderδ = static_cast<int>(p[100]);
		numberSliderγ = static_cast<int>(p[101]);
		numberSliderε = static_cast<int>(p[102]);
		numberSliderζ = static_cast<int>(p[103]);
		numberSliderη = static_cast<int>(p[104]);
		viscositySlider = p[105];
		viscositySliderαα = p[106];
		viscositySliderαβ = p[107];
		viscositySliderαγ = p[108];
		viscositySliderαδ = p[109];
		viscositySliderαε = p[110];
		viscositySliderαζ = p[111];
		viscositySliderαη = p[112];
		viscositySliderβα = p[113];
		viscositySliderββ = p[114];
		viscositySliderβγ = p[115];
		viscositySliderβδ = p[116];
		viscositySliderβε = p[117];
		viscositySliderβζ = p[118];
		viscositySliderβη = p[119];
		viscositySliderγα = p[120];
		viscositySliderγβ = p[121];
		viscositySliderγγ = p[122];
		viscositySliderγδ = p[123];
		viscositySliderγε = p[124];
		viscositySliderγζ = p[125];
		viscositySliderγη = p[126];
		viscositySliderδα = p[127];
		viscositySliderδβ = p[128];
		viscositySliderδγ = p[129];
		viscositySliderδδ = p[130];
		viscositySliderδε = p[131];
		viscositySliderδζ = p[132];
		viscositySliderδη = p[133];
		viscositySliderεα = p[134];
		viscositySliderεβ = p[135];
		viscositySliderεγ = p[136];
		viscositySliderεδ = p[137];
		viscositySliderεε = p[138];
		viscositySliderεζ = p[139];
		viscositySliderεη = p[140];
		viscositySliderζα = p[141];
		viscositySliderζβ = p[142];
		viscositySliderζγ = p[143];
		viscositySliderζδ = p[144];
		viscositySliderζε = p[145];
		viscositySliderζζ = p[146];
		viscositySliderζη = p[147];
		viscositySliderηα = p[148];
		viscositySliderηβ = p[149];
		viscositySliderηγ = p[150];
		viscositySliderηδ = p[151];
		viscositySliderηε = p[152];
		viscositySliderηζ = p[153];
		viscositySliderηη = p[154];
		InteractionEvoProbSlider = p[155];
		InteractionEvoAmountSlider = p[156];
		probabilitySlider = p[157];
		probabilitySliderαα = p[158];
		probabilitySliderαβ = p[159];
		probabilitySliderαγ = p[160];
		probabilitySliderαδ = p[161];
		probabilitySliderαε = p[162];
		probabilitySliderαζ = p[163];
		probabilitySliderαη = p[164];
		probabilitySliderβα = p[165];
		probabilitySliderββ = p[166];
		probabilitySliderβγ = p[167];
		probabilitySliderβδ = p[168];
		probabilitySliderβε = p[169];
		probabilitySliderβζ = p[170];
		probabilitySliderβη = p[171];
		probabilitySliderγα = p[172];
		probabilitySliderγβ = p[173];
		probabilitySliderγγ = p[174];
		probabilitySliderγδ = p[175];
		probabilitySliderγε = p[176];
		probabilitySliderγζ = p[177];
		probabilitySliderγη = p[178];
		probabilitySliderδα = p[179];
		probabilitySliderδβ = p[180];
		probabilitySliderδγ = p[181];
		probabilitySliderδδ = p[182];
		probabilitySliderδε = p[183];
		probabilitySliderδζ = p[184];
		probabilitySliderδη = p[185];
		probabilitySliderεα = p[186];
		probabilitySliderεβ = p[187];
		probabilitySliderεγ = p[188];
		probabilitySliderεδ = p[189];
		probabilitySliderεε = p[190];
		probabilitySliderεζ = p[191];
		probabilitySliderεη = p[192];
		probabilitySliderζα = p[193];
		probabilitySliderζβ = p[194];
		probabilitySliderζγ = p[195];
		probabilitySliderζδ = p[196];
		probabilitySliderζε = p[197];
		probabilitySliderζζ = p[198];
		probabilitySliderζη = p[199];
		probabilitySliderηα = p[200];
		probabilitySliderηβ = p[201];
		probabilitySliderηγ = p[202];
		probabilitySliderηδ = p[203];
		probabilitySliderηε = p[204];
		probabilitySliderηζ = p[205];
		probabilitySliderηη = p[206];
		minPowerSlider = p[207];
		maxPowerSlider = p[208];
		minRangeSlider = p[209];
		maxRangeSlider = p[210];
		ProbabilityEvoProbSlider = p[211];
		ProbabilityEvoAmountSlider = p[212];
		ViscosityEvoProbSlider = p[213];
		ViscosityEvoAmountSlider = p[214];
	}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - 7c49v49p version 1.7.0");
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

	// Quantity Group
	qtyGroup.setup("Quantity (require restart/randomize)");
	qtyGroup.add(numberSliderα.setup("Alpha", pnumberSliderα, 0, 10000));
	qtyGroup.add(numberSliderβ.setup("Beta", pnumberSliderβ, 0, 10000));
	qtyGroup.add(numberSliderγ.setup("Gamma", pnumberSliderγ, 0, 10000));
	qtyGroup.add(numberSliderδ.setup("Delta", pnumberSliderδ, 0, 10000));
	qtyGroup.add(numberSliderε.setup("Epsilon", pnumberSliderε, 0, 10000));
	qtyGroup.add(numberSliderζ.setup("Zeta", pnumberSliderζ, 0, 10000));
	qtyGroup.add(numberSliderη.setup("Eta", pnumberSliderη, 0, 10000));
	gui.add(&qtyGroup);
	qtyGroup.minimize();


	// Global Group
	globalGroup.setup("Global");
	globalGroup.add(evoToggle.setup("Evolve parameters", true));
	globalGroup.add(InteractionEvoProbSlider.setup("inter evo chance%", InterEvoChance, 0, 100));
	globalGroup.add(InteractionEvoAmountSlider.setup("inter evo amount%%", InterEvoAmount, 0, 100));
	globalGroup.add(ProbabilityEvoProbSlider.setup("prob evo chance%", ProbEvoChance, 0, 100));
	globalGroup.add(ProbabilityEvoAmountSlider.setup("prob evo amount%%", ProbEvoAmount, 0, 100));
	globalGroup.add(ViscosityEvoProbSlider.setup("visco evo chance%", ViscoEvoChance, 0, 100));
	globalGroup.add(ViscosityEvoAmountSlider.setup("visco evo amount%%", ViscoEvoAmount, 0, 100));
	globalGroup.add(minPowerSlider.setup("minimum power", minP, -200, 0));
	globalGroup.add(maxPowerSlider.setup("maximum power", maxP, 0, 200));
	globalGroup.add(minRangeSlider.setup("minimum range", minR, 0, 100));
	globalGroup.add(maxRangeSlider.setup("maximum range", maxR, 100, 500));
	globalGroup.add(probabilitySlider.setup("interaction prob%", probability, 1, 100));
	globalGroup.add(viscositySlider.setup("interaction viscosity", viscosity, 0, 1));
	globalGroup.add(motionBlurToggle.setup("Motion Blur", false));
	globalGroup.add(physicLabel.setup("physic (ms)", "0"));
	globalGroup.add(radiusToogle.setup("infinite radius", false));
	gui.add(&globalGroup);
	globalGroup.minimize();

	// alphaβEEN
	alphaGroup.setup("Alpha");
	alphaGroup.add(powerSliderαα.setup("power alpha x alpha:", ppowerSliderαα, minP, maxP));
	alphaGroup.add(powerSliderαβ.setup("power alpha x beta:", ppowerSliderαβ, minP, maxP));
	alphaGroup.add(powerSliderαδ.setup("power alpha x delta:", ppowerSliderαδ, minP, maxP));
	alphaGroup.add(powerSliderαγ.setup("power alpha x gamma:", ppowerSliderαγ, minP, maxP));
	alphaGroup.add(powerSliderαε.setup("power alpha x epsilon:", ppowerSliderαε, minP, maxP));
	alphaGroup.add(powerSliderαζ.setup("power alpha x zeta:", ppowerSliderαζ, minP, maxP));
	alphaGroup.add(powerSliderαη.setup("power alpha x eta:", ppowerSliderαη, minP, maxP));
	alphaGroup.add(vSliderαα.setup("radius alpha x alpha:", pvSliderαα, minR, maxR));
	alphaGroup.add(vSliderαβ.setup("radius alpha x beta:", pvSliderαβ, minR, maxR));
	alphaGroup.add(vSliderαδ.setup("radius alpha x delta:", pvSliderαδ, minR, maxR));
	alphaGroup.add(vSliderαγ.setup("radius alpha x gamma:", pvSliderαγ, minR, maxR));
	alphaGroup.add(vSliderαε.setup("radius alpha x epsilon:", pvSliderαε, minR, maxR));
	alphaGroup.add(vSliderαζ.setup("radius alpha x zeta:", pvSliderαζ, minR, maxR));
	alphaGroup.add(vSliderαη.setup("radius alpha x eta:", pvSliderαη, minR, maxR));
	alphaGroup.add(viscositySliderαβ.setup("Viscosity alpha x beta", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαα.setup("Viscosity alpha x alpha", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαγ.setup("Viscosity alpha x gamma", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαδ.setup("Viscosity alpha x delta", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαε.setup("Viscosity alpha x epsilon", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαζ.setup("Viscosity alpha x zeta", viscosity, 0, 1));
	alphaGroup.add(viscositySliderαη.setup("Viscosity alpha x eta", viscosity, 0, 1));
	alphaGroup.add(probabilitySliderαβ.setup("Probability alpha x beta", probabilityαβ, 0, 100));
	alphaGroup.add(probabilitySliderαα.setup("Probability alpha x alpha", probabilityαα, 0, 100));
	alphaGroup.add(probabilitySliderαγ.setup("Probability alpha x gamma", probabilityαγ, 0, 100));
	alphaGroup.add(probabilitySliderαδ.setup("Probability alpha x delta", probabilityαδ, 0, 100));
	alphaGroup.add(probabilitySliderαε.setup("Probability alpha x epsilon", probabilityαε, 0, 100));
	alphaGroup.add(probabilitySliderαζ.setup("Probability alpha x zeta", probabilityαζ, 0, 100));
	alphaGroup.add(probabilitySliderαη.setup("Probability alpha x eta", probabilityαη, 0, 100));
	alphaGroup.minimize();
	gui.add(&alphaGroup);

	// betaED
	betaGroup.setup("Beta");
	betaGroup.add(powerSliderβα.setup("power beta x alpha:", ppowerSliderβα, minP, maxP));
	betaGroup.add(powerSliderββ.setup("power beta x beta:", ppowerSliderββ, minP, maxP));
	betaGroup.add(powerSliderβδ.setup("power beta x delta:", ppowerSliderβδ, minP, maxP));
	betaGroup.add(powerSliderβγ.setup("power beta x gamma:", ppowerSliderβγ, minP, maxP));
	betaGroup.add(powerSliderβε.setup("power beta x epsilon:", ppowerSliderβε, minP, maxP));
	betaGroup.add(powerSliderβζ.setup("power beta x zeta:", ppowerSliderβζ, minP, maxP));
	betaGroup.add(powerSliderβη.setup("power beta x eta:", ppowerSliderβη, minP, maxP));
	betaGroup.add(vSliderβα.setup("radius r x alpha:", pvSliderβα, minR, maxR));
	betaGroup.add(vSliderββ.setup("radius r x beta:", pvSliderββ, minR, maxR));
	betaGroup.add(vSliderβδ.setup("radius r x delta:", pvSliderβδ, minR, maxR));
	betaGroup.add(vSliderβγ.setup("radius r x gamma:", pvSliderβγ, minR, maxR));
	betaGroup.add(vSliderβε.setup("radius r x epsilon:", pvSliderβε, minR, maxR));
	betaGroup.add(vSliderβζ.setup("radius r x zeta:", pvSliderβζ, minR, maxR));
	betaGroup.add(vSliderβη.setup("radius r x eta:", pvSliderβη, minR, maxR));
	betaGroup.add(viscositySliderββ.setup("Viscosity beta x beta", viscosity, 0, 1));
	betaGroup.add(viscositySliderβα.setup("Viscosity beta x alpha", viscosity, 0, 1));
	betaGroup.add(viscositySliderβγ.setup("Viscosity beta x gamma", viscosity, 0, 1));
	betaGroup.add(viscositySliderβδ.setup("Viscosity beta x delta", viscosity, 0, 1));
	betaGroup.add(viscositySliderβε.setup("Viscosity beta x epsilon", viscosity, 0, 1));
	betaGroup.add(viscositySliderβζ.setup("Viscosity beta x zeta", viscosity, 0, 1));
	betaGroup.add(viscositySliderβη.setup("Viscosity beta x eta", viscosity, 0, 1));
	betaGroup.add(probabilitySliderββ.setup("Probability beta x beta", probabilityββ, 0, 100));
	betaGroup.add(probabilitySliderβα.setup("Probability beta x alpha", probabilityβα, 0, 100));
	betaGroup.add(probabilitySliderβγ.setup("Probability beta x gamma", probabilityβγ, 0, 100));
	betaGroup.add(probabilitySliderβδ.setup("Probability beta x delta", probabilityβδ, 0, 100));
	betaGroup.add(probabilitySliderβε.setup("Probability beta x epsilon", probabilityβε, 0, 100));
	betaGroup.add(probabilitySliderβζ.setup("Probability beta x zeta", probabilityβζ, 0, 100));
	betaGroup.add(probabilitySliderβη.setup("Probability beta x eta", probabilityβη, 0, 100));
	betaGroup.minimize();
	gui.add(&betaGroup);

	// gammaLUE
	gammaGroup.setup("Gamma");
	gammaGroup.add(powerSliderγα.setup("power gamma x alpha:", ppowerSliderγα, minP, maxP));
	gammaGroup.add(powerSliderγβ.setup("power gamma x beta:", ppowerSliderγβ, minP, maxP));
	gammaGroup.add(powerSliderγδ.setup("power gamma x delta:", ppowerSliderγδ, minP, maxP));
	gammaGroup.add(powerSliderγγ.setup("power gamma x gamma:", ppowerSliderγγ, minP, maxP));
	gammaGroup.add(powerSliderγε.setup("power gamma x epsilon:", ppowerSliderγε, minP, maxP));
	gammaGroup.add(powerSliderγζ.setup("power gamma x zeta:", ppowerSliderγζ, minP, maxP));
	gammaGroup.add(powerSliderγη.setup("power gamma x eta:", ppowerSliderγη, minP, maxP));
	gammaGroup.add(vSliderγα.setup("radius b x alpha:", pvSliderγα, minR, maxR));
	gammaGroup.add(vSliderγβ.setup("radius b x beta:", pvSliderγβ, minR, maxR));
	gammaGroup.add(vSliderγδ.setup("radius b x delta:", pvSliderγδ, minR, maxR));
	gammaGroup.add(vSliderγγ.setup("radius b x gamma:", pvSliderγγ, minR, maxR));
	gammaGroup.add(vSliderγε.setup("radius b x epsilon:", pvSliderγε, minR, maxR));
	gammaGroup.add(vSliderγζ.setup("radius b x zeta:", pvSliderγζ, minR, maxR));
	gammaGroup.add(vSliderγη.setup("radius b x eta:", pvSliderγη, minR, maxR));
	gammaGroup.add(viscositySliderγβ.setup("Viscosity gamma x beta", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγα.setup("Viscosity gamma x alpha", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγγ.setup("Viscosity gamma x gamma", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγδ.setup("Viscosity gamma x delta", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγε.setup("Viscosity gamma x epsilon", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγζ.setup("Viscosity gamma x zeta", viscosity, 0, 1));
	gammaGroup.add(viscositySliderγη.setup("Viscosity gamma x eta", viscosity, 0, 1));
	gammaGroup.add(probabilitySliderγβ.setup("Probability gamma x beta", probabilityγβ, 0, 100));
	gammaGroup.add(probabilitySliderγα.setup("Probability gamma x alpha", probabilityγα, 0, 100));
	gammaGroup.add(probabilitySliderγγ.setup("Probability gamma x gamma", probabilityγγ, 0, 100));
	gammaGroup.add(probabilitySliderγδ.setup("Probability gamma x delta", probabilityγδ, 0, 100));
	gammaGroup.add(probabilitySliderγε.setup("Probability gamma x epsilon", probabilityγε, 0, 100));
	gammaGroup.add(probabilitySliderγζ.setup("Probability gamma x zeta", probabilityγζ, 0, 100));
	gammaGroup.add(probabilitySliderγη.setup("Probability gamma x eta", probabilityγη, 0, 100));
	gammaGroup.minimize();
	gui.add(&gammaGroup);

	// deltaHITE
	eltaGroup.setup("Delta");
	eltaGroup.add(powerSliderδα.setup("power delta x alpha:", ppowerSliderδα, minP, maxP));
	eltaGroup.add(powerSliderδβ.setup("power delta x beta:", ppowerSliderδβ, minP, maxP));
	eltaGroup.add(powerSliderδδ.setup("power delta x delta:", ppowerSliderδδ, minP, maxP));
	eltaGroup.add(powerSliderδγ.setup("power delta x gamma:", ppowerSliderδγ, minP, maxP));
	eltaGroup.add(powerSliderδε.setup("power delta x epsilon:", ppowerSliderδε, minP, maxP));
	eltaGroup.add(powerSliderδζ.setup("power delta x zeta:", ppowerSliderδζ, minP, maxP));
	eltaGroup.add(powerSliderδη.setup("power delta x eta:", ppowerSliderδη, minP, maxP));
	eltaGroup.add(vSliderδα.setup("radius w x alpha:", pvSliderδα, minR, maxR));
	eltaGroup.add(vSliderδβ.setup("radius w x beta:", pvSliderδβ, minR, maxR));
	eltaGroup.add(vSliderδδ.setup("radius w x delta:", pvSliderδδ, minR, maxR));
	eltaGroup.add(vSliderδγ.setup("radius w x gamma:", pvSliderδγ, minR, maxR));
	eltaGroup.add(vSliderδε.setup("radius w x epsilon:", pvSliderδε, minR, maxR));
	eltaGroup.add(vSliderδζ.setup("radius w x zeta:", pvSliderδζ, minR, maxR));
	eltaGroup.add(vSliderδη.setup("radius w x eta:", pvSliderδη, minR, maxR));
	eltaGroup.add(viscositySliderδβ.setup("Viscosity delta x beta", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδα.setup("Viscosity delta x alpha", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδγ.setup("Viscosity delta x gamma", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδδ.setup("Viscosity delta x delta", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδε.setup("Viscosity delta x epsilon", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδζ.setup("Viscosity delta x zeta", viscosity, 0, 1));
	eltaGroup.add(viscositySliderδη.setup("Viscosity delta x eta", viscosity, 0, 1));
	eltaGroup.add(probabilitySliderδβ.setup("Probability delta x beta", probabilityδβ, 0, 100));
	eltaGroup.add(probabilitySliderδα.setup("Probability delta x alpha", probabilityδα, 0, 100));
	eltaGroup.add(probabilitySliderδγ.setup("Probability delta x gamma", probabilityδγ, 0, 100));
	eltaGroup.add(probabilitySliderδδ.setup("Probability delta x delta", probabilityδδ, 0, 100));
	eltaGroup.add(probabilitySliderδε.setup("Probability delta x epsilon", probabilityδε, 0, 100));
	eltaGroup.add(probabilitySliderδζ.setup("Probability delta x zeta", probabilityδζ, 0, 100));
	eltaGroup.add(probabilitySliderδη.setup("Probability delta x eta", probabilityδη, 0, 100));
	eltaGroup.minimize();
	gui.add(&eltaGroup);

	// epsilonβANαE
	epsilonGroup.setup("Epsilon");
	epsilonGroup.add(powerSliderεα.setup("power epsilon x alpha:", ppowerSliderεα, minP, maxP));
	epsilonGroup.add(powerSliderεβ.setup("power epsilon x beta:", ppowerSliderεβ, minP, maxP));
	epsilonGroup.add(powerSliderεδ.setup("power epsilon x delta:", ppowerSliderεδ, minP, maxP));
	epsilonGroup.add(powerSliderεγ.setup("power epsilon x gamma:", ppowerSliderεγ, minP, maxP));
	epsilonGroup.add(powerSliderεε.setup("power epsilon x epsilon:", ppowerSliderεε, minP, maxP));
	epsilonGroup.add(powerSliderεζ.setup("power epsilon x zeta:", ppowerSliderεζ, minP, maxP));
	epsilonGroup.add(powerSliderεη.setup("power epsilon x eta:", ppowerSliderεη, minP, maxP));
	epsilonGroup.add(vSliderεα.setup("radius o x alpha:", pvSliderεα, minR, maxR));
	epsilonGroup.add(vSliderεβ.setup("radius o x beta:", pvSliderεβ, minR, maxR));
	epsilonGroup.add(vSliderεδ.setup("radius o x delta:", pvSliderεδ, minR, maxR));
	epsilonGroup.add(vSliderεγ.setup("radius o x gamma:", pvSliderεγ, minR, maxR));
	epsilonGroup.add(vSliderεε.setup("radius o x epsilon:", pvSliderεε, minR, maxR));
	epsilonGroup.add(vSliderεζ.setup("radius o x zeta:", pvSliderεζ, minR, maxR));
	epsilonGroup.add(vSliderεη.setup("radius o x eta:", pvSliderεη, minR, maxR));
	epsilonGroup.add(viscositySliderεβ.setup("Viscosity epsilon x beta", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεα.setup("Viscosity epsilon x alpha", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεγ.setup("Viscosity epsilon x gamma", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεδ.setup("Viscosity epsilon x delta", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεε.setup("Viscosity epsilon x epsilon", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεζ.setup("Viscosity epsilon x zeta", viscosity, 0, 1));
	epsilonGroup.add(viscositySliderεη.setup("Viscosity epsilon x eta", viscosity, 0, 1));
	epsilonGroup.add(probabilitySliderεβ.setup("Probability epsilon x beta", probabilityεβ, 0, 100));
	epsilonGroup.add(probabilitySliderεα.setup("Probability epsilon x alpha", probabilityεα, 0, 100));
	epsilonGroup.add(probabilitySliderεγ.setup("Probability epsilon x gamma", probabilityεγ, 0, 100));
	epsilonGroup.add(probabilitySliderεδ.setup("Probability epsilon x delta", probabilityεδ, 0, 100));
	epsilonGroup.add(probabilitySliderεε.setup("Probability epsilon x epsilon", probabilityεε, 0, 100));
	epsilonGroup.add(probabilitySliderεζ.setup("Probability epsilon x zeta", probabilityεζ, 0, 100));
	epsilonGroup.add(probabilitySliderεη.setup("Probability epsilon x eta", probabilityεη, 0, 100));
	epsilonGroup.minimize();
	gui.add(&epsilonGroup);

	// zetaHAζI
	zetaGroup.setup("Zeta");
	zetaGroup.add(powerSliderζα.setup("power zeta x alpha:", ppowerSliderζα, minP, maxP));
	zetaGroup.add(powerSliderζβ.setup("power zeta x beta:", ppowerSliderζβ, minP, maxP));
	zetaGroup.add(powerSliderζδ.setup("power zeta x delta:", ppowerSliderζδ, minP, maxP));
	zetaGroup.add(powerSliderζγ.setup("power zeta x gamma:", ppowerSliderζγ, minP, maxP));
	zetaGroup.add(powerSliderζε.setup("power zeta x epsilon:", ppowerSliderζε, minP, maxP));
	zetaGroup.add(powerSliderζζ.setup("power zeta x zeta:", ppowerSliderζζ, minP, maxP));
	zetaGroup.add(powerSliderζη.setup("power zeta x eta:", ppowerSliderζη, minP, maxP));
	zetaGroup.add(vSliderζα.setup("radius k x alpha:", pvSliderζα, minR, maxR));
	zetaGroup.add(vSliderζβ.setup("radius k x beta:", pvSliderζβ, minR, maxR));
	zetaGroup.add(vSliderζδ.setup("radius k x delta:", pvSliderζδ, minR, maxR));
	zetaGroup.add(vSliderζγ.setup("radius k x gamma:", pvSliderζγ, minR, maxR));
	zetaGroup.add(vSliderζε.setup("radius k x epsilon:", pvSliderζε, minR, maxR));
	zetaGroup.add(vSliderζζ.setup("radius k x zeta:", pvSliderζζ, minR, maxR));
	zetaGroup.add(vSliderζη.setup("radius k x eta:", pvSliderζη, minR, maxR));
	zetaGroup.add(viscositySliderζβ.setup("Viscosity zeta x beta", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζα.setup("Viscosity zeta x alpha", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζγ.setup("Viscosity zeta x gamma", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζδ.setup("Viscosity zeta x delta", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζε.setup("Viscosity zeta x epsilon", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζζ.setup("Viscosity zeta x zeta", viscosity, 0, 1));
	zetaGroup.add(viscositySliderζη.setup("Viscosity zeta x eta", viscosity, 0, 1));
	zetaGroup.add(probabilitySliderζβ.setup("Probability zeta x beta", probabilityζβ, 0, 100));
	zetaGroup.add(probabilitySliderζα.setup("Probability zeta x alpha", probabilityζα, 0, 100));
	zetaGroup.add(probabilitySliderζγ.setup("Probability zeta x gamma", probabilityζγ, 0, 100));
	zetaGroup.add(probabilitySliderζδ.setup("Probability zeta x delta", probabilityζδ, 0, 100));
	zetaGroup.add(probabilitySliderζε.setup("Probability zeta x epsilon", probabilityζε, 0, 100));
	zetaGroup.add(probabilitySliderζζ.setup("Probability zeta x zeta", probabilityζζ, 0, 100));
	zetaGroup.add(probabilitySliderζη.setup("Probability zeta x eta", probabilityζη, 0, 100));
	zetaGroup.minimize();
	gui.add(&zetaGroup);

	// etaβIMSεN
	etaGroup.setup("Eta");
	etaGroup.add(powerSliderηα.setup("power eta x alpha:", ppowerSliderηα, minP, maxP));
	etaGroup.add(powerSliderηβ.setup("power eta x beta:", ppowerSliderηβ, minP, maxP));
	etaGroup.add(powerSliderηδ.setup("power eta x delta:", ppowerSliderηδ, minP, maxP));
	etaGroup.add(powerSliderηγ.setup("power eta x gamma:", ppowerSliderηγ, minP, maxP));
	etaGroup.add(powerSliderηε.setup("power eta x epsilon:", ppowerSliderηε, minP, maxP));
	etaGroup.add(powerSliderηζ.setup("power eta x zeta:", ppowerSliderηζ, minP, maxP));
	etaGroup.add(powerSliderηη.setup("power eta x eta:", ppowerSliderηη, minP, maxP));
	etaGroup.add(vSliderηα.setup("radius c x alpha:", pvSliderηα, minR, maxR));
	etaGroup.add(vSliderηβ.setup("radius c x beta:", pvSliderηβ, minR, maxR));
	etaGroup.add(vSliderηδ.setup("radius c x delta:", pvSliderηδ, minR, maxR));
	etaGroup.add(vSliderηγ.setup("radius c x gamma:", pvSliderηγ, minR, maxR));
	etaGroup.add(vSliderηε.setup("radius c x epsilon:", pvSliderηε, minR, maxR));
	etaGroup.add(vSliderηζ.setup("radius c x zeta:", pvSliderηζ, minR, maxR));
	etaGroup.add(vSliderηη.setup("radius c x eta:", pvSliderηη, minR, maxR));
	etaGroup.add(viscositySliderηβ.setup("Viscosity eta x beta", viscosity, 0, 1));
	etaGroup.add(viscositySliderηα.setup("Viscosity eta x alpha", viscosity, 0, 1));
	etaGroup.add(viscositySliderηγ.setup("Viscosity eta x gamma", viscosity, 0, 1));
	etaGroup.add(viscositySliderηδ.setup("Viscosity eta x delta", viscosity, 0, 1));
	etaGroup.add(viscositySliderηε.setup("Viscosity eta x epsilon", viscosity, 0, 1));
	etaGroup.add(viscositySliderηζ.setup("Viscosity eta x zeta", viscosity, 0, 1));
	etaGroup.add(viscositySliderηη.setup("Viscosity eta x eta", viscosity, 0, 1));
	etaGroup.add(probabilitySliderηβ.setup("Probability eta x beta", probabilityηβ, 0, 100));
	etaGroup.add(probabilitySliderηα.setup("Probability eta x alpha", probabilityηα, 0, 100));
	etaGroup.add(probabilitySliderηγ.setup("Probability eta x gamma", probabilityηγ, 0, 100));
	etaGroup.add(probabilitySliderηδ.setup("Probability eta x delta", probabilityηδ, 0, 100));
	etaGroup.add(probabilitySliderηε.setup("Probability eta x epsilon", probabilityηε, 0, 100));
	etaGroup.add(probabilitySliderηζ.setup("Probability eta x zeta", probabilityηζ, 0, 100));
	etaGroup.add(probabilitySliderηη.setup("Probability eta x eta", probabilityηη, 0, 100));
	etaGroup.minimize();
	gui.add(&etaGroup);

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
	minP = minPowerSlider;
	maxP = maxPowerSlider;
	minR = minRangeSlider;
	maxR = maxRangeSlider;
	probability = probabilitySlider;
	probabilityαα = probabilitySliderαα;
	probabilityαβ = probabilitySliderαβ;
	probabilityαγ = probabilitySliderαγ;
	probabilityαδ = probabilitySliderαδ;
	probabilityαε = probabilitySliderαε;
	probabilityαζ = probabilitySliderαζ;
	probabilityαη = probabilitySliderαη;
	probabilityβα = probabilitySliderβα;
	probabilityββ = probabilitySliderββ;
	probabilityβγ = probabilitySliderβγ;
	probabilityβδ = probabilitySliderβδ;
	probabilityβε = probabilitySliderβε;
	probabilityβζ = probabilitySliderβζ;
	probabilityβη = probabilitySliderβη;
	probabilityγα = probabilitySliderγα;
	probabilityγβ = probabilitySliderγβ;
	probabilityγγ = probabilitySliderγγ;
	probabilityγδ = probabilitySliderγδ;
	probabilityγε = probabilitySliderγε;
	probabilityγζ = probabilitySliderγζ;
	probabilityγη = probabilitySliderγη;
	probabilityδα = probabilitySliderδα;
	probabilityδβ = probabilitySliderδβ;
	probabilityδγ = probabilitySliderδγ;
	probabilityδδ = probabilitySliderδδ;
	probabilityδε = probabilitySliderδε;
	probabilityδζ = probabilitySliderδζ;
	probabilityδη = probabilitySliderδη;
	probabilityεα = probabilitySliderεα;
	probabilityεβ = probabilitySliderεβ;
	probabilityεγ = probabilitySliderεγ;
	probabilityεδ = probabilitySliderεδ;
	probabilityεε = probabilitySliderεε;
	probabilityεζ = probabilitySliderεζ;
	probabilityεη = probabilitySliderεη;
	probabilityζβ = probabilitySliderζβ;
	probabilityζγ = probabilitySliderζγ;
	probabilityζδ = probabilitySliderζδ;
	probabilityζε = probabilitySliderζε;
	probabilityζζ = probabilitySliderζζ;
	probabilityζη = probabilitySliderζη;
	probabilityηα = probabilitySliderηα;
	probabilityηβ = probabilitySliderηβ;
	probabilityηγ = probabilitySliderηγ;
	probabilityηδ = probabilitySliderηδ;
	probabilityηε = probabilitySliderηε;
	probabilityηζ = probabilitySliderηζ;
	probabilityηη = probabilitySliderηη;
	viscosity = viscositySlider;
	viscosityαα = viscositySliderαα;
	viscosityαβ = viscositySliderαβ;
	viscosityαδ = viscositySliderαδ;
	viscosityαγ = viscositySliderαγ;
	viscosityαε = viscositySliderαε;
	viscosityαζ = viscositySliderαζ;
	viscosityαη = viscositySliderαη;
	viscosityβα = viscositySliderβα;
	viscosityββ = viscositySliderββ;
	viscosityβδ = viscositySliderβδ;
	viscosityβγ = viscositySliderβγ;
	viscosityβε = viscositySliderβε;
	viscosityβζ = viscositySliderβζ;
	viscosityβη = viscositySliderβη;
	viscosityγα = viscositySliderγα;
	viscosityγβ = viscositySliderγβ;
	viscosityγδ = viscositySliderγδ;
	viscosityγγ = viscositySliderγγ;
	viscosityγε = viscositySliderγε;
	viscosityγζ = viscositySliderγζ;
	viscosityγη = viscositySliderγη;
	viscosityδα = viscositySliderδα;
	viscosityδβ = viscositySliderδβ;
	viscosityδδ = viscositySliderδδ;
	viscosityδγ = viscositySliderδγ;
	viscosityδε = viscositySliderδε;
	viscosityδζ = viscositySliderδζ;
	viscosityδη = viscositySliderδη;
	viscosityεα = viscositySliderεα;
	viscosityεβ = viscositySliderεβ;
	viscosityεδ = viscositySliderεδ;
	viscosityεγ = viscositySliderεγ;
	viscosityεε = viscositySliderεε;
	viscosityεζ = viscositySliderεζ;
	viscosityεη = viscositySliderεη;
	viscosityζα = viscositySliderζα;
	viscosityζβ = viscositySliderζβ;
	viscosityζδ = viscositySliderζδ;
	viscosityζγ = viscositySliderζγ;
	viscosityζε = viscositySliderζε;
	viscosityζζ = viscositySliderζζ;
	viscosityζη = viscositySliderζη;
	viscosityηα = viscositySliderηα;
	viscosityηβ = viscositySliderηβ;
	viscosityηδ = viscositySliderηδ;
	viscosityηγ = viscositySliderηγ;
	viscosityηε = viscositySliderηε;
	viscosityηζ = viscositySliderηζ;
	viscosityηη = viscositySliderηη;

	worldGravity = gravitySlider;
	wallRepel = wallRepelSlider;
	InterEvoChance = InteractionEvoProbSlider;
	InterEvoAmount = InteractionEvoAmountSlider;
	ProbEvoChance = ProbabilityEvoProbSlider;
	ProbEvoAmount = ProbabilityEvoAmountSlider;
	ViscoEvoChance = ViscosityEvoProbSlider;
	ViscoEvoAmount = ViscosityEvoAmountSlider;

	if (evoToggle && ofRandom(1.0F) < (InterEvoChance / 100.0F))
	{
		powerSliderαα = powerSliderαα + ((ofRandom(2.0F) - 1.0F) * (powerSliderαα.getMax() - powerSliderαα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαβ = powerSliderαβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderαβ.getMax() - powerSliderαβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαγ = powerSliderαγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderαγ.getMax() - powerSliderαγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαδ = powerSliderαδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderαδ.getMax() - powerSliderαδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαε = powerSliderαε + ((ofRandom(2.0F) - 1.0F) * (powerSliderαε.getMax() - powerSliderαε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαζ = powerSliderαζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderαζ.getMax() - powerSliderαζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderαη = powerSliderαη + ((ofRandom(2.0F) - 1.0F) * (powerSliderαη.getMax() - powerSliderαη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβα = powerSliderβα + ((ofRandom(2.0F) - 1.0F) * (powerSliderβα.getMax() - powerSliderβα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderββ = powerSliderββ + ((ofRandom(2.0F) - 1.0F) * (powerSliderββ.getMax() - powerSliderββ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβγ = powerSliderβγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderβγ.getMax() - powerSliderβγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβδ = powerSliderβδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderβδ.getMax() - powerSliderβδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβε = powerSliderβε + ((ofRandom(2.0F) - 1.0F) * (powerSliderβε.getMax() - powerSliderβε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβζ = powerSliderβζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderβζ.getMax() - powerSliderβζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderβη = powerSliderβη + ((ofRandom(2.0F) - 1.0F) * (powerSliderβη.getMax() - powerSliderβη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγα = powerSliderγα + ((ofRandom(2.0F) - 1.0F) * (powerSliderγα.getMax() - powerSliderγα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγβ = powerSliderγβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderγβ.getMax() - powerSliderγβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγγ = powerSliderγγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderγγ.getMax() - powerSliderγγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγδ = powerSliderγδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderγδ.getMax() - powerSliderγδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγε = powerSliderγε + ((ofRandom(2.0F) - 1.0F) * (powerSliderγε.getMax() - powerSliderγε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγζ = powerSliderγζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderγζ.getMax() - powerSliderγζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderγη = powerSliderγη + ((ofRandom(2.0F) - 1.0F) * (powerSliderγη.getMax() - powerSliderγη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδα = powerSliderδα + ((ofRandom(2.0F) - 1.0F) * (powerSliderδα.getMax() - powerSliderδα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδβ = powerSliderδβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderδβ.getMax() - powerSliderδβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδγ = powerSliderδγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderδγ.getMax() - powerSliderδγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδδ = powerSliderδδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderδδ.getMax() - powerSliderδδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδε = powerSliderδε + ((ofRandom(2.0F) - 1.0F) * (powerSliderδε.getMax() - powerSliderδε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδζ = powerSliderδζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderδζ.getMax() - powerSliderδζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderδη = powerSliderδη + ((ofRandom(2.0F) - 1.0F) * (powerSliderδη.getMax() - powerSliderδη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεα = powerSliderεα + ((ofRandom(2.0F) - 1.0F) * (powerSliderεα.getMax() - powerSliderεα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεβ = powerSliderεβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderεβ.getMax() - powerSliderεβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεγ = powerSliderεγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderεγ.getMax() - powerSliderεγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεδ = powerSliderεδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderεδ.getMax() - powerSliderεδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεε = powerSliderεε + ((ofRandom(2.0F) - 1.0F) * (powerSliderεε.getMax() - powerSliderεε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεζ = powerSliderεζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderεζ.getMax() - powerSliderεζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderεη = powerSliderεη + ((ofRandom(2.0F) - 1.0F) * (powerSliderεη.getMax() - powerSliderεη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζα = powerSliderζα + ((ofRandom(2.0F) - 1.0F) * (powerSliderζα.getMax() - powerSliderζα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζβ = powerSliderζβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderζβ.getMax() - powerSliderζβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζγ = powerSliderζγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderζγ.getMax() - powerSliderζγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζδ = powerSliderζδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderζδ.getMax() - powerSliderζδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζε = powerSliderζε + ((ofRandom(2.0F) - 1.0F) * (powerSliderζε.getMax() - powerSliderζε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζζ = powerSliderζζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderζζ.getMax() - powerSliderζζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderζη = powerSliderζη + ((ofRandom(2.0F) - 1.0F) * (powerSliderζη.getMax() - powerSliderζη.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηα = powerSliderηα + ((ofRandom(2.0F) - 1.0F) * (powerSliderηα.getMax() - powerSliderηα.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηβ = powerSliderηβ + ((ofRandom(2.0F) - 1.0F) * (powerSliderηβ.getMax() - powerSliderηβ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηγ = powerSliderηγ + ((ofRandom(2.0F) - 1.0F) * (powerSliderηγ.getMax() - powerSliderηγ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηδ = powerSliderηδ + ((ofRandom(2.0F) - 1.0F) * (powerSliderηδ.getMax() - powerSliderηδ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηε = powerSliderηε + ((ofRandom(2.0F) - 1.0F) * (powerSliderηε.getMax() - powerSliderηε.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηζ = powerSliderηζ + ((ofRandom(2.0F) - 1.0F) * (powerSliderηζ.getMax() - powerSliderηζ.getMin()) * (InterEvoAmount / 100.0F));
		powerSliderηη = powerSliderηη + ((ofRandom(2.0F) - 1.0F) * (powerSliderηη.getMax() - powerSliderηη.getMin()) * (InterEvoAmount / 100.0F));

		vSliderαα = vSliderαα + ((ofRandom(2.0F) - 1.0F) * (vSliderαα.getMax() - vSliderαα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαβ = vSliderαβ + ((ofRandom(2.0F) - 1.0F) * (vSliderαβ.getMax() - vSliderαβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαγ = vSliderαγ + ((ofRandom(2.0F) - 1.0F) * (vSliderαγ.getMax() - vSliderαγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαδ = vSliderαδ + ((ofRandom(2.0F) - 1.0F) * (vSliderαδ.getMax() - vSliderαδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαε = vSliderαε + ((ofRandom(2.0F) - 1.0F) * (vSliderαε.getMax() - vSliderαε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαζ = vSliderαζ + ((ofRandom(2.0F) - 1.0F) * (vSliderαζ.getMax() - vSliderαζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderαη = vSliderαη + ((ofRandom(2.0F) - 1.0F) * (vSliderαη.getMax() - vSliderαη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβα = vSliderβα + ((ofRandom(2.0F) - 1.0F) * (vSliderβα.getMax() - vSliderβα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderββ = vSliderββ + ((ofRandom(2.0F) - 1.0F) * (vSliderββ.getMax() - vSliderββ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβγ = vSliderβγ + ((ofRandom(2.0F) - 1.0F) * (vSliderβγ.getMax() - vSliderβγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβδ = vSliderβδ + ((ofRandom(2.0F) - 1.0F) * (vSliderβδ.getMax() - vSliderβδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβε = vSliderβε + ((ofRandom(2.0F) - 1.0F) * (vSliderβε.getMax() - vSliderβε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβζ = vSliderβζ + ((ofRandom(2.0F) - 1.0F) * (vSliderβζ.getMax() - vSliderβζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderβη = vSliderβη + ((ofRandom(2.0F) - 1.0F) * (vSliderβη.getMax() - vSliderβη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγα = vSliderγα + ((ofRandom(2.0F) - 1.0F) * (vSliderγα.getMax() - vSliderγα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγβ = vSliderγβ + ((ofRandom(2.0F) - 1.0F) * (vSliderγβ.getMax() - vSliderγβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγγ = vSliderγγ + ((ofRandom(2.0F) - 1.0F) * (vSliderγγ.getMax() - vSliderγγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγδ = vSliderγδ + ((ofRandom(2.0F) - 1.0F) * (vSliderγδ.getMax() - vSliderγδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγε = vSliderγε + ((ofRandom(2.0F) - 1.0F) * (vSliderγε.getMax() - vSliderγε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγζ = vSliderγζ + ((ofRandom(2.0F) - 1.0F) * (vSliderγζ.getMax() - vSliderγζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderγη = vSliderγη + ((ofRandom(2.0F) - 1.0F) * (vSliderγη.getMax() - vSliderγη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδα = vSliderδα + ((ofRandom(2.0F) - 1.0F) * (vSliderδα.getMax() - vSliderδα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδβ = vSliderδβ + ((ofRandom(2.0F) - 1.0F) * (vSliderδβ.getMax() - vSliderδβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδγ = vSliderδγ + ((ofRandom(2.0F) - 1.0F) * (vSliderδγ.getMax() - vSliderδγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδδ = vSliderδδ + ((ofRandom(2.0F) - 1.0F) * (vSliderδδ.getMax() - vSliderδδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδε = vSliderδε + ((ofRandom(2.0F) - 1.0F) * (vSliderδε.getMax() - vSliderδε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδζ = vSliderδζ + ((ofRandom(2.0F) - 1.0F) * (vSliderδζ.getMax() - vSliderδζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderδη = vSliderδη + ((ofRandom(2.0F) - 1.0F) * (vSliderδη.getMax() - vSliderδη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεα = vSliderεα + ((ofRandom(2.0F) - 1.0F) * (vSliderεα.getMax() - vSliderεα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεβ = vSliderεβ + ((ofRandom(2.0F) - 1.0F) * (vSliderεβ.getMax() - vSliderεβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεγ = vSliderεγ + ((ofRandom(2.0F) - 1.0F) * (vSliderεγ.getMax() - vSliderεγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεδ = vSliderεδ + ((ofRandom(2.0F) - 1.0F) * (vSliderεδ.getMax() - vSliderεδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεε = vSliderεε + ((ofRandom(2.0F) - 1.0F) * (vSliderεε.getMax() - vSliderεε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεζ = vSliderεζ + ((ofRandom(2.0F) - 1.0F) * (vSliderεζ.getMax() - vSliderεζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderεη = vSliderεη + ((ofRandom(2.0F) - 1.0F) * (vSliderεη.getMax() - vSliderεη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζα = vSliderζα + ((ofRandom(2.0F) - 1.0F) * (vSliderζα.getMax() - vSliderζα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζβ = vSliderζβ + ((ofRandom(2.0F) - 1.0F) * (vSliderζβ.getMax() - vSliderζβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζγ = vSliderζγ + ((ofRandom(2.0F) - 1.0F) * (vSliderζγ.getMax() - vSliderζγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζδ = vSliderζδ + ((ofRandom(2.0F) - 1.0F) * (vSliderζδ.getMax() - vSliderζδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζε = vSliderζε + ((ofRandom(2.0F) - 1.0F) * (vSliderζε.getMax() - vSliderζε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζζ = vSliderζζ + ((ofRandom(2.0F) - 1.0F) * (vSliderζζ.getMax() - vSliderζζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderζη = vSliderζη + ((ofRandom(2.0F) - 1.0F) * (vSliderζη.getMax() - vSliderζη.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηα = vSliderηα + ((ofRandom(2.0F) - 1.0F) * (vSliderηα.getMax() - vSliderηα.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηβ = vSliderηβ + ((ofRandom(2.0F) - 1.0F) * (vSliderηβ.getMax() - vSliderηβ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηγ = vSliderηγ + ((ofRandom(2.0F) - 1.0F) * (vSliderηγ.getMax() - vSliderηγ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηδ = vSliderηδ + ((ofRandom(2.0F) - 1.0F) * (vSliderηδ.getMax() - vSliderηδ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηε = vSliderηε + ((ofRandom(2.0F) - 1.0F) * (vSliderηε.getMax() - vSliderηε.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηζ = vSliderηζ + ((ofRandom(2.0F) - 1.0F) * (vSliderηζ.getMax() - vSliderηζ.getMin()) * (InterEvoAmount / 100.0F));
		vSliderηη = vSliderηη + ((ofRandom(2.0F) - 1.0F) * (vSliderηη.getMax() - vSliderηη.getMin()) * (InterEvoAmount / 100.0F));

		if (powerSliderαα < minP) powerSliderαα = minP;
		if (powerSliderαβ < minP) powerSliderαβ = minP;
		if (powerSliderαγ < minP) powerSliderαγ = minP;
		if (powerSliderαδ < minP) powerSliderαδ = minP;
		if (powerSliderαε < minP) powerSliderαε = minP;
		if (powerSliderαζ < minP) powerSliderαζ = minP;
		if (powerSliderαη < minP) powerSliderαη = minP;
		if (powerSliderαα > maxP) powerSliderαα = maxP;
		if (powerSliderαβ > maxP) powerSliderαβ = maxP;
		if (powerSliderαγ > maxP) powerSliderαγ = maxP;
		if (powerSliderαδ > maxP) powerSliderαδ = maxP;
		if (powerSliderαε > maxP) powerSliderαε = maxP;
		if (powerSliderαζ > maxP) powerSliderαζ = maxP;
		if (powerSliderαη > maxP) powerSliderαη = maxP;
		if (vSliderαα < minR) vSliderαα = minR;
		if (vSliderαβ < minR) vSliderαβ = minR;
		if (vSliderαγ < minR) vSliderαγ = minR;
		if (vSliderαδ < minR) vSliderαδ = minR;
		if (vSliderαε < minR) vSliderαε = minR;
		if (vSliderαζ < minR) vSliderαζ = minR;
		if (vSliderαη < minR) vSliderαη = minR;
		if (vSliderαα > maxR) vSliderαα = maxR;
		if (vSliderαβ > maxR) vSliderαβ = maxR;
		if (vSliderαγ > maxR) vSliderαγ = maxR;
		if (vSliderαδ > maxR) vSliderαδ = maxR;
		if (vSliderαε > maxR) vSliderαε = maxR;
		if (vSliderαζ > maxR) vSliderαζ = maxR;
		if (vSliderαη > maxR) vSliderαη = maxR;
		if (powerSliderβα < minP) powerSliderβα = minP;
		if (powerSliderββ < minP) powerSliderββ = minP;
		if (powerSliderβγ < minP) powerSliderβγ = minP;
		if (powerSliderβδ < minP) powerSliderβδ = minP;
		if (powerSliderβε < minP) powerSliderβε = minP;
		if (powerSliderβζ < minP) powerSliderβζ = minP;
		if (powerSliderβη < minP) powerSliderβη = minP;
		if (powerSliderβα > maxP) powerSliderβα = maxP;
		if (powerSliderββ > maxP) powerSliderββ = maxP;
		if (powerSliderβγ > maxP) powerSliderβγ = maxP;
		if (powerSliderβδ > maxP) powerSliderβδ = maxP;
		if (powerSliderβε > maxP) powerSliderβε = maxP;
		if (powerSliderβζ > maxP) powerSliderβζ = maxP;
		if (powerSliderβη > maxP) powerSliderβη = maxP;
		if (vSliderβα < minR) vSliderβα = minR;
		if (vSliderββ < minR) vSliderββ = minR;
		if (vSliderβγ < minR) vSliderβγ = minR;
		if (vSliderβδ < minR) vSliderβδ = minR;
		if (vSliderβε < minR) vSliderβε = minR;
		if (vSliderβζ < minR) vSliderβζ = minR;
		if (vSliderβη < minR) vSliderβη = minR;
		if (vSliderβα > maxR) vSliderβα = maxR;
		if (vSliderββ > maxR) vSliderββ = maxR;
		if (vSliderβγ > maxR) vSliderβγ = maxR;
		if (vSliderβδ > maxR) vSliderβδ = maxR;
		if (vSliderβε > maxR) vSliderβε = maxR;
		if (vSliderβζ > maxR) vSliderβζ = maxR;
		if (vSliderβη > maxR) vSliderβη = maxR;
		if (powerSliderγα < minP) powerSliderγα = minP;
		if (powerSliderγβ < minP) powerSliderγβ = minP;
		if (powerSliderγγ < minP) powerSliderγγ = minP;
		if (powerSliderγδ < minP) powerSliderγδ = minP;
		if (powerSliderγε < minP) powerSliderγε = minP;
		if (powerSliderγζ < minP) powerSliderγζ = minP;
		if (powerSliderγη < minP) powerSliderγη = minP;
		if (powerSliderγα > maxP) powerSliderγα = maxP;
		if (powerSliderγβ > maxP) powerSliderγβ = maxP;
		if (powerSliderγγ > maxP) powerSliderγγ = maxP;
		if (powerSliderγδ > maxP) powerSliderγδ = maxP;
		if (powerSliderγε > maxP) powerSliderγε = maxP;
		if (powerSliderγζ > maxP) powerSliderγζ = maxP;
		if (powerSliderγη > maxP) powerSliderγη = maxP;
		if (vSliderγα < minR) vSliderγα = minR;
		if (vSliderγβ < minR) vSliderγβ = minR;
		if (vSliderγγ < minR) vSliderγγ = minR;
		if (vSliderγδ < minR) vSliderγδ = minR;
		if (vSliderγε < minR) vSliderγε = minR;
		if (vSliderγζ < minR) vSliderγζ = minR;
		if (vSliderγη < minR) vSliderγη = minR;
		if (vSliderγα > maxR) vSliderγα = maxR;
		if (vSliderγβ > maxR) vSliderγβ = maxR;
		if (vSliderγγ > maxR) vSliderγγ = maxR;
		if (vSliderγδ > maxR) vSliderγδ = maxR;
		if (vSliderγε > maxR) vSliderγε = maxR;
		if (vSliderγζ > maxR) vSliderγζ = maxR;
		if (vSliderγη > maxR) vSliderγη = maxR;
		if (powerSliderδα < minP) powerSliderδα = minP;
		if (powerSliderδβ < minP) powerSliderδβ = minP;
		if (powerSliderδγ < minP) powerSliderδγ = minP;
		if (powerSliderδδ < minP) powerSliderδδ = minP;
		if (powerSliderδε < minP) powerSliderδε = minP;
		if (powerSliderδζ < minP) powerSliderδζ = minP;
		if (powerSliderδη < minP) powerSliderδη = minP;
		if (powerSliderδα > maxP) powerSliderδα = maxP;
		if (powerSliderδβ > maxP) powerSliderδβ = maxP;
		if (powerSliderδγ > maxP) powerSliderδγ = maxP;
		if (powerSliderδδ > maxP) powerSliderδδ = maxP;
		if (powerSliderδε > maxP) powerSliderδε = maxP;
		if (powerSliderδζ > maxP) powerSliderδζ = maxP;
		if (powerSliderδη > maxP) powerSliderδη = maxP;
		if (vSliderδα < minR) vSliderδα = minR;
		if (vSliderδβ < minR) vSliderδβ = minR;
		if (vSliderδγ < minR) vSliderδγ = minR;
		if (vSliderδδ < minR) vSliderδδ = minR;
		if (vSliderδε < minR) vSliderδε = minR;
		if (vSliderδζ < minR) vSliderδζ = minR;
		if (vSliderδη < minR) vSliderδη = minR;
		if (vSliderδα > maxR) vSliderδα = maxR;
		if (vSliderδβ > maxR) vSliderδβ = maxR;
		if (vSliderδγ > maxR) vSliderδγ = maxR;
		if (vSliderδδ > maxR) vSliderδδ = maxR;
		if (vSliderδε > maxR) vSliderδε = maxR;
		if (vSliderδζ > maxR) vSliderδζ = maxR;
		if (vSliderδη > maxR) vSliderδη = maxR;
		if (powerSliderεα < minP) powerSliderεα = minP;
		if (powerSliderεβ < minP) powerSliderεβ = minP;
		if (powerSliderεγ < minP) powerSliderεγ = minP;
		if (powerSliderεδ < minP) powerSliderεδ = minP;
		if (powerSliderεε < minP) powerSliderεε = minP;
		if (powerSliderεζ < minP) powerSliderεζ = minP;
		if (powerSliderεη < minP) powerSliderεη = minP;
		if (powerSliderεα > maxP) powerSliderεα = maxP;
		if (powerSliderεβ > maxP) powerSliderεβ = maxP;
		if (powerSliderεγ > maxP) powerSliderεγ = maxP;
		if (powerSliderεδ > maxP) powerSliderεδ = maxP;
		if (powerSliderεε > maxP) powerSliderεε = maxP;
		if (powerSliderεζ > maxP) powerSliderεζ = maxP;
		if (powerSliderεη > maxP) powerSliderεη = maxP;
		if (vSliderεα < minR) vSliderεα = minR;
		if (vSliderεβ < minR) vSliderεβ = minR;
		if (vSliderεγ < minR) vSliderεγ = minR;
		if (vSliderεδ < minR) vSliderεδ = minR;
		if (vSliderεε < minR) vSliderεε = minR;
		if (vSliderεζ < minR) vSliderεζ = minR;
		if (vSliderεη < minR) vSliderεη = minR;
		if (vSliderεα > maxR) vSliderεα = maxR;
		if (vSliderεβ > maxR) vSliderεβ = maxR;
		if (vSliderεγ > maxR) vSliderεγ = maxR;
		if (vSliderεδ > maxR) vSliderεδ = maxR;
		if (vSliderεε > maxR) vSliderεε = maxR;
		if (vSliderεζ > maxR) vSliderεζ = maxR;
		if (vSliderεη > maxR) vSliderεη = maxR;
		if (powerSliderζα < minP) powerSliderζα = minP;
		if (powerSliderζβ < minP) powerSliderζβ = minP;
		if (powerSliderζγ < minP) powerSliderζγ = minP;
		if (powerSliderζδ < minP) powerSliderζδ = minP;
		if (powerSliderζε < minP) powerSliderζε = minP;
		if (powerSliderζζ < minP) powerSliderζζ = minP;
		if (powerSliderζη < minP) powerSliderζη = minP;
		if (powerSliderζα > maxP) powerSliderζα = maxP;
		if (powerSliderζβ > maxP) powerSliderζβ = maxP;
		if (powerSliderζγ > maxP) powerSliderζγ = maxP;
		if (powerSliderζδ > maxP) powerSliderζδ = maxP;
		if (powerSliderζε > maxP) powerSliderζε = maxP;
		if (powerSliderζζ > maxP) powerSliderζζ = maxP;
		if (powerSliderζη > maxP) powerSliderζη = maxP;
		if (vSliderζα < minR) vSliderζα = minR;
		if (vSliderζβ < minR) vSliderζβ = minR;
		if (vSliderζγ < minR) vSliderζγ = minR;
		if (vSliderζδ < minR) vSliderζδ = minR;
		if (vSliderζε < minR) vSliderζε = minR;
		if (vSliderζζ < minR) vSliderζζ = minR;
		if (vSliderζη < minR) vSliderζη = minR;
		if (vSliderζα > maxR) vSliderζα = maxR;
		if (vSliderζβ > maxR) vSliderζβ = maxR;
		if (vSliderζγ > maxR) vSliderζγ = maxR;
		if (vSliderζδ > maxR) vSliderζδ = maxR;
		if (vSliderζε > maxR) vSliderζε = maxR;
		if (vSliderζζ > maxR) vSliderζζ = maxR;
		if (vSliderζη > maxR) vSliderζη = maxR;
		if (powerSliderηα < minP) powerSliderηα = minP;
		if (powerSliderηβ < minP) powerSliderηβ = minP;
		if (powerSliderηγ < minP) powerSliderηγ = minP;
		if (powerSliderηδ < minP) powerSliderηδ = minP;
		if (powerSliderηε < minP) powerSliderηε = minP;
		if (powerSliderηζ < minP) powerSliderηζ = minP;
		if (powerSliderηη < minP) powerSliderηη = minP;
		if (powerSliderηα > maxP) powerSliderηα = maxP;
		if (powerSliderηβ > maxP) powerSliderηβ = maxP;
		if (powerSliderηγ > maxP) powerSliderηγ = maxP;
		if (powerSliderηδ > maxP) powerSliderηδ = maxP;
		if (powerSliderηε > maxP) powerSliderηε = maxP;
		if (powerSliderηζ > maxP) powerSliderηζ = maxP;
		if (powerSliderηη > maxP) powerSliderηη = maxP;
		if (vSliderηα < minR) vSliderηα = minR;
		if (vSliderηβ < minR) vSliderηβ = minR;
		if (vSliderηγ < minR) vSliderηγ = minR;
		if (vSliderηδ < minR) vSliderηδ = minR;
		if (vSliderηε < minR) vSliderηε = minR;
		if (vSliderηζ < minR) vSliderηζ = minR;
		if (vSliderηη < minR) vSliderηη = minR;
		if (vSliderηα > maxR) vSliderηα = maxR;
		if (vSliderηβ > maxR) vSliderηβ = maxR;
		if (vSliderηγ > maxR) vSliderηγ = maxR;
		if (vSliderηδ > maxR) vSliderηδ = maxR;
		if (vSliderηε > maxR) vSliderηε = maxR;
		if (vSliderηζ > maxR) vSliderηζ = maxR;
		if (vSliderηη > maxR) vSliderηη = maxR;
	}
	if (evoToggle && ofRandom(1.0F) < (ViscoEvoChance / 100.0F))
	{
		viscositySlider = viscositySlider + ((ofRandom(2.0F) - 1.0F) * (viscositySlider.getMax() - viscositySlider.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderαα = viscositySliderαα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαα.getMax() - viscositySliderαα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαβ = viscositySliderαβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαβ.getMax() - viscositySliderαβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαγ = viscositySliderαγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαγ.getMax() - viscositySliderαγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαδ = viscositySliderαδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαδ.getMax() - viscositySliderαδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαε = viscositySliderαε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαε.getMax() - viscositySliderαε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαζ = viscositySliderαζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαζ.getMax() - viscositySliderαζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderαη = viscositySliderαη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderαη.getMax() - viscositySliderαη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderβα = viscositySliderβα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβα.getMax() - viscositySliderβα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderββ = viscositySliderββ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderββ.getMax() - viscositySliderββ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderβγ = viscositySliderβγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβγ.getMax() - viscositySliderβγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderβδ = viscositySliderβδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβδ.getMax() - viscositySliderβδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderβε = viscositySliderβε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβε.getMax() - viscositySliderβε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderβζ = viscositySliderβζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβζ.getMax() - viscositySliderβζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderβη = viscositySliderβη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderβη.getMax() - viscositySliderβη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderγα = viscositySliderγα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγα.getMax() - viscositySliderγα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγβ = viscositySliderγβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγβ.getMax() - viscositySliderγβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγγ = viscositySliderγγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγγ.getMax() - viscositySliderγγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγδ = viscositySliderγδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγδ.getMax() - viscositySliderγδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγε = viscositySliderγε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγε.getMax() - viscositySliderγε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγζ = viscositySliderγζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγζ.getMax() - viscositySliderγζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderγη = viscositySliderγη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderγη.getMax() - viscositySliderγη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderδα = viscositySliderδα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδα.getMax() - viscositySliderδα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδβ = viscositySliderδβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδβ.getMax() - viscositySliderδβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδγ = viscositySliderδγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδγ.getMax() - viscositySliderδγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδδ = viscositySliderδδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδδ.getMax() - viscositySliderδδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδε = viscositySliderδε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδε.getMax() - viscositySliderδε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδζ = viscositySliderδζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδζ.getMax() - viscositySliderδζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderδη = viscositySliderδη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderδη.getMax() - viscositySliderδη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderεα = viscositySliderεα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεα.getMax() - viscositySliderεα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεβ = viscositySliderεβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεβ.getMax() - viscositySliderεβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεγ = viscositySliderεγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεγ.getMax() - viscositySliderεγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεδ = viscositySliderεδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεδ.getMax() - viscositySliderεδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεε = viscositySliderεε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεε.getMax() - viscositySliderεε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεζ = viscositySliderεζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεζ.getMax() - viscositySliderεζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderεη = viscositySliderεη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderεη.getMax() - viscositySliderεη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderζα = viscositySliderζα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζα.getMax() - viscositySliderζα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζβ = viscositySliderζβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζβ.getMax() - viscositySliderζβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζγ = viscositySliderζγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζγ.getMax() - viscositySliderζγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζδ = viscositySliderζδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζδ.getMax() - viscositySliderζδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζε = viscositySliderζε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζε.getMax() - viscositySliderζε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζζ = viscositySliderζζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζζ.getMax() - viscositySliderζζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderζη = viscositySliderζη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderζη.getMax() - viscositySliderζη.getMin()) * (ViscoEvoAmount / 100.0F));

		viscositySliderηα = viscositySliderηα + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηα.getMax() - viscositySliderηα.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηβ = viscositySliderηβ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηβ.getMax() - viscositySliderηβ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηγ = viscositySliderηγ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηγ.getMax() - viscositySliderηγ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηδ = viscositySliderηδ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηδ.getMax() - viscositySliderηδ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηε = viscositySliderηε + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηε.getMax() - viscositySliderηε.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηζ = viscositySliderηζ + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηζ.getMax() - viscositySliderηζ.getMin()) * (ViscoEvoAmount / 100.0F));
		viscositySliderηη = viscositySliderηη + ((ofRandom(2.0F) - 1.0F) * (viscositySliderηη.getMax() - viscositySliderηη.getMin()) * (ViscoEvoAmount / 100.0F));

		if (viscositySlider < 0.0F) viscositySlider = 0;
		if (viscositySlider > 1.0F) viscositySlider = 1.0F;
		if (viscositySliderαα < 0.0F) viscositySliderαα = 0;
		if (viscositySliderαβ < 0.0F) viscositySliderαβ = 0;
		if (viscositySliderαγ < 0.0F) viscositySliderαγ = 0;
		if (viscositySliderαδ < 0.0F) viscositySliderαδ = 0;
		if (viscositySliderαε < 0.0F) viscositySliderαε = 0;
		if (viscositySliderαζ < 0.0F) viscositySliderαζ = 0;
		if (viscositySliderαη < 0.0F) viscositySliderαη = 0;
		if (viscositySliderαα > 1.0F) viscositySliderαα = 1.0F;
		if (viscositySliderαβ > 1.0F) viscositySliderαβ = 1.0F;
		if (viscositySliderαγ > 1.0F) viscositySliderαγ = 1.0F;
		if (viscositySliderαδ > 1.0F) viscositySliderαδ = 1.0F;
		if (viscositySliderαε > 1.0F) viscositySliderαε = 1.0F;
		if (viscositySliderαζ > 1.0F) viscositySliderαζ = 1.0F;
		if (viscositySliderαη > 1.0F) viscositySliderαη = 1.0F;
		if (viscositySliderβα < 0.0F) viscositySliderβα = 0;
		if (viscositySliderββ < 0.0F) viscositySliderββ = 0;
		if (viscositySliderβγ < 0.0F) viscositySliderβγ = 0;
		if (viscositySliderβδ < 0.0F) viscositySliderβδ = 0;
		if (viscositySliderβε < 0.0F) viscositySliderβε = 0;
		if (viscositySliderβζ < 0.0F) viscositySliderβζ = 0;
		if (viscositySliderβη < 0.0F) viscositySliderβη = 0;
		if (viscositySliderβα > 1.0F) viscositySliderβα = 1.0F;
		if (viscositySliderββ > 1.0F) viscositySliderββ = 1.0F;
		if (viscositySliderβγ > 1.0F) viscositySliderβγ = 1.0F;
		if (viscositySliderβδ > 1.0F) viscositySliderβδ = 1.0F;
		if (viscositySliderβε > 1.0F) viscositySliderβε = 1.0F;
		if (viscositySliderβζ > 1.0F) viscositySliderβζ = 1.0F;
		if (viscositySliderβη > 1.0F) viscositySliderβη = 1.0F;
		if (viscositySliderγα < 0.0F) viscositySliderγα = 0;
		if (viscositySliderγβ < 0.0F) viscositySliderγβ = 0;
		if (viscositySliderγγ < 0.0F) viscositySliderγγ = 0;
		if (viscositySliderγδ < 0.0F) viscositySliderγδ = 0;
		if (viscositySliderγε < 0.0F) viscositySliderγε = 0;
		if (viscositySliderγζ < 0.0F) viscositySliderγζ = 0;
		if (viscositySliderγη < 0.0F) viscositySliderγη = 0;
		if (viscositySliderγα > 1.0F) viscositySliderγα = 1.0F;
		if (viscositySliderγβ > 1.0F) viscositySliderγβ = 1.0F;
		if (viscositySliderγγ > 1.0F) viscositySliderγγ = 1.0F;
		if (viscositySliderγδ > 1.0F) viscositySliderγδ = 1.0F;
		if (viscositySliderγε > 1.0F) viscositySliderγε = 1.0F;
		if (viscositySliderγζ > 1.0F) viscositySliderγζ = 1.0F;
		if (viscositySliderγη > 1.0F) viscositySliderγη = 1.0F;
		if (viscositySliderδα < 0.0F) viscositySliderδα = 0;
		if (viscositySliderδβ < 0.0F) viscositySliderδβ = 0;
		if (viscositySliderδγ < 0.0F) viscositySliderδγ = 0;
		if (viscositySliderδδ < 0.0F) viscositySliderδδ = 0;
		if (viscositySliderδε < 0.0F) viscositySliderδε = 0;
		if (viscositySliderδζ < 0.0F) viscositySliderδζ = 0;
		if (viscositySliderδη < 0.0F) viscositySliderδη = 0;
		if (viscositySliderδα > 1.0F) viscositySliderδα = 1.0F;
		if (viscositySliderδβ > 1.0F) viscositySliderδβ = 1.0F;
		if (viscositySliderδγ > 1.0F) viscositySliderδγ = 1.0F;
		if (viscositySliderδδ > 1.0F) viscositySliderδδ = 1.0F;
		if (viscositySliderδε > 1.0F) viscositySliderδε = 1.0F;
		if (viscositySliderδζ > 1.0F) viscositySliderδζ = 1.0F;
		if (viscositySliderδη > 1.0F) viscositySliderδη = 1.0F;
		if (viscositySliderεα < 0.0F) viscositySliderεα = 0;
		if (viscositySliderεβ < 0.0F) viscositySliderεβ = 0;
		if (viscositySliderεγ < 0.0F) viscositySliderεγ = 0;
		if (viscositySliderεδ < 0.0F) viscositySliderεδ = 0;
		if (viscositySliderεε < 0.0F) viscositySliderεε = 0;
		if (viscositySliderεζ < 0.0F) viscositySliderεζ = 0;
		if (viscositySliderεη < 0.0F) viscositySliderεη = 0;
		if (viscositySliderεα > 1.0F) viscositySliderεα = 1.0F;
		if (viscositySliderεβ > 1.0F) viscositySliderεβ = 1.0F;
		if (viscositySliderεγ > 1.0F) viscositySliderεγ = 1.0F;
		if (viscositySliderεδ > 1.0F) viscositySliderεδ = 1.0F;
		if (viscositySliderεε > 1.0F) viscositySliderεε = 1.0F;
		if (viscositySliderεζ > 1.0F) viscositySliderεζ = 1.0F;
		if (viscositySliderεη > 1.0F) viscositySliderεη = 1.0F;
		if (viscositySliderζα < 0.0F) viscositySliderζα = 0;
		if (viscositySliderζβ < 0.0F) viscositySliderζβ = 0;
		if (viscositySliderζγ < 0.0F) viscositySliderζγ = 0;
		if (viscositySliderζδ < 0.0F) viscositySliderζδ = 0;
		if (viscositySliderζε < 0.0F) viscositySliderζε = 0;
		if (viscositySliderζζ < 0.0F) viscositySliderζζ = 0;
		if (viscositySliderζη < 0.0F) viscositySliderζη = 0;
		if (viscositySliderζα > 1.0F) viscositySliderζα = 1.0F;
		if (viscositySliderζβ > 1.0F) viscositySliderζβ = 1.0F;
		if (viscositySliderζγ > 1.0F) viscositySliderζγ = 1.0F;
		if (viscositySliderζδ > 1.0F) viscositySliderζδ = 1.0F;
		if (viscositySliderζε > 1.0F) viscositySliderζε = 1.0F;
		if (viscositySliderζζ > 1.0F) viscositySliderζζ = 1.0F;
		if (viscositySliderζη > 1.0F) viscositySliderζη = 1.0F;
		if (viscositySliderηα < 0.0F) viscositySliderηα = 0;
		if (viscositySliderηβ < 0.0F) viscositySliderηβ = 0;
		if (viscositySliderηγ < 0.0F) viscositySliderηγ = 0;
		if (viscositySliderηδ < 0.0F) viscositySliderηδ = 0;
		if (viscositySliderηε < 0.0F) viscositySliderηε = 0;
		if (viscositySliderηζ < 0.0F) viscositySliderηζ = 0;
		if (viscositySliderηη < 0.0F) viscositySliderηη = 0;
		if (viscositySliderηα > 1.0F) viscositySliderηα = 1.0F;
		if (viscositySliderηβ > 1.0F) viscositySliderηβ = 1.0F;
		if (viscositySliderηγ > 1.0F) viscositySliderηγ = 1.0F;
		if (viscositySliderηδ > 1.0F) viscositySliderηδ = 1.0F;
		if (viscositySliderηε > 1.0F) viscositySliderηε = 1.0F;
		if (viscositySliderηζ > 1.0F) viscositySliderηζ = 1.0F;
		if (viscositySliderηη > 1.0F) viscositySliderηη = 1.0F;
	}
	if (evoToggle && ofRandom(1.0F) < (ProbEvoChance / 100.0F))
	{
		probabilitySliderαα = probabilitySliderαα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαα.getMax() - probabilitySliderαα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαβ = probabilitySliderαβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαβ.getMax() - probabilitySliderαβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαγ = probabilitySliderαγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαγ.getMax() - probabilitySliderαγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαδ = probabilitySliderαδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαδ.getMax() - probabilitySliderαδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαε = probabilitySliderαε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαε.getMax() - probabilitySliderαε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαζ = probabilitySliderαζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαζ.getMax() - probabilitySliderαζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderαη = probabilitySliderαη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderαη.getMax() - probabilitySliderαη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderβα = probabilitySliderβα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβα.getMax() - probabilitySliderβα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderββ = probabilitySliderββ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderββ.getMax() - probabilitySliderββ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderβγ = probabilitySliderβγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβγ.getMax() - probabilitySliderβγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderβδ = probabilitySliderβδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβδ.getMax() - probabilitySliderβδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderβε = probabilitySliderβε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβε.getMax() - probabilitySliderβε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderβζ = probabilitySliderβζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβζ.getMax() - probabilitySliderβζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderβη = probabilitySliderβη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderβη.getMax() - probabilitySliderβη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderγα = probabilitySliderγα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγα.getMax() - probabilitySliderγα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγβ = probabilitySliderγβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγβ.getMax() - probabilitySliderγβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγγ = probabilitySliderγγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγγ.getMax() - probabilitySliderγγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγδ = probabilitySliderγδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγδ.getMax() - probabilitySliderγδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγε = probabilitySliderγε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγε.getMax() - probabilitySliderγε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγζ = probabilitySliderγζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγζ.getMax() - probabilitySliderγζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderγη = probabilitySliderγη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderγη.getMax() - probabilitySliderγη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderδα = probabilitySliderδα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδα.getMax() - probabilitySliderδα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδβ = probabilitySliderδβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδβ.getMax() - probabilitySliderδβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδγ = probabilitySliderδγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδγ.getMax() - probabilitySliderδγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδδ = probabilitySliderδδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδδ.getMax() - probabilitySliderδδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδε = probabilitySliderδε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδε.getMax() - probabilitySliderδε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδζ = probabilitySliderδζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδζ.getMax() - probabilitySliderδζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderδη = probabilitySliderδη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderδη.getMax() - probabilitySliderδη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderεα = probabilitySliderεα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεα.getMax() - probabilitySliderεα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεβ = probabilitySliderεβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεβ.getMax() - probabilitySliderεβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεγ = probabilitySliderεγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεγ.getMax() - probabilitySliderεγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεδ = probabilitySliderεδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεδ.getMax() - probabilitySliderεδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεε = probabilitySliderεε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεε.getMax() - probabilitySliderεε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεζ = probabilitySliderεζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεζ.getMax() - probabilitySliderεζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderεη = probabilitySliderεη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderεη.getMax() - probabilitySliderεη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderζα = probabilitySliderζα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζα.getMax() - probabilitySliderζα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζβ = probabilitySliderζβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζβ.getMax() - probabilitySliderζβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζγ = probabilitySliderζγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζγ.getMax() - probabilitySliderζγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζδ = probabilitySliderζδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζδ.getMax() - probabilitySliderζδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζε = probabilitySliderζε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζε.getMax() - probabilitySliderζε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζζ = probabilitySliderζζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζζ.getMax() - probabilitySliderζζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderζη = probabilitySliderζη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderζη.getMax() - probabilitySliderζη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySliderηα = probabilitySliderηα + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηα.getMax() - probabilitySliderηα.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηβ = probabilitySliderηβ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηβ.getMax() - probabilitySliderηβ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηγ = probabilitySliderηγ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηγ.getMax() - probabilitySliderηγ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηδ = probabilitySliderηδ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηδ.getMax() - probabilitySliderηδ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηε = probabilitySliderηε + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηε.getMax() - probabilitySliderηε.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηζ = probabilitySliderηζ + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηζ.getMax() - probabilitySliderηζ.getMin()) * (ProbEvoAmount / 100.0F));
		probabilitySliderηη = probabilitySliderηη + ((ofRandom(2.0F) - 1.0F) * (probabilitySliderηη.getMax() - probabilitySliderηη.getMin()) * (ProbEvoAmount / 100.0F));

		probabilitySlider = probabilitySlider + ((ofRandom(2.0F) - 1.0F) * (probabilitySlider.getMax() - probabilitySlider.getMin()) * (ProbEvoAmount / 100.0F));

		if (probabilitySlider < 0.0F) probabilitySlider = 0;
		if (probabilitySlider > 100.0F) probabilitySlider = 100.0F;
		if (probabilitySliderαα < 0.0F) probabilitySliderαα = 0;
		if (probabilitySliderαβ < 0.0F) probabilitySliderαβ = 0;
		if (probabilitySliderαγ < 0.0F) probabilitySliderαγ = 0;
		if (probabilitySliderαδ < 0.0F) probabilitySliderαδ = 0;
		if (probabilitySliderαε < 0.0F) probabilitySliderαε = 0;
		if (probabilitySliderαζ < 0.0F) probabilitySliderαζ = 0;
		if (probabilitySliderαη < 0.0F) probabilitySliderαη = 0;
		if (probabilitySliderαα > 100.0F) probabilitySliderαα = 100.0F;
		if (probabilitySliderαβ > 100.0F) probabilitySliderαβ = 100.0F;
		if (probabilitySliderαγ > 100.0F) probabilitySliderαγ = 100.0F;
		if (probabilitySliderαδ > 100.0F) probabilitySliderαδ = 100.0F;
		if (probabilitySliderαε > 100.0F) probabilitySliderαε = 100.0F;
		if (probabilitySliderαζ > 100.0F) probabilitySliderαζ = 100.0F;
		if (probabilitySliderαη > 100.0F) probabilitySliderαη = 100.0F;
		if (probabilitySliderβα < 0.0F) probabilitySliderβα = 0;
		if (probabilitySliderββ < 0.0F) probabilitySliderββ = 0;
		if (probabilitySliderβγ < 0.0F) probabilitySliderβγ = 0;
		if (probabilitySliderβδ < 0.0F) probabilitySliderβδ = 0;
		if (probabilitySliderβε < 0.0F) probabilitySliderβε = 0;
		if (probabilitySliderβζ < 0.0F) probabilitySliderβζ = 0;
		if (probabilitySliderβη < 0.0F) probabilitySliderβη = 0;
		if (probabilitySliderβα > 100.0F) probabilitySliderβα = 100.0F;
		if (probabilitySliderββ > 100.0F) probabilitySliderββ = 100.0F;
		if (probabilitySliderβγ > 100.0F) probabilitySliderβγ = 100.0F;
		if (probabilitySliderβδ > 100.0F) probabilitySliderβδ = 100.0F;
		if (probabilitySliderβε > 100.0F) probabilitySliderβε = 100.0F;
		if (probabilitySliderβζ > 100.0F) probabilitySliderβζ = 100.0F;
		if (probabilitySliderβη > 100.0F) probabilitySliderβη = 100.0F;		
		if (probabilitySliderγα < 0.0F) probabilitySliderγα = 0;
		if (probabilitySliderγβ < 0.0F) probabilitySliderγβ = 0;
		if (probabilitySliderγγ < 0.0F) probabilitySliderγγ = 0;
		if (probabilitySliderγδ < 0.0F) probabilitySliderγδ = 0;
		if (probabilitySliderγε < 0.0F) probabilitySliderγε = 0;
		if (probabilitySliderγζ < 0.0F) probabilitySliderγζ = 0;
		if (probabilitySliderγη < 0.0F) probabilitySliderγη = 0;
		if (probabilitySliderγα > 100.0F) probabilitySliderγα = 100.0F;
		if (probabilitySliderγβ > 100.0F) probabilitySliderγβ = 100.0F;
		if (probabilitySliderγγ > 100.0F) probabilitySliderγγ = 100.0F;
		if (probabilitySliderγδ > 100.0F) probabilitySliderγδ = 100.0F;
		if (probabilitySliderγε > 100.0F) probabilitySliderγε = 100.0F;
		if (probabilitySliderγζ > 100.0F) probabilitySliderγζ = 100.0F;
		if (probabilitySliderγη > 100.0F) probabilitySliderγη = 100.0F;	
		if (probabilitySliderδα < 0.0F) probabilitySliderδα = 0;
		if (probabilitySliderδβ < 0.0F) probabilitySliderδβ = 0;
		if (probabilitySliderδγ < 0.0F) probabilitySliderδγ = 0;
		if (probabilitySliderδδ < 0.0F) probabilitySliderδδ = 0;
		if (probabilitySliderδε < 0.0F) probabilitySliderδε = 0;
		if (probabilitySliderδζ < 0.0F) probabilitySliderδζ = 0;
		if (probabilitySliderδη < 0.0F) probabilitySliderδη = 0;
		if (probabilitySliderδα > 100.0F) probabilitySliderδα = 100.0F;
		if (probabilitySliderδβ > 100.0F) probabilitySliderδβ = 100.0F;
		if (probabilitySliderδγ > 100.0F) probabilitySliderδγ = 100.0F;
		if (probabilitySliderδδ > 100.0F) probabilitySliderδδ = 100.0F;
		if (probabilitySliderδε > 100.0F) probabilitySliderδε = 100.0F;
		if (probabilitySliderδζ > 100.0F) probabilitySliderδζ = 100.0F;
		if (probabilitySliderδη > 100.0F) probabilitySliderδη = 100.0F;		
		if (probabilitySliderεα < 0.0F) probabilitySliderεα = 0;
		if (probabilitySliderεβ < 0.0F) probabilitySliderεβ = 0;
		if (probabilitySliderεγ < 0.0F) probabilitySliderεγ = 0;
		if (probabilitySliderεδ < 0.0F) probabilitySliderεδ = 0;
		if (probabilitySliderεε < 0.0F) probabilitySliderεε = 0;
		if (probabilitySliderεζ < 0.0F) probabilitySliderεζ = 0;
		if (probabilitySliderεη < 0.0F) probabilitySliderεη = 0;
		if (probabilitySliderεα > 100.0F) probabilitySliderεα = 100.0F;
		if (probabilitySliderεβ > 100.0F) probabilitySliderεβ = 100.0F;
		if (probabilitySliderεγ > 100.0F) probabilitySliderεγ = 100.0F;
		if (probabilitySliderεδ > 100.0F) probabilitySliderεδ = 100.0F;
		if (probabilitySliderεε > 100.0F) probabilitySliderεε = 100.0F;
		if (probabilitySliderεζ > 100.0F) probabilitySliderεζ = 100.0F;
		if (probabilitySliderεη > 100.0F) probabilitySliderεη = 100.0F;		
		if (probabilitySliderζα < 0.0F) probabilitySliderζα = 0;
		if (probabilitySliderζβ < 0.0F) probabilitySliderζβ = 0;
		if (probabilitySliderζγ < 0.0F) probabilitySliderζγ = 0;
		if (probabilitySliderζδ < 0.0F) probabilitySliderζδ = 0;
		if (probabilitySliderζε < 0.0F) probabilitySliderζε = 0;
		if (probabilitySliderζζ < 0.0F) probabilitySliderζζ = 0;
		if (probabilitySliderζη < 0.0F) probabilitySliderζη = 0;
		if (probabilitySliderζα > 100.0F) probabilitySliderζα = 100.0F;
		if (probabilitySliderζβ > 100.0F) probabilitySliderζβ = 100.0F;
		if (probabilitySliderζγ > 100.0F) probabilitySliderζγ = 100.0F;
		if (probabilitySliderζδ > 100.0F) probabilitySliderζδ = 100.0F;
		if (probabilitySliderζε > 100.0F) probabilitySliderζε = 100.0F;
		if (probabilitySliderζζ > 100.0F) probabilitySliderζζ = 100.0F;
		if (probabilitySliderζη > 100.0F) probabilitySliderζη = 100.0F;		
		if (probabilitySliderηα < 0.0F) probabilitySliderηα = 0;
		if (probabilitySliderηβ < 0.0F) probabilitySliderηβ = 0;
		if (probabilitySliderηγ < 0.0F) probabilitySliderηγ = 0;
		if (probabilitySliderηδ < 0.0F) probabilitySliderηδ = 0;
		if (probabilitySliderηε < 0.0F) probabilitySliderηε = 0;
		if (probabilitySliderηζ < 0.0F) probabilitySliderηζ = 0;
		if (probabilitySliderηη < 0.0F) probabilitySliderηη = 0;
		if (probabilitySliderηα > 100.0F) probabilitySliderηα = 100.0F;
		if (probabilitySliderηβ > 100.0F) probabilitySliderηβ = 100.0F;
		if (probabilitySliderηγ > 100.0F) probabilitySliderηγ = 100.0F;
		if (probabilitySliderηδ > 100.0F) probabilitySliderηδ = 100.0F;
		if (probabilitySliderηε > 100.0F) probabilitySliderηε = 100.0F;
		if (probabilitySliderηζ > 100.0F) probabilitySliderηζ = 100.0F;
		if (probabilitySliderηη > 100.0F) probabilitySliderηη = 100.0F;		
	}

	if (numberSliderα > 0)
	{
		interaction(&alpha, &alpha, powerSliderαα, vSliderαα, viscosityαα, probabilityαα);
		if (numberSliderβ > 0) interaction(&alpha, &beta, powerSliderαβ, vSliderαβ, viscosityαβ, probabilityαβ);
		if (numberSliderγ > 0) interaction(&alpha, &gamma, powerSliderαγ, vSliderαγ, viscosityαγ, probabilityαγ);
		if (numberSliderδ > 0) interaction(&alpha, &elta, powerSliderαδ, vSliderαδ, viscosityαδ, probabilityαδ);
		if (numberSliderε > 0) interaction(&alpha, &epsilon, powerSliderαε, vSliderαε, viscosityαε, probabilityαε);
		if (numberSliderζ > 0) interaction(&alpha, &zeta, powerSliderαζ, vSliderαζ, viscosityαζ, probabilityαζ);
		if (numberSliderη > 0) interaction(&alpha, &eta, powerSliderαη, vSliderαη, viscosityαη, probabilityαη);
	}

	if (numberSliderβ > 0)
	{
		interaction(&beta, &beta, powerSliderββ, vSliderββ, viscosityββ, probabilityββ);
		if (numberSliderα > 0) interaction(&beta, &alpha, powerSliderβα, vSliderβα, viscosityβα, probabilityβα);
		if (numberSliderγ > 0) interaction(&beta, &gamma, powerSliderβγ, vSliderβγ, viscosityβγ, probabilityβγ);
		if (numberSliderδ > 0) interaction(&beta, &elta, powerSliderβδ, vSliderβδ, viscosityβδ, probabilityβδ);
		if (numberSliderε > 0) interaction(&beta, &epsilon, powerSliderβε, vSliderβε, viscosityβε, probabilityβε);
		if (numberSliderζ > 0) interaction(&beta, &zeta, powerSliderβζ, vSliderβζ, viscosityβζ, probabilityβζ);
		if (numberSliderη > 0) interaction(&beta, &eta, powerSliderβη, vSliderβη, viscosityβη, probabilityβη);
	}

	if (numberSliderγ > 0)
	{
		interaction(&gamma, &gamma, powerSliderγγ, vSliderγγ, viscosityγγ, probabilityγγ);
		if (numberSliderα > 0) interaction(&gamma, &alpha, powerSliderγα, vSliderγα, viscosityγα, probabilityγα);
		if (numberSliderβ > 0) interaction(&gamma, &beta, powerSliderγβ, vSliderγβ, viscosityγβ, probabilityγβ);
		if (numberSliderδ > 0) interaction(&gamma, &elta, powerSliderγδ, vSliderγδ, viscosityγδ, probabilityγδ);
		if (numberSliderε > 0) interaction(&gamma, &epsilon, powerSliderγε, vSliderγε, viscosityγε, probabilityγε);
		if (numberSliderζ > 0) interaction(&gamma, &zeta, powerSliderγζ, vSliderγζ, viscosityγζ, probabilityγζ);
		if (numberSliderη > 0) interaction(&gamma, &eta, powerSliderγη, vSliderγη, viscosityγη, probabilityγη);
	}

	if (numberSliderδ > 0)
	{
		interaction(&elta, &elta, powerSliderδδ, vSliderδδ, viscosityδδ, probabilityδδ);
		if (numberSliderα > 0) interaction(&elta, &alpha, powerSliderδα, vSliderδα, viscosityδα, probabilityδα);
		if (numberSliderβ > 0) interaction(&elta, &beta, powerSliderδβ, vSliderδβ, viscosityδβ, probabilityδβ);
		if (numberSliderγ > 0) interaction(&elta, &gamma, powerSliderδγ, vSliderδγ, viscosityδγ, probabilityδγ);
		if (numberSliderε > 0) interaction(&elta, &epsilon, powerSliderδε, vSliderδε, viscosityδε, probabilityδε);
		if (numberSliderζ > 0) interaction(&elta, &zeta, powerSliderδζ, vSliderδζ, viscosityδζ, probabilityδζ);
		if (numberSliderη > 0) interaction(&elta, &eta, powerSliderδη, vSliderδη, viscosityδη, probabilityδη);
	}

	if (numberSliderε > 0)
	{
		interaction(&epsilon, &epsilon, powerSliderεε, vSliderεε, viscosityεε, probabilityεε);
		if (numberSliderα > 0) interaction(&epsilon, &alpha, powerSliderεα, vSliderεα, viscosityεα, probabilityεα);
		if (numberSliderβ > 0) interaction(&epsilon, &beta, powerSliderεβ, vSliderεβ, viscosityεβ, probabilityεβ);
		if (numberSliderγ > 0) interaction(&epsilon, &gamma, powerSliderεγ, vSliderεγ, viscosityεγ, probabilityεγ);
		if (numberSliderδ > 0) interaction(&epsilon, &elta, powerSliderεδ, vSliderεδ, viscosityεδ, probabilityεδ);
		if (numberSliderζ > 0) interaction(&epsilon, &zeta, powerSliderεζ, vSliderεζ, viscosityεζ, probabilityεζ);
		if (numberSliderη > 0) interaction(&epsilon, &eta, powerSliderεη, vSliderεη, viscosityεη, probabilityεη);
	}

	if (numberSliderζ > 0)
	{
		interaction(&zeta, &zeta, powerSliderζζ, vSliderζζ, viscosityζζ, probabilityζζ);
		if (numberSliderα > 0) interaction(&zeta, &alpha, powerSliderζα, vSliderζα, viscosityζα, probabilityζα);
		if (numberSliderβ > 0) interaction(&zeta, &beta, powerSliderζβ, vSliderζβ, viscosityζβ, probabilityζβ);
		if (numberSliderγ > 0) interaction(&zeta, &gamma, powerSliderζγ, vSliderζγ, viscosityζγ, probabilityζγ);
		if (numberSliderδ > 0) interaction(&zeta, &elta, powerSliderζδ, vSliderζδ, viscosityζδ, probabilityζδ);
		if (numberSliderε > 0) interaction(&zeta, &epsilon, powerSliderζε, vSliderζε, viscosityζε, probabilityζε);
		if (numberSliderη > 0) interaction(&zeta, &eta, powerSliderζη, vSliderζη, viscosityζη, probabilityζη);
	}

	if (numberSliderη > 0)
	{
		interaction(&eta, &eta, powerSliderηη, vSliderηη, viscosityηη, probabilityηη);
		if (numberSliderα > 0) interaction(&eta, &alpha, powerSliderηα, vSliderηα, viscosityηα, probabilityηα);
		if (numberSliderβ > 0) interaction(&eta, &beta, powerSliderηβ, vSliderηβ, viscosityηβ, probabilityηβ);
		if (numberSliderγ > 0) interaction(&eta, &gamma, powerSliderηγ, vSliderηγ, viscosityηγ, probabilityηγ);
		if (numberSliderδ > 0) interaction(&eta, &elta, powerSliderηδ, vSliderηδ, viscosityηδ, probabilityηδ);
		if (numberSliderε > 0) interaction(&eta, &epsilon, powerSliderηε, vSliderηε, viscosityηε, probabilityηε);
		if (numberSliderζ > 0) interaction(&eta, &zeta, powerSliderηζ, vSliderηζ, viscosityηζ, probabilityηζ);
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
	if (numberSliderα > 0) { Draw(&alpha); }
	if (numberSliderβ > 0) { Draw(&beta); }
	if (numberSliderδ > 0) { Draw(&elta); }
	if (numberSliderγ > 0) { Draw(&gamma); }
	if (numberSliderε > 0) { Draw(&epsilon); }
	if (numberSliderζ > 0) { Draw(&zeta); }
	if (numberSliderη > 0) { Draw(&eta); }
	if (numberSliderα < 0.0F) numberSliderα = 0;
	if (numberSliderβ < 0.0F) numberSliderβ = 0;
	if (numberSliderδ < 0.0F) numberSliderδ = 0;
	if (numberSliderγ < 0.0F) numberSliderγ = 0;
	if (numberSliderε < 0.0F) numberSliderε = 0;
	if (numberSliderζ < 0.0F) numberSliderζ = 0;
	if (numberSliderη < 0.0F) numberSliderη = 0;

	//Draw GUI
	//if (modelToggle == true)
	//{
		//ofSetColor(0, 0, 0);
		//ofDrawCircle(xshift, yshift, 150);

		//ofSetLineWidth(5);
		//ofSetColor(150.0F - powerSliderGR, 150.0F + powerSliderGR, 150);
		//ofDrawLine(p1x, p1y - 10, p2x, p2y - 10);
		//ofSetColor(150.0F - powerSliderRG, 150.0F + powerSliderRG, 150);
		//ofDrawLine(p1x, p1y + 10, p2x, p2y + 10);
		//ofSetColor(150.0F - powerSliderGW, 150.0F + powerSliderGW, 150);
		//ofDrawLine(p3x, p3y - 10, p1x, p1y - 10);
		//ofSetColor(150.0F - powerSliderWG, 150.0F + powerSliderWG, 150);
		//ofDrawLine(p3x, p3y + 10, p1x, p1y + 10);

		//ofSetColor(150.0F - powerSliderGB, 150.0F + powerSliderGB, 150);
		//ofDrawLine(p4x - 10, p4y, p1x - 10, p1y);
		//ofSetColor(150.0F - powerSliderBG, 150.0F + powerSliderBG, 150);
		//ofDrawLine(p4x + 10, p4y, p1x + 10, p1y);

		//ofSetColor(150.0F - powerSliderRW, 150.0F + powerSliderRW, 150);
		//ofDrawLine(p2x - 10, p2y, p3x - 10, p3y);
		//ofSetColor(150.0F - powerSliderWR, 150.0F + powerSliderWR, 150);
		//ofDrawLine(p2x + 10, p2y, p3x + 10, p3y);

		//ofSetColor(150.0F - powerSliderRB, 150.0F + powerSliderRB, 150);
		//ofDrawLine(p2x, p2y - 10, p4x, p4y - 10);
		//ofSetColor(150.0F - powerSliderBR, 150.0F + powerSliderBR, 150);
		//ofDrawLine(p2x, p2y + 10, p4x, p4y + 10);

		//ofSetColor(150.0F - powerSliderWB, 150.0F + powerSliderWB, 150);
		//ofDrawLine(p3x, p3y - 10, p4x, p4y - 10);
		//ofSetColor(150.0F - powerSliderBW, 150.0F + powerSliderBW, 150);
		//ofDrawLine(p3x, p3y + 10, p4x, p4y + 10);

		//ofNoFill();
		//ofSetColor(150.0F - powerSliderGG, 150.0F + powerSliderGG, 150);
		//ofDrawCircle(p1x - 20, p1y - 20, rr + 20);
		//ofSetColor(150.0F - powerSliderRR, 150.0F + powerSliderRR, 150);
		//ofDrawCircle(p2x + 20, p2y - 20, rr + 20);
		//ofSetColor(150.0F - powerSliderWW, 150.0F + powerSliderWW, 150);
		//ofDrawCircle(p3x + 20, p3y + 20, rr + 20);
		//ofSetColor(150.0F - powerSliderBB, 150.0F + powerSliderBB, 150);
		//ofDrawCircle(p4x - 20, p4y + 20, rr + 20);

		//ofFill();
		//ofSetColor(100, 250, 10);
		//ofDrawCircle(p1x, p1y, rr);
		//ofSetColor(250, 10, 100);
		//ofDrawCircle(p2x, p2y, rr);
		//ofSetColor(250, 250, 250);
		//ofDrawCircle(p3x, p3y, rr);
		//ofSetColor(100, 100, 250);
		//ofDrawCircle(p4x, p4y, rr);
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
