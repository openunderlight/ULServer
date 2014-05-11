// DSAddIn.h : header file
//

#if !defined(AFX_DSADDIN_H__896A5DFE_D2B9_49EE_9901_E14C15CE91EB__INCLUDED_)
#define AFX_DSADDIN_H__896A5DFE_D2B9_49EE_9901_E14C15CE91EB__INCLUDED_

#include "commands.h"

// {C0F4D72E-C3A6-442B-9590-D816C6C70B80}
DEFINE_GUID(CLSID_DSAddIn,
0xc0f4d72e, 0xc3a6, 0x442b, 0x95, 0x90, 0xd8, 0x16, 0xc6, 0xc7, 0xb, 0x80);

/////////////////////////////////////////////////////////////////////////////
// CDSAddIn

class CDSAddIn : 
	public IDSAddIn,
	public CComObjectRoot,
	public CComCoClass<CDSAddIn, &CLSID_DSAddIn>
{
public:
	DECLARE_REGISTRY(CDSAddIn, "FTPAddIn.DSAddIn.1",
		"FTPADDIN Developer Studio Add-in", IDS_FTPADDIN_LONGNAME,
		THREADFLAGS_BOTH)

	CDSAddIn() {}
	BEGIN_COM_MAP(CDSAddIn)
		COM_INTERFACE_ENTRY(IDSAddIn)
	END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CDSAddIn)

// IDSAddIns
public:
	STDMETHOD(OnConnection)(THIS_ IApplication* pApp, VARIANT_BOOL bFirstTime,
		long dwCookie, VARIANT_BOOL* OnConnection);
	STDMETHOD(OnDisconnection)(THIS_ VARIANT_BOOL bLastTime);

protected:
	CCommandsObj* m_pCommands;
	DWORD m_dwCookie;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSADDIN_H__896A5DFE_D2B9_49EE_9901_E14C15CE91EB__INCLUDED)
