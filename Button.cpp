#include "stdafx.h"
#include "structs.h"
#include "Button.h"
#include <string>
#include "Text.h"
#include "color.h"


#pragma warning (disable: 4267)

Button::Button() {}
Button::Button(LPCTSTR _visTxt)
{
	x = refX;
	y = refY;
	refY += BUTTON_HEIGHT;
	width = refWidth;
	height = refHeight;
	visibleText = _visTxt;
	componentName = L"";
	action = NULL;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = NULL;
	active = false;
	isCheckbox = false;
	isSelected = false;
	backgroundColor = refBackgroundColor;
	dim = false;
	type = refType;

	active = refDebug; // for testing only
}

Button::Button(LPCTSTR _componentLabel, LPCTSTR _componentName) {
	x = refX;
	y = refY;
	refY += BUTTON_HEIGHT;
	width = refWidth;
	height = refHeight;
	visibleText = _componentLabel;
	componentName = _componentName;
	action = NULL;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = NULL;
	active = false;
	isCheckbox = false;
	isSelected = false;
	backgroundColor = refBackgroundColor;
	dim = false;
	type = refType;

	active = refDebug; // for testing only
}
Button::Button(LPCTSTR _componentLabel, LPCTSTR _componentName, void(*_action)(void)) {
	x = refX;
	y = refY;
	refY += BUTTON_HEIGHT;
	width = refWidth;
	height = refHeight;
	visibleText = _componentLabel;
	componentName = _componentName;
	action = _action;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = NULL;
	active = false;
	isCheckbox = false;
	isSelected = false;
	backgroundColor = refBackgroundColor;
	dim = false;
	type = refType;

	active = refDebug; // for testing only
}
Button::Button(LPCTSTR _componentLabel, LPCTSTR _componentName, void(*_actionPage)(Sheet*)) {
	x = refX;
	y = refY;
	refY += BUTTON_HEIGHT;
	width = refWidth;
	height = refHeight;
	visibleText = _componentLabel;
	componentName = _componentName;
	action = NULL;
	action1 = NULL;
	actionPage = _actionPage;
	actionPageI = NULL;
	active = false;
	isCheckbox = false;
	isSelected = false;
	backgroundColor = refBackgroundColor;
	dim = false;
	type = refType;

	active = refDebug; // for testing only
}

Button::Button(LPCTSTR _componentLabel, LPCTSTR _componentName, void(*_actionPageI)(Sheet*, int i)) {
	x = refX;
	y = refY;
	refY += BUTTON_HEIGHT;
	width = refWidth;
	height = refHeight;
	visibleText = _componentLabel;
	componentName = _componentName;
	action = NULL;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = _actionPageI;
	active = false;
	isCheckbox = false;
	isSelected = false;
	backgroundColor = refBackgroundColor;
	dim = false;
	type = refType;

	active = refDebug; // for testing only
}

Button::Button(int inX, int inY, int inWidth, int inHeight, LPCTSTR _visTxt, void(*_action1)(int))
{
	x = inX;
	y = inY;
	width = inWidth;
	height = inHeight;
	visibleText = _visTxt;
	action = NULL;
	action1 = _action1;
	actionPage = NULL;
	actionPageI = NULL;
	backgroundColor = refBackgroundColor;
	dim = false;
}
Button::Button(int inX, int inY, int inWidth, int inHeight, LPCTSTR _visTxt, void(*_actionPageI)(Sheet*, int))
{
	x = inX;
	y = inY;
	width = inWidth;
	height = inHeight;
	visibleText = _visTxt;
	action = NULL;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = _actionPageI;
	backgroundColor = refBackgroundColor;
	dim = false;
}
Button::Button(int inX, int inY, int inWidth, int inHeight, LPCTSTR _visTxt, LPCTSTR _actionType)
{
	x = inX;
	y = inY;
	width = inWidth;
	height = inHeight;
	visibleText = _visTxt;
	action = NULL;
	action1 = NULL;
	actionPage = NULL;
	actionPageI = NULL;
	actionType = _actionType;
	backgroundColor = refBackgroundColor;
	dim = false;
}

int Button::refX = 0;
int Button::refY = 0;
int Button::refWidth = 0;
int Button::refHeight = 0;
COLORREF Button::refBackgroundColor = 0x000000;
int Button::refType = 0;
bool Button::refDebug = false;

void Button::Setup(int _x, int _y, int _width, int _height, COLORREF _bkgColor, int _type) {
	refX = _x;
	refY = _y;
	refWidth = _width;
	refHeight = _height;
	refBackgroundColor = _bkgColor;
	refType = _type;
}

void Button::NextColumn(int _y, int _width) {
	refX += refWidth;
	refY = _y;
	refWidth = _width;
}

void Button::Draw(HDC hDC) {  // doesn't use the brush color yet
	COLORREF bkgColor;
	RECT aRect;
	RECT strokeRect;

	if (dim)
		bkgColor = 0xF0F0F0;
	else
		bkgColor = backgroundColor; 
	HBRUSH frameStyle = GetColorBrush(RGB(0, 0, 0));
	HBRUSH fillStyle = GetColorBrush(bkgColor);
	aRect.left = x;
	aRect.top = y;
	aRect.right = x + width;
	aRect.bottom = y + height;

	FillRect(hDC, &aRect, fillStyle);
	FrameRect(hDC, &aRect, frameStyle);

	if (isCheckbox) {

		strokeRect.left = x + 3;
		strokeRect.top = y + 3;
		strokeRect.right = x + 3 + height - 6;
		strokeRect.bottom = y + 3 + height - 6;
		FrameRect(hDC, &strokeRect, frameStyle);

		TextOut(hDC, x + height, y + 1, visibleText, _tcslen(visibleText));

		if (isSelected) {
			MoveToEx(hDC, x + 3, y + 3, NULL);
			LineTo(hDC, x + height - 4, y + height - 4);
			MoveToEx(hDC, x + height - 3, y + 3, NULL);
			LineTo(hDC, x + 3, y + height - 3);
		}
	}
	else {
		COLORREF aColor;
		if (dim)
			aColor = 0x808080;
		else
			aColor = 0x000000;
		SetTextColor(hDC, aColor);

		SetBkColor(hDC, backgroundColor);
		TextOut(hDC, x + 4, y + 1, visibleText, _tcslen(visibleText));
	}

	active = true;
}

void Button::draw(HDC hDC, int xOffset, int yOffset ) { 
	RECT aRect;

	HBRUSH frameStyle = GetColorBrush(RGB(0, 0, 0)); // CreateSolidBrush
	HBRUSH fillStyle = GetColorBrush(RGB(230, 230, 230)); // CreateSolidBrush
	aRect.left = x + xOffset;
	aRect.top = y + yOffset;
	aRect.right = x + width + xOffset;
	aRect.bottom = y + height + yOffset;

	FillRect(hDC, &aRect, fillStyle);
	FrameRect(hDC, &aRect, frameStyle);

	SetTextColor(hDC, 0);
	TextOut(hDC, x + xOffset + 4, y + yOffset + 1, visibleText, _tcslen(visibleText));

	active = true;
}

bool Button::checkForMouseHit(POS pos) {
	int xx = x;// +that.cx;
	int yy = y;// +that.cy - 20;    TO DO wait until we have scrolling screens

	if (pos.x < xx)
		return false;
	if (pos.y < yy)
		return false;
	if (pos.x > xx + width)
		return false;
	if (pos.y > yy + height)
		return false;
	return true;
}