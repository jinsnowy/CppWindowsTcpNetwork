protoc.exe -I=./ --cpp_out=./Include/protocol ./UserProtocol.proto
IF ERRORLEVEL 1 PAUSE