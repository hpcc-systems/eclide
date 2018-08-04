#pragma once

#include "wlib.h"
#include <topology.h>
#include <workunit.h>
#include <repository.h>
#include <thread.h>
#include <CustomMessages.h>

enum UMCB
{
    UMCB_FIRST = CWM_LAST + 1,
    UMCB_POSTSTRINGINSERT, 
    UMCB_LAST
};

class CComboBoxEx2 : public CUnknown, private WTL::CComboBox
{
    typedef CComboBoxEx2 thisClass;
    typedef WTL::CComboBox baseClass;
protected:
    mutable boost::recursive_mutex m_mutex;

public:
    BEGIN_MSG_MAP(thisClass)
        MESSAGE_HANDLER(UMCB_POSTSTRINGINSERT, OnPostStringInsert);
    END_MSG_MAP()

    HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0, _U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
    {
        HWND retVal = baseClass::Create(hWndParent, rect, szWindowName, dwStyle, dwExStyle, MenuOrID, lpCreateParam);
        baseClass::LimitText(0);
        return retVal;
    }
    void Attach(HWND hWndNew)
    {
        baseClass::Attach(hWndNew);
    }
    baseClass& operator =(HWND hWnd)
    {
        return baseClass::operator =(hWnd);
    }
    int AddString(LPCTSTR lpszItem)
    {
        return baseClass::AddString(lpszItem);
    }
    int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
    {
        return baseClass::FindStringExact(nIndexStart, lpszFind);
    }
    int GetCurSel() const
    {
        return baseClass::GetCurSel();
    }
    int GetLBText(int nIndex, LPTSTR lpszText) const
    {
        return baseClass::GetLBText(nIndex, lpszText);
    }
    int GetLBText(int nIndex, _CSTRING_NS::CString& strText) const
    {
        return baseClass::GetLBText(nIndex, strText);
    }
    LRESULT OnPostStringInsert(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
    {
        ATLASSERT(lParam);
        bHandled = true;
        std::_tstring * str = (std::_tstring *)lParam;
        if (str)
        {
            InsertString(wParam, str->c_str());
            delete str;
        }
        return 0;
    }
    int PostInsertStringXXX(int nIndex, LPCTSTR lpszString)
    {
        std::_tstring * str = new std::_tstring();
        *str = lpszString;
        PostMessage(UMCB_POSTSTRINGINSERT, nIndex, (WPARAM)str); 
        return 0;
    }
    int InsertString(int nIndex, LPCTSTR lpszString)
    {
        //ATLASSERT(::IsWindow(m_hWnd));
        //return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
        if (IsWindow())
        {
            return baseClass::InsertString(nIndex, lpszString);
        }
        return 0;
    }
    void ResetContent()
    {
        baseClass::ResetContent();
    }
    int GetCount()
    {
        return baseClass::GetCount();
    }
    int SetCurSel(int nSelect)
    {
        return baseClass::SetCurSel(nSelect);
    }
    BOOL EnableWindow(BOOL bEnable = 1)
    {
        return baseClass::EnableWindow(bEnable);
    }
    void Clear()
    {
        baseClass::Clear();
    }
    void SetFont(HFONT hFont, BOOL bRedraw = TRUE) throw()
    {
        baseClass::SetFont(hFont, bRedraw);
    }
    operator HWND() const throw()
    { 
        return baseClass::operator HWND(); 
    }
    CWindow SetParent(HWND hWndNewParent) 
    {
        return baseClass::SetParent(hWndNewParent);
    }
    int SetDroppedWidth(UINT nWidth)
    {
        return baseClass::SetDroppedWidth(nWidth);
    }
    BOOL SetWindowPos(HWND hWndInsertAfter, LPCRECT lpRect, UINT nFlags) throw()
    {
        return baseClass::SetWindowPos(hWndInsertAfter, lpRect, nFlags);
    }
    BOOL GetWindowRect(LPRECT lpRect) const throw()
    {
        return baseClass::GetWindowRect(lpRect);
    }
    BOOL LimitText(int nMaxChars)
    {
        return baseClass::LimitText(nMaxChars);
    }
    void* GetItemDataPtr(int nIndex) const
    {
        return baseClass::GetItemDataPtr(nIndex);
    }
    int SetItemDataPtr(int nIndex, void* pData)
    {
        return baseClass::SetItemDataPtr(nIndex, pData);
    }
};

class WLIB_API CComboTopologyBase : public CComboBoxEx2
{
protected:
    CComboTopologyBase();

    CString m_defaultItem;

    virtual void SetIP(const TCHAR *ip);

public:
    virtual ~CComboTopologyBase();

    CString m_value;
    std::_tstring m_topoIP;

    virtual void Load(const TCHAR *ip=0) = 0;

    void ResetIfNewIP(const TCHAR *ip);
    const TCHAR* SelectDefault(const TCHAR *selection);
};


const TCHAR * const ALL_CLUSTERS = _T("All Clusters");

class WLIB_API CComboCluster : public CComboTopologyBase
{
protected:
    clib::recursive_mutex m_mutex;

public:
    CComboCluster();
    CComboCluster(bool includeAllClusters);

    virtual void Load(const TCHAR *ip=0);
    static void GetClusters(CComPtr<CComboCluster> self);
    void LoadClusters(Topology::IClusterVector & clusters);

    void RestrictType(const TCHAR *typeName);
    Topology::ICluster *GetSelectedCluster();
    void SelectDefault(const TCHAR *selection);

protected:
    bool m_includeAllClusters;
    CString m_restrictToType;
    Topology::IClusterVector m_clusters;
};

typedef std::pair<Topology::IEclServerAdapt, Topology::IClusterAdapt> QueueClusterPair;
typedef std::vector<QueueClusterPair> QueueClusterPairVector;
class WLIB_API CComboQueueCluster : public CComboTopologyBase
{
protected:
    QueueClusterPairVector m_queueClusters;

public:
    std::_tstring m_queue;
    std::_tstring m_cluster;

    CComboQueueCluster();

    virtual void Load(const TCHAR *ip=0);
    static void GetQueueClusters(CComPtr<CComboQueueCluster> self);
    void LoadQueueCluster(const QueueClusterPairVector & queueClusters);

    void SelectClusterQueue(const std::_tstring & queue, const std::_tstring & cluster);
    Topology::IEclServer *GetSelectedQueue();
    Topology::ICluster *GetSelectedCluster();
};

class WLIB_API CComboDropZone : public CComboTopologyBase
{
public:
    CComboDropZone();
    void Load(const TCHAR *ip=0);
    Topology::IDropZone *GetSelectedDropZone();

    bool SelectByMachineIP(const TCHAR *ip);
protected:
    Topology::IDropZoneVector m_dropZones;

};

class WLIB_API CComboState : public CComboBoxEx2
{
public:
    CString m_value;
    void Load();
    Dali::WUState GetState();
    Dali::WUState GetState(const CString& state);
    CComboState & operator = (HWND hWnd);
};

class WLIB_API CComboLabel : public CComboBoxEx2
{
protected:
    clib::recursive_mutex m_mutex;

public:
    CString m_value;
    void Load(const TCHAR *label);
    void Load(CComPtr<IRepository> rep, const TCHAR *label);
    static void GetLabels(CComPtr<CComboLabel> self);
    static void GetLabels2(CComPtr<CComboLabel> self, CComPtr<IRepository> rep);
    void LoadLabels(CRepLabelVector & labels);
    void SelectDefault(const TCHAR *selection);
    CComboLabel & operator = (HWND hWnd);
};

class WLIB_API CComboModule : public CComboBoxEx2
{
protected:
    clib::recursive_mutex m_mutex;
    CString m_value;

public:
    void Load(const TCHAR *label, bool includeBlank = true, bool includeReadOnly = true);
    static void GetModules(CComPtr<CComboModule> self, bool includeReadOnly);
    void LoadModules(IModuleVector & modules, bool includeReadOnly);

    void SelectDefault(const TCHAR *selection);
    CComboModule & operator = (HWND hWnd);
    const TCHAR * GetSelectedModule();
    IModule * GetSelectedIModule();
};


