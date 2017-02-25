#ifndef _NEWEXTDLG_A5096FA1_32DC_4392_A829_445558F0F4B5_
#define _NEWEXTDLG_A5096FA1_32DC_4392_A829_445558F0F4B5_
#pragma once
#include <atlddx.h>
#include "..\resource.h"
#include "..\helpers.h"
#include "CmdLink.h"
#include "EditFilter.h"


class CNewExtDlg:	public CDialogImpl<CNewExtDlg>,
					public CWinDataExchange<CNewExtDlg>,
					public CDialogDrag<CNewExtDlg>
{
public:
	enum { IDD = IDD_NEWEXTDLG };

	BEGIN_MSG_MAP(CNewExtDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_CLOSE, OnCloseDlg)
		MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnDialogColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColor)
		MESSAGE_HANDLER(WM_HELP, OnHelpMessage)//launch help on F1 - modal dialog accelerator issue workaround
		COMMAND_ID_HANDLER(IDOK, OnOk)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDABOUT1, OnAbout)
		COMMAND_ID_HANDLER(IDC_HELPLINK, OnHelpCommand)
		CHAIN_MSG_MAP(CDialogDrag<CNewExtDlg>)
	END_MSG_MAP()

    BEGIN_DDX_MAP(CNewExtDlg)
		DDX_TEXT(IDC_EDITCTRL, m_newext)
	END_DDX_MAP()

public:
	CNewExtDlg(LPCTSTR szNewExt=NULL);
	virtual ~CNewExtDlg(){}

public:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnOk(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	inline LRESULT OnHelpMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	inline LRESULT OnHelpCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	void LaunchHelp();
	LPCTSTR GetNewExt();

	LRESULT OnDialogColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCtrlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAbout(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCloseDlg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CString m_newext;//?samo TCHAR [_MAX_EXT]
	CCmdLink m_helpLink;
	CFilterEdit m_extEdit;
	LRESULT m_lrDwprc;
};
#endif//_NEWEXTDLG_A5096FA1_32DC_4392_A829_445558F0F4B5_
