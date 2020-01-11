#include "framework.h"
#include "SolarSystemGenerator2.0.h"
#include "ConfigStructs.cpp"
#include "Structs.cpp"
#include "MiscFunctions.cpp"

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

std::mt19937 mt_star, mt_planet, mt_moon, mt_ships;
std::uniform_int_distribution<int> genpercent{ 1, 100 };
std::uniform_real_distribution<> gendegree{ 0, 360 };

/*---------------------------------------------------------------------------------------#
|	Function Declaration:																			 |
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
	void SavePreset(HWND);

	void Clear_Screen();
	void Load_Screen_General();
	void Load_Screen_SystemPlanet();
	void Load_Screen_Life();
	void Load_Screen_Ships();
	void Load_Screen_Exotic();
	void Load_Screen_Advanced();

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

	void SetInfoBox(int);
	void SetCheckBoxText(HWND, int); /*
	Window Functions
	//int OpenPreset();
	
#######################################################
	*/	void BeginGenerate(); 
		void GenerateStar(STAR&);
		void PrintStar(STAR&, std::ofstream&);
		void GeneratePlanet(STAR&, PLANET&); /*
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
				SavePreset(hWnd);
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


			case 69:

				Clear_Screen();
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
			Clear_Screen();
			//Load_Screen_General();
			//Load_Screen_SystemPlanet();
			//Load_Screen_Life();
			//Load_Screen_Ships();
			//Load_Screen_Exotic();
			Load_Screen_Advanced();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
	/*
	void LoadImages();
	void LoadPresets(HWND);
	void LoadMainScreen(HWND);

	void UpdatePreset(Preset, HWND);
	void SavePreset(HWND);

	void Clear_Screen();
	void Load_Screen_General();
	void Load_Screen_SystemPlanet();
	void Load_Screen_Life();
	void Load_Screen_Ships();
	void Load_Screen_Exotic();
	void Load_Screen_Advanced();

	void GetConfigData(HWND);

	void SetInfoBox(int);
	void SetCheckBoxText(HWND, int);
	*/

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

		for (int i = 0; i < addresses.size(); i++)
		{
			HANDLE presetFile = CreateFileW(&addresses.at(i)[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			Preset empty;
			preset.push_back(empty);

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
			wchar_t holder, numholder[16];
			int x = 0;

			//name
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				preset.at(i).name[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			preset.at(i).name[x - 2] = '\0';
			x = 0;

			//star output folder
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				preset.at(i).starOutputFolder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			preset.at(i).starOutputFolder[x - 2] = '\0';
			x = 0;

			//planet output folder
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				preset.at(i).planetOutputFolder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			preset.at(i).planetOutputFolder[x - 2] = '\0';
			x = 0;

			//seed
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).seed = _wtoi(numholder);
			x = 0;

			//number of runs
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).numberOfRuns = _wtoi(numholder);
			x = 0;

			//smart placement
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).smartPlacement = _wtoi(&holder);
			parse++;
			preset.at(i).smartPlacementState = (preset.at(i).smartPlacement == true) ? L"Enabled" : L"Disabled";

			//min planet number
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).minPlanetNumber = _wtoi(numholder);
			x = 0;

			//min distance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).minDistance = _wtof(numholder);
			x = 0;

			//max distance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).maxDistance = _wtof(numholder);
			x = 0;

			//planet spacing
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).planetSpacing = _wtof(numholder);
			x = 0;

			//generate dwarf planets
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).generateDwarfPlanets = _wtoi(&holder);
			parse++;
			preset.at(i).generateDwarfPlanetsState = (preset.at(i).generateDwarfPlanets == true) ? L"Enabled" : L"Disabled";

			//dwarf planet chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).dwarfPlanetChance = _wtoi(numholder);
			x = 0;

			//average eccentricity
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).avgEccentricity = _wtof(numholder);
			x = 0;

			//SD eccentricity
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).SDEccentricity = _wtof(numholder);
			x = 0;

			//average inclination
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).avgInclination = _wtof(numholder);
			x = 0;

			//SD inclination
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).SDInclination = _wtof(numholder);
			x = 0;

			//average obliquity
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).avgObliquity = _wtof(numholder);
			x = 0;

			//SD obliquity
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).SDObliquity = _wtof(numholder);
			x = 0;

			//star class O
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassO = _wtoi(&holder);
			parse++;

			//star class B
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassB = _wtoi(&holder);
			parse++;

			//star class A
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassA = _wtoi(&holder);
			parse++;

			//star class F
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassF = _wtoi(&holder);
			parse++;

			//star class G
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassG = _wtoi(&holder);
			parse++;

			//star class K
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassK = _wtoi(&holder);
			parse++;

			//star class M
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassM = _wtoi(&holder);
			parse++;

			//star class WD
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassWD = _wtoi(&holder);
			parse++;

			//star class Q
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassQ = _wtoi(&holder);
			parse++;

			//star class X
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).starClassX = _wtoi(&holder);
			parse++;

			//life organic chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).life_OrganicChance = _wtoi(numholder);
			x = 0;

			//life exotic chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).life_ExoticChance = _wtoi(numholder);
			x = 0;

			//life multicell chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).life_MulticellChance = _wtoi(numholder);
			x = 0;

			//force life
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).forceLife = _wtoi(&holder);
			parse++;
			preset.at(i).forceLifeState = (preset.at(i).forceLife == true) ? L"Enabled" : L"Disabled";

			//traditional life
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).traditionalLife = _wtoi(&holder);
			parse++;
			preset.at(i).traditonalLifeState = (preset.at(i).traditionalLife == true) ? L"Enabled" : L"Disabled";

			//ship chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).exotic_ShipChance = _wtoi(numholder);
			x = 0;

			//models folder
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				preset.at(i).modelsFolder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			preset.at(i).modelsFolder[x - 2] = '\0';
			x = 0;

			//ships need life life
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			preset.at(i).shipsNeedLife = _wtoi(&holder);
			parse++;
			preset.at(i).shipsNeedLifeState = (preset.at(i).shipsNeedLife == true) ? L"Enabled" : L"Disabled";

			//exotic orbit chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).exotic_OrbitChance = _wtoi(numholder);
			x = 0;

			//exotic axial tilt chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).exotic_AxialTiltChance = _wtoi(numholder);
			x = 0;

			//exotic debris ring chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).exotic_DebrisRingChance = _wtoi(numholder);
			x = 0;

			//exotic companion chance
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			preset.at(i).exotic_CompanionOrbitChance = _wtoi(numholder);
			x = 0;

			//debug
			holder = Buffer[parse];
			preset.at(i).debug = _wtoi(&holder);
			preset.at(i).debugState = (preset.at(i).debug == true) ? L"Enabled" : L"Disabled";

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
			WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			760, 240, 200, 500,
			hWnd, NULL, hInst, NULL);

		CONFIG.namePresetDropDown.DESC = CreateWindowW(L"static", L"Select a Name Preset:",
			WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
			760, 220, 180, 20,
			hWnd, NULL, hInst, NULL);

		CONFIG.namePresetDropDown.INFOBUTTON = CreateWindowW(L"button", L"I",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			942, 222, 16, 16,
			hWnd, (HMENU)IB_SELECTNAMEPRESET, NULL, NULL);


		for (int i = 0; i < addresses.size(); i++)
		{
			HANDLE presetFile = CreateFileW(&addresses.at(i)[0], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			NamePreset empty;
			npreset.push_back(empty);

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
			wchar_t holder, numholder[16];
			int x = 0;






			//name
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				npreset.at(i).name[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			npreset.at(i).name[x - 2] = '\0';
			x = 0;

			//simple generator
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useSimpleGenerator = _wtoi(&holder);
			parse++;
			//prefix list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).PrefixList[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			} 
			npreset.at(i).PrefixList[x] = '\0';
			x = 0;
			//suffix list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).SuffixList[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).SuffixList[x] = '\0';
			x = 0;

			//use star pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useStarPreMods = _wtoi(&holder);
			parse++;
			//prob star pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probStarPreMod = _wtoi(numholder);
			x = 0;
			//star pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).StarPreMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).StarPreMods[x] = '\0';
			x = 0;
			//use star post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useStarPostMods = _wtoi(&holder);
			parse++;
			//prob star post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probStarPostMod = _wtoi(numholder);
			x = 0;
			//star pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).StarPostMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).StarPostMods[x] = '\0';
			x = 0;
			//use star number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useStarNumberMods = _wtoi(&holder);
			parse++;
			//prob star number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probStarNumberMod = _wtoi(numholder);
			x = 0;

			//use planet pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).usePlanetPreMods = _wtoi(&holder);
			parse++;
			//prob planet pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probPlanetPreMod = _wtoi(numholder);
			x = 0;
			//planet pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).PlanetPreMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).PlanetPreMods[x] = '\0';
			x = 0;
			//use planet post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).usePlanetPostMods = _wtoi(&holder);
			parse++;
			//prob planet post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probPlanetPostMod = _wtoi(numholder);
			x = 0;
			//planet pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).PlanetPostMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).PlanetPostMods[x] = '\0';
			x = 0;
			//use planet number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).usePlanetNumberMods = _wtoi(&holder);
			parse++;
			//prob planet number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probPlanetNumberMod = _wtoi(numholder);
			x = 0;

			//name terra moons
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).nameTerraMoons = _wtoi(&holder);
			parse++;
			//name gas moons
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).nameGasMoons = _wtoi(&holder);
			parse++;
			//use moon pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useMoonPreMods = _wtoi(&holder);
			parse++;
			//prob moon pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probMoonPreMod = _wtoi(numholder);
			x = 0;
			//moon pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).MoonPreMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).MoonPreMods[x] = '\0';
			x = 0;
			//use moon post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useMoonPostMods = _wtoi(&holder);
			parse++;
			//prob moon post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probMoonPostMod = _wtoi(numholder);
			x = 0;
			//moon pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).MoonPostMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).MoonPostMods[x] = '\0';
			x = 0;
			//use moon number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useMoonNumberMods = _wtoi(&holder);
			parse++;
			//prob moon number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probMoonNumberMod = _wtoi(numholder);
			x = 0;

			//name terra dwarf moons
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).nameTerraDwarfMoons = _wtoi(&holder);
			parse++;
			//name gas dwarf moons
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).nameGasDwarfMoons = _wtoi(&holder);
			parse++;
			//use dwarf moon pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useDwarfMoonPreMods = _wtoi(&holder);
			parse++;
			//prob dwarf moon pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probDwarfMoonPreMod = _wtoi(numholder);
			x = 0;
			//dwarf moon pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).DwarfMoonPreMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).DwarfMoonPreMods[x] = '\0';
			x = 0;
			//use dwarf moon post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useDwarfMoonPostMods = _wtoi(&holder);
			parse++;
			//prob dwarf moon post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probDwarfMoonPostMod = _wtoi(numholder);
			x = 0;
			//dwarf moon pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).DwarfMoonPostMods[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).DwarfMoonPostMods[x] = '\0';
			x = 0;
			//use dwarf moon number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useDwarfMoonNumberMods = _wtoi(&holder);
			parse++;
			//prob dwarf moon number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probDwarfMoonNumberMod = _wtoi(numholder);
			x = 0;

			//use ship all pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPreMods_All = _wtoi(&holder);
			parse++;
			//prob ship all pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPreMod_All = _wtoi(numholder);
			x = 0;
			//ship all pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPreMods_All[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPreMods_All[x] = '\0';
			x = 0;
			//use ship all post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPostMods_All = _wtoi(&holder);
			parse++;
			//prob ship all post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPostMod_All = _wtoi(numholder);
			x = 0;
			//ship all pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPostMods_All[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPostMods_All[x] = '\0';
			x = 0;

			//use ship colony pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPreMods_Colony = _wtoi(&holder);
			parse++;
			//prob ship colony pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPreMod_Colony = _wtoi(numholder);
			x = 0;
			//ship colony pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPreMods_Colony[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPreMods_Colony[x] = '\0';
			x = 0;
			//use ship colony post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPostMods_Colony = _wtoi(&holder);
			parse++;
			//prob ship colony post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPostMod_Colony = _wtoi(numholder);
			x = 0;
			//ship colony pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPostMods_Colony[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPostMods_Colony[x] = '\0';
			x = 0;
			//use ship colony number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipNumberMods_Colony = _wtoi(&holder);
			parse++;
			//prob ship colony post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipNumberMod_Colony = _wtoi(numholder);
			x = 0;

			//use ship instrument pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPreMods_Instrument = _wtoi(&holder);
			parse++;
			//prob ship instrument pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPreMod_Instrument = _wtoi(numholder);
			x = 0;
			//ship instrument pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPreMods_Instrument[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPreMods_Instrument[x] = '\0';
			x = 0;
			//use ship instrument post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPostMods_Instrument = _wtoi(&holder);
			parse++;
			//prob ship instrument post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPostMod_Instrument = _wtoi(numholder);
			x = 0;
			//ship instrument pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPostMods_Instrument[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPostMods_Instrument[x] = '\0';
			x = 0;
			//use ship instrument number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipNumberMods_Instrument = _wtoi(&holder);
			parse++;
			//prob ship instrument post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipNumberMod_Instrument = _wtoi(numholder);
			x = 0;

			//use ship satellite pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPreMods_Satellite = _wtoi(&holder);
			parse++;
			//prob ship satellite pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPreMod_Satellite = _wtoi(numholder);
			x = 0;
			//ship satellite pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPreMods_Satellite[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPreMods_Satellite[x] = '\0';
			x = 0;
			//use ship satellite post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPostMods_Satellite = _wtoi(&holder);
			parse++;
			//prob ship satellite post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPostMod_Satellite = _wtoi(numholder);
			x = 0;
			//ship satellite pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPostMods_Satellite[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPostMods_Satellite[x] = '\0';
			x = 0;
			//use ship satellite number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipNumberMods_Satellite = _wtoi(&holder);
			parse++;
			//prob ship satellite post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipNumberMod_Satellite = _wtoi(numholder);
			x = 0;

			//use ship station pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPreMods_Station = _wtoi(&holder);
			parse++;
			//prob ship station pre mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPreMod_Station = _wtoi(numholder);
			x = 0;
			//ship station pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPreMods_Station[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPreMods_Station[x] = '\0';
			x = 0;
			//use ship station post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipPostMods_Station = _wtoi(&holder);
			parse++;
			//prob ship station post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipPostMod_Station = _wtoi(numholder);
			x = 0;
			//ship station pre mod list
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).ShipPostMods_Station[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).ShipPostMods_Station[x] = '\0';
			x = 0;
			//use ship station number mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			holder = Buffer[parse];
			npreset.at(i).useShipNumberMods_Station = _wtoi(&holder);
			parse++;
			//prob ship station post mod
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).probShipNumberMod_Station = _wtoi(numholder);
			x = 0;

			//order
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).order = _wtoi(numholder);
			x = 0;
			//word varience
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).wordVarience = _wtoi(numholder);
			x = 0;
			//min length
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).min_length = _wtoi(numholder);
			x = 0;
			//max length
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '='); //find =
			do
			{
				holder = Buffer[parse];
				numholder[x] = holder;
				parse++;
				x++;
			} while (holder != '\n');
			numholder[x] = '\0';
			npreset.at(i).max_length = _wtoi(numholder);
			x = 0;
			//dataset
			do
			{
				holder = Buffer[parse];
				parse++;
			} while (holder != '{'); //find {
			holder = Buffer[parse];
			while (holder != '}')
			{
				npreset.at(i).Markov_RawDataset[x] = holder;
				parse++;
				holder = Buffer[parse];
				x++;
			}
			npreset.at(i).Markov_RawDataset[x] = '\0';
			x = 0;

			SendMessage(CONFIG.namePresetDropDown.HANDLE, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)preset.at(i).name);

			CloseHandle(presetFile);
		}

		SendMessage(CONFIG.namePresetDropDown.HANDLE, (UINT)CB_SETCURSEL, (WPARAM)0, NULL);

		CONFIG.namePresetDropDown.EXTRA = CreateWindowW(L"button", L"Update",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
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

			//Star class Stuff
			//I used the O class's EXTRA to create the groupbox for all the classes,
			//therefore, it is the parent for all the class checkboxes
			CONFIG.starClassOH.EXTRA = CreateWindowW(L"button", L"Star Classes to Choose From:",
				WS_CHILD | BS_GROUPBOX,
				750, 100, 250, 170,
				hWnd, NULL, NULL, NULL);
			CONFIG.starClassOH.INFOBUTTON = CreateWindowW(L"button", L"I",
				WS_CHILD | WS_BORDER,
				960, 102, 16, 16,
				hWnd, (HMENU)IB_STARCLASS, NULL, NULL);

			CONFIG.starClassOH.HANDLE = CreateWindowW(L"button", L"O",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 20, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_OCLASS, NULL, NULL);
			CONFIG.starClassBH.HANDLE = CreateWindowW(L"button", L"B",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 40, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_BCLASS, NULL, NULL);
			CONFIG.starClassAH.HANDLE = CreateWindowW(L"button", L"A",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 60, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_ACLASS, NULL, NULL);
			CONFIG.starClassFH.HANDLE = CreateWindowW(L"button", L"F",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 80, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_FCLASS, NULL, NULL);
			CONFIG.starClassGH.HANDLE = CreateWindowW(L"button", L"G",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 100, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_GCLASS, NULL, NULL);
			CONFIG.starClassKH.HANDLE = CreateWindowW(L"button", L"K",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 120, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_KCLASS, NULL, NULL);
			CONFIG.starClassMH.HANDLE = CreateWindowW(L"button", L"M",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				10, 140, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_MCLASS, NULL, NULL);

			CONFIG.starClassWDH.HANDLE = CreateWindowW(L"button", L"WD",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				100, 20, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_WDCLASS, NULL, NULL);
			CONFIG.starClassQH.HANDLE = CreateWindowW(L"button", L"Q",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				100, 40, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_QCLASS, NULL, NULL);
			CONFIG.starClassXH.HANDLE = CreateWindowW(L"button", L"X",
				WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | BS_AUTOCHECKBOX,
				100, 60, 50, 20,
				CONFIG.starClassOH.EXTRA, (HMENU)CB_XCLASS, NULL, NULL);
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

				// all of the buttons for the name groups
				// the buttons are only VISUALLY inside the groupbox, but their
				// parent is still hWnd because they send messages to the
				// window procedure
				CONFIG.buttonUpdate = CreateWindowW(L"button", L"Update Names",
					WS_CHILD | WS_BORDER | BS_MULTILINE,
					550, 120, 70, 50,
					hWnd, (HMENU)BUTTON_NAME_UPDATE, NULL, NULL);
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
						WS_CHILD | WS_VISIBLE | WS_BORDER,
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
						500, 242, 16, 16,
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
						WS_CHILD | WS_VISIBLE | WS_BORDER,
						12, 80, 596, 130,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);

					NV.SuffixListDESC = CreateWindowW(L"static", L"Suffix List:",
						WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTER,
						10, 230, 600, 20,
						NV.GROUP_SIMPLE, NULL, NULL, NULL);
					NV.SuffixListH = CreateWindowW(L"edit", L"",
						WS_CHILD | WS_VISIBLE | WS_BORDER,
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
		wchar_t genVar[16];
		char genChar[26];

		SetWindowTextW(CONFIG.starOutputFolderH.HANDLE, P.starOutputFolder);
		SetWindowTextW(CONFIG.planetOutputFolderH.HANDLE, P.planetOutputFolder);
		_itow_s(P.seed, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.seedH.HANDLE, genVar);
		_itow_s(P.numberOfRuns, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.numberOfRunsH.HANDLE, genVar);
		CheckDlgButton(hWnd, CB_DEBUG, P.debug);
		SetWindowTextW(CONFIG.debugH.HANDLE, P.debugState);

		CheckDlgButton(hWnd, CB_SMARTPLACEMENT, P.smartPlacement);
		SetWindowTextW(CONFIG.smartPlacementH.HANDLE, P.smartPlacementState);
		_itow_s(P.minPlanetNumber, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.minPlanetNumberH.HANDLE, genVar);

		_gcvt_s(genChar, sizeof(genChar), P.minDistance, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.minDistanceH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.maxDistance, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.maxDistanceH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.planetSpacing, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.planetSpacingH.HANDLE, genVar);
		CheckDlgButton(hWnd, CB_GENERATEDWARFPLANET, P.generateDwarfPlanets);
		SetWindowTextW(CONFIG.generateDwarfPlanetsH.HANDLE, P.generateDwarfPlanetsState);
		_itow_s(P.dwarfPlanetChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.dwarfPlanetChanceH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.avgEccentricity, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.avgEccentricityH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.SDEccentricity, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.SDEccentricityH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.avgInclination, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.avgInclinationH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.SDInclination, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.SDInclinationH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.avgObliquity, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.avgObliquityH.HANDLE, genVar);
		_gcvt_s(genChar, sizeof(genChar), P.SDObliquity, 10);
		mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
		SetWindowTextW(CONFIG.SDObliquityH.HANDLE, genVar);

		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_OCLASS, P.starClassO);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_BCLASS, P.starClassB);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_ACLASS, P.starClassA);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_FCLASS, P.starClassF);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_GCLASS, P.starClassG);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_KCLASS, P.starClassK);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_MCLASS, P.starClassM);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_WDCLASS, P.starClassWD);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_QCLASS, P.starClassQ);
		CheckDlgButton(CONFIG.starClassOH.EXTRA, CB_XCLASS, P.starClassX);

		_itow_s(P.life_OrganicChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.life_OrganicChanceH.HANDLE, genVar);
		_itow_s(P.life_ExoticChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.life_ExoticChanceH.HANDLE, genVar);
		_itow_s(P.life_MulticellChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.life_MulticellChanceH.HANDLE, genVar);
		CheckDlgButton(hWnd, CB_FORCELIFE, P.forceLife);
		SetWindowTextW(CONFIG.forceLifeH.HANDLE, P.forceLifeState);
		CheckDlgButton(hWnd, CB_TRADITIONALLIFE, P.traditionalLife);
		SetWindowTextW(CONFIG.traditionalLifeH.HANDLE, P.traditonalLifeState);

		_itow_s(P.exotic_ShipChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.exotic_ShipChanceH.HANDLE, genVar);
		SetWindowTextW(CONFIG.modelsFolderH.HANDLE, P.modelsFolder);
		CheckDlgButton(hWnd, CB_SHIPSNEEDLIFE, P.shipsNeedLife);
		SetWindowTextW(CONFIG.shipsNeedLifeH.HANDLE, P.shipsNeedLifeState);

		_itow_s(P.exotic_OrbitChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.exotic_OrbitChanceH.HANDLE, genVar);
		_itow_s(P.exotic_AxialTiltChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.exotic_AxialTiltChanceH.HANDLE, genVar);
		_itow_s(P.exotic_DebrisRingChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.exotic_DebrisRingChanceH.HANDLE, genVar);
		_itow_s(P.exotic_CompanionOrbitChance, genVar, sizeof(genVar) / 2, 10);
		SetWindowTextW(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, genVar);
	}
	void SavePreset(HWND hWnd)
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


	void Clear_Screen()
	{
		/*----------------------------------------------------------
		This function clears all of the handles for every variable.
		-----------------------------------------------------------*/

		/*#####################################################
			General Variables
		#####################################################*/

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

		/*#####################################################
			System and Planet Variables
		#####################################################*/

		//Header
		ShowWindow(CONFIG.HEADER_SYSTEMPLANET, 0);

		//Handles
		ShowWindow(CONFIG.smartPlacementH.HANDLE, 0);
		ShowWindow(CONFIG.generateDwarfPlanetsH.HANDLE, 0);
		ShowWindow(CONFIG.dwarfPlanetChanceH.HANDLE, 0);
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

		ShowWindow(CONFIG.starClassAH.DESC, 0);
		ShowWindow(CONFIG.starClassBH.DESC, 0);
		ShowWindow(CONFIG.starClassFH.DESC, 0);
		ShowWindow(CONFIG.starClassGH.DESC, 0);
		ShowWindow(CONFIG.starClassKH.DESC, 0);
		ShowWindow(CONFIG.starClassMH.DESC, 0);
		ShowWindow(CONFIG.starClassOH.DESC, 0);
		ShowWindow(CONFIG.starClassQH.DESC, 0);
		ShowWindow(CONFIG.starClassWDH.DESC, 0);
		ShowWindow(CONFIG.starClassXH.DESC, 0);

		//Info
		ShowWindow(CONFIG.smartPlacementH.INFOBUTTON, 0);
		ShowWindow(CONFIG.generateDwarfPlanetsH.INFOBUTTON, 0);
		ShowWindow(CONFIG.dwarfPlanetChanceH.INFOBUTTON, 0);
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

		/*#####################################################
			Life Variables
		#####################################################*/

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

		/*#####################################################
			Ships Variables
		#####################################################*/

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

		/*#####################################################
			Exotic Variables
		#####################################################*/

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

		/*#####################################################
			Advanced Variables
		#####################################################*/

		//Header
		ShowWindow(CONFIG.HEADER_ADVANCED, 0);

		ShowWindow(CONFIG.advNameGroup.EXTRA, 0);

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
	}
	void Load_Screen_SystemPlanet()
	{
		//Header
		ShowWindow(CONFIG.HEADER_SYSTEMPLANET, 1);

		//Handles
		ShowWindow(CONFIG.smartPlacementH.HANDLE, 1);
		ShowWindow(CONFIG.generateDwarfPlanetsH.HANDLE, 1);
		ShowWindow(CONFIG.dwarfPlanetChanceH.HANDLE, 1);
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
	}
	void Load_Screen_Advanced()
	{
		//Header
		ShowWindow(CONFIG.HEADER_ADVANCED, 1);

		ShowWindow(CONFIG.advNameGroup.EXTRA, 1);

		ShowWindow(CONFIG.buttonUpdate, 1);
		ShowWindow(CONFIG.buttonStar, 1);
		ShowWindow(CONFIG.buttonPlanet, 1);
		ShowWindow(CONFIG.buttonMoon, 1);
		ShowWindow(CONFIG.buttonDwarfMoon, 1);
		ShowWindow(CONFIG.buttonShip, 1);
		ShowWindow(CONFIG.buttonDataset, 1);
		ShowWindow(CONFIG.buttonSimple, 1);
	}

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
		wchar_t genvar[WSIZE];
		int i = 0;

		GetWindowTextW(CONFIG.starOutputFolderH.HANDLE, genvar, WSIZE);
		while (genvar[i] != '\0')
		{
			CONFIG.starOutputFolder[i] = genvar[i];
			i++;
		}
		i = 0;
		GetWindowTextW(CONFIG.planetOutputFolderH.HANDLE, genvar, WSIZE);
		while (genvar[i] != '\0')
		{
			CONFIG.planetOutputFolder[i] = genvar[i];
			i++;
		}
		i = 0;
		GetWindowTextW(CONFIG.seedH.HANDLE, genvar, 20);
		CONFIG.seed = _wtoi(genvar);
		GetWindowTextW(CONFIG.numberOfRunsH.HANDLE, genvar, 20);
		CONFIG.numberOfRuns = _wtoi(genvar);
		CONFIG.smartPlacement = (IsDlgButtonChecked(hWnd, CB_SMARTPLACEMENT) == BST_CHECKED) ? true : false;

		GetWindowTextW(CONFIG.minPlanetNumberH.HANDLE, genvar, 20);
		CONFIG.minPlanetNumber = _wtoi(genvar);
		GetWindowTextW(CONFIG.minDistanceH.HANDLE, genvar, 20);
		CONFIG.minDistance = _wtof(genvar);
		GetWindowTextW(CONFIG.maxDistanceH.HANDLE, genvar, 20);
		CONFIG.maxDistance = _wtof(genvar);
		GetWindowTextW(CONFIG.planetSpacingH.HANDLE, genvar, 20);
		CONFIG.planetSpacing = _wtof(genvar);
		CONFIG.generateDwarfPlanets = (IsDlgButtonChecked(hWnd, CB_GENERATEDWARFPLANET) == BST_CHECKED) ? true : false;
		GetWindowTextW(CONFIG.dwarfPlanetChanceH.HANDLE, genvar, 20);
		CONFIG.dwarfPlanetChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.avgEccentricityH.HANDLE, genvar, 20);
		CONFIG.avgEccentricity = _wtof(genvar);
		GetWindowTextW(CONFIG.SDEccentricityH.HANDLE, genvar, 20);
		CONFIG.SDEccentricity = _wtof(genvar);
		GetWindowTextW(CONFIG.avgInclinationH.HANDLE, genvar, 20);
		CONFIG.avgInclination = _wtof(genvar);
		GetWindowTextW(CONFIG.SDInclinationH.HANDLE, genvar, 20);
		CONFIG.SDInclination = _wtof(genvar);
		GetWindowTextW(CONFIG.avgObliquityH.HANDLE, genvar, 20);
		CONFIG.avgObliquity = _wtof(genvar);
		GetWindowTextW(CONFIG.SDObliquityH.HANDLE, genvar, 20);
		CONFIG.SDObliquity = _wtof(genvar);

		CONFIG.starClassO = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_OCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassB = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_BCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassA = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_ACLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassF = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_FCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassG = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_GCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassK = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_KCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassM = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_MCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassWD = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_WDCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassQ = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_QCLASS) == BST_CHECKED) ? true : false;
		CONFIG.starClassX = (IsDlgButtonChecked(CONFIG.starClassOH.EXTRA, CB_XCLASS) == BST_CHECKED) ? true : false;
		if (CONFIG.starClassO)
			CONFIG.classList.push_back(L"O");
		if (CONFIG.starClassB)
			CONFIG.classList.push_back(L"B");
		if (CONFIG.starClassA)
			CONFIG.classList.push_back(L"A");
		if (CONFIG.starClassF)
			CONFIG.classList.push_back(L"F");
		if (CONFIG.starClassG)
			CONFIG.classList.push_back(L"G");
		if (CONFIG.starClassK)
			CONFIG.classList.push_back(L"K");
		if (CONFIG.starClassM)
			CONFIG.classList.push_back(L"M");
		if (CONFIG.starClassWD)
			CONFIG.classList.push_back(L"WD");
		if (CONFIG.starClassQ)
			CONFIG.classList.push_back(L"Q");
		if (CONFIG.starClassX)
			CONFIG.classList.push_back(L"X");

		GetWindowTextW(CONFIG.life_OrganicChanceH.HANDLE, genvar, 20);
		CONFIG.life_OrganicChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.life_ExoticChanceH.HANDLE, genvar, 20);
		CONFIG.life_ExoticChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.life_MulticellChanceH.HANDLE, genvar, 20);
		CONFIG.life_MulticellChance = _wtoi(genvar);
		CONFIG.forceLife = (IsDlgButtonChecked(hWnd, CB_FORCELIFE) == BST_CHECKED) ? true : false;
		CONFIG.traditionalLife = (IsDlgButtonChecked(hWnd, CB_TRADITIONALLIFE) == BST_CHECKED) ? true : false;

		GetWindowTextW(CONFIG.exotic_ShipChanceH.HANDLE, genvar, 20);
		CONFIG.exotic_ShipChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.modelsFolderH.HANDLE, genvar, WSIZE);
		while (genvar[i] != '\0')
		{
			CONFIG.modelsFolder[i] = genvar[i];
			i++;
		}
		i = 0;
		CONFIG.shipsNeedLife = (IsDlgButtonChecked(hWnd, CB_SHIPSNEEDLIFE) == BST_CHECKED) ? true : false;

		GetWindowTextW(CONFIG.exotic_OrbitChanceH.HANDLE, genvar, 20);
		CONFIG.exotic_OrbitChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.exotic_AxialTiltChanceH.HANDLE, genvar, 20);
		CONFIG.exotic_AxialTiltChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.exotic_DebrisRingChanceH.HANDLE, genvar, 20);
		CONFIG.exotic_DebrisRingChance = _wtoi(genvar);
		GetWindowTextW(CONFIG.exotic_CompanionOrbitChanceH.HANDLE, genvar, 20);
		CONFIG.exotic_CompanionOrbitChance = _wtoi(genvar);
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
	|	THE GENERATOR:																			 |
	#---------------------------------------------------------------------------------------*/


	void BeginGenerate()
	{
		if (CONFIG.seed == 0)
		{
			std::mt19937 mtseed;
			mtseed.seed(time(0));
			std::uniform_int_distribution<int> genseed{ 0, 2147483647 };
			CONFIG.seed = genseed(mtseed);
		}

		mt_star.seed(CONFIG.seed);
		mt_planet.seed(CONFIG.seed);
		mt_moon.seed(CONFIG.seed);
		mt_ships.seed(CONFIG.seed);






		STAR currentStar;
		GenerateStar(currentStar);

		std::wstring starFileName = CONFIG.starOutputFolder;	//Creates the star file
		starFileName += currentStar.name;
		starFileName += L" Star.sc";
		//starFileName += L"Test Star.sc";
		std::ofstream starFile(starFileName.c_str());
		PrintStar(currentStar, starFile);

		std::uniform_int_distribution<int> genplanetnum{ CONFIG.minPlanetNumber, currentStar.maxPlanetNumber };
		int planetNumber = genplanetnum(mt_planet);
		std::vector<PLANET> planetList(planetNumber);
		
		for (int currentPlanet = 0; currentPlanet < planetNumber; currentPlanet++)
		{
			planetList.at(currentPlanet).planetType = L"Planet";
			GeneratePlanet(currentStar, planetList.at(currentPlanet));

			/*#########################################
				Used fopr temporary planet names	
			#########################################*/
			wchar_t temp[3];
			_itow_s(currentPlanet + 1, temp, 10);

			planetList.at(currentPlanet).name += temp;
			/*########################################*/
		}
		while (genpercent(mt_planet) < CONFIG.dwarfPlanetChance)
		{
			//Dwarf Planets will be generated here
		}


		starFile.close();
		return;
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
		star.name = L"test star";

		//######################################################################################################
			//	CLASS GENERATION

		CONFIG.classListSize = CONFIG.classList.size();
		std::uniform_int_distribution<int> genclass{ 0, (CONFIG.classListSize - 1) };
		star.class_ = CONFIG.classList.at(genclass(mt_star));
		
		
		if (star.class_ == L"M")
		{
			std::normal_distribution<> genm{ 0.265, 0.185 };
			std::normal_distribution<> genr{ 0.5, 0.2 };
			std::uniform_int_distribution<int> gent{ 2200, 3800 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.07);

			do star.radius = genr(mt_star);
			while (star.radius <= 0);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"K")
		{
			std::normal_distribution<> genm{ 0.625, 0.175 };
			std::normal_distribution<> genr{ 0.83, 0.13 };
			std::uniform_int_distribution<int> gent{ 3700, 5300 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.08);

			do star.radius = genr(mt_star);
			while (star.radius <= 0);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"G")
		{
			std::normal_distribution<> genm{ 0.92, 0.12 };
			std::normal_distribution<> genr{ 1.055, 0.095 };
			std::uniform_int_distribution<int> gent{ 5200, 6100 };
			
			do star.mass = genm(mt_star);
			while (star.mass <= 0.45);

			do star.radius = genr(mt_star);
			while (star.radius <= 0.7);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"F")
		{
			std::normal_distribution<> genm{ 1.22, 0.18 };
			std::normal_distribution<> genr{ 1.275, 0.125 };
			std::uniform_int_distribution<int> gent{ 6000, 7600 };

			do star.mass = genm(mt_star);
			while (star.mass <= 0.8);

			do star.radius = genr(mt_star);
			while (star.radius <= 0.96);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"A")
		{
			std::normal_distribution<> genm{ 1.75, 0.35 };
			std::normal_distribution<> genr{ 1.6, 0.2 };
			std::uniform_int_distribution<int> gent{ 7500, 11000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 1.04);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.15);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"B")
		{
			std::normal_distribution<> genm{ 9.05, 6.95 };
			std::normal_distribution<> genr{ 4.2, 2.4 };
			std::uniform_int_distribution<int> gent{ 10000, 31000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 1.4);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.4);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"O")
		{
			std::normal_distribution<> genm{ 24, 8 };
			std::normal_distribution<> genr{ 9.9, 3.3 };
			std::uniform_int_distribution<int> gent{ 30000, 55000 };

			do star.mass = genm(mt_star);
			while (star.mass <= 6);

			do star.radius = genr(mt_star);
			while (star.radius <= 1.8);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"Q")
		{
			std::normal_distribution<> genm{ 1.8, 0.5 };
			std::normal_distribution<> genr{ 10, 5 };
			std::uniform_int_distribution<int> gent{ 100000, 1000000 };

			do star.mass = genm(mt_star);
			while (star.mass < 1.1 || star.mass > 2.5);

			do star.radius = genr(mt_star);
			while (star.radius < 5 || star.radius > 20);
			star.radius /= 695508.0;
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"WD")
		{
			std::normal_distribution<> genm{ 0.675, .2625 };
			std::normal_distribution<> genr{ 0.009, 0.0005 };
			std::uniform_int_distribution<int> gent{ 60000, 120000 };

			do star.mass = genm(mt_star);
			while (star.mass < 0.15 || star.mass > 1.4);

			do star.radius = genr(mt_star);
			while (star.radius < 0.005 || star.radius > 0.015);
			star.temperatureK = gent(mt_star);
		}
		else if (star.class_ == L"X")
		{
			int c = 299792458;
			int G = 190809;
		
			std::uniform_real_distribution<> genm{ 1, 100 };
			star.mass = genm(mt_star);
			star.radius = ((2 * star.mass * G) / (pow(c, 2)) * 1000000);

			//std::uniform_int_distribution<int> gent{ 60000, 120000 };
			//star.temperatureK = gent(mt_star);
			star.temperatureK = 0;
		}

		tempSol = (star.temperatureK / 5778.0);
		star.luminosity = (pow(star.radius, 2) * pow(tempSol, 4));
		star.innerLimit = 0.1 * star.mass;
		star.outerLimit = 45 * star.mass;
		star.frostLine = (4.85 * sqrt(star.luminosity));
		star.habitZoneInnerLimit = sqrt(star.luminosity / 1.1);
		star.habitZoneOuterLimit = sqrt(star.luminosity / 0.53);

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

	void GeneratePlanet(STAR& star, PLANET& planet)
	{
		std::normal_distribution<> genobliquity{ CONFIG.avgObliquity, CONFIG.SDObliquity };
		std::normal_distribution<> geneccentricity{ CONFIG.avgEccentricity, CONFIG.SDEccentricity };
		std::normal_distribution<> geninclination{ CONFIG.avgInclination, CONFIG.SDInclination };
		planet.name = L"Planet ";

	}


