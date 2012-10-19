#if !defined(_PERFECTXML_MSXML4SAXHANDLERS_BASE_H_)
#define _PERFECTXML_MSXML4SAXHANDLERS_BASE_H_

#include "clib.h"

class CLIB_API ContentHandlerImplBase: public CUnknown, public ISAXContentHandler
{
public:
	IMPLEMENT_CUNKNOWN

	//	This must be correctly implemented, if your handler must be a COM Object 
	//	the current implementation is NOT thread-safe
	virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
		/* [in] */ ISAXLocator *pLocator);

	virtual HRESULT STDMETHODCALLTYPE startDocument( void);

	virtual HRESULT STDMETHODCALLTYPE endDocument( void);

	virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
		/* [in] */ const wchar_t *pwchPrefix,
		/* [in] */ int cchPrefix,
		/* [in] */ const wchar_t *pwchUri,
		/* [in] */ int cchUri);

	virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
		/* [in] */ const wchar_t *pwchPrefix,
		/* [in] */ int cchPrefix);

	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName,
		/* [in] */ ISAXAttributes *pAttributes);

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ const wchar_t *pwchNamespaceUri,
		/* [in] */ int cchNamespaceUri,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t *pwchQName,
		/* [in] */ int cchQName);

	virtual HRESULT STDMETHODCALLTYPE characters( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars);

	virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars);

	virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
		/* [in] */ const wchar_t *pwchTarget,
		/* [in] */ int cchTarget,
		/* [in] */ const wchar_t *pwchData,
		/* [in] */ int cchData);

	virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
		/* [in] */ const wchar_t *pwchName,
		/* [in] */ int cchName);

};

class CLIB_API Element
{
protected:
	std::wstring m_content;
	std::_tstring m_public_content;

public:
	std::_tstring m_tag;
	struct ciLessBoost : std::binary_function<std::_tstring, std::_tstring, bool> 
	{ 
		bool operator() (const std::_tstring & s1, const std::_tstring & s2) const { 
			return boost::lexicographical_compare(s1, s2, boost::is_iless()); 
		} 
	}; 
	typedef std::map<std::_tstring, std::_tstring, ciLessBoost> StringStringMap;
	StringStringMap m_attr;
	Element()
	{
	}

	Element(const Element & e) : m_tag(e.m_tag), m_content(e.m_content), m_attr(e.m_attr)
	{
	}

	Element & operator = (const Element & e)
	{
		m_tag = e.m_tag;
		m_content = e.m_content;
		m_attr = e.m_attr;
		return * this;
	}

	void AppendContent(const wchar_t * content, int len)
	{
		m_content.append(content, len);
	}

	const TCHAR * GetContent()
	{
		m_public_content = CW2T(m_content.c_str(), CP_UTF8);
		return m_public_content.c_str();
	}
};

#if (_MSC_VER <= 1400) 
class ElementStack : public std::stack<Element>
{
public:
	const std::deque<Element>& _Get_container() const
	{	// get reference to container
		return (c);
	}
};
#else
class CLIB_API ElementStack : public std::stack<Element>
{
public:
};
#endif
class CLIB_API StackHandlerImpl : public ContentHandlerImplBase
{
protected:
	 ElementStack m_stack;

public:
	virtual HRESULT STDMETHODCALLTYPE startElement( 
		/* [in] */ const wchar_t * /*pwchNamespaceUri*/,
		/* [in] */ int /*cchNamespaceUri*/,
		/* [in] */ const wchar_t *pwchLocalName,
		/* [in] */ int cchLocalName,
		/* [in] */ const wchar_t * /*pwchQName*/,
		/* [in] */ int /*cchQName*/,
		/* [in] */ ISAXAttributes *pAttributes)
	{ 
		Element e;
		std::wstring tag;
		tag.append(pwchLocalName, cchLocalName);
		e.m_tag = CW2T(tag.c_str(), CP_UTF8);

		int l;
		pAttributes->getLength(&l);
		for ( int i=0; i<l; i++ ) 
		{
			const wchar_t * ln, * vl; 
			int lnl, vll;
			std::wstring key, val;
			pAttributes->getLocalName(i, &ln, &lnl);
			key.append(ln, lnl);
			pAttributes->getValue(i,&vl,&vll);	//Will normalize all whitespace (includeing \r, \n and \t)
			val.append(vl, vll);
			e.m_attr[static_cast<const TCHAR *>(CW2T(key.c_str(), CP_UTF8))] = CW2T(val.c_str(), CP_UTF8);
		}

		m_stack.push(e);

		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE endElement( 
		/* [in] */ const wchar_t * /*pwchNamespaceUri*/,
		/* [in] */ int /*cchNamespaceUri*/,
		/* [in] */ const wchar_t * /*pwchLocalName*/,
		/* [in] */ int /*cchLocalName*/,
		/* [in] */ const wchar_t * /*pwchQName*/,
		/* [in] */ int /*cchQName*/)
	{ 
		m_stack.pop();
		return S_OK; 
	}

	virtual HRESULT STDMETHODCALLTYPE characters( 
		/* [in] */ const wchar_t *pwchChars,
		/* [in] */ int cchChars)
	{ 
		m_stack.top().AppendContent(pwchChars, cchChars);
		return S_OK; 
	}
};

#endif //	!defined(_PERFECTXML_MSXML4SAXHANDLERS_BASE_H_)
