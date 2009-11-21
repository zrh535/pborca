@echo off

echo -------------------------------------------------------------
echo This batch will copy PBORCA.EXE to WINDOWS\SYSTEM32 directory
echo and apply PBORCA.REG to your registry
echo -------------------------------------------------------------

echo After installation you will be able to 
echo open context menu for the "*.ORC" files
echo and select one of the following items:
echo - Edit Script  (notepad.exe)
echo - Run Script   (pborca.exe)
echo -------------------------------------------------------------



echo press CTRL+C to cancel
pause
copy pborca.exe %SYSTEMROOT%\system32
regedit.exe /S pborca.reg
