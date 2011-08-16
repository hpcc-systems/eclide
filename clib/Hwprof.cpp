/* *************************************************************************

Source Profiler -- Copyright (C) 1995, 1996, 1997, 1998 - Harald Hoyer
-----------------------------------------------------

************************************************************************* */

#include "stdafx.h"
#include "hwprof.h"

__int64 Ticks()
{
	LARGE_INTEGER x;
	BOOL rc = ::QueryPerformanceCounter(&x);
	return x.QuadPart;
};


__int64 TicksPerSecond()
{
	LARGE_INTEGER x;
	BOOL rc = ::QueryPerformanceFrequency(&x);
	return x.QuadPart;
};


class CHWProfileEntry
{
public:
	CHWProfileEntry* Next;
	const TCHAR *const FileName;
	const int Line;
	const TCHAR *const Flag;

	__int64 LastTick;
	__int64 xTicks;
	__int64 Calls;

	CHWProfileEntry(CHWProfileEntry*x,const TCHAR* fn,int l,const TCHAR* fl)
	: Next(x), FileName(fn), Line(l), Flag(fl), xTicks(0), Calls(0)
	{};

	void Suspend() {xTicks += ::Ticks() - LastTick;};
	void Resume () {LastTick = ::Ticks();};

	void Suspend(__int64 Correction) {xTicks += ::Ticks() - LastTick - Correction;};
	void Resume (__int64 Correction) {LastTick = ::Ticks() + Correction;};
};


class CHWProfileStack
{
public:
	CHWProfileStack* Next;
	CHWProfileEntry &Top;
	CHWProfileStack(CHWProfileStack*n, CHWProfileEntry&t): Next(n), Top(t) {};
};


CHWProfile::CHWProfile()
: First(NULL)
,Current(NULL) 
{
	for(int ixxx=0; ixxx<1000; ixxx++)
	{
		Start(_T("***Calibrate***"),-1,_T("suspend time"));
		Start(_T("***Calibrate***"),-1,_T("resume time"));
		End();
		End();
	};
	Start(_T("***Calibrate***"),-1,_T("calibration"));
	End();
};


void CHWProfile::Start(const TCHAR* FileName,int Line, const TCHAR* Flag)
{       
	static __int64 SuspendTime = 0;
	static __int64 ResumeTime = 0;
	
	if(Current)
		Current->Top.Suspend(SuspendTime);

	CHWProfileEntry* NewEntry = First;
	for( ;
		NewEntry && (_tcscmp(FileName,NewEntry->FileName) || Flag != NewEntry->Flag);
		NewEntry = NewEntry->Next
	);

	if(NewEntry==0)
	{
		First = new CHWProfileEntry(First, FileName,Line, Flag);
		NewEntry = First;
	};

	Current = new CHWProfileStack(Current,*NewEntry);
    
    // Obtain overhead of sample
	if(Line == -1 && Flag && *Flag == _T('c'))
	{
		for(CHWProfileEntry* Entry = First;Entry;Entry = Entry->Next)
		{
			if( Entry->Line == -1 )
			{
				switch(*Entry->Flag)
				{
					case _T('s'):
						SuspendTime = Entry->xTicks / Entry->Calls;  
						break;
					case _T('r'):
						ResumeTime = Entry->xTicks / Entry->Calls;
						break;
				};
			};
		};  
		SuspendTime -= ResumeTime;
	};	
	
	NewEntry->Calls++;
	NewEntry->Resume(ResumeTime);
};


void CHWProfile::End()
{
	if(Current)
	{
		Current->Top.Suspend();
		CHWProfileStack* ccc = Current;
		Current = ccc->Next;
		delete ccc;
		if(Current) Current->Top.Resume();
	};
};


void CHWProfile::reset()
{
	while (Current) End();

	while(First)
	{
		CHWProfileEntry*eee = First;
		First = First->Next;
		delete eee;
	};
};


void dumpprint(const TCHAR* Text)
{
	::OutputDebugString(Text);
};

            
int dumpprint(__int64 x)
{
	TCHAR Text[100];
	_ltot(int(x),Text,10);
	dumpprint(Text);
	return _tcslen(Text);
};

            
void dumpprint(__int64 x, int Scale, const TCHAR* xText)
{
	TCHAR Text[100];
	_ltot(int(x),Text+1,10);

	int Dot=1;
	for(; Text[Dot]; Dot++);
	Dot -= Scale;
	Dot--;
	for(int i=0; i < Dot; i++)
		Text[i] = Text[i+1];
	if(Scale > 0) Text[Dot] = _T('.');	
	else Text[Dot] = 0;
	dumpprint(Text);
	dumpprint(xText);
};

            
void dumpprinttime(__int64 x, __int64 q)
{       
	double xx = double(x)/(double(TicksPerSecond())*double(q)*3600.0);
	if(xx > 10.0)
	{
		dumpprint(int(xx),0,_T("h"));
		return;
	};

	if(xx >= 1.0)
	{
		int h = int(xx);
		dumpprint(h);
		dumpprint(_T(":"));
		xx-=h;
		xx*=60;
		int m = int(xx);
		if(m<10) dumpprint(_T("0"));
		dumpprint(m);
		return;
	};

	xx*=60;
	if(xx >= 1.0)
	{
		dumpprint(_T(":"));
		int m = int(xx);
		if(m<10) dumpprint(_T("0"));
		dumpprint(m);
		dumpprint(_T(":"));
		xx-=m;
		xx*=60;
		int s = int(xx);
		if(s<10) dumpprint(_T("0"));
		dumpprint(s);
		return;
	};

	xx*=600;

	TCHAR* Unit[6] = {_T("s"),_T("ms"),_T("µs"),_T("ns"),_T("ps"),_T("fs")};
	for(int i=1;i<18;i++)
	{
		if(xx >= 100.0)
		{
			dumpprint(int(xx),i%3,Unit[i/3]);
			return;
		};
		xx*=10;
	};
	dumpprint(_T("0"));
};

            
void CHWProfile::dumpprint(int Hide)const
{
	if(First == NULL)
	{
		::dumpprint(_T("\n\r=========== Profile empty ============\n\r"));
		return;
	}
	
	::dumpprint(_T("\n\r=========== Profile ==================\n\r"));

	__int64 All = 0;
	int Count = 0;
	CHWProfileEntry* That;
	for(That = First; That; That = That->Next)
	{              
		if(That->Line >= 0)
		{
			Count++;
    		All += That->xTicks;
		};
    }

	CHWProfileEntry** Map = new CHWProfileEntry*[Count];

	Count = 0;	
	for(That = First; That; That = That->Next)
	{         
		if(That->Line >= 0)
		{
			int i=0;
			for (; i<Count && Map[i]->xTicks > That->xTicks; i++);
			for (int j=Count; j>i; j--)
				Map[j] = Map[j-1];
			Map[i] = That;
			Count++;
		};
	};

	__int64 ShowTime = All;
	__int64 TimeNotShown = All;
	int ShowCount = Count;
	if(Hide>0)
		ShowTime -= All/Hide;
	else if(-Hide > 0 && -Hide < Count)
		ShowCount = -Hide;
	int LFileName = 0;

	int i = 0;
	for(; i < ShowCount && ShowTime > 0; i++)
	{
		That = Map[i];
		__int64 xTicks = That->xTicks;
		if(xTicks < 0) xTicks = 0;
		ShowTime -= xTicks;
		TimeNotShown -= xTicks;
		int iLFileName = _tcslen(That->FileName);

		if(LFileName < iLFileName) 
			LFileName = iLFileName;
	};
	ShowCount = i;
	
	for(i = 0; i < ShowCount; i++)
	{
		That = Map[i];
		__int64 xTicks = That->xTicks;
		if(xTicks < 0) xTicks = 0;
		::dumpprint(That->FileName);
		::dumpprint(_T("("));
		int LLine = ::dumpprint(That->Line);
		::dumpprint(_T("):"));
		
		int Spaces = LFileName + 6 - _tcslen(That->FileName) - LLine;
		while(Spaces > 0)
		{
			::dumpprint(_T(" "));
			--Spaces;
		};

		::dumpprint(i);
		::dumpprint(_T(":\t"));
		int LCalls = ::dumpprint(That->Calls);
		::dumpprint(_T("x"));
		while(LCalls++ < 6)	::dumpprint(_T(" "));
		::dumpprint(_T("\t"));
		::dumpprinttime(xTicks, That->Calls);
		::dumpprint(_T(" \t"));
		::dumpprinttime(xTicks, 1);
		::dumpprint(_T(" \t"));
		::dumpprint(__int64(100.0*float(xTicks) / float(All)));
		::dumpprint(_T("%\t"));
		::dumpprint(That->Flag);
		::dumpprint(_T("\n\r"));
	};

	delete Map;

	::dumpprint(_T("Total: \t"));
	::dumpprinttime(All, 1);
	
	if(i < Count) 
	{
		::dumpprint(_T(" ("));
		::dumpprint(Count-i);
		::dumpprint(_T(" not-shown-items "));
		::dumpprinttime(TimeNotShown,1);
		::dumpprint(_T(")"));
	};

	::dumpprint(_T("\n\r======================================\n\r"));
	
};






