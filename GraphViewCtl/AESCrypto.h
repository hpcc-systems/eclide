#pragma once

#include "aes/aescpp.h"

#ifdef SEISINT_LIBEXPORTS
#  define GRAPHVIEWCTL_API
#else
#  ifdef GRAPHVIEWCTL_EXPORTS
#    define GRAPHVIEWCTL_API __declspec(dllexport)
#  else
#    define GRAPHVIEWCTL_API __declspec(dllimport)
#  endif
#endif

class GRAPHVIEWCTL_API CAESCrypto
{
private:
	AESdecrypt m_decryptor;
	AESencrypt m_encryptor;
public:
	CAESCrypto();

	std::string& Decode(std::string& buff);
	std::string& Decrypt(std::string& buff);

	std::string& Encode(std::string& buff);
	std::string& Encrypt(std::string& buff);
};
