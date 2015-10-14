#pragma once

#include "Global.h"
#include "SplitterWnd.h"
#include "GraphView2.h"

class CMyEdit : public WTL::CWindowImpl<CMyEdit, WTL::CEdit>,
    public WTL::CEditCommands<CMyEdit>
{
public:
    BEGIN_MSG_MAP(CMyEdit)
            // your handlers...
        CHAIN_MSG_MAP_ALT(WTL::CEditCommands<CMyEdit>, 1)
        DEFAULT_REFLECTION_HANDLER()
    END_MSG_MAP()
    // other stuff...
};

enum WALK
{
    WALK_UNKNOWN,
    WALK_FALSE,
    WALK_TRUE
};
typedef std::vector<unsigned int> UIntVectorT;
typedef std::map<unsigned int, HTREEITEM> UIntTreeItemMapT;
typedef std::vector<std::_tstring> StringVectorT;
typedef std::set<unsigned int> UIntSetT;
typedef std::pair<UIntSetT, WALK> UIntSetBoolPairT;
typedef std::map<unsigned int, UIntSetBoolPairT> UIntUIntSetBoolPairMapT;
typedef std::map<unsigned int, std::_tstring> UIntStringMapT;
typedef std::map<std::_tstring, unsigned int> StringUIntMapT;

class CDependency
{
protected:
    boost::filesystem::path m_envFolder;

    std::_tstring m_services;
    std::_tstring m_xml;
    std::_tstring m_emptyStr;
    UIntUIntSetBoolPairMapT m_attrGraph;
    mutable StringUIntMapT m_attrHashXXX;
    mutable UIntStringMapT m_hashAttrXXX;

public:
    unsigned int StringToHash(const std::_tstring & str) const;
    const TCHAR * HashToString(unsigned int hash) const;

public:
    CDependency();
    void SetEnvFolder(const boost::filesystem::path & p);
    void Clear();

    void SetServices(const std::_tstring & services);
    const TCHAR * GetServices() const;
    void Save() const;
    void Load();

    bool CacheExists() const;
    void SaveCache() const;
    void LoadCache();

    void SetXML(const std::_tstring & xml);
    bool HasXML() const;
    const TCHAR * GetXML() const;

    void ClearLinks(const std::_tstring & attribute);
    void InsertLink(const std::_tstring & attribute, const std::_tstring & dependantAttribute);

    bool ParseDependencies();

    void RefreshCrossreferences(WTL::CTreeViewCtrl & tree, const UIntSetT & attrs, const UIntSetT & services);

protected:
    void ClearInclusion();
    WALK CalcInclusion(WTL::CTreeViewCtrl & tree, int attrHash, UIntSetT & targets, UIntVectorT & ancestors, UIntTreeItemMapT & ancestorTreeItem);
    //bool RecursiveLoadTree(WTL::CTreeViewCtrl & tree, HTREEITEM parent, int attrHash, UIntSetT & targets, UIntVectorT & ancestors) const;
};

class CDependeesWnd : public CWindowImpl<CDependeesWnd>
{
protected:
    CMySplitterWindow<true> m_splitterH;
    CMySplitterWindow<false> m_splitterV;
    WTL::CPaneContainer m_paneAttrs;
    CMyEdit m_attrs;
    WTL::CPaneContainer m_paneServices;
    CMyEdit m_services;
    WTL::CPaneContainer m_paneCrossref;
    WTL::CTreeViewCtrl m_tree;

    //CMySplitterWindow<true> m_splitterH2;
    //WTL::CPaneContainer m_paneGraph;
    //CGraphView2 m_graph;

    CDependency m_dependency;

    CComPtr<IConfig> m_cfgRHS;
    
public:
    CDependeesWnd();
    void SetConfig(IConfig * cfgRHS);

    BOOL PreTranslateMessage(MSG* pMsg);

    void SetAttributes(const std::_tstring & attrs);
    void GetAttributes(std::_tstring & attributes, const std::_tstring & delim);
    bool HasAttributes();
    void SaveAttributes();
    void LoadAttributes();

    void SetServices(const std::_tstring & services);
    const TCHAR * GetServices(std::_tstring & services, const std::_tstring & delim) const;
    bool HasServices();

    bool SetDependencies(const std::_tstring & depends);
    bool HasDependency();
    void SaveDependencies();
    void LoadDependencies();

    void CalcCrossreferences();
    const TCHAR * GetCrossreferenced(std::_tstring & crossreferenced) const;
    bool HasCrossreferenced() const;

    BEGIN_MSG_MAP(CDiffView)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_SIZE(OnSize)

        COMMAND_CODE_HANDLER(EN_CHANGE, OnEnChangeEdit)

        CHAIN_COMMANDS_MEMBER(m_attrs)
        CHAIN_COMMANDS_MEMBER(m_services)
        REFLECT_NOTIFICATIONS()
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, CSize size);
    LRESULT OnEnChangeEdit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};
