
#include "stdafx.h"
#include "CanvasBitmap.h"
#include "CanvasComponents.h"
#include "Sheet.h"
#include "structs.h"
#include "Text.h"
#include <string>
#include "Windowsx.h"
#include "ScreenDialog.h"
#include "cdib.h"
#include "rotate.h"



extern void button2dDisplayModeAndDashAndOffset_Completion(Sheet* pg, int i, double* doubleValues, bool* boolValues, int radioValue);


#if 0
// ********************* bitmap rotator First attempt ********************************

// GetRotatedBitmapNT	- Create a new bitmap with rotated image
// Returns		- Returns new bitmap with rotated image
// hBitmap		- Bitmap to rotate
// radians		- Angle of rotation in radians
// clrBack		- Color of pixels in the resulting bitmap that do
//			  not get covered by source pixels
HBITMAP GetRotatedBitmapNT(Sheet* pg, HBITMAP hBitmap, float radians, COLORREF clrBack)
{
	// Create a memory DC compatible with the display
	HDC sourceDC = CreateCompatibleDC(pg->hDC2);
	HDC destDC = CreateCompatibleDC(pg->hDC2);

	// Get logical coordinates
	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);

	float cosine = (float)cos(radians);
	float sine = (float)sin(radians);

	// Compute dimensions of the resulting bitmap
	// First get the coordinates of the 3 corners other than origin
	int x1 = (int)(bm.bmHeight * sine);
	int y1 = (int)(bm.bmHeight * cosine);
	int x2 = (int)(bm.bmWidth * cosine + bm.bmHeight * sine);
	int y2 = (int)(bm.bmHeight * cosine - bm.bmWidth * sine);
	int x3 = (int)(bm.bmWidth * cosine);
	int y3 = (int)(-bm.bmWidth * sine);

	int minx = min(0, min(x1, min(x2, x3)));
	int miny = min(0, min(y1, min(y2, y3)));
	int maxx = max(0, max(x1, max(x2, x3)));
	int maxy = max(0, max(y1, max(y2, y3)));

	int w = maxx - minx;
	int h = maxy - miny;

	// Create a bitmap to hold the result
	HBITMAP hbmResult = CreateCompatibleBitmap(pg->hDC2, w, h);

	HBITMAP hbmOldSource = (HBITMAP)SelectObject(sourceDC, hBitmap);
	HBITMAP hbmOldDest = (HBITMAP)SelectObject(destDC, hbmResult);

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush(clrBack);
	HBRUSH hbrOld = (HBRUSH)::SelectObject(pg->hDC2, hbrBack);
	//destDC.PatBlt(0, 0, w, h, PATCOPY);
	DeleteObject(SelectObject(destDC, hbrOld));

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cosine;
	xform.eM12 = -sine;
	xform.eM21 = sine;
	xform.eM22 = cosine;
	xform.eDx = (float)-minx;
	xform.eDy = (float)-miny;

	SetWorldTransform(destDC, &xform);

	// Now do the actual rotating - a pixel at a time
	BitBlt(destDC, 0, 0, bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY);

	// Restore DCs
	//::SelectObject(sourceDC.m_hDC, hbmOldSource);
	//::SelectObject(destDC.m_hDC, hbmOldDest);

	return hbmResult;
}
#endif


// ************************ Fast Bitmap Rotation and Scaling *******************
// ************************ Steven M Mortimer ********************************

// defines
#define _MINSCALE   0.4f
#define _MAXSCALE   5.0f
#define SZIMAGE     L"test.bmp"

// Globals
CDIB* gDibSrc = NULL;
CDIB* gDibDst = NULL;
float   gdScale = _MAXSCALE;
float   gdAngle = 0.0f;
float   gdScaleDir = 0.1f;
double  gdTicksPerSec = 0.0;
bool    gbTimeFunction = false;

// Function Prototypes
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



void Update(HDC hdc)
{
	//double dStartT = GetTimer();

	// Call Rotate routine, using center of the window and the
	// center of the source image as the points to rotate around
	FastRotate(
		gDibDst->m_pSrcBits, gDibDst->m_iWidth,
		gDibDst->m_iHeight, gDibDst->m_iSWidth,
		gDibSrc->m_pSrcBits, gDibSrc->m_iWidth,
		gDibSrc->m_iHeight, gDibSrc->m_iSWidth,
		(float)gDibDst->m_iWidth / 2, (float)gDibDst->m_iHeight / 2,
		(float)gDibSrc->m_iWidth / 2, (float)gDibSrc->m_iHeight / 2,
		gdAngle, gdScale);

	// Change direction of the scale
	if (gdScale <= _MINSCALE || gdScale >= _MAXSCALE)
	{
		gdScaleDir *= -1.0;
	}

	// Update angle and scale
	gdScale += gdScaleDir;
	gdAngle += 0.02f;

	///double dUpdateT = GetTimer();

	// Copy our rotated image to the screen
	BitBlt(hdc, 0, 0, gDibDst->m_iWidth, gDibDst->m_iHeight,
		gDibDst->m_hdc, 0, 0, SRCCOPY);

	//double dRenderT = GetTimer();

	// Print function timing satistics
	//if (gbTimeFunction)
	//{
	//	char szBuffer[256];
	//	TextOut(hdc, 5, 5, szBuffer,
	//		sprintf(szBuffer, "Update took %3.6f (~%3.2ffps)",
	//			dUpdateT - dStartT, 1.0 / (dUpdateT - dStartT)));
	//	TextOut(hdc, 5, 20, szBuffer,
	//		sprintf(szBuffer, "Render took %3.6f (~%3.2ffps)",
	//			dRenderT - dUpdateT, 1.0 / (dRenderT - dUpdateT)));
	//}
}

BOOL OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
	BOOL bSuccess = FALSE;

	// Load test bitmap
	HBITMAP hbm = (HBITMAP)LoadImage(NULL, SZIMAGE, IMAGE_BITMAP,
		0, 0, LR_LOADFROMFILE);

	if (hbm)
	{
		// Map the bitmap into a dc
		HDC hdc = CreateCompatibleDC(NULL);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);

		// Get info about this bitmap       
		BITMAP bm;
		if (GetObject(hbm, sizeof(BITMAP), &bm) != 0)
		{
			// Convert the bitmap into DIB of known colour depth
			if (gDibSrc->Create(hdc, 0, 0,
				bm.bmWidth, bm.bmHeight))
			{
				// Start the update timer
				SetTimer(hwnd, 0, 100, NULL);
				bSuccess = TRUE;
			}

			// cleanup hdc
			SelectObject(hdc, hbmOld);
			DeleteDC(hdc);
		}
		// delete the loaded image
		DeleteObject(hbm);
	}
	else
	{
		char szError[512];
		//wsprintf(szError, "Error loading image %s", SZIMAGE);
		MessageBox(hwnd, L"rotate", L"oops", MB_OK);
	}
	return bSuccess;
}

/////////////////////////////////////////////////////////////////
void OnSize(HWND hwnd, UINT state, int x, int y)
{
	// Recreate the window DIB to match the size of the window
	gDibDst->Create(NULL, 0, 0, x, y);
}
/////////////////////////////////////////////////////////////////
BOOL OnEraseBkGnd(HWND hwnd, HDC hdc)
{
	// clearing the bk results in tears.
	return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnPaint(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;

	hdc = BeginPaint(hwnd, &ps);
	Update(hdc);
	EndPaint(hwnd, &ps);
}

/////////////////////////////////////////////////////////////////
BOOL OnTimer(HWND hwnd, UINT id)
{
	HDC hdc = GetDC(hwnd);
	Update(hdc);
	ReleaseDC(hwnd, hdc);
	return TRUE;
}
/////////////////////////////////////////////////////////////////
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}


//LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
//	WPARAM wParam, LPARAM lParam)
//{
//	switch (iMsg)
//	{
//		HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
//		HANDLE_MSG(hwnd, WM_SIZE, OnSize);
//		HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
//		HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkGnd);
//		HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
//		HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
//	}
//
//	return DefWindowProc(hwnd, iMsg, wParam, lParam);
//}



// *********************** Bitmap (the component) *******************************

bool Canvas_Bitmap_Component::dial2d(Sheet* pg) {
	if (pg->pCanvasWindow == NULL)
		return clearOutputs();


	int loops = inputsToLoops();
	if (loops == 0)
		return clearOutputs();

	output[0].value.resize(loops);
	output[1].value.resize(loops);

	for (int k = 0; k < loops; k++) {
		int pin = 0;
		int group = 0;

		output[0].value[k] = 0;
		output[1].value[k] = 0;

		getPositionGroup(group++, &pin, k);		// "X Position", "Y Position" 
		getSizeGroup(group++, &pin, k, 10);		// "Width", "Height"
		if ((inputSizeX <= 0) || (inputSizeY <= 0))
			continue;

		string bitmapPath = "test.bmp";
		if ((int)setting[0] & bitSetArray[group++]) {
			bitmapPath = getLoopingStringValueD(pin++, k, "test.bmp");
			if (bitmapPath.size() < 4)
				bitmapPath = "test.bmp";
		}

		LPWSTR path = ConvertToLPWSTR(bitmapPath);
		HBITMAP	hBitmapLoaded = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);
		if (hBitmapLoaded == NULL) {
			//MessageBox(NULL, L"Can't find BMP file.", L"2D Bitmap", MB_OK);
			continue;
		}
		
		// Map the bitmap into a dc
		HDC hdcMem = CreateCompatibleDC(NULL);
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmapLoaded);

		// Get info about this bitmap       
		BITMAP bm;
		if (GetObject(hBitmapLoaded, sizeof(BITMAP), &bm) != 0)
		{
			// Convert the bitmap into DIB of known colour depth
			if (gDibSrc->Create(hdcMem, 0, 0,
				bm.bmWidth, bm.bmHeight))
			{
				// Start the update timer
				//SetTimer(hwnd, 0, 100, NULL);
				//bSuccess = TRUE;
			}

			// cleanup hdc
			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);
		}
		// delete the loaded image
		DeleteObject(hBitmapLoaded);
		
		output[0].value[k] = bm.bmWidth;
		output[1].value[k] = bm.bmHeight; //  these get out eventually

		// NOW finish loading the inputs

		double inputBitmapLeft = 0;
		double inputBitmapTop = 0;
		if ((int)setting[0] & bitSetArray[group++]) {
			inputBitmapLeft = getLoopingValueD(pin++, k, 0); //"Bitmap Left",
			if (inputBitmapLeft <= 0)
				inputBitmapLeft = 0;
			inputBitmapTop = getLoopingValueD(pin++, k, 0); //"Bitmap Top"
			if (inputBitmapTop <= 0)
				inputBitmapTop = 0;
		}

		double inputBitmapWidth = bm.bmWidth;
		double inputBitmapHeight = bm.bmHeight;
		if ((int)setting[0] & bitSetArray[group++]) {
			inputBitmapWidth = getLoopingValueD(pin++, k, bm.bmWidth); //"Bitmap width",
			if (inputBitmapWidth > bm.bmWidth)
				inputBitmapWidth = bm.bmWidth;
			inputBitmapHeight = getLoopingValueD(pin++, k, bm.bmHeight); //"Bitmap height"
			if (inputBitmapHeight > bm.bmHeight)
				inputBitmapHeight = bm.bmHeight;
		}

		double inputAlpha = getLoopingValueD(group++, &pin, k, 1);
		if ((inputAlpha <= 0))
			continue;

		double inputWidth = sizeTo2dCanvas(pg, inputSizeX, displayMode);
		if ((inputWidth <= 0) || (inputWidth > 10000))
			continue;
		double inputHeight = sizeTo2dCanvas(pg, inputSizeY, displayMode);
		if ((inputHeight <= 0) || (inputHeight > 10000))
			continue;

		int xx = xTo2dCanvas2(pg, inputX + setting[2] * inputWidth / 2, displayMode);
		int yy = yTo2dCanvas2(pg, inputY + setting[3] * inputHeight / 2, displayMode);

		StretchBlt(pg->hDC2, xx - inputWidth / 2, yy - inputHeight / 2, inputWidth, inputHeight, hdcMem,
			inputBitmapLeft, inputBitmapTop, inputBitmapWidth, inputBitmapHeight, SRCCOPY);

		DeleteDC(hdcMem);
		DeleteObject(hBitmapLoaded);
	}

	return true;
}

void doubleClick2dBitmapDialog(Sheet* pg, int i) {
	pg->modalDialogComponent = i;

	vector<string> doubleLabels = { "X Offset:",					"Y Offset:" };
	vector<double> doubleValues = { pg->dgrm.cL[i]->setting[2], pg->dgrm.cL[i]->setting[3] };

	vector<string> checkLabels{ "Ignore Display Scale" };
	vector<bool> checkValues = { (bool)pg->dgrm.cL[i]->displayMode };

	buildMultipleValuesDialog(pg, i, doubleLabels, doubleValues, 80,
		checkLabels, checkValues, {}, 0,
		button2dDisplayModeAndDashAndOffset_Completion);
}
std::vector<Button*>bitmapButtonList = {
	new Button(3, 40, 85, 20, L"Inputs...", buildUniversalPinDialogButtons),
	new Button(3, 60, 85, 20, L"Options...", doubleClick2dBitmapDialog) };


bool Canvas_Bitmap_Component::onLoad(Sheet* pg) {
	if (setting.size() < 4) {
		setSetting(1, 0); // spare
		setSetting(2, 0); // x offset
		setSetting(3, 0); // y offset
	}
	buildPinLabels();
	return true;
}
vector<int> Canvas_Bitmap_Component::GetPinGroups() { return vector<int>{2, 2, 1, 2, 2, 1 }; }
vector<string> Canvas_Bitmap_Component::GetPinLabels() {
	return vector<string>{
		"X Position", "Y Position",	// 0
			"Width", "Height",			// 1
			"Source Path",				// 2
			"Left", "Top",				// 3
			"Right", "Bottom",			// 4
			"Alpha" };					// 5
}
Canvas_Bitmap_Component::Canvas_Bitmap_Component()
	: Canvas_Component(L"2dBitmap", L"2D Bitmap", squareShape, 5, 1, 2, 1)
{
	dialogButtonList = bitmapButtonList;
	setRightLabels(3, "Width", "Height");
	setSetting(0, 1 + 2); // input pins
	setSetting(1, 0); // spare
	setSetting(2, 0); // x offset
	setSetting(3, 0); // y offset
	pinMaskIndex = 0;
	width = 35;
}
