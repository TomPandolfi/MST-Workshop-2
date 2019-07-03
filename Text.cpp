#include "stdafx.h"
#include "structs.h"
#include <string>
#include <stdio.h>
#include <vector>
#include "Text.h"


#pragma warning (disable: 4267)
#pragma warning (disable: 4244)


std::vector<HFONT> fontList;
std::vector<int> fontSizeList;

HFONT GetFont(int theSize) {

	if (theSize < MINIMUM_FONT_SIZE)
		theSize = MINIMUM_FONT_SIZE;
	if (theSize > MAXIMUM_FONT_SIZE)
		theSize = MAXIMUM_FONT_SIZE;
	
	if (theSize < 50)
		theSize = (theSize / 2) * 2;
	else if( theSize < 150)
		theSize = (theSize / 5) * 5;
	else
		theSize = (theSize / 25) * 25;

	// search for existing font
	for (int j = 0; j < fontSizeList.size(); j++) {
		if (fontSizeList[j] == theSize)
			return fontList[j];
	}
	
	// couldnt find it -- create it
	LOGFONT myFont;
	memset(&myFont, 0, sizeof(myFont)); // 92 bytes each
	myFont.lfHeight = -theSize; // see PS
//	myFont.lfWeight = FW_BOLD;
	myFont.lfFaceName[0] = 'V';
	myFont.lfFaceName[1] = 'e';
	myFont.lfFaceName[2] = 'r';
	myFont.lfFaceName[3] = 'd';
	myFont.lfFaceName[4] = 'a';
	myFont.lfFaceName[5] = 'n';
	myFont.lfFaceName[6] = 'a';
	myFont.lfFaceName[7] = 0;
	HFONT hfont = CreateFontIndirect(&myFont);

	fontList.push_back(hfont);
	fontSizeList.push_back(theSize);

	return hfont;
}



std::vector<HFONT> fontList2;
std::vector<int> fontSizeList2;
std::vector<std::string> fontNameList2;

HFONT GetFont(std::string fontName, int theSize) {

	if (theSize < MINIMUM_FONT_SIZE)
		theSize = MINIMUM_FONT_SIZE;
	else if (theSize > MAXIMUM_FONT_SIZE)
		theSize = MAXIMUM_FONT_SIZE;
	else if (theSize < 50)
		theSize = (theSize / 2) * 2;
	else
		theSize = (theSize / 5) * 5;
	

	// search for existing font
	for (int j = 0; j < fontSizeList2.size(); j++) {
		if ((fontSizeList2[j] == theSize) && (fontNameList2[j] == fontName))
			return fontList2[j];
	}

	// couldnt find it -- create it
	LOGFONT myFont;
	memset(&myFont, 0, sizeof(myFont)); // 92 bytes each
	myFont.lfHeight = -theSize; // see PS
//	myFont.lfWeight = FW_BOLD;
	
	for( int j = 0; j < fontName.size(); j++ )
		myFont.lfFaceName[j] = fontName[j];
	myFont.lfFaceName[fontName.size()] = 0;

	HFONT hfont = CreateFontIndirect(&myFont);

	fontList2.push_back(hfont);
	fontSizeList2.push_back(theSize);
	fontNameList2.push_back(fontName);

	return hfont;
}



#if 0
int EnumFont(
	const LOGFONT    *lpelfe,
	const TEXTMETRIC *lpntme,
	DWORD      FontType,
	LPARAM     lParam
) {
	return 1;
}
#endif

void drawTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string text) {

#if 0
	LOGFONTW myFont;
	memset(&myFont, 0, sizeof(myFont));
	myFont.lfCharSet = DEFAULT_CHARSET;
	myFont.lfFaceName[0] = 0;
	myFont.lfPitchAndFamily = 0;

	int count = EnumFontFamiliesExW(hDC, &myFont, EnumFont, 0, 0);
#endif


	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT thisFont = GetFont(fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);

	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}
void drawWideTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::wstring text) {

	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT thisFont = GetFont(fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);

	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}


void drawTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string fontName, std::string text) {

	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT thisFont = GetFont(fontName, fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);

	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}

void drawWideTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string fontName, std::wstring text) {

	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT thisFont = GetFont(fontName, fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);

	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}

int calcStringWidthFontSize(HDC hDC, int fontSize, std::string aString) {

	HFONT thisFont = GetFont(fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < aString.size(); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth); // doesnt always work!!!
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}
int calcWideStringWidthFontSize(HDC hDC, int fontSize, std::wstring aString) {

	HFONT thisFont = GetFont(fontSize);
	HFONT oldHFont = (HFONT)SelectObject(hDC, thisFont);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < aString.size(); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth); // doesnt always work!!!
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}

//*********************************

std::string convertLPCTSTR_toString(LPCTSTR text) {
	std::string result;
	for (int k = 0; k < _tcslen(text); k++)
		result += text[k];
	return result;
}

LPWSTR ConvertToLPWSTR(const std::string& s)
{
	LPWSTR ws = new wchar_t[s.size() + 1]; // +1 for zero at the end
	copy(s.begin(), s.end(), ws);
	ws[s.size()] = 0; // zero at the end
	return ws;
}
char* ConvertToChar(const std::string& s)
{
	char* cs = new char[s.size() + 1]; // +1 for zero at the end
	copy(s.begin(), s.end(), cs);
	cs[s.size()] = 0; // zero at the end
	return cs;
}
std::string tomString(double input)
{
	if (input == 0.0)
		return "0";
	
	bool isNegative = false;
	if (input < 0.0) {
		isNegative = true;
		input = -input;
	}

	char buffer[20];

	if (input == (long)input) {
		snprintf(buffer, 20, "%d", (int)input);
	}
	else {
		if (input < 0.000001)
			snprintf(buffer, 20, "%e", input);
		else {
			snprintf(buffer, 20, "%f", input);
			int len = strlen(buffer);
			while (buffer[len - 1] == '0')
				buffer[--len] = 0;// shorten it if zero 
		}
	}
	
	std::string result;
	if (isNegative)
		result += '-';
	int len = strlen(buffer);
	for (int j = 0; j < len; j++)
		result += buffer[j];

	return result;
}


double tomNumber(std::string input) {

	std::string output;
	bool hasDigits = false;
	bool hasPoint = false;
	bool hasSign = false;
	bool hasExponent = false;
	int len = input.size();
	for (int j = 0; j < len; j++) {
		if ((input[j] >= '0') && (input[j] <= '9')) {
			output += input[j];
			hasDigits = true;
		}
		else if (input[j] == '+') {
			if (hasSign)
				break;
			output += input[j];
			hasSign = true;
		}
		else if (input[j] == '-'){
			if (hasSign)
				break;
			output += input[j];
			hasSign = true;
		}
		else if ((!hasPoint) && (input[j] == '.')) {
			output += input[j];
			hasPoint = true;
		}
		else if ((input[j] == 'e') || (input[j] == 'E')) {
			if (hasExponent)
				break;
			output += input[j];
			hasExponent = true;
			hasSign = false; // exponent can have a sign
		}
		else
			break;
	}
	if ((output.size() > 0) && hasDigits)
		return stod(output);
	return 0;
}


// ********************* 9 point font for main screen **********************

bool fontIsReady = false;
HFONT hfont9;
void CreateMyFont() {
	if (fontIsReady)
		return;

	LOGFONT myFont;
	memset(&myFont, 0, sizeof(myFont));
	myFont.lfHeight = -9; // see PS
//	myFont.lfWeight = FW_BOLD;
	myFont.lfFaceName[0] = 'V';
	myFont.lfFaceName[1] = 'e';
	myFont.lfFaceName[2] = 'r';
	myFont.lfFaceName[3] = 'd';
	myFont.lfFaceName[4] = 'a';
	myFont.lfFaceName[5] = 'n';
	myFont.lfFaceName[6] = 'a';
	myFont.lfFaceName[7] = 0;

	hfont9 = CreateFontIndirect(&myFont);
	fontIsReady = true;
}

// there must be a better way to do this 
void drawTextString(HDC hDC, int x, int y, COLORREF textColor, std::string text) {
	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	CreateMyFont();
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont9);
		
	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}


void drawTextLptStr(HDC hDC, int x, int y, COLORREF textColor, LPCTSTR tchar) {

	// set text color and font
	CreateMyFont();
	
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont9);
	SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, x, y, tchar, _tcslen(tchar));

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}


int calcStringWidthFont(HDC hDC, LPCTSTR aString) {
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont9);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < _tcslen(aString); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth);
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}

int calcStringWidthFont(HDC hDC, std::string aString) {
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont9);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < aString.size(); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth); // doesnt always work!!!
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}



std::string trimTextToFit(HDC hDC, std::string text, int maxWidth) {
	if (text.size() == 0 || maxWidth <= 0)
		return "";

	// first see if it fits
	if (calcStringWidthFont(hDC, text) < maxWidth - 1)
		return text;
	// now trim it allowing for the ellipsis
	int ellipsisWidth = calcStringWidthFont(hDC, "...");
	maxWidth -= ellipsisWidth + 4;

	// now start at the beginning and measure text to see what fits
	int kfit = 0;
	int k;
	for (k = 1; k < text.size(); k++) {
		if (calcStringWidthFont(hDC, text.substr(0, k)) > maxWidth)
			break;
		kfit = k;
	}
	return text.substr(0, k) + "...";
}

// ********************* 12 point font for 2D Canvas **********************

bool largeFontIsReady = false;
HFONT hfont12;
void CreateMyLargeFont() {
	if (largeFontIsReady)
		return;

	LOGFONT myFont;
	memset(&myFont, 0, sizeof(myFont));
	myFont.lfHeight = -12; // see PS
//	myFont.lfWeight = FW_BOLD;
	myFont.lfFaceName[0] = 'A';
	myFont.lfFaceName[1] = 'r';
	myFont.lfFaceName[2] = 'i';
	myFont.lfFaceName[3] = 'a';
	myFont.lfFaceName[4] = 'l';
	myFont.lfFaceName[5] = 0;

	hfont12 = CreateFontIndirect(&myFont);
	largeFontIsReady = true;
}

void drawLargeTextString(HDC hDC, int x, int y, COLORREF textColor, std::string text) {
	int slen = text.size() + 1;
	LPTSTR tchar = new TCHAR[slen];
	for (int k = 0; k < slen; k++)
		tchar[k] = text[k];

	// set text color and font
	CreateMyLargeFont();
	COLORREF oldTextColor = SetTextColor(hDC, textColor);
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont12);

	TextOut(hDC, x, y, tchar, _tcslen(tchar));
	delete(tchar);

	// restore text color and font
	SetTextColor(hDC, oldTextColor);
	SelectObject(hDC, oldHFont);
}


int calcLargeStringWidthFont(HDC hDC, LPCTSTR aString) {
	CreateMyLargeFont();
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont12);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < _tcslen(aString); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth);
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}

int calcLargeStringWidthFont(HDC hDC, std::string aString) {
	CreateMyLargeFont();
	HFONT oldHFont = (HFONT)SelectObject(hDC, hfont12);
	int nCharWidth = 0;
	int width = 0;
	for (int i = 0; i < aString.size(); i++) {
		GetCharWidth32(hDC, (UINT)aString[i], (UINT)aString[i], &nCharWidth);
		width += nCharWidth;
	}
	SelectObject(hDC, oldHFont);
	return width;
}



