#pragma once

#define AMT_SHOW_NONE				0x0000
#define AMT_SHOW_NOT_MATCHING		0x0001
#define AMT_SHOW_MATCHING			0x0002
#define AMT_SHOW_ORPHANS			0x0004

template<typename T>
bool CanShow(int filter, T lhs, T rhs)
{
    if (filter & AMT_SHOW_MATCHING && filter & AMT_SHOW_NOT_MATCHING && filter & AMT_SHOW_ORPHANS)
    {
        return true;
    }
    if(filter & AMT_SHOW_ORPHANS)
    {
        if (!lhs->Exists() || !rhs->Exists())
            return true;
    }
    if (filter & AMT_SHOW_MATCHING || filter & AMT_SHOW_NOT_MATCHING)
    {
        if (lhs->Exists() && rhs->Exists())	//  Hack for case where module has no crc...
        {
            bool equals = boost::algorithm::equals(lhs->GetChecksum(), rhs->GetChecksum());
            if (filter & AMT_SHOW_MATCHING && equals)
                return true;
            else if (filter & AMT_SHOW_NOT_MATCHING && !equals)
                return true;
        }
    }
    return false;
}
