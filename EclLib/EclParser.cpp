#include "StdAfx.h"
#include "eclparser.h"
#include <clib.h>
#include <util.h> //clib
#include "ecl_grammar.h"
#include "UnicodeFile.h"
#include "SAXHandlersBase.h"
#include "XMLWriter.h"
#include <limits>
typedef std::vector<IEclIdentifierAdapt> Identifiers_t;

static const TCHAR BOOLEAN_TYPE[] = _T("boolean");
static const TCHAR DATA_TYPE[] = _T("data");
static const TCHAR DECIMAL_TYPE[] = _T("decimal");
static const TCHAR INTEGER_TYPE[] = _T("integer");
static const TCHAR REAL_TYPE[] = _T("real");
static const TCHAR STRING_TYPE[] = _T("string");

//match this with enum EclType
LPCTSTR g_TypeList[] = {
    BOOLEAN_TYPE, 
        DATA_TYPE, 
        DECIMAL_TYPE,
        INTEGER_TYPE,
        REAL_TYPE, 
        STRING_TYPE,
        0
};

LPCTSTR *GetTypeList()
{
    return g_TypeList;
}

void trim(std::_tstring & str)
{
    boost::algorithm::trim_if(str, boost::algorithm::is_any_of(_T(";\r\n \t")));
}

static const TCHAR ExportToken[] = _T("export");
static const TCHAR SharedToken[] = _T("shared");
static const TCHAR SelfToken[] = _T("self");

//  ===========================================================================
typedef StlLinked<IEclElement> IEclElementAdapt;
typedef std::vector<IEclElementAdapt> IEclElementVector;

typedef char const* iterator_t;
typedef boost::spirit::classic::tree_match<iterator_t> parse_tree_match_t;
typedef parse_tree_match_t::tree_iterator iter_t;

IEclElement *CreateIEclParamaterizedAttribute(const iter_t &itr);
IEclValueList *CreateIEclValueList(const iter_t &itr);

//  ===========================================================================
class CEclType;

class CEclElement : public CUnknown
{
public:
    CEclElement()
    {
    }

    virtual ~CEclElement()
    {
    }

    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclElement)
    END_CUNKNOWN(CUnknown)
};

template <class T>
class CEclElementImpl : public CEclElement
{
private:
    IEclElementVector m_children;

protected:

    void clone(const CEclElementImpl &copy)
    {
        m_children = copy.m_children;
    }

public:

    //virtual IEclElement *Clone() =0; //maybe some day

    unsigned GetChildCount() const
    {
        return m_children.size();
    }

    IEclElement * GetChild(unsigned item) const
    {
        ATLASSERT(item >= 0 && item < GetChildCount());
        return m_children[item].get();
    }

    void AddChild(IEclElement *newChild)
    {
        AddChild(newChild,-1); //add to end
    }

    void AddChild(IEclElement *newChild, int pos)
    {
        ATLASSERT(newChild);
        //this is just to eat the ref count, since we don't Detach on other end
        IEclElementAdapt el = newChild;

        if ( pos < 0 || unsigned(pos) >= GetChildCount() )
            m_children.push_back(newChild);
        else
            m_children.insert(m_children.begin() + pos, newChild);
    }

    void DeleteChild(int pos)
    {
        m_children.erase(m_children.begin() + pos);
    }

    bool DeleteChild(IEclElement *child)
    {
        ATLASSERT(child);
        for(IEclElementVector::iterator i=m_children.begin(); i!=m_children.end(); ++i)
        {
            if ( child == *i )
            {
                m_children.erase(i);
                return true;
            }
        }
        return false;
    }

    template<typename T>
    IEclType * CreateType(T j)
    {
        parser_id id = j->value.id();
        if (id == ecl_grammar::typeBooleanID)
            return (IEclBoolean*)new CEclBoolean(j);
        else if (id == ecl_grammar::typeIntegerID)
            return (IEclInteger*)new CEclInteger(j, false, true);
        else if (id == ecl_grammar::typeUnsignedID)
            return (IEclInteger*)new CEclInteger(j, true, false);
        else if (id == ecl_grammar::typeRealID)
            return (IEclReal*)new CEclReal(j);
        else if (id == ecl_grammar::typeDecimalID)
            return (IEclDecimal*)new CEclDecimal(j, false);
        else if (id == ecl_grammar::typeUDecimalID)
            return (IEclDecimal*)new CEclDecimal(j, true);
        else if (id == ecl_grammar::typeStringID)
            return (IEclString*)new CEclString(j);
        else if (id == ecl_grammar::typeQStringID)
            return (IEclString*)new CEclString(j);
        else if (id == ecl_grammar::typeUnicodeID)
            return (IEclString*)new CEclString(j);
        else if (id == ecl_grammar::typeVarStringID)
            return (IEclString*)new CEclString(j);
        else if (id == ecl_grammar::typeVarUnicodeID)
            return (IEclString*)new CEclString(j);
        else if (id == ecl_grammar::typeDataID)
            return (IEclData*)new CEclData(j);
        else
        {
            ATLTRACE(_T("Unknown Type (%0x)\n"), id );
            AtlThrow(E_FAIL);
        }
    }

    template<typename T>
    void AddTypeChild(T j)
    {
        AddChild(CreateType(j));
    }

    void ProcessRecordBody(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        for(i; i != itr->children.end(); ++i)
        {
            CComPtr<IEclComment> comment;
            if (i->value.id() == ecl_grammar::commentID)
            {
                comment = new CEclComment(i);
                ++i;
                if (i == itr->children.end())
                {
                    break;
                }
            }

            if (i->value.id() == ecl_grammar::assignmentID)
                AddChild(new CEclAssign(i));
            else if (i->value.id() == ecl_grammar::declAndAssignID)
                AddChild(new CEclDeclAndAssign(i));
            else if (i->value.id() == ecl_grammar::declarationID)
                AddChild(new CEclDeclaration(i));
            else if (i->value.id() == ecl_grammar::recordID)
            {
                //Do Nothing...
            }
            else
            {
                ATLTRACE("CEclRecord::ProcessRecordBody TODO:  %i\n", i->value.id());
            }
        }
    }

    const TCHAR* SerializeChildren(std::_tstring &ecl, int depth = 0) const
    {
        for(unsigned i = 0; i < GetChildCount(); ++i)
        {
            IEclElement *child = GetChild(i);
            std::_tstring childEcl;
            child->Serialize(childEcl, depth);
            if(childEcl.length())
            {
                ecl += childEcl;
                CComQIPtr<IEclComment> comment = child;
                CComQIPtr<IEclGrETLComment> gretlComment = child;
                if(!(comment || gretlComment) && !(boost::algorithm::ends_with(childEcl, _T(";"))))
                {
                    ecl += _T(";");
                }

                if(!(boost::algorithm::ends_with(childEcl, _T("\r\n"))))
                {
                    ecl += _T("\r\n");
                }
            }
        }
        return ecl.c_str();
    }

    const TCHAR* Serialize(std::_tstring &ecl, int /*depth = 0*/) const
    {
        ATLASSERT(!"Unable to serialize base class.");
        return ecl.c_str();
    }

    bool ReplaceElement(IEclElement* from, IEclElement* to)
    {
        ATLASSERT(from);
        ATLASSERT(to);
        for(IEclElementVector::iterator i=m_children.begin(); i!=m_children.end(); ++i)
        {
            if ( from == i->get() )
            {
                *i = to;
                return true;
            }
        }
        return false;
    }

};

//  ===========================================================================
class CEclUnknown : public CEclElementImpl<CEclUnknown>, public IEclUnknown 
{
    typedef CEclElementImpl<CEclUnknown> base;

protected:
    std::_tstring m_ecl;
    bool m_bAddSemi;
    bool IsOtherChild(boost::spirit::classic::parser_id id)
    {
        return (id == ecl_grammar::otherSemiID || 
            id == ecl_grammar::otherEolID ||
            id == ecl_grammar::otherEofID ||
            id == ecl_grammar::otherModuleID);
    }

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclUnknown)
    END_CUNKNOWN(base)


    CEclUnknown(const iter_t &itr)
    {
        //one element
        iter_t i = itr->children.begin();
        boost::spirit::classic::parser_id id = i->value.id();
        if(!IsOtherChild(id))
            AtlThrow(E_FAIL);

        m_bAddSemi = (id == ecl_grammar::otherSemiID);
        m_bAddSemi = false;

        if ( i->children.size() )
            i = i->children.begin();

        m_ecl.assign(i->value.begin(), i->value.end());
        trim(m_ecl);
    }

    CEclUnknown(IEclUnknown *copy)
    {
        ATLASSERT(copy);
        m_ecl = copy->GetEcl();
        m_bAddSemi = copy->EndsInSemi();
    }

    CEclUnknown(const CEclUnknown &copy)
    {
        m_ecl = copy.m_ecl;
        m_bAddSemi = copy.m_bAddSemi;
    }

    IEclElement *Clone()
    {
        return new CEclUnknown(*this);
    }

    //IEclUnknown
    const TCHAR * GetEcl()
    {
        return m_ecl.c_str();
    }
    void SetEcl(const TCHAR *ecl)
    {
        ATLASSERT(ecl);
        m_ecl = ecl;
    }
    bool EndsInSemi(void)
    {
        return m_bAddSemi;
    }

    //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        if (m_ecl.length())
        {
            for (int j = depth; j > 0; j--)
                ecl += _T("\t");

            ecl += m_ecl.c_str();

            if ( m_bAddSemi )
                ecl += _T(";\r\n");
        }
        return ecl.c_str();
    }
};

//  ===========================================================================
class CEclLineComment : public CEclElementImpl<CEclLineComment>, public IEclComment 
{
    typedef CEclElementImpl<CEclLineComment> base;
protected:
    std::_tstring m_comment;
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclComment)
    END_CUNKNOWN(base)


    CEclLineComment(const iter_t &itr) 
    {
        iter_t i = itr->children.begin();
        m_comment.assign(i->value.begin(), i->value.end());
    }
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += m_comment.c_str();
        return ecl.c_str();
    }
    //IEclComment
    const TCHAR *GetComment(std::_tstring& comment)
    {
        comment = m_comment.c_str();
        return comment.c_str();
    }
    bool IsLineComment() const
    {
        return true;
    }
    bool IsInvokeComment() const
    {
        return false;
    }
    bool IsMetaDataComment() const
    {
        return false;
    }
};
//  ===========================================================================
class CEclBlockComment : public CEclElementImpl<CEclBlockComment>, public IEclComment
{
    typedef CEclElementImpl<CEclBlockComment> base;
protected:
    std::_tstring m_comment;
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclComment)
    END_CUNKNOWN(base)


    CEclBlockComment(const iter_t &itr) 
    {
        iter_t i = itr->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::commentContentID);
        iter_t j = i->children.begin();
        ATLASSERT(j->value.id() == ecl_grammar::commentContentID);
        m_comment.assign(j->value.begin(), j->value.end());

    }
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("/*");
        ecl += m_comment.c_str();
        ecl += _T("*/");
        return ecl.c_str();
    }
    //IEclComment
    const TCHAR *GetComment(std::_tstring& comment)
    {
        comment = m_comment.c_str();
        return comment.c_str();
    }
};


//  ===========================================================================
class GrETLHandlerImpl : public StackHandlerImpl
{
private:
    bool m_position;
    bool m_state;
    bool m_size;

public:
    bool m_window;
    bool m_showEdgeLabels;
    bool m_compressRows;
    int m_x;
    int m_y;
    bool m_bWSMin;
    bool m_bWSMax;
    int m_topindex;
    int m_width;
    int m_height;

    GrETLHandlerImpl()
    {
        m_window = false;
        m_showEdgeLabels = false;
        m_compressRows = false;
        m_position = false;
        m_state = false;
        m_size = false;
        m_x = 0;
        m_y = 0;
        m_bWSMin = false;
        m_bWSMax = false;
        m_topindex = 0;
        m_width = 0;
        m_height = 0;
    }

    virtual HRESULT STDMETHODCALLTYPE startElement( 
        /* [in] */ const wchar_t *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ const wchar_t *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ const wchar_t *pwchQName,
        /* [in] */ int cchQName,
        /* [in] */ ISAXAttributes *pAttributes)
    { 
        __super::startElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName, pAttributes);

        Element &e = m_stack.top();
        if (m_window==false && boost::algorithm::iequals(e.m_tag, _T("window")))
        {
            m_window = true;
            return S_OK;
        }

        if(m_window)
        {
            if (boost::algorithm::iequals(e.m_tag, _T("position")))
            {
                m_position = true;
            }
            else if (boost::algorithm::iequals(e.m_tag, _T("state")))
            {
                m_state=true;
            }
            else if (boost::algorithm::iequals(e.m_tag, _T("size")))
            {
                m_size=true;
            }
        }

        return S_OK; 
    }

    virtual HRESULT STDMETHODCALLTYPE endElement( 
        /* [in] */ const wchar_t *pwchNamespaceUri,
        /* [in] */ int cchNamespaceUri,
        /* [in] */ const wchar_t *pwchLocalName,
        /* [in] */ int cchLocalName,
        /* [in] */ const wchar_t *pwchQName,
        /* [in] */ int cchQName)
    { 
        Element & e = m_stack.top();

        if (boost::algorithm::iequals(e.m_tag, _T("position")))
        {
            m_position = false;
        }
        else if (boost::algorithm::iequals(e.m_tag, _T("state")))
        {
            m_state=false;
        }
        else if (boost::algorithm::iequals(e.m_tag, _T("size")))
        {
            m_size=false;
        }

        if(m_position == true)
        {
            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("x")))
                {
                    m_x = (int)boost::lexical_cast<int>(e.GetContent());
                }
                if (boost::algorithm::iequals(e.m_tag, _T("y")))
                {
                    m_y = (int)boost::lexical_cast<int>(e.GetContent());
                }
            }
            catch (boost::bad_lexical_cast &)
            {
                m_x = 0;
                m_y = 0;
            }
        }

        if(m_state == true)
        {
            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("minimized")))
                    m_bWSMin = (bool)boost::lexical_cast<bool>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &)
            {
                m_bWSMin=false;
            }

            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("maximized")))
                    m_bWSMax = (bool)boost::lexical_cast<bool>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &)
            {
                m_bWSMax=false;
            }

            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("scrollpos")))
                    m_topindex = (int)boost::lexical_cast<int>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &)
            {
                m_topindex=0;
            }
        }

        if(m_size == true)
        {
            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("width")))
                    m_width = (int)boost::lexical_cast<int>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &) 
            {
                m_width=0;
            }

            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("height")))
                    m_height = (int)boost::lexical_cast<int>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &) 
            {
                m_width=0;
            }
        }

        if(!m_window)
        {
            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("show_edge_labels")))
                    m_showEdgeLabels = (bool)boost::lexical_cast<bool>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &)
            {
                m_showEdgeLabels=false;
            }

            try
            {
                if (boost::algorithm::iequals(e.m_tag, _T("hide_connected_rows")))
                    m_compressRows = (bool)boost::lexical_cast<bool>(e.GetContent());
            }
            catch (boost::bad_lexical_cast &)
            {
                m_compressRows=false;
            }
        }

        return __super::endElement(pwchNamespaceUri, cchNamespaceUri, pwchLocalName, cchLocalName, pwchQName, cchQName);
    }
};

class CEclGrETLComment : public CEclElementImpl<CEclGrETLComment>, public IEclGrETLComment
{
    typedef CEclElementImpl<CEclGrETLComment> base;
protected:
    mutable std::_tstring m_xml;
    bool m_window;
    bool m_showEdgeLabels;
    bool m_compressRows;
    int m_x;
    int m_y;
    bool m_bWSMin;
    bool m_bWSMax;
    int m_topindex;
    int m_width;
    int m_height;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclGrETLComment)
    END_CUNKNOWN(base)


    CEclGrETLComment(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::GrETLContentID);
        iter_t j = i->children.begin();
        ATLASSERT(j->value.id() == ecl_grammar::commentContentID);
        iter_t k = j->children.begin();
        ATLASSERT(k->value.id() == ecl_grammar::commentContentID);
        m_xml.assign(k->value.begin(), k->value.end());
        trim(m_xml);

        CComPtr<GrETLHandlerImpl> pMc = new GrETLHandlerImpl();
        CComInitialize com;
        CComPtr<ISAXXMLReader> pRdr; 
        HRESULT hr = pRdr.CoCreateInstance(CLSID_SAXXMLReader30);//, NULL, CLSCTX_ALL, IID_ISAXXMLReader, (void **)&pRdr); 
        if (hr == S_OK)
        {
            hr = pRdr->putContentHandler(pMc); 
            if (hr == S_OK)
            {
                hr = pRdr->parse(CComVariant(m_xml.c_str()));
                m_window=pMc->m_window;
                if(m_window)
                {
                    m_x = pMc->m_x;
                    m_y = pMc->m_y;
                    m_bWSMin = pMc->m_bWSMin;
                    m_bWSMax = pMc->m_bWSMax;
                    m_topindex = pMc->m_topindex;
                    m_height = pMc->m_height;
                    m_width = pMc->m_width;
                }
                else
                {
                    m_showEdgeLabels = pMc->m_showEdgeLabels;
                    m_compressRows = pMc->m_compressRows;
                }
            }
        }
    }

    CEclGrETLComment()
        : m_window(false), m_showEdgeLabels(false), m_compressRows(false),
        m_x(0), m_y(0), m_bWSMin(false), m_bWSMax(false), m_topindex(0),
        m_height(0), m_width(0)
    {
    }

    CEclGrETLComment(const CEclGrETLComment &copy)
    {
        m_xml = copy.m_xml;
    }

    IEclElement *Clone()
    {
        return new CEclGrETLComment(*this);
    }

    //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        static const TCHAR * const GRETL_COMMENT = _T("/*--GrETL--\r\n%1%\r\n*/\r\n");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        std::_tstring tpl;
        std::_tstring fields;

        CComPtr<IXMLWriter> writer = CreateIXMLWriter(true);

        if(m_window)
        {
            writer->PushElement(_T("window"));

            writer->PushElement(_T("position"));

            writer->PushElement(_T("x"), boost::lexical_cast<std::_tstring>(m_x).c_str());
            writer->PopElement();
            writer->PushElement(_T("y"), boost::lexical_cast<std::_tstring>(m_y).c_str());
            writer->PopElement();

            writer->PopElement();

            writer->PushElement(_T("state"));

            writer->PushElement(_T("minimized"), m_bWSMin ? _T("1"): _T("0"));
            writer->PopElement();
            writer->PushElement(_T("maximized"), m_bWSMax ? _T("1"): _T("0"));
            writer->PopElement();
            writer->PushElement(_T("scrollpos"), boost::lexical_cast<std::_tstring>(m_topindex).c_str());
            writer->PopElement();

            writer->PopElement();

            writer->PushElement(_T("size"));

            writer->PushElement(_T("width"), boost::lexical_cast<std::_tstring>(m_width).c_str());
            writer->PopElement();
            writer->PushElement(_T("height"), boost::lexical_cast<std::_tstring>(m_height).c_str());
            writer->PopElement();

            writer->PopElement();

            writer->PopElement();
        }
        else
        {
            writer->PushElement(_T("master"));

            writer->PushElement(_T("show_edge_labels"), m_showEdgeLabels ? _T("1"): _T("0"));
            writer->PopElement();
            writer->PushElement(_T("hide_connected_rows"), m_compressRows ? _T("1"): _T("0"));
            writer->PopElement();

            writer->PopElement();
        }

        writer->EndDocument(fields);

        tpl = (boost::_tformat(GRETL_COMMENT) % fields).str();
        // refresh m_xml
        m_xml=tpl.c_str();

        ecl += tpl.c_str();
        return ecl.c_str();
    }

    //IEclComment
    const TCHAR *GetComment(std::_tstring& comment)
    {
        comment = m_xml.c_str();
        return comment.c_str();
    }

    //IEclGrETLComment
    bool IsWindow() const
    {
        return m_window;
    }

    void SetWindow(bool wnd)
    {
        m_window=wnd;
    }

    bool IsShowEdgeLabels() const
    {
        return m_showEdgeLabels;
    }

    void SetShowEdgeLabels(bool show)
    {
        m_showEdgeLabels=show;
    }

    bool IsCompressRows() const
    {
        return m_compressRows;
    }

    void SetCompressRows(bool compress)
    {
        m_compressRows=compress;
    }

    int GetX() const
    {
        return m_x;
    }

    int GetY() const
    {
        return m_y;
    }

    void SetX(int x)
    {
        m_x=x;
    }

    void SetY(int y)
    {
        m_y=y;
    }

    bool IsMinimized() const
    {
        return m_bWSMin;
    }

    void SetMinimized(bool set)
    {
        m_bWSMin=set;
    }

    bool IsMaximized() const
    {
        return m_bWSMax;
    }

    void SetMaximized(bool set)
    {
        m_bWSMax=set;
    }

    int GetTopIndex() const
    {
        return m_topindex;
    }

    void SetTopIndex(int idx/*=0*/)
    {
        m_topindex=idx;
    }

    int GetWidth() const
    {
        return m_width;
    }

    void SetWidth(int width)
    {
        m_width=width;
    }

    int GetHeight() const
    {
        return m_height;
    }

    void SetHeight(int height)
    {
        m_height=height;
    }
};


//  ===========================================================================
class CEclInvokeComment : public CEclElementImpl<CEclInvokeComment>, public IEclInvokeComment
{
    typedef CEclElementImpl<CEclInvokeComment> base;
protected:
    std::_tstring m_label;
    std::_tstring m_ecl;
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclInvokeComment)
    END_CUNKNOWN(base)


    CEclInvokeComment(const iter_t &itr)
    {
        iter_t invokeItr = itr->children.begin();	
        iter_t invokeChildItr = invokeItr->children.begin();	
        iter_t invokeLabelPartsItr = invokeChildItr->children.begin();	
        ++invokeLabelPartsItr;	//Skip "--INVOKE("
        m_label.assign(invokeLabelPartsItr->value.begin(), invokeLabelPartsItr->value.end());
        trim(m_label);

        ++invokeChildItr;	//Move to Content
        iter_t commentContentItr = invokeChildItr->children.begin();	
        m_ecl.assign(commentContentItr->value.begin(), commentContentItr->value.end());
        trim(m_ecl);
    }
    const TCHAR * GetLabel()
    {
        return m_label.c_str();
    }
    const TCHAR * GetEcl()
    {
        return m_ecl.c_str();
    }
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += (boost::_tformat(_T(
            "/*--INVOKE(%1%)--\r\n\
            %2%\r\n\
            */")) % m_label % m_ecl).str();
        return ecl.c_str();
    }
    //IEclComment
    const TCHAR *GetComment(std::_tstring& comment)
    {
        comment = m_ecl.c_str();
        return comment.c_str();
    }
};

//  ===========================================================================
class CEclComment : public CEclElementImpl<CEclComment>, public IEclComment
{
    typedef CEclElementImpl<CEclComment> base;
protected:
    StlLinked<IEclComment> m_comment;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclComment)
    END_CUNKNOWN(base)


    CEclComment(const iter_t &itr) 
    {
        //must be exactly one child!
        iter_t i = itr->children.begin();
        for(i; i != itr->children.end(); ++i)
        {
            if(i->value.id() == ecl_grammar::blockCommentID)
            {
                m_comment = new CEclBlockComment(i);
            }
            else if (i->value.id() == ecl_grammar::lineCommentID)
            {
                m_comment = new CEclLineComment(i);
            }
            else if (i->value.id() == ecl_grammar::invokeCommentID)
            {
                m_comment = new CEclInvokeComment(i);
            }
            else
            {
                ATLTRACE("CEclComment missing handler:  %i", i->value.id());
            }
        }
    }
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        m_comment->Serialize(ecl, depth);
        return ecl.c_str();
    }

    //IEclComment
    const TCHAR *GetComment(std::_tstring& comment)
    {
        return m_comment->GetComment(comment);
    }
};
//  ===========================================================================
class CEclAttribute : public CEclElementImpl<CEclAttribute>, public IEclAttribute
{
    typedef CEclElementImpl<CEclAttribute> base;
protected:
    std::_tstring m_ecl;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclAttribute)
    END_CUNKNOWN(base)


        CEclAttribute(const iter_t &itr)
    {
        //one element
        iter_t i = itr->children.begin();
        m_ecl.assign(i->value.begin(), i->value.end());
        trim(m_ecl);
    }

    CEclAttribute(const CEclAttribute &copy)
    {
        m_ecl = copy.m_ecl;
    }

    IEclElement *Clone()
    {
        return new CEclAttribute(*this);
    }

    const TCHAR * GetAttribute()
    {
        return m_ecl.c_str();
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += m_ecl.c_str();
        return ecl.c_str();
    }
};
//  ===========================================================================

class CEclType : public CEclElementImpl<CEclType>
{
    typedef CEclElementImpl<CEclType> base;
public:
    BEGIN_CUNKNOWN
    END_CUNKNOWN(base)


    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        ATLASSERT(!"TODO.");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        return ecl.c_str();
    }

    const TCHAR *GetTypeName()
    {
        ATLASSERT(!"Abstract Base");
        return 0;
    }

    unsigned GetFixedLogicalWidth()
    {
        return 0; //unknown
    }
    void SetFixedLogicalWidth(unsigned  /*width*/)
    {
        //nothing todo?
    }
    unsigned GetFixedPhysicalWidth()
    {
        return 0; //unknown
    }

    unsigned CanCast(IEclType* /*target*/)
    {
        return 0;
    }

    unsigned MaxChars()
    {
        return 0;
    }

protected:
};
//  ---------------------------------------------------------------------------
class CEclBoolean : public CEclType, public IEclBoolean  
{
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclBoolean)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclBoolean, CEclType, CEclBoolean)

    CEclBoolean()
    {
    }

    CEclBoolean(const CEclBoolean & /*copy*/)
    {
    }

    CEclBoolean(const iter_t & /*itr*/)
    {
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += BOOLEAN_TYPE;
        return ecl.c_str();
    }

    IEclType* CloneType()
    {
        return (IEclBoolean* )(new CEclBoolean(*this));
    }

    const TCHAR *GetTypeName()
    {
        return BOOLEAN_TYPE;
    }
    unsigned GetFixedLogicalWidth()
    {
        return 0;
    }
    void SetFixedLogicalWidth(unsigned  /*width*/)
    {
        //nothing todo?
    }
    unsigned GetFixedPhysicalWidth()
    {
        return 1; 
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
            castScore=100;
        else if(intt)
            castScore=100;
        else if(realt)
            castScore=100;
        else if(decimalt)
        {
            // this case may cause ECL compilation to fail
            if(decimalt->getTotalDigitCount()==decimalt->getDecimalDigitCount())
            {
                castScore=0;
            }
            else
            {
                if(decimalt->isUnsigned())
                {
                    if(decimalt->getDecimalDigitCount()==1)
                        castScore=0; // doesn't like this
                    else
                        castScore=100;
                }
                else
                {
                    castScore=100;
                }
            }
        }
        else if(stringt)
        {
            castScore=stringt->getCharCount()>0 ? 100 : 0;
        }
        else if(datat)
        {
            castScore=datat->getByteCount()>0 ? 100 : 0;
        }
        return castScore;
    }

    unsigned MaxChars()
    {
        return 1;
    }

};

//  ---------------------------------------------------------------------------
class CEclInteger : public CEclType, public IEclInteger 
{
protected:
    std::_tstring big_endian;
    static std::_tstring m_unsigned;
    bool m_isBigEndian;
    bool m_isUnsigned;
    bool m_IntegerSpecified;
    unsigned m_byteCount;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclInteger)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclInteger, CEclType, CEclInteger)

    CEclInteger(const iter_t &itr, bool isUnsigned, bool integerSpecified)
    {
        big_endian = _T("big_endian");
        m_isBigEndian = false;
        m_isUnsigned = isUnsigned;
        m_IntegerSpecified = integerSpecified;
        m_byteCount = 8;
        for(iter_t i = itr->children.begin(); i != itr->children.end(); ++i)
        {
            std::_tstring value(i->value.begin(), i->value.end());

            if (boost::algorithm::iequals(big_endian, value))
                m_isBigEndian = true;
            else if (boost::algorithm::iequals(m_unsigned, value))
                m_isUnsigned = true;
            else
            {
                try
                {
                    m_byteCount = boost::lexical_cast<int>(value);
                }
                catch(const boost::bad_lexical_cast &)
                {
                }
            }
        }
    }

    CEclInteger()
    {
        m_isBigEndian = false;
        m_isUnsigned = false;
        m_IntegerSpecified = true;
        m_byteCount = 8;
    }

    CEclInteger(const CEclInteger &copy)
    {
        m_isBigEndian = copy.m_isBigEndian;
        m_isUnsigned = copy.m_isUnsigned;
        m_IntegerSpecified = copy.m_IntegerSpecified;
        m_byteCount = copy.m_byteCount;
    }

    const TCHAR* Serialize(std::_tstring &resultEcl, int depth = 0) const
    {
        std::_tstring ecl;
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_isBigEndian)
        {
            ecl += big_endian;
        }
        if (m_isUnsigned)
        {
            ecl += _T(" ");
            ecl += m_unsigned;
        }
        if (m_IntegerSpecified)
        {
            ecl += _T(" ");
            ecl += INTEGER_TYPE;
        }
        if (m_byteCount > 0)
        {
            ecl += boost::lexical_cast<std::_tstring>(m_byteCount).c_str();
        }
        boost::algorithm::trim(ecl);
        resultEcl += ecl;
        return resultEcl.c_str();
    }

    IEclType* CloneType()
    {
        return (IEclInteger*)(new CEclInteger(*this));
    }

    const TCHAR *GetTypeName()
    {
        return INTEGER_TYPE;
    }
    unsigned GetFixedLogicalWidth()
    {
        return 0; //unknown
    }
    void SetFixedLogicalWidth(unsigned)
    {
        //nothing todo?
    }
    unsigned GetFixedPhysicalWidth()
    {
        return m_byteCount;
    }
    bool isBigEndian()
    {
        return m_isBigEndian;
    }
    bool isUnsigned()
    {
        return m_isUnsigned;
    }
    unsigned getByteCount()
    {
        return m_byteCount;
    }
    void setByteCount(unsigned byteCount)
    {
        m_byteCount = byteCount;
    }
    void setBigEndian(bool isBigEndian)
    {
        m_isBigEndian = isBigEndian;
    }
    void setUnsigned(bool isUnsigned)
    {
        m_isUnsigned = isUnsigned;
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
            castScore=100;
        else if(intt)
        {
            if(intt->getByteCount()>getByteCount())
            {
                castScore=100;
            }
            else if(intt->getByteCount()==getByteCount())
            {
                if(isUnsigned() && !intt->isUnsigned())
                    castScore=90;
                else
                    castScore=100;
            }
            else
            {
                if(isUnsigned() && !intt->isUnsigned())
                    castScore=(intt->getByteCount()*90)/getByteCount();
                else
                    castScore=(intt->getByteCount()*100)/getByteCount();
            }
        }
        else if(realt)
        {
            castScore=75; // hard to be clever without a value here
        }
        else if(decimalt)
        {
            unsigned integerpart=decimalt->getTotalDigitCount()-decimalt->getDecimalDigitCount();

            if(MaxChars()<=integerpart)
                castScore=100; // !!!  WHAT ABOUT DIFFERENT SIGNS, CAN THEY FIT?
            else
                castScore=75;
        }
        else if(stringt)
        {
            if(MaxChars()<=stringt->getCharCount())
                castScore=100;
            else
                castScore=50;
        }
        else if(datat)
        {
            if(MaxChars()<=datat->getByteCount())
                castScore=100;
            else
                castScore=50;
        }

        return castScore;
    }

    unsigned MaxChars() // in decimal representation
    {
        if(isUnsigned())
        {
            switch(m_byteCount)
            {
            case 1: 
                return 3; // 255
            case 2:
                return 5; // 65535
            case 3:
                return 8; //16777215
            case 4:
                return 10; // 4294967295
            case 5:
                return 13; // 1099511627775
            case 6:
                return 15;// 281474976710655
            case 7:
                return 17; // 72057594037927935
            default:
            case 8:
                return 20; // unsigned 8 byte integer in base 10
            }
        }
        else
        {
            switch(m_byteCount)
            {
            case 1: 
                return 4; // -127
            case 2:
                return 6; // -32767
            case 3:
                return 8; // -8388607
            case 4:
                return 11; // -2147483647
            case 5:
                return 13; // -549755813887
            case 6:
                return 16; // -140737488355327
            case 7:
                return 18; // -36028797018963967
            default:
            case 8:
                return 20; // -9223372036854775807
            }
        }
    }

};
std::_tstring CEclInteger::m_unsigned = _T("unsigned");

//  ---------------------------------------------------------------------------
class CEclReal : public CEclType, public IEclReal
{
protected:
    unsigned m_byteCount;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclReal)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclReal, CEclType, CEclReal)

        CEclReal(const iter_t &itr)
    {
        m_byteCount = 8;
        std::_tstring real = REAL_TYPE;
        iter_t i = itr->children.begin();
        ATLASSERT(boost::algorithm::iequals(real, std::_tstring(i->value.begin(), i->value.end())));
        ++i;
        if (i != itr->children.end())
        {
            std::_tstring value(i->value.begin(), i->value.end());
            try
            {
                m_byteCount = boost::lexical_cast<int>(value);
            }
            catch(const boost::bad_lexical_cast &)
            {
            }
        }
    }

    CEclReal()
    {
        m_byteCount = 8;
    }

    CEclReal(const CEclReal& copy)
    {
        m_byteCount = copy.m_byteCount;
    }

    IEclType* CloneType()
    {
        return (IEclReal*)(new CEclReal(*this));
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += REAL_TYPE;
        if (m_byteCount > 0)
            ecl += boost::lexical_cast<std::_tstring>(m_byteCount).c_str();
        return ecl.c_str();
    }
    const TCHAR *GetTypeName()
    {
        return REAL_TYPE;
    }
    unsigned GetFixedLogicalWidth()
    {
        return 0; //unknown
    }
    void SetFixedLogicalWidth(unsigned  /*width*/)
    {
        //nothing todo?
    }
    unsigned GetFixedPhysicalWidth()
    {
        return m_byteCount;
    }
    unsigned getByteCount()
    {
        return m_byteCount;
    }
    void setByteCount(unsigned byteCount)
    {
        m_byteCount = byteCount;
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
        {
            castScore=100; // all non-zero reals give true and 0.0 gives false
        }
        else if(intt)
        {
            castScore=10; // !! DON"T REALLY KNOW
        }
        else if(realt)
        {
            if(realt->getByteCount()>=getByteCount())
            {
                castScore=100;
            }
            else
            {
                castScore=50;
            }
        }
        else if(decimalt)
        {
            castScore=50; // don't know really
        }
        else if(stringt)
        {
            if(stringt->getCharCount()>=MaxChars())
                castScore=100;
            else
                castScore=50;
        }
        else if(datat)
        {
            if(datat->getByteCount()>=MaxChars())
                castScore=100;
            else
                castScore=50;
        }

        return castScore;
    }

    unsigned MaxChars()
    {
        if(getByteCount()==8)
            return 23;	// DBL_MAX 1.7976931348623158e+308
        else if(getByteCount()==4)
            return 12; // VERIFY AGAIN !!!
        else
            return 23; // default is double
    }

};
//  ---------------------------------------------------------------------------
class CEclDecimal : public CEclType, public IEclDecimal
{
protected:
    bool m_isUnsigned;
    bool m_unsignedSpecified;
    unsigned m_totalDigitCount;
    unsigned m_decimalDigitCount;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclDecimal)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclDecimal, CEclType, CEclDecimal)

        CEclDecimal(const iter_t &itr, bool isUnsigned)
    {
        std::_tstring _unsigned = _T("unsigned");
        m_isUnsigned = isUnsigned;
        m_unsignedSpecified = false;
        m_totalDigitCount = 0;
        m_decimalDigitCount = 0;
        for(iter_t i = itr->children.begin(); i != itr->children.end(); ++i)
        {
            std::_tstring value(i->value.begin(), i->value.end());

            if (boost::algorithm::iequals(_unsigned, value))
            {
                m_unsignedSpecified = true;
                m_isUnsigned = true;
            }
            else
            {
                try
                {
                    if (!m_totalDigitCount)
                        m_totalDigitCount = boost::lexical_cast<int>(value);
                    else
                        m_decimalDigitCount = boost::lexical_cast<int>(value);
                }
                catch(const boost::bad_lexical_cast &)
                {
                }
            }
        }
    }

    CEclDecimal()
    {
        m_isUnsigned = false;
        m_unsignedSpecified = false;
        m_totalDigitCount = 0;
        m_decimalDigitCount = 0;
    }

    CEclDecimal(const CEclDecimal& copy)
    {
        m_isUnsigned = copy.m_isUnsigned;
        m_unsignedSpecified = copy.m_unsignedSpecified;
        m_totalDigitCount = copy.m_totalDigitCount;
        m_decimalDigitCount = copy.m_decimalDigitCount;
    }

    IEclType* CloneType()
    {
        return (IEclDecimal *)new CEclDecimal(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_unsignedSpecified)
            ecl += _T("unsigned ");
        else if (m_isUnsigned)
            ecl += _T("u");
        ecl += DECIMAL_TYPE;
        ecl += boost::lexical_cast<std::_tstring>(m_totalDigitCount).c_str();
        if (m_decimalDigitCount)
        {
            ecl += _T("_");
            ecl += boost::lexical_cast<std::_tstring>(m_decimalDigitCount).c_str();
        }
        return ecl.c_str();
    }
    const TCHAR *GetTypeName()
    {
        return DECIMAL_TYPE;
    }
    unsigned GetFixedLogicalWidth()
    {
        return 0; //unknown
    }
    void SetFixedLogicalWidth(unsigned  /*width*/)
    {
        //nothing todo?
    }
    unsigned GetFixedPhysicalWidth()
    {
        //TODO:?????
        return m_totalDigitCount;
    }
    bool isUnsigned()
    {
        return m_isUnsigned;
    }
    void setUnsigned(bool isUnsigned)
    {
        m_isUnsigned = isUnsigned;
    }
    unsigned getTotalDigitCount()
    {
        return m_totalDigitCount;
    }
    void setTotalDigitCount(unsigned totalDigitCount)
    {
        m_totalDigitCount = totalDigitCount;
    }
    unsigned getDecimalDigitCount()
    {
        return m_decimalDigitCount;
    }
    void setDecimalDigitCount(unsigned decimalDigitCount)
    {
        m_decimalDigitCount = decimalDigitCount;
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
        {
            castScore=0; // never producs true from non-zero decimal
        }
        else if(intt)
        {
            if(isUnsigned())
            {
                unsigned integerpart=getTotalDigitCount()-getDecimalDigitCount();

                if(intt->MaxChars()>integerpart) 
                    castScore=90; // lost only precision i.e. decimal part
                else
                    castScore=(intt->MaxChars()*90)/integerpart;
            }
            else
            {
                // signed decimals, if negative, are cast to positive integers
                // and I think that's so bad that it deserves 0 score
                castScore=0;
            }
        }
        else if(realt)
        {
            if(realt->getByteCount()==4)
            {
                if(getTotalDigitCount()<=7) // precision of float is 7 decimal points
                    castScore=100; 
                else
                    castScore=50;
            }
            else // real of 8 bytes
            {
                if(getTotalDigitCount()<=15) // precision 15 digits in double
                    castScore=100; 
                else
                    castScore=50;
            }
        }
        else if(decimalt)
        {
            if(decimalt->getTotalDigitCount()>=getTotalDigitCount() &&
                decimalt->getDecimalDigitCount()>=getDecimalDigitCount())
            {
                castScore=100;
            }
            else
            {
                castScore=(decimalt->getTotalDigitCount()*100)/getTotalDigitCount();
            }
        }
        else if(stringt)
        {
            // if strings are shorter, the last precision digit gets lost
            if(stringt->MaxChars()>=MaxChars())
                castScore=100;
            else
                castScore=(stringt->MaxChars()*100)/MaxChars();
        }
        else if(datat)
        {
            // datat is similar to string 
            if(datat->getByteCount()>=MaxChars())
                castScore=100;
            else
                castScore=(datat->getByteCount()*100)/MaxChars();
        }

        return castScore;
    }

    unsigned MaxChars()
    {
        unsigned chars = getTotalDigitCount();
        if(getDecimalDigitCount()>0)
            chars++;
        if(!isUnsigned())
            chars++;
        return chars; // max 32 decimals, decimal point and a sign
    }

};
//  ---------------------------------------------------------------------------
class CEclString : public CEclType, public IEclString
{
protected:
    bool m_isEbcdic;
    bool m_hasAscii;
    bool m_isCompressed;
    bool m_isUnicode;
    bool m_isVariable;
    unsigned m_charCount;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclString)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclString, CEclType, CEclString)

        CEclString(const iter_t &itr)
    {
        std::_tstring ebcdic = _T("ebcdic");
        std::_tstring ascii = _T("ascii");
        std::_tstring string = _T("string");
        std::_tstring qstring = _T("qstring");
        std::_tstring varstring = _T("varstring");
        std::_tstring unicode = _T("unicode");
        std::_tstring varunicode = _T("varunicode");

        m_isEbcdic = false;
        m_hasAscii = false;
        m_isCompressed = false;
        m_isUnicode = false;
        m_isVariable = false;
        m_charCount = UINT_MAX;

        for(iter_t i = itr->children.begin(); i != itr->children.end(); ++i)
        {
            std::_tstring value(i->value.begin(), i->value.end());

            if (boost::algorithm::iequals(ebcdic, value))
                m_isEbcdic = true;
            else if (boost::algorithm::iequals(ascii, value))
                m_hasAscii = true;
            else if (boost::algorithm::iequals(qstring, value))
                m_isCompressed = true;
            else if (boost::algorithm::iequals(varstring, value))
                m_isVariable = true;
            else if (boost::algorithm::iequals(unicode, value))
                m_isUnicode = true;
            else if (boost::algorithm::iequals(varunicode, value))
            {
                m_isVariable = true;
                m_isUnicode = true;
            }
            else if(boost::algorithm::iequals(string, value)) 
            {
                // implied ASCII should be explicit ??? 
                // if(!m_isEbcdic && !m_hasAscii)
                //	m_hasAscii=true;
            }
            else
            {
                try
                {
                    m_charCount = boost::lexical_cast<int>(value);
                }
                catch(const boost::bad_lexical_cast &)
                {
                }
            }
        }
    }

    CEclString()
    {
        m_isEbcdic = false;
        m_hasAscii = false;
        m_isCompressed = false;
        m_isUnicode = false;
        m_isVariable = false;
        m_charCount = UINT_MAX;
    }

    CEclString(const CEclString& copy)
    {
        m_isEbcdic = copy.m_isEbcdic;
        m_hasAscii = copy.m_hasAscii;
        m_isCompressed = copy.m_isCompressed;
        m_isUnicode = copy.m_isUnicode;
        m_isVariable = copy.m_isVariable;
        m_charCount = copy.m_charCount;
    }

    IEclType* CloneType()
    {
        return (IEclString*)new CEclString(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_isEbcdic)
            ecl += _T("ebcdic ");
        else if(m_hasAscii)
            ecl += _T("ascii ");

        if (m_isUnicode)
        {
            if (m_isVariable)
                ecl += _T("varunicode");
            else
                ecl += _T("unicode");
        }
        else if (m_isCompressed)
            ecl += _T("qstring");
        else if (m_isVariable)
            ecl += _T("varstring");
        else 
            ecl += STRING_TYPE;

        if (m_charCount!=UINT_MAX)
            ecl += boost::lexical_cast<std::_tstring>(m_charCount).c_str();
        return ecl.c_str();
    }
    const TCHAR *GetTypeName()
    {
        return STRING_TYPE;
    }

    unsigned GetFixedLogicalWidth()
    {
        return m_charCount;
    }
    void SetFixedLogicalWidth(unsigned width)
    {
        m_charCount = width;
    }
    unsigned GetFixedPhysicalWidth()
    {
        if ( m_isUnicode )
            return m_charCount*2;
        else
            return m_charCount;
    }

    bool isEbcdic()
    {
        return m_isEbcdic;
    }
    bool isCompressed()
    {
        return m_isCompressed;
    }
    bool isUnicode()
    {
        return m_isUnicode;
    }
    bool isVariable()
    {
        return m_isVariable;
    }
    unsigned getCharCount()
    {
        return m_charCount;
    }
    void setEbcdic(bool isEbcdic)
    {
        m_isEbcdic = isEbcdic;
    }
    void setCompressed(bool isCompressed)
    {
        m_isCompressed = isCompressed;
    }
    void setUnicode(bool isUnicode)
    {
        m_isUnicode = isUnicode;
    }
    void setVariable(bool isVariable)
    {
        m_isVariable = isVariable;
    }
    void setCharCount(unsigned charCount)
    {
        m_charCount = charCount;
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
        {
            // normally returns true for non-empty strings but with just spaces returns false
            // assume longer string will have spaces with less probability and that
            // probability of not-finding space in a character is 254/256

            castScore=static_cast<unsigned>(floor(pow(254.0f/255.0f, (int)getCharCount())*100.0f)); 
        }
        else if(intt)
        { 
            if(getCharCount()>=intt->MaxChars())
                castScore=90; // won't give it 100 since string may contain non-numeric chars 
            else
                castScore=(getCharCount()*90)/intt->MaxChars();
        }
        else if(realt)
        {
            if(getCharCount()>=realt->MaxChars())
                castScore=90; // won't give it 100 since string may contain non-numeric chars 
            else
                castScore=(getCharCount()*90)/realt->MaxChars();
        }
        else if(stringt)
        {
            unsigned targetcount=stringt->getCharCount();

            if( targetcount>=getCharCount())
            {
                // assume some loss from UNICODE to ASCII
                if(isUnicode() && !stringt->isUnicode())
                    castScore=90;
                else
                    castScore=100;
            }
            else
            {
                // calc percentage difference
                castScore=(targetcount*100)/getCharCount();
                ATLASSERT(castScore<100);
            }
        }
        else if(datat)
        {
            unsigned bytedata = datat->getByteCount();
            if(isUnicode())
            {
                if(bytedata>=getCharCount()*2)
                    castScore=100;
                else
                    castScore=(bytedata*100)/(getCharCount()*2);
            }
            else
            {
                if(bytedata>=getCharCount())
                    castScore=100;
                else
                    castScore=(bytedata*100)/getCharCount();
            }
        }

        return castScore;
    }

    unsigned MaxChars()
    {
        return m_charCount;
    }

};
//  ---------------------------------------------------------------------------
class CEclData : public CEclType, public IEclData
{
protected:
    unsigned m_byteCount;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclData)
        IMPLEMENT_INTERFACE(IEclType)
    END_CUNKNOWN(CEclType)

    //IMPLEMENT_CECLELEMENT2(IEclData, CEclType, CEclData)

        CEclData(const iter_t &itr)
    {
        m_byteCount = 0;
        for(iter_t i = itr->children.begin(); i != itr->children.end(); ++i)
        {
            std::_tstring value(i->value.begin(), i->value.end());
            try
            {
                m_byteCount = boost::lexical_cast<int>(value);
            }
            catch(const boost::bad_lexical_cast &)
            {
            }
        }
    }

    CEclData()
    {
        m_byteCount = 0;
    }

    CEclData(const CEclData& copy)
    {
        m_byteCount = copy.m_byteCount;
    }

    IEclType* CloneType()
    {
        return (IEclData*)new CEclData(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += DATA_TYPE;
        if (m_byteCount)
            ecl += boost::lexical_cast<std::_tstring>(m_byteCount).c_str();
        return ecl.c_str();
    }
    const TCHAR *GetTypeName()
    {
        return DATA_TYPE;
    }
    unsigned GetFixedLogicalWidth()
    {
        return m_byteCount;
    }
    void SetFixedLogicalWidth(unsigned width)
    {
        m_byteCount = width;
    }
    unsigned GetFixedPhysicalWidth()
    {
        return m_byteCount;
    }
    unsigned getByteCount()
    {
        return m_byteCount;
    }
    void SetByteCount(unsigned byteCount)
    {
        m_byteCount = byteCount;
    }

    unsigned CanCast(IEclType* target)
    {
        unsigned castScore=0;

        CComQIPtr<IEclBoolean> boolt=target;
        CComQIPtr<IEclInteger> intt=target;
        CComQIPtr<IEclReal> realt=target;
        CComQIPtr<IEclDecimal> decimalt=target;
        CComQIPtr<IEclString> stringt=target;
        CComQIPtr<IEclData> datat=target;

        if(boolt)
            castScore=0; 
        else if(intt)
        { 
            if(intt->getByteCount()>=getByteCount())
                castScore=100; 
            else
                castScore=50;
        }
        else if(realt)
        {
            if(realt->MaxChars()>=getByteCount())
                castScore=100;
            else
                castScore=50;
        }
        else if(stringt)
        {
            // *2 because of hex output
            if(stringt->getCharCount()>=getByteCount()*2) 
                castScore=100;
            else
                castScore=(stringt->getCharCount()*100)/(getByteCount()*2);
        }
        else if(datat)
        {
            if(datat->getByteCount()>=getByteCount())
                castScore=100;
            else
                castScore=(datat->getByteCount()*100)/getByteCount(); 
        }

        return castScore;
    }

    unsigned MaxChars()
    {
        return m_byteCount*2; // since each byte is 2 hex chars
    }

};
//  ===========================================================================
class CEclIdentifier : public CEclElementImpl<CEclIdentifier>, public IEclIdentifier
{
    typedef CEclElementImpl<CEclIdentifier> base;
protected:
    std::_tstring m_id;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclIdentifier)
    END_CUNKNOWN(base)


    CEclIdentifier(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        if(i->value.id() != ecl_grammar::identifierID)
            AtlThrow(E_FAIL);
        m_id.assign(i->value.begin(), i->value.end());
    }

    CEclIdentifier(IEclIdentifier *id)
    {
        ATLASSERT(id);
        std::_tstring name;
        m_id = id->GetIdentifierName(name);
    }

    CEclIdentifier(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id = id;
    }

    CEclIdentifier(const CEclIdentifier &copy)
    {
        m_id = copy.m_id;
    }

    virtual IEclElement *Clone()
    {
        return new CEclIdentifier(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += m_id.c_str();
        return ecl.c_str();
    }

    //IEclIdentifier
    void SetIdentifierName(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id = id;
    }

    const TCHAR * GetIdentifierName(std::_tstring & name) const
    {
        name = m_id.c_str();
        return name.c_str();
    }

    bool Equals(const std::_tstring & label) const
    {
        return boost::algorithm::iequals(m_id, label);
    }

    bool Equals(const IEclIdentifier * other) const
    {
        std::_tstring name;
        return Equals(other->GetIdentifierName(name));
    }
};

//===========================================================================
class CEclIdentifierReference : public CEclElementImpl<CEclIdentifierReference>, public IEclIdentifierReference
{
    typedef CEclElementImpl<CEclIdentifierReference> base;
protected:
    Identifiers_t m_identifiers;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclIdentifierReference)
    END_CUNKNOWN(base)


    CEclIdentifierReference(const iter_t &itr)
    {
        init(itr);
    }

    //for simplicity sake, let this work as a plain old identifier!

    CEclIdentifierReference(IEclIdentifier *pId)
    {
        ATLASSERT(pId);
        IEclIdentifierAdapt id;
        std::_tstring name;
        id = new CEclIdentifier(pId->GetIdentifierName(name));
        m_identifiers.push_back(id);
    }

    CEclIdentifierReference(IEclIdentifierReference * pId)
    {
        ATLASSERT(pId);
        for (int count = 0; count < pId->GetIdentifierCount(); count++)
        {
            std::_tstring name;
            SetIdentifierName(pId->GetIdentifierName(count, name));
        }
    }

    CEclIdentifierReference(const TCHAR *pId)
    {
        if (pId)
        {
            IEclIdentifierAdapt id = new CEclIdentifier(pId);
            m_identifiers.push_back(id);
        }
    }

    CEclIdentifierReference(const std::_tstring & module, const std::_tstring & pId)
    {
        if (module.length())
        {
            IEclIdentifierAdapt id = new CEclIdentifier(module.c_str());
            m_identifiers.push_back(id);
        }
        if (pId.length())
        {
            IEclIdentifierAdapt id = new CEclIdentifier(pId.c_str());
            m_identifiers.push_back(id);
        }
    }

    CEclIdentifierReference(const CEclIdentifierReference &copy)
    {
        m_identifiers = copy.m_identifiers;
    }

    virtual IEclElement *Clone()
    {
        return new CEclIdentifierReference(*this);
    }

protected:
    void init(const iter_t &itr)
    {
        IEclIdentifierAdapt id;

        for ( iter_t i = itr->children.begin(); i != itr->children.end(); i++ )
        {
            if ( i->value.id() == ecl_grammar::identifierID )
            {
                id = new CEclIdentifier(i);
                m_identifiers.push_back(id);
            }
            else if ( i->value.id() == ecl_grammar::qualifiedIdentifierID )
            {
                init(i);
            }
        }
    }

public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        Identifiers_t::const_iterator i = m_identifiers.begin();
        while ( i != m_identifiers.end() )
        {
            i->get()->Serialize(ecl);
            i++;
            if ( i != m_identifiers.end() )
                ecl += _T(".");
        }
        return ecl.c_str();
    }

public: //IEclIdentifierReference
    int GetIdentifierCount()
    {
        return m_identifiers.size();
    }
    const TCHAR * GetIdentifierName(int i, std::_tstring & name)
    {
        if ( i < 0 || i >= (int)m_identifiers.size() )
            return 0;
        else
        {
            return m_identifiers[i]->GetIdentifierName(name);
        }
    }
    const TCHAR * GetIdentifierMinorName(std::_tstring& name)
    {
        //no qualifiers
        name = GetIdentifierName((int)m_identifiers.size()-1, name);
        return name.c_str();
    }

    //IEclIdentifier
    const TCHAR * GetIdentifierName(std::_tstring & name) const
    {
        name = _T("");
        Serialize(name);
        return name.c_str();
    }
    void SetIdentifierName(const TCHAR *pId)
    {
        ATLASSERT(pId);
        IEclIdentifierAdapt id = new CEclIdentifier(pId);
        m_identifiers.push_back(id);
    }
    bool Equals(const std::_tstring & label) const
    {
        std::_tstring name;
        Serialize(name);
        return boost::algorithm::iequals(name, label);
    }

    bool Equals(const IEclIdentifier * other) const
    {
        std::_tstring name;
        return Equals(other->GetIdentifierName(name));
    }
};

//===========================================================================
class CEclSizeOf : public CEclElementImpl<CEclSizeOf>, public IEclSizeOf
{
    typedef CEclElementImpl<CEclSizeOf> base;
protected:
    StlLinked<IEclIdentifierReference> m_idRef;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclSizeOf)
    END_CUNKNOWN(base)


    CEclSizeOf(const iter_t &itr)
    {
        m_idRef = new CEclIdentifierReference(itr->children.begin());
    }

public: //IEclElement
    virtual const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("sizeof(");
        m_idRef->Serialize(ecl);
        ecl += _T(")");
        return ecl.c_str();
    }

public: //IEclSizeOf
    IEclIdentifierReference* GetIdentifier()
    {
        return m_idRef;
    }

};

//===========================================================================
class CEclEmptySet : public IEclEmptySet, public CEclElementImpl<CEclEmptySet>
{
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclEmptySet)
    END_CUNKNOWN(CUnknown)


    CEclEmptySet(const iter_t &/*itr*/)
    {
    }

    CEclEmptySet()
    {
    }

public: //IEclElement
    virtual const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("[]");
        return ecl.c_str();
    }
};

//==============================================================================================
class CEclSelfIdentifier : public IEclSelfIdentifier, public CEclElementImpl<CEclSelfIdentifier>
{
protected:
    StlLinked<IEclIdentifier> m_id;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclSelfIdentifier)
    END_CUNKNOWN(CUnknown)


    CEclSelfIdentifier(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        if (i->value.id() != ecl_grammar::identifierID)
            AtlThrow(E_FAIL);
        m_id = new CEclIdentifier(i);
    }

    CEclSelfIdentifier(const CEclSelfIdentifier &copy)
    {
        m_id = copy.m_id;
    }

    CEclSelfIdentifier(IEclSelfIdentifier *id)
    {
        ATLASSERT(id);
        std::_tstring name;
        m_id = new CEclIdentifier(id->GetIdentifierName(name));
    }

    CEclSelfIdentifier(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id = new CEclIdentifier(id);
    }

    IEclElement *Clone()
    {
        return new CEclSelfIdentifier(*this);
    }

    void SetIdentifierName(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id->SetIdentifierName(id);
    }

    const TCHAR *GetIdentifierName(std::_tstring & name) const
    {
        return m_id->GetIdentifierName(name);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("SELF.");
        m_id->Serialize(ecl);
        return ecl.c_str();
    }

    bool Equals(const std::_tstring & label) const
    {
        return m_id->Equals(label);
    }

    bool Equals(const IEclIdentifier * other) const
    {
        std::_tstring name;
        return Equals(other->GetIdentifierName(name));
    }
};

//  ===========================================================================
class CEclValue : public IEclValue, public CEclElementImpl<CEclValue>
{
protected:
    std::_tstring				m_value;
    boost::spirit::classic::parser_id	m_parserId;
    StlLinked<IEclElement>		m_element;
    StlLinked<IEclComment>		m_comment;
    StlLinked<IEclType>			m_cast;
    bool						m_boolValue;
    bool					    m_bDefault;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclValue)
    END_CUNKNOWN(CUnknown)


    CEclValue(iter_t &itr)
    {
        m_boolValue = false;
        init(itr);
    }

    CEclValue()
    {
        //a default value
        m_parserId = ecl_grammar::unknownID;
    }

    CEclValue(IEclValue *value)
    {
        ATLASSERT(value);
        m_parserId = ecl_grammar::unknownID;
        if ( value->GetBoolValue(m_boolValue) )
            m_parserId = ecl_grammar::boolLiteralID;
        value->GetStringValue(m_value);
    }

    CEclValue(const TCHAR *value)
    {
        ATLASSERT(value);
        m_parserId = ecl_grammar::stringLiteralID;
        m_value = value;
        m_boolValue = false;
    }

    CEclValue(const CEclValue &copy)
    {
        m_parserId = copy.m_parserId;
        m_boolValue = copy.m_boolValue;
        m_element = copy.m_element;
    }

    void init(iter_t &itr)
    {
        /*
        value
        = boolLiteral 
        | stringLiteral 
        | numericLiteral
        | sizeOf
        | ParamaterizedAttribute 
        | identifierReference
        ;
        */

        //default value check (no children)
        if (itr->value.id() == ecl_grammar::valueOrDefaultID && !itr->children.size())
        {
            //this should be set already!
            ATLASSERT(m_parserId == ecl_grammar::unknownID);
            return;
        }

        if (itr->value.id() == ecl_grammar::castID)
        {
            iter_t type_itr = itr->children.begin();
            ATLASSERT(type_itr->value.id() == ecl_grammar::typeID);
            m_cast = CreateType(type_itr->children.begin());
            ++itr;
        }

        switch (itr->value.id().to_long())
        {
        case ecl_grammar::valueOrDefaultID:
        case ecl_grammar::valueID:
            {
                iter_t i = itr->children.begin();
                init(i); //go deeper
            }
            break;
        case ecl_grammar::identifierReferenceID:
            m_element = new CEclIdentifierReference(itr);
            m_parserId = itr->value.id();
            break;
        case ecl_grammar::paramaterizedAttributeID:
            m_element = CreateIEclParamaterizedAttribute(itr);
            m_parserId = itr->value.id();
            break;
        case ecl_grammar::sizeOfID:
            m_element = new CEclSizeOf(itr);
            m_parserId = itr->value.id();
            break;
        case ecl_grammar::emptySetID:
            m_element = new CEclEmptySet(itr);
            m_parserId = itr->value.id();
            break;
        case ecl_grammar::stringLiteralID:
        case ecl_grammar::boolLiteralID:
        case ecl_grammar::numericLiteralID:
            //drop down one more if needed
            if (itr->children.size() && itr->children.begin()->value.id() == itr->value.id())
            {
                itr = itr->children.begin();
            }
            m_parserId = itr->value.id();
            m_value.assign(itr->value.begin(), itr->value.end());
            if ( m_parserId == ecl_grammar::boolLiteralID )
            {
                m_boolValue = (boost::algorithm::iequals(m_value, _T("true")));
            }
            break;
        case ecl_grammar::blockCommentID:
            m_comment = new CEclBlockComment(itr);
            break;
        default:
            ATLASSERT(!"CEclValue TODO");
        }
    }

    IEclElement *Clone()
    {
        return new CEclValue(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if ( m_element.isLinked() )
            m_element->Serialize(ecl, depth);
        else
            ecl += m_value.c_str();
        if ( m_comment.isLinked() )
        {
            std::_tstring comment;
            ecl += m_comment->Serialize(comment);
        }
        return ecl.c_str();
    }

    //IEclValue
    bool IsStringLiteral() const
    {
        return (m_parserId == ecl_grammar::stringLiteralID);
    }

    bool IsEmptySet() const
    {
        return (m_parserId == ecl_grammar::emptySetID);
    }

    bool IsDefaultValue() const
    {
        return (m_parserId == ecl_grammar::unknownID);
    }

    bool GetBoolValue(bool &value) 
    {
        if ( m_parserId == ecl_grammar::boolLiteralID )
        {
            value = m_boolValue;
            return true;
        }
        return false;
    }

    bool IsIdentifierReference() const
    {
        return m_parserId == ecl_grammar::identifierReferenceID;
    }

    IEclIdentifierReference* GetIdentifierReference()
    {
        CComQIPtr<IEclIdentifierReference> iref;
        iref=m_element;
        return iref;
    }

    const TCHAR * GetIdentifierReferenceName(std::_tstring & name)
    {
        if (IsIdentifierReference())
        {
            CComPtr<IEclIdentifierReference> reference = GetIdentifierReference();
            return reference->GetIdentifierName(name);
        }
        return name.c_str();
    }

    bool IsParamaterizedAttribute() const
    {
        return m_parserId == ecl_grammar::paramaterizedAttributeID;
    }

    IEclParamaterizedAttribute* GetParamaterizedAttribute()
    {
        CComQIPtr<IEclParamaterizedAttribute> iref;
        iref=m_element;
        return iref;
    }

    const TCHAR * GetStringValue(std::_tstring & result)
    {
        ATLASSERT(m_parserId == ecl_grammar::stringLiteralID);
        result = m_value.c_str();
        boost::algorithm::trim_left_if(result, boost::algorithm::is_any_of("'")); //TODO this should be done at parse.
        boost::algorithm::trim_right_if(result, boost::algorithm::is_any_of("'")); 
        return result.c_str();
    }
};


//===========================================================================
class CEclValueList : public IEclValueList, public CEclElementImpl<CEclValueList>
{
protected:

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclValueList)
    END_CUNKNOWN(CUnknown)


        CEclValueList(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        while ( i != itr->children.end() )
        {
            parser_id id = i->value.id();
            if ( id == ecl_grammar::valueID || id == ecl_grammar::valueOrDefaultID)
                AddChild(new CEclValue(i));
            else if ( id == ecl_grammar::commentID )
                AddChild(new CEclComment(i));
            else
            {
                ATLASSERT(!"CEclValueList TODO");
            }
            i++;
        }
    }

    CEclValueList(IEclValueList *list)
    {
        if (list)
        {
            for (unsigned i=0; i<list->GetValueCount(); i++)
            {
                AddValue(list->GetValue(i),i);
            }
        }
    }

    CEclValueList(const TCHAR* values[], unsigned count, bool bQuote)
    {
        if (values)
        {
            for (unsigned i=0; i<count; i++)
            {
                //TODO: bQuote
                if ( bQuote )
                {
                    std::_tstring encoded;
                    ::EncodeForEcl(values[i], encoded);
                    AddChild(new CEclValue(encoded.c_str()),i);
                }
                else
                {
                    AddChild(new CEclValue(values[i]),i);
                }
            }
        }
    }

    CEclValueList(const CEclValueList &copy)
    {
        clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclValueList(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("(");
        unsigned count = GetChildCount();
        for(unsigned i = 0; i<count; ++i)
        {
            GetChild(i)->Serialize(ecl);
            if ( i+1<count )
                ecl += _T(", ");
        }
        ecl += _T(")");
        return ecl.c_str();
    }

    //IEclValueList
    unsigned GetValueCount()
    {
        return GetChildCount();
    }

    IEclValue * GetValue(unsigned item)
    {
        CComQIPtr<IEclValue> param;
        param = GetChild(item);
        return param;
    }

    void AddValue(IEclValue *param, int pos)
    {
        AddChild(new CEclValue(param),pos);
    }

    void DeleteValue(int pos)
    {
        DeleteChild(pos);
    }
};
//===========================================================================
class CEclParamaterizedAttribute : public IEclParamaterizedAttribute, public CEclElementImpl<CEclParamaterizedAttribute>
{
protected:
    IEclIdentifierReferenceAdapt	m_id;
    StlLinked<IEclValueList>		m_valueList;
    bool							m_bStatement;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclParamaterizedAttribute)
    END_CUNKNOWN(CUnknown)


    CEclParamaterizedAttribute(const iter_t &itr, bool bStatement)
    {
        iter_t i = itr->children.begin();
        m_id = new CEclIdentifierReference(i);
        i++;
        m_valueList = new CEclValueList(i);
        m_bStatement = bStatement;
    }

    CEclParamaterizedAttribute(IEclParamaterizedAttribute *decl)
    {
        ATLASSERT(decl);
        m_valueList = decl->GetValueList();
        std::_tstring name;
        m_id = new CEclIdentifierReference(decl->GetIdentifierName(name));
    }

    CEclParamaterizedAttribute(const TCHAR *id, IEclValueList *valueList, bool bStatement)
    {
        ATLASSERT(id);
        ATLASSERT(valueList);
        m_valueList = valueList;
        m_id = new CEclIdentifierReference(id);
        m_bStatement = bStatement;
    }

    CEclParamaterizedAttribute(const CEclParamaterizedAttribute &copy)
    {
        m_valueList = copy.m_valueList;
        m_id = copy.m_id;
        m_bStatement = copy.m_bStatement;
    }

    IEclElement *Clone()
    {
        return new CEclParamaterizedAttribute(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        m_id->Serialize(ecl);
        m_valueList->Serialize(ecl);
        return ecl.c_str();
    }

    //IEclParamaterizedAttribute
    void SetIdentifierName(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id->SetIdentifierName(id);
    }

    const TCHAR *GetIdentifierName(std::_tstring& name) 
    {
        return m_id->GetIdentifierName(name);
    }

    void SetValueList(IEclValueList *valueList)
    {
        ATLASSERT(valueList);
        m_valueList = valueList;
    }

    IEclValueList* GetValueList() 
    {
        return m_valueList.get();
    }

    bool IsStatement() const
    {
        return m_bStatement;
    }
};

IEclElement *CreateIEclParamaterizedAttribute(const iter_t &itr)
{
    return new CEclParamaterizedAttribute(itr,false);
}

//  ===========================================================================
class CEclParameter : public IEclParameter, public CEclElementImpl<CEclParameter>
{
protected:
    StlLinked<IEclIdentifierReference> m_idType;
    StlLinked<IEclType>		  m_type;
    StlLinked<IEclIdentifier> m_id;
    bool					  m_bConst;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclParameter)
    END_CUNKNOWN(CUnknown)


        CEclParameter(iter_t &itr)
    {
        m_bConst = false;

        /*
        parameter
        =  !constParam
        >> (type | qualifiedIdentifier | identifier) >> identifier 
        >> !(discard_node_d[COMMA])
        ;
        */

        iter_t i = itr->children.begin();

        if(i->value.id() == ecl_grammar::constParamID )
        {
            m_bConst = true;
            i++;
        }

        if(i->value.id() == ecl_grammar::typeID )
            m_type = CreateType(i->children.begin());
        else
            m_idType = new CEclIdentifierReference(i);
        i++;

        m_id = new CEclIdentifier(i);
    }

    CEclParameter(const CEclParameter &copy)
    {
        m_id = copy.m_id;
        m_idType = copy.m_idType;
        m_type = copy.m_type;
        m_bConst = copy.m_bConst;
    }

    CEclParameter(IEclParameter * copy)
    {
        std::_tstring param, typeName;
        m_id = new CEclIdentifier(copy->GetParameter(param));
        m_idType = new CEclIdentifierReference(copy->GetTypeName(typeName));
        m_type = copy->GetType();
        m_bConst = copy->IsConst();
    }

    CEclParameter(const TCHAR *id, IEclType *type, const TCHAR *typeId)
    {
        ATLASSERT(id);
        m_id = new CEclIdentifier(id);
        m_bConst = false;

        if (type)
            m_type = type;
        if (typeId)
            m_idType = new CEclIdentifierReference(typeId);
    }

    IEclElement *Clone()
    {
        return new CEclParameter(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if ( m_bConst )
            ecl += _T("const ");

        if ( m_idType && m_idType.isLinked() )
            m_idType->Serialize(ecl);
        else if ( m_type )
            m_type->Serialize(ecl);
        ecl += _T(" ");
        if (m_id)
            m_id->Serialize(ecl);
        return ecl.c_str();
    }

    //IEclParameter
    void SetParameter(const TCHAR *param)
    {
        ATLASSERT(param);
        m_id->SetIdentifierName(param);
    }

    void SetType(const TCHAR *typeName)
    {
        ATLASSERT(typeName);
        m_idType = new CEclIdentifierReference(typeName);
        m_type = 0;
    }

    void SetType(IEclType *iType)
    {
        ATLASSERT(iType);
        m_type = iType;
        m_idType = 0;
    }

    const TCHAR * GetTypeName(std::_tstring &name)
    {
        if ( m_idType && m_idType.isLinked() )
        {
            m_idType->Serialize(name);
        }
        else if ( m_type )
            name = m_type->GetTypeName();
        return name.c_str();
    }

    const TCHAR * GetParameter(std::_tstring &id) 
    {
        if (m_id)
        {
            m_id->GetIdentifierName(id);
        }
        return id.c_str();
    }

    IEclType * GetType() 
    {
        if (m_type)
            return m_type.get();
        else
            return 0;
    }

    bool IsConst() const
    {
        return m_bConst;
    }
};

//===========================================================================
class CEclParameterList : public IEclParameterList, public CEclElementImpl<CEclParameterList>
{
protected:

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclParameterList)
    END_CUNKNOWN(CUnknown)


    CEclParameterList(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        while ( i != itr->children.end() )
        {
            parser_id id = i->value.id();
            if ( id == ecl_grammar::parameterID )
                AddChild(new CEclParameter(i));
            else if ( id == ecl_grammar::commentID )
                AddChild(new CEclComment(i));
            else if ( id == ecl_grammar::parameterListID )
            {
                //the comma, ignore it
            }
            else
            {
                ATLASSERT(!"CEclParameterList TODO");
            }
            i++;
        }
    }

    CEclParameterList(IEclParameterList *list)
    {
        if (list)
        {
            for (unsigned i=0; i<list->GetParameterCount(); i++)
            {
                AddParameter(list->GetParameter(i),i);
            }
        }
    }

    CEclParameterList(const CEclParameterList &copy)
    {
        clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclParameterList(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("(");
        unsigned count = GetChildCount();
        for(unsigned i = 0; i<count; ++i)
        {
            GetChild(i)->Serialize(ecl);
            if ( i+1<count )
                ecl += _T(",");
        }
        ecl += _T(") ");
        return ecl.c_str();
    }

    //IEclParameterList
    unsigned GetParameterCount()
    {
        return GetChildCount();
    }

    IEclParameter * GetParameter(unsigned item)
    {
        CComQIPtr<IEclParameter> param;
        param = GetChild(item);
        return param;
    }

    void AddParameter(IEclParameter *param, int pos)
    {
        AddChild(new CEclParameter(param),pos);
    }

    void DeleteParameter(int pos)
    {
        DeleteChild(pos);
    }
};

//===========================================================================
class CEclFunctionDeclaration : public IEclFunctionDeclaration, public CEclElementImpl<CEclFunctionDeclaration>
{
protected:
    StlLinked<IEclIdentifier> m_id;
    StlLinked<IEclParameterList>  m_parameterList;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclFunctionDeclaration)
    END_CUNKNOWN(CUnknown)


        CEclFunctionDeclaration(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        m_id = new CEclIdentifier(i);
        i++;
        m_parameterList = new CEclParameterList(i);
    }

    CEclFunctionDeclaration(IEclFunctionDeclaration *decl)
    {
        ATLASSERT(decl);
        m_parameterList = decl->GetParameterList();
        std::_tstring name;
        m_id = new CEclIdentifier(decl->GetIdentifierName(name));
    }

    CEclFunctionDeclaration(const TCHAR *id, IEclParameterList *parameterList)
    {
        m_parameterList = parameterList;
        m_id = new CEclIdentifier(id);
    }

    CEclFunctionDeclaration(const CEclFunctionDeclaration &copy)
    {
        m_parameterList = copy.m_parameterList;
        m_id = copy.m_id;
    }

    IEclElement *Clone()
    {
        return new CEclFunctionDeclaration(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        m_id->Serialize(ecl);
        m_parameterList->Serialize(ecl);
        return ecl.c_str();
    }

    //IEclFunctionDeclaration
    void SetIdentifierName(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id->SetIdentifierName(id);
    }

    const TCHAR * GetIdentifierName(std::_tstring & name) 
    {
        return m_id->GetIdentifierName(name);
    }

    void SetParameterList(IEclParameterList *parameterList)
    {
        ATLASSERT(parameterList);
        m_parameterList = parameterList;
    }

    IEclParameterList* GetParameterList() 
    {
        return m_parameterList.get();
    }

};

//===========================================================================
class CEclDeclaration : public IEclDeclaration, public CEclElementImpl<CEclDeclaration>
{
protected:
    StlLinked<IEclType> m_type;
    StlLinked<IEclIdentifier> m_identifier;
    StlLinked<IEclAttribute> m_attribute;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclDeclaration)
    END_CUNKNOWN(CUnknown)


    CEclDeclaration(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        if(i->value.id() != ecl_grammar::typeID)
            AtlThrow(E_FAIL);
        m_type = CreateType(i->children.begin());
        i++;
        if(i->value.id() != ecl_grammar::identifierID)
            AtlThrow(E_FAIL);
        m_identifier = new CEclIdentifier(i);
        i++;
        if ( i != itr->children.end() )
        {
            if(i->value.id() != ecl_grammar::attributeListID)
                AtlThrow(E_FAIL);
            m_attribute = new CEclAttribute(i);
        }
    }

    CEclDeclaration(IEclDeclaration *idecl)
    {
        ATLASSERT(idecl);
        m_type = idecl->GetType();
        m_identifier = new CEclIdentifier(idecl->GetIdentifier());
    }

    CEclDeclaration(IEclIdentifier *id, IEclType *type)
    {
        ATLASSERT(id);
        ATLASSERT(type);

        m_type = type;
        m_identifier = new CEclIdentifier(id);
    }

    CEclDeclaration(const CEclDeclaration &copy)
    {
        m_type = copy.m_type;
        m_identifier = copy.m_identifier;
        m_attribute = copy.m_attribute;
    }

    IEclElement *Clone()
    {
        return new CEclDeclaration(*this);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        ATLASSERT(m_type.get());
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        m_type->Serialize(ecl);
        ecl += _T(" ");
        ATLASSERT(m_identifier.get());
        m_identifier->Serialize(ecl);
        if ( m_attribute.isLinked() )
        {
            ecl += _T(" ");
            m_attribute->Serialize(ecl);
        }

        return ecl.c_str();
    }

    IEclType * GetType()
    {
        ATLASSERT(m_type.get());
        return m_type.get();
    }
    void SetType(IEclType *_type)
    {
        ATLASSERT(_type);
        m_type = _type;
    }

    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_identifier->GetIdentifierName(name);
    }
    IEclIdentifier * GetIdentifier()
    {
        ATLASSERT(m_identifier.get());
        return m_identifier.get();
    }
    //void SetIdentifier(IEclIdentifier *id)
    //{
    //	m_identifier = new CEclIdentifier(id);
    //}

};
//  ===========================================================================
class CEclRecord : public IEclRecord, public CEclElementImpl<CEclRecord>
{
protected:

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclRecord)
    END_CUNKNOWN(CUnknown)


    CEclRecord(const iter_t &itr)
    {
        ATLASSERT(itr->value.id() == ecl_grammar::recordID);
        iter_t i = itr->children.begin();
        for(i; i != itr->children.end(); ++i)
        {
            ATLASSERT(i->value.id() == ecl_grammar::recordBodyID);
            ProcessRecordBody(i);
        }
    }

    CEclRecord()
    {
    }

    CEclRecord(IEclRecord *copy)
    {
        CEclRecord& other = *(CEclRecord*)copy;
        CEclElementImpl<CEclRecord>::clone(other);
    }

    CEclRecord(const CEclRecord &copy)
    {
        CEclElementImpl<CEclRecord>::clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclRecord(*this);
    }

public:

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        ecl += _T("record\r\n");

        SerializeChildren(ecl, depth + 1);
        
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("end");
        return ecl.c_str();
    }

    unsigned GetFieldCount()
    {
        return GetChildCount();
    }

    IEclElement * GetField(unsigned i)
    {
        return GetChild(i);
    }

    void AddField(IEclAssign *element, int pos);

    void AddField(IEclDeclaration *idecl, int pos)
    {
        AddChild(new CEclDeclaration(idecl),pos);
    }

    void DeleteField(int pos)
    {
        DeleteChild(pos);
    }

    void SplitField(int field, int offset)
    {
        CComQIPtr<IEclDeclaration> decl = GetChild(field);
        CComQIPtr<IEclString> stringType = decl->GetType();
        if (stringType) // Can only split strings
        {
            int width = stringType->getCharCount();
            CComQIPtr<IEclIdentifier> id = decl->GetIdentifier();
            std::_tstring label;
            id->GetIdentifierName(label);
            std::_tstring label_left = label + _T("_1");
            std::_tstring label_right = label + _T("_2");
            stringType->setCharCount(offset);
            id->SetIdentifierName(label_left.c_str());

            id = CreateIEclIdentifier(label_right.c_str());
            stringType = CreateIEclString();
            stringType->setCharCount(width - offset);
            decl = CreateIEclDeclaration(id, stringType);
            AddField(decl, field + 1);
        }
    }

    void mergeWithPrevious(int field)
    {
        CComQIPtr<IEclDeclaration> decl = GetChild(field);
        CComQIPtr<IEclString> stringType = decl->GetType();
        if (stringType) // Can only split strings
        {
            CComQIPtr<IEclDeclaration> prevDecl = GetChild(field - 1);
            CComQIPtr<IEclString> prevStringType = prevDecl->GetType();
            prevStringType->setCharCount(prevStringType->getCharCount() + stringType->getCharCount());
            DeleteField(field);
        }
    }

    void mergeWithNext(int field)
    {
        CComQIPtr<IEclDeclaration> decl = GetChild(field);
        CComQIPtr<IEclString> stringType = decl->GetType();
        if (stringType) // Can only split strings
        {
            CComQIPtr<IEclDeclaration> nextDecl = GetChild(field + 1);
            CComQIPtr<IEclString> nextStringType = nextDecl->GetType();
            nextStringType->setCharCount(nextStringType->getCharCount() + stringType->getCharCount());
            DeleteField(field);
        }
    }

    void rename(int field, const std::_tstring & name)
    {
        CComQIPtr<IEclDeclaration> decl = GetChild(field);
        CComQIPtr<IEclIdentifier> id = decl->GetIdentifier();
        id->SetIdentifierName(name.c_str());
    }

    int GetFieldPos(const std::_tstring & label)
    {
        for (unsigned int i = 0; i < GetFieldCount(); ++i)
        {
            CComPtr<IEclIdentifier> id;
            if (CComQIPtr<IEclDeclaration> child = GetChild(i))
            {
                id = child->GetIdentifier();
            }
            else if (CComQIPtr<IEclAssign> child = GetChild(i))
            {
                id = child->GetIdentifier();
            }
            else if (CComQIPtr<IEclDeclAndAssign> child = GetChild(i))
            {
                id = child->GetIdentifier();
            }
            ATLASSERT(id);
            if (id->Equals(label))
            {
                return i;
            }
        }
        return -1;
    }

    const TCHAR * GetFieldLabel(unsigned int pos, std::_tstring & label)
    {
        CComPtr<IEclElement> elem = GetField(pos);

        if(CComQIPtr<IEclDeclaration> decl = elem.p)
        {
            decl->GetIdentifierName(label);
        }
        else if (CComQIPtr<IEclDeclAndAssign> decl = elem.p)
        {
            decl->GetIdentifierName(label);
        }
        return label.c_str();
    }

    IEclType * GetFieldType(int field)
    {
        CComPtr<IEclElement> elem = GetField(field);
        CComPtr<IEclType> type;
        if(CComQIPtr<IEclDeclaration> decl = elem.p)
        {
            type = decl->GetType();
        }
        else if (CComQIPtr<IEclDeclAndAssign> decl = elem.p)
        {
            type = decl->GetType();
        }
        ATLASSERT(type);
        return type;
    }

    const TCHAR * GetFieldTypeEcl(int field, bool typeOnly, std::_tstring & result)
    {
        CComPtr<IEclType> type = GetFieldType(field);
        if (typeOnly)
        {
            result = type->GetTypeName();
        }
        else
        {
            type->Serialize(result);
        }
        return result.c_str();
    }
};
//  ===========================================================================
class CEclProject : public IEclProject, public CEclElementImpl<CEclProject>
{
protected:
    CComPtr<IEclIdentifier> m_dataset;
    CComPtr<IEclIdentifier> m_transform;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclProject)
    END_CUNKNOWN(CUnknown)


    CEclProject(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        if(i->value.id() == ecl_grammar::qualifiedIdentifierID)
        {
            m_dataset = new CEclIdentifierReference(i++);
        }
        else if (i->value.id() == ecl_grammar::identifierID)
        {
            m_dataset = new CEclIdentifier(i++);
        }
        ATLASSERT(i->value.id() == ecl_grammar::projectID);	//comma
        ++i;
        if(i->value.id() == ecl_grammar::identifierID)
        {
            m_transform = new CEclIdentifier(i++);
        }
    }

    CEclProject(IEclIdentifier * dataset, IEclIdentifier * transform)
    {
        m_dataset = dataset;
        m_transform = transform;
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        (depth);
        std::_tstring datasetRef, transformRef;
        if (m_dataset)
        {
            m_dataset->Serialize(datasetRef);
        }
        m_transform->Serialize(transformRef);

        std::_tstring tpl;
        if (datasetRef.length() > 0)
        {
            tpl = (boost::_tformat(_T("project(%1%, %2%(left))")) % datasetRef % transformRef).str();
        }
        else
        {
            tpl = (boost::_tformat(_T("project(, %1%(left))")) % transformRef).str();
        }
        ecl += tpl;
        return ecl.c_str();
    }
};
//  ===========================================================================
class CEclDataset : public IEclDataset, public CEclElementImpl<CEclDataset>
{
protected:
    CComPtr<IEclValue> m_file;
    CComPtr<IEclValue> m_record;
    ECLDATASETFORMAT m_format;

    std::_tstring m_csv_heading;
    std::_tstring m_csv_maxLength;
    std::_tstring m_csv_separator;
    std::_tstring m_csv_terminator;

    CComPtr<IEclValue> m_xml_xpath;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclDataset)
    END_CUNKNOWN(CUnknown)


    CEclDataset(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::valueOrDefaultID);
        m_file = new CEclValue(i++);
        ATLASSERT(i->value.id() == ecl_grammar::valueOrDefaultID);
        m_record = new CEclValue(i++);
        ATLASSERT(i->value.id() == ecl_grammar::datasetFormatID);
        SetFormat(i++);
    }

    CEclDataset(IEclValue *file, IEclValue *record, ECLDATASETFORMAT format = ECLDATASETFORMAT_FLAT)
    {
        ATLASSERT(file);
        ATLASSERT(record);
        m_file = file;
        m_record = record;
        m_format = format;
    }

    CEclDataset(IEclDataset *copy)
    {
        ATLASSERT(copy);
        m_file = copy->GetFile();
        m_record = copy->GetRecord();
        m_format = copy->GetFormat();
    }

    CEclDataset(const CEclDataset &copy)
    {
        m_file = copy.m_file;
        m_record = copy.m_record;
        m_format = copy.m_format;
        CEclElementImpl<CEclDataset>::clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclDataset(*this);
    }

protected:

    void SetFormat(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        boost::spirit::classic::parser_id id = i->value.id();
        switch(id.to_long())
        {
        case ecl_grammar::datasetFormatFlatID:
            m_format = 	ECLDATASETFORMAT_FLAT;
            i++;
            break;
        case ecl_grammar::datasetFormatCsvID:
            m_format = 	ECLDATASETFORMAT_CSV;
            SetCsvAttr(i++);
            break;
        case ecl_grammar::datasetFormatXmlID:
            m_format = 	ECLDATASETFORMAT_XML;
            SetXmlAttr(i++);
            break;
        default:
            m_format = 	ECLDATASETFORMAT_UNKNOWN;
            i++;
            break;
        }
    }

    void SetCsvAttr(const iter_t &itr)
    {
        for(iter_t csvAttrs = itr->children.begin(); csvAttrs != itr->children.end(); ++csvAttrs)
        {
            ATLASSERT(csvAttrs->value.id() == ecl_grammar::datasetFormatCsvAttrID);
            iter_t i = csvAttrs->children.begin();
            std::_tstring value;
            switch(i->value.id().to_long())
            {
            case ecl_grammar::datasetFormatCsvAttrHeadingID:
                m_csv_heading = GetValue(i, value);
                break;
            case ecl_grammar::datasetFormatCsvAttrMaxlengthID:
                m_csv_maxLength = GetValue(i, value);
                break;
            case ecl_grammar::datasetFormatCsvAttrSeparatorID:
                m_csv_separator = GetValue(i, value);
                break;
            case ecl_grammar::datasetFormatCsvAttrTerminatorID:
                m_csv_terminator = GetValue(i, value);
                break;
            }
        }
    }

    void SetXmlAttr(const iter_t &itr)
    {
        for(iter_t xmlAttrs = itr->children.begin(); xmlAttrs != itr->children.end(); ++xmlAttrs)
        {
            ATLASSERT(xmlAttrs->value.id() == ecl_grammar::datasetFormatXmlAttrID);
            iter_t i = xmlAttrs->children.begin();
            switch(i->value.id().to_long())
            {
            case ecl_grammar::stringLiteralID:
                m_xml_xpath = new CEclValue(i++);
                break;
            }
        }
    }

    const TCHAR * GetValue(const iter_t &itr, std::_tstring & value)
    {
        iter_t cvsValue = itr->children.begin();
        ATLASSERT(cvsValue->value.id() == ecl_grammar::datasetFormatCsvAttrValueID);
        iter_t i = cvsValue->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::datasetFormatCsvAttrValueID);
        value.assign(i->value.begin(), i->value.end());
        return value.c_str();
    }

public:
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("dataset(");
        m_file->Serialize(ecl);
        ecl += _T(", ");
        m_record->Serialize(ecl);
        ecl += _T(", ");
        switch(m_format)
        {
        case ECLDATASETFORMAT_FLAT:
            ecl += _T("flat");
            break;
        case ECLDATASETFORMAT_CSV:
            ecl += _T("csv(");
            if (m_csv_heading.length())
            {
                if (!boost::algorithm::ends_with(ecl, _T("(")))
                    ecl += _T(", ");
                ecl += (boost::_tformat(_T("heading(%1%)")) % m_csv_heading).str().c_str();
            }
            if (m_csv_maxLength.length())
            {
                if (!boost::algorithm::ends_with(ecl, _T("(")))
                    ecl += _T(", ");
                ecl += (boost::_tformat(_T("maxlength(%1%)")) % m_csv_maxLength).str().c_str();
            }
            if (m_csv_separator.length())
            {
                if (!boost::algorithm::ends_with(ecl, _T("(")))
                    ecl += _T(", ");
                ecl += (boost::_tformat(_T("separator(%1%)")) % m_csv_separator).str().c_str();
            }
            if (m_csv_terminator.length())
            {
                if (!boost::algorithm::ends_with(ecl, _T("(")))
                    ecl += _T(", ");
                ecl += (boost::_tformat(_T("terminator(%1%)")) % m_csv_terminator).str().c_str();
            }
            ecl += _T(")");
            break;
        case ECLDATASETFORMAT_XML:
            ecl += _T("xml(");
            ATLASSERT(m_xml_xpath);
            m_xml_xpath->Serialize(ecl);
            ecl += _T(")");
            break;
        }
        ecl += _T(")");
        return ecl.c_str();
    }

    IEclValue * GetFile()
    {
        return m_file;
    }

    void SetFile(IEclValue * value)
    {
        m_file = value;
    }

    IEclValue * GetRecord()
    {
        return m_record;
    }
    ECLDATASETFORMAT GetFormat()
    {
        return m_format;
    }

    int GetCSVHeading()
    {
        int retVal = 0;
        try 
        {
            retVal = boost::lexical_cast<int>(m_csv_heading);
        }
        catch (boost::bad_lexical_cast &)
        {
        }
        return retVal;
    }
    void SetCSVHeading(int count)
    {
        m_csv_heading = boost::lexical_cast<std::_tstring>(count);
    }
    void SetCSVMaxLength(int count)
    {
        m_csv_maxLength = boost::lexical_cast<std::_tstring>(count);
    }
    void SetCSVSeparator(const std::_tstring & sep)
    {
        m_csv_separator = sep;
    }
    void SetCSVTerminator(const std::_tstring & term)
    {
        m_csv_terminator = term;
    }

    void SetXmlXPath(const TCHAR* xpath)
    {
        m_xml_xpath = new CEclValue(xpath);
    }
};
//  ===========================================================================
class CEclOutput : public IEclOutput, public CEclElementImpl<CEclOutput>
{
protected:
    StlLinked<IEclIdentifier> m_recordset;
    StlLinked<IEclRecord> m_record;
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclOutput)
    END_CUNKNOWN(CUnknown)


        CEclOutput(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::identifierID);
        m_recordset = new CEclIdentifier(i);
        if (++i != itr->children.end())
        {
            ATLASSERT(i->value.id() == ecl_grammar::identifierID);
            m_record = new CEclRecord(i);
        }
    }
    CEclOutput(IEclOutput *copy)
    {
        ATLASSERT(copy);
        m_recordset = copy->GetIdentifier();
        m_record = copy->GetRecord();
    }
    CEclOutput(const CEclOutput &copy)
    {
        CEclElementImpl<CEclOutput>::clone(copy);
        m_recordset = copy.m_recordset;
        m_record = copy.m_record;
    }
    CEclOutput(IEclIdentifier *recordset, IEclRecord *record = NULL)
    {
        ATLASSERT(recordset);
        m_recordset = new CEclIdentifier(recordset);
        if (record)
        {
            m_record = new CEclRecord(record);
        }
    }
    IEclElement * Clone()
    {
        return new CEclOutput(*this);
    }
    const TCHAR * Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("output(");
        m_recordset->Serialize(ecl);
        if (m_record)
        {
            ecl += _T(", ");
            m_record->Serialize(ecl);
        }
        ecl += _T(")");
        return ecl.c_str();
    }
    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_recordset->GetIdentifierName(name);
    }
    IEclIdentifier * GetIdentifier()
    {
        return m_recordset;
    }
    IEclRecord * GetRecord()
    {
        return m_record;
    }
};
//  ===========================================================================
class CEclAssign : public IEclAssign, public CEclElementImpl<CEclAssign>
{
    typedef CEclElementImpl<CEclAssign> BaseClass;

    bool m_bShared;
    bool m_bExported;
    bool m_bSelf;

    StlLinked<IEclIdentifier> m_identifier;
    StlLinked<IEclElement> m_rhs;
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclAssign)
    END_CUNKNOWN(CUnknown)


        CEclAssign(const iter_t &itr)
    {
        m_bShared = false;
        m_bExported = false;
        m_bSelf = false;

        iter_t i = itr->children.begin();
        if(i->value.id() == ecl_grammar::scopeFlagID)
        {
            SetScope(i);
            ++i;
        }
        if(i->value.id() == ecl_grammar::selfFlagID)
        {
            m_bSelf = true;
            ++i;
        }
        if(i->value.id() != ecl_grammar::identifierID)
            AtlThrow(E_FAIL);
        m_identifier = new CEclIdentifier(i);

        ++i;
        if (i->value.id() == ecl_grammar::recordID)
        {
            m_rhs = new CEclRecord(i);
        }
        else if (i->value.id() == ecl_grammar::projectID)
        {
            m_rhs = new CEclProject(i);
        }
        else if (i->value.id() == ecl_grammar::datasetID)
        {
            m_rhs = new CEclDataset(i);
        }
        else if (i->value.id() == ecl_grammar::valueID)
        {
            m_rhs = new CEclValue(i);
        }
        else if(i->value.id() == ecl_grammar::otherID)
        {
            m_rhs = new CEclUnknown(i);
        }
        else
        {
            ATLASSERT(!"Unknown type.");
        }
    }

    CEclAssign(const CEclAssign &copy)
    {
        BaseClass::clone(copy);
        m_bShared = copy.m_bShared;
        m_bExported = copy.m_bExported;
        m_bSelf = copy.m_bSelf;
        m_identifier = copy.m_identifier;
        m_rhs = copy.m_rhs;
    }

    CEclAssign(IEclIdentifier* identifier, IEclElement* rhs)
    {
        m_bShared = false;
        m_bExported = false;
        m_bSelf = false;

        m_identifier = identifier;
        m_rhs = rhs;
    }

    IEclElement *Clone()
    {
        return new CEclAssign(*this);
    }

    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_identifier->GetIdentifierName(name);
    }

    IEclIdentifier * GetIdentifier()
    {
        return m_identifier;
    }

    IEclElement * GetRHS()
    {
        return m_rhs;
    }

    void SetExported(bool exported)
    {
        m_bExported = exported;
    }

    const TCHAR* Serialize(std::_tstring& ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_bExported)
        {
            ecl += ExportToken;
            ecl += _T(" ");
        }
        else if(m_bShared)
        {
            ecl += SharedToken;
            ecl += _T(" ");
        }
        if (m_bSelf)
        {
            ecl += SelfToken;
            ecl += _T(".");
        }
        m_identifier->Serialize(ecl);
        ecl += _T(" := ");
        m_rhs->Serialize(ecl, depth);
        boost::algorithm::trim_right_if(ecl, boost::algorithm::is_any_of(_T(";\r\n \t")));
        return ecl.c_str();
    }

protected:
    void SetScope(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        std::_tstring flag;
        flag.assign(i->value.begin(), i->value.end());
        if (boost::algorithm::iequals(flag, ExportToken))
            m_bExported = true;
        else if (boost::algorithm::iequals(flag, SharedToken))
            m_bShared = true;
    }
};
//  ======== Workaround to resolve chicken-n-egg declaration order ============
void CEclRecord::AddField(IEclAssign *element, int pos)
{
    CEclAssign& other = *(CEclAssign*)element;
    AddChild(new CEclAssign(other), pos);
}
//  ===========================================================================
class CEclDeclAndAssign : public IEclDeclAndAssign, public CEclElementImpl<CEclDeclAndAssign>
{
protected:
    CComPtr<IEclType> m_type;
    CComPtr<IEclAssign> m_assign;
public:

    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclDeclAndAssign)
    END_CUNKNOWN(CUnknown)


    CEclDeclAndAssign(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        if (i->value.id() != ecl_grammar::typeID)
            AtlThrow(E_FAIL);
        m_type = CreateType(i->children.begin());

        i++;
        if (i->value.id() != ecl_grammar::assignmentID)
            AtlThrow(E_FAIL);
        m_assign = new CEclAssign(i);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        m_type->Serialize(ecl);
        ecl += _T(" ");
        m_assign->Serialize(ecl);
        return ecl.c_str();
    }
    IEclType * GetType()
    {
        return m_type;
    }
    void SetType(IEclType * type)
    {
        m_type = type;
    }

    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_assign->GetIdentifierName(name);
    }

    IEclIdentifier * GetIdentifier()
    {
        return m_assign->GetIdentifier();
    }
};
//  ===========================================================================
class CEclMacro : public IEclMacro, public CEclElementImpl<CEclMacro>
{
protected:
public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclMacro)
    END_CUNKNOWN(CUnknown)

        CEclMacro(const iter_t & /*itr*/)
    {
    }
    const TCHAR * GetParams()
    {
        return _T("");
    }
    const TCHAR* Serialize(std::_tstring & /*ecl*/, int /*depth*/ = 0) const
    {
        ATLASSERT(!"TODO.");
    }
};

//==============================================================================

class CEclServiceDeclaration : public IEclServiceDeclaration, public CEclElementImpl<CEclServiceDeclaration>
{
protected:
    StlLinked<IEclIdentifier>		m_id;
    StlLinked<IEclType>				m_returnType;
    StlLinked<IEclParameterList>	m_parameterList;
    StlLinked<IEclUnknown>			m_other;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclTransformAssignment)
    END_CUNKNOWN(CUnknown)


        CEclServiceDeclaration(const iter_t &itr)
    {
        /*
        serviceDeclaration 
        =	!type
        >>	identifier
        >>	parameterList
        >>	discard_node_d[COLON]
        >>	otherSemi //serviceDescription
        ;
        */
        iter_t i = itr->children.begin();

        if(i->value.id() == ecl_grammar::typeID)
        {
            m_returnType = CreateType(i->children.begin());
            i++;
        }

        m_id = new CEclIdentifier(i);
        i++;

        m_parameterList = new CEclParameterList(i);
        i++;

        m_other = new CEclUnknown(i);
    }

    CEclServiceDeclaration(IEclServiceDeclaration *copy)
    {
        ATLASSERT(copy);
        std::_tstring name;
        m_id = new CEclSelfIdentifier(copy->GetIdentifierName(name));
        //m_other = new CEclTransformExpression(copy->GetTransform());
    }

    CEclServiceDeclaration()
    {
    }

public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if ( m_returnType.isLinked() )
        {
            m_returnType->Serialize(ecl);
            ecl += _T(" ");
        }
        m_id->Serialize(ecl);
        m_parameterList->Serialize(ecl);
        m_other->Serialize(ecl);
        return ecl.c_str();
    }

public:	//IEclServiceDeclaration
    IEclIdentifier * GetIdentifier()
    {
        return m_id;
    }
    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_id->GetIdentifierName(name);
    }
    void SetIdentifierName(const TCHAR *id)
    {
        ATLASSERT(id);
        m_id = new CEclSelfIdentifier(id);
    }
};

//  ===========================================================================
class CEclService : public IEclService, public CEclElementImpl<CEclService>
{
protected:
    StlLinked<CEclIdentifier> m_id;
    bool m_bShared;
    bool m_bExported;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclService)
    END_CUNKNOWN(CUnknown)


        CEclService(const iter_t &itr)
    {
        /*
        service 
        =	!scopeFlag
        >>	identifier
        >>	discard_node_d[EQUIV]
        >>	SERVICE 
        >>	*(serviceDeclaration | comment)
        >>	END
        >>  discard_node_d[SEMI]
        ;
        */
        m_bShared = false;
        m_bExported = false;
        iter_t i = itr->children.begin();

        if(i->value.id() == ecl_grammar::scopeFlagID)
        {
            SetScope(i);
            i++;
        }

        if(i->value.id() == ecl_grammar::identifierID)
        {
            ATLASSERT(m_id.get() == NULL);
            m_id = new CEclIdentifier(i);
        }
        i++;

        for(i; i != itr->children.end(); ++i)
        {
            if (i->value.id() == ecl_grammar::serviceDeclarationID)
                AddChild(new CEclServiceDeclaration(i));
            else if (i->value.id() == ecl_grammar::serviceID)
            {
                //Do Nothing...end of service
            }
            else
            {
                ATLTRACE("CEclService TODO:  %i\n", i->value.id());
            }

        }
    }

    CEclService(IEclService *copy)
    {
        ATLASSERT(copy);
        m_id = new CEclIdentifier(copy->GetIdentifier());
        m_bShared = false;
        m_bExported = false;
    }

    CEclService(const CEclService &copy)
    {
        m_id = copy.m_id;
        m_bShared = copy.m_bShared;
        m_bExported = copy.m_bExported;
        CEclElementImpl<CEclService>::clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclService(*this);
    }

protected:
    void SetScope(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        std::_tstring flag;
        flag.assign(i->value.begin(), i->value.end());
        if (boost::algorithm::iequals(flag, ExportToken))
            m_bExported = true;
        else if (boost::algorithm::iequals(flag, SharedToken))
            m_bShared = true;
    }


public:
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_bExported)
        {
            ecl += ExportToken;
            ecl += _T(" ");
        }
        if (m_bShared)
        {
            ecl += SharedToken;
            ecl += _T(" ");
        }

        ATLASSERT(m_id.get() != NULL);
        m_id->Serialize(ecl);
        ecl += _T(" := \r\n  SERVICE");

        SerializeChildren(ecl, depth);

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("end");
        return ecl.c_str();
    }

    IEclIdentifier * GetIdentifier()
    {	
        return m_id.get();
    }

    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_id->GetIdentifierName(name);
    }

    void SetIdentifier(IEclIdentifier *id)
    {
        m_id = new CEclIdentifier(id);
    }

    unsigned GetFieldCount()
    {
        return GetChildCount();
    }

    IEclElement * GetField(unsigned i)
    {
        return GetChild(i);
    }

    void AddField(IEclDeclaration *idecl, int pos)
    {
        AddChild(new CEclDeclaration(idecl),pos);
    }
    void DeleteField(int pos)
    {
        DeleteChild(pos);
    }

};
//=====================================================================================
class CEclTransformExpression : public IEclTransformExpression, public CEclElementImpl<CEclTransformExpression>
{
protected:
    std::_tstring m_expression;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclTransformExpression)
    END_CUNKNOWN(CUnknown)


    CEclTransformExpression(const iter_t &itr)
    {
        //one element for now
        iter_t i = itr->children.begin();
        if ( i->children.size() )
            i = i->children.begin();

        m_expression.assign(i->value.begin(), i->value.end());
        trim(m_expression);
    }

    CEclTransformExpression(const TCHAR *expression)
    {
        ATLASSERT(expression);
        SetExpression(expression);
    }

    CEclTransformExpression()
    {
        SetExpression( _T("") );
    }
    //IEclTransformExpression
    const TCHAR * GetExpression()
    {
        return m_expression.c_str();
    }
    void SetExpression(const TCHAR *expression)
    {
        ATLASSERT(expression);
        m_expression = expression;
    }

    //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += m_expression.c_str();
        ecl += _T(";");
        return ecl.c_str();
    }
};

class CEclTransformAssignment : public IEclTransformAssignment, public CEclElementImpl<CEclTransformAssignment>
{
protected:
    StlLinked<IEclIdentifier> m_id;
    StlLinked<IEclTransformExpression> m_expression;
    bool m_local;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclTransformAssignment)
    END_CUNKNOWN(CUnknown)


    CEclTransformAssignment(const iter_t &itr)
    {
        //transformAssignment 
        //	=	(selfIdentifier | SELF)
        //	>>	discard_node_d[EQUIV]
        //	>>	transformationExpression
        iter_t i = itr->children.begin();
        if(i->value.id() == ecl_grammar::selfIdentifierID)
        {
            m_id = new CEclSelfIdentifier(i);
        }
        ++i;

        if(i->value.id() != ecl_grammar::otherSemiID)
            AtlThrow(E_FAIL);
        m_expression = new CEclTransformExpression(i);
        m_local = false;
    }

    CEclTransformAssignment(IEclTransformAssignment *copy)
    {
        ATLASSERT(copy);
        std::_tstring name;
        copy->GetIdentifierName(name);
        SetIdentifierName(name.c_str(), copy->IsLocal());
        m_expression = new CEclTransformExpression(copy->GetTransform());
    }

    CEclTransformAssignment(const TCHAR* id, const TCHAR* expression, bool local)
    {
        SetIdentifierName(id, local); 
        m_expression = new CEclTransformExpression(expression);
    }
public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if ( m_id.isLinked() | m_local )
        {
            m_id->Serialize(ecl);
            ecl += _T(" := ");
        }
        else
        {
            ecl += _T("SELF := ");
        }
        m_expression->Serialize(ecl);
        return ecl.c_str();
    }

public:	//IEclTransformAssignment
    IEclIdentifier * GetIdentifier()
    {
        return m_id;
    }
    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        if ( m_id.isLinked() || m_local )
        {
            m_id->GetIdentifierName(name);
        }
        else
        {
            name = _T("SELF");
        }
        return name.c_str();
    }
    bool IsDefault()
    {
        return !m_id.isLinked();
    }

    void MakeDefault()
    {
        m_id = 0;
    }
    const TCHAR *GetTransform(void)
    {
        return m_expression->GetExpression();
    }
    void SetIdentifierName(const TCHAR *id, bool local = false)
    {
        m_local = local;
        if (m_local)
            m_id = new CEclIdentifier(id);
        else
            m_id = new CEclSelfIdentifier(id);
    }
    void SetTransform(const TCHAR *ecl)
    {
        m_expression->SetExpression(ecl);
    }
    bool IsLocal()
    {
        return m_local;
    }

};
//  ===========================================================================
class CEclTransform : public IEclTransform, public CEclElementImpl<CEclTransform>
{
    typedef CEclElementImpl<CEclTransform> baseClass;
protected:
    StlLinked<IEclIdentifierReference> m_targetType;
    StlLinked<IEclIdentifier> m_id;
    StlLinked<IEclIdentifierReference> m_sourceType;
    StlLinked<IEclIdentifier> m_source;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclTransform)
    END_CUNKNOWN(CUnknown)


    CEclTransform(const iter_t &itr)
    {
        ATLASSERT(itr->value.id() == ecl_grammar::transformID);
        iter_t i = itr->children.begin();
        m_targetType = new CEclIdentifierReference(i++);
        m_id = new CEclIdentifier(i++);
        if (i != itr->children.end() && i->value.id() == ecl_grammar::identifierReferenceID)
        {
            m_sourceType = new CEclIdentifierReference(i++);
            m_source = new CEclIdentifier(i++);
        }

        for(i; i != itr->children.end(); ++i)
        {
            if(i->value.id() == ecl_grammar::recordBodyID)
            {
                ProcessRecordBody(i);
            }
        }
    }

    CEclTransform(IEclIdentifier * id, IEclIdentifierReference * targetType)
    {
        m_targetType = targetType;
        m_id = id;
    }

    CEclTransform(IEclTransform *copy)
    {
        ATLASSERT(copy);
        m_targetType = new CEclIdentifierReference(copy->GetTargetType());
        m_id = new CEclIdentifier(copy->GetIdentifier());
        m_sourceType = new CEclIdentifierReference(copy->GetSourceType());
        m_source = new CEclIdentifier(copy->GetSource());
        SetDefaultValues(copy->GetDefaultValues());
    }

    CEclTransform(const CEclTransform &copy)
    {
        m_targetType = copy.m_targetType;
        m_id = copy.m_id;
        m_sourceType = copy.m_sourceType;
        m_source = copy.m_source;
    }

    IEclElement *Clone()
    {
        return new CEclTransform(*this);
    }

public:
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        ATLASSERT(m_targetType);
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        m_targetType->Serialize(ecl, depth);
        ecl += _T(" ");
        m_id->Serialize(ecl, depth);
        ecl += _T("(");
        if (m_sourceType)
        {
            m_sourceType->Serialize(ecl, depth);
            ecl += _T(" ");
        }
        if (m_source)
        {
            m_source->Serialize(ecl, depth);
        }
        ecl += _T(")");
        ecl += _T(" := \r\n");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("transform\r\n");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (GetChildCount())
            SerializeChildren(ecl, depth);

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("end");
        return ecl.c_str();
    }

    //IEclTransform
    const TCHAR *GetResultTypeName(std::_tstring &name)
    {
        m_targetType->Serialize(name);
        return name.c_str();
    }
    IEclIdentifierReference* GetTargetType()
    {	
        return m_targetType.get();
    }
    const TCHAR * GetTargetQualifiedTypeName(std::_tstring & result) const
    {
        return m_targetType->GetIdentifierName(result);
    }
    const TCHAR * GetTargetTypeName(std::_tstring & result) const
    {	
        return m_targetType->GetIdentifierMinorName(result);
    }
    IEclIdentifier* GetIdentifier()
    {	
        return m_id.get();
    }
    IEclIdentifierReference* GetSourceType()
    {	
        return m_sourceType.get();
    }
    IEclIdentifier* GetSource()
    {	
        return m_source.get();
    }
    const TCHAR * GetSourceName(std::_tstring& name)
    {
        if (m_source)
        {
            return m_source->GetIdentifierName(name);
        }
        return NULL;
    }
    void SetTargetType(IEclIdentifierReference *targetType)
    {
        m_targetType = new CEclIdentifierReference(targetType);
    }
    void SetIdentifier(IEclIdentifier *id)
    {
        m_id= new CEclIdentifier(id);
    }
    void SetSourceType(IEclIdentifierReference *sourceType)
    {
        m_sourceType = new CEclIdentifierReference(sourceType);
    }
    void SetSource(IEclIdentifier *source)
    {
        m_source = new CEclIdentifier(source);
    }
    const TCHAR *GetIdentifierName(std::_tstring& name)
    {
        return m_id->GetIdentifierName(name);
    }
    unsigned GetFieldCount() const
    {
        return GetChildCount();
    }
    IEclElement * GetField(unsigned i) const
    {
        return GetChild(i);
    }
    IEclTransformAssignment * AddField(IEclTransformAssignment *field, int pos)
    {
        CEclTransformAssignment *copy = new CEclTransformAssignment(field);
        AddChild(copy,pos);
        //  Force self := left to end...
        if (GetDefaultValues())
        {
            SetNameMatchValues(false);
            SetNameMatchValues(true);
        }
        //  Force self := [] to end...
        if (GetDefaultValues())
        {
            SetDefaultValues(false);
            SetDefaultValues(true);
        }

        return copy;
    }
    void DeleteField(int pos)
    {
        DeleteChild(pos);
    }

    int FindDefaultValue() const
    {
        for(unsigned int i = 0; i < GetChildCount(); ++i)
        {
            if (CComQIPtr<IEclAssign> assign = GetChild(i))
            {
                if (CComQIPtr<IEclValue> rhs = assign->GetRHS())
                {
                    if (rhs->IsEmptySet())
                    {
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    void SetDefaultValues(bool value)
    {
        int pos = FindDefaultValue();
        if (value)
        {
            if (pos < 0)
            {
                AddChild(CreateIEclAssign(CreateIEclIdentifier(_T("self")), new CEclEmptySet()));
            }
        }
        else
        {
            if (pos >= 0)
            {
                DeleteChild(pos);
            }
        }
    }

    bool GetDefaultValues() const
    {
        return FindDefaultValue() >= 0;
    }

    int FindNameMatchValue() const
    {
        for(unsigned int i = 0; i < GetChildCount(); ++i)
        {
            if (CComQIPtr<IEclAssign> assign = GetChild(i))
            {
                if (CComQIPtr<IEclValue> rhs = assign->GetRHS())
                {
                    if (rhs->IsIdentifierReference() && m_source && m_source->Equals(rhs->GetIdentifierReference()))
                    {
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    void SetNameMatchValues(bool value)
    {
        int pos = FindNameMatchValue();
        if (value)
        {
            if (pos < 0 && m_source)
            {
                AddChild(CreateIEclAssign(CreateIEclIdentifier(_T("self")), new CEclIdentifier(m_source)));
            }
        }
        else
        {
            if (pos >= 0)
            {
                DeleteChild(pos);
            }
        }
    }

    bool GetNameMatchValues() const
    {
        return FindNameMatchValue() >= 0;
    }
};
//===========================================================================
class CEclImport : public IEclImport, public CEclElementImpl<CEclImport>
{
protected:
    Identifiers_t m_modules;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclImport)
    END_CUNKNOWN(CUnknown)


        CEclImport(const iter_t &itr)
    {
        init(itr);
    }

    CEclImport(const CEclImport &copy)
    {
        m_modules = copy.m_modules;
    }

    CEclImport(IEclImport * /*pId*/)
    {
        //TODO
    }

    virtual IEclElement *Clone()
    {
        return new CEclImport(*this);
    }

    void init(const iter_t &itr)
    {
        IEclIdentifierAdapt id;

        for ( iter_t i = itr->children.begin(); i != itr->children.end(); i++ )
        {
            if ( i->value.id() == ecl_grammar::identifierID )
            {
                id = new CEclIdentifier(i);
                m_modules.push_back(id);
            }
            else if ( i->value.id() == ecl_grammar::importID )
            {
                //the comma
            }
            else
            {
                ATLASSERT(!_T("TODO"));
            }
        }
    }

public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("IMPORT ");
        Identifiers_t::const_iterator i = m_modules.begin();
        while ( i != m_modules.end() )
        {
            i->get()->Serialize(ecl);
            i++;
            if ( i != m_modules.end() )
                ecl += _T(", ");
        }
        return ecl.c_str();
    }

    //IEclIdentifierReference
    int GetIdentifierCount()
    {
        return m_modules.size();
    }
    const TCHAR * GetIdentifierName(int i, std::_tstring & name)
    {
        if ( i >= (int)m_modules.size() )
            return name.c_str();
        else
            return m_modules[i]->GetIdentifierName(name);
    }
};

//  ===========================================================================
class CEclModuleExport : public IEclModuleExport, public CEclElementImpl<CEclModuleExport>
{
protected:
    CComPtr<IEclType> m_type;
    CComPtr<IEclIdentifier> m_id;
    CComPtr<IEclUnknown> m_other;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclModuleExport)
    END_CUNKNOWN(CUnknown)


        CEclModuleExport(const iter_t &itr)
    {
        init(itr);
    }

    CEclModuleExport(const CEclModuleExport &)
    {
    }

    CEclModuleExport(IEclModuleExport * /*pId*/)
    {
    }

    virtual IEclElement *Clone()
    {
        return new CEclModuleExport(*this);
    }

    void init(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        ATLASSERT(i->value.id() == ecl_grammar::moduleExportID);	//Really the EXPORT scope
        i++;
        ATLASSERT(i->value.id() == ecl_grammar::typeID);
        m_type = CreateType(i->children.begin());
        i++;
        ATLASSERT(i->value.id() == ecl_grammar::identifierID);
        m_id = new CEclIdentifier(i);
        i++;
        ATLASSERT(i->value.id() == ecl_grammar::otherSemiID);
        m_other = new CEclUnknown(i);
        i++;
    }

    IEclType * GetType()
    {
        return m_type;
    }
    IEclIdentifier * GetIdentifier()
    {
        return m_id;
    }

public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += ExportToken;
        ecl += _T(" ");
        m_type->Serialize(ecl);
        ecl += _T(" ");
        m_id->Serialize(ecl);
        ecl += _T(" := ");
        m_other->Serialize(ecl);
        return ecl.c_str();
    }
};
//  ===========================================================================
class CEclModule : public IEclModule, public CEclElementImpl<CEclModule>
{
protected:
    bool m_bExported;
    bool m_bShared;
    typedef StlLinked<IEclModuleExport> IEclModuleExportAdapt;
    typedef std::vector<IEclModuleExportAdapt> IEclModuleExportVector;
    IEclModuleExportVector m_exports;
    StlLinked<CEclFunctionDeclaration> m_functionDecl;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclModule)
    END_CUNKNOWN(CUnknown)


        CEclModule(const iter_t &itr)
    {
        init(itr);
    }

    CEclModule(const CEclModule &)
    {
    }

    CEclModule(IEclModule * /*pId*/)
    {
    }

    virtual IEclElement *Clone()
    {
        return new CEclModule(*this);
    }

    void init(const iter_t &itr)
    {
        m_bExported = false;
        m_bShared = false;

        iter_t i = itr->children.begin();
        SetScope(i);
        i++;
        m_functionDecl = new CEclFunctionDeclaration(i);
        i++;

        if (i->value.id() != ecl_grammar::moduleID)
        {
            AtlThrow(E_FAIL);
        }
        i++;

        for(i; i != itr->children.end(); ++i)
        {
            if (i->value.id() == ecl_grammar::moduleExportID)
            {
                StlLinked<IEclModuleExport> item = new CEclModuleExport(i);
                m_exports.push_back(item);
                AddChild(item);
            }
            else if (i->value.id() == ecl_grammar::otherModuleID)
            {
                AddChild(new CEclUnknown(i));
            }
            else if (i->value.id() == ecl_grammar::moduleID)
            {
                //The End
            }
            else
            {
                ATLASSERT(!_T("TODO"));
            }
        }
    }

public: //IEclElement
    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        if (m_bExported)
        {
            ecl += ExportToken;
            ecl += _T(" ");
        }
        else if(m_bShared)
        {
            ecl += SharedToken;
            ecl += _T(" ");
        }

        m_functionDecl->Serialize(ecl);

        ecl += _T(" :=\r\n");

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("module\r\n");

        SerializeChildren(ecl, depth + 1);

        for (int j = depth; j > 0; j--)
            ecl += _T("\t");

        ecl += _T("end");
        return ecl.c_str();
    }

    // IEclModule
    const TCHAR * GetIdentifierName(std::_tstring & name)
    {
        return m_functionDecl->GetIdentifierName(name);
    }

    IEclParameterList* GetInputParameterList()
    {
        return m_functionDecl->GetParameterList();
    }

    unsigned GetExportsCount()
    {
        return m_exports.size();
    }

    IEclModuleExport* GetExport(unsigned i)
    {
        ATLASSERT(i<m_exports.size());
        return m_exports[i];
    }

protected:
    void SetScope(const iter_t &itr)
    {
        iter_t i = itr->children.begin();
        std::_tstring flag;
        flag.assign(i->value.begin(), i->value.end());
        if (boost::algorithm::iequals(flag, ExportToken))
            m_bExported = true;
        else if (boost::algorithm::iequals(flag, SharedToken))
            m_bShared = true;
    }
};

//  ===========================================================================
class CEclProgram : public CEclElementImpl<CEclProgram>, public IEclProgram
{
    typedef CEclElementImpl<CEclProgram> baseClass;
protected:
    std::_tstring m_ecl;
    tree_parse_info<> m_info;

public:
    BEGIN_CUNKNOWN
        IMPLEMENT_INTERFACE(IEclProgram)
    END_CUNKNOWN(CUnknown)

    CEclProgram(const std::_tstring & ecl) : m_ecl(ecl)
    {
        if (ecl.empty())
        {
            return;
        }

        try
        {
            std::string necl = CT2A(ecl.c_str(), CP_UTF8);
            ::DoParse(necl, m_info);	//Stops compiler having to recompile the grammer all the time.
            if (m_info.match)
            {
                iter_t programItr = m_info.trees.begin();
                if (programItr->value.id() != ecl_grammar::programID)
                {
                    AtlThrow(E_FAIL);
                }

                for(iter_t queryItr = programItr->children.begin(); queryItr != programItr->children.end(); ++queryItr)
                {
                    if (queryItr->value.id() != ecl_grammar::queryID)
                    {
                        AtlThrow(E_FAIL);
                    }

                    for(iter_t i = queryItr->children.begin(); i != queryItr->children.end(); ++i)
                    {
                        parser_id id = i->value.id();
                        if (id == ecl_grammar::importID)
                            AddChild(new CEclImport(i));
                        else if (id == ecl_grammar::moduleID)
                            AddChild(new CEclModule(i));
                        else if (id == ecl_grammar::invokeCommentID)
                            AddChild(new CEclInvokeComment(i));
                        else if (id == ecl_grammar::GrETLCommentID)
                            AddChild(new CEclGrETLComment(i));
                        else if (id == ecl_grammar::declAndAssignID)
                            AddChild(new CEclDeclAndAssign(i));
                        else if (id == ecl_grammar::otherID)
                            AddChild(new CEclUnknown(i));
                        else if (id == ecl_grammar::assignmentID)
                            AddChild(new CEclAssign(i));
                        else if (id == ecl_grammar::serviceID)
                            AddChild(new CEclService(i));
                        else if (id == ecl_grammar::transformID)
                            AddChild(new CEclTransform(i));
                        else if (id == ecl_grammar::commentID)
                            AddChild(new CEclComment(i));
                        else if (id == ecl_grammar::paramaterizedAttributeID)
                            AddChild(new CEclParamaterizedAttribute(i,true));
                        else
                        {
                            ATLTRACE("CEclProgram Todo:  %i\n", id);
                        }
                    }
                }
            }
            else
                AtlThrow(E_FAIL);
        }
        catch(CAtlException)
        {
#ifdef _DEBUG
            {
                CUnicodeFile file;
                file.Create(_T("c:\\temp\\tmp.xml"));
                std::_tstring xml;
                SerializeToXml(xml);
                file.Write(xml.c_str());
            }
            ATLASSERT(!"Log written to tmp.xml");
#endif
            AtlThrow(E_FAIL);
        }
    }

    CEclProgram(const CEclProgram &copy)
    {
        m_ecl = copy.m_ecl;
        m_info = copy.m_info;
        CEclElementImpl<CEclProgram>::clone(copy);
    }

    IEclElement *Clone()
    {
        return new CEclProgram(*this);
    }

    unsigned GetChildCount() const 
    { 
        return baseClass::GetChildCount(); 
    } 

    bool ReplaceElement(IEclElement* from, IEclElement* to) 
    { 
        return baseClass::ReplaceElement(from,to); 
    } 

    IEclElement *GetChild(unsigned item) const 
    { 
        return baseClass::GetChild(item); 
    }

    void AddElement(IEclElement *element, int pos)
    {
        //ref count of element should be 1 at this point (ie detached)
        AddChild(element,pos);
    }

    bool DeleteElement(IEclElement *element)
    {
        return DeleteChild(element);
    }

    unsigned GetElementCount()
    {
        return GetChildCount();
    }

    IEclElement * GetElement(unsigned i)
    {
        return GetChild(i);
    }

    const TCHAR* Serialize(std::_tstring &ecl, int depth = 0) const
    {
        SerializeChildren(ecl, depth);
        return ecl.c_str();
    }
    const TCHAR* SerializeToXml(std::_tstring &xml)
    {
        std::map<parser_id, std::string> rule_names;
        rule_names[ecl_grammar::programID] = "program";
        rule_names[ecl_grammar::queryID] = "query";
        rule_names[ecl_grammar::commentID] = "comment";
        rule_names[ecl_grammar::blockCommentID] = "blockComment";
        rule_names[ecl_grammar::lineCommentID] = "lineComment";
        rule_names[ecl_grammar::GrETLCommentID] = "GrETLComment";
        rule_names[ecl_grammar::GrETLContentID] = "GrETLContent";
        rule_names[ecl_grammar::invokeCommentID] = "invokeComment";
        rule_names[ecl_grammar::invokeID] = "invoke";
        rule_names[ecl_grammar::invokeLabelID] = "invokeLabel";
        rule_names[ecl_grammar::commentContentID] = "commentContent";
        rule_names[ecl_grammar::scopeFlagID] = "scopeFlag";
        rule_names[ecl_grammar::selfFlagID] = "selfFlag";
        rule_names[ecl_grammar::moduleID] = "module";
        rule_names[ecl_grammar::moduleParamsID] = "moduleParams";
        rule_names[ecl_grammar::moduleExportID] = "moduleExport";
        rule_names[ecl_grammar::otherModuleID] = "otherModule";
        rule_names[ecl_grammar::macroID] = "macro";
        rule_names[ecl_grammar::macro_noparamID] = "macro_noparam";
        rule_names[ecl_grammar::macro_emptyparamID] = "macro_emptyparam";
        rule_names[ecl_grammar::macro_paramID] = "macro_param";
        rule_names[ecl_grammar::macroLabelID] = "macroLabel";
        rule_names[ecl_grammar::macroParamsID] = "macroParams";
        rule_names[ecl_grammar::macroContentID] = "macroContent";
        rule_names[ecl_grammar::importID] = "import";
        rule_names[ecl_grammar::identifierID] = "identifier";
        rule_names[ecl_grammar::qualifiedIdentifierID] = "qualifiedIdentifier";
        rule_names[ecl_grammar::identifierReferenceID] = "identifierReference";
        rule_names[ecl_grammar::declarationID] = "declaration";
        rule_names[ecl_grammar::assignmentID] = "assignment";
        rule_names[ecl_grammar::declAndAssignID] = "declAndAssign";
        rule_names[ecl_grammar::castID] = "castScore";
        rule_names[ecl_grammar::typeID] = "type";
        rule_names[ecl_grammar::typeBooleanID] = "typeBoolean";
        rule_names[ecl_grammar::typeIntegerID] = "typeInteger";
        rule_names[ecl_grammar::typeRealID] = "typeReal";
        rule_names[ecl_grammar::typeDecimalID] = "typeDecimal";
        rule_names[ecl_grammar::typeUDecimalID] = "typeUDecimal";
        rule_names[ecl_grammar::typeStringID] = "typeString";
        rule_names[ecl_grammar::typeQStringID] = "typeQString";
        rule_names[ecl_grammar::typeUnicodeID] = "typeUnicode";
        rule_names[ecl_grammar::typeDataID] = "typeData";
        rule_names[ecl_grammar::typeVarStringID] = "typeVarString";
        rule_names[ecl_grammar::typeVarUnicodeID] = "typeVarUnicode";
        rule_names[ecl_grammar::typeUnsignedID] = "typeUnsigned";
        rule_names[ecl_grammar::localeID] = "locale";
        rule_names[ecl_grammar::recordID] = "record";
        rule_names[ecl_grammar::recordBodyID] = "recordBody";
        rule_names[ecl_grammar::projectID] = "project";
        rule_names[ecl_grammar::datasetID] = "dataset";
        rule_names[ecl_grammar::datasetFormatID] = "datasetFormat";
        rule_names[ecl_grammar::datasetFormatFlatID] = "datasetFormatFlat";
        rule_names[ecl_grammar::datasetFormatCsvID] = "datasetFormatCsv";
        rule_names[ecl_grammar::datasetFormatCsvAttrID] = "datasetFormatCsvAttr";
        rule_names[ecl_grammar::datasetFormatCsvAttrHeadingID] = "datasetFormatCsvAttrHeading";
        rule_names[ecl_grammar::datasetFormatCsvAttrMaxlengthID] = "datasetFormatCsvAttrMaxlength";
        rule_names[ecl_grammar::datasetFormatCsvAttrSeparatorID] = "datasetFormatCsvAttrSeparator";
        rule_names[ecl_grammar::datasetFormatCsvAttrTerminatorID] = "datasetFormatCsvAttrTerminator";
        rule_names[ecl_grammar::datasetFormatCsvAttrValueID] = "datasetFormatCsvAttrValue";
        rule_names[ecl_grammar::datasetFormatXmlID] = "datasetFormatXml";
        rule_names[ecl_grammar::datasetFormatXmlAttrID] = "datasetFormatXmlAttr";
        rule_names[ecl_grammar::otherID] = "other";
        rule_names[ecl_grammar::otherSemiID] = "otherSemi";
        rule_names[ecl_grammar::otherEolID] = "otherEol";
        rule_names[ecl_grammar::otherEofID] = "otherEof";
        rule_names[ecl_grammar::attributeListID] = "attributeList";
        rule_names[ecl_grammar::transformID] = "transform";
        rule_names[ecl_grammar::parameterID] = "parameter";
        rule_names[ecl_grammar::parameterListID] = "parameterList";
        rule_names[ecl_grammar::functionDeclarationID] = "functionDeclaration";
        rule_names[ecl_grammar::selfIdentifierID] = "selfIdentifier";
        rule_names[ecl_grammar::sizeOfID] = "sizeof";
        rule_names[ecl_grammar::valueID] = "value";
        rule_names[ecl_grammar::valueOrDefaultID] = "valueOrDefault";
        rule_names[ecl_grammar::valueListID] = "valueList";
        rule_names[ecl_grammar::serviceID] = "service";
        rule_names[ecl_grammar::serviceDeclarationID] = "serviceDeclaration";
        rule_names[ecl_grammar::paramaterizedAttributeID] = "paramaterizedAttribute";
        rule_names[ecl_grammar::constParamID] = "const";
        rule_names[ecl_grammar::stringLiteralID] = "stringLiteral";
        rule_names[ecl_grammar::boolLiteralID] = "boolLiteral";
        rule_names[ecl_grammar::emptySetID] = "emptySet";
        rule_names[ecl_grammar::numericLiteralID] = "numericLiteral";

        std::stringstream strm;
        std::string necl = CT2A(m_ecl.c_str(), CP_UTF8);
        tree_to_xml(strm, m_info.trees, necl, rule_names);
        necl = strm.str();
        xml = CA2T(necl.c_str(), CP_UTF8);
        return xml.c_str();
    }
};
//  ===========================================================================
IEclProgram * ParseEcl(const TCHAR * ecl)
{
    try
    {
        return new CEclProgram(ecl);
    }
    catch(CAtlException)
    {
        return NULL;
    }
}

IEclProgram * ParseEcl(const CString &_ecl)
{
    std::_tstring ecl = static_cast<const TCHAR *>(_ecl);
    return ParseEcl(ecl.c_str());
}

IEclProgram * ParseEcl(const std::_tstring &_ecl)
{
    std::_tstring ecl = _ecl;
    return ParseEcl(ecl.c_str());
}

IEclAssign *CreateIEclAssign(IEclIdentifier *identifier, IEclElement* rhs)
{
    return new CEclAssign(identifier, rhs);
}
IEclBoolean *CreateIEclBoolean()
{
    return new CEclBoolean();
}
IEclDecimal *CreateIEclDecimal()
{
    return new CEclDecimal();
}
IEclString *CreateIEclString()
{
    return new CEclString();
}
IEclInteger *CreateIEclInteger()
{
    return new CEclInteger();
}
IEclReal *CreateIEclReal()
{
    return new CEclReal();
}
IEclData *CreateIEclData()
{
    return new CEclData();
}
IEclIdentifier *CreateIEclIdentifier(const std::_tstring &id)
{
    return new CEclIdentifier(id.c_str());
}
IEclIdentifierReference *CreateIEclIdentifierReference(const std::_tstring & qualifiedAttr)
{
    return new CEclIdentifierReference(_T(""), qualifiedAttr);
}
IEclIdentifierReference *CreateIEclIdentifierReference(const std::_tstring & module, const std::_tstring & attribute)
{
    return new CEclIdentifierReference(module, attribute);
}
IEclDeclaration *CreateIEclDeclaration(IEclIdentifier *id,IEclType *type)
{
    return new CEclDeclaration(id,type);
}
IEclRecord *CreateIEclRecord()
{
    return new CEclRecord();
}
IEclRecord *CreateIEclRecord(IEclRecord *copy)
{
    return new CEclRecord(copy);
}
IEclDataset *CreateIEclDataset(IEclDataset *copy)
{
    return new CEclDataset(copy);
}
IEclDataset *CreateIEclDataset(IEclValue *file, IEclValue *record, ECLDATASETFORMAT format)
{
    return new CEclDataset(file,record,format);
}
IEclTransform *CreateIEclTransform(IEclIdentifier * id, IEclIdentifierReference * targetRecord)
{
    return new CEclTransform(id, targetRecord);
}
IEclTransform *CreateIEclTransform(IEclTransform *copy)
{
    return new CEclTransform(copy);
}
IEclTransformAssignment *CreateIEclTransformAssignment(const TCHAR* id, const TCHAR* expression, bool local)
{
    return new CEclTransformAssignment(id, expression, local);
}
IEclValue *CreateIEclValue(const std::_tstring &id)
{
    return new CEclValue(id.c_str());
}
IEclValueList *CreateIEclValueList(const TCHAR* values[], unsigned count, bool bQuote)
{
    return new CEclValueList(values, count, bQuote);
}
IEclGrETLComment *CreateIEclGrETLComment()
{
    return new CEclGrETLComment();
}
IEclParameter *CreateIEclParameter( const std::_tstring &id, IEclType *type, const std::_tstring &typeId)
{
    return new CEclParameter(id.c_str(), type, typeId.c_str());
}
IEclParameterList *CreateIEclParameterList(IEclParameterList* list)
{
    return new CEclParameterList(list);
}
IEclParamaterizedAttribute *CreateIEclParamaterizedAttribute(const std::_tstring &id, IEclValueList *valueList, bool bStatement)
{
    return new CEclParamaterizedAttribute(id.c_str(), valueList, bStatement);
}
IEclFunctionDeclaration *CreateIEclFunctionDeclaration(const TCHAR* name, IEclParameterList* list)
{
    return new CEclFunctionDeclaration(name, list);
}
IEclOutput *CreateIEclOutput(IEclIdentifier *recordset, IEclRecord *record)
{
    return new CEclOutput(recordset, record);
}
IEclProject *CreateIEclProject(IEclIdentifier *dataset, IEclIdentifier *transform)
{
    return new CEclProject(dataset, transform);
}
//  ===========================================================================
bool IsValidIdentifier(const std::_tstring & label)
{
    if (label.length() == 0)
        return false;
    if (label.find_first_of(_T(" ")) != std::_tstring::npos)
        return false;
    const std::_tstring testEcl = label + _T(" := '';");
    StlLinked<IEclProgram> program = ParseEcl(testEcl);
    if (!program.get())
        return false;
    CComQIPtr<IEclAssign> assign = program->GetElement(0);
    if (!assign.p)
        return false;
    CComPtr<IEclIdentifier> id = assign->GetIdentifier();
    if (!id.p)
        return false;
    return true;
}

IEclValueList *CreateIEclValueList(const iter_t &itr)
{
    return new CEclValueList(itr);
}
//  ===========================================================================

