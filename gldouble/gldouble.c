#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

HDC hDC;
HGLRC hRC;
GLuint myList;
GLfloat xscale = 1.0;
GLfloat yscale = 1.0;

LRESULT CALLBACK WndProc(HWND hWnd,UINT	message,WPARAM	wParam,LPARAM	lParam)
{
	switch (message)
	{
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
			//glScalef(1.0,2.0,1.0);
			yscale = yscale * 2.0;
			break;
		case VK_DOWN:
			//glScalef(1.0,0.5,1.0);
			yscale = yscale * 0.5;
			break;
		default:
			break;
		}
		else
		switch (wParam)
		{
		case VK_RIGHT:
			glTranslatef(0.1,0.0,0.0);
			break;
		case VK_LEFT:
			glTranslatef(-0.1,0.0,0.0);
			break;
		case VK_UP:
			glTranslatef(0.0,0.1,0.0);
			break;
		case VK_DOWN:
			glTranslatef(0.0,-0.1,0.0);
			break;
		default:
			break;
		}
	break;	
	case WM_CREATE:
		hDC = GetDC(hWnd);		
		PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),1,PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,8,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,PFD_MAIN_PLANE,0,0,0,0 };
		SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		wglSwapIntervalEXT(1);
		myList = glGenLists(1);
		glNewList(myList, GL_COMPILE);
		glBegin(GL_LINE_LOOP);
			glVertex2f(-0.5,-0.5);
			glVertex2f(-0.5,0.5);
			glVertex2f(0.5,0.5);
			glVertex2f(0.5,-0.5);
		glEnd();
		glEndList();
		break;
	case WM_DESTROY:
		wglMakeCurrent(hDC,NULL);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		exit(0);
		break;
	case WM_SIZE:
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		//gluOrtho2D(-1.0,1.0,-1.0,1.0);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	GLfloat spin = 0.0;
	MSG Msg;
	WNDCLASS wc = {0,WndProc,0,0,hInstance,LoadIcon(hInstance, "Window"),LoadCursor(NULL, IDC_ARROW),NULL,"Menu","MainWindowClass"};
	RegisterClass(&wc);
	HWND hwnd = CreateWindow("MainWindowClass","Window",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,300,300,NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,SW_SHOWDEFAULT);
	while (1)
	{
		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glScalef(xscale,yscale,1.0);
		glRotatef(spin++,0.0,0.0,1.0);
		if (spin == 360.0) spin = 0.0;
		glCallList(myList);
		glPopMatrix();
		glFlush();
		SwapBuffers(hDC);
		ValidateRect(hwnd,NULL);
	}
	return 0;
}
