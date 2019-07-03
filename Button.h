#pragma once
#include "structs.h"



class Sheet;
class Component;


class Button {
public:
	int x;
	int y;
	int width;
	int height;
	LPCTSTR visibleText;
	void(*action)(void);
	void(*action1)(int);
	void(*actionPage)(Sheet*);
	void(*actionPageI)(Sheet*, int i);
	bool active;
	bool isCheckbox;
	bool isSelected;
	COLORREF backgroundColor;
	bool dim;
	int type;
	LPCTSTR componentName;
	LPCTSTR actionType;

	// statics
	static int refX;
	static int refY;
	static int refWidth;
	static int refHeight;
	static COLORREF refBackgroundColor;
	static int refType;
	static bool refDebug;
	
	Button();
	Button(LPCTSTR _label);
	Button(LPCTSTR _label, LPCTSTR _componentName);
	Button(LPCTSTR _label, LPCTSTR _componentName, void(*action)(void));
	Button(LPCTSTR _label, LPCTSTR _componentName, void(*actionPage)(Sheet*));
	Button(LPCTSTR _label, LPCTSTR _componentName, void(*actionPageI)(Sheet*, int i));
	Button(int x, int y, int width, int height, LPCTSTR _visTxt, void(*_action)(int));
	Button(int x, int y, int width, int height, LPCTSTR _visTxt, void(*_actionPageI)(Sheet*,int));
	Button(int x, int y, int width, int height, LPCTSTR _visTxt, LPCTSTR _actionType);
	static void Setup(int _x, int _y, int _width, int _height, COLORREF _bkgColor, int _type);
	static void NextColumn(int _y, int _width);
	void Draw(HDC hDC);
	void draw(HDC hDC, int xOffset, int yOffset);
	bool checkForMouseHit(POS rawpos);

};

