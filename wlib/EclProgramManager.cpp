#include "StdAfx.h"
#include ".\eclprogrammanager.h"
#include "UnicodeFile.h"
#include <Attribute.h> //comms
#include <Util.h> //clib
#include <atlrx.h>

CEclProgramManager::CEclProgramManager(const std::_tstring &ecl)
{
	SetEcl(ecl);
}

CEclProgramManager::CEclProgramManager(IEclProgram *program)
{
	m_program = program;
}

CEclProgramManager::CEclProgramManager(IAttribute *attr)
{
	m_attr = attr;
	SetEcl(attr->GetText());
}

CEclProgramManager::~CEclProgramManager(void)
{
}

bool CEclProgramManager::SetEcl(const std::_tstring &ecl)
{
	m_program = ::ParseEcl(ecl);
	return m_program.isLinked();
}

CEclProgramManager::operator IEclProgram*() const
{
	return m_program;
}

IEclProgram* CEclProgramManager::GetProgram()
{
	return m_program;
}

bool CEclProgramManager::HasVariableLengthFields(IEclRecord* record)
{
	for(unsigned i=0; i < record->GetFieldCount(); ++i)
	{
		CComQIPtr<IEclDeclaration> field = record->GetField(i);
		if ( field )
		{
			if ( field->GetType()->GetFixedPhysicalWidth() == 0 )
				return true;
		}
	}
	return false;
}

bool CEclProgramManager::ValidateIdentifier(const TCHAR* id)
{
	if ( !id || !*id )
		return false;
	else
	{
		CAtlRegExp<> re;
		re.Parse(_T("{[a-zA-Z_]+[a-zA-Z0-9_]*}"));
		CAtlREMatchContext<> mc;
		//no match or only partial match is no good
		bool good = re.Match(id,&mc) == TRUE;
		if ( good )
		{
			const TCHAR *pszStart;
			const TCHAR *pszEnd;
			mc.GetMatch(0,&pszStart,&pszEnd);
			good = (pszEnd-pszStart) == (int)_tcsclen(id);
		}
		return good;
	}
}

template <class T>
T * FindEclElement(IEclProgram *program, const TCHAR *pElementName)
{
	std::_tstring elementName;
	if (pElementName)
	{
		elementName = pElementName;
	}
	CComQIPtr<T> element;
	std::_tstring id;
	for(unsigned i = 0; i < program->GetElementCount(); ++i)
	{
		IEclElement *el = program->GetElement(i);
		element = el;
		if (element)
		{
			if (elementName.IsEmpty() || 0==elementName.CompareNoCase(element->GetIdentifierName(id)) )
			{
				return element;
			}
		}
	}
	return 0;
}

IEclRecord * CEclProgramManager::FindRecord(const TCHAR *pRecordName)
{
#ifdef _DEBUG
	{
		CUnicodeFile file;
		file.Create("c:\\temp\\record.xml");
		if ( file.IsOpen() )
		{
			std::_tstring xml;
			m_program->SerializeToXml(xml);
			file.Write(xml);
		}
	}
#endif
	ATLASSERT(!"EclRecord::GetIdentifier() has been deprecated");
	//return FindEclElement<IEclRecord>(m_program,pRecordName);
	return 0;
}

static IEclParameter* GetTransformInputParamter(IEclTransform *transform)
{
	ATLASSERT(false);
	/*
	IEclIdentifier *id = transform->GetTargetType();
	if ( id )
	{
	StdString resultType(id->GetIdentifier());
	IEclFunctionDeclaration *decl = transform->GetFunctionDeclaration();
	IEclParameterList *params = decl->GetParameterList();
	if ( params->GetParameterCount() > 0 )
	{
	return params->GetParameter(0);
	}
	}
	*/
	return 0; 
}

static void StripQualifier(std::_tstring &type)
{
	int nDot = type.find('.');
	if ( nDot > 0 )
	{
		type = type.substr(type.length()-(nDot+1), nDot);
	}
}

static void AddImports(const std::_tstring &type, std::_tstring &imports)
{
	int nDot = type.find('.');
	if ( nDot > 0 )
	{
		std::_tstring module( type.substr(0, nDot) );
		boost::algorithm::to_lower(module);
		if ( imports.find(module+ _T(';')) < 0 )
		{
			imports += _T("Import ") + module + _T(";\r\n");
		}
	}
}

//static
void CEclProgramManager::CommentAllCode(std::_tstring& ecl)
{
	ATLASSERT(!"Not Called");
	//if ( !ecl.empty() )
	//{
	//	std::_tstring base(ecl);
	//	boost::algorithm::replace_all(base, _T("\r\n"), _T("\n"));
	//	int nLen = base.length();

	//	ecl.clear();

	//	std::_tstring line;
	//	int pos = 0;
	//	do 
	//	{
	//		line = base.Tokenize(_T("\n"),pos);
	//		ecl += _T("//") + line + _T("\n");
	//	} while ( pos < nLen );
	//}
}

const TCHAR * CEclProgramManager::TransformOutputType(IEclTransform *transform, std::_tstring &type, bool bQualified)
{
	ATLASSERT(false);/*
					 transform->GetTargetQualifiedTypeName(type);
					 if ( !bQualified )
					 {
					 StripQualifier(type);
					 }
					 return type;
					 */
	return NULL;
}

//static
bool CEclProgramManager::GetQualifiedRecordType(IAttribute *attr, std::_tstring &recordType)
{

	CEclProgramManager mgr(attr);
	IEclRecord* record = mgr.FindRecord();
	if ( record )
	{
		recordType = attr->GetModuleLabel();
		recordType += _T(".");
		ATLASSERT(!"CEclRecord::GetIdentifierName() has been deprecated");
		//	std::_tstring name;
		//	recordType += record->GetIdentifierName(name);
		return true;
	}
	return false;
}

//static 
void CEclProgramManager::CreateRecord(IAttribute *layoutAttr, std::_tstring &ecl)
{
	CreateRecord(layoutAttr, _T("\tstring1 col1;"), ecl );
}

void CEclProgramManager::CreateRecord(IAttribute *layoutAttr, const TCHAR *recordItems, std::_tstring &recordEcl)
{
	std::_tstring name( layoutAttr->GetLabel() );
	recordEcl = (boost::_tformat(_T("export %1 := record\r\n%2\r\nend;\r\n")) % name % recordItems).str();
}

template <class T>
static const TCHAR* SprayDFUServer(const T &info, StdString &dfuServer)
{
	if ( info.m_dfuServerIpPort.clear() )
		dfuServer = _T("/*dfu server*/"); //use default
	else
	{
		std::_tstring encoded;
		dfuServer = ::EncodeForEcl(info.m_dfuServerIpPort.c_str(), encoded);
	}

	return dfuServer.c_str();
}

static const TCHAR* SprayRecordSize(const CSprayInfoFixed& info, StdString& recordSize)
{
	if ( !info.m_recordName.empty() )
	{
		Format(recordSize, _T("sizeof(%s)"), info.m_recordName.c_str());
	}
	else
	{
		ATLASSERT(info.m_recordSize>0);
		::IntToString(info.m_recordSize,recordSize);
	}
	return recordSize.c_str();
}

static void EclValue(StdString &target, IEclValue *value)
{
	if ( value->IsDefaultValue() )
	{
		target.clear();
	}
	else if ( value->IsStringLiteral() )
	{
		std::_tstring ecl;
		CString decoded;
		value->Serialize(ecl);
		::DecodeFromEcl(ecl.c_str(),decoded);
		target = decoded;
	}
	else
	{
		std::_tstring ecl;
		target = value->Serialize(ecl);
	}
}

//static 
const TCHAR* CEclProgramManager::ECLBool(bool b)
{
	return b ? _T("true") : _T("false");
}


static void RemoveClusterPrefix(StdString &name, StdString &prefix)
{
	//remove the ~cluster:: prefix from a file name
	if ( name.length() > 3 && name[0] == '~' )
	{
		for (unsigned i=1; i<name.length(); i++)
		{
			if ( name[i] == ':' && name[i-1] == ':' )
			{
				i++;
				prefix = name.substr(0,i);
				name = name.substr(i,name.length()-i);
				break;
			}
		}
	}
}

//static
void CEclProgramManager::GetEclFormatVariable(int headerRows, const StdString& _delimiter, const StdString& _terminator, const StdString& _quote, std::_tstring &format)
{
	//make the Maxlength relatively large, it's just a buffer on the server
	format = (boost::_tformat(_T("CSV(HEADING(%1%),MAXLENGTH(%2%)")) % headerRows % (16*1204) ).str();

	//use default delimiter if none specified
	if ( !_delimiter.empty() )
	{
		CString delimiter;
		::EncodeForEcl(_delimiter.c_str(), delimiter);
		std::_tstring delimiterParam;
		delimiterParam = (boost::_tformat(_T(",SEPARATOR(%1%)")) % delimiter.GetString()).str();
		format += delimiterParam;
	}

	//use default terminator if none found
	if ( !_terminator.empty() )
	{
		CString terminator;
		::EncodeForEcl(_terminator.c_str(), terminator);
		std::_tstring terminatorParam;
		terminatorParam = (boost::_tformat(_T(",TERMINATOR(%1%)")) % terminator.GetString()).str();
		format += terminatorParam;
	}

	//use default terminator if none found
	if ( !_quote.empty() )
	{
		CString quote;
		::EncodeForEcl(_quote.c_str(), quote);
		std::_tstring quoteParam;
		quoteParam = (boost::_tformat(_T(",QUOTE(%1%)")) % quote.GetString()).str();
		format += quoteParam;
	}

	format += _T(")");
}

//static 
void CEclProgramManager::GetEclFormat(CSprayInfoFixed &sprayInfo, std::_tstring& format)
{
	format = _T("Flat");
}
//static 
void CEclProgramManager::GetEclFormat(CSprayInfoVariable &sprayInfo, std::_tstring& format)
{
	GetEclFormatVariable( 0, //headerRows?
		sprayInfo.m_sourceCsvSeparate, 
		sprayInfo.m_sourceCsvTerminate, 
		sprayInfo.m_sourceCsvQuote, format);
}
//static 
void CEclProgramManager::GetEclFormat(CSprayInfoXml &sprayInfo, std::_tstring& format)
{
	format = (boost::_tformat(_T("XML(%1%)")) % sprayInfo.m_sourceRowTag).str();
}

