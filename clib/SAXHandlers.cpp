#include "stdafx.h"

#include "SAXHandlers.h"

//	---------------------------------------------------------------------------
DatasetXHandlerImpl::DatasetXHandlerImpl(HeaderT *header, GridT *grid) : m_Dataset(_T("Dataset")), m_Row(_T("Row"))
{
	m_header = header;
	m_grid = grid;
	m_firstRow = true;
}

DatasetXHandlerImpl::~DatasetXHandlerImpl()
{
}

//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE  DatasetXHandlerImpl::startDocument ( ) 
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE  DatasetXHandlerImpl::endDocument ( ) 
{
	return S_OK;
}
//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE DatasetXHandlerImpl::startElement( 
    /* [in] */ const wchar_t * /*pwchNamespaceUri*/,
    /* [in] */ int /*cchNamespaceUri*/,
    /* [in] */ const wchar_t * /*pwchLocalName*/,
    /* [in] */ int /*cchLocalName*/,
    /* [in] */ const wchar_t *pwchQName,
    /* [in] */ int /*cchQName*/,
    /* [in] */ ISAXAttributes * /*pAttributes*/)
{
	if (boost::algorithm::iequals(m_Dataset, pwchQName))
	{
		//m_grid->clear();
	}
	else if (boost::algorithm::iequals(m_Row, pwchQName))
	{
		m_row.clear();
		m_col = 0;
	}
	else 
	{
		if (m_firstRow)
		{
			m_colLabel = COLE2T(pwchQName);
			m_header->operator [](m_col) += m_colLabel;
		}
	}

	return S_OK;
}

//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE DatasetXHandlerImpl::endElement( 
    /* [in] */ const wchar_t * /*pwchNamespaceUri*/,
    /* [in] */ int /*cchNamespaceUri*/,
    /* [in] */ const wchar_t * /*pwchLocalName*/,
    /* [in] */ int /*cchLocalName*/,
    /* [in] */ const wchar_t *pwchQName,
    /* [in] */ int /*cchQName*/)
{
	if (boost::algorithm::iequals(m_Dataset, pwchQName))
	{
	}
	else if (boost::algorithm::iequals(m_Row, pwchQName))
	{
		m_grid->operator [](m_grid->size()) = m_row;
//		const char * d = m_grid->operator [](m_grid->size())[0];
		m_firstRow = false;
	}
	else
	{
		++m_col;
	}
	return S_OK;
}

//	---------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE DatasetXHandlerImpl::characters( 
    /* [in] */ const wchar_t *pwchChars,
    /* [in] */ int cchChars)
{
	CComBSTR x;
	if (m_row[m_col].vt != VT_EMPTY)
		x = m_row[m_col].bstrVal;
	x.Append(pwchChars, cchChars);
	m_row[m_col] = x;
	return S_OK;
}
