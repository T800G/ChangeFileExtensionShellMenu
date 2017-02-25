#include "stdafx.h"
#include "NewextDlg.h"
#include "aboutdlg.h"


CNewExtDlg::CNewExtDlg(LPCTSTR szNewExt)
{
	if (szNewExt) m_newext=szNewExt;
}

LRESULT CNewExtDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	//filtered edit control
	m_extEdit.SubclassWindow(GetDlgItem(IDC_EDITCTRL));
	m_extEdit.SetExcludeMask(_T("\\/:*?\"<>| "));//space char also!
	m_extEdit.LimitText(_MAX_EXT); //file extension can't be longer than this

	//command link
	CLLINK_CREATEINFO ci;
	SecureZeroMemory( &ci, sizeof(CLLINK_CREATEINFO));
	RECT rct={14,10,81,24};
	ci.rcPos=&rct;
	ci.bmpID=IDB_LINKBITMAP;
	ci.nID=IDC_HELPLINK;
	ci.hWndParent=m_hWnd;
	ci.szWindowName=_T("New extension:");
	m_helpLink.CreateCmdLink(&ci);

	DoDataExchange(DDX_LOAD);
	CenterWindow(GetParent());
	GotoDlgCtrl(GetDlgItem(IDC_EDITCTRL));//set focus to edit control
return FALSE;//leave TRUE if we let the dialog to set focus
}

LRESULT CNewExtDlg::OnCtrlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ 
	if (GetDlgItem(IDC_HELPLINK) != (HWND)lParam) return 0;
	SetBkMode((HDC)wParam, TRANSPARENT);
return (LRESULT)GetStockObject( NULL_BRUSH );
}

LRESULT CNewExtDlg::OnDialogColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{ 
	SetBkMode((HDC)wParam, TRANSPARENT);
return (LRESULT)GetStockObject( WHITE_BRUSH );
}

LPCTSTR CNewExtDlg::GetNewExt() {return m_newext;}

LRESULT CNewExtDlg::OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_SAVE);
	EndDialog(IDOK);
return 0;
}

LRESULT CNewExtDlg::OnHelpMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LaunchHelp();
return 0;
}

LRESULT CNewExtDlg::OnHelpCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LaunchHelp();
return 0;
}

void CNewExtDlg::LaunchHelp()
{
	ATLTRACE("CNewExtDlg::OnHelp\n");
try {
	//get path to html help file (must be in the same folder and have the same name as dll)
	TCHAR dllpath[MAX_PATH];
	if ((0==GetModuleFileName(_Module.GetModuleInstance(), dllpath, sizeof(dllpath))) ||
		(GetLastError()==ERROR_INSUFFICIENT_BUFFER))
	 {ATLTRACE("problem getting help path\n");return;}

	if (PathRenameExtension(dllpath, _T(".chm")))
	 if (PathFileExists(dllpath)) ShellExecute(NULL, _T("open"), dllpath, NULL, NULL, SW_SHOWNORMAL);

	} catch (...) {ATLTRACE("exception in CNewExtDlg::OnHelp\n");return;}
}


LRESULT CNewExtDlg::OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{CAboutDlg abdlg; abdlg.DoModal();return 0;};
LRESULT CNewExtDlg::OnCloseDlg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{EndDialog(IDCANCEL);return 0;};
LRESULT CNewExtDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{EndDialog(IDCANCEL);return 0;};
