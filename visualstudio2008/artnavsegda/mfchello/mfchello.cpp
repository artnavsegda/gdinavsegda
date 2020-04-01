 	#include <afxwin.h>
 	#include <string.h>
 	#include "mfchello.h"

 	// Создание одного и только одного экземпляра
 	// приложения
 
 	CApp App;
 
 	// Реализация
 
 	BOOL CApp::InitInstance()
 	{
 
 	// Создание главного окна приложения и его
 	// отображение.
 
 	// Член CApp::m_pMainWnd - это указатель на объект
 	// главного окна.
 
 	m_pMainWnd = new CMainWin;
 	m_pMainWnd->ShowWindow(SW_RESTORE);
 	m_pMainWnd->UpdateWindow();

 	// Сигнализируем MFC об успешной инициализации
 	// приложения. 
 
 	return TRUE;
 	}
 
 	CMainWin::CMainWin()
 	{
 	// Создание окна с заголовком. Используется
 	// встроенный в MFC
 
 	// класс окна, поэтому первый параметр 0.
 	this->Create(0, L"Простейшее приложение на MFC");
 	}
 
 	// Реализация карты сообщений
 	BEGIN_MESSAGE_MAP(CMainWin /*класс окна*/, CFrameWnd
 	/*класс-предок*/)
 	END_MESSAGE_MAP()