#pragma once

HFONT GetFont(int theSize);
HFONT GetFont(std::string typeface, int theSize);
void drawTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string text);
void drawWideTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::wstring text);
void drawTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string fontName, std::string text);
void drawWideTextString(HDC hDC, int x, int y, int fontSize, COLORREF textColor, std::string fontName, std::wstring text);
int calcStringWidthFontSize(HDC hDC, int fontSize, std::string text);
int calcWideStringWidthFontSize(HDC hDC, int fontSize, std::wstring text);


std::string convertLPCTSTR_toString(LPCTSTR text);
LPWSTR ConvertToLPWSTR(const std::string& s);
char* ConvertToChar(const std::string& s);

std::string tomString(double input);
double tomNumber(std::string input);


// there must be a better way to do this 
void drawTextString(HDC hDC, int x, int y, COLORREF textColor, std::string text);
void drawTextLptStr(HDC hDC, int x, int y, COLORREF textColor, LPCTSTR tchar);

int calcStringWidthFont(HDC hDC, LPCTSTR aString);
int calcStringWidthFont(HDC hDC, std::string aString);

void drawLargeTextString(HDC hDC, int x, int y, COLORREF textColor, std::string text);
int calcLargeStringWidthFont(HDC hDC, LPCTSTR aString);
int calcLargeStringWidthFont(HDC hDC, std::string aString);

std::string trimTextToFit(HDC hDC, std::string text, int maxWidth);