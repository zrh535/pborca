#

session begin pborc100.dll

library create lib\test.pbl, 

#set liblist begin
#lib\test.pbl
#set liblist end


set application ,


import expb\*.sr?, lib\test.pbl

session end

