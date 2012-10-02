set binFolder=%WindowsSdkDir%\bin
set strSignCode="%binFolder%\signtool.exe"
set strTimeStampUrl="http://timestamp.verisign.com/scripts/timstamp.dll"
set pw=789olkju7

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%ROOT_DIR%\build
set ECLIDE_DIR=%SRC_DIR%\eclide
set SETUP_DIR=%ECLIDE_DIR%\setup
set STAGE_DIR=%ROOT_DIR%\stage

"%BUILD_DIR%\setup\bin\Release\Dir2WXS.exe" "%STAGE_DIR%" > "%SETUP_DIR%\Generated.wxs"

%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% "%STAGE_DIR%\PF\HPCC\ECL IDE\*.exe" 
%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% "%STAGE_DIR%\PF\HPCC\ECL IDE\*.dll" 
%strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% "%STAGE_DIR%\PF\HPCC\Graph Control\*.dll" 
REM %strSignCode% sign /f "%SETUP_DIR%\hpcc_code_signing.pfx" /p %pw% /t %strTimeStampUrl% "%STAGE_DIR%\PF\HPCC\Graph Control x64\*.dll" 
