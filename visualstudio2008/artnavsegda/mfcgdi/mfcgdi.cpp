#include <afxwin.h>
#include "mfcgdi.h"
// Реализация
BOOL CApp::InitInstance()
{
	m_pMainWnd = new CMainWin;
	m_pMainWnd->ShowWindow(SW_RESTORE);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}
CMainWin::CMainWin()
{
	// Создать основное окно
	this->Create(0, "Обработка сообщений");
	// Инициализировать переменные объекта
	str.clear();
	pszMouseStr.clear();
	nMouseX = nMouseY = nOldMouseX = nOldMouseY = 0;
}
// Реализация карты сообщений главного окна
BEGIN_MESSAGE_MAP(CMainWin /* класс */, 
				  CFrameWnd /* базовый класс */)
	ON_WM_CHAR() // сообщение о нажатии символа
	ON_WM_PAINT() // сообщение о необходимости перерисовать окно
	ON_WM_LBUTTONDOWN() // сообщение о нажатии ЛКМ
	ON_WM_RBUTTONDOWN() // сообщение о нажатии ПКМ
END_MESSAGE_MAP()
// Реализация функций отклика на сообщения

afx_msg void CMainWin::OnChar(UINT ch, UINT, UINT)
{	
	str = ch;
	// Посылаем сообщение WM_PAINT
	// с необходимостью стереть и обновить все окно
	this->InvalidateRect(0); // принудительная отправка сообщения ON_WM_PAINT()
}

afx_msg void CMainWin::OnPaint()
{
	// Создадим контекст устройства для обработки WM_PAINT
	CPaintDC dc(this);
	// Затираем текст и снова выводим (возможно уже другой текст)
	dc.TextOut(nOldMouseX, nOldMouseY, " ");
	dc.TextOut(nMouseX, nMouseY, pszMouseStr.data());
	dc.TextOut(1, 1, " ");
	dc.TextOut(1, 1, str.data());
}

afx_msg void CMainWin::OnLButtonDown(UINT, CPoint loc)
{
	// Запоминаем в переменных класса координаты
	// мыши и текст.

	// Затем посылаем сообщение WM_PAINT - его
	// обработчик выведет все на экран.

	nOldMouseX = nMouseX; nOldMouseY = nMouseY;	
	pszMouseStr = "Нажата левая кнопка";
	nMouseX = loc.x; nMouseY = loc.y;
	this->InvalidateRect(0); // принудительная отправка сообщения ON_WM_PAINT()
}

afx_msg void CMainWin::OnRButtonDown(UINT, CPoint loc)
{

	// Запоминаем в переменных класса координаты
	// мыши и текст.

	// Затем посылаем сообщение WM_PAINT - его
	// обработчик выведет все на экран.

	nOldMouseX = nMouseX; nOldMouseY = nMouseY;	
	pszMouseStr = "Нажата правая кнопка";
	nMouseX = loc.x; nMouseY = loc.y;
	this->InvalidateRect(0); // принудительная отправка сообщения ON_WM_PAINT()
}
CApp App; // Единственный экземпляр приложения