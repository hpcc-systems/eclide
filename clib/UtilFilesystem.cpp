#include "stdafx.h"

#include "UtilFilesystem.h"

#if (BOOST_FILESYSTEM_VERSION == 3)
CLIB_API boost::filesystem::path wpathToPath(const boost::filesystem::wpath &path)
{
    return path;
}
CLIB_API boost::filesystem::wpath pathToWPath(const boost::filesystem::path &path)
{
    return path;
}

CLIB_API boost::filesystem::path stringToPath(const std::string & path)
{ 
    return boost::filesystem::path(path, boost::filesystem::native);
}
CLIB_API boost::filesystem::path stringToPath(const std::wstring & path)
{ 
    CT2A t2a(path.c_str());
    return boost::filesystem::path(std::string(t2a), boost::filesystem::native);
}

CLIB_API boost::filesystem::wpath stringToWPath(const std::string &path)
{
    return boost::filesystem::wpath(path, boost::filesystem::native);
}
CLIB_API boost::filesystem::wpath stringToWPath(const std::wstring &path)
{
    return stringToPath(path);
}

CLIB_API std::string pathToString(const boost::filesystem::path & path)
{ 
    return path.string();
}

CLIB_API std::wstring pathToWString(const boost::filesystem::path & path)
{ 
    return path.wstring();
}
#else
CLIB_API boost::filesystem::path wpathToPath(const boost::filesystem::wpath & path)
{
    return pathToString(path);
}
CLIB_API boost::filesystem::wpath pathToWPath(const boost::filesystem::path & path)
{
    return pathToWString(path);
}
CLIB_API boost::filesystem::path stringToPath(const std::string & path)
{ 
    return boost::filesystem::path(path, boost::filesystem::native);
}
CLIB_API boost::filesystem::path stringToPath(const std::wstring & path)
{ 
    CT2A t2a(path.c_str()); 
    return boost::filesystem::path(t2a, boost::filesystem::native);
}
CLIB_API std::wstring pathToWString(const boost::filesystem::path & path)
{ 
    CA2T a2t(path.native_file_string().c_str()); 
    return static_cast<const TCHAR *>(a2t);
}
CLIB_API std::string pathToString(const boost::filesystem::path & path)
{ 
    return path.native_file_string(); 
}
CLIB_API std::string pathToString(const boost::filesystem::wpath & path)
{
    CT2A t2a(path.native_file_string().c_str()); 
    return static_cast<const char *>(t2a);
}
CLIB_API std::wstring pathToWString(const boost::filesystem::wpath & path)
{ 
    return path.native_file_string();
}
#endif
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
