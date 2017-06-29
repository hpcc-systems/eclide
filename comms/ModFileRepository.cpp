#include "StdAfx.h"
#include "ModFileRepository.h"
#include "Module.h"
#include "cache.h"
#include "UnicodeFile.h"
#include "md5.hpp"
#include "ModuleHelper.h"
#include <UtilFilesystem.h>

#if _COMMS_VER < 68200
using namespace WsAttributes;
#else
#endif

IModule * CreateModFileModule(const IRepository *rep, const std::_tstring &label, bool noBroadcast = false);
IModule * CreateModFileModulePlaceholder(const IRepository *rep, const std::_tstring &label);
IAttribute * CreateModFileAttribute(const IRepository *rep, const std::_tstring &moduleLabel, const std::_tstring &label, const std::_tstring &ecl, const std::_tstring &extension);
IAttribute * GetModFileAttribute(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* ecl, const TCHAR* extension, unsigned version, bool sandboxed, bool placeholder);
IAttribute * CreateModFileAttributePlaceholder(const IRepository *rep, const TCHAR* module, const TCHAR* label, const TCHAR* ecl, const TCHAR* extension);

void ClearModFileAttributeCache();
void ClearModFileModuleCache();

#define IMPORT_MARKER _T("//Import:")
#define COMMENT_MARKER _T("//Comment:")

class CModFileRepository : public IRepository, public clib::CLockableUnknown
{
protected:
    CString m_url;
    CString m_label;
    boost::filesystem::path m_path;
    std::_tstring m_modFile;

public:
    BEGIN_CLOCKABLEUNKNOWN
    END_CUNKNOWN(clib::CLockableUnknown)

    CModFileRepository(const TCHAR* url, const TCHAR* label) : m_label(label), m_url(url)
    {
        m_path = stringToPath(url);

        CUnicodeFile file;
        if (file.Open(pathToWString(m_path).c_str()))
        {
            file.Read(m_modFile);
            file.Close();
        }
        boost::algorithm::replace_all(m_modFile, _T("\r\n\r\n"), _T("\r\n \r\n"));	//Hack to stop blank lines being eaten by the tokenizer.
    }

    ~CModFileRepository()
    {
    }

    void SetRepositoryLabel(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
    }

    const TCHAR * GetRepositoryLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return _T("");
    }

    const TCHAR * GetID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_url;
    }

    const TCHAR * GetCacheID() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_url;
    }

    const TCHAR * GetLabel() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return m_label;
    }

    const TCHAR * GetUserId() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return 0;
    }

    const TCHAR * GetPassword() const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return 0;
    }

    IModule * GetModule(const TCHAR* label, bool GetChecksum = false, bool noBroadcast = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IModuleVector modules;
        GetAllModules(modules, GetChecksum, false, noBroadcast);
        for (IModuleVector::iterator itr = modules.begin(); itr != modules.end(); ++itr)
        {
            if (boost::algorithm::iequals(itr->get()->GetQualifiedLabel(), label))
            {
                return itr->get();
            }
        }
        return NULL;
    }

    IModule * GetModulePlaceholder(const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IModule * module = CreateModFileModulePlaceholder(this, label);
        return module;
    }

    unsigned GetModules(const std::_tstring & module, IModuleVector & modules, bool GetChecksum = false, bool noRefresh=false) const
    {
        IModuleVector allModules;
        GetAllModules(allModules, GetChecksum, noRefresh);
        for(IModuleVector::iterator itr = allModules.begin(); itr != allModules.end(); ++itr)
        {
            IModule * parent = itr->get()->GetParentModule();
            if ((!parent && module.empty()) || (parent && boost::algorithm::equals(module, parent->GetQualifiedLabel())))
                modules.push_back(itr->get());
        }
        return 0;
    }

    unsigned GetAllModules(IModuleVector & modules, bool GetChecksum = false, bool noRefresh = false, bool noBroadcast = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        typedef std::map<std::_tstring, bool> DedupMap;
        DedupMap dedupMap;

        typedef std::vector<std::_tstring> split_vector_type;
        split_vector_type SplitVec; 
        boost::algorithm::split(SplitVec, m_modFile, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

        for (split_vector_type::const_iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
        {
            std::_tstring line = *itr;
            if (boost::algorithm::istarts_with(line, IMPORT_MARKER))
            {
                boost::algorithm::ireplace_first(line, IMPORT_MARKER, _T(""));
                boost::algorithm::trim(line);

                CModuleHelper moduleHelper(line);
                dedupMap[moduleHelper.GetModuleLabel()] = true;
            }
        }

        for(DedupMap::iterator itr = dedupMap.begin(); itr != dedupMap.end(); ++itr)
        {
            StlLinked<IModule> module = CreateModFileModule(this, itr->first, noBroadcast);
            modules.push_back(module);
        }

        std::sort(modules.begin(), modules.end(), IModuleCompare());
        return modules.size();
    }

    unsigned GetModulesAutoC(const std::_tstring & module, StdStringVector &modules) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::GetModulesAutoC not implemented");
        return 0;
    }
    IModule * InsertModule(const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return CreateModFileModule(this, label);
    }

    unsigned int GetAttributeTypes(IAttributeTypeVector & types) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        types.push_back(CreateIAttributeECLType());
        types.push_back(CreateIAttributeESDLType());
        types.push_back(CreateIAttributeSALTType());
        return types.size();
    }

    unsigned FindAttributes(const std::_tstring & searchText, const std::_tstring & module, const std::_tstring & user, SEARCHMODE searchMode, bool sandboxed, bool checkedout, bool locked, bool orphaned, const  TCHAR* utcDate, IAttributeVector & attributes) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::FindAttributes not implemented");
        return 0;
    }
    bool Label(const TCHAR* label, std::_tstring &error_message) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::Label not implemented");
        return false;
    }
    bool GetLabels(CRepLabelVector & labels) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return true;
    }
    bool GetLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & globalLabels, CRepLabelVector & localLabels) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return true;
    }
    bool GetAttributeLabels(const std::_tstring & module, const std::_tstring & attr, CRepLabelVector & results) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return true;
    }
    bool UpdateLabel(const std::_tstring & label, const std::_tstring & module, const std::_tstring & attr, IAttributeType * type, unsigned int version, std::_tstring &error_message) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::UpdateLabel not implemented");
        return false;
    }

    //  Module short cuts  ---
    unsigned GetAttributes(const TCHAR* module, const IAttributeTypeVector & types, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        struct CommentEclExt
        {
            std::_tstring m_comment;
            std::_tstring m_ecl;
            std::_tstring m_extension;
        };
        typedef std::map<std::_tstring, CommentEclExt> AttrMap;
        AttrMap attrs;
        std::_tstring attributeLabel;
        std::_tstring attributeComment;
        std::_tstring attributeEcl;
        std::_tstring attributeExtension;

        typedef std::vector<std::_tstring> split_vector_type;
        split_vector_type SplitVec; 
        boost::algorithm::split(SplitVec, m_modFile, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

        CommentEclExt attrInfo;

        for (split_vector_type::const_iterator itr = SplitVec.begin(); itr != SplitVec.end(); ++itr)
        {
            std::_tstring trimmedLine = *itr;
            boost::algorithm::trim(trimmedLine);
            if (boost::algorithm::istarts_with(trimmedLine, IMPORT_MARKER))
            {
                if (attributeLabel.length())
                {
                    attrInfo.m_comment = attributeComment;
                    attrInfo.m_ecl = attributeEcl;
                    attrInfo.m_extension = attributeExtension;
                    attrs[attributeLabel] = attrInfo;
                }
                boost::algorithm::ireplace_first(trimmedLine, IMPORT_MARKER, _T(""));
                attributeLabel = trimmedLine;

                std::vector<std::_tstring> parts;
                boost::split(parts, attributeLabel, boost::is_any_of(":"), boost::token_compress_on);
                if (parts.size() > 1) {
                    attributeLabel = parts[1];
                    attributeExtension = parts[0];
                }
                else {
                    attributeExtension = _T("ECL");
                }

                attributeComment.clear();
                attributeEcl.clear();
            }
            else if (boost::algorithm::istarts_with(trimmedLine, COMMENT_MARKER))
            {
                boost::algorithm::ireplace_first(trimmedLine, COMMENT_MARKER, _T(""));
                attributeComment += trimmedLine + _T("\r\n");
            }
            else
            {
                attributeEcl += *itr + _T("\r\n");
            }
        }
        if (attributeLabel.length())
        {
            attrInfo.m_comment = attributeComment;
            attrInfo.m_ecl = attributeEcl;
            attrInfo.m_extension = attributeExtension;
            attrs[attributeLabel] = attrInfo;
        }

        for(AttrMap::iterator itr = attrs.begin(); itr != attrs.end(); ++itr)
        {
            int curPos = 0;
            CModuleHelper moduleHelper(itr->first);
            if (boost::algorithm::iequals(moduleHelper.GetModuleLabel(), module))
            {
                StlLinked<IAttribute> attribute = CreateModFileAttribute(this, module, moduleHelper.GetAttributeLabel(), itr->second.m_ecl, itr->second.m_extension);
                attributes.push_back(attribute);
            }
        }

        std::sort(attributes.begin(), attributes.end(), IAttributeCompare());
        return attributes.size();
    }
    unsigned GetAttributes(const TCHAR* module, IAttributeVector & attributes, bool noRefresh=false, bool noBroadcast=false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IAttributeTypeVector types;
        return GetAttributes(module, types, attributes, noRefresh);
    }
    unsigned GetAttributesAutoC(const std::_tstring & module, StdStringVector & attributes) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::GetAttributesAutoC not implemented");
        return 0;
    }
    IModule * RenameModule(const TCHAR* module, const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::RenameModule not implemented");
        return 0;
    }
    bool DeleteModule(const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::DeleteModule not implemented");
        return false;
    }

    //  Attribute short cuts  ---
    IAttribute * GetAttribute(const TCHAR* module, const TCHAR* label, IAttributeType * type, int version, bool sandbox, bool text, bool placeholder) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IAttributeVector attributes;
        GetAttributes(module, attributes);
        for (IAttributeVector::iterator itr = attributes.begin(); itr != attributes.end(); ++itr)
        {
            if (boost::algorithm::iequals(itr->get()->GetLabel(), label))
            {
                return itr->get();
            }
        }
        return NULL;
    }

    IAttribute * GetAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return GetAttribute(module, attribute, type, version, sandbox, text, false);
    }

    IAttribute * GetAttributePlaceholder(const std::_tstring & moduleLabel, const std::_tstring & attributeLabel, IAttributeType * type) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        IAttribute * attribute = CreateModFileAttributePlaceholder(this, moduleLabel.c_str(), attributeLabel.c_str(), _T(""), type->GetFileExtension());
        return attribute;
    }

    IAttribute * GetAttribute(const TCHAR* pModAttr, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        CString module, attr;
        int curPos = 0;

        CString moduleAttribute(pModAttr);
        module = moduleAttribute.Tokenize(_T("."), curPos);
        if (module != "")
        {
            attr = moduleAttribute.Tokenize(_T("."), curPos);
            if (attr != "")
                return GetAttribute(module, attr, type, version, sandbox, text);
        }

        return NULL;
    }
    IAttribute * GetAttributeFast(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool sandbox = true, bool text = false, bool noBroadcast = false) const
    {
        IAttribute * retVal = ::GetModFileAttribute(this, module, attribute, _T(""), type->GetFileExtension(), version, sandbox, false);
        if (retVal)
            return retVal;

        return GetAttribute(module, attribute, type, version, sandbox, text, noBroadcast);
    }
    IAttribute * GetAttributeFuzzy(const std::_tstring & moduleAttribute, const std::_tstring & module, IAttributeType * type, int version = 0, bool sandbox = true, bool text = true) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::GetAttributeFuzzy not implemented");
        return 0;
    }
    bool GetAttributeText(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version, bool sandbox, CString &text) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::GetAttributeText not implemented");
        return false;
    }
    IAttribute * InsertAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return CreateModFileAttribute(this, module, attribute, type->GetFileExtension(), _T(""));
    }
    unsigned GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, IAttributeHistoryVector & attributes) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return NULL;
    }
    IAttribute * RenameAttribute(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::RenameAttribute not implemented");
        return NULL;
    }
    IAttribute * RenameAttribute(IAttribute * attribute, const TCHAR* label) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::RenameAttribute not implemented");
        return NULL;
    }
    IAttribute * ChangeAttributeType(IAttribute * attribute, const TCHAR* ext) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::ChangeAttributeType not implemented");
        return NULL;
    }
    bool DeleteAttributes(const IAttributeVector & attributes) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::DeleteAttribute not implemented");
        return false;
    }

    bool Checkin(IAttributeVector & attributes, const TCHAR* comment, bool andRevert)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }
    bool Checkout(IAttributeVector & attributes)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(false);
        return false;
    }
    bool Save(IAttributeVector & attributes, bool noBroadcast = false)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        for(IAttributeVector::iterator itr = attributes.begin(); itr != attributes.end(); ++itr)
        {
            std::_tstring newModFile;
            bool inCurrentAttr = false;
            bool alreadyExisted = false;

            typedef std::vector<std::_tstring> split_vector_type;
            split_vector_type SplitVec; 
            boost::algorithm::split(SplitVec, m_modFile, boost::algorithm::is_any_of("\r\n"), boost::algorithm::token_compress_on);

            for (split_vector_type::const_iterator itr2 = SplitVec.begin(); itr2 != SplitVec.end(); ++itr2)
            {
                std::_tstring line = *itr2;
                std::_tstring origline = line;
                if (boost::algorithm::istarts_with(line, IMPORT_MARKER))
                {
                    boost::algorithm::ireplace_first(line, IMPORT_MARKER, _T(""));
                    boost::algorithm::trim(line);
                    inCurrentAttr = boost::algorithm::iequals(line, itr->get()->GetQualifiedLabel());
                    if (inCurrentAttr)
                    {
                        newModFile += (boost::_tformat(_T("%1%%2%\r\n%3%\r\n")) % IMPORT_MARKER % itr->get()->GetQualifiedLabel() % itr->get()->GetText(true, noBroadcast)).str();
                        alreadyExisted = true;
                        continue;
                    }
                }
                if (!inCurrentAttr)
                {
                    newModFile += origline + _T("\r\n");
                }
            }
            if (!alreadyExisted)
                newModFile += (boost::_tformat(_T("%1%%2%\r\n%3%\r\n")) % IMPORT_MARKER % itr->get()->GetQualifiedLabel() % itr->get()->GetText(true, noBroadcast)).str();
            m_modFile = newModFile.c_str();
        }

        CUnicodeFile file;
        if (file.Create(pathToWString(m_path).c_str()))
        {
            file.Write(m_modFile);
            file.Close();
        }

        return true;
    }
    bool Move(IAttributeVector & attributes, const TCHAR* module)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(false);
        return false;
    }
    bool Rollback(IAttributeVector & attributes)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(false);
        return false;
    }
    //  Attribute History short cuts ----
    IAttributeHistory * GetAttributeHistory(const TCHAR* module, const TCHAR* attribute, IAttributeType * type, int version = 0, bool text = false) const
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        ATLASSERT(!"CModFileRepository::GetAttributeH not implemented");
        return NULL;
    }

    //  General  ---
    bool ModuleExists(const TCHAR* label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return (GetModule(label) != NULL);
    }
    bool AttributeExists(const TCHAR* module, const TCHAR* label, IAttributeType * type)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return (GetAttribute(module, label, type) != NULL);
    }
    IWorkspace * CreateIWorkspace(const std::_tstring & label = _T(""), bool showInRoot = true, bool placeholder = false)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return ::CreateIWorkspace(this, label, showInRoot, placeholder);
    }
    unsigned int GetWorkspaces(IWorkspaceVector * results, bool GetWorkspaces)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return results->size();
    }
    bool WorkspaceExists(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return false;
    }
    bool RemoveWorkspace(const std::_tstring & label)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return ::RemoveWorkspace(this, label);
    }
    void SupressModuleChecksum(bool supressModuleChecksum)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        //  GetChecksum is ignored anyway  ---
    }
    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const std::_tstring & label, const std::_tstring & path)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return ::CreateIWorkspaceItem(this, type, label, path);
    }
    IWorkspaceItem * CreateIWorkspaceItem(IAttribute * attr, const std::_tstring & path)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return ::CreateIWorkspaceItem(this, attr, path);
    }
    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return  ::CreateIWorkspaceItem(this, type);
    }
    IWorkspaceItem * CreateIWorkspaceItem(WORKSPACE_ITEM_TYPE type, const CPersistMap & pm)
    {
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        return  ::CreateIWorkspaceItem(this, type, pm);
    }
    const boost::filesystem::path & GetEnvironmentFolder(boost::filesystem::path & path) const
    {	
        clib::recursive_mutex::scoped_lock proc(m_mutex);
        path /= "%TEMP%";
        return path;
    }
    void ClearShortTermCache()
    {
    }
};

static CacheT<std::_tstring, CModFileRepository> ModFileRepositoryCache;

void ClearModFileRepositorySingletons()
{
    ClearModFileAttributeCache();
    ClearModFileModuleCache();
    ModFileRepositoryCache.Clear();
}

COMMS_API IRepository * AttachModFileRepository(const TCHAR* url, bool bClearCache)
{
    CModFileRepository *rep = new CModFileRepository(url, url);
    if ( bClearCache )
        ModFileRepositoryCache.Clear(rep);
    return ModFileRepositoryCache.Get(rep);
}
