#pragma once

#include "clib.h"

// Modern Boost.Filesystem - path handles both narrow and wide strings

CLIB_API boost::filesystem::path stringToPath(const std::string & path);
CLIB_API boost::filesystem::path stringToPath(const std::wstring & path);

CLIB_API std::string pathToString(const boost::filesystem::path & path);
CLIB_API std::wstring pathToWString(const boost::filesystem::path & path);
CLIB_API std::wstring stringToWString(const std::string & str);
CLIB_API std::wstring stringToWString(const std::wstring & str);

namespace clib {
    namespace filesystem {
        CLIB_API bool exists(const boost::filesystem::path & path);
        CLIB_API bool is_directory(const boost::filesystem::path & path);
    }
}
