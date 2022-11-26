protoc.exe -I=./ --cpp_out=./Include/protocol ./Enum.proto
protoc.exe -I=./ --cpp_out=./Include/protocol ./Struct.proto
protoc.exe -I=./ --cpp_out=./Include/protocol ./UserProtocol.proto
IF ERRORLEVEL 1 PAUSE