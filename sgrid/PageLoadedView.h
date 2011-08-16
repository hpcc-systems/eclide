#pragma once

#include <vector>

class RowRange;
typedef std::vector<RowRange> RowRangeVector;
class RowRange
{
public:
	int m_from;
	int m_to;
	enum STATE
	{
		S_BLANK,
		S_REQUESTED,
		S_LOADED
	} m_state;

	RowRange();
	RowRange(int from, int to, STATE state);

	bool IsValid() const;
	size_t SplitMerge(RowRange & other, RowRangeVector & results) const;

	bool IsMatchedNeighbour(const RowRange & other) const;
	bool Contains(const RowRange & other) const;
	bool ContainsFrom(const RowRange & other) const;
	bool ContainsTo(const RowRange & other) const;

	bool operator < (const RowRange & r) const;
};

class CPageLoadedView
{
protected:
	int m_total;
	RowRangeVector m_loaded;

public:
	CPageLoadedView();
	~CPageLoadedView();

	void SetTotal(int total);
	void SetRequested(int from, int to);
	void SetLoaded(int from, int to);
	bool IsFullyLoaded() const;
	void DoPaint(WTL::CDCHandle & dc, CRect rect) const;

protected:
	void InsertItem(int from, int to, RowRange::STATE state);
	void CalcNonLoaded();
};
