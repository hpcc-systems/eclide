#pragma once

#include "RecursiveMutex.h"
#include "Attribute.h"

class COMMS_API CDiskMonitor : public clib::CLockableUnknown
{
protected:
    boost::filesystem::wpath m_folder;
    boost::filesystem::wpath m_file;

    bool m_doMonitor;
    std::set<boost::filesystem::path> m_modified;

    static void thread_MonitorFolder(CComPtr<CDiskMonitor> self);

protected:
    void monitor(const boost::filesystem::path & path);
    void modified(const boost::filesystem::path & path);
    void broadcast();
    virtual void notify();

public:
    CDiskMonitor();
    ~CDiskMonitor();

    virtual ULONG STDMETHODCALLTYPE Release() throw();
};


class COMMS_API CAttributeMonitor : public CDiskMonitor
{
    CComPtr<IAttribute> m_attr;

protected:
    virtual void notify();

public:
    CAttributeMonitor(IAttribute * attr);
};


