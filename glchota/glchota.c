#include <stdio.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#pragma comment(lib, "opengl32.lib")
#include <GL/glu.h>
#pragma comment(lib, "glu32.lib")
#include "font.h"

float colors[][3] = { { 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 },{ 255,255,255 } };

#define VSYNC 1

#define LODWORD(ull) ((DWORD)((ULONGLONG)(ull) & 0x00000000ffffffff))

//typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
//PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT;

GLuint fontOffset;

char configfile[] = ".\\glchota.ini";

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int Value, LPCTSTR lpFileName)
{
	char Buffer[16];
	sprintf(Buffer, "%d", Value);
	return WritePrivateProfileString(lpAppName, lpKeyName, Buffer, lpFileName);
}

HMENU menu;
HDC hDC;
HGLRC hRC;

int displaymode = 1;

#define MAXLENGTH 9000000
int open = 0;
GLfloat xscale = 1.0;
GLfloat yscale = 1.0;
double xpos = 0.0;
float dscale = 1.0;

float mousexprev = 0;
float xscaleprev = 1.0;
float sourcexprev = 0;
int yoffset = 0;

POINT first, second;
DWORD dwarg;

short m[14][9001000];
char mtime[9001000][50];
int max[14];
int min[14];
int col[14];
int l;

float xspan = 0.0;
int xwidth = 100, yheight = 100;
//double xscale = 1.0;
POINT mouse;
int mousex = 0, mousey = 0;
int deltax = 0, deltay = 0;

int leveli = 0;
int level[20];

char globalmetricsfilename[255];

int bindecode(HWND hwnd, unsigned int yo)
{
	for (int i = 1; i <= 13; i++)
	{
		int state = 0;
		
		state = (yo >> i) & 1;

		if (state == 1)
			CheckMenuItem(GetMenu(hwnd), i + 10, MF_BYCOMMAND | MF_CHECKED);
	}

	return 0;
}

unsigned int binencode(HWND hwnd)
{
	unsigned int yo = 0;

	for (int i = 1; i <= 13; i++)
	{
		if (GetMenuState(GetMenu(hwnd), i + 10, MF_BYCOMMAND) & MF_CHECKED)
		yo = yo | (1 << i);
	}

	return yo;
}

int writemetrics(char filemetricsname[])
{
	char filemetricspath[255];
	sprintf(filemetricspath, ".\\%s.metrics", filemetricsname);
	FILE *filemetrics = fopen(filemetricspath, "w");
	fwrite(level, sizeof(level), 1, filemetrics);
	fwrite(colors, sizeof(colors), 1, filemetrics);
	fclose(filemetrics);//?
	return 0;
}

int makedefaultmetrics(void)
{
	FILE *defaultmetrics = fopen(".\\default.metrics", "w");
	memset(level, 0, sizeof(level));
	fwrite(level, sizeof(level), 1, defaultmetrics);
	fwrite(colors, sizeof(colors), 1, defaultmetrics);
	fclose(defaultmetrics);
	return 0;
}

int openmetrics(char basepath[])
{
	FILE *filemetrics;
	char metricsfilename[255];
	sprintf(metricsfilename, ".\\%s.metrics", basepath);
	filemetrics = fopen(metricsfilename,"r");
	if (filemetrics == NULL)
	{
		filemetrics = fopen(".\\default.metrics", "r");
		if (filemetrics == NULL)
		{
			makedefaultmetrics();
			return 0;
		}
	}
	fread(level, sizeof(level), 1, filemetrics);
	fread(colors, sizeof(colors), 1, filemetrics);
	fclose(filemetrics);
	return 0;
}

int developmetrics(char filename[])
{
	char drive[255];
	char dir[255];
	char ext[255];
	_splitpath(filename, drive, dir, globalmetricsfilename, ext);
	openmetrics(globalmetricsfilename);
	return 0;
}

GLuint lines[20];

int makelists()
{
	for (int iz = 1; iz <= 13; iz++)
	{
		lines[iz] = glGenLists(1);
		glNewList(lines[iz], GL_COMPILE);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i<l; i++)
			glVertex2i(i, m[iz][i]);
		glEnd();
		glEndList();
		GLenum glErr = glGetError();
		if (glErr != GL_NO_ERROR)
		{
			char error[10];
			snprintf(error, 10, "%d", glErr);
			MessageBox(NULL, error, "error code", MB_OK);
			if (glErr == (GLenum)1285)
				MessageBox(NULL, "out of memory", "opengl error", MB_OK);
		}
	}
	return 0;
}

int binary = 0;

int developmassive(char filename[])
{
	sourcexprev = 0;
	xscaleprev = 1.0;
	binary = 0;
	l = 0;
	FILE *sora;
//	sora = fopen("./chota.txt","r");
	sora = fopen(filename,"r");
	if (sora == NULL)
	{
		printf("fucking error");
		exit(0);
	}
	else
		open = 1;
	developmetrics(filename);
	//makedefaultmetrics();
	memset(max,0,sizeof(max));
	memset(max,0,sizeof(min));
	while (fscanf(sora,"%hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd %hd\n", &m[1][l],&m[2][l],&m[3][l],&m[4][l],&m[5][l],&m[6][l],&m[7][l],&m[8][l],&m[9][l],&m[10][l],&m[11][l],&m[12][l],&m[13][l])!=EOF)
	{
		for(int i=1;i<=13;i++)
		{
			if (min[i]==0) min[i]=m[i][l];
			if (max[i]==0) max[i]=m[i][l];
			if (m[i][l]<min[i]) min[i]=m[i][l];
			if (m[i][l]>max[i]) max[i]=m[i][l];
		}
		if (l++ > MAXLENGTH)
			break;
	}
	printf("length is %d\n",l);
	for(int i=1;i<=13;i++)
		printf("%d: min %d, max %d, span %d\n",i,min[i],max[i],max[i]-min[i]);
	fclose(sora);
	if (displaymode)
		makelists();
	xscale = (float)xwidth / (float)l;
	return 0;
}

int developbinary(char filename[])
{
	sourcexprev = 0;
	xscaleprev = 1.0;
	binary = 1;
	unsigned char buf[100];
	short sbuf[100];
	l = 0;
	FILE *sora;
	//	sora = fopen("./chota.txt","r");
	sora = fopen(filename, "rb");
	if (sora == NULL)
	{
		printf("fucking error");
		exit(0);
	}
	else
		open = 1;
	developmetrics(filename);
	//makedefaultmetrics();
	memset(max, 0, sizeof(max));
	memset(max, 0, sizeof(min));
	while (1)
	{
		fread(buf, 1, 1, sora);
		if (buf[0] == 0xf4)
		{
			//	printf("%hhx start\n",buf[0]);
		}
		else
			break;
		fread(buf, 6, 1, sora);
		if (buf[0] >= 0x30)
			buf[0] -= 0x10;
		if (buf[1] >= 0x30)
			buf[1] -= 0x10;
		{
			snprintf(mtime[l], 50, "%hhu%hhu:%hhu%hhu:%hhu%hhu ", buf[0] - 0x20, buf[1] - 0x20, buf[2] - 0x30, buf[3] - 0x30, buf[4] - 0x30, buf[5] - 0x30);
			//printf("%hhu%hhu:%hhu%hhu:%hhu%hhu ", buf[0] - 0x20, buf[1] - 0x20, buf[2] - 0x30, buf[3] - 0x30, buf[4] - 0x30, buf[5] - 0x30);
		}
		fread(buf, 1, 1, sora);
		if (buf[0] == 0xa5)
		{
			//	printf("%hhx marker\n",buf[0]);
		}
		else
			break;
		fread(sbuf, 20, 1, sora);
		fread(buf, 3, 1, sora);
		if (buf[2] == 0xb5)
		{
			//	printf("%hhx marker\n",buf[0]);
		}
		else
			break;
		//printf("%hhu %hu %hu %hu %hd %hu %hu %hu %hu %hu %hu %hu %hhu %hhu\n", (char)0xa5, sbuf[0], sbuf[1], sbuf[2], sbuf[3], sbuf[4], sbuf[5], sbuf[6], sbuf[7], sbuf[8], sbuf[9], sbuf[10], buf[0], buf[1]);
		m[1][l] = (unsigned char)0xa5;
		m[2][l] = (unsigned short)sbuf[0];
		m[3][l] = (unsigned short)sbuf[1];
		m[4][l] = (unsigned short)sbuf[2];
		m[5][l] = (signed short)sbuf[3];
		m[6][l] = (unsigned short)sbuf[4];
		m[7][l] = (unsigned short)sbuf[5];
		m[8][l] = (unsigned short)sbuf[6];
		m[9][l] = (unsigned short)sbuf[7];
		m[10][l] = (unsigned short)sbuf[8];
		m[11][l] = (unsigned short)sbuf[9];
		m[12][l] = (unsigned short)sbuf[10];
		m[13][l] = (unsigned char)buf[0];
		m[14][l] = (unsigned char)buf[1];

		fread(buf, 4, 1, sora);
		//printf("incoming data %hhx %hhx %hhx %hhx\n",   buf[0], buf[1], buf[2], buf[3]);
		fread(buf, 1, 1, sora);
		if (buf[0] == 0xf8)
		{
			//	printf("%hhx stop\n",buf[0]);
		}
		else
			break;
		for (int i = 1; i <= 13; i++)
		{
			if (min[i] == 0) min[i] = m[i][l];
			if (max[i] == 0) max[i] = m[i][l];
			if (m[i][l]<min[i]) min[i] = m[i][l];
			if (m[i][l]>max[i]) max[i] = m[i][l];
		}
		if (l++ > MAXLENGTH)
			break;
	}
	printf("length is %d\n", l);
	for (int i = 1; i <= 13; i++)
		printf("%d: min %d, max %d, span %d\n", i, min[i], max[i], max[i] - min[i]);
	fclose(sora);
	if (displaymode)
		makelists();
	xscale = (float)xwidth / (float)l;
	return 0;
}

BOOL CALLBACK DialogFunc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
	char workstring[100];
    switch (message) 
    {
		case WM_INITDIALOG:
			for (int i = 1; i <= 13; i++)
				CheckDlgButton(hwndDlg, 4 + i, col[i]);
			for (int i = 1; i <= 13; i++)
				SetDlgItemInt(hwndDlg, 24+i, level[i], TRUE);

		break;
        case WM_COMMAND: 
            switch (LOWORD(wParam)) 
            { 
                case 4: 
                    //if (!GetDlgItemText(hwndDlg, ID_ITEMNAME, szItemName, 80)) 
                    //     *szItemName=0; 
 				break;
                case 2:
					//SendDlgItemMessage(hwndDlg,3,EM_GETLINE,(WPARAM)0,(LPARAM)startsectorstring);
					//startsector = atoi(startsectorstring);
					//sscanf(startsectorstring,"%d",&startsector);
					//slip = GetDlgItemInt(hwndDlg,3,NULL,FALSE);
					for (int i = 1; i <= 13; i++)
						min[i] = GetDlgItemInt(hwndDlg, 24+i, NULL, FALSE);
                    EndDialog(hwndDlg, wParam);
                    return TRUE; 
				break;
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
				case 17:
					col[LOWORD(wParam) - 4] = IsDlgButtonChecked(hwndDlg, LOWORD(wParam));
					break;
            }
		break;
    } 
    return FALSE; 
} 



//LRESULT DecodeGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	POINT ptZoomCenter;
//	double k;
//	// Create a structure to populate and retrieve the extra message info.
//	GESTUREINFO gi;
//
//	ZeroMemory(&gi, sizeof(GESTUREINFO));
//
//	gi.cbSize = sizeof(GESTUREINFO);
//
//	BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);
//	BOOL bHandled = FALSE;
//
//	if (bResult)
//	{
//		// now interpret the gesture
//		switch (gi.dwID)
//		{
//		case GID_ZOOM:
//			// Code for zooming goes here     
//			switch (gi.dwFlags)
//			{
//			case GF_BEGIN:
//				dwarg = LODWORD(gi.ullArguments);
//				first.x = gi.ptsLocation.x;
//				first.y = gi.ptsLocation.y;
//				ScreenToClient(hWnd, &first);
//				mousex = first.x;
//				break;
//			default:
//				second.x = gi.ptsLocation.x;
//				second.y = gi.ptsLocation.y;
//				ScreenToClient(hWnd, &second);
//				ptZoomCenter.x = (first.x + second.x) / 2;
//				ptZoomCenter.y = (first.y + second.y) / 2;
//				mousex = ptZoomCenter.x;
//				k = (double)(LODWORD(gi.ullArguments)) / (double)(dwarg);
//				//glScalef(k, k, 1.0);
//				xscale = xscale * k;
//				//yscale = yscale * k;
//				InvalidateRect(hWnd, NULL, TRUE);
//				first = second;
//				dwarg = LODWORD(gi.ullArguments);
//				break;
//			}
//			bHandled = TRUE;
//			break;
//		case GID_PAN:
//			// Code for panning goes here
//			switch (gi.dwFlags)
//			{
//			case GF_BEGIN:
//				first.x = gi.ptsLocation.x;
//				first.y = gi.ptsLocation.y;
//				ScreenToClient(hWnd, &first);
//				mousex = first.x;
//				break;
//			default:
//				second.x = gi.ptsLocation.x;
//				second.y = gi.ptsLocation.y;
//				ScreenToClient(hWnd, &second);
//				//glTranslatef(second.x - first.x, first.y - second.y, 0.0);
//
//				mousex = first.x;
//				deltax = first.x - second.x;
//				deltay = first.y - second.y;
//				sourcexprev = sourcexprev + (deltax / xscaleprev);
//				yoffset = yoffset + deltay;
//
//				InvalidateRect(hWnd, NULL, TRUE);
//				first = second;
//				break;
//			}
//			bHandled = TRUE;
//			break;
//		case GID_ROTATE:
//			// Code for rotation goes here
//			bHandled = TRUE;
//			break;
//		case GID_TWOFINGERTAP:
//			// Code for two-finger tap goes here
//			MessageBoxW(hWnd, L"Weehee!", L"two finger tap recieved", MB_OK);
//			bHandled = TRUE;
//			break;
//		case GID_PRESSANDTAP:
//			// Code for roll over goes here
//			bHandled = TRUE;
//			break;
//		default:
//			// A gesture was not recognized
//			break;
//		}
//	}
//	else
//	{
//		DWORD dwErr = GetLastError();
//		if (dwErr > 0)
//		{
//			//MessageBoxW(hWnd, L"Error!", L"Could not retrieve a GESTUREINFO structure.", MB_OK);
//		}
//	}
//	if (bHandled)
//	{
//		return 0;
//	}
//	else
//	{
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//}

float sourcetodest(float source)
{
	if (sourcexprev == 0)
		return l / ((float)xwidth / (float)mousex);
	else
		return sourcexprev - ((mousexprev - mousex)/(float)xscaleprev);
}

int openrecent(HWND hwnd)
{
	char filename[260];
	//WritePrivateProfileString("window", "filename", ofn.lpstrFile, configfile);
	GetPrivateProfileString("window", "filename", "default", filename, 260, configfile);
	//GetPrivateProfileInt("window", "width", 300, configfile);
	if (strcmp(filename, "default") == 0)
		SendMessage(hwnd, WM_COMMAND, 3, 0);
	else if (_stricmp(".txt", PathFindExtension(filename)) == 0)
		developmassive(filename);
	else if (_stricmp(".bin", PathFindExtension(filename)) == 0)
		developbinary(filename);
	return 0;
}

static DWORD rgbCurrent = 0x00ffff00;        // initial color selection

int render(HWND hwnd)
{
	glClear(GL_COLOR_BUFFER_BIT);
	if (open == 1)
	{
		//double scaleX = (double)xwidth / (double)l;
		//double scaleY = 300 / yheight;
		//float destx = xwidth / 2;
		double destx = mousex;
		double desty = yheight / 2;
		//float sourcex = l / ((float)xwidth / (float)mousex);
		double sourcex = sourcetodest(destx);

		//double sourcey = 150;
		double sourcey = 0;

		mousexprev = mousex;
		xscaleprev = xscale;
		sourcexprev = sourcex;

		glColor3f(1.0, 1.0, 1.0);
		glPushMatrix();
		glTranslatef(destx, desty + yoffset, 0.0);
		glScalef(xscale, yscale, 1.0);
		glTranslatef(sourcex * -1.0, sourcey * -1.0, 0.0);

		glBegin(GL_LINES);
		glVertex2i(0, 0);
		glVertex2i(l, 0);
		glEnd();


		for (int iz = 1; iz <= 13; iz++)
		{
			if (GetMenuState(GetMenu(hwnd), iz + 10, MF_BYCOMMAND) & MF_CHECKED)
				//if (iz == 13)
			{
				glPushMatrix();
				glTranslatef(0.0, level[iz], 0.0);
				if (iz == leveli)
					glColor3ub(255, 255, 0);
				else
					glColor3ub(colors[iz][1], colors[iz][2], colors[iz][3]);
				if (displaymode)
					glCallList(lines[iz]);
				else
				{
					glBegin(GL_LINE_STRIP);
					for (int i = 0; i < l; i++)
						glVertex2i(i, m[iz][i]);
					glEnd();
				}
				glPopMatrix();
			}
		}

		glPopMatrix();
		glColor3ub(255, 255, 255);

		char string[100];

		glRasterPos2i(10, 10);
		glPushAttrib(GL_LIST_BIT);
		glListBase(fontOffset);
		snprintf(string, 10, "%9d", (int)sourcex);
		glCallLists(10, GL_UNSIGNED_BYTE, string);


		for (int iz = 1; iz <= 13; iz++)
		{
			if (iz == leveli)
			{
				//glColor3f(1.0, 1.0, 0.0);
				glColor3ub(255, 255, 0);
			}
			else
			{
				//glColor3f(1.0, 1.0, 1.0);
				glColor3ub(colors[iz][1], colors[iz][2], colors[iz][3]);
			}
			glRasterPos2i(iz*55, 30);
			snprintf(string, 6, "%5d", m[iz][(int)sourcex]);
			glCallLists(5, GL_UNSIGNED_BYTE, string);
		}
		glColor3ub(255, 255, 255);

//		glRasterPos2i(10, 30);
		//165 608 29 882 27 34 273 1545 9984 34 34 8 197
//		snprintf(string, 77, "%5u %5u %5u %5u %5d %5u %5u %5u %5u %5u %5u %5u %5u", m[1][(int)sourcex], m[2][(int)sourcex], m[3][(int)sourcex], m[4][(int)sourcex], m[5][(int)sourcex], m[6][(int)sourcex], m[7][(int)sourcex], m[8][(int)sourcex], m[9][(int)sourcex], m[10][(int)sourcex], m[11][(int)sourcex], m[12][(int)sourcex], m[13][(int)sourcex]);
		//snprintf(string, 10, "%9d", m[leveli][(int)sourcex]);
//		glCallLists(77, GL_UNSIGNED_BYTE, string);
		if (binary == 1)
		{
			if (sourcex > 0)
			{
				glRasterPos2i(30, 50);
				glCallLists(50, GL_UNSIGNED_BYTE, mtime[(int)sourcex]);
			}
		}
		glPopAttrib();
	}
	glBegin(GL_LINES);
	glVertex2i(mousex, 0);
	glVertex2i(mousex, yheight);
	glEnd();
	glFlush();
	SwapBuffers(hDC);
	return 0;
}

int animate(HWND hwnd, int count, int xdelta, int ydelta, double zoom)
{
	while (count)
	{
		sourcexprev = sourcexprev + (xdelta / xscaleprev);
		xscale = xscale * zoom;
		//glTranslatef(0.0, , 0.0);
		yoffset = yoffset + ydelta;
		render(hwnd);
		count--;
	}
	return 0;
}

int movable = 0;

int developcache(char filename[])
{
	char message[100];
	int error;
	FILE *sora;
	//	sora = fopen("./chota.txt","r");
	sora = fopen(filename, "rb");
	if (sora == NULL)
	{
		MessageBox(NULL, "cannot open", "message", MB_OK);
		exit(0);
	}
	else
		open = 1;
	developmetrics(filename);
	//makedefaultmetrics();
	memset(max, 0, sizeof(max));
	memset(max, 0, sizeof(min));

	fread(&l, sizeof(int), 1, sora);
	for (int i = 1; i <= 13; i++)
	{
		fread(&m[i], sizeof(short) * l, 1, sora);
	}
	fclose(sora);

	if (displaymode)
		makelists();
	xscale = (float)xwidth / (float)l;

	return 0;
}

int integratecache(char filename[])
{
	FILE *sora;
	//	sora = fopen("./chota.txt","r");
	sora = fopen(filename, "wb");
	if (sora == NULL)
	{
		printf("fucking error");
		exit(0);
	}

	fwrite(&l, sizeof(int), 1, sora);
	for (int i = 1; i <= 13; i++)
	{
		fwrite(&m[i], sizeof(short) * l, 1, sora);
	}
	fclose(sora);

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static OPENFILENAME ofn;
	static char szFile[260];
	char text[100];
	switch(msg)
	{
	case WM_RBUTTONDOWN:
		//mousex = GET_X_LPARAM(lParam);
		//mousey = GET_Y_LPARAM(lParam);
		//xscale = xscale * 2.0;
		if (xscale / 1.1 > (double)xwidth / l)
		{
			//xscale = xscale / 1.1;
			animate(hwnd, 5, 0.0, 0.0, 0.9);
		}
		else
		{
			xscale = (double)xwidth / l;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		//TrackPopupMenu(menu, TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hwnd, NULL);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 0:
			DestroyWindow(hwnd);
			break;
		case 1:
			DialogBox(GetModuleHandle(NULL), "MainDialog", hwnd, (DLGPROC)DialogFunc);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		case 2:
			WritePrivateProfileInt("window", "height", yheight, configfile);
			WritePrivateProfileInt("window", "width", xwidth, configfile);
			if (open == 1)
			{
				writemetrics(globalmetricsfilename);
			}
			break;
		case 3:
		{
			sourcexprev = 0;
			xscaleprev = 1.0;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Record\0*.TXT;*.BIN;*.cache\0Text\0*.TXT\0Binary\0*.BIN\0Cache\0*.cache\0All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetOpenFileName(&ofn) == TRUE)
			{
				WritePrivateProfileString("window", "filename", ofn.lpstrFile, configfile);
				if (_stricmp(".txt", PathFindExtension(ofn.lpstrFile)) == 0)
				{
					developmassive(ofn.lpstrFile);
					SetWindowText(hwnd, ofn.lpstrFile);
				}
				else if (_stricmp(".bin", PathFindExtension(ofn.lpstrFile)) == 0)
				{
					developbinary(ofn.lpstrFile);
					SetWindowText(hwnd, ofn.lpstrFile);
				}
				else if (_stricmp(".cache", PathFindExtension(ofn.lpstrFile)) == 0)
				{
					developcache(ofn.lpstrFile);
					SetWindowText(hwnd, ofn.lpstrFile);
				}
				else
				{
					MessageBox(hwnd, "Not a text file", "message", MB_OK);
				}
				//xscale = (float)xwidth / (float)l;
				InvalidateRect(hwnd, NULL, TRUE);
			}
			//else
			//	DestroyWindow(hwnd);
		}
		break;
		case 4:
			;
			CHOOSECOLOR cc;                 // common dialog box structure 
			static COLORREF acrCustClr[16]; // array of custom colors 
			HBRUSH hbrush;                  // brush handle

			ZeroMemory(&cc, sizeof(cc));
			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hwnd;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE)
			{
				hbrush = CreateSolidBrush(cc.rgbResult);
				rgbCurrent = cc.rgbResult;
			}
			colors[leveli][1] = GetRValue(rgbCurrent);
			colors[leveli][2] = GetGValue(rgbCurrent);
			colors[leveli][3] = GetBValue(rgbCurrent);
			break;
		case 5:
			;
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hwnd;
			ofn.lpstrFile = szFile;
			//ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "Text\0*.TXT\0All\0*.*\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			if (GetSaveFileName(&ofn) == TRUE)
			{
				integratecache(ofn.lpstrFile);
			}
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
		case 21:
		case 22:
		case 23:
			if (GetMenuState(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND) & MF_CHECKED)
				CheckMenuItem(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND | MF_UNCHECKED);
			else
				CheckMenuItem(GetMenu(hwnd), LOWORD(wParam), MF_BYCOMMAND | MF_CHECKED);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		break;
	case WM_CREATE:
		DragAcceptFiles(hwnd, TRUE);
		hDC = GetDC(hwnd);
		PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),1,PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,8,0,0,0,0,0,0,0,0,0,0,0,0,0,16,0,0,PFD_MAIN_PLANE,0,0,0,0 };
		SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
//		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
//		wglSwapIntervalEXT(VSYNC);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		fontOffset = glGenLists(128);
		for (GLuint i = 32; i < 127; i++)
		{
			glNewList(i + fontOffset, GL_COMPILE);
			glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, font[i - 32]);
			glEndList();
		}
		glListBase(fontOffset);
		//openrecent(hwnd);
		bindecode(hwnd, GetPrivateProfileInt("window", "state", 0, configfile));
		displaymode = GetPrivateProfileInt("window", "mode", 1, configfile);

		/*char filename[260];
		if (GetPrivateProfileInt("window", "openrecent", 1, configfile))
		{
			GetPrivateProfileString("window", "filename", "default", filename, 260, configfile);
			//GetPrivateProfileInt("window", "width", 300, configfile);
			if (strcmp(filename, "default") == 0)
				SendMessage(hwnd, WM_COMMAND, 3, 0);
			else if (_stricmp(".txt", PathFindExtension(filename)) == 0)
			{
				developmassive(filename);
				SetWindowText(hwnd, filename);
			}
			else if (_stricmp(".bin", PathFindExtension(filename)) == 0)
			{
				developbinary(filename);
				SetWindowText(hwnd, filename);
			}
			else
				SendMessage(hwnd, WM_COMMAND, 3, 0);
		}
		else
			SendMessage(hwnd, WM_COMMAND, 3, 0);*/
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		WritePrivateProfileInt("window", "height", yheight, configfile);
		WritePrivateProfileInt("window", "width", xwidth, configfile);
		WritePrivateProfileInt("window", "state", binencode(hwnd), configfile);
		WritePrivateProfileInt("window", "mode", displaymode, configfile);
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
		if (open == 1)
		{
			writemetrics(globalmetricsfilename);
		}
		PostQuitMessage(0);
		break;
	case WM_DROPFILES:
		sourcexprev = 0;
		xscaleprev = 1.0;
		TCHAR lpszFile[MAX_PATH] = { 0 };
		UINT uFile = 0;
		HDROP hDrop = (HDROP)wParam;

		uFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);
		if (uFile != 1) {
			MessageBox(hwnd, "Dropping multiple files is not supported.", NULL, MB_ICONERROR);
			DragFinish(hDrop);
			break;
		}
		lpszFile[0] = '\0';
		if (DragQueryFile(hDrop, 0, lpszFile, MAX_PATH))
		{
			WritePrivateProfileString("window", "filename", lpszFile, configfile);
			if (_stricmp(".txt", PathFindExtension(lpszFile)) == 0)
			{
				developmassive(lpszFile);
				SetWindowText(hwnd, lpszFile);
			}
			else if (_stricmp(".bin", PathFindExtension(lpszFile)) == 0)
			{
				developbinary(lpszFile);
				SetWindowText(hwnd, lpszFile);
			}
			else
			{
				MessageBox(hwnd, "Not a log file", lpszFile, MB_OK);
			}
		}

		DragFinish(hDrop);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	//case WM_GESTURENOTIFY:
	//	GESTURECONFIG gc = { 0, GC_ALLGESTURES, 0 };
	//	SetGestureConfig(hwnd, 0, 1, &gc, sizeof(GESTURECONFIG));
	//	return DefWindowProc(hwnd, WM_GESTURENOTIFY, wParam, lParam);
	//	break;
	//case WM_GESTURE:
	//	return DecodeGesture(hwnd, msg, wParam, lParam);
	//	break;
	case WM_KEYDOWN:
		if (GetKeyState(VK_CONTROL)<0)
			switch (wParam)
			{
			case VK_UP:
				xscale = xscale * 2.0;
				break;
			case VK_DOWN:
				if (xscale * 0.5>(double)xwidth / l)
					xscale = xscale * 0.5;
				else
					xscale = (double)xwidth / l;
				break;
			}
		else
			switch (wParam)
			{
			case VK_RIGHT:
				animate(hwnd, 5, 5.0, 0.0, 1.0);
				break;
			case VK_LEFT:
				animate(hwnd, 5, -5.0, 0.0, 1.0);
				break;
			case VK_UP:
				//glTranslatef(0.0, 10.0, 0.0);
				animate(hwnd,10, 0.0, 3.0, 1.0);
				break;
			case VK_DOWN:
				//glTranslatef(0.0, -10.0, 0.0);
				animate(hwnd,10, 0.0, -3.0, 1.0);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				leveli = wParam - 0x30;
				break;
			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
				leveli = wParam - 0x41 + 10;
				break;
			default:
				break;
			}
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_LBUTTONDOWN:
		movable = 1;
		mousex = GET_X_LPARAM(lParam);
		mousey = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONUP:
		movable = 0;
		mousex = GET_X_LPARAM(lParam);
		mousey = GET_Y_LPARAM(lParam);
		break;
	case WM_LBUTTONDBLCLK:
		mousex = GET_X_LPARAM(lParam);
		mousey = GET_Y_LPARAM(lParam);
		//xscale = xscale * 2.0;
		animate(hwnd, 5, 0.0, 0.0, 1.1);
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_MOUSEMOVE:
		switch (LOWORD(wParam))
		{
		case MK_LBUTTON:
			deltax = mousex - GET_X_LPARAM(lParam);
			mousex = GET_X_LPARAM(lParam);
			deltay = mousey - GET_Y_LPARAM(lParam);
			mousey = GET_Y_LPARAM(lParam);
			if (movable == 1)
			{
				sourcexprev = sourcexprev + (deltax / xscaleprev);
				yoffset = yoffset + deltay;
			}
			//glTranslatef(0.0, deltay / yscale, 0.0);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		case MK_LBUTTON | MK_SHIFT:
			//deltax = mousex - GET_X_LPARAM(lParam);
			//mousex = GET_X_LPARAM(lParam);
			deltay = mousey - GET_Y_LPARAM(lParam);
			mousey = GET_Y_LPARAM(lParam);
			level[leveli] = level[leveli] + (deltay / yscale);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		default:
			mousex = GET_X_LPARAM(lParam);
			mousey = GET_Y_LPARAM(lParam);
			InvalidateRect(hwnd, NULL, TRUE);
			break;
		}
		break;
	case WM_MOUSEHWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			//sourcexprev = sourcexprev + (10 / xscaleprev);
			animate(hwnd, 5, 5.0, 0.0, 1.0);
		}
		else
		{
			//sourcexprev = sourcexprev - (10 / xscaleprev);
			animate(hwnd, 5, -5.0, 0.0, 1.0);
		}
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_MOUSEWHEEL:
		mouse.x = GET_X_LPARAM(lParam);
		mouse.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hwnd, &mouse);
		mousex = mouse.x;
		mousey = mouse.y;
		switch (LOWORD(wParam))
		{
		case MK_CONTROL | MK_SHIFT:
			break;
		case MK_CONTROL:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				animate(hwnd, 5, 0.0, 0.0, 1.05);
				//xscale = xscale * 1.1;
			}
			else
				if (xscale / 1.1 > (double)xwidth / l)
				{
					//xscale = xscale / 1.1;
					animate(hwnd, 5, 0.0, 0.0, 0.95);
				}
				else
				{
					xscale = (double)xwidth / l;
				}
			break;
		case MK_SHIFT:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				yscale = yscale * 2;
				//glScalef(1.0, 2.0, 1.0);
			}
			else
			{
				yscale = yscale * 0.5;
				//glScalef(1.0, 0.5, 1.0);
			}
			break;
		default:
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				animate(hwnd, 5, 0.0, -5.0, 1.0);
				//glTranslatef(0.0, -10.0, 0.0);
			}
			else
			{
				animate(hwnd, 5, 0.0, 5.0, 1.0);
				//glTranslatef(0.0, 10.0, 0.0);
			}
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	case WM_PAINT:
			render(hwnd);
			ValidateRect(hwnd,NULL);
		break;
	case WM_SIZE:
		xwidth = LOWORD(lParam);
		yheight = HIWORD(lParam);
		glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
		glLoadIdentity();
		gluOrtho2D(0.0, (GLdouble)LOWORD(lParam), 0.0, (GLdouble)HIWORD(lParam));
		mousex = 0;
		sourcexprev = 0;
		xscale = (float)xwidth / (float)l;
		InvalidateRect(hwnd, NULL, TRUE);
		break;
	default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	MSG Msg;
	HWND hwnd;
	WNDCLASS wc = {CS_DBLCLKS,WindowProc,0,0,hInstance,LoadIcon(hInstance, "Window"),LoadCursor(NULL, IDC_ARROW),CreateSolidBrush(RGB(0,0,0)),"Menu","MainWindowClass"};
	RegisterClass(&wc);
	menu = GetSubMenu(LoadMenu(hInstance,"Menu"),0);
	xwidth = GetPrivateProfileInt("window", "width", 300, configfile);
	yheight = GetPrivateProfileInt("window", "height", 300, configfile);
	hwnd = CreateWindow("MainWindowClass","Window",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,xwidth,yheight,NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,SW_SHOWDEFAULT);
	PathUnquoteSpaces(lpCmdLine);
	if (_stricmp(".txt", PathFindExtension(lpCmdLine)) == 0)
	{
		developmassive(lpCmdLine);
		InvalidateRect(hwnd, NULL, TRUE);
		SetWindowText(hwnd, lpCmdLine);
	}
	else if (_stricmp(".bin", PathFindExtension(lpCmdLine)) == 0)
	{
		developbinary(lpCmdLine);
		InvalidateRect(hwnd, NULL, TRUE);
		SetWindowText(hwnd, lpCmdLine);
	}
	else if (GetPrivateProfileInt("window", "openrecent", 1, configfile))
	{
		char filename[260];
		GetPrivateProfileString("window", "filename", "default", filename, 260, configfile);
		if (strcmp(filename, "default") == 0)
			SendMessage(hwnd, WM_COMMAND, 3, 0);
		else if (_stricmp(".txt", PathFindExtension(filename)) == 0)
		{
			developmassive(filename);
			SetWindowText(hwnd, filename);
		}
		else if (_stricmp(".bin", PathFindExtension(filename)) == 0)
		{
			developbinary(filename);
			SetWindowText(hwnd, filename);
		}
		else
			SendMessage(hwnd, WM_COMMAND, 3, 0);
	}
	else
		SendMessage(hwnd, WM_COMMAND, 3, 0);
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return 0;
}
