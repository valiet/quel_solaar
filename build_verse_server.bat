@echo off

pushd .
call vsenv.cmd
popd

set LIBS = kernel32.lib user32.lib gdi32.lib shell32.lib Ws2_32.lib

cl -O1 -W2 -Zi -MT /nologo /Fobuild\server\ /Febin\server.exe /Iverse verse\*.c verse\server\*.c %LIBS%
