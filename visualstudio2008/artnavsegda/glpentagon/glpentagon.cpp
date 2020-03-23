
/* An example of the minimal Win32 & OpenGL program.  It only works in
   16 bit color modes or higher (since it doesn't create a
   palette). */


#include <stdio.h>
#define _USE_MATH_DEFINES // for C
#include <math.h>

#include <windows.h>			/* must include this before GL/gl.h */

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>

const double ugol = M_PI*72/180;

double getcortx(int num, double radius, int angle)
{
	return cos(M_PI*angle/180*(double)num)*radius;
}

double getcorty(int num, double radius, int angle)
{
	return sin(M_PI*angle/180*(double)num)*radius;
}

void regularPolygon(float radius, int sides)
{
	int angle = 360/sides;

	glBegin(GL_POLYGON);
	for (int i = 0; i < sides; ++i)
	{
		glIndexi(i);
		glVertex2d(getcortx(i,radius,angle), getcorty(i,radius,angle));
	}
	glEnd();
}

void
display()
{
    /* rotate a triangle around */
    glClear(GL_COLOR_BUFFER_BIT);

#define myradius 1.0

#define B0 glIndexi(1); glColor3f(1.0f, 1.0f, 1.0f); glVertex2d(0, 0)
#define B1 glIndexi(1); glColor3f(0.0f, 1.0f, 0.0f); glVertex2d(getcortx(1,myradius), getcorty(1,myradius))
#define B2 glIndexi(2); glColor3f(0.0f, 0.0f, 1.0f); glVertex2d(getcortx(2,myradius), getcorty(2,myradius))
#define B3 glIndexi(3); glColor3f(1.0f, 1.0f, 0.0f); glVertex2d(getcortx(3,myradius), getcorty(3,myradius))
#define B4 glIndexi(4); glColor3f(0.0f, 1.0f, 1.0f); glVertex2d(getcortx(4,myradius), getcorty(4,myradius))
#define B5 glIndexi(5); glColor3f(1.0f, 0.0f, 1.0f); glVertex2d(getcortx(5,myradius), getcorty(5,myradius))

	//glBegin(GL_POLYGON);
 //   B1; B2; B3; B4; B5;
	//glEnd();

	//glBegin(GL_TRIANGLE_FAN);
 //   B0; B1; B2; B3; B4; B5; B1;
	//glEnd();

	regularPolygon(1.0, 8);

    glFlush();
}


LONG WINAPI
WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
    static PAINTSTRUCT ps;

    switch(uMsg) {
    case WM_PAINT:
	display();
	BeginPaint(hWnd, &ps);
	EndPaint(hWnd, &ps);
	return 0;

    case WM_SIZE:
	glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
	PostMessage(hWnd, WM_PAINT, 0, 0);
	return 0;

    case WM_CHAR:
	switch (wParam) {
	case 27:			/* ESC key */
	    PostQuitMessage(0);
	    break;
	}
	return 0;

    case WM_CLOSE:
	PostQuitMessage(0);
	return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

HWND
CreateOpenGLWindow(char* title, int x, int y, int width, int height, 
		   BYTE type, DWORD flags)
{
    int         pf;
    HDC         hDC;
    HWND        hWnd;
    WNDCLASS    wc;
    PIXELFORMATDESCRIPTOR pfd;
    static HINSTANCE hInstance = 0;

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
	hInstance = GetModuleHandle(NULL);
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "OpenGL";

	if (!RegisterClass(&wc)) {
	    MessageBox(NULL, "RegisterClass() failed:  "
		       "Cannot register window class.", "Error", MB_OK);
	    return NULL;
	}
    }

    hWnd = CreateWindow("OpenGL", title, WS_OVERLAPPEDWINDOW |
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			x, y, width, height, NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
	MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
		   "Error", MB_OK);
	return NULL;
    }

    hDC = GetDC(hWnd);

    /* there is no guarantee that the contents of the stack that become
       the pfd are zeroed, therefore _make sure_ to clear these bits. */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize        = sizeof(pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
    pfd.iPixelType   = type;
    pfd.cColorBits   = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
	MessageBox(NULL, "ChoosePixelFormat() failed:  "
		   "Cannot find a suitable pixel format.", "Error", MB_OK); 
	return 0;
    } 
 
    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
	MessageBox(NULL, "SetPixelFormat() failed:  "
		   "Cannot set format specified.", "Error", MB_OK);
	return 0;
    } 

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    ReleaseDC(hWnd, hDC);

    return hWnd;
}    

int APIENTRY
WinMain(HINSTANCE hCurrentInst, HINSTANCE hPreviousInst,
	LPSTR lpszCmdLine, int nCmdShow)
{
    HDC hDC;				/* device context */
    HGLRC hRC;				/* opengl context */
    HWND  hWnd;				/* window */
    MSG   msg;				/* message */

    hWnd = CreateOpenGLWindow("minimal", 0, 0, 1000, 1000, PFD_TYPE_RGBA, 0);
    if (hWnd == NULL)
	exit(1);

    hDC = GetDC(hWnd);
    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    ShowWindow(hWnd, nCmdShow);

    while(GetMessage(&msg, hWnd, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    wglMakeCurrent(NULL, NULL);
    ReleaseDC(hWnd, hDC);
    wglDeleteContext(hRC);
    DestroyWindow(hWnd);

    return msg.wParam;
}
