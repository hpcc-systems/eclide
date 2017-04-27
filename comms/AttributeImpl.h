#pragma once

#include "Attribute.h"
#include "repository.h"
#include "cmdProcess.h"
#include "EclErrorParser.h"
#include <UtilFilesystem.h>
#include "logger.h"
#include "EclCC.h"

class CAttributeBase : public clib::CLockableUnknown
{
protected:
    IRepository * m_repository;

public:
    virtual IAttributeType *GetType() const = 0;
    virtual const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const = 0;
    virtual const TCHAR *GetText(bool refresh, bool noBroadcast = false) const = 0;
    virtual const TCHAR *GetLabel() const = 0;

    bool locatePlugin(const std::string & batchFile, boost::filesystem::path & foundFolder) const
    {
        boost::filesystem::path folder, path;
        //  Check for ./plugin install  ---
        GetProgramFolder(folder);
        folder /= "plugin";
        path = folder / batchFile;
        if (clib::filesystem::exists(path))
        {
            foundFolder = folder;
            return true;
        }
        //  Check for sibling install  ---
        GetProgramFolder(folder);
        if (folder.has_parent_path()) 
        {
            folder = folder.parent_path();
            if (folder.has_parent_path()) 
            {
                folder = folder.parent_path();
                folder /= stringToPath(GetType()->GetRepositoryCode());
                path = folder / batchFile;
                if (clib::filesystem::exists(path))
                {
                    foundFolder = folder;
                    return true;
                }
            }
        }
        if (!boost::algorithm::iequals(batchFile, "ecl.bat")) {
            _DBGLOG(LEVEL_WARNING, (boost::format("Plugin not found - %1%") % batchFile).str().c_str());
        }
        return false;
    }

    virtual int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, Dali::CEclExceptionVector & errs) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        CComPtr<IEclCC> eclcc = CreateIEclCC();
        std::_tstring eclFolders;
        if (eclcc)
        {
            int eclFolderTotal = eclcc->GetEclFolderCount();
            for (int i = 0; i < eclFolderTotal; ++i)
            {
                if (i > 0)
                    eclFolders += _T(";");
                eclFolders += eclcc->GetEclFolder(i);
            }
        }
        std::string batchFile = CT2A(GetType()->GetRepositoryCode());
        batchFile += ".bat";
        boost::filesystem::path folder;
        if (locatePlugin(batchFile, folder)) 
        {
            switch (action)
            {
            case PREPROCESS_SAVE:
            case PREPROCESS_SYNTAXCHECK:
                //  Need to gather all included salt/esdl files
                {
                    IAttributeVector nonECLattrs;
                    PreProcess(PREPROCESS_CALCINCLUDES, overrideEcl, nonECLattrs, errs);
                    for(IAttributeVector::const_iterator itr = nonECLattrs.begin(); itr != nonECLattrs.end(); ++itr)
                    {
                        //  Convert from imported attribute to real attribute  ---
                        CComPtr<IAttribute> includeAttr = m_repository->GetAttribute(itr->get()->GetQualifiedLabel(), GetType());
                        if (includeAttr) 
                        {
                            IAttributeVector tmp;
                            includeAttr->PreProcess(PREPROCESS_CALCINCLUDES, includeAttr->GetText(), tmp, errs);
                            nonECLattrs.insert(nonECLattrs.end(), tmp.begin(), tmp.end());
                        }
                    }
                }
                break;
            }

            CAtlTemporaryFile inputFile, outputFile, errorFile;

            inputFile.Create();
            DWORD bytesWritten;
            std::string text = CT2A((overrideEcl != NULL ? overrideEcl : GetText(false)), CP_UTF8);

            inputFile.Write(text.c_str(), text.length(), &bytesWritten);
            inputFile.HandsOff();

            outputFile.Create(NULL, GENERIC_READ);
            outputFile.HandsOff();

            errorFile.Create(NULL, GENERIC_READ);
            errorFile.HandsOff();
            
            std::_tstring cmd = (boost::_tformat(_T("cmd /c %1% %2% %3% %4% %5% %6% %7% %8% \"%9%\" %10%")) % 
                batchFile.c_str() %
                PREPROCESS_LABEL[action] %
                GetModuleQualifiedLabel(true) % 
                GetLabel() % 
                inputFile.TempFileName() % 
                outputFile.TempFileName() % 
                errorFile.TempFileName() % 
                static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))) % 
                eclFolders % 
                static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT)))).str();

            //_DBGLOG(m_url, LEVEL_INFO, cmd.c_str());
            std::_tstring in;
            std::_tstring out;
            std::_tstring err;
            int result = runProcess(cmd, pathToWString(folder), _T(""), in, out, err);
            inputFile.HandsOn();

            CComPtr<IRepository> rep = AttachModFileRepository(outputFile.TempFileName(), false);
            outputFile.HandsOn();

            IModuleVector modules;
            rep->GetModules(_T(""), modules);
            for(IModuleVector::const_iterator itr = modules.begin(); itr != modules.end(); ++itr)
            {
                itr->get()->GetAttributes(attrs);
            }

            {
                std::_tstring errStr;
                CUnicodeFile f;
                if (f.Open(errorFile.TempFileName()))
                {
                    f.Read(errStr);
                    f.Close();
                }

                std::_tstring kelCacheFolder = pathToWString(stringToPath(inputFile.TempFileName()).parent_path() / stringToPath("kelcache"));
                typedef std::vector<std::_tstring> split_vector_type;
                split_vector_type SplitVec; 
                boost::algorithm::split(SplitVec, errStr, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);
                for(split_vector_type::iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
                {
                    ParsedEclError err;
                    if (ParseEclError(*itr, err))
                    {
                        StlLinked<Dali::CEclException> exception = new Dali::CEclException;
                        exception->m_code = err.code;
                        exception->m_lineNo = err.row;
                        exception->m_column = err.col;
                        exception->m_message = err.other.empty() ? err.message.c_str() : (boost::_tformat(_T("%1% -> %2%")) % err.message % err.other).str().c_str();
                        exception->m_severity = err.type.empty() ? _T("Error") : err.type.c_str();
                        if (boost::algorithm::iequals(inputFile.TempFileName(), err.location))
                            exception->m_fileName = L"";
                        else if (boost::algorithm::starts_with(err.location, kelCacheFolder))
                        {
                            boost::algorithm::erase_head(err.location, kelCacheFolder.length());
                            boost::algorithm::trim_left_if(err.location, boost::algorithm::is_any_of(_T("\\/")));
                            boost::filesystem::path errPath = stringToPath(err.location);
                            errPath = errPath.replace_extension();
                            err.location = pathToWString(errPath);
                            boost::algorithm::replace_all(err.location, _T("\\"), _T("."));
                            exception->m_fileName = err.location.c_str();
                        }
                        else
                            exception->m_fileName = err.location.c_str();
                        exception->m_fileType = GetType()->GetRepositoryCode();
                        errs.push_back(exception);
                    }
                }
            }
            errorFile.HandsOn();
        }
        return attrs.size();
    }
};
