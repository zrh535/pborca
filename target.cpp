#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include "pborca.h"
#include "orca.h"
#include "target.h"



//method to parse target file "*.pbt"
//call back function cb on each entry
//cb have to return true if everything is ok
BOOL _readTargetFile( TCHAR * fname, ORCA_TARGET_INFO cb, void*udata ){
	FILE * f=_tfopen(fname,TEXT("rt"));
	TCHAR c;
	int i=0;
	int itype=-1;
	TCHAR buf[MAX_PATH+1];
	//0: default, 1:valid key chars, 2: key end (space), 3: the value quoted with '"' 
	int status=0; 
	BOOL err=false;
	
	if(f){
		//create variable that contains source directory
		TCHAR * srcDir=new TCHAR[ _tcslen(fname) +2 ];
		_tcscpy(srcDir,fname);
		getFileDir(srcDir);
		
		while( (c=_fgettc(f))!=_TEOF ){
			if(status==0){
				c=_totlower(c);
				if(c>='a' && c<='z' ){
					status=1;
					i=0;
				}
			}
			
			if(status==1){
				c=_totlower(c);
				if(c>='a' && c<='z' ){
					if(i<=MAX_PATH){buf[i]=c;i++;}
				}else{
					buf[i]=0;
					status=2;
					if( !_tcscmp(TEXT("appname"), buf) )
						itype=TARGET_INFO_APP;
					else if( !_tcscmp(TEXT("applib"),  buf) )
						itype=TARGET_INFO_APP_LIB;
					else if( !_tcscmp(TEXT("liblist"), buf) )
						itype=TARGET_INFO_LIB;
					else {
						//find newline or end of file
						while( c!=_TEOF && c!='\r' &&c!='\n' ){
							c=_fgettc(f);
						}
						itype=-1;
						status=0;
					}
				}
			}
			
			if(status==2){
				if(c=='"'){
					status=3;
					i=0;
					continue; //goto next char;
				}
			}
			if(status==3){
				if(c==';'){
					buf[i]=0;
					i=0;
					if(itype!=-1){
						if( !cb(itype,buf,srcDir,udata) ){
							err=true;
							break;
						}
					}
					continue; //goto next char;
				}else if(c=='"'){
					buf[i]=0;
					i=0;
					if(itype!=-1){
						if( !cb(itype,buf,srcDir,udata) ){
							err=true;
							break;
						}
					}
					//find newline or end of file
					while( c!=_TEOF && c!='\r' &&c!='\n' ){
						c=_fgettc(f);
					}
					status=0;
					continue; //goto next char;
				}else if(c=='\\'){
					c=_fgettc(f);
					if(c==_TEOF)break;
					switch(c){
						case 'n': c='\n';break;
						case 'r': c='\r';break;
						case 't': c='\t';break;
					}
					if(i<=MAX_PATH){buf[i]=c;i++;}
				}else{
					if(i<=MAX_PATH){buf[i]=c;i++;}
				}
			}
			if(c==_TEOF || err)break;
		}
		delete []srcDir;
		fclose(f);
	}else{
		printline();
		_tprintf(TEXT("can't find target file \"%s\".\n"),fname);
		return false;
	}
	return !err;
}
