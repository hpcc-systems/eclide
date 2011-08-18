#include "stdafx.h"

#include "SAXHandlersBase.h"

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::putDocumentLocator( 
	/* [in] */ ISAXLocator * /*pLocator*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::startDocument( void){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::endDocument( void){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::startPrefixMapping( 
	/* [in] */ const wchar_t * /*pwchPrefix*/,
	/* [in] */ int /*cchPrefix*/,
	/* [in] */ const wchar_t * /*pwchUri*/,
	/* [in] */ int /*cchUri*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::endPrefixMapping( 
	/* [in] */ const wchar_t * /*pwchPrefix*/,
	/* [in] */ int /*cchPrefix*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::startElement( 
	/* [in] */ const wchar_t * /*pwchNamespaceUri*/,
	/* [in] */ int /*cchNamespaceUri*/,
	/* [in] */ const wchar_t * /*pwchLocalName*/,
	/* [in] */ int /*cchLocalName*/,
	/* [in] */ const wchar_t * /*pwchQName*/,
	/* [in] */ int /*cchQName*/,
	/* [in] */ ISAXAttributes * /*pAttributes*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::endElement( 
	/* [in] */ const wchar_t * /*pwchNamespaceUri*/,
	/* [in] */ int /*cchNamespaceUri*/,
	/* [in] */ const wchar_t * /*pwchLocalName*/,
	/* [in] */ int /*cchLocalName*/,
	/* [in] */ const wchar_t * /*pwchQName*/,
	/* [in] */ int /*cchQName*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::characters( 
	/* [in] */ const wchar_t * /*pwchChars*/,
	/* [in] */ int /*cchChars*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::ignorableWhitespace( 
	/* [in] */ const wchar_t * /*pwchChars*/,
	/* [in] */ int /*cchChars*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::processingInstruction( 
	/* [in] */ const wchar_t * /*pwchTarget*/,
	/* [in] */ int /*cchTarget*/,
	/* [in] */ const wchar_t * /*pwchData*/,
	/* [in] */ int /*cchData*/){ return S_OK; }

HRESULT STDMETHODCALLTYPE ContentHandlerImplBase::skippedEntity( 
	/* [in] */ const wchar_t * /*pwchName*/,
	/* [in] */ int /*cchName*/){ return S_OK; }



