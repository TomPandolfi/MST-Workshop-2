#include "structs.h"
#include "stdafx.h"
#include "FrameButtons.h"
#include "Button.h"
#include "ButtonGroups.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "Time.h"
#include "color.h"


#pragma warning (disable: 4267)


void dummy(){}

extern void FrameBottomRightDuplicateGroup(Sheet* pg);
extern void FrameBottomRightSubroutineGroup(Sheet* pg);
extern void FrameBottomRightDeleteGroup(Sheet* pg);



void buildTopFrameButtons(Sheet* pg, HWND hWnd) {
	//if (suppressButtons) {
	//	return;
	//}

	RECT    clientRect;
	GetClientRect(hWnd, &clientRect); 
	int canvasWidth = clientRect.right - clientRect.left;

	//ctx.scale(1 / pg->cs, 1 / pg->cs);

	//ctx.font = '9px ' + defaultFontFamily;
	for (int j = 0; j < pg->frameTopButtons.size(); j++)
		delete(pg->frameTopButtons[j]);
	pg->frameTopButtons.clear();

	int Sheet_BUTTON_INDEX = 6;
	// Left justified
	Button::Setup(0, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0xE0E0E0, 0);
	pg->frameTopButtons.push_back(new Button(L"Options", L"Options", dummy));

	Button::Setup( BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, CONTROL_COLORS, 0);
	pg->frameTopButtons.push_back(new Button(L"Controls", L"Controls", dummy));

	Button::Setup(2* BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, FUNCTION_COLORS, 0);
	pg->frameTopButtons.push_back(new Button(L"Functions", L"Functions", dummy));

	Button::Setup(3* BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, DRAWING_COLORS, 0);
	pg->frameTopButtons.push_back(new Button(L"Drawings", L"Drawings", dummy));

	Button::Setup(4* BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, ARRAY_COLORS, 0);
	pg->frameTopButtons.push_back(new Button(L"Arrays", L"Arrays", dummy));

	Button::Setup(5 * BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0xFFFFDD, 0);
	pg->frameTopButtons.push_back(new Button(L"Images", L"Images", dummy));

	Button::Setup(6 * BUTTON_WIDTH, 0, BUTTON_WIDTH+20, BUTTON_HEIGHT, 0xFF88EE, 0);
	pg->frameTopButtons.push_back(new Button(L"Subroutines", L"Subroutines", dummy));


	// Right justified
	Button::Setup(canvasWidth - BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0x00FFFF, 0);
	pg->frameTopButtons.push_back(new Button(L"Find", L"Find", dummy));

	Button::Setup(canvasWidth - 2*BUTTON_WIDTH, 0, BUTTON_WIDTH, BUTTON_HEIGHT, 0x00FFFF, 0);
	pg->frameTopButtons.push_back(new Button(L"Help", L"Help", dummy));
}

void buildBottomFrameButtons(Sheet* pg, HWND hWnd) {
	//if (suppressButtons) {
	//	return;
	//}

	RECT    clientRect;
	GetClientRect(hWnd, &clientRect);
	int canvasWidth = clientRect.right - clientRect.left;
	int canvasHeight = clientRect.bottom - clientRect.top;

	//ctx.scale(1 / pg->cs, 1 / pg->cs);

	//ctx.font = '9px ' + defaultFontFamily;
	for (int j = 0; j < pg->frameBottomButtons.size(); j++)
		delete(pg->frameBottomButtons[j]);
	pg->frameBottomButtons.clear();

	bool someTimeBased = checkForTimeBased(pg);

	Button::Setup(0, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xD0F0D0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Reset", L"ResetButton", dummy));

	Button::Setup(3 * BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xD0F0D0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Step", L"StepButton", dummy));

	Button::Setup(BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xD0F0D0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Start", L"StartButton", dummy));

	Button::Setup(2 * BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xD0F0D0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Stop", L"StopButton", dummy));



	Button::Setup(canvasWidth - 3.7*BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, 1.7*BUTTON_WIDTH, BUTTON_HEIGHT, 0xE0E0E0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Make Subroutine", L"MakeSubroutine", FrameBottomRightSubroutineGroup));

	Button::Setup(canvasWidth - 2 * BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xE0E0E0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Duplicate", L"DuplicateButton", FrameBottomRightDuplicateGroup));

	Button::Setup(canvasWidth - BUTTON_WIDTH, canvasHeight - BUTTON_HEIGHT, BUTTON_WIDTH, BUTTON_HEIGHT, 0xE0E0E0, 0);
	pg->frameBottomButtons.push_back(new Button(L"Delete", L"DeleteButton", FrameBottomRightDeleteGroup));
}

void drawFrameButtons(Sheet* pg, HDC hDC) {

	RECT    clientRect;
	GetClientRect(WindowFromDC(hDC), &clientRect);
	int canvasWidth = clientRect.right - clientRect.left;
	int canvasHeight = clientRect.bottom - clientRect.top;

	// first the background gray frame
	RECT aRect;
	aRect.left = 0;
	aRect.top = 0;
	aRect.right = canvasWidth;
	aRect.bottom = BUTTON_HEIGHT + 5;
	HBRUSH frameColor = GetColorBrush(RGB(0, 0, 0)); // CreateSolidBrush
	HBRUSH fillColor = GetColorBrush(RGB(240, 240, 240)); // CreateSolidBrush
	FillRect(hDC, &aRect, fillColor);
	FrameRect(hDC, &aRect, frameColor);
	//DeleteObject(frameColor);
	//DeleteObject(fillColor);
	

	bool someTimeBased = checkForTimeBased(pg);
	
	COLORREF fillEE;
	COLORREF fillFF;
	COLORREF fill00;
	COLORREF stroke00;
	if (someTimeBased) {
		fillEE = 0xE0E0E0;
		fillFF = 0xFFFFFF;
		fill00 = 0x000000;
		stroke00 = 0x000000;
	}
	else {
		fillEE = 0xFFFFFF;
		fillFF = 0xFFFFFF;
		fill00 = 0x888888;
		stroke00 = 0x888888;
		
	}

	pg->frameBottomButtons[0]->backgroundColor = fillEE; // reset
	pg->frameBottomButtons[0]->dim = !someTimeBased;

	pg->frameBottomButtons[1]->backgroundColor = fillEE; //step
	pg->frameBottomButtons[1]->dim = !someTimeBased;

	if (pg->timerID == 0){
		pg->frameBottomButtons[2]->backgroundColor = fillEE;  // Start
		pg->frameBottomButtons[2]->dim = !someTimeBased;
		pg->frameBottomButtons[3]->backgroundColor = fillFF;  // Stop
		pg->frameBottomButtons[3]->dim = !someTimeBased;
	} else {
		pg->frameBottomButtons[2]->backgroundColor = fillFF;  // Start
		pg->frameBottomButtons[2]->dim = !someTimeBased;
		pg->frameBottomButtons[3]->backgroundColor = fillEE;  // Stop
		pg->frameBottomButtons[3]->dim = !someTimeBased;
    }
	
	bool someSelected = false;
	int len = pg->dgrm.cL.size();
	for (int i = 0; i < len; i++)
		if (pg->dgrm.cL[i]->selected) {
			someSelected = true;
			break;
		}
	if (!someSelected) {
		len = pg->dgrm.wL.size();
		for (int i = 0; i < len; i++)
			if (pg->dgrm.wL[i]->selected) {
				someSelected = true;
				break;
			}
	}
	pg->frameBottomButtons[4]->dim = !someSelected;// Subroutine 
	pg->frameBottomButtons[5]->dim = !someSelected;// Duplicate
	pg->frameBottomButtons[6]->dim = !someSelected;// Delete

	for (int j = 0; j < pg->frameTopButtons.size(); j++)
		pg->frameTopButtons[j]->Draw(hDC);

	for (int j = 0; j < pg->frameBottomButtons.size(); j++)
		pg->frameBottomButtons[j]->Draw(hDC);
}

int checkFrameTopButtons(Sheet* pg, POS aPos )
{
	for (int j = 0; j < pg->frameTopButtons.size(); j++) {
		if (aPos.x < pg->frameTopButtons[j]->x )
			continue;
		if (aPos.x > pg->frameTopButtons[j]->x + pg->frameTopButtons[j]->width)
			continue;
		if (aPos.y < pg->frameTopButtons[j]->y)
			continue;
		if (aPos.y > pg->frameTopButtons[j]->y + pg->frameTopButtons[j]->height)
			continue;

		return j;
	}
	return -1;
}

//int checkFrameBottomButtons(Sheet* pg, POS aPos)
//{
//	for (int j = 0; j < pg->frameBottomButtons.size(); j++) {
//		if (aPos.x < pg->frameBottomButtons[j]->x)
//			continue;
//		if (aPos.x > pg->frameBottomButtons[j]->x + pg->frameBottomButtons[j]->width)
//			continue;
//		if (aPos.y < pg->frameBottomButtons[j]->y)
//			continue;
//		if (aPos.y > pg->frameBottomButtons[j]->y + pg->frameBottomButtons[j]->height)
//			continue;
//
//		return j;
//	}
//	return -1;
//}

bool checkFrameButtons(Sheet* pg, POS rawpos) {

	int group = checkFrameTopButtons(pg, rawpos);
	if (group == -1)
		return false; // didnt find it

	activateButtonGroup(pg, group);
	return true;
}

int checkTimeButtons(Sheet* pg, POS aPos) {
	if (!checkForTimeBased(pg))
		return -1;

	for (int j = 0; j < 4; j++) {

		if (aPos.x < pg->frameBottomButtons[j]->x)
			continue;
		if (aPos.x > pg->frameBottomButtons[j]->x + pg->frameBottomButtons[j]->width)
			continue;
		if (aPos.y < pg->frameBottomButtons[j]->y)
			continue;
		if (aPos.y > pg->frameBottomButtons[j]->y + pg->frameBottomButtons[j]->height)
			continue;

		return j;
	}
	return -1;
}



bool DoSelectedComponentButtons(Sheet* pg, POS aPos) {
	
	for (int j = 4; j < 7; j++) {
		
		if (aPos.x < pg->frameBottomButtons[j]->x)
			continue;
		if (aPos.x > pg->frameBottomButtons[j]->x + pg->frameBottomButtons[j]->width)
			continue;
		if (aPos.y < pg->frameBottomButtons[j]->y)
			continue;
		if (aPos.y > pg->frameBottomButtons[j]->y + pg->frameBottomButtons[j]->height)
			continue;
		
		if (!pg->frameBottomButtons[j]->dim) {
			if(pg->frameBottomButtons[j]->action )
				pg->frameBottomButtons[j]->action();
			if (pg->frameBottomButtons[j]->actionPage)
				pg->frameBottomButtons[j]->actionPage(pg);
			return true;
		}
	}
	return false;
}