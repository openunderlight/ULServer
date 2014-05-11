// stdafx.cpp : source file that includes just the standard includes
//  stdafx.pch will be the pre-compiled header
//  stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#include <statreg.cpp>
#endif

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include "LyraDefs.h"
#include <atlimpl.cpp>

#include "LmGlobalDB.h"
#include "LmUtil.h"
#include "LmServerDBC.h"
#include "LmNew.h"

extern TCHAR server_working_directory[_MAX_PATH];
extern CServiceModule _Module;

struct  childInfo_t {
	HANDLE hProcess;
	int	   nIndex;
};

bool CServiceModule::SetWorkingDirectory(void)
{
	// change to a working directory
	TCHAR buffer[_MAX_PATH];
	memset(server_working_directory,0,sizeof(server_working_directory));
	HKEY reg_key = NULL;
	unsigned long result,size;
	DWORD reg_type;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE, UL_SERVER_REGISTRY_KEY,0, 
		NULL,0,KEY_ALL_ACCESS, NULL, &reg_key, &result);
	if (result == REG_OPENED_EXISTING_KEY)
	{
		size = sizeof(server_working_directory);
		RegQueryValueEx(reg_key, "server_working_directory", NULL, &reg_type,
			(unsigned char *)buffer, &size);
		RegCloseKey(reg_key);
#ifdef _DEBUG
		_stprintf(server_working_directory, "%s\\Bin\\Debug", buffer);
#else
		_stprintf(server_working_directory, "%s\\Bin\\Release", buffer);
#endif
	}
	else
		return false;

	_tcscpy(m_szUlRoot, buffer);
	
	if(_chdir(server_working_directory))
		return false;

	_stprintf(buffer, "Working directory set to %s", server_working_directory);
    LogEvent(_T(buffer));

	return true;
	
}


// we need a non-class wrapper function with a specific prototype
// here to use as a thread launching function
void __cdecl MonitorSingleServer(void* param)
{
	int nIndex = *((int*)param);
	_Module.MonitorSingleServer(nIndex);
}

void CServiceModule::MonitorSingleServer(int nIndex)
{
	int num_deaths = 0;
	HANDLE hProcess = m_pi[nIndex].hProcess;

    // Wait until child process exits.
	while (num_deaths < 9999)
	{
		WaitForSingleObject(hProcess, INFINITE );
		num_deaths++;
		Sleep(5000);
		TCHAR buffer[256];
		_stprintf(buffer, "Restarting server %d after exit; total deaths = %d\n", nIndex, num_deaths);
		_Module.LogEvent(_T(buffer));
		hProcess = SpawnServer(nIndex, serverdbc_->ServerType(nIndex));
	}
}

bool CServiceModule::StartServers(int nType)
{
  int nNumServers = serverdbc_->NumServers();
  for (int i = 0; i < nNumServers; ++i) {
    // right server type?
    if (nType != serverdbc_->ServerType(i))
      continue;

	// note: Windows version does not check host ID because it 
	// assumes everything is running on one machine

    // arg1 = 0?
    if (serverdbc_->Arg1(i) == 0) {
      continue;
    }

	if (!SpawnServer(i, nType))
		return false;

	// note the use of the class member array m_indexes - this is because we're
	// spawning a new thread, and want to be sure it gets a pointer to the correct ID as 
	// the parameter

	unsigned long thread_id = _beginthread(::MonitorSingleServer, 0, (void*)(&(m_indexes[i])));

  }
 
  return true;

}

HANDLE CServiceModule::SpawnServer(int nIndex, int nType)
{
	CloseHandle(m_pi[nIndex].hProcess);
	CloseHandle(m_pi[nIndex].hThread);

    LogEvent(_T("Attempting to spawn child process"));
	
	// put integer args into strings
	TCHAR arg1[32], arg2[32], arg3[32];
	_stprintf(arg1, "%d", serverdbc_->Arg1(nIndex));
	_stprintf(arg2, "%d", serverdbc_->Arg2(nIndex));
	_stprintf(arg3, "\"%s\"", serverdbc_->HostName(nIndex));
	
	TCHAR EXEName[256];
	switch (nType) 
	{
	case LmServerDBC::ST_LEVEL:
			_stprintf(EXEName, "LevelD %s %s %s %s", m_szUlRoot, arg1, arg2, arg3);
			break;
	case LmServerDBC::ST_GAME:
			_stprintf(EXEName, "GameD %s %s %s %s", m_szUlRoot, arg1, arg2, arg3);
			break;
	default: 
		MessageBox(NULL, "Illegal server type", "Error", MB_OK);
		return NULL;
	}
	//_stprintf(EXEName, "TestApp");

//	MessageBox(NULL, EXEName, "args", MB_OK);
	
	/*
	switch (nType) 
	{
	case LmServerDBC::ST_LEVEL: 
		sprintf(EXEName, "leveld %s %s %s", arg1, arg2, arg3);
		break;
	case LmServerDBC::ST_GAME:
		_tcscpy(EXEName, "gamed %s %s %s", arg1, arg2, arg3);
		break;
	default:
		LogEvent(_T("Unknown server type - not gamed or leved - can not start"));
		return false;
	}
*/		
    // Start the child process. 
    if( !CreateProcess( NULL, // No module name (use command line). 
		EXEName,
        NULL,             // Process handle not inheritable. 
        NULL,             // Thread handle not inheritable. 
        FALSE,            // Set handle inheritance to FALSE. 
        0,                // No creation flags. 
        NULL,             // Use parent's environment block. 
        server_working_directory,             
        &m_si,              // Pointer to STARTUPINFO structure.
        &(m_pi[nIndex]))    // Pointer to PROCESS_INFORMATION structure.
    ) 
    {
		LogEvent(_T("Could not create Underlight server process!\n"));
		return NULL;
    }
	else
		return m_pi[nIndex].hProcess;
}



bool CServiceModule::Initialize()
{
  globaldb_ = LmNEW(LmGlobalDB(m_szUlRoot));
  serverdbc_ = LmNEW(LmServerDBC());

  for (int i=0; i<MAX_UL_SERVERS; i++)
	  m_indexes[i] = i;

  // load passwords

  TCHAR pw_file[FILENAME_MAX];
  globaldb_->GetPasswordFile(pw_file);
  serverdbc_->LoadPasswords(pw_file);
  serverdbc_->Connect();
  serverdbc_->Load();
  serverdbc_->Disconnect();

  return true;

}

void CServiceModule::StopServers() 
{
	unsigned long recipients = BSM_APPLICATIONS;
		// shutdown code
	BroadcastSystemMessage(BSF_IGNORECURRENTTASK, // do not send message to this process
							&recipients,      // broadcast only to applications
							m_wm_shutdown_servers,       // message registered in previous step
							0, 0);
}


