#include "StdAfx.h"
#include "AESCrypto.h"

// 128 bit / 16 byte key (17 with null terminator for ease of use)
const BYTE key[17] = "54e3e450ca89685d";

CAESCrypto::CAESCrypto() : m_encryptor(key), m_decryptor(key)
{
}

std::string& CAESCrypto::Encrypt(std::string& buff)
{
    int len = buff.length();
    int remainder = len % 16;
    if (remainder == 0)
        remainder = 16;
    int newlen = len + (16 - remainder);
    buff.resize(newlen, 0);

    for (int i=0; i < len; i+=16) {
        unsigned char in[16];
        unsigned char out[16];
        memcpy(in, &buff[i], 16);
        m_encryptor.encrypt(in, out);
        memcpy(&buff[i], out, 16);
    }
    return buff;
}

std::string& CAESCrypto::Encode(std::string& buff)
{
    int len = buff.length();
    DWORD base64Flags = ATL_BASE64_FLAG_NOPAD | ATL_BASE64_FLAG_NOCRLF;
    char * result;
    int resultLen = Base64EncodeGetRequiredLength(len, base64Flags) + 1;
    result = (char*)malloc(resultLen);
    Base64Encode((BYTE*)&buff[0], len, result, &resultLen, base64Flags);
    buff.assign(result, resultLen);
    free(result);
    return buff;
}

std::string& CAESCrypto::Decrypt(std::string& buff)
{
    int len = buff.length();
    int remainder = len % 16;
    if (remainder == 0)
        remainder = 16;
    int newlen = len + (16 - remainder);
    buff.resize(newlen, 0);

    unsigned char in[16];
    unsigned char out[16];
    char null[16];
    memset(&null[0], 0, 16);

    for (int i=0; i < len; i+=16) {
        memcpy(in, &buff[i], 16);
        bool isNull = true;
        // backwards compatibility - sometimes all nulls got somehow stored, which were therefore
        // not encrypted, fixed in current code, but old files might exist
        for (int j=0; j < 16; j++)
        {
            if (in[j] != null[j])
            {
                isNull = false;
                break;
            }
        }
        if (isNull)
            memcpy(&buff[i], in, 16);
        else {
            m_decryptor.decrypt(in, out);
            memcpy(&buff[i], out, 16);
        }
    }

    //buff.resize(len, 0);
    buff = buff.c_str();
    return buff;
}

std::string& CAESCrypto::Decode(std::string& buff)
{
    char * result;
    //AtlTrace(_T("decode:%s\n"), CA2T(buff.c_str()));

    int len = Base64DecodeGetRequiredLength(buff.length());
    result = new char[len + 1];
    Base64Decode(buff.c_str(), len, (BYTE *)result, &len);
    result[len] = '\0';
    buff.assign(result, len);

    //AtlTrace(_T("decoded:%s\n"), CA2T(result));

    delete[](result);
    return buff;
}
