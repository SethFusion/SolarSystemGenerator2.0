#include "framework.h"
#include "resource.h"

static std::string wstr_to_str(std::wstring wstr)
{
	std::string str;
	for (int i = 0; i < wstr.size(); i++)
		str += wstr[i];
	return str;
}