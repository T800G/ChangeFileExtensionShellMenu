#ifndef _SHELLCTXMENU_H__6DDF9D3C_AF32_4D68_9C24_1BFA45875849_
#define _SHELLCTXMENU_H__6DDF9D3C_AF32_4D68_9C24_1BFA45875849_
#pragma once

#include "resource.h"       // main symbols
#include "ExtManager.h"

#include <shlobj.h>
#include <comdef.h>


class ATL_NO_VTABLE CShellCtxMenu : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShellCtxMenu,&CLSID_ShellCtxMenu>,
	public IShellExtInit,
	public IContextMenu
{
public:
	CShellCtxMenu();
	virtual ~CShellCtxMenu();

	BEGIN_COM_MAP(CShellCtxMenu)
		COM_INTERFACE_ENTRY(IShellExtInit)
		COM_INTERFACE_ENTRY(IContextMenu)
	END_COM_MAP()

	DECLARE_NOT_AGGREGATABLE(CShellCtxMenu) 
	DECLARE_REGISTRY_RESOURCEID(IDR_ShellCtxMenu)
	DECLARE_PROTECT_FINAL_CONSTRUCT()

public:
	// IShellExtInit
	STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

	// IContextMenu
	STDMETHOD(GetCommandString)(UINT_PTR, UINT, UINT*, LPSTR, UINT);
	STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO);
	STDMETHOD(QueryContextMenu)(HMENU, UINT, UINT, UINT, UINT);
private:
	CExtManager m_ExtMgr;
	CString m_sCtxExt;
	HBITMAP m_hMenuBmp;
};

#endif//_SHELLCTXMENU_H__6DDF9D3C_AF32_4D68_9C24_1BFA45875849_
