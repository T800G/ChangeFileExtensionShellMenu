#include "StdAfx.h"
#include "CmdLink.h"

CCmdLink::CCmdLink(void)
{
	//link font
	m_font=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
	if (m_font.IsNull()) throw CLERR_CREATEFONT;
	LOGFONT lfont={0};
	GetObject(m_font,sizeof(LOGFONT), &lfont);
	lfont.lfUnderline=TRUE;
	m_fonthover.CreateFontIndirect(&lfont);

	m_handCursor=::LoadCursor(NULL,IDC_HAND);
	m_defaultCursor=::LoadCursor(NULL,IDC_ARROW);

	m_hover=false;
	if (!m_bmp.IsNull()) m_bmp.DeleteObject();
	m_margin=5;
}
//
HWND CCmdLink::CreateCmdLink(LPCLLINK_CREATEINFO ci)
{
	if (!::MapDialogRect(ci->hWndParent,ci->rcPos)) return NULL;
	m_ID=ci->nID;
	m_text=ci->szWindowName;
	HWND hw=CWindowImpl<CCmdLink, ATL::CWindow, ATL::CControlWinTraits>::Create( ci->hWndParent, *ci->rcPos,
												ci->szWindowName, CLWINTRAITS, 0, ci->nID, 0);
	if (hw==NULL) return NULL;

	if (ci->bmpID!=NULL) { m_bmp.LoadBitmapW(ci->bmpID); m_bmp.GetSize(m_bmpsize); }

return hw;
}
//
LRESULT CCmdLink::OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_hover && (m_hWnd==(HWND)wParam)) SetCursor(m_handCursor);
	else bHandled = FALSE;
return (LRESULT)TRUE;
}
//
LRESULT CCmdLink::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (mouseInHotRect(lParam)) {m_hover=true;startTrackMouseLeave();}
	else m_hover=false;
	Invalidate();
return 0;
}
//
LRESULT CCmdLink::OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{ m_hover=false; Invalidate(); return 0; }

LRESULT CCmdLink::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	if (mouseInHotRect(lParam)) {m_hover=true;Invalidate();SetCapture();} return 0;}
//
LRESULT CCmdLink::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (GetCapture()==m_hWnd)
	{
	ReleaseCapture();
		if (mouseInHotRect(lParam))
		{
			m_hover=true; Invalidate();
			::SendMessage(GetParent(),WM_COMMAND, MAKEWPARAM(m_ID, BN_CLICKED),(LPARAM)m_hWnd);
		}
	}
return 0;
}
//
void CCmdLink::DoPaint(CDCHandle dc)
{
	GetClientRect(m_clientRect);
	CDC MemDC;
	MemDC.CreateCompatibleDC(dc);

	dc.FillSolidRect(m_clientRect, CLCOLOR_WHITE);

	SIZE txtsize={0};
	m_bmpRect.SetRect(0,0,0,0);
	m_textRect.SetRect(0,0,0,0);

	CFont oldfont=dc.SelectFont(m_hover ? m_fonthover : m_font);
	dc.GetTextExtent(m_text,m_text.GetLength(),&txtsize);

	//Vcenter
	m_bmpRect.top=(m_clientRect.Height()-m_bmpsize.cy)/2;
	m_bmpRect.bottom=m_bmpRect.top+m_bmpsize.cy;
	m_textRect.top=(m_clientRect.Height()-txtsize.cy)/2;
	m_textRect.bottom=m_textRect.top+txtsize.cy;

	//Hcenter
	m_bmpRect.right=m_bmpsize.cx;
	m_textRect.left=m_bmpRect.right+m_margin;
	m_textRect.right=m_textRect.left+txtsize.cx;
	m_textRect.right=(m_clientRect.Width()<m_textRect.right) ? m_clientRect.Width() : m_textRect.right;
	
	//draw bmp
	CBitmap m_oldbmp=MemDC.SelectBitmap(m_bmp);
	dc.TransparentBlt(m_bmpRect.left, m_bmpRect.top, m_bmpsize.cx,m_bmpsize.cy,
							MemDC, 0,0, m_bmpsize.cx, m_bmpsize.cy, CLCOLOR_TRANSPARENT);
	MemDC.SelectBitmap(m_oldbmp);

	//draw text
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_hover ? CLCOLOR_HOVER : CLCOLOR_NORMAL);
	dc.DrawTextW(m_text,m_text.GetLength(), m_textRect, DT_LEFT | DT_VCENTER);

	dc.SelectFont(oldfont);
}

BOOL CCmdLink::startTrackMouseLeave()
{
	TRACKMOUSEEVENT tme = { 0 };
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE;
	tme.hwndTrack = m_hWnd;
return ::_TrackMouseEvent(&tme);
}

BOOL CCmdLink::mouseInHotRect(LPARAM lParam)
{
	CPoint pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	CRect r2(m_bmpRect.left,m_textRect.top,m_textRect.right,m_textRect.bottom );
return (m_bmpRect.PtInRect(pt) || r2.PtInRect(pt));
}
