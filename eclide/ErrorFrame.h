#pragma once

#include "WTLDockablePane.h"
#include "OutputView.h"

class CErrorFrame :	public CWtlDockablePane<CErrorView>

{
	typedef CWtlDockablePane<CErrorView> baseClass;
	typedef CErrorFrame thisClass;

public:
	CErrorFrame();
	void Send_Clear();
	void Send_Reset();
	void Post_Reset();
};


