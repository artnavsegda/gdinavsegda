#include <afxwin.h>
#include "mfcgdi.h"
// ����������
BOOL CApp::InitInstance()
{
	m_pMainWnd = new CMainWin;
	m_pMainWnd->ShowWindow(SW_RESTORE);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}
CMainWin::CMainWin()
{
	// ������� �������� ����
	this->Create(0, "��������� ���������");
	// ���������������� ���������� �������
	str.clear();
	pszMouseStr.clear();
	nMouseX = nMouseY = nOldMouseX = nOldMouseY = 0;
}
// ���������� ����� ��������� �������� ����
BEGIN_MESSAGE_MAP(CMainWin /* ����� */, 
				  CFrameWnd /* ������� ����� */)
	ON_WM_CHAR() // ��������� � ������� �������
	ON_WM_PAINT() // ��������� � ������������� ������������ ����
	ON_WM_LBUTTONDOWN() // ��������� � ������� ���
	ON_WM_RBUTTONDOWN() // ��������� � ������� ���
END_MESSAGE_MAP()
// ���������� ������� ������� �� ���������

afx_msg void CMainWin::OnChar(UINT ch, UINT, UINT)
{	
	str = ch;
	// �������� ��������� WM_PAINT
	// � �������������� ������� � �������� ��� ����
	this->InvalidateRect(0); // �������������� �������� ��������� ON_WM_PAINT()
}

afx_msg void CMainWin::OnPaint()
{
	// �������� �������� ���������� ��� ��������� WM_PAINT
	CPaintDC dc(this);
	// �������� ����� � ����� ������� (�������� ��� ������ �����)
	dc.TextOut(nOldMouseX, nOldMouseY, " ");
	dc.TextOut(nMouseX, nMouseY, pszMouseStr.data());
	dc.TextOut(1, 1, " ");
	dc.TextOut(1, 1, str.data());
}

afx_msg void CMainWin::OnLButtonDown(UINT, CPoint loc)
{
	// ���������� � ���������� ������ ����������
	// ���� � �����.

	// ����� �������� ��������� WM_PAINT - ���
	// ���������� ������� ��� �� �����.

	nOldMouseX = nMouseX; nOldMouseY = nMouseY;	
	pszMouseStr = "������ ����� ������";
	nMouseX = loc.x; nMouseY = loc.y;
	this->InvalidateRect(0); // �������������� �������� ��������� ON_WM_PAINT()
}

afx_msg void CMainWin::OnRButtonDown(UINT, CPoint loc)
{

	// ���������� � ���������� ������ ����������
	// ���� � �����.

	// ����� �������� ��������� WM_PAINT - ���
	// ���������� ������� ��� �� �����.

	nOldMouseX = nMouseX; nOldMouseY = nMouseY;	
	pszMouseStr = "������ ������ ������";
	nMouseX = loc.x; nMouseY = loc.y;
	this->InvalidateRect(0); // �������������� �������� ��������� ON_WM_PAINT()
}
CApp App; // ������������ ��������� ����������