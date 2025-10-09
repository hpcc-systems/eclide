#include "stdafx.h"

#include "UtilFilesystem.h"

// Modern Boost.Filesystem - path handles both narrow and wide strings

CLIB_API boost::filesystem::path stringToPath(const std::string & path)
{ 
    return boost::filesystem::path(path);
}

CLIB_API boost::filesystem::path stringToPath(const std::wstring & path)
{ 
    return boost::filesystem::path(path);
}

CLIB_API std::string pathToString(const boost::filesystem::path & path)
{ 
    return path.string();
}

CLIB_API std::wstring pathToWString(const boost::filesystem::path & path)
{ 
    return path.wstring();
}

CLIB_API std::wstring stringToWString(const std::string & str)
{
    CA2T a2t(str.c_str()); 
    return static_cast<const TCHAR *>(a2t);
}
CLIB_API std::wstring stringToWString(const std::wstring & str)
{
    return str;
}

namespace clib {
    namespace filesystem {
        CLIB_API bool exists(const boost::filesystem::path & path)
        {
            try {
                return boost::filesystem::exists(path);
            }
            catch (boost::filesystem::filesystem_error &) {
            }
            return false;
        }
        CLIB_API bool is_directory(const boost::filesystem::path & path)
        {
            try {
                return boost::filesystem::is_directory(path);
            }
            catch (boost::filesystem::filesystem_error &) {
            }
            return false;
        }
    }
}
