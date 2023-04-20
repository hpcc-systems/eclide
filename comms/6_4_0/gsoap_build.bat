set GSOAP=..\..\build\vcpkg_installed\x86-windows\share\gsoap
set GSOAP_BIN=%GSOAP%\bin\win32
set wsdl2h=%GSOAP_BIN%\wsdl2h.exe -I%GSOAP% -I.\hpcc
set soapcpp2=%GSOAP_BIN%\soapcpp2.exe -C -x -i -I%GSOAP%\import

%wsdl2h% -o Services.h Ws_Account.wsdl WsAttributes.wsdl WsDfu.wsdl WsSMC.wsdl WsTopology.wsdl WsWorkunits.wsdl

%soapcpp2% Services.h

%soapcpp2% -penv .\hpcc\env.h
