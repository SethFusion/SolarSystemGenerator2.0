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

Configuration CONFIG;
std::vector<Preset> preset;
NameVariables NV;
std::vector<NamePreset> npreset;

std::mt19937 mt_star, mt_planet, mt_moon, mt_ship, mt_name;
std::uniform_int_distribution<int> genpercent{ 1, 100 };
std::uniform_real_distribution<> gendegree{ 0, 360 };
enum Object_Type {typeStar = 1, typePlanet = 2, typeMoon = 3, typeDwarfMoon = 4,
	typeShipColony = 5, typeShipInstrument = 6, typeShipSatellite = 7, typeShipStation = 8};
enum Screen {General = 1, System = 2, Life = 3, Ships = 4, Exotic = 5, Advanced = 6};
Screen lastScreen;

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

#######################################################
	*/	void LoadImages();
	void LoadPresets(HWND);
	void LoadNamePresets(HWND);
	void LoadMainScreen(HWND);

	void UpdatePreset(Preset, HWND);
	void UpdateNamePreset(NamePreset, HWND);
	void SavePreset();
	void SaveNamePreset();	

	void Clear_Screen();
	void Load_Screen_General();
	void Load_Screen_SystemPlanet();
	void Load_Screen_Life();
	void Load_Screen_Ships();
	void Load_Screen_Exotic();
	void Load_Screen_Advanced();

	//void WINAPI TBNotifications(WPARAM wParam, HWND hwndTrack, UINT iSelMin, UINT iSelMax);

	void Clear_Advanced();
	void Load_Name_Star();
	void Load_Name_Planet();
	void Load_Name_Moon();
	void Load_Name_DwarfMoon();
	void Load_Name_All_Ship();
	void Load_Name_Colony_Ship();
	void Load_Name_Instrument_Ship();
	void Load_Name_Satellite_Ship();
	void Load_Name_Station_Ship();


	void Load_Name_Dataset();
	void Load_Name_Simple();

	void GetConfigData(HWND);
	void CreateNameVectors(HWND);

	void SetInfoBox(int);
	void SetCheckBoxText(HWND, int); /*
	Window Functions
	//int OpenPreset();
	
#######################################################
	*/	std::wstring GenName(Object_Type);
		std::wstring GenNumberModifier();
		void BeginGenerate(); 
		void SortVector(std::vector<PLANET>&, int, int);
		int Partition(std::vector<PLANET>&, int, int);
		void GenerateStar(STAR&);
		void PrintStar(STAR&, std::ofstream&);
		void PrintPlanet(PLANET&, std::ofstream&);
		void PrintShip(PLANET& ship, std::ofstream& planetFile);
		void GeneratePlanet(STAR&, PLANET&);
		void GenerateDwarfPlanet(STAR&, PLANET&);
		void GenerateMajorMoon(STAR&, PLANET&, PLANET&, int);
		void GenerateMinorMoon(PLANET&, PLANET&, int);
		void GenerateDwarfMinor(PLANET&, PLANET&, int);

		void ExoticGenerateLife(PLANET&);
		void ExoticDebrisRing(PLANET&, PLANET&, double, double, double, double);
		void GenerateShip(PLANET&, double, double, double, double, double, double);
		/*
	Generator Functions

#####################################################*/

		

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
			case BUTTON_SYSTEMPLANET:
				Clear_Screen();
				Load_Screen_SystemPlanet();
				break;
			case BUTTON_LIFE:
				Clear_Screen();
				Load_Screen_Life();
				break;
			case BUTTON_SHIPS:
				Clear_Screen();
				Load_Screen_Ships();
				break;
			case BUTTON_EXOTIC:
				Clear_Screen();
				Load_Screen_Exotic();
				break;
			case BUTTON_ADVANCED:
				Clear_Screen();
				Load_Screen_Advanced();
				break;
			case BUTTON_SAVEPRESET:
				GetConfigData(hWnd);
				SavePreset();
				LoadPresets(hWnd);
				break;
			case BUTTON_UPDATEPRESET:
				wmId = SendMessage(CONFIG.presetDropDown.HANDLE, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				UpdatePreset(preset.at(wmId), hWnd);
				break;
			case BUTTON_GENERATE:
				GetConfigData(hWnd);
				BeginGenerate();
				break;

			case BUTTON_NAME_SAVEPRESET:
				CreateNameVectors(hWnd);
				SaveNamePreset();
				LoadNamePresets(hWnd);
				break;
			case BUTTON_NAME_UPDATE:
				wmId = SendMessage(CONFIG.namePresetDropDown.HANDLE, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				UpdateNamePreset(npreset.at(wmId), hWnd);
				break;
			case BUTTON_NAME_LOAD_VECTORS:
				CreateNameVectors(hWnd);
				break;

			case BUTTON_NAME_STAR:
				Clear_Advanced();
				Load_Name_Star();
				break;
			case BUTTON_NAME_PLANET:
				Clear_Advanced();
				Load_Name_Planet();
				break;
			case BUTTON_NAME_MOON:
				Clear_Advanced();
				Load_Name_Moon();
				break;
			case BUTTON_NAME_DWARFMOON:
				Clear_Advanced();
				Load_Name_DwarfMoon();
				break;
			case BUTTON_NAME_SHIP_ALL:
				Clear_Advanced();
				Load_Name_All_Ship();
				break;
			case BUTTON_NAME_SHIP_COLONY:
				Clear_Advanced();
				Load_Name_Colony_Ship();
				break;
			case BUTTON_NAME_SHIP_INSTRUMENT:
				Clear_Advanced();
				Load_Name_Instrument_Ship();
				break;
			case BUTTON_NAME_SHIP_SATELLITE:
				Clear_Advanced();
				Load_Name_Satellite_Ship();
				break;
			case BUTTON_NAME_SHIP_STATION:
				Clear_Advanced();
				Load_Name_Station_Ship();
				break;
			case BUTTON_NAME_DATASET:
				Clear_Advanced();
				Load_Name_Dataset();
				break;
			case BUTTON_NAME_SIMPLE:
				Clear_Advanced();
				Load_Name_Simple();
				break;





			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
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
			//Load_Screen_SystemPlanet();
			//Load_Screen_Life();
			//Load_Screen_Ships();
			//Load_Screen_Exotic();
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

		DestroyWindow(CONFIG.presetDropDown.HANDLE);
		DestroyWindow(CONFIG.presetDropDown.DESC);
		DestroyWindow(CONFIG.presetDropDown.INFOBUTTON);
		DestroyWindow(CONFIG.presetDropDown.EXTRA);
		CONFIG.presetDropDown.HANDLE = CreateWindowW(L"combobox", L"",
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			760, 120, 200, 500,
			hWnd, NULL, hInst, NULL);
		CONFIG.presetDropDown.DESC = CreateWindowW(L"static", L"Select a Preset:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
			760, 100, 180, 20,
			hWnd, NULL, hInst, NULL);
		CONFIG.presetDropDown.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			942, 102, 16, 16,
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
			Preset temp;

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
			LoadVariableFromFile(Buffer, parse, temp.name);	
			LoadVariableFromFile(Buffer, parse, temp.starOutputFolder);		
			LoadVariableFromFile(Buffer, parse, temp.planetOutputFolder);		
			LoadVariableFromFile(Buffer, parse, temp.seed);	
			LoadVariableFromFile(Buffer, parse, temp.numberOfRuns);	
			LoadVariableFromFile(Buffer, parse, temp.smartPlacement);
			temp.smartPlacementState = (temp.smartPlacement == true) ? L"Enabled" : L"Disabled";	
			LoadVariableFromFile(Buffer, parse, temp.minPlanetNumber);		
			LoadVariableFromFile(Buffer, parse, temp.minDistance);		
			LoadVariableFromFile(Buffer, parse, temp.maxDistance);		
			LoadVariableFromFile(Buffer, parse, temp.planetSpacing);		
			LoadVariableFromFile(Buffer, parse, temp.generateDwarfPlanets);
			temp.generateDwarfPlanetsState = (temp.generateDwarfPlanets == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, temp.dwarfPlanetChance);
			LoadVariableFromFile(Buffer, parse, temp.weightedMoons);
			temp.weightedMoonsState = (temp.weightedMoons == true) ? L"Enabled" : L"Disabled";
			LoadVariableFromFile(Buffer, parse, temp.avgEccentricity);		
			LoadVariableFromFile(Buffer, parse, temp.SDEccentricity);		
			LoadVariableFromFile(Buffer, parse, temp.avgInclination);		
			LoadVariableFromFile(Buffer, parse, temp.SDInclination);		
			LoadVariableFromFile(Buffer, parse, temp.avgObliquity);		
			LoadVariableFromFile(Buffer, parse, temp.SDObliquity);
			LoadVariableFromFile(Buffer, parse, temp.starClassO);		
			LoadVariableFromFile(Buffer, parse, temp.starClassB);		
			LoadVariableFromFile(Buffer, parse, temp.starClassA);		
			LoadVariableFromFile(Buffer, parse, temp.starClassF);		
			LoadVariableFromFile(Buffer, parse, temp.starClassG);		
			LoadVariableFromFile(Buffer, parse, temp.starClassK);		
			LoadVariableFromFile(Buffer, parse, temp.starClassM);		
			LoadVariableFromFile(Buffer, parse, temp.starClassWD);		
			LoadVariableFromFile(Buffer, parse, temp.starClassQ);		
			LoadVariableFromFile(Buffer, parse, temp.starClassX);	
			LoadVariableFromFile(Buffer, parse, temp.life_OrganicChance);		
			LoadVariableFromFile(Buffer, parse, temp.life_ExoticChance);		
			LoadVariableFromFile(Buffer, parse, temp.life_MulticellChance);		
			LoadVariableFromFile(Buffer, parse, temp.forceLife);
			temp.forceLifeState = (temp.forceLife == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, temp.traditionalLife);
			temp.traditonalLifeState = (temp.traditionalLife == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, temp.exotic_ShipChance);		
			LoadVariableFromFile(Buffer, parse, temp.modelsFolder);		
			LoadVariableFromFile(Buffer, parse, temp.shipsNeedLife);
			temp.shipsNeedLifeState = (temp.shipsNeedLife == true) ? L"Enabled" : L"Disabled";		
			LoadVariableFromFile(Buffer, parse, temp.exotic_OrbitChance);		
			LoadVariableFromFile(Buffer, parse, temp.exotic_AxialTiltChance);		
			LoadVariableFromFile(Buffer, parse, temp.exotic_DebrisRingChance);		
			LoadVariableFromFile(Buffer, parse, temp.exotic_CompanionOrbitChance);

			wchar_t holder = Buffer[parse];
			temp.debug = _wtoi(&holder);
			temp.debugState = (temp.debug == true) ? L"Enabled" : L"Disabled";

			preset.push_back(temp);
			SendMessage(CONFIG.presetDropDown.HANDLE, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)preset.at(i).name);
			CloseHandle(presetFile);
		}

		SendMessage(CONFIG.presetDropDown.HANDLE, (UINT)CB_SETCURSEL, (WPARAM)0, NULL);

		CONFIG.presetDropDown.EXTRA = CreateWindowW(L"button", L"Update",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			965, 98, 50, 50,
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

		DestroyWindow(CONFIG.namePresetDropDown.HANDLE);
		DestroyWindow(CONFIG.namePresetDropDown.DESC);
		DestroyWindow(CONFIG.namePresetDropDown.INFOBUTTON);
		DestroyWindow(CONFIG.namePresetDropDown.EXTRA);
		CONFIG.namePresetDropDown.HANDLE = CreateWindowW(L"combobox", L"",
			WS_CHILD | CBS_DROPDOWNLIST,
			760, 240, 200, 500,
			hWnd, NULL, hInst, NULL);
		CONFIG.namePresetDropDown.DESC = CreateWindowW(L"static", L"Select a Name Preset:",
			WS_CHILD | WS_BORDER | SS_CENTER,
			760, 220, 180, 20,
			hWnd, NULL, hInst, NULL);
		CONFIG.namePresetDropDown.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			942, 222, 16, 16,
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
			SendMessage(CONFIG.namePresetDropDown.HANDLE, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)temp.name);
			CloseHandle(presetFile);
		}

		SendMessage(CONFIG.namePresetDropDown.HANDLE, (UINT)CB_SETCURSEL, (WPARAM)0, NULL);

		CONFIG.namePresetDropDown.EXTRA = CreateWindowW(L"button", L"Update",
			WS_CHILD | WS_BORDER,
			965, 218, 50, 50,
			hWnd, (HMENU)BUTTON_NAME_UPDATE, NULL, NULL);
	}

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

		//Loads all the buttons at the top of the page

		/*###############################################################################
			Main Buttons */ {
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

			CONFIG.tabH = CreateWindow(WC_TABCONTROL, L"",
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | TCS_FIXEDWIDTH,
				340, 280, 690, 400,
				hWnd, NULL, hInst, NULL);
			TabCtrl_SetItemSize(CONFIG.tabH, 114, 50);

			tabStruct.pszText = tabGeneral;
			TabCtrl_InsertItem(CONFIG.tabH, 0, &tabStruct);
			tabStruct.pszText = tabSystem;
			TabCtrl_InsertItem(CONFIG.tabH, 1, &tabStruct);
			tabStruct.pszText = tabLife;
			TabCtrl_InsertItem(CONFIG.tabH, 2, &tabStruct);
			tabStruct.pszText = tabShips;
			TabCtrl_InsertItem(CONFIG.tabH, 3, &tabStruct);
			tabStruct.pszText = tabExotic;
			TabCtrl_InsertItem(CONFIG.tabH, 4, &tabStruct);
			tabStruct.pszText = tabAdvanced;
			TabCtrl_InsertItem(CONFIG.tabH, 5, &tabStruct);
			*/
			
			CreateWindowW(L"button", //type
				L"General", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				340, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_GENERAL, NULL, NULL); // Parent & Command
			//System & Planet Button
			CreateWindowW(L"button", //type
				L"System / Planet", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				454, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_SYSTEMPLANET, NULL, NULL); // Parent & Command
			//Life Button
			CreateWindowW(L"button", //type
				L"Life", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				568, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_LIFE, NULL, NULL); // Parent & Command
			//Ships Button
			CreateWindowW(L"button", //type
				L"Ships", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				682, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_SHIPS, NULL, NULL); // Parent & Command
			//Exotic Button
			CreateWindowW(L"button", //type
				L"Exotic Stuff", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				796, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_EXOTIC, NULL, NULL); // Parent & Command
			//Super Advanced Button
			CreateWindowW(L"button", //type
				L"Advanced", //Text Field
				WS_VISIBLE | WS_CHILD | WS_BORDER, // Effects
				910, 0, // Position
				114, 50, // Width & Height
				hWnd, (HMENU)BUTTON_ADVANCED, NULL, NULL); // Parent & Command
		}
		//###############################################################################

		/*###############################################################################
			Info Panel & Generate Button */ {
		//###############################################################################
			CreateWindowW(L"static", L"",
				WS_VISIBLE | WS_CHILD,
				0, 280, 340, 402,
				hWnd, NULL, NULL, NULL);
			CreateWindowW(L"static", L"Info",
				WS_VISIBLE | WS_CHILD,
				150, 300, 40, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.INFO_BOX = CreateWindowW(L"static", L"Welcome to the Science Fiction Solar System Generator!",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
				10, 330, 320, 250,
				hWnd, NULL, NULL, NULL);
			CreateWindowW(L"button", L"Generate",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				50, 600, 240, 50,
				hWnd, (HMENU)BUTTON_GENERATE, NULL, NULL);
		}
		//###############################################################################

		/*###############################################################################
			General Screen Handles */ {
		//###############################################################################
			CONFIG.HEADER_GENERAL = CreateWindowW(L"static", L"General Variables",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			CONFIG.seedH.DESC = CreateWindowW(L"static", L"Seed:",
				WS_CHILD | WS_BORDER,
				370, 100, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.seedH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 100, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.seedH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 102, 16, 16,
				hWnd, (HMENU)IB_SEED, NULL, NULL);

			CONFIG.numberOfRunsH.DESC = CreateWindowW(L"static", L"# of Systems to Generate:",
				WS_CHILD | WS_BORDER,
				370, 120, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.numberOfRunsH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_NUMBER | ES_RIGHT,
				600, 120, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.numberOfRunsH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 122, 16, 16,
				hWnd, (HMENU)IB_NUMBEROFRUNS, NULL, NULL);

			CONFIG.debugH.DESC = CreateWindowW(L"static", L"Debug:",
				WS_CHILD | WS_BORDER,
				370, 140, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.debugH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 140, 100, 20,
				hWnd, (HMENU)CB_DEBUG, NULL, NULL);
			CONFIG.debugH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 142, 16, 16,
				hWnd, (HMENU)IB_DEBUG, NULL, NULL);

			//star output folder
			CONFIG.starOutputFolderH.DESC = CreateWindowW(L"static", L"Star Output Folder:",
				WS_CHILD | WS_BORDER,
				370, 160, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.starOutputFolderH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				470, 160, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.starOutputFolderH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 162, 16, 16,
				hWnd, (HMENU)IB_STAROUTPUTFOLDER, NULL, NULL);

			//planet output folder
			CONFIG.planetOutputFolderH.DESC = CreateWindowW(L"static", L"Planet Output Folder:",
				WS_CHILD | WS_BORDER,
				370, 180, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.planetOutputFolderH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				470, 180, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.planetOutputFolderH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 182, 16, 16,
				hWnd, (HMENU)IB_PLANETOUTPUTFOLDER, NULL, NULL);


			//Preset handles are in the Load Presets function!

			CONFIG.savePresetButton.DESC = CreateWindowW(L"static", L"Save a Preset...",
				WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
				760, 160, 180, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.savePresetButton.HANDLE = CreateWindowW(L"edit", L"New Preset",
				WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
				760, 180, 200, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.savePresetButton.EXTRA = CreateWindowW(L"button", L"Save",
				WS_CHILD | WS_VISIBLE | WS_BORDER,
				965, 158, 50, 50,
				hWnd, (HMENU)BUTTON_SAVEPRESET, NULL, NULL);
			CONFIG.savePresetButton.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				942, 162, 16, 16,
				hWnd, (HMENU)IB_SAVEPRESET, NULL, NULL);
		}
		//###############################################################################

		/*###############################################################################
			System and Planet Screen Handles */ {
		//###############################################################################
			CONFIG.HEADER_SYSTEMPLANET = CreateWindowW(L"static", L"System and Planet Variables",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			//smart placement
			CONFIG.smartPlacementH.DESC = CreateWindowW(L"static", L"Smart Placement:",
				WS_CHILD | WS_BORDER,
				370, 100, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.smartPlacementH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 100, 100, 20,
				hWnd, (HMENU)CB_SMARTPLACEMENT, NULL, NULL);
			CONFIG.smartPlacementH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 102, 16, 16,
				hWnd, (HMENU)IB_SMARTPLACEMENT, NULL, NULL);

			//min and max distance
			CONFIG.minDistanceH.DESC = CreateWindowW(L"static", L"Minimum Distance:",
				WS_CHILD | WS_BORDER,
				370, 120, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.minDistanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 120, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.maxDistanceH.DESC = CreateWindowW(L"static", L"Maximum Distance:",
				WS_CHILD | WS_BORDER,
				370, 140, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.maxDistanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 140, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.maxDistanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 132, 16, 16,
				hWnd, (HMENU)IB_DISTANCE, NULL, NULL);

			//generate dwarf planets
			CONFIG.generateDwarfPlanetsH.DESC = CreateWindowW(L"static", L"Generate Dwarf Planets:",
				WS_CHILD | WS_BORDER,
				370, 300, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.generateDwarfPlanetsH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 300, 100, 20,
				hWnd, (HMENU)CB_GENERATEDWARFPLANET, NULL, NULL);
			CONFIG.generateDwarfPlanetsH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 302, 16, 16,
				hWnd, (HMENU)IB_GENERATEDWARFPLANET, NULL, NULL);

			//weighted moons
			CONFIG.weightedMoonsH.DESC = CreateWindowW(L"static", L"Weighted Moons:",
				WS_CHILD | WS_BORDER,
				370, 400, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.weightedMoonsH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 400, 100, 20,
				hWnd, (HMENU)CB_WEIGHTEDMOONS, NULL, NULL);
			CONFIG.weightedMoonsH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 402, 16, 16,
				hWnd, (HMENU)IB_WEIGHTEDMOONS, NULL, NULL);

			//min planet number
			CONFIG.dwarfPlanetChanceH.DESC = CreateWindowW(L"static", L"Dwarf Planet % Chance:",
				WS_CHILD | WS_BORDER,
				370, 320, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.dwarfPlanetChanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 320, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.dwarfPlanetChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 322, 16, 16,
				hWnd, (HMENU)IB_DWARFPALNETCHANCE, NULL, NULL);

			//min planet number
			CONFIG.minPlanetNumberH.DESC = CreateWindowW(L"static", L"Minimum # of Planets:",
				WS_CHILD | WS_BORDER,
				370, 500, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.minPlanetNumberH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 500, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.minPlanetNumberH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 502, 16, 16,
				hWnd, (HMENU)IB_MINPLANETNUMBER, NULL, NULL);

			//obliquity stuff
			CONFIG.avgObliquityH.DESC = CreateWindowW(L"static", L"Axial Tilt Average:",
				WS_CHILD | WS_BORDER,
				370, 170, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.avgObliquityH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 170, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDObliquityH.DESC = CreateWindowW(L"static", L"Axial Tilt Standard Deviation:",
				WS_CHILD | WS_BORDER,
				370, 190, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDObliquityH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 190, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDObliquityH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 182, 16, 16,
				hWnd, (HMENU)IB_OBLIQUITY, NULL, NULL);

			//inclination stuff
			CONFIG.avgInclinationH.DESC = CreateWindowW(L"static", L"Inclination Average:",
				WS_CHILD | WS_BORDER,
				370, 211, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.avgInclinationH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 211, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDInclinationH.DESC = CreateWindowW(L"static", L"Inclination Standard Deviation:",
				WS_CHILD | WS_BORDER,
				370, 231, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDInclinationH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 231, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDInclinationH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 223, 16, 16,
				hWnd, (HMENU)IB_INCLINATION, NULL, NULL);

			//eccentricity stuff
			CONFIG.avgEccentricityH.DESC = CreateWindowW(L"static", L"Eccentricity Average:",
				WS_CHILD | WS_BORDER,
				370, 252, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.avgEccentricityH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 252, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDEccentricityH.DESC = CreateWindowW(L"static", L"Eccentricity Standard Deviation:",
				WS_CHILD | WS_BORDER,
				370, 272, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDEccentricityH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 272, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.SDEccentricityH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 264, 16, 16,
				hWnd, (HMENU)IB_ECCENTRICITY, NULL, NULL);

			// Trackbar stuff
			/*
			CONFIG.avgEccentricityH.HANDLE = CreateWindowW(TRACKBAR_CLASS, L"Track Test",
				WS_CHILD | WS_BORDER | TBS_NOTICKS,
				600, 400, 100, 20,
				hWnd, NULL, NULL, NULL);
			SendMessage(CONFIG.avgEccentricityH.HANDLE, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(1, 9999));
			SendMessage(CONFIG.avgEccentricityH.HANDLE, TBM_SETPAGESIZE, 0, (LPARAM)100);
			CONFIG.avgEccentricityH.EXTRA = CreateWindowW(L"static", L"",
				WS_CHILD | WS_VISIBLE | SS_RIGHT,
				0, 0, 70, 20,
				hWnd, NULL, NULL, NULL);
			SendMessage(CONFIG.avgEccentricityH.HANDLE, TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)CONFIG.avgEccentricityH.EXTRA);
			*/

			//Star class Stuff
			//I used the O class's EXTRA to create the groupbox for all the classes,
			//therefore, it is the parent for all the class checkboxes
			CONFIG.starClassOH.EXTRA = CreateWindowW(L"button", L"Star Class Weights:",
				WS_CHILD | BS_GROUPBOX,
				750, 100, 250, 170,
				hWnd, NULL, NULL, NULL);
			CONFIG.starClassOH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				960, 102, 16, 16,
				hWnd, (HMENU)IB_STARCLASS, NULL, NULL);


			CONFIG.starClassOH.DESC = CreateWindowW(L"static", L"O:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 20, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassOH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 20, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassBH.DESC = CreateWindowW(L"static", L"B:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 40, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassBH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 40, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassAH.DESC = CreateWindowW(L"static", L"A:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 60, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassAH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 60, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassFH.DESC = CreateWindowW(L"static", L"F:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 80, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassFH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 80, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassGH.DESC = CreateWindowW(L"static", L"G:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 100, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassGH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 100, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassKH.DESC = CreateWindowW(L"static", L"K:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 120, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassKH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 120, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassMH.DESC = CreateWindowW(L"static", L"M:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				10, 140, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassMH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				50, 140, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassWDH.DESC = CreateWindowW(L"static", L"WD:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				100, 20, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassWDH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				140, 20, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassQH.DESC = CreateWindowW(L"static", L"Q:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				100, 40, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassQH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				140, 40, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);

			CONFIG.starClassXH.DESC = CreateWindowW(L"static", L"X:",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP,
				100, 60, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
			CONFIG.starClassXH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_VISIBLE | WS_BORDER | WS_GROUP | ES_AUTOHSCROLL | ES_RIGHT,
				140, 60, 40, 20,
				CONFIG.starClassOH.EXTRA, NULL, NULL, NULL);
		}
		//###############################################################################

		/*###############################################################################
			Life Screen Handles  */ 
		//###############################################################################
			CONFIG.HEADER_LIFE = CreateWindowW(L"static", L"Life Variables",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			//Organic Chance
			CONFIG.life_OrganicChanceH.DESC = CreateWindowW(L"static", L"% Chance for Organic Life:",
				WS_CHILD | WS_BORDER,
				370, 100, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_OrganicChanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 100, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_OrganicChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 102, 16, 16,
				hWnd, (HMENU)IB_LIFEORGANICCHANCE, NULL, NULL);

			//Exotic Chance
			CONFIG.life_ExoticChanceH.DESC = CreateWindowW(L"static", L"% Chance for Exotic Life:",
				WS_CHILD | WS_BORDER,
				370, 120, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_ExoticChanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 120, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_ExoticChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 122, 16, 16,
				hWnd, (HMENU)IB_LIFEEXOTICCHANCE, NULL, NULL);

			//MultiCell Chance
			CONFIG.life_MulticellChanceH.DESC = CreateWindowW(L"static", L"% Chance for Multicellular Life:",
				WS_CHILD | WS_BORDER,
				370, 140, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_MulticellChanceH.HANDLE = CreateWindowW(L"edit", L"",
				WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
				600, 140, 100, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.life_MulticellChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 142, 16, 16,
				hWnd, (HMENU)IB_LIFEMULTICHANCE, NULL, NULL);

			//Traditional Life
			CONFIG.traditionalLifeH.DESC = CreateWindowW(L"static", L"Traditional Life:",
				WS_CHILD | WS_BORDER,
				370, 160, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.traditionalLifeH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 160, 100, 20,
				hWnd, (HMENU)CB_TRADITIONALLIFE, NULL, NULL);
			CONFIG.traditionalLifeH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 162, 16, 16,
				hWnd, (HMENU)IB_TRADITIONALLIFE, NULL, NULL);

			//Force Life
			CONFIG.forceLifeH.DESC = CreateWindowW(L"static", L"Force Life:",
				WS_CHILD | WS_BORDER,
				370, 180, 230, 20,
				hWnd, NULL, NULL, NULL);
			CONFIG.forceLifeH.HANDLE = CreateWindowW(L"button", L"",
				WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
				600, 180, 100, 20,
				hWnd, (HMENU)CB_FORCELIFE, NULL, NULL);
			CONFIG.forceLifeH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				702, 182, 16, 16,
				hWnd, (HMENU)IB_FORCELIFE, NULL, NULL);
		
		//###############################################################################

		/*###############################################################################
			Ships Screen Handles */ {
		//###############################################################################

		CONFIG.HEADER_SHIPS = CreateWindowW(L"static", L"Ship Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//Ship Chance
		CONFIG.exotic_ShipChanceH.DESC = CreateWindowW(L"static", L"% Chance for Ships to Spawn:",
			WS_CHILD | WS_BORDER,
			370, 100, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_ShipChanceH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 100, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_ShipChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 102, 16, 16,
			hWnd, (HMENU)IB_SHIPCHANCE, NULL, NULL);

		//Models folder
		CONFIG.modelsFolderH.DESC = CreateWindowW(L"static", L"Models Folder Path:",
			WS_CHILD | WS_BORDER,
			370, 120, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.modelsFolderH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			470, 120, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.modelsFolderH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 122, 16, 16,
			hWnd, (HMENU)IB_MODELSFOLDER, NULL, NULL);

		//Ships Need Life
		CONFIG.shipsNeedLifeH.DESC = CreateWindowW(L"static", L"Ships Need Life to Spawn:",
			WS_CHILD | WS_BORDER,
			370, 140, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.shipsNeedLifeH.HANDLE = CreateWindowW(L"button", L"",
			WS_CHILD | WS_BORDER | BS_AUTOCHECKBOX | BS_RIGHTBUTTON,
			600, 140, 100, 20,
			hWnd, (HMENU)CB_SHIPSNEEDLIFE, NULL, NULL);
		CONFIG.shipsNeedLifeH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 142, 16, 16,
			hWnd, (HMENU)IB_SHIPSNEEDLIFE, NULL, NULL);
		}
		//###############################################################################

		/*###############################################################################
			Exotic Screen Handles */ {
		//###############################################################################

		CONFIG.HEADER_EXOTIC = CreateWindowW(L"static", L"Exotic Variables",
			WS_CHILD | WS_BORDER | ES_CENTER, //Effects
			350, 60, 664, 30, //X, Y, Width & Height
			hWnd, NULL, NULL, NULL); //Parent

		//Exotic Orbit Chance
		CONFIG.exotic_OrbitChanceH.DESC = CreateWindowW(L"static", L"% Chance for Exotic Orbit:",
			WS_CHILD | WS_BORDER,
			370, 100, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_OrbitChanceH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 100, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_OrbitChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 102, 16, 16,
			hWnd, (HMENU)IB_EXOTICORBIT, NULL, NULL);

		//Exotic Obliquity Chance
		CONFIG.exotic_AxialTiltChanceH.DESC = CreateWindowW(L"static", L"% Chance for Exotic Axial Tilt:",
			WS_CHILD | WS_BORDER,
			370, 120, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_AxialTiltChanceH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 120, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_AxialTiltChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 122, 16, 16,
			hWnd, (HMENU)IB_EXOTICOBLIQUITY, NULL, NULL);

		//Exotic Companion Chance
		CONFIG.exotic_CompanionOrbitChanceH.DESC = CreateWindowW(L"static", L"% Chance for Comapnion Orbit:",
			WS_CHILD | WS_BORDER,
			370, 140, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_CompanionOrbitChanceH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 140, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_CompanionOrbitChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 142, 16, 16,
			hWnd, (HMENU)IB_EXOTICCOMPANION, NULL, NULL);

		//Exotic Debris Ring Chance
		CONFIG.exotic_DebrisRingChanceH.DESC = CreateWindowW(L"static", L"% Chance for Debris Ring:",
			WS_CHILD | WS_BORDER,
			370, 160, 230, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_DebrisRingChanceH.HANDLE = CreateWindowW(L"edit", L"",
			WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_RIGHT,
			600, 160, 100, 20,
			hWnd, NULL, NULL, NULL);
		CONFIG.exotic_DebrisRingChanceH.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_BORDER,
			702, 162, 16, 16,
			hWnd, (HMENU)IB_EXOTICDEBRISRING, NULL, NULL);
		}
		//###############################################################################

		/*###############################################################################
			Advanced Screen Handles */ {
		//###############################################################################

			CONFIG.HEADER_ADVANCED = CreateWindowW(L"static", L"Advanced Variables:",
				WS_CHILD | WS_BORDER | ES_CENTER, //Effects
				350, 60, 664, 30, //X, Y, Width & Height
				hWnd, NULL, NULL, NULL); //Parent

			/*###############################################################################
				Name Stuff */
			//###############################################################################
			{
				// groupbox to hold the buttons
				CONFIG.advNameGroup.EXTRA = CreateWindowW(L"button", L"Name Variables:",
					WS_CHILD | WS_GROUP | BS_GROUPBOX,
					370, 100, 260, 130,
					hWnd, NULL, hInst, NULL);

				// save preset stuff
				CONFIG.saveNamePresetButton.DESC = CreateWindowW(L"static", L"Save Name Preset...",
					WS_CHILD  | WS_BORDER,
					630, 110, 140, 20,
					hWnd, NULL, NULL, NULL);
				CONFIG.saveNamePresetButton.HANDLE = CreateWindowW(L"edit", L"New Name Preset",
					WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
					630, 130, 160, 20,
					hWnd, NULL, NULL, NULL);
				CONFIG.saveNamePresetButton.EXTRA = CreateWindowW(L"button", L"Save",
					WS_CHILD | WS_BORDER,
					632, 152, 156, 20,
					hWnd, (HMENU)BUTTON_NAME_SAVEPRESET, NULL, NULL);
				CONFIG.saveNamePresetButton.INFOBUTTON = CreateWindowW(L"button", L"I",
					WS_CHILD | WS_BORDER,
					772, 112, 16, 16,
					hWnd, (HMENU)IB_SAVENAMEPRESET, NULL, NULL);

				// all of the buttons for the name groups
				// the buttons are only VISUALLY inside the groupbox, but their
				// parent is still hWnd because they send messages to the
				// window procedure
				CONFIG.buttonUpdate = CreateWindowW(L"button", L"Update Names",
					WS_CHILD | WS_BORDER | BS_MULTILINE,
					550, 120, 70, 50,
					hWnd, (HMENU)BUTTON_NAME_LOAD_VECTORS, NULL, NULL);
				CONFIG.buttonStar = CreateWindowW(L"button", L"Star",
					WS_CHILD | WS_BORDER,
					380, 120, 50, 30,
					hWnd, (HMENU)BUTTON_NAME_STAR, NULL, NULL);
				CONFIG.buttonPlanet = CreateWindowW(L"button", L"Planet",
					WS_CHILD | WS_BORDER,
					430, 120, 50, 30,
					hWnd, (HMENU)BUTTON_NAME_PLANET, NULL, NULL);
				CONFIG.buttonMoon = CreateWindowW(L"button", L"Moon",
					WS_CHILD | WS_BORDER,
					480, 120, 50, 30,
					hWnd, (HMENU)BUTTON_NAME_MOON, NULL, NULL);
				CONFIG.buttonDwarfMoon = CreateWindowW(L"button", L"Dwarf Moon",
					WS_CHILD | WS_BORDER,
					380, 150, 90, 30,
					hWnd, (HMENU)BUTTON_NAME_DWARFMOON, NULL, NULL);
				CONFIG.buttonShip = CreateWindowW(L"button", L"Ship",
					WS_CHILD | WS_BORDER,
					470, 150, 50, 30,
					hWnd, (HMENU)BUTTON_NAME_SHIP_ALL, NULL, NULL);
				CONFIG.buttonDataset = CreateWindowW(L"button", L"Name Dataset",
					WS_CHILD | WS_BORDER,
					380, 180, 110, 30,
					hWnd, (HMENU)BUTTON_NAME_DATASET, NULL, NULL);
				CONFIG.buttonSimple = CreateWindowW(L"button", L"Simple Generator",
					WS_CHILD | WS_BORDER,
					490, 180, 130, 30,
					hWnd, (HMENU)BUTTON_NAME_SIMPLE, NULL, NULL);

				// Info buttons for the pre/post/number mods, which are the same for
				// each name group
				NV.PreMod_INFO.DESC = CreateWindowW(L"static", L"Pre Modification Description:",
					WS_CHILD | WS_BORDER,
					760, 260, 220, 20,
					hWnd, NULL, NULL, NULL);
				NV.PreMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"I",
					WS_CHILD | WS_BORDER,
					962, 262, 16, 16,
					hWnd, (HMENU)IB_NAMEPREMODS, NULL, NULL);
				NV.PostMod_INFO.DESC = CreateWindowW(L"static", L"Post Modification Description:",
					WS_CHILD | WS_BORDER,
					760, 280, 220, 20,
					hWnd, NULL, NULL, NULL);
				NV.PostMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"I",
					WS_CHILD | WS_BORDER,
					962, 282, 16, 16,
					hWnd, (HMENU)IB_NAMEPOSTMODS, NULL, NULL);
				NV.NumberMod_INFO.DESC = CreateWindowW(L"static", L"Num Modification Description:",
					WS_CHILD | WS_BORDER,
					760, 300, 220, 20,
					hWnd, NULL, NULL, NULL);
				NV.NumberMod_INFO.INFOBUTTON = CreateWindowW(L"button", L"I",
					WS_CHILD | WS_BORDER,
					962, 302, 16, 16,
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

				/*###############################################################################
					Star Group
				###############################################################################*/
				{
					NV.GROUP_STAR = CreateWindowW(L"button", L"Star Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useStarModsDESC = CreateWindowW(L"static", L"Use Star Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.useStarPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARPREMOD, NULL, NULL);
					NV.useStarPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARPOSTMOD, NULL, NULL);
					NV.useStarNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_STAR, (HMENU)NVCB_STARNUMBERMOD, NULL, NULL);

					NV.starModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.probStarNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_STAR, NULL, NULL, NULL);

					NV.starPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_STAR, NULL, NULL, NULL);
					NV.starPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_STAR, NULL, NULL, NULL);
				}
				/*###############################################################################
					Planet Group
				###############################################################################*/
				{
					NV.GROUP_PLANET = CreateWindowW(L"button", L"Planet Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.usePlanetModsDESC = CreateWindowW(L"static", L"Use Planet Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.usePlanetPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETPREMOD, NULL, NULL);
					NV.usePlanetPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETPOSTMOD, NULL, NULL);
					NV.usePlanetNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_PLANET, (HMENU)NVCB_PLANETNUMBERMOD, NULL, NULL);

					NV.planetModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.probPlanetNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_PLANET, NULL, NULL, NULL);

					NV.planetPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_PLANET, NULL, NULL, NULL);
					NV.planetPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_PLANET, NULL, NULL, NULL);
				}
				/*###############################################################################
					Moon Group
				###############################################################################*/
				{
					NV.GROUP_MOON = CreateWindowW(L"button", L"Moon Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameMoonsH.DESC = CreateWindowW(L"static", L"Name Terra / Gas Giant Moons:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 20, 300, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.nameMoonsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 20, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_NAMETERRAMOONS, NULL, NULL);
					NV.nameMoonsH.EXTRA = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						330, 20, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_NAMEGASMOONS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useMoonModsDESC = CreateWindowW(L"static", L"Use Moon Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.useMoonPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONPREMOD, NULL, NULL);
					NV.useMoonPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONPOSTMOD, NULL, NULL);
					NV.useMoonNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_MOON, (HMENU)NVCB_MOONNUMBERMOD, NULL, NULL);

					NV.moonModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.probMoonNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_MOON, NULL, NULL, NULL);

					NV.moonPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_MOON, NULL, NULL, NULL);
					NV.moonPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_MOON, NULL, NULL, NULL);
				}
				/*###############################################################################
					Dwarf Moon Group
				###############################################################################*/
				{
					NV.GROUP_DWARFMOON = CreateWindowW(L"button", L"Dwarf Moon Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.nameDwarfMoonsH.DESC = CreateWindowW(L"static", L"Name Terra / Gas Giant Dwarf Moons:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 20, 300, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.nameDwarfMoonsH.HANDLE = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 20, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_NAMETERRADWARFMOONS, NULL, NULL);
					NV.nameDwarfMoonsH.EXTRA = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						330, 20, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_NAMEGASDWARFMOONS, NULL, NULL);
					NV.nameMoonsH.INFOBUTTON = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						662, 262, 16, 16,
						hWnd, (HMENU)IB_NAMEMOONS, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useDwarfMoonModsDESC = CreateWindowW(L"static", L"Use Dwarf Moon Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.useDwarfMoonPreModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONPREMOD, NULL, NULL);
					NV.useDwarfMoonPostModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONPOSTMOD, NULL, NULL);
					NV.useDwarfMoonNumberModsH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_DWARFMOON, (HMENU)NVCB_DWARFMOONNUMBERMOD, NULL, NULL);

					NV.dwarfMoonModsProbDESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonPreModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonPostModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.probDwarfMoonNumberModH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);

					NV.dwarfMoonPreModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
					NV.dwarfMoonPostModList = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_DWARFMOON, NULL, NULL, NULL);
				}
				/*###############################################################################
					Ship_All Group
				###############################################################################*/
				{
					NV.GROUP_ALL_SHIP = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_All_DESC = CreateWindowW(L"static", L"Use Pre / Post Mods for All Ships:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);
					NV.useShipPreMods_AllH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_ALL_SHIP, (HMENU)NVCB_SHIPALLPREMOD, NULL, NULL);
					NV.useShipPostMods_AllH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_ALL_SHIP, (HMENU)NVCB_SHIPALLPOSTMOD, NULL, NULL);

					NV.shipModsProb_All_DESC = CreateWindowW(L"static", L"Probability for Pre / Post Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);
					NV.probShipPreMod_AllH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);
					NV.probShipPostMod_AllH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);

					NV.shipPreModList_All = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);
					NV.shipPostModList_All = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_ALL_SHIP, NULL, NULL, NULL);
				}
				/*###############################################################################
					Ship_Colony Group
				###############################################################################*/
				{
					NV.GROUP_COLONY_SHIP = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_Colony_DESC = CreateWindowW(L"static", L"Use Colony Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
					NV.useShipPreMods_ColonyH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_COLONY_SHIP, (HMENU)NVCB_SHIPCOLONYPREMOD, NULL, NULL);
					NV.useShipPostMods_ColonyH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_COLONY_SHIP, (HMENU)NVCB_SHIPCOLONYPOSTMOD, NULL, NULL);
					NV.useShipNumberMods_ColonyH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_COLONY_SHIP, (HMENU)NVCB_SHIPCOLONYNUMBERMOD, NULL, NULL);

					NV.shipModsProb_Colony_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
					NV.probShipPreMod_ColonyH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
					NV.probShipPostMod_ColonyH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
					NV.probShipNumberMod_ColonyH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);

					NV.shipPreModList_Colony = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
					NV.shipPostModList_Colony = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_COLONY_SHIP, NULL, NULL, NULL);
				}
				/*###############################################################################
					Ship_Instrument Group
				###############################################################################*/
				{
					NV.GROUP_INSTRUMENT_SHIP = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_Instrument_DESC = CreateWindowW(L"static", L"Use Instrument Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
					NV.useShipPreMods_InstrumentH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_INSTRUMENT_SHIP, (HMENU)NVCB_SHIPINSTRUMENTPREMOD, NULL, NULL);
					NV.useShipPostMods_InstrumentH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_INSTRUMENT_SHIP, (HMENU)NVCB_SHIPINSTRUMENTPOSTMOD, NULL, NULL);
					NV.useShipNumberMods_InstrumentH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_INSTRUMENT_SHIP, (HMENU)NVCB_SHIPINSTRUMENTNUMBERMOD, NULL, NULL);

					NV.shipModsProb_Instrument_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
					NV.probShipPreMod_InstrumentH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
					NV.probShipPostMod_InstrumentH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
					NV.probShipNumberMod_InstrumentH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);

					NV.shipPreModList_Instrument = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
					NV.shipPostModList_Instrument = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_INSTRUMENT_SHIP, NULL, NULL, NULL);
				}
				/*###############################################################################
					Ship_Satellite Group
				###############################################################################*/
				{
					NV.GROUP_SATELLITE_SHIP = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_Satellite_DESC = CreateWindowW(L"static", L"Use Satellite Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
					NV.useShipPreMods_SatelliteH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_SATELLITE_SHIP, (HMENU)NVCB_SHIPSATELLITEPREMOD, NULL, NULL);
					NV.useShipPostMods_SatelliteH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_SATELLITE_SHIP, (HMENU)NVCB_SHIPSATELLITEPOSTMOD, NULL, NULL);
					NV.useShipNumberMods_SatelliteH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_SATELLITE_SHIP, (HMENU)NVCB_SHIPSATELLITENUMBERMOD, NULL, NULL);

					NV.shipModsProb_Satellite_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
					NV.probShipPreMod_SatelliteH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
					NV.probShipPostMod_SatelliteH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
					NV.probShipNumberMod_SatelliteH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);

					NV.shipPreModList_Satellite = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
					NV.shipPostModList_Satellite = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_SATELLITE_SHIP, NULL, NULL, NULL);
				}
				/*###############################################################################
					Ship_Station Group
				###############################################################################*/
				{

					NV.GROUP_STATION_SHIP = CreateWindowW(L"button", L"Ship Name Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					// These handes are initialized as visible because they are
					// shown/hiden with the group
					NV.useShipMods_Station_DESC = CreateWindowW(L"static", L"Use Station Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 40, 300, 20,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);
					NV.useShipPreMods_StationH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX | BS_CENTER,
						310, 40, 20, 20,
						NV.GROUP_STATION_SHIP, (HMENU)NVCB_SHIPSTATIONPREMOD, NULL, NULL);
					NV.useShipPostMods_StationH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						330, 40, 20, 20,
						NV.GROUP_STATION_SHIP, (HMENU)NVCB_SHIPSTATIONPOSTMOD, NULL, NULL);
					NV.useShipNumberMods_StationH = CreateWindowW(L"button", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | BS_AUTOCHECKBOX,
						350, 40, 20, 20,
						NV.GROUP_STATION_SHIP, (HMENU)NVCB_SHIPSTATIONNUMBERMOD, NULL, NULL);

					NV.shipModsProb_Station_DESC = CreateWindowW(L"static", L"Probability for Pre / Post / Number Mods:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						10, 60, 300, 20,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);
					NV.probShipPreMod_StationH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						10, 80, 40, 20,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);
					NV.probShipPostMod_StationH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						50, 80, 40, 20,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);
					NV.probShipNumberMod_StationH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
						90, 80, 40, 20,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);

					NV.shipPreModList_Station = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 130, 296, 266,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);
					NV.shipPostModList_Station = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						312, 130, 296, 266,
						NV.GROUP_STATION_SHIP, NULL, NULL, NULL);

					// These buttons load with the ship page, but will load their own page for storeing different data
					NV.BUTTON_COLONY = CreateWindowW(L"button", L"Colony",
						WS_CHILD | WS_BORDER,
						550, 238, 80, 20,
						hWnd, (HMENU)BUTTON_NAME_SHIP_COLONY, NULL, NULL);
					NV.BUTTON_INSTRUMENT = CreateWindowW(L"button", L"Instrument",
						WS_CHILD | WS_BORDER,
						650, 238, 80, 20,
						hWnd, (HMENU)BUTTON_NAME_SHIP_INSTRUMENT, NULL, NULL);
					NV.BUTTON_SATELLITE = CreateWindowW(L"button", L"Satellite",
						WS_CHILD | WS_BORDER,
						750, 238, 80, 20,
						hWnd, (HMENU)BUTTON_NAME_SHIP_SATELLITE, NULL, NULL);
					NV.BUTTON_STATION = CreateWindowW(L"button", L"Station",
						WS_CHILD | WS_BORDER,
						850, 238, 80, 20,
						hWnd, (HMENU)BUTTON_NAME_SHIP_STATION, NULL, NULL);
				}
				/*###############################################################################
					Dataset Group
				###############################################################################*/
				{
					NV.GROUP_DATASET = CreateWindowW(L"button", L"Markov Variables:",
						WS_CHILD | WS_GROUP | BS_GROUPBOX,
						370, 240, 620, 410,
						hWnd, NULL, NULL, NULL);

					NV.Markov_INFO = CreateWindowW(L"button", L"I",
						WS_CHILD | WS_BORDER,
						500, 242, 16, 16,
						hWnd, (HMENU)IB_MARKOV, NULL, NULL);

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
						300, 20, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.max_lengthDESC = CreateWindowW(L"static", L"Max Length:",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						300, 40, 120, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);

					NV.orderH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
						130, 20, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.wordVarienceH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
						130, 40, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.min_lengthH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
						420, 20, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.max_lengthH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT,
						420, 40, 50, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);

					NV.Markov_RawDatasetDESC = CreateWindowW(L"static", L"Dataset:",
						WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
						10, 80, 600, 20,
						NV.GROUP_DATASET, NULL, NULL, NULL);
					NV.Markov_RawDatasetH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL,
						12, 100, 596, 300,
						NV.GROUP_DATASET, NULL, NULL, NULL);
				}
				/*###############################################################################
					Simple Generator Group
				###############################################################################*/
				{
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

				}
			}
		}
		//###############################################################################



		// Page Edges
		CreateWindowW(L"button", L"Bottom Most Edge",
			WS_VISIBLE | WS_CHILD,
			0, 681, 684, 50,
			hWnd, NULL, NULL, NULL);
		CreateWindowW(L"button", L"Right Most Edge",
			WS_VISIBLE | WS_CHILD,
			1024, 0, 200, 50,
			hWnd, NULL, NULL, NULL);
	}


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
		SetWindowTextW(CONFIG.seedH.HANDLE, seedarr);





		SetWindowTextW(CONFIG.starOutputFolderH.HANDLE, P.starOutputFolder);
		SetWindowTextW(CONFIG.planetOutputFolderH.HANDLE, P.planetOutputFolder);
		SetVariableToWindow(CONFIG.numberOfRunsH.HANDLE, P.numberOfRuns);
		CheckDlgButton(hWnd, CB_DEBUG, P.debug);
		SetWindowTextW(CONFIG.debugH.HANDLE, P.debugState);

		CheckDlgButton(hWnd, CB_SMARTPLACEMENT, P.smartPlacement);
		SetWindowTextW(CONFIG.smartPlacementH.HANDLE, P.smartPlacementState);
		SetVariableToWindow(CONFIG.minPlanetNumberH.HANDLE, P.minPlanetNumber);

		SetVariableToWindow(CONFIG.minDistanceH.HANDLE, P.minDistance);
		SetVariableToWindow(CONFIG.maxDistanceH.HANDLE, P.maxDistance);
		SetVariableToWindow(CONFIG.planetSpacingH.HANDLE, P.planetSpacing);
		CheckDlgButton(hWnd, CB_GENERATEDWARFPLANET, P.generateDwarfPlanets);
		SetWindowTextW(CONFIG.generateDwarfPlanetsH.HANDLE, P.generateDwarfPlanetsState);
		CheckDlgButton(hWnd, CB_WEIGHTEDMOONS, P.weightedMoons);
		SetWindowTextW(CONFIG.weightedMoonsH.HANDLE, P.weightedMoonsState);
		SetVariableToWindow(CONFIG.dwarfPlanetChanceH.HANDLE, P.dwarfPlanetChance);
		SetVariableToWindow(CONFIG.avgEccentricityH.HANDLE, P.avgEccentricity);
		SetVariableToWindow(CONFIG.SDEccentricityH.HANDLE, P.SDEccentricity); 
		SetVariableToWindow(CONFIG.avgInclinationH.HANDLE, P.avgInclination); // change to extra for trackbar
		SetVariableToWindow(CONFIG.SDInclinationH.HANDLE, P.SDInclination);
		SetVariableToWindow(CONFIG.avgObliquityH.HANDLE, P.avgObliquity);
		SetVariableToWindow(CONFIG.SDObliquityH.HANDLE, P.SDObliquity);

		SetVariableToWindow(CONFIG.starClassOH.HANDLE, P.starClassO);
		SetVariableToWindow(CONFIG.starClassBH.HANDLE, P.starClassB);
		SetVariableToWindow(CONFIG.starClassAH.HANDLE, P.starClassA);
		SetVariableToWindow(CONFIG.starClassFH.HANDLE, P.starClassF);
		SetVariableToWindow(CONFIG.starClassGH.HANDLE, P.starClassG);
		SetVariableToWindow(CONFIG.starClassKH.HANDLE, P.starClassK);
		SetVariableToWindow(CONFIG.starClassMH.HANDLE, P.starClassM);
		SetVariableToWindow(CONFIG.starClassWDH.HANDLE, P.starClassWD);
		SetVariableToWindow(CONFIG.starClassQH.HANDLE, P.starClassQ);
		SetVariableToWindow(CONFIG.starClassXH.HANDLE, P.starClassX);

		SetVariableToWindow(CONFIG.life_OrganicChanceH.HANDLE, P.life_OrganicChance);
		SetVariableToWindow(CONFIG.life_ExoticChanceH.HANDLE, P.life_ExoticChance);
		SetVariableToWindow(CONFIG.life_MulticellChanceH.HANDLE, P.life_MulticellChance);
		CheckDlgButton(hWnd, CB_FORCELIFE, P.forceLife);
		SetWindowTextW(CONFIG.forceLifeH.HANDLE, P.forceLifeState);
		CheckDlgButton(hWnd, CB_TRADITIONALLIFE, P.traditionalLife);
		SetWindowTextW(CONFIG.traditionalLifeH.HANDLE, P.traditonalLifeState);

		SetVariableToWindow(CONFIG.exotic_ShipChanceH.HANDLE, P.exotic_ShipChance);
		SetWindowTextW(CONFIG.modelsFolderH.HANDLE, P.modelsFolder);
		CheckDlgButton(hWnd, CB_SHIPSNEEDLIFE, P.shipsNeedLife);
		SetWindowTextW(CONFIG.shipsNeedLifeH.HANDLE, P.shipsNeedLifeState);

		SetVariableToWindow(CONFIG.exotic_OrbitChanceH.HANDLE, P.exotic_OrbitChance);
		SetVariableToWindow(CONFIG.exotic_AxialTiltChanceH.HANDLE, P.exotic_AxialTiltChance);
		SetVariableToWindow(CONFIG.exotic_DebrisRingChanceH.HANDLE, P.exotic_DebrisRingChance);
		SetVariableToWindow(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, P.exotic_CompanionOrbitChance);
	}
	void UpdateNamePreset(NamePreset P, HWND hWnd)
	{
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

		CheckDlgButton(NV.GROUP_ALL_SHIP, NVCB_SHIPALLPREMOD, P.useShipPreMods_All);
		SetVariableToWindow(NV.probShipPreMod_AllH, P.probShipPreMod_All);
		SetWindowTextW(NV.shipPreModList_All, P.ShipPreMods_All);
		CheckDlgButton(NV.GROUP_ALL_SHIP, NVCB_SHIPALLPOSTMOD, P.useShipPostMods_All);
		SetVariableToWindow(NV.probShipPostMod_AllH, P.probShipPostMod_All);
		SetWindowTextW(NV.shipPostModList_All, P.ShipPostMods_All);

		CheckDlgButton(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYPREMOD, P.useShipPreMods_Colony);
		SetVariableToWindow(NV.probShipPreMod_ColonyH, P.probShipPreMod_Colony);
		SetWindowTextW(NV.shipPreModList_Colony, P.ShipPreMods_Colony);
		CheckDlgButton(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYPOSTMOD, P.useShipPostMods_Colony);
		SetVariableToWindow(NV.probShipPostMod_ColonyH, P.probShipPostMod_Colony);
		SetWindowTextW(NV.shipPostModList_Colony, P.ShipPostMods_Colony);
		CheckDlgButton(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYNUMBERMOD, P.useShipNumberMods_Colony);
		SetVariableToWindow(NV.probShipNumberMod_ColonyH, P.probShipNumberMod_Colony);

		CheckDlgButton(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTPREMOD, P.useShipPreMods_Instrument);
		SetVariableToWindow(NV.probShipPreMod_InstrumentH, P.probShipPreMod_Instrument);
		SetWindowTextW(NV.shipPreModList_Instrument, P.ShipPreMods_Instrument);
		CheckDlgButton(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTPOSTMOD, P.useShipPostMods_Instrument);
		SetVariableToWindow(NV.probShipPostMod_InstrumentH, P.probShipPostMod_Instrument);
		SetWindowTextW(NV.shipPostModList_Instrument, P.ShipPostMods_Instrument);
		CheckDlgButton(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTNUMBERMOD, P.useShipNumberMods_Instrument);
		SetVariableToWindow(NV.probShipNumberMod_InstrumentH, P.probShipNumberMod_Instrument);

		CheckDlgButton(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITEPREMOD, P.useShipPreMods_Satellite);
		SetVariableToWindow(NV.probShipPreMod_SatelliteH, P.probShipPreMod_Satellite);
		SetWindowTextW(NV.shipPreModList_Satellite, P.ShipPreMods_Satellite);
		CheckDlgButton(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITEPOSTMOD, P.useShipPostMods_Satellite);
		SetVariableToWindow(NV.probShipPostMod_SatelliteH, P.probShipPostMod_Satellite);
		SetWindowTextW(NV.shipPostModList_Satellite, P.ShipPostMods_Satellite);
		CheckDlgButton(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITENUMBERMOD, P.useShipNumberMods_Satellite);
		SetVariableToWindow(NV.probShipNumberMod_SatelliteH, P.probShipNumberMod_Satellite);

		CheckDlgButton(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONPREMOD, P.useShipPreMods_Station);
		SetVariableToWindow(NV.probShipPreMod_StationH, P.probShipPreMod_Station);
		SetWindowTextW(NV.shipPreModList_Station, P.ShipPreMods_Station);
		CheckDlgButton(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONPOSTMOD, P.useShipPostMods_Station);
		SetVariableToWindow(NV.probShipPostMod_StationH, P.probShipPostMod_Station);
		SetWindowTextW(NV.shipPostModList_Station, P.ShipPostMods_Station);
		CheckDlgButton(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONNUMBERMOD, P.useShipNumberMods_Station);
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

		GetWindowTextW(CONFIG.savePresetButton.HANDLE, wfileName, NAMESIZE);
		fileName = wfileName;
		filePath += fileName + L".preset";
		std::ofstream outputFile(filePath.c_str()); // creates the file

		outputFile << "PresetName=" << wstr_to_str(fileName) << "\n"
			<< "starOutputFolder=" << wstr_to_str(CONFIG.starOutputFolder) << "\n"
			<< "planetOutputFolder=" << wstr_to_str(CONFIG.planetOutputFolder) << "\n"
			<< "seed=" << CONFIG.seed << "\n"
			<< "numberOfRuns=" << CONFIG.numberOfRuns << "\n"
			<< "smartPlacement=" << CONFIG.smartPlacement << "\n"
			<< "minPlanetNumber=" << CONFIG.minPlanetNumber << "\n"
			<< "minDistance=" << CONFIG.minDistance << "\n"
			<< "maxDistance=" << CONFIG.maxDistance << "\n"
			<< "planetSpacing=" << CONFIG.planetSpacing << "\n"
			<< "generateDwarfPlanets=" << CONFIG.generateDwarfPlanets << "\n"
			<< "dwarfPlanetChance=" << CONFIG.dwarfPlanetChance << "\n"
			<< "weightedMoons=" << CONFIG.weightedMoons << "\n"
			<< "avgEccentricity=" << CONFIG.avgEccentricity << "\n"
			<< "SDEccentricity=" << CONFIG.SDEccentricity << "\n"
			<< "avgInclination=" << CONFIG.avgInclination << "\n"
			<< "SDInclination=" << CONFIG.SDInclination << "\n"
			<< "avgObliquity=" << CONFIG.avgObliquity << "\n"
			<< "SDObliquity=" << CONFIG.SDObliquity << "\n"
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
			<< "exotic_ShipChance=" << CONFIG.exotic_ShipChance << "\n"
			<< "ModelsFolder=" << wstr_to_str(CONFIG.modelsFolder) << "\n"
			<< "ShipsNeedLife=" << CONFIG.shipsNeedLife << "\n"
			<< "exotic_OrbitChance=" << CONFIG.exotic_CompanionOrbitChance << "\n"
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

		GetWindowTextW(CONFIG.saveNamePresetButton.HANDLE, wfileName, NAMESIZE);
		fileName = wfileName;
		filePath += fileName + L".npreset";
		std::ofstream outputFile(filePath.c_str()); // creates the file

		outputFile << "PresetName=" << wstr_to_str(fileName) << "\n"
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
			ShowWindow(CONFIG.HEADER_GENERAL, 0);
			//Handles
			ShowWindow(CONFIG.seedH.HANDLE, 0);
			ShowWindow(CONFIG.numberOfRunsH.HANDLE, 0);
			ShowWindow(CONFIG.debugH.HANDLE, 0);
			ShowWindow(CONFIG.starOutputFolderH.HANDLE, 0);
			ShowWindow(CONFIG.planetOutputFolderH.HANDLE, 0);
			ShowWindow(CONFIG.presetDropDown.HANDLE, 0);
			ShowWindow(CONFIG.savePresetButton.HANDLE, 0);
			ShowWindow(CONFIG.namePresetDropDown.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG.seedH.DESC, 0);
			ShowWindow(CONFIG.numberOfRunsH.DESC, 0);
			ShowWindow(CONFIG.debugH.DESC, 0);
			ShowWindow(CONFIG.starOutputFolderH.DESC, 0);
			ShowWindow(CONFIG.planetOutputFolderH.DESC, 0);
			ShowWindow(CONFIG.presetDropDown.DESC, 0);
			ShowWindow(CONFIG.savePresetButton.DESC, 0);
			ShowWindow(CONFIG.namePresetDropDown.DESC, 0);
			//Info
			ShowWindow(CONFIG.seedH.INFOBUTTON, 0);
			ShowWindow(CONFIG.numberOfRunsH.INFOBUTTON, 0);
			ShowWindow(CONFIG.debugH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starOutputFolderH.INFOBUTTON, 0);
			ShowWindow(CONFIG.planetOutputFolderH.INFOBUTTON, 0);
			ShowWindow(CONFIG.presetDropDown.INFOBUTTON, 0);
			ShowWindow(CONFIG.savePresetButton.INFOBUTTON, 0);
			ShowWindow(CONFIG.namePresetDropDown.INFOBUTTON, 0);
			//Extra
			ShowWindow(CONFIG.presetDropDown.EXTRA, 0);
			ShowWindow(CONFIG.savePresetButton.EXTRA, 0);
			ShowWindow(CONFIG.namePresetDropDown.EXTRA, 0);
		}
			break;
		case System:
		{
			//Header
			ShowWindow(CONFIG.HEADER_SYSTEMPLANET, 0);
			//Handles
			ShowWindow(CONFIG.smartPlacementH.HANDLE, 0);
			ShowWindow(CONFIG.generateDwarfPlanetsH.HANDLE, 0);
			ShowWindow(CONFIG.dwarfPlanetChanceH.HANDLE, 0);
			ShowWindow(CONFIG.weightedMoonsH.HANDLE, 0);
			ShowWindow(CONFIG.minPlanetNumberH.HANDLE, 0);
			ShowWindow(CONFIG.minDistanceH.HANDLE, 0);
			ShowWindow(CONFIG.maxDistanceH.HANDLE, 0);
			ShowWindow(CONFIG.planetSpacingH.HANDLE, 0);
			ShowWindow(CONFIG.avgEccentricityH.HANDLE, 0);
			ShowWindow(CONFIG.SDEccentricityH.HANDLE, 0);
			ShowWindow(CONFIG.avgInclinationH.HANDLE, 0);
			ShowWindow(CONFIG.SDInclinationH.HANDLE, 0);
			ShowWindow(CONFIG.avgObliquityH.HANDLE, 0);
			ShowWindow(CONFIG.SDObliquityH.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG.smartPlacementH.DESC, 0);
			ShowWindow(CONFIG.generateDwarfPlanetsH.DESC, 0);
			ShowWindow(CONFIG.dwarfPlanetChanceH.DESC, 0);
			ShowWindow(CONFIG.weightedMoonsH.DESC, 0);
			ShowWindow(CONFIG.minPlanetNumberH.DESC, 0);
			ShowWindow(CONFIG.minDistanceH.DESC, 0);
			ShowWindow(CONFIG.maxDistanceH.DESC, 0);
			ShowWindow(CONFIG.planetSpacingH.DESC, 0);
			ShowWindow(CONFIG.avgEccentricityH.DESC, 0);
			ShowWindow(CONFIG.SDEccentricityH.DESC, 0);
			ShowWindow(CONFIG.avgInclinationH.DESC, 0);
			ShowWindow(CONFIG.SDInclinationH.DESC, 0);
			ShowWindow(CONFIG.avgObliquityH.DESC, 0);
			ShowWindow(CONFIG.SDObliquityH.DESC, 0);
			//Info
			ShowWindow(CONFIG.smartPlacementH.INFOBUTTON, 0);
			ShowWindow(CONFIG.generateDwarfPlanetsH.INFOBUTTON, 0);
			ShowWindow(CONFIG.dwarfPlanetChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.weightedMoonsH.INFOBUTTON, 0);
			ShowWindow(CONFIG.minPlanetNumberH.INFOBUTTON, 0);
			ShowWindow(CONFIG.minDistanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.maxDistanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.planetSpacingH.INFOBUTTON, 0);
			ShowWindow(CONFIG.avgEccentricityH.INFOBUTTON, 0);
			ShowWindow(CONFIG.SDEccentricityH.INFOBUTTON, 0);
			ShowWindow(CONFIG.avgInclinationH.INFOBUTTON, 0);
			ShowWindow(CONFIG.SDInclinationH.INFOBUTTON, 0);
			ShowWindow(CONFIG.avgObliquityH.INFOBUTTON, 0);
			ShowWindow(CONFIG.SDObliquityH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassAH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassBH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassFH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassGH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassKH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassMH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassOH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassQH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassWDH.INFOBUTTON, 0);
			ShowWindow(CONFIG.starClassXH.INFOBUTTON, 0);
			//Extra
			ShowWindow(CONFIG.starClassOH.EXTRA, 0);
		}
			break;
		case Life:
		{
			//Header
			ShowWindow(CONFIG.HEADER_LIFE, 0);
			//Handles
			ShowWindow(CONFIG.life_OrganicChanceH.HANDLE, 0);
			ShowWindow(CONFIG.life_ExoticChanceH.HANDLE, 0);
			ShowWindow(CONFIG.life_MulticellChanceH.HANDLE, 0);
			ShowWindow(CONFIG.forceLifeH.HANDLE, 0);
			ShowWindow(CONFIG.traditionalLifeH.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG.life_OrganicChanceH.DESC, 0);
			ShowWindow(CONFIG.life_ExoticChanceH.DESC, 0);
			ShowWindow(CONFIG.life_MulticellChanceH.DESC, 0);
			ShowWindow(CONFIG.forceLifeH.DESC, 0);
			ShowWindow(CONFIG.traditionalLifeH.DESC, 0);
			//Info
			ShowWindow(CONFIG.life_OrganicChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.life_ExoticChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.life_MulticellChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.forceLifeH.INFOBUTTON, 0);
			ShowWindow(CONFIG.traditionalLifeH.INFOBUTTON, 0);
			//Extra
		}
			break;
		case Ships:
		{
			//Header
			ShowWindow(CONFIG.HEADER_SHIPS, 0);
			//Handles
			ShowWindow(CONFIG.exotic_ShipChanceH.HANDLE, 0);
			ShowWindow(CONFIG.shipsNeedLifeH.HANDLE, 0);
			ShowWindow(CONFIG.modelsFolderH.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG.exotic_ShipChanceH.DESC, 0);
			ShowWindow(CONFIG.shipsNeedLifeH.DESC, 0);
			ShowWindow(CONFIG.modelsFolderH.DESC, 0);
			//Info
			ShowWindow(CONFIG.exotic_ShipChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.shipsNeedLifeH.INFOBUTTON, 0);
			ShowWindow(CONFIG.modelsFolderH.INFOBUTTON, 0);
			//Extra
		}
			break;
		case Exotic:
		{
			//Header
			ShowWindow(CONFIG.HEADER_EXOTIC, 0);
			//Handles
			ShowWindow(CONFIG.exotic_OrbitChanceH.HANDLE, 0);
			ShowWindow(CONFIG.exotic_AxialTiltChanceH.HANDLE, 0);
			ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, 0);
			ShowWindow(CONFIG.exotic_DebrisRingChanceH.HANDLE, 0);
			//Desc
			ShowWindow(CONFIG.exotic_OrbitChanceH.DESC, 0);
			ShowWindow(CONFIG.exotic_AxialTiltChanceH.DESC, 0);
			ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.DESC, 0);
			ShowWindow(CONFIG.exotic_DebrisRingChanceH.DESC, 0);
			//Info
			ShowWindow(CONFIG.exotic_OrbitChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.exotic_AxialTiltChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.INFOBUTTON, 0);
			ShowWindow(CONFIG.exotic_DebrisRingChanceH.INFOBUTTON, 0);
			//Extra
		}
			break;
		case Advanced:
		{
			ShowWindow(CONFIG.HEADER_ADVANCED, 0);
			ShowWindow(CONFIG.advNameGroup.EXTRA, 0);
			ShowWindow(CONFIG.saveNamePresetButton.HANDLE, 0);
			ShowWindow(CONFIG.saveNamePresetButton.DESC, 0);
			ShowWindow(CONFIG.saveNamePresetButton.INFOBUTTON, 0);
			ShowWindow(CONFIG.saveNamePresetButton.EXTRA, 0);
			ShowWindow(CONFIG.buttonDwarfMoon, 0);
			ShowWindow(CONFIG.buttonMoon, 0);
			ShowWindow(CONFIG.buttonPlanet, 0);
			ShowWindow(CONFIG.buttonUpdate, 0);
			ShowWindow(CONFIG.buttonShip, 0);
			ShowWindow(CONFIG.buttonStar, 0);
			ShowWindow(CONFIG.buttonDataset, 0);
			ShowWindow(CONFIG.buttonSimple, 0);

			Clear_Advanced();
		}
			break;
		}
	}
	void Load_Screen_General()
	{
		//Header
		ShowWindow(CONFIG.HEADER_GENERAL, 1);

		//Handles
		ShowWindow(CONFIG.seedH.HANDLE, 1);
		ShowWindow(CONFIG.numberOfRunsH.HANDLE, 1);
		ShowWindow(CONFIG.debugH.HANDLE, 1);
		ShowWindow(CONFIG.starOutputFolderH.HANDLE, 1);
		ShowWindow(CONFIG.planetOutputFolderH.HANDLE, 1);

		ShowWindow(CONFIG.presetDropDown.HANDLE, 1);
		ShowWindow(CONFIG.savePresetButton.HANDLE, 1);
		ShowWindow(CONFIG.namePresetDropDown.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG.seedH.DESC, 1);
		ShowWindow(CONFIG.numberOfRunsH.DESC, 1);
		ShowWindow(CONFIG.debugH.DESC, 1);
		ShowWindow(CONFIG.starOutputFolderH.DESC, 1);
		ShowWindow(CONFIG.planetOutputFolderH.DESC, 1);

		ShowWindow(CONFIG.presetDropDown.DESC, 1);
		ShowWindow(CONFIG.savePresetButton.DESC, 1);
		ShowWindow(CONFIG.namePresetDropDown.DESC, 1);

		//Info
		ShowWindow(CONFIG.seedH.INFOBUTTON, 1);
		ShowWindow(CONFIG.numberOfRunsH.INFOBUTTON, 1);
		ShowWindow(CONFIG.debugH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starOutputFolderH.INFOBUTTON, 1);
		ShowWindow(CONFIG.planetOutputFolderH.INFOBUTTON, 1);

		ShowWindow(CONFIG.presetDropDown.INFOBUTTON, 1);
		ShowWindow(CONFIG.savePresetButton.INFOBUTTON, 1);
		ShowWindow(CONFIG.namePresetDropDown.INFOBUTTON, 1);

		//Extra
		ShowWindow(CONFIG.presetDropDown.EXTRA, 1);
		ShowWindow(CONFIG.savePresetButton.EXTRA, 1);
		ShowWindow(CONFIG.namePresetDropDown.EXTRA, 1);

		lastScreen = General;
	}
	void Load_Screen_SystemPlanet()
	{
		//Header
		ShowWindow(CONFIG.HEADER_SYSTEMPLANET, 1);

		//Handles
		ShowWindow(CONFIG.smartPlacementH.HANDLE, 1);
		ShowWindow(CONFIG.generateDwarfPlanetsH.HANDLE, 1);
		ShowWindow(CONFIG.dwarfPlanetChanceH.HANDLE, 1);
		ShowWindow(CONFIG.weightedMoonsH.HANDLE, 1);
		ShowWindow(CONFIG.minPlanetNumberH.HANDLE, 1);
		ShowWindow(CONFIG.minDistanceH.HANDLE, 1);
		ShowWindow(CONFIG.maxDistanceH.HANDLE, 1);
		ShowWindow(CONFIG.planetSpacingH.HANDLE, 1);
		ShowWindow(CONFIG.avgEccentricityH.HANDLE, 1);
		ShowWindow(CONFIG.SDEccentricityH.HANDLE, 1);
		ShowWindow(CONFIG.avgInclinationH.HANDLE, 1);
		ShowWindow(CONFIG.SDInclinationH.HANDLE, 1);
		ShowWindow(CONFIG.avgObliquityH.HANDLE, 1);
		ShowWindow(CONFIG.SDObliquityH.HANDLE, 1);

		ShowWindow(CONFIG.starClassAH.HANDLE, 1);
		ShowWindow(CONFIG.starClassBH.HANDLE, 1);
		ShowWindow(CONFIG.starClassFH.HANDLE, 1);
		ShowWindow(CONFIG.starClassGH.HANDLE, 1);
		ShowWindow(CONFIG.starClassKH.HANDLE, 1);
		ShowWindow(CONFIG.starClassMH.HANDLE, 1);
		ShowWindow(CONFIG.starClassOH.HANDLE, 1);
		ShowWindow(CONFIG.starClassQH.HANDLE, 1);
		ShowWindow(CONFIG.starClassWDH.HANDLE, 1);
		ShowWindow(CONFIG.starClassXH.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG.smartPlacementH.DESC, 1);
		ShowWindow(CONFIG.generateDwarfPlanetsH.DESC, 1);
		ShowWindow(CONFIG.dwarfPlanetChanceH.DESC, 1);
		ShowWindow(CONFIG.weightedMoonsH.DESC, 1);
		ShowWindow(CONFIG.minPlanetNumberH.DESC, 1);
		ShowWindow(CONFIG.minDistanceH.DESC, 1);
		ShowWindow(CONFIG.maxDistanceH.DESC, 1);
		ShowWindow(CONFIG.planetSpacingH.DESC, 1);
		ShowWindow(CONFIG.avgEccentricityH.DESC, 1);
		ShowWindow(CONFIG.SDEccentricityH.DESC, 1);
		ShowWindow(CONFIG.avgInclinationH.DESC, 1);
		ShowWindow(CONFIG.SDInclinationH.DESC, 1);
		ShowWindow(CONFIG.avgObliquityH.DESC, 1);
		ShowWindow(CONFIG.SDObliquityH.DESC, 1);

		//Info
		ShowWindow(CONFIG.smartPlacementH.INFOBUTTON, 1);
		ShowWindow(CONFIG.generateDwarfPlanetsH.INFOBUTTON, 1);
		ShowWindow(CONFIG.dwarfPlanetChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.weightedMoonsH.INFOBUTTON, 1);
		ShowWindow(CONFIG.minPlanetNumberH.INFOBUTTON, 1);
		ShowWindow(CONFIG.minDistanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.maxDistanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.planetSpacingH.INFOBUTTON, 1);
		ShowWindow(CONFIG.avgEccentricityH.INFOBUTTON, 1);
		ShowWindow(CONFIG.SDEccentricityH.INFOBUTTON, 1);
		ShowWindow(CONFIG.avgInclinationH.INFOBUTTON, 1);
		ShowWindow(CONFIG.SDInclinationH.INFOBUTTON, 1);
		ShowWindow(CONFIG.avgObliquityH.INFOBUTTON, 1);
		ShowWindow(CONFIG.SDObliquityH.INFOBUTTON, 1);

		ShowWindow(CONFIG.starClassAH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassBH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassFH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassGH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassKH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassMH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassOH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassQH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassWDH.INFOBUTTON, 1);
		ShowWindow(CONFIG.starClassXH.INFOBUTTON, 1);

		//Extra
		ShowWindow(CONFIG.starClassOH.EXTRA, 1);

		lastScreen = System;
	}
	void Load_Screen_Life()
	{
		//Header
		ShowWindow(CONFIG.HEADER_LIFE, 1);

		//Handles
		ShowWindow(CONFIG.life_OrganicChanceH.HANDLE, 1);
		ShowWindow(CONFIG.life_ExoticChanceH.HANDLE, 1);
		ShowWindow(CONFIG.life_MulticellChanceH.HANDLE, 1);
		ShowWindow(CONFIG.forceLifeH.HANDLE, 1);
		ShowWindow(CONFIG.traditionalLifeH.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG.life_OrganicChanceH.DESC, 1);
		ShowWindow(CONFIG.life_ExoticChanceH.DESC, 1);
		ShowWindow(CONFIG.life_MulticellChanceH.DESC, 1);
		ShowWindow(CONFIG.forceLifeH.DESC, 1);
		ShowWindow(CONFIG.traditionalLifeH.DESC, 1);

		//Info
		ShowWindow(CONFIG.life_OrganicChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.life_ExoticChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.life_MulticellChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.forceLifeH.INFOBUTTON, 1);
		ShowWindow(CONFIG.traditionalLifeH.INFOBUTTON, 1);

		//Extra

		lastScreen = Life;
	}
	void Load_Screen_Ships()
	{
		//Header
		ShowWindow(CONFIG.HEADER_SHIPS, 1);

		//Handles
		ShowWindow(CONFIG.exotic_ShipChanceH.HANDLE, 1);
		ShowWindow(CONFIG.shipsNeedLifeH.HANDLE, 1);
		ShowWindow(CONFIG.modelsFolderH.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG.exotic_ShipChanceH.DESC, 1);
		ShowWindow(CONFIG.shipsNeedLifeH.DESC, 1);
		ShowWindow(CONFIG.modelsFolderH.DESC, 1);

		//Info
		ShowWindow(CONFIG.exotic_ShipChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.shipsNeedLifeH.INFOBUTTON, 1);
		ShowWindow(CONFIG.modelsFolderH.INFOBUTTON, 1);

		lastScreen = Ships;
	}
	void Load_Screen_Exotic()
	{
		//Header
		ShowWindow(CONFIG.HEADER_EXOTIC, 1);

		//Handles
		ShowWindow(CONFIG.exotic_OrbitChanceH.HANDLE, 1);
		ShowWindow(CONFIG.exotic_AxialTiltChanceH.HANDLE, 1);
		ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, 1);
		ShowWindow(CONFIG.exotic_DebrisRingChanceH.HANDLE, 1);

		//Desc
		ShowWindow(CONFIG.exotic_OrbitChanceH.DESC, 1);
		ShowWindow(CONFIG.exotic_AxialTiltChanceH.DESC, 1);
		ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.DESC, 1);
		ShowWindow(CONFIG.exotic_DebrisRingChanceH.DESC, 1);

		//Info
		ShowWindow(CONFIG.exotic_OrbitChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.exotic_AxialTiltChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.exotic_CompanionOrbitChanceH.INFOBUTTON, 1);
		ShowWindow(CONFIG.exotic_DebrisRingChanceH.INFOBUTTON, 1);

		lastScreen = Exotic;
	}
	void Load_Screen_Advanced()
	{
		//Header
		ShowWindow(CONFIG.HEADER_ADVANCED, 1);

		ShowWindow(CONFIG.saveNamePresetButton.HANDLE, 1);
		ShowWindow(CONFIG.saveNamePresetButton.DESC, 1);
		ShowWindow(CONFIG.saveNamePresetButton.INFOBUTTON, 1);
		ShowWindow(CONFIG.saveNamePresetButton.EXTRA, 1);

		ShowWindow(CONFIG.advNameGroup.EXTRA, 1);

		ShowWindow(CONFIG.buttonUpdate, 1);
		ShowWindow(CONFIG.buttonStar, 1);
		ShowWindow(CONFIG.buttonPlanet, 1);
		ShowWindow(CONFIG.buttonMoon, 1);
		ShowWindow(CONFIG.buttonDwarfMoon, 1);
		ShowWindow(CONFIG.buttonShip, 1);
		ShowWindow(CONFIG.buttonDataset, 1);
		ShowWindow(CONFIG.buttonSimple, 1);

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
		ShowWindow(NV.GROUP_DWARFMOON, 0);
		ShowWindow(NV.GROUP_ALL_SHIP, 0);
		ShowWindow(NV.GROUP_COLONY_SHIP, 0);
		ShowWindow(NV.GROUP_INSTRUMENT_SHIP, 0);
		ShowWindow(NV.GROUP_SATELLITE_SHIP, 0);
		ShowWindow(NV.GROUP_STATION_SHIP, 0);
		ShowWindow(NV.GROUP_DATASET, 0);
		ShowWindow(NV.GROUP_SIMPLE, 0);

		ShowWindow(NV.BUTTON_COLONY, 0);
		ShowWindow(NV.BUTTON_INSTRUMENT, 0);
		ShowWindow(NV.BUTTON_SATELLITE, 0);
		ShowWindow(NV.BUTTON_STATION, 0);

		ShowWindow(NV.nameMoonsH.INFOBUTTON, 0);
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

		ShowWindow(NV.BUTTON_COLONY, 1);
		ShowWindow(NV.BUTTON_INSTRUMENT, 1);
		ShowWindow(NV.BUTTON_SATELLITE, 1);
		ShowWindow(NV.BUTTON_STATION, 1);

		ShowWindow(NV.GROUP_ALL_SHIP, 1);
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

		ShowWindow(NV.BUTTON_COLONY, 1);
		ShowWindow(NV.BUTTON_INSTRUMENT, 1);
		ShowWindow(NV.BUTTON_SATELLITE, 1);
		ShowWindow(NV.BUTTON_STATION, 1);

		ShowWindow(NV.GROUP_COLONY_SHIP, 1);
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

		ShowWindow(NV.BUTTON_COLONY, 1);
		ShowWindow(NV.BUTTON_INSTRUMENT, 1);
		ShowWindow(NV.BUTTON_SATELLITE, 1);
		ShowWindow(NV.BUTTON_STATION, 1);

		ShowWindow(NV.GROUP_INSTRUMENT_SHIP, 1);
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

		ShowWindow(NV.BUTTON_COLONY, 1);
		ShowWindow(NV.BUTTON_INSTRUMENT, 1);
		ShowWindow(NV.BUTTON_SATELLITE, 1);
		ShowWindow(NV.BUTTON_STATION, 1);

		ShowWindow(NV.GROUP_SATELLITE_SHIP, 1);
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

		ShowWindow(NV.BUTTON_COLONY, 1);
		ShowWindow(NV.BUTTON_INSTRUMENT, 1);
		ShowWindow(NV.BUTTON_SATELLITE, 1);
		ShowWindow(NV.BUTTON_STATION, 1);

		ShowWindow(NV.GROUP_STATION_SHIP, 1);
	}

	void Load_Name_Dataset()
	{
		ShowWindow(NV.Markov_INFO, 1);

		ShowWindow(NV.GROUP_DATASET, 1);
	}
	void Load_Name_Simple()
	{
		ShowWindow(NV.SimpleGenerator_INFO, 1);

		ShowWindow(NV.GROUP_SIMPLE, 1);
	}

	void GetConfigData(HWND hWnd)
	{
		GetVariableFromWindow(CONFIG.starOutputFolderH.HANDLE, CONFIG.starOutputFolder);
		GetVariableFromWindow(CONFIG.planetOutputFolderH.HANDLE, CONFIG.planetOutputFolder);

		// this little maneuver generates the seed from the input
		CONFIG.seed = 0;
		wchar_t seedarray[WSIZE] = L"";
		GetVariableFromWindow(CONFIG.seedH.HANDLE, seedarray);
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

		GetVariableFromWindow(CONFIG.numberOfRunsH.HANDLE, CONFIG.numberOfRuns);
		CONFIG.smartPlacement = (IsDlgButtonChecked(hWnd, CB_SMARTPLACEMENT) == BST_CHECKED) ? true : false;

		GetVariableFromWindow(CONFIG.minPlanetNumberH.HANDLE, CONFIG.minPlanetNumber);
		GetVariableFromWindow(CONFIG.minDistanceH.HANDLE, CONFIG.minDistance);
		GetVariableFromWindow(CONFIG.maxDistanceH.HANDLE, CONFIG.maxDistance);
		GetVariableFromWindow(CONFIG.planetSpacingH.HANDLE, CONFIG.planetSpacing);
		CONFIG.generateDwarfPlanets = (IsDlgButtonChecked(hWnd, CB_GENERATEDWARFPLANET) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG.dwarfPlanetChanceH.HANDLE, CONFIG.dwarfPlanetChance);
		CONFIG.weightedMoons = (IsDlgButtonChecked(hWnd, CB_WEIGHTEDMOONS) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(CONFIG.avgEccentricityH.HANDLE, CONFIG.avgEccentricity); // change to extra for trackbar
		GetVariableFromWindow(CONFIG.SDEccentricityH.HANDLE, CONFIG.SDEccentricity);
		GetVariableFromWindow(CONFIG.avgInclinationH.HANDLE, CONFIG.avgInclination);
		GetVariableFromWindow(CONFIG.SDInclinationH.HANDLE, CONFIG.SDInclination);
		GetVariableFromWindow(CONFIG.avgObliquityH.HANDLE, CONFIG.avgObliquity);
		GetVariableFromWindow(CONFIG.SDObliquityH.HANDLE, CONFIG.SDObliquity);

		GetVariableFromWindow(CONFIG.starClassOH.HANDLE, CONFIG.starClassO);
		GetVariableFromWindow(CONFIG.starClassBH.HANDLE, CONFIG.starClassB);
		GetVariableFromWindow(CONFIG.starClassAH.HANDLE, CONFIG.starClassA);
		GetVariableFromWindow(CONFIG.starClassFH.HANDLE, CONFIG.starClassF);
		GetVariableFromWindow(CONFIG.starClassGH.HANDLE, CONFIG.starClassG);
		GetVariableFromWindow(CONFIG.starClassKH.HANDLE, CONFIG.starClassK);
		GetVariableFromWindow(CONFIG.starClassMH.HANDLE, CONFIG.starClassM);
		GetVariableFromWindow(CONFIG.starClassWDH.HANDLE, CONFIG.starClassWD);
		GetVariableFromWindow(CONFIG.starClassQH.HANDLE, CONFIG.starClassQ);
		GetVariableFromWindow(CONFIG.starClassXH.HANDLE, CONFIG.starClassX);

		GetVariableFromWindow(CONFIG.life_OrganicChanceH.HANDLE, CONFIG.life_OrganicChance);
		GetVariableFromWindow(CONFIG.life_ExoticChanceH.HANDLE, CONFIG.life_ExoticChance);
		GetVariableFromWindow(CONFIG.life_MulticellChanceH.HANDLE, CONFIG.life_MulticellChance);
		CONFIG.forceLife = (IsDlgButtonChecked(hWnd, CB_FORCELIFE) == BST_CHECKED) ? true : false;
		CONFIG.traditionalLife = (IsDlgButtonChecked(hWnd, CB_TRADITIONALLIFE) == BST_CHECKED) ? true : false;

		GetVariableFromWindow(CONFIG.exotic_ShipChanceH.HANDLE, CONFIG.exotic_ShipChance);
		GetVariableFromWindow(CONFIG.modelsFolderH.HANDLE, CONFIG.modelsFolder);
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

		GetVariableFromWindow(CONFIG.exotic_OrbitChanceH.HANDLE, CONFIG.exotic_OrbitChance);
		GetVariableFromWindow(CONFIG.exotic_AxialTiltChanceH.HANDLE, CONFIG.exotic_AxialTiltChance);
		GetVariableFromWindow(CONFIG.exotic_DebrisRingChanceH.HANDLE, CONFIG.exotic_DebrisRingChance);
		GetVariableFromWindow(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, CONFIG.exotic_CompanionOrbitChance);

		CONFIG.debug = true;
		CONFIG.planetSpacing = 1.0;
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
		NV.useShipPreMods_All = (IsDlgButtonChecked(NV.GROUP_ALL_SHIP, NVCB_SHIPALLPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_AllH, NV.probShipPreMod_All);
		FillModList(NV.shipPreModList_All, NV.ShipPreMods_All);
		NV.useShipPostMods_All = (IsDlgButtonChecked(NV.GROUP_ALL_SHIP, NVCB_SHIPALLPOSTMOD) == BST_CHECKED) ? true : false;
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
		NV.useShipPreMods_Colony = (IsDlgButtonChecked(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_ColonyH, NV.probShipPreMod_Colony);
		FillModList(NV.shipPreModList_Colony, NV.ShipPreMods_Colony);
		NV.useShipPostMods_Colony = (IsDlgButtonChecked(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_ColonyH, NV.probShipPostMod_Colony);
		FillModList(NV.shipPostModList_Colony, NV.ShipPostMods_Colony);
		NV.useShipNumberMods_Colony = (IsDlgButtonChecked(NV.GROUP_COLONY_SHIP, NVCB_SHIPCOLONYNUMBERMOD) == BST_CHECKED) ? true : false;
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
		NV.useShipPreMods_Instrument = (IsDlgButtonChecked(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_InstrumentH, NV.probShipPreMod_Instrument);
		FillModList(NV.shipPreModList_Instrument, NV.ShipPreMods_Instrument);
		NV.useShipPostMods_Instrument = (IsDlgButtonChecked(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_InstrumentH, NV.probShipPostMod_Instrument);
		FillModList(NV.shipPostModList_Instrument, NV.ShipPostMods_Instrument);
		NV.useShipNumberMods_Instrument = (IsDlgButtonChecked(NV.GROUP_INSTRUMENT_SHIP, NVCB_SHIPINSTRUMENTNUMBERMOD) == BST_CHECKED) ? true : false;
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
		NV.useShipPreMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITEPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_SatelliteH, NV.probShipPreMod_Satellite);
		FillModList(NV.shipPreModList_Satellite, NV.ShipPreMods_Satellite);
		NV.useShipPostMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITEPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_SatelliteH, NV.probShipPostMod_Satellite);
		FillModList(NV.shipPostModList_Satellite, NV.ShipPostMods_Satellite);
		NV.useShipNumberMods_Satellite = (IsDlgButtonChecked(NV.GROUP_SATELLITE_SHIP, NVCB_SHIPSATELLITENUMBERMOD) == BST_CHECKED) ? true : false;
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
		NV.useShipPreMods_Station = (IsDlgButtonChecked(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONPREMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPreMod_StationH, NV.probShipPreMod_Station);
		FillModList(NV.shipPreModList_Station, NV.ShipPreMods_Station);
		NV.useShipPostMods_Station = (IsDlgButtonChecked(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONPOSTMOD) == BST_CHECKED) ? true : false;
		GetVariableFromWindow(NV.probShipPostMod_StationH, NV.probShipPostMod_Station);
		FillModList(NV.shipPostModList_Station, NV.ShipPostMods_Station);
		NV.useShipNumberMods_Station = (IsDlgButtonChecked(NV.GROUP_STATION_SHIP, NVCB_SHIPSTATIONNUMBERMOD) == BST_CHECKED) ? true : false;
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


	void SetInfoBox(int command)
	{
		switch (command)
		{
		case IB_SEED:
			SetWindowTextW(CONFIG.INFO_BOX, L"Seed is what generates numbers.");
			break;
		case IB_NUMBEROFRUNS:
			SetWindowTextW(CONFIG.INFO_BOX, L"This is how many systems the program will generate.");
			break;
		case IB_DEBUG:
			SetWindowTextW(CONFIG.INFO_BOX, L"Enable debug mode.");
			break;
		case IB_STAROUTPUTFOLDER:
			SetWindowTextW(CONFIG.INFO_BOX, L"Star Output Folder.");
			break;
		case IB_PLANETOUTPUTFOLDER:
			SetWindowTextW(CONFIG.INFO_BOX, L"Planet Output Folder.");
			break;
		case IB_SMARTPLACEMENT:
			SetWindowTextW(CONFIG.INFO_BOX, L"Places things smartly.");
			break;
		case IB_DISTANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"The distance from Earth in parsecs.");
			break;
		case IB_OBLIQUITY:
			SetWindowTextW(CONFIG.INFO_BOX, L"The axial tilt of planets.");
			break;
		case IB_INCLINATION:
			SetWindowTextW(CONFIG.INFO_BOX, L"The inclination of planets.");
			break;
		case IB_ECCENTRICITY:
			SetWindowTextW(CONFIG.INFO_BOX, L"The eccentricity of planets.");
			break;
		case IB_MINPLANETNUMBER:
			SetWindowTextW(CONFIG.INFO_BOX, L"min planet number.");
			break;
		case IB_STARCLASS:
			SetWindowTextW(CONFIG.INFO_BOX, L"class of stars.");
			break;
		case IB_LIFEORGANICCHANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"Organic life.");
			break;
		case IB_LIFEEXOTICCHANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"exotic life");
			break;
		case IB_LIFEMULTICHANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"multicel life");
			break;
		case IB_FORCELIFE:
			SetWindowTextW(CONFIG.INFO_BOX, L"force life");
			break;
		case IB_TRADITIONALLIFE:
			SetWindowTextW(CONFIG.INFO_BOX, L"life traditional");
			break;
		case IB_SHIPCHANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"ship chance");
			break;
		case IB_SHIPSNEEDLIFE:
			SetWindowTextW(CONFIG.INFO_BOX, L"ships need life");
			break;
		case IB_MODELSFOLDER:
			SetWindowTextW(CONFIG.INFO_BOX, L"models folder");
			break;
		case IB_EXOTICORBIT:
			SetWindowTextW(CONFIG.INFO_BOX, L"exotic orbit");
			break;
		case IB_EXOTICOBLIQUITY:
			SetWindowTextW(CONFIG.INFO_BOX, L"exotic obliquity");
			break;
		case IB_EXOTICDEBRISRING:
			SetWindowTextW(CONFIG.INFO_BOX, L"exotic debris ring");
			break;
		case IB_EXOTICCOMPANION:
			SetWindowTextW(CONFIG.INFO_BOX, L"exotic companion orbit");
			break;
		case IB_SELECTPRESET:
			SetWindowTextW(CONFIG.INFO_BOX, L"Select the preset variables.");
			break;
		case IB_SAVEPRESET:
			SetWindowTextW(CONFIG.INFO_BOX, L"Save the current variables as a new preset.");
			break;
		case IB_GENERATEDWARFPLANET:
			SetWindowTextW(CONFIG.INFO_BOX, L"generate dwarf planets true/false.");
			break;
		case IB_DWARFPALNETCHANCE:
			SetWindowTextW(CONFIG.INFO_BOX, L"the chance dwarf planets will spawn.");
			break;
		case IB_NAMEPREMODS:
			SetWindowTextW(CONFIG.INFO_BOX, L"the pre mods");
			break;
		case IB_NAMEPOSTMODS:
			SetWindowTextW(CONFIG.INFO_BOX, L"the post mods");
			break;
		case IB_NAMENUMBERMODS:
			SetWindowTextW(CONFIG.INFO_BOX, L"the numbermods");
			break;
		case IB_NAMEPREMODLIST:
			SetWindowTextW(CONFIG.INFO_BOX, L"box to hold pre mods");
			break;
		case IB_NAMEPOSTMODLIST:
			SetWindowTextW(CONFIG.INFO_BOX, L"box to hold post mods");
			break;
		case IB_NAMEMOONS:
			SetWindowTextW(CONFIG.INFO_BOX, L"enables moons or something");
			break;
		}
	}
	void SetCheckBoxText(HWND hWnd, int command)
	{
		int msg = IsDlgButtonChecked(hWnd, command);


		switch (command)
		{
		case CB_DEBUG:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.debugH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.debugH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_SMARTPLACEMENT:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.smartPlacementH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.smartPlacementH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_TRADITIONALLIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.traditionalLifeH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.traditionalLifeH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_FORCELIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.forceLifeH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.forceLifeH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_SHIPSNEEDLIFE:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.shipsNeedLifeH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.shipsNeedLifeH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_GENERATEDWARFPLANET:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.generateDwarfPlanetsH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.generateDwarfPlanetsH.HANDLE, L"Enabled");
				break;
			}
			break;
		case CB_WEIGHTEDMOONS:
			switch (msg)
			{
			case BST_UNCHECKED:
				SetWindowTextW(CONFIG.weightedMoonsH.HANDLE, L"Disabled");
				break;
			case BST_CHECKED:
				SetWindowTextW(CONFIG.weightedMoonsH.HANDLE, L"Enabled");
				break;
			}
			break;
		}
	}

	//open preset function
	/*
	int OpenPreset()
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
							SetWindowTextW(CONFIG.savePresetButton.DESC, pszFilePath);


							MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileOpen->Release();
			}
			CoUninitialize();
		}
		return 0;
	}
	*/

	/*---------------------------------------------------------------------------------------#
	|	THE GENERATOR:																		 |
	#---------------------------------------------------------------------------------------*/

	std::wstring GenName(Object_Type type)
	{
		//######################################################################################################
			//  VARIABLES

		std::wstring finalName;
		int wordCount, wordPercent, syllCount, syllPercent; // number of words in the name, and % chance of multiple words. Number of syllables in a word and percent for multiple syllables
		bool has_prename_mod, has_postname_mod, has_number_mod, has_shipall_premod, has_shipall_postmod, testName;

		//######################################################################################################
			// PUTS NAME TOGETHER

		do //loop to check if the name has been used already
		{
			finalName = L"";
			wordCount = 1;
			wordPercent = 5;
			testName = false;
			has_prename_mod = false;
			has_postname_mod = false;
			has_number_mod = false;
			has_shipall_premod = false;
			has_shipall_postmod = false;

			//######################################################################################################
				//  GENERATES NUMBER OF WORDS

			while (genpercent(mt_name) <= wordPercent)
				wordCount++;

			//######################################################################################################
				// MODIFIER PICKER

			switch (type)
			{
			case typeDwarfMoon:
			{
				if (NV.useDwarfMoonPreMods)
					has_prename_mod = (genpercent(mt_name) <= NV.probDwarfMoonPreMod) ? true : false;
				if (NV.useDwarfMoonPostMods)
					has_postname_mod = (genpercent(mt_name) <= NV.probDwarfMoonPostMod) ? true : false;
				if (NV.useDwarfMoonNumberMods)
					has_number_mod = (genpercent(mt_name) <= NV.probDwarfMoonNumberMod) ? true : false;
			}
				break;
			case typeMoon:
			{
				if (NV.useMoonPreMods)
					has_prename_mod = (genpercent(mt_name) <= NV.probMoonPreMod) ? true : false;
				if (NV.useMoonPostMods)
					has_postname_mod = (genpercent(mt_name) <= NV.probMoonPostMod) ? true : false;
				if (NV.useMoonNumberMods)
					has_number_mod = (genpercent(mt_name) <= NV.probMoonNumberMod) ? true : false;
			}			
				break;
			case typePlanet:
			{
				if (NV.usePlanetPreMods)
					has_prename_mod = (genpercent(mt_name) <= NV.probPlanetPreMod) ? true : false;
				if (NV.usePlanetPostMods)
					has_postname_mod = (genpercent(mt_name) <= NV.probPlanetPostMod) ? true : false;
				if (NV.usePlanetNumberMods)
					has_number_mod = (genpercent(mt_name) <= NV.probPlanetNumberMod) ? true : false;
			}
				break;
			case typeShipColony:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) <= NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) <= NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Colony)
					has_prename_mod = (genpercent(mt_name) <= NV.probShipPreMod_Colony) ? true : false;
				if (NV.useShipPostMods_Colony)
					has_postname_mod = (genpercent(mt_name) <= NV.probShipPostMod_Colony) ? true : false;
				if (NV.useShipNumberMods_Colony)
					has_number_mod = (genpercent(mt_name) <= NV.probShipNumberMod_Colony) ? true : false;
			}
				break;
			case typeShipInstrument:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) <= NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) <= NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Instrument)
					has_prename_mod = (genpercent(mt_name) <= NV.probShipPreMod_Instrument) ? true : false;
				if (NV.useShipPostMods_Instrument)
					has_postname_mod = (genpercent(mt_name) <= NV.probShipPostMod_Instrument) ? true : false;
				if (NV.useShipNumberMods_Instrument)
					has_number_mod = (genpercent(mt_name) <= NV.probShipNumberMod_Instrument) ? true : false;
			}
				break;
			case typeShipSatellite:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) <= NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) <= NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Satellite)
					has_prename_mod = (genpercent(mt_name) <= NV.probShipPreMod_Satellite) ? true : false;
				if (NV.useShipPostMods_Satellite)
					has_postname_mod = (genpercent(mt_name) <= NV.probShipPostMod_Satellite) ? true : false;
				if (NV.useShipNumberMods_Satellite)
					has_number_mod = (genpercent(mt_name) <= NV.probShipNumberMod_Satellite) ? true : false;
			}
				break;
			case typeShipStation:
			{
				if (NV.useShipPreMods_All)
					has_shipall_premod = (genpercent(mt_name) <= NV.probShipPreMod_All) ? true : false;
				if (NV.useShipPostMods_All)
					has_shipall_postmod = (genpercent(mt_name) <= NV.probShipPostMod_All) ? true : false;
				if (NV.useShipPreMods_Station)
					has_prename_mod = (genpercent(mt_name) <= NV.probShipPreMod_Station) ? true : false;
				if (NV.useShipPostMods_Station)
					has_postname_mod = (genpercent(mt_name) <= NV.probShipPostMod_Station) ? true : false;
				if (NV.useShipNumberMods_Station)
					has_number_mod = (genpercent(mt_name) <= NV.probShipNumberMod_Station) ? true : false;
			}
				break;
			case typeStar:
			{
				if (NV.useStarPreMods)
					has_prename_mod = (genpercent(mt_name) <= NV.probStarPreMod) ? true : false;
				if (NV.useStarPostMods)
					has_postname_mod = (genpercent(mt_name) <= NV.probStarPostMod) ? true : false;
				if (NV.useStarNumberMods)
					has_number_mod = (genpercent(mt_name) <= NV.probStarNumberMod) ? true : false;
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
					if (syllPercent <= 10)
						syllCount = 1;
					else if (syllPercent < 100)
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

			if (has_number_mod == true)
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

	void SortVector(std::vector<PLANET>& vector, int low, int high)
	{
		if (low < high)
		{
			int part = Partition(vector, low, high);
			SortVector(vector, low, part - 1);
			SortVector(vector, part + 1, high);
		}
	}
	int Partition(std::vector<PLANET>& vector, int low, int high)
	{
		double pivot = vector.at(high).semimajorAxis;
		int i = (low - 1);

		for (int j = low; j <= high - 1; j++)
		{
			if (vector.at(j).semimajorAxis <= pivot)
			{
				i++;
				PLANET temp = vector.at(i);
				vector.at(i) = vector.at(j);
				vector.at(j) = temp;
			}
		}

		PLANET temp = vector.at(i + 1);
		vector.at(i + 1) = vector.at(high);
		vector.at(high) = temp;
		return (i + 1);
	}

	// Generation starts here:
	void BeginGenerate()
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

		mt_star.seed(CONFIG.seed);
		mt_planet.seed(CONFIG.seed);
		mt_moon.seed(CONFIG.seed);
		mt_ship.seed(CONFIG.seed);
		mt_name.seed(CONFIG.seed);

		/*###############################################################################
				OUTER LOOP
		###############################################################################*/
		for (int run = 0; run < CONFIG.numberOfRuns; run++)
		{
			STAR currentStar;
			GenerateStar(currentStar);

			std::wstring starFileName = CONFIG.starOutputFolder;	//Creates the star file
			starFileName += currentStar.name;
			starFileName += L" Star.sc";
			//starFileName += L"Test Star.sc";
			std::ofstream starFile(starFileName.c_str());
			PrintStar(currentStar, starFile);

			std::wstring planetFileName = CONFIG.planetOutputFolder;	//Creates the planet file
			planetFileName += currentStar.name;
			planetFileName += L" System.sc";
			//planetFileName += L"Test System.sc";
			std::ofstream planetFile(planetFileName.c_str());

			std::uniform_int_distribution<int> genplanetnum{ CONFIG.minPlanetNumber, currentStar.maxPlanetNumber };
			int planetNumber = genplanetnum(mt_planet);
			std::vector<PLANET> planetList;

			// test line

			/*###############################################################################
					PLANET GENERATOR
			###############################################################################*/

			for (int planetCounter = 0; planetCounter < planetNumber; planetCounter++)
			{
				PLANET currentPlanet;
				GeneratePlanet(currentStar, currentPlanet);
				planetList.push_back(currentPlanet);

				// Debug moon orbits
				/*
				if (CONFIG.debug == 1)
				{
					PLANET Debug;

					Debug.planetType = L"Star";
					Debug.parentBody = currentPlanet.name;
					Debug.class_ = L"Z";
					Debug.mass = 0.000000001;
					Debug.radius = 0.000000001;
					Debug.obliquity = 0;
					Debug.eccentricity = 0;
					Debug.inclination = 0;
					Debug.ascendingNode = 0;
					Debug.argofPericenter = 0;
					Debug.meanAnomaly = 0;

					Debug.name = L"Hill Sphere Outer Limit";
					Debug.semimajorAxis = (currentPlanet.hillSphereOuterLimit / 149598000);
					PrintPlanet(Debug, planetFile);

					Debug.name = L"Hill Sphere Inner Limit";
					if (currentPlanet.class_ == L"Jupiter")
						Debug.semimajorAxis = ((2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 149598000;
					else
						Debug.semimajorAxis = ((2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 149598000;
					PrintPlanet(Debug, planetFile);

					if (currentPlanet.class_ == L"Jupiter")
					{
						Debug.name = L"Hill Sphere Fifteenth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 15) / 149598000);
						PrintPlanet(Debug, planetFile);

						Debug.name = L"Hill Sphere Tenth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 10) / 149598000);
						PrintPlanet(Debug, planetFile);

						Debug.name = L"Hill Sphere Fifth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 5) / 149598000);
						PrintPlanet(Debug, planetFile);
					}
					else if (currentPlanet.class_ == L"Neptune")
					{
						Debug.name = L"Hill Sphere Fifteenth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 15) / 149598000);
						PrintPlanet(Debug, planetFile);

						Debug.name = L"Hill Sphere Tenth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 10) / 149598000);
						PrintPlanet(Debug, planetFile);

						Debug.name = L"Hill Sphere Fifth Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.1), (1.0 / 3.0))) * currentPlanet.radius) / 5) / 149598000);
						PrintPlanet(Debug, planetFile);
					}
					else
					{
						Debug.name = L"Hill Sphere Third Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 3) / 149598000);
						PrintPlanet(Debug, planetFile);

						Debug.name = L"Hill Sphere Half Limit";
						Debug.semimajorAxis = (((currentPlanet.hillSphereOuterLimit + (2.44 * pow((currentPlanet.density / 0.05), (1.0 / 3.0))) * currentPlanet.radius) / 2) / 149598000);
						PrintPlanet(Debug, planetFile);
					}
				}
				*/
			}
			while (genpercent(mt_planet) < CONFIG.dwarfPlanetChance)
			{
				PLANET currentDwarf;
				GenerateDwarfPlanet(currentStar, currentDwarf);
				planetList.push_back(currentDwarf);
			}
			SortVector(planetList, 0, planetList.size() - 1);

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

				if (!lifeTest)
				{
					int position;
					do
					{
						int vectSize = planetList.size() - 1;
						std::uniform_int_distribution<int> genplanetposition{ 0, vectSize };
						position = genplanetposition(mt_planet);
						lifeTest = true;
					} while (planetList.at(position).planetType == L"DwarfPlanet");
					planetList.at(position).life_exotic.haslife = true;
					ExoticGenerateLife(planetList.at(position));
				}
			}

			/*###############################################################################
					MOON GENERATOR PER PLANET
			###############################################################################*/
			for (int currentPlanet = 0; currentPlanet < planetList.size(); currentPlanet++)
			{
				std::vector<PLANET> majorMoon, minorMoon;
				// weighted moons uses new system, else old system
				if (CONFIG.weightedMoons)
				{
					planetList.at(currentPlanet).numberOfMajorMoons = 0;
					planetList.at(currentPlanet).numberOfMinorMoons = 0;

					while (genpercent(mt_moon) < planetList.at(currentPlanet).majorMoonPercent)
					{
						PLANET currentMoon;
						GenerateMajorMoon(currentStar, planetList.at(currentPlanet), currentMoon, planetList.at(currentPlanet).numberOfMajorMoons);
						majorMoon.push_back(currentMoon);
						planetList.at(currentPlanet).numberOfMajorMoons++;
						std::uniform_int_distribution<int> gennumber{ 1, planetList.at(currentPlanet).numberOfMajorMoons };
						planetList.at(currentPlanet).majorMoonPercent -= gennumber(mt_moon);
					}
				
					while (genpercent(mt_moon) < planetList.at(currentPlanet).minorMoonPercent)
					{
						PLANET currentMoon;
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
						PLANET currentMoon;
						GenerateMajorMoon(currentStar, planetList.at(currentPlanet), currentMoon, Moon_Count);
						majorMoon.push_back(currentMoon);
						Moon_Count++;
					}
					Moon_Count = 0;

					while (Moon_Count < planetList.at(currentPlanet).numberOfMinorMoons)
					{
						PLANET currentMoon;
						GenerateMinorMoon(planetList.at(currentPlanet), currentMoon, Moon_Count);
						minorMoon.push_back(currentMoon);
						Moon_Count++;
					}
				}

				SortVector(majorMoon, 0, majorMoon.size() - 1);
				SortVector(minorMoon, 0, minorMoon.size() - 1);

				/*###############################################################################
					PRINTING
				###############################################################################*/
				if ((planetList.at(currentPlanet).semimajorAxis * CONFIG.planetSpacing) > currentStar.innerLimit)
				{
					planetList.at(currentPlanet).semimajorAxis *= CONFIG.planetSpacing;

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
				if (planetList.at(currentPlanet).debrisCount > 0 && currentPlanet != 0)
				{
					// Generates data for debris to spawn around
					double SMCenterSpread, SMCenterPoint, inclinationCenter = 0.0, inclinationSD = 0.0, DebrisSpread = 0.0;

					std::uniform_int_distribution<int> genpreset{ 1, 6 };
					int preset = genpreset(mt_planet);

					//##############################
							// CONTROL PANEL
							// preset =	1	;
					//##############################

					if (preset == 1) // Classic, thin ring around the equator, moderate distance from planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.8, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}
					else if (preset == 2) // Classic, thin ring around the equator, close to planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.2, 1.5 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}
					else if (preset == 3) // Classic, thin ring around the equator, far from planet
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 5, 6 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}
					else if (preset == 4) // Wide ring around the equator
					{
						std::uniform_real_distribution<> gensd{ 0.1, 1 };
						std::uniform_real_distribution<> genspread{ 1.3, 2 };
						std::uniform_real_distribution<> gensmspread{ 1.5, 5 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}
					else if (preset == 5) // Thin ring around the equator, higher inclination
					{
						std::uniform_real_distribution<> gensd{ 2, 7 };
						std::uniform_real_distribution<> genspread{ 1.05, 1.2 };
						std::uniform_real_distribution<> gensmspread{ 1.2, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}
					else if (preset == 6) // Wide ring around the equator, higher inclination
					{
						std::uniform_real_distribution<> gensd{ 2, 7 };
						std::uniform_real_distribution<> genspread{ 1.3, 2 };
						std::uniform_real_distribution<> gensmspread{ 1.5, 4 };

						inclinationCenter = 0.0;
						inclinationSD = gensd(mt_planet);
						DebrisSpread = genspread(mt_planet);
						SMCenterSpread = gensmspread(mt_planet);
					}

					SMCenterPoint = planetList.at(currentPlanet).radius * SMCenterSpread;

					// generates and prints the debris
					PLANET debrisMoon;
					debrisMoon.planetType = L"DwarfMoon";
					debrisMoon.class_ = L"Asteroid";
					debrisMoon.parentBody = planetList.at(currentPlanet).name;

					if (planetList.at(currentPlanet).semimajorAxis > currentStar.innerLimit)
					{
						for (int count = 0; count < planetList.at(currentPlanet).debrisCount; count++)
						{
							ExoticDebrisRing(planetList.at(currentPlanet), debrisMoon, inclinationCenter, inclinationSD, DebrisSpread, SMCenterPoint);
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
					enable_colony = false;
				if (CONFIG.shipList_Instrument.size() == 0)
					enable_instrument = false;
				if (CONFIG.shipList_Satellite.size() == 0)
					enable_satellite = false;
				if (CONFIG.shipList_Station.size() == 0)
					enable_station = false;

				while (genpercent(mt_ship) <= CONFIG.exotic_ShipChance)
				{
					int size = planetList.size(), parent;
					PLANET ship;
					ship.planetType = L"Spacecraft";

					std::uniform_int_distribution<int> genship{ 0, size };
					parent = genship(mt_ship);

					// the star is the parent
					if (parent == planetList.size())
					{
						ship.parentBody = currentStar.name;

						// final check before the ship type is generated
						if (enable_colony || enable_instrument || enable_station)
						{
							double min_dist = 0, max_dist = 0;
							std::discrete_distribution<int> genshiptype{ 0, 0, 0, 0, 0, (double)enable_colony, (double)enable_instrument, 0, (double)enable_station };
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

								min_dist = (currentStar.habitZoneInnerLimit / 2) * 149598000;
								max_dist = (currentStar.habitZoneOuterLimit * 2) * 149598000;
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

								min_dist = (currentStar.habitZoneInnerLimit / 2) * 149598000;
								max_dist = currentStar.outerLimit * 149598000;
								break;
							}
							}

							GenerateShip(ship, min_dist, max_dist, CONFIG.avgEccentricity, CONFIG.SDEccentricity, CONFIG.avgInclination, CONFIG.SDInclination);
							PrintShip(ship, planetFile);
						}
					}
					// a planet is the parent
					else
					{
						// weeds out some ship types based on the planet type
						ship.parentBody = planetList.at(parent).name;
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
							std::discrete_distribution<int> genshiptype{ 0, 0, 0, 0, 0, (double)enable_colony, (double)enable_instrument, (double)enable_satellite, (double)enable_station };
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
					}
				}
			}

			// Debug star Orbits
			if (CONFIG.debug == 1)
			{
				PLANET Debug;

				Debug.planetType = L"Star";
				Debug.parentBody = currentStar.name;

				Debug.class_ = L"Z";
				Debug.mass = 0.000000001;
				Debug.radius = 0.000000001;
				Debug.obliquity = 0;
				Debug.eccentricity = 0;
				Debug.inclination = 0;
				Debug.ascendingNode = 0;
				Debug.argofPericenter = 0;
				Debug.meanAnomaly = 0;

				Debug.name = L"Inner Limit";
				Debug.semimajorAxis = currentStar.innerLimit;
				PrintPlanet(Debug, planetFile);

				Debug.name = L"Outer Limit";
				Debug.semimajorAxis = currentStar.outerLimit;
				PrintPlanet(Debug, planetFile);

				Debug.name = L"Frost Limit";
				Debug.semimajorAxis = currentStar.frostLine;
				PrintPlanet(Debug, planetFile);

				Debug.name = L"HZ Inner Limit";
				Debug.semimajorAxis = currentStar.habitZoneInnerLimit;
				PrintPlanet(Debug, planetFile);

				Debug.name = L"HZ Outer Limit";
				Debug.semimajorAxis = currentStar.habitZoneOuterLimit;
				PrintPlanet(Debug, planetFile);
			}

			//end of every star generated
			currentStar.maxPlanetNumber = 0;
			currentStar.semimajorList.clear();
			starFile.close();
			planetFile.close();
		}
		return;
	}

	void PrintStar(STAR& star, std::ofstream& file)
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
	void PrintPlanet(PLANET& planet, std::ofstream& file)
	{
		if (planet.planetType == L"DwarfPlanet")
			file << wstr_to_str(planet.planetType) << "\t\t\t\t\t\"" << wstr_to_str(planet.name) << "\"\n{";
		else
			file << wstr_to_str(planet.planetType) << "\t\t\t\t\t\t\"" << wstr_to_str(planet.name) << "\"\n{";
		file << "\n\tParentBody\t\t\t\t\"" << wstr_to_str(planet.parentBody) << "\""
			<< "\n\tClass\t\t\t\t\t\"" << wstr_to_str(planet.class_) << "\"\n";
		if (planet.planetType != L"DwarfMoon")
			file << "\n\tMass\t\t\t\t\t" << planet.mass << "\n";
		file << "\tRadius\t\t\t\t\t" << planet.radius << "\n"
			<< "\tObliquity\t\t\t\t" << planet.obliquity << "\n";

		if (planet.life_organic.haslife == true)
		{
			file << "\n\tLife\n\t{"
				<< "\n\t\tClass\t\"" << wstr_to_str(planet.life_organic._class) << "\""
				<< "\n\t\tType\t\"" << wstr_to_str(planet.life_organic.type) << "\"";
			if (planet.life_organic.panspermia == true)
				file << "\n\t\tPanspermia\ttrue";
			file << "\n\t}\n";
		}
		if (planet.life_exotic.haslife == true)
		{
			file << "\n\tLife\n\t{"
				<< "\n\t\tClass\t\"" << wstr_to_str(planet.life_exotic._class) << "\""
				<< "\n\t\tType\t\"" << wstr_to_str(planet.life_exotic.type) << "\"";
			if (planet.life_exotic.panspermia == true)
				file << "\n\t\tPanspermia\ttrue";
			file << "\n\t}\n";
		}

		file << "\n\tOrbit\n\t{\n\t\t"
			<< "RefPlane\t\t\t\"Equator\"";
		if (planet.planetType == L"Moon" || planet.planetType == L"DwarfMoon")
			file << "\n\t\tSemiMajorAxis\t\t" << (planet.semimajorAxis / 149598000);
		else
			file << "\n\t\tSemiMajorAxis\t\t" << planet.semimajorAxis;
		file << "\n\t\tEccentricity\t\t" << planet.eccentricity
			<< "\n\t\tInclination\t\t\t" << planet.inclination
			<< "\n\t\tAscendingNode\t\t" << planet.ascendingNode
			<< "\n\t\tArgOfPericenter\t\t" << planet.argofPericenter
			<< "\n\t\tMeanAnomaly\t\t\t" << planet.meanAnomaly
			<< "\n\t}\n}\n\n";
	}
	void PrintShip(PLANET& ship, std::ofstream& planetFile)
	{
		planetFile << wstr_to_str(ship.planetType) << "\t\t\t\t\t\t\"" << wstr_to_str(ship.name) << "\"\n{"
			<< "\n\tModel\t\t\"" << wstr_to_str(ship.model) << "\""
			<< "\n\tParentBody\t\t\t\t\"" << wstr_to_str(ship.parentBody) << "\""
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



	void GenerateStar(STAR& star)
	{
		double SMpoint, tempSol;

		std::uniform_int_distribution<int> genhour{ 0, 23 };
		std::uniform_int_distribution<int> genminute{ 0, 59 };
		std::uniform_real_distribution<> gensec{ 0, 60 };
		std::uniform_int_distribution<int> gendecl{ -90, 90 };
		std::uniform_real_distribution<> gendistance{ CONFIG.minDistance, CONFIG.maxDistance };
		std::normal_distribution<> genSM{ 1.7, 0.2 };
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

			std::uniform_real_distribution<> genm{ 1, 100 };
			star.mass = genm(mt_star);
			star.radius = ((2 * star.mass * G) / (pow(c, 2)) * 1000000);

			//std::uniform_int_distribution<int> gent{ 60000, 120000 };
			//star.temperatureK = gent(mt_star);
			star.temperatureK = 0;
			break;
		}
		}

		tempSol = (star.temperatureK / 5778.0);
		star.luminosity = (pow(star.radius, 2) * pow(tempSol, 4));
		star.innerLimit = 0.1 * star.mass;
		star.outerLimit = 40 * star.mass;
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

		for (SMpoint = star.innerLimit * genSM(mt_star); SMpoint < star.outerLimit; (SMpoint *= genSM(mt_star)))		//Set semimajor points
			star.semimajorList.push_back(SMpoint);
		star.semimajorStaticList = star.semimajorList; // keeps a copy of the original semimajor points

		star.maxPlanetNumber = star.semimajorList.size();
	}
	void GeneratePlanet(STAR& star, PLANET& planet)
	{
		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, CONFIG.SDEccentricity };
		std::normal_distribution<> geninclination{ CONFIG.avgInclination, CONFIG.SDInclination };
		planet.planetType = L"Planet";
		planet.name = GenName(typePlanet);
		planet.parentBody = star.name;

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
		if (CONFIG.smartPlacement == 1)
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
			//planet.class_ = L"Terra";
		//################################

		//######################################################################################################
			//	MASS + RADIUS GENERATION

		if (planet.class_ == L"Jupiter")
		{
			if (genpercent(mt_planet) <= 60)
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
				if (genpercent(mt_planet) <= 50)
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
				if (genpercent(mt_planet) <= 50)
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
				if (genpercent(mt_planet) <= 50)
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
				if (genpercent(mt_planet) <= 50)
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

		//######################################################################################################
			//	ORBIT GENERATION

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
		if (genpercent(mt_planet) <= CONFIG.exotic_OrbitChance)
		{
			if (genpercent(mt_planet) <= 50)
				planet.inclination += 180;
			else
			{
				std::normal_distribution<> genhighinclination{ 60, 10 };
				planet.inclination = genhighinclination(mt_planet);
			}
		}

		// Determines an exotic rotation
		if (genpercent(mt_planet) <= CONFIG.exotic_AxialTiltChance)
		{
			if (genpercent(mt_planet) <= 50)
				planet.obliquity += 180;
			else
			{
				std::normal_distribution<> genhighobliquity{ 90, 20 };
				planet.obliquity = genhighobliquity(mt_planet);
			}
		}

		planet.debrisCount = 0;
		// Determines Debris Ring
		if (genpercent(mt_planet) <= CONFIG.exotic_DebrisRingChance)
		{
			std::uniform_int_distribution<> GenDebrisNumber{ 15, 60 };
			planet.debrisCount = GenDebrisNumber(mt_planet);
		}

		planet.life_exotic.haslife = false;
		planet.life_exotic.panspermia = false;
		planet.life_organic.haslife = false;
		planet.life_organic.panspermia = false;
		// Determines Life
		if (CONFIG.traditionalLife)
		{
			if (planet.semimajorAxis > star.habitZoneInnerLimit && planet.semimajorAxis < star.habitZoneOuterLimit
					&& planet.class_ != L"Jupiter" && planet.class_ != L"Neptune"
					&& genpercent(mt_planet) <= CONFIG.life_OrganicChance)
				planet.life_organic.haslife = true;

			if (genpercent(mt_planet) <= CONFIG.life_ExoticChance)
				planet.life_exotic.haslife = true;
		}
		else
		{
			if (genpercent(mt_planet) <= CONFIG.life_OrganicChance)
				planet.life_organic.haslife = true;
			if (genpercent(mt_planet) <= CONFIG.life_ExoticChance)
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
			planet.majorMoonPercent = ceil(((planet.semimajorAxis - star.innerLimit) / star.totalDist) * 100); // the relative disatnce to the oyter limit
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
				else if (planet.planetType == L"DwarfPlanet")
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
				else if (planet.planetType == L"DwarfPlanet")
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
			planet.numberOfMajorMoons = -1;
			planet.numberOfMinorMoons = -1;
			planet.majorMoonPercent = -1;
			planet.minorMoonPercent = -1;
		}
	}
	void GenerateDwarfPlanet(STAR& star, PLANET& planet)
	{
		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		planet.planetType = L"DwarfPlanet";
		planet.name = GenName(typePlanet);
		planet.parentBody = star.name;

		//######################################################################################################
			//	CLASS / RADIUS GENERATION

		if (genpercent(mt_planet) <= 50)
			planet.class_ = L"Terra";
		else
			planet.class_ = L"Aquaria";

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

		//######################################################################################################
			//	semimajorAxis GENERATION

		if (CONFIG.smartPlacement == 1)
		{
			//Class is Selena...
			if (planet.class_ == L"Terra")
			{
				int percent = genpercent(mt_planet);
				if (percent <= 40)
				{
					std::uniform_real_distribution<> gensemimajorAxis{ (star.outerLimit / 1.5), (star.outerLimit * 3) };
					planet.semimajorAxis = gensemimajorAxis(mt_planet);
				}
				else if (percent >= 60)
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
			// Class is Ice World...
			else if (star.frostLine > star.outerLimit) // for large stars like O
			{
				std::uniform_real_distribution<> gensemimajorAxis{ (star.frostLine / 1.2), (star.frostLine * 3) };
				planet.semimajorAxis = gensemimajorAxis(mt_planet);
			}
			else if (star.frostLine < star.innerLimit) // for some Q class and low luminosity
			{
				int percent = genpercent(mt_planet);
				if (percent <= 60)
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
				if (percent <= 60)
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

		planet.obliquity = genobliquity(mt_planet);
		planet.ascendingNode = gendegree(mt_planet);
		planet.argofPericenter = gendegree(mt_planet);
		planet.meanAnomaly = gendegree(mt_planet);

		//######################################################################################################
			//	MOON NUMBER GENERATION

		planet.hillSphereOuterLimit = planet.semimajorAxis * pow((planet.mass / star.mass), (1.0 / 3.0)) * 235;
		planet.hillSphereOuterLimit *= planet.radius;

		// Hill Sphere Caps
		if (planet.hillSphereOuterLimit > 1495980)
			planet.hillSphereOuterLimit = 1495980; // hillSphereOuterLimit Limit Capped at 0.01 AU

		planet.numberOfMajorMoons = 0;

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

		// if hill sphere inner is larger than outer
		if ((2.44 * planet.radius * pow((planet.density / 0.5), (1.0 / 3.0))) > planet.hillSphereOuterLimit)
			planet.numberOfMinorMoons = 0; 
	}
	void GenerateMajorMoon(STAR& star, PLANET& parent, PLANET& moon, int spacer)
	{
		int testsemi;
		moon.planetType = L"Moon";
		moon.parentBody = parent.name;

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
			//	MASS / RADIUS GENERATION

		if (parent.mass >= 320)
		{
			if (moon.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.3 };
				std::uniform_real_distribution<> genr{ 3000, 12000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.1, 1 };
				std::uniform_real_distribution<> genr{ 500, 12000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Ferria")
			{
				std::uniform_real_distribution<> gend{ 1, 2 };
				std::uniform_real_distribution<> genr{ 500, 12000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::uniform_real_distribution<> gend{ 0.5, 1 };
				std::uniform_real_distribution<> genr{ 3000, 12000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
		}
		else if (parent.mass >= 100)
		{
			if (moon.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.3 };
				std::uniform_real_distribution<> genr{ 1000, 4500 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.1, 1 };
				std::uniform_real_distribution<> genr{ 500, 4500 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Ferria")
			{
				std::uniform_real_distribution<> gend{ 1, 2 };
				std::uniform_real_distribution<> genr{ 500, 4500 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::uniform_real_distribution<> gend{ 0.5, 1 };
				std::uniform_real_distribution<> genr{ 1000, 4500 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
		}
		else if (parent.mass >= 30)
		{
			if (moon.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.3 };
				std::uniform_real_distribution<> genr{ 500, 3000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.1, 1 };
				std::uniform_real_distribution<> genr{ 500, 3000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Ferria")
			{
				std::uniform_real_distribution<> gend{ 1, 2 };
				std::uniform_real_distribution<> genr{ 500, 3000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::uniform_real_distribution<> gend{ 0.5, 1 };
				std::uniform_real_distribution<> genr{ 500, 3000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
		}
		else
		{
			if (moon.class_ == L"Terra")
			{
				std::uniform_real_distribution<> gend{ 0.7, 1.3 };
				std::uniform_real_distribution<> genr{ 500, 2000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Aquaria")
			{
				std::uniform_real_distribution<> gend{ 0.1, 1 };
				std::uniform_real_distribution<> genr{ 500, 2000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Ferria")
			{
				std::uniform_real_distribution<> gend{ 1, 2 };
				std::uniform_real_distribution<> genr{ 500, 2000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
			else if (moon.class_ == L"Carbonia")
			{
				std::uniform_real_distribution<> gend{ 0.5, 1 };
				std::uniform_real_distribution<> genr{ 500, 2000 };

				moon.density = gend(mt_moon);
				moon.radius = genr(mt_moon);
			}
		}

		moon.earthRadius = moon.radius / 6378.14;
		moon.mass = pow(moon.earthRadius, 3) * moon.density;
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
			testsemi = 0;

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
			}
			else
			{
				std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, parent.hillSphereOuterLimit };
				moon.semimajorAxis = gensemi(mt_moon);
			}

			// Checks the semi major against others to make sure no moons are too close. if they are, it re-gens.
			for (int count = 0; count < parent.usedSemimajor_moon.size(); count++)
			{
				if ((moon.semimajorAxis - (moon.radius * 2)) < (parent.usedSemimajor_moon.at(count) + (parent.usedRadius_moon.at(count) * 2)) &&
					(moon.semimajorAxis + (moon.radius * 2)) > (parent.usedSemimajor_moon.at(count) - (parent.usedRadius_moon.at(count) * 2)))
					testsemi = 1; // means moons were too close
				else if (testsemi != 1)
					testsemi = 0;
			}
		} while (testsemi == 1);
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

		moon.ascendingNode = gendegree(mt_moon);
		moon.argofPericenter = gendegree(mt_moon);
		moon.meanAnomaly = gendegree(mt_moon);

		//######################################################################################################
			//	EXOTIC GENERATION

		// Determines an exotic orbit
		if (genpercent(mt_moon) <= CONFIG.exotic_OrbitChance)
			moon.inclination += 180;

		// Determines an exotic rotation
		if (genpercent(mt_moon) <= CONFIG.exotic_AxialTiltChance)
		{
			if (genpercent(mt_moon) <= 50)
				moon.obliquity += 180;
			else
			{
				std::normal_distribution<> genhighobliquity{ 90, 20 };
				moon.obliquity = genhighobliquity(mt_moon);
			}
		}

		moon.life_exotic.haslife = false;
		moon.life_exotic.panspermia = false;
		moon.life_organic.haslife = false;
		moon.life_organic.panspermia = false;
		// Determines Life
		if (CONFIG.traditionalLife)
		{
			if (parent.semimajorAxis > star.habitZoneInnerLimit && parent.semimajorAxis < star.habitZoneOuterLimit
				&& moon.radius > 1000
				&& genpercent(mt_moon) <= CONFIG.life_OrganicChance)

				moon.life_organic.haslife = true;
			if (moon.radius > 1000 && genpercent(mt_moon) <= CONFIG.life_ExoticChance)
				moon.life_exotic.haslife = true;
		}
		else
		{
			if (genpercent(mt_moon) <= CONFIG.life_OrganicChance)
				moon.life_organic.haslife = true;
			if (genpercent(mt_moon) <= CONFIG.life_ExoticChance)
				moon.life_exotic.haslife = true;
		}
		ExoticGenerateLife(moon);
	}
	void GenerateMinorMoon(PLANET& parent, PLANET& moon, int currentMoon)
	{
		int astSize, testsemi;
		moon.planetType = L"DwarfMoon";
		moon.class_ = L"Asteroid";
		moon.parentBody = parent.name;

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

		astSize = genpercent(mt_moon);
		if (parent.class_ == L"Jupiter" || parent.class_ == L"Neptune")
		{
			if (astSize <= 65)
			{
				std::uniform_real_distribution<> genr{ 0.5, 30 };
				moon.radius = genr(mt_moon);
			}
			else if (astSize <= 80)
			{
				std::uniform_real_distribution<> genr{ 30, 80 };
				moon.radius = genr(mt_moon);
			}
			else if (astSize <= 95)
			{
				std::uniform_real_distribution<> genr{ 80, 120 };
				moon.radius = genr(mt_moon);
			}
			else
			{
				std::uniform_real_distribution<> genr{ 120, 200 };
				moon.radius = genr(mt_moon);
			}
		}
		else
		{
			if (astSize <= 80)
			{
				std::uniform_real_distribution<> genr{ 0.3, 15 };
				moon.radius = genr(mt_moon);
			}
			else if (astSize <= 95)
			{
				std::uniform_real_distribution<> genr{ 15, 50 };
				moon.radius = genr(mt_moon);
			}
			else
			{
				std::uniform_real_distribution<> genr{ 50, 80 };
				moon.radius = genr(mt_moon);
			}
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
				if (genpercent(mt_moon) <= 10)
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
				if (genpercent(mt_moon) <= 40)
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

		moon.ascendingNode = gendegree(mt_moon);
		moon.argofPericenter = gendegree(mt_moon);
		moon.meanAnomaly = gendegree(mt_moon);
	}
	void GenerateDwarfMinor(PLANET& parent, PLANET& moon, int currentMoon)
	{
		int astSize;
		moon.planetType = L"DwarfMoon";
		moon.class_ = L"Asteroid";
		moon.parentBody = parent.name;

		if (NV.nameTerraDwarfMoons)
			moon.name = GenName(typeDwarfMoon);
		else
		{
			moon.name = L"Minor Moon ";
			moon.name += std::to_wstring(currentMoon + 1);
		}

		//######################################################################################################
			//	RADIUS GENERATION

		astSize = genpercent(mt_moon);

		if (astSize <= 80)
		{
			std::uniform_real_distribution<> genr{ 0.3, 15 };
			moon.radius = genr(mt_moon);
		}
		else if (astSize <= 95)
		{
			std::uniform_real_distribution<> genr{ 15, 50 };
			moon.radius = genr(mt_moon);
		}
		else
		{
			std::uniform_real_distribution<> genr{ 50, 100 };
			moon.radius = genr(mt_moon);
		}


		//######################################################################################################
			//	SEMI MAJOR GENERATION

		moon.hillSphereInnerLimit = 2.44 * pow((parent.density / 1), (1.0 / 3.0));
		moon.hillSphereInnerLimit *= parent.radius;

		if (moon.hillSphereInnerLimit < parent.hillSphereOuterLimit)
		{
			if (genpercent(mt_moon) <= 50)
			{
				std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, moon.hillSphereInnerLimit * 3 };
				moon.semimajorAxis = gensemi(mt_moon);
			}
			else
			{
				std::uniform_real_distribution<> gensemi{ moon.hillSphereInnerLimit, parent.hillSphereOuterLimit };
				moon.semimajorAxis = gensemi(mt_moon);
			}
		}
		else
		{
			std::uniform_real_distribution<> gensemi{ parent.hillSphereOuterLimit, parent.hillSphereOuterLimit * 5 };
			moon.semimajorAxis = gensemi(mt_moon);
		}

		//######################################################################################################
			//	ORBIT GENERATION

		if (moon.semimajorAxis < moon.hillSphereInnerLimit * 3)
		{
			// Set 5
			std::normal_distribution<> genobliquity{ 0, 5 };
			std::normal_distribution<> geneccentricity{ 0.1, 0.05 };
			std::normal_distribution<> geninclination{ 0, 5 };

			moon.obliquity = genobliquity(mt_moon);
			do moon.eccentricity = geneccentricity(mt_moon);
			while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
			moon.inclination = geninclination(mt_moon);
		}
		else
		{
			// Set 6
			std::normal_distribution<> genobliquity{ 0, 10 };
			std::normal_distribution<> geneccentricity{ 0.1, 0.15 };
			std::normal_distribution<> geninclination{ 0, 40 };

			moon.obliquity = genobliquity(mt_moon);
			do moon.eccentricity = geneccentricity(mt_moon);
			while (moon.eccentricity <= 0 || moon.eccentricity >= 1);
			moon.inclination = geninclination(mt_moon);
		}

		moon.ascendingNode = gendegree(mt_moon);
		moon.argofPericenter = gendegree(mt_moon);
		moon.meanAnomaly = gendegree(mt_moon);
	}

	void ExoticGenerateLife(PLANET& body)
	{
		if (body.life_organic.haslife)
		{
			body.life_organic._class = L"Organic";

			if (genpercent(mt_planet) <= CONFIG.life_MulticellChance)
				body.life_organic.type = L"Multicellular";
			else
				body.life_organic.type = L"Unicellular";

			if (genpercent(mt_planet) <= 50)
				body.life_organic.panspermia = true;
		}

		if (body.life_exotic.haslife)
		{
			body.life_exotic._class = L"Exotic";

			if (genpercent(mt_planet) <= CONFIG.life_MulticellChance)
				body.life_exotic.type = L"Multicellular";
			else
				body.life_exotic.type = L"Unicellular";

			if (genpercent(mt_planet) <= 50)
				body.life_exotic.panspermia = true;
		}
	}
	void ExoticDebrisRing(PLANET& parent, PLANET& debris, double inclinationationCenter, double inclinationationSD, double DebrisSpread, double SMCenterPoint)
	{
		debris.name = L"Debris Moon";

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

		debris.ascendingNode = gendegree(mt_moon);
		debris.argofPericenter = gendegree(mt_moon);
		debris.meanAnomaly = gendegree(mt_moon);
	}
	void GenerateShip(PLANET& ship, double min_dist, double max_dist, double eccent_avg, double eccent_sd, double inclin_avg, double inclin_sd)
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
