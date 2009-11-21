@echo off
del /Q pborcaw.zip
svn export deploy pborcaw
svn export ..\test2 pborcaw\test2
copy ..\bin\*.exe pborcaw\
pkzipc -add -lev=9 -path=specify -rec pborcaw.zip pborcaw\*.*
rmdir /Q /S pborcaw
copy /Y deploy\pborca.htm .\
