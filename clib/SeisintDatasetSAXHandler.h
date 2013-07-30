#pragma once

#include "SAXHandlersBase.h"
#include "Util.h"
#include "Table.h"

//xsd schema style types
const TCHAR * const XSD_TYPE_BOOLEAN				= _T("xs:boolean");
const TCHAR * const XSD_TYPE_INTEGER				= _T("xs:integer");
const TCHAR * const XSD_TYPE_NONNEGATIVEINTEGER		= _T("xs:nonNegativeInteger");
const TCHAR * const XSD_TYPE_REAL					= _T("xs:real");
const TCHAR * const XSD_TYPE_STRING					= _T("xs:string");
const TCHAR * const XSD_TYPE_HEXBINARY				= _T("xs:hexBinary");
//ecl schema style type prefixes
const TCHAR * const XSD_TYPE_ECLDECIMAL				= _T("decimal");
const TCHAR * const XSD_TYPE_ECLUDECIMAL			= _T("udecimal");
const TCHAR * const XSD_TYPE_ECLSTRING				= _T("string");
const TCHAR * const XSD_TYPE_ECLDATA				= _T("data");

class CLIB_API SeisintDatasetHandlerImpl : public StackHandlerImpl
{
private:
	enum BRANCH
	{
		BRANCH_UNKNOWN,
		BRANCH_EXCEPTION,
		BRANCH_XMLSCHEMA,
		BRANCH_DATA
	};

protected:
	BRANCH  m_branch;
	ITableMap &m_tableMap;
	std::_tstring m_datasetName;
	__int64 m_row;
	int m_col;
	typedef std::map<std::_tstring, COLUMN_TYPE_ENUM> StringTypeMap;
	StringTypeMap m_ColumnNameTypeMap;

public:
	SeisintDatasetHandlerImpl(ITableMap & table) : m_tableMap(table)
	{
	}

	int EnsureColumnExists(ITable * table, const std::_tstring & col)
	{
		ATLASSERT(table);
		StringTypeMap::iterator found = m_ColumnNameTypeMap.find(col);
		return table->FindColumn(col, true, found != m_ColumnNameTypeMap.end() ? found->second : CT_STRING);
	}

	int EnsureColumnExists(const std::_tstring & dataset, const std::_tstring & col)
	{
		CComPtr<ITable> table = m_tableMap[dataset].m_table.get();
		return EnsureColumnExists(table, col);
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

		//  Calculate Which Branch we are in...
		switch (m_stack.size())
		{
		case 1: //Result
			break;
		case 2: //Dataset, Exception
			if (e.m_tag.compare(_T("Exception")) == 0)
				m_branch = BRANCH_EXCEPTION;
			else if (e.m_tag.compare(_T("XmlSchema")) == 0)
				m_branch = BRANCH_XMLSCHEMA;
			else if (e.m_tag.compare(_T("Dataset")) == 0)
			{
				m_branch = BRANCH_DATA;
				m_datasetName = e.m_attr[_T("name")];
				m_row = -1;	//When first row is hit it will be incremented...
				if (m_tableMap.find(m_datasetName) == m_tableMap.end())
				{
					StlLinked<ITable> table = CreateISparseTable();
					m_tableMap[m_datasetName].m_schema = e.m_attr[_T("xmlSchema")];
					m_tableMap[m_datasetName].m_table = table;
				}
			}
			else if (e.m_tag.compare(_T("print")) == 0)
			{
				m_branch = BRANCH_DATA;
				m_datasetName = e.m_attr[_T("activity")] + _T("_") + e.m_attr[_T("outputIdx")];
				m_row = -1;	//When first row is hit it will be incremented...
				if (m_tableMap.find(m_datasetName) == m_tableMap.end())
				{
					StlLinked<ITable> table = CreateISparseTable();
					m_tableMap[m_datasetName].m_schema = e.m_attr[_T("xmlSchema")];
					m_tableMap[m_datasetName].m_table = table;
				}
			}
			else
				m_branch = BRANCH_UNKNOWN;
			break;
		}

		switch (m_branch)
		{
		case BRANCH_XMLSCHEMA:
			switch (m_stack.size())
			{
			case 4:
				//TODO:  Add schema support.
				break;
			default:
				if (m_stack.size() > 10)
				{
					//TODO:  Mark schema as having child datasets.
				}
			}
			break;

		case BRANCH_DATA:
			switch (m_stack.size())
			{
			case 3:
				{
					++m_row;	
				}
				break;

			case 4:
				{
					m_col = EnsureColumnExists(m_datasetName, e.m_tag);
				}
				break;

			default:
				if(m_stack.size() > 4)
				{
					CComPtr<ITable> table = m_tableMap[m_datasetName].m_table.get();
					std::_tstring startTag;
					if (boost::algorithm::iequals(e.m_tag, _T("Item")))
					{
						if (table->GetCellLength(m_row, m_col) > 0)
							startTag += _T(", ");
						startTag += _T("'");
					}
					else
					{
						startTag = _T("<");
						startTag += e.m_tag;
						startTag += _T(">");
					}
					table->AppendCell(m_row, m_col, startTag.c_str());
				}
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

		switch (m_branch)
		{
		case BRANCH_XMLSCHEMA:
			switch (m_stack.size())
			{
			case 4:
				m_branch = BRANCH_UNKNOWN;
				break;

			case 10:
				{
					COLUMN_TYPE_ENUM type = CT_STRING;
					std::_tstring name = e.m_attr[_T("name")];
					if (boost::algorithm::iequals(e.m_attr[_T("type")], XSD_TYPE_BOOLEAN))
					{
						type = CT_BOOLEAN;
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("type")], XSD_TYPE_INTEGER))
					{
						type = CT_INT;
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("type")], XSD_TYPE_NONNEGATIVEINTEGER))
					{
						type = CT_UINT;
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("type")], XSD_TYPE_REAL))
					{
						type = CT_REAL;
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("type")], XSD_TYPE_STRING))
					{
						type = CT_STRING;
					}
					m_ColumnNameTypeMap[e.m_attr[_T("name")]] = type;
				}
				break;
			}
			break;

		case BRANCH_DATA:
			switch (m_stack.size())
			{
			case 3:
				{
					CComPtr<ITable> table = m_tableMap[m_datasetName].m_table.get();
					std::_tstring cell = table->GetCell(m_row, 0);
					if (cell.empty())
					{
						int col = EnsureColumnExists(table, _T("##"));
						//table->SetCell(m_row, col, _T(""));
					}
					for (Element::StringStringMap::const_iterator itr = e.m_attr.begin(); itr != e.m_attr.end(); ++itr)
					{
						if (boost::algorithm::iequals(itr->first, _T("seq")) ||
							boost::algorithm::iequals(itr->first, _T("eog")) ||
							boost::algorithm::iequals(itr->first, _T("eof")) ||
							boost::algorithm::iequals(itr->first, _T("skip")) ||
							boost::algorithm::iequals(itr->first, _T("limit")) ||
							boost::algorithm::iequals(itr->first, _T("count")))
							continue;
						std::_tstring columnLabel = _T("@") + itr->first;
						int col = EnsureColumnExists(table, columnLabel);
						table->SetCell(m_row, col, itr->second);
					}
					if (boost::algorithm::iequals(e.m_attr[_T("eog")], _T("true")))
					{
						int col = EnsureColumnExists(table, _T("##"));
						table->SetCell(m_row, col, _T("EOG"));
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("eof")], _T("true")))
					{
						int col = EnsureColumnExists(table, _T("##"));
						table->SetCell(m_row, col, _T("EOF"));
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("skip")], _T("true")))
					{
						int col = EnsureColumnExists(table, _T("##"));
						table->SetCell(m_row, col, _T("Skipped"));
					}
					else if (boost::algorithm::iequals(e.m_attr[_T("limit")], _T("true")))
					{
						int col = EnsureColumnExists(table, _T("##"));
						table->SetCell(m_row, col, _T("Limit"));
					}
					else if (e.m_attr[_T("count")].length())
					{
						int col = EnsureColumnExists(table, _T("##"));
						table->SetCell(m_row, col, e.m_attr[_T("count")].c_str());
					}
				}
				break;
			case 4:
				{
					CComPtr<ITable> table = m_tableMap[m_datasetName].m_table.get();
					int col = EnsureColumnExists(table, e.m_tag);
					table->AppendCell(m_row, col, e.GetContent());
				}
				break;

			default:
				if (m_stack.size() > 4)
				{
					CComPtr<ITable> table = m_tableMap[m_datasetName].m_table.get();
					table->SetHasChildDataset(true);
					std::_tstring content, _content = e.GetContent();
					table->AppendCell(m_row, m_col, EscapeXML(_content, content));
					std::_tstring endTag;
					if (boost::algorithm::iequals(e.m_tag, _T("Item")))
					{
						endTag = _T("'");
					}
					else
					{
						endTag = _T("</");
						endTag += e.m_tag;
						endTag += _T(">");
					}
					table->AppendCell(m_row, m_col, endTag.c_str());
				}
			}
			break;
		}

		return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
	}
};
/*
class CLIB_API XGMMLHandlerImpl : public StackHandlerImpl
{
private:
	std::map<std::_tstring, std::_tstring> m_edge;	// accumulated edges
	std::_tstring edge_source;
	std::_tstring edge_target;

protected:
	std::stack<IGraphSubgraphAdapt> m_stackSubgraph;
	IGraph * m_graph;
	mutable clib::recursive_mutex m_mutex;

public:
	XGMMLHandlerImpl(IGraph* graph);

	typedef std::map<std::_tstring, std::_tstring> edge_type;

	virtual HRESULT STDMETHODCALLTYPE endDocument();
	virtual HRESULT STDMETHODCALLTYPE startElement(
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName,
		ISAXAttributes *pAttributes);
	virtual HRESULT STDMETHODCALLTYPE endElement( 
		const wchar_t *pwchNamespaceUri,
		int cchNamespaceUri,
		const wchar_t *pwchLocalName,
		int cchLocalName,
		const wchar_t *pwchQName,
		int cchQName);
};
*/