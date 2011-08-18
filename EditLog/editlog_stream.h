/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
\ editlog_stream.h,  Use C++ std-library to write into a Windows Edit-Control
/
\ Version:	1.0, created 2000-04-02
/ Author:	Daniel Lohmann (daniel@uni-koblenz.de)
\
/ Please send comments and bugfixes to the above email address.
\
/ This code is provided AS IS, you use it at your own risk! 
\ You may use it for whatever you want.
/
\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/

#ifndef __EDITLOG_STREAM_H__
#define __EDITLOG_STREAM_H__

#include "EditLog.h"



// Note: I do declare all std-related code also in the namespace std.
// If you don't like this, you can replace the namespace by whatever
// you want
namespace std {

	///////////////////////////////////
	// basic_streambuf class, that writes output to a CEditLog
	// Save to be used for output from multiple threads. 
	//
	template <class E, class T = std::char_traits<E>, int BUF_SIZE = 512 >
	class basic_editstreambuf : public std::basic_streambuf< E, T >
	{
	public:
		basic_editstreambuf( CEditLog& Log );
		virtual ~basic_editstreambuf();
	protected:
		virtual int_type overflow( int_type c = T::eof() );
		virtual int sync();
		virtual std::streamsize xsputn(const char_type* pch, std::streamsize n);

		char_type*	psz;
		CEditLog&   m_Log;
		CRITICAL_SECTION	m_csLock;
	};

	// Ansi version
	typedef basic_editstreambuf< char >		editstreambuf;
	// Unicode version
	typedef basic_editstreambuf< wchar_t>	weditstreambuf;

	// TCHAR version
	#ifdef _UNICODE
	#define _teditstreambuf weditstreambuf
	#else
	#define _teditstreambuf editstreambuf
	#endif

	////////////////////////////////////////////////////////////////////////////////
	//
	// IMPLEMENTATION
	//
	//

	template <class E, class T, int BUF_SIZE >
	basic_editstreambuf<E,T,BUF_SIZE>::basic_editstreambuf(
			CEditLog& Log		
			)
		: std::basic_streambuf<E,T>(), m_Log(Log)
	{
		psz = new char_type[ BUF_SIZE ];
		setbuf( psz, BUF_SIZE );
		// leave place for single char + 0 terminator
		setp( psz, psz + BUF_SIZE - 2 );
		::InitializeCriticalSection( &m_csLock );
	}

	template <class E, class T, int BUF_SIZE >
	basic_editstreambuf<E,T,BUF_SIZE>::~basic_editstreambuf()
	{
		::DeleteCriticalSection( &m_csLock );
		delete psz;
	}

	template <class E, class T, int BUF_SIZE >
	int basic_editstreambuf<E,T,BUF_SIZE>::sync(
			)
	{
		overflow();
		return 0;
	}

	template <class E, class T, int BUF_SIZE >
	typename std::streamsize basic_editstreambuf<E,T,BUF_SIZE>::xsputn(
			const char_type* pch,
			std::streamsize n
			)
	{
		std::streamsize nMax, nPut;
		::EnterCriticalSection( &m_csLock );
		for( nPut = 0; 0 < n; ) {
			if( pptr() != 0 && 0 < (nMax = epptr() - pptr()) ) {
				if( n < nMax )
					nMax = n;
				traits_type::copy( pptr(), pch, nMax );
				
				// Sync if string contains LF
				bool bSync = traits_type::find( pch, nMax, traits_type::to_char_type( '\n' ) ) != NULL;
				pch += nMax, nPut += nMax, n -= nMax, pbump(nMax);
				if( bSync )
					sync();

				
			}
			else if (traits_type::eq_int_type(traits_type::eof(),
				overflow(traits_type::to_int_type(*pch))))
				break;
			else
				++pch, ++nPut, --n;
		}
		::LeaveCriticalSection( &m_csLock );
		return (nPut); 
	}

	template <class E, class T, int BUF_SIZE >
	typename basic_editstreambuf< E, T, BUF_SIZE >::int_type basic_editstreambuf< E,T,BUF_SIZE >::overflow(
			int_type c		// = T::eof() 
			)
	{
		::EnterCriticalSection( &m_csLock );
		char_type* plast = pptr();
		if( c != T::eof() )
			// add c to buffer
			*plast++ = c;
		*plast = char_type();

		// Pass text to the edit control
		m_Log.AddText( pbase(), true );
		setp( pbase(), epptr() );

		::LeaveCriticalSection( &m_csLock );
		return c != T::eof() ? T::not_eof( c ) : T::eof();
	}

} // namespace std

#endif