#pragma once

#include "comms.h"

enum PREPROCESS_TYPE
{
    PREPROCESS_UNKNOWN, 
    PREPROCESS_CREATE, 
    PREPROCESS_SYNTAXCHECK, 
    PREPROCESS_CALCINCLUDES, 
    PREPROCESS_SAVE, 
    PREPROCESS_COMMIT, 
    PREPROCESS_SUBMIT, 
    PREPROCESS_LAST
};

const TCHAR * const PREPROCESS_LABEL[] = 
{
    _T("Unknown"),
    _T("Create"),
    _T("SyntaxCheck"),
    _T("CalcIncludes"),
    _T("Save"),
    _T("Commit"),
    _T("Submit"),
    _T("Last")
};

__interface IAttributeType : public IUnknown
{
    const TCHAR * GetRepositoryCode();
    const TCHAR * GetFileExtension();
    const TCHAR * GetDescription();
    const TCHAR * GetDefaultText(const std::_tstring & attrLabel);
    bool IsReadOnly();
};
typedef StlLinked<IAttributeType> IAttributeTypeAdapt;
typedef std::vector<IAttributeTypeAdapt> IAttributeTypeVector;

COMMS_API IAttributeType * CreateIAttributeGENERALType();
COMMS_API IAttributeType * CreateIAttributeECLType();
COMMS_API IAttributeType * CreateIAttributeESDLType();
COMMS_API IAttributeType * CreateIAttributeECMType();
COMMS_API IAttributeType * CreateIAttributeKELType();
COMMS_API IAttributeType * CreateIAttributeDUDType();
COMMS_API IAttributeType * CreateIAttributeSALTType();
COMMS_API IAttributeType * CreateIAttributeType(const std::_tstring & repositoryType, const std::_tstring & description = _T(""));
COMMS_API unsigned int GetAttributeTypes(IAttributeTypeVector & types);
COMMS_API bool IsValidExtension(const std::_tstring & ext);
COMMS_API bool HasValidExtension(const std::_tstring & filename);
