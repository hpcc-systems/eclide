#include "StdAfx.h"

#include "RecursiveMutex.h"
#include "SAXHandlers.h"
#include "SeisintDatasetSAXHandler.h"
#include "util.h"
#include "global.h"
#include "unicodefile.h"
#include "ResultFormat.h"
#include "table.h"

//  ===========================================================================

class CColumnNumber
{
public:
    CColumnNumber()
    {
        m_col = -1;
    }
    CColumnNumber(int val)
    {
        m_col = val;
    }
    CColumnNumber(const CColumnNumber &other)
    {
        m_col = other.m_col;
    }
    int operator=(int val)
    {
        m_col = val;
        return m_col;
    }
    int m_col;
};

//  ===========================================================================

typedef boost::variant<bool, int, unsigned int, float, std::_tstring> TableCellVariantT;
typedef std::pair<std::_tstring, StlLinked<IUnknown> > TableCellT;
class AppendValue : public boost::static_visitor<>
{
protected:
    const std::_tstring & m_itemToAppend;

public:
    AppendValue(const std::_tstring & itemToAppend) : m_itemToAppend(itemToAppend)
    {
    }

    void operator()(bool &) const
    {
        ATLASSERT(!"Should not be possible");
    }

    void operator()(int &) const
    {
        ATLASSERT(!"Should not be possible");
    }

    void operator()(unsigned int &) const
    {
        ATLASSERT(!"Should not be possible");
    }

    void operator()(float &) const
    {
        ATLASSERT(!"Should not be possible");
    }

    void operator()(std::_tstring & str) const
    {
        str += m_itemToAppend;
    }
};
//  ===========================================================================

typedef std::pair<std::_tstring, COLUMN_TYPE_ENUM> TableHeaderItemT;
typedef std::vector<TableHeaderItemT> TableHeaderT;
typedef std::map<std::_tstring, CColumnNumber> RTableHeaderT;
typedef std::vector<TableCellT> Row;
typedef std::vector<Row> Data;
typedef std::map<__int64, int> Table;	//Handles the sparse part. 

class AlphaNumCompare
{
protected:
    typedef std::list<boost::tuples::tuple<unsigned int, COLUMN_TYPE_ENUM, bool> > ColHistory;
    ColHistory m_cols;

public:
    void Init(unsigned int col, COLUMN_TYPE_ENUM type, bool bAscending)
    {
        std::remove(m_cols.begin(), m_cols.end(), boost::tuples::make_tuple(col, type, true));
        std::remove(m_cols.begin(), m_cols.end(), boost::tuples::make_tuple(col, type, false));
        m_cols.push_front(boost::tuples::make_tuple(col, type, bAscending));
    }

    int compare(const Row & l, const Row & r, unsigned int col, COLUMN_TYPE_ENUM type, bool ascending)
    {
        if (col >= l.size() || col >= r.size())
        {
            if (col < l.size())				//Two rows may have different number of columns...
                return ascending ? 1 : -1;
            else if (col < r.size())
                return ascending ? -1 : 1;
            return 0;
        }
        TableCellT left = ascending ? l[col] : r[col];
        TableCellT right = ascending ? r[col] : l[col];

        switch(type)
        {
        case CT_UINT:
            {
                unsigned __int64 nValue1 = _ttoi64(left.first.c_str());
                unsigned __int64 nValue2 = _ttoi64(right.first.c_str());
                if (nValue1 < nValue2)
                {
                    return -1;
                }
                else if (nValue1 > nValue2)
                {
                    return 1;
                }
                return nValue1 < nValue2;
            }
            break;
        case CT_INT:
            {
                __int64 nValue1 = _ttoi64(left.first.c_str());
                __int64 nValue2 = _ttoi64(right.first.c_str());
                if (nValue1 < nValue2)
                {
                    return -1;
                }
                else if (nValue1 > nValue2)
                {
                    return 1;
                }
            }
            break;
        case CT_REAL:
        case CT_DEC:
        case CT_UDEC:
            {
#ifdef _UNICODE
                double nValue1 = _wtof(left.first.c_str());
                double nValue2 = _wtof(right.first.c_str());
#else
                double nValue1 = atof(left.first.c_str());
                double nValue2 = atof(right.first.c_str());
#endif
                if (nValue1 < nValue2)
                {
                    return -1;
                }
                else if (nValue1 > nValue2)
                {
                    return 1;
                }
            }
            break;
        default:
            if (left < right)
            {
                return -1;
            }
            else if (left > right)
            {
                return 1;
            }
        }
        return 0;
    }

    bool operator ()(const Row & l, const Row & r)
    {
        int retVal = 0;
        for(ColHistory::iterator itr = m_cols.begin(); itr != m_cols.end(); ++itr)
        {
            retVal = compare(l, r, itr->get<0>(), itr->get<1>(), itr->get<2>());
            if (retVal != 0)
            {
                break;
            }
        }
        return retVal < 0;
    }
};

const TCHAR * const NoString = _T("");

class CSparseTableImpl : public CUnknown, public ISparseTable
{
private:
    mutable clib::recursive_mutex m_mutex;
    RTableHeaderT m_rheader;
    TableHeaderT m_header;
    Data m_data;
    Table m_table;
    bool m_changed;
    bool m_hasChildDataset;

    AlphaNumCompare m_compare;
    unsigned int m_nSortColumn;
    bool m_bAscending;

public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    CSparseTableImpl() : m_mutex(), m_changed(true)
    {
        m_hasChildDataset = false;
        m_nSortColumn = 0;
        m_bAscending = true;
        m_compare.Init(m_nSortColumn, CT_UNKNOWN, m_bAscending);
    }

    ~CSparseTableImpl()
    {
    }

    bool GetScopedLock(clib::scoped_lock_ref_counted_adapt &lock) const
    {
        lock = new clib::scoped_lock_ref_counted(m_mutex);
        return true;
    }

    __int64 GetRowCount() const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return m_table.size();
    }

    bool RemoveRow(__int64 tableRow)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        Table::iterator itr = m_table.find(tableRow);
        if(itr != m_table.end())
        {
            ATLASSERT(itr->second >= 0);
            int dataRow = itr->second;
            Data::iterator i_itr = m_data.begin() + itr->second;
            ATLASSERT(i_itr != m_data.end());
            m_data.erase(i_itr);
            for(Table::iterator fix_itr = m_table.begin(); fix_itr != m_table.end(); ++fix_itr)
            {
                if (fix_itr->first > tableRow || fix_itr->second > dataRow)
                {
                    __int64 tablePos = fix_itr->first > tableRow ? fix_itr->first - 1 : fix_itr->first; 
                    int dataPos = fix_itr->second > dataRow ? fix_itr->second - 1 : fix_itr->second;
                    m_table[tablePos] = dataPos;
                }
            }
            Table::iterator lastRowItr = m_table.find(GetMaxRowNum());
            if (lastRowItr != m_table.end())
            {
                m_table.erase(lastRowItr);
            }
            return true;
        }
        return false;
    }

    __int64 GetMaxRowNum() const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        Table::const_reverse_iterator itr = m_table.rbegin();
        if (itr == m_table.rend())
            return 0;
        return itr->first;
    }

    unsigned GetColumnCount() const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return m_header.size();
    }

    const TCHAR * GetColumn(unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (col >= m_header.size())
            return NoString;

        return m_header[col].first.c_str();
    }

    COLUMN_TYPE_ENUM GetColumnType(unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (col >= m_header.size())
            return CT_UNKNOWN;

        return m_header[col].second;
    }

    bool IsColumnString(unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        switch (GetColumnType(col))
        {
        case CT_BOOLEAN:
        case CT_INT:
        case CT_UINT:
        case CT_REAL:
        case CT_DEC:
        case CT_UDEC:
            return false;
        case CT_STRING:
        case CT_DATA:
            return true;
        }
        return true;
    }

    signed FindColumn(const std::_tstring & val) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!val.length())
            return -1;

        RTableHeaderT::const_iterator found =  m_rheader.find(val);
        if (found != m_rheader.end())
        {
            return found->second.m_col;
        }

        return -1;
    }

    signed FindColumn(const std::_tstring & val, bool addIfMissing, COLUMN_TYPE_ENUM type = CT_STRING)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!val.length())
            return -1;

        int col = m_rheader[val].m_col;
        if ( col == -1 && addIfMissing)
        {
            return AddColumn(val.c_str(), type);
        }

        return col;
    }

    int InsertColumn(unsigned int pos, const std::_tstring & val)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        ATLASSERT(pos < GetColumnCount());
        TableHeaderT::iterator itr = m_header.begin() + pos;
        m_header.insert(itr, TableHeaderItemT(val.c_str(), CT_STRING));
        m_rheader[val] = pos;
        m_changed = true;
        return pos;
    }

    void SetColumn(unsigned col, const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (col >= m_header.size())
        {
            m_header.resize(col + 1);
        }

        m_header[col] = TableHeaderItemT(val, type);
        m_rheader[val] = col;
        m_changed = true;
    }

    int AddColumn(const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        int col = GetColumnCount();
        SetColumn(col, val, type);
        return col;
    }

    Row & ResolveRow(__int64 row)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        Table::const_iterator itr = m_table.find(row);
        if(itr != m_table.end())
        {
            return m_data[itr->second];
        }
        else
        {
            m_data.push_back(Row());
            int index = m_data.size() - 1;
            m_table[row] = index;
            return m_data[index];
        }
    }

    const TCHAR * GetCell(__int64 row, unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!RowExists(row))
        {
            return NoString;
        }

        Table::const_iterator itr = m_table.find(row);
        if(itr != m_table.end() && col < m_data[itr->second].size())
        {
            return m_data[itr->second][col].first.c_str();
        }

        return NoString;
    }
    const TCHAR * GetCell(__int64 row, const std::_tstring & col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return GetCell(row, FindColumn(col));
    }

    void SetCell(__int64 row, unsigned col, const std::_tstring & val)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);

        //force the header to be as wide as widest row
        if (col >= m_header.size())
        {
            m_header.resize(col + 1);
        }

        if (col >= ResolveRow(row).size())
            ResolveRow(row).resize( m_header.size() );		

        ResolveRow(row)[col].first = val;
        //switch(GetColumnType(col))
        //{
        //case CT_UNKNOWN:
        //	break;
        //case CT_BOOLEAN:
        //	ResolveRow(row)[col].first = boost::lexical_cast<bool>(static_cast<const TCHAR *>(val));
        //	break;
        //case CT_INT:
        //	ResolveRow(row)[col].first = boost::lexical_cast<int>(static_cast<const TCHAR *>(val));
        //	break;
        //case CT_UINT:
        //	ResolveRow(row)[col].first = boost::lexical_cast<unsigned int>(static_cast<const TCHAR *>(val));
        //	break;
        //case CT_REAL:
        //	ResolveRow(row)[col].first = boost::lexical_cast<float>(static_cast<const TCHAR *>(val));
        //	break;
        //case CT_STRING:
        //	ResolveRow(row)[col].first = static_cast<const TCHAR *>(val);
        //	break;
        //case CT_DATA:
        //	ResolveRow(row)[col].first = static_cast<const TCHAR *>(val);
        //	break;
        //case CT_DEC:
        //	ResolveRow(row)[col].first = static_cast<const TCHAR *>(val);
        //	break;
        //case CT_UDEC:
        //	ResolveRow(row)[col].first = static_cast<const TCHAR *>(val);
        //	break;
        //case CT_LAST:
        //	ATLASSERT(false);
        //	break;
        //default:
        //	ATLASSERT(false);
        //	break;
        //}
        //ResolveRow(row)[col].second = static_cast<const TCHAR *>(val);
        m_changed = true;
    }

    void AppendCell(__int64 row, unsigned col, const std::_tstring & val)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);

        //force the header to be as wide as widest row
        if (col >= m_header.size())
            m_header.resize(col + 1);

        if (col >= ResolveRow(row).size())
            ResolveRow(row).resize( m_header.size() );

        //boost::apply_visitor(AppendValue(static_cast<const TCHAR *>(val)), ResolveRow(row)[col].first);
        ResolveRow(row)[col].first += val;
        //ResolveRow(row)[col].second += static_cast<const TCHAR *>(val);
        m_changed = true;
    }

    IUnknown * GetCellUnknown(__int64 row, unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!RowExists(row))
        {
            return NULL;
        }

        Table::const_iterator itr = m_table.find(row);
        if(itr != m_table.end() && col < m_data[itr->second].size())
        {
            return m_data[itr->second][col].second;
        }

        return NULL;
    }

    void SetCellUnknown(__int64 row, unsigned col, IUnknown * data)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);

        //force the header to be as wide as widest row
        if (col >= m_header.size())
            m_header.resize(col + 1);

        if (col >= ResolveRow(row).size())
            ResolveRow(row).resize( m_header.size() );

        ResolveRow(row)[col].second = data;
        m_changed = true;
    }

    bool RowExists(__int64 row) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return (m_table.find(row) != m_table.end());
    }

    void DoClear()
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        m_rheader.clear();
        m_header.clear();
        m_table.clear();
        m_changed = true;
    }

    bool IsChanged()
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (m_changed)
        {
            m_changed = false;
            return true;
        }
        return false;
    }

    void LoadFrom(ITable * table)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        LoadFrom(table, GetRowCount());
    }

    void LoadFrom(ITable * table, __int64 start)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        m_hasChildDataset = table->GetHasChildDataset();
        for (unsigned i = 0; i < table->GetRowCount(); ++i)
        {
            for (unsigned j = 0; j < table->GetColumnCount(); ++j)
            {
                int newCol = FindColumn(table->GetColumn(j), true, table->GetColumnType(j));
                if (newCol >= 0)
                    SetCell(start + i, newCol, table->GetCell(i, j));
                else
                    SetCell(start + i, j, table->GetCell(i, j));
            }
        }
    }
    
    void CopyFrom(ITable * table, __int64 start, __int64 count)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        ATLASSERT(start + count <= table->GetRowCount());
        m_hasChildDataset = table->GetHasChildDataset();
        for (__int64 i = start; i < start + count; ++i)
        {
            for (unsigned j = 0; j < table->GetColumnCount(); ++j)
            {
                int newCol = FindColumn(table->GetColumn(j), true, table->GetColumnType(j));
                if (newCol >= 0)
                    SetCell(i, newCol, table->GetCell(i, j));
                else
                    SetCell(i, j, table->GetCell(i, j));
            }
        }
    }

    template<typename T>
    bool LoadFrom(T *pMc, const CComBSTR & xml)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        bool retVal = false;

        CComInitialize com;
        CComPtr<ISAXXMLReader> pRdr; 

        HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);//, NULL, CLSCTX_ALL, IID_ISAXXMLReader, (void **)&pRdr); 
        if (hr == S_OK)
        {
            hr = pRdr->putContentHandler(pMc); 
            if (hr == S_OK)
            {
                //SetClipboard(CString(CW2T(xml)));
                hr = pRdr->parse(CComVariant(xml));
                retVal = (hr == S_OK);
            }
        }
        return retVal;
    }

    bool LoadFrom(const CComBSTR & xml, __int64 start)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        ITableMap tmap;
        StlLinked<SeisintDatasetHandlerImpl> pMcXml = new SeisintDatasetHandlerImpl(tmap); 
        bool retVal = LoadFrom(pMcXml.get(), xml);
        if (!retVal)
            return false;

        ITableMap::reverse_iterator lastTable = tmap.rbegin();
        if (lastTable != tmap.rend())
            LoadFrom(lastTable->second.m_table.get(), start);
        return retVal;
    }

    bool LoadFrom(const CComBSTR & xsd, const CComBSTR & xml)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        XsdHandlerImpl pMcXsd(this); 
        bool d = LoadFrom(&pMcXsd, xsd);
        bool d2 = LoadFrom(xml, 0);
        ATLASSERT(d && d2);
        return (d && d2);
    }

    bool LoadFromDataset(const CComBSTR & xml)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        StlLinked<SimpleDatasetHandlerImpl> pMcXml = new SimpleDatasetHandlerImpl(*this); 
        return LoadFrom(pMcXml.get(), xml);
    }

    bool ToXML(std::_tstring & xml, bool skipFirstColumn) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return ToXML(m_table.begin(), m_table.end(), _T("dataset"), _T("table"), xml, skipFirstColumn);
    }
    bool ToXML(const std::_tstring & table_label, const std::_tstring & row_label, std::_tstring & xml, bool skipFirstColumn) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return ToXML(m_table.begin(), m_table.end(), table_label.c_str(), row_label.c_str(), xml, skipFirstColumn);
    }

    bool ToXML(__int64 row, const std::_tstring & table_label, const std::_tstring & row_label, std::_tstring & xml, bool skipFirstColumn) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        Table::const_iterator begin = m_table.find(row);
        if (begin != m_table.end())
        {
            ToXML(m_table.find(row), ++begin, table_label.c_str(), row_label.c_str(), xml, skipFirstColumn);
            return true;
        }
        return false;
    }

    bool ToXML(const Table::const_iterator &begin, const Table::const_iterator &end, const CString &label, const CString &rowLabel, std::_tstring & xml, bool skipFirstColumn) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        CComInitialize com;
        CComPtr<IMXWriter> pWriter;
        HRESULT hr = pWriter.CoCreateInstance(CLSID_MXXMLWriter30, NULL);
        _ASSERT(SUCCEEDED(hr));

        //since this is just returned as a fragment, no header
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
        //pMXAttrs->setAttribute();

        CComBSTR empty(L"");
        if (!label.IsEmpty())
        {
            CComBSTR dataset(label);
            pContentHandler->startElement(empty, empty.Length(), empty, empty.Length(), dataset, dataset.Length(), pSAXAttrs);
        }
        for ( Table::const_iterator i=begin; i!=end; i++)
        {
            ToXML(pContentHandler, pSAXAttrs, rowLabel, i->first, skipFirstColumn);
        }
        if (!label.IsEmpty())
        {
            CComBSTR dataset(label);
            pContentHandler->endElement(empty, empty.Length(), empty, empty.Length(), dataset, dataset.Length());
        }
        // output XML
        var.Clear();
        hr = pWriter->get_output(&var);
        _ASSERT(SUCCEEDED(hr));
        xml = CString(var);

        return true;
    }

    bool ToXML(ISAXContentHandler *pContentHandler, ISAXAttributes *pSAXAttrs, const CString & rowLabel, __int64 _row, bool skipFirstColumn) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        CComBSTR empty(L"");

        Table::const_iterator itr = m_table.find(_row);
        if(itr == m_table.end())
        {
            return false;
        }

        if (!rowLabel.IsEmpty())
        {
            CComBSTR table(rowLabel);
            pContentHandler->startElement(empty, empty.Length(), empty, empty.Length(), table, table.Length(), pSAXAttrs);
        }
        for(unsigned j = skipFirstColumn == true ? 1 : 0; j < m_data[itr->second].size(); ++j)
        {
            CComBSTR column(m_header[j].first.c_str());
            CComBSTR content(m_data[itr->second][j].first.c_str());
            if (content.Length() != 0)
            {
                if (column.Length() == 0)
                {
                    column = "unknown_";
                    column.Append(boost::lexical_cast<std::_tstring>(j + 1).c_str());
                }
                CComPtr<CSparseTableImpl> childDataset = static_cast<CSparseTableImpl *>(GetChildDataset(_row, j));
                pContentHandler->startElement(empty, empty.Length(), empty, empty.Length(), column, column.Length(), pSAXAttrs);
                if (childDataset)
                {
                    for (int childRow = 0; childRow < childDataset->GetRowCount(); ++childRow)
                    {
                        childDataset->ToXML(pContentHandler, pSAXAttrs, rowLabel, childRow, skipFirstColumn);
                    }
                }
                else
                {
                    pContentHandler->characters(content, content.Length());
                }
                pContentHandler->endElement(empty, empty.Length(), empty, empty.Length(), column, column.Length());
            }
        }
        if (!rowLabel.IsEmpty())
        {
            CComBSTR table(rowLabel);
            pContentHandler->endElement(empty, empty.Length(),	empty, empty.Length(), table, table.Length());
        }

        return true;
    }

    unsigned GetCellLength(__int64 row, unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!RowExists(row))
        {
            return 0;
        }

        Table::const_iterator itr = m_table.find(row);
        if(itr != m_table.end() && col < m_data[itr->second].size())
        {
            return m_data[itr->second][col].first.length();
        }

        return 0;
    }

    unsigned GetMaxCellLength(unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        unsigned max = 0;
        for (unsigned row = 0; row < m_table.size();  ++row)
        {
            unsigned rowLen = GetCellLength(row, col);
            max = max < rowLen ? rowLen : max;
        }
        return max;
    }

    void CopyRow(__int64 row, const CResultFormatter &formatter, std::_tstring &line) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        line.clear();
        Table::const_iterator itr = m_table.find(row);
        if(itr == m_table.end())
        {
            return;
        }

        for(unsigned col=0; col < m_data[itr->second].size(); col++)
        {
            formatter.FormatEx(m_data[itr->second][col].first.c_str(), line, col==0, true);
        }

        if (!line.empty())
        {
            formatter.FormatEndOfLine(line);
        }
    }

    bool getHeaderRow(std::_tstring &header, const CResultFormatter &formatter) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);

        //write out the column titles (if the format wants that)
        int colCount = GetColumnCount();
        for (int col=0; col<colCount; col++)
        {
            formatter.FormatHeader(m_header[col].first.c_str(), header, col==0);
        }
        if ( !header.empty() )
        {
            formatter.FormatEndOfLine(header);
            return true;
        }
        return false;
    }

    void SaveAs(std::_tstring &all, const CResultFormatter &formatter) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        std::_tstring line;
        if ( getHeaderRow(line,formatter) )
        {
            all += line;
        }
        for ( Table::const_iterator i=m_table.begin(); i!=m_table.end(); i++)
        {
            CopyRow(i->first,formatter,line);
            all += line;
        }
    }

    void SaveAs(CUnicodeFile &file, const CResultFormatter &formatter) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if ( formatter.FormatsAsXml() )
        {
            file.Write(_T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"));
            std::_tstring xml;
            ToXML(xml, false);
            file.Write(xml);
        }
        else
        {
            std::_tstring line;
            if ( getHeaderRow(line,formatter) )
            {
                file.Write(line);
            }
            for ( Table::const_iterator i=m_table.begin(); i!=m_table.end(); i++)
            {
                CopyRow(i->first,formatter,line);
                file.Write(line);
            }
        }
    }

    //ISearchable
    bool CanBeSearched() const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return GetRowCount() > 0;
    }

    //ISearchable
    bool Search(const CSearchableInfo &info, CSearchableResult &result) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);

        int startRow = (int)info.StartRow();
        int endRow = (int)m_table.size();
        if ( startRow < 0 || startRow >= endRow)
            return false;

        int direction = 1;
        if ( !info.SearchDown() )
        {
            direction = -1;
            endRow = -1;
        }

        for(int i=startRow; i != endRow; i += direction)
        {
            Table::const_iterator itr = m_table.find(i);
            if(itr != m_table.end())
            {
                for(unsigned j = 0; j < m_data[itr->second].size(); ++j)
                {
                    std::_tstring cell;
                    if (info.Find(m_data[itr->second][j].first.c_str()))
                    {
                        result.SetResult(i,j);
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void SplitColumn(unsigned int col, const SplitVector & positions)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        int position_count = positions.size();
        for(int i = 0; i < position_count; ++i)
        {
            InsertColumn(col + i, positions[i].second);
        }

        for(Table::iterator itr = m_table.begin(); itr != m_table.end(); ++itr)
        {
            CString origVal = m_data[itr->second][col].first.c_str();
            Row::iterator row_itr = m_data[itr->second].begin() + col;

            int prevPos = 0;
            for(int i = 0; i < position_count; ++i)
            {
                std::_tstring partialStr = static_cast<const TCHAR *>(origVal.Mid(prevPos, positions[i].first - prevPos));
                row_itr = m_data[itr->second].insert(row_itr, TableCellT(partialStr, (IUnknown *)NULL)) + 1;
                prevPos = positions[i].first;
            }
            std::_tstring partialStr = static_cast<const TCHAR *>(origVal.Mid(prevPos, origVal.GetLength() - prevPos));
            m_data[itr->second][col + position_count].first = partialStr;
        }
    }

    void SetHasChildDataset(bool hasChildDataset=true)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        m_hasChildDataset = hasChildDataset;
    }

    bool GetHasChildDataset() const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return m_hasChildDataset;
    }

    ITable * CreateDataset(const std::_tstring & name, const std::_tstring & value, const std::_tstring & xmlFormat) const
    {
        ITableMap tableMap;
        std::_tstring xml = (boost::_tformat(xmlFormat) % name % value).str();

        CComPtr<ISAXXMLReader> pRdr; 

        pRdr.CoCreateInstance(CLSID_SAXXMLReader30); 

        CComPtr<SeisintDatasetHandlerImpl> pMc = new SeisintDatasetHandlerImpl(tableMap); 
        HRESULT hr = pRdr->putContentHandler(pMc); 

        hr = pRdr->parse(CComVariant(xml.c_str()));
        if (hr == S_OK && tableMap[name].m_table->GetRowCount() > 0)
        {
            return tableMap[name].m_table.Detach();
        }
        return NULL;
    }

    ITable * GetChildDataset(__int64 row, unsigned col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (!RowExists(row))
        {
            return NULL;
        }

        Table::const_iterator itr = m_table.find(row);
        if(itr != m_table.end() && col < m_data[itr->second].size())
        {
            ITable * retVal = CreateDataset(GetColumn(col), m_data[itr->second][col].first.c_str(), _T("<Result><Dataset name=\"%1%\">%2%</Dataset></Result>"));
            if (retVal)
                return retVal;

            retVal = CreateDataset(GetColumn(col), m_data[itr->second][col].first.c_str(), _T("<Result><Dataset name=\"%1%\"><Row>%2%</Row></Dataset></Result>"));
            if (retVal)  //Allow for header
                return retVal;
        }

        return NULL;
    }

    ITable * GetChildDataset(__int64 row, const std::_tstring & col) const
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        return GetChildDataset(row, FindColumn(col));
    }

    void ResetSort()
    {
        ATLASSERT(!"todo");
    }

    void Sort(unsigned int col)
    {
        clib::scoped_mutex_lock scoped_lock(m_mutex);
        if (m_nSortColumn == col)
        {
            m_bAscending = !m_bAscending;
        }
        else
        {
            m_bAscending = TRUE;
            m_nSortColumn = col;
        }
        SortAgain();
    }
    void SortAgain()
    {
        m_compare.Init(m_nSortColumn, GetColumnType(m_nSortColumn), m_bAscending);
        std::sort(m_data.begin(), m_data.end(), m_compare);
#ifdef DEBUG
        for(Data::iterator itr = m_data.begin(); itr != m_data.end(); ++itr)
        {
            ATLTRACE(_T("%s\n"), itr->operator[](0).first.c_str());
        }
#endif
    }
};

CLIB_API ISparseTable * CreateISparseTable()
{
    return new CSparseTableImpl();
}
