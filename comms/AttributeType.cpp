#include "StdAfx.h"

#include <Wildcard.h>
#include "AttributeType.h"
#include "cache.h"
#include <UtilFilesystem.h>

//  ===========================================================================
#define ATTRIBUTE_TYPE_ECL _T("ecl")
#define ATTRIBUTE_TYPE_ECLMOD _T("eclmod")
#define ATTRIBUTE_TYPE_ECLLIB _T("ecllib")
#define ATTRIBUTE_TYPE_ECLOTHER _T("ecl*")
#define ATTRIBUTE_TYPE_PLUGIN _T("dll")
#define ATTRIBUTE_TYPE_ESDL _T("esdl")
#define ATTRIBUTE_TYPE_SALT _T("salt")
#define ATTRIBUTE_TYPE_XSLT _T("xslt")
#define ATTRIBUTE_TYPE_WORKSPACE _T("ecl_ws")

class CAttributeType : public IAttributeType, public CUnknown
{
protected:
	std::_tstring m_repositoryCode;
	std::_tstring m_fileExtension;
	std::_tstring m_description;
	std::_tstring m_defaultText;

public:
	CAttributeType(const std::_tstring & repositoryCode) : m_repositoryCode(repositoryCode)
	{
		boost::algorithm::trim_left_if(m_repositoryCode, boost::algorithm::is_any_of(_T(".")));
		if (m_repositoryCode.empty()) 
		{
			m_repositoryCode = ATTRIBUTE_TYPE_ECL;
		}
	}

	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	const TCHAR * GetCacheID() const
	{
		return m_repositoryCode.c_str();
	}
	const TCHAR * GetRepositoryCode()
	{
		return m_repositoryCode.c_str();
	}
	const TCHAR * GetFileExtension()
	{
		if (m_fileExtension.empty())
		{
			m_fileExtension = _T(".");
			m_fileExtension += GetRepositoryCode();
		}
		return m_fileExtension.c_str();
	}
	const TCHAR * GetDescription()
	{
		if(m_description.empty())
		{
			if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECL))
				m_description = _T("ECL - Enterprise Control Language");
			else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ESDL))
				m_description = _T("ESDL - Enterprise Services Definition Language");
			else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_SALT))
				m_description = _T("SPC - Salt Definition File");
			else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_WORKSPACE))
				m_description = _T("ECL_WS - ECL Workspace Persistance File");
			else
				m_description = _T("???");
		}
		return m_description.c_str();
	}
	const TCHAR * GetDefaultText(const std::_tstring & attrLabel)
	{
		if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECL))
			m_defaultText = (boost::_tformat(_T("EXPORT %1% := 'todo';")) % attrLabel.c_str()).str();
		else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ESDL))
			m_defaultText = (boost::_tformat(_T("ESDL:  %s;")) % attrLabel.c_str()).str();
		else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_SALT))
			m_defaultText = (boost::_tformat(_T("SALT:  %s;")) % attrLabel.c_str()).str();
		else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_WORKSPACE))
			m_defaultText = (boost::_tformat(_T("ECL_WS:  %s;")) % attrLabel.c_str()).str();
		else
			m_defaultText = _T("???");

		return m_defaultText.c_str();
	}
	bool IsReadOnly()
	{
		if (boost::algorithm::iends_with(m_repositoryCode, ATTRIBUTE_TYPE_PLUGIN))
			return true;
		return false;
	}

	void Update(const std::_tstring & description)
	{
		if (!description.empty())
			m_description = description;
	}
};

//  ===========================================================================
static CacheT<std::_tstring, CAttributeType> AttributeTypeCache;

void ClearAttributeTypeCache()
{
	ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), AttributeTypeCache.Size());
	AttributeTypeCache.Clear();
}

IAttributeType * CreateIAttributeType(const std::_tstring & repositoryType, const std::_tstring & description)
{
	CAttributeType * retVal = AttributeTypeCache.Get(new CAttributeType(repositoryType));
	retVal->Update(description);
	return retVal;
}
IAttributeType * CreateIAttributeECLType()
{
	return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_ECL));
}
IAttributeType * CreateIAttributeESDLType()
{
	return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_ESDL));
}
IAttributeType * CreateIAttributeSALTType()
{
	return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_SALT));
}
bool IsValidExtension(const std::_tstring & ext)
{
	if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECL))
		return true;
	else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECLMOD))
		return true;
	else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECLLIB))
		return true;
	else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ESDL))
		return true;
	else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_SALT))
		return true;
	else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_PLUGIN))
		return true;
	else if (WildMatch(ext, std::_tstring(_T(".")) + ATTRIBUTE_TYPE_ECLOTHER, true))
		return true;
	return false;
}
COMMS_API bool HasValidExtension(const std::_tstring & filename)
{
	return IsValidExtension(pathToWString(boost::filesystem::extension(filename)));
}
