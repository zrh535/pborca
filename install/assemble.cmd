@echo off
del /Q *.zip

if NOT EXIST ..\bin\pborca.exe exit /0

for /F %%i in ('..\bin\pborca.exe -v') do set PBORCA_%%i

set target=pborca_%PBORCA_VERSION%

svn export deploy %target%
svn export ..\test2 %target%\test2

copy ..\bin\*.exe %target%\
pkzipc -add -lev=9 -path=specify -rec %target%.zip %target%\*.*
rmdir /Q /S %target%
