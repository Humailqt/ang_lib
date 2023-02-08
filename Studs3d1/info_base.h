#pragma once
#include "StdAfx.h"
#include <cstring>
#include <string>

int LibMessage(LPCTSTR str, int flags );

struct info_base
{
	unsigned int flag;
	CString function_name;
	CString info;
	CString error;
};


class showInfo:info_base
{
public:
	showInfo();
	~showInfo();
	void set_info(CString info) { this->info = info; };
	void show(int line);

private:

};

