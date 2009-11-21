#include <tchar.h>
#include <direct.h>

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "parm.h"
#include "orca.h"
#include "target.h"

#define IFERR(a)	if(!a)goto error;


typedef struct {
    TCHAR*       cmd;
    ORCA_FUNC   func;
    int         ftype;//0, 1, 2, 3, 4: number of parameters
}COMMANDINFO;

COMMANDINFO cmdinf[]={
	{TEXT("session begin"),      orcaSessionBegin,     1,}, //dllname
	{TEXT("session end"),        orcaSessionEnd,       0,},
	{TEXT("copy item"),          orcaLibraryEntryCopy, 4,},
	{TEXT("set application"),    orcaSetApplication,   2,},
	{TEXT("library create"),     orcaLibraryCreate,    2,},
	{TEXT("build app"),          orcaBuildApplication, 1,},
	{TEXT("build exe"),          orcaBuildExecutable,  4,},
	{TEXT("build library"),      orcaBuildLibrary,     3,}, //library, pbd, type=pbd|dll
	{TEXT("sys"),                orcaSys,              1,},
	{TEXT("timestamp"),          orcaTimestamp,        0,},
	{TEXT("delete item"),        orcaDeleteItem,       3,},
	{TEXT("echo"),               orcaEcho,             1,},
	{TEXT("delete duplex"),      orcaDeleteDuplex,     2,},
	{TEXT("regenerate"),         orcaRegenerate,       3,}, //library, entry, type
	{TEXT("regenerateall"),      orcaRegenerateAll,    0,}, 
	{TEXT("export"),             orcaExport,           4,}, //library, entry, type, folder
	{TEXT("import moveok"),      orcaImportMoveOk,     2,}, //srcFile, lib
	{TEXT("import move"),        orcaImportMove,       2,}, //srcFile, lib
	{TEXT("import pbg2pbl any"), orcaImportPbg2PblAny, 2,}, //path to pbg file, destination folder
	{TEXT("import pbg2pbl"),     orcaImportPbg2Pbl,    2,}, //path to pbg file, destination folder
	{TEXT("import"),             orcaImport,           2,}, //srcFile, lib
//	{TEXT("scc list"),           orcaCheckList,        1,}, //lib
	{TEXT("cd"),                 orcaCD,               1,}, //new directory
	{TEXT("target create lib"),  orcaTargetCreateLib,  1,}, //pb target file
	{TEXT("target import any"),  orcaTargetImportAny,  2,}, //pb target file, source directory
	{TEXT("target import"),      orcaTargetImport,     2,}, //pb target file, source directory
	{TEXT("target set app"),     orcaTargetSetApp,     1,}, //pb target file
	{TEXT("target set liblist"), orcaTargetSetLiblist, 1,}, //pb target file
	{TEXT("set exeinfo property"), orcaSetExeInfo,     2,}, //property_name, value
	{TEXT("profile string"),     orcaProfileString,    3,}, //file, section, key
	{TEXT("profile exeinfo"),    orcaProfileExeInfo,   2,}, //file, section
	{NULL,                       NULL,                 0,}, //last null item
};


int linenumber=0;
TCHAR _script[MAX_PATH+5];
BOOL wait=false;

void printline(){
	if(linenumber)_tprintf(TEXT("%s (%i) : "),_script,linenumber);
}
void settitle(TCHAR*info){
	TCHAR title[1024];
	_tcscpy(title,info);
	_tcscat(title,_script);
	_tcscat(title,TEXT(" - pborca"));
	
	SetConsoleTitle(title);
}

int help(){
	printf("version 2009-11-19 ");
#ifdef _UNICODE
	printf("(UNICODE)\n");
#else
	printf("(ANSI)\n");
#endif
	printf("PBORCA: allows you to execute batch scripts \n\tto process PowerBuilder libraries\n\n");
	printf("usage: pborca [options] <script.orc>\n");
	printf("\twhere options:\n");
	printf("\t-D<key>=<value>          Set environment variable (repeatable)\n");
	printf("\t-w                       Wait for user action at the end of the script\n");
	
	return 1;
}

TCHAR * trim(TCHAR * c,TCHAR*space){
	int i=0;
	while(c[0]&&_tcschr(space,c[0]))c++;
	i=_tcslen(c);
	do{
		c[i]=0;
		i-- ;
	}while( i>=0 && _tcschr(space,c[i]) );
	return c;
}

BOOL call_command(TCHAR*cmd){
	TCHAR*parm;
	TCHAR**parmlist=NULL;
	int parmcount,k;
	BOOL ret;
	
	for(int i=0;cmdinf[i].cmd;i++){
		if( (parm = iscmd(cmd,cmdinf[i].cmd) ) ){
			if(i>0 /*session begin*/ && !isSessionStarted())return false;
			parmcount=getParmCount(parm);
			if(parmcount!=cmdinf[i].ftype){
				printline();
				_tprintf(TEXT("wrong parm count=%i for command \"%s\"\n"),parmcount,cmdinf[i].cmd);
				return false;
			}
			if(parmcount)parmlist=new TCHAR*[cmdinf[i].ftype];
			for(k=0; k<parmcount; k++) parmlist[k]=getParm(parm,k+1);
			ret=cmdinf[i].func(parmlist);
			for(k=0; k<parmcount; k++) freeParm(parmlist[k]);
			if(parmlist)delete []parmlist;
			parmlist=NULL;
			return ret;
		}
	}
	printline();
	_tprintf(TEXT("unknown command: %s\n"),cmd);
	return false;
}

void set_dir(char*path){
	//set current dir to the script
	int dir=0;
	for(int i=0;path[i];i++){
		if(path[i]=='\\' || path[i]=='/')dir=i;
	}
	if(dir){
		path[dir]=0;
		SetCurrentDirectoryA(path);
		path[dir]='\\';
	}
	WIN32_FIND_DATAA ffd;
	HANDLE h=FindFirstFileA(path,&ffd);
	if(h!=INVALID_HANDLE_VALUE){
		//_tcscpy(_script,ffd.cFileName);
		//ANSI support
		//MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,ffd.cFileName,-1,_script,_tsizeof(_script));
		_stprintf(_script,TEXT("%hs"),ffd.cFileName);
		FindClose(h);
	}else{
		_tcscpy(_script,TEXT("Wrong filename!"));
	}
}

void _wait(){
	if(wait){
		printf("press any key ...");
		getch();
	}
}
BOOL setEnvVariable(char*str){
	BOOL ret;
	char * pos=strchr(str,'=');
	if (!pos) {
		printf("the symbol = is required for key -D%s\n",str);
		return false;
	}
	pos[0]=0;
	ret=SetEnvironmentVariableA(str,pos+1);
	pos[0]='=';
	if(!ret)
		printf("can't set variable: %s\n",str);
	return ret;
}


int main(int argc, char* argv[]){
	char * orcfile=NULL;
	//parse params
	for(int i=1;i<argc;i++){
		if(argv[i][0]=='-'){
			switch(argv[i][1]){
				case '?':return help();
				case 'h':return help();
				case 'w':{
					wait=true;
					break;
				}
				case 'D':{
					if(!setEnvVariable(argv[i]+2))return false;
					break;
				}
				default:return help();
			}
		}else{
			if(orcfile)return help();
			orcfile=argv[i];
		}
	}
	if(!orcfile)return help();
	
	
	set_dir(orcfile);
	settitle(TEXT(""));
	
	FILE * f=fopen( orcfile, "rt");
	if(f){
		TCHAR buf[4096];
		TCHAR *str;
		
		int status=0; //0:usual; 1:set liblist
		linenumber=0;
		while(_fgetts(buf,_tsizeof(buf),f)){
			linenumber++;
			if(_tcslen(buf)>4000){
				printline();
				printf("line is longer then 4000 symbols\n");
				goto error;
			}
			if(status==1){
				str=trim(buf);
				if(str[0]==0||str[0]=='#')continue;
				if(!_tcscmp(str,TEXT("set liblist end"))){
					status=0;
					if(!isSessionStarted())goto error;
					orcaSetLiblistEnd(NULL);
				}else{
					//add library to list
					IFERR(orcaSetLiblistAdd(str));
				}
			}else{
				str=trim(buf);
				if(str[0]==0||str[0]=='#')continue;
				if( !_tcscmp(str,TEXT("set liblist begin")) )status=1;
				else if(!call_command(str))goto error;
			}
			
		}
		fclose(f);
		f=NULL;
	}else{
		printsyserr();
		printf("can't open script file \"%s\". errno=%i\n",orcfile,errno);
		goto error;
	}
	
	
	orcaSessionEnd(NULL);
	printf("\nOK\n");
	settitle(TEXT("[+] "));
	_wait();
	return 0;
	error:
	if(f)fclose(f);
	f=NULL;
	orcaSessionEnd(NULL);
	printf("\nFAIL\n");
	settitle(TEXT("[!] "));
	_wait();
	return 1;
}
