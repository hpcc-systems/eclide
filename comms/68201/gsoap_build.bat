set wsdl2h=..\..\..\Third_Party\gsoap\bin\win32\wsdl2h.exe -I..\..\..\Third_Party\gsoap -I..\..\..\Third_Party\gsoap\ws
set soapcpp2=..\..\..\Third_Party\gsoap\bin\win32\soapcpp2.exe -C -x -i -I..\..\..\Third_Party\gsoap\import

del ..\WsAccount\WsAccount.wsdl

%wsdl2h% -o Services.h ..\WsAccount\Ws_Account.wsdl ..\WsAttributes\WsAttributes.wsdl ..\WsDfu\WsDfu.wsdl ..\WsSMC\WsSMC.wsdl ..\WsTopology\WsTopology.wsdl ..\WsWorkunits\WsWorkunits.wsdl

%soapcpp2% Services.h

%soapcpp2% -penv ..\gsoap\env.h
pause