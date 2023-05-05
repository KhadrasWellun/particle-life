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
float minAttP = -50;
float maxAttP = 50;
float minAttR = 0;
float maxAttR = 100;
float minRepP = -50;
float maxRepP = 50;
float minRepR = 0;
float maxRepR = 100;
float minAttV = 0.7;
float maxAttV = 1.0;
float minRepV = 0.7;
float maxRepV = 1.0;
float minAttI = 0.7;
float maxAttI = 100.0;
float minRepI = 0.7;
float maxRepI = 100.0;

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

void ofApp::interaction(std::vector<point>* Group1, const std::vector<point>* Group2, float attractForce, float repelForce, float attractRadius, float repelRadius, float attractViscosity, float repelViscosity, float attractProbability, float repelProbability)
{
	// const float g = G / -100;	//Gravity coefficient

	const bool radius_toggle = radiusToogle;

	boundHeight = ofGetHeight();
	boundWidth = ofGetWidth();


	const auto group1size = Group1->size();
	const auto group2size = Group2->size();
	std::random_device rd;
#pragma omp parallel
	{
#pragma omp for
		for (auto i = 0; i < group1size; i++) {
			auto& p1 = (*Group1)[i];
			float fx = 0;
			float fy = 0;

			for (auto j = 0; j < group2size; j++) {
				const auto& p2 = (*Group2)[j];

				const auto dx = p1.x - p2.x;
				const auto dy = p1.y - p2.y;
				const auto r = dx * dx + dy * dy;

				if (r < attractRadius * attractRadius && r != 0.0F) {
					if (rd() % 100 < attractProbability) {
						fx += (dx / std::sqrt(r)) * attractForce;
						fy += (dy / std::sqrt(r)) * attractForce;
					}
				}
				else if (r < repelRadius * repelRadius && r != 0.0F) {
					if (rd() % 100 < repelProbability) {
						fx -= (dx / std::sqrt(r)) * repelForce;
						fy -= (dy / std::sqrt(r)) * repelForce;
					}
				}
			}

			float forceViscosity = (fx > 0) ? attractViscosity : repelViscosity;
			p1.vx = (p1.vx + fx) * (1 - forceViscosity);
			p1.vy = (p1.vy + fy) * (1 - forceViscosity) + worldGravity;

			// Add the rest of the code for wall repel, updating position, and checking for canvas bounds
			// ...
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
				// Checking for canvas bounds and implementing toroidal wrapping
			{
				if (p1.x < 0)
				{
					p1.x += boundWidth;
				}
				else if (p1.x > boundWidth)
				{
					p1.x -= boundWidth;
				}

				if (p1.y < 0)
				{
					p1.y += boundHeight;
				}
				else if (p1.y > boundHeight)
				{
					p1.y -= boundHeight;
				}
			}

			//Acest cod verifică dacă o particulă a ieșit din limitele ecranului pe oricare dintre margini și ajustează coordonatele în consecință pentru a crea efectul de wrapping toroidal.

		}

	}
}
void ofApp::restart()
{
	if (numberSliderα > 0) { alpha = CreatePoints(numberSliderα, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderβ > 0) { betha = CreatePoints(numberSliderβ, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderγ > 0) { gamma = CreatePoints(numberSliderγ, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderδ > 0) { elta = CreatePoints(numberSliderδ, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderε > 0) { epsilon = CreatePoints(numberSliderε, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderζ > 0) { zeta = CreatePoints(numberSliderζ, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderη > 0) { eta = CreatePoints(numberSliderη, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
	if (numberSliderθ > 0) { teta = CreatePoints(numberSliderθ, ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)); }
}
	
	void ofApp::random()
{
	AttractEvoProbSlider = RandomFloat(0.1, 1.5);
	AttractEvoAmountSlider = RandomFloat(0.1, 3);
	ProbAttractEvoProbSlider = RandomFloat(0.1, 1.5);
	ProbAttractEvoAmountSlider = RandomFloat(0.1, 3);
	ViscoAttractEvoProbSlider = RandomFloat(0.1, 1.5);
	ViscoAttractEvoAmountSlider = RandomFloat(0.1, 3);
	RepelEvoProbSlider = RandomFloat(0.1, 1.5);
	RepelEvoAmountSlider = RandomFloat(0.1, 3);
	ProbRepelEvoProbSlider = RandomFloat(0.1, 1.5);
	ProbRepelEvoAmountSlider = RandomFloat(0.1, 3);
	ViscoRepelEvoProbSlider = RandomFloat(0.1, 1.5);
	ViscoRepelEvoAmountSlider = RandomFloat(0.1, 3);


	numberSliderβ = RandomInt(500, 2000);
	numberSliderα = RandomInt(500, 2000);
	numberSliderδ = RandomInt(500, 2000);
	numberSliderγ = RandomInt(500, 2000);
	numberSliderε = RandomInt(500, 2000);
	numberSliderζ = RandomInt(500, 2000);
	numberSliderη = RandomInt(500, 2000);
	numberSliderθ = RandomInt(500, 2000);

	viscositySlider = RandomFloat(minAttV, maxAttV);
	probabilitySlider = RandomFloat(minAttI, maxAttI);

	ViscosityAttractSliderαα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderβα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderββ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderγα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderδα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderεα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderζα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderηα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηθ = RandomFloat(minAttV, maxAttV);
		
	ViscosityAttractSliderθα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθθ = RandomFloat(minAttV, maxAttV);

	ProbAttractSliderαα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderβα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderββ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderγα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderδα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderεα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderζα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderηα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderθα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθθ = RandomFloat(minAttI, maxAttI);

	ViscosityRepelSliderαα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderβα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderββ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderγα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderδα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderεα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderζα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderηα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderθα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθθ = RandomFloat(minRepV, maxRepV);

	ProbRepelSliderαα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderβα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderββ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderγα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderδα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderεα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderζα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderηα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηθ = RandomFloat(minRepI, maxRepI);
		
	ProbRepelSliderθα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθθ = RandomFloat(minRepI, maxRepI);

	// Alpha
	AttractPowerSliderαα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderαβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderαα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderαβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Betha
	
	AttractPowerSliderβα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderββ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderββ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderβα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderββ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderββ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Gamma
	
	AttractPowerSliderγα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderγβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderγα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderγβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Delta
	
	AttractPowerSliderδα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderδβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderδα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderδβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Epsilon
	
	AttractPowerSliderεα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderεβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderεα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderεβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Zeta
	
	AttractPowerSliderζα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderζβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderζα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderζβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Etha
	
	AttractPowerSliderηα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderηβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderηα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderηβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Tetha
	
	AttractPowerSliderθα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderθβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderθα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderθβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
}
void ofApp::rndrel()
{
	viscositySlider = RandomFloat(minAttV, maxAttV);
	probabilitySlider = RandomFloat(minAttI, maxAttI);

	ViscosityAttractSliderαα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderβα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderββ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderγα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderδα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderεα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderζα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderηα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderθα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθθ = RandomFloat(minAttV, maxAttV);

	ProbAttractSliderαα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderβα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderββ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderγα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderδα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderεα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderζα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderηα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderθα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθθ = RandomFloat(minAttI, maxAttI);

	ViscosityRepelSliderαα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderβα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderββ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderγα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderδα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderεα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderζα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderηα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderθα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθθ = RandomFloat(minRepV, maxRepV);

	ProbRepelSliderαα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderβα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderββ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderγα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderδα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderεα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderζα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderηα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderθα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθθ = RandomFloat(minRepI, maxRepI);

	// Alpha
	AttractPowerSliderαα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderαβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderαα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderαβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Betha

	AttractPowerSliderβα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderββ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderββ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderβα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderββ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderββ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Gamma

	AttractPowerSliderγα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderγβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderγα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderγβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Delta

	AttractPowerSliderδα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderδβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderδα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderδβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Epsilon

	AttractPowerSliderεα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderεβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderεα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderεβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Zeta

	AttractPowerSliderζα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderζβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderζα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderζβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Etha

	AttractPowerSliderηα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderηβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderηα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderηβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Tetha

	AttractPowerSliderθα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderθβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderθα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderθβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;	
}
void ofApp::monads() {
	numberSliderβ = RandomInt(500, 2000);
	numberSliderα = RandomInt(500, 2000);
	numberSliderδ = RandomInt(500, 2000);
	numberSliderγ = RandomInt(500, 2000);
	numberSliderε = RandomInt(500, 2000);
	numberSliderζ = RandomInt(500, 2000);
	numberSliderη = RandomInt(500, 2000);
	numberSliderθ = RandomInt(500, 2000);
}
void ofApp::rndvsc() {
	viscositySlider = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderαα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderαθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderβα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderββ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderβθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderγα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderγθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderδα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderδθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderεα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderεθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderζα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderζθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderηα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderηθ = RandomFloat(minAttV, maxAttV);

	ViscosityAttractSliderθα = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθβ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθδ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθγ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθε = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθζ = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθη = RandomFloat(minAttV, maxAttV);
	ViscosityAttractSliderθθ = RandomFloat(minAttV, maxAttV);

	ViscosityRepelSliderαα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderαθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderβα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderββ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderβθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderγα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderγθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderδα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderδθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderεα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderεθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderζα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderζθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderηα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderηθ = RandomFloat(minRepV, maxRepV);

	ViscosityRepelSliderθα = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθβ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθδ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθγ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθε = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθζ = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθη = RandomFloat(minRepV, maxRepV);
	ViscosityRepelSliderθθ = RandomFloat(minRepV, maxRepV);
}
void ofApp::rndprob() {
	probabilitySlider = RandomFloat(minAttI, maxAttI);
	
	ProbAttractSliderαα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderαθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderβα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderββ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderβθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderγα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderγθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderδα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderδθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderεα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderεθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderζα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderζθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderηα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderηθ = RandomFloat(minAttI, maxAttI);

	ProbAttractSliderθα = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθβ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθγ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθδ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθε = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθζ = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθη = RandomFloat(minAttI, maxAttI);
	ProbAttractSliderθθ = RandomFloat(minAttI, maxAttI);

	ProbRepelSliderαα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderαθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderβα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderββ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderβθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderγα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderγθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderδα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderδθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderεα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderεθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderζα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderζθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderηα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderηθ = RandomFloat(minRepI, maxRepI);

	ProbRepelSliderθα = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθβ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθγ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθδ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθε = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθζ = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθη = RandomFloat(minRepI, maxRepI);
	ProbRepelSliderθθ = RandomFloat(minRepI, maxRepI);
}
void ofApp::rndir() {
	// Alpha
	AttractPowerSliderαα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderαθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderαβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderαθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderαα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderαθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderαβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderαθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Betha

	AttractPowerSliderβα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderββ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderβθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderββ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderβθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderβα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderββ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderβθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderββ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderβθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Gamma

	AttractPowerSliderγα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderγθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderγβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderγθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderγα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderγθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderγβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderγθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Delta

	AttractPowerSliderδα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderδθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderδβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderδθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderδα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderδθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderδβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderδθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Epsilon

	AttractPowerSliderεα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderεθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderεβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderεθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderεα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderεθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderεβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderεθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Zeta

	AttractPowerSliderζα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderζθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderζβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderζθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderζα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderζθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderζβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderζθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Etha

	AttractPowerSliderηα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderηθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderηβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderηθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderηα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderηθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderηβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderηθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;

	// Tetha

	AttractPowerSliderθα = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθβ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθδ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθγ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθε = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθζ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθη = RandomFloat(minAttP, maxAttP) * AttractForceVariance;
	AttractPowerSliderθθ = RandomFloat(minAttP, maxAttP) * AttractForceVariance;

	AttractDistanceSliderθβ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθα = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθδ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθγ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθε = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθζ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθη = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;
	AttractDistanceSliderθθ = RandomFloat(minAttR, maxAttR) * AttractRadiusVariance;

	RepelPowerSliderθα = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθβ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθδ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθγ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθε = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθζ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθη = RandomFloat(minRepP, maxRepP) * RepelForceVariance;
	RepelPowerSliderθθ = RandomFloat(minRepP, maxRepP) * RepelForceVariance;

	RepelDistanceSliderθβ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθα = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθδ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθγ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθε = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθζ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθη = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
	RepelDistanceSliderθθ = RandomFloat(minRepR, maxRepR) * RepelRadiusVariance;
}
void ofApp::freeze() {
	AttractEvoProbSlider = 0;
	AttractEvoAmountSlider = 0;
	ProbAttractEvoProbSlider = 0;
	ProbAttractEvoAmountSlider = 0;
	ViscoAttractEvoProbSlider = 0;
	ViscoAttractEvoAmountSlider = 0;
	RepelEvoProbSlider = 0;
	RepelEvoAmountSlider = 0;
	ProbRepelEvoProbSlider = 0;
	ProbRepelEvoAmountSlider = 0;
	ViscoRepelEvoProbSlider = 0;
	ViscoRepelEvoAmountSlider = 0;

	viscositySlider = 100;

	ViscosityAttractSliderαα = 100;
	ViscosityAttractSliderαβ = 100;
	ViscosityAttractSliderαδ = 100;
	ViscosityAttractSliderαγ = 100;
	ViscosityAttractSliderαε = 100;
	ViscosityAttractSliderαζ = 100;
	ViscosityAttractSliderαη = 100;

	ViscosityAttractSliderβα = 100;
	ViscosityAttractSliderββ = 100;
	ViscosityAttractSliderβδ = 100;
	ViscosityAttractSliderβγ = 100;
	ViscosityAttractSliderβε = 100;
	ViscosityAttractSliderβζ = 100;
	ViscosityAttractSliderβη = 100;

	ViscosityAttractSliderγα = 100;
	ViscosityAttractSliderγβ = 100;
	ViscosityAttractSliderγδ = 100;
	ViscosityAttractSliderγγ = 100;
	ViscosityAttractSliderγε = 100;
	ViscosityAttractSliderγζ = 100;
	ViscosityAttractSliderγη = 100;

	ViscosityAttractSliderδα = 100;
	ViscosityAttractSliderδβ = 100;
	ViscosityAttractSliderδδ = 100;
	ViscosityAttractSliderδγ = 100;
	ViscosityAttractSliderδε = 100;
	ViscosityAttractSliderδζ = 100;
	ViscosityAttractSliderδη = 100;

	ViscosityAttractSliderεα = 100;
	ViscosityAttractSliderεβ = 100;
	ViscosityAttractSliderεδ = 100;
	ViscosityAttractSliderεγ = 100;
	ViscosityAttractSliderεε = 100;
	ViscosityAttractSliderεζ = 100;
	ViscosityAttractSliderεη = 100;

	ViscosityAttractSliderζα = 100;
	ViscosityAttractSliderζβ = 100;
	ViscosityAttractSliderζδ = 100;
	ViscosityAttractSliderζγ = 100;
	ViscosityAttractSliderζε = 100;
	ViscosityAttractSliderζζ = 100;
	ViscosityAttractSliderζη = 100;

	ViscosityAttractSliderηα = 100;
	ViscosityAttractSliderηβ = 100;
	ViscosityAttractSliderηδ = 100;
	ViscosityAttractSliderηγ = 100;
	ViscosityAttractSliderηε = 100;
	ViscosityAttractSliderηζ = 100;
	ViscosityAttractSliderηη = 100;

	ViscosityAttractSliderαθ = 100;
	ViscosityAttractSliderβθ = 100;
	ViscosityAttractSliderδθ = 100;
	ViscosityAttractSliderγθ = 100;
	ViscosityAttractSliderεθ = 100;
	ViscosityAttractSliderζθ = 100;
	ViscosityAttractSliderηθ = 100;
	ViscosityAttractSliderθα = 100;
	ViscosityAttractSliderθβ = 100;
	ViscosityAttractSliderθδ = 100;
	ViscosityAttractSliderθγ = 100;
	ViscosityAttractSliderθε = 100;
	ViscosityAttractSliderθζ = 100;
	ViscosityAttractSliderθη = 100;
	ViscosityAttractSliderθθ = 100;

	ViscosityRepelSliderαα = 100;
	ViscosityRepelSliderαβ = 100;
	ViscosityRepelSliderαδ = 100;
	ViscosityRepelSliderαγ = 100;
	ViscosityRepelSliderαε = 100;
	ViscosityRepelSliderαζ = 100;
	ViscosityRepelSliderαη = 100;

	ViscosityRepelSliderβα = 100;
	ViscosityRepelSliderββ = 100;
	ViscosityRepelSliderβδ = 100;
	ViscosityRepelSliderβγ = 100;
	ViscosityRepelSliderβε = 100;
	ViscosityRepelSliderβζ = 100;
	ViscosityRepelSliderβη = 100;

	ViscosityRepelSliderγα = 100;
	ViscosityRepelSliderγβ = 100;
	ViscosityRepelSliderγδ = 100;
	ViscosityRepelSliderγγ = 100;
	ViscosityRepelSliderγε = 100;
	ViscosityRepelSliderγζ = 100;
	ViscosityRepelSliderγη = 100;

	ViscosityRepelSliderδα = 100;
	ViscosityRepelSliderδβ = 100;
	ViscosityRepelSliderδδ = 100;
	ViscosityRepelSliderδγ = 100;
	ViscosityRepelSliderδε = 100;
	ViscosityRepelSliderδζ = 100;
	ViscosityRepelSliderδη = 100;

	ViscosityRepelSliderεα = 100;
	ViscosityRepelSliderεβ = 100;
	ViscosityRepelSliderεδ = 100;
	ViscosityRepelSliderεγ = 100;
	ViscosityRepelSliderεε = 100;
	ViscosityRepelSliderεζ = 100;
	ViscosityRepelSliderεη = 100;

	ViscosityRepelSliderζα = 100;
	ViscosityRepelSliderζβ = 100;
	ViscosityRepelSliderζδ = 100;
	ViscosityRepelSliderζγ = 100;
	ViscosityRepelSliderζε = 100;
	ViscosityRepelSliderζζ = 100;
	ViscosityRepelSliderζη = 100;

	ViscosityRepelSliderηα = 100;
	ViscosityRepelSliderηβ = 100;
	ViscosityRepelSliderηδ = 100;
	ViscosityRepelSliderηγ = 100;
	ViscosityRepelSliderηε = 100;
	ViscosityRepelSliderηζ = 100;
	ViscosityRepelSliderηη = 100;

	ViscosityRepelSliderαθ = 100;
	ViscosityRepelSliderβθ = 100;
	ViscosityRepelSliderδθ = 100;
	ViscosityRepelSliderγθ = 100;
	ViscosityRepelSliderεθ = 100;
	ViscosityRepelSliderζθ = 100;
	ViscosityRepelSliderηθ = 100;
	ViscosityRepelSliderθα = 100;
	ViscosityRepelSliderθβ = 100;
	ViscosityRepelSliderθδ = 100;
	ViscosityRepelSliderθγ = 100;
	ViscosityRepelSliderθε = 100;
	ViscosityRepelSliderθζ = 100;
	ViscosityRepelSliderθη = 100;
	ViscosityRepelSliderθθ = 100;

	probabilitySlider = 0;

	ProbAttractSliderαα = 0;
	ProbAttractSliderαβ = 0;
	ProbAttractSliderαγ = 0;
	ProbAttractSliderαδ = 0;
	ProbAttractSliderαε = 0;
	ProbAttractSliderαζ = 0;
	ProbAttractSliderαη = 0;

	ProbAttractSliderβα = 0;
	ProbAttractSliderββ = 0;
	ProbAttractSliderβγ = 0;
	ProbAttractSliderβδ = 0;
	ProbAttractSliderβε = 0;
	ProbAttractSliderβζ = 0;
	ProbAttractSliderβη = 0;

	ProbAttractSliderγα = 0;
	ProbAttractSliderγβ = 0;
	ProbAttractSliderγγ = 0;
	ProbAttractSliderγδ = 0;
	ProbAttractSliderγε = 0;
	ProbAttractSliderγζ = 0;
	ProbAttractSliderγη = 0;

	ProbAttractSliderδα = 0;
	ProbAttractSliderδβ = 0;
	ProbAttractSliderδγ = 0;
	ProbAttractSliderδδ = 0;
	ProbAttractSliderδε = 0;
	ProbAttractSliderδζ = 0;
	ProbAttractSliderδη = 0;

	ProbAttractSliderεα = 0;
	ProbAttractSliderεβ = 0;
	ProbAttractSliderεγ = 0;
	ProbAttractSliderεδ = 0;
	ProbAttractSliderεε = 0;
	ProbAttractSliderεζ = 0;
	ProbAttractSliderεη = 0;

	ProbAttractSliderζα = 0;
	ProbAttractSliderζβ = 0;
	ProbAttractSliderζγ = 0;
	ProbAttractSliderζδ = 0;
	ProbAttractSliderζε = 0;
	ProbAttractSliderζζ = 0;
	ProbAttractSliderζη = 0;

	ProbAttractSliderηα = 0;
	ProbAttractSliderηβ = 0;
	ProbAttractSliderηγ = 0;
	ProbAttractSliderηδ = 0;
	ProbAttractSliderηε = 0;
	ProbAttractSliderηζ = 0;
	ProbAttractSliderηη = 0;

	ProbAttractSliderαθ = 0;
	ProbAttractSliderβθ = 0;
	ProbAttractSliderγθ = 0;
	ProbAttractSliderδθ = 0;
	ProbAttractSliderεθ = 0;
	ProbAttractSliderζθ = 0;
	ProbAttractSliderηθ = 0;
	ProbAttractSliderθα = 0;
	ProbAttractSliderθβ = 0;
	ProbAttractSliderθγ = 0;
	ProbAttractSliderθδ = 0;
	ProbAttractSliderθε = 0;
	ProbAttractSliderθζ = 0;
	ProbAttractSliderθη = 0;
	ProbAttractSliderθθ = 0;

	ProbRepelSliderαα = 0;
	ProbRepelSliderαβ = 0;
	ProbRepelSliderαγ = 0;
	ProbRepelSliderαδ = 0;
	ProbRepelSliderαε = 0;
	ProbRepelSliderαζ = 0;
	ProbRepelSliderαη = 0;

	ProbRepelSliderβα = 0;
	ProbRepelSliderββ = 0;
	ProbRepelSliderβγ = 0;
	ProbRepelSliderβδ = 0;
	ProbRepelSliderβε = 0;
	ProbRepelSliderβζ = 0;
	ProbRepelSliderβη = 0;

	ProbRepelSliderγα = 0;
	ProbRepelSliderγβ = 0;
	ProbRepelSliderγγ = 0;
	ProbRepelSliderγδ = 0;
	ProbRepelSliderγε = 0;
	ProbRepelSliderγζ = 0;
	ProbRepelSliderγη = 0;

	ProbRepelSliderδα = 0;
	ProbRepelSliderδβ = 0;
	ProbRepelSliderδγ = 0;
	ProbRepelSliderδδ = 0;
	ProbRepelSliderδε = 0;
	ProbRepelSliderδζ = 0;
	ProbRepelSliderδη = 0;

	ProbRepelSliderεα = 0;
	ProbRepelSliderεβ = 0;
	ProbRepelSliderεγ = 0;
	ProbRepelSliderεδ = 0;
	ProbRepelSliderεε = 0;
	ProbRepelSliderεζ = 0;
	ProbRepelSliderεη = 0;

	ProbRepelSliderζα = 0;
	ProbRepelSliderζβ = 0;
	ProbRepelSliderζγ = 0;
	ProbRepelSliderζδ = 0;
	ProbRepelSliderζε = 0;
	ProbRepelSliderζζ = 0;
	ProbRepelSliderζη = 0;

	ProbRepelSliderηα = 0;
	ProbRepelSliderηβ = 0;
	ProbRepelSliderηγ = 0;
	ProbRepelSliderηδ = 0;
	ProbRepelSliderηε = 0;
	ProbRepelSliderηζ = 0;
	ProbRepelSliderηη = 0;

	ProbRepelSliderαθ = 0;
	ProbRepelSliderβθ = 0;
	ProbRepelSliderγθ = 0;
	ProbRepelSliderδθ = 0;
	ProbRepelSliderεθ = 0;
	ProbRepelSliderζθ = 0;
	ProbRepelSliderηθ = 0;
	ProbRepelSliderθα = 0;
	ProbRepelSliderθβ = 0;
	ProbRepelSliderθγ = 0;
	ProbRepelSliderθδ = 0;
	ProbRepelSliderθε = 0;
	ProbRepelSliderθζ = 0;
	ProbRepelSliderθη = 0;
	ProbRepelSliderθθ = 0;

	// Betha

	AttractPowerSliderββ = 0;
	AttractPowerSliderβα = 0;
	AttractPowerSliderβδ = 0;
	AttractPowerSliderβγ = 0;
	AttractPowerSliderβε = 0;
	AttractPowerSliderβζ = 0;
	AttractPowerSliderβη = 0;

	AttractDistanceSliderββ = 0;
	AttractDistanceSliderβα = 0;
	AttractDistanceSliderβδ = 0;
	AttractDistanceSliderβγ = 0;
	AttractDistanceSliderβε = 0;
	AttractDistanceSliderβζ = 0;
	AttractDistanceSliderβη = 0;

	// Alpha

	AttractPowerSliderαα = 0;
	AttractPowerSliderαβ = 0;
	AttractPowerSliderαδ = 0;
	AttractPowerSliderαγ = 0;
	AttractPowerSliderαε = 0;
	AttractPowerSliderαζ = 0;
	AttractPowerSliderαη = 0;

	AttractDistanceSliderαβ = 0;
	AttractDistanceSliderαα = 0;
	AttractDistanceSliderαδ = 0;
	AttractDistanceSliderαγ = 0;
	AttractDistanceSliderαε = 0;
	AttractDistanceSliderαζ = 0;
	AttractDistanceSliderαη = 0;

	// Delta

	AttractPowerSliderδδ = 0;
	AttractPowerSliderδα = 0;
	AttractPowerSliderδβ = 0;
	AttractPowerSliderδγ = 0;
	AttractPowerSliderδε = 0;
	AttractPowerSliderδζ = 0;
	AttractPowerSliderδη = 0;

	AttractDistanceSliderδβ = 0;
	AttractDistanceSliderδα = 0;
	AttractDistanceSliderδδ = 0;
	AttractDistanceSliderδγ = 0;
	AttractDistanceSliderδε = 0;
	AttractDistanceSliderδζ = 0;
	AttractDistanceSliderδη = 0;

	// Gamma

	AttractPowerSliderγγ = 0;
	AttractPowerSliderγδ = 0;
	AttractPowerSliderγα = 0;
	AttractPowerSliderγβ = 0;
	AttractPowerSliderγε = 0;
	AttractPowerSliderγζ = 0;
	AttractPowerSliderγη = 0;

	AttractDistanceSliderγβ = 0;
	AttractDistanceSliderγα = 0;
	AttractDistanceSliderγδ = 0;
	AttractDistanceSliderγγ = 0;
	AttractDistanceSliderγε = 0;
	AttractDistanceSliderγζ = 0;
	AttractDistanceSliderγη = 0;

	// Epsilon

	AttractPowerSliderεγ = 0;
	AttractPowerSliderεδ = 0;
	AttractPowerSliderεα = 0;
	AttractPowerSliderεβ = 0;
	AttractPowerSliderεε = 0;
	AttractPowerSliderεζ = 0;
	AttractPowerSliderεη = 0;

	AttractDistanceSliderεβ = 0;
	AttractDistanceSliderεα = 0;
	AttractDistanceSliderεδ = 0;
	AttractDistanceSliderεγ = 0;
	AttractDistanceSliderεε = 0;
	AttractDistanceSliderεζ = 0;
	AttractDistanceSliderεη = 0;

	// Zetha

	AttractPowerSliderζγ = 0;
	AttractPowerSliderζδ = 0;
	AttractPowerSliderζα = 0;
	AttractPowerSliderζβ = 0;
	AttractPowerSliderζε = 0;
	AttractPowerSliderζζ = 0;
	AttractPowerSliderζη = 0;

	AttractDistanceSliderζβ = 0;
	AttractDistanceSliderζα = 0;
	AttractDistanceSliderζδ = 0;
	AttractDistanceSliderζγ = 0;
	AttractDistanceSliderζε = 0;
	AttractDistanceSliderζζ = 0;
	AttractDistanceSliderζη = 0;

	// Etha

	AttractPowerSliderηγ = 0;
	AttractPowerSliderηδ = 0;
	AttractPowerSliderηα = 0;
	AttractPowerSliderηβ = 0;
	AttractPowerSliderηε = 0;
	AttractPowerSliderηζ = 0;
	AttractPowerSliderηη = 0;

	AttractDistanceSliderηβ = 0;
	AttractDistanceSliderηα = 0;
	AttractDistanceSliderηδ = 0;
	AttractDistanceSliderηγ = 0;
	AttractDistanceSliderηε = 0;
	AttractDistanceSliderηζ = 0;
	AttractDistanceSliderηη = 0;

	// Etha

	AttractPowerSliderθγ = 0;
	AttractPowerSliderθδ = 0;
	AttractPowerSliderθα = 0;
	AttractPowerSliderθβ = 0;
	AttractPowerSliderθε = 0;
	AttractPowerSliderθζ = 0;
	AttractPowerSliderθη = 0;
	AttractPowerSliderθθ = 0;

	AttractDistanceSliderθβ = 0;
	AttractDistanceSliderθα = 0;
	AttractDistanceSliderθδ = 0;
	AttractDistanceSliderθγ = 0;
	AttractDistanceSliderθε = 0;
	AttractDistanceSliderθζ = 0;
	AttractDistanceSliderθη = 0;
	AttractDistanceSliderθθ = 0;

	AttractPowerSliderγθ = 0;
	AttractPowerSliderδθ = 0;
	AttractPowerSliderαθ = 0;
	AttractPowerSliderβθ = 0;
	AttractPowerSliderεθ = 0;
	AttractPowerSliderζθ = 0;
	AttractPowerSliderηθ = 0;

	AttractDistanceSliderβθ = 0;
	AttractDistanceSliderαθ = 0;
	AttractDistanceSliderδθ = 0;
	AttractDistanceSliderγθ = 0;
	AttractDistanceSliderεθ = 0;
	AttractDistanceSliderζθ = 0;
	AttractDistanceSliderηθ = 0;

	// Betha

	RepelPowerSliderββ = 0;
	RepelPowerSliderβα = 0;
	RepelPowerSliderβδ = 0;
	RepelPowerSliderβγ = 0;
	RepelPowerSliderβε = 0;
	RepelPowerSliderβζ = 0;
	RepelPowerSliderβη = 0;

	RepelDistanceSliderββ = 0;
	RepelDistanceSliderβα = 0;
	RepelDistanceSliderβδ = 0;
	RepelDistanceSliderβγ = 0;
	RepelDistanceSliderβε = 0;
	RepelDistanceSliderβζ = 0;
	RepelDistanceSliderβη = 0;

	// Alpha

	RepelPowerSliderαα = 0;
	RepelPowerSliderαβ = 0;
	RepelPowerSliderαδ = 0;
	RepelPowerSliderαγ = 0;
	RepelPowerSliderαε = 0;
	RepelPowerSliderαζ = 0;
	RepelPowerSliderαη = 0;

	RepelDistanceSliderαβ = 0;
	RepelDistanceSliderαα = 0;
	RepelDistanceSliderαδ = 0;
	RepelDistanceSliderαγ = 0;
	RepelDistanceSliderαε = 0;
	RepelDistanceSliderαζ = 0;
	RepelDistanceSliderαη = 0;

	// Delta

	RepelPowerSliderδδ = 0;
	RepelPowerSliderδα = 0;
	RepelPowerSliderδβ = 0;
	RepelPowerSliderδγ = 0;
	RepelPowerSliderδε = 0;
	RepelPowerSliderδζ = 0;
	RepelPowerSliderδη = 0;

	RepelDistanceSliderδβ = 0;
	RepelDistanceSliderδα = 0;
	RepelDistanceSliderδδ = 0;
	RepelDistanceSliderδγ = 0;
	RepelDistanceSliderδε = 0;
	RepelDistanceSliderδζ = 0;
	RepelDistanceSliderδη = 0;

	// Gamma

	RepelPowerSliderγγ = 0;
	RepelPowerSliderγδ = 0;
	RepelPowerSliderγα = 0;
	RepelPowerSliderγβ = 0;
	RepelPowerSliderγε = 0;
	RepelPowerSliderγζ = 0;
	RepelPowerSliderγη = 0;

	RepelDistanceSliderγβ = 0;
	RepelDistanceSliderγα = 0;
	RepelDistanceSliderγδ = 0;
	RepelDistanceSliderγγ = 0;
	RepelDistanceSliderγε = 0;
	RepelDistanceSliderγζ = 0;
	RepelDistanceSliderγη = 0;

	// Epsilon

	RepelPowerSliderεγ = 0;
	RepelPowerSliderεδ = 0;
	RepelPowerSliderεα = 0;
	RepelPowerSliderεβ = 0;
	RepelPowerSliderεε = 0;
	RepelPowerSliderεζ = 0;
	RepelPowerSliderεη = 0;

	RepelDistanceSliderεβ = 0;
	RepelDistanceSliderεα = 0;
	RepelDistanceSliderεδ = 0;
	RepelDistanceSliderεγ = 0;
	RepelDistanceSliderεε = 0;
	RepelDistanceSliderεζ = 0;
	RepelDistanceSliderεη = 0;

	// Zetha

	RepelPowerSliderζγ = 0;
	RepelPowerSliderζδ = 0;
	RepelPowerSliderζα = 0;
	RepelPowerSliderζβ = 0;
	RepelPowerSliderζε = 0;
	RepelPowerSliderζζ = 0;
	RepelPowerSliderζη = 0;

	RepelDistanceSliderζβ = 0;
	RepelDistanceSliderζα = 0;
	RepelDistanceSliderζδ = 0;
	RepelDistanceSliderζγ = 0;
	RepelDistanceSliderζε = 0;
	RepelDistanceSliderζζ = 0;
	RepelDistanceSliderζη = 0;

	// Etha

	RepelPowerSliderηγ = 0;
	RepelPowerSliderηδ = 0;
	RepelPowerSliderηα = 0;
	RepelPowerSliderηβ = 0;
	RepelPowerSliderηε = 0;
	RepelPowerSliderηζ = 0;
	RepelPowerSliderηη = 0;

	RepelDistanceSliderηβ = 0;
	RepelDistanceSliderηα = 0;
	RepelDistanceSliderηδ = 0;
	RepelDistanceSliderηγ = 0;
	RepelDistanceSliderηε = 0;
	RepelDistanceSliderηζ = 0;
	RepelDistanceSliderηη = 0;

	// Etha

	RepelPowerSliderθγ = 0;
	RepelPowerSliderθδ = 0;
	RepelPowerSliderθα = 0;
	RepelPowerSliderθβ = 0;
	RepelPowerSliderθε = 0;
	RepelPowerSliderθζ = 0;
	RepelPowerSliderθη = 0;
	RepelPowerSliderθθ = 0;

	RepelDistanceSliderθβ = 0;
	RepelDistanceSliderθα = 0;
	RepelDistanceSliderθδ = 0;
	RepelDistanceSliderθγ = 0;
	RepelDistanceSliderθε = 0;
	RepelDistanceSliderθζ = 0;
	RepelDistanceSliderθη = 0;
	RepelDistanceSliderθθ = 0;

	RepelPowerSliderγθ = 0;
	RepelPowerSliderδθ = 0;
	RepelPowerSliderαθ = 0;
	RepelPowerSliderβθ = 0;
	RepelPowerSliderεθ = 0;
	RepelPowerSliderζθ = 0;
	RepelPowerSliderηθ = 0;

	RepelDistanceSliderβθ = 0;
	RepelDistanceSliderαθ = 0;
	RepelDistanceSliderδθ = 0;
	RepelDistanceSliderγθ = 0;
	RepelDistanceSliderεθ = 0;
	RepelDistanceSliderζθ = 0;
	RepelDistanceSliderηθ = 0;
}

/// this is a cheap and quick way to save and load parameters (openFramework have betters ways but requires some additional library setups) 
// Dialog gui tested on windows machine only. Not sure if it works on Mac or Linux too.
void ofApp::saveSettings()
{
	const std::vector<float> settings = {
		AttractPowerSliderαα, AttractPowerSliderαβ, AttractPowerSliderαδ, AttractPowerSliderαγ, AttractPowerSliderαε, AttractPowerSliderαζ, AttractPowerSliderαη, AttractPowerSliderαθ,
		AttractDistanceSliderαα, AttractDistanceSliderαβ, AttractDistanceSliderαδ, AttractDistanceSliderαγ, AttractDistanceSliderαε, AttractDistanceSliderαζ, AttractDistanceSliderαη, AttractDistanceSliderαθ,
		AttractPowerSliderβα, AttractPowerSliderββ, AttractPowerSliderβδ, AttractPowerSliderβγ, AttractPowerSliderβε, AttractPowerSliderβζ, AttractPowerSliderβη, AttractPowerSliderβθ,
		AttractDistanceSliderβα, AttractDistanceSliderββ, AttractDistanceSliderβδ, AttractDistanceSliderβγ, AttractDistanceSliderβε, AttractDistanceSliderβζ, AttractDistanceSliderβη, AttractDistanceSliderβθ,
		AttractPowerSliderγα, AttractPowerSliderγβ, AttractPowerSliderγδ, AttractPowerSliderγγ, AttractPowerSliderγε, AttractPowerSliderγζ, AttractPowerSliderγη, AttractPowerSliderγθ,
		AttractDistanceSliderγα, AttractDistanceSliderγβ, AttractDistanceSliderγδ, AttractDistanceSliderγγ, AttractDistanceSliderγε, AttractDistanceSliderγζ, AttractDistanceSliderγη, AttractDistanceSliderγθ,
		AttractPowerSliderδα, AttractPowerSliderδβ, AttractPowerSliderδδ, AttractPowerSliderδγ, AttractPowerSliderδε, AttractPowerSliderδζ, AttractPowerSliderδη, AttractPowerSliderδθ,
		AttractDistanceSliderδα, AttractDistanceSliderδβ, AttractDistanceSliderδδ, AttractDistanceSliderδγ, AttractDistanceSliderδε, AttractDistanceSliderδζ, AttractDistanceSliderδη, AttractDistanceSliderδθ,
		AttractPowerSliderεα, AttractPowerSliderεβ, AttractPowerSliderεδ, AttractPowerSliderεγ, AttractPowerSliderεε, AttractPowerSliderεζ, AttractPowerSliderεη, AttractPowerSliderεθ,
		AttractDistanceSliderεα, AttractDistanceSliderεβ, AttractDistanceSliderεδ, AttractDistanceSliderεγ, AttractDistanceSliderεε, AttractDistanceSliderεζ, AttractDistanceSliderεη, AttractDistanceSliderεθ,
		AttractPowerSliderζα, AttractPowerSliderζβ, AttractPowerSliderζδ, AttractPowerSliderζγ, AttractPowerSliderζε, AttractPowerSliderζζ, AttractPowerSliderζη, AttractPowerSliderζθ,
		AttractDistanceSliderζα, AttractDistanceSliderζβ, AttractDistanceSliderζδ, AttractDistanceSliderζγ, AttractDistanceSliderζε, AttractDistanceSliderζζ, AttractDistanceSliderζη, AttractDistanceSliderζθ,
		AttractPowerSliderηα, AttractPowerSliderηβ, AttractPowerSliderηδ, AttractPowerSliderηγ, AttractPowerSliderηε, AttractPowerSliderηζ, AttractPowerSliderηη, AttractPowerSliderηθ,
		AttractDistanceSliderηα, AttractDistanceSliderηβ, AttractDistanceSliderηδ, AttractDistanceSliderηγ, AttractDistanceSliderηε, AttractDistanceSliderηζ, AttractDistanceSliderηη, AttractDistanceSliderηθ,
		AttractPowerSliderθα, AttractPowerSliderθβ, AttractPowerSliderθδ, AttractPowerSliderθγ, AttractPowerSliderθε, AttractPowerSliderθζ, AttractPowerSliderθη, AttractPowerSliderθθ,
		AttractDistanceSliderθα, AttractDistanceSliderθβ, AttractDistanceSliderθδ, AttractDistanceSliderθγ, AttractDistanceSliderθε, AttractDistanceSliderθζ, AttractDistanceSliderηθ, AttractDistanceSliderθθ,
		RepelPowerSliderαα, RepelPowerSliderαβ, RepelPowerSliderαδ, RepelPowerSliderαγ, RepelPowerSliderαε, RepelPowerSliderαζ, RepelPowerSliderαη, RepelPowerSliderαθ,
		RepelDistanceSliderαα, RepelDistanceSliderαβ, RepelDistanceSliderαδ, RepelDistanceSliderαγ, RepelDistanceSliderαε, RepelDistanceSliderαζ, RepelDistanceSliderαη, RepelDistanceSliderαθ,
		RepelPowerSliderβα, RepelPowerSliderββ, RepelPowerSliderβδ, RepelPowerSliderβγ, RepelPowerSliderβε, RepelPowerSliderβζ, RepelPowerSliderβη, RepelPowerSliderβθ,
		RepelDistanceSliderβα, RepelDistanceSliderββ, RepelDistanceSliderβδ, RepelDistanceSliderβγ, RepelDistanceSliderβε, RepelDistanceSliderβζ, RepelDistanceSliderβη, RepelDistanceSliderβθ,
		RepelPowerSliderγα, RepelPowerSliderγβ, RepelPowerSliderγδ, RepelPowerSliderγγ, RepelPowerSliderγε, RepelPowerSliderγζ, RepelPowerSliderγη, RepelPowerSliderγθ,
		RepelDistanceSliderγα, RepelDistanceSliderγβ, RepelDistanceSliderγδ, RepelDistanceSliderγγ, RepelDistanceSliderγε, RepelDistanceSliderγζ, RepelDistanceSliderγη, RepelDistanceSliderγθ,
		RepelPowerSliderδα, RepelPowerSliderδβ, RepelPowerSliderδδ, RepelPowerSliderδγ, RepelPowerSliderδε, RepelPowerSliderδζ, RepelPowerSliderδη, RepelPowerSliderδθ,
		RepelDistanceSliderδα, RepelDistanceSliderδβ, RepelDistanceSliderδδ, RepelDistanceSliderδγ, RepelDistanceSliderδε, RepelDistanceSliderδζ, RepelDistanceSliderδη, RepelDistanceSliderδθ,
		RepelPowerSliderεα, RepelPowerSliderεβ, RepelPowerSliderεδ, RepelPowerSliderεγ, RepelPowerSliderεε, RepelPowerSliderεζ, RepelPowerSliderεη, RepelPowerSliderεθ,
		RepelDistanceSliderεα, RepelDistanceSliderεβ, RepelDistanceSliderεδ, RepelDistanceSliderεγ, RepelDistanceSliderεε, RepelDistanceSliderεζ, RepelDistanceSliderεη, RepelDistanceSliderεθ,
		RepelPowerSliderζα, RepelPowerSliderζβ, RepelPowerSliderζδ, RepelPowerSliderζγ, RepelPowerSliderζε, RepelPowerSliderζζ, RepelPowerSliderζη, RepelPowerSliderζθ,
		RepelDistanceSliderζα, RepelDistanceSliderζβ, RepelDistanceSliderζδ, RepelDistanceSliderζγ, RepelDistanceSliderζε, RepelDistanceSliderζζ, RepelDistanceSliderζη, RepelDistanceSliderζθ,
		RepelPowerSliderηα, RepelPowerSliderηβ, RepelPowerSliderηδ, RepelPowerSliderηγ, RepelPowerSliderηε, RepelPowerSliderηζ, RepelPowerSliderηη, RepelPowerSliderηθ,
		RepelDistanceSliderηα, RepelDistanceSliderηβ, RepelDistanceSliderηδ, RepelDistanceSliderηγ, RepelDistanceSliderηε, RepelDistanceSliderηζ, RepelDistanceSliderηη, RepelDistanceSliderηθ,
		RepelPowerSliderθα, RepelPowerSliderθβ, RepelPowerSliderθδ, RepelPowerSliderθγ, RepelPowerSliderθε, RepelPowerSliderθζ, RepelPowerSliderθη, RepelPowerSliderθθ,
		RepelDistanceSliderθα, RepelDistanceSliderθβ, RepelDistanceSliderθδ, RepelDistanceSliderθγ, RepelDistanceSliderθε, RepelDistanceSliderθζ, RepelDistanceSliderηθ, RepelDistanceSliderθθ,
		static_cast<float>(numberSliderα),
		static_cast<float>(numberSliderβ),
		static_cast<float>(numberSliderα),
		static_cast<float>(numberSliderδ),
		static_cast<float>(numberSliderγ),
		static_cast<float>(numberSliderε),
		static_cast<float>(numberSliderζ),
		static_cast<float>(numberSliderη),
		static_cast<float>(numberSliderθ),
		viscositySlider,
		ViscosityAttractSliderαα,
		ViscosityAttractSliderαβ,
		ViscosityAttractSliderαγ,
		ViscosityAttractSliderαδ,
		ViscosityAttractSliderαε,
		ViscosityAttractSliderαζ,
		ViscosityAttractSliderαη,
		ViscosityAttractSliderβα,
		ViscosityAttractSliderββ,
		ViscosityAttractSliderβγ,
		ViscosityAttractSliderβδ,
		ViscosityAttractSliderβε,
		ViscosityAttractSliderβζ,
		ViscosityAttractSliderβη,
		ViscosityAttractSliderγα,
		ViscosityAttractSliderγβ,
		ViscosityAttractSliderγγ,
		ViscosityAttractSliderγδ,
		ViscosityAttractSliderγε,
		ViscosityAttractSliderγζ,
		ViscosityAttractSliderγη,
		ViscosityAttractSliderδα,
		ViscosityAttractSliderδβ,
		ViscosityAttractSliderδγ,
		ViscosityAttractSliderδδ,
		ViscosityAttractSliderδε,
		ViscosityAttractSliderδζ,
		ViscosityAttractSliderδη,
		ViscosityAttractSliderεα,
		ViscosityAttractSliderεβ,
		ViscosityAttractSliderεγ,
		ViscosityAttractSliderεδ,
		ViscosityAttractSliderεε,
		ViscosityAttractSliderεζ,
		ViscosityAttractSliderεη,
		ViscosityAttractSliderζα,
		ViscosityAttractSliderζβ,
		ViscosityAttractSliderζγ,
		ViscosityAttractSliderζδ,
		ViscosityAttractSliderζε,
		ViscosityAttractSliderζζ,
		ViscosityAttractSliderζη,
		ViscosityAttractSliderηα,
		ViscosityAttractSliderηβ,
		ViscosityAttractSliderηγ,
		ViscosityAttractSliderηδ,
		ViscosityAttractSliderηε,
		ViscosityAttractSliderηζ,
		ViscosityAttractSliderηη,
		ViscosityAttractSliderαθ,
		ViscosityAttractSliderβθ,
		ViscosityAttractSliderγθ,
		ViscosityAttractSliderδθ,
		ViscosityAttractSliderεθ,
		ViscosityAttractSliderζθ,
		ViscosityAttractSliderηθ,
		ViscosityAttractSliderθα,
		ViscosityAttractSliderθβ,
		ViscosityAttractSliderθγ,
		ViscosityAttractSliderθδ,
		ViscosityAttractSliderθε,
		ViscosityAttractSliderθζ,
		ViscosityAttractSliderθη,
		ViscosityAttractSliderθθ,
		ViscosityRepelSliderαα,
		ViscosityRepelSliderαβ,
		ViscosityRepelSliderαγ,
		ViscosityRepelSliderαδ,
		ViscosityRepelSliderαε,
		ViscosityRepelSliderαζ,
		ViscosityRepelSliderαη,
		ViscosityRepelSliderβα,
		ViscosityRepelSliderββ,
		ViscosityRepelSliderβγ,
		ViscosityRepelSliderβδ,
		ViscosityRepelSliderβε,
		ViscosityRepelSliderβζ,
		ViscosityRepelSliderβη,
		ViscosityRepelSliderγα,
		ViscosityRepelSliderγβ,
		ViscosityRepelSliderγγ,
		ViscosityRepelSliderγδ,
		ViscosityRepelSliderγε,
		ViscosityRepelSliderγζ,
		ViscosityRepelSliderγη,
		ViscosityRepelSliderδα,
		ViscosityRepelSliderδβ,
		ViscosityRepelSliderδγ,
		ViscosityRepelSliderδδ,
		ViscosityRepelSliderδε,
		ViscosityRepelSliderδζ,
		ViscosityRepelSliderδη,
		ViscosityRepelSliderεα,
		ViscosityRepelSliderεβ,
		ViscosityRepelSliderεγ,
		ViscosityRepelSliderεδ,
		ViscosityRepelSliderεε,
		ViscosityRepelSliderεζ,
		ViscosityRepelSliderεη,
		ViscosityRepelSliderζα,
		ViscosityRepelSliderζβ,
		ViscosityRepelSliderζγ,
		ViscosityRepelSliderζδ,
		ViscosityRepelSliderζε,
		ViscosityRepelSliderζζ,
		ViscosityRepelSliderζη,
		ViscosityRepelSliderηα,
		ViscosityRepelSliderηβ,
		ViscosityRepelSliderηγ,
		ViscosityRepelSliderηδ,
		ViscosityRepelSliderηε,
		ViscosityRepelSliderηζ,
		ViscosityRepelSliderηη,
		ViscosityRepelSliderαθ,
		ViscosityRepelSliderβθ,
		ViscosityRepelSliderγθ,
		ViscosityRepelSliderδθ,
		ViscosityRepelSliderεθ,
		ViscosityRepelSliderζθ,
		ViscosityRepelSliderηθ,
		ViscosityRepelSliderθα,
		ViscosityRepelSliderθβ,
		ViscosityRepelSliderθγ,
		ViscosityRepelSliderθδ,
		ViscosityRepelSliderθε,
		ViscosityRepelSliderθζ,
		ViscosityRepelSliderθη,
		ViscosityRepelSliderθθ,
		probabilitySlider,
		ProbAttractSliderαα,
		ProbAttractSliderαβ,
		ProbAttractSliderαγ,
		ProbAttractSliderαδ,
		ProbAttractSliderαε,
		ProbAttractSliderαζ,
		ProbAttractSliderαη,
		ProbAttractSliderβα,
		ProbAttractSliderββ,
		ProbAttractSliderβγ,
		ProbAttractSliderβδ,
		ProbAttractSliderβε,
		ProbAttractSliderβζ,
		ProbAttractSliderβη,
		ProbAttractSliderγα,
		ProbAttractSliderγβ,
		ProbAttractSliderγγ,
		ProbAttractSliderγδ,
		ProbAttractSliderγε,
		ProbAttractSliderγζ,
		ProbAttractSliderγη,
		ProbAttractSliderδα,
		ProbAttractSliderδβ,
		ProbAttractSliderδγ,
		ProbAttractSliderδδ,
		ProbAttractSliderδε,
		ProbAttractSliderδζ,
		ProbAttractSliderδη,
		ProbAttractSliderεα,
		ProbAttractSliderεβ,
		ProbAttractSliderεγ,
		ProbAttractSliderεδ,
		ProbAttractSliderεε,
		ProbAttractSliderεζ,
		ProbAttractSliderεη,
		ProbAttractSliderζα,
		ProbAttractSliderζβ,
		ProbAttractSliderζγ,
		ProbAttractSliderζδ,
		ProbAttractSliderζε,
		ProbAttractSliderζζ,
		ProbAttractSliderζη,
		ProbAttractSliderηα,
		ProbAttractSliderηβ,
		ProbAttractSliderηγ,
		ProbAttractSliderηδ,
		ProbAttractSliderηε,
		ProbAttractSliderηζ,
		ProbAttractSliderηη,
		ProbAttractSliderαθ,
		ProbAttractSliderβθ,
		ProbAttractSliderγθ,
		ProbAttractSliderδθ,
		ProbAttractSliderεθ,
		ProbAttractSliderζθ,
		ProbAttractSliderηθ,
		ProbAttractSliderθα,
		ProbAttractSliderθβ,
		ProbAttractSliderθγ,
		ProbAttractSliderθδ,
		ProbAttractSliderθε,
		ProbAttractSliderθζ,
		ProbAttractSliderθη,
		ProbAttractSliderθθ,
		ProbRepelSliderαα,
		ProbRepelSliderαβ,
		ProbRepelSliderαγ,
		ProbRepelSliderαδ,
		ProbRepelSliderαε,
		ProbRepelSliderαζ,
		ProbRepelSliderαη,
		ProbRepelSliderβα,
		ProbRepelSliderββ,
		ProbRepelSliderβγ,
		ProbRepelSliderβδ,
		ProbRepelSliderβε,
		ProbRepelSliderβζ,
		ProbRepelSliderβη,
		ProbRepelSliderγα,
		ProbRepelSliderγβ,
		ProbRepelSliderγγ,
		ProbRepelSliderγδ,
		ProbRepelSliderγε,
		ProbRepelSliderγζ,
		ProbRepelSliderγη,
		ProbRepelSliderδα,
		ProbRepelSliderδβ,
		ProbRepelSliderδγ,
		ProbRepelSliderδδ,
		ProbRepelSliderδε,
		ProbRepelSliderδζ,
		ProbRepelSliderδη,
		ProbRepelSliderεα,
		ProbRepelSliderεβ,
		ProbRepelSliderεγ,
		ProbRepelSliderεδ,
		ProbRepelSliderεε,
		ProbRepelSliderεζ,
		ProbRepelSliderεη,
		ProbRepelSliderζα,
		ProbRepelSliderζβ,
		ProbRepelSliderζγ,
		ProbRepelSliderζδ,
		ProbRepelSliderζε,
		ProbRepelSliderζζ,
		ProbRepelSliderζη,
		ProbRepelSliderηα,
		ProbRepelSliderηβ,
		ProbRepelSliderηγ,
		ProbRepelSliderηδ,
		ProbRepelSliderηε,
		ProbRepelSliderηζ,
		ProbRepelSliderηη,
		ProbRepelSliderαθ,
		ProbRepelSliderβθ,
		ProbRepelSliderγθ,
		ProbRepelSliderδθ,
		ProbRepelSliderεθ,
		ProbRepelSliderζθ,
		ProbRepelSliderηθ,
		ProbRepelSliderθα,
		ProbRepelSliderθβ,
		ProbRepelSliderθγ,
		ProbRepelSliderθδ,
		ProbRepelSliderθε,
		ProbRepelSliderθζ,
		ProbRepelSliderθη,
		ProbRepelSliderθθ,
		minAttractPowerSlider,
		maxAttractPowerSlider,
		minRepelPowerSlider,
		maxRepelPowerSlider,
		minAttractRangeSlider,
		maxAttractRangeSlider,
		minRepelRangeSlider,
		maxRepelRangeSlider,
		minAttractViscoSlider,
		maxAttractViscoSlider,
		minRepelViscoSlider,
		maxRepelViscoSlider,
		minAttractProbSlider,
		maxAttractProbSlider,
		minRepelProbSlider,
		maxRepelProbSlider,
		AttractEvoProbSlider,
		AttractEvoAmountSlider,
		ProbAttractEvoProbSlider,
		ProbAttractEvoAmountSlider,
		ViscoAttractEvoProbSlider,
		ViscoAttractEvoAmountSlider,
		RepelEvoProbSlider,
		RepelEvoAmountSlider,
		ProbRepelEvoProbSlider,
		ProbRepelEvoAmountSlider,
		ViscoRepelEvoProbSlider,
		ViscoRepelEvoAmountSlider,
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

	if (p.size() < 550)
	{
		// better checks needed
		ofSystemAlertDialog("Could not read the file!");
	}
	else
	{
AttractPowerSliderαα = p[0];
AttractPowerSliderαβ = p[1];
AttractPowerSliderαδ = p[2];
AttractPowerSliderαγ = p[3];
AttractPowerSliderαε = p[4];
AttractPowerSliderαζ = p[5];
AttractPowerSliderαη = p[6];
AttractPowerSliderαθ = p[7];
AttractDistanceSliderαα = p[8];
AttractDistanceSliderαβ = p[9];
AttractDistanceSliderαδ = p[10];
AttractDistanceSliderαγ = p[11];
AttractDistanceSliderαε = p[12];
AttractDistanceSliderαζ = p[13];
AttractDistanceSliderαη = p[14];
AttractDistanceSliderαθ = p[15];
AttractPowerSliderβα = p[16];
AttractPowerSliderββ = p[17];
AttractPowerSliderβδ = p[18];
AttractPowerSliderβγ = p[19];
AttractPowerSliderβε = p[20];
AttractPowerSliderβζ = p[21];
AttractPowerSliderβη = p[22];
AttractPowerSliderβθ = p[23];
AttractDistanceSliderβα = p[24];
AttractDistanceSliderββ = p[25];
AttractDistanceSliderβδ = p[26];
AttractDistanceSliderβγ = p[27];
AttractDistanceSliderβε = p[28];
AttractDistanceSliderβζ = p[29];
AttractDistanceSliderβη = p[30];
AttractDistanceSliderβθ = p[31];
AttractPowerSliderγα = p[32];
AttractPowerSliderγβ = p[33];
AttractPowerSliderγδ = p[34];
AttractPowerSliderγγ = p[35];
AttractPowerSliderγε = p[36];
AttractPowerSliderγζ = p[37];
AttractPowerSliderγη = p[38];
AttractPowerSliderγθ = p[39];
AttractDistanceSliderγα = p[40];
AttractDistanceSliderγβ = p[41];
AttractDistanceSliderγδ = p[42];
AttractDistanceSliderγγ = p[43];
AttractDistanceSliderγε = p[44];
AttractDistanceSliderγζ = p[45];
AttractDistanceSliderγη = p[46];
AttractDistanceSliderγθ = p[47];
AttractPowerSliderδα = p[48];
AttractPowerSliderδβ = p[49];
AttractPowerSliderδδ = p[50];
AttractPowerSliderδγ = p[51];
AttractPowerSliderδε = p[52];
AttractPowerSliderδζ = p[53];
AttractPowerSliderδη = p[54];
AttractPowerSliderδθ = p[55];
AttractDistanceSliderδα = p[56];
AttractDistanceSliderδβ = p[57];
AttractDistanceSliderδδ = p[58];
AttractDistanceSliderδγ = p[59];
AttractDistanceSliderδε = p[60];
AttractDistanceSliderδζ = p[61];
AttractDistanceSliderδη = p[62];
AttractDistanceSliderδθ = p[63];
AttractPowerSliderεα = p[64];
AttractPowerSliderεβ = p[65];
AttractPowerSliderεδ = p[66];
AttractPowerSliderεγ = p[67];
AttractPowerSliderεε = p[68];
AttractPowerSliderεζ = p[69];
AttractPowerSliderεη = p[70];
AttractPowerSliderεθ = p[71];
AttractDistanceSliderεα = p[72];
AttractDistanceSliderεβ = p[73];
AttractDistanceSliderεδ = p[74];
AttractDistanceSliderεγ = p[75];
AttractDistanceSliderεε = p[76];
AttractDistanceSliderεζ = p[77];
AttractDistanceSliderεη = p[78];
AttractDistanceSliderεθ = p[79];
AttractPowerSliderζα = p[80];
AttractPowerSliderζβ = p[81];
AttractPowerSliderζδ = p[82];
AttractPowerSliderζγ = p[83];
AttractPowerSliderζε = p[84];
AttractPowerSliderζζ = p[85];
AttractPowerSliderζη = p[86];
AttractPowerSliderζθ = p[87];
AttractDistanceSliderζα = p[88];
AttractDistanceSliderζβ = p[89];
AttractDistanceSliderζδ = p[90];
AttractDistanceSliderζγ = p[91];
AttractDistanceSliderζε = p[92];
AttractDistanceSliderζζ = p[93];
AttractDistanceSliderζη = p[94];
AttractDistanceSliderζθ = p[95];
AttractPowerSliderηα = p[96];
AttractPowerSliderηβ = p[97];
AttractPowerSliderηδ = p[98];
AttractPowerSliderηγ = p[99];
AttractPowerSliderηε = p[100];
AttractPowerSliderηζ = p[101];
AttractPowerSliderηη = p[102];
AttractPowerSliderηθ = p[103];
AttractDistanceSliderηα = p[104];
AttractDistanceSliderηβ = p[105];
AttractDistanceSliderηδ = p[106];
AttractDistanceSliderηγ = p[107];
AttractDistanceSliderηε = p[108];
AttractDistanceSliderηζ = p[109];
AttractDistanceSliderηη = p[110];
AttractDistanceSliderηθ = p[111];
AttractPowerSliderθα = p[112];
AttractPowerSliderθβ = p[113];
AttractPowerSliderθδ = p[114];
AttractPowerSliderθγ = p[115];
AttractPowerSliderθε = p[116];
AttractPowerSliderθζ = p[117];
AttractPowerSliderθη = p[118];
AttractPowerSliderθθ = p[119];
AttractDistanceSliderθα = p[120];
AttractDistanceSliderθβ = p[121];
AttractDistanceSliderθδ = p[122];
AttractDistanceSliderθγ = p[123];
AttractDistanceSliderθε = p[124];
AttractDistanceSliderθζ = p[125];
AttractDistanceSliderηθ = p[126];
AttractDistanceSliderθθ = p[127];
RepelPowerSliderαα = p[128];
RepelPowerSliderαβ = p[129];
RepelPowerSliderαδ = p[130];
RepelPowerSliderαγ = p[131];
RepelPowerSliderαε = p[132];
RepelPowerSliderαζ = p[133];
RepelPowerSliderαη = p[134];
RepelPowerSliderαθ = p[135];
RepelDistanceSliderαα = p[136];
RepelDistanceSliderαβ = p[137];
RepelDistanceSliderαδ = p[138];
RepelDistanceSliderαγ = p[139];
RepelDistanceSliderαε = p[140];
RepelDistanceSliderαζ = p[141];
RepelDistanceSliderαη = p[142];
RepelDistanceSliderαθ = p[143];
RepelPowerSliderβα = p[144];
RepelPowerSliderββ = p[145];
RepelPowerSliderβδ = p[146];
RepelPowerSliderβγ = p[147];
RepelPowerSliderβε = p[148];
RepelPowerSliderβζ = p[149];
RepelPowerSliderβη = p[150];
RepelPowerSliderβθ = p[151];
RepelDistanceSliderβα = p[152];
RepelDistanceSliderββ = p[153];
RepelDistanceSliderβδ = p[154];
RepelDistanceSliderβγ = p[155];
RepelDistanceSliderβε = p[156];
RepelDistanceSliderβζ = p[157];
RepelDistanceSliderβη = p[158];
RepelDistanceSliderβθ = p[159];
RepelPowerSliderγα = p[160];
RepelPowerSliderγβ = p[161];
RepelPowerSliderγδ = p[162];
RepelPowerSliderγγ = p[163];
RepelPowerSliderγε = p[164];
RepelPowerSliderγζ = p[165];
RepelPowerSliderγη = p[166];
RepelPowerSliderγθ = p[167];
RepelDistanceSliderγα = p[168];
RepelDistanceSliderγβ = p[169];
RepelDistanceSliderγδ = p[170];
RepelDistanceSliderγγ = p[171];
RepelDistanceSliderγε = p[172];
RepelDistanceSliderγζ = p[173];
RepelDistanceSliderγη = p[174];
RepelDistanceSliderγθ = p[175];
RepelPowerSliderδα = p[176];
RepelPowerSliderδβ = p[177];
RepelPowerSliderδδ = p[178];
RepelPowerSliderδγ = p[179];
RepelPowerSliderδε = p[180];
RepelPowerSliderδζ = p[181];
RepelPowerSliderδη = p[182];
RepelPowerSliderδθ = p[183];
RepelDistanceSliderδα = p[184];
RepelDistanceSliderδβ = p[185];
RepelDistanceSliderδδ = p[186];
RepelDistanceSliderδγ = p[187];
RepelDistanceSliderδε = p[188];
RepelDistanceSliderδζ = p[189];
RepelDistanceSliderδη = p[190];
RepelDistanceSliderδθ = p[191];
RepelPowerSliderεα = p[192];
RepelPowerSliderεβ = p[193];
RepelPowerSliderεδ = p[194];
RepelPowerSliderεγ = p[195];
RepelPowerSliderεε = p[196];
RepelPowerSliderεζ = p[197];
RepelPowerSliderεη = p[198];
RepelPowerSliderεθ = p[199];
RepelDistanceSliderεα = p[200];
RepelDistanceSliderεβ = p[201];
RepelDistanceSliderεδ = p[202];
RepelDistanceSliderεγ = p[203];
RepelDistanceSliderεε = p[204];
RepelDistanceSliderεζ = p[205];
RepelDistanceSliderεη = p[206];
RepelDistanceSliderεθ = p[207];
RepelPowerSliderζα = p[208];
RepelPowerSliderζβ = p[209];
RepelPowerSliderζδ = p[210];
RepelPowerSliderζγ = p[211];
RepelPowerSliderζε = p[212];
RepelPowerSliderζζ = p[213];
RepelPowerSliderζη = p[214];
RepelPowerSliderζθ = p[215];
RepelDistanceSliderζα = p[216];
RepelDistanceSliderζβ = p[217];
RepelDistanceSliderζδ = p[218];
RepelDistanceSliderζγ = p[219];
RepelDistanceSliderζε = p[220];
RepelDistanceSliderζζ = p[221];
RepelDistanceSliderζη = p[222];
RepelDistanceSliderζθ = p[223];
RepelPowerSliderηα = p[224];
RepelPowerSliderηβ = p[225];
RepelPowerSliderηδ = p[226];
RepelPowerSliderηγ = p[227];
RepelPowerSliderηε = p[228];
RepelPowerSliderηζ = p[229];
RepelPowerSliderηη = p[230];
RepelPowerSliderηθ = p[231];
RepelDistanceSliderηα = p[232];
RepelDistanceSliderηβ = p[233];
RepelDistanceSliderηδ = p[234];
RepelDistanceSliderηγ = p[235];
RepelDistanceSliderηε = p[236];
RepelDistanceSliderηζ = p[237];
RepelDistanceSliderηη = p[238];
RepelDistanceSliderηθ = p[239];
RepelPowerSliderθα = p[240];
RepelPowerSliderθβ = p[241];
RepelPowerSliderθδ = p[242];
RepelPowerSliderθγ = p[243];
RepelPowerSliderθε = p[244];
RepelPowerSliderθζ = p[245];
RepelPowerSliderθη = p[246];
RepelPowerSliderθθ = p[247];
RepelDistanceSliderθα = p[248];
RepelDistanceSliderθβ = p[249];
RepelDistanceSliderθδ = p[250];
RepelDistanceSliderθγ = p[251];
RepelDistanceSliderθε = p[252];
RepelDistanceSliderθζ = p[253];
RepelDistanceSliderηθ = p[254];
RepelDistanceSliderθθ = p[255];
numberSliderβ = static_cast<int>(p[256]);
numberSliderα = static_cast<int>(p[257]);
numberSliderδ = static_cast<int>(p[258]);
numberSliderγ = static_cast<int>(p[259]);
numberSliderε = static_cast<int>(p[260]);
numberSliderζ = static_cast<int>(p[261]);
numberSliderη = static_cast<int>(p[262]);
numberSliderθ = static_cast<int>(p[263]);
viscositySlider = p[264];
ViscosityAttractSliderαα = p[265];
ViscosityAttractSliderαβ = p[266];
ViscosityAttractSliderαγ = p[267];
ViscosityAttractSliderαδ = p[268];
ViscosityAttractSliderαε = p[269];
ViscosityAttractSliderαζ = p[270];
ViscosityAttractSliderαη = p[271];
ViscosityAttractSliderβα = p[272];
ViscosityAttractSliderββ = p[273];
ViscosityAttractSliderβγ = p[274];
ViscosityAttractSliderβδ = p[275];
ViscosityAttractSliderβε = p[276];
ViscosityAttractSliderβζ = p[277];
ViscosityAttractSliderβη = p[278];
ViscosityAttractSliderγα = p[279];
ViscosityAttractSliderγβ = p[280];
ViscosityAttractSliderγγ = p[281];
ViscosityAttractSliderγδ = p[282];
ViscosityAttractSliderγε = p[283];
ViscosityAttractSliderγζ = p[284];
ViscosityAttractSliderγη = p[285];
ViscosityAttractSliderδα = p[286];
ViscosityAttractSliderδβ = p[287];
ViscosityAttractSliderδγ = p[288];
ViscosityAttractSliderδδ = p[289];
ViscosityAttractSliderδε = p[290];
ViscosityAttractSliderδζ = p[291];
ViscosityAttractSliderδη = p[292];
ViscosityAttractSliderεα = p[293];
ViscosityAttractSliderεβ = p[294];
ViscosityAttractSliderεγ = p[295];
ViscosityAttractSliderεδ = p[296];
ViscosityAttractSliderεε = p[297];
ViscosityAttractSliderεζ = p[298];
ViscosityAttractSliderεη = p[299];
ViscosityAttractSliderζα = p[300];
ViscosityAttractSliderζβ = p[301];
ViscosityAttractSliderζγ = p[302];
ViscosityAttractSliderζδ = p[303];
ViscosityAttractSliderζε = p[304];
ViscosityAttractSliderζζ = p[305];
ViscosityAttractSliderζη = p[306];
ViscosityAttractSliderηα = p[307];
ViscosityAttractSliderηβ = p[308];
ViscosityAttractSliderηγ = p[309];
ViscosityAttractSliderηδ = p[310];
ViscosityAttractSliderηε = p[311];
ViscosityAttractSliderηζ = p[312];
ViscosityAttractSliderηη = p[313];
ViscosityAttractSliderαθ = p[314];
ViscosityAttractSliderβθ = p[315];
ViscosityAttractSliderγθ = p[316];
ViscosityAttractSliderδθ = p[317];
ViscosityAttractSliderεθ = p[318];
ViscosityAttractSliderζθ = p[319];
ViscosityAttractSliderηθ = p[320];
ViscosityAttractSliderθα = p[321];
ViscosityAttractSliderθβ = p[322];
ViscosityAttractSliderθγ = p[323];
ViscosityAttractSliderθδ = p[324];
ViscosityAttractSliderθε = p[325];
ViscosityAttractSliderθζ = p[326];
ViscosityAttractSliderθη = p[327];
ViscosityAttractSliderθθ = p[328];
ViscosityRepelSliderαα = p[329];
ViscosityRepelSliderαβ = p[330];
ViscosityRepelSliderαγ = p[331];
ViscosityRepelSliderαδ = p[332];
ViscosityRepelSliderαε = p[333];
ViscosityRepelSliderαζ = p[334];
ViscosityRepelSliderαη = p[335];
ViscosityRepelSliderβα = p[336];
ViscosityRepelSliderββ = p[337];
ViscosityRepelSliderβγ = p[338];
ViscosityRepelSliderβδ = p[339];
ViscosityRepelSliderβε = p[340];
ViscosityRepelSliderβζ = p[341];
ViscosityRepelSliderβη = p[342];
ViscosityRepelSliderγα = p[343];
ViscosityRepelSliderγβ = p[344];
ViscosityRepelSliderγγ = p[345];
ViscosityRepelSliderγδ = p[346];
ViscosityRepelSliderγε = p[347];
ViscosityRepelSliderγζ = p[348];
ViscosityRepelSliderγη = p[349];
ViscosityRepelSliderδα = p[350];
ViscosityRepelSliderδβ = p[351];
ViscosityRepelSliderδγ = p[352];
ViscosityRepelSliderδδ = p[353];
ViscosityRepelSliderδε = p[354];
ViscosityRepelSliderδζ = p[355];
ViscosityRepelSliderδη = p[356];
ViscosityRepelSliderεα = p[357];
ViscosityRepelSliderεβ = p[358];
ViscosityRepelSliderεγ = p[359];
ViscosityRepelSliderεδ = p[360];
ViscosityRepelSliderεε = p[361];
ViscosityRepelSliderεζ = p[362];
ViscosityRepelSliderεη = p[363];
ViscosityRepelSliderζα = p[364];
ViscosityRepelSliderζβ = p[365];
ViscosityRepelSliderζγ = p[366];
ViscosityRepelSliderζδ = p[367];
ViscosityRepelSliderζε = p[368];
ViscosityRepelSliderζζ = p[369];
ViscosityRepelSliderζη = p[370];
ViscosityRepelSliderηα = p[371];
ViscosityRepelSliderηβ = p[372];
ViscosityRepelSliderηγ = p[373];
ViscosityRepelSliderηδ = p[374];
ViscosityRepelSliderηε = p[375];
ViscosityRepelSliderηζ = p[376];
ViscosityRepelSliderηη = p[377];
ViscosityRepelSliderαθ = p[378];
ViscosityRepelSliderβθ = p[379];
ViscosityRepelSliderγθ = p[380];
ViscosityRepelSliderδθ = p[381];
ViscosityRepelSliderεθ = p[382];
ViscosityRepelSliderζθ = p[383];
ViscosityRepelSliderηθ = p[384];
ViscosityRepelSliderθα = p[385];
ViscosityRepelSliderθβ = p[386];
ViscosityRepelSliderθγ = p[387];
ViscosityRepelSliderθδ = p[388];
ViscosityRepelSliderθε = p[389];
ViscosityRepelSliderθζ = p[390];
ViscosityRepelSliderθη = p[391];
ViscosityRepelSliderθθ = p[392];
probabilitySlider = p[393];
ProbAttractSliderαα = p[394];
ProbAttractSliderαβ = p[395];
ProbAttractSliderαγ = p[396];
ProbAttractSliderαδ = p[397];
ProbAttractSliderαε = p[398];
ProbAttractSliderαζ = p[399];
ProbAttractSliderαη = p[400];
ProbAttractSliderβα = p[401];
ProbAttractSliderββ = p[402];
ProbAttractSliderβγ = p[403];
ProbAttractSliderβδ = p[404];
ProbAttractSliderβε = p[405];
ProbAttractSliderβζ = p[406];
ProbAttractSliderβη = p[407];
ProbAttractSliderγα = p[408];
ProbAttractSliderγβ = p[409];
ProbAttractSliderγγ = p[410];
ProbAttractSliderγδ = p[411];
ProbAttractSliderγε = p[412];
ProbAttractSliderγζ = p[413];
ProbAttractSliderγη = p[414];
ProbAttractSliderδα = p[415];
ProbAttractSliderδβ = p[416];
ProbAttractSliderδγ = p[417];
ProbAttractSliderδδ = p[418];
ProbAttractSliderδε = p[419];
ProbAttractSliderδζ = p[420];
ProbAttractSliderδη = p[421];
ProbAttractSliderεα = p[422];
ProbAttractSliderεβ = p[423];
ProbAttractSliderεγ = p[424];
ProbAttractSliderεδ = p[425];
ProbAttractSliderεε = p[426];
ProbAttractSliderεζ = p[427];
ProbAttractSliderεη = p[428];
ProbAttractSliderζα = p[429];
ProbAttractSliderζβ = p[430];
ProbAttractSliderζγ = p[431];
ProbAttractSliderζδ = p[432];
ProbAttractSliderζε = p[433];
ProbAttractSliderζζ = p[434];
ProbAttractSliderζη = p[435];
ProbAttractSliderηα = p[436];
ProbAttractSliderηβ = p[437];
ProbAttractSliderηγ = p[438];
ProbAttractSliderηδ = p[439];
ProbAttractSliderηε = p[440];
ProbAttractSliderηζ = p[441];
ProbAttractSliderηη = p[442];
ProbAttractSliderαθ = p[443];
ProbAttractSliderβθ = p[444];
ProbAttractSliderγθ = p[445];
ProbAttractSliderδθ = p[446];
ProbAttractSliderεθ = p[447];
ProbAttractSliderζθ = p[448];
ProbAttractSliderηθ = p[449];
ProbAttractSliderθα = p[450];
ProbAttractSliderθβ = p[451];
ProbAttractSliderθγ = p[452];
ProbAttractSliderθδ = p[453];
ProbAttractSliderθε = p[454];
ProbAttractSliderθζ = p[455];
ProbAttractSliderθη = p[456];
ProbAttractSliderθθ = p[457];
ProbRepelSliderαα = p[458];
ProbRepelSliderαβ = p[459];
ProbRepelSliderαγ = p[460];
ProbRepelSliderαδ = p[461];
ProbRepelSliderαε = p[462];
ProbRepelSliderαζ = p[463];
ProbRepelSliderαη = p[464];
ProbRepelSliderβα = p[465];
ProbRepelSliderββ = p[466];
ProbRepelSliderβγ = p[467];
ProbRepelSliderβδ = p[468];
ProbRepelSliderβε = p[469];
ProbRepelSliderβζ = p[470];
ProbRepelSliderβη = p[471];
ProbRepelSliderγα = p[472];
ProbRepelSliderγβ = p[473];
ProbRepelSliderγγ = p[474];
ProbRepelSliderγδ = p[475];
ProbRepelSliderγε = p[476];
ProbRepelSliderγζ = p[477];
ProbRepelSliderγη = p[478];
ProbRepelSliderδα = p[479];
ProbRepelSliderδβ = p[480];
ProbRepelSliderδγ = p[481];
ProbRepelSliderδδ = p[482];
ProbRepelSliderδε = p[483];
ProbRepelSliderδζ = p[484];
ProbRepelSliderδη = p[485];
ProbRepelSliderεα = p[486];
ProbRepelSliderεβ = p[487];
ProbRepelSliderεγ = p[488];
ProbRepelSliderεδ = p[489];
ProbRepelSliderεε = p[490];
ProbRepelSliderεζ = p[491];
ProbRepelSliderεη = p[492];
ProbRepelSliderζα = p[493];
ProbRepelSliderζβ = p[494];
ProbRepelSliderζγ = p[495];
ProbRepelSliderζδ = p[496];
ProbRepelSliderζε = p[497];
ProbRepelSliderζζ = p[498];
ProbRepelSliderζη = p[499];
ProbRepelSliderηα = p[500];
ProbRepelSliderηβ = p[501];
ProbRepelSliderηγ = p[502];
ProbRepelSliderηδ = p[503];
ProbRepelSliderηε = p[504];
ProbRepelSliderηζ = p[505];
ProbRepelSliderηη = p[506];
ProbRepelSliderαθ = p[507];
ProbRepelSliderβθ = p[508];
ProbRepelSliderγθ = p[509];
ProbRepelSliderδθ = p[510];
ProbRepelSliderεθ = p[511];
ProbRepelSliderζθ = p[512];
ProbRepelSliderηθ = p[513];
ProbRepelSliderθα = p[514];
ProbRepelSliderθβ = p[515];
ProbRepelSliderθγ = p[516];
ProbRepelSliderθδ = p[517];
ProbRepelSliderθε = p[518];
ProbRepelSliderθζ = p[519];
ProbRepelSliderθη = p[520];
ProbRepelSliderθθ = p[521];
minAttractPowerSlider = p[522];
maxAttractPowerSlider = p[523];
minRepelPowerSlider = p[524];
maxRepelPowerSlider = p[525];
minAttractRangeSlider = p[526];
maxAttractRangeSlider = p[527];
minRepelRangeSlider = p[528];
maxRepelRangeSlider = p[529];
minAttractViscoSlider = p[530];
maxAttractViscoSlider = p[531];
minRepelViscoSlider = p[532];
maxRepelViscoSlider = p[533];
minAttractProbSlider = p[534];
maxAttractProbSlider = p[535];
minRepelProbSlider = p[536];
maxRepelProbSlider = p[537];
AttractEvoProbSlider = p[538];
AttractEvoAmountSlider = p[539];
ProbAttractEvoProbSlider = p[540];
ProbAttractEvoAmountSlider = p[541];
ViscoAttractEvoProbSlider = p[542];
ViscoAttractEvoAmountSlider = p[543];
RepelEvoProbSlider = p[544];
RepelEvoAmountSlider = p[545];
ProbRepelEvoProbSlider = p[546];
ProbRepelEvoAmountSlider = p[547];
ViscoRepelEvoProbSlider = p[548];
ViscoRepelEvoAmountSlider = p[549];
}
	restart();
}


//------------------------------GUI initialization------------------------------
void ofApp::setup()
{
	lastTime = clock();
	ofSetWindowTitle("Particle Life - 8c128v128p version 1.8.0");
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
	rndGroup.add(randomRelations.setup("Randomize all interactions (a)"));
	rndGroup.add(randomCount.setup("Randomize particle count (q)"));
	rndGroup.add(randomVsc.setup("Randomize Viscosities (v)"));
	rndGroup.add(randomProb.setup("Randomize Probabilities (p)"));
	rndGroup.add(randomChoice.setup("Randomize IP (i)"));
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
	qtyGroup.add(numberSliderθ.setup("Teta", pnumberSliderθ, 0, 10000));
	gui.add(&qtyGroup);
	qtyGroup.minimize();

	// Evolve Group
	evolveGroup.setup("Evolution of Parameters");
	evolveGroup.add(evoToggle.setup("Evolve parameters", false));
	evolveGroup.add(AttractEvoProbSlider.setup("Attract power evo chance%", AttractEvoChance, 0, 100));
	evolveGroup.add(AttractEvoAmountSlider.setup("Attract power evo amount%%", AttractEvoAmount, 0, 100));
	evolveGroup.add(ProbAttractEvoProbSlider.setup("Attract prob evo chance%", ProbAttractEvoChance, 0, 100));
	evolveGroup.add(ProbAttractEvoAmountSlider.setup("Attract prob evo amount%%", ProbAttractEvoAmount, 0, 100));
	evolveGroup.add(ViscoAttractEvoProbSlider.setup("Attract visco evo chance%", ViscoAttractEvoChance, 0, 100));
	evolveGroup.add(ViscoAttractEvoAmountSlider.setup("Attract visco evo amount%%", ViscoAttractEvoAmount, 0, 100));
	evolveGroup.add(RepelEvoProbSlider.setup("Repel power evo chance%", RepelEvoChance, 0, 100));
	evolveGroup.add(RepelEvoAmountSlider.setup("Repel power evo amount%%", RepelEvoAmount, 0, 100));
	evolveGroup.add(ProbRepelEvoProbSlider.setup("Repel prob evo chance%", ProbRepelEvoChance, 0, 100));
	evolveGroup.add(ProbRepelEvoAmountSlider.setup("Repel prob evo amount%%", ProbRepelEvoAmount, 0, 100));
	evolveGroup.add(ViscoRepelEvoProbSlider.setup("Repel visco evo chance%", ViscoRepelEvoChance, 0, 100));
	evolveGroup.add(ViscoRepelEvoAmountSlider.setup("Repel visco evo amount%%", ViscoRepelEvoAmount, 0, 100));
	evolveGroup.add(probabilitySlider.setup("interaction prob%", probability, 1, 100));
	evolveGroup.add(viscositySlider.setup("interaction viscosity", viscosity, 0, 1));
	gui.add(&evolveGroup);
	evolveGroup.minimize();

	// MinMax Group
	MinMaxGroup.setup("MinMax");
	MinMaxGroup.add(minAttractPowerSlider.setup("Attract minimum power", minAttP, 0, 50));
	MinMaxGroup.add(maxAttractPowerSlider.setup("Attract maximum power", maxAttP, 0, 50));
	MinMaxGroup.add(minAttractRangeSlider.setup("Attract minimum range", minAttR, 0, 100));
	MinMaxGroup.add(maxAttractRangeSlider.setup("Attract maximum range", maxAttR, 0, 100));
	MinMaxGroup.add(minAttractViscoSlider.setup("Attract minimum viscosity", minAttV, 0.0, 1.0));
	MinMaxGroup.add(maxAttractViscoSlider.setup("Attract maximum viscosity", maxAttV, 0.0, 1.0));
	MinMaxGroup.add(minAttractProbSlider.setup("Attract minimum probability", minAttI, 0, 100));
	MinMaxGroup.add(maxAttractProbSlider.setup("Attract maximum probability", maxAttI, 0, 100));
	MinMaxGroup.add(minRepelPowerSlider.setup("Repel minimum power", minRepP, -50, 0));
	MinMaxGroup.add(maxRepelPowerSlider.setup("Repel maximum power", maxRepP, -50, 0));
	MinMaxGroup.add(minRepelRangeSlider.setup("Repel minimum range", minRepR, 0, 100));
	MinMaxGroup.add(maxRepelRangeSlider.setup("Repel maximum range", maxRepR, 0, 100));
	MinMaxGroup.add(minRepelViscoSlider.setup("Repel minimum viscosity", minRepV, 0.0, 1.0));
	MinMaxGroup.add(maxRepelViscoSlider.setup("Repel maximum viscosity", maxRepV, 0.0, 1.0));
	MinMaxGroup.add(minRepelProbSlider.setup("Repel minimum probability", minRepI, 0, 100));
	MinMaxGroup.add(maxRepelProbSlider.setup("Repel maximum probability", maxRepI, 0, 100));
	gui.add(&MinMaxGroup);
	MinMaxGroup.minimize();

	// Alpha
	alphaGroup.setup("Alpha");
	alphaGroup.add(AttractPowerSliderαα.setup("Attract power alpha x alpha:", pAttractPowerSliderαα, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαβ.setup("Attract power alpha x betha:", pAttractPowerSliderαβ, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαγ.setup("Attract power alpha x gamma:", pAttractPowerSliderαγ, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαδ.setup("Attract power alpha x delta:", pAttractPowerSliderαδ, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαε.setup("Attract power alpha x epsilon:", pAttractPowerSliderαε, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαζ.setup("Attract power alpha x zeta:", pAttractPowerSliderαζ, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαη.setup("Attract power alpha x eta:", pAttractPowerSliderαη, minAttP, maxAttP));
	alphaGroup.add(AttractPowerSliderαθ.setup("Attract power alpha x teta:", pAttractPowerSliderαθ, minAttP, maxAttP));
	alphaGroup.add(AttractDistanceSliderαα.setup("Attract radius alpha x alpha:", pAttractDistanceSliderαα, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαβ.setup("Attract radius alpha x betha:", pAttractDistanceSliderαβ, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαγ.setup("Attract radius alpha x gamma:", pAttractDistanceSliderαγ, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαδ.setup("Attract radius alpha x delta:", pAttractDistanceSliderαδ, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαε.setup("Attract radius alpha x epsilon:", pAttractDistanceSliderαε, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαζ.setup("Attract radius alpha x zeta:", pAttractDistanceSliderαζ, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαη.setup("Attract radius alpha x eta:", pAttractDistanceSliderαη, minAttR, maxAttR));
	alphaGroup.add(AttractDistanceSliderαθ.setup("Attract radius alpha x teta:", pAttractDistanceSliderαθ, minAttR, maxAttR));
	alphaGroup.add(ViscosityAttractSliderαβ.setup("Attract viscosity alpha x betha", ViscosityAttractαβ, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαα.setup("Attract viscosity alpha x alpha", ViscosityAttractαα, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαγ.setup("Attract viscosity alpha x gamma", ViscosityAttractαγ, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαδ.setup("Attract viscosity alpha x delta", ViscosityAttractαδ, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαε.setup("Attract viscosity alpha x epsilon", ViscosityAttractαε, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαζ.setup("Attract viscosity alpha x zeta", ViscosityAttractαζ, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαη.setup("Attract viscosity alpha x eta", ViscosityAttractαη, minAttV, maxAttV));
	alphaGroup.add(ViscosityAttractSliderαθ.setup("Attract viscosity alpha x teta", ViscosityAttractαθ, minAttV, maxAttV));
	alphaGroup.add(ProbAttractSliderαβ.setup("Attract prob alpha x betha", ProbAttractαβ, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαα.setup("Attract prob alpha x alpha", ProbAttractαα, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαγ.setup("Attract prob alpha x gamma", ProbAttractαγ, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαδ.setup("Attract prob alpha x delta", ProbAttractαδ, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαε.setup("Attract prob alpha x epsilon", ProbAttractαε, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαζ.setup("Attract prob alpha x zeta", ProbAttractαζ, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαη.setup("Attract prob alpha x eta", ProbAttractαη, minAttI, maxAttI));
	alphaGroup.add(ProbAttractSliderαθ.setup("Attract prob alpha x teta", ProbAttractαθ, minAttI, maxAttI));
	alphaGroup.add(RepelPowerSliderαα.setup("Repel power alpha x alpha:", pRepelPowerSliderαα, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαβ.setup("Repel power alpha x betha:", pRepelPowerSliderαβ, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαγ.setup("Repel power alpha x gamma:", pRepelPowerSliderαγ, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαδ.setup("Repel power alpha x delta:", pRepelPowerSliderαδ, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαε.setup("Repel power alpha x epsilon:", pRepelPowerSliderαε, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαζ.setup("Repel power alpha x zeta:", pRepelPowerSliderαζ, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαη.setup("Repel power alpha x eta:", pRepelPowerSliderαη, minRepP, maxRepP));
	alphaGroup.add(RepelPowerSliderαθ.setup("Repel power alpha x teta:", pRepelPowerSliderαθ, minRepP, maxRepP));
	alphaGroup.add(RepelDistanceSliderαα.setup("Repel radius alpha x alpha:", pRepelDistanceSliderαα, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαβ.setup("Repel radius alpha x betha:", pRepelDistanceSliderαβ, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαγ.setup("Repel radius alpha x gamma:", pRepelDistanceSliderαγ, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαδ.setup("Repel radius alpha x delta:", pRepelDistanceSliderαδ, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαε.setup("Repel radius alpha x epsilon:", pRepelDistanceSliderαε, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαζ.setup("Repel radius alpha x zeta:", pRepelDistanceSliderαζ, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαη.setup("Repel radius alpha x eta:", pRepelDistanceSliderαη, minRepR, maxRepR));
	alphaGroup.add(RepelDistanceSliderαθ.setup("Repel radius alpha x teta:", pRepelDistanceSliderαθ, minRepR, maxRepR));
	alphaGroup.add(ViscosityRepelSliderαβ.setup("Repel viscosity alpha x betha", ViscosityRepelαβ, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαα.setup("Repel viscosity alpha x alpha", ViscosityRepelαα, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαγ.setup("Repel viscosity alpha x gamma", ViscosityRepelαγ, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαδ.setup("Repel viscosity alpha x delta", ViscosityRepelαδ, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαε.setup("Repel viscosity alpha x epsilon", ViscosityRepelαε, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαζ.setup("Repel viscosity alpha x zeta", ViscosityRepelαζ, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαη.setup("Repel viscosity alpha x eta", ViscosityRepelαη, minRepV, maxRepV));
	alphaGroup.add(ViscosityRepelSliderαθ.setup("Repel viscosity alpha x teta", ViscosityRepelαθ, minRepV, maxRepV));
	alphaGroup.add(ProbRepelSliderαβ.setup("Repel prob alpha x betha", ProbRepelαβ, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαα.setup("Repel prob alpha x alpha", ProbRepelαα, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαγ.setup("Repel prob alpha x gamma", ProbRepelαγ, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαδ.setup("Repel prob alpha x delta", ProbRepelαδ, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαε.setup("Repel prob alpha x epsilon", ProbRepelαε, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαζ.setup("Repel prob alpha x zeta", ProbRepelαζ, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαη.setup("Repel prob alpha x eta", ProbRepelαη, minRepI, maxRepI));
	alphaGroup.add(ProbRepelSliderαθ.setup("Repel prob alpha x teta", ProbRepelαθ, minRepI, maxRepI));
	alphaGroup.minimize();
	gui.add(&alphaGroup);

	// Betha
	bethaGroup.setup("Betha");
	bethaGroup.add(AttractPowerSliderβα.setup("Attract power betha x alpha:", pAttractPowerSliderβα, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderββ.setup("Attract power betha x betha:", pAttractPowerSliderββ, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβγ.setup("Attract power betha x gamma:", pAttractPowerSliderβγ, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβδ.setup("Attract power betha x delta:", pAttractPowerSliderβδ, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβε.setup("Attract power betha x epsilon:", pAttractPowerSliderβε, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβζ.setup("Attract power betha x zeta:", pAttractPowerSliderβζ, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβη.setup("Attract power betha x eta:", pAttractPowerSliderβη, minAttP, maxAttP));
	bethaGroup.add(AttractPowerSliderβθ.setup("Attract power betha x teta:", pAttractPowerSliderβθ, minAttP, maxAttP));
	bethaGroup.add(AttractDistanceSliderβα.setup("Attract radius betha x alpha:", pAttractDistanceSliderβα, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderββ.setup("Attract radius betha x betha:", pAttractDistanceSliderββ, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβγ.setup("Attract radius betha x gamma:", pAttractDistanceSliderβγ, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβδ.setup("Attract radius betha x delta:", pAttractDistanceSliderβδ, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβε.setup("Attract radius betha x epsilon:", pAttractDistanceSliderβε, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβζ.setup("Attract radius betha x zeta:", pAttractDistanceSliderβζ, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβη.setup("Attract radius betha x eta:", pAttractDistanceSliderβη, minAttR, maxAttR));
	bethaGroup.add(AttractDistanceSliderβθ.setup("Attract radius betha x teta:", pAttractDistanceSliderβθ, minAttR, maxAttR));
	bethaGroup.add(ViscosityAttractSliderββ.setup("Attract viscosity betha x betha", ViscosityAttractββ, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβα.setup("Attract viscosity betha x alpha", ViscosityAttractβα, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβγ.setup("Attract viscosity betha x gamma", ViscosityAttractβγ, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβδ.setup("Attract viscosity betha x delta", ViscosityAttractβδ, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβε.setup("Attract viscosity betha x epsilon", ViscosityAttractβε, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβζ.setup("Attract viscosity betha x zeta", ViscosityAttractβζ, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβη.setup("Attract viscosity betha x eta", ViscosityAttractβη, minAttV, maxAttV));
	bethaGroup.add(ViscosityAttractSliderβθ.setup("Attract viscosity betha x teta", ViscosityAttractβθ, minAttV, maxAttV));
	bethaGroup.add(ProbAttractSliderββ.setup("Attract prob betha x betha", ProbAttractββ, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβα.setup("Attract prob betha x alpha", ProbAttractβα, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβγ.setup("Attract prob betha x gamma", ProbAttractβγ, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβδ.setup("Attract prob betha x delta", ProbAttractβδ, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβε.setup("Attract prob betha x epsilon", ProbAttractβε, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβζ.setup("Attract prob betha x zeta", ProbAttractβζ, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβη.setup("Attract prob betha x eta", ProbAttractβη, minAttI, maxAttI));
	bethaGroup.add(ProbAttractSliderβθ.setup("Attract prob betha x teta", ProbAttractβθ, minAttI, maxAttI));
	bethaGroup.add(RepelPowerSliderβα.setup("Repel power betha x alpha:", pRepelPowerSliderβα, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderββ.setup("Repel power betha x betha:", pRepelPowerSliderββ, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβγ.setup("Repel power betha x gamma:", pRepelPowerSliderβγ, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβδ.setup("Repel power betha x delta:", pRepelPowerSliderβδ, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβε.setup("Repel power betha x epsilon:", pRepelPowerSliderβε, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβζ.setup("Repel power betha x zeta:", pRepelPowerSliderβζ, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβη.setup("Repel power betha x eta:", pRepelPowerSliderβη, minRepP, maxRepP));
	bethaGroup.add(RepelPowerSliderβθ.setup("Repel power betha x teta:", pRepelPowerSliderβθ, minRepP, maxRepP));
	bethaGroup.add(RepelDistanceSliderβα.setup("Repel radius betha x alpha:", pRepelDistanceSliderβα, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderββ.setup("Repel radius betha x betha:", pRepelDistanceSliderββ, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβγ.setup("Repel radius betha x gamma:", pRepelDistanceSliderβγ, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβδ.setup("Repel radius betha x delta:", pRepelDistanceSliderβδ, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβε.setup("Repel radius betha x epsilon:", pRepelDistanceSliderβε, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβζ.setup("Repel radius betha x zeta:", pRepelDistanceSliderβζ, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβη.setup("Repel radius betha x eta:", pRepelDistanceSliderβη, minRepR, maxRepR));
	bethaGroup.add(RepelDistanceSliderβθ.setup("Repel radius betha x teta:", pRepelDistanceSliderβθ, minRepR, maxRepR));
	bethaGroup.add(ViscosityRepelSliderββ.setup("Repel viscosity betha x betha", ViscosityRepelββ, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβα.setup("Repel viscosity betha x alpha", ViscosityRepelβα, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβγ.setup("Repel viscosity betha x gamma", ViscosityRepelβγ, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβδ.setup("Repel viscosity betha x delta", ViscosityRepelβδ, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβε.setup("Repel viscosity betha x epsilon", ViscosityRepelβε, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβζ.setup("Repel viscosity betha x zeta", ViscosityRepelβζ, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβη.setup("Repel viscosity betha x eta", ViscosityRepelβη, minRepV, maxRepV));
	bethaGroup.add(ViscosityRepelSliderβθ.setup("Repel viscosity betha x teta", ViscosityRepelβθ, minRepV, maxRepV));
	bethaGroup.add(ProbRepelSliderββ.setup("Repel prob betha x betha", ProbRepelββ, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβα.setup("Repel prob betha x alpha", ProbRepelβα, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβγ.setup("Repel prob betha x gamma", ProbRepelβγ, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβδ.setup("Repel prob betha x delta", ProbRepelβδ, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβε.setup("Repel prob betha x epsilon", ProbRepelβε, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβζ.setup("Repel prob betha x zeta", ProbRepelβζ, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβη.setup("Repel prob betha x eta", ProbRepelβη, minRepI, maxRepI));
	bethaGroup.add(ProbRepelSliderβθ.setup("Repel prob betha x teta", ProbRepelβθ, minRepI, maxRepI));
	bethaGroup.minimize();
	gui.add(&bethaGroup);

	// Gamma
	gammaGroup.setup("Gamma");
	gammaGroup.add(AttractPowerSliderγα.setup("Attract power gamma x alpha:", pAttractPowerSliderγα, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγβ.setup("Attract power gamma x betha:", pAttractPowerSliderγβ, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγγ.setup("Attract power gamma x gamma:", pAttractPowerSliderγγ, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγδ.setup("Attract power gamma x delta:", pAttractPowerSliderγδ, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγε.setup("Attract power gamma x epsilon:", pAttractPowerSliderγε, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγζ.setup("Attract power gamma x zeta:", pAttractPowerSliderγζ, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγη.setup("Attract power gamma x eta:", pAttractPowerSliderγη, minAttP, maxAttP));
	gammaGroup.add(AttractPowerSliderγθ.setup("Attract power gamma x teta:", pAttractPowerSliderγθ, minAttP, maxAttP));
	gammaGroup.add(AttractDistanceSliderγα.setup("Attract radius gamma x alpha:", pAttractDistanceSliderγα, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγβ.setup("Attract radius gamma x betha:", pAttractDistanceSliderγβ, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγγ.setup("Attract radius gamma x gamma:", pAttractDistanceSliderγγ, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγδ.setup("Attract radius gamma x delta:", pAttractDistanceSliderγδ, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγε.setup("Attract radius gamma x epsilon:", pAttractDistanceSliderγε, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγζ.setup("Attract radius gamma x zeta:", pAttractDistanceSliderγζ, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγη.setup("Attract radius gamma x eta:", pAttractDistanceSliderγη, minAttR, maxAttR));
	gammaGroup.add(AttractDistanceSliderγθ.setup("Attract radius gamma x teta:", pAttractDistanceSliderγθ, minAttR, maxAttR));
	gammaGroup.add(ViscosityAttractSliderγβ.setup("Attract viscosity gamma x betha", ViscosityAttractγβ, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγα.setup("Attract viscosity gamma x alpha", ViscosityAttractγα, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγγ.setup("Attract viscosity gamma x gamma", ViscosityAttractγγ, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγδ.setup("Attract viscosity gamma x delta", ViscosityAttractγδ, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγε.setup("Attract viscosity gamma x epsilon", ViscosityAttractγε, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγζ.setup("Attract viscosity gamma x zeta", ViscosityAttractγζ, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγη.setup("Attract viscosity gamma x eta", ViscosityAttractγη, minAttV, maxAttV));
	gammaGroup.add(ViscosityAttractSliderγθ.setup("Attract viscosity gamma x teta", ViscosityAttractγθ, minAttV, maxAttV));
	gammaGroup.add(ProbAttractSliderγβ.setup("Attract prob gamma x betha", ProbAttractγβ, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγα.setup("Attract prob gamma x alpha", ProbAttractγα, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγγ.setup("Attract prob gamma x gamma", ProbAttractγγ, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγδ.setup("Attract prob gamma x delta", ProbAttractγδ, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγε.setup("Attract prob gamma x epsilon", ProbAttractγε, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγζ.setup("Attract prob gamma x zeta", ProbAttractγζ, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγη.setup("Attract prob gamma x eta", ProbAttractγη, minAttI, maxAttI));
	gammaGroup.add(ProbAttractSliderγθ.setup("Attract prob gamma x teta", ProbAttractγθ, minAttI, maxAttI));
	gammaGroup.add(RepelPowerSliderγα.setup("Repel power gamma x alpha:", pRepelPowerSliderγα, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγβ.setup("Repel power gamma x betha:", pRepelPowerSliderγβ, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγγ.setup("Repel power gamma x gamma:", pRepelPowerSliderγγ, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγδ.setup("Repel power gamma x delta:", pRepelPowerSliderγδ, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγε.setup("Repel power gamma x epsilon:", pRepelPowerSliderγε, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγζ.setup("Repel power gamma x zeta:", pRepelPowerSliderγζ, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγη.setup("Repel power gamma x eta:", pRepelPowerSliderγη, minRepP, maxRepP));
	gammaGroup.add(RepelPowerSliderγθ.setup("Repel power gamma x teta:", pRepelPowerSliderγθ, minRepP, maxRepP));
	gammaGroup.add(RepelDistanceSliderγα.setup("Repel radius gamma x alpha:", pRepelDistanceSliderγα, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγβ.setup("Repel radius gamma x betha:", pRepelDistanceSliderγβ, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγγ.setup("Repel radius gamma x gamma:", pRepelDistanceSliderγγ, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγδ.setup("Repel radius gamma x delta:", pRepelDistanceSliderγδ, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγε.setup("Repel radius gamma x epsilon:", pRepelDistanceSliderγε, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγζ.setup("Repel radius gamma x zeta:", pRepelDistanceSliderγζ, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγη.setup("Repel radius gamma x eta:", pRepelDistanceSliderγη, minRepR, maxRepR));
	gammaGroup.add(RepelDistanceSliderγθ.setup("Repel radius gamma x teta:", pRepelDistanceSliderγθ, minRepR, maxRepR));
	gammaGroup.add(ViscosityRepelSliderγβ.setup("Repel viscosity gamma x betha", ViscosityRepelγβ, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγα.setup("Repel viscosity gamma x alpha", ViscosityRepelγα, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγγ.setup("Repel viscosity gamma x gamma", ViscosityRepelγγ, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγδ.setup("Repel viscosity gamma x delta", ViscosityRepelγδ, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγε.setup("Repel viscosity gamma x epsilon", ViscosityRepelγε, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγζ.setup("Repel viscosity gamma x zeta", ViscosityRepelγζ, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγη.setup("Repel viscosity gamma x eta", ViscosityRepelγη, minRepV, maxRepV));
	gammaGroup.add(ViscosityRepelSliderγθ.setup("Repel viscosity gamma x teta", ViscosityRepelγθ, minRepV, maxRepV));
	gammaGroup.add(ProbRepelSliderγβ.setup("Repel prob gamma x betha", ProbRepelγβ, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγα.setup("Repel prob gamma x alpha", ProbRepelγα, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγγ.setup("Repel prob gamma x gamma", ProbRepelγγ, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγδ.setup("Repel prob gamma x delta", ProbRepelγδ, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγε.setup("Repel prob gamma x epsilon", ProbRepelγε, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγζ.setup("Repel prob gamma x zeta", ProbRepelγζ, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγη.setup("Repel prob gamma x eta", ProbRepelγη, minRepI, maxRepI));
	gammaGroup.add(ProbRepelSliderγθ.setup("Repel prob gamma x teta", ProbRepelγθ, minRepI, maxRepI));
	gammaGroup.minimize();
	gui.add(&gammaGroup);

	// Delta
	eltaGroup.setup("Delta");
	eltaGroup.add(AttractPowerSliderδα.setup("Attract power delta x alpha:", pAttractPowerSliderδα, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδβ.setup("Attract power delta x betha:", pAttractPowerSliderδβ, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδγ.setup("Attract power delta x gamma:", pAttractPowerSliderδγ, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδδ.setup("Attract power delta x delta:", pAttractPowerSliderδδ, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδε.setup("Attract power delta x epsilon:", pAttractPowerSliderδε, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδζ.setup("Attract power delta x zeta:", pAttractPowerSliderδζ, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδη.setup("Attract power delta x eta:", pAttractPowerSliderδη, minAttP, maxAttP));
	eltaGroup.add(AttractPowerSliderδθ.setup("Attract power delta x teta:", pAttractPowerSliderδθ, minAttP, maxAttP));
	eltaGroup.add(AttractDistanceSliderδα.setup("Attract radius delta x alpha:", pAttractDistanceSliderδα, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδβ.setup("Attract radius delta x betha:", pAttractDistanceSliderδβ, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδγ.setup("Attract radius delta x gamma:", pAttractDistanceSliderδγ, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδδ.setup("Attract radius delta x delta:", pAttractDistanceSliderδδ, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδε.setup("Attract radius delta x epsilon:", pAttractDistanceSliderδε, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδζ.setup("Attract radius delta x zeta:", pAttractDistanceSliderδζ, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδη.setup("Attract radius delta x eta:", pAttractDistanceSliderδη, minAttR, maxAttR));
	eltaGroup.add(AttractDistanceSliderδθ.setup("Attract radius delta x teta:", pAttractDistanceSliderδθ, minAttR, maxAttR));
	eltaGroup.add(ViscosityAttractSliderδβ.setup("Attract viscosity delta x betha", ViscosityAttractδβ, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδα.setup("Attract viscosity delta x alpha", ViscosityAttractδα, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδγ.setup("Attract viscosity delta x gamma", ViscosityAttractδγ, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδδ.setup("Attract viscosity delta x delta", ViscosityAttractδδ, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδε.setup("Attract viscosity delta x epsilon", ViscosityAttractδε, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδζ.setup("Attract viscosity delta x zeta", ViscosityAttractδζ, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδη.setup("Attract viscosity delta x eta", ViscosityAttractδη, minAttV, maxAttV));
	eltaGroup.add(ViscosityAttractSliderδθ.setup("Attract viscosity delta x teta", ViscosityAttractδθ, minAttV, maxAttV));
	eltaGroup.add(ProbAttractSliderδβ.setup("Attract prob delta x betha", ProbAttractδβ, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδα.setup("Attract prob delta x alpha", ProbAttractδα, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδγ.setup("Attract prob delta x gamma", ProbAttractδγ, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδδ.setup("Attract prob delta x delta", ProbAttractδδ, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδε.setup("Attract prob delta x epsilon", ProbAttractδε, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδζ.setup("Attract prob delta x zeta", ProbAttractδζ, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδη.setup("Attract prob delta x eta", ProbAttractδη, minAttI, maxAttI));
	eltaGroup.add(ProbAttractSliderδθ.setup("Attract prob delta x teta", ProbAttractδθ, minAttI, maxAttI));
	eltaGroup.add(RepelPowerSliderδα.setup("Repel power delta x alpha:", pRepelPowerSliderδα, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδβ.setup("Repel power delta x betha:", pRepelPowerSliderδβ, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδγ.setup("Repel power delta x gamma:", pRepelPowerSliderδγ, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδδ.setup("Repel power delta x delta:", pRepelPowerSliderδδ, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδε.setup("Repel power delta x epsilon:", pRepelPowerSliderδε, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδζ.setup("Repel power delta x zeta:", pRepelPowerSliderδζ, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδη.setup("Repel power delta x eta:", pRepelPowerSliderδη, minRepP, maxRepP));
	eltaGroup.add(RepelPowerSliderδθ.setup("Repel power delta x teta:", pRepelPowerSliderδθ, minRepP, maxRepP));
	eltaGroup.add(RepelDistanceSliderδα.setup("Repel radius delta x alpha:", pRepelDistanceSliderδα, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδβ.setup("Repel radius delta x betha:", pRepelDistanceSliderδβ, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδγ.setup("Repel radius delta x gamma:", pRepelDistanceSliderδγ, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδδ.setup("Repel radius delta x delta:", pRepelDistanceSliderδδ, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδε.setup("Repel radius delta x epsilon:", pRepelDistanceSliderδε, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδζ.setup("Repel radius delta x zeta:", pRepelDistanceSliderδζ, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδη.setup("Repel radius delta x eta:", pRepelDistanceSliderδη, minRepR, maxRepR));
	eltaGroup.add(RepelDistanceSliderδθ.setup("Repel radius delta x teta:", pRepelDistanceSliderδθ, minRepR, maxRepR));
	eltaGroup.add(ViscosityRepelSliderδβ.setup("Repel viscosity delta x betha", ViscosityRepelδβ, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδα.setup("Repel viscosity delta x alpha", ViscosityRepelδα, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδγ.setup("Repel viscosity delta x gamma", ViscosityRepelδγ, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδδ.setup("Repel viscosity delta x delta", ViscosityRepelδδ, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδε.setup("Repel viscosity delta x epsilon", ViscosityRepelδε, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδζ.setup("Repel viscosity delta x zeta", ViscosityRepelδζ, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδη.setup("Repel viscosity delta x eta", ViscosityRepelδη, minRepV, maxRepV));
	eltaGroup.add(ViscosityRepelSliderδθ.setup("Repel viscosity delta x teta", ViscosityRepelδθ, minRepV, maxRepV));
	eltaGroup.add(ProbRepelSliderδβ.setup("Repel prob delta x betha", ProbRepelδβ, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδα.setup("Repel prob delta x alpha", ProbRepelδα, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδγ.setup("Repel prob delta x gamma", ProbRepelδγ, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδδ.setup("Repel prob delta x delta", ProbRepelδδ, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδε.setup("Repel prob delta x epsilon", ProbRepelδε, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδζ.setup("Repel prob delta x zeta", ProbRepelδζ, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδη.setup("Repel prob delta x eta", ProbRepelδη, minRepI, maxRepI));
	eltaGroup.add(ProbRepelSliderδθ.setup("Repel prob delta x teta", ProbRepelδθ, minRepI, maxRepI));
	eltaGroup.minimize();
	gui.add(&eltaGroup);

	// Epsilon
	epsilonGroup.setup("Epsilon");
	epsilonGroup.add(AttractPowerSliderεα.setup("Attract power epsilon x alpha:", pAttractPowerSliderεα, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεβ.setup("Attract power epsilon x betha:", pAttractPowerSliderεβ, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεγ.setup("Attract power epsilon x gamma:", pAttractPowerSliderεγ, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεδ.setup("Attract power epsilon x delta:", pAttractPowerSliderεδ, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεε.setup("Attract power epsilon x epsilon:", pAttractPowerSliderεε, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεζ.setup("Attract power epsilon x zeta:", pAttractPowerSliderεζ, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεη.setup("Attract power epsilon x eta:", pAttractPowerSliderεη, minAttP, maxAttP));
	epsilonGroup.add(AttractPowerSliderεθ.setup("Attract power epsilon x teta:", pAttractPowerSliderεθ, minAttP, maxAttP));
	epsilonGroup.add(AttractDistanceSliderεα.setup("Attract radius epsilon x alpha:", pAttractDistanceSliderεα, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεβ.setup("Attract radius epsilon x betha:", pAttractDistanceSliderεβ, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεγ.setup("Attract radius epsilon x gamma:", pAttractDistanceSliderεγ, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεδ.setup("Attract radius epsilon x delta:", pAttractDistanceSliderεδ, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεε.setup("Attract radius epsilon x epsilon:", pAttractDistanceSliderεε, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεζ.setup("Attract radius epsilon x zeta:", pAttractDistanceSliderεζ, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεη.setup("Attract radius epsilon x eta:", pAttractDistanceSliderεη, minAttR, maxAttR));
	epsilonGroup.add(AttractDistanceSliderεθ.setup("Attract radius epsilon x teta:", pAttractDistanceSliderεθ, minAttR, maxAttR));
	epsilonGroup.add(ViscosityAttractSliderεβ.setup("Attract viscosity epsilon x betha", ViscosityAttractεβ, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεα.setup("Attract viscosity epsilon x alpha", ViscosityAttractεα, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεγ.setup("Attract viscosity epsilon x gamma", ViscosityAttractεγ, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεδ.setup("Attract viscosity epsilon x delta", ViscosityAttractεδ, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεε.setup("Attract viscosity epsilon x epsilon", ViscosityAttractεε, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεζ.setup("Attract viscosity epsilon x zeta", ViscosityAttractεζ, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεη.setup("Attract viscosity epsilon x eta", ViscosityAttractεη, minAttV, maxAttV));
	epsilonGroup.add(ViscosityAttractSliderεθ.setup("Attract viscosity epsilon x teta", ViscosityAttractεθ, minAttV, maxAttV));
	epsilonGroup.add(ProbAttractSliderεβ.setup("Attract prob epsilon x betha", ProbAttractεβ, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεα.setup("Attract prob epsilon x alpha", ProbAttractεα, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεγ.setup("Attract prob epsilon x gamma", ProbAttractεγ, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεδ.setup("Attract prob epsilon x delta", ProbAttractεδ, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεε.setup("Attract prob epsilon x epsilon", ProbAttractεε, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεζ.setup("Attract prob epsilon x zeta", ProbAttractεζ, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεη.setup("Attract prob epsilon x eta", ProbAttractεη, minAttI, maxAttI));
	epsilonGroup.add(ProbAttractSliderεθ.setup("Attract prob epsilon x teta", ProbAttractεθ, minAttI, maxAttI));
	epsilonGroup.add(RepelPowerSliderεα.setup("Repel power epsilon x alpha:", pRepelPowerSliderεα, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεβ.setup("Repel power epsilon x betha:", pRepelPowerSliderεβ, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεγ.setup("Repel power epsilon x gamma:", pRepelPowerSliderεγ, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεδ.setup("Repel power epsilon x delta:", pRepelPowerSliderεδ, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεε.setup("Repel power epsilon x epsilon:", pRepelPowerSliderεε, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεζ.setup("Repel power epsilon x zeta:", pRepelPowerSliderεζ, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεη.setup("Repel power epsilon x eta:", pRepelPowerSliderεη, minRepP, maxRepP));
	epsilonGroup.add(RepelPowerSliderεθ.setup("Repel power epsilon x teta:", pRepelPowerSliderεθ, minRepP, maxRepP));
	epsilonGroup.add(RepelDistanceSliderεα.setup("Repel radius epsilon x alpha:", pRepelDistanceSliderεα, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεβ.setup("Repel radius epsilon x betha:", pRepelDistanceSliderεβ, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεγ.setup("Repel radius epsilon x gamma:", pRepelDistanceSliderεγ, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεδ.setup("Repel radius epsilon x delta:", pRepelDistanceSliderεδ, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεε.setup("Repel radius epsilon x epsilon:", pRepelDistanceSliderεε, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεζ.setup("Repel radius epsilon x zeta:", pRepelDistanceSliderεζ, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεη.setup("Repel radius epsilon x eta:", pRepelDistanceSliderεη, minRepR, maxRepR));
	epsilonGroup.add(RepelDistanceSliderεθ.setup("Repel radius epsilon x teta:", pRepelDistanceSliderεθ, minRepR, maxRepR));
	epsilonGroup.add(ViscosityRepelSliderεβ.setup("Repel viscosity epsilon x betha", ViscosityRepelεβ, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεα.setup("Repel viscosity epsilon x alpha", ViscosityRepelεα, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεγ.setup("Repel viscosity epsilon x gamma", ViscosityRepelεγ, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεδ.setup("Repel viscosity epsilon x delta", ViscosityRepelεδ, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεε.setup("Repel viscosity epsilon x epsilon", ViscosityRepelεε, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεζ.setup("Repel viscosity epsilon x zeta", ViscosityRepelεζ, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεη.setup("Repel viscosity epsilon x eta", ViscosityRepelεη, minRepV, maxRepV));
	epsilonGroup.add(ViscosityRepelSliderεθ.setup("Repel viscosity epsilon x teta", ViscosityRepelεθ, minRepV, maxRepV));
	epsilonGroup.add(ProbRepelSliderεβ.setup("Repel prob epsilon x betha", ProbRepelεβ, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεα.setup("Repel prob epsilon x alpha", ProbRepelεα, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεγ.setup("Repel prob epsilon x gamma", ProbRepelεγ, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεδ.setup("Repel prob epsilon x delta", ProbRepelεδ, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεε.setup("Repel prob epsilon x epsilon", ProbRepelεε, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεζ.setup("Repel prob epsilon x zeta", ProbRepelεζ, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεη.setup("Repel prob epsilon x eta", ProbRepelεη, minRepI, maxRepI));
	epsilonGroup.add(ProbRepelSliderεθ.setup("Repel prob epsilon x teta", ProbRepelεθ, minRepI, maxRepI));
	epsilonGroup.minimize();
	gui.add(&epsilonGroup);

	// Zeta
	zetaGroup.setup("Zeta");
	zetaGroup.add(AttractPowerSliderζα.setup("Attract power zeta x alpha:", pAttractPowerSliderζα, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζβ.setup("Attract power zeta x betha:", pAttractPowerSliderζβ, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζγ.setup("Attract power zeta x gamma:", pAttractPowerSliderζγ, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζδ.setup("Attract power zeta x delta:", pAttractPowerSliderζδ, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζε.setup("Attract power zeta x epsilon:", pAttractPowerSliderζε, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζζ.setup("Attract power zeta x zeta:", pAttractPowerSliderζζ, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζη.setup("Attract power zeta x eta:", pAttractPowerSliderζη, minAttP, maxAttP));
	zetaGroup.add(AttractPowerSliderζθ.setup("Attract power zeta x teta:", pAttractPowerSliderζθ, minAttP, maxAttP));
	zetaGroup.add(AttractDistanceSliderζα.setup("Attract radius zeta x alpha:", pAttractDistanceSliderζα, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζβ.setup("Attract radius zeta x betha:", pAttractDistanceSliderζβ, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζγ.setup("Attract radius zeta x gamma:", pAttractDistanceSliderζγ, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζδ.setup("Attract radius zeta x delta:", pAttractDistanceSliderζδ, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζε.setup("Attract radius zeta x epsilon:", pAttractDistanceSliderζε, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζζ.setup("Attract radius zeta x zeta:", pAttractDistanceSliderζζ, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζη.setup("Attract radius zeta x eta:", pAttractDistanceSliderζη, minAttR, maxAttR));
	zetaGroup.add(AttractDistanceSliderζθ.setup("Attract radius zeta x teta:", pAttractDistanceSliderζθ, minAttR, maxAttR));
	zetaGroup.add(ViscosityAttractSliderζβ.setup("Attract viscosity zeta x betha", ViscosityAttractζβ, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζα.setup("Attract viscosity zeta x alpha", ViscosityAttractζα, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζγ.setup("Attract viscosity zeta x gamma", ViscosityAttractζγ, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζδ.setup("Attract viscosity zeta x delta", ViscosityAttractζδ, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζε.setup("Attract viscosity zeta x epsilon", ViscosityAttractζε, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζζ.setup("Attract viscosity zeta x zeta", ViscosityAttractζζ, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζη.setup("Attract viscosity zeta x eta", ViscosityAttractζη, minAttV, maxAttV));
	zetaGroup.add(ViscosityAttractSliderζθ.setup("Attract viscosity zeta x teta", ViscosityAttractζθ, minAttV, maxAttV));
	zetaGroup.add(ProbAttractSliderζβ.setup("Attract prob zeta x betha", ProbAttractζβ, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζα.setup("Attract prob zeta x alpha", ProbAttractζα, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζγ.setup("Attract prob zeta x gamma", ProbAttractζγ, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζδ.setup("Attract prob zeta x delta", ProbAttractζδ, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζε.setup("Attract prob zeta x epsilon", ProbAttractζε, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζζ.setup("Attract prob zeta x zeta", ProbAttractζζ, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζη.setup("Attract prob zeta x eta", ProbAttractζη, minAttI, maxAttI));
	zetaGroup.add(ProbAttractSliderζθ.setup("Attract prob zeta x teta", ProbAttractζθ, minAttI, maxAttI));
	zetaGroup.add(RepelPowerSliderζα.setup("Repel power zeta x alpha:", pRepelPowerSliderζα, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζβ.setup("Repel power zeta x betha:", pRepelPowerSliderζβ, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζγ.setup("Repel power zeta x gamma:", pRepelPowerSliderζγ, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζδ.setup("Repel power zeta x delta:", pRepelPowerSliderζδ, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζε.setup("Repel power zeta x epsilon:", pRepelPowerSliderζε, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζζ.setup("Repel power zeta x zeta:", pRepelPowerSliderζζ, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζη.setup("Repel power zeta x eta:", pRepelPowerSliderζη, minRepP, maxRepP));
	zetaGroup.add(RepelPowerSliderζθ.setup("Repel power zeta x teta:", pRepelPowerSliderζθ, minRepP, maxRepP));
	zetaGroup.add(RepelDistanceSliderζα.setup("Repel radius zeta x alpha:", pRepelDistanceSliderζα, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζβ.setup("Repel radius zeta x betha:", pRepelDistanceSliderζβ, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζγ.setup("Repel radius zeta x gamma:", pRepelDistanceSliderζγ, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζδ.setup("Repel radius zeta x delta:", pRepelDistanceSliderζδ, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζε.setup("Repel radius zeta x epsilon:", pRepelDistanceSliderζε, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζζ.setup("Repel radius zeta x zeta:", pRepelDistanceSliderζζ, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζη.setup("Repel radius zeta x eta:", pRepelDistanceSliderζη, minRepR, maxRepR));
	zetaGroup.add(RepelDistanceSliderζθ.setup("Repel radius zeta x teta:", pRepelDistanceSliderζθ, minRepR, maxRepR));
	zetaGroup.add(ViscosityRepelSliderζβ.setup("Repel viscosity zeta x betha", ViscosityRepelζβ, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζα.setup("Repel viscosity zeta x alpha", ViscosityRepelζα, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζγ.setup("Repel viscosity zeta x gamma", ViscosityRepelζγ, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζδ.setup("Repel viscosity zeta x delta", ViscosityRepelζδ, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζε.setup("Repel viscosity zeta x epsilon", ViscosityRepelζε, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζζ.setup("Repel viscosity zeta x zeta", ViscosityRepelζζ, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζη.setup("Repel viscosity zeta x eta", ViscosityRepelζη, minRepV, maxRepV));
	zetaGroup.add(ViscosityRepelSliderζθ.setup("Repel viscosity zeta x teta", ViscosityRepelζθ, minRepV, maxRepV));
	zetaGroup.add(ProbRepelSliderζβ.setup("Repel prob zeta x betha", ProbRepelζβ, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζα.setup("Repel prob zeta x alpha", ProbRepelζα, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζγ.setup("Repel prob zeta x gamma", ProbRepelζγ, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζδ.setup("Repel prob zeta x delta", ProbRepelζδ, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζε.setup("Repel prob zeta x epsilon", ProbRepelζε, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζζ.setup("Repel prob zeta x zeta", ProbRepelζζ, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζη.setup("Repel prob zeta x eta", ProbRepelζη, minRepI, maxRepI));
	zetaGroup.add(ProbRepelSliderζθ.setup("Repel prob zeta x teta", ProbRepelζθ, minRepI, maxRepI));
	zetaGroup.minimize();
	gui.add(&zetaGroup);

	// Eta
	etaGroup.setup("Eta");
	etaGroup.add(AttractPowerSliderηα.setup("Attract power eta x alpha:", pAttractPowerSliderηα, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηβ.setup("Attract power eta x betha:", pAttractPowerSliderηβ, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηγ.setup("Attract power eta x gamma:", pAttractPowerSliderηγ, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηδ.setup("Attract power eta x delta:", pAttractPowerSliderηδ, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηε.setup("Attract power eta x epsilon:", pAttractPowerSliderηε, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηζ.setup("Attract power eta x zeta:", pAttractPowerSliderηζ, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηη.setup("Attract power eta x eta:", pAttractPowerSliderηη, minAttP, maxAttP));
	etaGroup.add(AttractPowerSliderηθ.setup("Attract power eta x teta:", pAttractPowerSliderηθ, minAttP, maxAttP));
	etaGroup.add(AttractDistanceSliderηα.setup("Attract radius eta x alpha:", pAttractDistanceSliderηα, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηβ.setup("Attract radius eta x betha:", pAttractDistanceSliderηβ, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηγ.setup("Attract radius eta x gamma:", pAttractDistanceSliderηγ, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηδ.setup("Attract radius eta x delta:", pAttractDistanceSliderηδ, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηε.setup("Attract radius eta x epsilon:", pAttractDistanceSliderηε, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηζ.setup("Attract radius eta x zeta:", pAttractDistanceSliderηζ, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηη.setup("Attract radius eta x eta:", pAttractDistanceSliderηη, minAttR, maxAttR));
	etaGroup.add(AttractDistanceSliderηθ.setup("Attract radius eta x teta:", pAttractDistanceSliderηθ, minAttR, maxAttR));
	etaGroup.add(ViscosityAttractSliderηβ.setup("Attract viscosity eta x betha", ViscosityAttractηβ, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηα.setup("Attract viscosity eta x alpha", ViscosityAttractηα, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηγ.setup("Attract viscosity eta x gamma", ViscosityAttractηγ, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηδ.setup("Attract viscosity eta x delta", ViscosityAttractηδ, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηε.setup("Attract viscosity eta x epsilon", ViscosityAttractηε, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηζ.setup("Attract viscosity eta x zeta", ViscosityAttractηζ, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηη.setup("Attract viscosity eta x eta", ViscosityAttractηη, minAttV, maxAttV));
	etaGroup.add(ViscosityAttractSliderηθ.setup("Attract viscosity eta x teta", ViscosityAttractηθ, minAttV, maxAttV));
	etaGroup.add(ProbAttractSliderηβ.setup("Attract prob eta x betha", ProbAttractηβ, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηα.setup("Attract prob eta x alpha", ProbAttractηα, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηγ.setup("Attract prob eta x gamma", ProbAttractηγ, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηδ.setup("Attract prob eta x delta", ProbAttractηδ, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηε.setup("Attract prob eta x epsilon", ProbAttractηε, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηζ.setup("Attract prob eta x zeta", ProbAttractηζ, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηη.setup("Attract prob eta x eta", ProbAttractηη, minAttI, maxAttI));
	etaGroup.add(ProbAttractSliderηθ.setup("Attract prob eta x teta", ProbAttractηθ, minAttI, maxAttI));
	etaGroup.add(RepelPowerSliderηα.setup("Repel power eta x alpha:", pRepelPowerSliderηα, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηβ.setup("Repel power eta x betha:", pRepelPowerSliderηβ, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηγ.setup("Repel power eta x gamma:", pRepelPowerSliderηγ, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηδ.setup("Repel power eta x delta:", pRepelPowerSliderηδ, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηε.setup("Repel power eta x epsilon:", pRepelPowerSliderηε, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηζ.setup("Repel power eta x zeta:", pRepelPowerSliderηζ, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηη.setup("Repel power eta x eta:", pRepelPowerSliderηη, minRepP, maxRepP));
	etaGroup.add(RepelPowerSliderηθ.setup("Repel power eta x teta:", pRepelPowerSliderηθ, minRepP, maxRepP));
	etaGroup.add(RepelDistanceSliderηα.setup("Repel radius eta x alpha:", pRepelDistanceSliderηα, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηβ.setup("Repel radius eta x betha:", pRepelDistanceSliderηβ, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηγ.setup("Repel radius eta x gamma:", pRepelDistanceSliderηγ, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηδ.setup("Repel radius eta x delta:", pRepelDistanceSliderηδ, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηε.setup("Repel radius eta x epsilon:", pRepelDistanceSliderηε, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηζ.setup("Repel radius eta x zeta:", pRepelDistanceSliderηζ, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηη.setup("Repel radius eta x eta:", pRepelDistanceSliderηη, minRepR, maxRepR));
	etaGroup.add(RepelDistanceSliderηθ.setup("Repel radius eta x teta:", pRepelDistanceSliderηθ, minRepR, maxRepR));
	etaGroup.add(ViscosityRepelSliderηβ.setup("Repel viscosity eta x betha", ViscosityRepelηβ, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηα.setup("Repel viscosity eta x alpha", ViscosityRepelηα, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηγ.setup("Repel viscosity eta x gamma", ViscosityRepelηγ, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηδ.setup("Repel viscosity eta x delta", ViscosityRepelηδ, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηε.setup("Repel viscosity eta x epsilon", ViscosityRepelηε, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηζ.setup("Repel viscosity eta x zeta", ViscosityRepelηζ, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηη.setup("Repel viscosity eta x eta", ViscosityRepelηη, minRepV, maxRepV));
	etaGroup.add(ViscosityRepelSliderηθ.setup("Repel viscosity eta x teta", ViscosityRepelηθ, minRepV, maxRepV));
	etaGroup.add(ProbRepelSliderηβ.setup("Repel prob eta x betha", ProbRepelηβ, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηα.setup("Repel prob eta x alpha", ProbRepelηα, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηγ.setup("Repel prob eta x gamma", ProbRepelηγ, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηδ.setup("Repel prob eta x delta", ProbRepelηδ, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηε.setup("Repel prob eta x epsilon", ProbRepelηε, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηζ.setup("Repel prob eta x zeta", ProbRepelηζ, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηη.setup("Repel prob eta x eta", ProbRepelηη, minRepI, maxRepI));
	etaGroup.add(ProbRepelSliderηθ.setup("Repel prob eta x teta", ProbRepelηθ, minRepI, maxRepI));
	etaGroup.minimize();
	gui.add(&etaGroup);

	// Teta
	tetaGroup.setup("Teta - Dark");
	tetaGroup.add(AttractPowerSliderθα.setup("Attract power teta x alpha:", pAttractPowerSliderθα, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθβ.setup("Attract power teta x betha:", pAttractPowerSliderθβ, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθγ.setup("Attract power teta x gamma:", pAttractPowerSliderθγ, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθδ.setup("Attract power teta x delta:", pAttractPowerSliderθδ, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθε.setup("Attract power teta x epsilon:", pAttractPowerSliderθε, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθζ.setup("Attract power teta x zeta:", pAttractPowerSliderθζ, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθη.setup("Attract power teta x eta:", pAttractPowerSliderθη, minAttP, maxAttP));
	tetaGroup.add(AttractPowerSliderθθ.setup("Attract power teta x teta:", pAttractPowerSliderθθ, minAttP, maxAttP));
	tetaGroup.add(AttractDistanceSliderθα.setup("Attract radius teta x alpha:", pAttractDistanceSliderθα, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθβ.setup("Attract radius teta x betha:", pAttractDistanceSliderθβ, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθγ.setup("Attract radius teta x gamma:", pAttractDistanceSliderθγ, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθδ.setup("Attract radius teta x delta:", pAttractDistanceSliderθδ, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθε.setup("Attract radius teta x epsilon:", pAttractDistanceSliderθε, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθζ.setup("Attract radius teta x zeta:", pAttractDistanceSliderθζ, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθη.setup("Attract radius teta x eta:", pAttractDistanceSliderθη, minAttR, maxAttR));
	tetaGroup.add(AttractDistanceSliderθθ.setup("Attract radius teta x teta:", pAttractDistanceSliderθθ, minAttR, maxAttR));
	tetaGroup.add(ViscosityAttractSliderθβ.setup("Attract viscosity teta x betha", ViscosityAttractθβ, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθα.setup("Attract viscosity teta x alpha", ViscosityAttractθα, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθγ.setup("Attract viscosity teta x gamma", ViscosityAttractθγ, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθδ.setup("Attract viscosity teta x delta", ViscosityAttractθδ, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθε.setup("Attract viscosity teta x epsilon", ViscosityAttractθε, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθζ.setup("Attract viscosity teta x zeta", ViscosityAttractθζ, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθη.setup("Attract viscosity teta x eta", ViscosityAttractθη, minAttV, maxAttV));
	tetaGroup.add(ViscosityAttractSliderθθ.setup("Attract viscosity teta x teta", ViscosityAttractθθ, minAttV, maxAttV));
	tetaGroup.add(ProbAttractSliderθβ.setup("Attract prob teta x betha", ProbAttractθβ, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθα.setup("Attract prob teta x alpha", ProbAttractθα, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθγ.setup("Attract prob teta x gamma", ProbAttractθγ, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθδ.setup("Attract prob teta x delta", ProbAttractθδ, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθε.setup("Attract prob teta x epsilon", ProbAttractθε, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθζ.setup("Attract prob teta x zeta", ProbAttractθζ, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθη.setup("Attract prob teta x eta", ProbAttractθη, minAttI, maxAttI));
	tetaGroup.add(ProbAttractSliderθθ.setup("Attract prob teta x teta", ProbAttractθθ, minAttI, maxAttI));
	tetaGroup.add(RepelPowerSliderθα.setup("Repel power teta x alpha:", pRepelPowerSliderθα, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθβ.setup("Repel power teta x betha:", pRepelPowerSliderθβ, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθγ.setup("Repel power teta x gamma:", pRepelPowerSliderθγ, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθδ.setup("Repel power teta x delta:", pRepelPowerSliderθδ, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθε.setup("Repel power teta x epsilon:", pRepelPowerSliderθε, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθζ.setup("Repel power teta x zeta:", pRepelPowerSliderθζ, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθη.setup("Repel power teta x eta:", pRepelPowerSliderθη, minRepP, maxRepP));
	tetaGroup.add(RepelPowerSliderθθ.setup("Repel power teta x teta:", pRepelPowerSliderθθ, minRepP, maxRepP));
	tetaGroup.add(RepelDistanceSliderθα.setup("Repel radius teta x alpha:", pRepelDistanceSliderθα, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθβ.setup("Repel radius teta x betha:", pRepelDistanceSliderθβ, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθγ.setup("Repel radius teta x gamma:", pRepelDistanceSliderθγ, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθδ.setup("Repel radius teta x delta:", pRepelDistanceSliderθδ, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθε.setup("Repel radius teta x epsilon:", pRepelDistanceSliderθε, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθζ.setup("Repel radius teta x zeta:", pRepelDistanceSliderθζ, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθη.setup("Repel radius teta x eta:", pRepelDistanceSliderθη, minRepR, maxRepR));
	tetaGroup.add(RepelDistanceSliderθθ.setup("Repel radius teta x teta:", pRepelDistanceSliderθθ, minRepR, maxRepR));
	tetaGroup.add(ViscosityRepelSliderθβ.setup("Repel viscosity teta x betha", ViscosityRepelθβ, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθα.setup("Repel viscosity teta x alpha", ViscosityRepelθα, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθγ.setup("Repel viscosity teta x gamma", ViscosityRepelθγ, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθδ.setup("Repel viscosity teta x delta", ViscosityRepelθδ, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθε.setup("Repel viscosity teta x epsilon", ViscosityRepelθε, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθζ.setup("Repel viscosity teta x zeta", ViscosityRepelθζ, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθη.setup("Repel viscosity teta x eta", ViscosityRepelθη, minRepV, maxRepV));
	tetaGroup.add(ViscosityRepelSliderθθ.setup("Repel viscosity teta x teta", ViscosityRepelθθ, minRepV, maxRepV));
	tetaGroup.add(ProbRepelSliderθβ.setup("Repel prob teta x betha", ProbRepelθβ, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθα.setup("Repel prob teta x alpha", ProbRepelθα, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθγ.setup("Repel prob teta x gamma", ProbRepelθγ, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθδ.setup("Repel prob teta x delta", ProbRepelθδ, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθε.setup("Repel prob teta x epsilon", ProbRepelθε, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθζ.setup("Repel prob teta x zeta", ProbRepelθζ, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθη.setup("Repel prob teta x eta", ProbRepelθη, minRepI, maxRepI));
	tetaGroup.add(ProbRepelSliderθθ.setup("Repel prob teta x teta", ProbRepelθθ, minRepI, maxRepI));
	tetaGroup.minimize();
	gui.add(&tetaGroup);


	expGroup.setup("Experimental");
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
	minAttP = minAttractPowerSlider;
	maxAttP = maxAttractPowerSlider;
	minAttR = minAttractRangeSlider;
	maxAttR = maxAttractRangeSlider;
	minAttV = minAttractViscoSlider;
	maxAttV = maxAttractViscoSlider;
	minAttI = minAttractProbSlider;
	maxAttI = maxAttractProbSlider;
	minRepP = minRepelPowerSlider;
	maxRepP = maxRepelPowerSlider;
	minRepR = minRepelRangeSlider;
	maxRepR = maxRepelRangeSlider;
	minRepV = minRepelViscoSlider;
	maxRepV = maxRepelViscoSlider;
	minRepI = minRepelProbSlider;
	maxRepI = maxRepelProbSlider;
	probability = probabilitySlider;
	ProbAttractαα = ProbAttractSliderαα;
	ProbAttractαβ = ProbAttractSliderαβ;
	ProbAttractαγ = ProbAttractSliderαγ;
	ProbAttractαδ = ProbAttractSliderαδ;
	ProbAttractαε = ProbAttractSliderαε;
	ProbAttractαζ = ProbAttractSliderαζ;
	ProbAttractαη = ProbAttractSliderαη;
	ProbAttractβα = ProbAttractSliderβα;
	ProbAttractββ = ProbAttractSliderββ;
	ProbAttractβγ = ProbAttractSliderβγ;
	ProbAttractβδ = ProbAttractSliderβδ;
	ProbAttractβε = ProbAttractSliderβε;
	ProbAttractβζ = ProbAttractSliderβζ;
	ProbAttractβη = ProbAttractSliderβη;
	ProbAttractγα = ProbAttractSliderγα;
	ProbAttractγβ = ProbAttractSliderγβ;
	ProbAttractγγ = ProbAttractSliderγγ;
	ProbAttractγδ = ProbAttractSliderγδ;
	ProbAttractγε = ProbAttractSliderγε;
	ProbAttractγζ = ProbAttractSliderγζ;
	ProbAttractγη = ProbAttractSliderγη;
	ProbAttractδα = ProbAttractSliderδα;
	ProbAttractδβ = ProbAttractSliderδβ;
	ProbAttractδγ = ProbAttractSliderδγ;
	ProbAttractδδ = ProbAttractSliderδδ;
	ProbAttractδε = ProbAttractSliderδε;
	ProbAttractδζ = ProbAttractSliderδζ;
	ProbAttractδη = ProbAttractSliderδη;
	ProbAttractεα = ProbAttractSliderεα;
	ProbAttractεβ = ProbAttractSliderεβ;
	ProbAttractεγ = ProbAttractSliderεγ;
	ProbAttractεδ = ProbAttractSliderεδ;
	ProbAttractεε = ProbAttractSliderεε;
	ProbAttractεζ = ProbAttractSliderεζ;
	ProbAttractεη = ProbAttractSliderεη;
	ProbAttractζβ = ProbAttractSliderζβ;
	ProbAttractζγ = ProbAttractSliderζγ;
	ProbAttractζδ = ProbAttractSliderζδ;
	ProbAttractζε = ProbAttractSliderζε;
	ProbAttractζζ = ProbAttractSliderζζ;
	ProbAttractζη = ProbAttractSliderζη;
	ProbAttractηα = ProbAttractSliderηα;
	ProbAttractηβ = ProbAttractSliderηβ;
	ProbAttractηγ = ProbAttractSliderηγ;
	ProbAttractηδ = ProbAttractSliderηδ;
	ProbAttractηε = ProbAttractSliderηε;
	ProbAttractηζ = ProbAttractSliderηζ;
	ProbAttractηη = ProbAttractSliderηη;
	ProbAttractαθ = ProbAttractSliderαθ;
	ProbAttractβθ = ProbAttractSliderβθ;
	ProbAttractγθ = ProbAttractSliderγθ;
	ProbAttractδθ = ProbAttractSliderδθ;
	ProbAttractεθ = ProbAttractSliderεθ;
	ProbAttractζθ = ProbAttractSliderζθ;
	ProbAttractηθ = ProbAttractSliderηθ;
	ProbAttractθα = ProbAttractSliderθα;
	ProbAttractθβ = ProbAttractSliderθβ;
	ProbAttractθγ = ProbAttractSliderθγ;
	ProbAttractθδ = ProbAttractSliderθδ;
	ProbAttractθε = ProbAttractSliderθε;
	ProbAttractθζ = ProbAttractSliderθζ;
	ProbAttractθη = ProbAttractSliderθη;
	ProbAttractθθ = ProbAttractSliderθθ;
	ProbRepelαα = ProbRepelSliderαα;
	ProbRepelαβ = ProbRepelSliderαβ;
	ProbRepelαγ = ProbRepelSliderαγ;
	ProbRepelαδ = ProbRepelSliderαδ;
	ProbRepelαε = ProbRepelSliderαε;
	ProbRepelαζ = ProbRepelSliderαζ;
	ProbRepelαη = ProbRepelSliderαη;
	ProbRepelβα = ProbRepelSliderβα;
	ProbRepelββ = ProbRepelSliderββ;
	ProbRepelβγ = ProbRepelSliderβγ;
	ProbRepelβδ = ProbRepelSliderβδ;
	ProbRepelβε = ProbRepelSliderβε;
	ProbRepelβζ = ProbRepelSliderβζ;
	ProbRepelβη = ProbRepelSliderβη;
	ProbRepelγα = ProbRepelSliderγα;
	ProbRepelγβ = ProbRepelSliderγβ;
	ProbRepelγγ = ProbRepelSliderγγ;
	ProbRepelγδ = ProbRepelSliderγδ;
	ProbRepelγε = ProbRepelSliderγε;
	ProbRepelγζ = ProbRepelSliderγζ;
	ProbRepelγη = ProbRepelSliderγη;
	ProbRepelδα = ProbRepelSliderδα;
	ProbRepelδβ = ProbRepelSliderδβ;
	ProbRepelδγ = ProbRepelSliderδγ;
	ProbRepelδδ = ProbRepelSliderδδ;
	ProbRepelδε = ProbRepelSliderδε;
	ProbRepelδζ = ProbRepelSliderδζ;
	ProbRepelδη = ProbRepelSliderδη;
	ProbRepelεα = ProbRepelSliderεα;
	ProbRepelεβ = ProbRepelSliderεβ;
	ProbRepelεγ = ProbRepelSliderεγ;
	ProbRepelεδ = ProbRepelSliderεδ;
	ProbRepelεε = ProbRepelSliderεε;
	ProbRepelεζ = ProbRepelSliderεζ;
	ProbRepelεη = ProbRepelSliderεη;
	ProbRepelζβ = ProbRepelSliderζβ;
	ProbRepelζγ = ProbRepelSliderζγ;
	ProbRepelζδ = ProbRepelSliderζδ;
	ProbRepelζε = ProbRepelSliderζε;
	ProbRepelζζ = ProbRepelSliderζζ;
	ProbRepelζη = ProbRepelSliderζη;
	ProbRepelηα = ProbRepelSliderηα;
	ProbRepelηβ = ProbRepelSliderηβ;
	ProbRepelηγ = ProbRepelSliderηγ;
	ProbRepelηδ = ProbRepelSliderηδ;
	ProbRepelηε = ProbRepelSliderηε;
	ProbRepelηζ = ProbRepelSliderηζ;
	ProbRepelηη = ProbRepelSliderηη;
	ProbRepelαθ = ProbRepelSliderαθ;
	ProbRepelβθ = ProbRepelSliderβθ;
	ProbRepelγθ = ProbRepelSliderγθ;
	ProbRepelδθ = ProbRepelSliderδθ;
	ProbRepelεθ = ProbRepelSliderεθ;
	ProbRepelζθ = ProbRepelSliderζθ;
	ProbRepelηθ = ProbRepelSliderηθ;
	ProbRepelθα = ProbRepelSliderθα;
	ProbRepelθβ = ProbRepelSliderθβ;
	ProbRepelθγ = ProbRepelSliderθγ;
	ProbRepelθδ = ProbRepelSliderθδ;
	ProbRepelθε = ProbRepelSliderθε;
	ProbRepelθζ = ProbRepelSliderθζ;
	ProbRepelθη = ProbRepelSliderθη;
	ProbRepelθθ = ProbRepelSliderθθ;
	viscosity = viscositySlider;
	ViscosityAttractαα = ViscosityAttractSliderαα;
	ViscosityAttractαβ = ViscosityAttractSliderαβ;
	ViscosityAttractαδ = ViscosityAttractSliderαδ;
	ViscosityAttractαγ = ViscosityAttractSliderαγ;
	ViscosityAttractαε = ViscosityAttractSliderαε;
	ViscosityAttractαζ = ViscosityAttractSliderαζ;
	ViscosityAttractαη = ViscosityAttractSliderαη;
	ViscosityAttractβα = ViscosityAttractSliderβα;
	ViscosityAttractββ = ViscosityAttractSliderββ;
	ViscosityAttractβδ = ViscosityAttractSliderβδ;
	ViscosityAttractβγ = ViscosityAttractSliderβγ;
	ViscosityAttractβε = ViscosityAttractSliderβε;
	ViscosityAttractβζ = ViscosityAttractSliderβζ;
	ViscosityAttractβη = ViscosityAttractSliderβη;
	ViscosityAttractγα = ViscosityAttractSliderγα;
	ViscosityAttractγβ = ViscosityAttractSliderγβ;
	ViscosityAttractγδ = ViscosityAttractSliderγδ;
	ViscosityAttractγγ = ViscosityAttractSliderγγ;
	ViscosityAttractγε = ViscosityAttractSliderγε;
	ViscosityAttractγζ = ViscosityAttractSliderγζ;
	ViscosityAttractγη = ViscosityAttractSliderγη;
	ViscosityAttractδα = ViscosityAttractSliderδα;
	ViscosityAttractδβ = ViscosityAttractSliderδβ;
	ViscosityAttractδδ = ViscosityAttractSliderδδ;
	ViscosityAttractδγ = ViscosityAttractSliderδγ;
	ViscosityAttractδε = ViscosityAttractSliderδε;
	ViscosityAttractδζ = ViscosityAttractSliderδζ;
	ViscosityAttractδη = ViscosityAttractSliderδη;
	ViscosityAttractεα = ViscosityAttractSliderεα;
	ViscosityAttractεβ = ViscosityAttractSliderεβ;
	ViscosityAttractεδ = ViscosityAttractSliderεδ;
	ViscosityAttractεγ = ViscosityAttractSliderεγ;
	ViscosityAttractεε = ViscosityAttractSliderεε;
	ViscosityAttractεζ = ViscosityAttractSliderεζ;
	ViscosityAttractεη = ViscosityAttractSliderεη;
	ViscosityAttractζα = ViscosityAttractSliderζα;
	ViscosityAttractζβ = ViscosityAttractSliderζβ;
	ViscosityAttractζδ = ViscosityAttractSliderζδ;
	ViscosityAttractζγ = ViscosityAttractSliderζγ;
	ViscosityAttractζε = ViscosityAttractSliderζε;
	ViscosityAttractζζ = ViscosityAttractSliderζζ;
	ViscosityAttractζη = ViscosityAttractSliderζη;
	ViscosityAttractηα = ViscosityAttractSliderηα;
	ViscosityAttractηβ = ViscosityAttractSliderηβ;
	ViscosityAttractηδ = ViscosityAttractSliderηδ;
	ViscosityAttractηγ = ViscosityAttractSliderηγ;
	ViscosityAttractηε = ViscosityAttractSliderηε;
	ViscosityAttractηζ = ViscosityAttractSliderηζ;
	ViscosityAttractηη = ViscosityAttractSliderηη;
	ViscosityAttractαθ = ViscosityAttractSliderαθ;
	ViscosityAttractβθ = ViscosityAttractSliderβθ;
	ViscosityAttractδθ = ViscosityAttractSliderδθ;
	ViscosityAttractγθ = ViscosityAttractSliderγθ;
	ViscosityAttractεθ = ViscosityAttractSliderεθ;
	ViscosityAttractζθ = ViscosityAttractSliderζθ;
	ViscosityAttractηθ = ViscosityAttractSliderηθ;
	ViscosityAttractθα = ViscosityAttractSliderθα;
	ViscosityAttractθβ = ViscosityAttractSliderθβ;
	ViscosityAttractθδ = ViscosityAttractSliderθδ;
	ViscosityAttractθγ = ViscosityAttractSliderθγ;
	ViscosityAttractθε = ViscosityAttractSliderθε;
	ViscosityAttractθζ = ViscosityAttractSliderθζ;
	ViscosityAttractθη = ViscosityAttractSliderθη;
	ViscosityAttractθθ = ViscosityAttractSliderθθ;
	ViscosityRepelαα = ViscosityRepelSliderαα;
	ViscosityRepelαβ = ViscosityRepelSliderαβ;
	ViscosityRepelαδ = ViscosityRepelSliderαδ;
	ViscosityRepelαγ = ViscosityRepelSliderαγ;
	ViscosityRepelαε = ViscosityRepelSliderαε;
	ViscosityRepelαζ = ViscosityRepelSliderαζ;
	ViscosityRepelαη = ViscosityRepelSliderαη;
	ViscosityRepelβα = ViscosityRepelSliderβα;
	ViscosityRepelββ = ViscosityRepelSliderββ;
	ViscosityRepelβδ = ViscosityRepelSliderβδ;
	ViscosityRepelβγ = ViscosityRepelSliderβγ;
	ViscosityRepelβε = ViscosityRepelSliderβε;
	ViscosityRepelβζ = ViscosityRepelSliderβζ;
	ViscosityRepelβη = ViscosityRepelSliderβη;
	ViscosityRepelγα = ViscosityRepelSliderγα;
	ViscosityRepelγβ = ViscosityRepelSliderγβ;
	ViscosityRepelγδ = ViscosityRepelSliderγδ;
	ViscosityRepelγγ = ViscosityRepelSliderγγ;
	ViscosityRepelγε = ViscosityRepelSliderγε;
	ViscosityRepelγζ = ViscosityRepelSliderγζ;
	ViscosityRepelγη = ViscosityRepelSliderγη;
	ViscosityRepelδα = ViscosityRepelSliderδα;
	ViscosityRepelδβ = ViscosityRepelSliderδβ;
	ViscosityRepelδδ = ViscosityRepelSliderδδ;
	ViscosityRepelδγ = ViscosityRepelSliderδγ;
	ViscosityRepelδε = ViscosityRepelSliderδε;
	ViscosityRepelδζ = ViscosityRepelSliderδζ;
	ViscosityRepelδη = ViscosityRepelSliderδη;
	ViscosityRepelεα = ViscosityRepelSliderεα;
	ViscosityRepelεβ = ViscosityRepelSliderεβ;
	ViscosityRepelεδ = ViscosityRepelSliderεδ;
	ViscosityRepelεγ = ViscosityRepelSliderεγ;
	ViscosityRepelεε = ViscosityRepelSliderεε;
	ViscosityRepelεζ = ViscosityRepelSliderεζ;
	ViscosityRepelεη = ViscosityRepelSliderεη;
	ViscosityRepelζα = ViscosityRepelSliderζα;
	ViscosityRepelζβ = ViscosityRepelSliderζβ;
	ViscosityRepelζδ = ViscosityRepelSliderζδ;
	ViscosityRepelζγ = ViscosityRepelSliderζγ;
	ViscosityRepelζε = ViscosityRepelSliderζε;
	ViscosityRepelζζ = ViscosityRepelSliderζζ;
	ViscosityRepelζη = ViscosityRepelSliderζη;
	ViscosityRepelηα = ViscosityRepelSliderηα;
	ViscosityRepelηβ = ViscosityRepelSliderηβ;
	ViscosityRepelηδ = ViscosityRepelSliderηδ;
	ViscosityRepelηγ = ViscosityRepelSliderηγ;
	ViscosityRepelηε = ViscosityRepelSliderηε;
	ViscosityRepelηζ = ViscosityRepelSliderηζ;
	ViscosityRepelηη = ViscosityRepelSliderηη;
	ViscosityRepelαθ = ViscosityRepelSliderαθ;
	ViscosityRepelβθ = ViscosityRepelSliderβθ;
	ViscosityRepelδθ = ViscosityRepelSliderδθ;
	ViscosityRepelγθ = ViscosityRepelSliderγθ;
	ViscosityRepelεθ = ViscosityRepelSliderεθ;
	ViscosityRepelζθ = ViscosityRepelSliderζθ;
	ViscosityRepelηθ = ViscosityRepelSliderηθ;
	ViscosityRepelθα = ViscosityRepelSliderθα;
	ViscosityRepelθβ = ViscosityRepelSliderθβ;
	ViscosityRepelθδ = ViscosityRepelSliderθδ;
	ViscosityRepelθγ = ViscosityRepelSliderθγ;
	ViscosityRepelθε = ViscosityRepelSliderθε;
	ViscosityRepelθζ = ViscosityRepelSliderθζ;
	ViscosityRepelθη = ViscosityRepelSliderθη;
	ViscosityRepelθθ = ViscosityRepelSliderθθ;
	worldGravity = gravitySlider;
	wallRepel = wallRepelSlider;
	AttractEvoChance = AttractEvoProbSlider;
	AttractEvoAmount = AttractEvoAmountSlider;
	ProbAttractEvoChance = ProbAttractEvoProbSlider;
	ProbAttractEvoAmount = ProbAttractEvoAmountSlider;
	ViscoAttractEvoChance = ViscoAttractEvoProbSlider;
	ViscoAttractEvoAmount = ViscoAttractEvoAmountSlider;
	RepelEvoChance = RepelEvoProbSlider;
	RepelEvoAmount = RepelEvoAmountSlider;
	ProbRepelEvoChance = ProbRepelEvoProbSlider;
	ProbRepelEvoAmount = ProbRepelEvoAmountSlider;
	ViscoRepelEvoChance = ViscoRepelEvoProbSlider;
	ViscoRepelEvoAmount = ViscoRepelEvoAmountSlider;

	if (evoToggle && ofRandom(1.0F) < (AttractEvoChance / 100.0F))
	{
		for (auto& slider : AttractPowerSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (AttractEvoAmount / 100.0F));
			if (*slider < minAttP) *slider = minAttP;
			if (*slider > maxAttP) *slider = maxAttP;
		}
		for (auto& slider : AttractDistanceSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (AttractEvoAmount / 100.0F));
			if (*slider < minAttR) *slider = minAttR;
			if (*slider > maxAttR) *slider = maxAttR;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (RepelEvoChance / 100.0F))
	{
		for (auto& slider : RepelPowerSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (RepelEvoAmount / 100.0F));
			if (*slider < minRepP) *slider = minRepP;
			if (*slider > maxRepP) *slider = maxRepP;
		}
		for (auto& slider : RepelDistanceSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (RepelEvoAmount / 100.0F));
			if (*slider < minRepR) *slider = minRepR;
			if (*slider > maxRepR) *slider = maxRepR;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ViscoAttractEvoChance / 100.0F))
	{
		for (auto& slider : ViscosityAttractSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ViscoAttractEvoAmount / 100.0F));
			if (*slider < minAttV) *slider = minAttV;
			if (*slider > maxAttV) *slider = maxAttV;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ViscoRepelEvoChance / 100.0F))
	{
		for (auto& slider : ViscosityRepelSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ViscoRepelEvoAmount / 100.0F));
			if (*slider < minRepV) *slider = minRepV;
			if (*slider > maxRepV) *slider = maxRepV;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ProbAttractEvoChance / 100.0F))
	{
		for (auto& slider : ProbAttractSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ProbAttractEvoAmount / 100.0F));
			if (*slider < minAttI) *slider = minAttI;
			if (*slider > maxAttI) *slider = maxAttI;
		}
	}
	if (evoToggle && ofRandom(1.0F) < (ProbRepelEvoChance / 100.0F))
	{
		for (auto& slider : ProbRepelSliders) {
			*slider = *slider + ((ofRandom(2.0F) - 1.0F) * (slider->getMax() - slider->getMin()) * (ProbRepelEvoAmount / 100.0F));
			if (*slider < minAttI) *slider = minAttI;
			if (*slider > maxAttI) *slider = maxAttI;
		}
	}

	if (numberSliderα > 0)
	{
		interaction(&alpha, &alpha, AttractPowerSliderαα, RepelPowerSliderαα, AttractDistanceSliderαα, RepelDistanceSliderαα, ViscosityAttractSliderαα, ViscosityRepelSliderαα, ProbAttractSliderαα, ProbRepelSliderαα);
	if (numberSliderβ > 0) interaction(&alpha, &betha, AttractPowerSliderαβ, RepelPowerSliderαβ, AttractDistanceSliderαβ, RepelDistanceSliderαβ, ViscosityAttractSliderαβ, ViscosityRepelSliderαβ, ProbAttractSliderαβ, ProbRepelSliderαβ);
	if (numberSliderγ > 0) interaction(&alpha, &gamma, AttractPowerSliderαγ, RepelPowerSliderαγ, AttractDistanceSliderαγ, RepelDistanceSliderαγ, ViscosityAttractSliderαγ, ViscosityRepelSliderαγ, ProbAttractSliderαγ, ProbRepelSliderαγ);
	if (numberSliderδ > 0) interaction(&alpha, &elta, AttractPowerSliderαδ, RepelPowerSliderαδ, AttractDistanceSliderαδ, RepelDistanceSliderαδ, ViscosityAttractSliderαδ, ViscosityRepelSliderαδ, ProbAttractSliderαδ, ProbRepelSliderαδ);
	if (numberSliderε > 0) interaction(&alpha, &epsilon, AttractPowerSliderαε, RepelPowerSliderαε, AttractDistanceSliderαε, RepelDistanceSliderαε, ViscosityAttractSliderαε, ViscosityRepelSliderαε, ProbAttractSliderαε, ProbRepelSliderαε);
	if (numberSliderζ > 0) interaction(&alpha, &zeta, AttractPowerSliderαζ, RepelPowerSliderαζ, AttractDistanceSliderαζ, RepelDistanceSliderαζ, ViscosityAttractSliderαζ, ViscosityRepelSliderαζ, ProbAttractSliderαζ, ProbRepelSliderαζ);
	if (numberSliderη > 0) interaction(&alpha, &eta, AttractPowerSliderαη, RepelPowerSliderαη, AttractDistanceSliderαη, RepelDistanceSliderαη, ViscosityAttractSliderαη, ViscosityRepelSliderαη, ProbAttractSliderαη, ProbRepelSliderαη);
	if (numberSliderθ > 0) interaction(&alpha, &teta, AttractPowerSliderαθ, RepelPowerSliderαθ, AttractDistanceSliderαθ, RepelDistanceSliderαθ, ViscosityAttractSliderαθ, ViscosityRepelSliderαθ, ProbAttractSliderαθ, ProbRepelSliderαθ);
	}

	if (numberSliderβ > 0)
	{
		interaction(&betha, &betha, AttractPowerSliderββ, RepelPowerSliderββ, AttractDistanceSliderββ, RepelDistanceSliderββ, ViscosityAttractSliderββ, ViscosityRepelSliderββ, ProbAttractSliderββ, ProbRepelSliderββ);
		if (numberSliderα > 0) interaction(&betha, &alpha, AttractPowerSliderβα, RepelPowerSliderβα, AttractDistanceSliderβα, RepelDistanceSliderβα, ViscosityAttractSliderβα, ViscosityRepelSliderβα, ProbAttractSliderβα, ProbRepelSliderβα);
		if (numberSliderγ > 0) interaction(&betha, &gamma, AttractPowerSliderβγ, RepelPowerSliderβγ, AttractDistanceSliderβγ, RepelDistanceSliderβγ, ViscosityAttractSliderβγ, ViscosityRepelSliderβγ, ProbAttractSliderβγ, ProbRepelSliderβγ);
		if (numberSliderδ > 0) interaction(&betha, &elta, AttractPowerSliderβδ, RepelPowerSliderβδ, AttractDistanceSliderβδ, RepelDistanceSliderβδ, ViscosityAttractSliderβδ, ViscosityRepelSliderβδ, ProbAttractSliderβδ, ProbRepelSliderβδ);
		if (numberSliderε > 0) interaction(&betha, &epsilon, AttractPowerSliderβε, RepelPowerSliderβε, AttractDistanceSliderβε, RepelDistanceSliderβε, ViscosityAttractSliderβε, ViscosityRepelSliderβε, ProbAttractSliderβε, ProbRepelSliderβε);
		if (numberSliderζ > 0) interaction(&betha, &zeta, AttractPowerSliderβζ, RepelPowerSliderβζ, AttractDistanceSliderβζ, RepelDistanceSliderβζ, ViscosityAttractSliderβζ, ViscosityRepelSliderβζ, ProbAttractSliderβζ, ProbRepelSliderβζ);
		if (numberSliderη > 0) interaction(&betha, &eta, AttractPowerSliderβη, RepelPowerSliderβη, AttractDistanceSliderβη, RepelDistanceSliderβη, ViscosityAttractSliderβη, ViscosityRepelSliderβη, ProbAttractSliderβη, ProbRepelSliderβη);
		if (numberSliderθ > 0) interaction(&betha, &teta, AttractPowerSliderβθ, RepelPowerSliderβθ, AttractDistanceSliderβθ, RepelDistanceSliderβθ, ViscosityAttractSliderβθ, ViscosityRepelSliderβθ, ProbAttractSliderβθ, ProbRepelSliderβθ);
	}

	if (numberSliderγ > 0)
	{
		interaction(&gamma, &gamma, AttractPowerSliderγγ, RepelPowerSliderγγ, AttractDistanceSliderγγ, RepelDistanceSliderγγ, ViscosityAttractSliderγγ, ViscosityRepelSliderγγ, ProbAttractSliderγγ, ProbRepelSliderγγ);
		if (numberSliderα > 0) interaction(&gamma, &alpha, AttractPowerSliderγα, RepelPowerSliderγα, AttractDistanceSliderγα, RepelDistanceSliderγα, ViscosityAttractSliderγα, ViscosityRepelSliderγα, ProbAttractSliderγα, ProbRepelSliderγα);
		if (numberSliderβ > 0) interaction(&gamma, &betha, AttractPowerSliderγβ, RepelPowerSliderγβ, AttractDistanceSliderγβ, RepelDistanceSliderγβ, ViscosityAttractSliderγβ, ViscosityRepelSliderγβ, ProbAttractSliderγβ, ProbRepelSliderγβ);
		if (numberSliderδ > 0) interaction(&gamma, &elta, AttractPowerSliderγδ, RepelPowerSliderγδ, AttractDistanceSliderγδ, RepelDistanceSliderγδ, ViscosityAttractSliderγδ, ViscosityRepelSliderγδ, ProbAttractSliderγδ, ProbRepelSliderγδ);
		if (numberSliderε > 0) interaction(&gamma, &epsilon, AttractPowerSliderγε, RepelPowerSliderγε, AttractDistanceSliderγε, RepelDistanceSliderγε, ViscosityAttractSliderγε, ViscosityRepelSliderγε, ProbAttractSliderγε, ProbRepelSliderγε);
		if (numberSliderζ > 0) interaction(&gamma, &zeta, AttractPowerSliderγζ, RepelPowerSliderγζ, AttractDistanceSliderγζ, RepelDistanceSliderγζ, ViscosityAttractSliderγζ, ViscosityRepelSliderγζ, ProbAttractSliderγζ, ProbRepelSliderγζ);
		if (numberSliderη > 0) interaction(&gamma, &eta, AttractPowerSliderγη, RepelPowerSliderγη, AttractDistanceSliderγη, RepelDistanceSliderγη, ViscosityAttractSliderγη, ViscosityRepelSliderγη, ProbAttractSliderγη, ProbRepelSliderγη);
		if (numberSliderθ > 0) interaction(&gamma, &teta, AttractPowerSliderγθ, RepelPowerSliderγθ, AttractDistanceSliderγθ, RepelDistanceSliderγθ, ViscosityAttractSliderγθ, ViscosityRepelSliderγθ, ProbAttractSliderγθ, ProbRepelSliderγθ);
	}

	if (numberSliderδ > 0)
	{
		interaction(&elta, &elta, AttractPowerSliderδδ, RepelPowerSliderδδ, AttractDistanceSliderδδ, RepelDistanceSliderδδ, ViscosityAttractSliderδδ, ViscosityRepelSliderδδ, ProbAttractSliderδδ, ProbRepelSliderδδ);
		if (numberSliderα > 0) interaction(&elta, &alpha, AttractPowerSliderδα, RepelPowerSliderδα, AttractDistanceSliderδα, RepelDistanceSliderδα, ViscosityAttractSliderδα, ViscosityRepelSliderδα, ProbAttractSliderδα, ProbRepelSliderδα);
		if (numberSliderβ > 0) interaction(&elta, &betha, AttractPowerSliderδβ, RepelPowerSliderδβ, AttractDistanceSliderδβ, RepelDistanceSliderδβ, ViscosityAttractSliderδβ, ViscosityRepelSliderδβ, ProbAttractSliderδβ, ProbRepelSliderδβ);
		if (numberSliderγ > 0) interaction(&elta, &gamma, AttractPowerSliderδγ, RepelPowerSliderδγ, AttractDistanceSliderδγ, RepelDistanceSliderδγ, ViscosityAttractSliderδγ, ViscosityRepelSliderδγ, ProbAttractSliderδγ, ProbRepelSliderδγ);
		if (numberSliderε > 0) interaction(&elta, &epsilon, AttractPowerSliderδε, RepelPowerSliderδε, AttractDistanceSliderδε, RepelDistanceSliderδε, ViscosityAttractSliderδε, ViscosityRepelSliderδε, ProbAttractSliderδε, ProbRepelSliderδε);
		if (numberSliderζ > 0) interaction(&elta, &zeta, AttractPowerSliderδζ, RepelPowerSliderδζ, AttractDistanceSliderδζ, RepelDistanceSliderδζ, ViscosityAttractSliderδζ, ViscosityRepelSliderδζ, ProbAttractSliderδζ, ProbRepelSliderδζ);
		if (numberSliderη > 0) interaction(&elta, &eta, AttractPowerSliderδη, RepelPowerSliderδη, AttractDistanceSliderδη, RepelDistanceSliderδη, ViscosityAttractSliderδη, ViscosityRepelSliderδη, ProbAttractSliderδη, ProbRepelSliderδη);
		if (numberSliderθ > 0) interaction(&elta, &teta, AttractPowerSliderδθ, RepelPowerSliderδθ, AttractDistanceSliderδθ, RepelDistanceSliderδθ, ViscosityAttractSliderδθ, ViscosityRepelSliderδθ, ProbAttractSliderδθ, ProbRepelSliderδθ);
	}

	if (numberSliderε > 0)
	{
		interaction(&epsilon, &epsilon, AttractPowerSliderεε, RepelPowerSliderεε, AttractDistanceSliderεε, RepelDistanceSliderεε, ViscosityAttractSliderεε, ViscosityRepelSliderεε, ProbAttractSliderεε, ProbRepelSliderεε);
		if (numberSliderα > 0) interaction(&epsilon, &alpha, AttractPowerSliderεα, RepelPowerSliderεα, AttractDistanceSliderεα, RepelDistanceSliderεα, ViscosityAttractSliderεα, ViscosityRepelSliderεα, ProbAttractSliderεα, ProbRepelSliderεα);
		if (numberSliderβ > 0) interaction(&epsilon, &betha, AttractPowerSliderεβ, RepelPowerSliderεβ, AttractDistanceSliderεβ, RepelDistanceSliderεβ, ViscosityAttractSliderεβ, ViscosityRepelSliderεβ, ProbAttractSliderεβ, ProbRepelSliderεβ);
		if (numberSliderγ > 0) interaction(&epsilon, &gamma, AttractPowerSliderεγ, RepelPowerSliderεγ, AttractDistanceSliderεγ, RepelDistanceSliderεγ, ViscosityAttractSliderεγ, ViscosityRepelSliderεγ, ProbAttractSliderεγ, ProbRepelSliderεγ);
		if (numberSliderδ > 0) interaction(&epsilon, &elta, AttractPowerSliderεδ, RepelPowerSliderεδ, AttractDistanceSliderεδ, RepelDistanceSliderεδ, ViscosityAttractSliderεδ, ViscosityRepelSliderεδ, ProbAttractSliderεδ, ProbRepelSliderεδ);
		if (numberSliderζ > 0) interaction(&epsilon, &zeta, AttractPowerSliderεζ, RepelPowerSliderεζ, AttractDistanceSliderεζ, RepelDistanceSliderεζ, ViscosityAttractSliderεζ, ViscosityRepelSliderεζ, ProbAttractSliderεζ, ProbRepelSliderεζ);
		if (numberSliderη > 0) interaction(&epsilon, &eta, AttractPowerSliderεη, RepelPowerSliderεη, AttractDistanceSliderεη, RepelDistanceSliderεη, ViscosityAttractSliderεη, ViscosityRepelSliderεη, ProbAttractSliderεη, ProbRepelSliderεη);
		if (numberSliderθ > 0) interaction(&epsilon, &teta, AttractPowerSliderεθ, RepelPowerSliderεθ, AttractDistanceSliderεθ, RepelDistanceSliderεθ, ViscosityAttractSliderεθ, ViscosityRepelSliderεθ, ProbAttractSliderεθ, ProbRepelSliderεθ);
	}

	if (numberSliderζ > 0)
	{
		interaction(&zeta, &zeta, AttractPowerSliderζζ, RepelPowerSliderζζ, AttractDistanceSliderζζ, RepelDistanceSliderζζ, ViscosityAttractSliderζζ, ViscosityRepelSliderζζ, ProbAttractSliderζζ, ProbRepelSliderζζ);
		if (numberSliderα > 0) interaction(&zeta, &alpha, AttractPowerSliderζα, RepelPowerSliderζα, AttractDistanceSliderζα, RepelDistanceSliderζα, ViscosityAttractSliderζα, ViscosityRepelSliderζα, ProbAttractSliderζα, ProbRepelSliderζα);
		if (numberSliderβ > 0) interaction(&zeta, &betha, AttractPowerSliderζβ, RepelPowerSliderζβ, AttractDistanceSliderζβ, RepelDistanceSliderζβ, ViscosityAttractSliderζβ, ViscosityRepelSliderζβ, ProbAttractSliderζβ, ProbRepelSliderζβ);
		if (numberSliderγ > 0) interaction(&zeta, &gamma, AttractPowerSliderζγ, RepelPowerSliderζγ, AttractDistanceSliderζγ, RepelDistanceSliderζγ, ViscosityAttractSliderζγ, ViscosityRepelSliderζγ, ProbAttractSliderζγ, ProbRepelSliderζγ);
		if (numberSliderδ > 0) interaction(&zeta, &elta, AttractPowerSliderζδ, RepelPowerSliderζδ, AttractDistanceSliderζδ, RepelDistanceSliderζδ, ViscosityAttractSliderζδ, ViscosityRepelSliderζδ, ProbAttractSliderζδ, ProbRepelSliderζδ);
		if (numberSliderε > 0) interaction(&zeta, &epsilon, AttractPowerSliderζε, RepelPowerSliderζε, AttractDistanceSliderζε, RepelDistanceSliderζε, ViscosityAttractSliderζε, ViscosityRepelSliderζε, ProbAttractSliderζε, ProbRepelSliderζε);
		if (numberSliderη > 0) interaction(&zeta, &eta, AttractPowerSliderζη, RepelPowerSliderζη, AttractDistanceSliderζη, RepelDistanceSliderζη, ViscosityAttractSliderζη, ViscosityRepelSliderζη, ProbAttractSliderζη, ProbRepelSliderζη);
		if (numberSliderθ > 0) interaction(&zeta, &teta, AttractPowerSliderζθ, RepelPowerSliderζθ, AttractDistanceSliderζθ, RepelDistanceSliderζθ, ViscosityAttractSliderζθ, ViscosityRepelSliderζθ, ProbAttractSliderζθ, ProbRepelSliderζθ);
	}

	if (numberSliderη > 0)
	{
		interaction(&eta, &eta, AttractPowerSliderηη, RepelPowerSliderηη, AttractDistanceSliderηη, RepelDistanceSliderηη, ViscosityAttractSliderηη, ViscosityRepelSliderηη, ProbAttractSliderηη, ProbRepelSliderηη);
		if (numberSliderα > 0) interaction(&eta, &alpha, AttractPowerSliderηα, RepelPowerSliderηα, AttractDistanceSliderηα, RepelDistanceSliderηα, ViscosityAttractSliderηα, ViscosityRepelSliderηα, ProbAttractSliderηα, ProbRepelSliderηα);
		if (numberSliderβ > 0) interaction(&eta, &betha, AttractPowerSliderηβ, RepelPowerSliderηβ, AttractDistanceSliderηβ, RepelDistanceSliderηβ, ViscosityAttractSliderηβ, ViscosityRepelSliderηβ, ProbAttractSliderηβ, ProbRepelSliderηβ);
		if (numberSliderγ > 0) interaction(&eta, &gamma, AttractPowerSliderηγ, RepelPowerSliderηγ, AttractDistanceSliderηγ, RepelDistanceSliderηγ, ViscosityAttractSliderηγ, ViscosityRepelSliderηγ, ProbAttractSliderηγ, ProbRepelSliderηγ);
		if (numberSliderδ > 0) interaction(&eta, &elta, AttractPowerSliderηδ, RepelPowerSliderηδ, AttractDistanceSliderηδ, RepelDistanceSliderηδ, ViscosityAttractSliderηδ, ViscosityRepelSliderηδ, ProbAttractSliderηδ, ProbRepelSliderηδ);
		if (numberSliderε > 0) interaction(&eta, &epsilon, AttractPowerSliderηε, RepelPowerSliderηε, AttractDistanceSliderηε, RepelDistanceSliderηε, ViscosityAttractSliderηε, ViscosityRepelSliderηε, ProbAttractSliderηε, ProbRepelSliderηε);
		if (numberSliderζ > 0) interaction(&eta, &zeta, AttractPowerSliderηζ, RepelPowerSliderηζ, AttractDistanceSliderηζ, RepelDistanceSliderηζ, ViscosityAttractSliderηζ, ViscosityRepelSliderηζ, ProbAttractSliderηζ, ProbRepelSliderηζ);
		if (numberSliderθ > 0) interaction(&eta, &teta, AttractPowerSliderηθ, RepelPowerSliderηθ, AttractDistanceSliderηθ, RepelDistanceSliderηθ, ViscosityAttractSliderηθ, ViscosityRepelSliderηθ, ProbAttractSliderηθ, ProbRepelSliderηθ);
	}

	if (numberSliderθ > 0)
	{
		interaction(&teta, &teta, AttractPowerSliderθθ, RepelPowerSliderθθ, AttractDistanceSliderθθ, RepelDistanceSliderθθ, ViscosityAttractSliderθθ, ViscosityRepelSliderθθ, ProbAttractSliderθθ, ProbRepelSliderθθ);
		if (numberSliderα > 0) interaction(&teta, &alpha, AttractPowerSliderθα, RepelPowerSliderθα, AttractDistanceSliderθα, RepelDistanceSliderθα, ViscosityAttractSliderθα, ViscosityRepelSliderθα, ProbAttractSliderθα, ProbRepelSliderθα);
		if (numberSliderβ > 0) interaction(&teta, &betha, AttractPowerSliderθβ, RepelPowerSliderθβ, AttractDistanceSliderθβ, RepelDistanceSliderθβ, ViscosityAttractSliderθβ, ViscosityRepelSliderθβ, ProbAttractSliderθβ, ProbRepelSliderθβ);
		if (numberSliderγ > 0) interaction(&teta, &gamma, AttractPowerSliderθγ, RepelPowerSliderθγ, AttractDistanceSliderθγ, RepelDistanceSliderθγ, ViscosityAttractSliderθγ, ViscosityRepelSliderθγ, ProbAttractSliderθγ, ProbRepelSliderθγ);
		if (numberSliderδ > 0) interaction(&teta, &elta, AttractPowerSliderθδ, RepelPowerSliderθδ, AttractDistanceSliderθδ, RepelDistanceSliderθδ, ViscosityAttractSliderθδ, ViscosityRepelSliderθδ, ProbAttractSliderθδ, ProbRepelSliderθδ);
		if (numberSliderε > 0) interaction(&teta, &epsilon, AttractPowerSliderθε, RepelPowerSliderθε, AttractDistanceSliderθε, RepelDistanceSliderθε, ViscosityAttractSliderθε, ViscosityRepelSliderθε, ProbAttractSliderθε, ProbRepelSliderθε);
		if (numberSliderζ > 0) interaction(&teta, &zeta, AttractPowerSliderθζ, RepelPowerSliderθζ, AttractDistanceSliderθζ, RepelDistanceSliderθζ, ViscosityAttractSliderθζ, ViscosityRepelSliderθζ, ProbAttractSliderθζ, ProbRepelSliderθζ);
		if (numberSliderη > 0) interaction(&teta, &eta, AttractPowerSliderθη, RepelPowerSliderθη, AttractDistanceSliderθη, RepelDistanceSliderθη, ViscosityAttractSliderθη, ViscosityRepelSliderθη, ProbAttractSliderθη, ProbRepelSliderθη);
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
		if (freezeButton)
		{
			freeze();
		}
		if (randomGeneral)
		{
			random();
			restart();
		}
		if (randomRelations)
		{
			rndrel();
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
			if (key == 'a')
			{
				rndrel();
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
			if (key == 'r')
			{
				restart();
			}
			
	}
	
	
		
	
