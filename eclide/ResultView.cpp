#include "stdafx.h"
#include "..\en_us\resource.h"

#include "mainfrm.h"
#include "ResultView.h"
#include "global.h"
#include "Table.h"
#include "clib.h"
#include "wutil.h"
#include "unicodefile.h"
#include <RecursiveMutex.h> //clib
#include <shellHelper.h>
#include <GotoDlg.h>
#include "FindDlgMgr.h"
#include <HtmlView.h>
#include <UrlMon.h>
#include <AtlTime.h>
#include "thread.h"
#include <preferencedlg.h> //wlib
#include "SummaryView.h"
#include "ExceptionView.h"
#include "GraphView.h"
#include "GraphView2.h"
#include "GraphView3.h"
#include "DebugView.h"
#include "attributedlg.h"

#include "atlsgrid.h"

const TCHAR * COPY = _T("copy");
const TCHAR * DRILLDOWN = _T("drilldown");
const TCHAR * DRILLDOWN_ECL = _T("%s.%s(%s);");
const TCHAR * DRILLDOWN_ECL2 = _T("%1%.%2%();");

enum UM
{
	UM_FIRST = CWM_LAST + 1,
	UM_SHOWDATASETURL,
	UM_SHOWECLWATCH,
	UM_CONFIGUREGRID,
	UM_LAST
};

const TCHAR * DoCat(CString & a, const CString & b = _T(""))
{
	a += b;
	return a;
}

const TCHAR * GetFramedWorkUnitEclWatchURL(Dali::IWorkunit *wu, CString &url)
{
	//tp://IP:PORT?inner=/WsWorkunits/WUInfo?Wuid=W20050302-182157
	std::_tstring str = static_cast<const TCHAR * >(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT)));
	boost::algorithm::ireplace_first(str, _T("/WsWorkunits"), _T("?inner=/WsWorkunits/WUInfo?Wuid="));
	url = str.c_str();
	url += wu->GetWuid();
	return url;
}

const TCHAR * GetWorkUnitEclWatchURL(Dali::IWorkunit *wu, CString &url)
{
	url = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT);
	url += _T("/WUInfo?Wuid=");
	url += wu->GetWuid();
	return url;
}

const TCHAR * GetResultEclWatchURL(Dali::IWorkunit *wu, CString &url, int sequence)
{
	//http://10.150.64.208:8010/WsWorkunits/WUResult?Wuid=W20041006-101104&Sequence=0
	url = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT);
	url += _T("/WUResult?Wuid=");
	url += wu->GetWuid();
	url += _T("&Sequence=");
	url += boost::lexical_cast<std::_tstring>(sequence).c_str();
	return url;
}

static const TCHAR * const BOOLEANSTR = _T("boolean");
static const TCHAR * const INTEGER4 = _T("integer4");
static const TCHAR * const UINTEGER4 = _T("unsigned integer4");
static const TCHAR * const REAL = _T("real");
static const TCHAR * const VARSTRING = _T("varstring");
// =============================================================================
class CResultDataSource : public IDataSource, public ISearchable, public CUnknown, public boost::signals::trackable
{
protected:
	CString m_wuid;
	CComPtr<Dali::IResult> m_result;
	StlLinked<ISparseTable> m_table;
	__int64 m_total;
	__int64 m_totalLoaded;
	typedef std::vector<IDataSourceListener *> ListenerVector;
	ListenerVector m_listeners;
	IResultSlot* m_resultSlot;
	Dali::CServerConfig m_daliConfig;
	StlLinked<clib::CThreadQueue> m_pool;
	clib::recursive_mutex m_mutex;

public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CResultDataSource(const CString & wuid, Dali::IResult * result, IResultSlot *resultSlot)
		: m_wuid(wuid), m_result(result), m_resultSlot(resultSlot),
		  m_daliConfig(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT)),_T("DALI"))
	{
		m_table = CreateISparseTable();
		m_table->SetColumn(0, _T("##"), CT_UINT);
		m_total = 1;
		m_totalLoaded = 0;
		m_pool = new clib::CThreadQueue(2);
	}

	~CResultDataSource()
	{
	}

	bool GetHasChildDataset()
	{
		return m_table->GetHasChildDataset();
	}

	//ISearchable
	bool CanBeSearched() const
	{
		return m_table->CanBeSearched();
	}
	//ISearchable
	bool Search(const CSearchableInfo &info, CSearchableResult &result) const 
	{
		return m_table->Search(info,result);
	}
	bool GetXML(std::_tstring & xml, bool skipFirstColumn)
	{
		return m_table->ToXML(xml, skipFirstColumn);
	}
	bool GetXML(__int64 row, const std::_tstring & table_label, const std::_tstring & row_label, std::_tstring & xml, bool skipFirstColumn)
	{
		return m_table->ToXML(row, table_label, row_label, xml, skipFirstColumn);
	}
	bool RequestedRow(__int64 row)
	{
		return m_table->RowExists(row);
	}
	bool CopyRow(__int64 row, const CResultFormatter &formatter, std::_tstring & line)
	{
		if ( m_table->RowExists(row) )
		{
			m_table->CopyRow(row,formatter,line);
			return true;
		}
		else
			return false;
	}

	bool DoCopyRows(const CGridSelectionCollection& gridSel, const CResultFormatter &formatter, std::_tstring &sel)
	{
		CGridSelectionCollection::SelectionRows rows;
		if ( gridSel.GetSelectionRows(rows) )
		{
			for(unsigned r = 0; r < rows.size(); r++)
			{
				std::_tstring row;
				CopyRow(rows[r], formatter, row);
				sel += row;
			}
			return true;
		}
		else
			return false;
	}		

	void DoCopyRow(__int64 r, CCellRange & gridSel, const CResultFormatter &formatter, std::_tstring & sel, bool cut, CGridCtrlEx* gc=0)
	{
		std::_tstring row;
		bool bAnyData = false;
		for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
		{
			bAnyData = true;
			int truec;
			if(gc!=0)
				truec=gc->DisplayToDataColumn(c);
			else
				truec=c;
			formatter.FormatEx(GetCell(r,truec), row, c == gridSel.GetMinCol(), IsColQuoted(truec));
			if (cut)
				SetCell(r,truec,_T(""));
		}
		if ( bAnyData )
		{
			formatter.FormatEndOfLine(row);
			sel += row;
		}
	}

	bool DoCopy(const CCellRange & _gridSel, const CResultFormatter &formatter, std::_tstring & sel, bool cut, bool withHeaders, CGridCtrlEx* gc=0)
	{
		CCellRange gridSel = _gridSel;
		if (gridSel.IsValid())
		{
			if (withHeaders)
				DoCopyRow(-1, gridSel, formatter, sel, cut, gc);
			for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
			{
				DoCopyRow(r, gridSel, formatter, sel, cut, gc);
			}
			return true;
		}
		else
			return false;
	}

	bool DoCopyBox(const CGridSelectionCollection& gridSel, const CResultFormatter &formatter, std::_tstring & sel)
	{
		CGridSelectionCollection::SelectionBox box;
		if ( gridSel.GetSelectionBox(box,GetRowCount(),GetColCount()) )
		{
			for(__int64 r = box.rowTop; r <= box.rowBottom; r++)
			{
				std::_tstring row;
				bool bAnyData = false;
				for(int c=box.columnLeft; c<=box.columnRight; c++)
				{
					bool isSelected = gridSel.IsSelected(r,c);
					bAnyData |= isSelected;
					formatter.FormatEx(
						isSelected ? GetCell(r,c) : _T(""),
						row, c == box.columnLeft, IsColQuoted(c));
				}
				if ( bAnyData )
				{
					formatter.FormatEndOfLine(row);
					sel += row;
				}
			}
			return true;
		}
		else
			return false;
	}

	bool DoCopyColumns(const CGridSelectionCollection& gridSel, const CResultFormatter &formatter, std::_tstring & sel)
	{
		CGridSelectionCollection::SelectionColumns columns;
		if ( gridSel.GetSelectionColumns(columns) )
		{
			for(unsigned pos = 0; pos < GetRowCount(); pos++)
			{
				std::_tstring row;
				for(unsigned c=0; c<columns.size(); c++)
				{
					formatter.FormatEx(GetCell(pos, columns[c]),row,c==0, IsColQuoted(c));
				}
				formatter.FormatEndOfLine(row);
				sel += row;
			}
			return true;
		}
		else
			return false;
	}


	void SaveAs(std::_tstring & target, const CResultFormatter &formatter, const CGridSelectionCollection* selection, bool withHeaders)
	{
		ATLASSERT(!"Not used but will need withHeaders support if it is");
		if ( !selection || !selection->HasSelection() || selection->GetSelectionAll() )
		{
			m_table->SaveAs(target, formatter);
		}
		else
		{
			target.clear();
			DoCopyRows(*selection,formatter,target) || 
			DoCopyColumns(*selection,formatter,target) || 
			DoCopyBox(*selection,formatter,target);
		}
	}

	void SaveAs(CUnicodeFile &file, const CResultFormatter &formatter, const CGridSelectionCollection* selection, bool withHeaders)
	{
		ATLASSERT(!"Not used but will need withHeaders support if it is");
		if ( !selection || !selection->HasSelection() || selection->GetSelectionAll() )
		{
			m_table->SaveAs(file,formatter);
		}
		else
		{
			std::_tstring sel;
			if ( DoCopyRows(*selection,formatter,sel) || 
				 DoCopyColumns(*selection,formatter,sel) || 
				 DoCopyBox(*selection,formatter,sel) )
			{
				file.Write(sel);
			}
		}
	}

	void SaveAs(std::_tstring & target, const CResultFormatter &formatter, const CCellRange & selection, bool cut, bool withHeaders, CGridCtrlEx* gc)
	{
		if (selection.IsValid())
		{
			target.clear();
			DoCopy(selection, formatter, target, cut, withHeaders, gc);
		}
	}

	void SaveAs(CUnicodeFile &file, const CResultFormatter &formatter, const CCellRange & selection, bool cut, bool withHeaders)
	{
		if (selection.IsValid())
		{
			std::_tstring sel;
			DoCopy(selection, formatter, sel, cut, withHeaders);
			file.Write(sel);
		}
	}

	void RemoveRow(__int64 row)
	{
		if (m_table->RemoveRow(row))
		{
			--m_total;
			--m_totalLoaded;
		}
	}

	void AddRows(int count)
	{
		m_total+=count;
		m_totalLoaded+=count;
	}

	const TCHAR * GetCell(__int64 row, int col)
	{
		if (row == -1)
		{
			return m_table->GetColumn(col);
		}
		return m_table->GetCell(row, col);
	}

	void SetCell(__int64 row, int col, const std::_tstring & val)
	{
		if (row == -1)
		{
			return;
		}
		m_table->SetCell(row, col, val);
	}

	void SetColumn(unsigned col, const std::_tstring & val, COLUMN_TYPE_ENUM type)
	{
		m_table->SetColumn(col, val, type);
	}

#define MIN_CELL_REQ 512
#define MIN_ROW_REQ 24
#define MAX_CELL_REQ (3*1024)
#define MAX_ROW_REQ (512)

	int MinPageSize()
	{
		int cols = m_table->GetColumnCount();
		if ( !cols )
		{
			return MIN_ROW_REQ; //don't get too many if we don't know column size
		}
		else
		{
			return min(MIN_ROW_REQ, MIN_CELL_REQ / cols);
		}

	}

	int MaxPageSize()
	{
		int cols = m_table->GetColumnCount();
		if ( !cols )
		{
			return MIN_ROW_REQ; //don't get too many if we don't know column size
		}
		else
		{
			return min(MAX_ROW_REQ, MAX_CELL_REQ / cols);
		}

	}
	void SetLoaded(__int64 start, int count)
	{
		clib::scoped_lock_ref_counted_adapt lock;
		m_table->GetScopedLock(lock);

		//ATLASSERT(GetRowsLoaded() <= m_total);

		//update the row number cell to be normal
		for(int i = 0; i < count; ++i)
		{
			//test to see if we are loading a row for the first time "(#)" in first cell
			if ( '(' == *m_table->GetCell(start+i,0) )
			{
				++m_totalLoaded;
				//ATLASSERT(m_totalLoaded <= m_total);
				m_table->SetCell(start + i, 0, boost::lexical_cast<std::_tstring>(start + i + 1).c_str());
			}
		}
		if ( !count && !m_total )
		{
			m_table->SetCell(0, 0, _T("No results returned"));
		}
	}
	std::pair<__int64, __int64> PrepCache(__int64 rowFrom, __int64 rowTo, bool bHighPriority)
	{
		return PrepCache(rowFrom, rowTo, boost::ref(*this), bHighPriority);
	}
	std::pair<__int64, __int64> PrepCache(__int64 rowFrom, __int64 rowTo, const Dali::async_datacomplete_slot_type &s, bool bHighPriority)
	{
		if (rowFrom == -1)
		{
			//  Header is already loaded/
			rowFrom++;
		}
		bool doPrep = false;
		__int64 startPos,endPos;
		int count = static_cast<int>(rowTo - rowFrom);
		{
			clib::scoped_lock_ref_counted_adapt lock;
			m_table->GetScopedLock(lock);

			int minPageSize = MinPageSize();
			float midPoint = (float)rowFrom + (float)(count) / 2.0f;
			if (count < minPageSize)
				count = minPageSize;
			startPos = floor(midPoint) - count / 2;
			if (startPos < 0)
				startPos = 0;
			endPos = ceil(midPoint) + count / 2;
			if (endPos >= m_total)
				endPos = m_total - 1;

			for(; startPos <= endPos; ++startPos)
			{
				if (!m_table->RowExists(startPos))
					break;
			}

			for(; endPos >= startPos; --endPos)
			{
				if (!m_table->RowExists(endPos))
					break;
			}
			if (startPos <= endPos && startPos <= rowTo && endPos >= rowFrom)
			{
				doPrep = true;
				for (__int64 i = startPos; i <= endPos; ++i)
				{
					if (!m_table->RowExists(i))
					{
						m_table->SetCell(i, 0, (boost::_tformat(_T("(%i)")) % (i + 1)).str());
					}
				}
			}
		}

		if (doPrep)
		{
			count = static_cast<int>(endPos - startPos + 1);
			ATLTRACE(_T("PrepCache - doPrep (%d) %I64d for %d \n"), m_result->GetSequence(), startPos, count);

			Dali::CResultDataRequest* req = new Dali::CResultDataRequest(s);
			req->m_result = m_result;
			req->m_start = startPos;
			req->m_count = count;
			req->m_resultTable = m_table.get();

			if ( bHighPriority )
				m_pool->Push(__FUNCTION__, boost::bind(&GetResultData, m_daliConfig, req));
			else
				m_pool->Append(__FUNCTION__, boost::bind(&GetResultData, m_daliConfig, req));

			return std::make_pair(startPos, endPos);
		}
		else
		{
			return std::make_pair(-1, -1);
		}
	}

	static void GetResultData(Dali::CServerConfig config, Dali::CResultDataRequest* req)
	{
		StlLinked<Dali::CResultDataRequest> ownedReq = req;
		Dali::GetResultData(config,req);
	}

	void AddListener(IDataSourceListener *listener, bool bAdd)
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		if ( bAdd )
		{
			m_listeners.push_back(listener);
		}
		else 
		{
			ListenerVector::iterator itr = std::find(m_listeners.begin(), m_listeners.end(), listener);
			if (itr != m_listeners.end())
			{
				m_listeners.erase(itr);
			}
		}
	}

	void Sort(unsigned int col) 
	{ 
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		CWaitCursor wait;
		m_table->Sort(col);
	} 

	void operator()(__int64 start, int count, __int64 total, bool success) 
	{
		clib::recursive_mutex::scoped_lock lock(m_mutex);
		SetLoaded(start, count);
		//notify all listeners
		ListenerVector listenersToRemove;
		for (ListenerVector::iterator i = m_listeners.begin(); i != m_listeners.end(); ++i)
		{
			IDataSourceListener *listener = *i;
			ATLASSERT(listener);
			if ( listener && !listener->DataLoaded(start, count, total) )
			{
				listenersToRemove.push_back(listener);
			}
		}

		for (ListenerVector::iterator i = listenersToRemove.begin(); i != listenersToRemove.end(); ++i)
		{
			IDataSourceListener *listener = *i;
			ListenerVector::iterator found = std::find(m_listeners.begin(), m_listeners.end(), listener) ;
			if (found != m_listeners.end())
			{
				m_listeners.erase(found);
			}
		}
		m_resultSlot->PostResults(start,count,total);
	}

	__int64 GetRowsLoaded() 
	{
		return m_totalLoaded;

		//GetRowCount will include all requested rows, not just the ones loaded
		//return m_table->GetRowCount();
	}
	bool IsFullyLoaded()
	{
		return (GetRowsRemaining() == 0);
	}

	__int64 GetRowsRemaining() const
	{
		return m_total - m_totalLoaded;
		//return m_total - m_table->GetRowCount();
	}
	void SetRowCount(__int64 rowCount)
	{
		m_total = rowCount;
	}

	void SetColumnLabel(unsigned int col, const std::_tstring & label)
	{
		m_table->SetColumn(col, label.c_str());
	}

	unsigned int SplitColumn(unsigned int col, const SplitVector & positions)
	{
		m_table->SplitColumn(col, positions);
		return m_table->GetColumnCount();
	}

	//IDataSource
	__int64 GetRowCount()
	{
		return m_total;
	}
	__int64 GetPhysicalRow(__int64 disprow)
	{
		return _ttoi(m_table->GetCell(disprow, 0)) - 1;
	}

	__int64 GetPhysicalRowCount() const
	{
		return m_table->GetRowCount();
	}

	IUnknown * GetCellUnknown(__int64 row, unsigned col) const
	{
		return	m_table->GetCellUnknown(row, col);
	}

	void SetCellUnknown(__int64 row, unsigned col, IUnknown * data)
	{
		m_table->SetCellUnknown(row, col, data);
	}

	//IDataSource
	int GetColCount()
	{
		return m_table->GetColumnCount();
	}
	const TCHAR * GetColumn(int i)
	{
		return m_table->GetColumn(i);
	}
	const TCHAR * GetColumnTypeAsECL(int i, std::_tstring & type) const
	{
		switch (m_table->GetColumnType(i))
		{
		case CT_BOOLEAN:
			type = BOOLEANSTR;
			break;
		case CT_INT:
			type = INTEGER4;
			break;
		case CT_UINT:
			type = UINTEGER4;
			break;
		case CT_REAL:
			type = REAL;
			break;
		case CT_STRING:
			type = VARSTRING;
			break;
		case CT_DATA:
			type = VARSTRING;
			break;
		case CT_DEC:
			type = VARSTRING;
			break;
		case CT_UDEC:
			type = VARSTRING;
			break;
		default:
			type = VARSTRING;
		}
		return type.c_str();
	}
	bool IsColQuoted(int col) const
	{
		std::_tstring type;
		return boost::algorithm::iequals(GetColumnTypeAsECL(col, type), VARSTRING);
	}
	const TCHAR * GetCellQuoted(__int64 row, int col, std::_tstring & result)
	{
		if (IsColQuoted(col))
		{
			::EncodeForEcl(GetCell(row, col), result);
		}
		else
		{
			::EncodeForEclNoQuote(GetCell(row, col), result);
		}
		return result.c_str();
	}
	__int64 FindRow(int & /*column*/)
	{
		return m_total;
	}
	void DebugToFile()
	{
		std::_tstring xml;
		m_table->ToXML(xml, false);
		std::ofstream fs("c:\\temp\\tmp.xml");
		fs << CT2A(xml.c_str());
	}
};
typedef StlLinked<CResultDataSource> CResultDataSourceAdapt;
//================================================
class CResultDownload : 
	public IDataSourceListener,
	public CUnknown
{
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CResultDownload(IResultSlot *resultSlot, bool withHeaders) : m_resultSlot(resultSlot), m_withHeaders(withHeaders)
	{
	}

	~CResultDownload()
	{
		Abort();
	}

	IResultSlot *m_resultSlot;

	void Abort()
	{
		if ( m_data.isLinked() )
		{
			CResultDataSourceAdapt data = m_data;
			m_data = 0;
			data->AddListener(this,false);
		}
	}

	bool Init(HWND hWndNotify, const TCHAR* path, const CResultFormatter &formatter, CResultDataSourceAdapt data, const CGridSelectionCollection* selection, WPARAM postFlag)
	{
		m_hWndNotify = hWndNotify;
		m_rows = data->GetRowCount();
		m_lastRow = 0;
		m_formatter = formatter.Clone();
		m_data = data;
		m_postFlag = postFlag;
		m_data->AddListener(this,true);
		m_path = path;
		if ( selection )
		{
			m_selection1 = *selection;
		}
		CUnicodeFile::ENCODING encoding = (CUnicodeFile::ENCODING)m_formatter->FileEncoding();
		return m_file.Create(path, GENERIC_WRITE, CREATE_ALWAYS, encoding);
	}

	bool Init(HWND hWndNotify, const TCHAR* path, const CResultFormatter &formatter, CResultDataSourceAdapt data, const CCellRange * selection, WPARAM postFlag)
	{
		m_hWndNotify = hWndNotify;
		m_rows = data->GetRowCount();
		m_lastRow = 0;
		m_formatter = formatter.Clone();
		m_data = data;
		m_postFlag = postFlag;
		m_data->AddListener(this,true);
		m_path = path;
		if (selection)
		{
			m_selection2 = *selection;
		}
		CUnicodeFile::ENCODING encoding = (CUnicodeFile::ENCODING)m_formatter->FileEncoding();
		return m_file.Create(path, GENERIC_WRITE, CREATE_ALWAYS, encoding);
	}

	void Save()
	{
		m_startTime = CTime::GetCurrentTime();
		ProcessTable();
	}

	//IDataSourceListener
	bool DataLoaded(__int64 /*start*/, int /*count*/, __int64 /*total*/) 
	{
		__int64 rowsRemaining = m_data->GetRowsRemaining();
		if ( rowsRemaining > 0 )
		{
			CString msg;
			msg.Format(_T("%I64d remaining"), rowsRemaining );
			m_resultSlot->PostStatus(msg);
			ProcessTable();
			return true; //keep listening
		}
		else
		{
			CTimeSpan diff = CTime::GetCurrentTime() - m_startTime;
			CString msg;
			msg.Format(_T("Download Completed in %d seconds"), diff.GetSeconds());
			m_resultSlot->PostStatus(msg);
			if (m_selection2.IsValid())
			{
				m_data->SaveAs(m_file, *(m_formatter.get()), m_selection2, false, m_withHeaders);
			}
			else
			{
				m_data->SaveAs(m_file, *(m_formatter.get()), &m_selection1, m_withHeaders);
			}
			m_file.Close();
			m_data = 0;
			::PostMessage(m_hWndNotify,CWM_BACKGROUNDSAVECOMPLETE,m_postFlag,0);
			return false; //stop listening
		}
	}

	const TCHAR* GetFilePath()
	{
		return m_path.c_str();
	}

protected:
	HWND	m_hWndNotify;
	__int64 m_lastRow;
	__int64 m_rows;
	StlLinked<CResultFormatter> m_formatter;
	CResultDataSourceAdapt	m_data;
	CGridSelectionCollection m_selection1;
	CCellRange m_selection2;
	bool m_withHeaders;
	CUnicodeFile			m_file;
	std::_tstring				m_path;
	CTime					m_startTime;
	WPARAM					m_postFlag;

	void ProcessTable()
	{
		//keep going until we hit a row that hasn't been requested
		for(__int64 i = m_lastRow; i < m_rows; i++)
		{
			if ( !m_data->RequestedRow(i) )
			{
				m_lastRow = m_data->PrepCache(i, i+m_data->MaxPageSize(), false).first;
				break;
			}
		}
	}
};
//  ======================================================================================================================================================
class CNestedDataSetResultView : public WTL::CDialogImpl<CNestedDataSetResultView>
{
	typedef CNestedDataSetResultView thisClass;
	CString m_url;
	WTL::CHyperLink m_link;

public:
	CNestedDataSetResultView()
	{
	}

	void SetURL(const TCHAR *url)
	{
		m_url = url;
		if ( IsWindow() )
			m_link.SetHyperLink(m_url);
	}

	enum { IDD = IDD_DIALOG_NESTED_DATASET };

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_COMMAND(OnCommand)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
	{
		SetMsgHandled(false);
		m_link.SetHyperLinkExtendedStyle(HLINK_COMMANDBUTTON,HLINK_COMMANDBUTTON);
		m_link.SetHyperLink(m_url);
		m_link.SubclassWindow(GetDlgItem(IDC_STATIC_ECLWATCH));
		return 0;
	}

	LRESULT OnCommand(UINT, int, HWND)
	{
		GetParent().SendMessage(UM_SHOWECLWATCH);
		return 0;	
	}
};
//  ======================================================================================================================================================
class CResultView : 
	public WTL::CDialogImpl<CResultView>,
	public boost::signals::trackable, 
	public CTabPane,
	public IResultGridView
{
	typedef CResultView thisClass;
	typedef WTL::CDialogImpl<thisClass> baseClass;

private:
	CComPtr<Dali::IWorkunit> m_wu;
	CComPtr<Dali::IResult> m_result;
	int	m_contextColumn;
	__int64	m_contextRow;
	CNestedDataSetResultView m_nested;
	CRect m_rcNested;

	CGridCtrlEx m_list;
	CResultDataSourceAdapt m_data;

	IResultSlot* m_resultSlot;
	std::vector<bool> m_hexColumns;
	IAttributeVector* m_drilldown;
	StlLinked<CResultDownload> m_backgroundSave;
	CSearchableInfo m_searchInfo;
	CSearchableResult m_searchResult;
	LONG m_prepped;

	std::_tstring m_CSVType; 

public:
	CResultView(Dali::IWorkunit *wu, IResultSlot *resultSlot, Dali::IResult * result, IAttributeVector *drilldown) 
		: m_wu(wu), m_result(result), m_drilldown(drilldown), m_resultSlot(resultSlot)
	{
		m_data = new CResultDataSource(wu->GetWuid(), result, m_resultSlot);
		m_list.SetDataSource(m_data.get());
		m_prepped = 0;
		::DocumentTypeName(_T("foo.csv"), m_CSVType);		
	}

	CResultView::~CResultView()
	{
	}

	enum { IDD = IDD_RESULTVIEW };

	//WPARAM on CWM_BACKGROUNDSAVECOMPLETE
	enum { BackgoundSaveToFile, BackgroundSaveToExcel };

	BEGIN_CUNKNOWN
		IMPLEMENT_INTERFACE(IResultGridView)
		IMPLEMENT_INTERFACE(IResultView)
	END_CUNKNOWN(CTabPane)

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)

		MSG_WM_PAINT(OnPaint);

		MSG_WM_CONTEXTMENU(OnContextMenu)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_SIZE(OnSize)

		MSG_WM_KEYUP(OnKeyUp)

		MESSAGE_HANDLER_EX(CWM_INITIALIZE, OnInitialize)
		MESSAGE_HANDLER_EX(CWM_DRILLDOWNSINGLE, OnDrilldownSingle)
		MESSAGE_HANDLER_EX(CWM_DRILLDOWN, OnDrilldown)
		MESSAGE_HANDLER_EX(CWM_DRILLDOWN2, OnDrilldown2)
		MESSAGE_HANDLER_EX(CWM_BACKGROUNDSAVECOMPLETE, OnBackgroundSaveComplete)
		MESSAGE_HANDLER_EX(UM_SHOWDATASETURL, OnShowDataSetURL)
		MESSAGE_HANDLER_EX(UM_SHOWECLWATCH, OnShowEclWatch)
		MESSAGE_HANDLER_EX(UM_CONFIGUREGRID, OnResultConfigureGrid)

		COMMAND_ID_HANDLER_EX(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER_EX(ID_FILE_SAVE_ASXML, OnFileSaveAsXML)

		COMMAND_ID_HANDLER_EX(ID_EDIT_CUT, OnEditCut)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPY, OnEditCopy)
		COMMAND_ID_HANDLER_EX(ID_EDIT_PASTE, OnEditPaste)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPYASECL, OnEditCopyAsEcl)
		COMMAND_ID_HANDLER_EX(ID_EDIT_SELECT_ALL, OnEditSelectAll)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPYASCSV, OnEditCopyToCSV)
		COMMAND_ID_HANDLER_EX(ID_EDIT_COPYASCSV_WITHHEADERS, OnEditCopyToCSVWithHeaders)
		COMMAND_ID_HANDLER_EX(ID_EDIT_GOTO, OnEditGoTo)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FIND, OnEditFind)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDNEXT, OnEditFindNext)
		COMMAND_ID_HANDLER_EX(ID_EDIT_FINDPREV, OnEditFindPrev)
		COMMAND_ID_HANDLER_EX(ID_RESULTS_SHOWCOLUMNINHEX, OnResultsFormatHex)
		COMMAND_ID_HANDLER_EX(ID_RESULTS_AUTOSIZECOLUMN, OnResultsAutoSizeColumn)
		COMMAND_ID_HANDLER_EX(ID_RESULTS_AUTOSIZEALLCOLUMNS, OnResultsAutoSizeAllColumns)
		COMMAND_ID_HANDLER_EX(ID_RESULTS_SELECTCOLUMN, OnEditSelectColumn)
		COMMAND_ID_HANDLER_EX(ID_RESULTS_SELECTROW, OnEditSelectRow)
		COMMAND_ID_HANDLER_EX(ID_RESULT_DRILLDOWN_SELECT, OnEditDrilldownSelect)
		COMMAND_ID_HANDLER_EX(ID_RESULT_DRILLDOWNSINGLE_SELECT, OnEditDrilldownSingleSelect)
		COMMAND_ID_HANDLER_EX(ID_RESULT_DRILLDOWN2_SELECT, OnEditDrilldown2Select)
		COMMAND_ID_HANDLER_EX(ID_RESULT_DELETE_SELECTION, OnResultDeleteColRow)
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FONTSTYLE, OnSelectionLogFont)
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FONTBOLD, OnBoldenSelection)	
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FONTITALIC, OnItalicizeSelection)	
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FONTCOLOR, OnSelectionTextColor)	
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FILLCOLOR, OnSelectionBkColor)
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FONTCOLOR_SELECT, OnTextColorSelect)	
		COMMAND_ID_HANDLER_EX(ID_FORMAT_FILLCOLOR_SELECT, OnBkColorSelect)

		//COMMAND_ID_HANDLER_EX(ID_RESULTS_DRILLDOWN, OnResultsDrilldown)
		MESSAGE_HANDLER(WM_COMMAND, OnFindReplaceCmd)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void AutoSizeColumn(int column)
	{
		CWaitCursor cursor;
		if ( column < 0 )
		{
			m_list.AutoSizeColumns(0, m_list.GetColCount());
		}
		else
		{
			m_list.AutoSizeColumns(m_contextColumn, m_contextColumn);
		}
	}

	LRESULT OnShowDataSetURL(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		ATLASSERT(m_nested.m_hWnd==NULL);
		m_nested.Create(*this);
		m_nested.GetWindowRect(&m_rcNested);
		CString url;
		m_nested.SetURL(GetWorkUnitEclWatchURL(m_wu,url));
		//need to force update the Window
		CRect rc;
		GetWindowRect(&rc);
		CSize sz;
		sz.cx = rc.Width();
		sz.cy = rc.Height();
		OnSize(SIZE_RESTORED, sz);
		return 0;
	}

	LRESULT OnResultConfigureGrid(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		//m_resultSlot->ConfigureGrid(m_list);
		return 0;
	}

	LRESULT OnShowEclWatch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		GetParent().GetParent().SendMessage(UM_SHOWECLWATCH);
		return 0;
	}

    LRESULT OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
	{
		SetMsgHandled(false);
		m_name = m_result->GetName();
		SetWindowText(m_name);

		CRect rc;
		DWORD style;
		FindDialogControlPlaceholder(*this, IDC_STATIC_PLACEHOLDER, rc, style);
		GetClientRect(&rc);
		m_list.Create(*this, rc, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP, NULL, IDC_STATIC_PLACEHOLDER);

		return 0;
	}

	LRESULT OnInitialize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
	{
		m_list.SetCallbackFunc(m_list.GridCallback, (LPARAM)&m_list);
		m_list.SetCallbackFuncPrepCache(m_list.GridCallbackPrepCache, (LPARAM)&m_list);
		m_list.SetCallbackFuncCellClick(m_list.GridCallbackCellClick, (LPARAM)&m_list);
		m_list.SetFixedRowCount(1);
		m_list.SetFixedColumnCount(1);
		m_list.SetTrim(true);

		std::_tstring fontName = CString((GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_FONT_RESULT)));
		int fontSize = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_FONTSIZE_RESULT);

		m_list.SetCellFont(fontName, fontSize);

		m_data->PrepCache(0, 10, boost::ref(*this), true);

		//CRect rc;
		//GetClientRect(&rc);
		//m_list.MoveWindow(rc, false);

		return 0;
	}

	void OnSize(UINT wParam, const CSize & size)
	{
		if(IsWindow() && wParam != SIZE_MINIMIZED)
		{
			UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING;
			int nestedHeight = 0;
			if ( m_nested.IsWindow() )
			{
				nestedHeight = m_rcNested.Height();
				m_nested.SetWindowPos( NULL, 0, 0, size.cx, nestedHeight, flags);
			}
			if (m_list.IsWindow())
				m_list.SetWindowPos( NULL, 0, nestedHeight, size.cx, size.cy-nestedHeight, flags);
		}
	}

	void OnKeyUp(TCHAR chr, UINT repeatCount, UINT exInfo)
	{
		if (GetKeyState(VK_LCONTROL) > 0 || GetKeyState(VK_RCONTROL) > 0)
		{
			switch (chr)
			{
			case 'x':
			case 'X':
				break;
			case 'c':
			case 'C':
				break;
			case 'v':
			case 'V':
				break;
			}
		}

	}

	LRESULT OnPaint(HDC /*dc*/)
	{
		SetMsgHandled(false);
		if (::InterlockedIncrement(&m_prepped) == 1)
		{
			PostMessage(CWM_INITIALIZE);
		}
		return 1;
	}

	const CString& GetTabToolTip(CString &tip) const 
	{ 
		tip = m_name; 
		CString value(m_result->GetValue());
		if ( !value.IsEmpty() )
			tip += _T(" = ") + value;
		return tip; 
	};

	void ResetSearch(bool bNext)
	{
		m_searchResult.SetResult(0, false);
		DWORD flags = GetFindReplace().m_dwFlags;
		flags &= ~FR_DOWN;
		flags |= FR_FINDNEXT | (bNext ? FR_DOWN : 0);
		m_searchInfo.Init(GetFindReplace().m_szFindWhat, flags);
	}

	void DoEditFind()
	{
		m_searchInfo.SetPosition(-1);
		FindReplaceOnEditFind(_T(""), false, false);
	}

	void OnEditFind(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFind();
	}

	LRESULT OnFindReplaceCmd(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		if ( wParam != ID_EDIT_FINDREPLACECMD )
		{
			bHandled = FALSE;
			return 1;
		}

		if(GetFindReplace().FindNext())
		{
			//search up would be previous
			DoEditFindAgain(GetFindReplace().SearchDown() == TRUE);
		}
		return 0;
	}

	void DoEditFindAgain(bool bNext)
	{
		if(GetFindReplace().m_szFindWhat[0] == 0)
		{
			DoEditFind();
			return;
		}

		ResetSearch(bNext);

		if ( !m_data->Search(m_searchInfo, m_searchResult))
			::MessageBeep(MB_ICONEXCLAMATION);
		else
		{	
			m_searchInfo.SetPosition(m_searchResult.m_row);
			m_list.GotoLine(m_searchResult.m_row, m_searchResult.m_col);
		}
	}

	void OnEditFindNext(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFindAgain(true);
	}

	void OnEditFindPrev(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoEditFindAgain(false);
	}

	LRESULT OnDrilldownSingle(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
	{
		ATLASSERT(IsDrilldownColumn(wParam));
		DoDrilldownSingle(DRILLDOWN, m_data->GetColumn(wParam));
		return 0;
	}

	LRESULT OnDrilldown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
	{
		ATLASSERT(IsDrilldownColumn(wParam));
		DoDrilldown(DRILLDOWN, m_data->GetColumn(wParam));
		return 0;
	}

	LRESULT OnDrilldown2(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
	{
		ATLASSERT(IsDrilldownColumn(wParam));
		DoDrilldown2(DRILLDOWN, m_data->GetColumn(wParam));
		return 0;
	}

	void OnEditDrilldownSingleSelect(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CComPtr<IRepository> rep = AttachRepository();
		CComPtr<IAttribute> attr;
		if (IDOK == ::DoGetAttr(rep, attr, _T("Select File")))
		{
			DoDrilldownSingle(attr->GetModuleQualifiedLabel(), attr->GetLabel());
		}
	}

	void OnEditDrilldownSelect(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CComPtr<IRepository> rep = AttachRepository();
		CComPtr<IAttribute> attr;
		if (IDOK == ::DoGetAttr(rep, attr, _T("Select File")))
		{
			DoDrilldown(attr->GetModuleQualifiedLabel(), attr->GetLabel());
		}
	}

	void OnEditDrilldown2Select(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CComPtr<IRepository> rep = AttachRepository();
		CComPtr<IAttribute> attr;
		if (IDOK == ::DoGetAttr(rep, attr, _T("Select File")))
		{
			DoDrilldown2(attr->GetModuleQualifiedLabel(), attr->GetLabel());
		}
	}

	LRESULT OnBackgroundSaveComplete(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/)
	{
		m_resultSlot->PostStatus(NULL);
		if ( wParam == BackgroundSaveToExcel )
		{
			ShellExecOpen(m_backgroundSave->GetFilePath());
		}
		m_backgroundSave = 0;
		return 0;
	}

public:
	bool UpdateUI(CCmdUI * cui)
	{
		/* Anything in the BEGIN_UPDATE_UI_MAP must get turned on here
		   or in the parent if we want to use it
		*/
		bool bCanCopy = CanCopy();
		UPDATEUI(cui, ID_EDIT_CUT, bCanCopy);
		UPDATEUI(cui, ID_EDIT_COPYASCSV, bCanCopy);
		UPDATEUI(cui, ID_EDIT_COPYASECL, bCanCopy);
		bool isTextInClipboard=::IsClipboardFormatAvailable(CF_TEXT)!=FALSE;
		bool hasResults = HasResults();
		UPDATEUI(cui, ID_EDIT_PASTE, hasResults && isTextInClipboard);
		UPDATEUI(cui, ID_FILE_SAVE, hasResults);
		UPDATEUI(cui, ID_FILE_SAVE_AS, hasResults);
		UPDATEUI(cui, ID_FILE_SAVE_ASXML, hasResults);
		UPDATEUI(cui, ID_EDIT_GOTO, hasResults);
		UPDATEUI(cui, ID_EDIT_FIND, hasResults);

		UPDATEUI(cui, ID_FORMAT_FONTSTYLE, bCanCopy && hasResults);

		if (cui->m_nID == ID_FORMAT_FONTBOLD)
		{
			cui->Enable(bCanCopy && hasResults);
			cui->SetCheck(IsSelectionBold());
			return true;
		}

		if (cui->m_nID == ID_FORMAT_FONTITALIC)
		{
			cui->Enable(bCanCopy && hasResults);
			cui->SetCheck(IsSelectionItalic());
			return true;
		}

		UPDATEUI(cui, ID_FORMAT_FONTCOLOR, bCanCopy && hasResults);
		UPDATEUI(cui, ID_FORMAT_FILLCOLOR, bCanCopy && hasResults);

		UPDATEUI(cui, ID_FORMAT_FONTCOLOR_SELECT, bCanCopy && hasResults);
		UPDATEUI(cui, ID_FORMAT_FILLCOLOR_SELECT, bCanCopy && hasResults);
		return false;
	}

	virtual void Refresh()
	{
		m_data->SetRowCount(m_result->GetTotal());
		m_list.SetRowCount(m_result->GetTotal());
		m_list.Redraw();
	}

	//Command Handlers -------------------------------------------
	void OnLButtonUp(UINT /*hWnd*/, CPoint pt)
	{
		__int64 hitRow;
		int hitCol = HitTestForColumn(pt, &hitRow);
		if (m_resultSlot)
		{
			m_resultSlot->OnSingleClickResult(hitRow, hitCol);
		}
	}

	void OnContextMenu(HWND /*hWnd*/, CPoint pt)
	{
		WTL::CMenu m;
		m.LoadMenu(IDR_POPUP_RESULTS);

		//Update only the context specific items

		//format the menu text for this one
		CString strCurrent, strNew;
		m.GetMenuString(ID_EDIT_COPYASCSV, strCurrent, MF_BYCOMMAND);
		strNew.FormatMessage(strCurrent, m_CSVType.c_str());
		m.ModifyMenu(ID_EDIT_COPYASCSV, MF_BYCOMMAND, ID_EDIT_COPYASCSV, strNew);

		m.GetMenuString(ID_EDIT_COPYASCSV_WITHHEADERS, strCurrent, MF_BYCOMMAND);
		strNew.FormatMessage(strCurrent, m_CSVType.c_str());
		m.ModifyMenu(ID_EDIT_COPYASCSV_WITHHEADERS, MF_BYCOMMAND, ID_EDIT_COPYASCSV_WITHHEADERS, strNew);

		UINT canCopy = CanCopy() ? MF_ENABLED : MF_GRAYED;
		m.EnableMenuItem(ID_EDIT_COPY, canCopy);
		m.EnableMenuItem(ID_EDIT_COPYASCSV, canCopy);
		m.EnableMenuItem(ID_EDIT_COPYASCSV_WITHHEADERS, canCopy);
		m.EnableMenuItem(ID_EDIT_COPYASECL, canCopy);
		bool isTextInClipboard=::IsClipboardFormatAvailable(CF_TEXT)!=FALSE;
		m.EnableMenuItem(ID_EDIT_PASTE, isTextInClipboard ? MF_ENABLED : MF_GRAYED);

		CString cellData;
		int cellCharPos = -1;
		if ( !HasResults() )
		{
			m_contextColumn = -1;
			m_contextRow = -1;
		}
		else
		{
			__int64 hitRow;
			m_contextColumn = m_list.DisplayToDataColumn(HitTestForColumn(pt,&hitRow));
			m_contextRow = hitRow;
			if ( m_contextColumn >= 0 )
			{
				m.EnableMenuItem(ID_RESULTS_SELECTCOLUMN, MF_ENABLED);
				m.EnableMenuItem(ID_RESULTS_AUTOSIZECOLUMN, MF_ENABLED );
				m.EnableMenuItem(ID_RESULTS_SHOWCOLUMNINHEX, MF_ENABLED );
				if ( IsHexColumn(m_contextColumn) )
					m.CheckMenuItem(ID_RESULTS_SHOWCOLUMNINHEX, MF_CHECKED );
				if ( hitRow >= 0 )
				{
					m.EnableMenuItem(ID_RESULTS_SELECTROW, MF_ENABLED);
					cellData = m_data->GetCell(m_contextRow,m_contextColumn);
					cellCharPos = m_list.HitTestCharInCell(pt,m_contextRow,m_contextColumn);
				}

				bool foundDrilldown = false;
				for(int i = 0; i < m_list.GetColCount(); ++i)
				{
					int col=m_list.DisplayToDataColumn(i);

					if (IsDrilldownColumn(col))
					{
						//CMenuItemInfo mi;
						//mi.fMask = MIIM_ID | MIIM_TYPE;
						//mi.fType = MFT_STRING;
						//mi.dwTypeData = const_cast<LPTSTR>(m_data->GetColumn(i));
						//mi.wID = ID_RESULT_DRILLDOWN_NONE + 100 + i;
						//m.InsertMenuItem(ID_RESULT_DRILLDOWN_NONE, FALSE, &mi);
						InsertMenuItem(m, ID_RESULT_DRILLDOWN_NONE, ID_RESULT_DRILLDOWN_NONE + 100 + col, m_data->GetColumn(col));
						InsertMenuItem(m, ID_RESULT_DRILLDOWNSINGLE_NONE, ID_RESULT_DRILLDOWN_NONE + 200 + col, m_data->GetColumn(col));
						InsertMenuItem(m, ID_RESULT_DRILLDOWN2_NONE, ID_RESULT_DRILLDOWN_NONE + 300 + col, m_data->GetColumn(col));
						foundDrilldown = true;
					}
				}

				if (foundDrilldown)
				{
					{
						WTL::CMenuItemInfo mi;
						mi.fMask = MIIM_TYPE;
						mi.fType = MFT_SEPARATOR;
						m.InsertMenuItem(ID_RESULT_DRILLDOWN_NONE, FALSE, &mi);
					}
					{
						WTL::CMenuItemInfo mi;
						mi.fMask = MIIM_TYPE;
						mi.fType = MFT_SEPARATOR;
						m.InsertMenuItem(ID_RESULT_DRILLDOWNSINGLE_NONE, FALSE, &mi);
					}
					{
						WTL::CMenuItemInfo mi;
						mi.fMask = MIIM_TYPE;
						mi.fType = MFT_SEPARATOR;
						m.InsertMenuItem(ID_RESULT_DRILLDOWN2_NONE, FALSE, &mi);
					}
				}

				{
					WTL::CMenuItemInfo mi;
					mi.fMask = MIIM_ID | MIIM_TYPE;
					mi.fType = MFT_STRING;
					mi.dwTypeData = _T("Select...");
					mi.wID = ID_RESULT_DRILLDOWN_SELECT;
					m.InsertMenuItem(ID_RESULT_DRILLDOWN_NONE, FALSE, &mi);
				}

				{
					WTL::CMenuItemInfo mi;
					mi.fMask = MIIM_ID | MIIM_TYPE;
					mi.fType = MFT_STRING;
					mi.dwTypeData = _T("Select...");
					mi.wID = ID_RESULT_DRILLDOWNSINGLE_SELECT;
					m.InsertMenuItem(ID_RESULT_DRILLDOWNSINGLE_NONE, FALSE, &mi);
				}

				{
					WTL::CMenuItemInfo mi;
					mi.fMask = MIIM_ID | MIIM_TYPE;
					mi.fType = MFT_STRING;
					mi.dwTypeData = _T("Select...");
					mi.wID = ID_RESULT_DRILLDOWN2_SELECT;
					m.InsertMenuItem(ID_RESULT_DRILLDOWN2_NONE, FALSE, &mi);
				}

				m.RemoveMenu(ID_RESULT_DRILLDOWN_NONE, MF_BYCOMMAND);
				m.RemoveMenu(ID_RESULT_DRILLDOWNSINGLE_NONE, MF_BYCOMMAND);
				m.RemoveMenu(ID_RESULT_DRILLDOWN2_NONE, MF_BYCOMMAND);
			}

			WTL::CMenuItemInfo mi;
			mi.fMask = MIIM_ID | MIIM_TYPE;
			mi.fType = MFT_STRING;

			CCellRange range=m_list.GetSelected();
			if(range.IsValid())
			{
				m.EnableMenuItem(ID_RESULT_DRILLDOWNSINGLE_SELECT, MF_ENABLED);
				m.EnableMenuItem(ID_RESULT_DRILLDOWN_SELECT, MF_ENABLED);
				m.EnableMenuItem(ID_RESULT_DRILLDOWN2_SELECT, MF_ENABLED);
				bool fullrowcol=(range.GetColSpan()+1)==m_list.GetColCount();
				if(!fullrowcol)
					fullrowcol=range.GetRowSpan()==m_list.GetRowCount();

				if(fullrowcol)
				{
					m.EnableMenuItem(ID_RESULT_DELETE_SELECTION, MF_ENABLED);
				}
				else
				{
					m.EnableMenuItem(ID_RESULT_DELETE_SELECTION, MF_GRAYED);
				}

			}
			else
			{
				m.EnableMenuItem(ID_RESULT_DRILLDOWNSINGLE_SELECT, MF_GRAYED);
				m.EnableMenuItem(ID_RESULT_DRILLDOWN_SELECT, MF_GRAYED);
				m.EnableMenuItem(ID_RESULT_DRILLDOWN2_SELECT, MF_GRAYED);
				m.EnableMenuItem(ID_RESULT_DELETE_SELECTION, MF_GRAYED);
			}
		}

		CResultContext context(this, m,pt);
		context.m_row = m_contextRow;
		context.m_column = m_contextColumn;
		context.m_cellData = cellData;
		context.m_cellCharPos = cellCharPos;
		if ( !m_resultSlot || !m_resultSlot->OnContextResult(context) )
		{
			//if nobody handled it, then do it ourselves
			BOOL id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pt.x, pt.y, m_hWnd, NULL);
			if (id)
				PostMessage(WM_COMMAND, id);
		}

	}

	LRESULT OnResultsAutoSizeColumn(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		AutoSizeColumn(m_contextColumn);
		return 0;
	}

	LRESULT OnResultsAutoSizeAllColumns(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		AutoSizeColumn(-1);
		return 0;
	}

	LRESULT OnResultsFormatHex(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		ToggleHexColumnDisplay(m_contextColumn);

		return 0;
	}

	void OnEditGoTo(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		int line;
		if ( IDOK == DoModalGoto((int)CurrentLine(), (int)m_list.GetItemCount(), line) )
			GotoLine(line,0);
	}

	void OnEditCut(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoCut();
	}

	void OnEditCopy(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoCopy();
	}

	void OnEditPaste(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		m_list.DoPaste();
	}

	void OnEditCopyAsEcl(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoCopyAsEcl();
	}

	void OnEditCopyToCSV(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoCopyToCSV(false);
	}

	void OnEditCopyToCSVWithHeaders(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CWaitCursor cursor;
		DoCopyToCSV(true);
	}

	CString promptForFile(const TCHAR *szFilter, const TCHAR *szExt)
	{

		CFileDialogEx wndFileDialog( FALSE, szExt, _T(""),
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, m_hWnd );

		if ( IDOK == wndFileDialog.DoModal() ) 
		{
			return wndFileDialog.GetPathName();
		}

		return _T("");
	}

	CString promptForCSVFile()
	{
		static const TCHAR szFilter[] = _T("CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0\0");
		return promptForFile(szFilter, _T(".csv"));
	}

	CString promptForTabFile()
	{
		static const TCHAR szFilter[] = _T("Tab Files (*.tab)\0*.tab\0All Files (*.*)\0*.*\0\0");
		return promptForFile(szFilter, _T(".tab"));
	}

	CString promptForXMLFile()
	{
		static const TCHAR szFilter[] = _T("XML Files (*.xml)\0*.xml\0All Files (*.*)\0*.*\0\0");
		return promptForFile(szFilter, _T(".xml"));
	}

	void OnEditSelectAll(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoSelect(eAll);
	}
	void OnEditSelectColumn(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoSelect(eColumn);
	}
	void OnEditSelectRow(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		DoSelect(eRow);
	}
	/*
	void OnResultsDrilldown(UINT uNotifyCode, int nID, HWND hWnd)
	{
		DoDrilldown(m_contextRow, m_contextColumn);
	}
	*/

	void OnResultDeleteColRow(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)
	{
		CCellRange range=m_list.GetSelected();
		if(range.IsValid())
		{
			if((range.GetColSpan()+1)==m_list.GetColCount()) // entire row(s)?
			{
				for (__int64 row = range.GetMaxRow(); row >= range.GetMinRow(); --row)
				{
					m_list.SetRowCount(m_data->GetRowCount() - 1);
					m_data->RemoveRow(row);
				}
				m_list.Redraw();
			}
			else
			{
				ATLASSERT(range.GetRowSpan()==m_list.GetRowCount()); 
				if(range.GetRowSpan()==m_list.GetRowCount()) // entire column(s)?
					m_list.DeleteColumns(range.GetMinCol(), range.GetColSpan());
			}
		}
	}

	void OnSelectionBkColor(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CMainFrame* ifr=GetIMainFrame();
		m_list.SetSelectionBkColor(ifr->GetFillColor());
	}

	void OnSelectionTextColor(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		CMainFrame* ifr=GetIMainFrame();
		m_list.SetSelectionTextColor(ifr->GetTextColor());
	}

	void OnTextColorSelect(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		COLORREF clr;

		// be nice and initialize color dialog with current color, if any
		CCellID cell=m_list.GetFocusCell();
		ATLASSERT(cell.IsValid());

		int col=cell.col;	
		int row=cell.row;

		col=m_list.DisplayToDataColumn(col);
		CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, col));
		if(cs)
		{
			clr=cs->GetTextColor();
		}
		else
		{
			CMainFrame* ifr=GetIMainFrame();
			clr=ifr->GetTextColor();
		}

		WTL::CColorDialog cdlg(clr);
		if(cdlg.DoModal(*this)==IDOK)
		{
			clr=cdlg.GetColor();
			m_list.SetSelectionTextColor(clr);
		}
	}

	void OnBkColorSelect(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		COLORREF clr;
		CCellID cell=m_list.GetFocusCell();
		ATLASSERT(cell.IsValid());

		int col=cell.col;	
		int row=cell.row;

		col=m_list.DisplayToDataColumn(col);
		CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, col));
		if(cs)
		{
			clr=cs->GetBkColor();
		}
		else
		{
			CMainFrame* ifr=GetIMainFrame();
			clr=ifr->GetFillColor();
		}

		WTL::CColorDialog cdlg(clr);
		if(cdlg.DoModal(*this)==IDOK)
		{
			clr=cdlg.GetColor();
			m_list.SetSelectionBkColor(clr);
		}
	}

	void OnSelectionLogFont(UINT /*uNotifyCode*/, int /*nID*/, HWND /*hWnd*/)
	{
		WTL::CFontDialog fdlg(0, CF_SCREENFONTS | CF_NOSCRIPTSEL);
		LOGFONT lf=m_list.GetSelectedCellFont();
		fdlg.SetLogFont(&lf);
		if(fdlg.DoModal(*this)==IDOK)
		{
			m_list.SetSelectionFont(fdlg.m_lf);
		}
	}

	void OnBoldenSelection(UINT uNotifyCode, int nID, HWND hWnd)
	{
		CMainFrame* ifr = GetIMainFrame();
		m_list.BoldenSelection(!ifr->GetTextBold());	//Needs to be negative as check will not have been updated yet.
	}

	void OnItalicizeSelection(UINT /*uNotifyCode*/, int nID, HWND /*hWnd*/)	
	{
		CMainFrame* ifr=GetIMainFrame();
		m_list.ItalicizeSelection(!ifr->GetTextItalic());	//Needs to be negative as check will not have been updated yet.
	}

	LRESULT OnFileSave(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		if ( GetFilePath().IsEmpty() )
			OnFileSaveAs(0,0,0);
		else
		{
			CResultFormatterAsCSV formatter;
			SaveToFile(formatter);
		}
		return 0;
	}

	LRESULT OnFileSaveAs(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		//CSV is defaut
		CString fileName = promptForCSVFile();
		if ( !fileName.IsEmpty() )
		{
			CWaitCursor cursor;
			SetNamePath(fileName);
			CResultFormatterAsCSV formatter;
			SaveToFile(formatter);
		}

		return 0;
	}

	LRESULT OnFileSaveAsXML(UINT /*wNotifyCode*/, int /*nID*/, HWND /*hWndCtl*/)
	{
		CString fileName = promptForXMLFile();
		if ( !fileName.IsEmpty() )
		{
			CWaitCursor cursor;
			SetNamePath(fileName);
			CResultFormatterAsXML formatter;
			SaveToFile(formatter);
		}

		return 0;
	}

	void DoCut() //AsTabDelimited
	{
		std::_tstring sel;
		CResultFormatterAsTabDelimited formatter;
		CCellRange range = m_list.GetGridSelection();
		m_data->SaveAs(sel, formatter, range, true, false, NULL);
		SetClipboard(sel.c_str());
		m_list.Redraw();
	}

	void DoCopy() //AsTabDelimited
	{
		std::_tstring sel;
		CResultFormatterAsTabDelimited formatter;
		CCellRange range = m_list.GetGridSelection();
		m_data->SaveAs(sel, formatter, range, false, false, &m_list);
		SetClipboard(sel.c_str());
	}

	void DoCopyAsEcl_A(std::_tstring & ecl)
	{
		std::_tstring sel;
		sel.reserve(1024); 

		CResultFormatterAsECL formatter;
		//do one variable per column!
		CCellRange gridSel = m_list.GetGridSelection();
		if (gridSel.IsValid())
		{
			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				std::_tstring col = _T("Set_");;
				col += m_data->GetColumn(c);
				if (col.empty())
					continue;	
				col += _T(":=[");
				bool bFirstValue = true;
				for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
				{
					formatter.FormatEx(m_data->GetCell(r, c), col, bFirstValue, m_data->IsColQuoted(c));
					bFirstValue = false;
				}
				formatter.FormatEndOfLine(col);
				sel += col;
			}
		}
		ecl += sel;
	}

	void DoCopyAsEcl_B(std::_tstring & ecl)
	{
		std::_tstring sel;
		sel.reserve(1024); 
		std::_tstring row;
		row.reserve(1024);

		CResultFormatterAsECL formatter;
		//do one variable per column!
		CCellRange gridSel = m_list.GetGridSelection();
		if (gridSel.IsValid())
		{
			for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); ++r)
			{
				row = (boost::_tformat(_T("Row_%1%")) % m_data->GetCell(r, 0)).str().c_str();
				if (row.empty())
					continue;	
				row += _T(":=[");
				bool bFirstValue = true;
				for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); ++c)
				{
					formatter.FormatEx(m_data->GetCell(r, c), row, bFirstValue, true);//Rows must always be strings...
					bFirstValue = false;
				}
				formatter.FormatEndOfLine(row);
				sel += row;
			}
		}
		ecl += sel;
	}

	void DoCopyAsEcl_C(std::_tstring & ecl, bool drilldownFlag)
	{
		static const TCHAR * const RecordTPL = _T("%1%_Record :=\r\n  record\r\n%2%  end;\r\n");
		static const TCHAR * const FieldTPL = _T("    %1% %2%;\r\n");
		static const TCHAR * const DatasetTPL = _T("%1%_Dataset :=\r\n  dataset([\r\n%2%], %1%_Record);\r\n");
		static const TCHAR * const RowTPL = _T("    {%1%}");
		CCellRange gridSel = m_list.GetGridSelection();
		if (gridSel.IsValid())
		{
			std::_tstring fields;
			for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
			{
				std::_tstring type;
				fields += (boost::_tformat(FieldTPL) % m_data->GetColumnTypeAsECL(c, type) % m_data->GetColumn(c)).str();
			}
			ecl += (boost::_tformat(RecordTPL) % (drilldownFlag ? DRILLDOWN : COPY) % fields).str();

			std::_tstring dataset;
			for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); ++r)
			{
				std::_tstring row;
				for(int c = gridSel.GetMinCol(); c <= gridSel.GetMaxCol(); c++)
				{
					std::_tstring cell;
					row += m_data->GetCellQuoted(r, c, cell);
					if (c < gridSel.GetMaxCol())
					{
						row += _T(", ");
					}
				}
				dataset += (boost::_tformat(RowTPL) % row).str();
				if (r < gridSel.GetMaxRow())
				{
					dataset += _T(", \r\n");
				}
			}
			ecl += (boost::_tformat(DatasetTPL) % (drilldownFlag ? DRILLDOWN : COPY) % dataset).str();
		}
	}

	const TCHAR * GetCopyAsEcl(bool drilldownFlag, std::_tstring & ecl)
	{
		DoCopyAsEcl_A(ecl);
		DoCopyAsEcl_B(ecl);
		DoCopyAsEcl_C(ecl, drilldownFlag);
		return ecl.c_str();
	}

	void DoCopyAsEcl()
	{
		std::_tstring ecl;
		GetCopyAsEcl(false, ecl);
		SetClipboard(ecl.c_str());
	}

	void SaveSelectionToFile(CUnicodeFile &file, bool withHeaders)
	{
		CResultFormatterAsCSV formatter;
		CCellRange range = m_list.GetGridSelection();
		m_data->SaveAs(file, formatter, range, false, withHeaders);
	}

	void DoCopyToCSV(bool withHeaders)
	{
		CAtlTemporaryFile temp;
		temp.Create();
		CString fileName = temp.TempFileName() ;
		temp.HandsOff();
		if ( !fileName.IsEmpty() )
		{
			fileName += ".csv";

			//if the selection spans the entire data set then
			//we may want to background save
			CCellRange gridSel = m_list.GetGridSelection();
			if (gridSel.GetRowSpan() == m_list.GetRowCount())
			{
				CResultFormatterAsCSV formatter;
				if (BackgroundSave(formatter, fileName, BackgroundSaveToExcel, true, withHeaders) )
				{
					return;
				}
			}

			CWaitCursor cursor;
			CString msg;
			msg.FormatMessage(_T("Sending to %1"), m_CSVType.c_str());
			m_resultSlot->PostStatus(msg);
			CUnicodeFile file;
			if (file.Create(fileName, 
				GENERIC_WRITE, CREATE_ALWAYS,
				CUnicodeFile::ENCODING_UNKNOWN))
			{
				SaveSelectionToFile(file, withHeaders);
				file.Close();
				::ShellExecOpen(fileName);
			}
			m_resultSlot->PostStatus(NULL);
		}
	}

	void DoSelect(ResultCopyObject eObj)
	{
		switch ( eObj )
		{
		case eAll:
			m_list.SelectAll();
			break;
		case eColumn:
			m_list.SelectColumn();
			break;
		case eRow:
			m_list.SelectRow();
			break;
		}
	}

	bool GetRowsWithSelection(CGridSelectionCollection::SelectionRows &rows_result)
	{
		CCellRange gridSel = m_list.GetGridSelection();
		if (gridSel.IsValid())
		{
			for(__int64 r = gridSel.GetMinRow(); r <= gridSel.GetMaxRow(); r++)
			{
				rows_result.push_back(r);
			}
			return true;
		}
		return false;
	}

	bool DoDrilldownSingle(const CString & module, CString header)
	{
		CGridSelectionCollection::SelectionRows rows;
		if (GetRowsWithSelection(rows))
		{
			if (header.Find(_T("__")) == 0)
				header = header.Mid(2, header.GetLength() - 2);

			std::_tstring xml;
			for(unsigned r = 0; r < rows.size(); r++)
			{
				std::_tstring row_xml;
				m_data->GetXML(rows[r], _T(""), _T("row"), row_xml, true);
				xml += row_xml;
			}
			xml = _T("<root>") + xml + _T("</root>");
			std::_tstring ecl_xml;
			CString ecl;
			ecl.Format(DRILLDOWN_ECL, module, header, ::EncodeForEcl(xml.c_str(), ecl_xml));
			m_resultSlot->Drilldown(ecl);
			return true;
		}
		return false;
	}

	void DoDrilldown(__int64 row, const CString & module, const CString & header, CString & ecl_result)
	{
		std::_tstring xml;
		m_data->GetXML(row, _T("root"), _T(""), xml, true);

		std::_tstring ecl_xml;
		CString ecl;
		ecl.Format(DRILLDOWN_ECL, module, header, ::EncodeForEcl(xml.c_str(), ecl_xml));
		ecl_result += ecl + _T("\r\n");
	}

	bool DoDrilldown(CString module, CString header)
	{
		CGridSelectionCollection::SelectionRows rows;
		if (GetRowsWithSelection(rows))
		{
			if (header.Find(_T("__")) == 0)
				header = header.Mid(2, header.GetLength() - 2);

			CString ecl;
			for(unsigned r = 0; r < rows.size(); r++)
			{
				DoDrilldown(rows[r], module, header, ecl);
			}
			m_resultSlot->Drilldown(ecl);
			return true;
		}
		return false;
	}

	bool DoDrilldown2(CString module, CString header)
	{
		std::_tstring ecl;
		GetCopyAsEcl(true, ecl);
		ecl += (boost::_tformat(DRILLDOWN_ECL2) % static_cast<const TCHAR *>(module) % static_cast<const TCHAR *>(header)).str();
		m_resultSlot->Drilldown(ecl.c_str());
		return true;
	}

	bool CanCopy()
	{
		return m_list.HasSelection();
	}

	bool CanSelect()
	{
		return (m_list.GetItemCount() > 0);
	}

	bool HasResults() const
	{
		return m_data->GetRowCount() > 0;
	}

	bool IsSelectionBold() const
	{
		CCellID cell=m_list.GetFocusCell();
		if(!cell.IsValid())
			return false;

		int col=cell.col;	
		int row=cell.row;

		col=m_list.DisplayToDataColumn(col);
		CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, col)); 
		if (cs && cs->IsBold())
			return true;
		return false;
	}

	bool IsSelectionItalic() const
	{
		CCellID cell=m_list.GetFocusCell();
		if(!cell.IsValid())
			return false;

		int col=cell.col;	
		int row=cell.row;

		col=m_list.DisplayToDataColumn(col);
		CCellState* cs=(CCellState*)(m_data->GetCellUnknown(row, col)); 
		if (cs && cs->IsItalic())
			return true;
		return false;
	}

	void GetResults(__int64 &total, __int64 &loaded)
	{
		total = m_data->GetRowCount();
		loaded = m_data->GetRowsLoaded();
	}

	void GotoLine(__int64 line, __int64 col)
	{
		m_list.GotoLine(line,col);
	}

	__int64 CurrentLine()
	{ 
		__int64 line = m_list.CurrentLine();
		return line <= 0 ? 1 : line + 1;  //convert to one based
	}

	static void SaveInBackground(StlLinked<CResultDownload> pDownLoader)
	{
		pDownLoader->Save();
	}

	bool BackgroundSaving()
	{
		return m_backgroundSave.get() != NULL;
	}

	bool BackgroundSave(const CResultFormatter &formatter, const TCHAR* fileName=0, WPARAM postFlag=BackgoundSaveToFile, bool bUseSelection=false, bool withHeaders=false)
	{
		if ( BackgroundSaving() )
		{
			MessageBox(_T("Background save still in progress."), m_wu->GetWuid(), MB_OK | MB_ICONINFORMATION);
			return true; //done (sort of)
		}

		if ( !fileName )
			fileName = m_path;

		ATLASSERT(fileName && fileName[0]); //must have set file name first

		__int64 total = m_data->GetRowCount();
		__int64 loaded = m_data->GetRowsLoaded();
		if ( total > loaded )
		{
			CString msg;
			msg.Format(_T("%I64d of %I64d rows loaded"), loaded, total );
			m_resultSlot->PostStatus(msg);
			msg.Format(_T("Not all results have been downloaded. Results will be downloaded and saved in the background."));
			if ( IDOK == MessageBox(msg, m_wu->GetWuid(), MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1))
			{
				m_backgroundSave = new CResultDownload(m_resultSlot, withHeaders);
				if (bUseSelection)
				{
					CCellRange cellRange = m_list.GetGridSelection();
					m_backgroundSave->Init(m_hWnd, fileName, formatter, m_data,	&cellRange, postFlag);
				}
				else
				{
					m_backgroundSave->Init(m_hWnd, fileName, formatter, m_data,	(CCellRange *)NULL, postFlag);
				}
				m_resultSlot->PostStatus(_T("Saving in Background"));
				clib::thread run(__FUNCTION__, boost::bind(&SaveInBackground, m_backgroundSave.get()));
				return true; //done
			}
			else
			{
				m_resultSlot->PostStatus(NULL); //clear the status
			}
		}

		return false; //didn't do a background save

	}

	void SaveToFile(const CResultFormatter &formatter)
	{
		ATLASSERT(m_path.GetLength()); //must have set file name first

		if ( !BackgroundSave(formatter) )
		{
			CUnicodeFile file;
			CUnicodeFile::ENCODING encoding = (CUnicodeFile::ENCODING)formatter.FileEncoding();
			if (file.Create(m_path,	GENERIC_WRITE, CREATE_ALWAYS, encoding))
			{
				m_data->SaveAs(file, formatter, 0, false);
				file.Close();
			}
		}
	}

	//CTabPane
	virtual HWND GetHWND()
	{
		return m_hWnd;
	}

	void operator()(__int64 start, int count, __int64 total, bool success)
	{
		//this is only called once to get columns created
		ATLTRACE(_T("Cache Results %I64d - %d, %I64d\n"), start, count, total);
		if (!m_list.IsWindow())
			return;
		//this is only called once to get columns created
		m_data->SetRowCount(total);
		m_data->SetLoaded(start, count);
		int cols = m_data->GetColCount();
		m_list.SetColumnCount(cols);

		//this will be in worker thread, call result slot in window thread
		PostMessage(UM_CONFIGUREGRID); //m_resultSlot->ConfigureGrid(m_list);

		if ( total == 0 )
		{
			m_list.SetRowCount(1); //one row for our warning of no data
		}
		else
		{
			m_list.SetRowCount(static_cast<int>(total));
		}
		//m_list.EnsureVisible(0, false);
		//m_list.EnsureColumnVisible(0, false);

		//let the list handle the data
		m_list.DataLoaded(start, count, total);

		if ( m_data->GetHasChildDataset() )
		{
			PostMessage(UM_SHOWDATASETURL);
		}
		m_resultSlot->PostResults(start, count, total);
	}

	int HitTestForColumn(const POINT &pntScreen, __int64 *pRowHit=0)
	{
		POINT pointList = pntScreen;
		::MapWindowPoints(0, m_list.m_hWnd, &pointList, 1 );
		int col = -1;
		__int64 row = m_list.HitTestForColumn(pointList, &col);
		if ( pRowHit )
			*pRowHit = row;
		return col;
	}

	bool IsHexColumn(int col)
	{
		if ( col >= (int)m_hexColumns.size() )
			m_hexColumns.resize(col+1);
		return m_hexColumns[col];
	}

	bool IsDrilldownColumn(int col)
	{
		CString colHeader = m_data->GetColumn(col);
		if (colHeader.Find(_T("__")) == 0)
			colHeader = colHeader.Mid(2, colHeader.GetLength() - 2);
		for (IAttributeVector::iterator itr = m_drilldown->begin(); itr != m_drilldown->end(); ++itr)
		{
			if (colHeader.CompareNoCase(itr->get()->GetLabel()) == 0)
				return true;
		}
		return false;
	}

	bool ToggleHexColumnDisplay(int column)
	{
		column=m_list.DisplayToDataColumn(column);
		if ( column >= (int)m_hexColumns.size() )
			m_hexColumns.resize(column+1);
		bool bShow = m_hexColumns[column] = !m_hexColumns[column];

		m_list.SetColumnFormatter(column, (bShow ? new CResultFormatterAsHex() : 0) );
		m_list.Redraw();
		return bShow;
	}

	//ISearchable
	bool CanBeSearched() const
	{
		return m_data->CanBeSearched();
	}

	bool Search(const CSearchableInfo &info, CSearchableResult &result) const
	{
		return m_data->Search(info,result);
	}

	//  --  IResultView  ---
	HWND GetHwnd() const 
	{
		return m_hWnd;
	}

	//  --  IResultGridView  ---
	void RenameColumn(unsigned int col, const std::_tstring & label)
	{
		m_data->SetColumnLabel(col + 1, label);
		m_list.Redraw();
	}

	void SplitColumn(unsigned int col, const SplitVector & positions)
	{
		int cols = m_data->SplitColumn(col + 1, positions);
		m_list.SetColumnCount(cols);
		m_list.Redraw();
	}
};

typedef StlLinked<CResultView> CResultViewPtr;
typedef std::vector<CResultViewPtr> CResultViewPtrVector;
//  ======================================================================================================================================================
class CMultiResultView :
	public WTL::CDialogImpl<CMultiResultView>,
	public WTL::CDialogResize<CMultiResultView>,
	public CEclExec,
	public IResultViewer
{
	typedef CMultiResultView thisClass;
	typedef WTL::CDialogImpl<CMultiResultView> baseClass;
	typedef WTL::CDialogResize<CMultiResultView> baseResizeClass;

private:
	mutable CTabbedChildWindowEx<CColDotNetButtonTabCtrl<CTabViewTabItem> > m_tabbedChildWindow;  //mutable due to lack of const in 3rd party tab control.

	Dali::IWorkunitAdapt m_wu;
	boost::signals::connection m_wuConn;
	CTabPanePtrVector m_tabs;
	IResultSlot		*m_resultSlot;
	CTabPanePtr		 m_exceptionView;
	CTabPanePtr		 m_debugView;
	CTabPanePtr		 m_summaryView;
#ifdef USE_LEGACY_GRAPH
	StlLinked<CGraphView> m_graphView;
#else
	StlLinked<CGraphView3> m_graphView;
#endif
	StlLinked<CGraphView2> m_eclGraphView;
	CTabPanePtr		 m_firstResult;
	std::map<std::_tstring, bool> m_loaded;
	BuilderStartup	 m_startupMode;
	//bool			 m_showEclWatch;
	//std::_tstring	 m_showResultTab;
	//bool			 m_showGraph;
	bool			 m_bDeleteWU;
	IAttributeVector m_drilldown;
	bool m_prepareForDisplay;
	bool m_launchDebugger;

	bool CreateSummaryWindow();
	bool CreateGraphWindow();
	bool CreateEclGraphWindow();
	bool CreateResultWindow(Dali::IResult * result);
	bool CreateExceptionWindow();
	bool CreateDebugWindow();

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);
	LRESULT OnRefresh(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
	LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/);
	LRESULT OnTabSelected(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnShowEclWatch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/);
public:
	BEGIN_CUNKNOWN
	END_CUNKNOWN(CUnknown)

	CMultiResultView(IResultSlot *resultSlot);
	CMultiResultView(IResultSlot *resultSlot, Dali::IWorkunit *wu, BuilderStartup startupMode);
	CMultiResultView(IResultSlot *resultSlot, const CString & wuid);
	~CMultiResultView();

	enum { IDD = IDD_MULTIRESULTVIEW };

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return baseClass::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(thisClass)
		MSG_WM_INITDIALOG(OnInitDialog)
		MESSAGE_HANDLER_EX(CWM_REFRESH, OnRefresh)
		MESSAGE_HANDLER_EX(UM_SHOWECLWATCH, OnShowEclWatch)
		MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		NOTIFY_CODE_HANDLER(CTCN_SELCHANGE, OnTabSelected)

		CHAIN_MSG_MAP(baseResizeClass)
		CHAIN_COMMANDS_MEMBER(m_tabbedChildWindow)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(thisClass)
		DLGRESIZE_CONTROL(IDC_STATIC_PLACEHOLDER, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_DDX_MAP(thisClass)
	END_DDX_MAP()

	__int64 GetItemCount();
	void GotoLine(__int64 line, __int64 col);
	void GetResults(__int64 &total, __int64 &loaded);
	bool HasResults() const;
	bool CanCopy();
	bool CanSelect();

	void SaveSelectionToFile(CUnicodeFile &file);
	void SetNamePath(const CString & sPathName);
	CString GetFilePath();
	void SaveToFile(const CResultFormatter &formatter);
	CString GetTitle();
	bool IsDirty();

	void Clear();

	//IResultViewer
	HWND Create(HWND hWndParent);
	HWND GetHwnd() { return m_hWnd; };
	void AutoSizeColumn(int column);
	bool ToggleHexColumnDisplay(int column);
	void RenameColumn(unsigned col, const std::_tstring& newname);

	const TCHAR *GetID(CString & result);
	void SetWorkunit(const CString & wuid);
	void SetWorkunit(Dali::IWorkunit *wu);
	void PrepareForDisplay(bool bRefresh=true);
	bool UpdateUI(CCmdUI * cui);
	void CloseTab(unsigned int tab);
	int GetTabCount();
	void CloseAllTabs();
	void DestroyWindow();
	void ExecEcl(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *eclSource, const TCHAR *eclPath, const TCHAR *scheduled, const TCHAR *label, int resultLimit, const TCHAR *debugString, bool archive, int maxRuntime, bool debug);
	void SetDeleteWorkunit(bool bDelete=true);
	void PostSelectRibbon();

	//CExecEcl
	virtual void WorkunitCreated(Dali::IWorkunit * src);
	virtual void WorkunitUpdated(Dali::IWorkunit * src);
	virtual void WorkunitDeleted(Dali::IWorkunit * src);

	//ISearchable
	bool CanBeSearched() const;
	bool Search(const CSearchableInfo &info, CSearchableResult &result) const;
};

CMultiResultView::CMultiResultView(IResultSlot *resultSlot)
{
	m_resultSlot = resultSlot;
	m_startupMode = StartFirstResult;
	m_bDeleteWU = false;
	m_prepareForDisplay = false;
	m_launchDebugger = false;
}

CMultiResultView::CMultiResultView(IResultSlot *resultSlot, Dali::IWorkunit *wu, BuilderStartup startupMode)
{
	m_resultSlot = resultSlot;
	m_startupMode = startupMode;
	m_bDeleteWU = false;
	m_prepareForDisplay = false;
	m_launchDebugger = false;
	SetWorkunit(wu);
}

CMultiResultView::CMultiResultView(IResultSlot *resultSlot, const CString & wuid)
{
	m_resultSlot = resultSlot;
	m_startupMode = StartUnknown;
	m_bDeleteWU = false;
	m_prepareForDisplay = false;
	m_launchDebugger = false;
	SetWorkunit(wuid);
}

CMultiResultView::~CMultiResultView()
{
	if (m_wu.get() && m_bDeleteWU)
	{
		m_wuConn.disconnect();
		Dali::IWorkunitVector workunits;
		workunits.push_back(m_wu);
		m_wu = NULL;

		StlLinked<Dali::IDali> server = Dali::AttachDali();
		ATLASSERT( server.isLinked() );
		server->DeleteWorkunits(&workunits);
	}
}

HWND CMultiResultView::Create(HWND hWndParent)
{
	return baseClass::Create(hWndParent, rcDefault);
}

void CMultiResultView::DestroyWindow()
{
	baseClass::DestroyWindow();
}

void CMultiResultView::CloseTab(unsigned int tab)
{
	if (tab < 0 || tab > m_tabs.size())
		return;

	HWND hWnd = m_tabs[tab].get()->GetHWND();
	if (m_exceptionView.get()->GetHWND() == hWnd)
		m_exceptionView = NULL;
	else if (m_debugView.get()->GetHWND() == hWnd)
		m_debugView = NULL;
	else if (m_summaryView.get()->GetHWND() == hWnd)
		m_summaryView = NULL;
	else if (m_graphView.get()->GetHWND() == hWnd)
		m_graphView = NULL;

	m_loaded[static_cast<const TCHAR *>(m_tabs[tab].get()->GetTabName())] = false;
	::DestroyWindow(hWnd);
	m_tabbedChildWindow.GetTabCtrl().DeleteItem(tab);
}

int CMultiResultView::GetTabCount()
{
	return m_tabs.size();
}

void CMultiResultView::CloseAllTabs()
{
	for(CTabPanePtrVector::iterator itr = m_tabs.begin(); itr != m_tabs.end(); ++itr)
	{
		::DestroyWindow(itr->get()->GetHWND());
	}
	m_tabbedChildWindow.GetTabCtrl().DeleteAllItems(true);
	m_exceptionView = NULL;
	m_debugView = NULL;
	m_summaryView = NULL;
	m_graphView = NULL;
	m_loaded.clear();
	m_drilldown.clear();
	m_tabs.clear();
}

void CMultiResultView::AutoSizeColumn(int column)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->AutoSizeColumn(column);
}

bool CMultiResultView::ToggleHexColumnDisplay(int column)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->ToggleHexColumnDisplay(column);
	return false;
}

void CMultiResultView::RenameColumn(unsigned col, const std::_tstring& newname)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->RenameColumn(col, newname);
}

void CMultiResultView::ExecEcl(const TCHAR *clusterName, const TCHAR *queueName, Dali::WUAction action, const TCHAR *eclSource, const TCHAR *eclPath, const TCHAR *scheduled, const TCHAR *label, int resultLimit, const TCHAR * debugString, bool archive, int maxRuntime, bool debug)
{
	m_launchDebugger = debug;
	CEclExec::ExecEcl(clusterName, queueName, action, eclSource, eclPath, scheduled, label, resultLimit, debugString, archive, maxRuntime, debug);
}

void CMultiResultView::SetDeleteWorkunit(bool bDelete)
{
	m_bDeleteWU = bDelete;
}

void CMultiResultView::PostSelectRibbon()
{
	if (m_prepareForDisplay)
		return;

	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
	{
		if (dynamic_cast<CExceptionView *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_HOME);
		else if (dynamic_cast<CGraphView *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_GRAPH);
		else if (dynamic_cast<CGraphView2 *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_GRAPH2);
		else if (dynamic_cast<CGraphView3 *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_GRAPH);
		else if (dynamic_cast<CSummaryView *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_BROWSER);
		else if (dynamic_cast<CResultView *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_RESULT);
		else if (dynamic_cast<CDebugView *>(m_tabs[n].get()))
			GetIMainFrame()->PostSelectRibbon(RIBBON_DEBUG);
	}
}

void CMultiResultView::Clear()
{
	CloseAllTabs();
}

LRESULT CMultiResultView::OnTabSelected(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	CWaitCursor wait;
	bHandled = true;
	NMCTC2ITEMS *pItems = (NMCTC2ITEMS *)pnmh;
	if ( pItems->iItem1 != pItems->iItem2)
	{
		unsigned n = pItems->iItem2;
		if (n >= 0 && n < m_tabs.size())
			::SendMessage(m_tabs[n]->GetHWND(), CWM_REFRESH, 0, 0);
	}
	if (IsWindowVisible())
		PostSelectRibbon();
	return 0;
}

LRESULT CMultiResultView::OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMSG pMsg = (LPMSG)lParam;
	if ( /*pMsg->message == WM_COMMAND ||*/ pMsg->message == WM_CONTEXTMENU )
	{
		//push message down
		unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
		if (n >= 0 && n < m_tabs.size())
			::SendMessage(m_tabs[n]->GetHWND(), pMsg->message, pMsg->wParam, pMsg->lParam);
		bHandled = TRUE;
		return 1;
	}
	else
	{
		return m_tabbedChildWindow.PreTranslateMessage(pMsg);
		//ATLTRACE(_T("CMultiResultView::OnForwardMsg %0x\r\n"), pMsg->message );
		//m_tabbedChildWindow.OnForwardMsg(uMsg, wParam, lParam, bHandled);
	}

	return 0;
}

LRESULT CMultiResultView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
	SetMsgHandled(false);
	DlgResize_Init(false, true, 0);

	DoDataExchange();

	if (m_wu)
	{
		m_wuConn = m_wu->on_refresh_connect(boost::ref(*this));
		m_wu->MonitorState();
	}
	return 0;
}

void CMultiResultView::PrepareForDisplay(bool bRefresh)
{
	if ( !m_tabbedChildWindow.IsWindow() )
	{
		m_prepareForDisplay = true;
		CWindow wndPlaceholder = GetDlgItem(IDC_STATIC_PLACEHOLDER);
		CRect rc(0, 0, 0, 0);
		wndPlaceholder.GetWindowRect(rc);
		ScreenToClient(rc);
		wndPlaceholder.DestroyWindow();

		m_tabbedChildWindow.SetReflectNotifications(true);
		m_tabbedChildWindow.SetTabStyles(CTCS_BOTTOM | CTCS_TOOLTIPS | CTCS_SCROLL);
		m_tabbedChildWindow.Create(m_hWnd, rc, NULL, NULL, NULL, IDC_STATIC_PLACEHOLDER);
		if ( m_wu && bRefresh )
			OnRefresh(0,0,0);
		m_prepareForDisplay = false;
		PostSelectRibbon();
	}
}

bool CMultiResultView::CreateSummaryWindow()
{
	if (m_wu && !m_summaryView.isLinked())
	{
		StlLinked<CSummaryView> r = new CSummaryView(m_wu, m_resultSlot);
		{
			m_summaryView = r.get();
			r->Create(m_tabbedChildWindow);
			CTabPanePtr tab = r;
			m_tabs.push_back(tab);

			m_tabbedChildWindow.DisplayTab(r.get()->m_hWnd, TRUE);
		}
		return true;
	}
	else if (m_summaryView.isLinked())
	{
		//if this is the current tab, refresh it
		unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
		if (n+1 == m_tabs.size())
			::SendMessage(m_summaryView->GetHWND(), CWM_REFRESH, 0, 0);
	}
	return false;
}

bool CMultiResultView::CreateGraphWindow()
{
	if (m_wu && !m_graphView.isLinked())
	{
#ifdef USE_LEGACY_GRAPH
		StlLinked<CGraphView> r = new CGraphView(m_wu, m_resultSlot);
#else
		StlLinked<CGraphView3> r = new CGraphView3(m_wu, m_resultSlot);
#endif
		{
			m_graphView = r.get();
			r->Create(m_tabbedChildWindow);
			CTabPanePtr tab = r;
			m_tabs.push_back(tab);

			m_tabbedChildWindow.DisplayTab(r.get()->m_hWnd, TRUE);
		}
		return true;
	}
	else if (m_graphView.isLinked())
	{
		//if this is the current tab, refresh it
		unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
		if (n+1 == m_tabs.size())
			::SendMessage(m_graphView->GetHWND(), CWM_REFRESH, 0, 0);
	}
	return false;
}

bool CMultiResultView::CreateEclGraphWindow()
{
	if (m_wu && !m_eclGraphView.isLinked())
	{
		StlLinked<CGraphView2> r = new CGraphView2(m_wu, m_resultSlot);
		{
			m_eclGraphView = r.get();
			r->Create(m_tabbedChildWindow);
			CTabPanePtr tab = r;
			m_tabs.push_back(tab);

			m_tabbedChildWindow.DisplayTab(r->m_hWnd, TRUE);
		}
		return true;
	}
	else if (m_eclGraphView.isLinked())
	{
		//if this is the current tab, refresh it
		unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
		if (n+1 == m_tabs.size())
			::SendMessage(m_eclGraphView->GetHWND(), CWM_REFRESH, 0, 0);
	}
	return false;
}

bool CMultiResultView::CreateExceptionWindow()
{
	bool bNewWindow = false;
	if ( m_exceptionView.get() == 0 )
	{
		StlLinked<CExceptionView> r = new CExceptionView(m_wu, m_resultSlot);
		m_exceptionView = r.get();
		r->Create(m_tabbedChildWindow);

		//add as first one
		m_tabs.insert(m_tabs.begin(), r.get());

		m_tabbedChildWindow.AddTab(r->m_hWnd, r->GetTabName(), -1, 0);
		m_tabbedChildWindow.DisplayTab(r->m_hWnd);
		bNewWindow = true;
	}
	else
	{
		m_exceptionView->Refresh();
	}
	return bNewWindow;
}

bool CMultiResultView::CreateDebugWindow()
{
	bool bNewWindow = false;
	if ( m_debugView.get() == 0 )
	{
		StlLinked<CDebugView> r = new CDebugView(m_wu, m_resultSlot);
		m_debugView = r.get();
		r->Create(m_tabbedChildWindow);

		//add as first one
		m_tabs.insert(m_tabs.begin(), r.get());

		m_tabbedChildWindow.AddTab(r->m_hWnd, r->GetTabName(), -1, 0);
		m_tabbedChildWindow.DisplayTab(r->m_hWnd);
		bNewWindow = true;
	}
	else
	{
		m_debugView->Refresh();
	}
	return bNewWindow;
}

bool CMultiResultView::CreateResultWindow(Dali::IResult * result)
{
	if (m_loaded[result->GetID()] == true)	
		return false;

	m_loaded[result->GetID()] = true;

	StlLinked<CResultView> r = new CResultView(m_wu, m_resultSlot, result, &m_drilldown);
	if (m_firstResult.get() == NULL)
		m_firstResult = r.get();
	r->Create(m_tabbedChildWindow);

	//there will always? be a summary window, add these right before that
	int pos = m_tabs.size();

	m_tabs.push_back(r.get());

	CString tip;
	m_tabbedChildWindow.AddTab(r->m_hWnd, r->GetTabName(), -1, pos);
	m_tabbedChildWindow.UpdateTabToolTip(r->m_hWnd, r->GetTabToolTip(tip));
	m_tabbedChildWindow.DisplayTab(r->m_hWnd);

	return true;
}

LRESULT CMultiResultView::OnRefresh(UINT /*uMsg*/, WPARAM bCreated, LPARAM bDeleted)
{
	ATLTRACE(_T("CMultiResultView::OnRefresh(%s - %s)\n"), m_wu->GetWuid(), m_wu->GetStateLabel());

	if (bDeleted)
	{
		m_resultSlot->WorkunitDeleted(m_wu);
	}
	else
	{
		if (m_tabbedChildWindow.IsWindow())
		{
			if (m_wu->GetExceptionCount())
				CreateExceptionWindow();

			if (!(boost::algorithm::starts_with(m_wu->GetWuid(), _T("L")) && m_wu->GetState() != Dali::WUStateCompleted))
				CreateSummaryWindow();

			{
				// Big Hack to trick graphs to load (it just worked when graphs came after results window)
				CComPtr<clib::scoped_lock_ref_counted> lock;
				m_wu->Lock(lock);
				Dali::IResultVector::const_iterator itr, end;
				boost::tie(itr, end) = m_wu->GetResultIterator();
			}

			if (m_wu->GetGraphCount())
			{
				CreateGraphWindow();
			}

			if (m_wu->IsComplete())
			{
				int curSel = m_tabbedChildWindow.GetTabCtrl().GetCurSel();

				if (m_drilldown.size() == 0)
				{
					CComPtr<IRepository> rep = AttachRepository();
					rep->GetAttributes(DRILLDOWN, m_drilldown);
				}

				CComPtr<clib::scoped_lock_ref_counted> lock;
				m_wu->Lock(lock);
				Dali::IResultVector::const_iterator itr, end;
				boost::tie(itr, end) = m_wu->GetResultIterator();
				for (; itr != end; ++itr)
					CreateResultWindow(itr->get());

				if (m_wu->GetResultCount() == 2)
				{
					int graphResultCount = 0;
					boost::tie(itr, end) = m_wu->GetResultIterator();
					for (; itr != end; ++itr)
					{
						CComPtr<ITable> result = CreateISparseTable();
						itr->get()->GetResultData(0, 1, result);
						if (result->FindColumn(_T("vertex_id")) != -1)
							++graphResultCount;
						else if (result->FindColumn(_T("edge_from_id")) != -1)
							++graphResultCount;
					}

					if (graphResultCount == 2)
						CreateEclGraphWindow();
				}
				m_tabbedChildWindow.GetTabCtrl().SetCurSel(curSel);
			}

			if (m_wu->IsDebugging())
			{
				CreateDebugWindow();
				if (m_launchDebugger)
				{
#define NLAUNCH_DEBUGGING
#ifdef LAUNCH_DEBUGGING
					boost::filesystem::path folder;
					GetProgramFolder(folder);
					boost::filesystem::path file = folder / boost::filesystem::path("RoxieDebugger.exe", boost::filesystem::native);

					::CString cfg = GetIConfig(QUERYBUILDER_CFG)->GetLabel();
					::CString id = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER);
					::CString pw = GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD);
					std::_tstring params = (boost::_tformat(_T("--cfg %1% --id %2% --pw %3% --wuid %4%")) % (const TCHAR *)cfg % (const TCHAR *)id % (const TCHAR *)pw % (const TCHAR *)wuid).str();
					::ShellExecute(m_hWnd, _T("open"), CA2T(file.native_file_string().c_str()), params.c_str(), CA2T(folder.native_file_string().c_str()), SW_SHOWNORMAL);
#endif
					m_launchDebugger = false;
				}
			}
			switch(m_startupMode)
			{
			case StartNoChange:
				m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			case StartEcl:
				m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			case StartExceptions:
				if (m_exceptionView)
					m_tabbedChildWindow.DisplayTab(m_exceptionView->GetHWND(), FALSE);
				else
					m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			case StartEclWatch:
				if (m_summaryView)
					m_tabbedChildWindow.DisplayTab(m_summaryView->GetHWND(), FALSE);
				else
					m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			case StartGraph:
				if (m_graphView)
				{
					m_tabbedChildWindow.DisplayTab(m_graphView->GetHWND(), FALSE);
					m_graphView->PostMessage(WM_COMMAND, ID_GRAPH_FOLLOWACTIVE);
					m_graphView->PostMessage(WM_COMMAND, ID_GRAPH_MINIMIZEINACTIVE);
				}
				else
					m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			case StartFirstResult:
				if (m_firstResult)
					m_tabbedChildWindow.DisplayTab(m_firstResult->GetHWND(), FALSE);
				else
					m_tabbedChildWindow.GetTabCtrl().SetCurSel(0);
				break;
			}
			m_startupMode = StartUnknown;
		}

		m_resultSlot->WorkunitUpdated(m_wu, (bCreated != 0));
	}

	return 0;
}

void CMultiResultView::SetWorkunit(const CString & wuid)
{
	StlLinked<Dali::IDali> server = Dali::AttachDali(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT), _T("Dali"));
	SetWorkunit(server->GetWorkunit(wuid));
}

void CMultiResultView::SetWorkunit(Dali::IWorkunit *wu)
{
	ATLASSERT(!m_wu.get()); //really can't change it!!!!
	m_wu = wu;
	//PrepareForDisplay();
}

//  CExecEcl override  ---
void CMultiResultView::WorkunitCreated(Dali::IWorkunit * workunit)
{
	if (::IsWindow(m_hWnd))
	{
		ATLTRACE(_T("CMultiResultView::WorkunitCreated(%s - %d)\n"), workunit->GetWuid(), workunit->GetState());
		m_wu = workunit;
		PostMessage(CWM_REFRESH, TRUE);
	}
}

void CMultiResultView::WorkunitUpdated(Dali::IWorkunit * workunit)
{
	if (::IsWindow(m_hWnd))
	{
		ATLTRACE(_T("CMultiResultView::WorkunitUpdated(%s - %d)\n"), workunit->GetWuid(), workunit->GetState());
		PostMessage(CWM_REFRESH, 0);
	}
}

void CMultiResultView::WorkunitDeleted(Dali::IWorkunit * workunit)
{
	if (::IsWindow(m_hWnd))
	{
		ATLTRACE(_T("CMultiResultView::WorkunitDeleted(%s - %d)\n"), workunit->GetWuid(), workunit->GetState());
		PostMessage(CWM_REFRESH, FALSE, TRUE);
	}
}

bool CMultiResultView::UpdateUI(CCmdUI *cui)
{
	//this is for the top level frame menu, but will get
	//applied indirectly to context menus
	UPDATEUI(cui, ID_EDIT_COPY, CanCopy());
	UPDATEUI(cui, ID_EDIT_SELECT_ALL, CanSelect() );
	UPDATEUI(cui, ID_ECL_SYNCTOC, true);
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->UpdateUI(cui);
	return false;
}

bool CMultiResultView::HasResults() const
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->HasResults();
	return false;
}
bool CMultiResultView::CanSelect()
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->CanSelect();
	return false;
}
bool CMultiResultView::CanCopy()
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->CanCopy();
	return false;
}
__int64 CMultiResultView::GetItemCount()
{
	__int64 total = 0;
	__int64 loaded;
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->GetResults(total,loaded);
	return total;
}

void CMultiResultView::SaveSelectionToFile(CUnicodeFile &file)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->SaveSelectionToFile(file);
}

void CMultiResultView::SaveToFile(const CResultFormatter &formatter)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->SaveToFile(formatter);
}

void CMultiResultView::SetNamePath(const CString & sPathName)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->SetNamePath(sPathName);
}

CString CMultiResultView::GetFilePath()
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->GetFilePath();
	return _T("");
}

const TCHAR *CMultiResultView::GetID(CString & result)
{
	if ( !m_wu )
		result = _T("");
	else
	{
		result = m_wu->GetWuid();
	}
	return result;
}

CString CMultiResultView::GetTitle()
{
	ATLASSERT(m_wu != NULL);
	return m_wu->GetLabel();
}

bool CMultiResultView::IsDirty()
{
	return false;
}

//ISearchable
bool CMultiResultView::CanBeSearched() const
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->CanBeSearched();
	return false;
}

//ISearchable
bool CMultiResultView::Search(const CSearchableInfo &info, CSearchableResult &result) const
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		return m_tabs[n]->Search(info,result);
	return false;
}

void CMultiResultView::GotoLine(__int64 line, __int64 col)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->GotoLine(line,col);
}

void CMultiResultView::GetResults(__int64 &total, __int64 &loaded)
{
	unsigned n = m_tabbedChildWindow.GetTabCtrl().GetCurSel();
	if (n >= 0 && n < m_tabs.size())
		m_tabs[n]->GetResults(total,loaded);

}

LRESULT CMultiResultView::OnShowEclWatch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if ( m_summaryView.isLinked() )
	{
		m_tabbedChildWindow.DisplayTab(m_summaryView->GetHWND(), TRUE);
	}
	return 0;
}

//  ======================================================================================================================================================
IResultViewer *CreateIResultViewer(IResultSlot *owner)
{
	return new CMultiResultView(owner);
}

IResultViewer *CreateIResultViewer(IResultSlot *owner, Dali::IWorkunit *wu, BuilderStartup startupMode)
{
	return new CMultiResultView(owner, wu, startupMode);
}

IResultViewer *CreateIResultViewer(IResultSlot *owner, const CString & wuid)
{
	return new CMultiResultView(owner, wuid);
}




