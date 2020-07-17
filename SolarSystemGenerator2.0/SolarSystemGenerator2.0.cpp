#include "framework.h"
#include "SolarSystemGenerator2.0.h"
#include "ConfigStructs.h"
#include "Structs.h"
#include "MiscFunctions.cpp"
#include "CheckSeed.cpp"

/*---------------------------------------------------------------------------------------#
|	Global Variables:																	 |
#---------------------------------------------------------------------------------------*/

HINSTANCE hInst; // current instance
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name
WCHAR WINDOW_TITLE[MAX_LOADSTRING] = (L"Seth Fusion's Solar System Generator V2.0");
HWND Handle_Logo;
HBITMAP Handle_Image_Logo;

ConfigurationHWNDs CONFIG_H;
ConfigurationVariables CONFIG;

std::vector<Preset> preset;
NameVariables NV;
std::vector<NamePreset> npreset;

std::mt19937 mt_star, mt_planet, mt_moon, mt_ship, mt_name;
std::uniform_int_distribution<int> genpercent{ 0, 99 }; // Number to compare represents real % ex: genpercent() < 50 = 50%
std::uniform_real_distribution<> gendegree{ 0, 360 };
enum Object_Type {typeStar = 1, typePlanet = 2, typeMoon = 3, typeDwarfMoon = 4, typeAsteroid = 5, typeComet = 6,
	typeShipColony = 7, typeShipInstrument = 8, typeShipSatellite = 9, typeShipStation = 10};
enum Screen {General = 1, System = 2, Planet = 3, Surface = 4, Special = 5, Advanced = 6};
Screen lastScreen;


SEPlanet SystemDebug;
int SystemDebugCounter = 0;
std::ofstream* DebugFileP;

/*---------------------------------------------------------------------------------------#
|	Function Declaration:																 |
#---------------------------------------------------------------------------------------*/

/*#####################################################
	*/	ATOM MyRegisterClass(HINSTANCE hInstance); /*
	Registers the window class.

#######################################################
	*/	BOOL InitInstance(HINSTANCE, int); /*
	Saves instance handle and creates main window.
	In this function, we save the instance handle in
	a global variable and create and display the
	main program window.

#######################################################
	*/	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); /*
	Processes messages for the main window.

#######################################################*/	
#pragma region UI Functions

	void LoadImages();
	void LoadPresets(HWND);
	void LoadNamePresets(HWND);
	void LoadMainScreen(HWND);

	void UpdatePreset(Preset, HWND);
	void UpdateNamePreset(NamePreset, HWND);
	void SavePreset();
	void SaveNamePreset();

	void Clear_Screen();
	void Load_Screen_General();
	void Load_Screen_System();
	void Load_Screen_Planet();
	void Load_Screen_Surface();
	void Load_Screen_Special();
	void Load_Screen_Advanced();

	//void WINAPI TBNotifications(WPARAM wParam, HWND hwndTrack, UINT iSelMin, UINT iSelMax);

	void Load_Name_Dataset();
	void Load_Advanced_Main();
	void Clear_Advanced();

	void Load_Tabgroup_Advanced_Outer();
	void Load_Tabgroup_Name_Inner();
	void Load_Tabgroup_Advanced_Inner();
	void Load_Tabgroup_Ship_Inner();
	
	void Load_Name_Star();
	void Load_Name_Planet();
	void Load_Name_Moon();
	void Load_Name_DwarfMoon();
	void Load_Name_Asteroid();
	void Load_Name_Comet();
	void Load_Name_All_Ship();
	void Load_Name_Colony_Ship();
	void Load_Name_Instrument_Ship();
	void Load_Name_Satellite_Ship();
	void Load_Name_Station_Ship();	
	void Load_Name_Simple();

	void GetConfigData(HWND);
	bool CheckConfigData(HWND);
	void CreateNameVectors(HWND);
	void TestNames();

	void SetInfoBox(int);
	void SetCheckBoxText(HWND, int);

	//Windows Functions
	void BrowseFolder(HWND); // handle parameter stores the folder path
#pragma endregion
//#######################################################
#pragma region Generator Functions

	std::wstring GenName(Object_Type);
	std::wstring GenNumberModifier();
	void BeginGenerate(HWND);
	template <typename vect, typename type> void SortVector(vect&, int, int);
	template <typename vect, typename type> int Partition(vect&, int, int);
	void GenerateStar(SEStar&);
	void PrintStar(SEStar&, std::ofstream&);
	void PrintPlanet(SEPlanet&, std::ofstream&);
	void PrintShip(SEShip&, std::ofstream&);
	void GeneratePlanet(SEStar&, SEPlanet&);
	void GenerateDwarfPlanet(SEStar&, SEPlanet&);
	bool GenerateMajorMoon(SEStar&, SEPlanet&, SEPlanet&, int);
	void GenerateMinorMoon(SEPlanet&, SEPlanet&, int);
	void GenerateAsteroid(SEStar&, SEPlanet&, double, double, double, double);
	void GenerateComet(SEStar&, SEPlanet&);

	void ExoticGenerateLife(SEPlanet&);
	void ExoticDebrisRing(SEPlanet&, SEPlanet&, double, double, double, double);
	void GenerateShip(SEShip&, double, double, double, double, double, double);

	void DebugMessage(std::wstring, int, SEObject & = SystemDebug, double = (++SystemDebugCounter * 10000.0));
	void PrintDebug(SEPlanet&, std::string);
#pragma endregion	
//#######################################################

/*---------------------------------------------------------------------------------------#
|	THE WINDOW:																			 |
#---------------------------------------------------------------------------------------*/

/*#####################################################
	Main Entry Function for Windows */
	int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
	{
		UNREFERENCED_PARAMETER(hPrevInstance);
		UNREFERENCED_PARAMETER(lpCmdLine);

		// TODO: Place code here.

		// Initialize global strings

		//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_SOLARSYSTEMGENERATOR20, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		// Perform application initialization:
		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}

		HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SOLARSYSTEMGENERATOR20));

		MSG msg;

		// Main message loop:
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (int)msg.wParam;
	}
/*###################################################*/

	ATOM MyRegisterClass(HINSTANCE hInstance)
	{
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SOLARSYSTEMGENERATOR20));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0; //MAKEINTRESOURCEW(IDC_SCIFIGENERATOR20);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}

	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
	{
		hInst = hInstance; // Store instance handle in our global variable

		HWND hWnd = CreateWindowW(szWindowClass, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, 1040, 720, nullptr, nullptr, hInstance, nullptr);
		// Loads the page at 1024 px wide

		if (!hWnd)
		{
			return FALSE;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		return TRUE;
	}

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
		{
			//First half looks through all of the Hiword commands
			int wmId = HIWORD(wParam);
			switch (wmId)
			{
			}

			//Second half looks through all of the Loword commands
			wmId = LOWORD(wParam);
			if (wmId >= 1500) //edits text for checkboxes
			{
				SetCheckBoxText(hWnd, wmId);
				return 0;
			}
			if (wmId >= 1000) //sets the info box
			{
				SetInfoBox(wmId);
				return 0;
			}
			switch (wmId) //Specific Commands
			{
			case BUTTON_GENERAL:
				Clear_Screen();
				Load_Screen_General();
				break;
			case BUTTON_SYSTEM:
				Clear_Screen();
				Load_Screen_System();
				break;
			case BUTTON_PLANET:
				Clear_Screen();
				Load_Screen_Planet();
				break;
			case BUTTON_SURFACE:
				Clear_Screen();
				Load_Screen_Surface();
				break;
			case BUTTON_SPECIAL:
				Clear_Screen();
				Load_Screen_Special();
				break;
			case BUTTON_ADVANCED:
				Clear_Screen();
				Load_Screen_Advanced();
				break;

			case BUTTON_BROWSE_STAR_FOLDER:
				BrowseFolder(CONFIG_H.starOutputFolder.HANDLE);
				break;
			case BUTTON_BROWSE_PLANET_FOLDER:
				BrowseFolder(CONFIG_H.planetOutputFolder.HANDLE);
				break;
			case BUTTON_BROWSE_MODELS_FOLDER:
				BrowseFolder(CONFIG_H.modelsFolder.HANDLE);
				break;

			case BUTTON_SAVEPRESET:
				GetConfigData(hWnd);
				SavePreset();
				LoadPresets(hWnd);
				break;
			case BUTTON_UPDATEPRESET:
				wmId = SendMessage(CONFIG_H.presetDropDown.HANDLE, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				UpdatePreset(preset.at(wmId), hWnd);
				break;
			case BUTTON_GENERATE:
				GetConfigData(hWnd);
				if (!CheckConfigData(hWnd))
					BeginGenerate(hWnd);
				break;
			case BUTTON_NAME_SAVEPRESET:
				CreateNameVectors(hWnd);
				SaveNamePreset();
				LoadNamePresets(hWnd);
				break;
			case BUTTON_NAME_UPDATE:
				wmId = SendMessage(CONFIG_H.namePresetDropDown.HANDLE, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				UpdateNamePreset(npreset.at(wmId), hWnd);
				break;
			case BUTTON_NAME_LOAD_VECTORS:
				CreateNameVectors(hWnd);
				break;
			case BUTTON_TEST_NAMES:
				CreateNameVectors(hWnd);
				TestNames();
				break;


			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_NOTIFY:
		{
			NMHDR* note = (LPNMHDR)lParam;
			switch (note->code)
			{
			case TCN_SELCHANGING:
			{
				Clear_Advanced();
			}
			break;
			case TCN_SELCHANGE:
			{
				// determines which tabgroup is trying to switch screens
				switch (note->idFrom) 
				{
				case TABGROUP_ADVANCED_OUTER:
					Load_Tabgroup_Advanced_Outer();
					break;
				case TABGROUP_ADVANCED_INNER:
					//empty for now
					break;
				case TABGROUP_NAME_INNER:
					Load_Tabgroup_Name_Inner();
					break;
				case TABGROUP_SHIP_INNER:
					Load_Tabgroup_Ship_Inner();
					break;
				}
			}
			break;
			}
		}
		break;
		// for trackbar stuff
		/*case WM_HSCROLL:
			UINT min, max;
			min = SendMessage((HWND)lParam, TBM_GETRANGEMIN, 0, 0);
			max = SendMessage((HWND)lParam, TBM_GETRANGEMAX, 0, 0);
			TBNotifications(wParam, (HWND)lParam, min, max);
			break;*/
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_CREATE:
			LoadImages();
			LoadPresets(hWnd);
			LoadNamePresets(hWnd);
			LoadMainScreen(hWnd);
			UpdatePreset(preset.at(0), hWnd);
			UpdateNamePreset(npreset.at(0), hWnd);
			Clear_Screen();
			Load_Screen_General();
			//Load_Screen_System();
			//Load_Screen_Planet();
			//Load_Screen_Surface();
			//Load_Screen_Special();
			//Load_Screen_Advanced();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void LoadImages()
	{
		Handle_Image_Logo = (HBITMAP)LoadImageW(NULL, L"GneratorLogoTest.bmp", IMAGE_BITMAP, 340, 280, LR_LOADFROMFILE);
	}
	void LoadPresets(HWND hWnd)
	{
		wchar_t direct[WSIZE] = L".\\presets\\????????????????????????????????.preset";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		hFind = FindFirstFileW(direct, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			return;
		}

		std::vector<std::wstring> addresses;
		do
		{
			wchar_t prefix[WSIZE] = L".\\presets\\";
			wcscat_s(prefix, FindFileData.cFileName);
			std::wstring str(prefix);
			addresses.push_back(str);
		} while (FindNextFileW(hFind, &FindFileData) != 0);

		DestroyWindow(CONFIG_H.presetDropDown.HANDLE);
		DestroyWindow(CONFIG_H.presetDropDown.DESC);
		DestroyWindow(CONFIG_H.presetDropDown.INFOBUTTON);
		DestroyWindow(CONFIG_H.presetDropDown.EXTRA);
		CONFIG_H.presetDropDown.HANDLE = CreateWindowW(L"combobox", L"",
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			370, 300, 200, 500,
			hWnd, NULL, hInst, NULL);
		CONFIG_H.presetDropDown.DESC = CreateWindowW(L"static", L"Select a Preset:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
			370, 280, 180, 20,
			hWnd, NULL, hInst, NULL);
		CONFIG_H.presetDropDown.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			552, 282, 16, 16,
			hWnd, (HMENU)IB_SELECTPRESET, NULL, NULL);

		if (preset.size() > 0)
		{
			int size = preset.size();
			for (int i = 0; i < size; i++)
				preset.pop_back();
		}

		for (int i = 0; i < addresses.size(); i++)
		{
			HANDLE presetFile = CreateFileW(&addresses.at(i)[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			Preset P;

			const int BUFFSIZE = 4096; // about 4 kilobytes
			DWORD bytesread = 0;
			char Buffer[BUFFSIZE];

			if (!ReadFile(presetFile, Buffer, BUFFSIZE, &bytesread, NULL))
			{
				//BAD JUJU HAPPENED
				CloseHandle(presetFile);
				return;
			}
			Buffer[bytesread] = '\0';

			int parse = 0;
			LoadVariableFromFile(Buffer, parse, P.name);	
			LoadVariableFromFile(Buffer, parse, P.starOutputFolder);		
			LoadVariableFromFile(Buffer, parse, P.planetOutputFolder);		
			LoadVariableFromFile(Buffer, parse, P.seed);	
			LoadVariableFromFile(Buffer, parse, P.numberOfRuns);	
			LoadVariableFromFile(Buffer, parse, P.smartPlacement);
			P.smartPlacementState = (P.smartPlacement == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.systemRadiusModifier);
			LoadVariableFromFile(Buffer, parse, P.minPlanetNumber);		
			LoadVariableFromFile(Buffer, parse, P.minDistance);		
			LoadVariableFromFile(Buffer, parse, P.maxDistance);		
			LoadVariableFromFile(Buffer, parse, P.planetSpaceAvg);
			LoadVariableFromFile(Buffer, parse, P.planetSpaceSD);
			LoadVariableFromFile(Buffer, parse, P.moonDistanceBoundary);
			LoadVariableFromFile(Buffer, parse, P.generateDwarfPlanets);
			P.generateDwarfPlanetsState = (P.generateDwarfPlanets == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, P.dwarfPlanetChance);
			LoadVariableFromFile(Buffer, parse, P.weightedMoons);
			P.weightedMoonsState = (P.weightedMoons == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.avgEccentricity);		
			LoadVariableFromFile(Buffer, parse, P.SDEccentricity);		
			LoadVariableFromFile(Buffer, parse, P.avgInclination);		
			LoadVariableFromFile(Buffer, parse, P.SDInclination);		
			LoadVariableFromFile(Buffer, parse, P.avgObliquity);		
			LoadVariableFromFile(Buffer, parse, P.SDObliquity);
			LoadVariableFromFile(Buffer, parse, P.generateAsteroidBelt);
			P.generateAsteroidBeltState = (P.generateAsteroidBelt == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.maxAsteroidBelts);
			LoadVariableFromFile(Buffer, parse, P.minAsteroidCount);
			LoadVariableFromFile(Buffer, parse, P.maxAsteroidCount);
			LoadVariableFromFile(Buffer, parse, P.generateComets);
			P.generateCometsState = (P.generateComets == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.minCometCount);
			LoadVariableFromFile(Buffer, parse, P.maxCometCount);
			LoadVariableFromFile(Buffer, parse, P.starClassO);		
			LoadVariableFromFile(Buffer, parse, P.starClassB);		
			LoadVariableFromFile(Buffer, parse, P.starClassA);		
			LoadVariableFromFile(Buffer, parse, P.starClassF);		
			LoadVariableFromFile(Buffer, parse, P.starClassG);		
			LoadVariableFromFile(Buffer, parse, P.starClassK);		
			LoadVariableFromFile(Buffer, parse, P.starClassM);		
			LoadVariableFromFile(Buffer, parse, P.starClassWD);		
			LoadVariableFromFile(Buffer, parse, P.starClassQ);		
			LoadVariableFromFile(Buffer, parse, P.starClassX);	
			LoadVariableFromFile(Buffer, parse, P.life_OrganicChance);		
			LoadVariableFromFile(Buffer, parse, P.life_ExoticChance);		
			LoadVariableFromFile(Buffer, parse, P.life_MulticellChance);		
			LoadVariableFromFile(Buffer, parse, P.forceLife);
			P.forceLifeState = (P.forceLife == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, P.traditionalLife);
			P.traditonalLifeState = (P.traditionalLife == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.generateComposition);
			P.generateCompositionState = (P.generateComposition == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, P.exotic_ShipChance);		
			LoadVariableFromFile(Buffer, parse, P.modelsFolder);		
			LoadVariableFromFile(Buffer, parse, P.shipsNeedLife);
			P.shipsNeedLifeState = (P.shipsNeedLife == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, P.exotic_OrbitChance);		
			LoadVariableFromFile(Buffer, parse, P.exotic_AxialTiltChance);		
			LoadVariableFromFile(Buffer, parse, P.exotic_DebrisRingChance);		
			LoadVariableFromFile(Buffer, parse, P.exotic_CompanionOrbitChance);

			wchar_t holder = Buffer[parse];
			P.debug = _wtoi(&holder);

			preset.push_back(P);
			SendMessage(CONFIG_H.presetDropDown.HANDLE, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)preset.at(i).name);
			CloseHandle(presetFile);
		}

		SendMessage(CONFIG_H.presetDropDown.HANDLE, (UINT)CB_SETCURSEL, (WPARAM)0, NULL);

		CONFIG_H.presetDropDown.EXTRA = CreateWindowW(L"button", L"Update",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			575, 278, 50, 50,
			hWnd, (HMENU)BUTTON_UPDATEPRESET, NULL, NULL);
	}
	void LoadNamePresets(HWND hWnd)
	{
		wchar_t direct[WSIZE] = L".\\presets\\????????????????????????????????.npreset";
		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		hFind = FindFirstFileW(direct, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			return;
		}

		std::vector<std::wstring> addresses;
		do
		{
			wchar_t prefix[WSIZE] = L".\\presets\\";
			wcscat_s(prefix, FindFileData.cFileName);
			std::wstring str(prefix);
			addresses.push_back(str);
		} while (FindNextFileW(hFind, &FindFileData) != 0);

		DestroyWindow(CONFIG_H.namePresetDropDown.HANDLE);
		DestroyWindow(CONFIG_H.namePresetDropDown.DESC);
		DestroyWindow(CONFIG_H.namePresetDropDown.INFOBUTTON);
		DestroyWindow(CONFIG_H.namePresetDropDown.EXTRA);
		CONFIG_H.namePresetDropDown.HANDLE = CreateWindowW(L"combobox", L"",
			WS_CHILD | CBS_DROPDOWNLIST,
			740, 300, 200, 500,
			hWnd, NULL, hInst, NULL);
		CONFIG_H.namePresetDropDown.DESC = CreateWindowW(L"static", L"Select a Name Preset:",
			WS_CHILD | WS_BORDER | SS_CENTER,
			740, 280, 180, 20,
			hWnd, NULL, hInst, NULL);
		CONFIG_H.namePresetDropDown.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			922, 282, 16, 16,
			hWnd, (HMENU)IB_SELECTNAMEPRESET, NULL, NULL);

		if (npreset.size() > 0)
		{
			int size = npreset.size();
			for (int i = 0; i < size; i++)
				npreset.pop_back();
		}

		for (int i = 0; i < addresses.size(); i++)
		{
			HANDLE presetFile = CreateFileW(&addresses.at(i)[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			NamePreset temp;
			

			const int BUFFSIZE = 32768; // about 33 kilobytes
			DWORD bytesread = 0;
			char Buffer[BUFFSIZE];

			if (!ReadFile(presetFile, Buffer, BUFFSIZE, &bytesread, NULL))
			{
				//BAD JUJU HAPPENED
				CloseHandle(presetFile);
				return;
			}
			Buffer[bytesread] = '\0';

			int parse = 0;
			LoadVariableFromFile(Buffer, parse, temp.name);
			LoadVariableFromFile(Buffer, parse, temp.wordPercent);
			LoadVariableFromFile(Buffer, parse, temp.useSimpleGenerator);
			LoadListFromFile(Buffer, parse, temp.PrefixList);
			LoadListFromFile(Buffer, parse, temp.SuffixList);

			LoadVariableFromFile(Buffer, parse, temp.useStarPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probStarPreMod);
			LoadListFromFile(Buffer, parse, temp.StarPreMods);
			LoadVariableFromFile(Buffer, parse, temp.useStarPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probStarPostMod);
			LoadListFromFile(Buffer, parse, temp.StarPostMods);
			LoadVariableFromFile(Buffer, parse, temp.useStarNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probStarNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.usePlanetPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probPlanetPreMod);
			LoadListFromFile(Buffer, parse, temp.PlanetPreMods);
			LoadVariableFromFile(Buffer, parse, temp.usePlanetPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probPlanetPostMod);
			LoadListFromFile(Buffer, parse, temp.PlanetPostMods);
			LoadVariableFromFile(Buffer, parse, temp.usePlanetNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probPlanetNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.nameTerraMoons);
			LoadVariableFromFile(Buffer, parse, temp.nameGasMoons);
			LoadVariableFromFile(Buffer, parse, temp.useMoonPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probMoonPreMod);
			LoadListFromFile(Buffer, parse, temp.MoonPreMods);
			LoadVariableFromFile(Buffer, parse, temp.useMoonPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probMoonPostMod);
			LoadListFromFile(Buffer, parse, temp.MoonPostMods);
			LoadVariableFromFile(Buffer, parse, temp.useMoonNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probMoonNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.nameTerraDwarfMoons);
			LoadVariableFromFile(Buffer, parse, temp.nameGasDwarfMoons);
			LoadVariableFromFile(Buffer, parse, temp.useDwarfMoonPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probDwarfMoonPreMod);
			LoadListFromFile(Buffer, parse, temp.DwarfMoonPreMods);
			LoadVariableFromFile(Buffer, parse, temp.useDwarfMoonPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probDwarfMoonPostMod);
			LoadListFromFile(Buffer, parse, temp.DwarfMoonPostMods);
			LoadVariableFromFile(Buffer, parse, temp.useDwarfMoonNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probDwarfMoonNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.nameAsteroids);
			LoadVariableFromFile(Buffer, parse, temp.useAsteroidPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probAsteroidPreMod);
			LoadListFromFile(Buffer, parse, temp.AsteroidPreMods);
			LoadVariableFromFile(Buffer, parse, temp.useAsteroidPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probAsteroidPostMod);
			LoadListFromFile(Buffer, parse, temp.AsteroidPostMods);
			LoadVariableFromFile(Buffer, parse, temp.useAsteroidNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probAsteroidNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.nameComets);
			LoadVariableFromFile(Buffer, parse, temp.useCometPreMods);
			LoadVariableFromFile(Buffer, parse, temp.probCometPreMod);
			LoadListFromFile(Buffer, parse, temp.CometPreMods);
			LoadVariableFromFile(Buffer, parse, temp.useCometPostMods);
			LoadVariableFromFile(Buffer, parse, temp.probCometPostMod);
			LoadListFromFile(Buffer, parse, temp.CometPostMods);
			LoadVariableFromFile(Buffer, parse, temp.useCometNumberMods);
			LoadVariableFromFile(Buffer, parse, temp.probCometNumberMod);

			LoadVariableFromFile(Buffer, parse, temp.useShipPreMods_All);
			LoadVariableFromFile(Buffer, parse, temp.probShipPreMod_All);
			LoadListFromFile(Buffer, parse, temp.ShipPreMods_All);
			LoadVariableFromFile(Buffer, parse, temp.useShipPostMods_All);
			LoadVariableFromFile(Buffer, parse, temp.probShipPostMod_All);
			LoadListFromFile(Buffer, parse, temp.ShipPostMods_All);

			LoadVariableFromFile(Buffer, parse, temp.useShipPreMods_Colony);
			LoadVariableFromFile(Buffer, parse, temp.probShipPreMod_Colony);
			LoadListFromFile(Buffer, parse, temp.ShipPreMods_Colony);
			LoadVariableFromFile(Buffer, parse, temp.useShipPostMods_Colony);
			LoadVariableFromFile(Buffer, parse, temp.probShipPostMod_Colony);
			LoadListFromFile(Buffer, parse, temp.ShipPostMods_Colony);
			LoadVariableFromFile(Buffer, parse, temp.useShipNumberMods_Colony);
			LoadVariableFromFile(Buffer, parse, temp.probShipNumberMod_Colony);

			LoadVariableFromFile(Buffer, parse, temp.useShipPreMods_Instrument);
			LoadVariableFromFile(Buffer, parse, temp.probShipPreMod_Instrument);
			LoadListFromFile(Buffer, parse, temp.ShipPreMods_Instrument);
			LoadVariableFromFile(Buffer, parse, temp.useShipPostMods_Instrument);
			LoadVariableFromFile(Buffer, parse, temp.probShipPostMod_Instrument);
			LoadListFromFile(Buffer, parse, temp.ShipPostMods_Instrument);
			LoadVariableFromFile(Buffer, parse, temp.useShipNumberMods_Instrument);
			LoadVariableFromFile(Buffer, parse, temp.probShipNumberMod_Instrument);

			LoadVariableFromFile(Buffer, parse, temp.useShipPreMods_Satellite);
			LoadVariableFromFile(Buffer, parse, temp.probShipPreMod_Satellite);
			LoadListFromFile(Buffer, parse, temp.ShipPreMods_Satellite);
			LoadVariableFromFile(Buffer, parse, temp.useShipPostMods_Satellite);
			LoadVariableFromFile(Buffer, parse, temp.probShipPostMod_Satellite);
			LoadListFromFile(Buffer, parse, temp.ShipPostMods_Satellite);
			LoadVariableFromFile(Buffer, parse, temp.useShipNumberMods_Satellite);
			LoadVariableFromFile(Buffer, parse, temp.probShipNumberMod_Satellite);

			LoadVariableFromFile(Buffer, parse, temp.useShipPreMods_Station);
			LoadVariableFromFile(Buffer, parse, temp.probShipPreMod_Station);
			LoadListFromFile(Buffer, parse, temp.ShipPreMods_Station);
			LoadVariableFromFile(Buffer, parse, temp.useShipPostMods_Station);
			LoadVariableFromFile(Buffer, parse, temp.probShipPostMod_Station);
			LoadListFromFile(Buffer, parse, temp.ShipPostMods_Station);
			LoadVariableFromFile(Buffer, parse, temp.useShipNumberMods_Station);
			LoadVariableFromFile(Buffer, parse, temp.probShipNumberMod_Station);

			LoadVariableFromFile(Buffer, parse, temp.order);
			LoadVariableFromFile(Buffer, parse, temp.wordVarience);
			LoadVariableFromFile(Buffer, parse, temp.min_length);
			LoadVariableFromFile(Buffer, parse, temp.max_length);
			LoadListFromFile(Buffer, parse, temp.Markov_RawDataset);

			npreset.push_back(temp);
			SendMessage(CONFIG_H.namePresetDropDown.HANDLE, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp.name);
			CloseHandle(presetFile);
		}

		SendMessage(CONFIG_H.namePresetDropDown.HANDLE, (UINT)CB_SETCURSEL, (WPARAM)0, NULL);

		CONFIG_H.namePresetDropDown.EXTRA = CreateWindowW(L"button", L"Update",
			WS_CHILD | WS_BORDER,
			945, 278, 50, 50,
			hWnd, (HMENU)BUTTON_NAME_UPDATE, NULL, NULL);
	}

	//-------------------------------------
	void LoadMainScreen(HWND hWnd)
	{
		//Loads the main Logo Image
		//###############################################################################
		Handle_Logo = CreateWindowW(L"static", NULL, //type and Text Field
			WS_VISIBLE | WS_CHILD | SS_BITMAP, //Effects
			0, 0, 340, 280, //x, y, Width & Height
			hWnd, NULL, NULL, NULL); // Parent
		SendMessageW(Handle_Logo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)Handle_Image_Logo);
		//###############################################################################

		//###############################################################################
			#pragma region Main Buttons
		//###############################################################################

			// Stuff for tabs if I want to come back to it one day
			/*
			wchar_t tabGeneral[16] = L"General";
			wchar_t tabSystem[16] = L"System / Planet";
			wchar_t tabLife[16] = L"Life";
			wchar_t tabShips[16] = L"Ships";
			wchar_t tabExotic[16] = L"Exotic";
			wchar_t tabAdvanced[16] = L"Advanced";

			TCITEM tabStruct;
			tabStruct.mask = TCIF_TEXT;

			CONFIG_H.tabH = CreateWindow(WC_TABCONTROL, L"",
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | TCS_FIXEDWIDTH,
				340, 280, 690, 400,
				hWnd, NULL, hInst, NULL);
			TabCtrl_SetItemSize(CONFIG_H.tabH, 114, 50);

			tabStruct.pszText = tabGeneral;
			TabCtrl_InsertItem(CONFIG_H.tabH, 0, &tabStruct);
			tabStruct.pszText = tabSystem;
			TabCtrl_InsertItem(CONFIG_H.tabH, 1, &tabStruct);
			tabStruct.pszText = tabLife;
			TabCtrl_InsertItem(CONFIG_H.tabH, 2, &tabStruct);
			tabStruct.pszText = tabShips;
			TabCtrl_InsertItem(CONFIG_H.tabH, 3, &tabStruct);
			tabStruct.pszText = tabExotic;
			TabCtrl_InsertItem(CONFIG_H.tabH, 4, &tabStruct);
			tabStruct.pszText = tabAdvanced;
			TabCtrl_InsertItem(CONFIG_H.tabH, 5, &tabStruct);
			*/

			//Loads all the buttons at the top of the page

		CreateWindowW(L"button", L"General",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			340, 0, 114, 50, 
			hWnd, (HMENU)BUTTON_GENERAL, NULL, NULL);
		CreateWindowW(L"button", L"System",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			454, 0, 114, 50,
			hWnd, (HMENU)BUTTON_SYSTEM, NULL, NULL);
		CreateWindowW(L"button", L"Planet",
			WS_VISIBLE | WS_CHILD | WS_BORDER, 
			568, 0, 114, 50,
			hWnd, (HMENU)BUTTON_PLANET, NULL, NULL);
		CreateWindowW(L"button", L"Surface",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			682, 0, 114, 50,
			hWnd, (HMENU)BUTTON_SURFACE, NULL, NULL);
		CreateWindowW(L"button", L"Special",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			796, 0, 114, 50,
			hWnd, (HMENU)BUTTON_SPECIAL, NULL, NULL);
		CreateWindowW(L"button", L"Advanced",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			910, 0, 114, 50,
			hWnd, (HMENU)BUTTON_ADVANCED, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region Info Panel & Generate Button
		//###############################################################################

		CreateWindowW(L"static", L"",
			WS_VISIBLE | WS_CHILD,
			0, 280, 340, 402,
			hWnd, NULL, NULL, NULL);
		CreateWindowW(L"static", L"Info",
			WS_VISIBLE | WS_CHILD,
			150, 300, 40, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.INFO_BOX = CreateWindowW(L"static", L"Welcome to Seth Fusion's Solar System Generator! Navigate through variables using the tabs at the top of the screen, or hit the Generate button to get started right away!",
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
			10, 330, 320, 250,
			hWnd, NULL, NULL, NULL);
		CreateWindowW(L"button", L"Generate",
			WS_VISIBLE | WS_CHILD | WS_BORDER,
			50, 600, 240, 50,
			hWnd, (HMENU)BUTTON_GENERATE, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region General Screen Handles
		//###############################################################################

		CONFIG_H.HEADER_GENERAL = CreateWindowW(L"static", L"General Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//debug
		CONFIG_H.debug.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | BS_AUTOCHECKBOX,
			100, 100, 100, 20,
			hWnd, (HMENU)CB_DEBUG, NULL, NULL);

		CONFIG_H.seed.DESC = CreateWindowW(L"static", L"Seed:",
			WS_CHILD | WS_BORDER,
			370, 100, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.seed.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 100, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.seed.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 102, 16, 16,
			hWnd, (HMENU)IB_SEED, NULL, NULL);

		CONFIG_H.numberOfRuns.DESC = CreateWindowW(L"static", L"# of Systems to Generate:",
			WS_CHILD | WS_BORDER,
			370, 120, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.numberOfRuns.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER | ES_RIGHT,
			600, 120, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.numberOfRuns.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 122, 16, 16,
			hWnd, (HMENU)IB_NUMBEROFRUNS, NULL, NULL);

		//star output folder
		CONFIG_H.starOutputFolder.DESC = CreateWindowW(L"static", L"Star Output Folder:",
			WS_CHILD | WS_BORDER,
			370, 180, 150, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.starOutputFolder.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			520, 180, 350, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.starOutputFolder.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 182, 16, 16,
			hWnd, (HMENU)IB_STAROUTPUTFOLDER, NULL, NULL);
		CONFIG_H.starOutputFolder.EXTRA = CreateWindowW(L"button", L"Browse...",
			WS_CHILD | WS_BORDER,
			870, 180, 80, 20,
			hWnd, (HMENU)BUTTON_BROWSE_STAR_FOLDER, NULL, NULL);

		//planet output folder
		CONFIG_H.planetOutputFolder.DESC = CreateWindowW(L"static", L"Planet Output Folder:",
			WS_CHILD | WS_BORDER,
			370, 200, 150, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetOutputFolder.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			520, 200, 350, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetOutputFolder.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 202, 16, 16,
			hWnd, (HMENU)IB_PLANETOUTPUTFOLDER, NULL, NULL);
		CONFIG_H.planetOutputFolder.EXTRA = CreateWindowW(L"button", L"Browse...",
			WS_CHILD | WS_BORDER,
			870, 200, 80, 20,
			hWnd, (HMENU)BUTTON_BROWSE_PLANET_FOLDER, NULL, NULL);

		//Preset handles are in the Load Presets function!

		CONFIG_H.savePresetButton.DESC = CreateWindowW(L"static", L"Save a Preset...",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
			370, 350, 180, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.savePresetButton.HANDLE = CreateWindowW(L"edit", L"New Preset",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			370, 370, 200, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.savePresetButton.EXTRA = CreateWindowW(L"button", L"Save",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			575, 348, 50, 50,
			hWnd, (HMENU)BUTTON_SAVEPRESET, NULL, NULL);
		CONFIG_H.savePresetButton.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			552, 352, 16, 16,
			hWnd, (HMENU)IB_SAVEPRESET, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region System Screen Handles
		//###############################################################################
		CONFIG_H.HEADER_SYSTEM = CreateWindowW(L"static", L"System Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//min and max distance
		CONFIG_H.minDistance.DESC = CreateWindowW(L"static", L"Minimum Distance:",
			WS_CHILD | WS_BORDER,
			370, 100, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minDistance.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 100, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxDistance.DESC = CreateWindowW(L"static", L"Maximum Distance:",
			WS_CHILD | WS_BORDER,
			370, 120, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxDistance.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 120, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxDistance.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 112, 16, 16,
			hWnd, (HMENU)IB_DISTANCE, NULL, NULL);

		//system radius modifier
		CONFIG_H.systemRadiusModifier.DESC = CreateWindowW(L"static", L"System Radius Modifier:",
			WS_CHILD | WS_BORDER,
			370, 200, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.systemRadiusModifier.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 200, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.systemRadiusModifier.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 202, 16, 16,
			hWnd, (HMENU)IB_SYSTEMRADIUSMODIFIER, NULL, NULL);

		//min planet number
		CONFIG_H.minPlanetNumber.DESC = CreateWindowW(L"static", L"Min % of System Filled:",
			WS_CHILD | WS_BORDER,
			370, 220, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minPlanetNumber.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 220, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minPlanetNumber.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 222, 16, 16,
			hWnd, (HMENU)IB_MINPLANETNUMBER, NULL, NULL);

		//planet spacing stuff
		CONFIG_H.planetSpaceAvg.DESC = CreateWindowW(L"static", L"Planet Spacing Average:",
			WS_CHILD | WS_BORDER,
			370, 240, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetSpaceAvg.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 240, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetSpaceSD.DESC = CreateWindowW(L"static", L"Planet Spacing Standard Dev:",
			WS_CHILD | WS_BORDER,
			370, 260, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetSpaceSD.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 260, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.planetSpaceAvg.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 252, 16, 16,
			hWnd, (HMENU)IB_PLANETSPACING, NULL, NULL);

		//smart placement
		CONFIG_H.smartPlacement.DESC = CreateWindowW(L"static", L"Smart Placement:",
			WS_CHILD | WS_BORDER,
			370, 280, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.smartPlacement.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 280, 100, 20,
			hWnd, (HMENU)CB_SMARTPLACEMENT, NULL, NULL);
		CONFIG_H.smartPlacement.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 282, 16, 16,
			hWnd, (HMENU)IB_SMARTPLACEMENT, NULL, NULL);

		//generate dwarf planets
		CONFIG_H.generateDwarfPlanets.DESC = CreateWindowW(L"static", L"Generate Dwarf Planets:",
			WS_CHILD | WS_BORDER,
			370, 300, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.generateDwarfPlanets.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 300, 100, 20,
			hWnd, (HMENU)CB_GENERATEDWARFPLANET, NULL, NULL);
		CONFIG_H.generateDwarfPlanets.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 302, 16, 16,
			hWnd, (HMENU)IB_GENERATEDWARFPLANET, NULL, NULL);

		//dwarf planet chance
		CONFIG_H.dwarfPlanetChance.DESC = CreateWindowW(L"static", L"Dwarf Planet % Chance:",
			WS_CHILD | WS_BORDER,
			370, 320, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.dwarfPlanetChance.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 320, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.dwarfPlanetChance.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 322, 16, 16,
			hWnd, (HMENU)IB_DWARFPALNETCHANCE, NULL, NULL);

		//obliquity stuff
		CONFIG_H.avgObliquity.DESC = CreateWindowW(L"static", L"Axial Tilt Average:",
			WS_CHILD | WS_BORDER,
			370, 360, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.avgObliquity.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 360, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDObliquity.DESC = CreateWindowW(L"static", L"Axial Tilt Standard Deviation:",
			WS_CHILD | WS_BORDER,
			370, 380, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDObliquity.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 380, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDObliquity.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 372, 16, 16,
			hWnd, (HMENU)IB_OBLIQUITY, NULL, NULL);

		//inclination stuff
		CONFIG_H.avgInclination.DESC = CreateWindowW(L"static", L"Inclination Average:",
			WS_CHILD | WS_BORDER,
			370, 400, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.avgInclination.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 400, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDInclination.DESC = CreateWindowW(L"static", L"Inclination Standard Deviation:",
			WS_CHILD | WS_BORDER,
			370, 420, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDInclination.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 420, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDInclination.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 412, 16, 16,
			hWnd, (HMENU)IB_INCLINATION, NULL, NULL);

		//eccentricity stuff
		CONFIG_H.avgEccentricity.DESC = CreateWindowW(L"static", L"Eccentricity Average:",
			WS_CHILD | WS_BORDER,
			370, 440, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.avgEccentricity.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 440, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDEccentricity.DESC = CreateWindowW(L"static", L"Eccentricity Standard Deviation:",
			WS_CHILD | WS_BORDER,
			370, 460, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDEccentricity.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 460, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.SDEccentricity.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 454, 16, 16,
			hWnd, (HMENU)IB_ECCENTRICITY, NULL, NULL);

		//generate asteroids
		CONFIG_H.generateAsteroidBelt.DESC = CreateWindowW(L"static", L"Generate Asteroid Belt:",
			WS_CHILD | WS_BORDER,
			370, 500, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.generateAsteroidBelt.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 500, 100, 20,
			hWnd, (HMENU)CB_GENERATEASTEROIDS, NULL, NULL);
		CONFIG_H.generateAsteroidBelt.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 502, 16, 16,
			hWnd, (HMENU)IB_GENERATEASTEROIDS, NULL, NULL);

		//max asteroid belts
		CONFIG_H.maxAsteroidBelts.DESC = CreateWindowW(L"static", L"Maximum Asteroid Belts:",
			WS_CHILD | WS_BORDER,
			370, 520, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxAsteroidBelts.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 520, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxAsteroidBelts.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 522, 16, 16,
			hWnd, (HMENU)IB_MAXASTEROIDBELTS, NULL, NULL);

		//min/max asteroid count
		CONFIG_H.minAsteroidCount.DESC = CreateWindowW(L"static", L"Minimum Asteroids Per Belt:",
			WS_CHILD | WS_BORDER,
			370, 540, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minAsteroidCount.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 540, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxAsteroidCount.DESC = CreateWindowW(L"static", L"Maximum Asteroids Per Belt:",
			WS_CHILD | WS_BORDER,
			370, 560, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxAsteroidCount.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 560, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minAsteroidCount.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 552, 16, 16,
			hWnd, (HMENU)IB_ASTEROIDCOUNT, NULL, NULL);

		//generate comets
		CONFIG_H.generateComets.DESC = CreateWindowW(L"static", L"Generate Comets:",
			WS_CHILD | WS_BORDER,
			370, 580, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.generateComets.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 580, 100, 20,
			hWnd, (HMENU)CB_GENERATECOMETS, NULL, NULL);
		CONFIG_H.generateComets.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 582, 16, 16,
			hWnd, (HMENU)IB_GENERATECOMETS, NULL, NULL);

		//min/max asteroid count
		CONFIG_H.minCometCount.DESC = CreateWindowW(L"static", L"Minimum Comets:",
			WS_CHILD | WS_BORDER,
			370, 600, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minCometCount.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 600, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxCometCount.DESC = CreateWindowW(L"static", L"Maximum Comets:",
			WS_CHILD | WS_BORDER,
			370, 620, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.maxCometCount.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 620, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.minCometCount.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 612, 16, 16,
			hWnd, (HMENU)IB_COMETCOUNT, NULL, NULL);

		// Trackbar stuff
		/*
		CONFIG_H.avgEccentricity.HANDLE = CreateWindowW(TRACKBAR_CLASS, L"Track Test",
			WS_CHILD | WS_BORDER | TBS_NOTICKS,
			600, 400, 100, 20,
			hWnd, NULL, NULL, NULL);
		SendMessage(CONFIG_H.avgEccentricity.HANDLE, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 9999));
		SendMessage(CONFIG_H.avgEccentricity.HANDLE, TBM_SETPAGESIZE, 0, (LPARAM)100);
		CONFIG_H.avgEccentricity.EXTRA = CreateWindowW(L"static", L"",
			WS_CHILD | WS_VISIBLE | SS_RIGHT,
			0, 0, 70, 20,
			hWnd, NULL, NULL, NULL);
		SendMessage(CONFIG_H.avgEccentricity.HANDLE, TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)CONFIG_H.avgEccentricity.EXTRA);
		*/

		//Star class Stuff
		//I used the O class's EXTRA to create the groupbox for all the classes,
		//therefore, it is the parent for all the class checkboxes
		CONFIG_H.starClassO.EXTRA = CreateWindowW(L"button", L"Star Class Weights:",
			WS_CHILD | BS_GROUPBOX,
			750, 100, 250, 170,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.starClassO.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			960, 102, 16, 16,
			hWnd, (HMENU)IB_STARCLASS, NULL, NULL);


		CONFIG_H.starClassO.DESC = CreateWindowW(L"static", L"O:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 20, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassO.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 20, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassB.DESC = CreateWindowW(L"static", L"B:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 40, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassB.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 40, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassA.DESC = CreateWindowW(L"static", L"A:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 60, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassA.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 60, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassF.DESC = CreateWindowW(L"static", L"F:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 80, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassF.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 80, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassG.DESC = CreateWindowW(L"static", L"G:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 100, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassG.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 100, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassK.DESC = CreateWindowW(L"static", L"K:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 120, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassK.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 120, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassM.DESC = CreateWindowW(L"static", L"M:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			10, 140, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassM.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			50, 140, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassWD.DESC = CreateWindowW(L"static", L"WD:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			120, 20, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassWD.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			160, 20, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassQ.DESC = CreateWindowW(L"static", L"Q:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			120, 40, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassQ.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			160, 40, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		CONFIG_H.starClassX.DESC = CreateWindowW(L"static", L"X:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
			120, 60, 40, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);
		CONFIG_H.starClassX.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
			160, 60, 60, 20,
			CONFIG_H.starClassO.EXTRA, NULL, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region Planet Screen Handles
		//###############################################################################

			CONFIG_H.HEADER_PLANET = CreateWindowW(L"static", L"Planet Variables",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			//weighted moons
			CONFIG_H.weightedMoons.DESC = CreateWindowW(L"static", L"Weighted Moons:",
				WS_CHILD | WS_BORDER,
				370, 140, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.weightedMoons.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 140, 100, 20,
				hWnd, (HMENU)CB_WEIGHTEDMOONS, NULL, NULL);
			CONFIG_H.weightedMoons.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 142, 16, 16,
				hWnd, (HMENU)IB_WEIGHTEDMOONS, NULL, NULL);

			// moon distance boundary
			CONFIG_H.moonDistanceBoundary.DESC = CreateWindowW(L"static", L"Moon Distance Boundary:",
				WS_CHILD | WS_BORDER,
				370, 160, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.moonDistanceBoundary.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 160, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.moonDistanceBoundary.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 162, 16, 16,
				hWnd, (HMENU)IB_MOONDISTANCEBOUNDARY, NULL, NULL);

			//High Orbit Chance
			CONFIG_H.exotic_OrbitChance.DESC = CreateWindowW(L"static", L"% Chance for Exotic Orbit:",
				WS_CHILD | WS_BORDER,
				370, 200, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_OrbitChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 200, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_OrbitChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 202, 16, 16,
				hWnd, (HMENU)IB_EXOTICORBIT, NULL, NULL);

			//High Obliquity Chance
			CONFIG_H.exotic_AxialTiltChance.DESC = CreateWindowW(L"static", L"% Chance for Exotic Axial Tilt:",
				WS_CHILD | WS_BORDER,
				370, 220, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_AxialTiltChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 220, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_AxialTiltChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 222, 16, 16,
				hWnd, (HMENU)IB_EXOTICOBLIQUITY, NULL, NULL);

			//Companion Orbit Chance
			CONFIG_H.exotic_CompanionOrbitChance.DESC = CreateWindowW(L"static", L"% Chance for Comapnion Orbit:",
				WS_CHILD | WS_BORDER,
				370, 240, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_CompanionOrbitChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 240, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_CompanionOrbitChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 242, 16, 16,
				hWnd, (HMENU)IB_EXOTICCOMPANION, NULL, NULL);

			//Debris Ring Chance
			CONFIG_H.exotic_DebrisRingChance.DESC = CreateWindowW(L"static", L"% Chance for Debris Ring:",
				WS_CHILD | WS_BORDER,
				370, 260, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_DebrisRingChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 260, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.exotic_DebrisRingChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 262, 16, 16,
				hWnd, (HMENU)IB_EXOTICDEBRISRING, NULL, NULL);

			//Organic Chance
			CONFIG_H.life_OrganicChance.DESC = CreateWindowW(L"static", L"% Chance for Organic Life:",
				WS_CHILD | WS_BORDER,
				370, 300, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_OrganicChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 300, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_OrganicChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 302, 16, 16,
				hWnd, (HMENU)IB_LIFEORGANICCHANCE, NULL, NULL);

			//Exotic Chance
			CONFIG_H.life_ExoticChance.DESC = CreateWindowW(L"static", L"% Chance for Exotic Life:",
				WS_CHILD | WS_BORDER,
				370, 320, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_ExoticChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 320, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_ExoticChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 322, 16, 16,
				hWnd, (HMENU)IB_LIFEEXOTICCHANCE, NULL, NULL);

			//MultiCell Chance
			CONFIG_H.life_MulticellChance.DESC = CreateWindowW(L"static", L"% Chance for Multicellular Life:",
				WS_CHILD | WS_BORDER,
				370, 340, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_MulticellChance.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
				600, 340, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.life_MulticellChance.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 342, 16, 16,
				hWnd, (HMENU)IB_LIFEMULTICHANCE, NULL, NULL);

			//Traditional Life
			CONFIG_H.traditionalLife.DESC = CreateWindowW(L"static", L"Traditional Life:",
				WS_CHILD | WS_BORDER,
				370, 360, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.traditionalLife.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 360, 100, 20,
				hWnd, (HMENU)CB_TRADITIONALLIFE, NULL, NULL);
			CONFIG_H.traditionalLife.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 362, 16, 16,
				hWnd, (HMENU)IB_TRADITIONALLIFE, NULL, NULL);

			//Force Life
			CONFIG_H.forceLife.DESC = CreateWindowW(L"static", L"Force Life:",
				WS_CHILD | WS_BORDER,
				370, 380, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG_H.forceLife.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 380, 100, 20,
				hWnd, (HMENU)CB_FORCELIFE, NULL, NULL);
			CONFIG_H.forceLife.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				352, 382, 16, 16,
				hWnd, (HMENU)IB_FORCELIFE, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region Surface Screen Handles
		//###############################################################################

		CONFIG_H.HEADER_SURFACE = CreateWindowW(L"static", L"Surface Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//generate composition
		CONFIG_H.generateComposition.DESC = CreateWindowW(L"static", L"Generate Composition:",
			WS_CHILD | WS_BORDER,
			370, 100, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.generateComposition.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 100, 100, 20,
			hWnd, (HMENU)CB_GENERATECOMPOSITION, NULL, NULL);
		CONFIG_H.generateComposition.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 102, 16, 16,
			hWnd, (HMENU)IB_GENERATECOMPOSITION, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region Special Screen Handles
		//###############################################################################

		CONFIG_H.HEADER_SPECIAL = CreateWindowW(L"static", L"Special Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//Models folder
		CONFIG_H.modelsFolder.DESC = CreateWindowW(L"static", L"Models Folder Path:",
			WS_CHILD | WS_BORDER,
			370, 100, 150, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.modelsFolder.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			520, 100, 350, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.modelsFolder.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 102, 16, 16,
			hWnd, (HMENU)IB_MODELSFOLDER, NULL, NULL);
		CONFIG_H.modelsFolder.EXTRA = CreateWindowW(L"button", L"Browse...",
			WS_CHILD | WS_BORDER,
			870, 100, 80, 20,
			hWnd, (HMENU)BUTTON_BROWSE_MODELS_FOLDER, NULL, NULL);

		//Ship Chance
		CONFIG_H.exotic_ShipChance.DESC = CreateWindowW(L"static", L"% Chance for Ships to Spawn:",
			WS_CHILD | WS_BORDER,
			370, 120, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.exotic_ShipChance.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT | ES_NUMBER,
			600, 120, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.exotic_ShipChance.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 122, 16, 16,
			hWnd, (HMENU)IB_SHIPCHANCE, NULL, NULL);

		//Ships Need Life
		CONFIG_H.shipsNeedLife.DESC = CreateWindowW(L"static", L"Ships Need Life to Spawn:",
			WS_CHILD | WS_BORDER,
			370, 140, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG_H.shipsNeedLife.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 140, 100, 20,
			hWnd, (HMENU)CB_SHIPSNEEDLIFE, NULL, NULL);
		CONFIG_H.shipsNeedLife.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			352, 142, 16, 16,
			hWnd, (HMENU)IB_SHIPSNEEDLIFE, NULL, NULL);

		#pragma endregion
		//###############################################################################

		//###############################################################################
			#pragma region Advanced Screen Handles
		//###############################################################################

			CONFIG_H.HEADER_ADVANCED = CreateWindowW(L"static", L"Advanced Variables:",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			//###############################################################################
				#pragma region Name Stuff
			//###############################################################################
			
				TCITEM tab;
				tab.mask = TCIF_TEXT;

				/*=====================================================
					Tab groups are declared in order from 
					deepest layer to outmost layer. This is because
					either Win32 is stupid, or I am. So the ship
					tab group is the first, followed by the name
					inner & advanced inner. Outmost tabs are last.
				=====================================================*/

				// tab to hold the ship buttons
				NV.Tab_Ship_Inner = CreateWindowW(WC_TABCONTROL, L"",
					WS_CHILD,
					360, 170, 645, 490,
					hWnd, (HMENU)TABGROUP_SHIP_INNER, hInst, NULL);
				// insert main tabs
				TCHAR tab11[32] = L"All Ship Names";
				TCHAR tab12[32] = L"Colony";
				TCHAR tab13[32] = L"Instrument";
				TCHAR tab14[32] = L"Satellite";
				TCHAR tab15[32] = L"Station";
				tab.pszText = tab11;
				TabCtrl_InsertItem(NV.Tab_Ship_Inner, TAB_SHIP_ALL, &tab);
				tab.pszText = tab12;
				TabCtrl_InsertItem(NV.Tab_Ship_Inner, TAB_SHIP_COLONY, &tab);
				tab.pszText = tab13;
				TabCtrl_InsertItem(NV.Tab_Ship_Inner, TAB_SHIP_INSTRUMENT, &tab);
				tab.pszText = tab14;
				TabCtrl_InsertItem(NV.Tab_Ship_Inner, TAB_SHIP_SATELLITE, &tab);
				tab.pszText = tab15;
				TabCtrl_InsertItem(NV.Tab_Ship_Inner, TAB_SHIP_STATION, &tab);


				NV.Tab_Name_Inner = CreateWindowW(WC_TABCONTROL, L"",
					WS_CHILD | TCS_MULTILINE,
					355, 125, 655, 540,
					hWnd, (HMENU)TABGROUP_NAME_INNER, hInst, NULL);
				//insterts tabs into name variables
				TCHAR tab2[32] = L"Name Dataset";
				TCHAR tab3[32] = L"Star Names";
				TCHAR tab4[32] = L"Planet Names";
				TCHAR tab5[32] = L"Moon Names";
				TCHAR tab6[32] = L"Ship Names";
				TCHAR tab7[32] = L"DwarfMoon Names";
				TCHAR tab8[32] = L"Asteroid Names";
				TCHAR tab9[32] = L"Comet Names";				
				TCHAR tab10[32] = L"Simple Name Gen";
				tab.pszText = tab2;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_DATASET, &tab);
				tab.pszText = tab3;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_STAR, &tab);
				tab.pszText = tab4;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_PLANET, &tab);
				tab.pszText = tab5;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_MOON, &tab);
				tab.pszText = tab6;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_SHIP, &tab);
				tab.pszText = tab7;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_DWARFMOON, &tab);
				tab.pszText = tab8;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_ASTEROID, &tab);			
				tab.pszText = tab9;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_COMET, &tab);
				tab.pszText = tab10;
				TabCtrl_InsertItem(NV.Tab_Name_Inner, TAB_NAME_SIMPLE, &tab);
				
			
				// tab to hold the outer buttons
				CONFIG.Tab_Advanced_Outer = CreateWindowW(WC_TABCONTROL, L"",
					WS_CHILD | TCS_MULTILINE | TCS_RIGHTJUSTIFY,
					350, 100, 665, 570,
					hWnd, (HMENU)TABGROUP_ADVANCED_OUTER, hInst, NULL);
				// insert main tabs
				TCHAR tab0[32] = L"Name Variables";
				TCHAR tab1[32] = L"Advanced Variables";
				tab.pszText = tab0;
				TabCtrl_InsertItem(CONFIG.Tab_Advanced_Outer, TAB_NAMES, &tab);
				tab.pszText = tab1;
				TabCtrl_InsertItem(CONFIG.Tab_Advanced_Outer, TAB_ADVANCED, &tab);

				//###############################################################################
					#pragma region Name Inner Tab

					// Info buttons for the pre/post/number mods, which are the same for
					// each name group
					NV.PreMod_INFO.DESC = CreateWindowW(L"static", L"Pre Modification",
						WS_CHILD | WS_BORDER,
						380, 325, 170, 20,
						hWnd, NULL, NULL, NULL);
					NV.PreMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"INFO",
						WS_CHILD | WS_BORDER,
						500, 327, 46, 16,
						hWnd, (HMENU)IB_NAMEPREMODS, NULL, NULL);
					NV.PostMod_INFO.DESC = CreateWindowW(L"static", L"Post Modification",
						WS_CHILD | WS_BORDER,
						550, 325, 170, 20,
						hWnd, NULL, NULL, NULL);
					NV.PostMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"INFO",
						WS_CHILD | WS_BORDER,
						670, 327, 46, 16,
						hWnd, (HMENU)IB_NAMEPOSTMODS, NULL, NULL);
					NV.NumberMod_INFO.DESC = CreateWindowW(L"static", L"Num Modification",
						WS_CHILD | WS_BORDER,
						720, 325, 170, 20,
						hWnd, NULL, NULL, NULL);
					NV.NumberMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"INFO",
						WS_CHILD | WS_BORDER,
						840, 327, 46, 16,
						hWnd, (HMENU)IB_NAMENUMBERMODS, NULL, NULL);

					// Info buttons for the pre / post name lists, which are the same for
					// each name group
					NV.PreMods_List.DESC = CreateWindowW(L"static", L"Pre Modification List:",
						WS_CHILD | WS_BORDER | SS_CENTER,
						380, 350, 300, 20,
						hWnd, NULL, NULL, NULL);
					NV.PreMods_List.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						662, 352, 16, 16,
						hWnd, (HMENU)IB_NAMEPREMODLIST, NULL, NULL);
					NV.PostMods_List.DESC = CreateWindowW(L"static", L"Post Modification List:",
						WS_CHILD | WS_BORDER | SS_CENTER,
						680, 350, 300, 20,
						hWnd, NULL, NULL, NULL);
					NV.PostMods_List.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						962, 352, 16, 16,
						hWnd, (HMENU)IB_NAMEPOSTMODLIST, NULL, NULL);

					//###############################################################################
						#pragma region Dataset Group
					//###############################################################################

					NV.GROUP_DATASET = CreateWindowW(L"button", L"Markov Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// Update names button
					NV.buttonUpdate = CreateWindowW(L"button", L"Update Names",
						WS_CHILD | WS_BORDER | BS_MULTILINE,
						880, 260, 80, 40,
						hWnd, (HMENU)BUTTON_NAME_LOAD_VECTORS, NULL, NULL);

					// save preset stuff
					// parent is hWnd, but loads with dataset page
					NV.saveNamePresetButton.DESC = CreateWindowW(L"static", L"Save Name Preset...",
						WS_CHILD | WS_BORDER,
						820, 180, 140, 20,
						hWnd, NULL, NULL, NULL);
					NV.saveNamePresetButton.HANDLE = CreateWindowW(L"edit", L"New Name Preset",
						WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
						820, 200, 160, 20,
						hWnd, NULL, NULL, NULL);
					NV.saveNamePresetButton.EXTRA = CreateWindowW(L"button", L"Save",
						WS_CHILD | WS_BORDER,
						822, 222, 156, 20,
						hWnd, (HMENU)BUTTON_NAME_SAVEPRESET, NULL, NULL);
					NV.saveNamePresetButton.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						962, 182, 16, 16,
						hWnd, (HMENU)IB_SAVENAMEPRESET, NULL, NULL);

					NV.Markov_INFO = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						500, 242, 16, 16,
						hWnd, (HMENU)IB_MARKOV, NULL, NULL);

					// The word percent variable is shown on both the markov screen and the simple generator screen
					// Therefore, it's parent is hWnd. It is only declared here because there is no group to put it under
					NV.wordPercentH.DESC = CreateWindowW(L"static", L"Multi Word Percent:",
						WS_CHILD | WS_BORDER,
						560, 220, 140, 20,
						hWnd, NULL, NULL, NULL);
					NV.wordPercentH.HANDLE = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_BORDER | ES_RIGHT | ES_NUMBER,
						700, 220, 50, 20,
						hWnd, NULL, NULL, NULL);
					NV.wordPercentH.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						752, 222, 16, 16,
						hWnd, (HMENU)IB_WORDPERCENT, NULL, NULL);

					// The word percent extra is used for the test names button. It's parent is also hWnd
					// because it sends a message to the main windows procedure, but visually it only appears
					// under the markov screen
					NV.wordPercentH.EXTRA = CreateWindowW(L"button", L"Test Names",
						WS_CHILD | WS_BORDER | BS_MULTILINE,
						760, 260, 80, 40,
						hWnd, (HMENU)BUTTON_TEST_NAMES, NULL, NULL);

					NV.orderDESC = CreateWindowW(L"static", L"Order:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 20, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.wordVarienceDESC = CreateWindowW(L"static", L"Word Varience:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.min_lengthDESC = CreateWindowW(L"static", L"Min Length:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						200, 20, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.max_lengthDESC = CreateWindowW(L"static", L"Max Length:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						200, 40, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);

					NV.orderH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NUMBER,
						130, 20, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.wordVarienceH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NUMBER,
						130, 40, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.min_lengthH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NUMBER,
						320, 20, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.max_lengthH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT | ES_NUMBER,
						320, 40, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);

					NV.Markov_RawDatasetDESC = CreateWindowW(L"static", L"Dataset:",
						WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
						10, 80, 600, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.Markov_RawDatasetH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 100, 596, 300,
						NV.GROUP_DATASET, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Star Group
					//###############################################################################
					NV.GROUP_STAR = CreateWindowW(L"button", L"Star Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useStarModsDESC = CreateWindowW(L"static", L"Use Star Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.useStarPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARPREMOD, NULL, NULL);
					NV.useStarPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARPOSTMOD, NULL, NULL);
					NV.useStarNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);

					NV.starPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.starPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_STAR, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Planet Group
					//###############################################################################
					NV.GROUP_PLANET = CreateWindowW(L"button", L"Planet Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.usePlanetModsDESC = CreateWindowW(L"static", L"Use Planet Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.usePlanetPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETPREMOD, NULL, NULL);
					NV.usePlanetPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETPOSTMOD, NULL, NULL);
					NV.usePlanetNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);

					NV.planetPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.planetPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_PLANET, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Moon Group
					//###############################################################################

					NV.GROUP_MOON = CreateWindowW(L"button", L"Moon Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameMoonsH.DESC = CreateWindowW(L"static", L"Name Terra / Gas Giant Moons:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 16, 420, 24,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.nameMoonsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 18, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_NAMETERRAMOONS, NULL, NULL);
					NV.nameMoonsH.EXTRA = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 18, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_NAMEGASMOONS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useMoonModsDESC = CreateWindowW(L"static", L"Use Moon Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.useMoonPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONPREMOD, NULL, NULL);
					NV.useMoonPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONPOSTMOD, NULL, NULL);
					NV.useMoonNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);

					NV.moonPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.moonPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_MOON, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Dwarf Moon Group
					//###############################################################################

					NV.GROUP_DWARFMOON = CreateWindowW(L"button", L"Dwarf Moon Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameDwarfMoonsH.DESC = CreateWindowW(L"static", L"Name Terra / Gas Giant Dwarf Moons:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 16, 420, 24,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.nameDwarfMoonsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 18, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_NAMETERRADWARFMOONS, NULL, NULL);
					NV.nameDwarfMoonsH.EXTRA = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 18, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_NAMEGASDWARFMOONS, NULL, NULL);
					NV.nameMoonsH.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						802, 259, 16, 16,
						hWnd, (HMENU)IB_NAMEMOONS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useDwarfMoonModsDESC = CreateWindowW(L"static", L"Use DwarfMoon Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.useDwarfMoonPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONPREMOD, NULL, NULL);
					NV.useDwarfMoonPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONPOSTMOD, NULL, NULL);
					NV.useDwarfMoonNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);

					NV.dwarfMoonPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.dwarfMoonPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Asteroid Group
					//###############################################################################

					NV.GROUP_ASTEROID = CreateWindowW(L"button", L"Asteroid Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameAsteroidsH.DESC = CreateWindowW(L"static", L"Name Asteroids:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 16, 420, 24,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.nameAsteroidsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 18, 20, 20,
						NV.GROUP_ASTEROID, (HMENU)NVCB_NAMEASTEROIDS, NULL, NULL);
					NV.nameAsteroidsH.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						802, 259, 16, 16,
						hWnd, (HMENU)IB_NAMEASTEROIDS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useAsteroidModsDESC = CreateWindowW(L"static", L"Use Asteroid Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.useAsteroidPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_ASTEROID, (HMENU)NVCB_ASTEROIDPREMOD, NULL, NULL);
					NV.useAsteroidPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_ASTEROID, (HMENU)NVCB_ASTEROIDPOSTMOD, NULL, NULL);
					NV.useAsteroidNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_ASTEROID, (HMENU)NVCB_ASTEROIDNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.probAsteroidPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.probAsteroidPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.probAsteroidNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);

					NV.asteroidPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);
					NV.asteroidPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_ASTEROID, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Comet Group
					//###############################################################################

					NV.GROUP_COMET = CreateWindowW(L"button", L"Comet Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameCometsH.DESC = CreateWindowW(L"static", L"Name Comets:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 16, 420, 24,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.nameCometsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 18, 20, 20,
						NV.GROUP_COMET, (HMENU)NVCB_NAMECOMETS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useCometModsDESC = CreateWindowW(L"static", L"Use Comet Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 420, 22,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.useCometPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_COMET, (HMENU)NVCB_COMETPREMOD, NULL, NULL);
					NV.useCometPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_COMET, (HMENU)NVCB_COMETPOSTMOD, NULL, NULL);
					NV.useCometNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
						402, 39, 20, 20,
						NV.GROUP_COMET, (HMENU)NVCB_COMETNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.probCometPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						310, 60, 40, 20,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.probCometPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						350, 60, 40, 20,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.probCometNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER | ES_CENTER,
						390, 60, 40, 20,
						NV.GROUP_COMET, NULL, NULL, NULL);

					NV.cometPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_COMET, NULL, NULL, NULL);
					NV.cometPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_COMET, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Ship_All Group
					//###############################################################################

					NV.GROUP_SHIP_ALL = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_All_DESC = CreateWindowW(L"static", L"Use Pre / Post Mods for All Ships:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 38, 380, 22,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);
					NV.useShipPreMods_AllH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						322, 39, 20, 20,
						NV.GROUP_SHIP_ALL, (HMENU)NVCB_SHIPALLPREMOD, NULL, NULL);
					NV.useShipPostMods_AllH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						362, 39, 20, 20,
						NV.GROUP_SHIP_ALL, (HMENU)NVCB_SHIPALLPOSTMOD, NULL, NULL);

					NV.shipModsProb_All_DESC = CreateWindowW(L"static", L"Probability for Pre / Post Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);
					NV.probShipPreMod_AllH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						310, 60, 40, 20,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);
					NV.probShipPostMod_AllH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						350, 60, 40, 20,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);

					NV.shipPreModList_All = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);
					NV.shipPostModList_All = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_SHIP_ALL, NULL, NULL, NULL);

						//###############################################################################
							#pragma region Ship Types

							//###############################################################################
								#pragma region Ship_Colony Group
							//###############################################################################

							NV.GROUP_SHIP_COLONY = CreateWindowW(L"button", L"Ship Name Variables:",
								WS_CHILD | WS_GROUP | BS_GROUPBOX,
								370, 240, 620, 410,
								hWnd, NULL, NULL, NULL);

							// These handes are initialized as visible because they are
							// shown/hiden with the group
							NV.useShipMods_Colony_DESC = CreateWindowW(L"static", L"Use Colony Pre / Post Mods for Ships:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 38, 420, 22,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);
							NV.useShipPreMods_ColonyH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								322, 39, 20, 20,
								NV.GROUP_SHIP_COLONY, (HMENU)NVCB_SHIPCOLONYPREMOD, NULL, NULL);
							NV.useShipPostMods_ColonyH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								362, 39, 20, 20,
								NV.GROUP_SHIP_COLONY, (HMENU)NVCB_SHIPCOLONYPOSTMOD, NULL, NULL);
							NV.useShipNumberMods_ColonyH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								402, 39, 20, 20,
								NV.GROUP_SHIP_COLONY, (HMENU)NVCB_SHIPCOLONYNUMBERMOD, NULL, NULL);

							NV.shipModsProb_Colony_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 60, 300, 20,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);
							NV.probShipPreMod_ColonyH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								310, 60, 40, 20,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);
							NV.probShipPostMod_ColonyH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								350, 60, 40, 20,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);
							NV.probShipNumberMod_ColonyH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								390, 60, 40, 20,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);

							NV.shipPreModList_Colony = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								12, 130, 296, 266,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);
							NV.shipPostModList_Colony = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								312, 130, 296, 266,
								NV.GROUP_SHIP_COLONY, NULL, NULL, NULL);

								#pragma endregion
							//###############################################################################

							//###############################################################################
								#pragma region Ship_Instrument Group
							//###############################################################################

							NV.GROUP_SHIP_INSTRUMENT = CreateWindowW(L"button", L"Ship Name Variables:",
								WS_CHILD | WS_GROUP | BS_GROUPBOX,
								370, 240, 620, 410,
								hWnd, NULL, NULL, NULL);

							// These handes are initialized as visible because they are
							// shown/hiden with the group
							NV.useShipMods_Instrument_DESC = CreateWindowW(L"static", L"Use Instrument Pre / Post Mods for Ships:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 38, 420, 22,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);
							NV.useShipPreMods_InstrumentH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								322, 39, 20, 20,
								NV.GROUP_SHIP_INSTRUMENT, (HMENU)NVCB_SHIPINSTRUMENTPREMOD, NULL, NULL);
							NV.useShipPostMods_InstrumentH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								362, 39, 20, 20,
								NV.GROUP_SHIP_INSTRUMENT, (HMENU)NVCB_SHIPINSTRUMENTPOSTMOD, NULL, NULL);
							NV.useShipNumberMods_InstrumentH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								402, 39, 20, 20,
								NV.GROUP_SHIP_INSTRUMENT, (HMENU)NVCB_SHIPINSTRUMENTNUMBERMOD, NULL, NULL);

							NV.shipModsProb_Instrument_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 60, 300, 20,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);
							NV.probShipPreMod_InstrumentH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								310, 60, 40, 20,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);
							NV.probShipPostMod_InstrumentH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								350, 60, 40, 20,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);
							NV.probShipNumberMod_InstrumentH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								390, 60, 40, 20,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);

							NV.shipPreModList_Instrument = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								12, 130, 296, 266,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);
							NV.shipPostModList_Instrument = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								312, 130, 296, 266,
								NV.GROUP_SHIP_INSTRUMENT, NULL, NULL, NULL);

								#pragma endregion
							//###############################################################################

							//###############################################################################
								#pragma region Ship_Satellite Group
							//###############################################################################

							NV.GROUP_SHIP_SATELLITE = CreateWindowW(L"button", L"Ship Name Variables:",
								WS_CHILD | WS_GROUP | BS_GROUPBOX,
								370, 240, 620, 410,
								hWnd, NULL, NULL, NULL);

							// These handes are initialized as visible because they are
							// shown/hiden with the group
							NV.useShipMods_Satellite_DESC = CreateWindowW(L"static", L"Use Satellite Pre / Post Mods for Ships:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 38, 420, 22,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);
							NV.useShipPreMods_SatelliteH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								322, 39, 20, 20,
								NV.GROUP_SHIP_SATELLITE, (HMENU)NVCB_SHIPSATELLITEPREMOD, NULL, NULL);
							NV.useShipPostMods_SatelliteH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								362, 39, 20, 20,
								NV.GROUP_SHIP_SATELLITE, (HMENU)NVCB_SHIPSATELLITEPOSTMOD, NULL, NULL);
							NV.useShipNumberMods_SatelliteH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								402, 39, 20, 20,
								NV.GROUP_SHIP_SATELLITE, (HMENU)NVCB_SHIPSATELLITENUMBERMOD, NULL, NULL);

							NV.shipModsProb_Satellite_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 60, 300, 20,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);
							NV.probShipPreMod_SatelliteH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								310, 60, 40, 20,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);
							NV.probShipPostMod_SatelliteH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								350, 60, 40, 20,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);
							NV.probShipNumberMod_SatelliteH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								390, 60, 40, 20,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);

							NV.shipPreModList_Satellite = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								12, 130, 296, 266,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);
							NV.shipPostModList_Satellite = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								312, 130, 296, 266,
								NV.GROUP_SHIP_SATELLITE, NULL, NULL, NULL);

								#pragma endregion
							//###############################################################################

							//###############################################################################
								#pragma region Ship_Station Group
							//###############################################################################

							NV.GROUP_SHIP_STATION = CreateWindowW(L"button", L"Ship Name Variables:",
								WS_CHILD | WS_GROUP | BS_GROUPBOX,
								370, 240, 620, 410,
								hWnd, NULL, NULL, NULL);

							// These handes are initialized as visible because they are
							// shown/hiden with the group
							NV.useShipMods_Station_DESC = CreateWindowW(L"static", L"Use Station Pre / Post Mods for Ships:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 38, 420, 22,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);
							NV.useShipPreMods_StationH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								322, 39, 20, 20,
								NV.GROUP_SHIP_STATION, (HMENU)NVCB_SHIPSTATIONPREMOD, NULL, NULL);
							NV.useShipPostMods_StationH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								362, 39, 20, 20,
								NV.GROUP_SHIP_STATION, (HMENU)NVCB_SHIPSTATIONPOSTMOD, NULL, NULL);
							NV.useShipNumberMods_StationH = CreateWindowW(L"button", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
								402, 39, 20, 20,
								NV.GROUP_SHIP_STATION, (HMENU)NVCB_SHIPSTATIONNUMBERMOD, NULL, NULL);

							NV.shipModsProb_Station_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
								WS_CHILD | WS_VISIBLE | WS_BORDER,
								10, 60, 300, 20,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);
							NV.probShipPreMod_StationH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								310, 60, 40, 20,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);
							NV.probShipPostMod_StationH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								350, 60, 40, 20,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);
							NV.probShipNumberMod_StationH = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
								390, 60, 40, 20,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);

							NV.shipPreModList_Station = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								12, 130, 296, 266,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);
							NV.shipPostModList_Station = CreateWindowW(L"edit", L"",
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
								312, 130, 296, 266,
								NV.GROUP_SHIP_STATION, NULL, NULL, NULL);

								#pragma endregion
							//###############################################################################

							#pragma endregion
						//###############################################################################

						#pragma endregion
					//###############################################################################

					//###############################################################################
						#pragma region Simple Generator Group	
					//###############################################################################

					NV.GROUP_SIMPLE = CreateWindowW(L"button", L"Simple Generator:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.SimpleGenerator_INFO = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						602, 262, 16, 16,
						hWnd, (HMENU)IB_SIMPLEGENERATOR, NULL, NULL);

					NV.useSimpleGeneratorDESC = CreateWindowW(L"static", L"Use Simple Generator:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 20, 200, 20,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);
					NV.useSimpleGeneratorH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						210, 20, 20, 20,
						NV.GROUP_SIMPLE, (HMENU)NVCB_SIMPLEGENERATOR, NULL, NULL);

					NV.PrefixListDESC = CreateWindowW(L"static", L"Prefix List:",
						WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
						10, 60, 600, 20,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);
					NV.PrefixListH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 80, 596, 130,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);

					NV.SuffixListDESC = CreateWindowW(L"static", L"Suffix List:",
						WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
						10, 230, 600, 20,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);
					NV.SuffixListH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 250, 596, 130,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);

						#pragma endregion
					//###############################################################################

					#pragma endregion
				//###############################################################################

				//###############################################################################
					#pragma region Advanced Inner Tab

					#pragma endregion
				//###############################################################################

				//###############################################################################
				#pragma endregion
			//###############################################################################
			#pragma endregion
		//###############################################################################

		// Page Edges
		/*
		CreateWindowW(L"button", L"Bottom Most Edge",
			WS_VISIBLE | WS_CHILD,
			0, 681, 684, 50,
			hWnd, NULL, NULL, NULL);
		CreateWindowW(L"button", L"Right Most Edge",
			WS_VISIBLE | WS_CHILD,
			1024, 0, 200, 50,
			hWnd, NULL, NULL, NULL);
		*/
	}
	//-------------------------------------

	void UpdatePreset(Preset P, HWND hWnd)
	{
		// this little part converts the seed to a wchar array

		wchar_t seedarr[20] = L"";
		std::wstring seedstr = std::to_wstring(P.seed);
		int i = 0;
		while (seedstr[i] != '\0')
		{
			seedarr[i] = seedstr[i];
			i++;
		}
		SetWindowTextW(CONFIG_H.seed.HANDLE, seedarr);

		SetWindowTextW(CONFIG_H.starOutputFolder.HANDLE, P.starOutputFolder);
		SetWindowTextW(CONFIG_H.planetOutputFolder.HANDLE, P.planetOutputFolder);
		SetVariableToWindow(CONFIG_H.numberOfRuns.HANDLE, P.numberOfRuns);
		CheckDlgButton(hWnd, CB_DEBUG, P.debug);

		CheckDlgButton(hWnd, CB_SMARTPLACEMENT, P.smartPlacement);
		SetWindowTextW(CONFIG_H.smartPlacement.HANDLE, P.smartPlacementState);
		SetVariableToWindow(CONFIG_H.systemRadiusModifier.HANDLE, P.systemRadiusModifier);
		SetVariableToWindow(CONFIG_H.minPlanetNumber.HANDLE, P.minPlanetNumber);

		SetVariableToWindow(CONFIG_H.minDistance.HANDLE, P.minDistance);
		SetVariableToWindow(CONFIG_H.maxDistance.HANDLE, P.maxDistance);
		SetVariableToWindow(CONFIG_H.planetSpaceAvg.HANDLE, P.planetSpaceAvg);
		SetVariableToWindow(CONFIG_H.planetSpaceSD.HANDLE, P.planetSpaceSD);
		SetVariableToWindow(CONFIG_H.moonDistanceBoundary.HANDLE, P.moonDistanceBoundary);
		CheckDlgButton(hWnd, CB_GENERATEDWARFPLANET, P.generateDwarfPlanets);
		SetWindowTextW(CONFIG_H.generateDwarfPlanets.HANDLE, P.generateDwarfPlanetsState);
		CheckDlgButton(hWnd, CB_WEIGHTEDMOONS, P.weightedMoons);
		SetWindowTextW(CONFIG_H.weightedMoons.HANDLE, P.weightedMoonsState);
		SetVariableToWindow(CONFIG_H.dwarfPlanetChance.HANDLE, P.dwarfPlanetChance);
		SetVariableToWindow(CONFIG_H.avgEccentricity.HANDLE, P.avgEccentricity);
		SetVariableToWindow(CONFIG_H.SDEccentricity.HANDLE, P.SDEccentricity); 
		SetVariableToWindow(CONFIG_H.avgInclination.HANDLE, P.avgInclination); // change to extra for trackbar
		SetVariableToWindow(CONFIG_H.SDInclination.HANDLE, P.SDInclination);
		SetVariableToWindow(CONFIG_H.avgObliquity.HANDLE, P.avgObliquity);
		SetVariableToWindow(CONFIG_H.SDObliquity.HANDLE, P.SDObliquity);
		CheckDlgButton(hWnd, CB_GENERATEASTEROIDS, P.generateAsteroidBelt);
		SetWindowTextW(CONFIG_H.generateAsteroidBelt.HANDLE, P.generateAsteroidBeltState);
		SetVariableToWindow(CONFIG_H.maxAsteroidBelts.HANDLE, P.maxAsteroidBelts);
		SetVariableToWindow(CONFIG_H.minAsteroidCount.HANDLE, P.minAsteroidCount);
		SetVariableToWindow(CONFIG_H.maxAsteroidCount.HANDLE, P.maxAsteroidCount);
		CheckDlgButton(hWnd, CB_GENERATECOMETS, P.generateComets);
		SetWindowTextW(CONFIG_H.generateComets.HANDLE, P.generateCometsState);
		SetVariableToWindow(CONFIG_H.minCometCount.HANDLE, P.minCometCount);
		SetVariableToWindow(CONFIG_H.maxCometCount.HANDLE, P.maxCometCount);

		SetVariableToWindow(CONFIG_H.starClassO.HANDLE, P.starClassO);
		SetVariableToWindow(CONFIG_H.starClassB.HANDLE, P.starClassB);
		SetVariableToWindow(CONFIG_H.starClassA.HANDLE, P.starClassA);
		SetVariableToWindow(CONFIG_H.starClassF.HANDLE, P.starClassF);
		SetVariableToWindow(CONFIG_H.starClassG.HANDLE, P.starClassG);
		SetVariableToWindow(CONFIG_H.starClassK.HANDLE, P.starClassK);
		SetVariableToWindow(CONFIG_H.starClassM.HANDLE, P.starClassM);
		SetVariableToWindow(CONFIG_H.starClassWD.HANDLE, P.starClassWD);
		SetVariableToWindow(CONFIG_H.starClassQ.HANDLE, P.starClassQ);
		SetVariableToWindow(CONFIG_H.starClassX.HANDLE, P.starClassX);

		SetVariableToWindow(CONFIG_H.life_OrganicChance.HANDLE, P.life_OrganicChance);
		SetVariableToWindow(CONFIG_H.life_ExoticChance.HANDLE, P.life_ExoticChance);
		SetVariableToWindow(CONFIG_H.life_MulticellChance.HANDLE, P.life_MulticellChance);
		CheckDlgButton(hWnd, CB_FORCELIFE, P.forceLife);
		SetWindowTextW(CONFIG_H.forceLife.HANDLE, P.forceLifeState);
		CheckDlgButton(hWnd, CB_TRADITIONALLIFE, P.traditionalLife);
		SetWindowTextW(CONFIG_H.traditionalLife.HANDLE, P.traditonalLifeState);

		CheckDlgButton(hWnd, CB_GENERATECOMPOSITION, P.generateComposition);
		SetWindowTextW(CONFIG_H.generateComposition.HANDLE, P.generateCompositionState);

		SetVariableToWindow(CONFIG_H.exotic_ShipChance.HANDLE, P.exotic_ShipChance);
		SetWindowTextW(CONFIG_H.modelsFolder.HANDLE, P.modelsFolder);
		CheckDlgButton(hWnd, CB_SHIPSNEEDLIFE, P.shipsNeedLife);
		SetWindowTextW(CONFIG_H.shipsNeedLife.HANDLE, P.shipsNeedLifeState);

		SetVariableToWindow(CONFIG_H.exotic_OrbitChance.HANDLE, P.exotic_OrbitChance);
		SetVariableToWindow(CONFIG_H.exotic_AxialTiltChance.HANDLE, P.exotic_AxialTiltChance);
		SetVariableToWindow(CONFIG_H.exotic_DebrisRingChance.HANDLE, P.exotic_DebrisRingChance);
		SetVariableToWindow(CONFIG_H.exotic_CompanionOrbitChance.HANDLE, P.exotic_CompanionOrbitChance);
	}
	void UpdateNamePreset(NamePreset P, HWND hWnd)
	{
		SetVariableToWindow(NV.wordPercentH.HANDLE, P.wordPercent);
		CheckDlgButton(NV.GROUP_SIMPLE, NVCB_SIMPLEGENERATOR, P.useSimpleGenerator);
		SetWindowTextW(NV.PrefixListH, P.PrefixList);
		SetWindowTextW(NV.SuffixListH, P.SuffixList);

		CheckDlgButton(NV.GROUP_STAR, NVCB_STARPREMOD, P.useStarPreMods);
		SetVariableToWindow(NV.probStarPreModH, P.probStarPreMod);
		SetWindowTextW(NV.starPreModList, P.StarPreMods);
		CheckDlgButton(NV.GROUP_STAR, NVCB_STARPOSTMOD, P.useStarPostMods);
		SetVariableToWindow(NV.probStarPostModH, P.probStarPostMod);
		SetWindowTextW(NV.starPostModList, P.StarPostMods);
		CheckDlgButton(NV.GROUP_STAR, NVCB_STARNUMBERMOD, P.useStarNumberMods);
		SetVariableToWindow(NV.probStarNumberModH, P.probStarNumberMod);

		CheckDlgButton(NV.GROUP_PLANET, NVCB_PLANETPREMOD, P.usePlanetPreMods);
		SetVariableToWindow(NV.probPlanetPreModH, P.probPlanetPreMod);
		SetWindowTextW(NV.planetPreModList, P.PlanetPreMods);
		CheckDlgButton(NV.GROUP_PLANET, NVCB_PLANETPOSTMOD, P.usePlanetPostMods);
		SetVariableToWindow(NV.probPlanetPostModH, P.probPlanetPostMod);
		SetWindowTextW(NV.planetPostModList, P.PlanetPostMods);
		CheckDlgButton(NV.GROUP_PLANET, NVCB_PLANETNUMBERMOD, P.usePlanetNumberMods);
		SetVariableToWindow(NV.probPlanetNumberModH, P.probPlanetNumberMod);

		CheckDlgButton(NV.GROUP_MOON, NVCB_NAMETERRAMOONS, P.nameTerraMoons);
		CheckDlgButton(NV.GROUP_MOON, NVCB_NAMEGASMOONS, P.nameGasMoons);
		CheckDlgButton(NV.GROUP_MOON, NVCB_MOONPREMOD, P.useMoonPreMods);
		SetVariableToWindow(NV.probMoonPreModH, P.probMoonPreMod);
		SetWindowTextW(NV.moonPreModList, P.MoonPreMods);
		CheckDlgButton(NV.GROUP_MOON, NVCB_MOONPOSTMOD, P.useMoonPostMods);
		SetVariableToWindow(NV.probMoonPostModH, P.probMoonPostMod);
		SetWindowTextW(NV.moonPostModList, P.MoonPostMods);
		CheckDlgButton(NV.GROUP_MOON, NVCB_MOONNUMBERMOD, P.useMoonNumberMods);
		SetVariableToWindow(NV.probMoonNumberModH, P.probMoonNumberMod);

		CheckDlgButton(NV.GROUP_ASTEROID, NVCB_NAMEASTEROIDS, P.nameAsteroids);
		CheckDlgButton(NV.GROUP_ASTEROID, NVCB_ASTEROIDPREMOD, P.useAsteroidPreMods);
		SetVariableToWindow(NV.probAsteroidPreModH, P.probAsteroidPreMod);
		SetWindowTextW(NV.asteroidPreModList, P.AsteroidPreMods);
		CheckDlgButton(NV.GROUP_ASTEROID, NVCB_ASTEROIDPOSTMOD, P.useAsteroidPostMods);
		SetVariableToWindow(NV.probAsteroidPostModH, P.probAsteroidPostMod);
		SetWindowTextW(NV.asteroidPostModList, P.AsteroidPostMods);
		CheckDlgButton(NV.GROUP_ASTEROID, NVCB_ASTEROIDNUMBERMOD, P.useAsteroidNumberMods);
		SetVariableToWindow(NV.probAsteroidNumberModH, P.probAsteroidNumberMod);

		CheckDlgButton(NV.GROUP_COMET, NVCB_NAMECOMETS, P.nameComets);
		CheckDlgButton(NV.GROUP_COMET, NVCB_COMETPREMOD, P.useCometPreMods);
		SetVariableToWindow(NV.probCometPreModH, P.probCometPreMod);
		SetWindowTextW(NV.cometPreModList, P.CometPreMods);
		CheckDlgButton(NV.GROUP_COMET, NVCB_COMETPOSTMOD, P.useCometPostMods);
		SetVariableToWindow(NV.probCometPostModH, P.probCometPostMod);
		SetWindowTextW(NV.cometPostModList, P.CometPostMods);
		CheckDlgButton(NV.GROUP_COMET, NVCB_COMETNUMBERMOD, P.useCometNumberMods);
		SetVariableToWindow(NV.probCometNumberModH, P.probCometNumberMod);

		CheckDlgButton(NV.GROUP_DWARFMOON, NVCB_NAMETERRADWARFMOONS, P.nameTerraDwarfMoons);
		CheckDlgButton(NV.GROUP_DWARFMOON, NVCB_NAMEGASDWARFMOONS, P.nameGasDwarfMoons);
		CheckDlgButton(NV.GROUP_DWARFMOON, NVCB_DWARFMOONPREMOD, P.useDwarfMoonPreMods);
		SetVariableToWindow(NV.probDwarfMoonPreModH, P.probDwarfMoonPreMod);
		SetWindowTextW(NV.dwarfMoonPreModList, P.DwarfMoonPreMods);
		CheckDlgButton(NV.GROUP_DWARFMOON, NVCB_DWARFMOONPOSTMOD, P.useDwarfMoonPostMods);
		SetVariableToWindow(NV.probDwarfMoonPostModH, P.probDwarfMoonPostMod);
		SetWindowTextW(NV.dwarfMoonPostModList, P.DwarfMoonPostMods);
		CheckDlgButton(NV.GROUP_DWARFMOON, NVCB_DWARFMOONNUMBERMOD, P.useDwarfMoonNumberMods);
		SetVariableToWindow(NV.probDwarfMoonNumberModH, P.probDwarfMoonNumberMod);

		CheckDlgButton(NV.GROUP_SHIP_ALL, NVCB_SHIPALLPREMOD, P.useShipPreMods_All);
		SetVariableToWindow(NV.probShipPreMod_AllH, P.probShipPreMod_All);
		SetWindowTextW(NV.shipPreModList_All, P.ShipPreMods_All);
		CheckDlgButton(NV.GROUP_SHIP_ALL, NVCB_SHIPALLPOSTMOD, P.useShipPostMods_All);
		SetVariableToWindow(NV.probShipPostMod_AllH, P.probShipPostMod_All);
		SetWindowTextW(NV.shipPostModList_All, P.ShipPostMods_All);

		CheckDlgButton(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYPREMOD, P.useShipPreMods_Colony);
		SetVariableToWindow(NV.probShipPreMod_ColonyH, P.probShipPreMod_Colony);
		SetWindowTextW(NV.shipPreModList_Colony, P.ShipPreMods_Colony);
		CheckDlgButton(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYPOSTMOD, P.useShipPostMods_Colony);
		SetVariableToWindow(NV.probShipPostMod_ColonyH, P.probShipPostMod_Colony);
		SetWindowTextW(NV.shipPostModList_Colony, P.ShipPostMods_Colony);
		CheckDlgButton(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYNUMBERMOD, P.useShipNumberMods_Colony);
		SetVariableToWindow(NV.probShipNumberMod_ColonyH, P.probShipNumberMod_Colony);

		CheckDlgButton(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTPREMOD, P.useShipPreMods_Instrument);
		SetVariableToWindow(NV.probShipPreMod_InstrumentH, P.probShipPreMod_Instrument);
		SetWindowTextW(NV.shipPreModList_Instrument, P.ShipPreMods_Instrument);
		CheckDlgButton(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTPOSTMOD, P.useShipPostMods_Instrument);
		SetVariableToWindow(NV.probShipPostMod_InstrumentH, P.probShipPostMod_Instrument);
		SetWindowTextW(NV.shipPostModList_Instrument, P.ShipPostMods_Instrument);
		CheckDlgButton(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTNUMBERMOD, P.useShipNumberMods_Instrument);
		SetVariableToWindow(NV.probShipNumberMod_InstrumentH, P.probShipNumberMod_Instrument);

		CheckDlgButton(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITEPREMOD, P.useShipPreMods_Satellite);
		SetVariableToWindow(NV.probShipPreMod_SatelliteH, P.probShipPreMod_Satellite);
		SetWindowTextW(NV.shipPreModList_Satellite, P.ShipPreMods_Satellite);
		CheckDlgButton(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITEPOSTMOD, P.useShipPostMods_Satellite);
		SetVariableToWindow(NV.probShipPostMod_SatelliteH, P.probShipPostMod_Satellite);
		SetWindowTextW(NV.shipPostModList_Satellite, P.ShipPostMods_Satellite);
		CheckDlgButton(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITENUMBERMOD, P.useShipNumberMods_Satellite);
		SetVariableToWindow(NV.probShipNumberMod_SatelliteH, P.probShipNumberMod_Satellite);

		CheckDlgButton(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONPREMOD, P.useShipPreMods_Station);
		SetVariableToWindow(NV.probShipPreMod_StationH, P.probShipPreMod_Station);
		SetWindowTextW(NV.shipPreModList_Station, P.ShipPreMods_Station);
		CheckDlgButton(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONPOSTMOD, P.useShipPostMods_Station);
		SetVariableToWindow(NV.probShipPostMod_StationH, P.probShipPostMod_Station);
		SetWindowTextW(NV.shipPostModList_Station, P.ShipPostMods_Station);
		CheckDlgButton(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONNUMBERMOD, P.useShipNumberMods_Station);
		SetVariableToWindow(NV.probShipNumberMod_StationH, P.probShipNumberMod_Station);

		SetVariableToWindow(NV.orderH, P.order);
		SetVariableToWindow(NV.wordVarienceH, P.wordVarience);
		SetVariableToWindow(NV.min_lengthH, P.min_length);
		SetVariableToWindow(NV.max_lengthH, P.max_length);
		SetWindowTextW(NV.Markov_RawDatasetH, P.Markov_RawDataset);

		CreateNameVectors(hWnd);
	}
	void SavePreset()
	{
		const int NAMESIZE = 64;
		
		std::wstring filePath = L"presets\\";
		wchar_t wfileName[NAMESIZE];
		std::wstring fileName;

		GetWindowTextW(CONFIG_H.savePresetButton.HANDLE, wfileName, NAMESIZE);
		fileName = wfileName;
		filePath += fileName + L".preset";
		std::ofstream outputFile(filePath.c_str()); // creates the file

		outputFile << "PresetName=" << wstr_to_str(fileName) << "\n"
			<< "starOutputFolder=" << wstr_to_str(CONFIG.starOutputFolder) << "\n"
			<< "planetOutputFolder=" << wstr_to_str(CONFIG.planetOutputFolder) << "\n"
			<< "seed=" << CONFIG.seed << "\n"
			<< "numberOfRuns=" << CONFIG.numberOfRuns << "\n"
			<< "smartPlacement=" << CONFIG.smartPlacement << "\n"
			<< "systemRadiusModifier=" << CONFIG.systemRadiusModifier << "\n"
			<< "minPlanetNumber=" << CONFIG.minPlanetNumber << "\n"
			<< "minDistance=" << CONFIG.minDistance << "\n"
			<< "maxDistance=" << CONFIG.maxDistance << "\n"
			<< "planetSpaceAvg=" << CONFIG.planetSpaceAvg << "\n"
			<< "planetSpaceSD=" << CONFIG.planetSpaceSD << "\n"
			<< "moonDistanceBoundary=" << CONFIG.moonDistanceBoundary << "\n"
			<< "generateDwarfPlanets=" << CONFIG.generateDwarfPlanets << "\n"
			<< "dwarfPlanetChance=" << CONFIG.dwarfPlanetChance << "\n"
			<< "weightedMoons=" << CONFIG.weightedMoons << "\n"
			<< "avgEccentricity=" << CONFIG.avgEccentricity << "\n"
			<< "SDEccentricity=" << CONFIG.SDEccentricity << "\n"
			<< "avgInclination=" << CONFIG.avgInclination << "\n"
			<< "SDInclination=" << CONFIG.SDInclination << "\n"
			<< "avgObliquity=" << CONFIG.avgObliquity << "\n"
			<< "SDObliquity=" << CONFIG.SDObliquity << "\n"
			<< "generateAsteroidBelt=" << CONFIG.generateAsteroidBelt << "\n"
			<< "maxAsteroidBelts=" << CONFIG.maxAsteroidBelts << "\n"
			<< "minAsteroidCount=" << CONFIG.minAsteroidCount << "\n"
			<< "maxAsteroidCount=" << CONFIG.maxAsteroidCount << "\n"
			<< "generateComets=" << CONFIG.generateComets << "\n"
			<< "minCometCount=" << CONFIG.minCometCount << "\n"
			<< "maxCometCount=" << CONFIG.maxCometCount << "\n"
			<< "starClassO=" << CONFIG.starClassO << "\n"
			<< "starClassB=" << CONFIG.starClassB << "\n"
			<< "starClassA=" << CONFIG.starClassA << "\n"
			<< "starClassF=" << CONFIG.starClassF << "\n"
			<< "starClassG=" << CONFIG.starClassG << "\n"
			<< "starClassK=" << CONFIG.starClassK << "\n"
			<< "starClassM=" << CONFIG.starClassM << "\n"
			<< "starClassWD=" << CONFIG.starClassWD << "\n"
			<< "starClassQ=" << CONFIG.starClassQ << "\n"
			<< "starClassX=" << CONFIG.starClassX << "\n"
			<< "life_OrganicChance=" << CONFIG.life_OrganicChance << "\n"
			<< "life_ExoticChance=" << CONFIG.life_ExoticChance << "\n"
			<< "life_MulticellChance=" << CONFIG.life_MulticellChance << "\n"
			<< "forceLife=" << CONFIG.forceLife << "\n"
			<< "traditionalLife=" << CONFIG.traditionalLife << "\n"
			<< "generateComposition=" << CONFIG.generateComposition << "\n"
			<< "exotic_ShipChance=" << CONFIG.exotic_ShipChance << "\n"
			<< "ModelsFolder=" << wstr_to_str(CONFIG.modelsFolder) << "\n"
			<< "ShipsNeedLife=" << CONFIG.shipsNeedLife << "\n"
			<< "exotic_OrbitChance=" << CONFIG.exotic_OrbitChance << "\n"
			<< "exotic_AxialTiltChance=" << CONFIG.exotic_AxialTiltChance << "\n"
			<< "exotic_DebrisRingChance=" << CONFIG.exotic_DebrisRingChance << "\n"
			<< "exotic_CompanionOrbitChance=" << CONFIG.exotic_CompanionOrbitChance << "\n";

		outputFile.close();
	}
	void SaveNamePreset()
	{
		const int NAMESIZE = 64;

		std::wstring filePath = L"presets\\";
		wchar_t wfileName[NAMESIZE];
		std::wstring fileName;

		GetWindowTextW(NV.saveNamePresetButton.HANDLE, wfileName, NAMESIZE);
		fileName = wfileName;
		filePath += fileName + L".npreset";
		std::ofstream outputFile(filePath.c_str()); // creates the file

		outputFile << "PresetName=" << wstr_to_str(fileName) << "\n"
			<< "WordPercent=" << NV.wordPercent << "\n"
			<< "useSimpleNameGenerator=" << NV.useSimpleGenerator << "\n"
			<< "prefixList {" << wstr_to_str(NV.PrefixList) << "}\n"
			<< "suffixList {" << wstr_to_str(NV.SuffixList) << "}\n"
			<< "\n"
			<< "useStarPreMods=" << NV.useStarPreMods << "\n"
			<< "probStarPreMod=" << NV.probStarPreMod << "\n"
			<< "starPreMods {" << wstr_to_str(NV.StarPreMods) << "}\n"
			<< "useStarPostMods=" << NV.useStarPostMods << "\n"
			<< "probStarPostMod=" << NV.probStarPostMod << "\n"
			<< "starPostMods {" << wstr_to_str(NV.StarPostMods) << "}\n"
			<< "useStarNumberMods=" << NV.useStarNumberMods << "\n"
			<< "probStarNumberMod=" << NV.probStarNumberMod << "\n"
			<< "\n"
			<< "usePlanetPreMods=" << NV.usePlanetPreMods << "\n"
			<< "probPlanetPreMod=" << NV.probPlanetPreMod << "\n"
			<< "planetPreMods {" << wstr_to_str(NV.PlanetPreMods) << "}\n"
			<< "usePlanetPostMods=" << NV.usePlanetPostMods << "\n"
			<< "probPlanetPostMod=" << NV.probPlanetPostMod << "\n"
			<< "planetPostMods {" << wstr_to_str(NV.PlanetPostMods) << "}\n"
			<< "usePlanetNumberMods=" << NV.usePlanetNumberMods << "\n"
			<< "probPlanetNumberMod=" << NV.probPlanetNumberMod << "\n"
			<< "\n"
			<< "nameTerraMoons=" << NV.nameTerraMoons << "\n"
			<< "nameGasMoons=" << NV.nameGasMoons << "\n"
			<< "useMoonPreMods=" << NV.useMoonPreMods << "\n"
			<< "probMoonPreMod=" << NV.probMoonPreMod << "\n"
			<< "moonPreMods {" << wstr_to_str(NV.MoonPreMods) << "}\n"
			<< "useMoonPostMods=" << NV.useMoonPostMods << "\n"
			<< "probMoonPostMod=" << NV.probMoonPostMod << "\n"
			<< "moonPostMods {" << wstr_to_str(NV.MoonPostMods) << "}\n"
			<< "useMoonNumberMods=" << NV.useMoonNumberMods << "\n"
			<< "probMoonNumberMod=" << NV.probMoonNumberMod << "\n"
			<< "\n"
			<< "nameTerraDwarfMoons=" << NV.nameTerraDwarfMoons << "\n"
			<< "nameGasDwarfMoons=" << NV.nameGasDwarfMoons << "\n"
			<< "useDwarfMoonPreMods=" << NV.useDwarfMoonPreMods << "\n"
			<< "probDwarfMoonPreMod=" << NV.probDwarfMoonPreMod << "\n"
			<< "dwarfMoonPreMods {" << wstr_to_str(NV.DwarfMoonPreMods) << "}\n"
			<< "useDwarfMoonPostMods=" << NV.useDwarfMoonPostMods << "\n"
			<< "probDwarfMoonPostMod=" << NV.probDwarfMoonPostMod << "\n"
			<< "dwarfMoonPostMods {" << wstr_to_str(NV.DwarfMoonPostMods) << "}\n"
			<< "useDwarfMoonNumberMods=" << NV.useDwarfMoonNumberMods << "\n"
			<< "probDwarfMoonNumberMod=" << NV.probDwarfMoonNumberMod << "\n"
			<< "\n"
			<< "nameAsteroids=" << NV.nameAsteroids << "\n"
			<< "useAsteroidPreMods=" << NV.useAsteroidPreMods << "\n"
			<< "probAsteroidPreMod=" << NV.probAsteroidPreMod << "\n"
			<< "asteroidPreMods {" << wstr_to_str(NV.AsteroidPreMods) << "}\n"
			<< "useAsteroidPostMods=" << NV.useAsteroidPostMods << "\n"
			<< "probAsteroidPostMod=" << NV.probAsteroidPostMod << "\n"
			<< "asteroidPostMods {" << wstr_to_str(NV.AsteroidPostMods) << "}\n"
			<< "useAsteroidNumberMods=" << NV.useAsteroidNumberMods << "\n"
			<< "probAsteroidNumberMod=" << NV.probAsteroidNumberMod << "\n"
			<< "\n"
			<< "nameComets=" << NV.nameComets << "\n"
			<< "useCometPreMods=" << NV.useCometPreMods << "\n"
			<< "probCometPreMod=" << NV.probCometPreMod << "\n"
			<< "cometPreMods {" << wstr_to_str(NV.CometPreMods) << "}\n"
			<< "useCometPostMods=" << NV.useCometPostMods << "\n"
			<< "probCometPostMod=" << NV.probCometPostMod << "\n"
			<< "cometPostMods {" << wstr_to_str(NV.CometPostMods) << "}\n"
			<< "useCometNumberMods=" << NV.useCometNumberMods << "\n"
			<< "probCometNumberMod=" << NV.probCometNumberMod << "\n"
			<< "\n"
			<< "useShipPreMods_All=" << NV.useShipPreMods_All << "\n"
			<< "probShipPreMod_All=" << NV.probShipPreMod_All << "\n"
			<< "ShipPreMods_All {" << wstr_to_str(NV.ShipPreMods_All) << "}\n"
			<< "useShipPostMods_All=" << NV.useShipPostMods_All << "\n"
			<< "probShipPostMod_All=" << NV.probShipPostMod_All << "\n"
			<< "ShipPostMods_All {" << wstr_to_str(NV.ShipPostMods_All) << "}\n"
			<< "\n"
			<< "useShipPreMods_Colony=" << NV.useShipPreMods_Colony << "\n"
			<< "probShipPreMod_Colony=" << NV.probShipPreMod_Colony << "\n"
			<< "ShipPreMods_Colony {" << wstr_to_str(NV.ShipPreMods_Colony) << "}\n"
			<< "useShipPostMods_Colony=" << NV.useShipPostMods_Colony << "\n"
			<< "probShipPostMod_Colony=" << NV.probShipPostMod_Colony << "\n"
			<< "ShipPostMods_Colony {" << wstr_to_str(NV.ShipPostMods_Colony) << "}\n"
			<< "useShipNumberMods_Colony=" << NV.useShipNumberMods_Colony << "\n"
			<< "probShipNumberMod_Colony=" << NV.probShipNumberMod_Colony << "\n"
			<< "\n"
			<< "useShipPreMods_Instrument=" << NV.useShipPreMods_Instrument << "\n"
			<< "probShipPreMod_Instrument=" << NV.probShipPreMod_Instrument << "\n"
			<< "ShipPreMods_Instrument {" << wstr_to_str(NV.ShipPreMods_Instrument) << "}\n"
			<< "useShipPostMods_Instrument=" << NV.useShipPostMods_Instrument << "\n"
			<< "probShipPostMod_Instrument=" << NV.probShipPostMod_Instrument << "\n"
			<< "ShipPostMods_Instrument {" << wstr_to_str(NV.ShipPostMods_Instrument) << "}\n"
			<< "useShipNumberMods_Instrument=" << NV.useShipNumberMods_Instrument << "\n"
			<< "probShipNumberMod_Instrument=" << NV.probShipNumberMod_Instrument << "\n"
			<< "\n"
			<< "useShipPreMods_Satellite=" << NV.useShipPreMods_Satellite << "\n"
			<< "probShipPreMod_Satellite=" << NV.probShipPreMod_Satellite << "\n"
			<< "ShipPreMods_Satellite {" << wstr_to_str(NV.ShipPreMods_Satellite) << "}\n"
			<< "useShipPostMods_Satellite=" << NV.useShipPostMods_Satellite << "\n"
			<< "probShipPostMod_Satellite=" << NV.probShipPostMod_Satellite << "\n"
			<< "ShipPostMods_Satellite {" << wstr_to_str(NV.ShipPostMods_Satellite) << "}\n"
			<< "useShipNumberMods_Satellite=" << NV.useShipNumberMods_Satellite << "\n"
			<< "probShipNumberMod_Satellite=" << NV.probShipNumberMod_Satellite << "\n"
			<< "\n"
			<< "useShipPreMods_Station=" << NV.useShipPreMods_Station << "\n"
			<< "probShipPreMod_Station=" << NV.probShipPreMod_Station << "\n"
			<< "ShipPreMods_Station {" << wstr_to_str(NV.ShipPreMods_Station) << "}\n"
			<< "useShipPostMods_Station=" << NV.useShipPostMods_Station << "\n"
			<< "probShipPostMod_Station=" << NV.probShipPostMod_Station << "\n"
			<< "ShipPostMods_Station {" << wstr_to_str(NV.ShipPostMods_Station) << "}\n"
			<< "useShipNumberMods_Station=" << NV.useShipNumberMods_Station << "\n"
			<< "probShipNumberMod_Station=" << NV.probShipNumberMod_Station << "\n"
			<< "\n"
			<< "order=" << NV.order << "\n"
			<< "wordVarience=" << NV.wordVarience << "\n"
			<< "minLength=" << NV.min_length << "\n"
			<< "maxLength=" << NV.max_length << "\n"
			<< "Dataset {" << wstr_to_str(NV.Markov_RawDataset) << "}\n";

		outputFile.close();
	}

	void Clear_Screen()
	{
		/*----------------------------------------------------------
		This function clears all of the handles for every variable.
		-----------------------------------------------------------*/

		switch (lastScreen)
		{
		case General:
		{
			//Header
			ShowWindow(CONFIG_H.HEADER_GENERAL, 0);
			//Handles
			ShowWindow(CONFIG_H.seed.HANDLE, 0);
			ShowWindow(CONFIG_H.numberOfRuns.HANDLE, 0);
			ShowWindow(CONFIG_H.starOutputFolder.HANDLE, 0);
			ShowWindow(CONFIG_H.planetOutputFolder.HANDLE, 0);
			ShowWindow(CONFIG_H.presetDropDown.HANDLE, 0);
			ShowWindow(CONFIG_H.savePresetButton.HANDLE, 0);
			ShowWindow(CONFIG_H.namePresetDropDown.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG_H.seed.DESC, 0);
			ShowWindow(CONFIG_H.numberOfRuns.DESC, 0);
			ShowWindow(CONFIG_H.starOutputFolder.DESC, 0);
			ShowWindow(CONFIG_H.planetOutputFolder.DESC, 0);
			ShowWindow(CONFIG_H.presetDropDown.DESC, 0);
			ShowWindow(CONFIG_H.savePresetButton.DESC, 0);
			ShowWindow(CONFIG_H.namePresetDropDown.DESC, 0);
			//Info
			ShowWindow(CONFIG_H.seed.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.numberOfRuns.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.starOutputFolder.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.planetOutputFolder.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.presetDropDown.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.savePresetButton.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.namePresetDropDown.INFOBUTTON, 0);
			//Extra
			ShowWindow(CONFIG_H.starOutputFolder.EXTRA, 0);
			ShowWindow(CONFIG_H.planetOutputFolder.EXTRA, 0);
			ShowWindow(CONFIG_H.presetDropDown.EXTRA, 0);
			ShowWindow(CONFIG_H.savePresetButton.EXTRA, 0);
			ShowWindow(CONFIG_H.namePresetDropDown.EXTRA, 0);
		}
			break;
		case System:
		{
			//Header
			ShowWindow(CONFIG_H.HEADER_SYSTEM, 0);
			//Handles
			ShowWindow(CONFIG_H.smartPlacement.HANDLE, 0);
			ShowWindow(CONFIG_H.generateDwarfPlanets.HANDLE, 0);
			ShowWindow(CONFIG_H.dwarfPlanetChance.HANDLE, 0);
			ShowWindow(CONFIG_H.systemRadiusModifier.HANDLE, 0);
			ShowWindow(CONFIG_H.minPlanetNumber.HANDLE, 0);
			ShowWindow(CONFIG_H.minDistance.HANDLE, 0);
			ShowWindow(CONFIG_H.maxDistance.HANDLE, 0);
			ShowWindow(CONFIG_H.planetSpaceAvg.HANDLE, 0);
			ShowWindow(CONFIG_H.planetSpaceSD.HANDLE, 0);
			ShowWindow(CONFIG_H.avgEccentricity.HANDLE, 0);
			ShowWindow(CONFIG_H.SDEccentricity.HANDLE, 0);
			ShowWindow(CONFIG_H.avgInclination.HANDLE, 0);
			ShowWindow(CONFIG_H.SDInclination.HANDLE, 0);
			ShowWindow(CONFIG_H.avgObliquity.HANDLE, 0);
			ShowWindow(CONFIG_H.SDObliquity.HANDLE, 0);
			ShowWindow(CONFIG_H.generateAsteroidBelt.HANDLE, 0);
			ShowWindow(CONFIG_H.maxAsteroidBelts.HANDLE, 0);
			ShowWindow(CONFIG_H.minAsteroidCount.HANDLE, 0);
			ShowWindow(CONFIG_H.maxAsteroidCount.HANDLE, 0);
			ShowWindow(CONFIG_H.generateComets.HANDLE, 0);
			ShowWindow(CONFIG_H.minCometCount.HANDLE, 0);
			ShowWindow(CONFIG_H.maxCometCount.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG_H.smartPlacement.DESC, 0);
			ShowWindow(CONFIG_H.generateDwarfPlanets.DESC, 0);
			ShowWindow(CONFIG_H.dwarfPlanetChance.DESC, 0);
			ShowWindow(CONFIG_H.systemRadiusModifier.DESC, 0);
			ShowWindow(CONFIG_H.minPlanetNumber.DESC, 0);
			ShowWindow(CONFIG_H.minDistance.DESC, 0);
			ShowWindow(CONFIG_H.maxDistance.DESC, 0);
			ShowWindow(CONFIG_H.planetSpaceAvg.DESC, 0);
			ShowWindow(CONFIG_H.planetSpaceSD.DESC, 0);
			ShowWindow(CONFIG_H.avgEccentricity.DESC, 0);
			ShowWindow(CONFIG_H.SDEccentricity.DESC, 0);
			ShowWindow(CONFIG_H.avgInclination.DESC, 0);
			ShowWindow(CONFIG_H.SDInclination.DESC, 0);
			ShowWindow(CONFIG_H.avgObliquity.DESC, 0);
			ShowWindow(CONFIG_H.SDObliquity.DESC, 0);
			ShowWindow(CONFIG_H.generateAsteroidBelt.DESC, 0);
			ShowWindow(CONFIG_H.maxAsteroidBelts.DESC, 0);
			ShowWindow(CONFIG_H.minAsteroidCount.DESC, 0);
			ShowWindow(CONFIG_H.maxAsteroidCount.DESC, 0);
			ShowWindow(CONFIG_H.generateComets.DESC, 0);
			ShowWindow(CONFIG_H.minCometCount.DESC, 0);
			ShowWindow(CONFIG_H.maxCometCount.DESC, 0);
			//Info
			ShowWindow(CONFIG_H.smartPlacement.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.generateDwarfPlanets.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.dwarfPlanetChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.systemRadiusModifier.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.minPlanetNumber.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.minDistance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.maxDistance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.planetSpaceAvg.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.avgEccentricity.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.SDEccentricity.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.avgInclination.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.SDInclination.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.avgObliquity.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.SDObliquity.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.generateAsteroidBelt.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.maxAsteroidBelts.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.minAsteroidCount.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.generateComets.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.minCometCount.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.starClassO.INFOBUTTON, 0);
			//Extra
			ShowWindow(CONFIG_H.starClassO.EXTRA, 0);
		}
			break;
		case Planet:
		{
			//Header
			ShowWindow(CONFIG_H.HEADER_PLANET, 0);
			//Handles
			ShowWindow(CONFIG_H.life_OrganicChance.HANDLE, 0);
			ShowWindow(CONFIG_H.life_ExoticChance.HANDLE, 0);
			ShowWindow(CONFIG_H.life_MulticellChance.HANDLE, 0);
			ShowWindow(CONFIG_H.forceLife.HANDLE, 0);
			ShowWindow(CONFIG_H.traditionalLife.HANDLE, 0);
			ShowWindow(CONFIG_H.weightedMoons.HANDLE, 0);
			ShowWindow(CONFIG_H.moonDistanceBoundary.HANDLE, 0);
			ShowWindow(CONFIG_H.exotic_OrbitChance.HANDLE, 0);
			ShowWindow(CONFIG_H.exotic_AxialTiltChance.HANDLE, 0);
			ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.HANDLE, 0);
			ShowWindow(CONFIG_H.exotic_DebrisRingChance.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG_H.life_OrganicChance.DESC, 0);
			ShowWindow(CONFIG_H.life_ExoticChance.DESC, 0);
			ShowWindow(CONFIG_H.life_MulticellChance.DESC, 0);
			ShowWindow(CONFIG_H.forceLife.DESC, 0);
			ShowWindow(CONFIG_H.traditionalLife.DESC, 0);
			ShowWindow(CONFIG_H.weightedMoons.DESC, 0);
			ShowWindow(CONFIG_H.moonDistanceBoundary.DESC, 0);
			ShowWindow(CONFIG_H.exotic_OrbitChance.DESC, 0);
			ShowWindow(CONFIG_H.exotic_AxialTiltChance.DESC, 0);
			ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.DESC, 0);
			ShowWindow(CONFIG_H.exotic_DebrisRingChance.DESC, 0);
			//Info
			ShowWindow(CONFIG_H.life_OrganicChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.life_ExoticChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.life_MulticellChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.forceLife.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.traditionalLife.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.weightedMoons.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.moonDistanceBoundary.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.exotic_OrbitChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.exotic_AxialTiltChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.exotic_DebrisRingChance.INFOBUTTON, 0);
			//Extra
		}
			break;
		case Surface:
		{
			//Header
			ShowWindow(CONFIG_H.HEADER_SURFACE, 0);
			//Handles
			ShowWindow(CONFIG_H.generateComposition.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG_H.generateComposition.DESC, 0);
			//Info
			ShowWindow(CONFIG_H.generateComposition.INFOBUTTON, 0);
			//Extra
		}
			break;
		case Special:
		{
			//Header
			ShowWindow(CONFIG_H.HEADER_SPECIAL, 0);
			//Handles
			ShowWindow(CONFIG_H.modelsFolder.HANDLE, 0);
			ShowWindow(CONFIG_H.exotic_ShipChance.HANDLE, 0);
			ShowWindow(CONFIG_H.shipsNeedLife.HANDLE, 0);			
			//Desc
			ShowWindow(CONFIG_H.modelsFolder.DESC, 0);
			ShowWindow(CONFIG_H.exotic_ShipChance.DESC, 0);
			ShowWindow(CONFIG_H.shipsNeedLife.DESC, 0);			
			//Info
			ShowWindow(CONFIG_H.modelsFolder.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.exotic_ShipChance.INFOBUTTON, 0);
			ShowWindow(CONFIG_H.shipsNeedLife.INFOBUTTON, 0);			
			//Extra
			ShowWindow(CONFIG_H.modelsFolder.EXTRA, 0);
		}
			break;
		case Advanced:
		{
			ShowWindow(CONFIG_H.HEADER_ADVANCED, 0);
			ShowWindow(CONFIG.Tab_Advanced_Outer, 0);
			Clear_Advanced();
		}
			break;
		}
	}
	void Load_Screen_General()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_GENERAL, 1);

		//Handles
		ShowWindow(CONFIG_H.seed.HANDLE, 1);
		ShowWindow(CONFIG_H.numberOfRuns.HANDLE, 1);
		ShowWindow(CONFIG_H.starOutputFolder.HANDLE, 1);
		ShowWindow(CONFIG_H.planetOutputFolder.HANDLE, 1);

		ShowWindow(CONFIG_H.presetDropDown.HANDLE, 1);
		ShowWindow(CONFIG_H.savePresetButton.HANDLE, 1);
		ShowWindow(CONFIG_H.namePresetDropDown.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG_H.seed.DESC, 1);
		ShowWindow(CONFIG_H.numberOfRuns.DESC, 1);
		ShowWindow(CONFIG_H.starOutputFolder.DESC, 1);
		ShowWindow(CONFIG_H.planetOutputFolder.DESC, 1);

		ShowWindow(CONFIG_H.presetDropDown.DESC, 1);
		ShowWindow(CONFIG_H.savePresetButton.DESC, 1);
		ShowWindow(CONFIG_H.namePresetDropDown.DESC, 1);

		//Info
		ShowWindow(CONFIG_H.seed.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.numberOfRuns.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.starOutputFolder.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.planetOutputFolder.INFOBUTTON, 1);

		ShowWindow(CONFIG_H.presetDropDown.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.savePresetButton.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.namePresetDropDown.INFOBUTTON, 1);

		//Extra
		ShowWindow(CONFIG_H.starOutputFolder.EXTRA, 1);
		ShowWindow(CONFIG_H.planetOutputFolder.EXTRA, 1);
		ShowWindow(CONFIG_H.presetDropDown.EXTRA, 1);
		ShowWindow(CONFIG_H.savePresetButton.EXTRA, 1);
		ShowWindow(CONFIG_H.namePresetDropDown.EXTRA, 1);

		lastScreen = General;
	}
	void Load_Screen_System()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_SYSTEM, 1);

		//Handles
		ShowWindow(CONFIG_H.smartPlacement.HANDLE, 1);
		ShowWindow(CONFIG_H.generateDwarfPlanets.HANDLE, 1);
		ShowWindow(CONFIG_H.dwarfPlanetChance.HANDLE, 1);
		ShowWindow(CONFIG_H.systemRadiusModifier.HANDLE, 1);
		ShowWindow(CONFIG_H.minPlanetNumber.HANDLE, 1);
		ShowWindow(CONFIG_H.minDistance.HANDLE, 1);
		ShowWindow(CONFIG_H.maxDistance.HANDLE, 1);
		ShowWindow(CONFIG_H.planetSpaceAvg.HANDLE, 1);
		ShowWindow(CONFIG_H.planetSpaceSD.HANDLE, 1);
		ShowWindow(CONFIG_H.avgEccentricity.HANDLE, 1);
		ShowWindow(CONFIG_H.SDEccentricity.HANDLE, 1);
		ShowWindow(CONFIG_H.avgInclination.HANDLE, 1);
		ShowWindow(CONFIG_H.SDInclination.HANDLE, 1);
		ShowWindow(CONFIG_H.avgObliquity.HANDLE, 1);
		ShowWindow(CONFIG_H.SDObliquity.HANDLE, 1);
		ShowWindow(CONFIG_H.generateAsteroidBelt.HANDLE, 1);
		ShowWindow(CONFIG_H.maxAsteroidBelts.HANDLE, 1);
		ShowWindow(CONFIG_H.minAsteroidCount.HANDLE, 1);
		ShowWindow(CONFIG_H.maxAsteroidCount.HANDLE, 1);
		ShowWindow(CONFIG_H.generateComets.HANDLE, 1);
		ShowWindow(CONFIG_H.minCometCount.HANDLE, 1);
		ShowWindow(CONFIG_H.maxCometCount.HANDLE, 1);

		ShowWindow(CONFIG_H.starClassO.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG_H.smartPlacement.DESC, 1);
		ShowWindow(CONFIG_H.generateDwarfPlanets.DESC, 1);
		ShowWindow(CONFIG_H.dwarfPlanetChance.DESC, 1);
		ShowWindow(CONFIG_H.systemRadiusModifier.DESC, 1);
		ShowWindow(CONFIG_H.minPlanetNumber.DESC, 1);
		ShowWindow(CONFIG_H.minDistance.DESC, 1);
		ShowWindow(CONFIG_H.maxDistance.DESC, 1);
		ShowWindow(CONFIG_H.planetSpaceAvg.DESC, 1);
		ShowWindow(CONFIG_H.planetSpaceSD.DESC, 1);
		ShowWindow(CONFIG_H.avgEccentricity.DESC, 1);
		ShowWindow(CONFIG_H.SDEccentricity.DESC, 1);
		ShowWindow(CONFIG_H.avgInclination.DESC, 1);
		ShowWindow(CONFIG_H.SDInclination.DESC, 1);
		ShowWindow(CONFIG_H.avgObliquity.DESC, 1);
		ShowWindow(CONFIG_H.SDObliquity.DESC, 1);
		ShowWindow(CONFIG_H.generateAsteroidBelt.DESC, 1);
		ShowWindow(CONFIG_H.maxAsteroidBelts.DESC, 1);
		ShowWindow(CONFIG_H.minAsteroidCount.DESC, 1);
		ShowWindow(CONFIG_H.maxAsteroidCount.DESC, 1);
		ShowWindow(CONFIG_H.generateComets.DESC, 1);
		ShowWindow(CONFIG_H.minCometCount.DESC, 1);
		ShowWindow(CONFIG_H.maxCometCount.DESC, 1);

		//Info
		ShowWindow(CONFIG_H.smartPlacement.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.generateDwarfPlanets.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.dwarfPlanetChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.systemRadiusModifier.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.minPlanetNumber.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.minDistance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.maxDistance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.planetSpaceAvg.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.avgEccentricity.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.SDEccentricity.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.avgInclination.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.SDInclination.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.avgObliquity.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.SDObliquity.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.generateAsteroidBelt.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.maxAsteroidBelts.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.minAsteroidCount.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.generateComets.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.minCometCount.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.starClassO.INFOBUTTON, 1);

		//Extra
		ShowWindow(CONFIG_H.starClassO.EXTRA, 1);

		lastScreen = System;
	}
	void Load_Screen_Planet()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_PLANET, 1);

		//Handles
		ShowWindow(CONFIG_H.life_OrganicChance.HANDLE, 1);
		ShowWindow(CONFIG_H.life_ExoticChance.HANDLE, 1);
		ShowWindow(CONFIG_H.life_MulticellChance.HANDLE, 1);
		ShowWindow(CONFIG_H.forceLife.HANDLE, 1);
		ShowWindow(CONFIG_H.traditionalLife.HANDLE, 1);
		ShowWindow(CONFIG_H.weightedMoons.HANDLE, 1);
		ShowWindow(CONFIG_H.moonDistanceBoundary.HANDLE, 1);
		ShowWindow(CONFIG_H.exotic_OrbitChance.HANDLE, 1);
		ShowWindow(CONFIG_H.exotic_AxialTiltChance.HANDLE, 1);
		ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.HANDLE, 1);
		ShowWindow(CONFIG_H.exotic_DebrisRingChance.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG_H.life_OrganicChance.DESC, 1);
		ShowWindow(CONFIG_H.life_ExoticChance.DESC, 1);
		ShowWindow(CONFIG_H.life_MulticellChance.DESC, 1);
		ShowWindow(CONFIG_H.forceLife.DESC, 1);
		ShowWindow(CONFIG_H.traditionalLife.DESC, 1);
		ShowWindow(CONFIG_H.weightedMoons.DESC, 1);
		ShowWindow(CONFIG_H.moonDistanceBoundary.DESC, 1);
		ShowWindow(CONFIG_H.exotic_OrbitChance.DESC, 1);
		ShowWindow(CONFIG_H.exotic_AxialTiltChance.DESC, 1);
		ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.DESC, 1);
		ShowWindow(CONFIG_H.exotic_DebrisRingChance.DESC, 1);

		//Info
		ShowWindow(CONFIG_H.life_OrganicChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.life_ExoticChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.life_MulticellChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.forceLife.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.traditionalLife.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.weightedMoons.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.moonDistanceBoundary.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.exotic_OrbitChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.exotic_AxialTiltChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.exotic_CompanionOrbitChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.exotic_DebrisRingChance.INFOBUTTON, 1);

		//Extra

		lastScreen = Planet;
	}
	void Load_Screen_Surface()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_SURFACE, 1);

		//Handles
		ShowWindow(CONFIG_H.generateComposition.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG_H.generateComposition.DESC, 1);

		//Info
		ShowWindow(CONFIG_H.generateComposition.INFOBUTTON, 1);

		lastScreen = Surface;
	}
	void Load_Screen_Special()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_SPECIAL, 1);

		//Handles
		ShowWindow(CONFIG_H.modelsFolder.HANDLE, 1);
		ShowWindow(CONFIG_H.exotic_ShipChance.HANDLE, 1);
		ShowWindow(CONFIG_H.shipsNeedLife.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG_H.modelsFolder.DESC, 1);
		ShowWindow(CONFIG_H.exotic_ShipChance.DESC, 1);
		ShowWindow(CONFIG_H.shipsNeedLife.DESC, 1);		

		//Info
		ShowWindow(CONFIG_H.modelsFolder.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.exotic_ShipChance.INFOBUTTON, 1);
		ShowWindow(CONFIG_H.shipsNeedLife.INFOBUTTON, 1);	

		//Extra
		ShowWindow(CONFIG_H.modelsFolder.EXTRA, 1);

		lastScreen = Special;
	}
	void Load_Screen_Advanced()
	{
		//Header
		ShowWindow(CONFIG_H.HEADER_ADVANCED, 1);
		ShowWindow(CONFIG.Tab_Advanced_Outer, 1);
		Load_Tabgroup_Advanced_Outer();

		lastScreen = Advanced;
	}

	// for trackbar calls
	/*
	void __stdcall TBNotifications(WPARAM wParam, HWND hwndTrack, UINT iSelMin, UINT iSelMax)
	{
		HWND temp;
		temp = (HWND)SendMessage(hwndTrack, TBM_GETBUDDY, (WPARAM)FALSE, 0);
		SetVariableToWindow(temp, (SendMessage(hwndTrack, TBM_GETPOS, 0, 0) / 10000.0));

	}*/

	void Clear_Advanced()
	{
		ShowWindow(CONFIG.Tab_Advanced_Inner, 0);
		ShowWindow(NV.Tab_Name_Inner, 0);
		ShowWindow(NV.Tab_Ship_Inner, 0);


		ShowWindow(NV.PreMod_INFO.DESC, 0);
		ShowWindow(NV.PostMod_INFO.DESC, 0);
		ShowWindow(NV.NumberMod_INFO.DESC, 0);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 0);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 0);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 0);
		ShowWindow(NV.Markov_INFO, 0);
		ShowWindow(NV.SimpleGenerator_INFO, 0);

		ShowWindow(NV.PreMods_List.DESC, 0);
		ShowWindow(NV.PostMods_List.DESC, 0);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 0);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 0);

		ShowWindow(NV.GROUP_STAR, 0);
		ShowWindow(NV.GROUP_PLANET, 0);
		ShowWindow(NV.GROUP_MOON, 0);
		ShowWindow(NV.GROUP_ASTEROID, 0);
		ShowWindow(NV.GROUP_COMET, 0);
		ShowWindow(NV.GROUP_DWARFMOON, 0);
		ShowWindow(NV.GROUP_SHIP_ALL, 0);
		ShowWindow(NV.GROUP_SHIP_COLONY, 0);
		ShowWindow(NV.GROUP_SHIP_INSTRUMENT, 0);
		ShowWindow(NV.GROUP_SHIP_SATELLITE, 0);
		ShowWindow(NV.GROUP_SHIP_STATION, 0);
		ShowWindow(NV.GROUP_DATASET, 0);
		ShowWindow(NV.GROUP_SIMPLE, 0);

		ShowWindow(NV.saveNamePresetButton.DESC, 0);
		ShowWindow(NV.saveNamePresetButton.HANDLE, 0);
		ShowWindow(NV.saveNamePresetButton.INFOBUTTON, 0);
		ShowWindow(NV.saveNamePresetButton.EXTRA, 0);
		ShowWindow(NV.buttonUpdate, 0);

		ShowWindow(NV.wordPercentH.DESC, 0);
		ShowWindow(NV.wordPercentH.HANDLE, 0);
		ShowWindow(NV.wordPercentH.INFOBUTTON, 0);
		ShowWindow(NV.wordPercentH.EXTRA, 0);

		ShowWindow(NV.nameMoonsH.INFOBUTTON, 0);
		ShowWindow(NV.nameAsteroidsH.INFOBUTTON, 0);
	}
	void Load_Tabgroup_Advanced_Outer()
	{
		switch (TabCtrl_GetCurSel(CONFIG.Tab_Advanced_Outer))
		{
		case TAB_NAMES:
			Load_Tabgroup_Name_Inner();
			break;
		case TAB_ADVANCED:
			//empty
			break;
		}
	}
	void Load_Tabgroup_Name_Inner()
	{
		ShowWindow(NV.Tab_Name_Inner, 1);
		ShowWindow(NV.buttonUpdate, 1);
		switch (TabCtrl_GetCurSel(NV.Tab_Name_Inner))
		{
		case TAB_NAME_DATASET:
			Load_Name_Dataset();
			break;
		case TAB_NAME_STAR:
			Load_Name_Star();
			break;
		case TAB_NAME_PLANET:
			Load_Name_Planet();
			break;
		case TAB_NAME_MOON:
			Load_Name_Moon();
			break;
		case TAB_NAME_SHIP:
			Load_Tabgroup_Ship_Inner();
			break;
		case TAB_NAME_DWARFMOON:
			Load_Name_DwarfMoon();
			break;
		case TAB_NAME_ASTEROID:
			Load_Name_Asteroid();
			break;
		case TAB_NAME_COMET:
			Load_Name_Comet();
			break;	
		case TAB_NAME_SIMPLE:
			Load_Name_Simple();
			break;
		}
	}
	void Load_Tabgroup_Advanced_Inner()
	{
	}
	void Load_Tabgroup_Ship_Inner()
	{
		ShowWindow(NV.Tab_Name_Inner, 1);
		ShowWindow(NV.Tab_Ship_Inner, 1);
		ShowWindow(NV.buttonUpdate, 1);
		switch (TabCtrl_GetCurSel(NV.Tab_Ship_Inner))
		{
		case TAB_SHIP_ALL:
			Load_Name_All_Ship();
			break;
		case TAB_SHIP_COLONY:
			Load_Name_Colony_Ship();
			break;
		case TAB_SHIP_INSTRUMENT:
			Load_Name_Instrument_Ship();
			break;
		case TAB_SHIP_SATELLITE:
			Load_Name_Satellite_Ship();
			break;
		case TAB_SHIP_STATION:
			Load_Name_Station_Ship();
			break;
		}
	}
	void Load_Name_Dataset()
	{
		ShowWindow(NV.Markov_INFO, 1);

		ShowWindow(NV.wordPercentH.DESC, 1);
		ShowWindow(NV.wordPercentH.HANDLE, 1);
		ShowWindow(NV.wordPercentH.INFOBUTTON, 1);
		ShowWindow(NV.wordPercentH.EXTRA, 1);

		ShowWindow(NV.saveNamePresetButton.HANDLE, 1);
		ShowWindow(NV.saveNamePresetButton.DESC, 1);
		ShowWindow(NV.saveNamePresetButton.INFOBUTTON, 1);
		ShowWindow(NV.saveNamePresetButton.EXTRA, 1);

		ShowWindow(NV.GROUP_DATASET, 1);
	}
	void Load_Name_Star()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_STAR, 1);
	}
	void Load_Name_Planet()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_PLANET, 1);
	}
	void Load_Name_Moon()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.nameMoonsH.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_MOON, 1);
	}
	void Load_Name_DwarfMoon()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.nameMoonsH.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_DWARFMOON, 1);
	}
	void Load_Name_Asteroid()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.nameAsteroidsH.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_ASTEROID, 1);
	}
	void Load_Name_Comet()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.nameAsteroidsH.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_COMET, 1);
	}
	void Load_Name_All_Ship()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SHIP_ALL, 1);
	}
	void Load_Name_Colony_Ship()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SHIP_COLONY, 1);
	}
	void Load_Name_Instrument_Ship()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SHIP_INSTRUMENT, 1);
	}
	void Load_Name_Satellite_Ship()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SHIP_SATELLITE, 1);
	}
	void Load_Name_Station_Ship()
	{
		ShowWindow(NV.PreMod_INFO.DESC, 1);
		ShowWindow(NV.PostMod_INFO.DESC, 1);
		ShowWindow(NV.NumberMod_INFO.DESC, 1);
		ShowWindow(NV.PreMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.PostMod_INFO.INFOBUTTON, 1);
		ShowWindow(NV.NumberMod_INFO.INFOBUTTON, 1);

		ShowWindow(NV.PreMods_List.DESC, 1);
		ShowWindow(NV.PostMods_List.DESC, 1);
		ShowWindow(NV.PreMods_List.INFOBUTTON, 1);
		ShowWindow(NV.PostMods_List.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SHIP_STATION, 1);
	}
	void Load_Name_Simple()
	{
		ShowWindow(NV.SimpleGenerator_INFO, 1);

		ShowWindow(NV.wordPercentH.DESC, 1);
		ShowWindow(NV.wordPercentH.HANDLE, 1);
		ShowWindow(NV.wordPercentH.INFOBUTTON, 1);

		ShowWindow(NV.GROUP_SIMPLE, 1);
	}

	void Load_Advanced_Main()
	{
	}
	
	void GetConfigData(HWND hWnd)
	{
		GetVariableFromWindow(CONFIG_H.starOutputFolder.HANDLE, CONFIG.starOutputFolder);
		GetVariableFromWindow(CONFIG_H.planetOutputFolder.HANDLE, CONFIG.planetOutputFolder);

		// this little maneuver generates the seed from the input
		CONFIG.seed = 0;
		wchar_t seedarray[WSIZE] = L"";
		GetVariableFromWindow(CONFIG_H.seed.HANDLE, seedarray);
		std::wstring seedstr(seedarray);

		//checks if seed is a number
		bool isnumber = true;
		for (int i = 0; i < seedstr.size(); i++)
			if (!iswdigit(seedstr[i]) && seedstr[0] != '-')
				isnumber = false;

		// creates seed from string
		if (isnumber)// if the string is just a number, use it
			CONFIG.seed = std::stoll(wstr_to_str(seedstr));
		else // else, turn the characters into a number
		{
			long long total = 0;
			for (int i = 0; seedstr[i] != '\0'; i++)
				total += ((int)seedstr[i] * (i + 1));
			CONFIG.seed = total * pow(2, seedstr.size());
			if ((int)seedstr[seedstr.size() / 2] % 2)
				CONFIG.seed *= -1;
		}
		CONFIG.debug = (IsDlgButtonChecked(hWnd, CB_DEBUG) == BST_CHECKED) ? true : false;

		GetVariableFromWindow(CONFIG_H.numberOfRuns.HANDLE, CONFIG.numberOfRuns);
		CONFIG.smartPlacement = (IsDlgButtonChecked(hWnd, CB_SMARTPLACEMENT) == BST_CHECKED) ? true : false;

		GetVariableFromWindow(CONFIG_H.systemRadiusModifier.HANDLE, CONFIG.systemRadiusModifier);
		GetVariableFromWindow(CONFIG_H.minPlanetNumber.HANDLE, CONFIG.minPlanetNumber);
		GetVariableFromWindow(CONFIG_H.minDistance.HANDLE, CONFIG.minDistance);
		GetVariableFromWindow(CONFIG_H.maxDistance.HANDLE, CONFIG.maxDistance);
		GetVariableFromWindow(CONFIG_H.planetSpaceAvg.HANDLE, CONFIG.planetSpaceAvg);
		GetVariableFromWindow(CONFIG_H.planetSpaceSD.HANDLE, CONFIG.planetSpaceSD);
		GetVariableFromWindow(CONFIG_H.moonDistanceBoundary.HANDLE, CONFIG.moonDistanceBoundary);
		CONFIG.generateDwarfPlanets = (IsDlgButtonChecked(hWnd, CB_GENERATEDWARFPLANET) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG_H.dwarfPlanetChance.HANDLE, CONFIG.dwarfPlanetChance);
		CONFIG.weightedMoons = (IsDlgButtonChecked(hWnd, CB_WEIGHTEDMOONS) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG_H.avgEccentricity.HANDLE, CONFIG.avgEccentricity); // change to extra for trackbar
		GetVariableFromWindow(CONFIG_H.SDEccentricity.HANDLE, CONFIG.SDEccentricity);
		GetVariableFromWindow(CONFIG_H.avgInclination.HANDLE, CONFIG.avgInclination);
		GetVariableFromWindow(CONFIG_H.SDInclination.HANDLE, CONFIG.SDInclination);
		GetVariableFromWindow(CONFIG_H.avgObliquity.HANDLE, CONFIG.avgObliquity);
		GetVariableFromWindow(CONFIG_H.SDObliquity.HANDLE, CONFIG.SDObliquity);
		CONFIG.generateAsteroidBelt = (IsDlgButtonChecked(hWnd, CB_GENERATEASTEROIDS) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG_H.maxAsteroidBelts.HANDLE, CONFIG.maxAsteroidBelts);
		GetVariableFromWindow(CONFIG_H.minAsteroidCount.HANDLE, CONFIG.minAsteroidCount);
		GetVariableFromWindow(CONFIG_H.maxAsteroidCount.HANDLE, CONFIG.maxAsteroidCount);
		CONFIG.generateComets = (IsDlgButtonChecked(hWnd, CB_GENERATECOMETS) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG_H.minCometCount.HANDLE, CONFIG.minCometCount);
		GetVariableFromWindow(CONFIG_H.maxCometCount.HANDLE, CONFIG.maxCometCount);

		GetVariableFromWindow(CONFIG_H.starClassO.HANDLE, CONFIG.starClassO);
		GetVariableFromWindow(CONFIG_H.starClassB.HANDLE, CONFIG.starClassB);
		GetVariableFromWindow(CONFIG_H.starClassA.HANDLE, CONFIG.starClassA);
		GetVariableFromWindow(CONFIG_H.starClassF.HANDLE, CONFIG.starClassF);
		GetVariableFromWindow(CONFIG_H.starClassG.HANDLE, CONFIG.starClassG);
		GetVariableFromWindow(CONFIG_H.starClassK.HANDLE, CONFIG.starClassK);
		GetVariableFromWindow(CONFIG_H.starClassM.HANDLE, CONFIG.starClassM);
		GetVariableFromWindow(CONFIG_H.starClassWD.HANDLE, CONFIG.starClassWD);
		GetVariableFromWindow(CONFIG_H.starClassQ.HANDLE, CONFIG.starClassQ);
		GetVariableFromWindow(CONFIG_H.starClassX.HANDLE, CONFIG.starClassX);

		GetVariableFromWindow(CONFIG_H.life_OrganicChance.HANDLE, CONFIG.life_OrganicChance);
		GetVariableFromWindow(CONFIG_H.life_ExoticChance.HANDLE, CONFIG.life_ExoticChance);
		GetVariableFromWindow(CONFIG_H.life_MulticellChance.HANDLE, CONFIG.life_MulticellChance);
		CONFIG.forceLife = (IsDlgButtonChecked(hWnd, CB_FORCELIFE) == BST_CHECKED) ? true : false;
		CONFIG.traditionalLife = (IsDlgButtonChecked(hWnd, CB_TRADITIONALLIFE) == BST_CHECKED) ? true : false;

		CONFIG.generateComposition = (IsDlgButtonChecked(hWnd, CB_GENERATECOMPOSITION) == BST_CHECKED) ? true : false;

		GetVariableFromWindow(CONFIG_H.exotic_ShipChance.HANDLE, CONFIG.exotic_ShipChance);
		GetVariableFromWindow(CONFIG_H.modelsFolder.HANDLE, CONFIG.modelsFolder);
		CONFIG.shipsNeedLife = (IsDlgButtonChecked(hWnd, CB_SHIPSNEEDLIFE) == BST_CHECKED) ? true : false;

		int size = CONFIG.shipList_Colony.size();
		for (int count = 0; count < size; count++)
			CONFIG.shipList_Colony.pop_back();
		size = CONFIG.shipList_Instrument.size();
		for (int count = 0; count < size; count++)
			CONFIG.shipList_Instrument.pop_back();
		size = CONFIG.shipList_Satellite.size();
		for (int count = 0; count < size; count++)
			CONFIG.shipList_Satellite.pop_back();
		size = CONFIG.shipList_Station.size();
		for (int count = 0; count < size; count++)
			CONFIG.shipList_Station.pop_back();

		int counter = 1;
		std::ifstream ShipInputTest;
		std::wstring shipFilePath, shipPrintHolder; // file path is used to open file, print is pushed onto vector

		shipFilePath = L"Spacecraft\\SFSG_Ships\\Colony\\colony_" + std::to_wstring(counter) + L".sss";
		shipFilePath = CONFIG.modelsFolder + shipFilePath;
		ShipInputTest.open(shipFilePath.c_str());

		while (ShipInputTest)
		{
			shipPrintHolder = L"Spacecraft/SFSG_Ships/Colony/colony_" + std::to_wstring(counter) + L".sss"; //  this is different because of how SE handles slashes: '/'
			CONFIG.shipList_Colony.push_back(shipPrintHolder);
			ShipInputTest.close();

			counter++;
			shipFilePath = L"Spacecraft\\SFSG_Ships\\Colony\\colony_" + std::to_wstring(counter) + L".sss";
			shipFilePath = CONFIG.modelsFolder + shipFilePath;
			ShipInputTest.open(shipFilePath.c_str());
		}

		counter = 1;
		shipFilePath = L"Spacecraft\\SFSG_Ships\\Instrument\\instrument_" + std::to_wstring(counter) + L".sss";
		shipFilePath = CONFIG.modelsFolder + shipFilePath;
		ShipInputTest.open(shipFilePath.c_str());
		while (ShipInputTest)
		{
			shipPrintHolder = L"Spacecraft/SFSG_Ships/Instrument/instrument_" + std::to_wstring(counter) + L".sss";
			CONFIG.shipList_Instrument.push_back(shipPrintHolder);
			ShipInputTest.close();

			counter++;
			shipFilePath = L"Spacecraft\\SFSG_Ships\\Instrument\\instrument_" + std::to_wstring(counter) + L".sss";
			shipFilePath = CONFIG.modelsFolder + shipFilePath;
			ShipInputTest.open(shipFilePath.c_str());
		}

		counter = 1;
		shipFilePath = L"Spacecraft\\SFSG_Ships\\Satellite\\satellite_" + std::to_wstring(counter) + L".sss";
		shipFilePath = CONFIG.modelsFolder + shipFilePath;
		ShipInputTest.open(shipFilePath.c_str());
		while (ShipInputTest)
		{
			shipPrintHolder = L"Spacecraft/SFSG_Ships/Satellite/satellite_" + std::to_wstring(counter) + L".sss";
			CONFIG.shipList_Satellite.push_back(shipPrintHolder);
			ShipInputTest.close();

			counter++;
			shipFilePath = L"Spacecraft\\SFSG_Ships\\Satellite\\satellite_" + std::to_wstring(counter) + L".sss";
			shipFilePath = CONFIG.modelsFolder + shipFilePath;
			ShipInputTest.open(shipFilePath.c_str());
		}

		counter = 1;
		shipFilePath = L"Spacecraft\\SFSG_Ships\\Station\\station_" + std::to_wstring(counter) + L".sss";
		shipFilePath = CONFIG.modelsFolder + shipFilePath;
		ShipInputTest.open(shipFilePath.c_str());
		while (ShipInputTest)
		{
			shipPrintHolder = L"Spacecraft/SFSG_Ships/Station/station_" + std::to_wstring(counter) + L".sss";
			CONFIG.shipList_Station.push_back(shipPrintHolder);
			ShipInputTest.close();

			counter++;
			shipFilePath = L"Spacecraft\\SFSG_Ships\\Station\\station_" + std::to_wstring(counter) + L".sss";
			shipFilePath = CONFIG.modelsFolder + shipFilePath;
			ShipInputTest.open(shipFilePath.c_str());
		}

		GetVariableFromWindow(CONFIG_H.exotic_OrbitChance.HANDLE, CONFIG.exotic_OrbitChance);
		GetVariableFromWindow(CONFIG_H.exotic_AxialTiltChance.HANDLE, CONFIG.exotic_AxialTiltChance);
		GetVariableFromWindow(CONFIG_H.exotic_DebrisRingChance.HANDLE, CONFIG.exotic_DebrisRingChance);
		GetVariableFromWindow(CONFIG_H.exotic_CompanionOrbitChance.HANDLE, CONFIG.exotic_CompanionOrbitChance);
	}
	bool CheckConfigData(HWND hWnd)
	{
		// This function contains most error checking for input by the user and stops the program if an error is found.

	#pragma region General Variables

		if (CONFIG.numberOfRuns < 1)
			return MessageBox(hWnd, L"You must generate more than 0 systems for anything to work!", L"Error", MB_ICONERROR);

	#pragma endregion

	#pragma region System Variables
		if (CONFIG.minDistance <= 0)
			return MessageBox(hWnd, L"Min/Max distance from Earth cannot be zero or negative!", L"Error", MB_ICONERROR);
		if (CONFIG.maxDistance < CONFIG.minDistance)
			return MessageBox(hWnd, L"Minimum Distance must be smaller than Maximum Distance!", L"Error", MB_ICONERROR);

		if (CONFIG.systemRadiusModifier <= 0)
			return MessageBox(hWnd, L"System radius modifier must be larger than zero!", L"Error", MB_ICONERROR);
		if (CONFIG.planetSpaceAvg < 1)
			return MessageBox(hWnd, L"Planet Spacing Average must be a number greater than one!", L"Error", MB_ICONERROR);

		if (CONFIG.SDObliquity <= 0)
			return MessageBox(hWnd, L"Standard Deviation cannot be less than or equal to zero!", L"Error", MB_ICONERROR);
		if (CONFIG.SDInclination <= 0)
			return MessageBox(hWnd, L"Standard Deviation cannot be less than or equal to zero!", L"Error", MB_ICONERROR);
		if (CONFIG.avgEccentricity <= 0 || CONFIG.avgEccentricity >= 1)
			return MessageBox(hWnd, L"Eccentricity must be a value between (but not equal to) zero and one!", L"Error", MB_ICONERROR);
		if (CONFIG.SDEccentricity <= 0)
			return MessageBox(hWnd, L"Standard Deviation cannot be less than or equal to zero!", L"Error", MB_ICONERROR);
		if (CONFIG.SDEccentricity >= 1)
			return MessageBox(hWnd, L"Standard Deviation for eccentricity is too high! The program will waste too much time throwing away unusable numbers!", L"Error", MB_ICONERROR);

		if (CONFIG.generateDwarfPlanets)
		{
			if (CONFIG.dwarfPlanetChance >= 100 || CONFIG.dwarfPlanetChance < 0)
				return MessageBox(hWnd, L"The chance for dwarf planets to spawn should be a number from 0 to 99!", L"Error", MB_ICONERROR);
		}

		if (CONFIG.minPlanetNumber < 0 || CONFIG.minPlanetNumber > 100)
			return MessageBox(hWnd, L"Min % of system filled is a percentage, so it must be between 0 and 100!", L"Error", MB_ICONERROR);

		if (CONFIG.generateAsteroidBelt)
		{
			if (CONFIG.maxAsteroidBelts < 1)
				return MessageBox(hWnd, L"Max asteroid belts must be at least one!\nIf you want to turn asteroid belts off, disable them with the checkbox.", L"Error", MB_ICONERROR);
			if (CONFIG.minAsteroidCount > CONFIG.maxAsteroidCount)
				return MessageBox(hWnd, L"Minimum number of asteroids per belt cannot be greater than maximum number of asteroids per belt!", L"Error", MB_ICONERROR);
		}
		if (CONFIG.generateComets)
		{
			if (CONFIG.minCometCount > CONFIG.maxCometCount)
				return MessageBox(hWnd, L"Minimum number of comets cannot be greater than maximum number of comets!", L"Error", MB_ICONERROR);
		}
			
		if (CONFIG.starClassA < 0 || CONFIG.starClassB < 0 || CONFIG.starClassF < 0 || CONFIG.starClassG < 0 || CONFIG.starClassK < 0 || CONFIG.starClassM < 0 || CONFIG.starClassO < 0 ||
			CONFIG.starClassQ < 0 || CONFIG.starClassWD < 0 || CONFIG.starClassX < 0)
			return MessageBox(hWnd, L"A star class cannot have a weight less than zero!", L"Error", MB_ICONERROR);

		if (CONFIG.starClassA == 0 && CONFIG.starClassB == 0 && CONFIG.starClassF == 0 && CONFIG.starClassG == 0 && CONFIG.starClassK == 0 && CONFIG.starClassM == 0 && CONFIG.starClassO == 0 &&
			CONFIG.starClassQ == 0 && CONFIG.starClassWD == 0 && CONFIG.starClassX == 0)
			return MessageBox(hWnd, L"All star classes have a weight of zero! That means no stars are avaialbe to be generated!\nFix this by giving any star class a weight higher than zero.", L"Error", MB_ICONERROR);

	#pragma endregion

	#pragma region Planet Variables

		if (CONFIG.moonDistanceBoundary < 0)
			return MessageBox(hWnd, L"Moon distance boundary cannot be less than zero!", L"Error", MB_ICONERROR);

	#pragma endregion

		return false; // No problems found in config
	}
	void CreateNameVectors(HWND hWnd)
	{	
		if (NV.usedNames.size() > 0)
		{
			int size = NV.usedNames.size();
			for (int i = 0; i < size; i++)
				NV.usedNames.pop_back();
		}

		if (NV.PrefixList.size() > 0)
		{
			int size = NV.PrefixList.size();
			for (int i = 0; i < size; i++)
				NV.PrefixList.pop_back();
		}
		if (NV.SuffixList.size() > 0)
		{
			int size = NV.SuffixList.size();
			for (int i = 0; i < size; i++)
				NV.SuffixList.pop_back();
		}
		NV.useSimpleGenerator = (IsDlgButtonChecked(NV.GROUP_SIMPLE, NVCB_SIMPLEGENERATOR) == BST_CHECKED) ? true : false;
		FillModList(NV.PrefixListH, NV.PrefixList);
		FillModList(NV.SuffixListH, NV.SuffixList);

		if (NV.StarPreMods.size() > 0)
		{
			int size = NV.StarPreMods.size();
			for (int i = 0; i < size; i++)
				NV.StarPreMods.pop_back();
		}
		if (NV.StarPostMods.size() > 0)
		{
			int size = NV.StarPostMods.size();
			for (int i = 0; i < size; i++)
				NV.StarPostMods.pop_back();
		}
		NV.useStarPreMods = (IsDlgButtonChecked(NV.GROUP_STAR, NVCB_STARPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probStarPreModH, NV.probStarPreMod);
		FillModList(NV.starPreModList, NV.StarPreMods);
		NV.useStarPostMods = (IsDlgButtonChecked(NV.GROUP_STAR, NVCB_STARPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probStarPostModH, NV.probStarPostMod);
		FillModList(NV.starPostModList, NV.StarPostMods);
		NV.useStarNumberMods = (IsDlgButtonChecked(NV.GROUP_STAR, NVCB_STARNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probStarNumberModH, NV.probStarNumberMod);

		if (NV.PlanetPreMods.size() > 0)
		{
			int size = NV.PlanetPreMods.size();
			for (int i = 0; i < size; i++)
				NV.PlanetPreMods.pop_back();
		}
		if (NV.PlanetPostMods.size() > 0)
		{
			int size = NV.PlanetPostMods.size();
			for (int i = 0; i < size; i++)
				NV.PlanetPostMods.pop_back();
		}
		NV.usePlanetPreMods = (IsDlgButtonChecked(NV.GROUP_PLANET, NVCB_PLANETPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probPlanetPreModH, NV.probPlanetPreMod);
		FillModList(NV.planetPreModList, NV.PlanetPreMods);
		NV.usePlanetPostMods = (IsDlgButtonChecked(NV.GROUP_PLANET, NVCB_PLANETPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probPlanetPostModH, NV.probPlanetPostMod);
		FillModList(NV.planetPostModList, NV.PlanetPostMods);
		NV.usePlanetNumberMods = (IsDlgButtonChecked(NV.GROUP_PLANET, NVCB_PLANETNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probPlanetNumberModH, NV.probPlanetNumberMod);

		if (NV.MoonPreMods.size() > 0)
		{
			int size = NV.MoonPreMods.size();
			for (int i = 0; i < size; i++)
				NV.MoonPreMods.pop_back();
		}
		if (NV.MoonPostMods.size() > 0)
		{
			int size = NV.MoonPostMods.size();
			for (int i = 0; i < size; i++)
				NV.MoonPostMods.pop_back();
		}
		NV.nameTerraMoons = (IsDlgButtonChecked(NV.GROUP_MOON, NVCB_NAMETERRAMOONS) == BST_CHECKED) ? true : false;
		NV.nameGasMoons = (IsDlgButtonChecked(NV.GROUP_MOON, NVCB_NAMEGASMOONS) == BST_CHECKED) ? true : false;
		NV.useMoonPreMods = (IsDlgButtonChecked(NV.GROUP_MOON, NVCB_MOONPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probMoonPreModH, NV.probMoonPreMod);
		FillModList(NV.moonPreModList, NV.MoonPreMods);
		NV.useMoonPostMods = (IsDlgButtonChecked(NV.GROUP_MOON, NVCB_MOONPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probMoonPostModH, NV.probMoonPostMod);
		FillModList(NV.moonPostModList, NV.MoonPostMods);
		NV.useMoonNumberMods = (IsDlgButtonChecked(NV.GROUP_MOON, NVCB_MOONNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probMoonNumberModH, NV.probMoonNumberMod);

		if (NV.AsteroidPreMods.size() > 0)
		{
			int size = NV.AsteroidPreMods.size();
			for (int i = 0; i < size; i++)
				NV.AsteroidPreMods.pop_back();
		}
		if (NV.AsteroidPostMods.size() > 0)
		{
			int size = NV.AsteroidPostMods.size();
			for (int i = 0; i < size; i++)
				NV.AsteroidPostMods.pop_back();
		}
		NV.nameAsteroids = (IsDlgButtonChecked(NV.GROUP_ASTEROID, NVCB_NAMEASTEROIDS) == BST_CHECKED) ? true : false;
		NV.useAsteroidPreMods = (IsDlgButtonChecked(NV.GROUP_ASTEROID, NVCB_ASTEROIDPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probAsteroidPreModH, NV.probAsteroidPreMod);
		FillModList(NV.asteroidPreModList, NV.AsteroidPreMods);
		NV.useAsteroidPostMods = (IsDlgButtonChecked(NV.GROUP_ASTEROID, NVCB_ASTEROIDPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probAsteroidPostModH, NV.probAsteroidPostMod);
		FillModList(NV.asteroidPostModList, NV.AsteroidPostMods);
		NV.useAsteroidNumberMods = (IsDlgButtonChecked(NV.GROUP_ASTEROID, NVCB_ASTEROIDNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probAsteroidNumberModH, NV.probAsteroidNumberMod);

		if (NV.CometPreMods.size() > 0)
		{
			int size = NV.CometPreMods.size();
			for (int i = 0; i < size; i++)
				NV.CometPreMods.pop_back();
		}
		if (NV.CometPostMods.size() > 0)
		{
			int size = NV.CometPostMods.size();
			for (int i = 0; i < size; i++)
				NV.CometPostMods.pop_back();
		}
		NV.nameComets = (IsDlgButtonChecked(NV.GROUP_COMET, NVCB_NAMECOMETS) == BST_CHECKED) ? true : false;
		NV.useCometPreMods = (IsDlgButtonChecked(NV.GROUP_COMET, NVCB_COMETPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probCometPreModH, NV.probCometPreMod);
		FillModList(NV.cometPreModList, NV.CometPreMods);
		NV.useCometPostMods = (IsDlgButtonChecked(NV.GROUP_COMET, NVCB_COMETPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probCometPostModH, NV.probCometPostMod);
		FillModList(NV.cometPostModList, NV.CometPostMods);
		NV.useCometNumberMods = (IsDlgButtonChecked(NV.GROUP_COMET, NVCB_COMETNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probCometNumberModH, NV.probCometNumberMod);

		if (NV.DwarfMoonPreMods.size() > 0)
		{
			int size = NV.DwarfMoonPreMods.size();
			for (int i = 0; i < size; i++)
				NV.DwarfMoonPreMods.pop_back();
		}
		if (NV.DwarfMoonPostMods.size() > 0)
		{
			int size = NV.DwarfMoonPostMods.size();
			for (int i = 0; i < size; i++)
				NV.DwarfMoonPostMods.pop_back();
		}
		NV.nameTerraDwarfMoons = (IsDlgButtonChecked(NV.GROUP_DWARFMOON, NVCB_NAMETERRADWARFMOONS) == BST_CHECKED) ? true : false;
		NV.nameGasDwarfMoons = (IsDlgButtonChecked(NV.GROUP_DWARFMOON, NVCB_NAMEGASDWARFMOONS) == BST_CHECKED) ? true : false;
		NV.useDwarfMoonPreMods = (IsDlgButtonChecked(NV.GROUP_DWARFMOON, NVCB_DWARFMOONPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probDwarfMoonPreModH, NV.probDwarfMoonPreMod);
		FillModList(NV.dwarfMoonPreModList, NV.DwarfMoonPreMods);
		NV.useDwarfMoonPostMods = (IsDlgButtonChecked(NV.GROUP_DWARFMOON, NVCB_DWARFMOONPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probDwarfMoonPostModH, NV.probDwarfMoonPostMod);
		FillModList(NV.dwarfMoonPostModList, NV.DwarfMoonPostMods);
		NV.useDwarfMoonNumberMods = (IsDlgButtonChecked(NV.GROUP_DWARFMOON, NVCB_DWARFMOONNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probDwarfMoonNumberModH, NV.probDwarfMoonNumberMod);

		if (NV.ShipPreMods_All.size() > 0)
		{
			int size = NV.ShipPreMods_All.size();
			for (int i = 0; i < size; i++)
				NV.ShipPreMods_All.pop_back();
		}
		if (NV.ShipPostMods_All.size() > 0)
		{
			int size = NV.ShipPostMods_All.size();
			for (int i = 0; i < size; i++)
				NV.ShipPostMods_All.pop_back();
		}
		NV.useShipPreMods_All = (IsDlgButtonChecked(NV.GROUP_SHIP_ALL, NVCB_SHIPALLPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_AllH, NV.probShipPreMod_All);
		FillModList(NV.shipPreModList_All, NV.ShipPreMods_All);
		NV.useShipPostMods_All = (IsDlgButtonChecked(NV.GROUP_SHIP_ALL, NVCB_SHIPALLPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_AllH, NV.probShipPostMod_All);
		FillModList(NV.shipPostModList_All, NV.ShipPostMods_All);

		if (NV.ShipPreMods_Colony.size() > 0)
		{
			int size = NV.ShipPreMods_Colony.size();
			for (int i = 0; i < size; i++)
				NV.ShipPreMods_Colony.pop_back();
		}
		if (NV.ShipPostMods_Colony.size() > 0)
		{
			int size = NV.ShipPostMods_Colony.size();
			for (int i = 0; i < size; i++)
				NV.ShipPostMods_Colony.pop_back();
		}
		NV.useShipPreMods_Colony = (IsDlgButtonChecked(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_ColonyH, NV.probShipPreMod_Colony);
		FillModList(NV.shipPreModList_Colony, NV.ShipPreMods_Colony);
		NV.useShipPostMods_Colony = (IsDlgButtonChecked(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_ColonyH, NV.probShipPostMod_Colony);
		FillModList(NV.shipPostModList_Colony, NV.ShipPostMods_Colony);
		NV.useShipNumberMods_Colony = (IsDlgButtonChecked(NV.GROUP_SHIP_COLONY, NVCB_SHIPCOLONYNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipNumberMod_ColonyH, NV.probShipNumberMod_Colony);

		if (NV.ShipPreMods_Instrument.size() > 0)
		{
			int size = NV.ShipPreMods_Instrument.size();
			for (int i = 0; i < size; i++)
				NV.ShipPreMods_Instrument.pop_back();
		}
		if (NV.ShipPostMods_Instrument.size() > 0)
		{
			int size = NV.ShipPostMods_Instrument.size();
			for (int i = 0; i < size; i++)
				NV.ShipPostMods_Instrument.pop_back();
		}
		NV.useShipPreMods_Instrument = (IsDlgButtonChecked(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_InstrumentH, NV.probShipPreMod_Instrument);
		FillModList(NV.shipPreModList_Instrument, NV.ShipPreMods_Instrument);
		NV.useShipPostMods_Instrument = (IsDlgButtonChecked(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_InstrumentH, NV.probShipPostMod_Instrument);
		FillModList(NV.shipPostModList_Instrument, NV.ShipPostMods_Instrument);
		NV.useShipNumberMods_Instrument = (IsDlgButtonChecked(NV.GROUP_SHIP_INSTRUMENT, NVCB_SHIPINSTRUMENTNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipNumberMod_InstrumentH, NV.probShipNumberMod_Instrument);

		if (NV.ShipPreMods_Satellite.size() > 0)
		{
			int size = NV.ShipPreMods_Satellite.size();
			for (int i = 0; i < size; i++)
				NV.ShipPreMods_Satellite.pop_back();
		}
		if (NV.ShipPostMods_Satellite.size() > 0)
		{
			int size = NV.ShipPostMods_Satellite.size();
			for (int i = 0; i < size; i++)
				NV.ShipPostMods_Satellite.pop_back();
		}
		NV.useShipPreMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITEPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_SatelliteH, NV.probShipPreMod_Satellite);
		FillModList(NV.shipPreModList_Satellite, NV.ShipPreMods_Satellite);
		NV.useShipPostMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITEPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_SatelliteH, NV.probShipPostMod_Satellite);
		FillModList(NV.shipPostModList_Satellite, NV.ShipPostMods_Satellite);
		NV.useShipNumberMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SHIP_SATELLITE, NVCB_SHIPSATELLITENUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipNumberMod_SatelliteH, NV.probShipNumberMod_Satellite);
		
		if (NV.ShipPreMods_Station.size() > 0)
		{
			int size = NV.ShipPreMods_Station.size();
			for (int i = 0; i < size; i++)
				NV.ShipPreMods_Station.pop_back();
		}
		if (NV.ShipPostMods_Station.size() > 0)
		{
			int size = NV.ShipPostMods_Station.size();
			for (int i = 0; i < size; i++)
				NV.ShipPostMods_Station.pop_back();
		}
		NV.useShipPreMods_Station = (IsDlgButtonChecked(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_StationH, NV.probShipPreMod_Station);
		FillModList(NV.shipPreModList_Station, NV.ShipPreMods_Station);
		NV.useShipPostMods_Station = (IsDlgButtonChecked(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_StationH, NV.probShipPostMod_Station);
		FillModList(NV.shipPostModList_Station, NV.ShipPostMods_Station);
		NV.useShipNumberMods_Station = (IsDlgButtonChecked(NV.GROUP_SHIP_STATION, NVCB_SHIPSTATIONNUMBERMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipNumberMod_StationH, NV.probShipNumberMod_Station);

		if (NV.Markov_RawDataset.size() > 0)
		{
			int size = NV.Markov_RawDataset.size();
			for (int i = 0; i < size; i++)
				NV.Markov_RawDataset.pop_back();
		}
		if (NV.main_ngrams.ngrams.size() > 0)
		{
			int size = NV.main_ngrams.ngrams.size();
			for (int i = 0; i < size; i++)
			{
				NV.main_ngrams.ngrams.pop_back();
				NV.main_ngrams.nextCharList.pop_back();
			}
			size = NV.twogram_list.ngrams.size();
			for (int i = 0; i < size; i++)
			{
				NV.twogram_list.ngrams.pop_back();
				NV.twogram_list.nextCharList.pop_back();
			}
		}
		GetVariableFromWindow(NV.orderH, NV.order);
		GetVariableFromWindow(NV.wordVarienceH, NV.wordVarience);
		GetVariableFromWindow(NV.min_lengthH, NV.min_length);
		GetVariableFromWindow(NV.max_lengthH, NV.max_length);
		GetVariableFromWindow(NV.wordPercentH.HANDLE, NV.wordPercent);
		FillDataset(NV.Markov_RawDatasetH, NV.Markov_RawDataset, NV.usedNames);

		// Creates lists of all n combinations of letters in the data set
		for (int count = 0; count < NV.Markov_RawDataset.size(); count++)
		{
			int Length_Test = ((NV.Markov_RawDataset.at(count).length()) - (NV.order + 1));
			for (int i = 0; i <= Length_Test; i++)
			{
				std::wstring gram(NV.Markov_RawDataset.at(count), i, NV.order);

				bool test = 0;
				for (int j = 0; j < NV.main_ngrams.ngrams.size(); j++)
				{
					if (NV.main_ngrams.ngrams.at(j) == gram)
					{
						std::wstring next_letter(NV.Markov_RawDataset.at(count), (i + NV.order), 1);
						NV.main_ngrams.nextCharList.at(j) += next_letter;
						test = 1;
					}
				}
				if (test == 0)
				{
					if (NV.Markov_RawDataset.at(count).length() > NV.order)
					{
						NV.main_ngrams.ngrams.push_back(gram);
						std::wstring next_letter(NV.Markov_RawDataset.at(count), (i + NV.order), 1);
						NV.main_ngrams.nextCharList.push_back(next_letter);
					}
					else
						NV.main_ngrams.nextCharList.push_back(L"");
				}

			}
		}

		// Creates lists of all Two Gram combinations in the data set
		for (int count = 0; count < NV.Markov_RawDataset.size(); count++)
		{
			int Length_Test = ((NV.Markov_RawDataset.at(count).length()) - 3);
			for (int i = 0; i <= Length_Test; i++)
			{
				std::wstring gram(NV.Markov_RawDataset.at(count), i, 2);

				bool test = 0;
				for (int j = 0; j < NV.twogram_list.ngrams.size(); j++)
				{
					if (NV.twogram_list.ngrams.at(j) == gram)
					{
						std::wstring next_letter(NV.Markov_RawDataset.at(count), (i + 2), 1);
						NV.twogram_list.nextCharList.at(j) += next_letter;
						test = 1;
					}
				}
				if (test == 0)
				{
					if (NV.Markov_RawDataset.at(count).length() > 2)
					{
						NV.twogram_list.ngrams.push_back(gram);
						std::wstring next_letter(NV.Markov_RawDataset.at(count), (i + 2), 1);
						NV.twogram_list.nextCharList.push_back(next_letter);
					}
					else
						NV.twogram_list.nextCharList.push_back(L"");
				}

			}
		}
	}
	void TestNames()
	{
		mt_name.seed(time(0));

		wchar_t sampleNames[WSIZE] = L"Sample names with the current dataset:\n\n";
		int pos = 40;
		std::wstring name;

		for (int count = 0; count < 13; count++)
		{
			std::uniform_int_distribution<int> gentype{ 1, 4 };
			name = GenName(static_cast<Object_Type>(gentype(mt_name)));
			for (int i = 0; i < name.size(); i++, pos++)
			{
				if (pos < 256)
					sampleNames[pos] = name.at(i);
			}
			if (pos < 256)
				sampleNames[pos++] = '\n';
		}

		if (pos > 255)
			sampleNames[255] = '\0';
		else
			sampleNames[pos] = '\0';

		SetWindowTextW(CONFIG_H.INFO_BOX, sampleNames);
	}

	void SetInfoBox(int command)
	{
		switch (command)
		{
//###############
	#pragma region General
//###############
		case IB_SEED:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The seed is the unique identifier of each system generated. Leave this at 0 for a random seed, or type in a value if you want to generate a system more than once.\n\nNote that a seed may not generate the exact same system if certain variables are changed.\n\nYou can also find the seed of any system by looking in the Star file.");
			break;
		case IB_NUMBEROFRUNS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is how many systems the program will generate.");
			break;
		case IB_DEBUG:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Enable debug mode.");
			break;
		case IB_STAROUTPUTFOLDER:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The Star file will be placed in this folder after generation is finished.\n\nThe Star file should then be placed under \"addons\\catalogs\\stars\\\" in the Space Engine folder.");
			break;
		case IB_PLANETOUTPUTFOLDER:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The Planet file will be placed in this folder after generation is finished.\n\nThe Planet file should then be placed under \"addons\\catalogs\\planets\\\" in the Space Engine folder.");
			break;
		case IB_SELECTPRESET:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Select a variable preset from the drop down menu, then click update to apply the changes to the generator.");
			break;
		case IB_SAVEPRESET:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Save the current variables as a new preset. Type the file name in the text box, then hit the \"save\" button.");
			break;
		case IB_SELECTNAMEPRESET:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Select a name preset from the drop down menu, then click update to apply the changes to the name generator. To save name presets, go to the \"advanced\" tab.");
			break;
	#pragma endregion
//###############
	#pragma region System
//###############
		case IB_SMARTPLACEMENT:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"When smart placement is enabled, the generator will try to place planets in relatively realistic locations. So typically you will find gas giants beyond the frost limit and rocky planets closer to the star. If disabled, any planet type can spawn in any location.");
			break;
		case IB_SYSTEMRADIUSMODIFIER:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This number modifies how far away a star's outer limit is. Past the outer limit, planets do not normally orbit the parent star.\n\n1.0 = a normal/realistic outer limit.\nA higher number = a further outer limit, meaning more planets can spawn.");
			break;
		case IB_DISTANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is the distance from Earth measured in parsecs. Note that large distances could make systems spawn very far away from the galaxy.");
			break;
		case IB_PLANETSPACING:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"These numbers affect how close planets can possibly be to each other, and also how many planets a system can have.\n\nThe generator works by creating a number of possible points planets can spwan at around the parent star. These numbers are modified by the planet spacing to make them closer togther or farther apart.\n\nHigh numbers = closer planets.\nCloser planets = more planets per system.");
			break;
		case IB_OBLIQUITY:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The average axial tilt of generated planets and a standard deviation, measured in degrees.\n\nThis can technically be any number, but anything outside of 0 - 360 is unecessary.");
			break;
		case IB_INCLINATION:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The average inclination of a planet's orbit around it's star and a stardard deviation, measured in degrees.\n\nThis can technically be any number, but anything outside of 0 - 360 is unecessary.");
			break;
		case IB_ECCENTRICITY:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The average eccentricity of a planet's orbit and a standard deviation, measured from 0 (zero) - 1 (one), but NOT including zero or one.\n\nThese boxes MUST have a decimal number between zero and one or the generator WILL break.");
			break;
		case IB_MINPLANETNUMBER:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The minimum percent of the system that should be filled with planets. Based on how many planets the star can hold.\n\nExample: If a star's maximum planet number is 10, and min % system filled is 20, then then star can generate anywhere from 2 (20% of total) to 10 planets. ");
			break;
		case IB_STARCLASS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"The weight assigned to each star class tells the generator how often to generate that class. So if one class has a weight of 1 (one) and another class has a weight of 10 (ten), the class with a weight of 10 is much more likely to be generated.");
			break;
		case IB_GENERATEDWARFPLANET:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If enabled, dwarf planets can spawn in the system, but do not affect total planet count. ");
			break;
		case IB_DWARFPALNETCHANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Percent chance for dwarf planets to generate and continue generating after the first. For every dwarf planet generated, one percent is subtracted from the total.");
			break;
		case IB_GENERATEASTEROIDS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Allows asteroid belts to spawn around the system. Asteroid belt can spawn between planets, very close to the star, or very far away from the star.");
			break;
		case IB_MAXASTEROIDBELTS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is the number of belts a system will spawn with. If asteroid belts are enabled, this value must be at least one");
			break;
		case IB_ASTEROIDCOUNT:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"asteroids min/max count");
			break;
		case IB_GENERATECOMETS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"generate comets");
			break;
		case IB_COMETCOUNT:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"comet min/max count");
			break;
		
	#pragma endregion
//###############
	#pragma region Planet
//###############
		case IB_WEIGHTEDMOONS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If enabled, moons will generate based on a new weight system, hopefully making generation more interesting and realistic.\n\nDisabled will use an older system with hard limits on how many moons an object can have.");
			break;
		case IB_MOONDISTANCEBOUNDARY:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Moon distance boundary defines how much space should be inbetween major moons, measured by the parent planet's radius. Gas giants are ten times less than this number.\n\nSetting this number to 0 (zero) will disable this feature, so major moons could spawn very close to each other.");
			break;
		case IB_LIFEORGANICCHANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Organic life."); // figure out what to put here
			break;
		case IB_LIFEEXOTICCHANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"exotic life"); // figure out what to put here
			break;
		case IB_LIFEMULTICHANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"multicel life"); // figure out what to put here
			break;
		case IB_FORCELIFE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If enabled, this will force every system generated to have at least one planet with life, no matter what type.");
			break;
		case IB_TRADITIONALLIFE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If enabled, organic life can only spawn if a planet is near the habitable zone, while exotic life can spawn anywhere.");
			break;
		case IB_EXOTICORBIT:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Pecent chance for a planet to have a high inclination and/or a retrograde orbit.");
			break;
		case IB_EXOTICOBLIQUITY:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Pecent chance for a planet to have a high degree axial tilt and/or a retrograde rotation.");
			break;
		case IB_EXOTICDEBRISRING:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"A debris ring is a cluster of moons orbiting near a planet resembling a ring. They come in many sizes.");
			break;
		case IB_EXOTICCOMPANION:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"A comapnion orbit is an object with the same orbit as another object, but orbiting on the oposite side side of the orbit.");
			break;
	#pragma endregion
//###############
	#pragma region Surface
//###############	
		case IB_GENERATECOMPOSITION:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Enables planet interior composition, which may affect planet generation.");
			break;
	#pragma endregion
//###############
	#pragma region Special
//###############	
		case IB_SHIPCHANCE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Percent chance that ships will spawn somewhere in the system and continue spawning after the first. Zero by default because you must set up the models folder first. Everytime a ship is created, one percent is subtracted from the total.");
			break;
		case IB_SHIPSNEEDLIFE:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If enabled, ships require a planet with life to have generated somewhere in the system to spawn.");
			break;
		case IB_MODELSFOLDER:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is the folder the generator pulls ship models from. The models folder that came with this generator should be placed under \"addons\\models\\\" in your Space Engine folder, then make sure to copy/paste the full file (ending with \"models\\\" ) path in the text box here.\n\nYou can also add your own ship models to this folder and the generator will use them.");
			break;
	#pragma endregion
//###############
	#pragma region Advanced
//###############
		case IB_SAVENAMEPRESET:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Save the current name variables as a new preset. Type the file name in the text box, then hit the \"save\" button.");
			break;
		case IB_MARKOV:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This program generates names using something called a Markov Chain. Basically, it uses an existing set of words (the dataset on this page) to create new words that look similar. If the dataset is very large, this can be a time consuming process, which is why you MUST press the \"Update Names\" button on this page if you make any changes to the dataset.\n\nOrder affects similarity to the dataset. Higher number = higher similarity.\nVarience affects randomness.\nMin/Max length affects total characters in each word.");
			break;
		case IB_SIMPLEGENERATOR:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is an old but fairly fast name generator that starts with a prefix and adds suffixes onto it randomly until it decides to stop. Doesn't create the best names, but very fast. If enabled, the dataset will be ignored, along with the markov chain generator.");
			break;
		case IB_NAMEPREMODS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Pre-name modifiers are words that can be added onto the front of a generated name.\n\nEach category of object has it's own list, as well as a percentage chance for a name to have a pre-modification.");
			break;
		case IB_NAMEPOSTMODS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Post-name modifiers are words that can be added onto the end of a generated name.\n\nEach category of object has it's own list, as well as a percentage chance for a name to have a post-modification.");
			break;
		case IB_NAMENUMBERMODS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Number modifiers can be added before or after a generated name. These are somewhat random collections of digits, roman numerals, and other symbols to add flavor to named objects.\n\nEach category of object has a percentage chance for a name to have a number modifier.");
			break;
		case IB_NAMEPREMODLIST:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Type pre-modifiers for the current object in this box.");
			break;
		case IB_NAMEPOSTMODLIST:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"Type post-modifiers for the current object in this box.");
			break;
		case IB_NAMEMOONS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If disabled, moons/dwarf moons will not generate names around rocky planets or gas giants, but will instead have a name counting up from 1. This will make the generator much faster since a lot of time is spent generating moon names, especially around gas giants.");
			break;
		case IB_WORDPERCENT:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"This is the percentage chance for a name to have another word added to it, and then to continue having more words. Every time another word is added to a name, once percent is subtracted from the total.");
			break;
		case IB_NAMEASTEROIDS:
			SetWindowTextW(CONFIG_H.INFO_BOX, L"If disabled, asteroids/comets will not generate names, but will instead have a name counting up from 1. This will make the generator much faster since a lot of time could be spent generating asteroid/comet names if they are enabled.\n\nThis checkbox can be toggled for asteroids, comets, or both.");
			break;
		}
	#pragma endregion
//###############
		
	}
	void SetCheckBoxText(HWND hWnd, int command)
	{
		int msg = IsDlgButtonChecked(hWnd, command);

		switch (command)
		{
		case CB_SMARTPLACEMENT:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.smartPlacement.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.smartPlacement.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_TRADITIONALLIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.traditionalLife.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.traditionalLife.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_FORCELIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.forceLife.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.forceLife.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_SHIPSNEEDLIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.shipsNeedLife.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.shipsNeedLife.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_GENERATEDWARFPLANET:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.generateDwarfPlanets.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.generateDwarfPlanets.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_WEIGHTEDMOONS:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.weightedMoons.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.weightedMoons.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_GENERATECOMPOSITION:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.generateComposition.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.generateComposition.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_GENERATEASTEROIDS:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.generateAsteroidBelt.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.generateAsteroidBelt.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_GENERATECOMETS:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG_H.generateComets.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG_H.generateComets.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_DEBUG:
			switch (msg)
			{
			case BST_UNCHECKED:
				CheckDlgButton(hWnd, CB_DEBUG, true);
				MessageBox(hWnd, L"Debug mode turned ON.", L"Debug", MB_OK);
				break;
			case BST_CHECKED:
				CheckDlgButton(hWnd, CB_DEBUG, false);
				MessageBox(hWnd, L"Debug mode turned OFF.", L"Debug", MB_OK);
				break;
			}
			break;
		}
	}

	void BrowseFolder(HWND textBox)
	{
		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (SUCCEEDED(hr))
		{
			IFileOpenDialog* pFileOpen;

			// Create the FileOpenDialog object.
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				pFileOpen->SetOptions(FOS_PICKFOLDERS);
				hr = pFileOpen->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileOpen->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							//###################################
							// This is where the magic happens
							int i = 0;
							while (pszFilePath[i] != '\0')
								i++;
							pszFilePath[i++] = '\\'; // adds a '\' to the end of the string
							pszFilePath[i] = '\0';
							
							SetWindowTextW(textBox, pszFilePath); // put files path in the textBox handle
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
	}

	/*---------------------------------------------------------------------------------------#
	|	THE GENERATOR:																		 |
	#---------------------------------------------------------------------------------------*/

	std::wstring GenName(Object_Type type)
	{
		//######################################################################################################
			//  VARIABLES

		std::wstring finalName;
		int wordCount, localWP, syllCount, syllPercent; // number of words in the name, local word percent, Number of syllables in a word and percent for multiple syllables
		bool has_prename_mod, has_postname_mod, has_number_mod, has_shipall_premod, has_shipall_postmod, testName;

		//######################################################################################################
			// PUTS NAME TOGETHER

		do //loop to check if the name has been used already
		{
			finalName = L"";
			localWP = NV.wordPercent;
			wordCount = 1;
			testName = false;
			has_prename_mod = false;
			has_postname_mod = false;
			has_number_mod = false;
			has_shipall_premod = false;
			has_shipall_postmod = false;

			//######################################################################################################
				//  GENERATES NUMBER OF WORDS

			while (genpercent(mt_name) < localWP--)
				wordCount++;

			//######################################################################################################
				// MODIFIER PICKER

			switch (type)
			{
			case typeAsteroid:
			{
				if (NV.useAsteroidPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probAsteroidPreMod) ? true : false;
				if (NV.useAsteroidPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probAsteroidPostMod) ? true : false;
				if (NV.useAsteroidNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probAsteroidNumberMod) ? true : false;
			}
				break;
			case typeComet:
			{
				if (NV.useCometPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probCometPreMod) ? true : false;
				if (NV.useCometPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probCometPostMod) ? true : false;
				if (NV.useCometNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probCometNumberMod) ? true : false;
			}
				break;
			case typeDwarfMoon:
			{
				if (NV.useDwarfMoonPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probDwarfMoonPreMod) ? true : false;
				if (NV.useDwarfMoonPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probDwarfMoonPostMod) ? true : false;
				if (NV.useDwarfMoonNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probDwarfMoonNumberMod) ? true : false;
			}
				break;
			case typeMoon:
			{
				if (NV.useMoonPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probMoonPreMod) ? true : false;
				if (NV.useMoonPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probMoonPostMod) ? true : false;
				if (NV.useMoonNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probMoonNumberMod) ? true : false;
			}			
				break;
			case typePlanet:
			{
				if (NV.usePlanetPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probPlanetPreMod) ? true : false;
				if (NV.usePlanetPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probPlanetPostMod) ? true : false;
				if (NV.usePlanetNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probPlanetNumberMod) ? true : false;
			}
				break;
			case typeShipColony:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) < NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) < NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Colony)
					has_prename_mod = (genpercent(mt_name) < NV.probShipPreMod_Colony) ? true : false;
				if (NV.useShipPostMods_Colony)
					has_postname_mod = (genpercent(mt_name) < NV.probShipPostMod_Colony) ? true : false;
				if (NV.useShipNumberMods_Colony)
					has_number_mod = (genpercent(mt_name) < NV.probShipNumberMod_Colony) ? true : false;
			}
				break;
			case typeShipInstrument:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) < NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) < NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Instrument)
					has_prename_mod = (genpercent(mt_name) < NV.probShipPreMod_Instrument) ? true : false;
				if (NV.useShipPostMods_Instrument)
					has_postname_mod = (genpercent(mt_name) < NV.probShipPostMod_Instrument) ? true : false;
				if (NV.useShipNumberMods_Instrument)
					has_number_mod = (genpercent(mt_name) < NV.probShipNumberMod_Instrument) ? true : false;
			}
				break;
			case typeShipSatellite:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) < NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) < NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Satellite)
					has_prename_mod = (genpercent(mt_name) < NV.probShipPreMod_Satellite) ? true : false;
				if (NV.useShipPostMods_Satellite)
					has_postname_mod = (genpercent(mt_name) < NV.probShipPostMod_Satellite) ? true : false;
				if (NV.useShipNumberMods_Satellite)
					has_number_mod = (genpercent(mt_name) < NV.probShipNumberMod_Satellite) ? true : false;
			}
				break;
			case typeShipStation:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) < NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) < NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Station)
					has_prename_mod = (genpercent(mt_name) < NV.probShipPreMod_Station) ? true : false;
				if (NV.useShipPostMods_Station)
					has_postname_mod = (genpercent(mt_name) < NV.probShipPostMod_Station) ? true : false;
				if (NV.useShipNumberMods_Station)
					has_number_mod = (genpercent(mt_name) < NV.probShipNumberMod_Station) ? true : false;
			}
				break;
			case typeStar:
			{
				if (NV.useStarPreMods)
					has_prename_mod = (genpercent(mt_name) < NV.probStarPreMod) ? true : false;
				if (NV.useStarPostMods)
					has_postname_mod = (genpercent(mt_name) < NV.probStarPostMod) ? true : false;
				if (NV.useStarNumberMods)
					has_number_mod = (genpercent(mt_name) < NV.probStarNumberMod) ? true : false;
			}				
				break;
			}

			//######################################################################################################
				// PRE NAME MODIFIER

			if (has_shipall_premod)
			{
				int listsize = NV.ShipPreMods_All.size() - 1;
				std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
				finalName = NV.ShipPreMods_All.at(gen_mod_position(mt_name));
				finalName += L" ";
			}

			if (has_prename_mod)
			{
				switch (type)
				{
				case typeAsteroid:
				{
					int listsize = NV.AsteroidPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.AsteroidPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
				break;
				case typeComet:
				{
					int listsize = NV.CometPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.CometPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
				break;
				case typeDwarfMoon:
				{
					int listsize = NV.DwarfMoonPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.DwarfMoonPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}		
					break;
				case typeMoon:
				{
					int listsize = NV.MoonPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.MoonPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}					
					break;
				case typePlanet:
				{
					int listsize = NV.PlanetPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.PlanetPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}				
					break;
				case typeShipColony:
				{
					int listsize = NV.ShipPreMods_Colony.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += NV.ShipPreMods_Colony.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
					break;
				case typeShipInstrument:
				{
					int listsize = NV.ShipPreMods_Instrument.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += NV.ShipPreMods_Instrument.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
					break;
				case typeShipSatellite:
				{
					int listsize = NV.ShipPreMods_Satellite.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += NV.ShipPreMods_Satellite.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
					break;
				case typeShipStation:
				{
					int listsize = NV.ShipPreMods_Station.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += NV.ShipPreMods_Station.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
					break;
				case typeStar:
				{
					int listsize = NV.StarPreMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName = NV.StarPreMods.at(gen_mod_position(mt_name));
					finalName += L" ";
				}
					break;
				}
			}

			//######################################################################################################
				// NAME GENERATION

			if (!NV.useSimpleGenerator) // markov name generator
			{
				std::wstring vowels = L"aeiou";
				std::wstring letters = L"abcdefghijklmnopqrstuvwxyz";
				std::uniform_int_distribution<int> pickvowel{ 0, 4 };
				std::uniform_int_distribution<int> pickletter{ 0, 25 };

				for (int count = 0; count < wordCount; count++)
				{
					int firstPosition = (NV.Markov_RawDataset.size() - 1);
					std::uniform_int_distribution<int> PosGen{ 0, firstPosition };
					std::wstring markovName(NV.Markov_RawDataset.at(PosGen(mt_name)), 0, 3);
					std::uniform_int_distribution<int> lengthgen{ (NV.min_length - 3), (NV.max_length - 3) };
					int _namelength = lengthgen(mt_name);

					for (int i = 0; i < _namelength; i++) // number of characters in the name
					{
						if (genpercent(mt_name) > NV.wordVarience) // generator to add a constant variance to name generation
						{
							bool gram_exists = false;
							std::wstring currentgram(markovName, (markovName.length() - NV.order), NV.order);
							int position;

							// Checks the current ORDER number of letters to see if it exists in the ngram list
							for (int j = 0; j < NV.main_ngrams.ngrams.size(); j++)
							{
								if (NV.main_ngrams.ngrams.at(j) == currentgram)
								{
									position = j;
									gram_exists = true;
									break;
								}
							}

							// if it does exist, use it to determine the next letter
							if (gram_exists)
							{
								int posSize = (NV.main_ngrams.nextCharList.at(position).length() - 1);
								std::uniform_int_distribution<int> nextCharGen{ 0, posSize };

								markovName += NV.main_ngrams.nextCharList.at(position).at(nextCharGen(mt_name));
							}

							// if it does not exist, decide to add another letter and how to add it
							else
							{
								if (genpercent(mt_name) < (100 - (markovName.length() * 5))) // adds another letter based on the current size of the name
								{
									if (genpercent(mt_name) < 50) // adds a random letter from list of vowels and list of all letters
									{
										if (genpercent(mt_name) < 50)
											markovName += vowels.at(pickvowel(mt_name));
										else
											markovName += letters.at(pickletter(mt_name));
									}
									else // adds a letter based on the list of possible letters when looking at only the last two letters (instead of the last ORDER # of letters)
									{
										std::wstring currenttwogram_list(markovName, (markovName.length() - 2), 2);

										for (int j = 0; j < NV.twogram_list.ngrams.size(); j++)
										{
											if (NV.twogram_list.ngrams.at(j) == currenttwogram_list)
											{
												position = j;
												gram_exists = true;
												break;
											}
										}
										if (gram_exists)
										{
											int posSize = (NV.twogram_list.nextCharList.at(position).length() - 1);
											std::uniform_int_distribution<int> nextCharGen{ 0, posSize };

											markovName += NV.twogram_list.nextCharList.at(position).at(nextCharGen(mt_name));
										}
									}
								}
							}
						}
						else // constant varience could add a random letter instead of using the markov chain
						{
							if (genpercent(mt_name) < 50)
								markovName += vowels.at(pickvowel(mt_name));
							else
								markovName += letters.at(pickletter(mt_name));
						}
					}

					markovName.at(0) = toupper(markovName.at(0));
					finalName += markovName;
					if (count < (wordCount - 1)) //Adds a space before the next word if there is another word
						finalName += L" ";
				}
			}
			else // old name generator
			{
				int plsize = NV.PrefixList.size() - 1, slsize = NV.SuffixList.size() - 1;
				std::uniform_int_distribution<int> gen_prefix_position{ 0, plsize };
				std::uniform_int_distribution<int> gen_suffix_position{ 0, slsize };

				for (int count = 0; count < wordCount; count++)
				{
					finalName += NV.PrefixList.at(gen_prefix_position(mt_name)); //Starts word off with a prefix

					syllPercent = genpercent(mt_name); //Determines number of syllables
					if (syllPercent < 10)
						syllCount = 1;
					else if (syllPercent < 80)
						syllCount = 2;
					else syllCount = 3;

					for (int count2 = 0; count2 < syllCount; count2++) //Adds syllables until the word is complete
						finalName += NV.SuffixList.at(gen_suffix_position(mt_name));

					if (count < (wordCount - 1)) //Adds a space before the next word if there is another word
						finalName += L" ";
				}
			}

			//######################################################################################################
				// POST NAME MODIFIER

			if (has_postname_mod)
			{
				switch (type)
				{
				case typeAsteroid:
				{
					int listsize = NV.AsteroidPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.AsteroidPostMods.at(gen_mod_position(mt_name));
				}
					break;
				case typeComet:
				{
					int listsize = NV.CometPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.CometPostMods.at(gen_mod_position(mt_name));
				}
					break;
				case typeDwarfMoon:
				{
					int listsize = NV.DwarfMoonPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.DwarfMoonPostMods.at(gen_mod_position(mt_name));
				}
					break;
				case typeMoon:
				{
					int listsize = NV.MoonPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.MoonPostMods.at(gen_mod_position(mt_name));
				}
					break;
				case typePlanet:
				{
					int listsize = NV.PlanetPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.PlanetPostMods.at(gen_mod_position(mt_name));
				}
					break;
				case typeShipColony:
				{
					int listsize = NV.ShipPostMods_Colony.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.ShipPostMods_Colony.at(gen_mod_position(mt_name));
					
				}
					break;
				case typeShipInstrument:
				{
					int listsize = NV.ShipPostMods_Instrument.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.ShipPostMods_Instrument.at(gen_mod_position(mt_name));
				}
					break;
				case typeShipSatellite:
				{
					int listsize = NV.ShipPostMods_Satellite.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.ShipPostMods_Satellite.at(gen_mod_position(mt_name));
				}
					break;
				case typeShipStation:
				{
					int listsize = NV.ShipPostMods_Station.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.ShipPostMods_Station.at(gen_mod_position(mt_name));
				}
					break;
				case typeStar:
				{
					int listsize = NV.StarPostMods.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName += NV.StarPostMods.at(gen_mod_position(mt_name));
				}
					break;
				}

				if (has_shipall_postmod)
				{
					int listsize = NV.ShipPostMods_All.size() - 1;
					std::uniform_int_distribution<int> gen_mod_position{ 0, listsize };
					finalName += L" ";
					finalName = NV.ShipPostMods_All.at(gen_mod_position(mt_name));
				}
			}

			//######################################################################################################
				// NUMBER MODIFIER

			if (has_number_mod)
			{
				if (genpercent(mt_name) < 50)
					finalName += L" " + GenNumberModifier();
				else
					finalName = GenNumberModifier() + L" " + finalName;
			}

			//######################################################################################################
				// USED NAME CHECKER

			for (int count = 0; count < NV.usedNames.size(); count++)
			{
				if (finalName == NV.usedNames.at(count))
				{
					testName = true;
					//SendDebugMessage((L"Name Warning: " + NV.usedNames.at(count) + L" used twice!"), debug_WARNING);
					count = NV.usedNames.size();
				}
			}
		} while (testName);

		//######################################################################################################
			// FINAL NAME OUTPUT

		NV.usedNames.push_back(finalName);
		return finalName;
	}
	std::wstring GenNumberModifier()
	{
		std::wstring FinalNumber = L"";
		std::wstring NumbersAlpha[10] = { L"One", L"Two", L"Three", L"Four", L"Five", L"Six", L"Seven", L"Eight", L"Nine", L"Ten" };
		std::wstring NumbersRoman[10] = { L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X" };
		std::wstring NumbersDigit = L"0123456789";
		std::wstring Letters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
		std::wstring Symbols = L"\\- ";

		int numType = genpercent(mt_name);

		// 50% chance to use digits
		if (numType < 50)
		{
			do
			{
				std::uniform_int_distribution<int> gennumber{ 0, 9 };
				FinalNumber += NumbersDigit.at(gennumber(mt_name));
			} while (genpercent(mt_name) < 30);
		}
		// 30% chance to use roman numerals
		else if (numType < 80)
		{
			std::uniform_int_distribution<int> gennumber{ 0, 9 };
			FinalNumber += NumbersRoman[gennumber(mt_name)];
		}
		// 20% chance to use words
		else
		{
			std::uniform_int_distribution<int> gennumber{ 0, 9 };
			FinalNumber += NumbersAlpha[gennumber(mt_name)];
		}


		// This adds special tags and stuff
		if (genpercent(mt_name) < 40)
		{
			do
			{
				// < 50 put the tag on the end, > 50 puts the tag on the front
				if (genpercent(mt_name) < 50)
				{
					std::uniform_int_distribution<int> gensymbol{ 0, 2 };
					FinalNumber += Symbols.at(gensymbol(mt_name));

					do
					{
						std::uniform_int_distribution<int> genletter{ 0, 25 };
						FinalNumber += Letters.at(genletter(mt_name));
					} while (genpercent(mt_name) < 30);
				}
				else
				{
					std::wstring tempName;

					do
					{
						std::uniform_int_distribution<int> genletter{ 0, 25 };
						tempName += Letters.at(genletter(mt_name));
					} while (genpercent(mt_name) < 30);

					std::uniform_int_distribution<int> gensymbol{ 0, 2 };
					tempName += Symbols.at(gensymbol(mt_name)) + FinalNumber;
					FinalNumber = tempName;
				}
			} while (genpercent(mt_name) < 5);
		}

		return FinalNumber;
	}

	template <typename vect, typename type> void SortVector(vect& vector, int low, int high)
	{
		if (low < high)
		{
			int part = Partition<vect, type>(vector, low, high);
			SortVector<vect, type>(vector, low, part - 1);
			SortVector<vect, type>(vector, part + 1, high);
		}
	}
	template <typename vect, typename type> int Partition(vect& vector, int low, int high)
	{
		double pivot = vector.at(high).semimajorAxis;
		int i = (low - 1);

		for (int j = low; j <= high - 1; j++)
		{
			if (vector.at(j).semimajorAxis <= pivot)
			{
				i++;
				type temp = vector.at(i);
				vector.at(i) = vector.at(j);
				vector.at(j) = temp;
			}
		}

		type temp = vector.at(i + 1);
		vector.at(i + 1) = vector.at(high);
		vector.at(high) = temp;
		return (i + 1);
	}

	// Generation starts here:
	void BeginGenerate(HWND hWnd)
	{
		if (CheckSeed(CONFIG.seed))
			return;

		if (CONFIG.seed == 0)
		{
			std::mt19937 mtseed;
			mtseed.seed(time(0));
			std::uniform_int_distribution<long long> genseed{ LLONG_MIN, LLONG_MAX };
			CONFIG.seed = genseed(mtseed);
		}

		mt_star.seed(CONFIG.seed + 1);
		mt_planet.seed(CONFIG.seed + 2);
		mt_moon.seed(CONFIG.seed + 3);
		mt_ship.seed(CONFIG.seed + 4);
		mt_name.seed(CONFIG.seed + 5);

		/*###############################################################################
				OUTER LOOP
		###############################################################################*/
		for (int run = 0; run < CONFIG.numberOfRuns; run++)
		{
			SEStar currentStar;
			GenerateStar(currentStar);

			std::wstring starFileName = CONFIG.starOutputFolder;	//Creates the star file
			//starFileName += currentStar.name;
			//starFileName += L" Star.sc";
			starFileName += L"Test Star.sc";
			std::ofstream starFile(starFileName.c_str());
			if (!starFile)
			{
				MessageBox(hWnd, L"Problem with star output folder!", L"Error", MB_ICONERROR);
				return;
			}
			PrintStar(currentStar, starFile);

			std::wstring planetFileName = CONFIG.planetOutputFolder;	//Creates the planet file
			//planetFileName += currentStar.name;
			//planetFileName += L" System.sc";
			planetFileName += L"Test System.sc";
			std::ofstream planetFile(planetFileName.c_str());
			if (!planetFile)
			{
				MessageBox(hWnd, L"Problem with planet output folder!", L"Error", MB_ICONERROR);
				return;
			}

		#pragma region Debug Setup

			SystemDebugCounter = 0;
			DebugFileP = &planetFile;
			SystemDebug.type = L"Planet";
			SystemDebug.class_ = L"Terra";
			SystemDebug.name = L"System Debug Holder";
			SystemDebug.parentBody = &currentStar;
			SystemDebug.semimajorAxis = AU_to_km(currentStar.outerLimit * 6);
			SystemDebug.mass = 1;
			SystemDebug.radius = 5000;
			SystemDebug.eccentricity = SystemDebug.inclination = SystemDebug.ascendingNode = SystemDebug.argofPericenter = SystemDebug.meanAnomaly = SystemDebug.obliquity = 0;
			if (CONFIG.debug)
				PrintDebug(SystemDebug, color_NORMAL);
		#pragma endregion

			std::uniform_int_distribution<int> genplanetnum{ (int)ceil(((CONFIG.minPlanetNumber / 100) * currentStar.maxPlanetNumber)), currentStar.maxPlanetNumber };
			int planetNumber = genplanetnum(mt_planet);
			std::vector<SEPlanet> planetList;

			if (currentStar.frostLine > currentStar.outerLimit)
				SendDebugMessage(L"Star's frost limit is larger than the outer limit.", debug_WARNING);
			else if (currentStar.frostLine < currentStar.innerLimit)
				SendDebugMessage(L"Star's frost limit is smaller than the inner limit.", debug_WARNING);
			SendDebugMessage( (std::to_wstring(planetNumber) + L" semimajors used out of " + std::to_wstring(currentStar.maxPlanetNumber) + L" possible"), debug_INFO);

			/*###############################################################################
					PLANET GENERATOR
			###############################################################################*/

			for (int planetCounter = 0; planetCounter < planetNumber; planetCounter++)
			{
				SEPlanet currentPlanet;
				GeneratePlanet(currentStar, currentPlanet);
				planetList.push_back(currentPlanet);

				// Debug moon orbits			
				if (CONFIG.debug)
				{
					// These are logged as errors, but that is only because they are annoying to look at in game

					DebugMessage(L"Hill Sphere Outer Limit", debug_PLANET_LIMITS, currentPlanet, currentPlanet.hillSphereOuterLimit);
					
					if (currentPlanet.class_ == L"Jupiter")
						DebugMessage(L"Hill Sphere Inner Limit", debug_PLANET_LIMITS, currentPlanet, ((2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius));
					else
						DebugMessage(L"Hill Sphere Inner Limit", debug_PLANET_LIMITS, currentPlanet, ((2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius));

					if (currentPlanet.class_ == L"Jupiter")
					{
						DebugMessage(L"Hill Sphere Fifteenth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 15));
						DebugMessage(L"Hill Sphere Tenth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 10));
						DebugMessage(L"Hill Sphere Fifth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 5));
					}
					else if (currentPlanet.class_ == L"Neptune")
					{
						DebugMessage(L"Hill Sphere Fiftheenth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 15));
						DebugMessage(L"Hill Sphere Tenth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 10));
						DebugMessage(L"Hill Sphere Fifth Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 5));
					}
					else
					{
						DebugMessage(L"Hill Sphere Third Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 3));
						DebugMessage(L"Hill Sphere Second Limit", debug_PLANET_LIMITS, currentPlanet, ((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 2));
					}
				}
			}
			while (genpercent(mt_planet) < CONFIG.dwarfPlanetChance)
			{
				SEPlanet currentDwarf;
				GenerateDwarfPlanet(currentStar, currentDwarf);
				planetList.push_back(currentDwarf);
				CONFIG.dwarfPlanetChance--;
			}
			SortVector <std::vector<SEPlanet>, SEPlanet> (planetList, 0, planetList.size() - 1);

			// Companion Orbit function for planets
			for (int i = 1; i < planetList.size(); i++)
			{
				if (planetList.at(i).hasCompanionOrbit)
				{
					if (!planetList.at(i - 1).hasCompanionOrbit)
					{
						planetList.at(i).semimajorAxis = planetList.at(i - 1).semimajorAxis;
						planetList.at(i).period = planetList.at(i - 1).period;
						planetList.at(i).inclination = planetList.at(i - 1).inclination;
						planetList.at(i).eccentricity = planetList.at(i - 1).eccentricity;
						planetList.at(i).ascendingNode = planetList.at(i - 1).ascendingNode;
						planetList.at(i).argofPericenter = planetList.at(i - 1).argofPericenter;
						planetList.at(i).meanAnomaly = planetList.at(i - 1).meanAnomaly + 180;
					}
					else
						planetList.at(i).hasCompanionOrbit = false;		
				}
			}

			// force life function
			bool lifeTest = false;
			if (CONFIG.forceLife)
			{
				for (int i = 0; i < planetList.size(); i++)
				{
					if (planetList.at(i).life_organic.haslife || planetList.at(i).life_exotic.haslife)
					{
						lifeTest = true;
						break;
					}
				}

				if (planetNumber > 0)
				{
					if (!lifeTest)
					{
						int position;
						do
						{
							int vectSize = planetList.size() - 1;
							std::uniform_int_distribution<int> genplanetposition{ 0, vectSize };
							position = genplanetposition(mt_planet);
							lifeTest = true;
						} while (planetList.at(position).type == L"DwarfPlanet");
						planetList.at(position).life_exotic.haslife = true;
						ExoticGenerateLife(planetList.at(position));
					}
				}
				else
				{
					SendDebugMessage(L"Force life is set to true, but there are no valid planets in the system.", debug_ERROR);
				}
				
			}

			/*###############################################################################
					MOON GENERATOR PER PLANET
			###############################################################################*/
			for (int currentPlanet = 0; currentPlanet < planetList.size(); currentPlanet++)
			{
				std::vector<SEPlanet> majorMoon, minorMoon;
				// weighted moons uses new system, else old system
				if (CONFIG.weightedMoons)
				{
					planetList.at(currentPlanet).numberOfMajorMoons = 0;
					planetList.at(currentPlanet).numberOfMinorMoons = 0;

					while (genpercent(mt_moon) < planetList.at(currentPlanet).majorMoonPercent)
					{
						SEPlanet currentMoon;
						if (GenerateMajorMoon(currentStar, planetList.at(currentPlanet), currentMoon, planetList.at(currentPlanet).numberOfMajorMoons))
						{
							majorMoon.push_back(currentMoon);
							planetList.at(currentPlanet).numberOfMajorMoons++;	
						}
						else
							SendDebugMessage(  (L"Moon failed to generate around " + planetList.at(currentPlanet).name + L"!"), debug_ERROR);

						std::uniform_int_distribution<int> gennumber{ 1, planetList.at(currentPlanet).numberOfMajorMoons };
						planetList.at(currentPlanet).majorMoonPercent -= gennumber(mt_moon);
					}
				
					while (genpercent(mt_moon) < planetList.at(currentPlanet).minorMoonPercent)
					{
						SEPlanet currentMoon;
						GenerateMinorMoon(planetList.at(currentPlanet), currentMoon, planetList.at(currentPlanet).numberOfMinorMoons);
						minorMoon.push_back(currentMoon);
						planetList.at(currentPlanet).numberOfMinorMoons++;
						std::uniform_int_distribution<int> gennumber { 1, planetList.at(currentPlanet).numberOfMinorMoons };
						planetList.at(currentPlanet).minorMoonPercent -= gennumber(mt_moon);
					}
				}
				else
				{
					int Moon_Count = 0;
					while (Moon_Count < planetList.at(currentPlanet).numberOfMajorMoons)
					{
						SEPlanet currentMoon;
						GenerateMajorMoon(currentStar, planetList.at(currentPlanet), currentMoon, Moon_Count);
						majorMoon.push_back(currentMoon);
						Moon_Count++;
					}
					Moon_Count = 0;

					while (Moon_Count < planetList.at(currentPlanet).numberOfMinorMoons)
					{
						SEPlanet currentMoon;
						GenerateMinorMoon(planetList.at(currentPlanet), currentMoon, Moon_Count);
						minorMoon.push_back(currentMoon);
						Moon_Count++;
					}
				}

				SortVector<std::vector<SEPlanet>, SEPlanet>(majorMoon, 0, majorMoon.size() - 1);
				SortVector<std::vector<SEPlanet>, SEPlanet>(minorMoon, 0, minorMoon.size() - 1);

				// Companion Orbit function for minor moons
				if (majorMoon.size() > 0)
				{
					majorMoon.at(0).hasCompanionOrbit = false;
					for (int i = 1; i < majorMoon.size(); i++)
					{
						if (majorMoon.at(i).hasCompanionOrbit)
						{
							if (!majorMoon.at(i - 1).hasCompanionOrbit)
							{
								majorMoon.at(i).semimajorAxis = majorMoon.at(i - 1).semimajorAxis;
								majorMoon.at(i).period = majorMoon.at(i - 1).period;
								majorMoon.at(i).inclination = majorMoon.at(i - 1).inclination;
								majorMoon.at(i).eccentricity = majorMoon.at(i - 1).eccentricity;
								majorMoon.at(i).ascendingNode = majorMoon.at(i - 1).ascendingNode;
								majorMoon.at(i).argofPericenter = majorMoon.at(i - 1).argofPericenter;
								majorMoon.at(i).meanAnomaly = majorMoon.at(i - 1).meanAnomaly + 180;
							}
							else
								majorMoon.at(i).hasCompanionOrbit = false;
						}
					}
				}
				// Companion Orbit function for major moons
				if (minorMoon.size() > 0)
				{
					minorMoon.at(0).hasCompanionOrbit = false;
					for (int i = 1; i < minorMoon.size(); i++)
					{
						if (minorMoon.at(i).hasCompanionOrbit)
						{
							if (!minorMoon.at(i - 1).hasCompanionOrbit)
							{
								minorMoon.at(i).semimajorAxis = minorMoon.at(i - 1).semimajorAxis;
								minorMoon.at(i).period = minorMoon.at(i - 1).period;
								minorMoon.at(i).inclination = minorMoon.at(i - 1).inclination;
								minorMoon.at(i).eccentricity = minorMoon.at(i - 1).eccentricity;
								minorMoon.at(i).ascendingNode = minorMoon.at(i - 1).ascendingNode;
								minorMoon.at(i).argofPericenter = minorMoon.at(i - 1).argofPericenter;
								minorMoon.at(i).meanAnomaly = minorMoon.at(i - 1).meanAnomaly + 180;
							}
							else
								minorMoon.at(i).hasCompanionOrbit = false;
						}
					}
				}
				
				/*###############################################################################
					PRINTING
				###############################################################################*/
				if (planetList.at(currentPlanet).semimajorAxis > currentStar.innerLimit)
				{
					PrintPlanet(planetList.at(currentPlanet), planetFile);
					for (int currentMoon = 0; currentMoon < planetList.at(currentPlanet).numberOfMajorMoons; currentMoon++)
						PrintPlanet(majorMoon.at(currentMoon), planetFile);
					for (int currentMoon = 0; currentMoon < planetList.at(currentPlanet).numberOfMinorMoons; currentMoon++)
						PrintPlanet(minorMoon.at(currentMoon), planetFile);
				}

				/*###############################################################################
					EXOTIC STUFF
				###############################################################################*/

				// Generates Debris Ring
				if (planetList.at(currentPlanet).debrisCount > 0)
				{
					SendDebugMessage(  (L"Generating debris ring around " + planetList.at(currentPlanet).name + L" with " + std::to_wstring(planetList.at(currentPlanet).debrisCount) + L" objects"), debug_INFO);

					// Generates data for debris to spawn around
					double SMCenterSpread, SMCenterPoint, inclinationCenter = 0.0, inclinationSD = 0.0, DebrisSpread = 0.0;

					std::uniform_int_distribution<int> genpreset{ 1, 6 };
					int preset = genpreset(mt_planet);

					//##############################
							// CONTROL PANEL
							// preset =	1	;
					//##############################

					switch (preset)
					{
					case 1: // Classic, thin ring around the equator, moderate distance from planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.8, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					case 2: // Classic, thin ring around the equator, close to planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.2, 1.5 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					case 3: // Classic, thin ring around the equator, far from planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 5, 6 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					case 4: // Wide ring around the equator
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.3, 2 };
						std::uniform_real_distribution<> gensmspread{ 1.5, 5 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					case 5: // Thin ring around the equator, higher inclination
					{
						std::uniform_real_distribution<> gensd{ 2, 7 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.2, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					case 6: // Wide ring around the equator, higher inclination
					{
						std::uniform_real_distribution<> gensd{ 2, 7 };
						std::uniform_real_distribution<> genspread{ 1.3, 2 };
						std::uniform_real_distribution<> gensmspread{ 1.5, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
						break;
					}
					}

					SMCenterPoint = planetList.at(currentPlanet).radius * SMCenterSpread;

					// generates and prints the debris
					SEPlanet debrisMoon;
					debrisMoon.type = L"DwarfMoon";
					debrisMoon.class_ = L"Asteroid";
					debrisMoon.parentBody = &planetList.at(currentPlanet);

					if (planetList.at(currentPlanet).semimajorAxis > currentStar.innerLimit)
					{
						for (int count = 0; count < planetList.at(currentPlanet).debrisCount; count++)
						{
							ExoticDebrisRing(planetList.at(currentPlanet), debrisMoon, inclinationCenter, inclinationSD, DebrisSpread, SMCenterPoint);
							debrisMoon.name += std::to_wstring(count + 1);
							PrintPlanet(debrisMoon, planetFile);
						}
					}
				}			

				// end of every planet generated
			}
			
			// This says if the system has life AND ships need it, we run the following code,
			// otherwise run it anyway
			if ((CONFIG.shipsNeedLife && lifeTest) || !CONFIG.shipsNeedLife)
			{
				// this disables ships if their list is empty
				bool enable_colony = true, enable_instrument = true, enable_satellite = true, enable_station = true;
				if (CONFIG.shipList_Colony.size() == 0)
				{
					enable_colony = false;
					SendDebugMessage(L"Colony ship list was empty! No colony ships can be generated!", debug_WARNING);
				}				
				if (CONFIG.shipList_Instrument.size() == 0)
				{
					enable_instrument = false;
					SendDebugMessage(L"Instrument ship list was empty! No instrument ships can be generated!", debug_WARNING);
				}			
				if (CONFIG.shipList_Satellite.size() == 0)
				{
					enable_satellite = false;
					SendDebugMessage(L"Satellite ship list was empty! No satellites can be generated!", debug_WARNING);
				}			
				if (CONFIG.shipList_Station.size() == 0)
				{
					enable_station = false;
					SendDebugMessage(L"Station ship list was empty! No stations can be generated!", debug_WARNING);
				}					

				while (genpercent(mt_ship) < CONFIG.exotic_ShipChance)
				{
					int size = planetList.size(), parent;
					SEShip ship;
					ship.type = L"Spacecraft";

					std::uniform_int_distribution<int> genship{ 0, size };
					parent = genship(mt_ship);

					// the star is the parent
					if (parent == planetList.size())
					{
						ship.parentBody = &currentStar;

						// final check before the ship type is generated
						if (enable_colony || enable_instrument || enable_station)
						{
							double min_dist = 0, max_dist = 0;
							std::discrete_distribution<int> genshiptype{ 0, 0, 0, 0, 0, 0, 0, (double)enable_colony, (double)enable_instrument, 0, (double)enable_station };
							Object_Type shipType = static_cast<Object_Type>(genshiptype(mt_ship));
							ship.name = GenName(shipType);

							// selects a model and creates the min & max orbit distances
							switch (shipType)
							{
							case typeShipColony:
							{
								int listSize = CONFIG.shipList_Colony.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Colony.at(genmodel(mt_ship));

								min_dist = AU_to_km(currentStar.habitZoneInnerLimit / 2);
								max_dist = AU_to_km(currentStar.habitZoneOuterLimit * 2);
								break;
							}
							case typeShipInstrument:
							{
								int listSize = CONFIG.shipList_Instrument.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Instrument.at(genmodel(mt_ship));

								min_dist = currentStar.radius + 10000;
								max_dist = currentStar.radius + 149598000;
								break;
							}
							case typeShipStation:
							{
								int listSize = CONFIG.shipList_Station.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Station.at(genmodel(mt_ship));

								min_dist = AU_to_km(currentStar.habitZoneInnerLimit / 2);
								max_dist = AU_to_km(currentStar.outerLimit);
								break;
							}
							}

							GenerateShip(ship, min_dist, max_dist, CONFIG.avgEccentricity, CONFIG.SDEccentricity, CONFIG.avgInclination, CONFIG.SDInclination);
							PrintShip(ship, planetFile);
						}
						else
							SendDebugMessage(L"Tried to generae a ship around the star, but there were no available ship types!", debug_WARNING);
					}
					// a planet is the parent
					else
					{
						// weeds out some ship types based on the planet type
						ship.parentBody = &planetList.at(parent);
						if (planetList.at(parent).class_ == L"Jupiter" || planetList.at(parent).class_ == L"Neptune")
						{
							enable_colony = false;
							enable_satellite = false;
						}
						else if (!planetList.at(parent).life_exotic.haslife && !planetList.at(parent).life_organic.haslife)
						{
							enable_satellite = false;
						}


						// final check before the ship type is generated
						if (enable_colony || enable_instrument || enable_satellite || enable_station)
						{
							// seelctes the ship type
							double min_dist = 0, max_dist = 0;
							std::discrete_distribution<int> genshiptype{ 0, 0, 0, 0, 0, 0, 0, (double)enable_colony, (double)enable_instrument, (double)enable_satellite, (double)enable_station };
							Object_Type shipType = static_cast<Object_Type>(genshiptype(mt_ship));
							ship.name = GenName(shipType);

							// selects a model and creates the min & max orbit distances
							switch (shipType)
							{
							case typeShipColony:
							{
								int listSize = CONFIG.shipList_Colony.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Colony.at(genmodel(mt_ship));

								min_dist = planetList.at(parent).radius + 300;
								max_dist = planetList.at(parent).radius + 1000;
								break;
							}
							case typeShipInstrument:
							{
								int listSize = CONFIG.shipList_Instrument.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Instrument.at(genmodel(mt_ship));

								min_dist = planetList.at(parent).radius + 300;
								max_dist = planetList.at(parent).radius + 10000;
								break;
							}
							case typeShipSatellite:
							{
								int listSize = CONFIG.shipList_Satellite.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Satellite.at(genmodel(mt_ship));

								min_dist = planetList.at(parent).radius + 2000;
								max_dist = planetList.at(parent).radius + 20000;
								break;
							}
							case typeShipStation:
							{
								int listSize = CONFIG.shipList_Station.size();
								std::uniform_int_distribution<int> genmodel{ 0, listSize - 1 };
								ship.model = CONFIG.shipList_Station.at(genmodel(mt_ship));

								min_dist = planetList.at(parent).radius + 300;
								max_dist = planetList.at(parent).radius + 1000;
								break;
							}
							}

							GenerateShip(ship, min_dist, max_dist, 0.0, 0.001, 0.0, 90.0);
							PrintShip(ship, planetFile);
						}
						else
							SendDebugMessage(L"Tried to generate a ship around a planet, but there were no available ship types!", debug_WARNING);
					}
					CONFIG.exotic_ShipChance--;
				}
			}
			else
				SendDebugMessage(L"No ships can spawn.", debug_INFO);

			// Asteroid Belt Generator
			if (CONFIG.generateAsteroidBelt)
			{
				SEPlanet asteroid;
				asteroid.type = asteroid.class_ = L"Asteroid";
				asteroid.parentBody = &currentStar;
				bool usedInner = false, usedOuter = false;
				int totalAsteroids = 0;

				std::uniform_int_distribution<> genruns{ 1, CONFIG.maxAsteroidBelts };
				int numberofBelts = genruns(mt_star);

				for (int currentBelt = 0; currentBelt < numberofBelts; currentBelt++)
				{
					/*--------------------------------------------------------------#
					|																|
					|	Basically what this does:									|
					|																|
					|	First we chose a random Semimajor from the leftover list	|
					|	of points IF there are still points left. Otherwise,		|
					|	our asteroid belt will be before the inner limit or			|
					|	beyond the outer limit.										|
					|																|
					|	Next, we detmine where that semimajor was in the original	|
					|	list. The points before and after it are our boundaries		|
					|	for the asteroid belt. If it was the first or last point	|
					|	in the list, we use the inner/outer limits as our			|
					|	boundaries.													|
					|																|
					#--------------------------------------------------------------*/

					double minSemimajor = -1, maxSemimajor = -1, avgSemimajor, sdSemimajor;
					std::uniform_int_distribution<> gennum{ CONFIG.minAsteroidCount, CONFIG.maxAsteroidCount };
					int asteroidCount = gennum(mt_star);

					if (currentStar.semimajorList.size() > 0 && genpercent(mt_star) < 33)
					{
						int pos, listSize = currentStar.semimajorList.size() - 1;
						std::uniform_int_distribution<> genpos{ 0, listSize };
						pos = genpos(mt_star);
						avgSemimajor = currentStar.semimajorList.at(pos);
						currentStar.semimajorList.erase(currentStar.semimajorList.begin() + pos);

						for (int i = 1; i < currentStar.semimajorStaticList.size() - 1; i++)
						{
							if (currentStar.semimajorStaticList.at(i) == avgSemimajor)
							{
								minSemimajor = currentStar.semimajorStaticList.at(i - 1);
								maxSemimajor = currentStar.semimajorStaticList.at(i + 1);
								i = currentStar.semimajorStaticList.size();
							}
						}
						if (minSemimajor < 0)
						{
							if (((avgSemimajor - currentStar.innerLimit) / currentStar.totalDist) < 0.5)
							{
								minSemimajor = currentStar.innerLimit;
								maxSemimajor = currentStar.semimajorStaticList.at(1);
							}
							else
							{
								minSemimajor = currentStar.semimajorStaticList.at(currentStar.semimajorStaticList.size() - 2);
								maxSemimajor = currentStar.outerLimit;
							}
						}

						sdSemimajor = ((maxSemimajor - minSemimajor) / 9);
					}
					else
					{
						// 50% for asteroid belt before inner limit, 50% for after outer limit
						if (!usedInner && genpercent(mt_star) < 50)
						{
							usedInner = true;

							if (AU_to_km(currentStar.innerLimit / 5) < radsol_to_km(currentStar.radius))
							{
								SendDebugMessage(L"Inner limit over 5 was inside the star's radius. No asteroid belt created.", debug_ERROR);
								goto NoBelt;
							}
								
							avgSemimajor = (currentStar.innerLimit / 2.5);
							minSemimajor = (currentStar.innerLimit / 5);
							maxSemimajor = currentStar.innerLimit;
							sdSemimajor = ((maxSemimajor - minSemimajor) / 9);
						}
						else if(!usedOuter)
						{
							avgSemimajor = (currentStar.outerLimit * 2);
							minSemimajor = currentStar.outerLimit;
							maxSemimajor = (currentStar.outerLimit * 5);
							sdSemimajor = ((maxSemimajor - minSemimajor) / 9);
							usedOuter = true;
						}
						else
						{
							SendDebugMessage(L"Inner limit and outer limit asteroid belts already created. Skipping asteroid belt...", debug_WARNING);
							goto NoBelt;
						}	
					}

					for (int currentAsteroid = 0; currentAsteroid < asteroidCount; currentAsteroid++)
					{
						GenerateAsteroid(currentStar, asteroid, minSemimajor, maxSemimajor, avgSemimajor, sdSemimajor);
						if (!NV.nameAsteroids)
							asteroid.name += std::to_wstring(++totalAsteroids);
						PrintPlanet(asteroid, planetFile);
					}
				NoBelt:;
				}	
			}

			// comet generator
			if (CONFIG.generateComets)
			{
				SEPlanet comet;
				comet.type = L"Comet";
				comet.class_ = L"Asteroid";
				comet.parentBody = &currentStar;

				std::uniform_int_distribution<> gennum{ CONFIG.minCometCount, CONFIG.maxCometCount };
				int cometCount = gennum(mt_star);

				for (int currentComet = 0; currentComet < cometCount; currentComet++)
				{
					GenerateComet(currentStar, comet);
					if (!NV.nameComets)
						comet.name += std::to_wstring(currentComet + 1);
					PrintPlanet(comet, planetFile);
				}
			}

			// Debug star Orbits
			if (CONFIG.debug)
			{
				DebugMessage(L"Inner Limit", debug_STAR_LIMITS, currentStar, AU_to_km(currentStar.innerLimit));
				DebugMessage(L"Outer Limit", debug_STAR_LIMITS, currentStar, AU_to_km(currentStar.outerLimit));
				DebugMessage(L"Frost Limit", debug_STAR_LIMITS, currentStar, AU_to_km(currentStar.frostLine));
				DebugMessage(L"HZ Inner Limit", debug_STAR_LIMITS, currentStar, AU_to_km(currentStar.habitZoneInnerLimit));
				DebugMessage(L"HZ Outer Limit", debug_STAR_LIMITS, currentStar, AU_to_km(currentStar.habitZoneOuterLimit));
			}

			//end of every star generated
			currentStar.maxPlanetNumber = 0;
			currentStar.semimajorList.clear();
			starFile.close();
			planetFile.close();
			DebugFileP = NULL;
		}
		return;
	}

	void PrintStar(SEStar& star, std::ofstream& file)
	{
		file << "Star\t\t\t\"" << wstr_to_str(star.name) << "\"\n{"
			<< "\n\tRA\t\t\t" << star.RA[0] << " " << star.RA[1] << " " << star.RA[2]
			<< "\n\tDec\t\t\t" << star.DEC[0] << " " << star.DEC[1] << " " << star.DEC[2]
			<< "\n\tDist\t\t" << star.distance
			<< "\n\tClass\t\t\"" << wstr_to_str(star.class_) << "\""
			<< "\n\tMassSol\t\t" << star.mass
			<< "\n\tRadSol\t\t" << star.radius
			<< "\n\tLum\t\t\t" << star.luminosity
			<< "\n\tTeff\t\t" << star.temperatureK
			<< "\n}";

		file << "\n\n\t" << wstr_to_str(star.name) << " has an inner limit around " << star.innerLimit << " AU,"
			<< " an outer limit around " << star.outerLimit << " AU,\n"
			<< "and a frost line around " << star.frostLine << " AU. The habitable zone is between " << star.habitZoneInnerLimit << " AU\n"
			<< "and " << star.habitZoneOuterLimit << " AU.";

		file << "\n\n\tSeed: " << CONFIG.seed;
	}
	void PrintPlanet(SEPlanet& planet, std::ofstream& file)
	{
		if (planet.type == L"DwarfPlanet")
			file << wstr_to_str(planet.type) << "\t\t\t\t\t\"" << wstr_to_str(planet.name) << "\"\n{";
		else
			file << wstr_to_str(planet.type) << "\t\t\t\t\t\t\"" << wstr_to_str(planet.name) << "\"\n{";
		file << "\n\tParentBody\t\t\t\t\"" << wstr_to_str(planet.parentBody->name) << "\""
			<< "\n\tClass\t\t\t\t\t\"" << wstr_to_str(planet.class_) << "\"\n";
		if (planet.class_ != L"Asteroid")
			file << "\n\tMass\t\t\t\t\t" << planet.mass << "\n";
		file << "\tRadius\t\t\t\t\t" << planet.radius << "\n"
			<< "\tObliquity\t\t\t\t" << planet.obliquity << "\n";

		if (CONFIG.generateComposition && planet.class_ != L"Asteroid")
		{
			file << "\n\tInterior\n\t{"
				<< "\n\t\tComposition\n\t\t{"
				<< "\n\t\t\tHydrogen\t" << planet.interior.hydrogen
				<< "\n\t\t\tHelium\t\t" << planet.interior.helium
				<< "\n\t\t\tSilicates\t" << planet.interior.silicates
				<< "\n\t\t\tCarbides\t" << planet.interior.carbides
				<< "\n\t\t\tIces\t\t" << planet.interior.ices
				<< "\n\t\t\tMetals\t\t" << planet.interior.metals
				<< "\n\t\t}"
				<< "\n\t}\n";
		}

		if (planet.life_organic.haslife)
		{
			file << "\n\tLife\n\t{"
				<< "\n\t\tClass\t\"" << wstr_to_str(planet.life_organic._class) << "\""
				<< "\n\t\tType\t\"" << wstr_to_str(planet.life_organic.type) << "\"";
			if (planet.life_organic.panspermia)
				file << "\n\t\tPanspermia\ttrue";
			file << "\n\t}\n";
		}
		if (planet.life_exotic.haslife)
		{
			file << "\n\tLife\n\t{"
				<< "\n\t\tClass\t\"" << wstr_to_str(planet.life_exotic._class) << "\""
				<< "\n\t\tType\t\"" << wstr_to_str(planet.life_exotic.type) << "\"";
			if (planet.life_exotic.panspermia)
				file << "\n\t\tPanspermia\ttrue";
			file << "\n\t}\n";
		}

		file << "\n\tOrbit\n\t{\n\t\t"
			<< "RefPlane\t\t\t\"Equator\"";
		if (planet.type == L"Moon" || planet.type == L"DwarfMoon")
			file << "\n\t\tSemiMajorAxis\t\t" << km_to_AU(planet.semimajorAxis);
		else
			file << "\n\t\tSemiMajorAxis\t\t" << planet.semimajorAxis;
		file << "\n\t\tPeriod\t\t\t\t" << planet.period
			<< "\n\t\tEccentricity\t\t" << planet.eccentricity
			<< "\n\t\tInclination\t\t\t" << planet.inclination
			<< "\n\t\tAscendingNode\t\t" << planet.ascendingNode
			<< "\n\t\tArgOfPericenter\t\t" << planet.argofPericenter
			<< "\n\t\tMeanAnomaly\t\t\t" << planet.meanAnomaly
			<< "\n\t}\n}\n\n";
	}
	void PrintShip(SEShip& ship, std::ofstream& planetFile)
	{
		planetFile << wstr_to_str(ship.type) << "\t\t\t\t\t\t\"" << wstr_to_str(ship.name) << "\"\n{"
			<< "\n\tModel\t\t\"" << wstr_to_str(ship.model) << "\""
			<< "\n\tParentBody\t\t\t\t\"" << wstr_to_str(ship.parentBody->name) << "\""
			<< "\n\tObliquity\t\t\t\t" << ship.obliquity << "\n"
			<< "\n\tOrbit\n\t{"
			<< "\n\t\tRefPlane\t\t\t\"Equator\""
			<< "\n\t\tSemiMajorAxisKm\t\t" << ship.semimajorAxis
			<< "\n\t\tEccentricity\t\t" << ship.eccentricity
			<< "\n\t\tInclination\t\t\t" << ship.inclination
			<< "\n\t\tAscendingNode\t\t" << ship.ascendingNode
			<< "\n\t\tArgOfPericenter\t\t" << ship.argofPericenter
			<< "\n\t\tMeanAnomaly\t\t\t" << ship.meanAnomaly
			<< "\n\t}\n}\n\n";
	}

	void GenerateStar(SEStar& star)
	{
		double tempSol;

		std::uniform_int_distribution<int> genhour{ 0, 23 };
		std::uniform_int_distribution<int> genminute{ 0, 59 };
		std::uniform_real_distribution<> gensec{ 0, 60 };
		std::uniform_int_distribution<int> gendecl{ -90, 90 };
		std::uniform_real_distribution<> gendistance{ CONFIG.minDistance, CONFIG.maxDistance };
		std::normal_distribution<> genSM{ CONFIG.planetSpaceAvg, CONFIG.planetSpaceSD };
		star.name = GenName(typeStar);

		//######################################################################################################
			//	CLASS GENERATION

		enum StarClass { O, B, A, F, G, K, M, WD, Q, X };
		StarClass starclass;
		std::discrete_distribution<int> genclass{ CONFIG.starClassO, CONFIG.starClassB, CONFIG.starClassA, CONFIG.starClassF, CONFIG.starClassG, 
												CONFIG.starClassK, CONFIG.starClassM, CONFIG.starClassWD, CONFIG.starClassQ, CONFIG.starClassX };
		starclass = static_cast<StarClass>(genclass(mt_star));

		switch (starclass)
		{
		case O:
		{
			star.class_ = L"O";

			std::normal_distribution<> genm{ 24, 8 };
			std::normal_distribution<> genr{ 9.9, 3.3 };
			std::uniform_int_distribution<int> gent{ 30000, 55000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 6);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.8);
			star.temperatureK = gent(mt_star);
			break;
		}
		case B:
		{
			star.class_ = L"B";

			std::normal_distribution<> genm{ 9.05, 6.95 };
			std::normal_distribution<> genr{ 4.2, 2.4 };
			std::uniform_int_distribution<int> gent{ 10000, 31000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 1.4);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.4);
			star.temperatureK = gent(mt_star);
			break;
		}
		case A:
		{
			star.class_ = L"A";

			std::normal_distribution<> genm{ 1.75, 0.35 };
			std::normal_distribution<> genr{ 1.6, 0.2 };
			std::uniform_int_distribution<int> gent{ 7500, 11000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 1.04);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.15);
			star.temperatureK = gent(mt_star);
			break;
		}
		case F:
		{
			star.class_ = L"F";

			std::normal_distribution<> genm{ 1.22, 0.18 };
			std::normal_distribution<> genr{ 1.275, 0.125 };
			std::uniform_int_distribution<int> gent{ 6000, 7600 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.8);

			do star.radius = genr(mt_star);
			while (star.radius <= 0.96);
			star.temperatureK = gent(mt_star);
			break;
		}
		case G:
		{
			star.class_ = L"G";

			std::normal_distribution<> genm{ 0.92, 0.12 };
			std::normal_distribution<> genr{ 1.055, 0.095 };
			std::uniform_int_distribution<int> gent{ 5200, 6100 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.45);

			do star.radius = genr(mt_star);
			while (star.radius <= 0.7);
			star.temperatureK = gent(mt_star);
			break;
		}
		case K:
		{
			star.class_ = L"K";

			std::normal_distribution<> genm{ 0.625, 0.175 };
			std::normal_distribution<> genr{ 0.83, 0.13 };
			std::uniform_int_distribution<int> gent{ 3700, 5300 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.08);

			do star.radius = genr(mt_star);
			while (star.radius <= 0);
			star.temperatureK = gent(mt_star);
			break;
		}
		case M:
		{
			star.class_ = L"M";

			std::normal_distribution<> genm{ 0.265, 0.185 };
			std::normal_distribution<> genr{ 0.5, 0.2 };
			std::uniform_int_distribution<int> gent{ 2200, 3800 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.07);

			do star.radius = genr(mt_star);
			while (star.radius <= 0);
			star.temperatureK = gent(mt_star);
			break;
		}
		case WD:
		{
			star.class_ = L"WD";

			std::normal_distribution<> genm{ 0.675, .2625 };
			std::normal_distribution<> genr{ 0.009, 0.0005 };
			std::uniform_int_distribution<int> gent{ 60000, 120000 };

			do star.mass = genm(mt_star);
			while (star.mass < 0.15 || star.mass > 1.4);

			do star.radius = genr(mt_star);
			while (star.radius < 0.005 || star.radius > 0.015);
			star.temperatureK = gent(mt_star);
			break;
		}
		case Q:
		{
			star.class_ = L"Q";

			std::normal_distribution<> genm{ 1.8, 0.5 };
			std::normal_distribution<> genr{ 10, 5 };
			std::uniform_int_distribution<int> gent{ 100000, 1000000 };

			do star.mass = genm(mt_star);
			while (star.mass < 1.1 || star.mass > 2.5);

			do star.radius = genr(mt_star);
			while (star.radius < 5 || star.radius > 20);
			star.radius /= 695508.0;
			star.temperatureK = gent(mt_star);
			break;
		}
		case X:
		{
			star.class_ = L"X";

			int c = 299792458;
			int G = 190809;

			std::uniform_real_distribution<> genm{ 1, 1000 };
			star.mass = genm(mt_star);
			star.radius = ((2 * star.mass * G) / (pow(c, 2)) * 1000000);
			star.temperatureK = 0;
			break;
		}
		}

		tempSol = (star.temperatureK / 5778.0);
		star.luminosity = (pow(star.radius, 2) * pow(tempSol, 4));
		star.innerLimit = 0.1 * star.mass;
		star.outerLimit = 40 * star.mass * CONFIG.systemRadiusModifier;
		star.frostLine = (4.85 * sqrt(star.luminosity));
		star.habitZoneInnerLimit = sqrt(star.luminosity / 1.1);
		star.habitZoneOuterLimit = sqrt(star.luminosity / 0.53);
		star.totalDist = star.outerLimit - star.innerLimit;

		star.RA[0] = genhour(mt_star);
		star.RA[1] = genminute(mt_star);
		star.RA[2] = gensec(mt_star);

		star.DEC[0] = gendecl(mt_star);
		star.DEC[1] = genminute(mt_star);
		star.DEC[2] = gensec(mt_star);

		star.distance = gendistance(mt_star);

		double SMmod = 0.0;
		double SMpoint = star.innerLimit;

		for (;SMpoint < star.outerLimit;)		//Set semimajor points
		{			
			do SMmod = genSM(mt_star);
			while (SMmod < 1);
			SMpoint *= SMmod;
			if (SMpoint > star.innerLimit && SMpoint < star.outerLimit)
				star.semimajorList.push_back(SMpoint);
		}
			
		star.semimajorStaticList = star.semimajorList; // keeps a copy of the original semimajor points
		star.maxPlanetNumber = star.semimajorList.size();
	}
	void GeneratePlanet(SEStar& star, SEPlanet& planet)
	{
		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, CONFIG.SDEccentricity };
		std::normal_distribution<> geninclination{ CONFIG.avgInclination, CONFIG.SDInclination };
		planet.type = L"Planet";
		planet.name = GenName(typePlanet);
		planet.parentBody = &star;

		//######################################################################################################
			//	GENERATE SEMIMAJOR

		int holder, size = star.semimajorList.size() - 1;
		std::uniform_int_distribution<int> gensemimajor{ 0, size };
		holder = gensemimajor(mt_planet);
		planet.semimajorAxis = star.semimajorList.at(holder);
		star.semimajorList.erase(star.semimajorList.begin() + holder);

		//######################################################################################################
			//	CLASS GENERATION

		// If Smart Generation is turned on
		if (CONFIG.smartPlacement)
		{
			// Classes less than Habit Zone / 2
			if (planet.semimajorAxis < (star.habitZoneInnerLimit / 2))
			{
				std::wstring planetClassList[4] = { L"Terra", L"Ferria", L"Carbonia", L"Neptune" };
				std::discrete_distribution<int> genclass{ 30, 5, 5, 10 };
				planet.class_ = planetClassList[genclass(mt_planet)];
			}
			// Classes around the HZ
			if (planet.semimajorAxis > (star.habitZoneInnerLimit / 2) && planet.semimajorAxis < (star.habitZoneOuterLimit * 2))
			{
				std::wstring planetClassList[4] = { L"Terra", L"Ferria", L"Carbonia", L"Aquaria" };
				std::discrete_distribution<int> genclass{ 25, 5, 5, 15 };
				planet.class_ = planetClassList[genclass(mt_planet)];
			}
			// Classes before frost line
			if (planet.semimajorAxis > (star.habitZoneOuterLimit * 2) && planet.semimajorAxis < star.frostLine)
			{
				std::wstring planetClassList[5] = { L"Terra", L"Ferria", L"Carbonia", L"Neptune", L"Aquaria" };
				std::discrete_distribution<int> genclass{ 10, 5, 5, 15, 15 };
				planet.class_ = planetClassList[genclass(mt_planet)];
			}
			// Classes after frost line
			if (planet.semimajorAxis > star.frostLine)
			{
				std::wstring planetClassList[6] = { L"Terra", L"Ferria", L"Carbonia", L"Neptune", L"Aquaria", L"Jupiter" };
				std::discrete_distribution<int> genclass{ 5, 5, 5, 10, 5, 20 };
				planet.class_ = planetClassList[genclass(mt_planet)];
			}
		}
		// If Smart Generation is turned off
		else
		{
			const int PCSIZE = 6;
			std::wstring planetClassList[PCSIZE] = { L"Terra", L"Ferria", L"Carbonia", L"Aquaria", L"Neptune", L"Jupiter" }; // All classes for every position
			std::uniform_int_distribution<int> genclass{ 0, (PCSIZE - 1) };
			planet.class_ = planetClassList[genclass(mt_planet)];
		}

			/*################################
				TEST BOX
			################################*/
			//	planet.class_ = L"Jupiter";
			//################################

		//######################################################################################################
			//	MASS / RADIUS / DENSITY GENERATION

		if (CONFIG.generateComposition)
		{
			double leftOver = 100;

			if (planet.class_ == L"Jupiter")
			{
				std::normal_distribution<> genmain{ 50, 20 };
				do planet.interior.hydrogen = genmain(mt_planet);
				while (planet.interior.hydrogen < 25 || planet.interior.hydrogen > 80);
				leftOver -= planet.interior.hydrogen;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.ices = gensecond(mt_planet);
				leftOver -= planet.interior.ices;

				std::uniform_real_distribution<> genthird{ 0 , leftOver };
				planet.interior.helium = genthird(mt_planet);
				leftOver -= planet.interior.helium;

				std::uniform_real_distribution<> genfourth{ 0 , leftOver };
				planet.interior.silicates = genfourth(mt_planet);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> genfifth{ 0 , leftOver };
					planet.interior.carbides = genfifth(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				planet.interior.metals = leftOver;

				if (genpercent(mt_planet) < 80)
				{
					std::uniform_real_distribution<> genr{ 15000, 72000 };
					planet.radius = genr(mt_planet);
				}
				else
				{
					std::normal_distribution<> genr{ 70000, 2000 };
					planet.radius = genr(mt_planet);
				}
			}
			if (planet.class_ == L"Neptune")
			{
				std::normal_distribution<> genmain{ 50, 20 };
				do planet.interior.ices = genmain(mt_planet);
				while (planet.interior.ices < 25 || planet.interior.ices > 80);
				leftOver -= planet.interior.ices;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.silicates = gensecond(mt_planet);
				leftOver -= planet.interior.silicates;

				std::uniform_real_distribution<> genthird{ 0 , leftOver };
				planet.interior.hydrogen = genthird(mt_planet);
				leftOver -= planet.interior.hydrogen;

				std::uniform_real_distribution<> genfourth{ 0 , leftOver };
				planet.interior.helium = genfourth(mt_planet);
				leftOver -= planet.interior.helium;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> genfifth{ 0 , leftOver };
					planet.interior.carbides = genfifth(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				planet.interior.metals = leftOver;

				std::uniform_real_distribution<> genr{ 8000, 55000 };
				planet.radius = genr(mt_planet);
			}

			if (planet.class_ == L"Terra")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.silicates = genmain(mt_planet);
				while (planet.interior.silicates < 50 || planet.interior.silicates > 99);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do planet.interior.ices = genthird(mt_planet);
					while (planet.interior.ices > 10);
					leftOver -= planet.interior.ices;
				}

				planet.interior.metals = leftOver;

				std::uniform_real_distribution<> genr{ 2000, 13000 };
				planet.radius = genr(mt_planet);
			}
			else if (planet.class_ == L"Ferria")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.metals = genmain(mt_planet);
				while (planet.interior.metals < 50 || planet.interior.metals > 99);
				leftOver -= planet.interior.metals;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do planet.interior.ices = genthird(mt_planet);
					while (planet.interior.ices > 10);
					leftOver -= planet.interior.ices;
				}

				planet.interior.silicates = leftOver;

				std::uniform_real_distribution<> genr{ 2000, 13000 };
				planet.radius = genr(mt_planet);
			}
			else if (planet.class_ == L"Aquaria")
			{
				std::normal_distribution<> genmain{ 30, 20 };
				do planet.interior.ices = genmain(mt_planet);
				while (planet.interior.ices < 10 || planet.interior.ices > 80);
				leftOver -= planet.interior.ices;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.silicates = gensecond(mt_planet);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 5)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				planet.interior.metals = leftOver;

				std::uniform_real_distribution<> genr{ 2000, 13000 };
				planet.radius = genr(mt_planet);
			}
			else if (planet.class_ == L"Carbonia")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.carbides = genmain(mt_planet);
				while (planet.interior.carbides < 50 || planet.interior.carbides > 99);
				leftOver -= planet.interior.carbides;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.silicates = gensecond(mt_planet);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.ices = gensecond(mt_planet);
					leftOver -= planet.interior.ices;
				}

				planet.interior.metals = leftOver;

				std::uniform_real_distribution<> genr{ 2000, 13000 };
				planet.radius = genr(mt_planet);
			}

			// gas giants are too complicated to calculate a density just based on composition,
			// so mass is pseudo-calculated based on radius.
			if (planet.class_ == L"Jupiter")
			{
				std::uniform_real_distribution<> genm{ 200, 800 }; // this modifies the radius to get a mass
				planet.mass = planet.radius / genm(mt_planet);

				if (planet.radius > 68000)
				{
					std::uniform_real_distribution<> genn{ 2, 4 };
					planet.mass *= genn(mt_planet);
				}

				planet.earthRadius = planet.radius / 6378.14;
				planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
				planet.density = planet.gravity / planet.earthRadius;
			}
			else if (planet.class_ == L"Neptune")
			{
				std::uniform_real_distribution<> genm{ 500, 2000 }; // this modifies the radius to get a mass
				planet.mass = planet.radius / genm(mt_planet);

				planet.earthRadius = planet.radius / 6378.14;
				planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
				planet.density = planet.gravity / planet.earthRadius;
			}
			else
			{
				planet.density = ((planet.interior.metals / 100) * 7.874) + ((planet.interior.silicates / 100) * 3.25) + (planet.interior.ices / 100) + ((planet.interior.carbides / 100) * 2.1);
				planet.earthRadius = planet.radius / 6378.14;
				planet.mass = pow(planet.earthRadius, 3) * (planet.density / 5.51);
			}	
		}
		else
		{
			if (planet.class_ == L"Jupiter")
			{
				if (genpercent(mt_planet) < 60)
				{
					std::uniform_real_distribution<> genm{ 10, 320 };
					planet.mass = genm(mt_planet);
				}
				else
				{
					std::uniform_real_distribution<> genm{ 320, 2000 };
					planet.mass = genm(mt_planet);
				}

				if (planet.mass < 60)
				{
					std::normal_distribution<> genr{ 20000, 5000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 13000 || planet.radius > 33000);
				}
				else if (planet.mass < 150)
				{
					std::normal_distribution<> genr{ 55000, 2000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 30000 || planet.radius > 80000);
				}
				else if (planet.mass < 320)
				{
					std::normal_distribution<> genr{ 71492, 7000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 60000 || planet.radius > 80000);
				}
				else
				{
					std::normal_distribution<> genr{ 71492, 15000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 20000 || planet.radius > 72000);
				}

				planet.earthRadius = planet.radius / 6378.14;
				planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
				planet.density = planet.gravity / planet.earthRadius;
				//		density = (gravity / earthRadius) * 5.514;
				//		cout << L"desnity : L" << density << L" g/cm^3" << endl;
			}
			else if (planet.class_ == L"Neptune")
			{
				std::uniform_real_distribution<> genm{ 2, 70 };
				planet.mass = genm(mt_planet);

				if (planet.mass < 8)
				{
					std::normal_distribution<> genr{ 10000, 500 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 7000 || planet.radius > 15000);
				}
				else if (planet.mass < 17)
				{
					std::normal_distribution<> genr{ 20000, 1000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 15000 || planet.radius > 25000);
				}
				else if (planet.mass < 25)
				{
					std::normal_distribution<> genr{ 23000, 1000 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 18000 || planet.radius > 28000);
				}
				else if (planet.mass < 35)
				{
					std::normal_distribution<> genr{ 26000, 750 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 21000 || planet.radius > 30000);
				}
				else if (planet.mass < 60)
				{
					std::normal_distribution<> genr{ 28000, 750 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 25000 || planet.radius > 30000);
				}
				else if (planet.mass <= 70)
				{
					std::normal_distribution<> genr{ 30000, 500 };
					do planet.radius = genr(mt_planet);
					while (planet.radius <= 28000 || planet.radius > 32000);
				}

				planet.earthRadius = planet.radius / 6378.14;
				planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
				planet.density = planet.gravity / planet.earthRadius;
			}

			else if (planet.class_ == L"Terra")
			{
				do
				{
					if (genpercent(mt_planet) < 50)
					{
						std::uniform_real_distribution<> genm{ 0.2, 2 };
						planet.mass = genm(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genm{ 2, 6 };
						planet.mass = genm(mt_planet);
					}

					if (planet.mass < 1)
					{
						std::uniform_real_distribution<> genr{ 2000, 6500 };
						planet.radius = genr(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genr{ 6500, 13000 };
						planet.radius = genr(mt_planet);
					}

					planet.earthRadius = planet.radius / 6378.14;
					planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
					planet.density = planet.gravity / planet.earthRadius;

				} while (planet.density < 0.75 || planet.density > 1.3);
			}
			else if (planet.class_ == L"Ferria")
			{
				do
				{
					if (genpercent(mt_planet) < 50)
					{
						std::uniform_real_distribution<> genm{ 0.2, 2 };
						planet.mass = genm(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genm{ 2, 6 };
						planet.mass = genm(mt_planet);
					}

					if (planet.mass < 1)
					{
						std::uniform_real_distribution<> genr{ 2000, 6500 };
						planet.radius = genr(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genr{ 6500, 13000 };
						planet.radius = genr(mt_planet);
					}

					planet.earthRadius = planet.radius / 6378.14;
					planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
					planet.density = planet.gravity / planet.earthRadius;

				} while (planet.density < 1 || planet.density > 2);
			}
			else if (planet.class_ == L"Carbonia")
			{
				do
				{
					if (genpercent(mt_planet) < 50)
					{
						std::uniform_real_distribution<> genm{ 0.2, 2 };
						planet.mass = genm(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genm{ 2, 6 };
						planet.mass = genm(mt_planet);
					}

					if (planet.mass < 1)
					{
						std::uniform_real_distribution<> genr{ 2000, 6500 };
						planet.radius = genr(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genr{ 6500, 13000 };
						planet.radius = genr(mt_planet);
					}

					planet.earthRadius = planet.radius / 6378.14;
					planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
					planet.density = planet.gravity / planet.earthRadius;

				} while (planet.density < 0.6 || planet.density > 1);
			}
			else if (planet.class_ == L"Aquaria")
			{
				do
				{
					if (genpercent(mt_planet) < 50)
					{
						std::uniform_real_distribution<> genm{ 0.2, 2 };
						planet.mass = genm(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genm{ 2, 6 };
						planet.mass = genm(mt_planet);
					}

					if (planet.mass < 1)
					{
						std::uniform_real_distribution<> genr{ 2000, 7000 };
						planet.radius = genr(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> genr{ 6500, 13000 };
						planet.radius = genr(mt_planet);
					}

					planet.earthRadius = planet.radius / 6378.14;
					planet.gravity = planet.mass / (pow(planet.earthRadius, 2));
					planet.density = planet.gravity / planet.earthRadius;

				} while (planet.density < 0.5 || planet.density > 0.75);
			}
		}

		//######################################################################################################
			//	ORBIT GENERATION

		planet.period = ( sqrt(pow(planet.semimajorAxis, 3) / planet.parentBody->mass) );
		planet.obliquity = genobliquity(mt_planet);
		do planet.eccentricity = geneccentricity(mt_planet);
		while (planet.eccentricity <= 0 || planet.eccentricity >= 1);
		planet.inclination = geninclination(mt_planet);
		planet.ascendingNode = gendegree(mt_planet);
		planet.argofPericenter = gendegree(mt_planet);
		planet.meanAnomaly = gendegree(mt_planet);

		//######################################################################################################
		//	EXOTIC GENERATION

		// Determines an exotic orbit
		if (genpercent(mt_planet) < CONFIG.exotic_OrbitChance)
		{
			if (genpercent(mt_planet) < 50)
				planet.inclination += 180;
			else
			{
				std::normal_distribution<> genhighinclination{ 60, 10 };
				planet.inclination = genhighinclination(mt_planet);
			}
		}

		// Determines an exotic rotation
		if (genpercent(mt_planet) < CONFIG.exotic_AxialTiltChance)
		{
			if (genpercent(mt_planet) < 50)
				planet.obliquity += 180;
			else
			{
				std::normal_distribution<> genhighobliquity{ 90, 20 };
				planet.obliquity = genhighobliquity(mt_planet);
			}
		}

		// companion orbit
		planet.hasCompanionOrbit = false;
		if (genpercent(mt_planet) < CONFIG.exotic_CompanionOrbitChance)
			planet.hasCompanionOrbit = true;

		planet.debrisCount = 0;
		// Determines Debris Ring
		if (genpercent(mt_planet) < CONFIG.exotic_DebrisRingChance)
		{
			std::uniform_int_distribution<> GenDebrisNumber{ 15, 60 };
			planet.debrisCount = GenDebrisNumber(mt_planet);
		}

		// Determines Life
		if (CONFIG.traditionalLife)
		{
			if (planet.semimajorAxis > star.habitZoneInnerLimit && planet.semimajorAxis < star.habitZoneOuterLimit
					&& planet.class_ != L"Jupiter" && planet.class_ != L"Neptune"
					&& genpercent(mt_planet) < CONFIG.life_OrganicChance)
				planet.life_organic.haslife = true;

			if (genpercent(mt_planet) < CONFIG.life_ExoticChance)
				planet.life_exotic.haslife = true;
		}
		else
		{
			if (genpercent(mt_planet) < CONFIG.life_OrganicChance)
				planet.life_organic.haslife = true;
			if (genpercent(mt_planet) < CONFIG.life_ExoticChance)
				planet.life_exotic.haslife = true;
		}
		ExoticGenerateLife(planet);

		//######################################################################################################
			//	MOON NUMBER GENERATION

		if (planet.mass > 10)
			planet.hillSphereOuterLimit = planet.semimajorAxis * pow(((planet.mass / 317.832) / star.mass), (1.0 / 3.0)) * 235;
		else
			planet.hillSphereOuterLimit = planet.semimajorAxis * pow((planet.mass / star.mass), (1.0 / 3.0)) * 235;
		planet.hillSphereOuterLimit *= planet.radius;

		// Hill Sphere Caps
		if (planet.class_ == L"Jupiter" && planet.mass > 600 && planet.hillSphereOuterLimit > 74799000)
			planet.hillSphereOuterLimit = 74799000; // hillSphereOuterLimit Limit Capped at 0.5 AU
		else if (planet.class_ == L"Jupiter" && planet.hillSphereOuterLimit > 37399500)
			planet.hillSphereOuterLimit = 37399500; // hillSphereOuterLimit Limit Capped at 0.25 AU
		else if (planet.class_ == L"Neptune" && planet.hillSphereOuterLimit > 14959800)
			planet.hillSphereOuterLimit = 14959800; // hillSphereOuterLimit Limit Capped at 0.1 AU
		else if (planet.class_ != L"Neptune" && planet.class_ != L"Jupiter" && planet.hillSphereOuterLimit > 1495980)
			planet.hillSphereOuterLimit = 1495980;  // hillSphereOuterLimit Limit Capped at 0.01 AU

		if (CONFIG.weightedMoons)
		{
			planet.majorMoonPercent = ceil(((planet.semimajorAxis - star.innerLimit) / star.totalDist) * 100); // the relative disatnce to the outer limit
			if (planet.class_ == L"Jupiter" || planet.class_ == L"Neptune")
			{
				if (planet.majorMoonPercent > 60)
					planet.majorMoonPercent = 60;
				planet.majorMoonPercent += ceil(planet.mass / 158.0); // gas giants add half jupiter mass
			}
			else
			{
				if (planet.majorMoonPercent > 50)
					planet.majorMoonPercent = 50;
				planet.majorMoonPercent += ceil(planet.mass); // terra add earth mass
			}

			// bonus percents
			if (planet.semimajorAxis > star.habitZoneInnerLimit && planet.semimajorAxis < star.habitZoneOuterLimit)
				planet.majorMoonPercent += 33; // if in the habitable zone, extra chance for moons
			if (planet.semimajorAxis > star.habitZoneOuterLimit)
				planet.majorMoonPercent += 15;
			if (planet.semimajorAxis > star.frostLine && (planet.class_ == L"Jupiter" || planet.class_ == L"Neptune"))
				planet.majorMoonPercent += 15; // past frost line

			if (planet.class_ == L"Jupiter" || planet.class_ == L"Neptune")
				planet.minorMoonPercent = planet.majorMoonPercent * 7; // gas giants multiply by 7
			else
				planet.minorMoonPercent = planet.majorMoonPercent * 2; // terras multiply by 2
		}
		else
		{
			if (planet.semimajorAxis < star.frostLine)
			{ // major up to 3, minor up to 15; ice/gas giants major up to 5, minor up to 20

				if (planet.class_ == L"Jupiter" || planet.class_ == L"Neptune")
				{
					std::discrete_distribution<int> genmajorc{ 1, 3, 5, 3, 2, 1 };
					std::discrete_distribution<int> genminorc{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

					planet.numberOfMajorMoons = genmajorc(mt_planet);
					planet.numberOfMinorMoons = genminorc(mt_planet);
				}
				else if (planet.type == L"DwarfPlanet")
				{
					std::discrete_distribution<int> genminorc{ 5, 1 };
					planet.numberOfMinorMoons = genminorc(mt_planet);
					planet.numberOfMajorMoons = 0;
				}
				else
				{
					std::discrete_distribution<int> genmajorc{ 7, 15, 5, 1 };
					std::discrete_distribution<int> genminorc{ 32, 30, 28, 26, 24, 22, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

					planet.numberOfMajorMoons = genmajorc(mt_planet);
					planet.numberOfMinorMoons = genminorc(mt_planet);
				}

			}
			else // planets above the frost line
			{ // major up to 5, minor up to 25; ice giants major up to 8, minor up to 40; gas giant major up to 12, minor up to 70

				if (planet.class_ == L"Jupiter")
				{
					std::discrete_distribution<int> genmajorc{ 0, 1, 3, 5, 7, 9, 10, 10, 9, 7, 5, 3, 1 };
					std::discrete_distribution<int> genminorc{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
															21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 35, 34, 33,
															32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
															14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

					planet.numberOfMajorMoons = genmajorc(mt_planet);
					planet.numberOfMinorMoons = genminorc(mt_planet);
				}
				else if (planet.class_ == L"Neptune")
				{
					std::discrete_distribution<int> genmajorc{ 0, 1, 3, 5, 7, 7, 5, 3, 1 };
					std::discrete_distribution<int> genminorc{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
															20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

					planet.numberOfMajorMoons = genmajorc(mt_planet);
					planet.numberOfMinorMoons = genminorc(mt_planet);
				}
				else if (planet.type == L"DwarfPlanet")
				{
					std::discrete_distribution<int> genminorc{ 5, 3, 2, 1 };
					planet.numberOfMinorMoons = genminorc(mt_planet);
					planet.numberOfMajorMoons = 0;
				}
				else
				{
					std::discrete_distribution<int> genmajorc{ 5, 15, 10, 5, 3, 1 };
					std::discrete_distribution<int> genminorc{ 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

					planet.numberOfMajorMoons = genmajorc(mt_planet);
					planet.numberOfMinorMoons = genminorc(mt_planet);
				}
			}
		}
		
		// If hill sphere inner is larger than outer, then moons are disabled
		if ((2.44 * pow((planet.density / 0.05), (1.0 / 3.0)) * planet.radius) > planet.hillSphereOuterLimit)
		{
			SendDebugMessage((planet.name + L"'s hill sphere inner was larger than it's hill sphere outer! Moons disabled..."), debug_ERROR);
			planet.numberOfMajorMoons = -1;
			planet.numberOfMinorMoons = -1;
			planet.majorMoonPercent = -1;
			planet.minorMoonPercent = -1;
		}
	}
	void GenerateDwarfPlanet(SEStar& star, SEPlanet& planet)
	{
		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		planet.type = L"DwarfPlanet";
		planet.name = GenName(typePlanet);
		planet.parentBody = &star;

		//######################################################################################################
			//	CLASS / RADIUS / DENSITY GENERATION

		std::wstring planetClassList[4] = { L"Terra", L"Ferria", L"Carbonia", L"Aquaria" };
		std::discrete_distribution<int> genclass{ 25, 5, 5, 15 };
		planet.class_ = planetClassList[genclass(mt_planet)];

		if (CONFIG.generateComposition)
		{
			double leftOver = 100;

			if (planet.class_ == L"Terra")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.silicates = genmain(mt_planet);
				while (planet.interior.silicates < 50 || planet.interior.silicates > 99);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do planet.interior.ices = genthird(mt_planet);
					while (planet.interior.ices > 10);
					leftOver -= planet.interior.ices;
				}

				planet.interior.metals = leftOver;
			}
			else if (planet.class_ == L"Ferria")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.metals = genmain(mt_planet);
				while (planet.interior.metals < 50 || planet.interior.metals > 99);
				leftOver -= planet.interior.metals;

				if (genpercent(mt_planet) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do planet.interior.ices = genthird(mt_planet);
					while (planet.interior.ices > 10);
					leftOver -= planet.interior.ices;
				}

				planet.interior.silicates = leftOver;
			}
			else if (planet.class_ == L"Aquaria")
			{
				std::normal_distribution<> genmain{ 30, 20 };
				do planet.interior.ices = genmain(mt_planet);
				while (planet.interior.ices < 10 || planet.interior.ices > 80);
				leftOver -= planet.interior.ices;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.silicates = gensecond(mt_planet);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 5)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.carbides = gensecond(mt_planet);
					leftOver -= planet.interior.carbides;
				}

				planet.interior.metals = leftOver;
			}
			else if (planet.class_ == L"Carbonia")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do planet.interior.carbides = genmain(mt_planet);
				while (planet.interior.carbides < 50 || planet.interior.carbides > 99);
				leftOver -= planet.interior.carbides;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				planet.interior.silicates = gensecond(mt_planet);
				leftOver -= planet.interior.silicates;

				if (genpercent(mt_planet) < 33)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					planet.interior.ices = gensecond(mt_planet);
					leftOver -= planet.interior.ices;
				}

				planet.interior.metals = leftOver;
			}

			std::uniform_real_distribution<> genr{ 190, 2000 };
			planet.radius = genr(mt_planet);

			planet.density = ((planet.interior.metals / 100) * 7.874) + ((planet.interior.silicates / 100) * 3.25) + (planet.interior.ices / 100) + ((planet.interior.carbides / 100) * 2.1);
			planet.earthRadius = planet.radius / 6378.14;
			planet.mass = pow(planet.earthRadius, 3) * (planet.density / 5.51);
		}
		else
		{
			if (planet.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.5 };
				std::uniform_real_distribution<> genr{ 190, 2000 };

				planet.density = gend(mt_planet);
				planet.radius = genr(mt_planet);
				planet.earthRadius = planet.radius / 6378.14;
				planet.mass = pow(planet.earthRadius, 3) * planet.density;
				planet.gravity = planet.mass / pow(planet.earthRadius, 2);
			}
			else if (planet.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.01, 0.5 };
				std::uniform_real_distribution<> genr{ 190, 2000 };

				planet.density = gend(mt_planet);
				planet.radius = genr(mt_planet);
				planet.earthRadius = planet.radius / 6378.14;
				planet.mass = pow(planet.earthRadius, 3) * planet.density;
				planet.gravity = planet.mass / pow(planet.earthRadius, 2);
			}
		}

		//######################################################################################################
			//	semimajorAxis GENERATION

		if (CONFIG.smartPlacement)
		{
			//Class is terra/ferria/carbonia...
			if (planet.class_ != L"Aquaria")
			{
				int percent = genpercent(mt_planet);
				if (percent < 40)
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.outerLimit / 1.5), (star.outerLimit * 3) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
				else if (percent > 60)
				{
					std::uniform_real_distribution<> gensemimajorAxis{ star.innerLimit, (star.innerLimit * 1.5) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
				else
				{
					if (star.frostLine < star.innerLimit)
					{
						std::uniform_real_distribution<> gensemimajorAxis{ (star.outerLimit / 1.5), (star.outerLimit * 3) };
						planet.semimajorAxis = gensemimajorAxis(mt_planet);
					}
					else
					{
						std::uniform_real_distribution<> gensemimajorAxis{ (star.frostLine / 1.5), (star.frostLine * 1.5) };
						planet.semimajorAxis = gensemimajorAxis(mt_planet);
					}
				}
			}
			// Class is aquaria...
			else if (star.frostLine > star.outerLimit) // for large stars like O
			{
				std::uniform_real_distribution<> gensemimajorAxis{ (star.frostLine / 1.2), (star.frostLine * 3) };
				planet.semimajorAxis = gensemimajorAxis(mt_planet);
			}
			else if (star.frostLine < star.innerLimit) // for some Q class and low luminosity
			{
				int percent = genpercent(mt_planet);
				if (percent < 60)
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.outerLimit / 1.5), (star.outerLimit * 3) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
				else
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.innerLimit / 1.2), (star.innerLimit * 3) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
			}
			else // for normal case stars
			{
				int percent = genpercent(mt_planet);
				if (percent < 60)
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.outerLimit / 1.5), (star.outerLimit * 3) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
				else
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.frostLine / 1.5), (star.frostLine * 1.5) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
			}
		}
		// If Smart Placement is off
		else
		{
			std::uniform_real_distribution<> gensemimajorAxis{ (star.innerLimit), (star.outerLimit * 3) };
			planet.semimajorAxis = gensemimajorAxis(mt_planet);
		}

		//######################################################################################################
			//	ORBIT GENERATION

		if (planet.semimajorAxis > star.outerLimit)
		{
			std::normal_distribution<> geneccentricity{ (CONFIG.avgEccentricity * 2), (CONFIG.SDEccentricity * 3) };
			std::normal_distribution<> geninclination{ CONFIG.avgInclination, (CONFIG.SDInclination * 4) };

			do planet.eccentricity = geneccentricity(mt_planet);
			while (planet.eccentricity <= 0 || planet.eccentricity >= 1);
			planet.inclination = geninclination(mt_planet);
		}
		else if (planet.semimajorAxis <= (star.frostLine * 1.5) && planet.semimajorAxis > (star.frostLine / 1.5))
		{
			std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, (CONFIG.SDEccentricity * 1.25) };
			std::normal_distribution<> geninclination{ CONFIG.avgInclination, (CONFIG.SDInclination * 2) };

			do planet.eccentricity = geneccentricity(mt_planet);
			while (planet.eccentricity <= 0 || planet.eccentricity >= 1);
			planet.inclination = geninclination(mt_planet);
		}
		else
		{
			std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, CONFIG.SDEccentricity };
			std::normal_distribution<> geninclination{ CONFIG.avgInclination, CONFIG.SDInclination };

			do planet.eccentricity = geneccentricity(mt_planet);
			while (planet.eccentricity <= 0 || planet.eccentricity >= 1);
			planet.inclination = geninclination(mt_planet);
		}

		planet.period = (sqrt(pow(planet.semimajorAxis, 3) / planet.parentBody->mass));
		planet.obliquity = genobliquity(mt_planet);
		planet.ascendingNode = gendegree(mt_planet);
		planet.argofPericenter = gendegree(mt_planet);
		planet.meanAnomaly = gendegree(mt_planet);

		// companion orbit
		if (genpercent(mt_planet) < CONFIG.exotic_CompanionOrbitChance)
			planet.hasCompanionOrbit = true;

		//######################################################################################################
			//	MOON NUMBER GENERATION

		planet.hillSphereOuterLimit = planet.semimajorAxis * pow((planet.mass / star.mass), (1.0 / 3.0)) * 235;
		planet.hillSphereOuterLimit *= planet.radius;

		// Hill Sphere Caps
		if (planet.hillSphereOuterLimit > 1495980)
			planet.hillSphereOuterLimit = 1495980; // hillSphereOuterLimit Limit Capped at 0.01 AU

		if (CONFIG.weightedMoons)
		{
			planet.minorMoonPercent = ceil(((planet.semimajorAxis - star.innerLimit) / star.totalDist) * 100); // the relative disatnce to the outer limit
			if (planet.minorMoonPercent > 70)
				planet.minorMoonPercent = 70;
			planet.minorMoonPercent += ceil(planet.mass); // terra add earth mass

			// bonus percents
			/*if (planet.semimajorAxis > star.habitZoneOuterLimit)
				planet.majorMoonPercent += 20;
			if (planet.semimajorAxis > star.frostLine)
				planet.majorMoonPercent += 20;*/
		}
		else
		{
			if (planet.semimajorAxis < star.frostLine)
			{ // up to three
				std::discrete_distribution<int> genminorc{ 10, 5, 3, 1 };
				planet.numberOfMinorMoons = genminorc(mt_planet);
			}
			else // planets above the frost line
			{ // up to 10
				std::discrete_distribution<int> genminorc{ 20, 20, 20, 10, 10, 10, 5, 5, 5, 3, 1 };
				planet.numberOfMinorMoons = genminorc(mt_planet);
			}
		}

		// if hill sphere inner is larger than outer
		if ((2.44 * planet.radius * pow((planet.density / 0.5), (1.0 / 3.0))) > planet.hillSphereOuterLimit)
		{
			SendDebugMessage((planet.name + L"'s hill sphere inner was larger than it's hill sphere outer! Moons disabled..."), debug_ERROR);
			planet.numberOfMajorMoons = -1;
			planet.numberOfMinorMoons = -1;
			planet.majorMoonPercent = -1;
			planet.minorMoonPercent = -1;
		}
	}
	bool GenerateMajorMoon(SEStar& star, SEPlanet& parent, SEPlanet& moon, int spacer)
	{
		bool testsemi = false;
		int testbreak = 0;
		moon.type = L"Moon";
		moon.parentBody = &parent;

		if (NV.nameTerraMoons && parent.class_ != L"Jupiter" && parent.class_ != L"Neptune")
			moon.name = GenName(typeMoon);
		else if (NV.nameGasMoons && (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune"))
			moon.name = GenName(typeMoon);
		else
		{
			moon.name = L"Major Moon ";
			moon.name += std::to_wstring(spacer + 1);
		}	

		//######################################################################################################
			//	CLASS GENERATION

		const int PCSIZE = 4;
		std::wstring moonClassList[PCSIZE] = { L"Terra", L"Ferria", L"Carbonia", L"Aquaria" };
		std::discrete_distribution<int> genclass{ 20, 5, 5, 20 };
		moon.class_ = moonClassList[genclass(mt_moon)];

		//class_ = L"Terra";

		//######################################################################################################
			//	DENSITY / MASS GENERATION

		// generate mass based on parent
		if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune") // gas giants
		{
			std::uniform_real_distribution<> genm{ parent.mass * 0.000003, parent.mass * 0.0001 };
			moon.mass = genm(mt_moon);
		}
		else // small planets
		{
			std::uniform_real_distribution<> genm{ parent.mass * 0.0001, parent.mass * 0.01 };
			moon.mass = genm(mt_moon);
		}

		if (CONFIG.generateComposition)
		{
			double leftOver = 100;

			if (moon.class_ == L"Terra")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do moon.interior.silicates = genmain(mt_moon);
				while (moon.interior.silicates < 50 || moon.interior.silicates > 99);
				leftOver -= moon.interior.silicates;

				if (genpercent(mt_moon) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					moon.interior.carbides = gensecond(mt_moon);
					leftOver -= moon.interior.carbides;
				}

				if (genpercent(mt_moon) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do moon.interior.ices = genthird(mt_moon);
					while (moon.interior.ices > 10);
					leftOver -= moon.interior.ices;
				}

				moon.interior.metals = leftOver;
			}
			else if (moon.class_ == L"Ferria")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do moon.interior.metals = genmain(mt_moon);
				while (moon.interior.metals < 50 || moon.interior.metals > 99);
				leftOver -= moon.interior.metals;

				if (genpercent(mt_moon) < 2)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					moon.interior.carbides = gensecond(mt_moon);
					leftOver -= moon.interior.carbides;
				}

				if (genpercent(mt_moon) < 33)
				{
					std::uniform_real_distribution<> genthird{ 0 , leftOver };
					do moon.interior.ices = genthird(mt_moon);
					while (moon.interior.ices > 10);
					leftOver -= moon.interior.ices;
				}

				moon.interior.silicates = leftOver;
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::normal_distribution<> genmain{ 30, 20 };
				do moon.interior.ices = genmain(mt_moon);
				while (moon.interior.ices < 10 || moon.interior.ices > 80);
				leftOver -= moon.interior.ices;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				moon.interior.silicates = gensecond(mt_moon);
				leftOver -= moon.interior.silicates;

				if (genpercent(mt_moon) < 5)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					moon.interior.carbides = gensecond(mt_moon);
					leftOver -= moon.interior.carbides;
				}

				moon.interior.metals = leftOver;
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::normal_distribution<> genmain{ 60, 20 };
				do moon.interior.carbides = genmain(mt_moon);
				while (moon.interior.carbides < 50 || moon.interior.carbides > 99);
				leftOver -= moon.interior.carbides;

				std::uniform_real_distribution<> gensecond{ 0 , leftOver };
				moon.interior.silicates = gensecond(mt_moon);
				leftOver -= moon.interior.silicates;

				if (genpercent(mt_moon) < 33)
				{
					std::uniform_real_distribution<> gensecond{ 0 , leftOver };
					moon.interior.ices = gensecond(mt_moon);
					leftOver -= moon.interior.ices;
				}

				moon.interior.metals = leftOver;
			}

			moon.density = ((moon.interior.metals / 100) * 7.874) + ((moon.interior.silicates / 100) * 3.25) + (moon.interior.ices / 100) + ((moon.interior.carbides / 100) * 2.1);
		}
		else
		{
			if (moon.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.3 };
				moon.density = gend(mt_moon);
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.1, 1 };
				moon.density = gend(mt_moon);
			}
			else if (moon.class_ == L"Ferria")
			{
				std::uniform_real_distribution<> gend{ 1, 2 };
				moon.density = gend(mt_moon);
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::uniform_real_distribution<> gend{ 0.5, 1 };
				moon.density = gend(mt_moon);
			}
		}

		moon.earthRadius = cbrt(moon.mass / moon.density);
		moon.radius = moon.earthRadius * 6378.14;
		moon.gravity = moon.mass / pow(moon.earthRadius, 2);

		parent.usedRadius_moon.push_back(moon.radius);

		//######################################################################################################
			//	SEMI MAJOR GENERATION

		if (moon.class_ == L"Jupiter")
			moon.hillSphereInnerLimit = 2.44 * pow((parent.density / 0.1), (1.0 / 3.0));
		else
			moon.hillSphereInnerLimit = 2.44 * pow((parent.density / 0.05), (1.0 / 3.0));
		moon.hillSphereInnerLimit *= parent.radius;

		do
		{
			testsemi = false;

			// If the planet is a Gas/Ice Giant, it will use the spacers, if not, it will generate a random num.
			if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune")
			{
				do
				{
					//do spacer = (spacer + 1) * genspacer(mt_moon);
					//while (spacer <= 1);
					std::uniform_real_distribution<> genfirstsemi{ 1, 1.3 };

					moon.semimajorAxis = moon.hillSphereInnerLimit * genfirstsemi(mt_moon);
					for (int count = 0; count < spacer; count++)
					{
						std::normal_distribution<> genspacer{ 1.7, 0.2 };
						moon.semimajorAxis *= genspacer(mt_moon);
					}

					if (moon.semimajorAxis > (parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 10)
					{
						std::uniform_real_distribution<> gensemi{ ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 5), parent.hillSphereOuterLimit };
						moon.semimajorAxis = gensemi(mt_moon);
					}

				} while (moon.semimajorAxis <= (moon.radius * 6) || moon.semimajorAxis > parent.hillSphereOuterLimit || moon.semimajorAxis < moon.hillSphereInnerLimit);

				// checks the semimajor of each moon to make sure they are not too close
				for (int count = 0; count < parent.usedSemimajor_moon.size(); count++)
					if (abs(moon.semimajorAxis - parent.usedSemimajor_moon.at(count)) < parent.radius * (CONFIG.moonDistanceBoundary / 10))
						testsemi = true; // means moons were too close
				
			}
			else
			{
				std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, parent.hillSphereOuterLimit };
				moon.semimajorAxis = gensemi(mt_moon);

				// checks the semimajor of each moon to make sure they are not too close
				for (int count = 0; count < parent.usedSemimajor_moon.size(); count++)
					if (abs(moon.semimajorAxis - parent.usedSemimajor_moon.at(count)) < (parent.radius * CONFIG.moonDistanceBoundary))
						testsemi = true; // means moons were too close
			}

			if (testbreak++ > 10)
			{
				SendDebugMessage( (L"Found no suitable semi major axis for a major moon around " + parent.name + L" in 10 attempts!"), debug_ERROR);
				return 0;
			}

		} while (testsemi);
		parent.usedSemimajor_moon.push_back(moon.semimajorAxis);

		//######################################################################################################
			//	ORBIT GENERATION

		if (parent.class_ == L"Jupiter")
		{
			if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 15))
			{
				// Set 1
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.001 };
				std::normal_distribution<> geninclination{ 0, 0.1 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 10))
			{
				// Set 2
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0.03, 0.01 };
				std::normal_distribution<> geninclination{ 0, 1 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 3
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.1, 0.2 };
				std::normal_distribution<> geninclination{ 0, 25 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}

		else if (parent.class_ == L"Neptune")
		{
			if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 15))
			{
				// Set 4
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.001 };
				std::normal_distribution<> geninclination{ 0, 0.1 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 10))
			{
				// Set 5
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0.03, 0.01 };
				std::normal_distribution<> geninclination{ 0, 1 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 6
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.1, 0.2 };
				std::normal_distribution<> geninclination{ 0, 25 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}
		else
		{
			if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 3))
			{
				// Set 7
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.02 };
				std::normal_distribution<> geninclination{ 0, 0.5 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else if (moon.semimajorAxis < ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 2))
			{
				// Set 8
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0.05, 0.05 };
				std::normal_distribution<> geninclination{ 0, 5 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 9
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.1, 0.1 };
				std::normal_distribution<> geninclination{ 0, 20 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}

		moon.period = (sqrt(pow(km_to_AU(moon.semimajorAxis), 3) / earthMass_to_solMass(moon.parentBody->mass)));
		moon.ascendingNode = gendegree(mt_moon);
		moon.argofPericenter = gendegree(mt_moon);
		moon.meanAnomaly = gendegree(mt_moon);

		//######################################################################################################
			//	EXOTIC GENERATION

		// Determines an exotic orbit
		if (genpercent(mt_moon) < CONFIG.exotic_OrbitChance)
			moon.inclination += 180;

		// Determines an exotic rotation
		if (genpercent(mt_moon) < CONFIG.exotic_AxialTiltChance)
		{
			if (genpercent(mt_moon) < 50)
				moon.obliquity += 180;
			else
			{
				std::normal_distribution<> genhighobliquity{ 90, 20 };
				moon.obliquity = genhighobliquity(mt_moon);
			}
		}

		// companion orbit
		if (genpercent(mt_planet) < CONFIG.exotic_CompanionOrbitChance)
			moon.hasCompanionOrbit = true;

		// Determines Life
		if (CONFIG.traditionalLife)
		{
			if (parent.semimajorAxis > star.habitZoneInnerLimit && parent.semimajorAxis < star.habitZoneOuterLimit
				&& moon.radius > 1000
				&& genpercent(mt_moon) < CONFIG.life_OrganicChance)

				moon.life_organic.haslife = true;

			if (moon.radius > 1000 && genpercent(mt_moon) < CONFIG.life_ExoticChance)
				moon.life_exotic.haslife = true;
		}
		else
		{
			if (genpercent(mt_moon) < CONFIG.life_OrganicChance)
				moon.life_organic.haslife = true;
			if (genpercent(mt_moon) < CONFIG.life_ExoticChance)
				moon.life_exotic.haslife = true;
		}
		ExoticGenerateLife(moon);

		return 1;
	}
	void GenerateMinorMoon(SEPlanet& parent, SEPlanet& moon, int currentMoon)
	{
		int testsemi;
		moon.type = L"DwarfMoon";
		moon.class_ = L"Asteroid";
		moon.parentBody = &parent;

		if (NV.nameTerraDwarfMoons && parent.class_ != L"Jupiter" && parent.class_ != L"Neptune")
			moon.name = GenName(typeDwarfMoon);
		else if (NV.nameGasDwarfMoons && (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune"))
			moon.name = GenName(typeDwarfMoon);
		else
		{
			moon.name = L"Minor Moon ";
			moon.name += std::to_wstring(currentMoon + 1);
		}		

		//######################################################################################################
			//	RADIUS GENERATION

		if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune")
		{
			std::uniform_real_distribution<> genr{ parent.radius * 0.00001, parent.radius * 0.002 };
			moon.radius = genr(mt_moon);
		}
		else
		{
			std::uniform_real_distribution<> genr{ parent.radius * 0.0001, parent.radius * 0.005 };
			moon.radius = genr(mt_moon);
		}

		//######################################################################################################
			//	SEMI MAJOR GENERATION

		if (parent.class_ == L"Jupiter")
			moon.hillSphereInnerLimit = 2.44 * pow((parent.density), (1.0 / 3.0));
		else
			moon.hillSphereInnerLimit = 2.44 * pow((parent.density / 0.5), (1.0 / 3.0));
		moon.hillSphereInnerLimit *= parent.radius;

		do
		{
			testsemi = 0;

			if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune")
			{
				if (genpercent(mt_moon) < 10)
				{
					std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, moon.hillSphereInnerLimit * 3 };
					moon.semimajorAxis = gensemi(mt_moon);
				}
				else
				{
					std::uniform_real_distribution<> gensemi{ ((parent.hillSphereOuterLimit + moon.hillSphereInnerLimit) / 3), parent.hillSphereOuterLimit };
					moon.semimajorAxis = gensemi(mt_moon);
				}
			}
			else
			{
				if (genpercent(mt_moon) < 40)
				{
					std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, moon.hillSphereInnerLimit * 4 };
					moon.semimajorAxis = gensemi(mt_moon);
				}
				else
				{
					std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, parent.hillSphereOuterLimit };
					moon.semimajorAxis = gensemi(mt_moon);
				}
			}

			// Checks the semi major againts others to make sure no moons are too close. if they are, it re-gens.
			for (int count = 0; count < parent.usedSemimajor_moon.size(); count++)
			{
				if ((moon.semimajorAxis - (moon.radius * 2)) < (parent.usedSemimajor_moon.at(count) + (parent.usedRadius_moon.at(count) * 2)) &&
					(moon.semimajorAxis + (moon.radius * 2)) > (parent.usedSemimajor_moon.at(count) - (parent.usedRadius_moon.at(count) * 2)))
					testsemi = 1; // means it was too close to major moon
				else if (testsemi != 1)
					testsemi = 0;
			}
		} while (testsemi == 1);

		//######################################################################################################
			//	ORBIT GENERATION

		if (parent.class_ == L"Jupiter")
		{
			if (moon.semimajorAxis < moon.hillSphereInnerLimit * 3)
			{
				// Set 1
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.001 };
				std::normal_distribution<> geninclination{ 0, 0.01 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 2
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.1, 0.2 };
				std::normal_distribution<> geninclination{ 0, 25 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}
		else if (parent.class_ == L"Neptune")
		{
			if (moon.semimajorAxis < moon.hillSphereInnerLimit * 3)
			{
				// Set 3
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.001 };
				std::normal_distribution<> geninclination{ 0, 0.01 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 4
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.1, 0.2 };
				std::normal_distribution<> geninclination{ 0, 25 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}
		else
		{
			if (moon.semimajorAxis < moon.hillSphereInnerLimit * 2)
			{
				// Set 5
				std::normal_distribution<> genobliquity{ 0, 1 };
				std::normal_distribution<> geneccentricity{ 0, 0.001 };
				std::normal_distribution<> geninclination{ 0, 0.1 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
			else
			{
				// Set 6
				std::normal_distribution<> genobliquity{ 0, 10 };
				std::normal_distribution<> geneccentricity{ 0.2, 0.15 };
				std::normal_distribution<> geninclination{ 0, 40 };

				moon.obliquity = genobliquity(mt_moon);
				do moon.eccentricity = geneccentricity(mt_moon);
				while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
				moon.inclination = geninclination(mt_moon);
			}
		}

		moon.period = (sqrt(pow(km_to_AU(moon.semimajorAxis), 3) / earthMass_to_solMass(moon.parentBody->mass)));
		moon.ascendingNode = gendegree(mt_moon);
		moon.argofPericenter = gendegree(mt_moon);
		moon.meanAnomaly = gendegree(mt_moon);

		// companion orbit
		if (genpercent(mt_planet) < CONFIG.exotic_CompanionOrbitChance)
			moon.hasCompanionOrbit = true;
	}
	void GenerateAsteroid(SEStar& parent, SEPlanet& asteroid, double minSemimajor, double maxSemimajor, double avgSemimajor, double sdSemimajor)
	{
		if (NV.nameAsteroids)
			asteroid.name = GenName(typeAsteroid);
		else
			asteroid.name = L"Asteroid ";

		std::normal_distribution<> genr{ 25, 75 };
		do asteroid.radius = genr(mt_star);
		while (asteroid.radius < 0.05);

		std::normal_distribution<> gensemi{ avgSemimajor, sdSemimajor };
		do asteroid.semimajorAxis = gensemi(mt_star);
		while (asteroid.semimajorAxis < minSemimajor || asteroid.semimajorAxis > maxSemimajor);

		if (genpercent(mt_star) < 1)
		{
			std::normal_distribution<> geneccentricity{ 0.5, 0.2 };
			do asteroid.eccentricity = geneccentricity(mt_star);
			while (asteroid.eccentricity <= 0 || asteroid.eccentricity >= 1);
		}
		else
		{
			std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, CONFIG.SDEccentricity };		
			do asteroid.eccentricity = geneccentricity(mt_star);
			while (asteroid.eccentricity <= 0 || asteroid.eccentricity >= 1);		
		}

		std::normal_distribution<> geninclination{ CONFIG.avgInclination, 20 };
		asteroid.inclination = geninclination(mt_star);
		
		asteroid.period = (sqrt(pow(asteroid.semimajorAxis, 3) / asteroid.parentBody->mass));
		asteroid.obliquity = gendegree(mt_star);
		asteroid.ascendingNode = gendegree(mt_star);
		asteroid.argofPericenter = gendegree(mt_star);
		asteroid.meanAnomaly = gendegree(mt_star);
	}
	void GenerateComet(SEStar& parent, SEPlanet& comet)
	{
		if (NV.nameComets)
			comet.name = GenName(typeComet);
		else
			comet.name = L"Comet ";

		std::normal_distribution<> genr{ 25, 50 };
		do comet.radius = genr(mt_star);
		while (comet.radius < 0.05);

		std::uniform_real_distribution<> gensemi{ parent.innerLimit * 2, parent.outerLimit };
		comet.semimajorAxis = gensemi(mt_star);

		std::normal_distribution<> geneccentricity{ 0.8, 0.1 };
		std::normal_distribution<> geninclination{ CONFIG.avgInclination, 35 };

		comet.obliquity = gendegree(mt_star);
		do comet.eccentricity = geneccentricity(mt_star);
		while (comet.eccentricity <= 0 || comet.eccentricity >= 1);
		comet.inclination = geninclination(mt_star);

		comet.period = (sqrt(pow(comet.semimajorAxis, 3) / comet.parentBody->mass));
		comet.ascendingNode = gendegree(mt_star);
		comet.argofPericenter = gendegree(mt_star);
		comet.meanAnomaly = gendegree(mt_star);
	}

	void ExoticGenerateLife(SEPlanet& body)
	{
		if (body.life_organic.haslife)
		{
			body.life_organic._class = L"Organic";

			if (genpercent(mt_planet) < CONFIG.life_MulticellChance)
				body.life_organic.type = L"Multicellular";
			else
				body.life_organic.type = L"Unicellular";

			if (genpercent(mt_planet) < 50)
				body.life_organic.panspermia = true;
		}

		if (body.life_exotic.haslife)
		{
			body.life_exotic._class = L"Exotic";

			if (genpercent(mt_planet) < CONFIG.life_MulticellChance)
				body.life_exotic.type = L"Multicellular";
			else
				body.life_exotic.type = L"Unicellular";

			if (genpercent(mt_planet) < 50)
				body.life_exotic.panspermia = true;
		}
	}
	void ExoticDebrisRing(SEPlanet& parent, SEPlanet& debris, double inclinationationCenter, double inclinationationSD, double DebrisSpread, double SMCenterPoint)
	{
		debris.name = L"Debris ";

		//######################################################################################################
			//	RADIUS GENERATION

		if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune")
		{
			std::uniform_real_distribution<> genr{ 1, 5 };
			debris.radius = genr(mt_moon);
		}
		else
		{
			std::uniform_real_distribution<> genr{ 0.01, 1.5 };
			debris.radius = genr(mt_moon);
		}

		//######################################################################################################
			//	SEMI MAJOR GENERATION

		std::uniform_real_distribution<> gensemi{ SMCenterPoint, SMCenterPoint * DebrisSpread };
		debris.semimajorAxis = gensemi(mt_moon);

		//######################################################################################################
			//	ORBIT GENERATION

		std::normal_distribution<> genobliquity{ 0, 1 };
		std::normal_distribution<> geneccentricity{ 0, 0.001 };
		std::normal_distribution<> geninclination{ inclinationationCenter, inclinationationSD };

		debris.obliquity = genobliquity(mt_moon);
		do debris.eccentricity = geneccentricity(mt_moon);
		while (debris.eccentricity <= 0 || debris.eccentricity >= 1);
		debris.inclination = geninclination(mt_moon);

		debris.period = (sqrt(pow(km_to_AU(debris.semimajorAxis), 3) / earthMass_to_solMass(debris.parentBody->mass)));
		debris.ascendingNode = gendegree(mt_moon);
		debris.argofPericenter = gendegree(mt_moon);
		debris.meanAnomaly = gendegree(mt_moon);
	}
	void GenerateShip(SEShip& ship, double min_dist, double max_dist, double eccent_avg, double eccent_sd, double inclin_avg, double inclin_sd)
	{
		std::uniform_real_distribution<> gensemi{ min_dist, max_dist };
		ship.semimajorAxis = gensemi(mt_ship);

		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		std::normal_distribution<> geneccentricity{ eccent_avg, eccent_sd };
		std::normal_distribution<> geninclinationtion{ inclin_avg, inclin_sd };

		ship.obliquity = genobliquity(mt_ship);
		do ship.eccentricity = geneccentricity(mt_ship);
		while (ship.eccentricity <= 0 || ship.eccentricity >= 1);
		ship.inclination = geninclinationtion(mt_ship);
		ship.ascendingNode = gendegree(mt_ship);
		ship.argofPericenter = gendegree(mt_ship);
		ship.meanAnomaly = gendegree(mt_ship);
	}

	void DebugMessage(std::wstring message, int type, SEObject& parent, double semimajorKm)
	{ 
		switch (type)
		{
		case debug_STAR_LIMITS:
		{
			SEPlanet Debug;
			Debug.type = L"Star";
			Debug.class_ = L"Z";
			Debug.name = message;
			Debug.parentBody = &parent;
			Debug.semimajorAxis = km_to_AU(semimajorKm);
			Debug.eccentricity = Debug.inclination = Debug.ascendingNode = Debug.argofPericenter = Debug.meanAnomaly = Debug.obliquity = 0;
			PrintPlanet(Debug, *DebugFileP);
			break;
		}	
		case debug_PLANET_LIMITS:
		{
			SEShip Debug;
			Debug.type = L"Spacecraft";
			Debug.name = message;
			Debug.parentBody = &parent;
			Debug.semimajorAxis = semimajorKm;
			Debug.eccentricity = Debug.inclination = Debug.ascendingNode = Debug.argofPericenter = Debug.meanAnomaly = Debug.obliquity = 0;
			PrintShip(Debug, *DebugFileP);
			break;
		}
		default:
		{
			SEPlanet Debug;
			Debug.type = L"Moon";
			Debug.class_ = L"Terra";
			Debug.mass = 0.0001;
			Debug.radius = 200;
			Debug.name = message;
			Debug.parentBody = &parent;
			Debug.semimajorAxis = semimajorKm;
			Debug.eccentricity = Debug.inclination = Debug.ascendingNode = Debug.argofPericenter = Debug.meanAnomaly = Debug.obliquity = 0;

			switch (type)
			{
			case debug_INFO:
				PrintDebug(Debug, color_NORMAL);
				break;
			case debug_WARNING:
				PrintDebug(Debug, color_HALFRED);
				break;
			case debug_ERROR:
				PrintDebug(Debug, color_RED);
				break;
			}
			break;
		}
				
		}
	}
	void PrintDebug(SEPlanet& Debug, std::string color)
	{
		*DebugFileP << wstr_to_str(Debug.type) << "\t\t\t\t\t\"" << wstr_to_str(Debug.name) << "\"\n{"
			<< "\n\tParentBody\t\t\t\t\"" << wstr_to_str(Debug.parentBody->name) << "\""
			<< "\n\tClass\t\t\t\t\t\"" << wstr_to_str(Debug.class_) << "\""
			<< "\n\tMass\t\t\t\t\t" << Debug.mass
			<< "\n\tRadius\t\t\t\t\t" << Debug.radius
			<< "\n\tSurface\n\t{\n\t\t"
			<< "ModulateColor\t\t" << color
			<< "\n\t}"
			<< "\n\tOrbit\n\t{\n\t\t"
			<< "RefPlane\t\t\t\"Equator\""
			<< "\n\t\tSemiMajorAxisKm\t\t" << Debug.semimajorAxis
			<< "\n\t\tPeriod\t\t\t\t" << Debug.period
			<< "\n\t\tEccentricity\t\t" << Debug.eccentricity
			<< "\n\t\tInclination\t\t\t" << Debug.inclination
			<< "\n\t\tAscendingNode\t\t" << Debug.ascendingNode
			<< "\n\t\tArgOfPericenter\t\t" << Debug.argofPericenter
			<< "\n\t\tMeanAnomaly\t\t\t" << Debug.meanAnomaly
			<< "\n\t}\n}\n\n";
	}
