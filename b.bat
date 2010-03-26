@echo off
set path=d:\HOME\VC6\bin;%path%
set include=d:\HOME\VC6\include
set lib=d:\HOME\VC6\lib

nmake /f makefilea clean

nmake /f makefilea
if errorlevel=1 exit /B 0

nmake /f makefilew clean
nmake /f makefilew
if errorlevel=1 exit /B 0

nmake /f makefilea clean
