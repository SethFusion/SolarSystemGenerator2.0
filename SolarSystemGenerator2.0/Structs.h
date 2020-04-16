#include "framework.h"
#include "resource.h"



struct STAR
{
	//generated
	std::wstring name, class_;
	int temperatureK, maxPlanetNumber; //starClassSize;
	double mass, radius, RA[3], DEC[3], distance;

	//calculated
	double luminosity, innerLimit, outerLimit, frostLine, habitZoneInnerLimit, habitZoneOuterLimit;
	std::vector<double> semimajorList, semimajorStaticList; //semimajorOuterList, semimajorInnerList;

	//for weighted moons
	double totalDist;
};

struct PLANET
{
	//generated for planets/moons
	std::wstring planetType, parentBody, name, class_;
	double mass, radius, semimajorAxis, eccentricity, inclination,
		ascendingNode, argofPericenter, meanAnomaly, obliquity;
	std::vector<double> usedSemimajor_moon, usedRadius_moon; //Lists used to check the spacing of moons
	//calculated for planets/moons
	double earthRadius, gravity, density;

	//generated for planets
	int numberOfMajorMoons, numberOfMinorMoons; // for old moon generation
	//calculated for planets
	int majorMoonPercent, minorMoonPercent; // used for weighted moon generation
	double hillSphereOuterLimit, hillSphereInnerLimit; // HSInner used to be for moons

	//exotic stuff
	int debrisCount;
	std::wstring model; // used for ships
	//life stuff
	struct Life
	{
		bool haslife, panspermia;
		std::wstring _class, type, biome;
	};
	Life life_organic, life_exotic;
};
