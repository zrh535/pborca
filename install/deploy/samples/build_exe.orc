#
# created by Dmitry Lukyanov

session begin pborc105.dll

timestamp

# expecting the following directory structure in svn
# note that "projectname", "img"  and "stdlib" are just samle names
# "stdlib" is a directory that contains a set of PBLs
# in repository they will be stored in the same subdirectories
# 
# /projectname/trunk
# /projectname/trunk/pb                 powerbuilder source files *.sr?, *.pbg, *.pbt
# /projectname/trunk/pb/img             image resources directory
# /projectname/trunk/pb/stdlib          powerbuilder source files for your "stdlib" PBL's
# ...
# /projectname/trunk/pb/stdlibX         powerbuilder source files for your "stdlibX" PBL's
# /projectname/trunk/build              the directory where you can store *.orc, *.bat, *.ini, *.pbw files




# export sources from svn repository to future exe
sys svn export ..\pb  ..\..\exe
# replace $WCREV$ in build.ini and put result in the future exe directory
sys subwcrev ..\ build.ini ..\..\exe\build.ini

cd ..\..\exe

echo create PBLs
# create libraries from target file expected PRJNAME variavle to be defined.
target create lib .\%%PRJNAME%%.pbt

# set library list from target file
target set liblist .\%%PRJNAME%%.pbt

# set default pb application (we don't have other in empty libs)
set application  ,


echo import objects
#import all objects according pbg files defined in target file
target import .\%%PRJNAME%%.pbt, .\

# delete imported objects
sys del /S /Q .\*.sr?
sys del /S /Q .\*.pbg


#end orca session. because we have to switch application
session end 

#in sybase pborcXX.dll there are could be memory leak
#so do pure memory cleanup, split this file 



session begin pborc105.dll

# set liblist and application from target
target set liblist .\%%PRJNAME%%.pbt
target set app .\%%PRJNAME%%.pbt

# do migration of the application (like full rebuild)
build app migrate

# build executable
build exe %%PRJNAME%%.exe, img\%%PRJNAME%%.ico, %%PRJNAME%%.pbr, pcode

# delete other source files
sys del /Q *.pbr
sys del /Q *.pbl
sys del /Q *.pbw
sys del /Q *.pbt
sys rmdir /Q /S img

session end 

#done
