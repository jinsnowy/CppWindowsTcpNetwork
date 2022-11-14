protoc.exe -I=./ --cpp_out=./Libraries/Include/protocol ./Protocol.proto
IF ERRORLEVEL 1 PAUSE