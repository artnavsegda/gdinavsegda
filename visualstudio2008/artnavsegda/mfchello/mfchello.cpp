 	#include <afxwin.h>
 	#include <string.h>
 	#include "mfchello.h"

 	// �������� ������ � ������ ������ ����������
 	// ����������
 
 	CApp App;
 
 	// ����������
 
 	BOOL CApp::InitInstance()
 	{
 
 	// �������� �������� ���� ���������� � ���
 	// �����������.
 
 	// ���� CApp::m_pMainWnd - ��� ��������� �� ������
 	// �������� ����.
 
 	m_pMainWnd = new CMainWin;
 	m_pMainWnd->ShowWindow(SW_RESTORE);
 	m_pMainWnd->UpdateWindow();

 	// ������������� MFC �� �������� �������������
 	// ����������. 
 
 	return TRUE;
 	}
 
 	CMainWin::CMainWin()
 	{
 	// �������� ���� � ����������. ������������
 	// ���������� � MFC
 
 	// ����� ����, ������� ������ �������� 0.
 	this->Create(0, L"���������� ���������� �� MFC");
 	}
 
 	// ���������� ����� ���������
 	BEGIN_MESSAGE_MAP(CMainWin /*����� ����*/, CFrameWnd
 	/*�����-������*/)
 	END_MESSAGE_MAP()