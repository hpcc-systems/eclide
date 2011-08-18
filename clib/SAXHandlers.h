#pragma once

#include "clib.h"
#include "SAXHandlersBase.h"
#include "Table.h"
#include "Logger.h"

//  ===========================================================================
typedef std::map<unsigned, std::_tstring> HeaderT;
typedef std::map<unsigned, _variant_t> CellT;
typedef std::map<unsigned, CellT> GridT;
//  ===========================================================================
class CLIB_API DatasetXHandlerImpl : public ContentHandlerImplBase
{
protected:
	const std::_tstring m_Dataset;
	const std::_tstring m_Row;
	HeaderT *m_header;
	GridT *m_grid;
	CellT m_row;
	std::_tstring m_colLabel;
	unsigned m_col;
	bool m_firstRow;

public:
	DatasetXHandlerImpl(HeaderT *header, GridT *grid);
	virtual ~DatasetXHandlerImpl();

	virtual HRESULT STDMETHODCALLTYPE startDocument( void);
    
    virtual HRESULT STDMETHODCALLTYPE endDocument( void);
        
    virtual HRESULT STDMETHODCALLTYPE startElement( 
        /* [in] */ const wchar_t *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ const wchar_t *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ const wchar_t *pwchQName,
        /* [in] */ int cchQName,
        /* [in] */ ISAXAttributes *pAttributes);
    
    virtual HRESULT STDMETHODCALLTYPE endElement( 
        /* [in] */ const wchar_t *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ const wchar_t *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ const wchar_t *pwchQName,
        /* [in] */ int cchQName);
    
    virtual HRESULT STDMETHODCALLTYPE characters( 
        /* [in] */ const wchar_t *pwchChars,
        /* [in] */ int cchChars);
};

//  ===========================================================================
class CLIB_API XsdHandlerImpl : public StackHandlerImpl
{
protected:
	CComPtr<ITable> m_table;

public:
	XsdHandlerImpl(ITable * table) : m_table(table)
	{
		m_table->SetColumn(0, _T("##"));
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

		switch (m_stack.size())
		{
		case 8:
			{
				m_table->FindColumn(e.m_attr[_T("name")], true);
			}
			break;
		}

		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}
};

//  ===========================================================================
class CLIB_API DatasetHandlerImpl : public StackHandlerImpl
{
private:
	enum BRANCH
	{
		BRANCH_UNKNOWN,
		BRANCH_SCHEMA,
		BRANCH_DATA
	} m_branch;

protected:
	CComPtr<ITable> m_table;
	__int64 m_row;

public:

	DatasetHandlerImpl(ITable * table) : m_table(table)
	{
		m_row = m_table->GetRowCount() + 1;
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

		switch (m_stack.size())
		{
		case 2:
			if (e.m_tag.compare(_T("schema")) == 0)
			{
				m_branch = BRANCH_SCHEMA;
			}
			else if (e.m_tag.compare(_T("diffgram")) == 0)
			{
				m_branch = BRANCH_DATA;
			}
			break;

		case 3:
			break;
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

		switch(m_branch)
		{
		case BRANCH_SCHEMA:
			if (m_stack.size() == 9 && e.m_tag.compare(_T("element")) == 0)
			{
				m_table->FindColumn(e.m_attr[_T("name")], true);
			}
			break;
		case BRANCH_DATA:
			switch (m_stack.size())
			{
			case 4:
				m_row++;
				break;
			case 5:
				m_table->SetCell(m_row, m_table->FindColumn(e.m_tag, true), e.GetContent());
				break;
			}
			break;
		}
		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}
};

//  ===========================================================================
class CLIB_API SimpleDatasetHandlerImpl : public StackHandlerImpl
{
private:
protected:
	ITable &m_table;
	__int64 m_row;

public:

	SimpleDatasetHandlerImpl(ITable & table) : m_table(table), m_row(0)
	{
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

		switch (m_stack.size())
		{
		case 2:
			m_row++;
			break;
		case 3:
			m_table.SetCell(m_row, m_table.FindColumn(e.m_tag, true), e.GetContent());
			break;
		}
		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}
};

//  ===========================================================================
class CLIB_API SAXErrorHandler: public CUnknown, public ISAXErrorHandler
{
protected:
	bool m_supressErrors;

public:
	IMPLEMENT_CUNKNOWN

	SAXErrorHandler::SAXErrorHandler(bool supressErrors = false) : m_supressErrors(supressErrors)
	{
	}

    virtual HRESULT STDMETHODCALLTYPE error( 
        /* [in] */ ISAXLocator *pLocator,
        /* [in] */ const wchar_t *pwchErrorMessage,
        /* [in] */ HRESULT hrErrorCode)
		{
			printError(pLocator, pwchErrorMessage, hrErrorCode);
			return S_OK;
		}
    
    virtual HRESULT STDMETHODCALLTYPE fatalError( 
        /* [in] */ ISAXLocator *pLocator,
        /* [in] */ const wchar_t *pwchErrorMessage,
        /* [in] */ HRESULT hrErrorCode)
		{
			printError(pLocator, pwchErrorMessage, hrErrorCode);
			return S_OK;
		}
    
    virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
        /* [in] */ ISAXLocator * /*pLocator*/,
        /* [in] */ const wchar_t * /*pwchErrorMessage*/,
        /* [in] */ HRESULT /*hrErrorCode*/)
		{
			return S_OK;
		}
private:
		void printError(
        /* [in] */ ISAXLocator *pLocator,
        /* [in] */ const wchar_t *pwchErrorMessage,
        /* [in] */ HRESULT /*hrErrorCode*/)
		{
			if (m_supressErrors)
				return;

			static const TCHAR* title=_T("Sax Parser");
			CString errmsg=CW2T(pwchErrorMessage, CP_UTF8);
			int line=-1;
			if(pLocator)
				pLocator->getLineNumber(&line);
			if(line!=-1)
			{
				if(line==1) // ignore invalid header
					return;
				CString msg;
				msg.Format(_T("Line %d: %s"), line, errmsg);
				_DBGLOG(LEVEL_WARNING, static_cast<const TCHAR *>(msg));
			}
			else
			{
				_DBGLOG(LEVEL_WARNING, static_cast<const TCHAR *>(errmsg));
			}
		}
};
//  ===========================================================================
//  ===========================================================================
