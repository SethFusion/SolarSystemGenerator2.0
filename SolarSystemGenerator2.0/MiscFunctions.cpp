#include "framework.h"
#include "resource.h"

static std::string wstr_to_str(std::wstring wstr)
{
	std::string str;
	for (int i = 0; i < wstr.size(); i++)
		str += wstr[i];
	return str;
}
static std::string wstr_to_str(std::vector<std::wstring> vwstr)
{
	std::string str;
	for (int i = 0; i < vwstr.size(); i++)
	{
		for (int j = 0; j < vwstr.at(i).size(); j++)
		{
			str += vwstr.at(i)[j];
		}
		if (!(i == vwstr.size() - 1))
			str += " ";
	}
	return str;
}




static void FindNextEntry(char* Buffer, int& parse)
{
	wchar_t holder;
	do
	{
		holder = Buffer[parse];
		parse++;
	} while (holder != '=');
}
static void LoadVariableFromFile(char* Buffer, int& parse, bool& store)
{
	FindNextEntry(Buffer, parse);
	wchar_t holder = Buffer[parse];
	store = _wtoi(&holder);
	parse++;
}
static void LoadVariableFromFile(char* Buffer, int& parse, wchar_t* store)
{
	FindNextEntry(Buffer, parse);
	wchar_t holder;
	int i = 0;
	do
	{
		holder = Buffer[parse];
		store[i] = holder;
		parse++;
		i++;
	} while (holder != '\n');
	store[i - 2] = '\0';
}
static void LoadVariableFromFile(char* Buffer, int& parse, int& store)
{
	FindNextEntry(Buffer, parse);
	wchar_t holder, numholder[16];
	int i = 0;
	do
	{
		holder = Buffer[parse];
		numholder[i] = holder;
		parse++;
		i++;
	} while (holder != '\n');
	numholder[i] = '\0';
	store = _wtoi(numholder);
}
static void LoadVariableFromFile(char* Buffer, int& parse, double& store)
{
	FindNextEntry(Buffer, parse);
	wchar_t holder, numholder[16];
	int i = 0;
	do
	{
		holder = Buffer[parse];
		numholder[i] = holder;
		parse++;
		i++;
	} while (holder != '\n');
	numholder[i] = '\0';
	store = _wtof(numholder);
}
static void LoadListFromFile(char* Buffer, int& parse, wchar_t* store)
{
	wchar_t holder;
	int i = 0;
	do
	{
		holder = Buffer[parse];
		parse++;
	} while (holder != '{');
	holder = Buffer[parse];
	while (holder != '}')
	{
		store[i] = holder;
		parse++;
		holder = Buffer[parse];
		i++;
	}
	store[i] = '\0';
}






static void GetVariableFromWindow(HWND& window, wchar_t* store)
{
	wchar_t genvar[WSIZE];
	int i = 0;
	GetWindowTextW(window, genvar, WSIZE);
	while (genvar[i] != '\0')
	{
		store[i] = genvar[i];
		i++;
	}
}
static void GetVariableFromWindow(HWND& window, int& store)
{
	wchar_t genvar[WSIZE];
	GetWindowTextW(window, genvar, 20);
	store = _wtoi(genvar);
}
static void GetVariableFromWindow(HWND& window, double& store)
{
	wchar_t genvar[WSIZE];
	GetWindowTextW(window, genvar, 20);
	store = _wtof(genvar);
}


static void SetVariableToWindow(HWND& window, int number)
{
	wchar_t genVar[16];
	_itow_s(number, genVar, sizeof(genVar) / 2, 10);
	SetWindowTextW(window, genVar);
}
static void SetVariableToWindow(HWND& window, double number)
{
	wchar_t genVar[16];
	char genChar[26];
	_gcvt_s(genChar, sizeof(genChar), number, 5);
	mbstowcs_s(NULL, genVar, sizeof(genVar) / 2, genChar, sizeof(genChar));
	SetWindowTextW(window, genVar);
}





static void FillModList(HWND &listH, std::vector<std::wstring> &list)
{
	wchar_t dataHolder[MODLIST_SIZE];
	GetWindowTextW(listH, dataHolder, MODLIST_SIZE);

	int i = 0;
	std::wstring strholder = L"";

	while (dataHolder[i] != '\0' && dataHolder[i - 1] != '\0')
	{
		strholder = L"";
		while (dataHolder[i] != ' ' && dataHolder[i] != '\0')
		{
			strholder += dataHolder[i];
			i++;
		}
		list.push_back(strholder);
		i++;
	}
}

static void FillDataset(HWND& dataH, std::vector<std::wstring> &list, std::vector<std::wstring> &usedNames)
{
	wchar_t dataHolder[DATASET_SIZE];
	GetWindowTextW(dataH, dataHolder, DATASET_SIZE);

	int i = 0;
	std::wstring strholder = L"";

	while (dataHolder[i] != '\0' && dataHolder[i - 1] != '\0')
	{
		strholder = L"";
		while (dataHolder[i] != ' ' && dataHolder[i] != '\0')
		{
			strholder += dataHolder[i];
			i++;
		}
		list.push_back(strholder);
		strholder.at(0) = toupper(strholder.at(0));
		usedNames.push_back(strholder);
		i++;
	}
}


