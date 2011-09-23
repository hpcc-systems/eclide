#pragma once

#include "Repository.h"

bool GetInsertModule(IModule * parent, CString & label, bool validate);
bool GetRenameModule(IModule * parent, CString & label, bool validate);
bool GetInsertAttribute(const std::_tstring & module, CString & label, CComPtr<IAttributeType> & type, bool validate);
bool GetRenameAttribute(const std::_tstring & module, CString & label, IAttributeType * type, bool validate);
bool SetVersionLabel(CString & label, bool SetAndValidate);
bool GetModuleLabel(std::_tstring & label);
bool GetAttributeLabel(CString & label);
bool SaveWorkspaceLabel(IRepository * repository, CString & label);
