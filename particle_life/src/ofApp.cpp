#include "ofApp.h"
#include "ofUtils.h"

#include <iostream>
#include <vector>
#include <random>

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
std::vector<point> betha;
std::vector<point> elta;
std::vector<point> gamma;
std::vector<point> epsilon;
std::vector<point> zeta;
std::vector<point> eta;
std::vector<point> teta;


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
	if (numberSliderα > 0) { alpha = CreatePoints(numberSliderα, 0, 0, ofRandom(64, 255)); }
	if (numberSliderβ > 0) { betha = CreatePoints(numberSliderβ, 0, ofRandom(64, 255), 0); }
	if (numberSliderγ > 0) { gamma = CreatePoints(numberSliderγ, ofRandom(64, 255), 0, 0); }
	if (numberSliderδ > 0) { elta = CreatePoints(numberSliderδ, ofRandom(64, 255), ofRandom(64, 255), 0); }
	if (numberSliderε > 0) { epsilon = CreatePoints(numberSliderε, ofRandom(64, 255), 0, ofRandom(64, 255)); }
	if (numberSliderζ > 0) { zeta = CreatePoints(numberSliderζ, 0, ofRandom(64, 255), ofRandom(64, 255)); }
	if (numberSliderη > 0) { eta = CreatePoints(numberSliderη, ofRandom(64, 255), ofRandom(64, 255), ofRandom(64, 255)); }
	if (numberSliderθ > 0) { teta = CreatePoints(numberSliderθ, 0, 0, 0); }
}

void ofApp::attractDark()
{
	// Tetha

	probabilitySliderθα = 30;
	probabilitySliderθβ = 30;
	probabilitySliderθγ = 30;
	probabilitySliderθδ = 30;
	probabilitySliderθε = 30;
	probabilitySliderθζ = 30;
	probabilitySliderθη = 30;

	viscositySliderθα = 30;
	viscositySliderθβ = 30;
	viscositySliderθδ = 30;
	viscositySliderθγ = 30;
	viscositySliderθε = 30;
	viscositySliderθζ = 30;
	viscositySliderθη = 30;

	powerSliderθγ = 10;
	powerSliderθδ = 10;
	powerSliderθα = 10;
	powerSliderθβ = 10;
	powerSliderθε = 10;
	powerSliderθζ = 10;
	powerSliderθη = 10;
	powerSliderθθ = 0;

	vSliderθβ = 300;
	vSliderθα = 300;
	vSliderθδ = 300;
	vSliderθγ = 300;
	vSliderθε = 300;
	vSliderθζ = 300;
	vSliderθη = 300;
	vSliderθθ = 0;

	powerSliderγθ = 0;
	powerSliderδθ = 0;
	powerSliderαθ = 0;
	powerSliderβθ = 0;
	powerSliderεθ = 0;
	powerSliderζθ = 0;
	powerSliderηθ = 0;

	vSliderβθ = 0;
	vSliderαθ = 0;
	vSliderδθ = 0;
	vSliderγθ = 0;
	vSliderεθ = 0;
	vSliderζθ = 0;
	vSliderηθ = 0;
}

void ofApp::repulseDark()
{
	// Tetha

	probabilitySliderθα = 30;
	probabilitySliderθβ = 30;
	probabilitySliderθγ = 30;
	probabilitySliderθδ = 30;
	probabilitySliderθε = 30;
	probabilitySliderθζ = 30;
	probabilitySliderθη = 30;

	viscositySliderθα = 30;
	viscositySliderθβ = 30;
	viscositySliderθδ = 30;
	viscositySliderθγ = 30;
	viscositySliderθε = 30;
	viscositySliderθζ = 30;
	viscositySliderθη = 30;

	powerSliderθγ = -10;
	powerSliderθδ = -10;
	powerSliderθα = -10;
	powerSliderθβ = -10;
	powerSliderθε = -10;
	powerSliderθζ = -10;
	powerSliderθη = -10;
	powerSliderθθ = 0;

	vSliderθβ = 300;
	vSliderθα = 300;
	vSliderθδ = 300;
	vSliderθγ = 300;
	vSliderθε = 300;
	vSliderθζ = 300;
	vSliderθη = 300;
	vSliderθθ = 0;

	powerSliderγθ = 0;
	powerSliderδθ = 0;
	powerSliderαθ = 0;
	powerSliderβθ = 0;
	powerSliderεθ = 0;
	powerSliderζθ = 0;
	powerSliderηθ = 0;

	vSliderβθ = 0;
	vSliderαθ = 0;
	vSliderδθ = 0;
	vSliderγθ = 0;
	vSliderεθ = 0;
	vSliderζθ = 0;
	vSliderηθ = 0;
}

void ofApp::rndDark()
{
	// Tetha
	
	probabilitySliderαθ = RandomFloat(minI, maxI);
	probabilitySliderβθ = RandomFloat(minI, maxI);
	probabilitySliderγθ = RandomFloat(minI, maxI);
	probabilitySliderδθ = RandomFloat(minI, maxI);
	probabilitySliderεθ = RandomFloat(minI, maxI);
	probabilitySliderζθ = RandomFloat(minI, maxI);
	probabilitySliderηθ = RandomFloat(minI, maxI);
	probabilitySliderθα = RandomFloat(minI, maxI);
	probabilitySliderθβ = RandomFloat(minI, maxI);
	probabilitySliderθγ = RandomFloat(minI, maxI);
	probabilitySliderθδ = RandomFloat(minI, maxI);
	probabilitySliderθε = RandomFloat(minI, maxI);
	probabilitySliderθζ = RandomFloat(minI, maxI);
	probabilitySliderθη = RandomFloat(minI, maxI);
	probabilitySliderθθ = RandomFloat(minI, maxI);

	viscositySliderαθ = RandomFloat(minV, maxV);
	viscositySliderβθ = RandomFloat(minV, maxV);
	viscositySliderδθ = RandomFloat(minV, maxV);
	viscositySliderγθ = RandomFloat(minV, maxV);
	viscositySliderεθ = RandomFloat(minV, maxV);
	viscositySliderζθ = RandomFloat(minV, maxV);
	viscositySliderηθ = RandomFloat(minV, maxV);
	viscositySliderθα = RandomFloat(minV, maxV);
	viscositySliderθβ = RandomFloat(minV, maxV);
	viscositySliderθδ = RandomFloat(minV, maxV);
	viscositySliderθγ = RandomFloat(minV, maxV);
	viscositySliderθε = RandomFloat(minV, maxV);
	viscositySliderθζ = RandomFloat(minV, maxV);
	viscositySliderθη = RandomFloat(minV, maxV);
	viscositySliderθθ = RandomFloat(minV, maxV);

	powerSliderθγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθη = RandomFloat(minP, maxP) * forceVariance;

	vSliderθβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθη = RandomFloat(minR, maxR) * radiusVariance;

	powerSliderγθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθθ = RandomFloat(minP, maxP) * forceVariance;

	vSliderβθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθθ = RandomFloat(minR, maxR) * radiusVariance;
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
	numberSliderθ = RandomInt(1000, 4000);

	viscositySlider = RandomFloat(minV, maxV);

	viscositySliderαα = RandomFloat(minV, maxV);
	viscositySliderαβ = RandomFloat(minV, maxV);
	viscositySliderαδ = RandomFloat(minV, maxV);
	viscositySliderαγ = RandomFloat(minV, maxV);
	viscositySliderαε = RandomFloat(minV, maxV);
	viscositySliderαζ = RandomFloat(minV, maxV);
	viscositySliderαη = RandomFloat(minV, maxV);

	viscositySliderβα = RandomFloat(minV, maxV);
	viscositySliderββ = RandomFloat(minV, maxV);
	viscositySliderβδ = RandomFloat(minV, maxV);
	viscositySliderβγ = RandomFloat(minV, maxV);
	viscositySliderβε = RandomFloat(minV, maxV);
	viscositySliderβζ = RandomFloat(minV, maxV);
	viscositySliderβη = RandomFloat(minV, maxV);

	viscositySliderγα = RandomFloat(minV, maxV);
	viscositySliderγβ = RandomFloat(minV, maxV);
	viscositySliderγδ = RandomFloat(minV, maxV);
	viscositySliderγγ = RandomFloat(minV, maxV);
	viscositySliderγε = RandomFloat(minV, maxV);
	viscositySliderγζ = RandomFloat(minV, maxV);
	viscositySliderγη = RandomFloat(minV, maxV);

	viscositySliderδα = RandomFloat(minV, maxV);
	viscositySliderδβ = RandomFloat(minV, maxV);
	viscositySliderδδ = RandomFloat(minV, maxV);
	viscositySliderδγ = RandomFloat(minV, maxV);
	viscositySliderδε = RandomFloat(minV, maxV);
	viscositySliderδζ = RandomFloat(minV, maxV);
	viscositySliderδη = RandomFloat(minV, maxV);

	viscositySliderεα = RandomFloat(minV, maxV);
	viscositySliderεβ = RandomFloat(minV, maxV);
	viscositySliderεδ = RandomFloat(minV, maxV);
	viscositySliderεγ = RandomFloat(minV, maxV);
	viscositySliderεε = RandomFloat(minV, maxV);
	viscositySliderεζ = RandomFloat(minV, maxV);
	viscositySliderεη = RandomFloat(minV, maxV);

	viscositySliderζα = RandomFloat(minV, maxV);
	viscositySliderζβ = RandomFloat(minV, maxV);
	viscositySliderζδ = RandomFloat(minV, maxV);
	viscositySliderζγ = RandomFloat(minV, maxV);
	viscositySliderζε = RandomFloat(minV, maxV);
	viscositySliderζζ = RandomFloat(minV, maxV);
	viscositySliderζη = RandomFloat(minV, maxV);

	viscositySliderηα = RandomFloat(minV, maxV);
	viscositySliderηβ = RandomFloat(minV, maxV);
	viscositySliderηδ = RandomFloat(minV, maxV);
	viscositySliderηγ = RandomFloat(minV, maxV);
	viscositySliderηε = RandomFloat(minV, maxV);
	viscositySliderηζ = RandomFloat(minV, maxV);
	viscositySliderηη = RandomFloat(minV, maxV);

	viscositySliderαθ = RandomFloat(minV, maxV);
	viscositySliderβθ = RandomFloat(minV, maxV);
	viscositySliderδθ = RandomFloat(minV, maxV);
	viscositySliderγθ = RandomFloat(minV, maxV);
	viscositySliderεθ = RandomFloat(minV, maxV);
	viscositySliderζθ = RandomFloat(minV, maxV);
	viscositySliderηθ = RandomFloat(minV, maxV);
	viscositySliderθα = RandomFloat(minV, maxV);
	viscositySliderθβ = RandomFloat(minV, maxV);
	viscositySliderθδ = RandomFloat(minV, maxV);
	viscositySliderθγ = RandomFloat(minV, maxV);
	viscositySliderθε = RandomFloat(minV, maxV);
	viscositySliderθζ = RandomFloat(minV, maxV);
	viscositySliderθη = RandomFloat(minV, maxV);
	viscositySliderθθ = RandomFloat(minV, maxV);

	probabilitySlider = RandomFloat(minI, maxI);

	probabilitySliderαα = RandomFloat(minI, maxI);
	probabilitySliderαβ = RandomFloat(minI, maxI);
	probabilitySliderαγ = RandomFloat(minI, maxI);
	probabilitySliderαδ = RandomFloat(minI, maxI);
	probabilitySliderαε = RandomFloat(minI, maxI);
	probabilitySliderαζ = RandomFloat(minI, maxI);
	probabilitySliderαη = RandomFloat(minI, maxI);

	probabilitySliderβα = RandomFloat(minI, maxI);
	probabilitySliderββ = RandomFloat(minI, maxI);
	probabilitySliderβγ = RandomFloat(minI, maxI);
	probabilitySliderβδ = RandomFloat(minI, maxI);
	probabilitySliderβε = RandomFloat(minI, maxI);
	probabilitySliderβζ = RandomFloat(minI, maxI);
	probabilitySliderβη = RandomFloat(minI, maxI);

	probabilitySliderγα = RandomFloat(minI, maxI);
	probabilitySliderγβ = RandomFloat(minI, maxI);
	probabilitySliderγγ = RandomFloat(minI, maxI);
	probabilitySliderγδ = RandomFloat(minI, maxI);
	probabilitySliderγε = RandomFloat(minI, maxI);
	probabilitySliderγζ = RandomFloat(minI, maxI);
	probabilitySliderγη = RandomFloat(minI, maxI);

	probabilitySliderδα = RandomFloat(minI, maxI);
	probabilitySliderδβ = RandomFloat(minI, maxI);
	probabilitySliderδγ = RandomFloat(minI, maxI);
	probabilitySliderδδ = RandomFloat(minI, maxI);
	probabilitySliderδε = RandomFloat(minI, maxI);
	probabilitySliderδζ = RandomFloat(minI, maxI);
	probabilitySliderδη = RandomFloat(minI, maxI);

	probabilitySliderεα = RandomFloat(minI, maxI);
	probabilitySliderεβ = RandomFloat(minI, maxI);
	probabilitySliderεγ = RandomFloat(minI, maxI);
	probabilitySliderεδ = RandomFloat(minI, maxI);
	probabilitySliderεε = RandomFloat(minI, maxI);
	probabilitySliderεζ = RandomFloat(minI, maxI);
	probabilitySliderεη = RandomFloat(minI, maxI);

	probabilitySliderζα = RandomFloat(minI, maxI);
	probabilitySliderζβ = RandomFloat(minI, maxI);
	probabilitySliderζγ = RandomFloat(minI, maxI);
	probabilitySliderζδ = RandomFloat(minI, maxI);
	probabilitySliderζε = RandomFloat(minI, maxI);
	probabilitySliderζζ = RandomFloat(minI, maxI);
	probabilitySliderζη = RandomFloat(minI, maxI);

	probabilitySliderηα = RandomFloat(minI, maxI);
	probabilitySliderηβ = RandomFloat(minI, maxI);
	probabilitySliderηγ = RandomFloat(minI, maxI);
	probabilitySliderηδ = RandomFloat(minI, maxI);
	probabilitySliderηε = RandomFloat(minI, maxI);
	probabilitySliderηζ = RandomFloat(minI, maxI);
	probabilitySliderηη = RandomFloat(minI, maxI);

	probabilitySliderαθ = RandomFloat(minI, maxI);
	probabilitySliderβθ = RandomFloat(minI, maxI);
	probabilitySliderγθ = RandomFloat(minI, maxI);
	probabilitySliderδθ = RandomFloat(minI, maxI);
	probabilitySliderεθ = RandomFloat(minI, maxI);
	probabilitySliderζθ = RandomFloat(minI, maxI);
	probabilitySliderηθ = RandomFloat(minI, maxI);
	probabilitySliderθα = RandomFloat(minI, maxI);
	probabilitySliderθβ = RandomFloat(minI, maxI);
	probabilitySliderθγ = RandomFloat(minI, maxI);
	probabilitySliderθδ = RandomFloat(minI, maxI);
	probabilitySliderθε = RandomFloat(minI, maxI);
	probabilitySliderθζ = RandomFloat(minI, maxI);
	probabilitySliderθη = RandomFloat(minI, maxI);
	probabilitySliderθθ = RandomFloat(minI, maxI);

	// Alpha
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

	// Betha
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

	// Gamma
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

	// Delta
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

	// Epsilon
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

	// Zeta
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

	// Etha
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

	// Tetha
	//numberSliderθ = RandomFloat(minR, 3000);
	powerSliderθγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθη = RandomFloat(minP, maxP) * forceVariance;

	vSliderθβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθη = RandomFloat(minR, maxR) * radiusVariance;

	powerSliderγθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθθ = RandomFloat(minP, maxP) * forceVariance;

	vSliderβθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθθ = RandomFloat(minR, maxR) * radiusVariance;
}

void ofApp::monads() {
	numberSliderβ = RandomInt(500, 2500);
	numberSliderα = RandomInt(500, 2500);
	numberSliderδ = RandomInt(500, 2500);
	numberSliderγ = RandomInt(500, 2500);
	numberSliderε = RandomInt(500, 2500);
	numberSliderζ = RandomInt(500, 2500);
	numberSliderη = RandomInt(500, 2500);
	numberSliderθ = RandomInt(1000, 4000);
}
void ofApp::rndvsc() {
	viscositySlider = RandomFloat(minV, maxV);

	viscositySliderαα = RandomFloat(minV, maxV);
	viscositySliderαβ = RandomFloat(minV, maxV);
	viscositySliderαδ = RandomFloat(minV, maxV);
	viscositySliderαγ = RandomFloat(minV, maxV);
	viscositySliderαε = RandomFloat(minV, maxV);
	viscositySliderαζ = RandomFloat(minV, maxV);
	viscositySliderαη = RandomFloat(minV, maxV);

	viscositySliderβα = RandomFloat(minV, maxV);
	viscositySliderββ = RandomFloat(minV, maxV);
	viscositySliderβδ = RandomFloat(minV, maxV);
	viscositySliderβγ = RandomFloat(minV, maxV);
	viscositySliderβε = RandomFloat(minV, maxV);
	viscositySliderβζ = RandomFloat(minV, maxV);
	viscositySliderβη = RandomFloat(minV, maxV);

	viscositySliderγα = RandomFloat(minV, maxV);
	viscositySliderγβ = RandomFloat(minV, maxV);
	viscositySliderγδ = RandomFloat(minV, maxV);
	viscositySliderγγ = RandomFloat(minV, maxV);
	viscositySliderγε = RandomFloat(minV, maxV);
	viscositySliderγζ = RandomFloat(minV, maxV);
	viscositySliderγη = RandomFloat(minV, maxV);

	viscositySliderδα = RandomFloat(minV, maxV);
	viscositySliderδβ = RandomFloat(minV, maxV);
	viscositySliderδδ = RandomFloat(minV, maxV);
	viscositySliderδγ = RandomFloat(minV, maxV);
	viscositySliderδε = RandomFloat(minV, maxV);
	viscositySliderδζ = RandomFloat(minV, maxV);
	viscositySliderδη = RandomFloat(minV, maxV);

	viscositySliderεα = RandomFloat(minV, maxV);
	viscositySliderεβ = RandomFloat(minV, maxV);
	viscositySliderεδ = RandomFloat(minV, maxV);
	viscositySliderεγ = RandomFloat(minV, maxV);
	viscositySliderεε = RandomFloat(minV, maxV);
	viscositySliderεζ = RandomFloat(minV, maxV);
	viscositySliderεη = RandomFloat(minV, maxV);

	viscositySliderζα = RandomFloat(minV, maxV);
	viscositySliderζβ = RandomFloat(minV, maxV);
	viscositySliderζδ = RandomFloat(minV, maxV);
	viscositySliderζγ = RandomFloat(minV, maxV);
	viscositySliderζε = RandomFloat(minV, maxV);
	viscositySliderζζ = RandomFloat(minV, maxV);
	viscositySliderζη = RandomFloat(minV, maxV);

	viscositySliderηα = RandomFloat(minV, maxV);
	viscositySliderηβ = RandomFloat(minV, maxV);
	viscositySliderηδ = RandomFloat(minV, maxV);
	viscositySliderηγ = RandomFloat(minV, maxV);
	viscositySliderηε = RandomFloat(minV, maxV);
	viscositySliderηζ = RandomFloat(minV, maxV);
	viscositySliderηη = RandomFloat(minV, maxV);

	viscositySliderαθ = RandomFloat(minV, maxV);
	viscositySliderβθ = RandomFloat(minV, maxV);
	viscositySliderδθ = RandomFloat(minV, maxV);
	viscositySliderγθ = RandomFloat(minV, maxV);
	viscositySliderεθ = RandomFloat(minV, maxV);
	viscositySliderζθ = RandomFloat(minV, maxV);
	viscositySliderηθ = RandomFloat(minV, maxV);
	viscositySliderθα = RandomFloat(minV, maxV);
	viscositySliderθβ = RandomFloat(minV, maxV);
	viscositySliderθδ = RandomFloat(minV, maxV);
	viscositySliderθγ = RandomFloat(minV, maxV);
	viscositySliderθε = RandomFloat(minV, maxV);
	viscositySliderθζ = RandomFloat(minV, maxV);
	viscositySliderθη = RandomFloat(minV, maxV);
	viscositySliderθθ = RandomFloat(minV, maxV);
}
void ofApp::rndprob() {
	probabilitySlider = RandomFloat(minI, maxI);

	probabilitySliderαα = RandomFloat(minI, maxI);
	probabilitySliderαβ = RandomFloat(minI, maxI);
	probabilitySliderαγ = RandomFloat(minI, maxI);
	probabilitySliderαδ = RandomFloat(minI, maxI);
	probabilitySliderαε = RandomFloat(minI, maxI);
	probabilitySliderαζ = RandomFloat(minI, maxI);
	probabilitySliderαη = RandomFloat(minI, maxI);

	probabilitySliderβα = RandomFloat(minI, maxI);
	probabilitySliderββ = RandomFloat(minI, maxI);
	probabilitySliderβγ = RandomFloat(minI, maxI);
	probabilitySliderβδ = RandomFloat(minI, maxI);
	probabilitySliderβε = RandomFloat(minI, maxI);
	probabilitySliderβζ = RandomFloat(minI, maxI);
	probabilitySliderβη = RandomFloat(minI, maxI);

	probabilitySliderγα = RandomFloat(minI, maxI);
	probabilitySliderγβ = RandomFloat(minI, maxI);
	probabilitySliderγγ = RandomFloat(minI, maxI);
	probabilitySliderγδ = RandomFloat(minI, maxI);
	probabilitySliderγε = RandomFloat(minI, maxI);
	probabilitySliderγζ = RandomFloat(minI, maxI);
	probabilitySliderγη = RandomFloat(minI, maxI);

	probabilitySliderδα = RandomFloat(minI, maxI);
	probabilitySliderδβ = RandomFloat(minI, maxI);
	probabilitySliderδγ = RandomFloat(minI, maxI);
	probabilitySliderδδ = RandomFloat(minI, maxI);
	probabilitySliderδε = RandomFloat(minI, maxI);
	probabilitySliderδζ = RandomFloat(minI, maxI);
	probabilitySliderδη = RandomFloat(minI, maxI);

	probabilitySliderεα = RandomFloat(minI, maxI);
	probabilitySliderεβ = RandomFloat(minI, maxI);
	probabilitySliderεγ = RandomFloat(minI, maxI);
	probabilitySliderεδ = RandomFloat(minI, maxI);
	probabilitySliderεε = RandomFloat(minI, maxI);
	probabilitySliderεζ = RandomFloat(minI, maxI);
	probabilitySliderεη = RandomFloat(minI, maxI);

	probabilitySliderζα = RandomFloat(minI, maxI);
	probabilitySliderζβ = RandomFloat(minI, maxI);
	probabilitySliderζγ = RandomFloat(minI, maxI);
	probabilitySliderζδ = RandomFloat(minI, maxI);
	probabilitySliderζε = RandomFloat(minI, maxI);
	probabilitySliderζζ = RandomFloat(minI, maxI);
	probabilitySliderζη = RandomFloat(minI, maxI);

	probabilitySliderηα = RandomFloat(minI, maxI);
	probabilitySliderηβ = RandomFloat(minI, maxI);
	probabilitySliderηγ = RandomFloat(minI, maxI);
	probabilitySliderηδ = RandomFloat(minI, maxI);
	probabilitySliderηε = RandomFloat(minI, maxI);
	probabilitySliderηζ = RandomFloat(minI, maxI);
	probabilitySliderηη = RandomFloat(minI, maxI);

	probabilitySliderαθ = RandomFloat(minI, maxI);
	probabilitySliderβθ = RandomFloat(minI, maxI);
	probabilitySliderγθ = RandomFloat(minI, maxI);
	probabilitySliderδθ = RandomFloat(minI, maxI);
	probabilitySliderεθ = RandomFloat(minI, maxI);
	probabilitySliderζθ = RandomFloat(minI, maxI);
	probabilitySliderηθ = RandomFloat(minI, maxI);
	probabilitySliderθα = RandomFloat(minI, maxI);
	probabilitySliderθβ = RandomFloat(minI, maxI);
	probabilitySliderθγ = RandomFloat(minI, maxI);
	probabilitySliderθδ = RandomFloat(minI, maxI);
	probabilitySliderθε = RandomFloat(minI, maxI);
	probabilitySliderθζ = RandomFloat(minI, maxI);
	probabilitySliderθη = RandomFloat(minI, maxI);
	probabilitySliderθθ = RandomFloat(minI, maxI);
}
void ofApp::rndir() {
	// Alpha
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

	// Betha
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

	// Gamma
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

	// Delta
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

	// Epsilon
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

	// Zeta
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

	// Etha
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

	// Tetha
	//numberSliderθ = RandomFloat(minR, 3000);
	powerSliderθγ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθδ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθα = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθβ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθε = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθζ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθη = RandomFloat(minP, maxP) * forceVariance;

	vSliderθβ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθα = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθδ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθγ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθε = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθζ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθη = RandomFloat(minR, maxR) * radiusVariance;

	powerSliderγθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderδθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderαθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderβθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderεθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderζθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderηθ = RandomFloat(minP, maxP) * forceVariance;
	powerSliderθθ = RandomFloat(minP, maxP) * forceVariance;

	vSliderβθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderαθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderδθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderγθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderεθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderζθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderηθ = RandomFloat(minR, maxR) * radiusVariance;
	vSliderθθ = RandomFloat(minR, maxR) * radiusVariance;
}

void ofApp::freeze() {
	InteractionEvoProbSlider = 0.3;
	InteractionEvoAmountSlider = 0.3;
	ProbabilityEvoProbSlider = 0.2;
	ProbabilityEvoAmountSlider = 0.2;
	ViscosityEvoProbSlider = 0.1;
	ViscosityEvoAmountSlider = 0.1;

	viscositySlider = 100;

	viscositySliderαα = 100;
	viscositySliderαβ = 100;
	viscositySliderαδ = 100;
	viscositySliderαγ = 100;
	viscositySliderαε = 100;
	viscositySliderαζ = 100;
	viscositySliderαη = 100;

	viscositySliderβα = 100;
	viscositySliderββ = 100;
	viscositySliderβδ = 100;
	viscositySliderβγ = 100;
	viscositySliderβε = 100;
	viscositySliderβζ = 100;
	viscositySliderβη = 100;

	viscositySliderγα = 100;
	viscositySliderγβ = 100;
	viscositySliderγδ = 100;
	viscositySliderγγ = 100;
	viscositySliderγε = 100;
	viscositySliderγζ = 100;
	viscositySliderγη = 100;

	viscositySliderδα = 100;
	viscositySliderδβ = 100;
	viscositySliderδδ = 100;
	viscositySliderδγ = 100;
	viscositySliderδε = 100;
	viscositySliderδζ = 100;
	viscositySliderδη = 100;

	viscositySliderεα = 100;
	viscositySliderεβ = 100;
	viscositySliderεδ = 100;
	viscositySliderεγ = 100;
	viscositySliderεε = 100;
	viscositySliderεζ = 100;
	viscositySliderεη = 100;

	viscositySliderζα = 100;
	viscositySliderζβ = 100;
	viscositySliderζδ = 100;
	viscositySliderζγ = 100;
	viscositySliderζε = 100;
	viscositySliderζζ = 100;
	viscositySliderζη = 100;

	viscositySliderηα = 100;
	viscositySliderηβ = 100;
	viscositySliderηδ = 100;
	viscositySliderηγ = 100;
	viscositySliderηε = 100;
	viscositySliderηζ = 100;
	viscositySliderηη = 100;

	viscositySliderαθ = 100;
	viscositySliderβθ = 100;
	viscositySliderδθ = 100;
	viscositySliderγθ = 100;
	viscositySliderεθ = 100;
	viscositySliderζθ = 100;
	viscositySliderηθ = 100;
	viscositySliderθα = 100;
	viscositySliderθβ = 100;
	viscositySliderθδ = 100;
	viscositySliderθγ = 100;
	viscositySliderθε = 100;
	viscositySliderθζ = 100;
	viscositySliderθη = 100;
	viscositySliderθθ = 100;

	probabilitySlider = 0;

	probabilitySliderαα = 0;
	probabilitySliderαβ = 0;
	probabilitySliderαγ = 0;
	probabilitySliderαδ = 0;
	probabilitySliderαε = 0;
	probabilitySliderαζ = 0;
	probabilitySliderαη = 0;

	probabilitySliderβα = 0;
	probabilitySliderββ = 0;
	probabilitySliderβγ = 0;
	probabilitySliderβδ = 0;
	probabilitySliderβε = 0;
	probabilitySliderβζ = 0;
	probabilitySliderβη = 0;

	probabilitySliderγα = 0;
	probabilitySliderγβ = 0;
	probabilitySliderγγ = 0;
	probabilitySliderγδ = 0;
	probabilitySliderγε = 0;
	probabilitySliderγζ = 0;
	probabilitySliderγη = 0;

	probabilitySliderδα = 0;
	probabilitySliderδβ = 0;
	probabilitySliderδγ = 0;
	probabilitySliderδδ = 0;
	probabilitySliderδε = 0;
	probabilitySliderδζ = 0;
	probabilitySliderδη = 0;

	probabilitySliderεα = 0;
	probabilitySliderεβ = 0;
	probabilitySliderεγ = 0;
	probabilitySliderεδ = 0;
	probabilitySliderεε = 0;
	probabilitySliderεζ = 0;
	probabilitySliderεη = 0;

	probabilitySliderζα = 0;
	probabilitySliderζβ = 0;
	probabilitySliderζγ = 0;
	probabilitySliderζδ = 0;
	probabilitySliderζε = 0;
	probabilitySliderζζ = 0;
	probabilitySliderζη = 0;

	probabilitySliderηα = 0;
	probabilitySliderηβ = 0;
	probabilitySliderηγ = 0;
	probabilitySliderηδ = 0;
	probabilitySliderηε = 0;
	probabilitySliderηζ = 0;
	probabilitySliderηη = 0;

	probabilitySliderαθ = 0;
	probabilitySliderβθ = 0;
	probabilitySliderγθ = 0;
	probabilitySliderδθ = 0;
	probabilitySliderεθ = 0;
	probabilitySliderζθ = 0;
	probabilitySliderηθ = 0;
	probabilitySliderθα = 0;
	probabilitySliderθβ = 0;
	probabilitySliderθγ = 0;
	probabilitySliderθδ = 0;
	probabilitySliderθε = 0;
	probabilitySliderθζ = 0;
	probabilitySliderθη = 0;
	probabilitySliderθθ = 0;

	// Betha

	powerSliderββ = 0;
	powerSliderβα = 0;
	powerSliderβδ = 0;
	powerSliderβγ = 0;
	powerSliderβε = 0;
	powerSliderβζ = 0;
	powerSliderβη = 0;

	vSliderββ = 0;
	vSliderβα = 0;
	vSliderβδ = 0;
	vSliderβγ = 0;
	vSliderβε = 0;
	vSliderβζ = 0;
	vSliderβη = 0;

	// Alpha
	
	powerSliderαα = 0;
	powerSliderαβ = 0;
	powerSliderαδ = 0;
	powerSliderαγ = 0;
	powerSliderαε = 0;
	powerSliderαζ = 0;
	powerSliderαη = 0;

	vSliderαβ = 0;
	vSliderαα = 0;
	vSliderαδ = 0;
	vSliderαγ = 0;
	vSliderαε = 0;
	vSliderαζ = 0;
	vSliderαη = 0;

	// Delta
	
	powerSliderδδ = 0;
	powerSliderδα = 0;
	powerSliderδβ = 0;
	powerSliderδγ = 0;
	powerSliderδε = 0;
	powerSliderδζ = 0;
	powerSliderδη = 0;

	vSliderδβ = 0;
	vSliderδα = 0;
	vSliderδδ = 0;
	vSliderδγ = 0;
	vSliderδε = 0;
	vSliderδζ = 0;
	vSliderδη = 0;

	// Gamma
	
	powerSliderγγ = 0;
	powerSliderγδ = 0;
	powerSliderγα = 0;
	powerSliderγβ = 0;
	powerSliderγε = 0;
	powerSliderγζ = 0;
	powerSliderγη = 0;

	vSliderγβ = 0;
	vSliderγα = 0;
	vSliderγδ = 0;
	vSliderγγ = 0;
	vSliderγε = 0;
	vSliderγζ = 0;
	vSliderγη = 0;

	// Epsilon
	
	powerSliderεγ = 0;
	powerSliderεδ = 0;
	powerSliderεα = 0;
	powerSliderεβ = 0;
	powerSliderεε = 0;
	powerSliderεζ = 0;
	powerSliderεη = 0;

	vSliderεβ = 0;
	vSliderεα = 0;
	vSliderεδ = 0;
	vSliderεγ = 0;
	vSliderεε = 0;
	vSliderεζ = 0;
	vSliderεη = 0;

	// Zetha
	
	powerSliderζγ = 0;
	powerSliderζδ = 0;
	powerSliderζα = 0;
	powerSliderζβ = 0;
	powerSliderζε = 0;
	powerSliderζζ = 0;
	powerSliderζη = 0;

	vSliderζβ = 0;
	vSliderζα = 0;
	vSliderζδ = 0;
	vSliderζγ = 0;
	vSliderζε = 0;
	vSliderζζ = 0;
	vSliderζη = 0;

	// Etha
	
	powerSliderηγ = 0;
	powerSliderηδ = 0;
	powerSliderηα = 0;
	powerSliderηβ = 0;
	powerSliderηε = 0;
	powerSliderηζ = 0;
	powerSliderηη = 0;

	vSliderηβ = 0;
	vSliderηα = 0;
	vSliderηδ = 0;
	vSliderηγ = 0;
	vSliderηε = 0;
	vSliderηζ = 0;
	vSliderηη = 0;

	// Etha

	powerSliderθγ = 0;
	powerSliderθδ = 0;
	powerSliderθα = 0;
	powerSliderθβ = 0;
	powerSliderθε = 0;
	powerSliderθζ = 0;
	powerSliderθη = 0;
	powerSliderθθ = 0;

	vSliderθβ = 0;
	vSliderθα = 0;
	vSliderθδ = 0;
	vSliderθγ = 0;
	vSliderθε = 0;
	vSliderθζ = 0;
	vSliderθη = 0;
	vSliderθθ = 0;

	powerSliderγθ = 0;
	powerSliderδθ = 0;
	powerSliderαθ = 0;
	powerSliderβθ = 0;
	powerSliderεθ = 0;
	powerSliderζθ = 0;
	powerSliderηθ = 0;

	vSliderβθ = 0;
	vSliderαθ = 0;
	vSliderδθ = 0;
	vSliderγθ = 0;
	vSliderεθ = 0;
	vSliderζθ = 0;
	vSliderηθ = 0;
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
		viscositySliderαθ,
		viscositySliderβθ,
		viscositySliderγθ,
		viscositySliderδθ,
		viscositySliderεθ,
		viscositySliderζθ,
		viscositySliderηθ,
		viscositySliderθα,
		viscositySliderθβ,
		viscositySliderθγ,
		viscositySliderθδ,
		viscositySliderθε,
		viscositySliderθζ,
		viscositySliderθη,
		viscositySliderθθ,
		probabilitySliderαθ,
		probabilitySliderβθ,
		probabilitySliderγθ,
		probabilitySliderδθ,
		probabilitySliderεθ,
		probabilitySliderζθ,
		probabilitySliderηθ,
		probabilitySliderθα,
		probabilitySliderθβ,
		probabilitySliderθγ,
		probabilitySliderθδ,
		probabilitySliderθε,
		probabilitySliderθζ,
		probabilitySliderθη,
		probabilitySliderθθ,
		static_cast<float>(numberSliderθ),
		powerSliderαθ,
		powerSliderβθ,
		powerSliderδθ,
		powerSliderγθ,
		powerSliderεθ,
		powerSliderζθ,
		powerSliderηθ,
		powerSliderθα,
		powerSliderθβ,
		powerSliderθδ,
		powerSliderθγ,
		powerSliderθε,
		powerSliderθζ,
		powerSliderθη,
		powerSliderθθ,
		vSliderαθ,
		vSliderβθ,
		vSliderδθ,
		vSliderγθ,
		vSliderεθ,
		vSliderζθ,
		vSliderηθ,
		vSliderθα,
		vSliderθβ,
		vSliderθδ,
		vSliderθγ,
		vSliderθε,
		vSliderθζ,
		vSliderθη,
		vSliderθθ,
		minViscoSlider,
		maxViscoSlider,
		minProbSlider,
		maxProbSlider,
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

	if (p.size() < 280)
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
		viscositySliderαθ = p[215];
		viscositySliderβθ = p[216];
		viscositySliderγθ = p[217];
		viscositySliderδθ = p[218];
		viscositySliderεθ = p[219];
		viscositySliderζθ = p[220];
		viscositySliderηθ = p[221];
		viscositySliderθα = p[222];
		viscositySliderθβ = p[223];
		viscositySliderθγ = p[224];
		viscositySliderθδ = p[225];
		viscositySliderθε = p[226];
		viscositySliderθζ = p[227];
		viscositySliderθη = p[228];
		viscositySliderθθ = p[229];
		probabilitySliderαθ = p[230];
		probabilitySliderβθ = p[231];
		probabilitySliderγθ = p[232];
		probabilitySliderδθ = p[233];
		probabilitySliderεθ = p[234];
		probabilitySliderζθ = p[235];
		probabilitySliderηθ = p[236];
		probabilitySliderθα = p[237];
		probabilitySliderθβ = p[238];
		probabilitySliderθγ = p[239];
		probabilitySliderθδ = p[240];
		probabilitySliderθε = p[241];
		probabilitySliderθζ = p[242];
		probabilitySliderθη = p[243];
		probabilitySliderθθ = p[244];
		numberSliderθ = static_cast<int>(p[245]);
		powerSliderαθ = p[246];
		powerSliderβθ = p[247];
		powerSliderδθ = p[248];
		powerSliderγθ = p[249];
		powerSliderεθ = p[250];
		powerSliderζθ = p[251];
		powerSliderηθ = p[252];
		powerSliderθα = p[253];
		powerSliderθβ = p[254];
		powerSliderθδ = p[255];
		powerSliderθγ = p[256];
		powerSliderθε = p[257];
		powerSliderθζ = p[258];
		powerSliderθη = p[259];
		powerSliderθθ = p[260];
		vSliderαθ = p[261];
		vSliderβθ = p[262];
		vSliderδθ = p[263];
		vSliderγθ = p[264];
		vSliderεθ = p[265];
		vSliderζθ = p[266];
		vSliderηθ = p[267];
		vSliderθα = p[268];
		vSliderθβ = p[269];
		vSliderθδ = p[270];
		vSliderθγ = p[271];
		vSliderθε = p[272];
		vSliderθζ = p[273];
		vSliderθη = p[274];
		vSliderθθ = p[275];
		minViscoSlider = p[276];
		maxViscoSlider = p[277];
		minProbSlider = p[278];
		maxProbSlider = p[279];
	}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - 7+1c64v64p version 1.7.6");
	ofSetVerticalSync(true);

	// Interface
	gui.setup("Settings");
	gui.loadFont("Arial", 10);
	gui.setWidthElements(265.0f);
	gui.add(fps.setup("FPS", "0"));
	gui.add(physicLabel.setup("physic (ms)", "0"));
	gui.add(resetButton.setup("Restart (r)"));
	gui.add(motionBlurToggle.setup("Motion Blur", false));
	gui.add(save.setup("Save Model"));
	gui.add(load.setup("Load Model"));
	//gui.add(modelToggle.setup("Show Model", false));

	rndGroup.setup("Randomize");
	rndGroup.add(randomGeneral.setup("Randomize all parameters (space bar)"));
	rndGroup.add(randomCount.setup("Randomize particle count (q)"));
	rndGroup.add(randomVsc.setup("Randomize Viscosities (v)"));
	rndGroup.add(randomProb.setup("Randomize Probabilities (p)"));
	rndGroup.add(randomChoice.setup("Randomize IP (i)"));
	rndGroup.add(rndDarkButton.setup("Random Dark IP (d)"));
	gui.add(&rndGroup);
	rndGroup.minimize();

	// Quantity Group
	qtyGroup.setup("Quantity (require restart/randomize)");
	qtyGroup.add(numberSliderα.setup("Alpha", pnumberSliderα, 0, 10000));
	qtyGroup.add(numberSliderβ.setup("betha", pnumberSliderβ, 0, 10000));
	qtyGroup.add(numberSliderγ.setup("Gamma", pnumberSliderγ, 0, 10000));
	qtyGroup.add(numberSliderδ.setup("Delta", pnumberSliderδ, 0, 10000));
	qtyGroup.add(numberSliderε.setup("Epsilon", pnumberSliderε, 0, 10000));
	qtyGroup.add(numberSliderζ.setup("Zeta", pnumberSliderζ, 0, 10000));
	qtyGroup.add(numberSliderη.setup("Eta", pnumberSliderη, 0, 10000));
	qtyGroup.add(numberSliderθ.setup("Teta - Dark", pnumberSliderθ, 0, 10000));
	gui.add(&qtyGroup);
	qtyGroup.minimize();

	// Evolve Group
	evolveGroup.setup("Evolution of Parameters");
	evolveGroup.add(evoToggle.setup("Evolve parameters", false));
	evolveGroup.add(InteractionEvoProbSlider.setup("inter evo chance%", InterEvoChance, 0, 100));
	evolveGroup.add(InteractionEvoAmountSlider.setup("inter evo amount%%", InterEvoAmount, 0, 100));
	evolveGroup.add(ProbabilityEvoProbSlider.setup("prob evo chance%", ProbEvoChance, 0, 100));
	evolveGroup.add(ProbabilityEvoAmountSlider.setup("prob evo amount%%", ProbEvoAmount, 0, 100));
	evolveGroup.add(ViscosityEvoProbSlider.setup("visco evo chance%", ViscoEvoChance, 0, 100));
	evolveGroup.add(ViscosityEvoAmountSlider.setup("visco evo amount%%", ViscoEvoAmount, 0, 100));
	evolveGroup.add(probabilitySlider.setup("interaction prob%", probability, 1, 100));
	evolveGroup.add(viscositySlider.setup("interaction viscosity", viscosity, 0, 1));
	gui.add(&evolveGroup);
	evolveGroup.minimize();

	// MinMax Group
	MinMaxGroup.setup("MinMax");
	MinMaxGroup.add(minPowerSlider.setup("minimum power", minP, -200, 200));
	MinMaxGroup.add(maxPowerSlider.setup("maximum power", maxP, -200, 200));
	MinMaxGroup.add(minRangeSlider.setup("minimum range", minR, 0, 500));
	MinMaxGroup.add(maxRangeSlider.setup("maximum range", maxR, 0, 500));
	MinMaxGroup.add(minViscoSlider.setup("minimum viscosity", minV, 0.0, 1.0));
	MinMaxGroup.add(maxViscoSlider.setup("maximum viscosity", maxV, 0.0, 1.0));
	MinMaxGroup.add(minProbSlider.setup("minimum probability", minI, 0, 100));
	MinMaxGroup.add(maxProbSlider.setup("maximum probability", maxI, 0, 100));
	gui.add(&MinMaxGroup);
	MinMaxGroup.minimize();

	// Alpha
	alphaGroup.setup("Alpha");
	alphaGroup.add(powerSliderαα.setup("power alpha x alpha:", ppowerSliderαα, minP, maxP));
	alphaGroup.add(powerSliderαβ.setup("power alpha x betha:", ppowerSliderαβ, minP, maxP));
	alphaGroup.add(powerSliderαγ.setup("power alpha x gamma:", ppowerSliderαγ, minP, maxP));
	alphaGroup.add(powerSliderαδ.setup("power alpha x delta:", ppowerSliderαδ, minP, maxP));
	alphaGroup.add(powerSliderαε.setup("power alpha x epsilon:", ppowerSliderαε, minP, maxP));
	alphaGroup.add(powerSliderαζ.setup("power alpha x zeta:", ppowerSliderαζ, minP, maxP));
	alphaGroup.add(powerSliderαη.setup("power alpha x eta:", ppowerSliderαη, minP, maxP));
	alphaGroup.add(powerSliderαθ.setup("power alpha x teta:", ppowerSliderαθ, minP, maxP));
	alphaGroup.add(vSliderαα.setup("radius alpha x alpha:", pvSliderαα, minR, maxR));
	alphaGroup.add(vSliderαβ.setup("radius alpha x betha:", pvSliderαβ, minR, maxR));
	alphaGroup.add(vSliderαγ.setup("radius alpha x gamma:", pvSliderαγ, minR, maxR));
	alphaGroup.add(vSliderαδ.setup("radius alpha x delta:", pvSliderαδ, minR, maxR));
	alphaGroup.add(vSliderαε.setup("radius alpha x epsilon:", pvSliderαε, minR, maxR));
	alphaGroup.add(vSliderαζ.setup("radius alpha x zeta:", pvSliderαζ, minR, maxR));
	alphaGroup.add(vSliderαη.setup("radius alpha x eta:", pvSliderαη, minR, maxR));
	alphaGroup.add(vSliderαθ.setup("radius alpha x teta:", pvSliderαθ, minR, maxR));
	alphaGroup.add(viscositySliderαβ.setup("Viscosity alpha x betha", viscosityαβ, minV, maxV));
	alphaGroup.add(viscositySliderαα.setup("Viscosity alpha x alpha", viscosityαα, minV, maxV));
	alphaGroup.add(viscositySliderαγ.setup("Viscosity alpha x gamma", viscosityαγ, minV, maxV));
	alphaGroup.add(viscositySliderαδ.setup("Viscosity alpha x delta", viscosityαδ, minV, maxV));
	alphaGroup.add(viscositySliderαε.setup("Viscosity alpha x epsilon", viscosityαε, minV, maxV));
	alphaGroup.add(viscositySliderαζ.setup("Viscosity alpha x zeta", viscosityαζ, minV, maxV));
	alphaGroup.add(viscositySliderαη.setup("Viscosity alpha x eta", viscosityαη, minV, maxV));
	alphaGroup.add(viscositySliderαθ.setup("Viscosity alpha x teta", viscosityαθ, minV, maxV));
	alphaGroup.add(probabilitySliderαβ.setup("Probability alpha x betha", probabilityαβ, minI, maxI));
	alphaGroup.add(probabilitySliderαα.setup("Probability alpha x alpha", probabilityαα, minI, maxI));
	alphaGroup.add(probabilitySliderαγ.setup("Probability alpha x gamma", probabilityαγ, minI, maxI));
	alphaGroup.add(probabilitySliderαδ.setup("Probability alpha x delta", probabilityαδ, minI, maxI));
	alphaGroup.add(probabilitySliderαε.setup("Probability alpha x epsilon", probabilityαε, minI, maxI));
	alphaGroup.add(probabilitySliderαζ.setup("Probability alpha x zeta", probabilityαζ, minI, maxI));
	alphaGroup.add(probabilitySliderαη.setup("Probability alpha x eta", probabilityαη, minI, maxI));
	alphaGroup.add(probabilitySliderαθ.setup("Probability alpha x teta", probabilityαθ, minI, maxI));
	alphaGroup.minimize();
	gui.add(&alphaGroup);

	// Betha
	bethaGroup.setup("Betha");
	bethaGroup.add(powerSliderβα.setup("power betha x alpha:", ppowerSliderβα, minP, maxP));
	bethaGroup.add(powerSliderββ.setup("power betha x betha:", ppowerSliderββ, minP, maxP));
	bethaGroup.add(powerSliderβγ.setup("power betha x gamma:", ppowerSliderβγ, minP, maxP));
	bethaGroup.add(powerSliderβδ.setup("power betha x delta:", ppowerSliderβδ, minP, maxP));
	bethaGroup.add(powerSliderβε.setup("power betha x epsilon:", ppowerSliderβε, minP, maxP));
	bethaGroup.add(powerSliderβζ.setup("power betha x zeta:", ppowerSliderβζ, minP, maxP));
	bethaGroup.add(powerSliderβη.setup("power betha x eta:", ppowerSliderβη, minP, maxP));
	bethaGroup.add(powerSliderβθ.setup("power betha x teta:", ppowerSliderβθ, minP, maxP));
	bethaGroup.add(vSliderβα.setup("radius r x alpha:", pvSliderβα, minR, maxR));
	bethaGroup.add(vSliderββ.setup("radius r x betha:", pvSliderββ, minR, maxR));
	bethaGroup.add(vSliderβγ.setup("radius r x gamma:", pvSliderβγ, minR, maxR));
	bethaGroup.add(vSliderβδ.setup("radius r x delta:", pvSliderβδ, minR, maxR));
	bethaGroup.add(vSliderβε.setup("radius r x epsilon:", pvSliderβε, minR, maxR));
	bethaGroup.add(vSliderβζ.setup("radius r x zeta:", pvSliderβζ, minR, maxR));
	bethaGroup.add(vSliderβη.setup("radius r x eta:", pvSliderβη, minR, maxR));
	bethaGroup.add(vSliderβθ.setup("radius betha x teta:", pvSliderβθ, minR, maxR));
	bethaGroup.add(viscositySliderββ.setup("Viscosity betha x betha", viscosityββ, minV, maxV));
	bethaGroup.add(viscositySliderβα.setup("Viscosity betha x alpha", viscosityβα, minV, maxV));
	bethaGroup.add(viscositySliderβγ.setup("Viscosity betha x gamma", viscosityβγ, minV, maxV));
	bethaGroup.add(viscositySliderβδ.setup("Viscosity betha x delta", viscosityβδ, minV, maxV));
	bethaGroup.add(viscositySliderβε.setup("Viscosity betha x epsilon", viscosityβε, minV, maxV));
	bethaGroup.add(viscositySliderβζ.setup("Viscosity betha x zeta", viscosityβζ, minV, maxV));
	bethaGroup.add(viscositySliderβη.setup("Viscosity betha x eta", viscosityβη, minV, maxV));
	bethaGroup.add(viscositySliderβθ.setup("Viscosity betha x teta", viscosityβθ, minV, maxV));
	bethaGroup.add(probabilitySliderββ.setup("Probability betha x betha", probabilityββ, minI, maxI));
	bethaGroup.add(probabilitySliderβα.setup("Probability betha x alpha", probabilityβα, minI, maxI));
	bethaGroup.add(probabilitySliderβγ.setup("Probability betha x gamma", probabilityβγ, minI, maxI));
	bethaGroup.add(probabilitySliderβδ.setup("Probability betha x delta", probabilityβδ, minI, maxI));
	bethaGroup.add(probabilitySliderβε.setup("Probability betha x epsilon", probabilityβε, minI, maxI));
	bethaGroup.add(probabilitySliderβζ.setup("Probability betha x zeta", probabilityβζ, minI, maxI));
	bethaGroup.add(probabilitySliderβη.setup("Probability betha x eta", probabilityβη, minI, maxI));
	bethaGroup.add(probabilitySliderβθ.setup("Probability betha x teta", probabilityβθ, minI, maxI));
	bethaGroup.minimize();
	gui.add(&bethaGroup);

	// Gamma
	gammaGroup.setup("Gamma");
	gammaGroup.add(powerSliderγα.setup("power gamma x alpha:", ppowerSliderγα, minP, maxP));
	gammaGroup.add(powerSliderγβ.setup("power gamma x betha:", ppowerSliderγβ, minP, maxP));
	gammaGroup.add(powerSliderγγ.setup("power gamma x gamma:", ppowerSliderγγ, minP, maxP));
	gammaGroup.add(powerSliderγδ.setup("power gamma x delta:", ppowerSliderγδ, minP, maxP));
	gammaGroup.add(powerSliderγε.setup("power gamma x epsilon:", ppowerSliderγε, minP, maxP));
	gammaGroup.add(powerSliderγζ.setup("power gamma x zeta:", ppowerSliderγζ, minP, maxP));
	gammaGroup.add(powerSliderγη.setup("power gamma x eta:", ppowerSliderγη, minP, maxP));
	gammaGroup.add(powerSliderγθ.setup("power gamma x teta:", ppowerSliderγθ, minP, maxP));
	gammaGroup.add(vSliderγα.setup("radius b x alpha:", pvSliderγα, minR, maxR));
	gammaGroup.add(vSliderγβ.setup("radius b x betha:", pvSliderγβ, minR, maxR));
	gammaGroup.add(vSliderγγ.setup("radius b x gamma:", pvSliderγγ, minR, maxR));
	gammaGroup.add(vSliderγδ.setup("radius b x delta:", pvSliderγδ, minR, maxR));
	gammaGroup.add(vSliderγε.setup("radius b x epsilon:", pvSliderγε, minR, maxR));
	gammaGroup.add(vSliderγζ.setup("radius b x zeta:", pvSliderγζ, minR, maxR));
	gammaGroup.add(vSliderγη.setup("radius b x eta:", pvSliderγη, minR, maxR));
	gammaGroup.add(vSliderγθ.setup("radius gamma x teta:", pvSliderγθ, minR, maxR));
	gammaGroup.add(viscositySliderγβ.setup("Viscosity gamma x betha", viscosityγβ, minV, maxV));
	gammaGroup.add(viscositySliderγα.setup("Viscosity gamma x alpha", viscosityγα, minV, maxV));
	gammaGroup.add(viscositySliderγγ.setup("Viscosity gamma x gamma", viscosityγγ, minV, maxV));
	gammaGroup.add(viscositySliderγδ.setup("Viscosity gamma x delta", viscosityγδ, minV, maxV));
	gammaGroup.add(viscositySliderγε.setup("Viscosity gamma x epsilon", viscosityγε, minV, maxV));
	gammaGroup.add(viscositySliderγζ.setup("Viscosity gamma x zeta", viscosityγζ, minV, maxV));
	gammaGroup.add(viscositySliderγη.setup("Viscosity gamma x eta", viscosityγη, minV, maxV));
	gammaGroup.add(viscositySliderγθ.setup("Viscosity gamma x teta", viscosityγθ, minV, maxV));
	gammaGroup.add(probabilitySliderγβ.setup("Probability gamma x betha", probabilityγβ, minI, maxI));
	gammaGroup.add(probabilitySliderγα.setup("Probability gamma x alpha", probabilityγα, minI, maxI));
	gammaGroup.add(probabilitySliderγγ.setup("Probability gamma x gamma", probabilityγγ, minI, maxI));
	gammaGroup.add(probabilitySliderγδ.setup("Probability gamma x delta", probabilityγδ, minI, maxI));
	gammaGroup.add(probabilitySliderγε.setup("Probability gamma x epsilon", probabilityγε, minI, maxI));
	gammaGroup.add(probabilitySliderγζ.setup("Probability gamma x zeta", probabilityγζ, minI, maxI));
	gammaGroup.add(probabilitySliderγη.setup("Probability gamma x eta", probabilityγη, minI, maxI));
	gammaGroup.add(probabilitySliderγθ.setup("Probability gamma x teta", probabilityγθ, minI, maxI));
	gammaGroup.minimize();
	gui.add(&gammaGroup);

	// Delta
	eltaGroup.setup("Delta");
	eltaGroup.add(powerSliderδα.setup("power delta x alpha:", ppowerSliderδα, minP, maxP));
	eltaGroup.add(powerSliderδβ.setup("power delta x betha:", ppowerSliderδβ, minP, maxP));
	eltaGroup.add(powerSliderδγ.setup("power delta x gamma:", ppowerSliderδγ, minP, maxP));
	eltaGroup.add(powerSliderδδ.setup("power delta x delta:", ppowerSliderδδ, minP, maxP));
	eltaGroup.add(powerSliderδε.setup("power delta x epsilon:", ppowerSliderδε, minP, maxP));
	eltaGroup.add(powerSliderδζ.setup("power delta x zeta:", ppowerSliderδζ, minP, maxP));
	eltaGroup.add(powerSliderδη.setup("power delta x eta:", ppowerSliderδη, minP, maxP));
	eltaGroup.add(powerSliderδθ.setup("power delta x teta:", ppowerSliderδθ, minP, maxP));
	eltaGroup.add(vSliderδα.setup("radius w x alpha:", pvSliderδα, minR, maxR));
	eltaGroup.add(vSliderδβ.setup("radius w x betha:", pvSliderδβ, minR, maxR));
	eltaGroup.add(vSliderδγ.setup("radius w x gamma:", pvSliderδγ, minR, maxR));
	eltaGroup.add(vSliderδδ.setup("radius w x delta:", pvSliderδδ, minR, maxR));
	eltaGroup.add(vSliderδε.setup("radius w x epsilon:", pvSliderδε, minR, maxR));
	eltaGroup.add(vSliderδζ.setup("radius w x zeta:", pvSliderδζ, minR, maxR));
	eltaGroup.add(vSliderδη.setup("radius w x eta:", pvSliderδη, minR, maxR));
	eltaGroup.add(vSliderδθ.setup("radius delta x teta:", pvSliderδθ, minR, maxR));
	eltaGroup.add(viscositySliderδβ.setup("Viscosity delta x betha", viscosityδβ, minV, maxV));
	eltaGroup.add(viscositySliderδα.setup("Viscosity delta x alpha", viscosityδα, minV, maxV));
	eltaGroup.add(viscositySliderδγ.setup("Viscosity delta x gamma", viscosityδγ, minV, maxV));
	eltaGroup.add(viscositySliderδδ.setup("Viscosity delta x delta", viscosityδδ, minV, maxV));
	eltaGroup.add(viscositySliderδε.setup("Viscosity delta x epsilon", viscosityδε, minV, maxV));
	eltaGroup.add(viscositySliderδζ.setup("Viscosity delta x zeta", viscosityδζ, minV, maxV));
	eltaGroup.add(viscositySliderδη.setup("Viscosity delta x eta", viscosityδη, minV, maxV));
	eltaGroup.add(viscositySliderδθ.setup("Viscosity delta x teta", viscosityδθ, minV, maxV));
	eltaGroup.add(probabilitySliderδβ.setup("Probability delta x betha", probabilityδβ, minI, maxI));
	eltaGroup.add(probabilitySliderδα.setup("Probability delta x alpha", probabilityδα, minI, maxI));
	eltaGroup.add(probabilitySliderδγ.setup("Probability delta x gamma", probabilityδγ, minI, maxI));
	eltaGroup.add(probabilitySliderδδ.setup("Probability delta x delta", probabilityδδ, minI, maxI));
	eltaGroup.add(probabilitySliderδε.setup("Probability delta x epsilon", probabilityδε, minI, maxI));
	eltaGroup.add(probabilitySliderδζ.setup("Probability delta x zeta", probabilityδζ, minI, maxI));
	eltaGroup.add(probabilitySliderδη.setup("Probability delta x eta", probabilityδη, minI, maxI));
	eltaGroup.add(probabilitySliderδθ.setup("Probability delta x teta", probabilityδθ, minI, maxI));
	eltaGroup.minimize();
	gui.add(&eltaGroup);

	// Epsilon
	epsilonGroup.setup("Epsilon");
	epsilonGroup.add(powerSliderεα.setup("power epsilon x alpha:", ppowerSliderεα, minP, maxP));
	epsilonGroup.add(powerSliderεβ.setup("power epsilon x betha:", ppowerSliderεβ, minP, maxP));
	epsilonGroup.add(powerSliderεγ.setup("power epsilon x gamma:", ppowerSliderεγ, minP, maxP));
	epsilonGroup.add(powerSliderεδ.setup("power epsilon x delta:", ppowerSliderεδ, minP, maxP));
	epsilonGroup.add(powerSliderεε.setup("power epsilon x epsilon:", ppowerSliderεε, minP, maxP));
	epsilonGroup.add(powerSliderεζ.setup("power epsilon x zeta:", ppowerSliderεζ, minP, maxP));
	epsilonGroup.add(powerSliderεη.setup("power epsilon x eta:", ppowerSliderεη, minP, maxP));
	epsilonGroup.add(powerSliderεθ.setup("power epsilon x teta:", ppowerSliderεθ, minP, maxP));
	epsilonGroup.add(vSliderεα.setup("radius o x alpha:", pvSliderεα, minR, maxR));
	epsilonGroup.add(vSliderεβ.setup("radius o x betha:", pvSliderεβ, minR, maxR));
	epsilonGroup.add(vSliderεγ.setup("radius o x gamma:", pvSliderεγ, minR, maxR));
	epsilonGroup.add(vSliderεδ.setup("radius o x delta:", pvSliderεδ, minR, maxR));
	epsilonGroup.add(vSliderεε.setup("radius o x epsilon:", pvSliderεε, minR, maxR));
	epsilonGroup.add(vSliderεζ.setup("radius o x zeta:", pvSliderεζ, minR, maxR));
	epsilonGroup.add(vSliderεη.setup("radius o x eta:", pvSliderεη, minR, maxR));
	epsilonGroup.add(vSliderεθ.setup("radius epsilon x teta:", pvSliderεθ, minR, maxR));
	epsilonGroup.add(viscositySliderεβ.setup("Viscosity epsilon x betha", viscosityεβ, minV, maxV));
	epsilonGroup.add(viscositySliderεα.setup("Viscosity epsilon x alpha", viscosityεα, minV, maxV));
	epsilonGroup.add(viscositySliderεγ.setup("Viscosity epsilon x gamma", viscosityεγ, minV, maxV));
	epsilonGroup.add(viscositySliderεδ.setup("Viscosity epsilon x delta", viscosityεδ, minV, maxV));
	epsilonGroup.add(viscositySliderεε.setup("Viscosity epsilon x epsilon", viscosityεε, minV, maxV));
	epsilonGroup.add(viscositySliderεζ.setup("Viscosity epsilon x zeta", viscosityεζ, minV, maxV));
	epsilonGroup.add(viscositySliderεη.setup("Viscosity epsilon x eta", viscosityεη, minV, maxV));
	epsilonGroup.add(viscositySliderεθ.setup("Viscosity epsilon x teta", viscosityεθ, minV, maxV));
	epsilonGroup.add(probabilitySliderεβ.setup("Probability epsilon x betha", probabilityεβ, minI, maxI));
	epsilonGroup.add(probabilitySliderεα.setup("Probability epsilon x alpha", probabilityεα, minI, maxI));
	epsilonGroup.add(probabilitySliderεγ.setup("Probability epsilon x gamma", probabilityεγ, minI, maxI));
	epsilonGroup.add(probabilitySliderεδ.setup("Probability epsilon x delta", probabilityεδ, minI, maxI));
	epsilonGroup.add(probabilitySliderεε.setup("Probability epsilon x epsilon", probabilityεε, minI, maxI));
	epsilonGroup.add(probabilitySliderεζ.setup("Probability epsilon x zeta", probabilityεζ, minI, maxI));
	epsilonGroup.add(probabilitySliderεη.setup("Probability epsilon x eta", probabilityεη, minI, maxI));
	epsilonGroup.add(probabilitySliderεθ.setup("Probability epsilon x teta", probabilityεθ, minI, maxI));
	epsilonGroup.minimize();
	gui.add(&epsilonGroup);

	// Zeta
	zetaGroup.setup("Zeta");
	zetaGroup.add(powerSliderζα.setup("power zeta x alpha:", ppowerSliderζα, minP, maxP));
	zetaGroup.add(powerSliderζβ.setup("power zeta x betha:", ppowerSliderζβ, minP, maxP));
	zetaGroup.add(powerSliderζγ.setup("power zeta x gamma:", ppowerSliderζγ, minP, maxP));
	zetaGroup.add(powerSliderζδ.setup("power zeta x delta:", ppowerSliderζδ, minP, maxP));
	zetaGroup.add(powerSliderζε.setup("power zeta x epsilon:", ppowerSliderζε, minP, maxP));
	zetaGroup.add(powerSliderζζ.setup("power zeta x zeta:", ppowerSliderζζ, minP, maxP));
	zetaGroup.add(powerSliderζη.setup("power zeta x eta:", ppowerSliderζη, minP, maxP));
	zetaGroup.add(powerSliderζθ.setup("power zeta x teta:", ppowerSliderζθ, minP, maxP));
	zetaGroup.add(vSliderζα.setup("radius k x alpha:", pvSliderζα, minR, maxR));
	zetaGroup.add(vSliderζβ.setup("radius k x betha:", pvSliderζβ, minR, maxR));
	zetaGroup.add(vSliderζγ.setup("radius k x gamma:", pvSliderζγ, minR, maxR));
	zetaGroup.add(vSliderζδ.setup("radius k x delta:", pvSliderζδ, minR, maxR));
	zetaGroup.add(vSliderζε.setup("radius k x epsilon:", pvSliderζε, minR, maxR));
	zetaGroup.add(vSliderζζ.setup("radius k x zeta:", pvSliderζζ, minR, maxR));
	zetaGroup.add(vSliderζη.setup("radius k x eta:", pvSliderζη, minR, maxR));
	zetaGroup.add(vSliderζθ.setup("radius zeta x teta:", pvSliderζθ, minR, maxR));
	zetaGroup.add(viscositySliderζβ.setup("Viscosity zeta x betha", viscosityζβ, minV, maxV));
	zetaGroup.add(viscositySliderζα.setup("Viscosity zeta x alpha", viscosityζα, minV, maxV));
	zetaGroup.add(viscositySliderζγ.setup("Viscosity zeta x gamma", viscosityζγ, minV, maxV));
	zetaGroup.add(viscositySliderζδ.setup("Viscosity zeta x delta", viscosityζδ, minV, maxV));
	zetaGroup.add(viscositySliderζε.setup("Viscosity zeta x epsilon", viscosityζε, minV, maxV));
	zetaGroup.add(viscositySliderζζ.setup("Viscosity zeta x zeta", viscosityζζ, minV, maxV));
	zetaGroup.add(viscositySliderζη.setup("Viscosity zeta x eta", viscosityζη, minV, maxV));
	zetaGroup.add(viscositySliderζθ.setup("Viscosity zeta x teta", viscosityζθ, minV, maxV));
	zetaGroup.add(probabilitySliderζβ.setup("Probability zeta x betha", probabilityζβ, minI, maxI));
	zetaGroup.add(probabilitySliderζα.setup("Probability zeta x alpha", probabilityζα, minI, maxI));
	zetaGroup.add(probabilitySliderζγ.setup("Probability zeta x gamma", probabilityζγ, minI, maxI));
	zetaGroup.add(probabilitySliderζδ.setup("Probability zeta x delta", probabilityζδ, minI, maxI));
	zetaGroup.add(probabilitySliderζε.setup("Probability zeta x epsilon", probabilityζε, minI, maxI));
	zetaGroup.add(probabilitySliderζζ.setup("Probability zeta x zeta", probabilityζζ, minI, maxI));
	zetaGroup.add(probabilitySliderζη.setup("Probability zeta x eta", probabilityζη, minI, maxI));
	zetaGroup.add(probabilitySliderζθ.setup("Probability zeta x teta", probabilityζθ, minI, maxI));
	zetaGroup.minimize();
	gui.add(&zetaGroup);

	// Eta
	etaGroup.setup("Eta");
	etaGroup.add(powerSliderηα.setup("power eta x alpha:", ppowerSliderηα, minP, maxP));
	etaGroup.add(powerSliderηβ.setup("power eta x betha:", ppowerSliderηβ, minP, maxP));
	etaGroup.add(powerSliderηγ.setup("power eta x gamma:", ppowerSliderηγ, minP, maxP));
	etaGroup.add(powerSliderηδ.setup("power eta x delta:", ppowerSliderηδ, minP, maxP));
	etaGroup.add(powerSliderηε.setup("power eta x epsilon:", ppowerSliderηε, minP, maxP));
	etaGroup.add(powerSliderηζ.setup("power eta x zeta:", ppowerSliderηζ, minP, maxP));
	etaGroup.add(powerSliderηη.setup("power eta x eta:", ppowerSliderηη, minP, maxP));
	etaGroup.add(powerSliderηθ.setup("power eta x teta:", ppowerSliderηθ, minP, maxP));
	etaGroup.add(vSliderηα.setup("radius c x alpha:", pvSliderηα, minR, maxR));
	etaGroup.add(vSliderηβ.setup("radius c x betha:", pvSliderηβ, minR, maxR));
	etaGroup.add(vSliderηγ.setup("radius c x gamma:", pvSliderηγ, minR, maxR));
	etaGroup.add(vSliderηδ.setup("radius c x delta:", pvSliderηδ, minR, maxR));
	etaGroup.add(vSliderηε.setup("radius c x epsilon:", pvSliderηε, minR, maxR));
	etaGroup.add(vSliderηζ.setup("radius c x zeta:", pvSliderηζ, minR, maxR));
	etaGroup.add(vSliderηη.setup("radius c x eta:", pvSliderηη, minR, maxR));
	etaGroup.add(vSliderηθ.setup("radius eta x teta:", pvSliderηθ, minR, maxR));
	etaGroup.add(viscositySliderηβ.setup("Viscosity eta x betha", viscosityηβ, minV, maxV));
	etaGroup.add(viscositySliderηα.setup("Viscosity eta x alpha", viscosityηα, minV, maxV));
	etaGroup.add(viscositySliderηγ.setup("Viscosity eta x gamma", viscosityηγ, minV, maxV));
	etaGroup.add(viscositySliderηδ.setup("Viscosity eta x delta", viscosityηδ, minV, maxV));
	etaGroup.add(viscositySliderηε.setup("Viscosity eta x epsilon", viscosityηε, minV, maxV));
	etaGroup.add(viscositySliderηζ.setup("Viscosity eta x zeta", viscosityηζ, minV, maxV));
	etaGroup.add(viscositySliderηη.setup("Viscosity eta x eta", viscosityηη, minV, maxV));
	etaGroup.add(viscositySliderηθ.setup("Viscosity eta x teta", viscosityηθ, minV, maxV));
	etaGroup.add(probabilitySliderηβ.setup("Probability eta x betha", probabilityηβ, minI, maxI));
	etaGroup.add(probabilitySliderηα.setup("Probability eta x alpha", probabilityηα, minI, maxI));
	etaGroup.add(probabilitySliderηγ.setup("Probability eta x gamma", probabilityηγ, minI, maxI));
	etaGroup.add(probabilitySliderηδ.setup("Probability eta x delta", probabilityηδ, minI, maxI));
	etaGroup.add(probabilitySliderηε.setup("Probability eta x epsilon", probabilityηε, minI, maxI));
	etaGroup.add(probabilitySliderηζ.setup("Probability eta x zeta", probabilityηζ, minI, maxI));
	etaGroup.add(probabilitySliderηη.setup("Probability eta x eta", probabilityηη, minI, maxI));
	etaGroup.add(probabilitySliderηθ.setup("Probability eta x teta", probabilityηθ, minI, maxI));
	etaGroup.minimize();
	gui.add(&etaGroup);

	// Teta
	tetaGroup.setup("Teta - Dark");
	tetaGroup.add(powerSliderθα.setup("power teta x alpha:", ppowerSliderθα, minP, maxP));
	tetaGroup.add(powerSliderθβ.setup("power teta x betha:", ppowerSliderθβ, minP, maxP));
	tetaGroup.add(powerSliderθγ.setup("power teta x gamma:", ppowerSliderθγ, minP, maxP));
	tetaGroup.add(powerSliderθδ.setup("power teta x delta:", ppowerSliderθδ, minP, maxP));
	tetaGroup.add(powerSliderθε.setup("power teta x epsilon:", ppowerSliderθε, minP, maxP));
	tetaGroup.add(powerSliderθζ.setup("power teta x zeta:", ppowerSliderθζ, minP, maxP));
	tetaGroup.add(powerSliderθη.setup("power teta x eta:", ppowerSliderθη, minP, maxP));
	tetaGroup.add(powerSliderθθ.setup("power teta x teta:", ppowerSliderθθ, minP, maxP));
	tetaGroup.add(vSliderθα.setup("radius teta x alpha:", pvSliderθα, minR, maxR));
	tetaGroup.add(vSliderθβ.setup("radius teta x betha:", pvSliderθβ, minR, maxR));
	tetaGroup.add(vSliderθγ.setup("radius teta x gamma:", pvSliderθγ, minR, maxR));
	tetaGroup.add(vSliderθδ.setup("radius teta x delta:", pvSliderθδ, minR, maxR));
	tetaGroup.add(vSliderθε.setup("radius teta x epsilon:", pvSliderθε, minR, maxR));
	tetaGroup.add(vSliderθζ.setup("radius teta x zeta:", pvSliderθζ, minR, maxR));
	tetaGroup.add(vSliderθη.setup("radius teta x eta:", pvSliderθη, minR, maxR));
	tetaGroup.add(vSliderθθ.setup("radius teta x teta:", pvSliderθθ, minR, maxR));
	tetaGroup.add(viscositySliderθα.setup("Viscosity teta x alpha", viscosityθα, minV, maxV));
	tetaGroup.add(viscositySliderθβ.setup("Viscosity teta x betha", viscosityθβ, minV, maxV));
	tetaGroup.add(viscositySliderθγ.setup("Viscosity teta x gamma", viscosityθγ, minV, maxV));
	tetaGroup.add(viscositySliderθδ.setup("Viscosity teta x delta", viscosityθδ, minV, maxV));
	tetaGroup.add(viscositySliderθε.setup("Viscosity teta x epsilon", viscosityθε, minV, maxV));
	tetaGroup.add(viscositySliderθζ.setup("Viscosity teta x zeta", viscosityθζ, minV, maxV));
	tetaGroup.add(viscositySliderθη.setup("Viscosity teta x eta", viscosityθη, minV, maxV));
	tetaGroup.add(viscositySliderθθ.setup("Viscosity teta x teta", viscosityθθ, minV, maxV));
	tetaGroup.add(probabilitySliderθα.setup("Probability teta x alpha", probabilityθα, minI, maxI));
	tetaGroup.add(probabilitySliderθβ.setup("Probability teta x betha", probabilityθβ, minI, maxI));
	tetaGroup.add(probabilitySliderθγ.setup("Probability teta x gamma", probabilityθγ, minI, maxI));
	tetaGroup.add(probabilitySliderθδ.setup("Probability teta x delta", probabilityθδ, minI, maxI));
	tetaGroup.add(probabilitySliderθε.setup("Probability teta x epsilon", probabilityθε, minI, maxI));
	tetaGroup.add(probabilitySliderθζ.setup("Probability teta x zeta", probabilityθζ, minI, maxI));
	tetaGroup.add(probabilitySliderθη.setup("Probability teta x eta", probabilityθη, minI, maxI));
	tetaGroup.add(probabilitySliderθθ.setup("Probability teta x teta", probabilityθθ, minI, maxI));
	tetaGroup.minimize();
	gui.add(&tetaGroup);

	expGroup.setup("Experimental");
	expGroup.add(attractDarkButton.setup("attractDark (j)"));
	expGroup.add(repulseDarkButton.setup("repulseDark (k)"));
	expGroup.add(freezeButton.setup("Freeze (f)"));
	expGroup.add(boundsToggle.setup("Bounded", true));
	expGroup.add(radiusToogle.setup("infinite radius", false));
	expGroup.add(wallRepelSlider.setup("Wall Repel", wallRepel, 0, 100));
	expGroup.add(gravitySlider.setup("Gravity", worldGravity, -1, 1));
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
	minV = minViscoSlider;
	maxV = maxViscoSlider;
	minI = minProbSlider;
	maxI = maxProbSlider;
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
	probabilityαθ = probabilitySliderαθ;
	probabilityβθ = probabilitySliderβθ;
	probabilityγθ = probabilitySliderγθ;
	probabilityδθ = probabilitySliderδθ;
	probabilityεθ = probabilitySliderεθ;
	probabilityζθ = probabilitySliderζθ;
	probabilityηθ = probabilitySliderηθ;
	probabilityθα = probabilitySliderθα;
	probabilityθβ = probabilitySliderθβ;
	probabilityθγ = probabilitySliderθγ;
	probabilityθδ = probabilitySliderθδ;
	probabilityθε = probabilitySliderθε;
	probabilityθζ = probabilitySliderθζ;
	probabilityθη = probabilitySliderθη;
	probabilityθθ = probabilitySliderθθ;

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
	viscosityαθ = viscositySliderαθ;
	viscosityβθ = viscositySliderβθ;
	viscosityδθ = viscositySliderδθ;
	viscosityγθ = viscositySliderγθ;
	viscosityεθ = viscositySliderεθ;
	viscosityζθ = viscositySliderζθ;
	viscosityηθ = viscositySliderηθ;
	viscosityθα = viscositySliderθα;
	viscosityθβ = viscositySliderθβ;
	viscosityθδ = viscositySliderθδ;
	viscosityθγ = viscositySliderθγ;
	viscosityθε = viscositySliderθε;
	viscosityθζ = viscositySliderθζ;
	viscosityθη = viscositySliderθη;
	viscosityθθ = viscositySliderθθ;

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
		for (auto& slider : powersliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (InterEvoAmount / 100.0F));
			if (*slider < minP) *slider = minP;
			if (*slider > maxP) *slider = maxP;
		}
		for (auto& slider : vsliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (InterEvoAmount / 100.0F));
			if (*slider < minR) *slider = minR;
			if (*slider > maxR) *slider = maxR;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ViscoEvoChance / 100.0F))
	{
		for (auto& slider : viscositysliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ViscoEvoAmount / 100.0F));
			if (*slider < minV) *slider = minV;
			if (*slider > maxV) *slider = maxV;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ProbEvoChance / 100.0F))
	{
		for (auto& slider : probabilitysliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ProbEvoAmount / 100.0F));
			if (*slider < minI) *slider = minI;
			if (*slider > maxI) *slider = maxI;
		}
	}

	if (numberSliderα > 0)
	{
		interaction(&alpha, &alpha, powerSliderαα, vSliderαα, viscosityαα, probabilityαα);
		if (numberSliderβ > 0) interaction(&alpha, &betha, powerSliderαβ, vSliderαβ, viscosityαβ, probabilityαβ);
		if (numberSliderγ > 0) interaction(&alpha, &gamma, powerSliderαγ, vSliderαγ, viscosityαγ, probabilityαγ);
		if (numberSliderδ > 0) interaction(&alpha, &elta, powerSliderαδ, vSliderαδ, viscosityαδ, probabilityαδ);
		if (numberSliderε > 0) interaction(&alpha, &epsilon, powerSliderαε, vSliderαε, viscosityαε, probabilityαε);
		if (numberSliderζ > 0) interaction(&alpha, &zeta, powerSliderαζ, vSliderαζ, viscosityαζ, probabilityαζ);
		if (numberSliderη > 0) interaction(&alpha, &eta, powerSliderαη, vSliderαη, viscosityαη, probabilityαη);
		if (numberSliderθ > 0) interaction(&alpha, &teta, powerSliderαθ, vSliderαθ, viscosityαθ, probabilityαθ);
	}

	if (numberSliderβ > 0)
	{
		interaction(&betha, &betha, powerSliderββ, vSliderββ, viscosityββ, probabilityββ);
		if (numberSliderα > 0) interaction(&betha, &alpha, powerSliderβα, vSliderβα, viscosityβα, probabilityβα);
		if (numberSliderγ > 0) interaction(&betha, &gamma, powerSliderβγ, vSliderβγ, viscosityβγ, probabilityβγ);
		if (numberSliderδ > 0) interaction(&betha, &elta, powerSliderβδ, vSliderβδ, viscosityβδ, probabilityβδ);
		if (numberSliderε > 0) interaction(&betha, &epsilon, powerSliderβε, vSliderβε, viscosityβε, probabilityβε);
		if (numberSliderζ > 0) interaction(&betha, &zeta, powerSliderβζ, vSliderβζ, viscosityβζ, probabilityβζ);
		if (numberSliderη > 0) interaction(&betha, &eta, powerSliderβη, vSliderβη, viscosityβη, probabilityβη);
		if (numberSliderθ > 0) interaction(&betha, &teta, powerSliderβθ, vSliderβθ, viscosityβθ, probabilityβθ);
	}

	if (numberSliderγ > 0)
	{
		interaction(&gamma, &gamma, powerSliderγγ, vSliderγγ, viscosityγγ, probabilityγγ);
		if (numberSliderα > 0) interaction(&gamma, &alpha, powerSliderγα, vSliderγα, viscosityγα, probabilityγα);
		if (numberSliderβ > 0) interaction(&gamma, &betha, powerSliderγβ, vSliderγβ, viscosityγβ, probabilityγβ);
		if (numberSliderδ > 0) interaction(&gamma, &elta, powerSliderγδ, vSliderγδ, viscosityγδ, probabilityγδ);
		if (numberSliderε > 0) interaction(&gamma, &epsilon, powerSliderγε, vSliderγε, viscosityγε, probabilityγε);
		if (numberSliderζ > 0) interaction(&gamma, &zeta, powerSliderγζ, vSliderγζ, viscosityγζ, probabilityγζ);
		if (numberSliderη > 0) interaction(&gamma, &eta, powerSliderγη, vSliderγη, viscosityγη, probabilityγη);
		if (numberSliderθ > 0) interaction(&gamma, &teta, powerSliderγθ, vSliderγθ, viscosityγθ, probabilityγθ);
	}

	if (numberSliderδ > 0)
	{
		interaction(&elta, &elta, powerSliderδδ, vSliderδδ, viscosityδδ, probabilityδδ);
		if (numberSliderα > 0) interaction(&elta, &alpha, powerSliderδα, vSliderδα, viscosityδα, probabilityδα);
		if (numberSliderβ > 0) interaction(&elta, &betha, powerSliderδβ, vSliderδβ, viscosityδβ, probabilityδβ);
		if (numberSliderγ > 0) interaction(&elta, &gamma, powerSliderδγ, vSliderδγ, viscosityδγ, probabilityδγ);
		if (numberSliderε > 0) interaction(&elta, &epsilon, powerSliderδε, vSliderδε, viscosityδε, probabilityδε);
		if (numberSliderζ > 0) interaction(&elta, &zeta, powerSliderδζ, vSliderδζ, viscosityδζ, probabilityδζ);
		if (numberSliderη > 0) interaction(&elta, &eta, powerSliderδη, vSliderδη, viscosityδη, probabilityδη);
		if (numberSliderθ > 0) interaction(&elta, &teta, powerSliderδθ, vSliderδθ, viscosityδθ, probabilityδθ);
	}

	if (numberSliderε > 0)
	{
		interaction(&epsilon, &epsilon, powerSliderεε, vSliderεε, viscosityεε, probabilityεε);
		if (numberSliderα > 0) interaction(&epsilon, &alpha, powerSliderεα, vSliderεα, viscosityεα, probabilityεα);
		if (numberSliderβ > 0) interaction(&epsilon, &betha, powerSliderεβ, vSliderεβ, viscosityεβ, probabilityεβ);
		if (numberSliderγ > 0) interaction(&epsilon, &gamma, powerSliderεγ, vSliderεγ, viscosityεγ, probabilityεγ);
		if (numberSliderδ > 0) interaction(&epsilon, &elta, powerSliderεδ, vSliderεδ, viscosityεδ, probabilityεδ);
		if (numberSliderζ > 0) interaction(&epsilon, &zeta, powerSliderεζ, vSliderεζ, viscosityεζ, probabilityεζ);
		if (numberSliderη > 0) interaction(&epsilon, &eta, powerSliderεη, vSliderεη, viscosityεη, probabilityεη);
		if (numberSliderθ > 0) interaction(&epsilon, &teta, powerSliderεθ, vSliderεθ, viscosityεθ, probabilityεθ);
	}

	if (numberSliderζ > 0)
	{
		interaction(&zeta, &zeta, powerSliderζζ, vSliderζζ, viscosityζζ, probabilityζζ);
		if (numberSliderα > 0) interaction(&zeta, &alpha, powerSliderζα, vSliderζα, viscosityζα, probabilityζα);
		if (numberSliderβ > 0) interaction(&zeta, &betha, powerSliderζβ, vSliderζβ, viscosityζβ, probabilityζβ);
		if (numberSliderγ > 0) interaction(&zeta, &gamma, powerSliderζγ, vSliderζγ, viscosityζγ, probabilityζγ);
		if (numberSliderδ > 0) interaction(&zeta, &elta, powerSliderζδ, vSliderζδ, viscosityζδ, probabilityζδ);
		if (numberSliderε > 0) interaction(&zeta, &epsilon, powerSliderζε, vSliderζε, viscosityζε, probabilityζε);
		if (numberSliderη > 0) interaction(&zeta, &eta, powerSliderζη, vSliderζη, viscosityζη, probabilityζη);
		if (numberSliderθ > 0) interaction(&zeta, &teta, powerSliderζθ, vSliderζθ, viscosityζθ, probabilityζθ);
	}

	if (numberSliderη > 0)
	{
		interaction(&eta, &eta, powerSliderηη, vSliderηη, viscosityηη, probabilityηη);
		if (numberSliderα > 0) interaction(&eta, &alpha, powerSliderηα, vSliderηα, viscosityηα, probabilityηα);
		if (numberSliderβ > 0) interaction(&eta, &betha, powerSliderηβ, vSliderηβ, viscosityηβ, probabilityηβ);
		if (numberSliderγ > 0) interaction(&eta, &gamma, powerSliderηγ, vSliderηγ, viscosityηγ, probabilityηγ);
		if (numberSliderδ > 0) interaction(&eta, &elta, powerSliderηδ, vSliderηδ, viscosityηδ, probabilityηδ);
		if (numberSliderε > 0) interaction(&eta, &epsilon, powerSliderηε, vSliderηε, viscosityηε, probabilityηε);
		if (numberSliderζ > 0) interaction(&eta, &zeta, powerSliderηζ, vSliderηζ, viscosityηζ, probabilityηζ);
		if (numberSliderθ > 0) interaction(&eta, &teta, powerSliderηθ, vSliderηθ, viscosityηθ, probabilityηθ);
	}

	if (numberSliderθ > 0)
	{
		interaction(&teta, &teta, powerSliderθθ, vSliderθθ, viscosityθθ, probabilityθθ);
		if (numberSliderα > 0) interaction(&teta, &alpha, powerSliderθα, vSliderθα, viscosityθα, probabilityθα);
		if (numberSliderβ > 0) interaction(&teta, &betha, powerSliderθβ, vSliderθβ, viscosityθβ, probabilityθβ);
		if (numberSliderγ > 0) interaction(&teta, &gamma, powerSliderθγ, vSliderθγ, viscosityθγ, probabilityθγ);
		if (numberSliderδ > 0) interaction(&teta, &elta, powerSliderθδ, vSliderθδ, viscosityθδ, probabilityθδ);
		if (numberSliderε > 0) interaction(&teta, &epsilon, powerSliderθε, vSliderθε, viscosityθε, probabilityθε);
		if (numberSliderζ > 0) interaction(&teta, &zeta, powerSliderθζ, vSliderθζ, viscosityθζ, probabilityθζ);
		if (numberSliderη > 0) interaction(&teta, &eta, powerSliderθη, vSliderθη, viscosityθη, probabilityθη);
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
	if (attractDarkButton)
	{
		attractDark();
	}
	if (repulseDarkButton)
	{
		repulseDark();
	}
	if (freezeButton)
	{
		freeze();
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
	if (rndDarkButton)
	{
		rndDark();
	}
	if (numberSliderα > 0) { Draw(&alpha); }
	if (numberSliderβ > 0) { Draw(&betha); }
	if (numberSliderδ > 0) { Draw(&elta); }
	if (numberSliderγ > 0) { Draw(&gamma); }
	if (numberSliderε > 0) { Draw(&epsilon); }
	if (numberSliderζ > 0) { Draw(&zeta); }
	if (numberSliderη > 0) { Draw(&eta); }
	if (numberSliderθ > 0) { Draw(&teta); }
	if (numberSliderα < 0.0F) numberSliderα = 0;
	if (numberSliderβ < 0.0F) numberSliderβ = 0;
	if (numberSliderδ < 0.0F) numberSliderδ = 0;
	if (numberSliderγ < 0.0F) numberSliderγ = 0;
	if (numberSliderε < 0.0F) numberSliderε = 0;
	if (numberSliderζ < 0.0F) numberSliderζ = 0;
	if (numberSliderη < 0.0F) numberSliderη = 0;
	if (numberSliderθ < 0.0F) numberSliderθ = 0;

	gui.draw();
}

void ofApp::keyPressed(int key)
{
	if (key == ' ')
	{
		random();
		restart();
	}
	if (key == 'j')
	{
		attractDark();
	}
	if (key == 'k')
	{
		repulseDark();
	}
	if (key == 'f')
	{
		freeze();
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
	if (key == 'd')
	{
		rndDark();
	}
	if (key == 'r')
	{
		restart();
	}
}
