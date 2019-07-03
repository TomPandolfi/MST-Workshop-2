#include "stdafx.h"
#include "windowsx.h"
#include "structs.h"
#include "Mouse.h"
#include "FrameButtons.h"
#include "ButtonGroups.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "Text.h"
#include <string>
#include "Printer.h"
#include "MST01.h"
#include "color.h"
#include "Fileio.h"

// Include these also 
#include <windows.h>
#include <winspool.h>
#include <Commdlg.h> 


extern void drawAllComponents(Sheet*, HDC);
extern void drawAllWires(Sheet*, HDC);
extern HDC hdcOffScreen;
extern Sheet* page;
//extern void BuildOffScreenBuffer(Sheet* pg, HWND hWnd, HDC hDC);


HDC GetPrinterDC(HWND Hwnd)
{
	// Initialize a PRINTDLG structure's size and set the PD_RETURNDC flag set the Owner flag to hwnd.
	// The PD_RETURNDC flag tells the dialog to return a printer device context.
	HDC hdc;
	PRINTDLG pd = { 0 };
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = Hwnd;
	pd.Flags = PD_RETURNDC;

	// Retrieves the printer DC
	PrintDlg(&pd);
	hdc = pd.hDC;
	return hdc;

}

extern HBITMAP offScreenBuffer;

void PrintMainWindow(HWND hwnd, HDC hDC, int inwidth, int inheight)
{
	DOCINFO di = { sizeof(DOCINFO), TEXT("Printing Picture...") };

	HDC prn;
	HBITMAP hBitmap;

	prn = GetPrinterDC(hwnd);
	if (prn == NULL)
		return; // user canceled

	int cxpage = GetDeviceCaps(prn, HORZRES);
	int cypage = GetDeviceCaps(prn, VERTRES);
	HDC hdcMem = CreateCompatibleDC(prn);

	hBitmap = (HBITMAP)LoadImage(NULL, L"printer_template.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);
	if(hBitmap == NULL) {
		MessageBox(NULL, L"Can't find file 'printer_template.bmp'.", L"Print Workspace", MB_OK);
		return;
	}
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	StartDoc(prn, &di);
	StartPage(prn);
	SetMapMode(prn, MM_ISOTROPIC);
	SetWindowExtEx(prn, cxpage, cypage, NULL);
	SetViewportExtEx(prn, cxpage, cypage, NULL);

	double zoomFactor = 4;
	if (((inwidth) > 100) && ((inheight) > 100)) {
		double zoomX = (double)cxpage / (double)inwidth;
		double zoomY = (double)cypage / (double)inheight;
		zoomFactor = min(zoomX, zoomY);
	}
	int bxWidth = cxpage / zoomFactor;
	int bxHeight = cypage / zoomFactor;

	SetViewportOrgEx(prn, 0, 0, NULL);

	drawCanvasForReal(page, hwnd, hdcMem, true);

	StretchBlt(prn, 0, 0, cxpage, cypage, hdcMem, 0, 0, bxWidth, bxHeight, SRCCOPY);
	EndPage(prn);
	EndDoc(prn);
	DeleteDC(prn);
	DeleteDC(hdcMem);
}

//extern HBITMAP offScreenBuffer2;
//extern HDC hdcOffScreen2;

void Print2dViewWindow(HWND hwnd, HDC hDC, int inwidth, int inheight)
{
	DOCINFO di = { sizeof(DOCINFO), TEXT("Printing Picture...") };

	HDC prn;
	HBITMAP hBitmap;

	prn = GetPrinterDC(hwnd);
	if (prn == NULL)
		return; // user canceled

	int cxpage = GetDeviceCaps(prn, HORZRES);
	int cypage = GetDeviceCaps(prn, VERTRES);
	HDC hdcMem = CreateCompatibleDC(prn);

	hBitmap = (HBITMAP)LoadImage(NULL, L"printer_template.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE | LR_VGACOLOR);
	if (hBitmap == NULL) {
		MessageBox(NULL, L"Can't find file 'printer_template.bmp'.", L"Print 2D View", MB_OK);
		return;
	}
	HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

	StartDoc(prn, &di);
	StartPage(prn);
	SetMapMode(prn, MM_ISOTROPIC);
	SetWindowExtEx(prn, cxpage, cypage, NULL);
	SetViewportExtEx(prn, cxpage, cypage, NULL);

	double zoomFactor = 4;
	if (((inwidth) > 100) && ((inheight) > 100)) {
		double zoomX = (double)cxpage / (double)inwidth;
		double zoomY = (double)cypage / (double)inheight;
		zoomFactor = min(zoomX, zoomY);
	}
	int bxWidth = cxpage / zoomFactor;
	int bxHeight = cypage / zoomFactor;

	SetViewportOrgEx(prn, 0, 0, NULL);

	drawCanvas2ForReal(hwnd, hdcMem);
	
	StretchBlt(prn, 0, 0, cxpage, cypage, hdcMem, 0, 0, bxWidth, bxHeight, SRCCOPY);
	EndPage(prn);
	EndDoc(prn);
	DeleteDC(prn);
	DeleteDC(hdcMem);
}


#include <iostream>
#include "stdafx.h"
#include <vector>
#include <fstream>
#include <cstring>
#include <windows.h>

using namespace std;
#if 0
bool HDCToFile(const char* FilePath, HDC Context, RECT Area, uint16_t BitsPerPixel = 24)
{
	uint32_t Width = Area.right - Area.left;
	uint32_t Height = Area.bottom - Area.top;
	BITMAPINFO Info;
	BITMAPFILEHEADER Header;
	memset(&Info, 0, sizeof(Info));
	memset(&Header, 0, sizeof(Header));
	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = Width;
	Info.bmiHeader.biHeight = Height;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = BitsPerPixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = Width * Height * (BitsPerPixel > 24 ? 4 : 3);
	Header.bfType = 0x4D42;
	Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	char* Pixels = NULL;
	HDC MemDC = CreateCompatibleDC(Context);
	HBITMAP Section = CreateDIBSection(Context, &Info, DIB_RGB_COLORS, (void**)&Pixels, 0, 0);
	DeleteObject(SelectObject(MemDC, Section));

	BOOL ReturnValue;
	ReturnValue = BitBlt(MemDC, 0, 0, Width, Height, Context, Area.left, Area.top, SRCCOPY);
	if (ReturnValue == TRUE)
	{
		cout << "Snapshot Successful\n";
	}
	if (ReturnValue == FALSE)
	{
		cout << "Snapshot FAILED!!!\n";
		DWORD LastError;
		LastError = GetLastError();
		cout << "Last Error Was : " << LastError << "\n";
	}
	DeleteDC(MemDC);
	std::fstream hFile(FilePath, std::ios::out | std::ios::binary);
	if (hFile.is_open())
	{
		hFile.write((char*)&Header, sizeof(Header));
		hFile.write((char*)&Info.bmiHeader, sizeof(Info.bmiHeader));
		hFile.write(Pixels, (((BitsPerPixel * Width + 31) & ~31) / 8) * Height);
		hFile.close();
		DeleteObject(Section);
		return true;
	}
	DeleteObject(Section);
	return false;
}
#endif


void PrintTheMainWindow(Sheet* pg)
{
#if 0
	// get extent of workspace
	int minX = 1e10;
	int minY = 1e10;
	int maxX = -1e10;
	int maxY = -1e10;
	Component* comp;
	for (int j = 0; j < pg->dgrm.cL.size(); j++) {
		comp = pg->dgrm.cL[j];
		if (comp->x + comp->width > maxX)
			maxX = comp->x + comp->width;
		if (comp->x - comp->width < minX)
			minX = comp->x - comp->width;

		if (comp->y + comp->height > maxY)
			maxY = comp->y + comp->height;
		if (comp->y - comp->height < minY)
			minY = comp->y - comp->height;
	}
	PrintMainWindow(pg->hWnd, pg->hDC, minX, maxX, minY, maxY);
#endif

	PrintMainWindow(pg->hWnd, pg->hDC, pg->workspaceWidth, pg->workspaceHeight);
}

void PrintTheCanvasWindow(Sheet* pg)
{
	RECT clientRect;
	GetClientRect(pg->hWnd2, &clientRect);
	Print2dViewWindow(pg->hWnd2, pg->hDC2, clientRect.right, clientRect.bottom);
}

#if 0
void SaveBmpTheMainWindow(Sheet* pg ) {
	
	// setup off screen buffer
	RECT    clientRect;
	GetClientRect(pg->hWnd, &clientRect);

	HDC hdcOffScreen = CreateCompatibleDC(pg->hDC);
	HBITMAP offScreenBuffer = CreateCompatibleBitmap(hdcOffScreen, clientRect.right, clientRect.bottom);

	SelectObject(hdcOffScreen, offScreenBuffer);

	//page->hDC = local_hdc;

	BeginPath(hdcOffScreen);
	MoveToEx(hdcOffScreen, 0, 0, NULL);
	LineTo(hdcOffScreen, 0, clientRect.bottom);
	LineTo(hdcOffScreen, clientRect.right, clientRect.bottom);
	LineTo(hdcOffScreen, clientRect.right, 0);
	LineTo(hdcOffScreen, 0, 0);
	EndPath(hdcOffScreen);

	HBRUSH fillColor = GetColorBrush(0xFFFFFF);
	SelectObject(hdcOffScreen, fillColor);
	FillPath(hdcOffScreen);
		
	if (showGraphPaperButton->isSelected) {

		int gridSpacing = 10;
		BeginPath(hdcOffScreen);
		int xStart = pg->cXo % gridSpacing;
		for (int xx = xStart; xx < clientRect.right; xx += gridSpacing) {
			MoveToEx(hdcOffScreen, xx, 0, NULL);
			LineTo(hdcOffScreen, xx, clientRect.bottom);
		}
		int yStart = pg->cYo % gridSpacing;
		for (int yy = yStart; yy < clientRect.bottom; yy += gridSpacing) {
			MoveToEx(hdcOffScreen, 0, yy, NULL);
			LineTo(hdcOffScreen, clientRect.right, yy);
		}
		EndPath(hdcOffScreen);

		SelectObject(hdcOffScreen, GetColorPen(RGB(220, 240, 255), 1));
		StrokePath(hdcOffScreen);
	}
	SelectObject(hdcOffScreen, GetColorPen(0, 1));

	drawAllComponents(pg, hdcOffScreen);
	drawAllWires(pg, hdcOffScreen);

	HDCToFile("FooBar5.bmp", hdcOffScreen, { 0, 0, clientRect.right, clientRect.bottom });
	ReleaseDC(pg->hWnd, hdcOffScreen);
}

void SaveBmpTheCanvasWindow(Sheet* pg ){
	if (pg->pCanvasWindow == NULL)
		return;
	HDCToFile("FooBar4.bmp", pg->hDC2, { 0, 0, pg->canvas2Width, pg->canvas2Height });
	ReleaseDC(pg->hWnd2, pg->hDC2);
}
#endif