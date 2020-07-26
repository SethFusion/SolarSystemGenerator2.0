//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by SciFiGenerator2.0.rc

#define MAX_LOADSTRING			100
#define WSIZE					256
#define MODLIST_SIZE			1024
#define DATASET_SIZE			8192

#define SendDebugMessage		if (CONFIG.debug) DebugMessage
#define debug_INFO				1
#define debug_WARNING			2
#define debug_ERROR				3
#define debug_STAR_LIMITS		4
#define debug_PLANET_LIMITS		5
#define color_NORMAL			"(0.1, 0.1, 0.1)"
#define color_HALFRED			"(1.0, 0.5, 0.5)"
#define color_RED				"(1.0, 0.0, 0.0)"

#define UpdateProgress			SendMessageW(CONFIG_H.PROGRESS_BAR, PBM_DELTAPOS, 1, 0)
#define SetProgress(str)		SetWindowText(CONFIG_H.INFO_BOX, str)
#define currentStar				starList.at(starKeys.at(currentstar))



/*---------------------------------------------
		Button Commands
---------------------------------------------*/

#define BUTTON_GENERATE				1
#define BUTTON_GENERAL				2
#define BUTTON_SYSTEM				3
#define BUTTON_PLANET				4
#define BUTTON_SURFACE				5
#define BUTTON_SPECIAL				6
#define BUTTON_ADVANCED				7

#define BUTTON_UPDATEPRESET			8
#define BUTTON_SAVEPRESET			9
#define BUTTON_SETOUTPUT			10

#define BUTTON_NAME_UPDATE			22
#define BUTTON_NAME_LOAD_VECTORS	23
#define BUTTON_NAME_SAVEPRESET		24

#define BUTTON_TEST_NAMES			25

#define BUTTON_BROWSE_STAR_FOLDER	26
#define BUTTON_BROWSE_PLANET_FOLDER	27
#define BUTTON_BROWSE_MODELS_FOLDER	28

#define TABGROUP_ADVANCED_OUTER		0
#define TABGROUP_ADVANCED_INNER		1 //not used for now
#define TABGROUP_NAME_INNER			2
#define TABGROUP_SHIP_INNER			3

#define TAB_NAMES					0
	#pragma region Under Name Variables
	#define TAB_NAME_DATASET			0
	#define TAB_NAME_STAR				1
	#define TAB_NAME_PLANET				2
	#define TAB_NAME_MOON				3
	#define TAB_NAME_SHIP				4
	#define TAB_NAME_DWARFMOON			5
	#define TAB_NAME_ASTEROID			6
	#define TAB_NAME_COMET				7	
		#pragma region Under Ship Variables
		#define TAB_SHIP_ALL				0
		#define TAB_SHIP_STARSHIP			1
		#define TAB_SHIP_PLANETSHIP			2
		#define TAB_SHIP_SATELLITE			3
		#define TAB_SHIP_STATION			4
		#define TAB_SHIP_PROBE				5	
		#pragma endregion
	#define TAB_NAME_SIMPLE				8
	#pragma endregion
#define TAB_ADVANCED				1
	#pragma region Under Advanced Variables

	#pragma endregion

//#define BUTTON_ADV_

/*---------------------------------------------
		Info Box IDs
---------------------------------------------*/

#define IB_SEED					1000
#define IB_NUMBEROFRUNS			1001
#define IB_STAROUTPUTFOLDER		1002
#define IB_PLANETOUTPUTFOLDER	1003
#define IB_SELECTPRESET			1004
#define IB_SAVEPRESET			1005
#define IB_SELECTNAMEPRESET		1006
#define IB_SAVENAMEPRESET		1007

#define IB_DISTANCE				1008
#define IB_SMARTPLACEMENT		1009
#define IB_MINPLANETNUMBER		1010
#define IB_SYSTEMRADIUSMODIFIER	1011
#define IB_PLANETSPACING		1012
#define IB_ECCENTRICITY			1013
#define IB_INCLINATION			1014
#define IB_OBLIQUITY			1015
#define IB_STARCLASS			1016
#define IB_GENERATEDWARFPLANET	1017
#define IB_DWARFPALNETCHANCE	1018
#define IB_GENERATEASTEROIDS	1019
#define IB_MAXASTEROIDBELTS		1020
#define IB_ASTEROIDCOUNT		1021
#define IB_GENERATECOMETS		1022
#define IB_COMETCOUNT			1023
#define IB_MULTISTARS			1049

#define IB_WEIGHTEDMOONS		1024
#define IB_MOONDISTANCEBOUNDARY 1025
#define IB_EXOTICORBIT			1026
#define IB_EXOTICOBLIQUITY		1027
#define IB_EXOTICCOMPANION		1028
#define IB_EXOTICDEBRISRING		1029
#define IB_LIFEORGANICCHANCE	1030
#define IB_LIFEEXOTICCHANCE		1031
#define IB_LIFEMULTICHANCE		1032
#define IB_FORCELIFE			1033
#define IB_TRADITIONALLIFE		1034

#define IB_GENERATECOMPOSITION	1035

#define IB_SHIPCHANCE			1036
#define IB_SHIPSNEEDLIFE		1037
#define IB_MODELSFOLDER			1038

#define IB_MARKOV				1039
#define IB_WORDPERCENT			1040
#define IB_NAMEPREMODS			1041
#define IB_NAMEPOSTMODS			1042
#define IB_NAMENUMBERMODS		1043
#define IB_NAMEPREMODLIST		1044
#define IB_NAMEPOSTMODLIST		1045
#define IB_NAMEMOONS			1046 // used for dwarf moons as well
#define IB_NAMEASTEROIDS		1047 // used for comets as well
#define IB_SIMPLEGENERATOR		1048

/*---------------------------------------------
		Check Box IDs
---------------------------------------------*/

#define CB_DEBUG				1500

#define CB_SMARTPLACEMENT		1501
#define CB_GENERATEDWARFPLANET	1502
#define CB_WEIGHTEDMOONS		1503
#define CB_GENERATEASTEROIDS	1508
#define CB_GENERATECOMETS		1509
#define CB_MULTISTARS			1510

#define CB_TRADITIONALLIFE		1504
#define CB_FORCELIFE			1505

#define CB_SHIPSNEEDLIFE		1506
#define CB_GENERATECOMPOSITION	1507

/*---------------------------------------------
		Trackbar IDs
---------------------------------------------*/

/*---------------------------------------------
		Name Variable Check Box IDs
---------------------------------------------*/

#define NVCB_SIMPLEGENERATOR			1600

#define NVCB_STARPREMOD					1601
#define NVCB_STARPOSTMOD				1602
#define NVCB_STARNUMBERMOD				1603

#define NVCB_PLANETPREMOD				1604
#define NVCB_PLANETPOSTMOD				1605
#define NVCB_PLANETNUMBERMOD			1606

#define NVCB_NAMETERRAMOONS				1607
#define NVCB_NAMEGASMOONS				1608
#define NVCB_MOONPREMOD					1609
#define NVCB_MOONPOSTMOD				1610
#define NVCB_MOONNUMBERMOD				1611

#define NVCB_NAMETERRADWARFMOONS		1612
#define NVCB_NAMEGASDWARFMOONS			1613
#define NVCB_DWARFMOONPREMOD			1614
#define NVCB_DWARFMOONPOSTMOD			1615
#define NVCB_DWARFMOONNUMBERMOD			1616

#define NVCB_NAMEASTEROIDS				1617
#define NVCB_ASTEROIDPREMOD				1618
#define NVCB_ASTEROIDPOSTMOD			1619
#define NVCB_ASTEROIDNUMBERMOD			1620

#define NVCB_NAMECOMETS					1621
#define NVCB_COMETPREMOD				1622
#define NVCB_COMETPOSTMOD				1623
#define NVCB_COMETNUMBERMOD				1624

#define NVCB_SHIPALLPREMOD				1625
#define NVCB_SHIPALLPOSTMOD				1626

#define NVCB_SHIPSTARSHIPPREMOD			1627
#define NVCB_SHIPSTARSHIPPOSTMOD		1628
#define NVCB_SHIPSTARSHIPNUMBERMOD		1629

#define NVCB_SHIPPLANETSHIPPREMOD		1630
#define NVCB_SHIPPLANETSHIPPOSTMOD		1631
#define NVCB_SHIPPLANETSHIPNUMBERMOD	1632

#define NVCB_SHIPSTATIONPREMOD			1633
#define NVCB_SHIPSTATIONPOSTMOD			1634
#define NVCB_SHIPSTATIONNUMBERMOD		1635

#define NVCB_SHIPSATELLITEPREMOD		1636
#define NVCB_SHIPSATELLITEPOSTMOD		1637
#define NVCB_SHIPSATELLITENUMBERMOD		1638

#define NVCB_SHIPPROBEPREMOD			1639
#define NVCB_SHIPPROBEPOSTMOD			1640
#define NVCB_SHIPPROBENUMBERMOD			1641







#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128
#define IDD_SOLARSYSTEMGENERATOR20_DIALOG	102
#define IDD_ABOUTBOX			103
#define IDM_ABOUT				104
#define IDM_EXIT				105
#define IDI_SOLARSYSTEMGENERATOR20			107
#define IDI_SMALL				108
#define IDC_SOLARSYSTEMGENERATOR20			109
#define IDC_MYICON				2
#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif
// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif
