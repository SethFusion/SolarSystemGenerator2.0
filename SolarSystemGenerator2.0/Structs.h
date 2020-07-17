#include "framework.h"
#include "resource.h"

struct Interior
{
	double hydrogen, helium, silicates, carbides, ices, metals;
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

class SEObject
{
public:
	std::wstring name, class_;

	SEObject* parentBody;
	double mass, radius, semimajorAxis, period, eccentricity, inclination,
		ascendingNode, argofPericenter, meanAnomaly, obliquity;
	double earthRadius, gravity, density;

	SEObject()
	{
		name = class_ = L"-empty-";
		parentBody = NULL;
		mass = radius = semimajorAxis = period = eccentricity = inclination =
			ascendingNode = argofPericenter = meanAnomaly = obliquity = -1;
		earthRadius = gravity = density = -1;
	}
};

class SEStar: public SEObject
{
public:
	//generated
	int temperatureK, maxPlanetNumber;
	double RA[3], DEC[3], distance;

	//calculated
	double luminosity, innerLimit, outerLimit, frostLine, habitZoneInnerLimit, habitZoneOuterLimit;
	std::vector<double> semimajorList, semimajorStaticList;

	//for weighted moons
	double totalDist;

	SEStar()
	{
		temperatureK = maxPlanetNumber = -1;
		RA[0] = RA[1] = RA[2] = DEC[0] = DEC[1] = DEC[2] = distance = -1;
		luminosity = innerLimit = outerLimit = frostLine = habitZoneInnerLimit = habitZoneOuterLimit =  totalDist = -1;
	}
};

class SEPlanet : public SEObject
{
public:
	//generated for planets/moons
	std::wstring type;
	std::vector<double> usedSemimajor_moon, usedRadius_moon; //Lists used to check the spacing of moons

	//generated for planets
	int numberOfMajorMoons, numberOfMinorMoons; // for old moon generation
	//calculated for planets
	int majorMoonPercent, minorMoonPercent; // used for weighted moon generation
	double hillSphereOuterLimit, hillSphereInnerLimit; // HSInner used to be for moons

	Interior interior;
	Atmosphere atmosphere;
	Life life_organic, life_exotic;

	//exotic stuff
	bool hasCompanionOrbit;
	int debrisCount;
	
	SEPlanet()
	{
		type = L"-empty-";
		numberOfMajorMoons = numberOfMinorMoons = majorMoonPercent = minorMoonPercent = -1;
		hillSphereInnerLimit = hillSphereOuterLimit = -1;
		hasCompanionOrbit = false;
		debrisCount = -1;
		
		atmosphere.height = atmosphere.pressure = -1;
		interior.hydrogen = interior.helium = interior.silicates = interior.carbides = interior.ices = interior.metals = 0;
		life_organic.haslife = life_organic.panspermia = life_exotic.haslife = life_exotic.panspermia = false;
	}
};

class SEShip : public SEObject
{
public:
	std::wstring type, model;

	SEShip()
	{
		type = model = L"-empty-";
	}
};
