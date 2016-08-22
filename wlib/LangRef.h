#pragma once

#include "wlib.h"
#include "StringSet.h"
#include "AttributeType.h"

__interface ILangRef : public IUnknown
{
    void Save();
    void Restore();
    void RestoreDefaults();

    int GetLexerType();
    int GetLangCatCount();
    int GetLangNameCount(int cat) const;
    const TCHAR * GetLangName(int cat, int row) const;
    int GetLangNameCatID(int cat, int row) const;
    CString & GetLangNames(int cat, CString &result);
    bool GetLangNamesAutoC(int cat, StdStringVector &set);
    bool GetAutoC(const std::_tstring & partialLabel, StdStringVector &set);	
    const TCHAR * GetLangTooltip(const std::_tstring &label);
    bool IsComment(int cat);

    int GetColorRowCount() const;
    int GetColorCatID(int row) const;
    int GetColorRow(int catID) const;
    const TCHAR * GetColorName(int catID) const;

    int GetColorFore(int catID) const;
    void SetColorFore(int catID, int color);
    int GetColorBack(int catID) const;
    void SetColorBack(int catID, int color);
    const TCHAR * GetFontName(int catID) const;
    void SetFontName(int catID, const std::_tstring & val);
    int GetFontSize(int catID) const;
    void SetFontSize(int catID, int size);
    bool GetFontBold(int catID) const;
    void SetFontBold(int catID, bool bold);
};

WLIB_API ILangRef * CreateLangRef(IAttributeType * type);
WLIB_API ILangRef * CreateLangRef(std::_tstring code, IAttributeType * type);
//  One of usage to upgrade XML file!
WLIB_API void ExportLangRef();
WLIB_API void ImportLangRef();
