#pragma once

#include "RecursiveMutex.h"
#include "searchable.h"

class CUnicodeFile;
class CResultFormatter;

typedef std::pair<unsigned int, std::_tstring> SplitPair;
typedef std::vector<SplitPair> SplitVector;

enum COLUMN_TYPE_ENUM
{
	CT_UNKNOWN,
	CT_BOOLEAN,
	CT_INT,
	CT_UINT,
	CT_REAL,
	CT_STRING,
	CT_DATA,
	CT_DEC,
	CT_UDEC,
	CT_LAST
};

__interface ITable : public ISearchable, public IUnknown
{
	//  Row Processing
	__int64 GetRowCount() const;
	bool RemoveRow(__int64 row);

	//  Cell Processing
	const TCHAR * GetCell(__int64 row, unsigned col) const;
	const TCHAR * GetCell(__int64 row, const std::_tstring & col) const;
	void SetCell(__int64 row, unsigned col, const std::_tstring &val);
	void AppendCell(__int64 row, unsigned col, const std::_tstring &val);
	IUnknown * GetCellUnknown(__int64 row, unsigned col) const;
	void SetCellUnknown(__int64 row, unsigned col, IUnknown * data);

	//  Header Processing
	unsigned GetColumnCount() const;
	const TCHAR * GetColumn(unsigned col) const;
	COLUMN_TYPE_ENUM GetColumnType(unsigned col) const;
	bool IsColumnString(unsigned col) const;
	signed FindColumn(const std::_tstring & val) const;
	signed FindColumn(const std::_tstring & val, bool addIfMissing, COLUMN_TYPE_ENUM type = CT_STRING);
	void SetColumn(unsigned col, const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING);
	int AddColumn(const std::_tstring & val, COLUMN_TYPE_ENUM type = CT_STRING);
	void SplitColumn(unsigned int col, const SplitVector & positions);

	void DoClear();
	bool IsChanged();
	void LoadFrom(ITable * table);
	void LoadFrom(ITable * table, __int64 start);
	void CopyFrom(ITable * table, __int64 start, __int64 count);
	bool LoadFrom(const CComBSTR & xml, __int64 start);			//SeisintDataset from supercomputer
	bool LoadFrom(const CComBSTR & xsd, const CComBSTR & xml);	//C# STyle Datatset
	bool LoadFromDataset(const CComBSTR & xml);					//Simple Style Dataset
	bool ToXML(std::_tstring & xml, bool skipFirstColumn) const;
	bool ToXML(const std::_tstring & table_label, const std::_tstring & row_label, std::_tstring & xml, bool skipFirstColumn) const;
	bool ToXML(__int64 row, const std::_tstring & table_label, const std::_tstring & row_label, std::_tstring & xml, bool skipFirstColumn) const;
	void CopyRow(__int64 row, const CResultFormatter &formatter, std::_tstring &line) const;
	void SaveAs(CUnicodeFile &file, const CResultFormatter &formatter) const;
	void SaveAs(std::_tstring &all, const CResultFormatter &formatter) const;
	unsigned GetCellLength(__int64 row, unsigned col) const;
	unsigned GetMaxCellLength(unsigned col) const;

	//ChildTableSet Support
	void SetHasChildDataset(bool hasChildDataset=true);
	bool GetHasChildDataset() const;
	ITable * GetChildDataset(__int64 row, unsigned col) const;
	ITable * GetChildDataset(__int64 row, const std::_tstring & col) const;

	//Sorting
	void Sort(unsigned int col);
	void SortAgain();
};

__interface ISparseTable : public ITable
{
	__int64 GetMaxRowNum() const;
	bool RowExists(__int64 row) const;
	bool GetScopedLock(clib::scoped_lock_ref_counted_adapt &lock) const;
};

typedef StlLinked<ISparseTable> ISparseTableAdapt;
typedef StlLinked<ITable> ITableAdapt;
typedef std::vector<ITableAdapt> ITableVector;
class TableInfo
{
public:
	std::_tstring m_schema;
	ITableAdapt m_table;
};
typedef std::map<std::_tstring, TableInfo> ITableMap;
typedef CUnknownT<ITableMap> ITableMapCom;
typedef StlLinked<ITableMapCom> ITableMapAdapt;

CLIB_API ISparseTable * CreateISparseTable();

