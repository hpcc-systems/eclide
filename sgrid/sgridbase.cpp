#include "stdafx.h"
#include "sgrid.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CSGridApp : public CWinApp
{
public:
	CSGridApp()
	{
	}
	virtual BOOL InitInstance()
	{
		CWinApp::InitInstance();
		SGRID::Register(AfxGetInstanceHandle());
		return TRUE;
	}
	virtual int CSGridApp::ExitInstance()
	{
		SGRID::Unregister();
		return CWinApp::ExitInstance();
	}

	DECLARE_MESSAGE_MAP()
} theApp;

BEGIN_MESSAGE_MAP(CSGridApp, CWinApp)
END_MESSAGE_MAP()
