#ifndef _EDITFILTER_64D19AA8_9DD4_46D0_8CEF_FBED05E4DD18_
#define _EDITFILTER_64D19AA8_9DD4_46D0_8CEF_FBED05E4DD18_
#pragma once

/////////////////////////////////////////////////////////////////////////////
// Improved by T800
// v4
/////////////////////////////////////////////////////////////////////////////
// CFilterEdit - Filtering Edit control
//
// Written by Bjarke Viksoe (bjarke@viksoe.dk)
// Copyright (c) 2001 Bjarke Viksoe.
//
// Thanks to Yarp (Senosoft.com) for the Alt+Gr fix.
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
/////////////////////////////////////////////////////////////////////////////


#ifndef __cplusplus
  #error ATL requires C++ compilation (use a .cpp suffix)
#endif
#include <atlctrls.h>
#ifndef __ATLCTRLS_H__
  #error FilterEdit.h requires atlctrls.h to be included first
#endif
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <strsafe.h>
#pragma comment(lib,"strsafe.lib")//safe strcopy


//////////////////////////////////////////////////////////////////////////////////
// MSDN: "Data is inserted only if the clipboard contains data in CF_TEXT format"
#ifdef _UNICODE
#define CF_tTEXT CF_UNICODETEXT
#else
#define CF_tTEXT CF_TEXT
#endif

#define FE_BADCHAR 0
#define FE_TXTOVERFLOW 1

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit

#define ES_EX_JUMPY           0x00000001
#define ES_EX_INCLUDEMASK     0x00000002
#define ES_EX_EXCLUDEMASK     0x00000004

class CFilterEdit : public CWindowImpl< CFilterEdit, CEdit, CControlWinTraits >
{
public:
   DECLARE_WND_SUPERCLASS(_T("WTL_FilterEdit"), CEdit::GetWndClassName())  

   BEGIN_MSG_MAP(CFilterEdit)
      MESSAGE_HANDLER(WM_CREATE, OnCreate)
      MESSAGE_HANDLER(WM_CHAR, OnChar)
      MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
      MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
      MESSAGE_HANDLER(WM_PASTE, OnPaste)
   END_MSG_MAP()

   DWORD m_dwExtStyle;
   WTL::CString m_szExclude;
   WTL::CString m_szInclude;

   // Operations

   BOOL SubclassWindow(HWND hWnd)
   {
      ATLASSERT(m_hWnd == NULL);
      ATLASSERT(::IsWindow(hWnd));
      BOOL bRet = CWindowImpl< CFilterEdit, CEdit, CControlWinTraits >::SubclassWindow(hWnd);
      if( bRet ) _Init();
      return bRet;
   }

   DWORD GetExtendedEditStyle() const
   {
      ATLASSERT(::IsWindow(m_hWnd));
      return m_dwExtStyle;
   }

   void SetExtendedEditStyle(DWORD dwStyle)
   {
      ATLASSERT(::IsWindow(m_hWnd));
      m_dwExtStyle = dwStyle;
      Invalidate();
   }

   void LimitText(UINT_PTR cch)
   {
	   ATLASSERT(::IsWindow(m_hWnd));
	   SendMessage(m_hWnd, (UINT)EM_LIMITTEXT, cch, NULL);
   }

   LRESULT GetLimitText()
   {
	   ATLASSERT(::IsWindow(m_hWnd));
	   return SendMessage(m_hWnd, (UINT)EM_GETLIMITTEXT, NULL, NULL);
   }

   void SetExcludeMask(LPCTSTR pstrMask)
   {
      ATLASSERT(pstrMask);
      m_dwExtStyle |= ES_EX_EXCLUDEMASK;
      m_szExclude=pstrMask;
   }

   LPCTSTR GetExcludeMask() { return m_szExclude;}
   LPCTSTR GetIncludeMask() { return m_szInclude;}

   void SetIncludeMask(LPCTSTR pstrMask)
   {
      ATLASSERT(pstrMask);
      m_dwExtStyle |= ES_EX_INCLUDEMASK;
      m_szInclude=pstrMask;
   }


   // Message Handlers

   LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
      _Init();
      return lRes;
   }

   LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
   {
      TCHAR ch = (TCHAR)wParam;
      bool bCombi = (::GetKeyState(VK_CONTROL) & 0x8000) || (::GetKeyState(VK_MENU) & 0x8000);
      if( bCombi && (::GetKeyState(VK_CONTROL) & 0x8000) && (::GetKeyState(VK_MENU) & 0x8000))
		  bCombi = false; // Alt+Gr key is Ok

      if( !bCombi && ch>=VK_SPACE )
	  {
         if( m_dwExtStyle & ES_EX_EXCLUDEMASK )
		 {
            if( ::StrChr(m_szExclude, ch)!=NULL )
			{
				ATLTRACE("bad char detected!\n");
				::MessageBeep((UINT)-1);
				ShowBaloonWarning(FE_BADCHAR);
				return 0;
            }
         }
         if( m_dwExtStyle & ES_EX_INCLUDEMASK )
		 {
            if( ::StrChr(m_szInclude, ch)==NULL )
			{
				ATLTRACE("bad char detected!\n");
				::MessageBeep((UINT)-1);
				return 0;
            }
         }
      }

      LRESULT lResult = DefWindowProc(uMsg, wParam, lParam);
      if( m_dwExtStyle & ES_EX_JUMPY ) _NextDlgItem();
   return lResult;
   }

   LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
   {
      if( wParam==VK_RIGHT && (m_dwExtStyle & ES_EX_JUMPY) )
	  {
         if(_NextDlgItem()) return 0;
      }
      bHandled = FALSE;
      return 0;
   }

///////////// excl/inlc filter?
   LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
   {
		if(!_CheckText((LPCTSTR)lParam))
		{
			WTL::CString pstrNew=(LPCTSTR)lParam;

			_StrRemove(pstrNew.GetBuffer(pstrNew.GetLength()), m_szExclude);//remove bad chars
			pstrNew.ReleaseBuffer();

			::MessageBeep((UINT)-1);
			ShowBaloonWarning(FE_BADCHAR);

			if (!_CheckText(pstrNew)) return 0;//failed
			if (pstrNew.IsEmpty()) return 0;//don't paste empty string
		return DefWindowProc(uMsg, wParam, (LPARAM)(LPCTSTR)pstrNew);
		}

		size_t strlen=0;
		if FAILED(::StringCchLength((LPCTSTR)lParam, STRSAFE_MAX_CCH, &strlen)) {::MessageBeep((UINT)-1);return 0;}
		if ((LRESULT)strlen > GetLimitText())
		{
			::MessageBeep((UINT)-1);
			ShowBaloonWarning(FE_TXTOVERFLOW);
			return FALSE;
		}

		bHandled = FALSE;
	return 0;
   }

///// note - we consider only removing chars for now
	LRESULT OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		HRESULT hr=0;
		LRESULT lr=0;

		CAutoClipboard cl;//automatically closes clipboard in dtor
		if (!cl.Open(m_hWnd)) return 0;

		//get clipboard string
		HANDLE hclStr=::GetClipboardData(CF_tTEXT);//clipboard owns this handle
		if (!hclStr) return 0;

		WTL::CString pstrNew;
		pstrNew=(TCHAR*)GlobalLock(hclStr);
		GlobalUnlock(hclStr);//never forget

		if (pstrNew.IsEmpty()) return 0;
		if (pstrNew.GetLength()>GetLimitText())
		{
			ShowBaloonWarning(FE_TXTOVERFLOW);
			return 0;
		}
		if( _CheckText(pstrNew)) return DefWindowProc(uMsg, wParam, lParam);//no bad chars

		_StrRemove(pstrNew.GetBuffer(pstrNew.GetLength()), m_szExclude);//remove bad chars
		pstrNew.ReleaseBuffer();
		if (!_CheckText(pstrNew)) return 0;//failed

		if (!pstrNew.IsEmpty())//don't paste empty string (erases selection in editbox)
		{
			HGLOBAL hglbCopy=GlobalAlloc(GMEM_MOVEABLE, (pstrNew.GetLength()+1)*sizeof(TCHAR));
			if (hglbCopy)
			{
				hr = StringCchCopy((LPTSTR)GlobalLock(hglbCopy), pstrNew.GetLength()+1, pstrNew);
				GlobalUnlock(hglbCopy);
				if FAILED(hr) return 0;
				if (SetClipboardData(CF_tTEXT, hglbCopy)) lr=DefWindowProc(uMsg, wParam, lParam);
			}
		}
		::MessageBeep((UINT)-1);
		ShowBaloonWarning(FE_BADCHAR);
	return lr;
	}

private:
   void _Init()
   {
      ATLASSERT(::IsWindow(m_hWnd));
#ifdef _DEBUG
      // Check class
      TCHAR szBuffer[16];
      if( ::GetClassName(m_hWnd, szBuffer, (sizeof(szBuffer)/sizeof(TCHAR))-1) ) {
         ATLASSERT(::lstrcmpi(szBuffer, CEdit::GetWndClassName())==0);
         ATLASSERT(GetStyle() & WS_CHILD);
         ATLASSERT((GetStyle() & (ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD|ES_READONLY))==0);
      }
#endif // _DEBUG
	  m_szExclude.Empty();
      m_szInclude.Empty();
      m_dwExtStyle = 0;
   }

   BOOL _NextDlgItem()
   {
      POINT pt;
      ::GetCaretPos(&pt);
      if( CharFromPos(pt)==(int)GetLimitText() )
	  {
         ::SetFocus( ::GetNextDlgTabItem(GetParent(), m_hWnd, FALSE) );
         return TRUE;
      }
      return FALSE;
   }

   BOOL _CheckText(LPCTSTR pstr)
   {
      if( pstr==NULL ) return TRUE;
      ATLASSERT(!::IsBadStringPtr(pstr, -1));
      if( m_dwExtStyle & ES_EX_EXCLUDEMASK )
	  {
         if( ::StrPBrk(pstr, m_szExclude)!=NULL ) return FALSE;
      }
      if( m_dwExtStyle & ES_EX_INCLUDEMASK )
	  {
         while( *pstr )
		 {
            if( ::StrChr(m_szInclude, *pstr)==NULL ) return FALSE;
            pstr = ::CharNext(pstr);
         }
      }
      return TRUE;
   }

//maybe? StrCrop																				/ BOOL bCaseSensitive=FALSE)
//maybe? _StrFilter(LPTSTR pszSource, LPCTSTR pszExcludeChars, LPCTSTR pszIncludeChars, BOOL bExclI=FALSE, BOOL bInclI=FALSE)
// we only need removing for now
	BOOL _StrRemove(LPTSTR pszSource, LPCTSTR pszExcludeChars, BOOL bCaseSensitive=FALSE)
	{
		ATLASSERT(pszSource || pszExcludeChars);
		if (!pszSource || !pszExcludeChars) return FALSE;

		size_t srclen=0;
		if FAILED(::StringCchLength(pszSource, STRSAFE_MAX_CCH, &srclen)) return FALSE;
		if (srclen==0) return FALSE;//empty string ("") is valid

		TCHAR* psrc=pszSource;
		TCHAR* pend=pszSource+srclen;
		while(*psrc && (psrc < pend)) //walk the source string
		{
			if (bCaseSensitive)
				while(::StrChr(pszExcludeChars, *psrc))
					::MoveMemory(psrc, ::CharNext(psrc), (pszSource+srclen-psrc)*sizeof(TCHAR));
			else
				while(::StrChrI(pszExcludeChars, *psrc))
					::MoveMemory(psrc, ::CharNext(psrc), (pszSource+srclen-psrc)*sizeof(TCHAR));

			psrc=::CharNext(psrc);
		}
		*psrc=_T('\0');//null-terminate (just in case)?
	return TRUE;
	}

protected:
	virtual void ShowBaloonWarning(int iw)
	{
		EDITBALLOONTIP baloonStruct;
		SecureZeroMemory(&baloonStruct, sizeof(EDITBALLOONTIP));
		baloonStruct.cbStruct=sizeof(baloonStruct);

		// 'if' for smaller number of cases, 'switch' for large
		if (iw==FE_BADCHAR)
		{
			baloonStruct.pszText=_T("A file extension cannot contain space\nor any of the following characters:\n\t\\ / : * ? \" < > |");
			baloonStruct.pszTitle=_T("Invalid characters");
			baloonStruct.ttiIcon=TTI_WARNING;
			if (!ShowBalloonTip(&baloonStruct))
			 MessageBox(baloonStruct.pszText,baloonStruct.pszTitle, MB_OK | MB_ICONWARNING | MB_TASKMODAL);//Win2000 fallback
			return;
		}
		if (iw==FE_TXTOVERFLOW)
		{
			baloonStruct.pszText=_T("The file extension is too long.\nTry a shorter extension.");
			baloonStruct.pszTitle=_T("Extension too long");
			baloonStruct.ttiIcon=TTI_WARNING;
			if (!ShowBalloonTip(&baloonStruct))
			 MessageBox(baloonStruct.pszText,baloonStruct.pszTitle, MB_OK | MB_ICONWARNING | MB_TASKMODAL);//Win2000 fallback
			return;
		}
	}

private:
	class CAutoClipboard
	{
	public:
		virtual ~CAutoClipboard() {::CloseClipboard();}
		BOOL Open(HWND hWndNewOwner) {return ::OpenClipboard(hWndNewOwner);}
	};
};

#endif//_EDITFILTER_64D19AA8_9DD4_46D0_8CEF_FBED05E4DD18_
