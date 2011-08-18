#pragma once

#include ".\ImportRecordAnalyze.h"

#include "topology.h"

class CLandingZonePath
{
protected:
	Topology::OS m_os;
	std::_tstring m_ip;
	boost::filesystem::path m_path;
	bool m_is_folder;
	mutable std::_tstring m_path_str;

protected:
	boost::filesystem::path GetPath(const std::_tstring & path)
	{
		try
		{
			switch(m_os)
			{
			case Topology::OS_UNKNOWN:
				break;
			case Topology::OS_WIN:
				return boost::filesystem::path(CT2A(path.c_str()), boost::filesystem::native);
			case Topology::OS_LINUX:
				return boost::filesystem::path(CT2A(path.c_str()), boost::filesystem::portable_posix_name);
			default:
				ATLASSERT(false);
				break;
			}
		}
		catch(boost::filesystem::filesystem_error &e)
		{
			(e);
			ATLTRACE("Exception:  %s\n", e.what());
		}
		return boost::filesystem::path();
	}

public:
	CLandingZonePath()
	{
		m_os = Topology::OS_UNKNOWN;
		m_is_folder = false;
	}

	bool Set(Topology::OS os, const std::_tstring & ip, const std::_tstring & path, bool is_folder)
	{
		m_is_folder = is_folder;
		m_os = os;
		m_ip = ip;
		m_path = GetPath(path);
		return true;
	}

	void SetOS(Topology::OS os) 
	{
		m_os = os;
	}
	Topology::OS GetOS() const
	{
		return m_os;
	}

	void SetIP(const std::_tstring & ip) 
	{
		m_ip = ip;
	}

	const TCHAR * GetIP() const
	{
		return m_ip.c_str();
	}

	const TCHAR * GetPath() const
	{
		m_path_str = _T("");
		switch(m_os)
		{
		case Topology::OS_WIN:
			m_path_str = CA2T(m_path.native_file_string().c_str());
			break;
		case Topology::OS_LINUX:
			m_path_str = CA2T(m_path.string().c_str());
			break;
		}
		return m_path_str.c_str();
	}

	const TCHAR * GetParentPath() const
	{
		m_path_str = _T("");
		boost::filesystem::path parent_path = m_path.branch_path();
		switch(m_os)
		{
		case Topology::OS_WIN:
			m_path_str = CA2T(parent_path.native_file_string().c_str());
			break;
		case Topology::OS_LINUX:
			m_path_str = CA2T(parent_path.string().c_str());
			break;
		}
		return m_path_str.c_str();
	}

	const TCHAR * GetCurrentFolder() const
	{
		if (m_is_folder)
			return GetPath();
		return GetParentPath();
	}

	bool IsEmpty() const
	{
		return m_path.empty();
	}

	void Up()
	{
		m_path = m_path.branch_path();
		m_is_folder = true;
	}

	void Down(const std::_tstring & folder, bool is_folder)
	{
		if (!m_is_folder)
		{
			Up();
		}

		m_path /= GetPath(folder);
		m_is_folder = is_folder;
	}

	const TCHAR * GetEclPath(std::_tstring & path)
	{
		path = GetIP();
		path += GetPath();
		boost::algorithm::replace_all(path, _T("\\"), _T("::"));
		boost::algorithm::replace_all(path, _T("/"), _T("::"));
		return path.c_str();
	}

	void SetEclPath(const std::_tstring & path)
	{
		typedef std::vector<std::_tstring> split_vector_type;
		split_vector_type SplitVec; 
		boost::algorithm::split(SplitVec, path, boost::algorithm::is_any_of("::"), boost::algorithm::token_compress_on);

		std::_tstring ip, tail_path;
		int i = 0;
		for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
		{
			switch(i)
			{
			case 0:	//"~file"
				break;
			case 1:	//ip
				ip = *itr;
				break;
			default:
				tail_path += std::_tstring(_T("/")) + *itr;
				break;
			}
			++i;
		}
		Set(Topology::OS_LINUX, ip, tail_path, false);
	}
};

class CWizardSprayData
{
protected:
	bool m_gretlFlag;
	bool m_showWelcome;
	__int64 m_fileSize;

	std::_tstring m_sourceEcl;
	std::_tstring m_destination;
	std::_tstring m_target;
	std::_tstring m_module;
	std::_tstring m_prefix;
	std::_tstring m_queue;
	std::_tstring m_cluster;

public:
	CImportRecordAnalyze m_analyzerInfo;
	CLandingZonePath m_lz_path;

public:
	CWizardSprayData(void);
	~CWizardSprayData(void);

	bool SetShowWelcome(bool showWelcome);
	bool GetShowWelcome(void) const;
	void SetGrETLFlag(bool gretlFlag);
	bool GetGrETLFlag() const;

	// Page Navigation
	int FindPreviousPage(int pageDialogId) const;
	int FindNextPage(int pageDialogId) const;
	int FindFirstPageAfterWelcome() const;

	// Help
	void ShowHelp(int pageDialogId, int controlId = 0);
	void ShowContextHelp(LPHELPINFO helpInfo);

	//Finish
	bool FinishWizard(HWND hWndParent);

	//  Accesors
	//void SetSourceEcl(const std::_tstring & ecl);
	//const TCHAR * GetSourceEcl();

	void SetSource(const std::_tstring & ecl);
	const TCHAR * GetSource();
	const TCHAR * GetSourceFilename(std::_tstring & filename);

	void SetModule(const std::_tstring & module);
	const TCHAR * GetModule(std::_tstring & module);

	void SetPrefix(const std::_tstring & prefix);
	const TCHAR * GetPrefix(std::_tstring & prefix);

	void SetDestination(const std::_tstring & destination);
	const TCHAR * GetDestination(std::_tstring & destination);

	void SetTarget(const std::_tstring & target);
	const TCHAR * GetTarget(std::_tstring & target);

	void SetQueue(const std::_tstring & queue);
	const TCHAR * GetQueue(std::_tstring & queue);

	void SetCluster(const std::_tstring & cluster);
	const TCHAR * GetCluster(std::_tstring & cluster);

	void SetFileSize(__int64 fileSize);
	__int64 GetFileSize();

	enum ATTRIBUTELABEL
	{
		ATTRIBUTELABEL_RECORD,
		ATTRIBUTELABEL_DATASET,
		ATTRIBUTELABEL_SPRAY,
		ATTRIBUTELABEL_TARGET,
		ATTRIBUTELABEL_TRANSFORM,
		ATTRIBUTELABEL_LAST
	};
	const TCHAR * GetEclAttributeLabel(ATTRIBUTELABEL type, bool qualified, std::_tstring & attribute);
	const TCHAR * GetEclRecord(std::_tstring & record);
	const TCHAR * GetEclDataset(std::_tstring & dataset);
	const TCHAR * GetEclDatasetRaw(std::_tstring & dataset);
	const TCHAR * GetEclSpray(std::_tstring & spray);
	const TCHAR * GetEclTarget(std::_tstring & target);
	const TCHAR * GetEclTransform(std::_tstring & transform);
	void SaveEcl(const CString & module, const CString & label, const CString & ecl, const CString & comments);

	const TCHAR * GetSprayBuilderEcl(std::_tstring & ecl);
};
