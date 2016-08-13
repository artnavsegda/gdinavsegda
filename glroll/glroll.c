#define _WINSOCKAPI_

#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <modbus.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "modbus.lib")

#define VSYNC 1

typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

HDC hDC;
HGLRC hRC;
GLuint myList;
GLfloat xscale = 1.0;
GLfloat yscale = 1.0;

char configfile[] = ".\\glroll.ini";

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	sprintf(Buffer, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}

int xwidth, yheight;

LRESULT CALLBACK WndProc(HWND hWnd,UINT	message,WPARAM	wParam,LPARAM	lParam)
{
	switch (message)
	{
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
		wglSwapIntervalEXT(VSYNC);
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
		xwidth = LOWORD(lParam);
		yheight = HIWORD(lParam);
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//gluOrtho2D(-1.0,1.0,-1.0,1.0);
		gluOrtho2D(0.0, (GLdouble)LOWORD(lParam), (GLdouble)HIWORD(lParam) / -2, (GLdouble)HIWORD(lParam) / 2);
		glMatrixMode(GL_MODELVIEW);
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	uint16_t tab_reg[256];
	int rc;
	int rollpos = 0;
	int roll[3000];
	int roll2[3000];
	int roll3[3000];
	int roll4[3000];
	int roll5[3000];
	int roll6[3000];
	memset(roll, 0, 3000 * sizeof(int));
	memset(roll2, 0, 3000 * sizeof(int));
	memset(roll3, 0, 3000 * sizeof(int));
	memset(roll4, 0, 3000 * sizeof(int));
	memset(roll5, 0, 3000 * sizeof(int));
	memset(roll6, 0, 3000 * sizeof(int));
	int incomdata = 0;
	int incomdata2 = 0;
	int incomdata3 = 0;
	int incomdata4 = 0;
	int incomdata5 = 0;
	int incomdata6 = 0;
	/*char incomstring[100];
	char incomstring2[100];
	char incomstring3[100];
	char incomstring4[100];
	char incomstring5[100];
	char incomstring6[100];*/
	char filename[260];
	GetPrivateProfileString("window", "ipaddress", "192.168.1.120", filename, 260, configfile);
	WritePrivateProfileString("window", "ipaddress", filename, configfile);
	modbus_t *mb = modbus_new_tcp(filename, 502);
	MSG Msg;
	WNDCLASS wc = {0,WndProc,0,0,hInstance,LoadIcon(hInstance, "Window"),LoadCursor(NULL, IDC_ARROW),NULL,"Menu","MainWindowClass"};
	RegisterClass(&wc);
	HWND hwnd = CreateWindow("MainWindowClass","Window",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,700,700,NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,SW_SHOWDEFAULT);
	while (1)
	{
		while(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) > 0)
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		while (modbus_connect(mb) == -1)
		{
			fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
			//modbus_free(mb);
			//return -1;
		}
		rc = modbus_read_registers(mb, 0, 10, tab_reg);
		if (rc == -1) {
			fprintf(stderr, "%s\n", modbus_strerror(errno));
			return -1;
		}
		modbus_close(mb);
		incomdata = tab_reg[0];
		incomdata2 = tab_reg[1];
		incomdata3 = tab_reg[2];
		incomdata4 = tab_reg[3];
		incomdata5 = tab_reg[4];
		incomdata6 = tab_reg[5];
		/*/sprintf(incomstring, "%d", incomdata);
		sprintf(incomstring2, "%d", incomdata2);
		sprintf(incomstring3, "%d", incomdata3);
		sprintf(incomstring4, "%d", incomdata4);
		sprintf(incomstring5, "%d", incomdata5);
		sprintf(incomstring6, "%d", incomdata6);
		printf("%d %d %d %d %d %d\n", incomdata, incomdata2, incomdata3, incomdata4, incomdata5, incomdata6);*/
		roll[rollpos] = incomdata;
		roll2[rollpos] = incomdata2;
		roll3[rollpos] = incomdata3;
		roll4[rollpos] = incomdata4;
		roll5[rollpos] = incomdata5;
		roll6[rollpos] = incomdata6;
		rollpos++;
		if (rollpos == xwidth)
			rollpos = 0;

		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		//glScalef(xscale,yscale,1.0);
		//glRotatef(spin++,0.0,0.0,1.0);
		//if (spin == 360.0) spin = 0.0;
		//glCallList(myList);
		/*glRasterPos2f(50.0, 50.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring);
		glRasterPos2f(50.0, 70.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring2);
		glRasterPos2f(50.0, 90.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring3);
		glRasterPos2f(50.0, 110.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring4);
		glRasterPos2f(50.0, 130.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring5);
		glRasterPos2f(50.0, 150.0);
		glCallLists(40, GL_UNSIGNED_BYTE, (GLubyte *)incomstring6);*/
		int i;
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll[rollpos - i + xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll2[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll2[rollpos - i + xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll3[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll3[rollpos - i + xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll4[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll4[rollpos - i + xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll5[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll5[rollpos - i + xwidth]);
		glEnd();
		glBegin(GL_LINE_STRIP);
		for (i = 0; rollpos - i>0; i++)
			glVertex2f(i, roll6[rollpos - i]);
		for (; i<xwidth; i++)
			glVertex2f(i, roll6[rollpos - i + xwidth]);
		glEnd();
		glPopMatrix();
		glFlush();
		SwapBuffers(hDC);
		ValidateRect(hwnd,NULL);
		Sleep(200);
	}
	modbus_free(mb);
	return 0;
}
