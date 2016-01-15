#pragma once

#include "clib.h"
#include "wlib.h"
#include "workunit.h"
#include "attribute.h"

enum ImageIDS
{
    IID_FOLDER_CLOSED = 0,
    IID_FOLDER_OPEN,
    IID_DOCUMENT,
    IID_DOCUMENT_LOCKED,
    IID_DOCUMENT_LOCKED_OTHER,
    IID_SANDBOXED,
    IID_SANDBOXED_LOCKED,
    IID_SANDBOXED_LOCKED_OTHER,
    IID_DOCUMENT_HISTORY,
    IID_DOCUMENT_PLACEHOLDER,
    IID_DOCUMENT_BLANK,
    IID_PLUGINFOLDER_CLOSED,
    IID_PLUGINFOLDER_OPEN,
    IID_PLUGINFOLDER_UNMATCHED,
    IID_PLUGINFOLDER_OPEN_UNMATCHED,
    IID_DOCUMENT_SYNTAXPENDING,
    IID_DOCUMENT_SYNTAXPASS,
    IID_DOCUMENT_SYNTAXWARN,
    IID_DOCUMENT_SYNTAXFAIL,
    IID_DOCUMENT_FOLLOWUP,
    IID_DOCUMENT_FOLLOWUPLOCKED,
    IID_FOLDER_UNMATCHED,
    IID_FOLDER_MISSING,
    IID_DOCUMENT_UNMATCHED,
    IID_DOCUMENT_MISSING,
    IID_FOLDER_PLACEHOLDER,
    IID_TRASH_EMPTY,
    IID_TRASH_FULL
};

enum StateIDS
{
    SID_NONE = 0,
    SID_GREENTICK,
    SID_GREENTICKWARNING,
    SID_WARNING,
    SID_PENCIL,
    SID_LOCK,
    SID_REDFLAG,
    SID_BLANK,
    SID_REDHALT,
    SID_REDSTOP,
    SID_REDX,
    SID_GREENARROW,
    SID_REDARROW,
    SID_BLUEARROW,
    SID_GREENCIRCLE,
    SID_BLUECIRCLE,
    SID_CLOCK,
    SID_LAST
};

WLIB_API StateIDS GetStateIcon(Dali::WUState state);
WLIB_API StateIDS GetStateIcon(ATTRSTATE state);

//  ===========================================================================
class WLIB_API __declspec(uuid("8175886e-3d91-43c5-b365-15fdc70b6fb9")) CTreeNode : public WTL::CTreeItem, public CUnknown
{
public:
    HTREEITEM InsertBelow(WTL::CTreeViewCtrlEx &tc, HTREEITEM hParent, bool bLast=true);
    HTREEITEM Insert(WTL::CTreeViewCtrlEx &tc, HTREEITEM hParent, HTREEITEM hAfter);

    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(CTreeNode)
    END_CUNKNOWN(CUnknown)

    CTreeNode();
    ~CTreeNode();

    void GetDispInfo(TVITEM & item);
    CTreeNode *GetParentNode() const;
    CTreeNode *GetChildNode() const;
    CTreeNode *GetNextVisibleItem() const;
    CTreeNode *GetNextSiblingItem() const;
    virtual int GetDispChildren();
    virtual const TCHAR * GetID() = 0;
    virtual const TCHAR * GetDispText() = 0;
    virtual int GetDispImage();
    virtual int GetDispSelectedImage();
    virtual void ItemExpanding(){}
    virtual void OnDblclk(const WTL::CTreeItem &ti);
    virtual void OnContextMenu(WTL::CTreeViewCtrlEx &tc, CPoint &point);
    virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
    virtual void Refresh();
    virtual bool CanEdit();
    virtual bool Rename(const std::_tstring &txt);
    virtual const TCHAR * GetToolTip(std::_tstring & result);
    void ReplaceWith(CTreeNode *newNode);
    unsigned int GetDepth() const;
    bool IsExpanded();
    bool IsSelected();
};

typedef StlLinked<CTreeNode> CTreeNodeAdapt;
typedef std::vector<CTreeNodeAdapt> CTreeNodeVector;

namespace TreeNode
{
//  CTreeNode Helper Functions  ---
WLIB_API CTreeNode * Locate(const std::_tstring & qualifiedLabel, CTreeNode * startingNode);
WLIB_API bool Select(const std::_tstring & qualifiedLabel, CTreeNode * startingNode, bool bExpand = false);
WLIB_API bool RefreshChildren(const std::_tstring & qualifiedLabel, CTreeNode * startingNode);
WLIB_API int GetNodeIndex(CTreeNode *node);
}


