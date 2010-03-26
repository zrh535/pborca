#

session begin pborc100.dll

#make target directory as current
cd pbl\target

target set liblist pbtestx.pbt
target set app     pbtestx.pbt

build app full

build exe pbtestx.exe, ,pbtestx.pbr , pcode

session end

