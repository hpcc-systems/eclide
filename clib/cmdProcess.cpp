#include "StdAfx.h"
#include "CmdProcess.h"
#include "Util.h"
#include "Logger.h"

using namespace std;

#define NEW_PROCESS
#ifdef NEW_PROCESS
class CProcess
{
protected:
	HANDLE hChildProcess;
	//HANDLE hStdIn; // Handle to parents std input.
	HANDLE hInputWrite, hOutputRead, hErrorRead;
	BOOL bRunThread;

	const std::wstring & m_in;

public:
	std::wstring & m_out;
	std::wstring & m_err;

	CProcess(const std::wstring & in, std::wstring & out, std::wstring & err) : m_in(in), m_out(out), m_err(err)
	{
		hChildProcess = NULL;
		//hStdIn = NULL; 
		hInputWrite = NULL;
		bRunThread = TRUE;
	}

	int Run(const std::_tstring &command, const std::_tstring &directory)
	{
		HANDLE hOutputReadTmp,hOutputWrite;
		HANDLE hInputWriteTmp,hInputRead;
		HANDLE hErrorReadTmp,hErrorWrite;
		HANDLE hThread, hThreadErr, hThreadOut;
		DWORD ThreadId, ThreadErrId, ThreadOutId;
		SECURITY_ATTRIBUTES sa;

		// Set up the security attributes struct.
		sa.nLength= sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		OSVERSIONINFO osv;
		::ZeroMemory(&osv, sizeof(OSVERSIONINFO));
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		::GetVersionEx(&osv);
		bool windows95 = osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;

		SECURITY_DESCRIPTOR sd;
		// If NT make a real security thing to allow inheriting handles
		if (!windows95) 
		{
			::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = &sd;
		}


		// Create the child output pipe.
		if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
			DisplayError(_T("CreatePipe"));


		// Create the child error pipe.
		if (!CreatePipe(&hErrorReadTmp,&hErrorWrite,&sa,0))
			DisplayError(_T("CreatePipe"));


		// Create the child input pipe.
		if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0))
			DisplayError(_T("CreatePipe"));


		// Create new output read handle and the input write handles. Set
		// the Properties to FALSE. Otherwise, the child inherits the
		// properties and, as a result, non-closeable handles to the pipes
		// are created.
		if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
			GetCurrentProcess(),
			&hOutputRead, // Address of new handle.
			0,FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS))
			DisplayError(_T("DupliateHandle"));

		if (!DuplicateHandle(GetCurrentProcess(),hErrorReadTmp,
			GetCurrentProcess(),
			&hErrorRead, // Address of new handle.
			0,FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS))
			DisplayError(_T("DupliateHandle"));

		if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
			GetCurrentProcess(),
			&hInputWrite, // Address of new handle.
			0,FALSE, // Make it uninheritable.
			DUPLICATE_SAME_ACCESS))
			DisplayError(_T("DupliateHandle"));


		// Close inheritable copies of the handles you do not want to be
		// inherited.
		if (!CloseHandle(hOutputReadTmp)) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hErrorReadTmp)) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hInputWriteTmp)) DisplayError(_T("CloseHandle"));


		// Get std input handle so you can close it and force the ReadFile to
		// fail when you want the input thread to exit.
		//if ( (hStdIn = GetStdHandle(STD_INPUT_HANDLE)) ==
		//	INVALID_HANDLE_VALUE )
		//	DisplayError(_T("GetStdHandle"));

		//::SetHandleInformation(hOutputWrite, HANDLE_FLAG_INHERIT, 0);
		//::SetHandleInformation(hInputRead, HANDLE_FLAG_INHERIT, 0);
		//::SetHandleInformation(hErrorWrite, HANDLE_FLAG_INHERIT, 0);
		PrepAndLaunchRedirectedChild(hOutputWrite,hInputRead,hErrorWrite, command, directory);

		// Close pipe handles (do not continue to modify the parent).
		// You need to make sure that no handles to the write end of the
		// output pipe are maintained in this process or else the pipe will
		// not close when the child process exits and the ReadFile will hang.
		if (!CloseHandle(hOutputWrite)) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hInputRead )) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hErrorWrite)) DisplayError(_T("CloseHandle"));


		// Launch the thread that gets the input and sends it to the child.
		hThread = CreateThread(NULL, 0, GetAndSendInputThread, (LPVOID)this, 0, &ThreadId);
		if (hThread == NULL) DisplayError(_T("CreateThread"));


		// Read the child's output.
		hThreadOut = CreateThread(NULL, 0, ReadAndHandleOutput, (LPVOID)this, 0, &ThreadOutId);//(hOutputRead, m_out);
		if (hThreadOut == NULL) DisplayError(_T("CreateThreadOut"));

		hThreadErr = CreateThread(NULL, 0, ReadAndHandleError, (LPVOID)this, 0, &ThreadErrId);//(hErrorRead, m_err)
		if (hThreadErr == NULL) DisplayError(_T("CreateThreadOut"));
		// Redirection is complete


		// Force the read on the input to return by closing the stdin handle.
		//if (!CloseHandle(hStdIn)) DisplayError(_T("CloseHandle"));

		// Tell the thread to exit and wait for thread to die.
		bRunThread = FALSE;

		if (WaitForSingleObject(hThread,INFINITE) == WAIT_FAILED)
			DisplayError(_T("WaitForSingleObject"));
		if (WaitForSingleObject(hThreadOut,INFINITE) == WAIT_FAILED)
			DisplayError(_T("WaitForSingleObject"));
		if (WaitForSingleObject(hThreadErr,INFINITE) == WAIT_FAILED)
			DisplayError(_T("WaitForSingleObject"));

		if (!CloseHandle(hOutputRead)) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hErrorRead)) DisplayError(_T("CloseHandle"));
		if (!CloseHandle(hInputWrite)) DisplayError(_T("CloseHandle"));
		return 0;
	}

	void PrepAndLaunchRedirectedChild(HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr, const std::_tstring & command, const std::_tstring & directory)
	{
		PROCESS_INFORMATION pi;
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		STARTUPINFO si;

		// Set up the start up info struct.
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.wShowWindow = SW_HIDE;
		si.hStdOutput = hChildStdOut;
		si.hStdInput  = hChildStdIn;
		si.hStdError  = hChildStdErr;
		// Use this if you want to hide the child:
		//     si.wShowWindow = SW_HIDE;
		// Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
		// use the wShowWindow flags.

		//std::wstring command = _T("\"C:\\PROGRAM FILES (X86)\\LEXISNEXIS\\HPCC\\BIN\\eclcc.exe\" -I\"c:\\temp\\rep2\" \"c:\\temp\\rep2\\builder001.ecl\"");
		//std::wstring command = _T("\"C:\\PROGRAM FILES (X86)\\LEXISNEXIS\\HPCC\\BIN\\eclcc.exe\"");
		//std::wstring startDirectory = _T("c:\\temp\\rep2\\");

		// Launch the process that you want to redirect (in this case,
		// Child.exe). Make sure Child.exe is in the same directory as
		// redirect.c launch redirect from a command line to prevent location
		// confusion.
		if (!CreateProcess(NULL,
			(TCHAR *)command.c_str(),
			NULL, NULL,
			TRUE, CREATE_NEW_PROCESS_GROUP,
			NULL,
			directory.c_str(),
			&si,&pi))
			DisplayError(_T("CreateProcess"));


		// Set global child process handle to cause threads to exit.
		hChildProcess = pi.hProcess;


		// Close any unnecessary handles.
		if (!CloseHandle(pi.hThread)) DisplayError(_T("CloseHandle"));
	}

	void DisplayError(TCHAR *pszAPI)
	{
		LPVOID lpvMessageBuffer;
		TCHAR szPrintBuffer[512];
		DWORD nCharsWritten;

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpvMessageBuffer, 0, NULL);

		wsprintf(szPrintBuffer,_T("ERROR: API    = %s.\n   error code = %d.\n   message    = %s.\n"), pszAPI, GetLastError(), (TCHAR *)lpvMessageBuffer);

		_DBGLOG(LEVEL_WARNING, szPrintBuffer);
//		WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE),szPrintBuffer, lstrlen(szPrintBuffer),&nCharsWritten,NULL);

		LocalFree(lpvMessageBuffer);
		//ExitProcess(GetLastError());
	}

	static DWORD WINAPI ReadAndHandleOutput(LPVOID lpvThreadParam)
	{
		CProcess * self = (CProcess *)lpvThreadParam;
		char lpBuffer[256];
		DWORD nBytesRead;
		std::string buff;

		while(TRUE)
		{
			if (!ReadFile(self->hOutputRead,lpBuffer,sizeof(lpBuffer),&nBytesRead,NULL) || !nBytesRead)
			{
				if (GetLastError() == ERROR_BROKEN_PIPE)
					break; // pipe done - normal exit path.
				else
					self->DisplayError(_T("ReadFile")); // Something bad happened.
			}

			buff.append(lpBuffer, nBytesRead);
		}
		self->m_out = CA2T(buff.c_str(), CP_UTF8);
		return 1;
	}

	static DWORD WINAPI ReadAndHandleError(LPVOID lpvThreadParam)
	{
		CProcess * self = (CProcess *)lpvThreadParam;
		char lpBuffer[256];
		DWORD nBytesRead;
		std::string buff;

		while(TRUE)
		{
			if (!ReadFile(self->hErrorRead,lpBuffer,sizeof(lpBuffer),&nBytesRead,NULL) || !nBytesRead)
			{
				if (GetLastError() == ERROR_BROKEN_PIPE)
					break; // pipe done - normal exit path.
				else
					self->DisplayError(_T("ReadFile")); // Something bad happened.
			}

			buff.append(lpBuffer, nBytesRead);
		}
		self->m_err = CA2T(buff.c_str(), CP_UTF8);
		return 1;
	}

	static DWORD WINAPI GetAndSendInputThread(LPVOID lpvThreadParam)
	{
		DWORD nBytesWrote;
		CProcess * self = (CProcess *)lpvThreadParam;
		std::string in = CT2A(self->m_in.c_str(), CP_UTF8);

		// Get input from our console and send it to child through the pipe.
		while (!in.empty() && self->bRunThread)
		{
			if (!WriteFile(self->hInputWrite, in.c_str(), in.length(), &nBytesWrote,NULL))
			{
				if (GetLastError() == ERROR_NO_DATA)
					break; // Pipe was closed (normal exit path).
				else
					self->DisplayError(_T("WriteFile"));
			}
		}

		return 1;
	}
};

int CLIB_API runProcess(const std::_tstring &command, const std::_tstring &directory, const std::_tstring &_path, const std::_tstring &in, std::_tstring &out, std::_tstring &err)
{
		if 	((GetKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			std::_tstring clippy = _T("\r\ncd ");
			clippy += directory + _T("\r\n");
			clippy += command + _T("\r\n");
			_DBGLOG(LEVEL_INFO, clippy.c_str());
			SetClipboard(clippy);
		}
//#endif

	std::_tstring envPath;
	if (!_path.empty())
	{
		std::_tstring path = _path + _T(";") + _path + _T("\\plugins");
		envPath = _tgetenv(_T("PATH")) == NULL ? _T("") : _tgetenv(_T("PATH"));
		if (!envPath.empty())
		{
			std::_tstring envVar = _T("PATH=");
			envVar += path;
			envVar += _T(";");
			envVar += envPath;
			_tputenv(envVar.c_str());
		}
	}
		
#ifndef _DEBUG	//Let debugger react to Exception
	try
	{
#endif
		CProcess proc(in, out, err);
		int retVal = proc.Run(command, directory);

		if (!envPath.empty())
			_tputenv(envPath.c_str());

		return retVal;
#ifndef _DEBUG
	}
	catch(...)
	{
		_DBGLOG(LEVEL_SEVERE, (boost::_tformat(_T("GPF - %1%")) % command.c_str()).str().c_str());
		ATLASSERT(false);
	}
#endif
	return -1;
}

#else
enum JobSubsystem {
	jobCLI = 0, jobGUI = 1, jobShell = 2, jobExtension = 3, jobHelp = 4, jobOtherHelp = 5
};

enum JobFlags {
	jobForceQueue=1, jobHasInput=2, jobQuiet=4, // 8 reserved for jobVeryQuiet
	jobRepSelMask=48, jobRepSelYes=16, jobRepSelAuto=32 
};

class Job {
public:
	std::_tstring command;
	std::_tstring directory;
	std::string input;
	JobSubsystem jobType;
	int flags;

	Job()
	{
		jobType = jobCLI;
		flags = 0;
	}
};

class CProcess : public CUnknown, public boost::noncopyable
{
private:
	long m_cancelFlag;
	HANDLE hWriteSubProcess;
	bool m_hide;
	DWORD m_subProcessGroupId;
	std::_tstring * m_out;
	std::_tstring * m_err;
	bool m_windows95;

public:
	IMPLEMENT_CUNKNOWN;
	CProcess()
	{
		ATLASSERT(false);
		m_cancelFlag = 0;
		m_hide = false;
		m_out = NULL;
		m_err = NULL;
		m_windows95 = false;
	}
	CProcess(std::_tstring * out, std::_tstring * err) : m_out(out), m_err(err)
	{
		m_cancelFlag = 0;
		m_hide = false;
		m_windows95 = false;
	}

	virtual ~CProcess()
	{
	}

	void OutputAppendStringSynchronised(const TCHAR *s, std::_tstring * out, int len = 0) 
	{
		if (len == 0)
			len = _tcsclen(s);
		out->append(s, len);
	}

	void AbsolutePath(TCHAR *absPath, const TCHAR *relativePath, int size) 
	{
		// The runtime libraries for GCC and Visual C++ give different results for _fullpath
		// so use the OS.
		*absPath = '\0';
		LPTSTR fileBit = 0;
		::GetFullPathName(relativePath, size, absPath, &fileBit);
		//Platform::DebugPrintf("AbsolutePath: <%s> -> <%s>\n", relativePath, absPath);
	}				

	DWORD run(const Job &jobToRun)
	{
		DWORD exitcode = 0;
		//ElapsedTime commandTime;

		OSVERSIONINFO osv;
		::ZeroMemory(&osv, sizeof(OSVERSIONINFO));
		osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		::GetVersionEx(&osv);
		m_windows95 = osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;

		SECURITY_ATTRIBUTES sa;
		::ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);

		//Platform::DebugPrintf("Execute <%s>\n", command);

		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = NULL;

		SECURITY_DESCRIPTOR sd;
		// If NT make a real security thing to allow inheriting handles
		if (!m_windows95) 
		{
			::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
			::SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
			sa.nLength = sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = &sd;
		}

		HANDLE hPipeWrite = NULL;
		HANDLE hPipeRead = NULL;
		// Create pipe for output redirection
		// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
		::CreatePipe(&hPipeRead, &hPipeWrite, &sa, 0);

		HANDLE hPipeErrWrite = NULL;
		HANDLE hPipeErrRead = NULL;
		// Create pipe for output redirection
		// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
		::CreatePipe(&hPipeErrRead, &hPipeErrWrite, &sa, 0);

		//Platform::DebugPrintf("2Execute <%s>\n");
		// Create pipe for input redirection. In this code, you do not
		// redirect the output of the child process, but you need a handle
		// to set the hStdInput field in the STARTUP_INFO struct. For safety,
		// you should not set the handles to an invalid handle.

		hWriteSubProcess = NULL;
		m_subProcessGroupId = 0;
		HANDLE hRead2 = NULL;
		// read handle, write handle, security attributes,  number of bytes reserved for pipe - 0 default
		::CreatePipe(&hRead2, &hWriteSubProcess, &sa, 0);

		::SetHandleInformation(hPipeRead, HANDLE_FLAG_INHERIT, 0);
		::SetHandleInformation(hWriteSubProcess, HANDLE_FLAG_INHERIT, 0);

		//Platform::DebugPrintf("3Execute <%s>\n");
		// Make child process use hPipeWrite as standard out, and make
		// sure it does not show on screen.
		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		if (jobToRun.jobType == jobCLI)
			si.wShowWindow = SW_HIDE;
		else
			si.wShowWindow = SW_SHOW;
		si.hStdInput = hRead2;
		si.hStdOutput = hPipeWrite;
		si.hStdError = hPipeErrWrite;

		TCHAR startDirectory[_MAX_PATH];
		startDirectory[0] = '\0';
		AbsolutePath(startDirectory, jobToRun.directory.c_str(), _MAX_PATH);

		PROCESS_INFORMATION pi;
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		BOOL running = ::CreateProcess(
			NULL,
			const_cast<TCHAR *>(jobToRun.command.c_str()),
			NULL, NULL,
			TRUE, CREATE_NEW_PROCESS_GROUP,
			NULL,
			startDirectory[0] ? startDirectory : NULL,
			&si, &pi);

		if (running)
		{
			m_subProcessGroupId = pi.dwProcessId;
			exitcode = ReadOutput(jobToRun, pi, hPipeRead, m_out);
			exitcode = ReadOutput(jobToRun, pi, hPipeErrRead, m_err);
		}
		else 
		{
			DWORD nRet = ::GetLastError();
			TCHAR lpMsgBuf[32000];
			::FormatMessage(
				//FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				nRet,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
				lpMsgBuf,
				sizeof(lpMsgBuf),
				NULL
				);
			if (lpMsgBuf)
			{
				OutputAppendStringSynchronised(lpMsgBuf, m_out);
				::LocalFree(lpMsgBuf);
			}
			//WarnUser(warnExecuteKO);
		}

		::CloseHandle(hPipeRead);
		::CloseHandle(hPipeWrite);
		::CloseHandle(hRead2);
		if (hWriteSubProcess != INVALID_HANDLE_VALUE)
			::CloseHandle(hWriteSubProcess);
		hWriteSubProcess = NULL;
		m_subProcessGroupId = 0;
		return exitcode;
	}

	DWORD ReadOutput(const Job &jobToRun, PROCESS_INFORMATION & pi, HANDLE hPipeRead, std::_tstring * strBuff)
	{
		DWORD exitcode = 0;
		bool seenOutput = false;
		char buffer[16384];

		bool running = true;
		bool cancelled = false;

		std::_tstring repSelBuf;

		DWORD timeDetectedDeath = 0;

		size_t totalBytesToWrite = 0;
		if (jobToRun.flags & jobHasInput) 
		{
			totalBytesToWrite = jobToRun.input.length();
		}

		unsigned writingPosition = 0;

		while (running) 
		{
			if (writingPosition >= totalBytesToWrite) 
			{
				::Sleep(100L);
			}

			DWORD bytesRead = 0;
			DWORD bytesAvail = 0;

			if (!::PeekNamedPipe(hPipeRead, buffer, sizeof(buffer), &bytesRead, &bytesAvail, NULL)) 
			{
				bytesAvail = 0;
			}

			if ((bytesAvail < 1000) && (hWriteSubProcess != INVALID_HANDLE_VALUE) && (writingPosition < totalBytesToWrite)) 
			{
				// There is input to transmit to the process.  Do it in small blocks, interleaved
				// with reads, so that our hRead buffer will not be overrun with results.

				size_t bytesToWrite = jobToRun.input.find("\n", writingPosition) + 1 - writingPosition;
				if ((bytesToWrite <= 0) || (writingPosition + bytesToWrite >= totalBytesToWrite)) 
				{
					bytesToWrite = totalBytesToWrite - writingPosition;
				}
				if (bytesToWrite > 250) 
				{
					bytesToWrite = 250;
				}

				DWORD bytesWrote = 0;

				int bTest = ::WriteFile(hWriteSubProcess, jobToRun.input.c_str() + writingPosition, bytesToWrite, &bytesWrote, NULL);

				if (bTest) 
				{
					if ((writingPosition + bytesToWrite) / 1024 > writingPosition / 1024) 
					{
						// sleep occasionally, even when writing
						::Sleep(100L);
					}

					writingPosition += bytesWrote;

					if (writingPosition >= totalBytesToWrite) 
					{
						if (m_windows95) 
						{
							// Write a Ctrl+Z to output to mark the end of the text
							TCHAR stop[] = _T("\032");
							::WriteFile(hWriteSubProcess,
								stop, static_cast<DWORD>(_tcsclen(stop)), &bytesWrote, NULL);
						}
						::CloseHandle(hWriteSubProcess);
						hWriteSubProcess = INVALID_HANDLE_VALUE;
					}

				} 
				else 
				{
					// Is this the right thing to do when writing to the pipe fails?
					::CloseHandle(hWriteSubProcess);
					hWriteSubProcess = INVALID_HANDLE_VALUE;
					OutputAppendStringSynchronised(_T("\n>Input pipe closed due to write failure.\n"), m_out);
				}

			} 
			else if (bytesAvail > 0) 
			{
				int bTest = ::ReadFile(hPipeRead, buffer, sizeof(buffer), &bytesRead, NULL);

				if (bTest && bytesRead) 
				{

					if (jobToRun.flags & jobRepSelMask) 
					{
						std::string strRead;
						strRead.append(buffer, bytesRead);
						repSelBuf.append(CA2T(strRead.c_str()));
					}

					if (!(jobToRun.flags & jobQuiet)) 
					{
						if (!seenOutput) 
						{
							//TODO:  MakeOutputVisible();
							seenOutput = true;
						}
						// Display the data
						std::string strRead;
						strRead.append(buffer, bytesRead);
						OutputAppendStringSynchronised(CA2T(strRead.c_str()), strBuff, strRead.length());
					}

					//TODO::UpdateWindow(MainHWND());
				} 
				else 
				{
					running = false;
				}
			} 
			else 
			{
				if (::GetExitCodeProcess(pi.hProcess, &exitcode)) 
				{
					if (STILL_ACTIVE != exitcode) 
					{
						if (m_windows95) 
						{
							// Process is dead, but wait a second in case there is some output in transit
							if (timeDetectedDeath == 0) 
							{
								timeDetectedDeath = ::GetTickCount();
							} 
							else 
							{
								if ((::GetTickCount() - timeDetectedDeath) > static_cast<unsigned int>(500)) 
								{
									running = false;    // It's a dead process
								}
							}
						} 
						else 
						{	// NT, so dead already
							running = false;
						}
					}
				}
			}

			if (::InterlockedExchange(&m_cancelFlag, 0)) 
			{
				if (WAIT_OBJECT_0 != ::WaitForSingleObject(pi.hProcess, 500)) 
				{
					// We should use it only if the GUI process is stuck and
					// don't answer to a normal termination command.
					// This function is dangerous: dependant DLLs don't know the process
					// is terminated, and memory isn't released.
					OutputAppendStringSynchronised(_T("\n>Process failed to respond; forcing abrupt termination...\n"), m_out);
					::TerminateProcess(pi.hProcess, 1);
				}
				running = false;
				cancelled = true;
			}
		}

		if (WAIT_OBJECT_0 != ::WaitForSingleObject(pi.hProcess, 1000)) 
		{
			OutputAppendStringSynchronised(_T("\n>Process failed to respond; forcing abrupt termination..."), m_out);
			::TerminateProcess(pi.hProcess, 2);
		}
		::GetExitCodeProcess(pi.hProcess, &exitcode);
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);

		if (!cancelled) 
		{
			bool doRepSel = false;
			if (jobToRun.flags & jobRepSelYes)
				doRepSel = true;
			else if (jobToRun.flags & jobRepSelAuto)
				doRepSel = (0 == exitcode);

			if (doRepSel) 
			{
				//SendEditor(SCI_REPLACESEL,0,(sptr_t)(repSelBuf.c_str()));
			}
		}

		//WarnUser(warnExecuteOK);
		return exitcode;
	} 
};
int CLIB_API runProcess(const std::_tstring &command, const std::_tstring &directory, const std::_tstring &path, const std::_tstring &in, std::_tstring &out, std::_tstring &err)
{
	Job j;
	j.command = command;
	j.directory = directory;
	j.input = CT2A(in.c_str());
	j.jobType = jobCLI;
	j.flags = jobHasInput;
	CComPtr<CProcess> proc = new CProcess(&out, &err);
	int retVal = proc->run(j);
	return retVal;
}
#endif
