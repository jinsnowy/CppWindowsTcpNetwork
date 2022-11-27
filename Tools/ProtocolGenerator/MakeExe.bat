pushd %~dp0
pyinstaller --onefile ProtocolGenerator.py
MOVE .\dist\ProtocolGenerator.exe .\GenProtocols.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\ProtocolGenerator.spec
PAUSE