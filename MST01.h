#pragma once

#include "resource.h"

void Open2DView();
void Open3DView();
void quitProgram();



class Window
{
public:
	HWND GetHWND() { return m_hwnd; }
protected:
	virtual LRESULT HandleMessage(
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void PaintContent(PAINTSTRUCT *pps) { }
	virtual LPCTSTR ClassName() = 0;
	virtual BOOL WinRegisterClass(WNDCLASS *pwc)
	{
		return RegisterClass(pwc);
	}
	virtual ~Window() { }

	HWND WinCreateWindow(DWORD dwExStyle, LPCTSTR pszName,
		DWORD dwStyle, int x, int y, int cx, int cy,
		HWND hwndParent, HMENU hmenu);
private:
	void Register();
	void OnPaint();
	void OnPrintClient(HDC hdc);
	static LRESULT CALLBACK s_WndProc(HWND hwnd,
		UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	HWND m_hwnd;
};


class CanvasWindow : public Window
{
public:
	virtual LPCTSTR ClassName() { return TEXT("Canvas"); }
	static CanvasWindow *Create();
protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate();
	virtual void PaintContent(PAINTSTRUCT *pps);
private:
	HWND m_hwndChild;
};

class RootWindow : public Window
{
public:
	virtual LPCTSTR ClassName() { return TEXT("MST Workshop"); }
	static RootWindow *Create();
	virtual void PaintContent(PAINTSTRUCT *pps);

protected:
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate();
private:
	HWND m_hwndChild;
};