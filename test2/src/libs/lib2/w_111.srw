HA$PBExportHeader$w_111.srw
forward
global type w_111 from window
end type
type cb_1 from commandbutton within w_111
end type
end forward

global type w_111 from window
integer width = 1481
integer height = 1384
boolean titlebar = true
string title = "Untitled"
boolean controlmenu = true
boolean minbox = true
boolean maxbox = true
boolean resizable = true
long backcolor = 67108864
string icon = "AppIcon!"
boolean center = true
cb_1 cb_1
end type
global w_111 w_111

on w_111.create
this.cb_1=create cb_1
this.Control[]={this.cb_1}
end on

on w_111.destroy
destroy(this.cb_1)
end on

type cb_1 from commandbutton within w_111
integer x = 521
integer y = 412
integer width = 402
integer height = 112
integer taborder = 10
integer textsize = -10
integer weight = 400
fontcharset fontcharset = russiancharset!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "none"
end type

event clicked;MessageBox('','Ok')
end event

