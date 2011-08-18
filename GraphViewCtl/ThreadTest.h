#pragma once


class ThreadTest
{
	DWORD m_id;
public:
	ThreadTest() : m_id(0) {}
	void TTInit() { m_id=::GetCurrentThreadId(); }
	void TTest() 
	{ 
		ATLASSERT(::GetCurrentThreadId()==m_id);
	}
};