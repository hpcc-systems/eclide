#pragma once

#include "TreePairNode.h"
#include "Repository.h"

enum THREESTATE
{
    THREESTATE_UNKNOWN = -1,
    THREESTATE_UNCHECKED,
    THREESTATE_CHECKED,
    THREESTATE_PARTIALCHECKED,
    THREESTATE_RADIO_UNCHECKED,
    THREESTATE_RADIO_CHECKED,
    THREESTATE_BLANK,
    THREESTATE_BUSY_UNCHECKED,
    THREESTATE_BUSY_CHECKED,
    THREESTATE_BUSY_PARTIALCHECKED,
    THREESTATE_BUSY_RADIO_UNCHECKED,
    THREESTATE_BUSY_RADIO_CHECKED,
    THREESTATE_LAST
};

struct IAttributeInfo
{
    int m_version;
    bool m_checked;
    CComPtr<IAttributeHistory> m_history;
};
typedef std::map<IAttributeAdapt, IAttributeInfo> AttributeStateMap;

//struct IAttributeSelection
//{
//	IAttributeHistoryAdapt m_attr;
//	std::_tstring m_moduleLabel;
//	std::_tstring m_attrLabel;
//	int m_version;
//	std::_tstring m_modifiedDate;
//	std::_tstring m_modifiedBy;
//};
//typedef std::vector<IAttributeSelection> IAttributeSelectionVector;

class CSelection
{
protected:
    WTL::CTreeViewCtrlEx * m_tree;
    AttributeStateMap m_attrs;

public:
    CSelection(WTL::CTreeViewCtrlEx * tree);
    ~CSelection();

    void Clear();

    THREESTATE CalcWorkspaceState(IWorkspace * workspace) const;
    THREESTATE CalcModuleState(IModule * module) const;
    THREESTATE CalcAttributeState(IAttribute * attribute) const;
    THREESTATE CalcAttributeHistoryState(IAttribute * attribute, int version) const;

    void InitState(CTreeNode * item, THREESTATE knownState = THREESTATE_UNKNOWN);
    void ItemClicked(CTreeNode * item, IAttributeVector * attrs, IAttributeVector * dependants);
    void SetState(IAttribute * attr, bool checked);

    void Refresh(CAttributePairNode * node);

    bool HasSelection() const;
    int GetSelection(IRepository * rep, IWorkspaceVector & workspaces, IAttributeHistoryVector & attrs) const;
    void SetSelection(IAttributeVector & attrs, bool checked);
    void SetSelection(CTreeNode * item, IAttributeVector & attrs, bool checked);
    int GetSelection(std::_tstring & attrs) const;

    // checkboxes only
    THREESTATE GetCheckThreeState(HTREEITEM hItem) const;
    BOOL SetCheckThreeState(HTREEITEM hItem, THREESTATE state);
};
