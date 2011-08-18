#pragma once

enum MARKER_TYPE
{
	MARKER_ERROR,
	MARKER_WARNING,
	MARKER_ARROW,
	MARKER_ADDED,
	MARKER_DELETED,
	MARKER_MODIFIED,
	MARKER_CIRCLEMINUS,
	MARKER_CIRCLE,
	MARKER_CIRCLEPLUS
};

#define MARKER_ERROR_BIT	0x0001
#define MARKER_WARNING_BIT	0x0002

enum SYNTAX_TYPE
{
	SYNTAX_UNKNOWN = 0,
	SYNTAX_INFO,
	SYNTAX_WARNING,
	SYNTAX_ERROR,
	SYNTAX_LAST
};

class CSyntaxError
{
public:
    SYNTAX_TYPE m_type;
    int m_code;
    CString m_fileName;
    CString m_message;
	int m_markerHandle;
	int m_lineNo;
	int m_colNo;
};

typedef std::vector<CSyntaxError> CSyntaxErrorVector;

__interface ISciSyntaxMarker
{
	size_t SyntaxGetCount();
	bool SyntaxGet(size_t indx, SYNTAX_TYPE &type, CString &msg, CString &code, CString &location);
	HWND SyntaxGetHwnd();
	HWND SyntaxGetParentHwnd();
};

template<class T>
class CSciSyntaxMarker : public ISciSyntaxMarker
{
private:
	int m_currIndex;
	CSyntaxErrorVector m_list;

public:
	CSciSyntaxMarker() : m_currIndex(-1)
	{
	}

	size_t SyntaxGetCount()
	{
		return m_list.size();
	}

	int SyntaxGetCurrent()
	{
		return m_currIndex;
	}

	void SyntaxGetMessage(CString & result)
	{
		if (m_currIndex != -1)
			result = m_list[m_currIndex].m_message;
	}

	bool SyntaxIsWarning()
	{
		if (m_currIndex != -1)
			return (m_list[m_currIndex].m_type == SYNTAX_WARNING);
		return false;
	}

	bool SyntaxIsError()
	{
		if (m_currIndex != -1)
			return (m_list[m_currIndex].m_type == SYNTAX_ERROR);
		return false;
	}

	void SyntaxGetSource(CString & result)
	{
		if (m_currIndex != -1)
			result = m_list[m_currIndex].m_fileName;
	}

	void SyntaxClearAll()
	{
		T* pT = static_cast<T*>(this);
		m_currIndex = -1;
		if (m_list.size())
		{
			m_list.clear();
			pT->MarkerDeleteAll(MARKER_ERROR);
			pT->MarkerDeleteAll(MARKER_WARNING);
			pT->MarkerDeleteAll(MARKER_ARROW);
			pT->Colourise(0, -1);
		}
		pT->AnnotationClearAll();
	}

	void SyntaxChecking()
	{
		SyntaxClearAll();
		SyntaxAdd(SYNTAX_INFO, 0, _T(""), _T("Checking..."), 0, 0);
	}

	void SyntaxAdd(SYNTAX_TYPE type, int code, const CString & fileName, const CString & message, int lineNo, int colNo)
	{
		T* pT = static_cast<T*>(this);
		CSyntaxError err;
		err.m_type = type;
		err.m_code = code;
		err.m_fileName = fileName;
		err.m_message = message;
		/*
		if (lineNo > pT->GetLineCount() - 1)
			lineNo = pT->GetLineCount() - 1;
		if (lineNo < 0)
			lineNo = 0;
		int colWidth = pT->GetLineLength(lineNo);
		if (colNo > colWidth - 1)
			colNo = colWidth - 1;
		if (colNo < 0)
			colNo = 0;
		*/
		err.m_lineNo = lineNo;
		err.m_colNo = colNo;
		err.m_markerHandle = -1;
		if (!fileName.GetLength() && lineNo >= 0)	//Local error
		{
			err.m_markerHandle = pT->MarkerAdd(lineNo, type == SYNTAX_ERROR ? MARKER_ERROR : MARKER_WARNING);
		}
		m_list.push_back(err);
	}

	bool SyntaxGet(size_t indx, int &lineNo, int &colNo)
	{
		T* pT = static_cast<T*>(this);
		if (indx >= 0 && indx < m_list.size())
		{
			lineNo = pT->MarkerLineFromHandle(m_list[indx].m_markerHandle);
			colNo = m_list[indx].m_colNo;
			return true;
		}
		return false;
	}

	bool SyntaxGet(size_t indx, SYNTAX_TYPE &type, CString &msg, CString &code, CString &location)
	{
		if (indx >= 0 && indx < m_list.size())
		{
			type = m_list[indx].m_type;
			msg = m_list[indx].m_message;
			switch (type)
			{
			case SYNTAX_ERROR:
			case SYNTAX_WARNING:
				msg += _T(" (");
				msg += boost::lexical_cast<std::_tstring>(m_list[indx].m_lineNo + 1).c_str();
				msg += _T(", ");
				msg += boost::lexical_cast<std::_tstring>(m_list[indx].m_colNo + 1).c_str();
				msg += _T(")");
				break;
			}
			code = boost::lexical_cast<std::_tstring>(m_list[indx].m_code).c_str();
			location = m_list[indx].m_fileName;
			return true;
		}
		return false;
	}

	HWND SyntaxGetHwnd()
	{
		T* pT = static_cast<T*>(this);
		return *pT;
	}

	HWND SyntaxGetParentHwnd()
	{
		T* pT = static_cast<T*>(this);
		return ::GetParent(*pT);
	}

	bool SyntaxGoto(size_t indx)
	{
		T* pT = static_cast<T*>(this);
		int lineNo, colNo;
		if (SyntaxGet(indx, lineNo, colNo))
		{
			m_currIndex = indx;
			CString source;
			pT->SyntaxGetSource(source);
			if (source.GetLength())
			{
				CSyntaxErrorVector errors;
				pT->SyntaxGet(source, errors);
				pT->m_owner->DoCheckAttributeSyntax(source, CreateIAttributeECLType(), errors);
			}
			else if (lineNo >= 0)
			{
				int pos = pT->SetSelLineCol(lineNo, colNo);
				pT->MarkerDeleteAll(MARKER_ARROW);
				pT->MarkerAdd(lineNo, MARKER_ARROW);
				pT->AnnotationClearAll();
				pT->AnnotationSetText(lineNo, static_cast<const TCHAR *>(m_list[m_currIndex].m_message));
				pT->AnnotationSetStyle(lineNo, m_list[m_currIndex].m_type == SYNTAX_WARNING ? 36 : 37);  //(36, 37 are from the categories in the color xml file).
				pT->SetFirstVisibleLine(lineNo - 3 >= 0 ? lineNo - 3 : 0);
				//pT->CallTipCancel();
				//pT->CallTipShow(pos, m_list[m_currIndex].m_message);
			}
			return true;
		}
		return false;
	}

	bool SyntaxSelNext()
	{
		return SyntaxGoto(m_currIndex + 1);
	}

	bool SyntaxSelPrevious()
	{
		return SyntaxGoto(m_currIndex - 1);
	}

	bool SyntaxHasNext()
	{
		return m_currIndex < static_cast<int>(m_list.size()) - 1;
	}

	bool SyntaxHasPrevious()
	{
		return m_currIndex > 0;
	}

	void SyntaxGet(const CString & modAttrLabel, CSyntaxErrorVector & result)
	{
		for(CSyntaxErrorVector::iterator itr = m_list.begin(); itr != m_list.end(); ++itr)
		{
			if(modAttrLabel.CompareNoCase(itr->m_fileName) == 0 && itr->m_code != 2101)
				result.push_back(*itr);
		}
	}
	void SyntaxSet(const CSyntaxErrorVector & error)
	{
		SyntaxClearAll();
		for(CSyntaxErrorVector::const_iterator itr = error.begin(); itr != error.end(); ++itr)
		{
			//FileName needs to be blank to indicate that the "new" owner is the current window.
			SyntaxAdd(itr->m_type, itr->m_code, _T(""), itr->m_message, itr->m_lineNo, itr->m_colNo);
		}
	}
};
