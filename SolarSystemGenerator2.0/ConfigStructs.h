#include "framework.h"
#include "resource.h"

	/*--------------------------------------------------------------#
	|																|
	|		Every variable needs a value in the						|
	|		ConfigurationVariables struct, a "var" in the			|
	|		ConfigurationHWNDs struct, and a value in the			|
	|		Presets structure.										|
	|																|
	|		EVERY variable neeeds to be added to these functions,	|
	|		using the proper struct, preferably in this order:		|
	|																|
	|		* LoadPresets - to get the information from the preset	|
	|			files. It needs text inside the preset files		|
	|			to function.										|
	|		* UpdatePreset - to distribute the values across		|
	|			the window handles as text inside the window		|
	|			boxes.												|
	|		* SavePreset - to put the values from the				|
	|			window boxes into preset files.						|
	|		* GetConfigData - to get the current values from		|
	|			the window boxes.									|
	|		* LoadMainScreen - to create the handles for the		|
	|			text to be displayed and edited, along with			|
	|			other static information.							|
	|		* ClearScreen - to hide the window handles.				|
	|		* LoadScreen_____ - to show the window boxes inside		|
	|			the proper group.									|
	|																|
	|		The new variable will also need an entry in the			|
	|		SetInfoBox function, and may need an entry in the		|
	|		SetCheckBoxText function if it is a boolean with		|
	|		Enabled/Disabled displayed next to it. This means		|
	|		every varible will need an IB_ number and may need		|
	|		a CB_ number under Resource.h							|
	|																|
	#--------------------------------------------------------------*/

struct var
{
	// A collection of handles needed for displaying a single variable in
	// the COnfigurationVariables struct

	HWND HANDLE, // a handle to the actual input field
		DESC, // Description; the text displayed next to the input field
		INFOBUTTON, // a handle to the info button displayed next to the input field
		EXTRA; // used for anything else I might need
};

struct ConfigurationVariables
{
	/*--------------------------------------------------------------#
	|																|
	|		This structure only contains variables used				|
	|		by the generator and I/O.								|
	|																|
	#--------------------------------------------------------------*/

	/*#####################################################
			GENERAL
	#####################################################*/

	long long seed;
	int numberOfRuns;
	bool debug;
	wchar_t starOutputFolder[WSIZE], planetOutputFolder[WSIZE];

	/*#####################################################
			SYSTEM
	#####################################################*/

	bool multiStars, smartPlacement, generateDwarfPlanets, generateAsteroidBelt, generateComets;
	int	dwarfPlanetChance, maxAsteroidBelts, minAsteroidCount, maxAsteroidCount, minCometCount, maxCometCount;
	double minDistance, maxDistance, 
		multiStarsAvg, multiStarsSD,
		minPlanetNumber, systemRadiusModifier,
		planetSpaceAvg, planetSpaceSD,
		avgEccentricity, SDEccentricity, 
		avgInclination, SDInclination, 
		avgObliquity, SDObliquity;
	double starClassO, starClassB, starClassA, starClassF, starClassG, 
		starClassK, starClassM, starClassWD, starClassQ, starClassX;

	/*#####################################################
			PLANET
	#####################################################*/

	bool weightedMoons, forceLife, traditionalLife;
	int exotic_OrbitChance, exotic_AxialTiltChance, exotic_CompanionOrbitChance, exotic_DebrisRingChance,
		life_OrganicChance, life_ExoticChance, life_MulticellChance;
	double moonDistanceBoundary;

	/*#####################################################
			SURFACE
	#####################################################*/

	bool generateComposition;

	/*#####################################################
			SPECIAL
	#####################################################*/

	int exotic_ShipChance;
	bool shipsNeedLife;
	wchar_t modelsFolder[WSIZE];

	std::vector<std::wstring> shipList_Starship, shipList_Planetship, shipList_Station, shipList_Satellite, shipList_Probe;

	/*#####################################################
			ADVANCED
	#####################################################*/

	HWND Tab_Advanced_Outer, Tab_Advanced_Inner;
};

struct ConfigurationHWNDs
{
	/*--------------------------------------------------------------#
	|																|
	|		This structure only contains handles used for			|
	|		creating and displaying the UI							|
	|																|
	#--------------------------------------------------------------*/

	//HWND tabH;

	/*#####################################################
		*/	HWND INFO_BOX; /*
		This handles the leftside info panel on the
		screen. When an info button is pressed, it
		re-drawns the info box with new text describing
		what variable the button corresponds to.

	#####################################################*/

	/*#####################################################
		*/	HWND PROGRESS_BAR; /*
		This handles the leftside info panel on the
		screen. When an info button is pressed, it
		re-drawns the info box with new text describing
		what variable the button corresponds to.

#####################################################*/

	/*#####################################################
		*/	HWND HEADER_GENERAL; /*
	#####################################################*/

	var seed, numberOfRuns;
	var debug;
	var starOutputFolder, planetOutputFolder;

	var presetDropDown;
	var savePresetButton;
	var namePresetDropDown;
	// save name button is under advanced

	/*#####################################################
		*/	HWND HEADER_SYSTEM; /*
	#####################################################*/
	
	var multiStars, smartPlacement, generateDwarfPlanets, generateAsteroidBelt, generateComets;
	var dwarfPlanetChance, maxAsteroidBelts, minAsteroidCount, maxAsteroidCount, minCometCount, maxCometCount;
	var minDistance, maxDistance, 
		multiStarsAvg, multiStarsSD,
		minPlanetNumber, systemRadiusModifier,
		planetSpaceAvg, planetSpaceSD,
		avgEccentricity, SDEccentricity, 
		avgInclination, SDInclination, 
		avgObliquity, SDObliquity;	
	var starClassO, starClassB, starClassA, starClassF, starClassG, starClassK, starClassM, starClassWD, starClassQ, starClassX;

	/*#####################################################
		*/	HWND HEADER_PLANET; /*
	#####################################################*/
	
	var  weightedMoons, moonDistanceBoundary;
	var exotic_OrbitChance, exotic_AxialTiltChance, exotic_CompanionOrbitChance, exotic_DebrisRingChance;
	var life_OrganicChance, life_ExoticChance, life_MulticellChance;	
	var forceLife, traditionalLife;

	/*#####################################################
		*/	HWND HEADER_SURFACE; /*
	#####################################################*/

		var generateComposition;

	/*#####################################################
		*/	HWND HEADER_SPECIAL; /*
	#####################################################*/

		var exotic_ShipChance;
		var shipsNeedLife;
		var	modelsFolder;

	/*#####################################################
		*/	HWND HEADER_ADVANCED; /*
	#####################################################*/

	/*=====================================================
		*/	 /*
		buttons to load different screens for name
		variables
	=====================================================*/
};

struct Preset
{
	wchar_t name[WSIZE];

	/*#####################################################
		General
	#####################################################*/

	long long seed;
	int numberOfRuns;
	bool debug;
	wchar_t starOutputFolder[WSIZE], planetOutputFolder[WSIZE];

	/*#####################################################
		System
	#####################################################*/

	bool multiStars, smartPlacement, generateDwarfPlanets, generateAsteroidBelt, generateComets;
	wchar_t const* multiStarsState, * smartPlacementState, * generateDwarfPlanetsState, * generateAsteroidBeltState, * generateCometsState;
	int	dwarfPlanetChance, maxAsteroidBelts, minAsteroidCount, maxAsteroidCount, minCometCount, maxCometCount;
	double minDistance, maxDistance, 
		multiStarsAvg, multiStarsSD,
		minPlanetNumber, systemRadiusModifier,
		planetSpaceAvg, planetSpaceSD,
		avgEccentricity, SDEccentricity, 
		avgInclination, SDInclination, 
		avgObliquity, SDObliquity;
	double starClassO, starClassB, starClassA, starClassF, starClassG, starClassK, starClassM, starClassWD, starClassQ, starClassX; // now represents weight

	/*#####################################################
		Planet
	#####################################################*/

	bool weightedMoons, forceLife, traditionalLife;
	wchar_t const* weightedMoonsState;
	int exotic_OrbitChance, exotic_AxialTiltChance, exotic_CompanionOrbitChance, exotic_DebrisRingChance,
		life_OrganicChance, life_ExoticChance, life_MulticellChance;
	double moonDistanceBoundary;
	wchar_t const* forceLifeState, const* traditonalLifeState;

	/*#####################################################
		Surface
	#####################################################*/

	bool generateComposition;
	wchar_t const* generateCompositionState;

	/*#####################################################
		Special
	#####################################################*/

	int exotic_ShipChance;
	bool shipsNeedLife;
	wchar_t const* shipsNeedLifeState;
	wchar_t modelsFolder[WSIZE];

	/*#####################################################
		Advanced
	#####################################################*/
};

struct NameVariables
{
	/*=====================================================
		*/	HWND Tab_Name_Inner, Tab_Ship_Inner;	/*
			The tab handles display the list of tabs
		the user can select, each tab loading
		a differnt screen a variables to edit
	=====================================================*/
	
	/*=====================================================
		*/	std::unordered_set<std::wstring> usedNames;	/*
			This map ensures every name is unique. 
		The wstring is the key, which is hashed into
		a long long. 
	=====================================================*/

	/*=====================================================
		*/	var PreMod_INFO, PostMod_INFO, NumberMod_INFO, 
			PreMods_List, PostMods_List;	/*
			These info buttons appear for all name groups
		because they remain true for each type of group.
		They are loaded in with parent hWnd, not using
		the group handle because when clicked, it needs
		to send a message to the window procedure.

		Vissually, their position is inside the group. 
	=====================================================*/

	//#####################################################
		#pragma region Star Names
		/*=====================================================
			*/	HWND GROUP_STAR;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useStarPreMods, useStarPostMods, useStarNumberMods;
		HWND useStarPreModsH, useStarPostModsH, useStarNumberModsH; // holds the checkboxs
		HWND useStarModsDESC; // static handle to hold the description

		int probStarPreMod, probStarPostMod, probStarNumberMod;
		HWND probStarPreModH, probStarPostModH, probStarNumberModH; // holds the checkboxs
		HWND starModsProbDESC; // static handle to hold description

		HWND starPreModList, starPostModList; // edit handles to hold the lists
		std::vector<std::wstring> StarPreMods, StarPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Planet Names
		/*=====================================================
			*/	HWND GROUP_PLANET;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool usePlanetPreMods, usePlanetPostMods, usePlanetNumberMods;
		HWND usePlanetPreModsH, usePlanetPostModsH, usePlanetNumberModsH; // holds the checkboxs
		HWND usePlanetModsDESC; // static handle to hold the description

		int probPlanetPreMod, probPlanetPostMod, probPlanetNumberMod;
		HWND probPlanetPreModH, probPlanetPostModH, probPlanetNumberModH; // holds the checkboxs
		HWND planetModsProbDESC; // static handle to hold description

		HWND planetPreModList, planetPostModList; // edit handles to hold the lists
		std::vector<std::wstring> PlanetPreMods, PlanetPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Moon Names
		/*=====================================================
			*/	HWND GROUP_MOON;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool nameTerraMoons, nameGasMoons;
		var nameMoonsH; // DESC and INFOBOX used normally, HANDLE and EXTRA are used for checkboxes.

		bool useMoonPreMods, useMoonPostMods, useMoonNumberMods;
		HWND useMoonPreModsH, useMoonPostModsH, useMoonNumberModsH; // holds the checkboxs
		HWND useMoonModsDESC; // static handle to hold the description

		int probMoonPreMod, probMoonPostMod, probMoonNumberMod;
		HWND probMoonPreModH, probMoonPostModH, probMoonNumberModH; // holds the checkboxs
		HWND moonModsProbDESC; // static handle to hold description

		HWND moonPreModList, moonPostModList; // edit handles to hold the lists
		std::vector<std::wstring> MoonPreMods, MoonPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region DwarfMoon Names
		/*=====================================================
			*/	HWND GROUP_DWARFMOON;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool nameTerraDwarfMoons, nameGasDwarfMoons;
		var nameDwarfMoonsH; // DESC and INFOBOX used normally, HANDLE and EXTRA are used for checkboxes.

		bool useDwarfMoonPreMods, useDwarfMoonPostMods, useDwarfMoonNumberMods;
		HWND useDwarfMoonPreModsH, useDwarfMoonPostModsH, useDwarfMoonNumberModsH; // holds the checkboxs
		HWND useDwarfMoonModsDESC; // static handle to hold the description

		int probDwarfMoonPreMod, probDwarfMoonPostMod, probDwarfMoonNumberMod;
		HWND probDwarfMoonPreModH, probDwarfMoonPostModH, probDwarfMoonNumberModH; // holds the checkboxs
		HWND dwarfMoonModsProbDESC; // static handle to hold description

		HWND dwarfMoonPreModList, dwarfMoonPostModList; // edit handles to hold the lists
		std::vector<std::wstring> DwarfMoonPreMods, DwarfMoonPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Asteroid Names
		/*=====================================================
			*/	HWND GROUP_ASTEROID;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool nameAsteroids;
		var nameAsteroidsH; // DESC and INFOBOX used normally, HANDLE used for checkbox.

		bool useAsteroidPreMods, useAsteroidPostMods, useAsteroidNumberMods;
		HWND useAsteroidPreModsH, useAsteroidPostModsH, useAsteroidNumberModsH; // holds the checkboxs
		HWND useAsteroidModsDESC; // static handle to hold the description

		int probAsteroidPreMod, probAsteroidPostMod, probAsteroidNumberMod;
		HWND probAsteroidPreModH, probAsteroidPostModH, probAsteroidNumberModH; // holds the checkboxs
		HWND asteroidModsProbDESC; // static handle to hold description

		HWND asteroidPreModList, asteroidPostModList; // edit handles to hold the lists
		std::vector<std::wstring> AsteroidPreMods, AsteroidPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Comet Names
		/*=====================================================
			*/	HWND GROUP_COMET;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool nameComets;
		var nameCometsH; // DESC and INFOBOX used normally, HANDLE used for checkbox.

		bool useCometPreMods, useCometPostMods, useCometNumberMods;
		HWND useCometPreModsH, useCometPostModsH, useCometNumberModsH; // holds the checkboxs
		HWND useCometModsDESC; // static handle to hold the description

		int probCometPreMod, probCometPostMod, probCometNumberMod;
		HWND probCometPreModH, probCometPostModH, probCometNumberModH; // holds the checkboxs
		HWND cometModsProbDESC; // static handle to hold description

		HWND cometPreModList, cometPostModList; // edit handles to hold the lists
		std::vector<std::wstring> CometPreMods, CometPostMods;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship All Names
		/*=====================================================
			*/	HWND GROUP_SHIP_ALL;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_All, useShipPostMods_All;
		HWND useShipPreMods_AllH, useShipPostMods_AllH;
		HWND useShipMods_All_DESC;

		int probShipPreMod_All, probShipPostMod_All;
		HWND probShipPreMod_AllH, probShipPostMod_AllH;
		HWND shipModsProb_All_DESC;

		HWND shipPreModList_All, shipPostModList_All;
		std::vector<std::wstring> ShipPreMods_All, ShipPostMods_All;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship Starship Names
		/*=====================================================
			*/	HWND GROUP_SHIP_STARSHIP;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_Starship, useShipPostMods_Starship, useShipNumberMods_Starship;
		HWND useShipPreMods_StarshipH, useShipPostMods_StarshipH, useShipNumberMods_StarshipH;
		HWND useShipMods_Starship_DESC;

		int probShipPreMod_Starship, probShipPostMod_Starship, probShipNumberMod_Starship;
		HWND probShipPreMod_StarshipH, probShipPostMod_StarshipH, probShipNumberMod_StarshipH;
		HWND shipModsProb_Starship_DESC;

		HWND shipPreModList_Starship, shipPostModList_Starship;
		std::vector<std::wstring> ShipPreMods_Starship, ShipPostMods_Starship;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship Planetship Names
		/*=====================================================
			*/	HWND GROUP_SHIP_PLANETSHIP;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_Planetship, useShipPostMods_Planetship, useShipNumberMods_Planetship;
		HWND useShipPreMods_PlanetshipH, useShipPostMods_PlanetshipH, useShipNumberMods_PlanetshipH;
		HWND useShipMods_Planetship_DESC;

		int probShipPreMod_Planetship, probShipPostMod_Planetship, probShipNumberMod_Planetship;
		HWND probShipPreMod_PlanetshipH, probShipPostMod_PlanetshipH, probShipNumberMod_PlanetshipH;
		HWND shipModsProb_Planetship_DESC;

		HWND shipPreModList_Planetship, shipPostModList_Planetship;
		std::vector<std::wstring> ShipPreMods_Planetship, ShipPostMods_Planetship;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship Station Names
		/*=====================================================
			*/	HWND GROUP_SHIP_STATION;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_Station, useShipPostMods_Station, useShipNumberMods_Station;
		HWND useShipPreMods_StationH, useShipPostMods_StationH, useShipNumberMods_StationH;
		HWND useShipMods_Station_DESC;

		int probShipPreMod_Station, probShipPostMod_Station, probShipNumberMod_Station;
		HWND probShipPreMod_StationH, probShipPostMod_StationH, probShipNumberMod_StationH;
		HWND shipModsProb_Station_DESC;

		HWND shipPreModList_Station, shipPostModList_Station;
		std::vector<std::wstring> ShipPreMods_Station, ShipPostMods_Station;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship Satellite Names
		/*=====================================================
			*/	HWND GROUP_SHIP_SATELLITE;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_Satellite, useShipPostMods_Satellite, useShipNumberMods_Satellite;
		HWND useShipPreMods_SatelliteH, useShipPostMods_SatelliteH, useShipNumberMods_SatelliteH;
		HWND useShipMods_Satellite_DESC;

		int probShipPreMod_Satellite, probShipPostMod_Satellite, probShipNumberMod_Satellite;
		HWND probShipPreMod_SatelliteH, probShipPostMod_SatelliteH, probShipNumberMod_SatelliteH;
		HWND shipModsProb_Satellite_DESC;

		HWND shipPreModList_Satellite, shipPostModList_Satellite;
		std::vector<std::wstring> ShipPreMods_Satellite, ShipPostMods_Satellite;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Ship Probe Names
		/*=====================================================
			*/	HWND GROUP_SHIP_PROBE;	/*
				Groupbox holding all variables below, making
			showing and hiding them all at once super easy
		=====================================================*/

		bool useShipPreMods_Probe, useShipPostMods_Probe, useShipNumberMods_Probe;
		HWND useShipPreMods_ProbeH, useShipPostMods_ProbeH, useShipNumberMods_ProbeH;
		HWND useShipMods_Probe_DESC;

		int probShipPreMod_Probe, probShipPostMod_Probe, probShipNumberMod_Probe;
		HWND probShipPreMod_ProbeH, probShipPostMod_ProbeH, probShipNumberMod_ProbeH;
		HWND shipModsProb_Probe_DESC;

		HWND shipPreModList_Probe, shipPostModList_Probe;
		std::vector<std::wstring> ShipPreMods_Probe, ShipPostMods_Probe;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Dataset Stuff
		/*=====================================================
		*/	HWND GROUP_DATASET;	/*
			Groupbox holding all variables below, making
		showing and hiding them all at once super easy
		=====================================================*/

		HWND Markov_INFO;
		var saveNamePresetButton;
		HWND buttonUpdate;

		int order, wordVarience, max_length, min_length, wordPercent;
		HWND orderH, wordVarienceH, max_lengthH, min_lengthH;
		HWND orderDESC, wordVarienceDESC, max_lengthDESC, min_lengthDESC;
		var wordPercentH; // Extra is used for the test button

		HWND Markov_RawDatasetH, Markov_RawDatasetDESC;
		std::vector<std::wstring> Markov_RawDataset;

		struct NGRAMS
		{
			std::vector<std::wstring> ngrams;
			std::vector<std::wstring> nextCharList;
		};
		NGRAMS main_ngrams, twogram_list;

		#pragma endregion
	//#####################################################

	//#####################################################
		#pragma region Simple Generator Stuff
		/*=====================================================
		*/	HWND GROUP_SIMPLE;	/*
			Groupbox holding all variables below, making
		showing and hiding them all at once super easy
		=====================================================*/

		HWND SimpleGenerator_INFO;

		bool useSimpleGenerator;
		HWND useSimpleGeneratorDESC, useSimpleGeneratorH;

		HWND PrefixListDESC, PrefixListH, SuffixListDESC, SuffixListH;
		std::vector<std::wstring> PrefixList, SuffixList;

		#pragma endregion
//#####################################################
};

struct NamePreset
{
	wchar_t name[WSIZE];

	int wordPercent;

	//Star
	bool useStarPreMods, useStarPostMods, useStarNumberMods;
	int probStarPreMod, probStarPostMod, probStarNumberMod;
	wchar_t StarPreMods[MODLIST_SIZE], StarPostMods[MODLIST_SIZE];

	//Planet
	bool usePlanetPreMods, usePlanetPostMods, usePlanetNumberMods;
	int probPlanetPreMod, probPlanetPostMod, probPlanetNumberMod;
	wchar_t PlanetPreMods[MODLIST_SIZE], PlanetPostMods[MODLIST_SIZE];

	//Moon
	bool nameTerraMoons, nameGasMoons;
	bool useMoonPreMods, useMoonPostMods, useMoonNumberMods;
	int probMoonPreMod, probMoonPostMod, probMoonNumberMod;
	wchar_t MoonPreMods[MODLIST_SIZE], MoonPostMods[MODLIST_SIZE];

	//Dwarf Moon
	bool nameTerraDwarfMoons, nameGasDwarfMoons;
	bool useDwarfMoonPreMods, useDwarfMoonPostMods, useDwarfMoonNumberMods;
	int probDwarfMoonPreMod, probDwarfMoonPostMod, probDwarfMoonNumberMod;
	wchar_t DwarfMoonPreMods[MODLIST_SIZE], DwarfMoonPostMods[MODLIST_SIZE];

	//Asteroid
	bool nameAsteroids;
	bool useAsteroidPreMods, useAsteroidPostMods, useAsteroidNumberMods;
	int probAsteroidPreMod, probAsteroidPostMod, probAsteroidNumberMod;
	wchar_t AsteroidPreMods[MODLIST_SIZE], AsteroidPostMods[MODLIST_SIZE];

	//Comet
	bool nameComets;
	bool useCometPreMods, useCometPostMods, useCometNumberMods;
	int probCometPreMod, probCometPostMod, probCometNumberMod;
	wchar_t CometPreMods[MODLIST_SIZE], CometPostMods[MODLIST_SIZE];

	//Ship All
	bool useShipPreMods_All, useShipPostMods_All;
	int probShipPreMod_All, probShipPostMod_All;
	wchar_t ShipPreMods_All[MODLIST_SIZE], ShipPostMods_All[MODLIST_SIZE];

	//Ship Starship
	bool useShipPreMods_Starship, useShipPostMods_Starship, useShipNumberMods_Starship;
	int probShipPreMod_Starship, probShipPostMod_Starship, probShipNumberMod_Starship;
	wchar_t ShipPreMods_Starship[MODLIST_SIZE], ShipPostMods_Starship[MODLIST_SIZE];

	//Ship Planetship
	bool useShipPreMods_Planetship, useShipPostMods_Planetship, useShipNumberMods_Planetship;
	int probShipPreMod_Planetship, probShipPostMod_Planetship, probShipNumberMod_Planetship;
	wchar_t ShipPreMods_Planetship[MODLIST_SIZE], ShipPostMods_Planetship[MODLIST_SIZE];

	//Ship Station
	bool useShipPreMods_Station, useShipPostMods_Station, useShipNumberMods_Station;
	int probShipPreMod_Station, probShipPostMod_Station, probShipNumberMod_Station;
	wchar_t ShipPreMods_Station[MODLIST_SIZE], ShipPostMods_Station[MODLIST_SIZE];

	//Ship Satellite
	bool useShipPreMods_Satellite, useShipPostMods_Satellite, useShipNumberMods_Satellite;
	int probShipPreMod_Satellite, probShipPostMod_Satellite, probShipNumberMod_Satellite;
	wchar_t ShipPreMods_Satellite[MODLIST_SIZE], ShipPostMods_Satellite[MODLIST_SIZE];

	//Ship Probe
	bool useShipPreMods_Probe, useShipPostMods_Probe, useShipNumberMods_Probe;
	int probShipPreMod_Probe, probShipPostMod_Probe, probShipNumberMod_Probe;
	wchar_t ShipPreMods_Probe[MODLIST_SIZE], ShipPostMods_Probe[MODLIST_SIZE];

	//Dataset
	int order, wordVarience, max_length, min_length;
	wchar_t Markov_RawDataset[DATASET_SIZE];

	//Simple Generator
	bool useSimpleGenerator;
	wchar_t PrefixList[MODLIST_SIZE], SuffixList[MODLIST_SIZE];
};