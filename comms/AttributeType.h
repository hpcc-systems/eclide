#pragma once

#include "comms.h"

#define ATTRIBUTE_TYPE_GENERAL _T("general")
#define ATTRIBUTE_TYPE_ECL _T("ecl")
#define ATTRIBUTE_TYPE_ECLMOD _T("eclmod")
#define ATTRIBUTE_TYPE_ECLLIB _T("ecllib")
#define ATTRIBUTE_TYPE_ECLOTHER _T("ecl*")
#define ATTRIBUTE_TYPE_PLUGIN _T("dll")
#define ATTRIBUTE_TYPE_ESDL _T("esdl")
#define ATTRIBUTE_TYPE_ECM _T("ecm")
#define ATTRIBUTE_TYPE_SALT _T("salt")
#define ATTRIBUTE_TYPE_KEL _T("kel")
#define ATTRIBUTE_TYPE_XSLT _T("xslt")
#define ATTRIBUTE_TYPE_WORKSPACE _T("ecl_ws")
#define ATTRIBUTE_TYPE_DUD _T("dud")
#define ATTRIBUTE_TYPE_CMP _T("cmp")
#define ATTRIBUTE_TYPE_LUCIFAMILY _T("mff")
#define ATTRIBUTE_TYPE_DEFT _T("dft")

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
COMMS_API IAttributeType * AttributeTypeFromExtension(const std::_tstring & extension);
COMMS_API IAttributeType * CreateIAttributeDUDType();
COMMS_API IAttributeType * CreateIAttributeECLType();
COMMS_API IAttributeType * CreateIAttributeESDLType();
COMMS_API IAttributeType * CreateIAttributeECMType();
COMMS_API IAttributeType * CreateIAttributeKELType();
COMMS_API IAttributeType * CreateIAttributeSALTType();
COMMS_API IAttributeType * CreateIAttributeType(const std::_tstring & repositoryType, const std::_tstring & description = _T(""));
COMMS_API unsigned int GetAttributeTypes(IAttributeTypeVector & types);
COMMS_API bool IsValidExtension(const std::_tstring & ext);
COMMS_API bool HasValidExtension(const std::_tstring & filename);
