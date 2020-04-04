#include <afxwin.h>
#include <string>
// Класс основного окна
class CMainWin: public CFrameWnd {
public:
	CMainWin();
	// Функции обработки сообщений
	afx_msg void OnChar(UINT ch, UINT, UINT); // нажат символ
	afx_msg void OnPaint(); // перерисовка клиентской области
	afx_msg void OnLButtonDown(UINT flags, CPoint Loc); // нажата ЛКМ
	afx_msg void OnRButtonDown(UINT flags, CPoint Loc); // нажата ПКМ
	// Вспомогательные член-данные
	std::string str;
	std::string pszMouseStr;
	int nMouseX, nMouseY, nOldMouseX, nOldMouseY;	
	// Декларирование карты откликов на сообщения
	DECLARE_MESSAGE_MAP()
};
// Класс приложения
class CApp: public CWinApp {
public:
	BOOL InitInstance();
};