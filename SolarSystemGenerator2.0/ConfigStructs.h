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

	bool smartPlacement, generateDwarfPlanets;
	int	minPlanetNumber, dwarfPlanetChance;
	double minDistance, maxDistance, planetSpacing,
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

	

	/*#####################################################
			SPECIAL
	#####################################################*/

	int exotic_ShipChance;
	bool shipsNeedLife;
	wchar_t modelsFolder[WSIZE];

	std::vector<std::wstring> shipList_Colony, shipList_Instrument, shipList_Satellite, shipList_Station;

	/*#####################################################
			ADVANCED
	#####################################################*/

	// empty for now

};

struct var
{
	// A collection of handles needed for displaying a single variable in
	// the COnfigurationVariables struct

	HWND HANDLE, // a handle to the actual input field
		DESC, // Description; the text displayed next to the input field
		INFOBUTTON, // a handle to the info button displayed next to the input field
		EXTRA; // used for anything else I might need
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
	
	var smartPlacement, generateDwarfPlanets;
	var minPlanetNumber, dwarfPlanetChance;	
	var minDistance, maxDistance, planetSpacing,
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

	/*#####################################################
		*/	HWND HEADER_SPECIAL; /*
	#####################################################*/

		var exotic_ShipChance;
		var shipsNeedLife;
		var	modelsFolder;

	/*#####################################################
		*/	HWND HEADER_ADVANCED; /*
	#####################################################*/

	var saveNamePresetButton;

	/*=====================================================
		*/	var advNameGroup;
			HWND buttonStar, 
			buttonPlanet, 
			buttonMoon, 
			buttonDwarfMoon, 
			buttonShip, 
			buttonDataset,
			buttonSimple,
			buttonUpdate; /*
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
	wchar_t const* debugState;
	wchar_t starOutputFolder[WSIZE], planetOutputFolder[WSIZE];

	/*#####################################################
		System
	#####################################################*/

	bool smartPlacement, generateDwarfPlanets;
	wchar_t const* smartPlacementState, * generateDwarfPlanetsState;
	int	minPlanetNumber, dwarfPlanetChance;
	double minDistance, maxDistance, planetSpacing,
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
	int wordPercent;
	var wordPercentH; // Extra is used for the test button

	/*#####################################################
	*/	HWND GROUP_SIMPLE; /*
	#####################################################*/

	HWND SimpleGenerator_INFO;

	bool useSimpleGenerator;
	HWND useSimpleGeneratorDESC, useSimpleGeneratorH;

	HWND PrefixListDESC, PrefixListH, SuffixListDESC, SuffixListH;
	std::vector<std::wstring> PrefixList, SuffixList;


	/*=====================================================
		*/	std::vector<std::wstring> usedNames;	/*
		This vector fills up with names as the program
		generates them. Every time a new name is
		generated, it checks the vector to see if that
		name has already been used. This ensures every
		name is unique.
	=====================================================*/

	/*=====================================================
		*/	var PreMod_INFO,
			PostMod_INFO,
			NumberMod_INFO, 
			PreMods_List, 
			PostMods_List;	/*
		These info buttons appear for all name groups
		because they remain true for each type of group.
		They are loaded in with parent hWnd, not using
		the group handle because when clicked, it needs
		to send a message to the window procedure.

		Vissually, their position is inside the group. 
	=====================================================*/

	/*#####################################################
		*/	HWND GROUP_STAR; /*
	#####################################################*/

	bool useStarPreMods, useStarPostMods, useStarNumberMods;
	HWND useStarPreModsH, useStarPostModsH, useStarNumberModsH; // holds the checkboxs
	HWND useStarModsDESC; // static handle to hold the description
	
	int probStarPreMod, probStarPostMod, probStarNumberMod;
	HWND probStarPreModH, probStarPostModH, probStarNumberModH; // holds the checkboxs
	HWND starModsProbDESC; // static handle to hold description

	HWND starPreModList, starPostModList; // edit handles to hold the lists
	std::vector<std::wstring> StarPreMods, StarPostMods;

	/*#####################################################
		*/	HWND GROUP_PLANET; /*
	#####################################################*/

	bool usePlanetPreMods, usePlanetPostMods, usePlanetNumberMods;
	HWND usePlanetPreModsH, usePlanetPostModsH, usePlanetNumberModsH; // holds the checkboxs
	HWND usePlanetModsDESC; // static handle to hold the description

	int probPlanetPreMod, probPlanetPostMod, probPlanetNumberMod;
	HWND probPlanetPreModH, probPlanetPostModH, probPlanetNumberModH; // holds the checkboxs
	HWND planetModsProbDESC; // static handle to hold description

	HWND planetPreModList, planetPostModList; // edit handles to hold the lists
	std::vector<std::wstring> PlanetPreMods, PlanetPostMods;

	/*#####################################################
		*/	HWND GROUP_MOON; /*
	#####################################################*/

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

	/*#####################################################
		*/	HWND GROUP_DWARFMOON; /*
	#####################################################*/

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

	/*#####################################################
		*/	HWND GROUP_ALL_SHIP; /*
	#####################################################*/
		
	bool useShipPreMods_All, useShipPostMods_All;
	HWND useShipPreMods_AllH, useShipPostMods_AllH;
	HWND useShipMods_All_DESC;

	int probShipPreMod_All, probShipPostMod_All;
	HWND probShipPreMod_AllH, probShipPostMod_AllH;
	HWND shipModsProb_All_DESC;

	HWND shipPreModList_All, shipPostModList_All;
	std::vector<std::wstring> ShipPreMods_All, ShipPostMods_All;

	/*#####################################################
		*/	HWND GROUP_COLONY_SHIP, 
				BUTTON_COLONY; /*
	#####################################################*/

	bool useShipPreMods_Colony, useShipPostMods_Colony, useShipNumberMods_Colony;
	HWND useShipPreMods_ColonyH, useShipPostMods_ColonyH, useShipNumberMods_ColonyH;
	HWND useShipMods_Colony_DESC;

	int probShipPreMod_Colony, probShipPostMod_Colony, probShipNumberMod_Colony;
	HWND probShipPreMod_ColonyH, probShipPostMod_ColonyH, probShipNumberMod_ColonyH;
	HWND shipModsProb_Colony_DESC;

	HWND shipPreModList_Colony, shipPostModList_Colony;
	std::vector<std::wstring> ShipPreMods_Colony, ShipPostMods_Colony;

	/*#####################################################
	*/	HWND GROUP_INSTRUMENT_SHIP, 
			BUTTON_INSTRUMENT; /*
	#####################################################*/

	bool useShipPreMods_Instrument, useShipPostMods_Instrument, useShipNumberMods_Instrument;
	HWND useShipPreMods_InstrumentH, useShipPostMods_InstrumentH, useShipNumberMods_InstrumentH;
	HWND useShipMods_Instrument_DESC;

	int probShipPreMod_Instrument, probShipPostMod_Instrument, probShipNumberMod_Instrument;
	HWND probShipPreMod_InstrumentH, probShipPostMod_InstrumentH, probShipNumberMod_InstrumentH;
	HWND shipModsProb_Instrument_DESC;

	HWND shipPreModList_Instrument, shipPostModList_Instrument;
	std::vector<std::wstring> ShipPreMods_Instrument, ShipPostMods_Instrument;

	/*#####################################################
	*/	HWND GROUP_SATELLITE_SHIP, 
			BUTTON_SATELLITE; /*
	#####################################################*/

	bool useShipPreMods_Satellite, useShipPostMods_Satellite, useShipNumberMods_Satellite;
	HWND useShipPreMods_SatelliteH, useShipPostMods_SatelliteH, useShipNumberMods_SatelliteH;
	HWND useShipMods_Satellite_DESC;

	int probShipPreMod_Satellite, probShipPostMod_Satellite, probShipNumberMod_Satellite;
	HWND probShipPreMod_SatelliteH, probShipPostMod_SatelliteH, probShipNumberMod_SatelliteH;
	HWND shipModsProb_Satellite_DESC;

	HWND shipPreModList_Satellite, shipPostModList_Satellite;
	std::vector<std::wstring> ShipPreMods_Satellite, ShipPostMods_Satellite;

	/*#####################################################
	*/	HWND GROUP_STATION_SHIP, 
			BUTTON_STATION; /*
	#####################################################*/

	bool useShipPreMods_Station, useShipPostMods_Station, useShipNumberMods_Station;
	HWND useShipPreMods_StationH, useShipPostMods_StationH, useShipNumberMods_StationH;
	HWND useShipMods_Station_DESC;

	int probShipPreMod_Station, probShipPostMod_Station, probShipNumberMod_Station;
	HWND probShipPreMod_StationH, probShipPostMod_StationH, probShipNumberMod_StationH;
	HWND shipModsProb_Station_DESC;

	HWND shipPreModList_Station, shipPostModList_Station;
	std::vector<std::wstring> ShipPreMods_Station, ShipPostMods_Station;

	/*#####################################################
		*/	HWND GROUP_DATASET; /*
	#####################################################*/

	HWND Markov_INFO;

	int order, wordVarience, max_length, min_length;
	HWND orderH, wordVarienceH, max_lengthH, min_lengthH;
	HWND orderDESC, wordVarienceDESC, max_lengthDESC, min_lengthDESC;

	HWND Markov_RawDatasetH, Markov_RawDatasetDESC;
	std::vector<std::wstring> Markov_RawDataset;

	struct NGRAMS
	{
		std::vector<std::wstring> ngrams;
		std::vector<std::wstring> nextCharList;
	};
	NGRAMS main_ngrams, twogram_list;
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

	//Ship All
	bool useShipPreMods_All, useShipPostMods_All;
	int probShipPreMod_All, probShipPostMod_All;
	wchar_t ShipPreMods_All[MODLIST_SIZE], ShipPostMods_All[MODLIST_SIZE];

	//Ship Colony
	bool useShipPreMods_Colony, useShipPostMods_Colony, useShipNumberMods_Colony;
	int probShipPreMod_Colony, probShipPostMod_Colony, probShipNumberMod_Colony;
	wchar_t ShipPreMods_Colony[MODLIST_SIZE], ShipPostMods_Colony[MODLIST_SIZE];

	//Ship Instrument
	bool useShipPreMods_Instrument, useShipPostMods_Instrument, useShipNumberMods_Instrument;
	int probShipPreMod_Instrument, probShipPostMod_Instrument, probShipNumberMod_Instrument;
	wchar_t ShipPreMods_Instrument[MODLIST_SIZE], ShipPostMods_Instrument[MODLIST_SIZE];

	//Ship Satellite
	bool useShipPreMods_Satellite, useShipPostMods_Satellite, useShipNumberMods_Satellite;
	int probShipPreMod_Satellite, probShipPostMod_Satellite, probShipNumberMod_Satellite;
	wchar_t ShipPreMods_Satellite[MODLIST_SIZE], ShipPostMods_Satellite[MODLIST_SIZE];

	//Ship Station
	bool useShipPreMods_Station, useShipPostMods_Station, useShipNumberMods_Station;
	int probShipPreMod_Station, probShipPostMod_Station, probShipNumberMod_Station;
	wchar_t ShipPreMods_Station[MODLIST_SIZE], ShipPostMods_Station[MODLIST_SIZE];

	//Dataset
	int order, wordVarience, max_length, min_length;
	wchar_t Markov_RawDataset[DATASET_SIZE];

	//Simple Generator
	bool useSimpleGenerator;
	wchar_t PrefixList[MODLIST_SIZE], SuffixList[MODLIST_SIZE];
};