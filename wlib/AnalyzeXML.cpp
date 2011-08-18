#include "StdAfx.h"

#include "clib.h"
#include "StringSet.h"
#include "SAXHandlersBase.h"
#include "AnalyzeXML.h"

//*****************************************************************************

class CAnalyzeXMLHandlerImpl : public StackHandlerImpl
{
	bool m_recFound;
	bool m_pathFound;
	std::_tstring m_xpath;
	StdStringVector m_columnNames;
public:

	CAnalyzeXMLHandlerImpl() : m_recFound(false), m_pathFound(false)
	{
	}

	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName,
		/* [in] */ ISAXAttributes *pAttributes)
	{ 
		__super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

		Element &e = m_stack.top();

		if(!m_pathFound)
		{
			switch (m_stack.size())
			{
			case 1:
				m_xpath=e.m_tag;
				break;
			case 2:
				m_xpath.append(_T("/"));
				m_xpath.append(e.m_tag);
				m_pathFound=true;
				break;
			}
		}

		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName)
	{ 
		Element & e = m_stack.top();

		if(!m_recFound)
		{
			switch (m_stack.size())
			{
			case 2:
				m_recFound=true;
				break;
			case 3:
				m_columnNames.push_back(e.m_tag);
				break;
			} // switch
		}
		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}


	bool IsRecordFound() const { return	m_recFound; }

	unsigned CountColumns() const { return m_columnNames.size(); }

	void GetXPath(std::_tstring& xpath) const { xpath=m_xpath; }

	void GetColumnNames(StdStringVector& cnames) const
	{
		cnames.insert(cnames.end(), m_columnNames.begin(), m_columnNames.end()); 
	}

};

//*****************************************************************************

class CXMLAnalyzer : public IXMLAnalyzer, public CUnknown
{
	unsigned m_columns;
	std::_tstring m_xpath;
	StdStringVector m_columnNames;

public:
	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IXMLAnalyzer)
	END_CUNKNOWN(CUnknown)

	CXMLAnalyzer() : m_columns(0) {}

	void Analyze(const std::_tstring& xml)
	{
		Reset();
		CComPtr<CAnalyzeXMLHandlerImpl> pHnd = new CAnalyzeXMLHandlerImpl();
		CComInitialize com;
		CComPtr<ISAXXMLReader> pRdr; 
		HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader);//, NULL, CLSCTX_ALL, IID_ISAXXMLReader, (void **)&pRdr); 
		if (hr == S_OK)
		{
			hr = pRdr->putContentHandler(pHnd); 
			if (hr == S_OK)
			{
				pRdr->parse(CComVariant(xml.c_str()));
				if(pHnd->IsRecordFound())
				{
					m_columns=pHnd->CountColumns();
					pHnd->GetXPath(m_xpath);
					pHnd->GetColumnNames(m_columnNames);
				}
			}
		}
	}

	void Reset()
	{
		m_columns=0;
		m_xpath.clear();
		m_columnNames.clear();
	}

	bool IsXML() const 
	{ 
		return m_columns>0 && !m_xpath.empty() && m_columnNames.size()==m_columns;
	}

	unsigned CountColumns() const { return m_columns; }

	void GetXPath(std::_tstring& xpath) const { xpath=m_xpath; }

	void GetColumnName(unsigned column, std::_tstring& name) const
	{
		assert(column<m_columns);
		name=m_columnNames[column];
	}
};

//*****************************************************************************

IXMLAnalyzer* CreateIXMLAnalyzer()
{
	IXMLAnalyzer* iface=new CXMLAnalyzer();
	return iface;
}


