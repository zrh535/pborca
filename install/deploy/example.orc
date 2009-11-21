# script for compiling pb project with pborca.exe tool
# all parameters should be separated by comma
# to specify comma itself use meta symbol %
# example: build executable exeName, iconName, pbrName


#the command to start orca session for PB version 70
session begin pborc70.dll


#entryName or/and entryType could be * for all objects
#entryType: app, dw, fn, menu, query, struct, uo, pipe, proxy, or win.
copy item pblSrc, entryName, entryType, pblDst

#creates a new library with comments
library create pblName, comments

#the list could be separated by ; like in PB library list
set liblist begin
#here also could be some comments
#to include PBL into exe - specify 0 after coma
1.pbl;  ,0
#you can cpesify a resource file for PBL (PBL should be compiled to PBD/DLL)
2.pbl  ,1, pbl2.pbr
3s.pbl
4.pbl;
set liblist end




#sets current application
set application pblName, applicationName

#rebuild application full, incremental, or migrate
build app <full | migrate | incremental >

#creates executable
build exe exeName, iconName, pbrName, <pcode | machinecode>


#deletes object
delete item pblName, entryName, entryType


#deletes objects from deleteFromLib that found in primaryLib
delete duplex primaryLib, deleteFromLib


#executes system command
sys <system command>

#Displays text messages
echo text

#displays current date and time
timestamp

#ends orca session
session end


