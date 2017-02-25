#ifndef _ABOUT_A5096FA1_32DC_4392_A829_445558F0F4B5_
#define _ABOUT_A5096FA1_32DC_4392_A829_445558F0F4B5_
#pragma once
#include "..\resource.h"

class CAboutDlg : public CDialogImpl<CAboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(CAboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnClick)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, OnDlgColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnDlgColor)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnDlgColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
	return (LRESULT)AtlGetStockBrush(WHITE_BRUSH);
	}
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		SetWindowLongW(GWL_STYLE,WS_BORDER);
	return TRUE;
	}
	LRESULT OnClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/){EndDialog(IDOK);return 0;}
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/){EndDialog(wID);return 0;}
};
#endif//_ABOUT_A5096FA1_32DC_4392_A829_445558F0F4B5_
