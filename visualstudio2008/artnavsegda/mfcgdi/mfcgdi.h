#include <afxwin.h>
#include <string>
// ����� ��������� ����
class CMainWin: public CFrameWnd {
public:
	CMainWin();
	// ������� ��������� ���������
	afx_msg void OnChar(UINT ch, UINT, UINT); // ����� ������
	afx_msg void OnPaint(); // ����������� ���������� �������
	afx_msg void OnLButtonDown(UINT flags, CPoint Loc); // ������ ���
	afx_msg void OnRButtonDown(UINT flags, CPoint Loc); // ������ ���
	// ��������������� ����-������
	std::string str;
	std::string pszMouseStr;
	int nMouseX, nMouseY, nOldMouseX, nOldMouseY;	
	// �������������� ����� �������� �� ���������
	DECLARE_MESSAGE_MAP()
};
// ����� ����������
class CApp: public CWinApp {
public:
	BOOL InitInstance();
};