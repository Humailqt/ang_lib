#include "info_base.h"

//extern int LibMessage(LPCTSTR str, int flags = 0);

showInfo::showInfo()
{

}

showInfo::~showInfo()
{
}

void showInfo::show(int line = 0)
{
	CString str;

	str += _T("Info:");
	str += info;
	str += _T("\nИсточник:");
	str += std::to_wstring(line).c_str();

	LibMessage(str, MB_OK | MB_ICONERROR);
	info =_T("");
	return;
}