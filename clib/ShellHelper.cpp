#include "StdAfx.h"

#include "shellhelper.h"

bool ShellExecOpen(const TCHAR *fileName)
{
	SHELLEXECUTEINFO info;
	memset(&info,0,sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.lpFile = fileName;
	return ::ShellExecuteEx(&info) == TRUE;
}

bool ShellExecExplore(const TCHAR *fileName)
{
	SHELLEXECUTEINFO info;
	memset(&info,0,sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.lpVerb = _T("open");
	info.lpFile = fileName;
	info.lpDirectory = fileName;
	return ::ShellExecuteEx(&info) == TRUE;
}

const TCHAR * FindExecutableForDocument(const TCHAR *fileName, std::_tstring & result)
{
	result.resize(MAX_PATH + 1);
	result[0] = 0;
	::FindExecutable(fileName, 0, &result[0]);
	return result.c_str();
}

const TCHAR * DocumentTypeName(const TCHAR *fileName, std::_tstring & result)
{
	SHFILEINFO info;
	info.szTypeName[0] = 0;
	::SHGetFileInfo(fileName, FILE_ATTRIBUTE_NORMAL, &info, sizeof(info), SHGFI_USEFILEATTRIBUTES + SHGFI_TYPENAME);
	result = info.szTypeName;
	return result.c_str();
}