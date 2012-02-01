#pragma once

#include "comms.h"

COMMS_API bool ChangePassword(const CString &serverIP, const CString &user, const CString &oldPassword, const CString &newPassword, const CString &confirmPassword, int &retCode, CString &retMsg);
COMMS_API bool VerifyUser(IConfig * config, const CString &user, const CString &password, int &retCode, CString &retMsg);
