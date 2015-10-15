#pragma once
#include "EclLib.h"
#include "clib.h"

__interface IEclIdentifierReference;
__interface IEclIdentifier;
__interface IEclType;
__interface IEclValueList;
__interface IEclValue;
__interface IEclParamaterizedAttribute;

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247df")) IEclElement : public IUnknown
{
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const;
};
typedef StlLinked<IEclElement> IEclElementAdapt;

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e0")) IEclUnknown : public IEclElement
{
    const TCHAR * GetEcl();
    void SetEcl(const TCHAR *ecl);
    bool EndsInSemi(void);
};

__interface __declspec(uuid("0C303091-0B8E-4666-982E-8F727E2A47FA")) IEclAttribute : public IEclElement
{
    const TCHAR * GetAttribute();
};

__interface __declspec(uuid("E4A0014E-06A4-4330-9B76-EF8D83304DBB")) IEclTransformExpression : public IEclElement
{
    const TCHAR * GetExpression();
    void SetExpression(const TCHAR *);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e1")) IEclComment : public IEclElement
{
    const TCHAR * GetComment(std::_tstring& comment);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e2")) IEclInvokeComment : public IEclComment
{
    const TCHAR * GetLabel();
    const TCHAR * GetEcl();
};

__interface __declspec(uuid("28319F0B-300C-4e2c-A5B9-76D9B414E5C8")) IEclGrETLComment : public IEclComment
{
    bool IsWindow() const;
    void SetWindow(bool wnd);
    bool IsShowEdgeLabels() const;
    void SetShowEdgeLabels(bool show);
    bool IsCompressRows() const;
    void SetCompressRows(bool compress);
    int GetX() const;
    int GetY() const;
    void SetX(int x);
    void SetY(int y);
    bool IsMinimized() const;
    void SetMinimized(bool set);
    bool IsMaximized() const;
    void SetMaximized(bool set);
    int GetTopIndex() const;
    void SetTopIndex(int idx=0);
    int GetWidth() const;
    void SetWidth(int width);
    int GetHeight() const;
    void SetHeight(int height);
};

typedef StlLinked<IEclGrETLComment> IEclGrETLCommentAdapt;

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e3")) IEclMacro : public IEclElement
{
    const TCHAR * GetParams();
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e4")) IEclProgram : public IEclElement
{
    unsigned GetElementCount();
    IEclElement * GetElement(unsigned i);
    void AddElement(IEclElement *element, int pos=-1); //pos 0 is first, -1 last
    bool DeleteElement(IEclElement *element);
    const TCHAR* SerializeToXml(std::_tstring &xml);
    bool ReplaceElement(IEclElement* from, IEclElement* to);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f6")) IEclDeclaration : public IEclElement
{
    IEclType * GetType();
    void SetType(IEclType *);
    IEclIdentifier * GetIdentifier();
    const TCHAR *GetIdentifierName(std::_tstring& name);
};
typedef StlLinked<IEclDeclaration> IEclDeclarationAdapt;

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e5")) IEclDeclAndAssign : public IEclDeclaration
{
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e7")) IEclAssign : public IEclElement
{
    const TCHAR *GetIdentifierName(std::_tstring& name);
    IEclIdentifier * GetIdentifier();
    IEclElement * GetRHS();
    void SetExported(bool);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e6")) IEclType : public IEclElement
{
    const TCHAR *GetTypeName();

    unsigned GetFixedLogicalWidth(); //logical width for splitting
    void SetFixedLogicalWidth(unsigned);

    unsigned GetFixedPhysicalWidth(); //in bytes
    unsigned CanCast(IEclType* target);
    unsigned MaxChars();

    IEclType* CloneType();
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e8")) IEclBoolean : public IEclType
{
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247e9")) IEclInteger : public IEclType
{
    bool isBigEndian();
    bool isUnsigned();
    unsigned getByteCount();
    void setBigEndian(bool);
    void setUnsigned(bool);
    void setByteCount(unsigned);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f0")) IEclReal : public IEclType
{
    unsigned getByteCount();
    void setByteCount(unsigned);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f1")) IEclDecimal : public IEclType
{
    bool isUnsigned();
    unsigned getTotalDigitCount();
    unsigned getDecimalDigitCount();
    void setTotalDigitCount(unsigned);
    void setDecimalDigitCount(unsigned);
    void setUnsigned(bool);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f2")) IEclString : public IEclType
{
    bool isEbcdic();
    bool isCompressed();
    bool isUnicode();
    bool isVariable();
    unsigned getCharCount();
    void setEbcdic(bool);
    void setCompressed(bool);
    void setUnicode(bool);
    void setVariable(bool);
    void setCharCount(unsigned);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f3")) IEclData : public IEclType
{
    unsigned getByteCount();
    void SetByteCount(unsigned byteCount);
};


__interface __declspec(uuid("BD8BF477-B1AC-44f9-97E8-3D3121C28C92")) IEclServiceDeclaration : public IEclElement
{
    IEclIdentifier * GetIdentifier();
    const TCHAR *GetIdentifierName(std::_tstring& name);
};


__interface __declspec(uuid("D4D8AE9D-D31D-492c-8A30-3523932BDD70")) IEclService : public IEclElement
{
    IEclIdentifier * GetIdentifier();
    const TCHAR *GetIdentifierName(std::_tstring& name);
};

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f4")) IEclRecord : public IEclElement
{
    unsigned GetFieldCount();
    IEclElement * GetField(unsigned i);	//Can be either a IEclDeclaration or an IEclDeclAndAssign (I may make the later inherit from the former).

    void AddField(IEclDeclaration *decl, int pos=-1); //pos 0 is first, -1 last
    void AddField(IEclAssign *assign, int pos=-1); //pos 0 is first, -1 last
    void DeleteField(int pos);
    void SplitField(int field, int offset);
    void mergeWithPrevious(int field);
    void mergeWithNext(int field);
    void rename(int field, const std::_tstring & name);
    int GetFieldPos(const std::_tstring & label); 
    const TCHAR * GetFieldLabel(unsigned int pos, std::_tstring & label);
    const TCHAR * GetFieldTypeEcl(int field, bool typeonly, std::_tstring & result);
    IEclType * GetFieldType(int field);
};
typedef StlLinked<IEclRecord> IEclRecordAdapt;

enum ECLDATASETFORMAT
{
    ECLDATASETFORMAT_UNKNOWN,
    ECLDATASETFORMAT_FLAT,
    ECLDATASETFORMAT_CSV,
    ECLDATASETFORMAT_XML
};

__interface __declspec(uuid("C75396C0-3A2E-4336-87DE-2D710493BBE0")) IEclDataset : public IEclElement
{
    IEclValue * GetFile();
    void SetFile(IEclValue * file);
    IEclValue * GetRecord();
    ECLDATASETFORMAT GetFormat();

    int GetCSVHeading();
    void SetCSVHeading(int count);
    void SetCSVMaxLength(int count);
    void SetCSVSeparator(const std::_tstring & sep);
    void SetCSVTerminator(const std::_tstring & sep);
    void SetXmlXPath(const TCHAR* xpath);
};
typedef StlLinked<IEclDataset> IEclDatasetAdapt;

__interface __declspec(uuid("CD7D0985-23EC-4b89-AA9D-64BFCB79C7AE")) IEclProject : public IEclElement
{
};
typedef StlLinked<IEclProject> IEclProjectAdapt;

__interface __declspec(uuid("6B4D3063-606D-4af8-8F1F-5802231A6FA4")) IEclOutput : public IEclElement
{
    IEclIdentifier * GetIdentifier(); //can be NULL!
    const TCHAR *GetIdentifierName(std::_tstring& name);
    IEclRecord * GetRecord(); //can be NULL!
};
typedef StlLinked<IEclOutput> IEclOutputAdapt;

__interface __declspec(uuid("3199E8AD-1D96-4840-8487-B3CA625FA4C8")) IEclTransformAssignment : public IEclElement
{
    IEclIdentifier * GetIdentifier(); //can be NULL!
    const TCHAR *GetIdentifierName(std::_tstring& name);
    bool IsDefault();
    bool IsLocal();
    const TCHAR *GetTransform(void);
    void SetIdentifierName(const TCHAR *id, bool local);
    void SetTransform(const TCHAR *ecl);
    void MakeDefault();
};
typedef StlLinked<IEclTransformAssignment> IEclTransformAssignmentAdapt;

__interface __declspec(uuid("2BBF24AA-DF13-4b3c-AE29-53F223AB52F1")) IEclParameter : public IEclElement
{
    const TCHAR * GetTypeName(std::_tstring &name);
    const TCHAR * GetParameter(std::_tstring &id);
    IEclType * GetType();
    bool IsConst() const;
    void SetParameter(const TCHAR *id);
    void SetType(const TCHAR *);
    void SetType(IEclType *);
};

__interface __declspec(uuid("BBFE58EE-22ED-490a-9E35-C6D0A0E6DB4E")) IEclValue : public IEclElement
{
    bool GetBoolValue(bool &value); //for true, false literals, return true if bool literal
    bool IsStringLiteral() const;
    bool IsDefaultValue() const;
    bool IsEmptySet() const;
    const TCHAR * GetStringValue(std::_tstring & result);
    bool IsIdentifierReference() const;
    IEclIdentifierReference* GetIdentifierReference();
    const TCHAR * GetIdentifierReferenceName(std::_tstring & name);
    bool IsParamaterizedAttribute() const;
    IEclParamaterizedAttribute* GetParamaterizedAttribute();
    //for raw value, just call Serialize!
};
typedef StlLinked<IEclValue> IEclValueAdapt ;

__interface __declspec(uuid("F27306DC-47A1-41cf-B7A0-DAF2DD02707F")) IEclValueList : public IEclElement
{
    unsigned GetValueCount();
    IEclValue * GetValue(unsigned pos); //0 based

    void AddValue(IEclValue *param, int pos=-1); //pos 0 is first, -1 last
    void DeleteValue(int pos);
};
typedef StlLinked<IEclValueList> IEclValueListAdapt ;

__interface __declspec(uuid("EFA21F5D-3551-4578-84E2-BA084C3D3372")) IEclParamaterizedAttribute : public IEclElement
{
    const TCHAR * GetIdentifierName(std::_tstring& name);
    void SetIdentifierName(const TCHAR *id);

    IEclValueList* GetValueList();
    void SetValueList(IEclValueList *list);
    bool IsStatement() const; //standalone statement (or function)
};


__interface __declspec(uuid("6F6B90E0-5C18-4ee0-9D92-2AD2D563654C")) IEclParameterList : public IEclElement
{
    unsigned GetParameterCount();
    IEclParameter * GetParameter(unsigned i);

    void AddParameter(IEclParameter *param, int pos=-1); //pos 0 is first, -1 last
    void DeleteParameter(int pos);
};

__interface __declspec(uuid("EFA21F5D-3551-4578-84E2-BA084C3D3372")) IEclFunctionDeclaration : public IEclElement
{
    const TCHAR *GetIdentifierName(std::_tstring& name);
    void SetIdentifierName(const TCHAR *id);

    IEclParameterList* GetParameterList();
    void SetParameterList(IEclParameterList *list);
};

__interface __declspec(uuid("DA34F6D6-4F4D-4b29-9EEB-4CF6A57B6112")) IEclTransform : public IEclElement
{
    IEclIdentifierReference * GetTargetType();
    const TCHAR * GetTargetTypeName(std::_tstring & result) const;
    const TCHAR * GetTargetQualifiedTypeName(std::_tstring & result) const;
    IEclIdentifier * GetIdentifier();
    const TCHAR * GetIdentifierName(std::_tstring& name);
    IEclIdentifierReference * GetSourceType();
    IEclIdentifier * GetSource();
    const TCHAR * GetSourceName(std::_tstring& name);

    unsigned GetFieldCount() const;
    IEclElement* GetField(unsigned i) const; //IEclTransformAssignment

    IEclTransformAssignment* AddField(IEclTransformAssignment *field, int pos=-1); //pos 0 is first, -1 last
    void DeleteField(int pos);

    void SetTargetType(IEclIdentifierReference *targetType);
    void SetIdentifier(IEclIdentifier *id);
    void SetSourceType(IEclIdentifierReference *sourceType);
    void SetSource(IEclIdentifier *source);

    void SetDefaultValues(bool value);	// self := [];
    bool GetDefaultValues() const;

    void SetNameMatchValues(bool value);	// self := left;
    bool GetNameMatchValues() const;
};
typedef StlLinked<IEclTransform> IEclTransformAdapt;

__interface __declspec(uuid("807f9b08-145b-496f-ba40-9c9d7db247f5")) IEclIdentifier : public IEclElement
{
    const TCHAR *GetIdentifierName(std::_tstring& name) const;
    void SetIdentifierName(const TCHAR *id);
    bool Equals(const std::_tstring & label) const;
    bool Equals(const IEclIdentifier * other) const;
};
typedef StlLinked<IEclIdentifier> IEclIdentifierAdapt;

__interface __declspec(uuid("6F998EBF-1F33-4ebf-B6C2-C372B54048E6")) IEclIdentifierReference : public IEclIdentifier
{
    int GetIdentifierCount(); //1 if just an identifier with no qualifiers
    const TCHAR * GetIdentifierName(int i, std::_tstring & name); //x.x.x := 2.1.0
    const TCHAR * GetIdentifierName(std::_tstring & name) const;
    const TCHAR * GetIdentifierMinorName(std::_tstring& name); //no qualifiers
};
typedef StlLinked<IEclIdentifierReference> IEclIdentifierReferenceAdapt;


__interface __declspec(uuid("24C35AD2-E2ED-45a2-82BA-71D46D246FDD")) IEclSizeOf : public IEclElement
{
    IEclIdentifierReference* GetIdentifier();
};

__interface __declspec(uuid("3E7FA8D2-FAC0-4010-BBCE-A1CB90C716EE")) IEclEmptySet : public IEclElement
{
};

__interface __declspec(uuid("C34BCD11-E297-4cbe-8A58-39AFF58FA8B1")) IEclSelfIdentifier : public IEclIdentifier
{
};

__interface __declspec(uuid("FA87FDA8-DA60-4601-BDE8-D27166BDD523")) IEclImport : public IEclElement
{
    int GetIdentifierCount(); //1 if just an identifier with no qualifiers
    const TCHAR * GetIdentifierName(int pos, std::_tstring & name); //0 based
};

__interface __declspec(uuid("1606BC65-BB51-4a50-B7F5-1FC2EBDBCD41")) IEclModuleExport : public IEclElement
{
    IEclType * GetType();
    IEclIdentifier * GetIdentifier();
};

__interface __declspec(uuid("1606BC65-BB51-4a50-B7F5-1FC2EBDBCD40")) IEclModule : public IEclElement
{
    const TCHAR * GetIdentifierName(std::_tstring & name);
    IEclParameterList* GetInputParameterList();
    unsigned GetExportsCount();
    IEclModuleExport* GetExport(unsigned i);
};

ECLLIB_API IEclProgram * ParseEcl(const std::_tstring &ecl);
ECLLIB_API IEclProgram * ParseEcl(const TCHAR *ecl);

ECLLIB_API LPCTSTR *GetTypeList();
enum EclType {
    eBOOLEAN_TYPE, 
    eDATA_TYPE, 
    eDECIMAL_TYPE,
    eINTEGER_TYPE,
    eREAL_TYPE, 
    eSTRING_TYPE 
};

ECLLIB_API IEclAssign *CreateIEclAssign(IEclIdentifier *identifier, IEclElement* rhs);
ECLLIB_API IEclBoolean *CreateIEclBoolean();
ECLLIB_API IEclDecimal *CreateIEclDecimal();
ECLLIB_API IEclString *CreateIEclString();
ECLLIB_API IEclInteger *CreateIEclInteger();
ECLLIB_API IEclReal *CreateIEclReal();
ECLLIB_API IEclData *CreateIEclData();
ECLLIB_API IEclIdentifier *CreateIEclIdentifier(const std::_tstring &id);
ECLLIB_API IEclIdentifierReference *CreateIEclIdentifierReference(const std::_tstring & qualifiedAttr);
ECLLIB_API IEclIdentifierReference *CreateIEclIdentifierReference(const std::_tstring & module, const std::_tstring & attr);
ECLLIB_API IEclDeclaration *CreateIEclDeclaration(IEclIdentifier *id,IEclType *type);
ECLLIB_API IEclRecord *CreateIEclRecord();
ECLLIB_API IEclRecord *CreateIEclRecord(IEclRecord *copy);
ECLLIB_API IEclDataset *CreateIEclDataset(IEclDataset *copy);
ECLLIB_API IEclDataset *CreateIEclDataset(IEclValue *file, IEclValue *record, ECLDATASETFORMAT format);
ECLLIB_API IEclTransformAssignment *CreateIEclTransformAssignment(const TCHAR* id, const TCHAR* expression, bool local = false);
ECLLIB_API IEclTransform *CreateIEclTransform(IEclIdentifier * id, IEclIdentifierReference * targetRecord);
ECLLIB_API IEclTransform *CreateIEclTransform(IEclTransform *copy);
ECLLIB_API IEclValue *CreateIEclValue(const std::_tstring & value);
ECLLIB_API IEclValueList *CreateIEclValueList(const TCHAR* values[] = 0, unsigned count = 0, bool bQuote = false);
ECLLIB_API IEclGrETLComment *CreateIEclGrETLComment();
ECLLIB_API IEclParameter *CreateIEclParameter(const std::_tstring &id, IEclType *type, const std::_tstring &typeId);
ECLLIB_API IEclParameterList *CreateIEclParameterList(IEclParameterList* list = 0);
ECLLIB_API IEclParamaterizedAttribute *CreateIEclParamaterizedAttribute(const std::_tstring &id, IEclValueList *valueList, bool bStatement);
ECLLIB_API IEclFunctionDeclaration *CreateIEclFunctionDeclaration(const TCHAR* name, IEclParameterList* list);
ECLLIB_API IEclOutput *CreateIEclOutput(IEclIdentifier *recordset, IEclRecord *record = 0);
ECLLIB_API IEclProject *CreateIEclProject(IEclIdentifier *dataset, IEclIdentifier *transform);
ECLLIB_API bool IsValidIdentifier(const std::_tstring & label);
