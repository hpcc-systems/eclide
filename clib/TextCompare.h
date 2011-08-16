#pragma once

#include "clib.h"

//  ===========================================================================
namespace CLIB
{
class CLIB_API CLine
{
public:
	enum TYPE
	{
		TYPE_UNKNOWN = 0,
		TYPE_MATCH,
		TYPE_MOVED,
		TYPE_SIMILAR,
		TYPE_ADDED,
		TYPE_DELETED,
		TYPE_LAST
	};
	typedef std::vector<TYPE> TypeVector;

protected:
	TYPE m_type;
	std::_tstring m_origText;
	TypeVector m_charType;

	int m_anchorHash;
	int m_hash;

public:
	CLine();
	CLine(TYPE type);
	CLine(const std::_tstring & line);

	void SetType(TYPE type);
	TYPE GetType() const;
	void SetCharType(std::size_t charPos, TYPE type);
	TYPE GetCharType(std::size_t charPos) const;
	int GetLength() const;
	const TCHAR * GetText() const;
	unsigned int Distance(const CLine & other) const;
	bool Equals(const CLine & other, unsigned int level) const;

protected:
	int GetAnchorHash() const;
	int GetHash() const;
	void hash_string(const std::_tstring & s);
};
typedef std::vector<CLine> CLineVector;
//  ===========================================================================
class CTable
{
protected:
	CLineVector m_rows;

public:
	CTable();
	CTable(const std::_tstring & table, bool line=false);
	CTable(const CTable & table, unsigned int from, unsigned int to);
	size_t size() const;
	CLine & operator[](int pos);
	const CLine & operator[](int pos) const;
	void push_back(const CLine & row);
	void Insert(int pos, int length, CLine::TYPE type);
	void Mark(int start, int length, CLine::TYPE type);
	void Get(CLineVector & results);

protected:
	void parse_rows(const std::_tstring & table);
	void parse_line(const std::_tstring & table);
};
//  ===========================================================================
class CMatchBlock;
typedef std::vector<CMatchBlock> CMatchBlockVector;
class CMatchBlock
{
protected:
	int m_base_pos;
	int m_comp_pos;
	int m_length;

public:
	CMatchBlock();
	CMatchBlock(int base_pos, int comp_pos, int length);

	int GetBasePos() const;
	int GetCompPos() const;
	int GetLength() const;

	bool BaseContains(int pos) const;
	bool BaseOverlaps(const CMatchBlock & other) const;
	bool CompContains(int pos) const;
	bool CompOverlaps(const CMatchBlock & other) const;
	bool Overlaps(const CMatchBlock & other) const;
	bool OverlapAndSplit(const CMatchBlock & other, CMatchBlockVector & additionalBlocks);
	bool IsBaseBefore(const CMatchBlock & other) const;
	bool IsBaseAfter(const CMatchBlock & other) const;
	bool IsCompBefore(const CMatchBlock & other) const;
	bool IsCompAfter(const CMatchBlock & other) const;
};
//  ===========================================================================
class CAnchors
{
protected:
	const CTable & m_baseTable;
	const CTable & m_compTable;
	int m_accuracy;

	CMatchBlockVector m_anchors;
	CMatchBlockVector m_moved;

public:
	CAnchors(const CTable & baseTable, const CTable & compTable, int accuracy);

	void GetResults(CLineVector & baseResult, CLineVector & compResult);
	void GetPaddedResults(CLineVector & baseResult, CLineVector & compResult) const;
	bool AnchorsBaseContains(unsigned int pos) const;
	bool AnchorsCompContains(unsigned int pos) const;
	bool MovedBaseContains(unsigned int pos) const;
	bool MovedCompContains(unsigned int pos) const;

	unsigned int GetMatchCount() const;
	void GetMatch(int gap, CTable & baseTable, CTable & compTable) const;
	unsigned int GetGapCount() const;
	void GetGap(int gap, CTable & baseTable, CTable & compTable) const;

protected:
	void CalculateMatches(CMatchBlockVector & matches) const;
};
//  ===========================================================================
void CLIB_API TextCompare(const std::_tstring & base, const std::_tstring & comp, CLineVector & baseResult, CLineVector & compResult);
//  ===========================================================================
}