#include "StdAfx.h"

#include "EclCC.h"
#include "comms.h"
#include <cmdProcess.h>
#include <Logger.h>
#include <EclErrorParser.h>
#include "Repository.h"

namespace fs = boost::filesystem;
namespace algo = boost::algorithm;

typedef std::pair<std::_tstring, bool> StringBoolPair;	//  Path, include in -I options
typedef std::vector<StringBoolPair> WPathVector;

TRI_BOOL g_EnableCompiler = TRI_BOOL_UNKNOWN;
TRI_BOOL g_EnableRemoteDali = TRI_BOOL_UNKNOWN;

class CEclCC : public IEclCC, public CUnknown
{
protected:
	CComPtr<IConfig> m_config;
	std::_tstring m_compilerFile;
	fs::wpath m_compilerFilePath;
	fs::wpath m_compilerFolderPath;

	std::_tstring m_arguments;

	std::_tstring m_workingFolder;
	fs::wpath m_workingFolderPath;

	WPathVector m_eclFolders;

	mutable std::_tstring m_version;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CEclCC() 
	{
		m_config = GetIConfig(QUERYBUILDER_CFG);
		ATLASSERT(m_config);
		m_compilerFile = CString(m_config->Get(GLOBAL_COMPILER_LOCATION));
		m_compilerFilePath = fs::wpath(m_compilerFile, fs::native);
		m_compilerFolderPath = m_compilerFilePath.parent_path();

		m_arguments = CString(m_config->Get(GLOBAL_COMPILER_ARGUMENTS));

		m_workingFolder = CString(m_config->Get(GLOBAL_COMPILER_ECLWORKINGFOLDER));
		m_workingFolderPath = fs::wpath(m_workingFolder, fs::native);

			//{
			//	const TCHAR * hpccbin = _tgetenv(_T("HPCCBIN"));
			//	if (hpccbin)
			//	{
		fs::wpath stdLibPath = m_compilerFolderPath / _T("ecllibrary");
		if (fs::exists(stdLibPath))
			m_eclFolders.push_back(std::make_pair(stdLibPath.directory_string(), false));
		for (int i = 0; i < 10; ++i)
		{
			CString text;
			switch (i)
			{
			case 0:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER00);
				break;
			case 1:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER01);
				break;
			case 2:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER02);
				break;
			case 3:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER03);
				break;
			case 4:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER04);
				break;
			case 5:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER05);
				break;
			case 6:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER06);
				break;
			case 7:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER07);
				break;
			case 8:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER08);
				break;
			case 9:
				text = m_config->Get(GLOBAL_COMPILER_ECLFOLDER09);
				break;
			}
			if (text.GetLength() > 0)
				m_eclFolders.push_back(std::make_pair(static_cast<const TCHAR *>(text), true));
		}
	}

	TRI_BOOL HasSettings() const
	{
		return g_EnableCompiler;
	}

	const TCHAR * GetVersion() const
	{
		if (m_version.empty())
		{
			std::_tstring command = m_compilerFile;
			if (!m_arguments.empty())
			{
				command += _T(" ");
				command += m_arguments;
			}
			command += _T(" --version");
			std::_tstring runPath = m_compilerFolderPath.directory_string().c_str();
			std::_tstring in, out, err;
			runProcess(command, runPath, _T(""), in, out, err);
			m_version = out;
		}
		return m_version.c_str();
	}

	const TCHAR * GetPrefWarnings(std::_tstring & warnings) const
	{
		if (!fs::exists(m_compilerFilePath))
			warnings = (boost::_tformat(_T("Compiler path invalid:  %1%")) % m_compilerFile).str() + _T("\r\n");
		else if (fs::is_directory(m_compilerFilePath))
			warnings = (boost::_tformat(_T("Compiler path does not specify eclcc.exe:  %1%")) % m_compilerFile).str() + _T("\r\n");

		if (!m_workingFolder.empty() && !fs::exists(m_workingFolder))
		{
			std::_tstring error_msg = (boost::_tformat(_T("Working folder invalid:  %1%")) % m_workingFolder).str() + _T("\r\nCreate?");
			if (::MessageBox(NULL, error_msg.c_str(), _T("ECL IDE"), MB_ICONASTERISK | MB_YESNO) == IDYES)
			{
				try {
					fs::create_directories(m_workingFolder);
				} catch (const std::exception & ex) {
					warnings += (boost::_tformat(_T("Failed to create folder:  %1%")) % m_workingFolder).str() + _T("\r\n");
				}

			}
			else
				warnings += (boost::_tformat(_T("Working folder invalid:  %1%")) % m_workingFolder).str() + _T("\r\n");
		}

		return warnings.c_str();
	}

	const TCHAR * GetWorkingFolder() const
	{
		return m_workingFolder.c_str();
	}

	int GetEclFolderCount() const
	{
		return m_eclFolders.size();
	}

	const TCHAR * GetEclFolder(int i) const
	{
		ATLASSERT(i >= 0 && i < m_eclFolders.size());
		return m_eclFolders[i].first.c_str();
	}

	void ParseErrors(const std::_tstring & sourcePath, const std::_tstring & _err, bool &hasErrors, Dali::CEclExceptionVector & errors, bool markAllAsErrors) const
	{
		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		algo::split(SplitVec, _err, algo::is_any_of("\r\n"), algo::token_compress_on);
		for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
		{
			ParsedEclError err;
			if (ParseEclError(*itr, err))
			{
				StlLinked<Dali::CEclException> e = new Dali::CEclException();
				e->m_lineNo = err.row;
				e->m_code = err.code;
				e->m_message = err.message.c_str();
				e->m_column = err.col;
				if (markAllAsErrors || algo::equals(err.type, "error"))
				{
					e->m_severity = _T("Error");
					hasErrors = true;
				}
				else if (algo::equals(err.type, "warning"))
					e->m_severity = _T("Warning");
				else 
					e->m_severity = err.type.c_str();
				if (algo::iends_with(sourcePath, err.location))
					e->m_fileName = _T("");
				else
					e->m_fileName = err.location.c_str();
				errors.push_back(e);
			}
			else if (!itr->empty())
			{
				StlLinked<Dali::CEclException> e = new Dali::CEclException();
				e->m_lineNo = 0;
				e->m_code = 0;
				e->m_message = itr->c_str();
				e->m_column = 0;
				e->m_severity = markAllAsErrors ? _T("Error") : _T("Info");
				errors.push_back(e);
			}
		}
	}

	const TCHAR * CallEclCC(const std::_tstring & _module, const std::_tstring & _attribute, const std::_tstring & path, const std::_tstring & ecl, const StdStringVector & args, std::_tstring & out, std::_tstring & err, bool &hasErrors, Dali::CEclExceptionVector & errors) const
	{
		hasErrors = false;
		CAtlTemporaryFile temp;
		std::_tstring sourcePath;
		if (_module.empty() && _attribute.empty() && path.empty())
		{
			temp.Create();
			sourcePath = temp.TempFileName();
			std::string necl = CT2A(ecl.c_str(), CP_UTF8);
			temp.Write(necl.c_str(), necl.length());
			temp.HandsOff();
		}
		else if (path.empty())
		{
			GetAttributeFilePath(_module, _attribute, sourcePath);
		}
		else
		{
			sourcePath = path;
		}

		std::_tstring command = _T("\"");
		command += m_compilerFile;
		command += _T("\"");
		if (!m_arguments.empty())
		{
			command += _T(" ");
			command += m_arguments;
		}
		for(StdStringVector::const_iterator itr = args.begin(); itr != args.end(); ++itr)
		{
			command += _T(" -");
			command += *itr;
		}
		if (!m_workingFolder.empty())
		{
			command += _T(" -P\"");
			command += m_workingFolder;
			command += _T("\"");
		}
		for(WPathVector::const_iterator itr = m_eclFolders.begin(); itr != m_eclFolders.end(); ++itr)
		{
			if (itr->second)	//  Exclude ecllibrary
			{
				command += _T(" -I\"");
				command += itr->first;
				command += _T("\"");
			}
		}
		command += _T(" \"");
		command += sourcePath;
		command += _T("\"");
		std::_tstring runPath = m_compilerFolderPath.directory_string().c_str();
		std::_tstring in = _T("");

		std::_tstring folder = m_workingFolder;
		if (folder.empty())
		{
			_TCHAR buf[MAX_PATH];
			 if(GetTempPath(MAX_PATH, buf))
				 folder = buf;
		}
		runProcess(command, folder, _T(""), in, out, err);

		ParseErrors(sourcePath, err, hasErrors, errors, false);

		if (_module.empty() && _attribute.empty() && path.empty())
		{
			temp.HandsOn();
		}

		return out.c_str();
	}

	void CheckSyntax(const std::_tstring & module, const std::_tstring & attribute, const std::_tstring & path, const std::_tstring & ecl, Dali::CEclExceptionVector & errors) const
	{
		StdStringVector args;
		args.push_back(_T("f\"syntaxcheck=1\""));

		std::_tstring out, err;
		bool hasErrors = false;
		CallEclCC(module, attribute, path, ecl, args, out, err, hasErrors, errors);
	}

	const TCHAR * GetWorkunitXML(const std::_tstring & wuid, std::_tstring & wuXml) const
	{
		std::_tstring filePath = (m_workingFolderPath / (wuid + _T(".xml"))).native_file_string();
		if (!fs::exists(filePath))
		{
			std::_tstring command = _T("wuget.exe \"");
			command += (m_workingFolderPath / (wuid + _T(".exe"))).native_file_string();
			command += _T("\"");
			std::_tstring err;
			runProcess(command, m_workingFolder, m_compilerFolderPath.native_directory_string(), _T(""), wuXml, err);
			CUnicodeFile file;
			if (file.Create(filePath.c_str()))
				file.Write(wuXml);
		}
		else
		{
			CUnicodeFile file;
			file.Open(filePath.c_str());
			file.Read(wuXml);
		}
		return wuXml.c_str();
	}

	const TCHAR * SaveWorkunitXML(const std::_tstring & wuid, std::_tstring & filePath) const
	{
		filePath = (m_workingFolderPath / (wuid + _T(".xml"))).native_file_string();
		if (!fs::exists(filePath))
		{
			std::_tstring wuXml;
			GetWorkunitXML(wuid, wuXml);
			if (!wuXml.empty())
			{
				CUnicodeFile file;
				if (file.Create(filePath.c_str()))
					file.Write(wuXml);
			}
		}
		return filePath.c_str();
	}

	const TCHAR * GetWorkunitResults(const std::_tstring & wuid, bool compileOnly, std::_tstring & results, bool & hasErrors, Dali::CEclExceptionVector & errors) const
	{
		fs::wpath resultPath = m_workingFolderPath / (wuid + _T("-result.xml"));
		fs::wpath exePath = m_workingFolderPath / (wuid + _T(".exe"));
		if (fs::exists(exePath) && !fs::exists(resultPath))
		{
			if (compileOnly)
			{
				results = _T("");//_T("<Dataset/>");
			}
			else
			{
				std::_tstring command = exePath.native_file_string();
				command += _T(" ");
				command += (const TCHAR *)CString(m_config->Get(GLOBAL_COMPILER_WUARGUMENTS));
				command += _T(" -xml");
				std::_tstring err;
				runProcess(command, m_workingFolder, m_compilerFolderPath.native_directory_string(), _T(""), results, err);
				ParseErrors(_T("POIOIUPOIPOIPOIPOIPOIP"), err, hasErrors, errors, true);
			}
			CUnicodeFile file;
			file.Create(resultPath.native_file_string().c_str());
			file.Write(results);
		}
		else if (fs::exists(resultPath))
		{
			CUnicodeFile file;
			file.Open(resultPath.native_file_string().c_str());
			file.Read(results);
		}
		return results.c_str();
	}

	const TCHAR * Compile(const std::_tstring & path, const std::_tstring & ecl, const std::_tstring & wuid, int resultLimit, const std::_tstring & debugString, std::_tstring & wuXml, bool & hasErrors, Dali::CEclExceptionVector & errors) const
	{
		StdStringVector args;
		args.push_back(_T("q"));
		if (resultLimit > 0)
		{
			args.push_back(_T("fapplyInstantEclTransformations=1"));
			args.push_back((boost::_tformat(_T("fapplyInstantEclTransformationsLimit=%1%")) % resultLimit).str());
		}
		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		algo::split(SplitVec, debugString, algo::is_any_of("\r\n"), algo::token_compress_on);
		for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
		{
			algo::trim(*itr);
			if (!itr->empty())
				args.push_back(_T("f") + *itr);
		}
		args.push_back((boost::_tformat(_T("o\"%1%\"")) % wuid).str());

		std::_tstring out, err;
		hasErrors = false;
		CallEclCC(_T(""), _T(""), path, ecl, args, out, err, hasErrors, errors);

		if (!hasErrors)
		{
			GetWorkunitXML(wuid, wuXml);
		}

		return wuXml.c_str();
	}

	bool GetArchive(const std::_tstring & path, const std::_tstring & ecl, std::_tstring & archive, Dali::CEclExceptionVector & errors) const
	{
		StdStringVector args;
		args.push_back(_T("E"));

		std::_tstring err;
		bool hasErrors = false;
		CallEclCC(_T(""), _T(""), path, ecl, args, archive, err, hasErrors, errors);

		return hasErrors == false;
	}

	const TCHAR * GetAttributeFilePath(const std::_tstring & module, const std::_tstring & attribute, std::_tstring & path) const
	{
		CComPtr<IRepository> rep = AttachRepository();
		CComPtr<IAttribute> attr = rep->GetAttributeFast(module.c_str(), attribute.c_str(), CreateIAttributeECLType());
		return GetAttributeFilePath(attr, path);
	}

	const TCHAR * GetAttributeFilePath(IAttribute * attr, std::_tstring & path) const
	{
		path = attr->GetPath();
		return path.c_str();
	}

	const TCHAR * GetAttributeLabel(IAttribute * attr, std::_tstring & label) const
	{
		label = attr->GetQualifiedLabel();
		algo::replace_all(label, _T("."), _T("/"));
		label += _T(".");
		label += attr->GetType()->GetRepositoryCode();
		return label.c_str();
	}
};

void EnableLocalRepository(TRI_BOOL enable)
{
	g_EnableCompiler = enable;
}

TRI_BOOL IsLocalRepositoryEnabled()
{
	return g_EnableCompiler;
}

void EnableRemoteDaliEnabled(TRI_BOOL enable)
{
	g_EnableRemoteDali = enable;
}

TRI_BOOL IsRemoteDaliEnabled()
{
	return g_EnableRemoteDali;
}

IEclCC * CreateIEclCC()
{
	CEclCC * retVal = new CEclCC();
	if (retVal->HasSettings())
		return retVal;
	delete retVal;
	return NULL;
}


