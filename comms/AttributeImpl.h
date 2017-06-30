#pragma once

#include "Attribute.h"
#include "repository.h"
#include "cmdProcess.h"
#include "EclErrorParser.h"
#include <UtilFilesystem.h>
#include "logger.h"
#include "EclCC.h"

typedef std::pair<std::_tstring, std::_tstring> EsdlPair;

class CAttributeBase : public clib::CLockableUnknown
{
protected:
    IRepository * m_repository;

public:
    virtual IAttributeType *GetType() const = 0;
    virtual const TCHAR *GetModuleQualifiedLabel(bool excludeRoot = false) const = 0;
    virtual const TCHAR *GetText(bool refresh, bool noBroadcast = false) const = 0;
    virtual const TCHAR *GetLabel() const = 0;

    bool pluginFolderExists(const std::string & batchFile, boost::filesystem::path & foundFolder, int level, bool typeFolder, boost::filesystem::path subPath) const
    {
        boost::filesystem::path folder, path;
        GetProgramFolder(folder);

        for (int i = 0; i < level; i++)
        {
            if (folder.has_parent_path())
            {
                folder = folder.parent_path();
            }
        }
        folder /= subPath;
        if (typeFolder)
        {
            folder /= stringToPath(GetType()->GetRepositoryCode());
        }
        path = folder / batchFile;
        if (clib::filesystem::exists(path))
        {
            foundFolder = folder;
            return true;
        }
        return false;
    }

    bool locatePlugin(const std::string & batchFile, boost::filesystem::path & foundFolder) const
    {
        boost::filesystem::path subPath, emptyPath;
        subPath = "clienttools";
        subPath /= "IDEPlugins";

        if (pluginFolderExists(batchFile, foundFolder, 2, true, subPath))
        {
            return true;
        }
        else if (pluginFolderExists(batchFile, foundFolder, 0, false, stringToPath("plugin")))
        {
            return true;
        }
        else if (pluginFolderExists(batchFile, foundFolder, 2, true, emptyPath))
        {
            return true;
        }
        else if (pluginFolderExists(batchFile, foundFolder, 3, true, emptyPath))
        {
            return true;
        }
        if (!boost::algorithm::iequals(batchFile, "ecl.bat")) {
            _DBGLOG(LEVEL_WARNING, (boost::format("Plugin not found - %1%") % batchFile).str().c_str());
        }
        return false;
    }

    EsdlPair ParseEsdlId(std::_tstring filename) const
    {
        EsdlPair esdlPair;

        boost::filesystem::path outFile = boost::filesystem::path(stringToPath(filename)).parent_path() / _T("log.txt");
        std::_tstring outStr;
        CUnicodeFile f;
        if (f.Open(outFile))
        {
            f.Read(outStr);
            f.Close();
        }

        std::string::size_type start_pos = 0;
        if (std::string::npos !=  (start_pos = outStr.find(_T("Successfully published"), start_pos)))
        {
            std::_tstring esdlID = outStr.substr(start_pos + 23, -1).c_str();
            if (std::string::npos != (start_pos = esdlID.find(_T("."), 0)))
            {
                std::_tstring definition = esdlID.substr(0, start_pos).c_str();
                std::_tstring version = esdlID.substr(start_pos + 1).c_str();
                esdlPair = std::make_pair(definition, version);
            }
        }

        return esdlPair;
     }

    virtual int PreProcess(PREPROCESS_TYPE action, const TCHAR * overrideEcl, IAttributeVector & attrs, IAttributeBookkeep & attrProcessed, Dali::CEclExceptionVector & errs, MetaInfo & metaInfo) const
    {
        std::_tstring label = (boost::_tformat(_T("%1%.%2%")) % GetModuleQualifiedLabel(true) % GetLabel()).str();
        if (attrProcessed[std::make_pair(action, label)]) {
            return 0;
        }
        attrProcessed[std::make_pair(action, label)] = true;
        metaInfo[MetaInfoItemRepositoryCode] = GetType()->GetRepositoryCode();

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
                    PreProcess(PREPROCESS_CALCINCLUDES, overrideEcl, nonECLattrs, attrProcessed, errs, metaInfo);
                    IAttributeVector::size_type idx = 0;
                    while (idx < nonECLattrs.size()) {
                        //  Convert from imported attribute to real attribute  ---
                        CComPtr<IAttribute> includeAttr = m_repository->GetAttribute(nonECLattrs[idx]->GetQualifiedLabel(), GetType());
                        if (includeAttr) 
                        {
                            IAttributeVector tmp;
                            includeAttr->PreProcess(PREPROCESS_CALCINCLUDES, includeAttr->GetText(), tmp, attrProcessed, errs, metaInfo);
                            nonECLattrs.insert(nonECLattrs.end(), tmp.begin(), tmp.end());
                        }
                        ++idx;
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
            
            std::_tstring cmd = (boost::_tformat(_T("cmd /c %1% %2% %3% %4% %5% %6% %7% \"%8%\" \"%9%\" \"%10%\" \"%11%\"")) % 
                batchFile.c_str() %
                PREPROCESS_LABEL[action] %
                GetModuleQualifiedLabel(true) % 
                GetLabel() % 
                inputFile.TempFileName() % 
                outputFile.TempFileName() % 
                errorFile.TempFileName() % 
                static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_USER))) % 
                eclFolders % 
                static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_SERVER_WORKUNIT))) %
                static_cast<const TCHAR *>(CString(GetIConfig(QUERYBUILDER_CFG)->Get(GLOBAL_PASSWORD)))).str();

            //_DBGLOG(m_url, LEVEL_INFO, cmd.c_str());
            std::_tstring in;
            std::_tstring out;
            std::_tstring err;
            int result = runProcess(cmd, pathToWString(folder), _T(""), in, out, err);
            inputFile.HandsOn();

            CComPtr<IRepository> rep = AttachModFileRepository(outputFile.TempFileName(), false);
            outputFile.HandsOn();
            if (action == PREPROCESS_SUBMIT && metaInfo[MetaInfoItemRepositoryCode] == ATTRIBUTE_TYPE_ESDL)
            {
                EsdlPair esdlPair = ParseEsdlId(outputFile.TempFileName());
                metaInfo[MetaInfoItemDesdlID] = esdlPair.first;
                metaInfo[MetaInfoItemDesdlVersion] = esdlPair.second;
            }

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
