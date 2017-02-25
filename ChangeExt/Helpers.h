#ifndef _HELPERS_F032C94F_CE1C_4D63_A438_4C516A234C0E_
#define _HELPERS_F032C94F_CE1C_4D63_A438_4C516A234C0E_
#include <sal.h>

#ifndef _UNICODE
	#error CSplitPath is UNICODE-only class
#endif
#include <math.h>
class CSplitPath
{
public:
	virtual ~CSplitPath(){}
    CSplitPath(){ SplitPathInitialize();}
    CSplitPath(LPCTSTR lpszPath){ SplitPathInitialize(); Split(lpszPath);}
    BOOL Split(LPCTSTR lpszPath )
	{
		SplitPathInitialize();
		if (lpszPath == NULL) return FALSE;
		SplPstatus=_wsplitpath_s(lpszPath, drive0, _MAX_DRIVE, dir0, _MAX_DIR, fname0, _MAX_FNAME, ext0, _MAX_EXT);
		fullpath=lpszPath;
		drive=drive0;
		dir=dir0;
		filename=fname0;
		ext_nodot=ext=ext0;
		ext_nodot.Delete(0,1);

		filetitle+=filename;
		filetitle+=ext;
	return ((SplPstatus!=EINVAL) && (SplPstatus!=ERANGE));
	}
	CString filetitle,fullpath, drive, dir, filename, ext, ext_nodot;
	errno_t SplPstatus;
private:
	void SplitPathInitialize()
		{
			SecureZeroMemory(drive0,sizeof(drive0));
			SecureZeroMemory(dir0,sizeof(dir0));
			SecureZeroMemory(fname0,sizeof(fname0));
			SecureZeroMemory(ext0,sizeof(ext0));
			SplPstatus=NULL;
			fullpath.Empty();
			drive.Empty();
			dir.Empty();
			filename.Empty();
			ext.Empty();
			ext_nodot.Empty();
			filetitle.Empty();
		}
    TCHAR drive0[_MAX_DRIVE];
    TCHAR dir0[_MAX_DIR];
    TCHAR fname0[_MAX_FNAME];
    TCHAR ext0[_MAX_EXT];
};

/////////////////////////////////////////////
// Replaces color in DIB palette
#include <windows.h>
inline BOOL ReplaceDIBColor(__inout HBITMAP* pDIB, __in COLORREF oldColor, __in COLORREF newColor)
{
	_ASSERTE(pDIB!=NULL);
	if (pDIB==NULL) return FALSE;
	if (oldColor==newColor) return TRUE;//nothing to do
	BOOL bRet=FALSE;

	//get color information
	DIBSECTION ds;
	if (!::GetObject(*pDIB, sizeof(DIBSECTION), &ds)) return FALSE;
	UINT nColors = ds.dsBmih.biClrUsed ? ds.dsBmih.biClrUsed : 1<<ds.dsBmih.biBitCount;//bpp to UINT
	if (ds.dsBmih.biBitCount>8 || nColors>256) return FALSE;//8bpp is max

	HDC hDC=::CreateCompatibleDC(NULL);
	if (!hDC) return FALSE;
	HBITMAP hbmpOld=(HBITMAP)::SelectObject(hDC, *pDIB);
	if (hbmpOld!=NULL)
	{
		//replace color table entries
		RGBQUAD clrtbl[256*sizeof(RGBQUAD)];
		if (::GetDIBColorTable(hDC, 0, nColors, clrtbl))
		{
			UINT i;
			for (i=0; i<nColors; i++)
			{
				if (oldColor==RGB(clrtbl[i].rgbRed, clrtbl[i].rgbGreen, clrtbl[i].rgbBlue))
				{
					clrtbl[i].rgbRed=GetRValue(newColor);
					clrtbl[i].rgbGreen=GetGValue(newColor);
					clrtbl[i].rgbBlue=GetBValue(newColor);
					bRet=TRUE;
				}
			}
			if (bRet)//set new table
				if (!::SetDIBColorTable(hDC, 0, nColors, clrtbl)) bRet=FALSE;
		}
	*pDIB=(HBITMAP)::SelectObject(hDC, hbmpOld);
	}
	::DeleteDC(hDC);
return bRet;
}


/////////////////////////////////////////////
// Works for all color depths
//static HBITMAP bmpReplaceColor(HBITMAP hbm, COLORREF oldColor, COLORREF newColor)
//{
//	CDC hdcOld = CreateCompatibleDC(NULL);
//	CDC hdcNew = CreateCompatibleDC(NULL);
//	CBitmapHandle _hbmOld = NULL, _hbmOld2 = NULL, bmOld = hbm;
//	BOOL bOK=false;
//	CBitmap bmNew = NULL;
//	SIZE bmpDim = { 0 };
//
//	bmOld.GetSize(bmpDim);
//	_hbmOld = hdcOld.SelectBitmap(bmOld);
//
//	bmNew.CreateCompatibleBitmap(hdcOld, bmpDim.cx, bmpDim.cy);
//	ATLASSERT(!bmNew.IsNull());//error checking!?
//
//	_hbmOld2 = hdcNew.SelectBitmap(bmNew);
//
//	hdcNew.FillSolidRect(0,0,bmpDim.cx, bmpDim.cy, newColor);
//	bOK = hdcNew.TransparentBlt(0,0,bmpDim.cx,bmpDim.cy, hdcOld, 0,0, bmpDim.cx,bmpDim.cy, oldColor);
//
//	hdcOld.SelectBitmap(_hbmOld);
//	hdcNew.SelectBitmap(_hbmOld2);
//	if (bOK == FALSE) bmNew.DeleteObject();
//return bmNew.Detach();
//}


//dialog drag support
template <class T> class CDialogDrag
{
public:
		BEGIN_MSG_MAP(CDialogDrag<T>)
			MESSAGE_HANDLER(WM_NCHITTEST, OnNCHitTest)
		END_MSG_MAP()

	LRESULT OnNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		T* pT=static_cast<T*>(this);
		m_lHitTest = ::DefWindowProcW(pT->m_hWnd, uMsg, wParam, lParam);
		if (m_lHitTest == HTCLIENT) return HTCAPTION;
	return m_lHitTest;
	}
private:
	LRESULT m_lHitTest;
};


#ifdef _DEBUG
#include <shobjidl.h>
#define dbg_TraceCMFlags(x) _dbgTraceCMFlags((x))
static void _dbgTraceCMFlags(UINT uFlags)
{
	ATLTRACE("IContextMenu::QueryContextMenu: uFlags=");
	if (uFlags & CMF_NORMAL) ATLTRACE("CMF_NORMAL");
	if (uFlags & CMF_DEFAULTONLY) ATLTRACE("|CMF_DEFAULTONLY");
	if (uFlags & CMF_VERBSONLY) ATLTRACE("|CMF_VERBSONLY");
	if (uFlags & CMF_EXPLORE) ATLTRACE("|CMF_EXPLORE");
	if (uFlags & CMF_NOVERBS) ATLTRACE("|CMF_NOVERBS");
	if (uFlags & CMF_CANRENAME) ATLTRACE("|CMF_CANRENAME");
	if (uFlags & CMF_NODEFAULT) ATLTRACE("|CMF_NODEFAULT");
	if (uFlags & CMF_INCLUDESTATIC) ATLTRACE("|CMF_INCLUDESTATIC");
	if (uFlags & 0x00000080) ATLTRACE("|CMF_ITEMMENU");
	if (uFlags & CMF_EXTENDEDVERBS) ATLTRACE("|CMF_EXTENDEDVERBS");
	if (uFlags & 0x00000200) ATLTRACE("|CMF_DISABLEDVERBS");
	if (uFlags & CMF_RESERVED) ATLTRACE("|CMF_RESERVED");
	ATLTRACE("\n");
}
#else
#define dbg_TraceCMFlags(x)
#endif//_DEBUG

#endif//_HELPERS_F032C94F_CE1C_4D63_A438_4C516A234C0E_
