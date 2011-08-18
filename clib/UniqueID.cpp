#include "StdAfx.h"

#include "Uniqueid.h"

static boost::crc_32_type computer;

CGuid::CGuid()
{
	memset(&m_guid, 0, sizeof(m_guid));
}

CGuid::CGuid(const GUID& guid)
{
	m_guid = guid;
}

const CGuid& CGuid::operator= (const CGuid& guid)
{
	m_guid = guid.GetGUID();
	return *this;
}

const CGuid& CGuid::operator= (const GUID& guid)
{
	m_guid = guid;
	return *this;
}

// NOTE: will not handle a formatted string, and directly writes to the GUID struct
//  > (BYTE*)&m_guid
const CGuid& CGuid::operator= (LPCSTR str)
{
	int size = AtlHexDecodeGetRequiredLength(strlen(str));
	AtlHexDecode(str, strlen(str), (BYTE*)&m_guid, &size);
	ATLASSERT(size == 16);
	return *this;
}

bool CGuid::operator== (const CGuid& guid) const
{
	return (m_guid == guid.GetGUID()) != 0;
}

const TCHAR* CGuid::ToString(std::_tstring& retVal) const
{
	char buff[256];
	int size = AtlHexEncodeGetRequiredLength(sizeof(m_guid));
	//ATLASSERT(size == 32);
	AtlHexEncode((BYTE*)&m_guid, sizeof(m_guid), buff, &size);
	buff[size] = 0;
	retVal = CA2T(buff, CP_UTF8);
	return retVal.c_str();
}

const bool CGuid::isEmpty() const
{
	bool isEmpty = (m_guid.Data1 == 0 && m_guid.Data2 == 0 && m_guid.Data3 == 0 && m_guid.Data4[0] == 0);
	return isEmpty;
}

const GUID& CGuid::GetGUID() const
{
	return m_guid;
}

//  ===========================================================================
CCategory::CCategory() : m_category(-1), m_hash_id(0)
{
}

//CCategory::CCategory(int category) : m_plugin(defaultGuid), m_category(category), m_hash_id(0)
//{
//}

CCategory::CCategory(const GUID& plugin, int category)
	: m_plugin(plugin), m_category(category), m_hash_id(0)
{
}

CCategory::CCategory(const CCategory & c) : m_plugin(c.m_plugin), m_category(c.m_category), m_hash_id(c.m_hash_id)
{
}

const CCategory & CCategory::operator = (const CCategory & c)
{
	m_plugin = c.m_plugin;
	m_category = c.m_category;
	m_hash_id = c.m_hash_id;
	return *this;
}

bool CCategory::operator < (const CCategory & c) const
{
	return (GetHash() < c.GetHash());
}

bool CCategory::isEmpty() const
{
	return (m_plugin.isEmpty() && m_category == -1);
}

bool CCategory::operator == (const CCategory & c) const
{
	return (GetHash() == c.GetHash());
}

bool CCategory::operator != (const CCategory & c) const
{
	return (GetHash() != c.GetHash());
}

const CGuid& CCategory::GetPlugin() const
{
	return m_plugin;
}

int CCategory::GetCategory() const
{
	return m_category;
}

unsigned int CCategory::GetHash() const
{
	if (!m_hash_id)
	{
		computer.reset();
		computer.process_bytes(&m_plugin, sizeof(m_plugin));
		computer.process_bytes(&m_category, sizeof(m_category));
		m_hash_id = computer.checksum();
	}
	return m_hash_id;
}

void CCategory::Serialize(IXMLWriter * writer, const std::_tstring & label) const
{
	writer->PushElement(label);
	std::_tstring pluginStr;
	writer->PushElement(_T("Plugin"), m_plugin.ToString(pluginStr));
	writer->PopElement();
	writer->PushElement(_T("Category"), boost::lexical_cast<std::_tstring>(m_category).c_str());
	writer->PopElement();
}

//  ===========================================================================
CUniqueID::CUniqueID() : m_hash_id(0)
{
}

CUniqueID::CUniqueID(const CCategory & category, const std::_tstring & id) : m_category(category), m_id(id), m_hash_id(0)
{
}

CUniqueID::CUniqueID(const GUID& plugin, int category, const std::_tstring & id) : m_category(plugin, category), m_id(id), m_hash_id(0)
{
}

//CUniqueID::CUniqueID(const int category, const std::_tstring & id) : m_category(category), m_id(id), m_hash_id(0)
//{
//}

CUniqueID::CUniqueID(const CUniqueID & c) : m_category(c.m_category), m_id(c.m_id), m_hash_id(c.m_hash_id)
{
}

const CUniqueID & CUniqueID::operator = (const CUniqueID & c)
{
	m_category = c.m_category;
	m_id = c.m_id;
	m_hash_id = c.m_hash_id;
	return *this;
}

bool CUniqueID::operator < (const CUniqueID & c) const
{
	return (GetHash() < c.GetHash());
}

bool CUniqueID::isEmpty() const
{
	return (m_category.isEmpty() && m_id.length() == 0);
}

bool CUniqueID::operator == (const CUniqueID & c) const
{
	return (GetHash() == c.GetHash());
}

bool CUniqueID::operator != (const CUniqueID & c) const
{
	return (GetHash() != c.GetHash());
}

const CGuid& CUniqueID::GetPlugin() const
{
	return m_category.GetPlugin();
}

const CCategory & CUniqueID::GetCategory() const
{
	return m_category;
}

const TCHAR * CUniqueID::GetID() const
{
	return m_id.c_str();
}

unsigned int CUniqueID::GetHash() const
{
	if (!m_hash_id)
	{
		unsigned int cat_hash = m_category.GetHash();
		computer.reset();
		computer.process_bytes(&cat_hash, sizeof(unsigned int));
		computer.process_bytes(m_id.c_str(), m_id.length() * sizeof(TCHAR));
		m_hash_id = computer.checksum();
	}
	return m_hash_id;
}

void CUniqueID::Serialize(IXMLWriter * writer, const std::_tstring & label) const
{
	writer->PushElement(label);
	m_category.Serialize(writer);
	writer->PushElement(_T("ID"), m_id);
	writer->PopElement();
}

//  ===========================================================================
