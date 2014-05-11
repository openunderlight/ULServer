// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__971CF7D6_65CB_4124_8E93_A65480122444__INCLUDED_)
#define AFX_STDAFX_H__971CF7D6_65CB_4124_8E93_A65480122444__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

const int MAX_UL_SERVERS = 250; // max # of processes to watch

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

#include "LyraDefs.h"
#include "LmServerDBC.h"


class LmGlobalDB;


class CServiceModule : public CComModule
{
public:
	HRESULT RegisterServer(BOOL bRegTypeLib, BOOL bService);
	HRESULT UnregisterServer();
	void Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid = NULL);
    void Start();
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    void Handler(DWORD dwOpcode);
    void Run();
    BOOL IsInstalled();
    BOOL Install();
    BOOL Uninstall();
	LONG Unlock();
	void LogEvent(LPCTSTR pszFormat, ...);
    void SetServiceStatus(DWORD dwState);
    void SetupAsLocalServer();

//Implementation
private:
	static void WINAPI _ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
    static void WINAPI _Handler(DWORD dwOpcode);

// data members
public:
	void StopServers(void);
	bool Initialize();
	bool SetWorkingDirectory(void);
	void MonitorSingleServer(int nIndex);
	bool StartServers(int nType);
	HANDLE SpawnServer(int nIndex, int nType);
	PROCESS_INFORMATION m_pi[MAX_UL_SERVERS];
	int m_indexes[MAX_UL_SERVERS];
	STARTUPINFO m_si;
	DWORD m_wm_shutdown_servers;


	LmGlobalDB* globaldb_;  // global database
    LmServerDBC* serverdbc_;  // server info database

	TCHAR m_szUlRoot[256];
    TCHAR m_szServiceName[256];
    SERVICE_STATUS_HANDLE m_hServiceStatus;
    SERVICE_STATUS m_status;
	DWORD dwThreadID;
	BOOL m_bService;
};

void __cdecl MonitorSingleServer (void* param);

extern CServiceModule _Module;
#include <atlcom.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__971CF7D6_65CB_4124_8E93_A65480122444__INCLUDED)
