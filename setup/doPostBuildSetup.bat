set binFolder=%WindowsSdkDir%\bin
set strSignCode="%binFolder%\signtool.exe"
set strTimeStampUrl="http://timestamp.verisign.com/scripts/timstamp.dll"
set pw=789olkju7

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%ROOT_DIR%\build
set ECLIDE_DIR=%SRC_DIR%\eclide
set SETUP_DIR=%ECLIDE_DIR%\setup

%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% /d SetupECLIDE.msi "%BUILD_DIR%\setup\msi\SetupECLIDE.msi"
%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% /d SetupGraphControl.msi "%BUILD_DIR%\setup\msi\SetupGraphControl.msi"
%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% /d SetupGraphControl64.msi "%BUILD_DIR%\setup\msi\SetupGraphControl64.msi"
