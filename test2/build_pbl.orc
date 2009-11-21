#

session begin pborc100.dll

sys mkdir pbl
sys xcopy /S /E /EXCLUDE:xcopy_ex.txt src\*.* pbl\

#make target directory as current
cd pbl\target

target create lib  pbtestx.pbt
target set liblist pbtestx.pbt
#set default application for the new libraries
set application , 
#import
target import      pbtestx.pbt, .\

session end

