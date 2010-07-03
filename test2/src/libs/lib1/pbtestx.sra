$PBExportHeader$pbtestx.sra
$PBExportComments$Generated Application Object
forward
global type pbtestx from application
end type
global transaction sqlca
global dynamicdescriptionarea sqlda
global dynamicstagingarea sqlsa
global error error
global message message
end forward

global type pbtestx from application
string appname = "pbtestx"
end type
global pbtestx pbtestx

on pbtestx.create
appname="pbtestx"
message=create message
sqlca=create transaction
sqlda=create dynamicdescriptionarea
sqlsa=create dynamicstagingarea
error=create error
end on

on pbtestx.destroy
destroy(sqlca)
destroy(sqlda)
destroy(sqlsa)
destroy(error)
destroy(message)
end on

event open;//привед
open(w_111)

end event

