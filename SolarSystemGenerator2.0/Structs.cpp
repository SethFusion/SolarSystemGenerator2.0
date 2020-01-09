#include "framework.h"
#include "resource.h"
#include <windows.h>



struct STAR
{
	//generated
	std::wstring name, class_;
	int temperatureK, maxPlanetNumber; //starClassSize;
	double mass, radius, RA[3], DEC[3], distance;

	//calculated
	double luminosity, innerLimit, outerLimit, frostLine, habitZoneInnerLimit, habitZoneOuterLimit;
	std::vector<double> semimajorList, semimajorStaticList; //semimajorOuterList, semimajorInnerList;
};

struct PLANET
{
	//generated for planets/moons
	std::wstring planetType, parentBody, name, class_;
	double mass, radius, semimajorAxis, eccentricity, inclination,
		ascendingNode, argofPericenter, meanAnomaly, obliquity;
	std::vector<double> usedSemimajor_moon, usedRadius_moon;
	//calculated for planets/moons
	double earthRadius, gravity, density;

	//generated for planets
	int numberOfMajorMoons, numberOfMinorMoons; //Lists used to check the spacing of moons
	//calculated for planets
	double hillSphereOuterLimit, hillSphereInnerLimit; // HSInner used to be for moons

	//exotic stuff
	int debrisCount;
	bool hasShip;
	std::wstring model; // used for ships
	//life stuff
	struct Life
	{
		bool haslife, panspermia;
		std::wstring _class, type, biome;
	};
	Life life_organic, life_exotic;
};
