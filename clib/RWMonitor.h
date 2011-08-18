#if !defined(AFX_RWMONITOR_H__20000928_3D27_0EF6_A123_0080AD509054__INCLUDED_)
#define AFX_RWMONITOR_H__20000928_3D27_0EF6_A123_0080AD509054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRWMonitor
{
private:
	LONG m_nReaders;
	LONG m_nWriters;

public:
	CRWMonitor()
	{
		m_nReaders = 0;
		m_nWriters = 0;
	}

	void LockR()
	{
		for(;;)
		{
			::InterlockedIncrement(&m_nReaders);
			if(m_nWriters == 0) 
				break;
			::InterlockedDecrement(&m_nReaders);
			::Sleep(0);
		}
	}

	void LockW()
	{
		for(;;) 
		{
			if( ::InterlockedExchange( &m_nWriters, 1 ) == 1 ) 
				::Sleep(0);
			else 
			{
				while(m_nReaders != 0) 
					::Sleep(0);
				break;
			}
		}
	}

	void UnlockR()
	{
		::InterlockedDecrement(&m_nReaders);
	}

	void UnlockW()
	{
		::InterlockedDecrement(&m_nWriters);
	}
};


#endif // !defined(AFX_RWMONITOR_H__20000928_3D27_0EF6_A123_0080AD509054__INCLUDED_)

