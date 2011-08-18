#include "stdafx.h"
#include "..\en_us\resource.h"

#include "langref.h"

#include "global.h"
#include "util.h"
#include "UnicodeFile.h"
#include "Repository.h"

const TCHAR * const LANGREFFILE_XML = _T("LanguageReference.xml");
const TCHAR * const LANGREFFILE_CSV = _T("LanguageReference.csv");
const TCHAR * const LANGCOLFILE = _T("LanguageColor.xml");
const TCHAR * const LANGCOLFILE2 = _T("LanguageColor2.xml");

struct RowCatStruct
{
	int m_row;
	int m_cat;
};

typedef std::map<std::_tstring, RowCatStruct> NameMap;
typedef std::map<int, NameMap> CatNameMap;

//  ===========================================================================
template<typename T>
void save(const T &s, const char * filename)
{
	// make an archive
	std::ofstream ofs(filename);
	assert(ofs.good());
	boost::archive::xml_oarchive oa(ofs);
	oa << BOOST_SERIALIZATION_NVP(s);
}

template<typename T>
void restore(T &s, const char * filename)
{
	// open the archive
	std::ifstream ifs(filename);
	assert(ifs.good());
	try
	{
		boost::archive::xml_iarchive ia(ifs);
		// restore the schedule from the archive
		ia >> BOOST_SERIALIZATION_NVP(s);
	}
	catch(boost::archive::archive_exception &)
	{
		ATLASSERT(false);
//		boost::filesystem::remove();
	}
}
//  ===========================================================================
class CLanguageColor
{
protected:
	//  Version 1  ---
	std::_tstring categoryid;
	std::_tstring name;
	std::_tstring fore;
	std::_tstring back;
	std::_tstring font;
	std::_tstring size;
	std::_tstring bold;
	//  --- --- ---

public:
	CLanguageColor()
	{
	}

	bool Cleanup(const CLanguageColor * const other)
	{
		if (boost::algorithm::equals(categoryid, other->categoryid))
			return false;

		bool retVal = false;
		if (boost::algorithm::equals(fore, other->fore))
		{
			fore = _T("");
			retVal = true;
		}
		if (boost::algorithm::equals(back, other->back))
		{
			back = _T("");
			retVal = true;
		}
		if (boost::algorithm::equals(font, other->font))
		{
			font = _T("");
			retVal = true;
		}
		if (boost::algorithm::equals(size, other->size))
		{
			size = _T("");
			retVal = true;
		}
		if (boost::algorithm::equals(bold, other->bold))
		{
			bold = _T("");
			retVal = true;
		}
		return retVal;
	}

	bool operator != (const CLanguageColor & other)
	{
		ATLASSERT(GetCategoryID() == other.GetCategoryID());
		if (!boost::algorithm::equals(name, other.name))
			return true;
		if (!boost::algorithm::equals(fore, other.fore))
			return true;
		if (!boost::algorithm::equals(back, other.back))
			return true;
		if (!boost::algorithm::equals(font, other.font))
			return true;
		if (!boost::algorithm::equals(size, other.size))
			return true;
		if (!boost::algorithm::equals(bold, other.bold))
			return true;
		return false;
	}

	bool Has(const std::_tstring & item) const
	{
		return item.length() > 0;
	}
	template<typename T>
	T Get(const std::_tstring & item, T defaultVal) const
	{
		try
		{
			return boost::lexical_cast<T>(item);
		}
		catch(boost::bad_lexical_cast &)
		{
		}
		return defaultVal;
	}
	template<typename T>
	void Set(std::_tstring & item, T val)
	{
		try
		{
			item = boost::lexical_cast<std::_tstring>(val);
		}
		catch(boost::bad_lexical_cast &)
		{
		}
	}
	bool HasCategoryID() const		{return Has(categoryid);}
	int GetCategoryID() const		{return Get(categoryid, -1);}
	bool HasName() const			{return Has(name);}
	const TCHAR * GetName() const	{return name.c_str();}
	bool HasFore() const			{return Has(fore);}
	int GetFore() const				{return Get(fore, 0x00000000);}
	void SetFore(int color)			{Set(fore, color);}
	bool HasBack() const			{return Has(back);}
	int GetBack() const				{return Get(back, 0x00ffffff);}
	void SetBack(int color)			{Set(back, color);}
	bool HasFont() const			{return Has(font);}
	const TCHAR * GetFont() const	{return font.c_str();}
	void SetFont(const std::_tstring & name) {Set(font, name);}
	bool HasSize() const			{return Has(size);}
	int GetSize() const				{return Get(size, 10);}
	void SetSize(int _size)			{return Set(size, _size);}
	bool HasBold() const			{return Has(bold);}
	bool GetBold() const			{return Get(bold, false);}
	void SetBold(bool _bold)		{return Set(bold, _bold);}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar	& BOOST_SERIALIZATION_NVP(categoryid)
			& BOOST_SERIALIZATION_NVP(name)
			& BOOST_SERIALIZATION_NVP(fore)
			& BOOST_SERIALIZATION_NVP(back)
			& BOOST_SERIALIZATION_NVP(font)
			& BOOST_SERIALIZATION_NVP(size)
			& BOOST_SERIALIZATION_NVP(bold);
	}
};
BOOST_CLASS_VERSION(CLanguageColor, 0)
typedef std::pair<int, CLanguageColor> CategoryLanguageColorPair;
typedef std::map<int, CLanguageColor> CategoryLanguageColorMap;
typedef std::vector<int> RowCategoryIDVector;
//  ===========================================================================
class CLanguageReference
{
protected:
	//  Version 1  ---
	std::_tstring categoryid;
	std::_tstring name;
	std::_tstring tooltip;
	std::_tstring insertbeforecursor;
	std::_tstring insertaftercursor;
	std::_tstring canbefollowedby;
	//  --- --- ---

public:
	CLanguageReference()
	{
	}
	bool Has(const std::_tstring & item) const
	{
		return item.length() > 0;
	}
	template<typename T>
	T Get(const std::_tstring & item, T defaultVal) const
	{
		try
		{
			return boost::lexical_cast<T>(item);
		}
		catch(boost::bad_lexical_cast &)
		{
		}
		return defaultVal;
	}
	template<typename T>
	void Set(std::_tstring & item, T val)
	{
		try
		{
			item = boost::lexical_cast<std::_tstring>(val);
		}
		catch(boost::bad_lexical_cast &)
		{
		}
	}
	bool HasCategoryID() const					{return Has(categoryid);}
	int GetCategoryID() const					{return Get(categoryid, -1);}
	bool HasName() const						{return Has(name);}
	const TCHAR * GetName() const				{return name.c_str();}
	bool HasTooltip() const						{return Has(tooltip);}
	const TCHAR * GetTooltip() const			{return tooltip.c_str();}
	bool HasInsertBeforeCursor() const			{return Has(insertbeforecursor);}
	const TCHAR * GetInsertBeforeCursor() const	{return insertbeforecursor.c_str();}
	bool HasInsertAfterCursor() const			{return Has(insertaftercursor);}
	const TCHAR * GetAfterBeforeCursor() const	{return insertaftercursor.c_str();}
	bool HasCanBeFollowedBy() const				{return Has(canbefollowedby);}
	const TCHAR * GetCanBeFollowedBy() const	{return canbefollowedby.c_str();}

	CLanguageReference(const std::vector<std::_tstring> & fields)
	{
		//  Keep in sync with "ToCSV(std::_tstring & line)" ---
		switch(fields.size())
		{
		case 6:
			canbefollowedby = fields[5];
		case 5:
			insertaftercursor = fields[4];
		case 4:
			insertbeforecursor = fields[3];
		case 3:
			tooltip = fields[2];
		case 2:
			name = fields[1];
		case 1:
			categoryid = fields[0];
		}
	}
	const TCHAR * ToCSV(std::_tstring & line)
	{
		//  Keep in sync with "CLanguageReference(const std::vector<std::_tstring> & fields)" ---
		std::_tstring encoded;
		line += EncodeForCSV(categoryid.c_str(), encoded);			line += _T(",");
		line += EncodeForCSV(name.c_str(), encoded);				line += _T(",");
		line += EncodeForCSV(tooltip.c_str(), encoded);				line += _T(",");
		line += EncodeForCSV(insertbeforecursor.c_str(), encoded);	line += _T(",");
		line += EncodeForCSV(insertaftercursor.c_str(), encoded);	line += _T(",");
		line += EncodeForCSV(canbefollowedby.c_str(), encoded);	
		return line.c_str();
	}

	bool operator < (const CLanguageReference & other)
	{
		if (GetCategoryID() == other.GetCategoryID())
			return name < other.name;
		return GetCategoryID() < other.GetCategoryID();
	}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar	& BOOST_SERIALIZATION_NVP(categoryid)
			& BOOST_SERIALIZATION_NVP(name)
			& BOOST_SERIALIZATION_NVP(tooltip)
			& BOOST_SERIALIZATION_NVP(insertbeforecursor)
			& BOOST_SERIALIZATION_NVP(insertaftercursor)
			& BOOST_SERIALIZATION_NVP(canbefollowedby);
	}
};
BOOST_CLASS_VERSION(CLanguageReference, 0)
typedef std::vector<CLanguageReference> CLanguageReferenceVector;
//  ===========================================================================
class CLangRef : public ILangRef, public CUnknown
{
protected:
	CLanguageReferenceVector m_lang;
	NameMap m_langNameIndex;
	CatNameMap m_langCatNameIndex;

	CategoryLanguageColorMap m_color;
	RowCategoryIDVector m_colorRowToCategoryID;
	CLanguageColor * m_defaultColor;

	std::_tstring m_emptyStr;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CLangRef()
	{
		m_defaultColor = NULL;
	}

	~CLangRef()
	{
	}

	void init()
	{
		loadReference(LANGREFFILE_XML, m_lang);
		loadMergedColor();
		m_emptyStr = _T("");
	}

	void loadMergedColor()
	{
		loadDefaultColor(LANGCOLFILE, m_color);

		m_defaultColor = NULL;
		m_colorRowToCategoryID.clear();
		for(CategoryLanguageColorMap::iterator itr = m_color.begin(); itr != m_color.end(); ++itr)
		{
			m_colorRowToCategoryID.push_back(itr->second.GetCategoryID());
			if (m_defaultColor == NULL && boost::algorithm::iequals(_T("Default"), itr->second.GetName()))
				m_defaultColor = &itr->second;
		}

		//  Quick Cleanup
		bool found = false;
		for(CategoryLanguageColorMap::iterator itr = m_color.begin(); itr != m_color.end(); ++itr)
		{
			if (itr->second.Cleanup(m_defaultColor))
				found = true;
		}
		if (found)
		{
#ifdef _DEBUG
			save(m_color, "c:\\temp\\tmp.xml");
#endif
		}

		//  Check if this user has customized the colors
		boost::filesystem::path appFolder;
		GetApplicationFolder(appFolder);
		boost::filesystem::path file = appFolder / boost::filesystem::path(CT2A(LANGCOLFILE2), boost::filesystem::native);
		if (boost::filesystem::exists(file))
		{
			CategoryLanguageColorMap userColors;
			restore(userColors, file.native_file_string().c_str());
			for(CategoryLanguageColorMap::iterator itr = userColors.begin(); itr != userColors.end(); ++itr)
			{
				m_color[itr->first] = itr->second;
			}
		}
	}

	bool loadDefaultColor(const TCHAR * fileName, CategoryLanguageColorMap & t)
	{
		t.clear();
		//  Check if this user has customized the colors
		boost::filesystem::path binFolder;
		GetProgramFolder(binFolder);
		boost::filesystem::path file = binFolder / boost::filesystem::path(CT2A(fileName), boost::filesystem::native);
		if (!boost::filesystem::exists(file))
		{
			return false;
		}
		restore(t, file.native_file_string().c_str());
		return true;
	}
	void loadReference(const TCHAR * fileName)
	{
		loadReference(fileName, m_lang);
	}
	void loadReference(const TCHAR * fileName, CLanguageReferenceVector & t)
	{
		t.clear();
		//  Check if this user has customized the colors
		boost::filesystem::path binFolder;
		GetProgramFolder(binFolder);
		boost::filesystem::path file = binFolder / boost::filesystem::path(CT2A(fileName), boost::filesystem::native);
		if (!boost::filesystem::exists(file))
		{
			ATLASSERT(!"Unable to locate LanguageReference.xml");
		}
		restore(t, file.native_file_string().c_str());
		std::sort(t.begin(), t.end());
		int row = 0;
		for(CLanguageReferenceVector::iterator itr = m_lang.begin(); itr != m_lang.end(); ++itr)
		{
			std::_tstring name = itr->GetName();
			std::transform(name.begin(), name.end(), name.begin(), std::tolower);
			m_langNameIndex[name].m_cat = itr->GetCategoryID();
			m_langNameIndex[name].m_row = row;
			m_langCatNameIndex[itr->GetCategoryID()][name].m_cat = itr->GetCategoryID();
			m_langCatNameIndex[itr->GetCategoryID()][name].m_row = row;
			++row;
		}
	}

	void Save()
	{
		boost::filesystem::path appFolder;
		GetApplicationFolder(appFolder);
		boost::filesystem::path file = appFolder / boost::filesystem::path(CT2A(LANGCOLFILE2), boost::filesystem::native);

		CategoryLanguageColorMap defaultColors, diffColors;
		loadDefaultColor(LANGCOLFILE, defaultColors);
		for(CategoryLanguageColorMap::iterator itr = m_color.begin(); itr != m_color.end(); ++itr)
		{
			if (defaultColors[itr->first] != itr->second)
			{
				diffColors[itr->first] = itr->second;
				diffColors[itr->first].Cleanup(&defaultColors[itr->first]);
			}
		}

		save(diffColors, file.native_file_string().c_str());
	}

	void Restore()
	{
		loadMergedColor();
	}

	void RestoreDefaults()
	{
		boost::filesystem::path appFolder;
		GetApplicationFolder(appFolder);
		boost::filesystem::path file = appFolder / boost::filesystem::path(CT2A(LANGCOLFILE2), boost::filesystem::native);
		if (boost::filesystem::exists(file))
			boost::filesystem::remove(file);
		loadMergedColor();
	}

	int GetLangCatCount()
	{
		int retVal = 0;
		for(int i = 0; i < GetLangCount(); ++i)
		{
			if (GetLangCat(i) > retVal)
				retVal = GetLangCat(i);
		}
		return retVal;
	}

	int GetLangCount()
	{
		return static_cast<int>(m_lang.size());
	}

	int GetLangCat(int row)
	{
		return m_lang[row].GetCategoryID();
	}

	const TCHAR * GetLangName(int row)
	{
		return m_lang[row].GetName();
	}

	const TCHAR * GetLangTooltip(int row)
	{
		return m_lang[row].GetTooltip();
	}

	int GetLangNameCount(int cat) const
	{
		CatNameMap::const_iterator found = m_langCatNameIndex.find(cat);
		if (found == m_langCatNameIndex.end())
			return 0;
		return found->second.size();
	}

	const TCHAR * GetLangName(int cat, int row) const
	{
		CatNameMap::const_iterator found = m_langCatNameIndex.find(cat);
		if (found == m_langCatNameIndex.end())
			return m_emptyStr.c_str();;

		for(NameMap::const_iterator itr = found->second.begin(); itr != found->second.end(); ++itr)
		{
			if (--row < 0)
				return itr->first.c_str();
		}
		return m_emptyStr.c_str();
	}

	int GetLangNameCatID(int cat, int row) const
	{
		CatNameMap::const_iterator found = m_langCatNameIndex.find(cat);
		if (found == m_langCatNameIndex.end())
		{
			ATLASSERT(false);
			return -1;
		}

		for(NameMap::const_iterator itr = found->second.begin(); itr != found->second.end(); ++itr)
		{
			if (--row < 0)
				return itr->second.m_cat;
		}
		ATLASSERT(false);
		return -1;
	}

	CString & GetLangNames(int cat, CString &result)
	{
		result = _T("");
		for(NameMap::iterator itr = m_langCatNameIndex[cat].begin(); itr != m_langCatNameIndex[cat].end(); ++itr)
		{
			if (result.GetLength())
				result += _T(" ");
			result += itr->first.c_str();
		}
		result.MakeLower();
		return result;
	}

	bool GetLangNamesAutoC(int /*cat*/, StdStringVector &set)
	{
		for(int i = 0; i < GetLangCount(); ++i)
			set.push_back(std::_tstring(GetLangName(i)) + _T("?2"));
		return true;
	}

	bool GetAutoC(const std::_tstring & partialLabel, StdStringVector &set)
	{
		CComPtr<IRepository> rep = AttachRepository();
		rep->GetModulesAutoC(partialLabel, set);
		if (!partialLabel.empty())
			rep->GetAttributesAutoC(partialLabel, set);
		return true;
	}

	const TCHAR * GetLangTooltip(const std::_tstring &label)
	{
		std::_tstring lower = label;
		std::transform(label.begin(), label.end(), lower.begin(), std::tolower);
		if (m_langNameIndex.find(lower) != m_langNameIndex.end())
			return GetLangTooltip(m_langNameIndex[lower].m_row);
		return m_emptyStr.c_str();
	}

	bool IsComment(int cat)
	{
		switch (cat)
		{
		case 1:	//Comment
		case 2:	//CommentLine
		case 15://CommentDoc
		case 17://CommentDocKeyword
		case 18://CommentDocKeywordError
			return true;
		}
		return false;
	}
	
	int GetColorRowCount() const
	{
		return static_cast<int>(m_color.size());
	}
	int GetColorCatID(int row) const
	{
		ATLASSERT((unsigned int)row >= 0 && (unsigned int)row < m_colorRowToCategoryID.size());
		return m_colorRowToCategoryID[row];
	}
	int GetColorRow(int catID) const
	{
		int retVal = 0;
		for(RowCategoryIDVector::const_iterator itr = m_colorRowToCategoryID.begin(); itr != m_colorRowToCategoryID.end(); ++itr)
		{
			if (*itr == catID)
				return retVal;
			++retVal;
		}
		ATLASSERT(!"Uniknown catID");
		return -1;
	}
	const TCHAR * GetColorName(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		ATLASSERT(itr->second.HasName());
		return itr->second.GetName();
	}
	int GetColorFore(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		return itr->second.HasFore() ? itr->second.GetFore() : m_defaultColor->GetFore();
	}
	void SetColorFore(int catID, int color)
	{
		CategoryLanguageColorMap::iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		itr->second.SetFore(color);
	}
	int GetColorBack(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		return itr->second.HasBack() ? itr->second.GetBack() : m_defaultColor->GetBack();
	}
	void SetColorBack(int catID, int color)
	{
		CategoryLanguageColorMap::iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		itr->second.SetBack(color);
	}
	const TCHAR * GetFontName(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		return itr->second.HasFont() ? itr->second.GetFont() : m_defaultColor->GetFont();
	}
	void SetFontName(int catID, const std::_tstring & val)
	{
		CategoryLanguageColorMap::iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		itr->second.SetFont(val);
	}
	int GetFontSize(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		return itr->second.HasSize() ? itr->second.GetSize() : m_defaultColor->GetSize();
	}
	void SetFontSize(int catID, int size)
	{
		CategoryLanguageColorMap::iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		itr->second.SetSize(size);
	}
	bool GetFontBold(int catID) const
	{
		CategoryLanguageColorMap::const_iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		return itr->second.HasBold() ? itr->second.GetBold() : m_defaultColor->GetBold();
	}
	void SetFontBold(int catID, bool bold)
	{
		CategoryLanguageColorMap::iterator itr = m_color.find(catID);
		ATLASSERT(itr != m_color.end());
		itr->second.SetBold(bold);
	}
	void ExportLanguageReference()
	{
		boost::filesystem::path binFolder;
		GetProgramFolder(binFolder);
		boost::filesystem::path path = binFolder / boost::filesystem::path(CT2A(LANGREFFILE_CSV), boost::filesystem::native);

		std::ofstream ofs(path.native_file_string().c_str());
		assert(ofs.good());

		for(CLanguageReferenceVector::iterator itr = m_lang.begin(); itr != m_lang.end(); ++itr)
		{
			std::_tstring line;
			ofs << CT2A(itr->ToCSV(line)) << std::endl;
		}
	}
	void ImportLanguageReference()
	{
		m_lang.clear();
		boost::filesystem::path binFolder;
		GetProgramFolder(binFolder);
		boost::filesystem::path path = binFolder / boost::filesystem::path(CT2A(LANGREFFILE_CSV), boost::filesystem::native);
		boost::filesystem::path xml_path = binFolder / boost::filesystem::path(CT2A(LANGREFFILE_XML), boost::filesystem::native);

		CUnicodeFile file;
		if (file.Open(CA2T(path.native_file_string().c_str())))
		{
			std::_tstring data;
			file.Read(data);

			typedef std::vector<std::_tstring> split_vector_type;
			split_vector_type SplitVec; 
			boost::algorithm::split(SplitVec, data, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

			for (split_vector_type::const_iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
			{
				std::_tstring line = *itr;
				if (line.length())
				{
					std::vector<std::_tstring> fields;
					DecodeCSV(line, fields);
					m_lang.push_back(fields);
				}
			}
		}
		save(m_lang, xml_path.native_file_string().c_str());
	}
};

StlLinked<CLangRef> g_langRef;

ILangRef * CreateEclLangRef()
{
	if (!g_langRef)
	{
		g_langRef = new CLangRef;
		g_langRef->init();
	}
	return g_langRef;
};

void ExportLangRef()
{
	CComPtr<CLangRef> retVal = new CLangRef;
	retVal->loadReference(LANGREFFILE_XML);
	retVal->ExportLanguageReference();
}

void ImportLangRef()
{
	CComPtr<CLangRef> retVal = new CLangRef;
	retVal->init();
	retVal->ImportLanguageReference();
}


