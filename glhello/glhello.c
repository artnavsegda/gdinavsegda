#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "font.h"

#define VSYNC 0

typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

GLuint fontOffset;
char configfile[] = ".\\glhello.ini";
GLfloat xscale = 1.0;
GLfloat yscale = 1.0;
GLint xoffset = 0;
GLint yoffset = 0;
int xwidth, yheight;
int l = 10000;
POINT mouse;
int mousex, mousey;
int deltax, deltay;

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	sprintf(Buffer, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}

float mousexprev = 0;
float xscaleprev = 1.0;
float sourcexprev = 0;

int sourcetodest(int source)
{
	if (sourcexprev == 0)
		return l / ((float)xwidth / (float)mousex);
	else
		return sourcexprev - ((mousexprev - mousex) / xscaleprev);
}

static HDC hDC;

int render(int animate)
{
	char string[10];
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(1.0, 1.0, 1.0);
		glRasterPos2i(10, 10);
		glPushAttrib(GL_LIST_BIT);
		glListBase(fontOffset);
		snprintf(string, 5, "%5d", mousex);
		glCallLists(5, GL_UNSIGNED_BYTE, string);
		glRasterPos2i(10, 30);
		snprintf(string, 5, "%5d", mousey);
		glCallLists(5, GL_UNSIGNED_BYTE, string);
		glPopAttrib();
		//glPushMatrix();
		//glTranslatef(0.0, animate, 0.0);
		//glRectf(10, 10, 20, 20);
		//glPopMatrix();
		glBegin(GL_LINES);
		glVertex2i(mousex, 0);
		glVertex2i(mousex, yheight);
		glEnd();
		glBegin(GL_LINES);
		glVertex2i(0, mousey*-1+yheight);
		glVertex2i(xwidth, mousey*-1 + yheight);
		glEnd();
		glFlush();
		SwapBuffers(hDC);
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd,UINT	message,WPARAM wParam,LPARAM lParam)
{
	static HGLRC hRC;
	switch (message)
	{
	case WM_CONTEXTMENU:
		TrackPopupMenu(GetSubMenu(GetMenu(hWnd), 0), TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hWnd, NULL);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0:
			DestroyWindow(hWnd);
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
			if (GetMenuState(GetMenu(hWnd), LOWORD(wParam), MF_BYCOMMAND) & MF_CHECKED)
				CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_BYCOMMAND | MF_UNCHECKED);
			else
				CheckMenuItem(GetMenu(hWnd), LOWORD(wParam), MF_BYCOMMAND | MF_CHECKED);
			break;
		}
		break;
	case WM_CREATE:
		hDC = GetDC(hWnd);
		static PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),1,PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,8,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,PFD_MAIN_PLANE,0,0,0,0 };
		SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapIntervalEXT(VSYNC);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		fontOffset = glGenLists(128);
		for (GLuint i = 32; i < 127; i++)
		{
			glNewList(i + fontOffset, GL_COMPILE);
			glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, font[i - 32]);
			glEndList();
		}
		glListBase(fontOffset);
		break;
	case WM_DESTROY:
		WritePrivateProfileInt("window", "height", yheight, configfile);
		WritePrivateProfileInt("window", "width", xwidth, configfile);
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (GetKeyState(VK_CONTROL)<0)
			switch (wParam)
			{
			case VK_RIGHT:
				//glScalef(2.0,1.0,1.0);
				xscale = xscale * 2.0;
				break;
			case VK_LEFT:
				//glScalef(0.5,1.0,1.0);
				xscale = xscale * 0.5;
				break;
			case VK_UP:
				glScalef(1.0,2.0,1.0);
				break;
			case VK_DOWN:
				glScalef(1.0,0.5,1.0);
				break;
			default:
				break;
			}
		else
			switch (wParam)
			{
			case VK_RIGHT:
				//translatex += 10;
				glTranslatef(10.0,0.0,0.0);
				//anitranslate(10, 3.0,0.0);
				break;
			case VK_LEFT:
				//translatex -= 10;
				glTranslatef(-10.0,0.0,0.0);
				//anitranslate(10, -3.0,0.0);
				break;
			case VK_UP:
				//translatey += 10;
				glTranslatef(0.0,10.0,0.0);
				//anitranslate(10, 0.0,3.0);
				break;
			case VK_DOWN:
				//translatey -= 10;
				glTranslatef(0.0,-10.0,0.0);
				//anitranslate(10, 0.0,-3.0);
				break;
			default:
				break;
			}
		//animate(10);
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_LBUTTONDOWN:
		mousex = GET_X_LPARAM(lParam);
		mousey = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		switch (LOWORD(wParam))
		{
		case MK_LBUTTON:
			deltax = mousex - GET_X_LPARAM(lParam);
			mousex = GET_X_LPARAM(lParam);
			deltay = mousey - GET_Y_LPARAM(lParam);
			mousey = GET_Y_LPARAM(lParam);
			sourcexprev = sourcexprev + (deltax / xscaleprev);
			//glTranslatef(0.0, deltay / yscale, 0.0);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		default:
			mousex = GET_X_LPARAM(lParam);
			mousey = GET_Y_LPARAM(lParam);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		break;
	case WM_MOUSEWHEEL:
		mouse.x = GET_X_LPARAM(lParam);
		mouse.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &mouse);
		mousex = mouse.x;
		mousey = mouse.y;
		switch (LOWORD(wParam))
		{
		case MK_CONTROL | MK_SHIFT:
			break;
		case MK_CONTROL:
			if (GET_WHEEL_DELTA_WPARAM(wParam)>0)
				xscale = xscale * 1.1;
			else
				if (xscale / 1.1>(double)xwidth / l)
					xscale = xscale / 1.1;
				else
					xscale = (double)xwidth / l;
			break;
		case MK_SHIFT:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				yscale = yscale * 2;
				glScalef(1.0, 2.0, 1.0);
			}
			else
			{
				yscale = yscale * 0.5;
				glScalef(1.0, 0.5, 1.0);
			}
			break;
		default:
			if (GET_WHEEL_DELTA_WPARAM(wParam)>0)
				glTranslatef(0.0, -10.0, 0.0);
			else
				glTranslatef(0.0, 10.0, 0.0);
			break;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_PAINT:
		render(0);
		ValidateRect(hWnd, NULL);
		break;
	case WM_SIZE:
		xwidth = LOWORD(lParam);
		yheight = HIWORD(lParam);
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, (GLdouble)LOWORD(lParam), 0.0, (GLdouble)HIWORD(lParam));
		//glOrtho(0.0, (GLdouble)LOWORD(lParam), 0.0, (GLdouble)HIWORD(lParam), -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	HWND hwnd;
	WNDCLASS wc = {0,WndProc,0,0,hInstance,LoadIcon(hInstance, "Window"),LoadCursor(NULL, IDC_ARROW),(HBRUSH)(COLOR_WINDOW+1),"Menu","MainWindowClass"};
	RegisterClass(&wc);
	hwnd = CreateWindow("MainWindowClass","Window",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,GetPrivateProfileInt("window","width",300,configfile),GetPrivateProfileInt("window","height",300,configfile),NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,SW_SHOWDEFAULT);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}
