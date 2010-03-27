@echo off
rmdir /Q /S pbl
rmdir /Q /S exe

set PBORCA=..\pborca.exe
rem next command to support current svn repository
if exist "..\bin\pborca.exe" set PBORCA=..\bin\pborca.exe

%PBORCA% build_pbl.orc
%PBORCA% build_exe.orc

mkdir exe

xcopy pbl\target\*.exe        exe\
xcopy pbl\target\tlib\*.pbd   exe\
xcopy pbl\libs\lib1\*.pbd     exe\
xcopy pbl\libs\lib2\*.pbd     exe\