#include "framework.h"
#include "resource.h"

static std::string wstr_to_str(std::wstring wstr)
{
	std::string str;
	for (int i = 0; i < wstr.size(); i++)
		str += wstr[i];
	return str;
}

static void GetIntFromWindow(HWND &window, int &store)
{
	wchar_t genvar[WSIZE];
	GetWindowTextW(window, genvar, 20);
	store = _wtoi(genvar);
}
static void SetIntToWindow(HWND &window, int number)
{
	wchar_t genVar[16];
	_itow_s(number, genVar, sizeof(genVar) / 2, 10);
	SetWindowTextW(window, genVar);
}

static void SetDoubleToWindow(HWND& window, double number)
{
	wchar_t genVar[16];
	char genChar[26];
	_gcvt_s(genChar, sizeof(genChar), number, 10);
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
		usedNames.push_back(strholder);
		i++;
	}
}