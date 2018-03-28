#include "StdAfx.h"

#include "EclMeta.h"

void CEclMeta::LoadMetaData(const WPathVector *folders)
{
    for (WPathVector::const_iterator itr = folders->begin(); itr != folders->end(); ++itr)
    {
        PopulateMeta(itr->first,_T(""));
    }
}

void CEclMeta::PopulateMeta(const boost::filesystem::wpath & fileOrDir, const std::wstring dottedPath, int level)
{
    if (clib::filesystem::exists(fileOrDir))
    {
        std::wstring newDottedPath;
        
        if (level)
        {
            if (dottedPath.empty())
                newDottedPath = fileOrDir.stem().c_str();
            else
                newDottedPath = dottedPath + _T(".") + fileOrDir.stem().c_str();
        }

        if (HasValidExtension(pathToWString(fileOrDir)))
        {
            m_masterMeta[newDottedPath] = new  CEclFile(fileOrDir);
        }
        else if (boost::filesystem::is_directory(fileOrDir))
        {
            if (level)
                m_masterMeta[newDottedPath] = new CEclFolder(fileOrDir);
            boost::filesystem::directory_iterator end_itr;
            for (boost::filesystem::directory_iterator itr(wpathToPath(fileOrDir)); itr != end_itr; ++itr)
            {
                PopulateMeta(pathToWString(*itr), newDottedPath, ++level);
            }
        }
    }
}

const StlLinked<CEclMetaData> CEclMeta::GetMetaInfo(const std::_tstring & moduleFile)
{
     return m_masterMeta[moduleFile];
}

bool CEclMeta::AddAutoStr(StdStringVector &set, const std::_tstring & str)
{
    if (std::find(set.begin(), set.end(), str) == set.end())
    {
        set.push_back(str);
        return true;
    }
    return false;
}

bool CEclMeta::GetAutoC(const std::_tstring & moduleFile, StdStringVector &set)
{
    const StlLinked<CEclMetaData> meta = GetMetaInfo(moduleFile);
    if (!meta)
        return false;

    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(meta.get()->GetPath()); itr != end_itr; ++itr)
    {
        boost::filesystem::wpath path = *itr;
        if (boost::filesystem::is_directory(path) || boost::algorithm::ends_with(path.extension().c_str(), ATTRIBUTE_TYPE_ECL))
            AddAutoStr(set, path.stem().c_str());
    }

    return false;
}
