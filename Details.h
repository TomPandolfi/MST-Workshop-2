#pragma once


#include "stdafx.h"
#include "structs.h"
#include <vector>

void generateDetailDocument();

class Detail {
public:
	LPCTSTR name;
	LPCTSTR text;
	bool used;
	bool isComponent;
	Detail(LPCTSTR _name, LPCTSTR _text);
	Detail(bool _isComponent, LPCTSTR _name, LPCTSTR _text);
};

class DetailList {
	std::vector<Detail> details;
};

LPCTSTR lookupDetail(LPCTSTR _name);
bool loolupDetailIsComponent(LPCTSTR _name);
void markAllDetailsUnused();
void markDetailUsed(LPCTSTR _name);
void lookupUnusedDetails();
void loadDetails();