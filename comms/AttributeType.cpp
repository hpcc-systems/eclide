#include "StdAfx.h"

#include <Wildcard.h>
#include "AttributeType.h"
#include "cache.h"
#include <UtilFilesystem.h>

//  ===========================================================================

const TCHAR * const DUD_TEXT =
    _T("/*This is an example plugin template.\r\n")
    _T("The AUTHOR should be your risk ID, just the name without @risk at the end or RISK/ at the beginning.\r\n")
    _T("Sometimes ECL IDE does not accurately report all errors encountered when validating a composition.\r\n")
    _T("the errors can also be found at c:\\users\\userid\\AppData\\Local\\Temp\\err.log\r\n")
    _T("When incorporated into a composition, the properties for this plugin would be populated as shown:\r\n")
    _T("INSTANCE MyNewPlugin(Ins002)\r\n")
    _T("dsInput=^Ins001|dsOutput\r\n")
    _T("Prefix=\"myprefix_\"\r\n")
    _T("field1=myname\r\n")
    _T("field2=100\r\n")
    _T("field3=VAL1\r\n")
    _T("END\r\n")
    _T("*/\r\n")
    _T("NAME %1%;\r\n")
    _T("LABEL \"New Plugin\";\r\n")
    _T("VERSION \"1.0.0\";\r\n")
    _T("AUTHOR \"riskid\";\r\n")
    _T("CATEGORY CLEAN;\r\n")
    _T("DESCRIPTION \"Sample Plugin Template. This template takes an input dataset and appends three fields. \r\n")
    _T("the appended dataset is exposed for downstream plugins to hook into.\";\r\n")
    _T("PERMISSIONS\r\n")
    _T("VIEW:PUBLIC;\r\n")
    _T("EDIT:PRIVATE;\r\n")
    _T("RUN:PUBLIC;\r\n")
    _T("END\r\n")
    _T("INPUTS\r\n")
    _T("STRING Prefix:LABEL(\"Append Column Prefix\"),DEFAULT(\"append_\"),DESCRIPTION(\"This prefix will be appended to all fields added by this plugin. This allows the same plugin to be chained more than once in a composition without creating field conflicts.\");\r\n")
    _T("DATASET dsInput:MAPBYNAME\r\n")
    _T("FIELD field1:LABEL(\"Field to use for appended field 1\"), DESCRIPTION(\"the content of the field selected will be used for appended field 1\");\r\n")
    _T("END\r\n")
    _T("INT field2:LABEL(\"Number to put into appended field 2\"),DESCRIPTION(\"This value will be put into field 2\");\r\n")
    _T("ENUM(VAL1=\"Value 1 Label\",VAL2=\"Value 2 Label\",VAL3=\"Value 3 Label\") field3:LABEL(\"Value to put into appended field 3\");\r\n")
    _T("END\r\n")
    _T("OUTPUTS\r\n")
    _T("/*\r\n")
    _T("this output dataset contains all the fields in the dsInput dataset, plus three additional fields.\r\n")
    _T("all three fields will have the input prefix prepended to their name.\r\n")
    _T("the APPEND flag indicates that the data should only be appended to existing records; no existing data should be changed\r\n")
    _T("and no records removed or added. ECL to validate this will be added to the plugin ecl.\r\n")
    _T("*/\r\n")
    _T("DATASET dsOutput(dsInput):PREFIX(INPUTS.Prefix),APPEND\r\n")
    _T("STRING field1;\r\n")
    _T("INTEGER field2;\r\n")
    _T("STRING field3;\r\n")
    _T("END\r\n")
    _T("//this dataset is a side effect, not available to downstream plugins\r\n")
    _T("DATASET field1Stats: SIDE,WUID\r\n")
    _T("STRING field1_unique_value : FEW;\r\n")
    _T("INTEGER field1_cnt : FEW;\r\n")
    _T("END\r\n")
    _T("END\r\n")
    _T("GENERATES INLINE\r\n")
    _T("//this should be in a macro; inline here for reference\r\n")
    _T("//create a record structure reflecting the record structure of dsOutput\r\n")
    _T("l_new:=RECORD\r\n")
    _T("RECORDOF(%%dsInput%%);\r\n")
    _T("STRING %%Prefix%%field1;\r\n")
    _T("INTEGER %%Prefix%%field2;\r\n")
    _T("STRING %%Prefix%%field3;\r\n")
    _T("END;\r\n")
    _T("//export (^e)a dsOutput attribute from the dsInput attribute. \r\n")
    _T("//field 1 is set to the value of the content of the input field.\r\n")
    _T("//field 2 is set to a hardcoded integer value.\r\n")
    _T("//fieldl 3 is set to the enum value selected, wrapped in single quotes. (^q)\r\n")
    _T("//The output is persisted. (%%^p%%) \r\n")
    _T("%%^edsOutput%% := PROJECT(%%dsInput%%,TRANSFORM(l_new,\r\n")
    _T("SELF.%%Prefix%%field1:=LEFT.%%field1%%;\r\n")
    _T("SELF.%%Prefix%%field2:=%%field2%%;\r\n")
    _T("SELF.%%Prefix%%field3:=%%^qfield3%%;\r\n")
    _T("SELF:=LEFT) %%^p%%;\r\n")
    _T("%%^efield1Stats%% := TABLE(%%dsOutput%%,\r\n")
    _T("{field1_unique_value:=%%Prefix%%field1, field1_cnt:=COUNT(GROUP)}\r\n")
    _T(",%%Prefix%%field1);\r\n")
    _T("ENDGENERATES\r\n");

const TCHAR * const CMP_TEXT = 
    _T("NAME %1%;\r\n")
    _T("LABEL \"New Composition\";\r\n")
    _T("AUTHOR \"riskid\";\r\n")
    _T("VERSION \"1.0.0\";\r\n")
    _T("DESCRIPTION \"Sample Template for a new composition.\r\n")
    _T("This composition includes two plugins, UseDataset (which reads data in from a thor file)\r\n")
    _T("hooked up to OutDataset (which writes data out to a thor file). Both are located in your \r\n")
    _T("HPCCSystems/(version)/HIPIE_Plugins folder. This folder is retrieved via git from \r\n")
    _T("http://gitlab.ins.risk.regn.net/HIPIE/HIPIE_Plugins.\r\n")
    _T("Plugin Developers may replace this with their own plugin repo by editing the repositories.cfg\r\n")
    _T("file in the HPCCSystems/(version) directory.\r\n")
    _T("The AUTHOR should be your risk ID, just the name without @risk at the end or RISK/ at the beginning.\r\n")
    _T("Saving this composition inside ECL IDE will generate a new module named HIPIE_(composition name) that contains\r\n")
    _T("the ecl generated by HIPIE for this composition. You may need to refresh the repository window \r\n")
    _T("for the new module to appear.\r\n")
    _T("Sometimes ECL IDE does not accurately report all errors encountered when validating a composition.\r\n")
    _T("the errors can also be found at c:\\users\\userid\\AppData\\Local\\Temp\\err.log\";\r\n")
    _T("PERMISSIONS\r\n")
    _T("VIEW:PRIVATE;\r\n")
    _T("EDIT:PRIVATE;\r\n")
    _T("RUN:PRIVATE;\r\n")
    _T("END\r\n")
    _T("INSTANCE HIPIE_Plugins.UseDataset.UseDataset(Ins001)\r\n")
    _T("LogicalFilename=\"~temp::infile\"\r\n")
    _T("Method=THOR\r\n")
    _T("Structure=INTEGER|field1,STRING|myname,STRING30|addr1\r\n")
    _T("END\r\n")
    _T("INSTANCE HIPIE_Plugins.OutDataset.OutDataset(Ins002)\r\n")
    _T("dsInput=^Ins001|dsOutput\r\n")
    _T("name=\"~temp::outputfile\"\r\n")
    _T("Method=THOR\r\n")
    _T("END\r\n");

    const TCHAR * const DEFT_TEXT =
    _T("/*This is an example DEFT training file.\r\n")
    _T("David Bayliss => David||Bayliss\r\n")
    _T("Steve Baglien => Steve||Baglien\r\n")
    _T("John Smith => John||Smith\r\n")
    _T("David Alan & Kelly Marie Van Bayliss => David|Alan|Van Bayliss => Kelly Marie||Van Bayliss\r\n")
    _T("David Alan   Bayliss => David|Alan|Bayliss\r\n")
    _T("Bayliss, David Alan => David|Alan|Bayliss\r\n")
    _T("David Lawrence-Hide => David||Lawrence Hide\r\n")
    _T("Sean Van Allen => Sean||Van Allen\r\n")
    _T("Jerry Mc Ever => Jerry||Mc Ever\r\n");

    const TCHAR * const LUCI_FAMILY_TEXT =
    _T("/*This is an example LUCI family file.\r\n")
    _T("MODULE:%1%\r\n")
    _T("luci/luci_ten_trees.csv\r\n")
    _T("luci/LUCI_X014.csv\r\n");

class CAttributeType : public IAttributeType, public CUnknown
{
protected:
    std::_tstring m_repositoryCode;
    std::_tstring m_fileExtension;
    std::_tstring m_description;
    std::_tstring m_defaultText;

public:
    CAttributeType(const std::_tstring & repositoryCode) : m_repositoryCode(repositoryCode)
    {
        boost::algorithm::trim_left_if(m_repositoryCode, boost::algorithm::is_any_of(_T(".")));
        boost::algorithm::to_lower(m_repositoryCode);
        if (m_repositoryCode.empty())
        {
            m_repositoryCode = ATTRIBUTE_TYPE_ECL;
        }
    }

    BEGIN_CUNKNOWN
    END_CUNKNOWN(CUnknown)

    const TCHAR * GetCacheID() const
    {
        return m_repositoryCode.c_str();
    }
    const TCHAR * GetRepositoryCode()
    {
        return m_repositoryCode.c_str();
    }
    const TCHAR * GetFileExtension()
    {
        if (m_fileExtension.empty())
        {
            m_fileExtension = _T(".");
            m_fileExtension += GetRepositoryCode();
        }
        return m_fileExtension.c_str();
    }
    const TCHAR * GetDescription()
    {
        if(m_description.empty())
        {
            if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECL))
                m_description = _T("ECL - Enterprise Control Language");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECM) || boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ESDL))
                m_description = _T("ESDL (ECM) - Enterprise Services Definition Language");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_SALT))
                m_description = _T("SPC - Salt Definition File");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_KEL))
                m_description = _T("KEL - Knowlege Engineering Language");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_DUD))
                m_description = _T("DUD - HIPIE Contract");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_CMP))
                m_description = _T("CMP - HIPIE Composition");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_WORKSPACE))
                m_description = _T("ECL_WS - ECL Workspace Persistance File");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_LUCIFAMILY))
                m_description = _T("MFF - Luci Family File");
            else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_DEFT))
                m_description = _T("DFT - Training file");
            else
                m_description = _T("???");
        }
        return m_description.c_str();
    }
    const TCHAR * GetDefaultText(const std::_tstring & attrLabel)
    {
        if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECL))
            m_defaultText = (boost::_tformat(_T("EXPORT %1% := 'todo';")) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ECM) || boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_ESDL))
            m_defaultText = (boost::_tformat(_T("ESDL (ECM):  %s;")) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_SALT))
            m_defaultText = (boost::_tformat(_T("SALT:  %s;")) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_KEL))
            m_defaultText = (boost::_tformat(_T("KEL:  %s;")) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_DUD))
            m_defaultText = (boost::_tformat(DUD_TEXT) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_CMP))
            m_defaultText = (boost::_tformat(CMP_TEXT) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_WORKSPACE))
            m_defaultText = (boost::_tformat(_T("ECL_WS:  %s;")) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_DEFT))
            m_defaultText = (boost::_tformat(DEFT_TEXT) % attrLabel.c_str()).str();
        else if (boost::algorithm::equals(m_repositoryCode, ATTRIBUTE_TYPE_LUCIFAMILY))
            m_defaultText = (boost::_tformat(LUCI_FAMILY_TEXT) % attrLabel.c_str()).str();
        else
            m_defaultText = _T("???");

        return m_defaultText.c_str();
    }
    bool IsReadOnly()
    {
        if (boost::algorithm::iends_with(m_repositoryCode, ATTRIBUTE_TYPE_PLUGIN))
            return true;
        return false;
    }

    void Update(const std::_tstring & description)
    {
        if (!description.empty())
            m_description = description;
    }
};

//  ===========================================================================
static CacheT<std::_tstring, CAttributeType> AttributeTypeCache;

IAttributeType *AttributeTypeFromExtension(const std::_tstring & extension) {
    IAttributeType *attrType = NULL;
    if (boost::algorithm::iequals(extension, ATTRIBUTE_TYPE_ESDL) || boost::algorithm::iequals(extension, ATTRIBUTE_TYPE_ECM))
        attrType = CreateIAttributeESDLType();
    else if (boost::algorithm::iequals(extension, ATTRIBUTE_TYPE_SALT))
        attrType = CreateIAttributeSALTType();
    else if (boost::algorithm::iequals(extension, ATTRIBUTE_TYPE_KEL))
        attrType = CreateIAttributeKELType();
    else if (boost::algorithm::iequals(extension, ATTRIBUTE_TYPE_DUD))
        attrType = CreateIAttributeDUDType();
    else
        attrType = CreateIAttributeECLType();

    return attrType;
}

void ClearAttributeTypeCache()
{
    ATLTRACE(_T("File cache before clearing(size=%u)\r\n"), AttributeTypeCache.Size());
    AttributeTypeCache.Clear();
}

IAttributeType * CreateIAttributeType(const std::_tstring & repositoryType, const std::_tstring & description)
{
    CAttributeType * retVal = AttributeTypeCache.Get(new CAttributeType(repositoryType));
    retVal->Update(description);
    return retVal;
}
IAttributeType * CreateIAttributeGENERALType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_GENERAL));
}
IAttributeType * CreateIAttributeECLType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_ECL));
}
IAttributeType * CreateIAttributeESDLType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_ESDL));
}
IAttributeType * CreateIAttributeECMType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_ECM));
}
IAttributeType * CreateIAttributeSALTType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_SALT));
}
IAttributeType * CreateIAttributeKELType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_KEL));
}
IAttributeType * CreateIAttributeDUDType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_DUD));
}
IAttributeType * CreateIAttributeCMPType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_CMP));
}
IAttributeType * CreateIAttributeDEFTType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_DEFT));
}
IAttributeType * CreateIAttributeLUCIFAMILYType()
{
    return AttributeTypeCache.Get(new CAttributeType(ATTRIBUTE_TYPE_LUCIFAMILY));
}
unsigned int GetAttributeTypes(IAttributeTypeVector & types)
{
    types.push_back(CreateIAttributeGENERALType());
    types.push_back(CreateIAttributeECLType());
    types.push_back(CreateIAttributeESDLType());
    types.push_back(CreateIAttributeSALTType());
    types.push_back(CreateIAttributeKELType());
    types.push_back(CreateIAttributeDUDType());
    types.push_back(CreateIAttributeCMPType());
    types.push_back(CreateIAttributeDEFTType());
    types.push_back(CreateIAttributeLUCIFAMILYType());
    return types.size();
}
bool IsValidExtension(const std::_tstring & ext)
{
    if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECL))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECLMOD))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECLLIB))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ECM) || boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_ESDL))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_SALT))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_KEL))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_DUD))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_CMP))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_PLUGIN))
        return true;
    else if (WildMatch(ext, std::_tstring(_T(".")) + ATTRIBUTE_TYPE_ECLOTHER, true))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_DEFT))
        return true;
    else if (boost::algorithm::iends_with(ext, ATTRIBUTE_TYPE_LUCIFAMILY))
        return true;
    return false;
}
COMMS_API bool HasValidExtension(const std::_tstring & filename)
{
    return IsValidExtension(pathToWString(boost::filesystem::extension(filename)));
}
