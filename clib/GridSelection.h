#pragma once

#include "clib.h"
#include <limits.h>


class CSelectionBox 
{
public:
	CSelectionBox() { Nowhere(); };

	bool RowIn(__int64 row) const { return row >= rowTop && row <= rowBottom; };
	void Nowhere() { rowTop = rowBottom = -1; columnLeft = columnRight = -1; };
	void ExtendableInit() {	rowTop = LONG_MAX; rowBottom  = -1;
							columnLeft = INT_MAX; columnRight = -1; };
	void Everywhere() { rowTop = 0; rowBottom = LONG_MAX; 
						columnLeft = 0; columnRight = INT_MAX; };

	__int64 rowTop;
	__int64 rowBottom;
	int columnLeft;
	int columnRight;
};

class CGridSelection : public CUnknown
{
protected:
	CGridSelection(void);
public:
	IMPLEMENT_CUNKNOWN

	virtual ~CGridSelection();
public:
	virtual bool IsSelected(__int64 row, int column) = 0;
	virtual bool operator ==(const CGridSelection *pOther) const = 0;
	virtual void ExtendBox(CSelectionBox &box) const = 0;

	typedef enum { eNone, eAll, eRow, eColumn, eCell, eBox } SelectionType;
	virtual SelectionType GetType() const = 0;
};

class CLIB_API CGridSelectionCollection
{
public:
	CGridSelectionCollection(void);
	virtual ~CGridSelectionCollection();
	virtual CGridSelectionCollection & operator =(const CGridSelectionCollection& other);

	typedef std::vector<__int64> SelectionRows;
	typedef std::vector<int> SelectionColumns;
	typedef struct {
		__int64 row;
		int column;
	} SelectionCell;
	typedef CSelectionBox SelectionBox;

	void SelectNothing();
	void SelectAll();
	void SelectCell(bool bAdd, __int64 row, int column);
	void SelectColumn(bool bAdd, int column, int fromColumn);
	void SelectRow(bool bAdd, __int64 row, __int64 fromRow, int numColumnsInRow);
	void SelectBox(__int64 row, int column);
	void SelectBox(SelectionBox &box);

	//keyboard interface
	void SelectLeft(bool extendedSelect, __int64 row, int column);
	void SelectHome(bool extendedSelect, __int64 row, int fromColumn);
	void SelectRight(bool extendedSelect, __int64 row, int column, int lastColumn);
	void SelectEnd(bool extendedSelect, __int64 row, int fromColumn, int lastColumn);
	void SelectUp(bool extendedSelect, __int64 row, int column);
	void SelectDown(bool extendedSelect, __int64 row, int column, __int64 lastRow);

	bool IsSelected(__int64 row, int column) const;
	bool HasSelection() const;

	void inline ResetSelectionChanged() { m_bSelectChanged = false; };
	bool inline SelectionChanged() const { return m_bSelectChanged; };

	bool GetSelectionAll() const;
	bool GetSelectionRows(SelectionRows &rows) const;
	bool GetSelectionColumns(SelectionColumns &columns) const;
	bool GetSelectionBox(SelectionBox &box) const;
	bool GetSelectionBox(SelectionBox &box, __int64 maxRows, int maxColumns) const;
	bool GetSelectionCell(SelectionCell &cell) const;

	bool AllRowsSelected(__int64 rows) const;

protected:
	typedef StlLinked<CGridSelection> CGridSelectionAdapt;
	typedef	std::list<CGridSelectionAdapt> GridSelectionList;

	GridSelectionList m_selectionList;
	bool	m_bSelectChanged;
	void Add(bool bAdd, CGridSelectionAdapt item);
	bool Exists(const CGridSelectionAdapt item) const;
	void RemoveOtherSelectionTypes(CGridSelection::SelectionType gridType);
	void ClearList();
	static void ExtendBox(SelectionBox &box, __int64 row, int column);
	CGridSelection::SelectionType GetSelectionType() const;
};

class CLIB_API CGridSelectionMarquee
{
public:
	CGridSelectionMarquee();
	~CGridSelectionMarquee() {};

	void Start(__int64 startRow, int startCol);
	void End();
	bool Update(__int64 endRow, int endCol);
	bool inline IsActive() const { return m_bActive; }
	bool GetSelectionBox(CSelectionBox &box) const;
protected:
	__int64 m_startRow, m_endRow;
	int	    m_startColumn, m_endColumn;
	bool    m_bActive;
};


