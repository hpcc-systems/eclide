#include "StdAfx.h"
#include "..\en_us\resource.h"

#include "BookmarksView.h"
#include "EclCC.h"
#include "ecldlgbuilder.h"
#include "XmlWriter.h"
#include "mainfrm.h"

const TCHAR * const BOOKMARKS_FILE = _T("bookmarks.xml");
const TCHAR * const BOOKMARKS_FILE_LOCAL = _T("bookmarks_local.xml");
static const SectionLabelDefault GLOBAL_BOOKMARKS_SHOWMINE(SectionLabel(_T("Bookmarks"), _T("Mine")), true);
static const SectionLabelDefault GLOBAL_BOOKMARKS_SHOWTODOS(SectionLabel(_T("Bookmarks"), _T("Todos")), true);
static const SectionLabelDefault GLOBAL_BOOKMARKS_SHOWHACKS(SectionLabel(_T("Bookmarks"), _T("Hacks")), false);
static const SectionLabelDefault GLOBAL_BOOKMARKS_SORTCOLUMN(SectionLabel(_T("Bookmarks"), _T("SortColumn")), false);
static const SectionLabelDefault GLOBAL_BOOKMARKS_SORTASCENDING(SectionLabel(_T("Bookmarks"), _T("SortAscending")), false);

//  ===========================================================================

CBookmarksView::CBookmarksView()
{
    m_prevBookmarksMarker = NULL;
}

CBookmarksView::~CBookmarksView()
{
}

void CBookmarksView::Init(IBookmarksSlot *owner)
{
    m_Owner = owner;
}

BOOL CBookmarksView::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) && (pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
        return FALSE;

    HWND hWndCtl = ::GetFocus();
    if (IsChild(hWndCtl))
    {
        // find a direct child of the dialog from the window that has focus
        while (::GetParent(hWndCtl) != m_hWnd)
            hWndCtl = ::GetParent(hWndCtl);

        // give control a chance to translate this message
        if (::SendMessage(hWndCtl, WM_FORWARDMSG, 0, (LPARAM)pMsg) != 0)
            return TRUE;
    }

    return CWindow::IsDialogMessage(pMsg);
}

LRESULT CBookmarksView::OnInitDialog(HWND /*hWnd*/, LPARAM /*lParam*/)
{
    SetMsgHandled(false);
    DlgResize_Init(false, false);

    CRect rc(0, 0, 0, 0);
    CWindow wndPlaceholder = GetDlgItem(IDC_BOOKMARKS_PLACEHOLDER);
    wndPlaceholder.GetWindowRect(rc);
    ScreenToClient(rc);
    wndPlaceholder.DestroyWindow();

    const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    if (!m_list.Create(dwViewStyle, rc, CWnd::FromHandle(m_hWnd), IDC_BOOKMARKS_PLACEHOLDER))
    {
        TRACE0("Failed to create Class View\n");
        return -1;      // fail to create
    }
    m_list.SetOwner(CWnd::FromHandle(m_hWnd));

    m_list.m_sortedCol = 0;
    m_list.m_sortAscending = false;

    m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    m_list.EnableMultipleSort(true);

    int col = 0;
    m_list.InsertColumn(col++, _T("Line"));
    m_list.InsertColumn(col++, _T("Type"));
    m_list.InsertColumn(col++, _T("User"));
    m_list.InsertColumn(col++, _T("Path"));
    m_list.InsertColumn(col++, _T("Module"));
    m_list.InsertColumn(col++, _T("Attribute"));
    m_list.InsertColumn(col++, _T("Attr Type"));
    m_list.InsertColumn(col++, _T("Description"));

    for (int i = 0; i < col; ++i)
    {
        m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    }

    if (!m_listMaster.Create(dwViewStyle, rc, CWnd::FromHandle(m_hWnd), IDC_BOOKMARKS_PLACEHOLDER))
    {
        TRACE0("Failed to create Class View\n");
        return -1;      // fail to create
    }
    col = 0;
    m_listMaster.InsertColumn(col++, _T("Line"));
    m_listMaster.InsertColumn(col++, _T("Type"));
    m_listMaster.InsertColumn(col++, _T("User"));
    m_listMaster.InsertColumn(col++, _T("ID"));
    m_listMaster.InsertColumn(col++, _T("Module"));
    m_listMaster.InsertColumn(col++, _T("Attribute"));
    m_listMaster.InsertColumn(col++, _T("Attr Type"));
    m_listMaster.InsertColumn(col++, _T("Description"));

    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

    m_checkMine = iniFile->Get(GLOBAL_BOOKMARKS_SHOWMINE);
    m_checkTodos = iniFile->Get(GLOBAL_BOOKMARKS_SHOWTODOS);
    m_checkHacks = iniFile->Get(GLOBAL_BOOKMARKS_SHOWHACKS);

    m_list.m_sortedCol = iniFile->Get(GLOBAL_BOOKMARKS_SORTCOLUMN);
    m_list.m_sortAscending = iniFile->Get(GLOBAL_BOOKMARKS_SORTASCENDING);

    m_serverType = BM_SERVER_TYPE_UNKNOWN;

    DoDataExchange();

    return 0;
}

void CBookmarksView::OnDestroy()
{
    DoDataExchange(true);

    boost::filesystem::path iniPath;
    GetIniPath(iniPath);
    CComPtr<IConfig> iniFile = CreateIConfig(QUERYBUILDER_INI, iniPath);

    iniFile->Set(GLOBAL_BOOKMARKS_SHOWMINE, m_checkMine);
    iniFile->Set(GLOBAL_BOOKMARKS_SHOWTODOS, m_checkTodos);
    iniFile->Set(GLOBAL_BOOKMARKS_SHOWHACKS, m_checkHacks);

    iniFile->Set(GLOBAL_BOOKMARKS_SORTCOLUMN, m_list.m_sortedCol);
    iniFile->Set(GLOBAL_BOOKMARKS_SORTASCENDING, m_list.m_sortAscending);

    m_list.DeleteAllItems();
    m_listMaster.DeleteAllItems();

    SetMsgHandled(false);
}

void CBookmarksView::OnContextMenu(HWND /*phWnd*/, CPoint pPoint)
{
    WTL::CMenu m;
    m.LoadMenu(IDR_POPUP_BOOKMARKS);
    m.EnableMenuItem(ID_BOOKMARKS_OPEN, m_list.GetItemCount() == 0);
    m.EnableMenuItem(ID_BOOKMARKS_SAVE, m_list.GetItemCount() == 0);
    m.EnableMenuItem(ID_BOOKMARKS_CLEAR, m_list.GetItemCount() == 0);

    unsigned int id = m.GetSubMenu(0).TrackPopupMenuEx(TPM_RETURNCMD, pPoint.x, pPoint.y, m_hWnd, NULL);
    switch (id)
    {
    case ID_BOOKMARKS_OPEN:
    {
        OnOpen();
    }
    break;
    case ID_BOOKMARKS_SAVE:
    {
        OnSaveFile();
    }
    break;
    case ID_BOOKMARKS_LOAD:
    {
        OnLoadFile();
    }
    break;
    case ID_BOOKMARKS_LOADMERGE:
    {
        OnLoadFile(true);
    }
    break;
    case ID_BOOKMARKS_DELETEFROMLIST:
    {
        OnDeleteLines();
    }
    break;
    case ID_BOOKMARKS_CLEAR:
    {
        if (MessageBox(CLEAR_BOOKMARKS_MSG, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES) {
            return;
        }
        OnClear(NULL, NULL, NULL);
    }
    break;
    }
}

LRESULT CBookmarksView::OnLvnItemchangedListHistory(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    int d;
    if (pNMLV->uChanged & LVIF_STATE)
        d = m_list.GetSelectionMark();

    return 0;
}

LRESULT CBookmarksView::OnLvnItemDelete(int /*idCtrl*/, LPNMHDR pNMHDR, BOOL& /*bHandled*/)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    m_list.OnDeleteItem(pNMLV->iItem);
    return 0;
}

void CBookmarksView::OnButtonSetBookmarks()
{
}

void CBookmarksView::OnUpdateButtonSetBookmarks(CCmdUI* pCmdUI)
{
}

void CBookmarksView::OnButtonRemoveBookmarks()
{
}

void CBookmarksView::OnUpdateButtonRemoveBookmarks(CCmdUI* pCmdUI)
{
}

void CBookmarksView::OnDeleteLines()
{
    if (MessageBox(DELETE_BOOKMARKS_MSG, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
    {
        return;
    }
    SetMarks(false);

    POSITION pos = m_list.GetFirstSelectedItemPosition();
    while (pos)
    {
        int sel = m_list.GetNextSelectedItem(pos);
        m_list.SetMark(sel, true);
    }
    DeleteMarkedBookmarks(true);
}

void CBookmarksView::OnOpen()
{
    std::vector<std::_tstring> opens;
    std::vector<int> rows;

    POSITION pos = m_list.GetFirstSelectedItemPosition();

    while (pos)
    {
        int sel = m_list.GetNextSelectedItem(pos);
        std::_tstring module = m_list.GetItemText(sel, 3);
        std::_tstring attribute = m_list.GetItemText(sel, 4);
        std::_tstring unique = module + attribute;

        bool found = false;
        for (std::vector<std::_tstring>::iterator it = opens.begin(); it != opens.end(); ++it)
        {
            if (*it == unique)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            opens.push_back(unique);
            rows.push_back(sel);
        }
    }

    for (std::vector<int>::iterator it = rows.begin(); it != rows.end(); ++it)
    {
        int row = *it;
        int col = 0;
        std::_tstring id = _T("");
        std::_tstring line = m_list.GetItemText(row, col);
        col = 3;
        std::_tstring module = m_list.GetItemText(row, col++);
        std::_tstring attribute = m_list.GetItemText(row, col++);
        std::_tstring attributeType = m_list.GetItemText(row, col++);
        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_list.GetItemData(row));

        int len = m_list.m_bookmarks[data->bookmarkType].length();

        OpenAttribute(line, data->column + 1, len, module, attribute, attributeType);
    }
}

void CBookmarksView::OnUpdateOpen(CCmdUI* pCmdUI)
{
    if (m_list.GetSelectionMark() >= 0)
    {
        pCmdUI->Enable();
    }
}

LRESULT CBookmarksView::OnNmDblClk(int idCtrl, LPNMHDR pNMHDR, BOOL& bHandled)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    int col = 0;
    std::_tstring id = _T("");

    std::_tstring line = m_list.GetItemText(pNMIA->iItem, 0);
    BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_list.GetItemData(pNMIA->iItem));
    col = 2;
    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
    {
        id = m_list.GetItemText(pNMIA->iItem, col++);
    }
    else
    {
        col++;
    }
    std::_tstring module = m_list.GetItemText(pNMIA->iItem, col++);
    std::_tstring attribute = m_list.GetItemText(pNMIA->iItem, col++);
    std::_tstring attributeType = m_list.GetItemText(pNMIA->iItem, col++);

    if (id.length())
    {
        if (id[1] == ':' && id[2] == '\\')
        {
            int row = boost::lexical_cast<int>(line);
            int len = m_listMaster.m_bookmarks[data->bookmarkType].length();
            GetIMainFrame()->DoFileOpen(id.c_str(), row, data->column + 1, len);
        }
    }
    else
    {
        int len = m_list.m_bookmarks[data->bookmarkType].length();
        OpenAttribute(line, data->column + 1, len, module, attribute, attributeType);
    }

    return 0;
}

CSortMFCListCtrl *CBookmarksView::GetListCtrl()
{
    return &m_list;
}

void CBookmarksView::OpenAttribute(std::_tstring line, int column, int len, std::_tstring module, std::_tstring attribute, std::_tstring attributeType)
{
    std::_tstring attrType(attributeType);
    IAttributeType *type = CreateIAttributeType(attrType);

    int row = boost::lexical_cast<int>(line);
    std::wstring m(module);
    std::wstring a(attribute);

    GetIMainFrame()->OpenAttribute(m, a, type, row, column, len);
}

void CBookmarksView::ParseBookmarks(IAttribute *attribute)
{
    std::_tstring id = _T("");
    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
    {
        id = attribute->GetPath();
    }
    
    std::_tstring ecl = attribute->GetText(false, true);
    std::_tstring module = attribute->GetModuleQualifiedLabel();
    std::_tstring attributeName = attribute->GetLabel();
    std::_tstring user = attribute->GetModifiedBy();
    CComPtr<IAttributeType> attrType = attribute->GetType();

    if (ecl.length() > 0) {
        ParseBookmarksEcl(ecl, user, id, module, attributeName, attrType);
    }
}

void CBookmarksView::SetMarks(bool val) {
    for (int i = 0; i < m_list.GetItemCount(); ++i)
    {
        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_list.GetItemData(i));
        data->marked = val;
    }
}

void CBookmarksView::DeleteMarkedBookmarks(bool val) {
    for (int i = m_list.GetItemCount() - 1; i >= 0; --i)
    {
        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_list.GetItemData(i));
        if (data)
        {
            if (data->marked == val) {
                m_list.DeleteItem(i);
            }
        }
    }
}

void CBookmarksView::SetMarks(std::_tstring inModule, std::_tstring inAttributeName,  bool val)
{
    int col = 0;
    for (int i = 0; i < m_listMaster.GetItemCount(); ++i)
    {
        col = 3;
        std::_tstring module = m_listMaster.GetItemText(i, col++);
        std::_tstring attributeName = m_listMaster.GetItemText(i, col++);

        if (module == inModule && attributeName == inAttributeName)
        {
            BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_listMaster.GetItemData(i));
            data->marked = val;
        }
    }
}

void CBookmarksView::DeleteMarkedBookmarks(std::_tstring inModule, std::_tstring inAttributeName, bool val)
{
    int col = 0;
    for (int i = m_listMaster.GetItemCount() - 1; i >= 0; --i)
    {
        col = 3;
        std::_tstring module = m_listMaster.GetItemText(i, col++);
        std::_tstring attributeName = m_listMaster.GetItemText(i, col++);

        BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_listMaster.GetItemData(i));
        if (module == inModule && attributeName == inAttributeName && data->marked == val)
        {
            m_listMaster.DeleteItem(i);
        }
    }
}

static inline std::_tstring &ltrim(std::_tstring &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static inline std::_tstring &rtrim(std::_tstring &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

static inline std::_tstring &trim(std::_tstring &s)
{
    return ltrim(rtrim(s));
}

void CBookmarksView::ParseBookmarksEcl(std::_tstring ecl, std::_tstring user, std::_tstring id, std::_tstring inModule, std::_tstring inAttributeName, IAttributeType *attrType)
{
    int i = 0;
    int col = 0;
    int row = 0;

    std::_tstring bookmark = _T("");

    SetMarks(inModule, inAttributeName, false);

    for (int b = 0; b < (int)m_listMaster.m_bookmarks.size(); b++)
    {
        bookmark = m_listMaster.m_bookmarks[b];
        if (bookmark.length() == 0) {
            continue;
        }

        int n = static_cast<int>(ecl.find(bookmark, i));

        if (n > 0) {
            std::_tstring line;
            int index = 0;
            bool found = false;

            typedef std::vector<std::_tstring> split_vector_type;
            split_vector_type lines;
            boost::algorithm::split(lines, ecl, boost::algorithm::is_any_of(_T("\n")), boost::algorithm::token_compress_on);

            for (split_vector_type::const_iterator itr = lines.begin(); itr != lines.end(); ++itr)
            {
                line = itr[0];
                ++index;

                n = static_cast<int>(line.find(_T("//"), 0));
                if (n < 0)
                {
                    continue;
                }

                n = static_cast<int>(line.find(bookmark, 0));

                if (n >= 0) {

                    std::wostringstream ss;
                    ss << (index);
                    std::_tstring nStr = ss.str();
                    found = false;

                    for (int i = 0; i < m_listMaster.GetItemCount(); ++i)
                    {
                        std::_tstring liner = m_listMaster.GetItemText(i, 0);
                        col = 3;
                        std::_tstring module = m_listMaster.GetItemText(i, col++);
                        std::_tstring attributeName = m_listMaster.GetItemText(i, col++);

                        if (liner == nStr && module == inModule && attributeName == inAttributeName)
                        {
                            found = true;
                            BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_listMaster.GetItemData(i));
                            data->marked = true;
                            break;
                        }
                    }

                    if (!found) {
                        col = 0;
                        row = m_listMaster.InsertItem(col++, nStr.c_str());
                        m_listMaster.SetItemText(row, col++, bookmark.c_str());
                        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                        {
                            m_listMaster.SetItemText(row, col++, id.c_str());
                        }
                        else
                        {
                            m_listMaster.SetItemText(row, col++, user.c_str());
                        }
                        m_listMaster.SetItemText(row, col++, inModule.c_str());
                        m_listMaster.SetItemText(row, col++, inAttributeName.c_str());
                        m_listMaster.SetItemText(row, col++, attrType->GetRepositoryCode());
                        m_listMaster.SetItemText(row, col++, trim(line.substr(n + m_listMaster.m_bookmarks[b].length())).c_str());
                        BookmarkItemData *data = new BookmarkItemData;
                        data->marked = true;
                        data->bookmarkType = (BM_TYPE)b;
                        data->column = n;
                        m_listMaster.SetItemData(row, (DWORD_PTR)data);
                    }
                }
            }
        }
    }

    DeleteMarkedBookmarks(inModule, inAttributeName,false);

    for (int i = 0; i < col; ++i)
    {
        m_listMaster.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    }

    DoRefresh(NULL, 0);
}

void CBookmarksView::OnUpdateSaveFile(CCmdUI* pCmdUI)
{
    if (m_list.GetItemCount() > 0) {
        pCmdUI->Enable();
    }
}

boost::filesystem::path CBookmarksView::BookmarksFilePath()
{
    boost::filesystem::path fullpath;
    GetUserFolder(fullpath, static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))));
    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
    {
        return fullpath / BOOKMARKS_FILE_LOCAL;
    }
    return fullpath / BOOKMARKS_FILE;
}

void CBookmarksView::OnSaveFile()
{
    if (m_list.GetItemCount() > 0)
    {
        if (MessageBox(SAVE_BOOKMARKS_MSG, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES) {
            return;
        }

        CComPtr<IXMLWriter> writer = CreateIXMLWriter(true);

        writer->PushElement(_T("bookmarks"));

        std::_tstring bookmarks;
        std::_tstring user;
        std::_tstring id;
        int col = 0;

        for (int i = 0; i < m_list.GetItemCount(); ++i)
        {
            col = 0;
            std::_tstring line = m_list.GetItemText(i, col++);
            std::_tstring type = m_list.GetItemText(i, col++);
            if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            {
                id = m_list.GetItemText(i, col++);
            }
            else
            {
                user = m_list.GetItemText(i, col++);
            }
            std::_tstring module = m_list.GetItemText(i, col++);
            std::_tstring attribute = m_list.GetItemText(i, col++);
            std::_tstring attributeType = m_list.GetItemText(i, col++);
            std::_tstring description = m_list.GetItemText(i, col++);

            BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_list.GetItemData(i));

            std::wstringstream col;
            col << (data->column);
            std::_tstring column = col.str();

            writer->PushElement(_T("bookmark"));
            writer->PushElement(_T("line"), line);
            writer->PopElement();
            writer->PushElement(_T("type"), type);
            writer->PopElement();
            writer->PushElement(_T("column"), column);
            writer->PopElement();
            if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
            {
                writer->PushElement(_T("id"), id);
                writer->PopElement();
            }
            else
            {
                writer->PushElement(_T("user"), user);
                writer->PopElement();
            }
            writer->PushElement(_T("module"), module);
            writer->PopElement();
            writer->PushElement(_T("attribute"), attribute);
            writer->PopElement();
            writer->PushElement(_T("attrtype"), attributeType);
            writer->PopElement();
            writer->PushElement(_T("description"), description);
            writer->PopElement();
            writer->PopElement(); // bookmark
        }

        writer->PopElement();
        writer->EndDocument(bookmarks);

        CUnicodeFile file;
        file.Create(BookmarksFilePath(), GENERIC_WRITE, CREATE_ALWAYS);
        if (file.IsOpen())
        {
            file.Write(bookmarks.c_str());
            file.Close();
        }
    }
}

void CBookmarksView::OnUpdateLoadFile(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();
}

std::_tstring CBookmarksView::FindTag(std::_tstring str, std::_tstring tag, int &index, bool zeroIndex) {
    std::_tstring tagEnd = _T("</") + tag + _T(">");
    std::_tstring foundStr = _T("");
    tag = _T("<") + tag + _T(">");

    int n = static_cast<int>(str.find(tag, index));
    if (n >= 0)
    {
        int tagLen = tag.length();
        int indexEnd = static_cast<int>(str.find(tagEnd, index));
        foundStr = str.substr(n + tagLen, indexEnd - n - tagLen);
        index = indexEnd + tagEnd.length() + 1;
    }
    else
    {
        if (zeroIndex) index = -1;
        foundStr = _T("");
    }
    return foundStr;
}

void CBookmarksView::OnLoadFile(bool mergeFlag)
{
    if (m_list.GetItemCount() > 0)
    {
        if (mergeFlag) {
            if (MessageBox(LOAD_MERGE_BOOKMARKS_MSG, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
            {
                return;
            }
        }
        else if (MessageBox(LOAD_BOOKMARKS_MSG, _T("Warning"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) != IDYES)
        {
            return;
        }
    }

    CUnicodeFile file;
    std::_tstring xmlStr;
    file.Open(BookmarksFilePath());
    if (file.IsOpen())
    {
        file.Read(xmlStr);
        file.Close();

        BOOL bResult = FALSE;
        std::_tstring bookmarks;
        int index = 0;

        bookmarks = FindTag(xmlStr, _T("bookmarks"), index);

        if (index >= 0)
        {
            index = 0;
            int col = 0;

            if (!mergeFlag)
            {
                m_listMaster.DeleteAllItems();
            }

            std::_tstring bookmark, line, type, user, lineNum, column, id, module, attribute, attributeType, description;

            while (index >= 0)
            {
                bookmark = FindTag(bookmarks, _T("bookmark"), index, true);

                if (index >= 0)
                {
                    int bookmarkIndex = 0;

                    lineNum = FindTag(bookmark, _T("line"), bookmarkIndex);
                    type = FindTag(bookmark, _T("type"), bookmarkIndex);
                    column = FindTag(bookmark, _T("column"), bookmarkIndex);
                    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                    {
                        id = FindTag(bookmark, _T("id"), bookmarkIndex);
                    }
                    else
                    {
                        user = FindTag(bookmark, _T("user"), bookmarkIndex);
                    }
                    module = FindTag(bookmark, _T("module"), bookmarkIndex);
                    attribute = FindTag(bookmark, _T("attribute"), bookmarkIndex);
                    attributeType = FindTag(bookmark, _T("attrtype"), bookmarkIndex);
                    description = FindTag(bookmark, _T("description"), bookmarkIndex);

                    bool foundDupe = false;
                    for (int i = 0; i < m_listMaster.GetItemCount(); ++i)
                    {
                        std::_tstring lineDupe = m_listMaster.GetItemText(i, 0);
                        std::_tstring idDupe = _T("");
                        std::_tstring userDupe = _T("");
                        col = 3;
                        if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                        {
                            idDupe = m_listMaster.GetItemText(i, col++);
                        }
                        else
                        {
                            userDupe = m_listMaster.GetItemText(i, col++);
                        }
                        std::_tstring moduleDupe = m_listMaster.GetItemText(i, col++);
                        std::_tstring attributeDupe = m_listMaster.GetItemText(i, col++);

                        if (lineDupe == lineNum && (idDupe == id || userDupe == user) && moduleDupe == module && attributeDupe == attribute)
                        {
                            foundDupe = true;
                        }
                    }

                    if (!foundDupe) {
                        if (lineNum.length() > 0 && attribute.length() > 0 && description.length() > 0)
                        {
                            int row = 0;
                            col = 0;
                            row = m_listMaster.InsertItem(col++, lineNum.c_str());
                            m_listMaster.SetItemText(row, col++, type.c_str());
                            if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                            {
                                m_listMaster.SetItemText(row, col++, id.c_str());
                            }
                            else
                            {
                                m_listMaster.SetItemText(row, col++, user.c_str());
                            }
                            m_listMaster.SetItemText(row, col++, module.c_str());
                            m_listMaster.SetItemText(row, col++, attribute.c_str());
                            m_listMaster.SetItemText(row, col++, attributeType.c_str());
                            m_listMaster.SetItemText(row, col++, description.c_str());
                            BookmarkItemData *data = new BookmarkItemData;
                            data->marked = true;
                            data->bookmarkType = m_listMaster.StringToType(type);
                            data->column = boost::lexical_cast<int>(column);
                            m_listMaster.SetItemData(row, (DWORD_PTR)data);
                        }
                    }
                }
            }

            for (int i = 0; i < col; ++i) {
                m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
            }
        }

        DoRefresh(NULL, 0);
    }
    else
    {
        MessageBox(LOAD_UNFOUND_BOOKMARKS_MSG, _T("Warning"));
    }
}

LRESULT CBookmarksView::OnClear(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoDataExchange();
    m_list.DeleteAllItems();
    m_listMaster.DeleteAllItems();
    m_prevBookmarksMarker = NULL;
    return 0;
}

LRESULT CBookmarksView::OnReset(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    DoDataExchange();
    m_list.DeleteAllItems();
    m_listMaster.DeleteAllItems();
    m_prevBookmarksMarker = NULL;
    return 0;
}

LRESULT CBookmarksView::OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam)
{
    DoRefresh(reinterpret_cast<ISciBookmarksMarker *>(wParam), lParam);
    return 0;
}

LRESULT CBookmarksView::OnBnClickedCheckFilter(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    DoDataExchange(true);

    DoRefresh(m_prevBookmarksMarker, 0);
    return 0;
}

void CBookmarksView::DoRefresh(ISciBookmarksMarker *bookmarks, int nSel)
{
    if (m_serverType == BM_SERVER_TYPE_UNKNOWN)
    {
        if (IsLocalRepositoryEnabled() != TRI_BOOL_TRUE)
        {
            VERIFY(m_list.SetColumnWidth(3, 0));
            m_list.DeleteColumn(3);
            m_serverType = BM_SERVER_TYPE_LOCAL;
        }
        else
        {
            VERIFY(m_list.SetColumnWidth(2, 0));
            m_list.DeleteColumn(2);
            m_serverType = BM_SERVER_TYPE_REMOTE;
        }
    }

    DoDataExchange(true);
    m_prevBookmarksMarker = bookmarks;

    if (nSel >= 0)
    {
        int mineCount = 0;
        int todoCount = 0;
        int hackCount = 0;
        int col = 0;

        m_list.DeleteAllItems();

        if (m_listMaster.GetItemCount() > 0)
        {
            std::_tstring user;
            std::_tstring id;

            for (int i = 0; i < m_listMaster.GetItemCount(); ++i)
            {
                BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_listMaster.GetItemData(i));

                if (data->bookmarkType == BM_TYPE::BM_MINE) {
                    mineCount++;
                }
                else if (data->bookmarkType == BM_TYPE::BM_TODO) {
                    todoCount++;
                }
                else if (data->bookmarkType == BM_TYPE::BM_HACK) {
                    hackCount++;
                }

                if ((!m_checkMine && !m_checkTodos && !m_checkHacks)
                    || (m_checkMine && BM_TYPE::BM_MINE == data->bookmarkType)
                    || (m_checkTodos && BM_TYPE::BM_TODO == data->bookmarkType)
                    || (m_checkHacks && BM_TYPE::BM_HACK == data->bookmarkType)
                    ) {

                    col = 0;
                    std::_tstring line = m_listMaster.GetItemText(i, col++);
                    std::_tstring type = m_listMaster.GetItemText(i, col++);
                    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                    {
                        id = m_listMaster.GetItemText(i, col++);
                    }
                    else
                    {
                        user = m_listMaster.GetItemText(i, col++);
                    }
                    std::_tstring module = m_listMaster.GetItemText(i, col++);
                    std::_tstring attribute = m_listMaster.GetItemText(i, col++);
                    std::_tstring attributeType = m_listMaster.GetItemText(i, col++);
                    std::_tstring description = m_listMaster.GetItemText(i, col++);
                    BookmarkItemData *data = reinterpret_cast<BookmarkItemData *>(m_listMaster.GetItemData(i));

                    col = 0;

                    int row = m_list.InsertItem(col++, line.c_str());
                    m_list.SetItemText(row, col++, type.c_str());
                    if (IsLocalRepositoryEnabled() == TRI_BOOL_TRUE)
                    {
                        m_list.SetItemText(row, col++, id.c_str());
                    }
                    else
                    {
                        m_list.SetItemText(row, col++, user.c_str());
                    }
                    m_list.SetItemText(row, col++, module.c_str());
                    m_list.SetItemText(row, col++, attribute.c_str());
                    m_list.SetItemText(row, col++, attributeType.c_str());
                    m_list.SetItemText(row, col++, description.c_str());
                    BookmarkItemData *d = new BookmarkItemData;
                    d->marked = data->marked;
                    d->bookmarkType = data->bookmarkType;
                    d->column = data->column;
                    m_list.SetItemData(row, (DWORD_PTR)d);
                }
            }
        }

        for (int i = 0; i < col; ++i) {
            m_list.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
        }

        ::SetWindowText(GetDlgItem(IDC_CHECK_MINE), (boost::_tformat(_T("%1% [BM]")) % mineCount).str().c_str());
        ::SetWindowText(GetDlgItem(IDC_CHECK_TODOS), (boost::_tformat(_T("%1% TODO:")) % todoCount).str().c_str());
        ::SetWindowText(GetDlgItem(IDC_CHECK_HACKS), (boost::_tformat(_T("%1% HACK:")) % hackCount).str().c_str());

        m_list.DoSort(m_list.m_sortedCol, m_list.m_sortAscending);
    }
}
