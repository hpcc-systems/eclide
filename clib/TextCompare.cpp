#include "StdAfx.h"
#include "TextCompare.h"

namespace CLIB
{
//  ===========================================================================
CLine::CLine() : m_origText(_T("\r\n"))
{
    m_type = TYPE_UNKNOWN;
    hash_string(m_origText);
    m_charType.resize(m_origText.length(), CLine::TYPE_MATCH);
}

CLine::CLine(TYPE type) : m_origText(_T("\r\n"))
{
    m_type = type;
    hash_string(m_origText);
    m_charType.resize(m_origText.length(), CLine::TYPE_MATCH);
}

CLine::CLine(const std::_tstring & text) : m_origText(text)
{
    m_type = TYPE_UNKNOWN;
    hash_string(m_origText);
    m_charType.resize(m_origText.length(), CLine::TYPE_MATCH);
}

void CLine::SetType(TYPE type) 
{
    if (m_type == TYPE_UNKNOWN)
        m_type = type;
}

CLine::TYPE CLine::GetType() const
{
    return m_type;
}

int CLine::GetLength() const 
{
    return m_origText.size();
}

void CLine::SetCharType(size_t charPos, TYPE type)
{
    if (charPos < m_charType.size())
        m_charType[charPos] = type;
}

CLine::TYPE CLine::GetCharType(size_t charPos) const
{
    return m_charType[charPos];
}

const TCHAR * CLine::GetText() const
{
    return m_origText.c_str();
}

unsigned int CLine::Distance(const CLine & other) const
{
    const std::_tstring & source = m_origText;
    const std::_tstring & target = other.m_origText;
    // Step 1
#define CUTOFF 160
    const int n = source.length() > CUTOFF ? CUTOFF : source.length();
    const int m = target.length() > CUTOFF ? CUTOFF : target.length();
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }

    typedef std::vector< std::vector<int> > Tmatrix; 
    Tmatrix matrix(n+1);
    // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
    // allow for allocation on declaration of 2.nd dimension of vec of vec
    for (int i = 0; i <= n; i++) {
        matrix[i].resize(m+1);
    }

    // Step 2
    for (int i = 0; i <= n; i++) {
        matrix[i][0]=i;
    }
    for (int j = 0; j <= m; j++) {
        matrix[0][j]=j;
    }

    // Step 3
    for (int i = 1; i <= n; i++) {
        const TCHAR s_i = source[i-1];

        // Step 4
        for (int j = 1; j <= m; j++) {
            const TCHAR t_j = target[j-1];

            // Step 5
            int cost;
            if (s_i == t_j) {
                cost = 0;
            }
            else {
                cost = 1;
            }

            // Step 6
            const int above = matrix[i-1][j];
            const int left = matrix[i][j-1];
            const int diag = matrix[i-1][j-1];
            int cell = min( above + 1, min(left + 1, diag + cost));

            // Step 6A: Cover transposition, in addition to deletion,
            // insertion and substitution. This step is taken from:
            // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's 
            // Enhanced Dynamic Programming ASM Algorithm"
            // (http://www.acm.org/~hlb/publications/asm/asm.html)
            if (i>2 && j>2) {
                int trans=matrix[i-2][j-2]+1;
                if (source[i-2]!=t_j) trans++;
                if (s_i!=target[j-2]) trans++;
                if (cell>trans) cell=trans;
            }

            matrix[i][j]=cell;
        }
    }

    // Step 7
    return matrix[n][m];
}

bool CLine::Equals(const CLine & other, unsigned int level) const
{
    if (level == 0)
        return other.GetHash() == GetHash();
    else if(level == 1)
        return other.GetAnchorHash() == GetAnchorHash();
    return Distance(other) < level;
}

int CLine::GetAnchorHash() const
{
    return m_anchorHash;
}

int CLine::GetHash() const
{
    return m_hash;
}

void CLine::hash_string(const std::_tstring & s)
{
    // Calc Hash
    boost::crc_32_type computer;
    computer.reset();
    computer.process_bytes(s.c_str(), sizeof(TCHAR) * s.length());
    m_hash = computer.checksum();

    // Calc Anchor Hash
    bool prevIsGraph = true;
    static boost::crc_32_type anchorComputer;
    anchorComputer.reset();
    for (unsigned int i = 0; i < s.length(); ++i)
    {
        bool isGraph = iswgraph(s[i]) != 0 || s[i] == '\r' || s[i] == '\n';
        if (isGraph)
            anchorComputer.process_bytes(&s[i], sizeof(TCHAR));
        prevIsGraph = isGraph;
    }
    m_anchorHash = anchorComputer.checksum();
}
//  ===========================================================================
const TCHAR * const GUID_START = _T("{EE9F6CEA-6904-4a9e-8D15-F2E225A8C02D}Start{EE9F6CEA-6904-4a9e-8D15-F2E225A8C02D}");
const TCHAR * const GUID_END   = _T("{EE9F6CEA-6904-4a9e-8D15-F2E225A8C02D} End {EE9F6CEA-6904-4a9e-8D15-F2E225A8C02D}");
CLine startLine(GUID_START);
CLine endLine(GUID_END);
bool IsDummyLine(const CLine & line)
{
    ATLASSERT((line.Equals(startLine, 0) || line.Equals(endLine, 0)) == false);
    return (line.Equals(startLine, 0) || line.Equals(endLine, 0));
}

CTable::CTable()
{
}

CTable::CTable(const std::_tstring & table, bool line)
{
    m_rows.push_back(CLine(GUID_START));
    if (line)
        parse_line(table);
    else
        parse_rows(table);
    m_rows.push_back(CLine(GUID_END));
}

CTable::CTable(const CTable & table, unsigned int from, unsigned int to)
{
    m_rows.push_back(CLine(GUID_START));
    if (from == 0)
        ++from;
    if (to == table.size())
        --to;
    for (unsigned int i = from; i < to; ++i)
        m_rows.push_back(table[i]);
    m_rows.push_back(CLine(GUID_END));
}

size_t CTable::size() const
{
    return m_rows.size();
}

CLine & CTable::operator[](int pos)
{
    return m_rows[pos];
}

const CLine & CTable::operator[](int pos) const
{
    return m_rows[pos];
}

void CTable::push_back(const CLine & row)
{
    m_rows.push_back(row);
}

void CTable::Insert(int pos, int length, CLine::TYPE type)
{
    m_rows.insert(m_rows.begin() + pos, CLine());
    Mark(pos, length, type);
}

void CTable::Mark(int start, int length, CLine::TYPE type)
{
    for(int i = start; i < start + length; ++i)
    {
        m_rows[i].SetType(type);
    }
}

void CTable::Get(CLineVector & results)
{
    results.clear();
    results.reserve(m_rows.size() - 2);
    for(CLineVector::iterator itr = m_rows.begin(); itr != m_rows.end();)
    {
        CLineVector::iterator this_itr = itr++;
        if (this_itr != m_rows.begin() && itr != m_rows.end())
            results.push_back(*this_itr);
    }
}

void CTable::parse_rows(const std::_tstring & table)
{
    std::_tstring line;
    for(std::size_t i = 0; i < table.size(); ++i)
    {
        line += table[i];
        if (table[i] == _T('\n'))
        {
            m_rows.push_back(CLine(line));
            line = _T("");
        }
    }
    if (line.size())
        m_rows.push_back(CLine(line));
}

void CTable::parse_line(const std::_tstring & table)
{
    for(std::size_t i = 0; i < table.size(); ++i)
    {
        std::_tstring tchar;
        tchar.resize(1, table[i]);
        m_rows.push_back(CLine(tchar));
    }
}
//  ===========================================================================
CMatchBlock::CMatchBlock()
{
    m_base_pos = 0;
    m_comp_pos = 0;
    m_length = 0;
}

CMatchBlock::CMatchBlock(int base_pos, int comp_pos, int length) : m_base_pos(base_pos), m_comp_pos(comp_pos), m_length(length)
{
}

int CMatchBlock::GetBasePos() const
{
    return m_base_pos;
}

int CMatchBlock::GetCompPos() const
{
    return m_comp_pos;
}

int CMatchBlock::GetLength() const
{
    return m_length;
}

bool CMatchBlock::BaseContains(int pos) const
{
    return pos >= m_base_pos && pos < m_base_pos + m_length;
}

bool CMatchBlock::BaseOverlaps(const CMatchBlock & other) const
{
    return (other.BaseContains(m_base_pos) || BaseContains(other.m_base_pos));
}

bool CMatchBlock::CompContains(int pos) const
{
    return pos >= m_comp_pos && pos < m_comp_pos + m_length;
}

bool CMatchBlock::CompOverlaps(const CMatchBlock & other) const
{
    return (other.CompContains(m_comp_pos) || CompContains(other.m_comp_pos));
}

bool CMatchBlock::Overlaps(const CMatchBlock & other) const
{
    return (BaseOverlaps(other) || CompOverlaps(other));
}

bool CMatchBlock::OverlapAndSplit(const CMatchBlock & other, CMatchBlockVector & additionalBlocks)
{
    //  Three Senarios:  Other overlaps start of this.  Other Overlaps end of this.  This is contained between start and end of Other.
    ATLASSERT(m_length <= other.m_length);
    if (BaseOverlaps(other) && CompOverlaps(other)) //This is contained in Other
    {
    }
    else if (BaseOverlaps(other))
    {
    }
    else if(CompOverlaps(other))
    {
    }
    else
    {
        ATLASSERT(!Overlaps(other));
        return false;
    }
    return true;
}

bool CMatchBlock::IsBaseBefore(const CMatchBlock & other) const
{
    return (m_base_pos + m_length - 1 < other.m_base_pos);
}

bool CMatchBlock::IsBaseAfter(const CMatchBlock & other) const
{
    return (other.m_base_pos + other.m_length - 1 < m_base_pos);
}

bool CMatchBlock::IsCompBefore(const CMatchBlock & other) const
{
    return (m_comp_pos + m_length - 1 < other.m_comp_pos);
}

bool CMatchBlock::IsCompAfter(const CMatchBlock & other) const
{
    return (other.m_comp_pos + other.m_length - 1 < m_comp_pos);
}
class CMatchBlockCompareLength
{
public:
    bool operator ()(const CMatchBlock & l, const CMatchBlock & r)
    {
        if (l.GetLength() == r.GetLength())
        {
            if (l.GetBasePos() == r.GetBasePos())
                return l.GetCompPos() < r.GetCompPos();

            return l.GetBasePos() < r.GetBasePos();
        }
        return l.GetLength() > r.GetLength();
    }
};
class CMatchBlockCompareBase
{
public:
    bool operator ()(const CMatchBlock & l, const CMatchBlock & r)
    {
        return l.GetBasePos() < r.GetBasePos();
    }
};
class CMatchBlockCompareComp
{
public:
    bool operator ()(const CMatchBlock & l, const CMatchBlock & r)
    {
        return l.GetCompPos() < r.GetCompPos();
    }
};
//  ===========================================================================
CAnchors::CAnchors(const CTable & baseTable, const CTable & compTable, int accuracy) : m_baseTable(baseTable), m_compTable(compTable), m_accuracy(accuracy)
{
    m_anchors.clear();
    m_moved.clear();

    //  Calculate all matches.
    CMatchBlockVector matches;
    CalculateMatches(matches); //matches are sorted -> largest length, smallest basePos, smallest compPos.

    //  Remove out of sequence (moved lines).  
    for(CMatchBlockVector::iterator matches_itr = matches.begin(); matches_itr != matches.end(); ++matches_itr)
    {
        bool fits_in = m_anchors.size() == 0 ? true : false;
        for (CMatchBlockVector::iterator anchors_next_itr = m_anchors.begin(); anchors_next_itr != m_anchors.end();)
        {				
            CMatchBlockVector::iterator anchors_itr = anchors_next_itr++;
            if (anchors_itr == m_anchors.begin() &&		matches_itr->IsBaseBefore(*anchors_itr)			&& matches_itr->IsCompBefore(*anchors_itr))
            {
                fits_in = true;
                break;
            }
            if (anchors_next_itr != m_anchors.end() &&	matches_itr->IsBaseAfter(*anchors_itr)			&& matches_itr->IsCompAfter(*anchors_itr) && 
                                                        matches_itr->IsBaseBefore(*anchors_next_itr)	&& matches_itr->IsCompBefore(*anchors_next_itr))
            {
                fits_in = true;
                break;
            }
            if (anchors_next_itr == m_anchors.end() &&	matches_itr->IsBaseAfter(*anchors_itr)			&& matches_itr->IsCompAfter(*anchors_itr))
            {
                fits_in = true;
                break;
            }
        }
        if (fits_in)
        {
            m_anchors.push_back(*matches_itr);
            std::sort(m_anchors.begin(), m_anchors.end(), CMatchBlockCompareBase());
        }
        else
        {
            m_moved.push_back(*matches_itr);
        }
    }
    std::sort(m_moved.begin(), m_moved.end(), CMatchBlockCompareBase());
}

void CAnchors::GetResults(CLineVector & baseResult, CLineVector & compResult)
{
    for(size_t i = 1; i < m_baseTable.size() - 1; ++i)
    {
        CLine line = m_baseTable[i];
        if (AnchorsBaseContains(i))
            line.SetType(CLine::TYPE_MATCH);
        else
            line.SetType(CLine::TYPE_SIMILAR);
        baseResult.push_back(line);
    }

    for(size_t i = 1; i < m_compTable.size() - 1; ++i)
    {
        CLine line = m_compTable[i];
        if (AnchorsCompContains(i))
            line.SetType(CLine::TYPE_MATCH);
        else
            line.SetType(CLine::TYPE_SIMILAR);
        compResult.push_back(line);
    }
}

void AppendTable(CLineVector & lineVector, const CTable & table, CLine::TYPE type)
{
    for (size_t i = 1; i < table.size() - 1; ++i)
    {
        CLine line = table[i];
        line.SetType(type);
        lineVector.push_back(line);
    }
}

void AppendBlankLines(CLineVector & lineVector, const CTable & otherTable, CLine::TYPE type)
{
    for (size_t i = 1; i < otherTable.size() - 1; ++i)
    {
        CLine line;
        if (!IsDummyLine(line))
        {
            line.SetType(otherTable[i].GetType() == CLine::TYPE_MOVED ? CLine::TYPE_MATCH : type);
            lineVector.push_back(line);
        }
    }
}

void SetCharTypes(CLine & line, const CLineVector & lineVector)
{
    int charPos = 0;
    for(CLineVector::const_iterator itr = lineVector.begin(); itr != lineVector.end(); ++itr)
    {
        switch(itr->GetType())
        {
        case CLine::TYPE_DELETED:
        case CLine::TYPE_SIMILAR:
        case CLine::TYPE_MOVED:
            line.SetCharType(charPos, CLine::TYPE_SIMILAR);
            break;
        }
        charPos++;
    }
}

void CAnchors::GetPaddedResults(CLineVector & baseResult, CLineVector & compResult) const
{
    unsigned int matchCount = GetMatchCount();
    unsigned int gapCount = GetGapCount();
    unsigned int matchPos = 0;
    unsigned int gapPos = 0;
    while(matchPos < matchCount)
    {
        {
            CTable baseTable, compTable;
            GetMatch(matchPos, baseTable, compTable);
            for (size_t i = 1; i < baseTable.size() - 1; ++i)
            {
                CLine baseLine = baseTable[i];
                CLine compLine = compTable[i];
                CLine::TYPE type = baseLine.Equals(compLine, 0) ? CLine::TYPE_MATCH : CLine::TYPE_SIMILAR;
                if (type == CLine::TYPE_SIMILAR)
                {
                    CTable baseLineTable(baseLine.GetText(), true);
                    CTable compLineTable(compLine.GetText(), true);
                    CAnchors lineAnchors(baseLineTable, compLineTable, 0);
                    CLineVector baseLineVector, compLineVector;
                    lineAnchors.GetResults(baseLineVector, compLineVector);
                    SetCharTypes(baseLine, baseLineVector);
                    SetCharTypes(compLine, compLineVector);
                }
                baseLine.SetType(type);
                compLine.SetType(type);
                baseResult.push_back(baseLine);
                compResult.push_back(compLine);
            }
            ATLASSERT(baseResult.size() == compResult.size());
            ++matchPos;
        }
        if (gapPos < gapCount)
        {
            CTable baseTable, compTable;
            GetGap(gapPos, baseTable, compTable);
            if (baseTable.size() <= 2)	//Added lines
            {
                ATLASSERT(compTable.size() > 2);
                AppendBlankLines(baseResult, compTable, CLine::TYPE_ADDED);
                AppendTable(compResult, compTable, CLine::TYPE_ADDED);
            }
            else if (compTable.size() <= 2)  //Deleted lines
            {
                ATLASSERT(baseTable.size() > 2);
                AppendTable(baseResult, baseTable, CLine::TYPE_DELETED);
                AppendBlankLines(compResult, baseTable, CLine::TYPE_DELETED);
            }
            else if (m_accuracy > 35)	//  Don't recurse too much.
            {
                AppendTable(baseResult, baseTable, CLine::TYPE_DELETED);
                AppendBlankLines(compResult, baseTable, CLine::TYPE_DELETED);
                AppendBlankLines(baseResult, compTable, CLine::TYPE_ADDED);
                AppendTable(compResult, compTable, CLine::TYPE_ADDED);
            }
            else
            {
                CAnchors gapAnchors(baseTable, compTable, m_accuracy + 5);
                gapAnchors.GetPaddedResults(baseResult, compResult);
            }
            ATLASSERT(baseResult.size() == compResult.size());
            ++gapPos;
        }
    }
}

bool Contains(const CMatchBlockVector & itmes, unsigned int pos, bool comp)
{
    for (CMatchBlockVector::const_iterator item_itr = itmes.begin(); item_itr != itmes.end(); ++item_itr)
    {
        if (comp && item_itr->CompContains(pos))
            return true;
        if (!comp && item_itr->BaseContains(pos))
            return true;
    }
    return false;
}

bool CAnchors::AnchorsBaseContains(unsigned int pos) const
{
    return Contains(m_anchors, pos, false);
}

bool CAnchors::AnchorsCompContains(unsigned int pos) const
{
    return Contains(m_anchors, pos, true);
}

bool CAnchors::MovedBaseContains(unsigned int pos) const
{
    return Contains(m_moved, pos, false);
}

bool CAnchors::MovedCompContains(unsigned int pos) const
{
    return Contains(m_moved, pos, true);
}

unsigned int CAnchors::GetMatchCount() const
{
    return m_anchors.size();
}

void CAnchors::GetMatch(unsigned int match, CTable & baseTable, CTable & compTable) const
{
    ATLASSERT(match >= 0 && match < GetMatchCount());
    for (CMatchBlockVector::const_iterator anchors_itr = m_anchors.begin(); anchors_itr != m_anchors.end(); ++anchors_itr)
    {				
        if (--match < 0)
        {
            baseTable = CTable(m_baseTable, anchors_itr->GetBasePos(), anchors_itr->GetBasePos() + anchors_itr->GetLength());
            compTable = CTable(m_compTable, anchors_itr->GetCompPos(), anchors_itr->GetCompPos() + anchors_itr->GetLength());
            break;
        }
    }
}

unsigned int CAnchors::GetGapCount() const
{
    ATLASSERT(m_anchors.size() >= 1);		//Because of dummy 1st and last lines...
    return m_anchors.size() - 1;
}

void CAnchors::GetGap(unsigned int gap, CTable & baseTable, CTable & compTable) const
{
    ATLASSERT(gap >= 0 && gap < GetGapCount());
    for (CMatchBlockVector::const_iterator anchors_next_itr = m_anchors.begin(); anchors_next_itr != m_anchors.end();)
    {				
        CMatchBlockVector::const_iterator anchors_itr = anchors_next_itr++;
        if (--gap < 0)
        {
            baseTable = CTable(m_baseTable, anchors_itr->GetBasePos() + anchors_itr->GetLength(), anchors_next_itr->GetBasePos());
            compTable = CTable(m_compTable, anchors_itr->GetCompPos() + anchors_itr->GetLength(), anchors_next_itr->GetCompPos());

            for(int i = 0; i < anchors_next_itr->GetBasePos() - (anchors_itr->GetBasePos() + anchors_itr->GetLength()); ++i)
            {
                if (MovedBaseContains(anchors_itr->GetBasePos() + anchors_itr->GetLength() + i))
                    baseTable[i + 1].SetType(CLine::TYPE_MOVED);
            }
            for(int i = 0; i < anchors_next_itr->GetCompPos() - (anchors_itr->GetCompPos() + anchors_itr->GetLength()); ++i)
            {
                if (MovedCompContains(anchors_itr->GetCompPos() + anchors_itr->GetLength() + i))
                    compTable[i + 1].SetType(CLine::TYPE_MOVED);
            }
            break;
        }
    }
}

void CAnchors::CalculateMatches(CMatchBlockVector & tidied_matches) const
{
    CMatchBlockVector matches;
    int base_total = m_baseTable.size();
    int comp_total = m_compTable.size();
    for(int i = 0; i < base_total; ++i)
    {
        for(int j = 0; j < comp_total; ++j)
        {
            int match_length = 0;
            int match_i = i, match_j = j;
            while (match_i < base_total && match_j < comp_total && m_baseTable[match_i].Equals(m_compTable[match_j], m_accuracy))
            {
                ++match_i;
                ++match_j;
                ++match_length;
            }
            for (int all_lengths = 1; all_lengths <= match_length; ++all_lengths)
                matches.push_back(CMatchBlock(i, j, all_lengths));
            j += match_length;
        }
    }

    //  Remove overlaps
    bool foundOverlap = true;
    //while(foundOverlap)
    {
        std::sort(matches.begin(), matches.end(), CMatchBlockCompareLength());
        for(CMatchBlockVector::iterator matches_itr = matches.begin(); matches_itr != matches.end(); ++matches_itr)
        {
            foundOverlap = false;
            int basePos = matches_itr->GetBasePos();
            for(CMatchBlockVector::iterator tidied_matches_itr = tidied_matches.begin(); tidied_matches_itr != tidied_matches.end(); ++tidied_matches_itr)
            {
                if (matches_itr->Overlaps(*tidied_matches_itr))
                {
                    foundOverlap = true;
                    break;
                }
            }
            if (!foundOverlap)
            //	break;
            //else
                tidied_matches.push_back(*matches_itr);
        }
    }
}
//  ===========================================================================
void TextCompare(const std::_tstring & base, const std::_tstring & comp, CLineVector & baseResult, CLineVector & compResult)
{
    if (base.length() == 0 && comp.length() == 0)
        return;

    CTable baseTable(base);
    CTable compTable(comp);
    CAnchors compare(baseTable, compTable, 1);
    compare.GetPaddedResults(baseResult, compResult);
}
//  ===========================================================================
}