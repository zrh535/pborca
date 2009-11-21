@echo off
rmdir /Q /S pbl

set PBORCA=..\pborca.exe
rem next command to support current svn repository
if exist "..\bin\pborca.exe" set PBORCA=..\bin\pborca.exe

%PBORCA% build_pbl.orc

