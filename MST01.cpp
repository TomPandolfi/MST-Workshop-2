// MST01.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MST01.h"
#include "FrameButtons.h"
#include "Mouse.h"
#include "ButtonGroups.h"
#include "Sheet.h"
#include "buildComponent.h"
#include "CanvasComponents.h"
#include "ThreeDComponents.h"
#include "Time.h"
#include <stdlib.h>
#include "Sheet.h"
#include <windows.h>
#include <windowsx.h>
#include <GLFW/glfw3.h>


#if MEMORY_CHECK
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#pragma warning (disable: 4305)
#pragma warning (disable: 4309)

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND localHwnd;

int temp_threeDwindowHandle = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

extern Sheet* page;



void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
	MessageBox(NULL, L"Open GL, GLEW ERROR.", L"error_callback", MB_OK);
}

void display3dView(void);
void resize_callback(GLFWwindow* window3d, int width, int height)
{
	//display3dView();
	page->update3Dview = true;
}

void Open3DView() {

	if (page->GLFW_window != NULL)
		glfwDestroyWindow(page->GLFW_window);

	if (!glfwInit())
	{
		// Initialization failed
		MessageBox(NULL, L"Open GL, GLEW Initialization failed.", L"glfwInit", MB_OK);
	}
	glfwSetErrorCallback(error_callback); // attach the above error display
	
	page->GLFW_window = glfwCreateWindow(640, 480, "MST Workshop - 3D View", NULL, NULL);
	if (!page->GLFW_window)
	{
		MessageBox(NULL, L"Open GL, GLEW Window failed to open.", L"glfwCreateWindow", MB_OK);
	}

	glfwMakeContextCurrent(page->GLFW_window);
	glfwSetWindowSizeCallback(page->GLFW_window, resize_callback);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	for (int light = 0; light < MY_GL_MAX_LIGHTS; light++)
		page->light_enable[light] = false;

	page->update3Dview = true;
}
void Open3DViewPlus( int posX, int posY, int width, int height) {

	if (page->GLFW_window != NULL) 
		glfwDestroyWindow(page->GLFW_window);
	
	if (!glfwInit())
	{
		// Initialization failed
		MessageBox(NULL, L"Open GL, GLEW Initialization failed.", L"glfwInit", MB_OK);
	}
	glfwSetErrorCallback(error_callback); // attach the above error display
	
	page->GLFW_window = glfwCreateWindow(width, height, "MST Workshop - 3D View", NULL, NULL);
	if (!page->GLFW_window)
	{
		MessageBox(NULL, L"Open GL, GLEW Window failed to open.", L"glfwCreateWindow", MB_OK);
	}
	
	glfwSetWindowPos(page->GLFW_window, posX, posY);
	glfwMakeContextCurrent(page->GLFW_window);
	glfwSetWindowSizeCallback(page->GLFW_window, resize_callback);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	for (int light = 0; light < MY_GL_MAX_LIGHTS; light++)
		page->light_enable[light] = false;

	page->update3Dview = true;
}

HDC magic_hDC;


void display3dView(void)
{
	if (!page->update3Dview)
		return;
	page->update3Dview = false;

	glClearColor(page->canvas3Red, page->canvas3Green, page->canvas3Blue, 1.0);

	float ratio;
	int width, height;
	
	glfwGetFramebufferSize(page->GLFW_window, &width, &height);
	ratio = width / (float)height;
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float dist = 128;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (width <= height)
		glFrustum(-dist, dist, -dist/ratio, dist/ratio, 2.5f * dist, 20.0f * dist);
	else
		glFrustum(-dist*ratio, dist*ratio, -dist, dist,  2.5f * dist, 20.0f * dist);
	
//	glFrustum(-dist, dist, -dist, dist, 2.5f * dist, 20.0f * dist);

	glTranslatef(0.0F, 0.0F, -5.0f * dist);

	glRotated(page->canvas3xRotate, 1.0f, 0.0f, 0.0f);
	glRotated(page->canvas3yRotate, 0.0f, 1.0f, 0.0f);
	glRotated(page->canvas3zRotate, 0.0f, 0.0f, 1.0f);
	glTranslated(page->canvas3xOffset, page->canvas3yOffset, page->canvas3zOffset);
	glScaled(page->canvas3Scale, page->canvas3Scale, page->canvas3Scale);

	int centerX = width / 2;// -mainWinInfo->horizontalPosition;
	int centerY = height / 2;//- mainWinInfo->verticalPosition;

	if (enable3dDefaultLightingButton->isSelected) {
		GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
		GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
		GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
		//	/* light_position is NOT default value */
		GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	else {

		glDisable(GL_LIGHT0);


		for (int light = 1; light < MY_GL_MAX_LIGHTS; light++) {
			int my_light = GL_LIGHT0 + light;
			
			if( !page->light_enable[light]) {
				glDisable(my_light);
				continue;
			}

			int setting = page->light_bitmask[light];
			glEnable(my_light);
			if (setting & 1) // ambient
				glLightfv(my_light, GL_AMBIENT, page->light_ambient[light]);
			if (setting & 2) // diffuse
				glLightfv(my_light, GL_DIFFUSE, page->light_diffuse[light]);
			if (setting & 4) // specular
				glLightfv(my_light, GL_SPECULAR, page->light_specular[light]);
			if (setting & 8) // position
				glLightfv(my_light, GL_POSITION, page->light_position[light]);
			if (setting & 16) // spot direction
				glLightfv(my_light, GL_SPOT_DIRECTION, page->light_direction[light]);
			if (setting & 32) // spot exponent
				glLightfv(my_light, GL_SPOT_EXPONENT, &page->light_exponent[light]);
			if (setting & 64) // spot cutoff
				glLightfv(my_light, GL_SPOT_CUTOFF, &page->light_cutoff[light]);
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < page->dgrm.cL.size(); i++) {
		page->dgrm.cL[i]->new3dOjbect();
	}


	glfwSwapBuffers(page->GLFW_window);
	//	glfwPollEvents();
}



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	int argc = 0;
	char argv = 'GLUT';
	char *argvptr;
	argvptr = &argv;
	char **argvptrptr;
	argvptrptr = &argvptr;

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MST01, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MST01));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			if (page->GLFW_window) {
				if (glfwWindowShouldClose(page->GLFW_window)) {
					glfwDestroyWindow(page->GLFW_window);
					page->GLFW_window = NULL;
				}
			}
			if (page->GLFW_window)
				display3dView();
			
			argc = 0;
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MST01));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_MST01);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
		return FALSE;
	localHwnd = hWnd;

	buildSheets(hWnd, temp_threeDwindowHandle);

	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(page));
	//Sheet* testPage;
	//testPage = reinterpret_cast<Sheet *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	
	buildTopFrameButtons(page, hWnd);
	buildBottomFrameButtons(page, hWnd);
	LoadButtonGroups(page);
	loadComponentButtons(page);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	
#if 0
	switch (message) {
	case 70:
	case 71:
	case 36:
	case 129:
	case 131:
	case 1:
	case 24:
	case 28:
	case 134:	
	case 127:
	case 6:
	case 641:
	case 642:
	case 7:
	case 133:
	case 20:
	case 5:
	case 3:
	case 15:
	case 799:
	case 49444:
	case 49562:
	case 132:
	case 32:
	case 136:
	case 8:
	case 512:
	case 160:
	case 0:

		break;
	default:
		int j = 0;
	}
#endif

	switch (message)
	{

	case WM_ERASEBKGND:
		break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		if (hDC != NULL) {
			drawCanvasForReal(page, hWnd, hDC, false);
			EndPaint(hWnd, &ps);
		}
	}
	break;
	case WM_CLOSE:

		//_CrtDumpMemoryLeaks();

		if (page->dgrm.cL.size() > 0) {
			if (page->isDirty) {
				int quitResult = MessageBox(NULL, L"You have unsaved changes. Do you want to save them now?", 
					L"MST Workshop", MB_YESNOCANCEL | MB_ICONWARNING);
				if (quitResult == IDYES)
					writeComponentsAndWires(page);
				if (quitResult == IDCANCEL)
					break;
			}
		}
		glfwTerminate();
		DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN: // LEFT BUTTON DOWN
		hDC = BeginPaint(hWnd, &ps);
		page->hDC = hDC;
		magic_hDC = page->hDC;
		MouseDownEventListener(page, lParam);
		EndPaint(hWnd, &ps);
		break;
	case WM_MOUSEMOVE: // MOUSE MOVE
		hDC = BeginPaint(hWnd, &ps);
		page->hDC = hDC;
		MouseMoveEventListener(page, lParam);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONUP: // LEFT BUTTON UP
		hDC = BeginPaint(hWnd, &ps);
		page->hDC = hDC;
		MouseUpEventListener(page, lParam);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDBLCLK: // DOUBLE - doesnt work
		break;
	case WM_RBUTTONDOWN: // RIGHT BUTTON DOWN
		hDC = BeginPaint(hWnd, &ps);
		page->hDC = hDC;
		MouseDownRightEventListener(page, lParam);
		EndPaint(hWnd, &ps);
		break;
	case WM_RBUTTONUP:	// RIGTH BUTTON UP
		hDC = BeginPaint(hWnd, &ps);
		page->hDC = hDC;
		MouseUpRightEventListener(page, lParam);
		EndPaint(hWnd, &ps);
		break;
	case WM_CHAR:
		KeyboardCharListener(page, (wchar_t)wParam);
		break;
	case WM_KEYDOWN:
		KeyboardKeydownListener(page, (wchar_t)wParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



void drawCanvas() {

	InvalidateRect(localHwnd, NULL, true);
	UpdateWindow(localHwnd);
}

void quitProgram() {
	int quitResult;
	if (page->dgrm.cL.size() > 0) {
		if (page->isDirty) {
			quitResult = MessageBox(NULL, L"You have unsaved changes. Do you want to save them now?", 
				L"MST Workshop", MB_YESNOCANCEL | MB_ICONWARNING);
			if (quitResult == IDYES)
				writeComponentsAndWires(page);
			if (quitResult == IDCANCEL)
				return;
		}
	}
	PostQuitMessage(0);
	return;
}


// ************************** Window *************************
HINSTANCE g_hinst;


HWND Window::WinCreateWindow(DWORD dwExStyle, LPCTSTR pszName,
	DWORD dwStyle, int x, int y, int cx, int cy,
	HWND hwndParent, HMENU hmenu)
{
	Register();
	return CreateWindowEx(dwExStyle, ClassName(), pszName, dwStyle,
		x, y, cx, cy, hwndParent, hmenu, g_hinst, this);
}

void Window::Register()
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = Window::s_WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hinst;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClassName();

	WinRegisterClass(&wc);
}

LRESULT CALLBACK Window::s_WndProc(
	HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window *self;
	if (uMsg == WM_NCCREATE) {
		LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
		self = reinterpret_cast<Window *>(lpcs->lpCreateParams);
		self->m_hwnd = hwnd;
		SetWindowLongPtr(hwnd, GWLP_USERDATA,
			reinterpret_cast<LPARAM>(self));
	}
	else {
		self = reinterpret_cast<Window *>
			(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}
	if (self) {
		return self->HandleMessage(uMsg, wParam, lParam);
	}
	else {
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

LRESULT Window::HandleMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lres;

	switch (uMsg) {
	case WM_NCDESTROY:
		lres = DefWindowProc(m_hwnd, uMsg, wParam, lParam);
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, 0);
		delete this;
		return lres;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_PRINTCLIENT:
		OnPrintClient(reinterpret_cast<HDC>(wParam));
		return 0;
	}

	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void Window::OnPaint()
{
	PAINTSTRUCT ps;
	BeginPaint(m_hwnd, &ps);
	PaintContent(&ps);
	EndPaint(m_hwnd, &ps);
}

void Window::OnPrintClient(HDC hdc)
{
	PAINTSTRUCT ps;
	ps.hdc = hdc;
	GetClientRect(m_hwnd, &ps.rcPaint);
	PaintContent(&ps);
}


// **************************** 2D View *****************************

LRESULT CanvasWindow::OnCreate()
{
	return 0;
}

LRESULT CanvasWindow::HandleMessage(
	UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	//HDC hDC;

	if (uMsg == 20)
		return 0;

	switch (uMsg) {
	case WM_CREATE:
		return OnCreate();

		//case WM_ERASEBKGND:
		//	break;
	case WM_LBUTTONDOWN:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseDownCanvasListener(page, lParam);
		EndPaint(m_hwnd, &ps);
		break;
	case WM_MOUSEMOVE:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseMoveCanvasListener(page, lParam);
		EndPaint(m_hwnd, &ps);
		break;
	case WM_LBUTTONUP:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseUpCanvasListener(page, lParam);
		EndPaint(m_hwnd, &ps);
		break;

	case WM_RBUTTONDOWN:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);		
		MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_RIGHT, true);
		EndPaint(m_hwnd, &ps);
		break;
	case WM_RBUTTONUP:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_RIGHT, false);
		EndPaint(m_hwnd, &ps);
		break;

	case WM_MBUTTONDOWN:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_CENTER, true);
		EndPaint(m_hwnd, &ps);
		break;
	case WM_MBUTTONUP:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_CENTER, false);
		EndPaint(m_hwnd, &ps);
		break;

	case WM_XBUTTONDOWN:
		if (lParam == NULL)
			break; 
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		if(HIWORD(wParam) & XBUTTON1)
			MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_XLEFT, true);
		if (HIWORD(wParam) & XBUTTON2)
			MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_XRIGHT, true);
		EndPaint(m_hwnd, &ps);
		break;
	case WM_XBUTTONUP:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		if (HIWORD(wParam) & XBUTTON1)
			MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_XLEFT, false);
		if (HIWORD(wParam) & XBUTTON2)
			MouseUpDownCanvasButtonListener(page, MOUSE_BUTTON_XRIGHT, false);
		EndPaint(m_hwnd, &ps);
		break;

	case WM_MOUSEWHEEL:
		if (lParam == NULL)
			break;
		page->hDC2 = BeginPaint(m_hwnd, &ps);
		MouseWheelCanvasButtonListener(page, GET_WHEEL_DELTA_WPARAM(wParam));
		EndPaint(m_hwnd, &ps);
		break;

	case WM_NCDESTROY:
		// Death of the root window ends the thread
	//	PostQuitMessage(0);
		page->pCanvasWindow = NULL;
		break;

	case WM_SIZE:
		if (m_hwndChild) {
			SetWindowPos(m_hwndChild, NULL, 0, 0,
				GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam),
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
		drawCanvas2();
		return 0;

	case WM_SETFOCUS:
		if (m_hwndChild) {
			SetFocus(m_hwndChild);
		}
		return 0;

	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

CanvasWindow *CanvasWindow::Create()
{
	CanvasWindow *self = new CanvasWindow();
	if (self && self->WinCreateWindow(0,
		TEXT("MST Workshop - 2D View"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL)) {
		return self;
	}
	delete self;
	return NULL;
}
void CanvasWindow::PaintContent(PAINTSTRUCT *pps) {

	HWND hWnd = GetHWND();
	drawCanvas2ForReal(hWnd, pps->hdc);
	drawCanvasWindowCounter++;
}


void drawCanvas2() {
	if (page->pCanvasWindow == NULL)
		return;
	HWND hwnd = page->pCanvasWindow->GetHWND();
	if (hwnd == NULL)
		return;
	InvalidateRect(hwnd, NULL, true);
	UpdateWindow(hwnd);
}

void Open2DView() {
	if (page->pCanvasWindow == NULL) {
		page->pCanvasWindow = CanvasWindow::Create();
		if (page->pCanvasWindow == NULL)
			return;
		ShowWindow(page->pCanvasWindow->GetHWND(), 10);
	}
	BringWindowToTop(page->pCanvasWindow->GetHWND());
}
void Open2DViewPlus( int posX, int posY, int width, int height) {
	
	if (page->pCanvasWindow == NULL) {
		page->pCanvasWindow = CanvasWindow::Create();
		if (page->pCanvasWindow == NULL)
			return;
		ShowWindow(page->pCanvasWindow->GetHWND(), 10);
		SetWindowPos(page->pCanvasWindow->GetHWND(), NULL, posX, posY, width, height, 0);
	}
	BringWindowToTop(page->pCanvasWindow->GetHWND());
}


