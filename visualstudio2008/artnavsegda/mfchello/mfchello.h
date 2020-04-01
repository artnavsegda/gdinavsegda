 	#include <afxwin.h>
 	// Класс основного окна приложения
 	class CMainWin: public CFrameWnd {
 	public:
 	CMainWin();
 	// Декларирование карты сообщений
 	DECLARE_MESSAGE_MAP()
 	};

 	// Класс приложения. Должен существовать только
 	// один экземпляр этого класса.
 
 	// Член-функция InitInstance() вызывается при запуске
 	// приложения.
 
 	class CApp: public CWinApp {
 	public:
 	BOOL InitInstance();
 	};