#include "framework.h"
#include "resource.h"
#include <windows.h>

struct var
{
	HWND HANDLE,
		DESC,
		INFOBUTTON,
		EXTRA;
};

struct Configuration
{
	/*--------------------------------------------------------------#
	|																|				
	|		Every variable needs a value holder, a var holder,		|
	|		and a value holder in the Presets structure.			|
	|																|
	|		EVERY variable neeeds to be added to these functions,	|
	|		preferably in this order:								|
	|																|
	|		* LoadPresets - to get the information from the preset	|
	|			files. It needs text inside the preset files		|
	|			to function.										|
	|		* UpdatePreset - to distribute the values across		|
	|			the window handles as text inside the window		|
	|			boxes.												|
	|		* SavePreset - to put current the values from the		|
	|			window boxes into files to be read.					|
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
	|		a CB_number under Resource.h.							|
	|																|
	#--------------------------------------------------------------*/

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

	int seed, numberOfRuns;
	var seedH, numberOfRunsH;
	bool debug;
	var  debugH;
	wchar_t starOutputFolder[WSIZE], planetOutputFolder[WSIZE];
	var starOutputFolderH, planetOutputFolderH;

	var presetDropDown;
	var savePresetButton;

	/*#####################################################
		*/	HWND HEADER_SYSTEMPLANET; /*
	#####################################################*/

	bool smartPlacement, generateDwarfPlanets;
	var smartPlacementH, generateDwarfPlanetsH;
	int	minPlanetNumber, dwarfPlanetChance;
	var minPlanetNumberH, dwarfPlanetChanceH;
	double minDistance, maxDistance, planetSpacing, avgEccentricity, SDEccentricity, avgInclination, SDInclination, avgObliquity, SDObliquity;
	var    minDistanceH, maxDistanceH, planetSpacingH, avgEccentricityH, SDEccentricityH, avgInclinationH, SDInclinationH, avgObliquityH, SDObliquityH;
	bool starClassO, starClassB, starClassA, starClassF, starClassG, starClassK, starClassM, starClassWD, starClassQ, starClassX;
	var  starClassOH, starClassBH, starClassAH, starClassFH, starClassGH, starClassKH, starClassMH, starClassWDH, starClassQH, starClassXH;

		/*=====================================================
			*/	std::vector<std::wstring> classList;
				int classListSize; /*
			If a star class is enabled via its checkbox, 
			it is added to this vector, which is used
			to select a random class.

		=====================================================*/

	/*#####################################################
		*/	HWND HEADER_LIFE; /*
	#####################################################*/

	int life_OrganicChance, life_ExoticChance, life_MulticellChance;
	var life_OrganicChanceH, life_ExoticChanceH, life_MulticellChanceH;
	bool forceLife, traditionalLife;
	var  forceLifeH, traditionalLifeH;

	/*#####################################################
		*/	HWND HEADER_SHIPS; /*
	#####################################################*/

	int exotic_ShipChance;
	var exotic_ShipChanceH;
	bool shipsNeedLife;
	var  shipsNeedLifeH;
	wchar_t modelsFolder[WSIZE];
	var		modelsFolderH;

	/*#####################################################
		*/	HWND HEADER_EXOTIC; /*
	#####################################################*/

	int exotic_OrbitChance, exotic_AxialTiltChance, exotic_CompanionOrbitChance, exotic_DebrisRingChance;
	var exotic_OrbitChanceH, exotic_AxialTiltChanceH, exotic_CompanionOrbitChanceH, exotic_DebrisRingChanceH;

	/*#####################################################
		*/	HWND HEADER_ADVANCED; /*
	#####################################################*/

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
		General Things
	#####################################################*/

	int seed, numberOfRuns;
	bool debug;
	wchar_t const* debugState;
	wchar_t starOutputFolder[WSIZE], planetOutputFolder[WSIZE];

	/*#####################################################
		System + Planet Stuff
	#####################################################*/

	bool smartPlacement, generateDwarfPlanets;
	wchar_t const* smartPlacementState, * generateDwarfPlanetsState;
	int	minPlanetNumber, dwarfPlanetChance;
	double minDistance, maxDistance, planetSpacing, avgEccentricity, SDEccentricity, avgInclination, SDInclination, avgObliquity, SDObliquity;
	bool starClassO, starClassB, starClassA, starClassF, starClassG, starClassK, starClassM, starClassWD, starClassQ, starClassX;

	/*#####################################################
		Life Stuff
	#####################################################*/

	int life_OrganicChance, life_ExoticChance, life_MulticellChance;
	bool forceLife, traditionalLife;
	wchar_t const* forceLifeState, const* traditonalLifeState;

	/*#####################################################
		Ships Stuff
	#####################################################*/

	int exotic_ShipChance;
	bool shipsNeedLife;
	wchar_t const* shipsNeedLifeState;
	wchar_t modelsFolder[WSIZE];

	/*#####################################################
		Exotic Do-Dads
	#####################################################*/

	int exotic_OrbitChance, exotic_AxialTiltChance, exotic_CompanionOrbitChance, exotic_DebrisRingChance;

	/*#####################################################
		Advanced Items Maybe?
	#####################################################*/
};

struct NameVariables
{
	/*#####################################################
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
	#####################################################*/

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
		*/	HWND GROUP_SHIP; /*
	#####################################################*/

	/*#####################################################
		*/	HWND GROUP_DATASET; /*
	#####################################################*/

	/*#####################################################
	*/	HWND GROUP_SIMPLE; /*
	#####################################################*/

	bool useSimpleGenerator;
	std::vector<std::wstring> PrefixList, SuffixList;
	
	






	// Terra moon names are for moons around small planets, Giant moon names are for moons around gas/ice giants
	bool useTerraMoonNames, useTerraDMoonNames, useGiantMoonNames, useGiantDMoonNames;

	// Ship name stuff
	bool useShipPreMods_All, useShipPostMods_All,
		useShipPreMods_Colony, useShipPostMods_Colony, useShipNumberMods_Colony,
		useShipPreMods_Instrument, useShipPostMods_Instrument, useShipNumberMods_Instrument,
		useShipPreMods_Satellite, useShipPostMods_Satellite, useShipNumberMods_Satellite,
		useShipPreMods_Station, useShipPostMods_Station, useShipNumberMods_Station;
	int probShipPreMod_All, probShipPostMod_All,
		probShipPreMod_Colony, probShipPostMod_Colony, probShipNumberMod_Colony,
		probShipPreMod_Instrument, probShipPostMod_Instrument, probShipNumberMod_Instrument,
		probShipPreMod_Satellite, probShipPostMod_Satellite, probShipNumberMod_Satellite,
		probShipPreMod_Station, probShipPostMod_Station, probShipNumberMod_Station;
	std::vector<std::wstring> ShipPreMods_All, ShipPostMods_All,
		ShipPreMods_Colony, ShipPostMods_Colony,
		ShipPreMods_Instrument, ShipPostMods_Instrument,
		ShipPreMods_Satellite, ShipPostMods_Satellite,
		ShipPreMods_Station, ShipPostMods_Station;

	struct NGRAMS
	{
		std::vector<std::wstring> ngrams;
		std::vector<std::wstring> nextCharList;
	};

	
	std::vector<std::wstring> MarkovData_vector;
	NGRAMS main_ngrams, twogram_list;
	int wordVarience, max_length, min_length;
	const int ORDER = 3;

	std::vector<std::wstring> usedNames;
};