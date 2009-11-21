DW Ripper Toolset 0.1a for Windows (Jan. 2003)
==============================================

Purpose:
--------
A simple commandline oriented tool to restore Datawindows
from a PowerBuilder generated Library (PBL/PBD/DLL/EXE) 
or to produce a list of all entries.

Installation:
-------------
Unpack the Zip-File to any directory.

Make sure that required PB Runtime DLLs of needed versions 
(PowerBuilder 5-9) can be found by the program. 
(Normally the directories containing the Runtime DDLs are included
in the PATH). 

File Descriptions:
------------------
dw_ripper.exe     (Shell-Program, PB independent)
dw_ripper5.exe    (dw_ripper for PB5)
dw_ripper6.exe    (dw_ripper for PB6)
dw_ripper7.exe    (dw_ripper for PB7)
dw_ripper8.exe    (dw_ripper for PB8)
dw_ripper9.exe    (dw_ripper for PB9b4)
readme.txt        (this file)

How to use:
----------
The DW Ripper toolset comes with one independent program (dw_ripper.exe)
and several PB-version related programs (dw_ripper5-9.exe). 

dw_ripper.exe (without a version number in it's name) is the 
main program, that executes the PB-version related dw_ripperX.exe
to process the ripping of Datawindows out of a specified 
PB-File. A valid PB-File is a PBL, PBD, DLL or EXE, which was created 
with PowerBuilder.

For quick use:

dw_ripper -?                   (show help)
dw_ripper -i my_app.dll        (list objects in my_appl.dll)
dw_ripper -i my_app.dll -o dws (extract datawindows to the directory dws)

Known bugs/problems:
-----------
Datetime is always displayed in GMT (+-0:00) and not converted to LocalTimeZone.

Licence, distribution and copyright:
------------------------------------
DW Ripper is provided as is. No guarantees or warrantees accompany it. 
Users should be aware that the tool can create output files (*.srd) 
which might overwrite existing files.

DW Ripper can be used free.
Copyright 2002-2003 © by A. Schmidt, dwoX.com.

Contact:
--------
Please report all bugs and suggestions to DW_Ripper@dwoX.com
DW Ripper homepage with the latest version and documentation is at:
http://www.dwoX.com

