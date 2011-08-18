#pragma once

#include "clib.h"
#include "XMLWriter.h"

// DO NOT CHANGE THIS GUID
// {934319fd-45b6-4d1d-88e3-569a1b8c739d}
const GUID guidDefault = 
	{ 0x934319fd, 0x45b6, 0x4d1d, 0x88, 0xe3, 0x56, 0x9a, 0x1b, 0x8c, 0x73, 0x9d };

class CLIB_API CGuid
{
protected:
	GUID m_guid;
public:
	CGuid();
	CGuid(const GUID&);
	const CGuid& operator= (const CGuid& guid);
	const CGuid& operator= (const GUID& guid);
	const CGuid& operator= (LPCSTR str);
	bool operator== (const CGuid& guid) const;
	const TCHAR* ToString(std::_tstring& retVal) const;
	const bool isEmpty() const;
	const GUID& GetGUID() const;
};

class CLIB_API CCategory
{
protected:
	CGuid m_plugin;
	int m_category;
	mutable unsigned int m_hash_id;

public:
	CCategory();
	//CCategory(int category);
	CCategory(const GUID& plugin, int category);
	CCategory(const CCategory & c);
	const CCategory & operator = (const CCategory & c);

	bool isEmpty() const;
	bool operator == (const CCategory & c) const;
	bool operator != (const CCategory & c) const;
	bool operator < (const CCategory & c) const;
	const CGuid& GetPlugin() const;
	int GetCategory() const;
	unsigned int GetHash() const;
	void Serialize(IXMLWriter * writer, const std::_tstring & label = _T("CCategory")) const;
};

class CLIB_API CUniqueID 
{
protected:
	CCategory m_category;
	std::_tstring m_id;
	mutable unsigned int m_hash_id;

public:
	CUniqueID();
	CUniqueID(const CCategory & cat, const std::_tstring & id);
	CUniqueID(const GUID& plugin, int cat, const std::_tstring & id);
	//CUniqueID(const int cat, const std::_tstring & id);
	CUniqueID(const CUniqueID & c);
	const CUniqueID & operator = (const CUniqueID & c);

	bool isEmpty() const;
	bool operator == (const CUniqueID & c) const;
	bool operator != (const CUniqueID & c) const;
	bool operator < (const CUniqueID & c) const;
	const CGuid& GetPlugin() const;
	const CCategory & GetCategory() const;
	const TCHAR * GetID() const;
	unsigned int GetHash() const;
	void Serialize(IXMLWriter * writer, const std::_tstring & label = _T("CCategory")) const;
};
