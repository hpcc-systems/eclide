#pragma once

#include <clib.h>
#include "wlib.h"
#include <EclParser.h>
#include <Attribute.h>

class CSprayInfo
{
public:
	CSprayInfo()
	{
		m_timeOut = -1;
		m_maxConnections = 1;
		m_allowOverwrite = false;
		m_replicate = false;
	}
	CSprayInfo(const CSprayInfo &other)
	{
		*this = other;
	}
	StdString m_sourceIP;
	StdString m_sourcePath;
	StdString m_destinationGroup;
	StdString m_destinationLogicalName;
	StdString m_prefix;
	int		  m_timeOut; //=-1
	StdString m_dfuServerIpPort; 
	int		  m_maxConnections;
	bool	  m_allowOverwrite; //false
	bool	  m_replicate; //false
};

class CSprayInfoFixed : public CSprayInfo
{
public:
	CSprayInfoFixed()
	{
		m_recordSize = 0;
	}
	void operator=(const CSprayInfo &other)
	{
		*(CSprayInfo *)this = other;
	}
	int	m_recordSize;
	StdString m_recordName;
};

class CSprayInfoVariable : public CSprayInfo
{
public:
	CSprayInfoVariable()
	{
		init();
	}
	void operator=(const CSprayInfo &other)
	{
		*(CSprayInfo *)this = other;
	}

	#define DEFAULTSOURCEMAXRECORDSIZE -1

	void init()
	{
		m_sourceMaxRecordSize = DEFAULTSOURCEMAXRECORDSIZE;
		m_sourceCsvSeparate = _T("\\,:");
		m_sourceCsvTerminate = _T("\\n,\\r\\n");
		m_sourceCsvQuote = _T("'");
	}

	const TCHAR *SourceMaxRecordSizeParam(std::_tstring &param)
	{
		if ( m_sourceMaxRecordSize == DEFAULTSOURCEMAXRECORDSIZE )
			param = _T("/*maxrecordsize*/");
		else
		{
			try
			{
				param = boost::lexical_cast<int>(m_sourceMaxRecordSize);
			}
			catch(boost::bad_lexical_cast &)
			{
			}
		}

		return param.c_str();
	}

	int		  m_sourceMaxRecordSize;
	StdString m_sourceCsvSeparate;
	StdString m_sourceCsvTerminate;
	StdString m_sourceCsvQuote;
};

class CSprayInfoXml : public CSprayInfo
{
public:
	CSprayInfoXml()
	{
		m_sourceMaxRecordSize = 8192;
		m_sourceEncoding = _T("utf8");
	}
	void operator=(const CSprayInfo &other)
	{
		*(CSprayInfo *)this = other;
	}
	int		  m_sourceMaxRecordSize;
	StdString m_sourceRowTag;
	StdString m_sourceEncoding;
};

class CDesprayInfo
{
public:
	CDesprayInfo()
	{
		m_timeOut = -1;
		m_maxConnections = 1;
		m_allowOverwrite = false;
	}
	StdString m_logicalName;
	StdString m_destinationIP;
	StdString m_destinationPath;
	int		  m_timeOut; //=-1
	StdString m_dfuServerIpPort; //=lib_system.ws_fs_server
	int		  m_maxConnections; //=-1
	bool	  m_allowOverwrite; //false
};


class WLIB_API CEclProgramManager
{
public:
	CEclProgramManager(IAttribute *attr);
	CEclProgramManager(const std::_tstring &ecl);
	CEclProgramManager(IEclProgram *program=0);
	~CEclProgramManager();

	bool SetEcl(const std::_tstring &ecl);
	bool HasVariableLengthFields(IEclRecord* record);

	//if name is null, return first matching element found
	IEclRecord * FindRecord(const TCHAR *recordName=0);

	operator IEclProgram*() const;
	IEclProgram* GetProgram();

	static const TCHAR* ECLBool(bool b); //true or false

	static const TCHAR * TransformInputType(IEclTransform *transform, std::_tstring &type, bool bQualified=false);
	static const TCHAR * TransformOutputType(IEclTransform *transform, std::_tstring &type, bool bQualified=false);

	static bool ValidateIdentifier(const TCHAR* id);
	static void CommentAllCode(std::_tstring& ecl);

	static bool GetQualifiedRecordType(IAttribute *transformAttr,std::_tstring &recordType);
	static void CreateRecord(IAttribute *layoutAttr, std::_tstring &recordEcl);
	static void CreateRecord(IAttribute *layoutAttr, const TCHAR *recordItems, std::_tstring &recordEcl);

	static void GetEclFormatVariable(int headerRows, const StdString& delimiter, const StdString& terminator, const StdString& quote, std::_tstring &format);
	static void GetEclFormat(CSprayInfoFixed &sprayInfo, std::_tstring& format);
	static void GetEclFormat(CSprayInfoVariable &sprayInfo, std::_tstring& format);
	static void GetEclFormat(CSprayInfoXml &sprayInfo, std::_tstring& format);

protected:
	StlLinked<IEclProgram> m_program;
	IAttributeAdapt m_attr;
};
