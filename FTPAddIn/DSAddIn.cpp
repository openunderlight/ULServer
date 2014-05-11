// AddInMod.cpp : implementation file
//

#include "stdafx.h"
#include "FTPAddIn.h"
#include "DSAddIn.h"
#include "Commands.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static TCHAR THIS_FILE[] = __FILE__;
#endif

// This is called when the user first loads the add-in, and on start-up
//  of each subsequent Developer Studio session
STDMETHODIMP CDSAddIn::OnConnection(IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	// Store info passed to us
	IApplication* pApplication = NULL;
	if (FAILED(pApp->QueryInterface(IID_IApplication, (void**) &pApplication))
		|| pApplication == NULL)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	m_dwCookie = dwCookie;

	// Create command dispatch, send info back to DevStudio
	CCommandsObj::CreateInstance(&m_pCommands);
	m_pCommands->AddRef();

	// The QueryInterface above AddRef'd the Application object.  It will
	//  be Release'd in CCommand's destructor.
	m_pCommands->SetApplicationObject(pApplication);

	// (see stdafx.h for the definition of VERIFY_OK)

	VERIFY_OK(pApplication->SetAddInInfo((long) AfxGetInstanceHandle(),
		(LPDISPATCH) m_pCommands, IDR_TOOLBAR_MEDIUM, IDR_TOOLBAR_LARGE, m_dwCookie));

	// Inform DevStudio of the commands we implement

	// The command name should not be localized to other languages.  The 
	//  tooltip, command description, and other strings related to this
	//  command are stored in the string table (IDS_CMD_STRING) and should
	//  be localized.

	// add command to FTP active file

	LPCTSTR szCommand1 = _T("FTP File");
	VARIANT_BOOL bRet;
	CString strCmdString;
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand1 + strCmdString;
	CComBSTR bszCmdString1(strCmdString);
	CComBSTR bszMethod1(_T("FTPFile"));
	CComBSTR bszCmdName1(szCommand1);
	VERIFY_OK(pApplication->AddCommand(bszCmdString1, bszMethod1, 0, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		// AddCommand failed because a command with this name already
		//  exists.  You may try adding your command under a different name.
		//  Or, you can fail to load as we will do here.
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	// Add toolbar buttons only if this is the first time the add-in
	//  is being loaded.  Toolbar buttons are automatically remembered
	//  by Developer Studio from session to session, so we should only
	//  add the toolbar buttons once.
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, bszCmdName1, m_dwCookie));
	}

	LPCTSTR szCommand2 = _T("FTP Project");
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand2 + strCmdString;
	CComBSTR bszCmdString2(strCmdString);
	CComBSTR bszMethod2(_T("FTPProject"));
	CComBSTR bszCmdName2(szCommand2);
	VERIFY_OK(pApplication->AddCommand(bszCmdString2, bszMethod2, 1, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}


	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, bszCmdName2, m_dwCookie));
	}

	LPCTSTR szCommand3 = _T("FTP All");
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand3 + strCmdString;
	CComBSTR bszCmdString3(strCmdString);
	CComBSTR bszMethod3(_T("FTPAll"));
	CComBSTR bszCmdName3(szCommand3);
	VERIFY_OK(pApplication->AddCommand(bszCmdString3, bszMethod3, 2, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, bszCmdName3, m_dwCookie));
	}

	LPCTSTR szCommand4 = _T("FTP Get File");
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand4 + strCmdString;
	CComBSTR bszCmdString4(strCmdString);
	CComBSTR bszMethod4(_T("FTPGetFile"));
	CComBSTR bszCmdName4(szCommand4);
	VERIFY_OK(pApplication->AddCommand(bszCmdString4, bszMethod4, 3, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, bszCmdName4, m_dwCookie));
	}

	LPCTSTR szCommand5 = _T("FTP Live");
	strCmdString.LoadString(IDS_CMD_STRING);
	strCmdString = szCommand5 + strCmdString;
	CComBSTR bszCmdString5(strCmdString);
	CComBSTR bszMethod5(_T("FTPFileLive"));
	CComBSTR bszCmdName5(szCommand5);
	VERIFY_OK(pApplication->AddCommand(bszCmdString5, bszMethod5, 4, m_dwCookie, &bRet));
	if (bRet == VARIANT_FALSE)
	{
		*OnConnection = VARIANT_FALSE;
		return S_OK;
	}

	
	if (bFirstTime == VARIANT_TRUE)
	{
		VERIFY_OK(pApplication->
			AddCommandBarButton(dsGlyph, bszCmdName5, m_dwCookie));
	}


	*OnConnection = VARIANT_TRUE;
	return S_OK;
}

// This is called on shut-down, and also when the user unloads the add-in
STDMETHODIMP CDSAddIn::OnDisconnection(VARIANT_BOOL bLastTime)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_pCommands->UnadviseFromEvents();
	m_pCommands->Release();
	m_pCommands = NULL;

	// TODO: Perform any cleanup work here

	return S_OK;
}
