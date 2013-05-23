#include "StdAfx.h"

#include "persistmap.h"
#include "logger.h"
#include "util.h"

static const TCHAR * const BlankString = _T("");

CPersistMap::CPersistMap()
{
}
void CPersistMap::Set(const std::_tstring & key, const std::_tstring & value)
{
	m_map[key] = value;
}
const TCHAR * CPersistMap::Get(const std::_tstring & key) const
{
	PersistMap::const_iterator itr = m_map.find(key);
	if (itr != m_map.end())
		return itr->second.c_str();
	return BlankString;
}
bool CPersistMap::HasContent() const
{
	return m_map.size() > 0;
}
bool CPersistMap::serialize(std::_tstring & resultXml) const
{
	CComInitialize com;
	CComPtr<IMXWriter> pWriter;
	HRESULT hr = pWriter.CoCreateInstance(CLSID_MXXMLWriter30, NULL);
	_ASSERT(SUCCEEDED(hr));
	pWriter->put_omitXMLDeclaration(VARIANT_TRUE);

	// create attributes
	CComPtr<IMXAttributes> pMXAttrs;  
	hr = pMXAttrs.CoCreateInstance(CLSID_SAXAttributes30, NULL);
	_ASSERT(SUCCEEDED(hr));

	CComQIPtr<ISAXAttributes> pSAXAttrs(pMXAttrs);

	// set them all to writer, writer implements all these interfaces
	CComQIPtr<ISAXContentHandler> pContentHandler(pWriter);
	CComQIPtr<ISAXDTDHandler> pDTDHandler(pWriter);
	CComQIPtr<ISAXLexicalHandler> pLexicalHandler(pWriter);
	CComQIPtr<ISAXDeclHandler> pDeclHandler(pWriter);
	CComQIPtr<ISAXErrorHandler> pErrorHandler(pWriter);

	CComVariant var(L"");
	hr = pWriter->put_output(var);
	_ASSERT(SUCCEEDED(hr));

	// and manually call necessary events to generate XML file
	pContentHandler->startDocument();
	pMXAttrs->clear();

	CComBSTR empty(L""), map(L"map");
	pContentHandler->startElement(empty, empty.Length(), empty, empty.Length(), map, map.Length(), pSAXAttrs);
	for(PersistMap::const_iterator itr = m_map.begin(); itr != m_map.end(); ++itr)
	{
		CComBSTR first = itr->first.c_str();
		CComBSTR second = itr->second.c_str();
		pContentHandler->startElement(empty, empty.Length(), empty, empty.Length(), first, first.Length(), pSAXAttrs);
		CString content(second, second.Length());
		CComBSTR utf8content = CA2W(CT2A(content, CP_UTF8));
		pContentHandler->characters(second, second.Length());
		pContentHandler->endElement(empty, empty.Length(),	empty, empty.Length(), first, first.Length());
	}
	pContentHandler->endElement(empty, empty.Length(), empty, empty.Length(), map, map.Length());

	// output XML
	hr = pWriter->get_output(&var);
	_ASSERT(SUCCEEDED(hr));
	resultXml = CString(var);

	return true;
}

bool CPersistMap::deserializeXML(const std::_tstring & xml)
{
	return deserialize(xml, false);
}

bool CPersistMap::deserialize(const std::_tstring & path)
{
	return deserialize(path, true);
}

bool CPersistMap::deserialize(const std::_tstring & xml_path, bool isPath)
{
	if (isPath && !boost::filesystem::exists(boost::filesystem::path(xml_path, boost::filesystem::native)))
		return false;

	CComInitialize com;
	CComPtr<IXMLDOMDocument> pDoc;
	HRESULT hRet = pDoc.CoCreateInstance(CLSID_DOMDocument30);
	ATLASSERT(SUCCEEDED(hRet));
	if (!SUCCEEDED(hRet))
		return false;

	VARIANT_BOOL r;
	pDoc->put_async(VARIANT_FALSE);
	if (isPath)
		pDoc->load(_variant_t(xml_path.c_str()), &r);
	else
		pDoc->loadXML(CComBSTR(xml_path.c_str()), &r);
	if (r != VARIANT_TRUE)
	{
		IXMLDOMParseError *pXMLErr=NULL;
		CComBSTR bstr = NULL;

		pDoc->get_parseError(&pXMLErr);
		pXMLErr->get_reason(&bstr);
		_DBGLOG(LEVEL_WARNING, (boost::_tformat(_T("Restoring Workspace:  %1% %2%")) % std::_tstring(xml_path) % std::_tstring(CW2T(bstr))).str().c_str());
		return false;
	}

	CComPtr<IXMLDOMNodeList> props;
	HRESULT hr = pDoc->selectNodes(CComBSTR(_T("//map/*")), &props);
	if (hr != S_OK)
		return false;

	long length;
	props->get_length(&length);
	for(long i = 0; i < length;  ++i)
	{
		CComPtr<IXMLDOMNode> prop;
		hr = props->get_item(i, &prop);
		if (hr != S_OK)
			return false;

		CComBSTR name;
		CComVariant value;
		hr = prop->get_nodeName(&name);
		if (hr != S_OK)
			return false;

		CComPtr<IXMLDOMNode> val;
		if (prop->get_firstChild(&val) == S_OK)
		{
			hr = val->get_nodeValue(&value);
			value.ChangeType(VT_BSTR);
			std::_tstring str = CW2T(value.bstrVal, CP_UTF8);
			TidyCRLF(str);
			m_map[static_cast<const TCHAR *>(CW2T(name, CP_UTF8))] = str;
		}
	}
	return (m_map.size() > 0);
}

CPersistWindow::CPersistWindow(IPersistable * owner) : m_owner(owner)
{
}

void CPersistWindow::SetPath(const std::string & path, const boost::filesystem::path & folder)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if (path.empty())
		GenerateTmpFile(folder);
	else
	{
		m_filePath = boost::filesystem::path(path, boost::filesystem::native);
		if (!(boost::filesystem::exists(m_filePath) && m_file.Open(m_filePath.string().c_str(), GENERIC_WRITE)))
			GenerateTmpFile(folder);
	}
	//ATLASSERT(boost::filesystem::exists(m_filePath));
}

void CPersistWindow::Save()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if ( m_file.IsOpen() )
	{
		CPersistMap persistInfo;

		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		BOOL hr = ::GetWindowPlacement(m_owner->GetHwnd(), &placement);
		ATLASSERT(hr);
		char encodedBuff[256];
		int encodedLen = sizeof(encodedBuff);
		hr = Base64Encode(reinterpret_cast<const BYTE *>(&placement), placement.length, encodedBuff, &encodedLen, ATL_BASE64_FLAG_NOPAD);
		ATLASSERT(hr);
		encodedBuff[encodedLen] = '\0';
		std::_tstring val = CA2T(encodedBuff);
		persistInfo.Set(PERSIST_PLACEMENT, val);

		m_owner->SavePersistInfo(persistInfo);
		DoAutoSave(&m_file, &persistInfo);
	}
}

void CPersistWindow::Restore()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	CPersistMap persistInfo;
	m_file.Close();
	if (persistInfo.deserialize(m_filePath.wstring()))
	{
		WINDOWPLACEMENT placement;
		std::string encodedStr = CT2A(persistInfo.Get(PERSIST_PLACEMENT));
		int decodedLen = sizeof(WINDOWPLACEMENT);
		BOOL hr = Base64Decode(encodedStr.c_str(), encodedStr.length(), reinterpret_cast<BYTE *>(&placement), &decodedLen);
		if (hr && placement.length == sizeof(WINDOWPLACEMENT))
		{
			if(placement.showCmd == SW_SHOWMINIMIZED )
			{
				//(Prevent Launch To Minimized)
				placement.showCmd = SW_SHOWNORMAL;
			}
			RECT rMaxWA;
			::SystemParametersInfo(SPI_GETWORKAREA, 0, &rMaxWA, 0);
			if( (placement.rcNormalPosition.left >= rMaxWA.left) &&
				(placement.rcNormalPosition.top >= rMaxWA.top) &&
				(placement.rcNormalPosition.right <= rMaxWA.right) &&
				(placement.rcNormalPosition.bottom <= rMaxWA.bottom) )
			{
				// If All Values Within The Visible Screen (Taking Into Account Only The Primary Monitor...!), Set The Placement
				::SetWindowPlacement(m_owner->GetHwnd(), &placement);
			}
		}
		m_owner->RestorePersistInfo(persistInfo);
	}
	m_file.Open(m_filePath.string().c_str(), GENERIC_WRITE);
}

void CPersistWindow::Remove()
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	if ( m_file.IsOpen() )
	{
		m_file.Close();
		boost::filesystem::remove(m_filePath);
	}
}

void CPersistWindow::GenerateTmpFile(const boost::filesystem::path & folder)
{
	clib::recursive_mutex::scoped_lock lock(m_mutex);
	//srand((unsigned)time(NULL));
	//for(;;)
	//{
	//	std::string fileName = "qb";
	//	fileName += boost::lexical_cast<std::string>(rand() % 999999);
	//	fileName += ".xml";
	//	m_filePath = folder / fileName;
	//	CString wideFilePath = m_filePath.native_file_string().c_str();
	//	if (!boost::filesystem::exists(m_filePath) && m_file.Create(wideFilePath))
	//		break;
	//}
	//ATLASSERT(boost::filesystem::exists(m_filePath));

	//CPersistMap map;
	//DoAutoSave(&m_file, &map);
}

void CPersistWindow::DoAutoSave(CUnicodeFile *f, CPersistMap *persistInfo)
{
	//No saving until everyone is on new workspae feature, then this whole class will be removed...
	//CString xml;
	//if (f->IsOpen() && persistInfo->serialize(xml))
	//{
	//	f->Empty();
	//	f->Write(xml);
	//}
}
