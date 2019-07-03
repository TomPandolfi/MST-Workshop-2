
#include "stdafx.h"
#include <vector>
#include "color.h"




std::vector<HBRUSH> brushList;
std::vector<COLORREF> brushRefList;

HBRUSH GetColorBrush(COLORREF aColorRef) {

	// search for existing brush
	for (int j = 0; j < brushRefList.size(); j++) {
		if (brushRefList[j] == aColorRef)
			return brushList[j];
	}

	// couldnt find it -- create it
	HBRUSH newBrush = CreateSolidBrush(aColorRef);
	if(newBrush == NULL )
		MessageBox(NULL, L"Can't Create a Solid Brush in Color.cpp (23)", L"Brush Memory", MB_OK);
	brushList.push_back(newBrush);
	brushRefList.push_back(aColorRef);

	return newBrush;
}

std::vector<HPEN> penList;
std::vector<COLORREF> penRefList;
std::vector<int> penWidthList;

HPEN GetColorPen(COLORREF aColorRef, int aWidth) {

	// search for existing pen
	for (int j = 0; j < penRefList.size(); j++) {
		if ((penRefList[j] == aColorRef) && (penWidthList[j] == aWidth))
			return penList[j];
	}

	// couldnt find it -- create it
	HPEN newPen = CreatePen(PS_COSMETIC, aWidth, aColorRef);
	if (newPen == NULL)
		MessageBox(NULL, L"Can't Create a Pen in Color.cpp (44)", L"Pen Memory", MB_OK);
	penList.push_back(newPen);
	penRefList.push_back(aColorRef);
	penWidthList.push_back(aWidth);

	return newPen;
}
