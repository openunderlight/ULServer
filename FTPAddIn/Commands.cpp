// Commands.cpp : implementation file
//

#include "stdafx.h"
#include "FTPAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

#define scriptFile "ftp-script.txt"
#define FTPDevUserName "lyra"
#define FTPDevPassword "P1nn@cles"
#define FTPDevHost "grzzt.lyrastudios.com"
#define FTPLiveUserName "lyra"
#define FTPLivePassword "Kl@m@th"
#define FTPLiveHost "asmodeus.lyrastudios.com"
#define FTPLocalBaseDirectory "/Projects/UlServer"
#define FTPScriptDirectory "\\Projects\\UlServer"
#define FTPRemoteBaseDirectory "/home/brent/lyra/src"

// these are the Win32 only projects that should not be FTP'd
// note that we do everything in lower case, since UNIX is case sensitive
const int  NUM_SHARED_PROJECTS = 15;
const static CString csSharedProjects[] = 
{
	"libsrc/db", 
	"libsrc/dbi_gdbm", 
	"libsrc/dbi_mysql", 
	"server/gamed", 
	"libsrc/gdbm", 
	"libsrc/gmsg", 
	"server/leveld", 
	"libsrc/net",
	"libsrc/private", 
	"libsrc/pth", 
	"libsrc/rmsg", 
	"libsrc/server", 
	"libsrc/shared", 
	"libsrc/smsg", 
	"libsrc/util"
};


/////////////////////////////////////////////////////////////////////////////
// CCommands

CCommands::CCommands()
{
	m_pApplication = NULL;
	m_pApplicationEventsObj = NULL;
	m_pDebuggerEventsObj = NULL;
}

CCommands::~CCommands()
{
	ASSERT (m_pApplication != NULL);
	m_pApplication->Release();
}

void CCommands::SetApplicationObject(IApplication* pApplication)
{
	// This function assumes pApplication has already been AddRef'd
	//  for us, which CDSAddIn did in its QueryInterface call
	//  just before it called us.
	m_pApplication = pApplication;
	
	// Create Application event handlers
	XApplicationEventsObj::CreateInstance(&m_pApplicationEventsObj);
	m_pApplicationEventsObj->AddRef();
	m_pApplicationEventsObj->Connect(m_pApplication);
	m_pApplicationEventsObj->m_pCommands = this;
	
	// Create Debugger event handler
	CComPtr<IDispatch> pDebugger;
	if (SUCCEEDED(m_pApplication->get_Debugger(&pDebugger)) 
		&& pDebugger != NULL)
	{
		XDebuggerEventsObj::CreateInstance(&m_pDebuggerEventsObj);
		m_pDebuggerEventsObj->AddRef();
		m_pDebuggerEventsObj->Connect(pDebugger);
		m_pDebuggerEventsObj->m_pCommands = this;
	}
}

void CCommands::UnadviseFromEvents()
{
	ASSERT (m_pApplicationEventsObj != NULL);
	m_pApplicationEventsObj->Disconnect(m_pApplication);
	m_pApplicationEventsObj->Release();
	m_pApplicationEventsObj = NULL;
	
	if (m_pDebuggerEventsObj != NULL)
	{
		// Since we were able to connect to the Debugger events, we
		//  should be able to access the Debugger object again to
		//  unadvise from its events (thus the VERIFY_OK below--see stdafx.h).
		CComPtr<IDispatch> pDebugger;
		VERIFY_OK(m_pApplication->get_Debugger(&pDebugger));
		ASSERT (pDebugger != NULL);
		m_pDebuggerEventsObj->Disconnect(pDebugger);
		m_pDebuggerEventsObj->Release();
		m_pDebuggerEventsObj = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Event handlers

// TODO: Fill out the implementation for those events you wish handle
//  Use m_pCommands->GetApplicationObject() to access the Developer
//  Studio Application object

// Application events

HRESULT CCommands::XApplicationEvents::BeforeBuildStart()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BuildFinish(long nNumErrors, long nNumWarnings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeApplicationShutDown()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentOpen(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::BeforeDocumentClose(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::DocumentSave(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewDocument(IDispatch* theDocument)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowActivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WindowDeactivate(IDispatch* theWindow)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceOpen()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::WorkspaceClose()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

HRESULT CCommands::XApplicationEvents::NewWorkspace()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}

// Debugger event

HRESULT CCommands::XDebuggerEvents::BreakpointHit(IDispatch* pBreakpoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCommands methods

// FTP active file to server
STDMETHODIMP CCommands::FTPFile() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VERIFY_OK(m_pApplication->EnableModeless(TRUE));
	
	CComPtr<IDispatch> pDispDocument;
	CComQIPtr<IGenericDocument, &IID_IGenericDocument> pDocument;
	m_pApplication->get_ActiveDocument(&pDispDocument);
	if (pDispDocument != NULL)
	{
		// find the name of the active document
		pDocument = pDispDocument;
		BSTR bszName;
		CString csSubDirName;
		CString csFileName;
		pDocument->get_FullName(&bszName);
		
		// now convert the name to ASCII, and parse out the
		// directory name below \Projects\UlServer and the
		// file name
		if (!(this->ParseFullName(bszName, csSubDirName, csFileName)))
			return S_OK;
		
//		MessageBox(NULL, csSubDirName, "Project Name", MB_OK);
//		MessageBox(NULL, csFileName, "File Name", MB_OK);
		
		// switch to proper remote directory
		_stprintf(m_FTPCommands[0], "cd  %s/%s\n", FTPRemoteBaseDirectory, csSubDirName);
		// switch to proper local directory
		_stprintf(m_FTPCommands[1], "lcd %s/%s\n", FTPLocalBaseDirectory, csSubDirName);
		// send file
		_stprintf(m_FTPCommands[2], "mput %s\n", csFileName);
		if (!FTPFiles(3, FTPDevUserName, FTPDevPassword, FTPDevHost))
		{
			TCHAR buffer[_MAX_PATH];
			sprintf(buffer, "Could not FTP file %s in project %s", csFileName, csSubDirName);
			MessageBox(NULL, buffer, "FTP Error!", MB_OK);
		}
	}
	
	return S_OK;
}

// FTP all relevant files in active project to server
STDMETHODIMP CCommands::FTPProject()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VERIFY_OK(m_pApplication->EnableModeless(TRUE));
	
	CComPtr<IDispatch> pDispProject;
	CComQIPtr<IGenericProject, &IID_IGenericProject> pProject;
	m_pApplication->get_ActiveProject(&pDispProject);
	if (pDispProject != NULL)
	{
		// find the name of the active Project
		pProject = pDispProject;
		BSTR bszName;
		CString csSubDirName;
		CString csFileName;
		pProject->get_FullName(&bszName);
		
		// now convert the name to ASCII, and parse out the
		// directory name below \Projects\UlServer and the
		// file name
		
		if (!(this->ParseFullName(bszName, csSubDirName, csFileName)))
			return S_OK; 
		
//		MessageBox(NULL, csSubDirName, "Project Name", MB_OK);
//		MessageBox(NULL, csFileName, "File Name", MB_OK);
		
		// switch to proper remote directory
		_stprintf(m_FTPCommands[0], "cd  %s/%s\n", FTPRemoteBaseDirectory, csSubDirName);
		// switch to proper local directory
		_stprintf(m_FTPCommands[1], "lcd %s/%s\n", FTPLocalBaseDirectory, csSubDirName);
		// send files
		_stprintf(m_FTPCommands[2], "mput *.cpp\n");
		_stprintf(m_FTPCommands[3], "mput *.i\n");
		_stprintf(m_FTPCommands[4], "mput *.h\n");
		
		if (!FTPFiles(5, FTPDevUserName, FTPDevPassword, FTPDevHost))
		{
			TCHAR buffer[_MAX_PATH];
			_stprintf(buffer, "Could not FTP project %s", csSubDirName);
			MessageBox(NULL, buffer, "FTP Error!", MB_OK);
		}
	}
	return S_OK;	
	
}

// FTP all relevant files to server
STDMETHODIMP CCommands::FTPAll()
{	
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VERIFY_OK(m_pApplication->EnableModeless(TRUE));

	// loop through all projects and send files for each one that is
	// on the shared list
	for (int i=0; i<NUM_SHARED_PROJECTS; i++) 
	{
		//MessageBox(NULL, csSharedProjects[i], "Project Name", MB_OK);
		// switch to proper remote directory
		_stprintf(m_FTPCommands[i*5 + 0], "cd  %s/%s\n", FTPRemoteBaseDirectory, csSharedProjects[i]);
		// switch to proper local directory
		_stprintf(m_FTPCommands[i*5 + 1], "lcd %s/%s\n", FTPLocalBaseDirectory, csSharedProjects[i]);
		// send files
		_stprintf(m_FTPCommands[i*5 + 2], "mput *.cpp\n");
		_stprintf(m_FTPCommands[i*5 + 3], "mput *.i\n");
		_stprintf(m_FTPCommands[i*5 + 4], "mput *.h\n");	
	}

	if (!FTPFiles(5*NUM_SHARED_PROJECTS, FTPDevUserName, FTPDevPassword, FTPDevHost))
	{
		MessageBox(NULL, "Could not FTP all files", "FTP Error!", MB_OK);
	}

	return S_OK;
}

bool CCommands::ParseFullName(BSTR& bszName, CString& csSubDirName, CString& csFileName)
{
	TCHAR cSourceProject[_MAX_PATH] = "UlServer";
	CString csFullName;
	int i; 
	unsigned int j;
	
	csFullName.GetBufferSetLength(SysStringByteLen(bszName));
	// store full name (including path) in fullName
	for (j=0; j<(SysStringByteLen(bszName)/2); j++)
		csFullName.SetAt(j, bszName[j]);

	csFullName.SetAt(j, '\0');
	// now reverse all the slashes
	csFullName.Replace('\\', '/'); 

	// extract the portion of the name below /UlServer
	// and break it into subdirectory and filename components

	// first chop off the path portion up to /UlServer
	i = csFullName.Find(cSourceProject);
	csSubDirName = csFullName.Mid(i + strlen(cSourceProject) + 1);
	i = csFullName.ReverseFind('/');
	csFileName = csFullName.Mid(i + 1);
	i = csSubDirName.ReverseFind('/');
	csSubDirName.SetAt(i, '\0');
	csSubDirName.MakeLower();

	// determine if we are supposed to share these files with the server
	bool bSharedProject = false;
	
	for (i=0; i<NUM_SHARED_PROJECTS; i++) 
	{
		if (0 == csSubDirName.CompareNoCase(csSharedProjects[i]))
		{
			bSharedProject = true;
			break;
		}
	}

	if (!bSharedProject) 
	{
		MessageBox(NULL, "This project does not exist on the UNIX server and should not be FTP'd", csSubDirName, MB_OK);
		return false;
	}
	
	return true;
}

bool CCommands::FTPFiles(int nNumCommands, char* username, char* password, char* host)
{
	// first, create a file with FTP commands
	if (!SetCurrentDirectory(FTPScriptDirectory))
		return false;
	
	if (!(m_pScriptFile = fopen(scriptFile, "w")))
		return false;
	
	// set up script to login to FTP server
	//_ftprintf(m_pScriptFile, "%s\n", FTPDevUserName);
	//_ftprintf(m_pScriptFile, "%s\n", FTPPassword);
	_ftprintf(m_pScriptFile, "%s\n", username);
	_ftprintf(m_pScriptFile, "%s\n", password);

	// now enumerate and execute the commands 
	for (int i=0; i<nNumCommands; i++)
	{
		//TCHAR buffer[256];
		//_stprintf(buffer, "command %i: %s", i, m_FTPCommands[i]);
		//MessageBox(NULL, buffer, "Debug", MB_OK);
		_ftprintf(m_pScriptFile, m_FTPCommands[i]);
	}
	_ftprintf(m_pScriptFile, "quit\n");
	
	fclose(m_pScriptFile);
	
	//MessageBox(NULL, "File Created", "Cool", MB_OK);
	
	// now execute the FTP script
	
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;
	
	// now launch FTP process
	memset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	TCHAR ftpCommand[256];
//	_stprintf(ftpCommand, "ftp -i -s:%s %s", scriptFile, FTPHost);
	_stprintf(ftpCommand, "ftp -i -s:%s %s", scriptFile, host);
	
	_tsystem(ftpCommand);
	
	
	if (!CreateProcess(NULL, // name of application to launchn 
		ftpCommand, // pointer to command line string
		NULL, // pointer to process security attributes 
		NULL, // pointer to thread attributes 
		FALSE, // handle inheritance flag 
		0,	// creation flags 
		NULL,  // pointer to new environment block 
		NULL,  // pointer to current directory name 
		&startup_info, // pointer to STARTUPINFO 
		&process_info // pointer to PROCESS_INFORMATION 
		))
		return false;
	
	DeleteFile(scriptFile);
	
	return true;
}

STDMETHODIMP CCommands::FTPGetFile()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	VERIFY_OK(m_pApplication->EnableModeless(TRUE));

	CComPtr<IDispatch> pDispDocument;
	CComQIPtr<IGenericDocument, &IID_IGenericDocument> pDocument;
	m_pApplication->get_ActiveDocument(&pDispDocument);
	if (pDispDocument != NULL)
	{
		// find the name of the active document
		pDocument = pDispDocument;
		BSTR bszName;
		CString csSubDirName;
		CString csFileName;
		pDocument->get_FullName(&bszName);
		
		// now convert the name to ASCII, and parse out the
		// directory name below \Projects\UlServer and the
		// file name
		if (!(this->ParseFullName(bszName, csSubDirName, csFileName)))
			return S_OK;
		
//		MessageBox(NULL, csSubDirName, "Project Name", MB_OK);
//		MessageBox(NULL, csFileName, "File Name", MB_OK);
		
		// switch to proper remote directory
		_stprintf(m_FTPCommands[0], "cd  %s/%s\n", FTPRemoteBaseDirectory, csSubDirName);
		// switch to proper local directory
		_stprintf(m_FTPCommands[1], "lcd %s/%s\n", FTPLocalBaseDirectory, csSubDirName);
		// send file
		_stprintf(m_FTPCommands[2], "mget %s\n", csFileName);
		if (!FTPFiles(3, FTPDevUserName, FTPDevPassword, FTPDevHost))
		{
			TCHAR buffer[_MAX_PATH];
			sprintf(buffer, "Could not get FTP file %s in project %s", csFileName, csSubDirName);
			MessageBox(NULL, buffer, "FTP Error!", MB_OK);
		}
	}

	return S_OK;
}

STDMETHODIMP CCommands::FTPFileLive()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	VERIFY_OK(m_pApplication->EnableModeless(TRUE));
	
	CComPtr<IDispatch> pDispDocument;
	CComQIPtr<IGenericDocument, &IID_IGenericDocument> pDocument;
	m_pApplication->get_ActiveDocument(&pDispDocument);
	if (pDispDocument != NULL)
	{
		// find the name of the active document
		pDocument = pDispDocument;
		BSTR bszName;
		CString csSubDirName;
		CString csFileName;
		pDocument->get_FullName(&bszName);
		
		// now convert the name to ASCII, and parse out the
		// directory name below \Projects\UlServer and the
		// file name
		if (!(this->ParseFullName(bszName, csSubDirName, csFileName)))
			return S_OK;
		
//		MessageBox(NULL, csSubDirName, "Project Name", MB_OK);
//		MessageBox(NULL, csFileName, "File Name", MB_OK);
		
		// switch to proper remote directory
		_stprintf(m_FTPCommands[0], "cd  %s/%s\n", FTPRemoteBaseDirectory, csSubDirName);
		// switch to proper local directory
		_stprintf(m_FTPCommands[1], "lcd %s/%s\n", FTPLocalBaseDirectory, csSubDirName);
		// send file
		_stprintf(m_FTPCommands[2], "mput %s\n", csFileName);
		if (!FTPFiles(3, FTPLiveUserName, FTPLivePassword, FTPLiveHost))
		{
			TCHAR buffer[_MAX_PATH];
			sprintf(buffer, "Could not FTP file %s in project %s", csFileName, csSubDirName);
			MessageBox(NULL, buffer, "FTP Error!", MB_OK);
		}
	}
	
	return S_OK;
}
