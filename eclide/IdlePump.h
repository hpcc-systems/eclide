#pragma once

class CTabClient : public CIdleHandler
{
public:
	virtual HWND getHwnd() = 0;
	virtual bool isModified() = 0;
};

class CIdleHandlerWithPump : public CIdleHandler
{
// Operations
public:

	void PumpIdleMessages() {
		
		// pump idle messages to update window
		CMessageLoop * pLoop = _Module.GetMessageLoop ();
		ATLASSERT(NULL != pLoop);		
		pLoop->OnIdle(0);
	}
};

