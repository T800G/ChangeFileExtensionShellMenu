#ifndef _EXTMANAGER_F1E7D5E1_C883_4D0A_9975_B8D9B7798168_
#define _EXTMANAGER_F1E7D5E1_C883_4D0A_9975_B8D9B7798168_
#pragma once
#include <vector>
#include <Shlobj.h>
#include <shlguid.h>
#include "dialog/NewextDlg.h"
#include "Helpers.h"

enum EXTMANAGER_STATUS {EXTM_NOCODE, EXTM_RES_ERROR, EXTM_FILEOP_ERROR};

class CExtManager
{
public:
	CExtManager() { m_filecount=0; }
	virtual ~CExtManager() { m_vFileList.clear(); }


	void AddFile(LPCTSTR szFile)
	{
		if (PathIsDirectory(szFile)) return;//skip directories
		if (PathMatchSpec(szFile, _T("*.lnk")))//skip selected shortcuts (renames target!)
		{
			ATLTRACE(_T("CExtManager::AddFile skipping shortcut: %s\n"), szFile);
			return;
		}

		CString fe=szFile;
		m_vFileList.push_back(fe);//build file list

		//get ext of right-clicked file
		if (m_filecount==0)//just first file ///  *vector.begin()->???
		{
			m_newext=CharNext(PathFindExtension(szFile));
			ATLTRACE(_T("CExtManager::getFirstExt: %s\n"), m_newext);
		}
		m_filecount++;
		//ATLTRACE(_T("CExtManager::AddFile %d. %s\n"), m_filecount, szFile);
	}

	void ProcessFiles() { ATLTRACE("CExtManager::processFiles\n"); }

	void ChangeExtensions(LPCTSTR szNewExt, HWND hWnd)
	{
		if (m_filecount==0) return;
		m_newext=szNewExt;


		///////////////
		IProgressDialog* pProgressDlg;
		ULONGLONG fcount=m_vFileList.size();
		HMODULE hdll = LoadLibrary(L"shell32.dll");
		HWND hw=NULL;
//>>>if (!m_bOneStepRename && (cocreate... (?don't load shell32.dll?)
		if (S_OK==::CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void**)&pProgressDlg))
		{
			pProgressDlg->SetTitle(L"Change File Extension");
			pProgressDlg->SetCancelMsg(L"Canceling...", NULL);
			CString line1;
			line1.Format(_T("Changing file extensions (%d items)"), fcount);
			pProgressDlg->SetLine(1, line1, TRUE, NULL);
			if (hdll) pProgressDlg->SetAnimation(hdll, 160);//for XP, 160 move, 161 copy
			pProgressDlg->StartProgressDialog(hWnd, NULL, PROGDLG_AUTOTIME, NULL);
		}

		//loop
		int i;
		for (i=0, m_vFileListIter=m_vFileList.begin(); m_vFileListIter<m_vFileList.end(); m_vFileListIter++, i++)
		{
			if (pProgressDlg && pProgressDlg->HasUserCancelled()) break;
			if (!ChangeExt(*m_vFileListIter, m_newext, hWnd)) break;
			if (pProgressDlg)
			{
				pProgressDlg->SetLine(2, PathFindFileName(m_vFileListIter->operator LPCTSTR()),TRUE,NULL);
				pProgressDlg->SetProgress64(i, fcount);
			}
		}
		
		////////////////
		if (pProgressDlg)
		{
			pProgressDlg->StopProgressDialog();
			pProgressDlg->Release();
		}
		if (hdll) FreeLibrary(hdll);
	}

	LPCTSTR GetNewExt() const {return m_newext;}
	BOOL IsExtEmpty() { return m_newext.IsEmpty();}

private:
	bool ChangeExt(CString &fname, LPCTSTR szNewext, HWND hWnd)
	{
		m_splitter.Split(fname);

		if (0==StrCmpI(m_splitter.ext_nodot, szNewext)) return true;//skip no-change

		SHFILEOPSTRUCT lpFileOp={0};
		lpFileOp.fFlags=FOF_ALLOWUNDO|FOF_SILENT;//undo changes with Ctrl-Z | no system progress dialog (slow)
		lpFileOp.wFunc=FO_RENAME;
		lpFileOp.hwnd=hWnd;//owner window for error msgboxes

		fname+='\0';//add buffer-terminating null
		lpFileOp.pFrom=fname;

		CString toname=m_splitter.drive + m_splitter.dir + m_splitter.filename + _T(".") + szNewext;//use path fcs?
		lpFileOp.pTo=toname;
		
		if (SHFileOperation(&lpFileOp))
		{
			MessageBeep((UINT)-1);
			ATLTRACE("SHFileOperation failed\n");
			return false;
		}
	return true;
	}

private:
	CString m_newext;
	CSplitPath m_splitter;
	UINT m_filecount;
	std::vector<CString> m_vFileList;
	std::vector<CString>::iterator m_vFileListIter;
};

#endif//_EXTMANAGER_F1E7D5E1_C883_4D0A_9975_B8D9B7798168_
