#ifndef _CLLINK_DBB212E5_FF1E_44C5_B14F_963CA91CCA52_
#define _CLLINK_DBB212E5_FF1E_44C5_B14F_963CA91CCA52_
#include <atlmisc.h>
#include <atlframe.h>


#define CLCLASSNAME _T("CommandLink")

#define CLCOLOR_TRANSPARENT RGB(255,0,255)
#define CLCOLOR_NORMAL  RGB(0,64,128)
#define CLCOLOR_HOVER  RGB(0,128,192)
#define CLCOLOR_BLACK 0
#define CLCOLOR_WHITE RGB(255,255,255)

#define CLWINTRAITS (WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

enum CLERROR {CLERR_VOID, CLERR_CREATE, CLERR_CREATEFONT, CLERR_DLGUNITSCONV};


typedef struct tagCLLINK_CREATEINFO
{
	HWND hWndParent;
	LPRECT rcPos;
	LPCTSTR szWindowName;
	DWORD dwStyle;
	DWORD dwExStyle;
	UINT nID;
	LPVOID lpCreateParam;
	UINT bmpID;

} CLLINK_CREATEINFO, *PCLLINK_CREATEINFO, NEAR *NPCLLINK_CREATEINFO, FAR *LPCLLINK_CREATEINFO;



class CCmdLink: public CDoubleBufferImpl<CCmdLink>, public CWindowImpl<CCmdLink, ATL::CWindow, ATL::CControlWinTraits>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg){pMsg; return FALSE;}

	BEGIN_MSG_MAP(CCmdLink)
		MESSAGE_HANDLER(WM_SETCURSOR	, OnSetCursor)
		MESSAGE_HANDLER(WM_MOUSEMOVE	, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE	, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN	, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP	, OnLButtonUp)
		CHAIN_MSG_MAP(CDoubleBufferImpl<CCmdLink>)
	END_MSG_MAP()

	CCmdLink(void);
	virtual ~CCmdLink(void){};

	void DoPaint(CDCHandle dc);
	HWND CreateCmdLink(LPCLLINK_CREATEINFO ci);

	LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseLeave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	UINT m_ID;
	CString m_text;
	CFont m_font,m_fonthover;
	CBitmap m_bmp;
	CSize m_bmpsize;
	int m_margin;
	CRect m_bmpRect, m_textRect, m_clientRect;
	bool m_hover;

	CCursor m_handCursor, m_defaultCursor;
	BOOL startTrackMouseLeave();
	BOOL mouseInHotRect(LPARAM lParam);	
};

#endif//_CLLINK_DBB212E5_FF1E_44C5_B14F_963CA91CCA52_
