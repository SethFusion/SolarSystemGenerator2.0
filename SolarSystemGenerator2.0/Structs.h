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

struct Atmosphere
{
	enum AtmoModel
	{
		None, Biogenic, Chlorine, Earth, Ethereal, Jupiter, Mars, Neptune, Pluto, Thick, Thin, Titan, Venus
	};

	enum Gas
	{
		H2,		// Hydrogen
		He,		// Helium
		Ne,		// Neon
		Ar,		// Argon
		Kr,		// Krypton
		Xe,		// Xenon
		O2,		// Oxygen
		N2,		// Nitrogen
		CO,		// Carbon Monoxide
		CO2,	// Carbon Dioxide
		SO,		// Sulfur Monoxide
		SO2,	// Sulfur Dioxide
		Cl2,	// Chlorine
		NaCl,	// Sodium Chloride / Salt
		H2S,	// Hydrogen Sulfide / Hydrosulfuric Acid
		H2O,	// Hydrogen Dioxide / Water
		NH3,	// Ammonia
		CH4,	// Methane
		C2H2,	// Acetylene / Ethyne
		C2H4,	// Ethylene / Ethene
		C2H6,	// Ethane
		C3H8	// Propane
	};

	AtmoModel model;
	double height, pressure;
	std::vector<std::pair<Gas, double>> composition;
};

struct Life
{
	bool haslife, panspermia;
	std::wstring _class, type, biome;
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

	Atmosphere atmosphere;
	Life life_organic, life_exotic;

	//exotic stuff
	bool hasCompanionOrbit;
	int debrisCount;
	std::wstring model; // used for ships
};

