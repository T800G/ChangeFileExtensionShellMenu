#include "stdafx.h"
#include <shlwapi.h>
#pragma comment(lib,"shlwapi")

#include "ChangeExt.h"
#include "ShellCtxMenu.h"

#define MENU_HELP_STRINGw L"Change file extension of the selected files."
#define MENU_HELP_STRINGa "Change file extension of the selected files." 

/////////////////////////////////////////////////////////////////////////////
// CShellCtxMenu
CShellCtxMenu::CShellCtxMenu()
{
	m_hMenuBmp=(HBITMAP)LoadImage(_Module.GetResourceInstance(),
										MAKEINTRESOURCE(IDB_MENUBITMAP),
											IMAGE_BITMAP, 0,0, LR_CREATEDIBSECTION);
	if (m_hMenuBmp)
		ReplaceDIBColor(&m_hMenuBmp, RGB(255,0,255), GetSysColor(COLOR_MENU));

}

CShellCtxMenu::~CShellCtxMenu()
{
	if (m_hMenuBmp) ::DeleteObject(m_hMenuBmp);
}


HRESULT CShellCtxMenu::Initialize ( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID )
{
	ATLTRACE("CShellCtxMenu::Initialize\n");
	HRESULT hr = S_OK;

try {
	// Look for CF_HDROP data in the data object
	FORMATETC fmt = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL }; 
	STGMEDIUM stg = { TYMED_HGLOBAL };
	if FAILED(pDataObj->GetData(&fmt, &stg)) return E_INVALIDARG;

	// Get a pointer to the actual data
	HDROP hDrop = (HDROP)GlobalLock(stg.hGlobal); 
	if (NULL==hDrop) return E_INVALIDARG;

	UINT i;
	UINT nFiles = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); // get number of selected files
	ATLTRACE("DragQueryFile:=%d files selected\n",nFiles);
	if (nFiles)
	{
		//find longest string
		ULONG sMaxLen=0;
		for (i=0; i<nFiles; i++) sMaxLen=max(sMaxLen, ::DragQueryFile(hDrop, i, NULL,0));
		ATLASSERT(sMaxLen);
		sMaxLen++; //DragQueryFile doesn't count terminating null

		TCHAR* pszFilename=(TCHAR*)::CoTaskMemAlloc(sizeof(TCHAR)*sMaxLen);
		if (pszFilename)
		{
			for (i=0; i<nFiles; i++)
			{
				if (0==::DragQueryFile(hDrop, i, pszFilename, sMaxLen))
					hr=E_INVALIDARG;
				else m_ExtMgr.AddFile(pszFilename);
			}
		}
		else {ATLTRACE("CoTaskMemAlloc failed\n"); hr=E_FAIL;}
		::CoTaskMemFree(pszFilename);
		m_ExtMgr.ProcessFiles();
	}
	else hr=E_INVALIDARG;

	GlobalUnlock(stg.hGlobal);
	ReleaseStgMedium(&stg);
	}
catch (...)
	{
	ATLTRACE("exception in CShellCtxMenu::Initialize\n");
	return E_FAIL;
	}
return hr;
}


HRESULT CShellCtxMenu::QueryContextMenu (HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags)
{
	//ATLTRACE("CShellCtxMenu::QueryContextMenu\n");
	dbg_TraceCMFlags(uFlags);
	if (uFlags & CMF_VERBSONLY)//skip shortcut files (*.lnk)
	{
		ATLTRACE("QueryContextMenu (uFlags & CMF_VERBSONLY) skipping shortuct\n");
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
	}
	if (uFlags & CMF_DEFAULTONLY) return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
		
	UINT uCmdID = uidFirstCmd;//ok until (uCmdID<uidLastCmd) ATLASSERT?

	if (m_ExtMgr.IsExtEmpty()) m_sCtxExt=_T("Change &File Extension...");
	else
	{
		m_sCtxExt=m_ExtMgr.GetNewExt();
		m_sCtxExt.MakeUpper();//show in shell menu in uppercase
		m_sCtxExt+=_T("   Change &File Extension...");
	}

	if (!InsertMenu(hmenu, uMenuIndex, MF_STRING | MF_BYPOSITION, uCmdID++, m_sCtxExt)) return E_FAIL;
	if (m_hMenuBmp) SetMenuItemBitmaps(hmenu, uMenuIndex, MF_BYPOSITION, m_hMenuBmp, m_hMenuBmp);

return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, (uCmdID - uidFirstCmd)+1);//proper way
}


HRESULT CShellCtxMenu::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax)
{
	//ATLTRACE("CShellCtxMenu::GetCommandString\n");
	// must be 0 since we have only one menu item
	if (idCmd > 0) return E_INVALIDARG;
	HRESULT hr=E_INVALIDARG;

	//Explorer is asking for a help string / must explicitely handle GCS_UNICODE
    if (uFlags & GCS_HELPTEXT)
	{
		hr=(uFlags & GCS_UNICODE) ? StringCchCopyW((LPWSTR)pszName, cchMax, MENU_HELP_STRINGw) :
									StringCchCopyA(pszName, cchMax, MENU_HELP_STRINGa);
	return (FAILED(hr) ? E_FAIL : S_OK);
	}
return E_INVALIDARG;
}

HRESULT CShellCtxMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pCmdInfo)
{
	ATLTRACE("InvokeCommand\n");
	//if lpVerb points to a string, ignore this function call (we don't have a verb)
    if (0!=HIWORD(pCmdInfo->lpVerb)) return E_INVALIDARG;

try {
		// Check that lpVerb is our command (index 0)
		if (LOWORD(pCmdInfo->lpVerb)==0) 
		{
			CNewExtDlg dlg(m_ExtMgr.GetNewExt()); //show dialog
			if (IDOK==dlg.DoModal(pCmdInfo->hwnd))
				m_ExtMgr.ChangeExtensions(dlg.GetNewExt(), pCmdInfo->hwnd);
		}
	}
catch(...)
	{
	ATLTRACE("exception in CShellCtxMenu::InvokeCommand\n");return E_FAIL;
	}
return S_OK;
}
