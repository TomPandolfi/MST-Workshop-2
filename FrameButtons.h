#pragma once
#include "structs.h"
#include "Sheet.h"


void buildTopFrameButtons(Sheet* pg, HWND hWnd);
void buildBottomFrameButtons(Sheet* pg, HWND hWnd);
void drawFrameButtons(Sheet* pg, HDC hDC);

//int checkComponentGroupButtons( POS aPos );

bool checkFrameButtons(Sheet* pg, POS rawpos);
int checkTimeButtons(Sheet* pg, POS rawpos);
