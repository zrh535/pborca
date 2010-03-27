#include <tchar.h>

#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include "pborca.h"
#include "pborcx.h"
#include "orca.h"
#include "parm.h"
#include "impexp.h"
#include "target.h"
#include "pbg.h"


typedef struct {
    TCHAR*	libSrc;
    TCHAR*	libDst;
	TCHAR*	entryName;
	TCHAR*	entryType;
	int		count;
	BOOL	ret;
}ENTRYLISTCOPY;


typedef struct {
    TCHAR**    parm;
    int       item;
	int       type;
	ORCA_FUNC f;
	BOOL      ret;
}ITEMITERATOR;


typedef struct {
	TCHAR *lib;
	BOOL  pbd;
	TCHAR *pbr;
	void *next;
} LIBLIST;

//global variables
HPBORCA session=NULL;
LIBLIST *liblist=NULL;
BOOL isLibListSet=false;
HMODULE pborcdll=NULL;
int libcount=0;


SYSTEMTIME timestamp;


/*
    PBORCA_APPLICATION,     
    PBORCA_DATAWINDOW,      
    PBORCA_FUNCTION,      
    PBORCA_MENU,      
    PBORCA_QUERY,      
    PBORCA_STRUCTURE,      
    PBORCA_USEROBJECT,      
    PBORCA_WINDOW,
    PBORCA_PIPELINE,
    PBORCA_PROJECT,    PBORCA_PROXYOBJECT,
    PBORCA_BINARY
*/
TCHAR *objtype[]={TEXT("app"), TEXT("dw"), TEXT("fn"), TEXT("menu"), TEXT("query"), 
				TEXT("struct"), TEXT("uo"), TEXT("win"), TEXT("pipe"), TEXT("prj"), TEXT("proxy"), TEXT("bin"), NULL};
TCHAR *objext=TEXT("adfmqsuwpjx");//extension variants


//int srcsizemodif[]={28, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int log(TCHAR * c){
	_tprintf(TEXT("%s\n"),c);
	return 0;
}

TCHAR*rtrim(TCHAR*s){
	TCHAR*eol=s;
	for(TCHAR*c=s;c[0];c++){
		if(c[0]>' ')eol=c+1;
	}
	eol[0]=0;
	return s;
}

_XORCA_SessionGetError          XORCA_SessionGetError          ;
_XORCA_LibraryDirectory         XORCA_LibraryDirectory         ;
_XORCA_SessionSetLibraryList    XORCA_SessionSetLibraryList    ;
_XORCA_SessionOpen              XORCA_SessionOpen              ;
_XORCA_SessionClose             XORCA_SessionClose             ;
_XORCA_LibraryCreate            XORCA_LibraryCreate            ;
_XORCA_LibraryEntryCopy         XORCA_LibraryEntryCopy         ;
_XORCA_SessionSetCurrentAppl    XORCA_SessionSetCurrentAppl    ;
_XORCA_ApplicationRebuild       XORCA_ApplicationRebuild       ;
_XORCA_DynamicLibraryCreate     XORCA_DynamicLibraryCreate     ;
_XORCA_ExecutableCreate         XORCA_ExecutableCreate         ;
_XORCA_LibraryEntryDelete       XORCA_LibraryEntryDelete       ;
_XORCA_CompileEntryRegenerate   XORCA_CompileEntryRegenerate   ;
_XORCA_LibraryEntryInformation  XORCA_LibraryEntryInformation  ;
_XORCA_CompileEntryImport       XORCA_CompileEntryImport       ;
_XORCA_LibraryEntryExport       XORCA_LibraryEntryExport       ;
_XORCA_SetExeInfo               XORCA_SetExeInfo               ;

void printsyserr(){
	LPVOID lpMsgBuf;
	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf, 0, NULL );
	rtrim((TCHAR*)lpMsgBuf);
	printline();
	_tprintf(TEXT("%s\n"),lpMsgBuf);
	LocalFree( lpMsgBuf );
}

#define CHKFUN(x)	if( (x)==NULL ) {printsyserr();return false;}

BOOL isSessionStarted(){
	if(session)return true;
	printline();
	_tprintf(TEXT("session not started. use command \"session begin\".\n"));
	return false;
}

INT	WINAPI notSupportedException(){
	printline();
	_tprintf(TEXT("command not supported.\npress any key to halt...\n"));
	getch();
	//assert
	_asm { int 3 }
	return 0;
}

FARPROC _pborcProcAddress(HMODULE m, CHAR*proc){
	FARPROC p=GetProcAddress(m,proc);
	if(!p){
		_tprintf(TEXT("warning: %hs function not found in current pborca library.\nSome commands will be unavailable.\n"),proc);
		p=notSupportedException;
	}
	return p;
}

BOOL _pborcLoadDll(TCHAR*name){
	if(pborcdll){
		printline();
		_tprintf(TEXT("please stop session before starting new one.\n"));
		return false;
	}
	pborcdll=LoadLibrary(name);
	if(!pborcdll){
		printsyserr();
		return false;
	}
//	PBORCA_SessionClose f=GetProcAddress(pborcdll,"PBORCA_SessionClose");
	CHKFUN(XORCA_SessionGetError         = (_XORCA_SessionGetError        )_pborcProcAddress(pborcdll,"PBORCA_SessionGetError"        ));
	CHKFUN(XORCA_LibraryDirectory        = (_XORCA_LibraryDirectory       )_pborcProcAddress(pborcdll,"PBORCA_LibraryDirectory"       ));
	CHKFUN(XORCA_SessionSetLibraryList   = (_XORCA_SessionSetLibraryList  )_pborcProcAddress(pborcdll,"PBORCA_SessionSetLibraryList"  ));
	CHKFUN(XORCA_SessionOpen             = (_XORCA_SessionOpen            )   GetProcAddress(pborcdll,"PBORCA_SessionOpen"            ));
	CHKFUN(XORCA_SessionClose            = (_XORCA_SessionClose           )   GetProcAddress(pborcdll,"PBORCA_SessionClose"           ));
	CHKFUN(XORCA_LibraryCreate           = (_XORCA_LibraryCreate          )_pborcProcAddress(pborcdll,"PBORCA_LibraryCreate"          ));
	CHKFUN(XORCA_LibraryEntryCopy        = (_XORCA_LibraryEntryCopy       )_pborcProcAddress(pborcdll,"PBORCA_LibraryEntryCopy"       ));
	CHKFUN(XORCA_SessionSetCurrentAppl   = (_XORCA_SessionSetCurrentAppl  )_pborcProcAddress(pborcdll,"PBORCA_SessionSetCurrentAppl"  ));
	CHKFUN(XORCA_ApplicationRebuild      = (_XORCA_ApplicationRebuild     )_pborcProcAddress(pborcdll,"PBORCA_ApplicationRebuild"     ));
	CHKFUN(XORCA_DynamicLibraryCreate    = (_XORCA_DynamicLibraryCreate   )_pborcProcAddress(pborcdll,"PBORCA_DynamicLibraryCreate"   ));
	CHKFUN(XORCA_ExecutableCreate        = (_XORCA_ExecutableCreate       )_pborcProcAddress(pborcdll,"PBORCA_ExecutableCreate"       ));
	CHKFUN(XORCA_LibraryEntryDelete      = (_XORCA_LibraryEntryDelete     )_pborcProcAddress(pborcdll,"PBORCA_LibraryEntryDelete"     ));
	CHKFUN(XORCA_CompileEntryRegenerate  = (_XORCA_CompileEntryRegenerate )_pborcProcAddress(pborcdll,"PBORCA_CompileEntryRegenerate" ));
	CHKFUN(XORCA_LibraryEntryInformation = (_XORCA_LibraryEntryInformation)_pborcProcAddress(pborcdll,"PBORCA_LibraryEntryInformation"));
	CHKFUN(XORCA_CompileEntryImport      = (_XORCA_CompileEntryImport     )_pborcProcAddress(pborcdll,"PBORCA_CompileEntryImport"     ));
	CHKFUN(XORCA_LibraryEntryExport      = (_XORCA_LibraryEntryExport     )_pborcProcAddress(pborcdll,"PBORCA_LibraryEntryExport"     ));
	CHKFUN(XORCA_SetExeInfo              = (_XORCA_SetExeInfo             )_pborcProcAddress(pborcdll,"PBORCA_SetExeInfo"             ));

	return true;
}

void _pborcFreeDll(){
	if(pborcdll){
		FreeLibrary(pborcdll);
		pborcdll=NULL;
	}
}


int orcaError(int ret=-99){
	TCHAR buf[256];
	if(ret==0)return 0;
	
	XORCA_SessionGetError ( session, buf, sizeof(buf) );
	printline();
	if(buf[0]==0)_tprintf(TEXT("unknown error %i\n"),ret);
	else log(buf);
	return 1;
}


//returns directory name of the file name
//the last '\' symbol will be fould and replaced by '\0' (end of line)
//if no dir, then '.' returned
//beware! file buffer must be minimum 2 chars
//returns the input parameter
TCHAR* getFileDir(TCHAR*file){
	TCHAR *d=file;
	for(TCHAR*c=file;c[0];c++)if(c[0]=='\\' || c[0]=='/')d=c;
	if(d==file){
		_tcscpy(d,TEXT("."));
	}else{
		d[0]=0;
	}
	return file;
}


//--------------------- EXE INFO ------------------------
PBORCA_EXEINFO * exeinfo=NULL;
	//reset app info
	
	

void exeInfoReset(){
	if(exeinfo){
		if(exeinfo->lpszCompanyName)delete []exeinfo->lpszCompanyName;
		if(exeinfo->lpszProductName)delete []exeinfo->lpszProductName;
		if(exeinfo->lpszDescription)delete []exeinfo->lpszDescription;
		if(exeinfo->lpszCopyright)delete []exeinfo->lpszCopyright;
		if(exeinfo->lpszFileVersion)delete []exeinfo->lpszFileVersion;
		if(exeinfo->lpszFileVersionNum)delete []exeinfo->lpszFileVersionNum;
		if(exeinfo->lpszProductVersion)delete []exeinfo->lpszProductVersion;
		if(exeinfo->lpszProductVersionNum)delete []exeinfo->lpszProductVersionNum;
		if(exeinfo->lpszManifestInfo)delete []exeinfo->lpszManifestInfo;
		
		delete exeinfo;
		exeinfo=NULL;
	}
}


//the sequence of propnames must correspond to order of exeinfo fields.
TCHAR*exeinfo_props[]={__T("companyname"), __T("productname"), __T("description"), __T("copyright"), 
			__T("fileversion"), __T("fileversionnum"), __T("productversion"), __T("productversionnum"),
			__T("manifestinfo") };
#define EXEINFO_PROPCOUNT 9

bool exeInfoSetProperty(TCHAR*prop,TCHAR*val){
	if(!exeinfo){
		exeinfo=new PBORCA_EXEINFO;
		memset(exeinfo, 0x00, sizeof(PBORCA_EXEINFO));
	}
	
	TCHAR**dst=(TCHAR**)exeinfo;
	for(int i=0; i<EXEINFO_PROPCOUNT; i++){
		if( !_tcsicmp(exeinfo_props[i], prop ) ) {
			if(dst[i])delete [](dst[i]);
			dst[i]=new TCHAR[_tcslen(val)+2];
			_tcscpy(dst[i],val);
			return true;
		}
	}
	printline();
	_tprintf(TEXT("the property name for \"set exeinfo\" is incorrect \"%s\".\nValid onces:\n"),prop);
	for(i=0;i<EXEINFO_PROPCOUNT; i++){
		_tprintf(TEXT("\t%s\n"),exeinfo_props[i]);
	}
	return false;
}





//returns true if s is a datawindow resource
BOOL isDwResource(TCHAR*s){
	TCHAR * obr=_tcschr(s,'(');
	TCHAR * cbr;
	PBORCA_ENTRYINFO ei;
	int ret;
	
	if(!obr)return false;
	//search for the last open bracket 
	while(_tcschr(obr+1,'('))obr=_tcschr(obr+1,'(');
	cbr=_tcschr(obr,')');
	//if there is no close bracket, return false
	if(!cbr)return false;
	//if close bracket is not a last symbol, return false
	if(cbr[1])return false;
	obr[0]=0;
	cbr[0]=0;
	
	memset(&ei,0x00, sizeof(ei));
	ret=XORCA_LibraryEntryInformation ( session, s, obr+1, PBORCA_DATAWINDOW, &ei);
	obr[0]='(';
	cbr[0]=')';
	if(orcaError(ret))return false;
	return true;
}

BOOL checkPBR(TCHAR*pbrName){
	if(!pbrName[0])return true;
	TCHAR buf[4096];
	BOOL ret=true;
	FILE * f=_tfopen(pbrName,TEXT("rt"));
	if(f){
		while(_fgetts(buf,_tsizeof(buf),f)){
			rtrim(buf);
			//buf[_tcscspn(buf,TEXT("\t\r\n"))]=0; //rtrim
			if( _taccess(buf,0) ){
				if( !isDwResource(buf) ){
					printline();
					_tprintf(TEXT("can't find resource \"%s\" specified in PBR file \"%s\".\n"),buf,pbrName);
					ret=false;
					break;
				}
			}
		}
		fclose(f);
	}else{
		printline();
		_tprintf(TEXT("can't find PBR file \"%s\".\n"),pbrName);
		return false;
	}
	return ret;
}

PBORCA_TYPE ctype2orca(TCHAR*c){
	int i;
	for(i=0;objtype[i];i++)if(!_tcscmp(c,objtype[i]))return (PBORCA_TYPE)i;
	printline();
	_tprintf(TEXT("wrong type \"%s\".\n\tavailable types:"),c);
	for(i=0;objtype[i];i++)_tprintf(TEXT(" %s%c"),objtype[i],(objtype[i+1]?',':'.'));
	return (PBORCA_TYPE)-1;//error
}


TCHAR*unify_dirname(TCHAR*dir){
	for(TCHAR*c=dir; c[0]; c++){
		if( (c[0]=='/' || c[0]=='\\' )&& c[1]==0 ){
			c[0]=0;
			break;
		}
	}
	return dir;
}

BOOL _fileIterator(ORCA_FUNC of,TCHAR**parm,int maskParm){
	BOOL bret=true;
	TCHAR*fmask=parm[maskParm];
	
	TCHAR*fpath=new TCHAR[_tcslen(fmask)+MAX_PATH+2];
	TCHAR*fname=fpath;
	_tcscpy(fpath,fmask);
	
	for(int i=0;fpath[i]!=0;i++){
		if(fpath[i]=='\\'||fpath[i]=='/')
			fname=fpath+i+1;
	}
	
	
	WIN32_FIND_DATA ffd;
	HANDLE ffh=FindFirstFile(fmask,&ffd);
	if(ffh==INVALID_HANDLE_VALUE){
		printline();
		_tprintf(TEXT("no files found for \"%s\"\n"),fmask);
		goto error;
	}
	
	do{
		_tcscpy(fname,ffd.cFileName);
		parm[maskParm]=fpath;
		bret=of(parm);
		parm[maskParm]=fmask;
	}while( FindNextFile(ffh,&ffd) && bret);
	goto end;
	error:
	bret=false;
	end:
	delete []fpath;
	if(ffh!=INVALID_HANDLE_VALUE)FindClose(ffh);
	return bret;
}


void __stdcall _cbItemIterator(PPBORCA_DIRENTRY dir, void*data){
	ITEMITERATOR * ii = (ITEMITERATOR*)data;
	TCHAR*item=ii->parm[ii->item];
	TCHAR*type=ii->parm[ii->type];
	PBORCA_TYPE t;
	BOOL bret;
	if( !ii->ret )return;
	if( item[0] && _tcscmp(item,dir->lpszEntryName))return;
	
	if( type[0] ) {
		t=ctype2orca(type);
		if(t==-1){
			ii->ret=false;
			return;
		}
		if(t!=dir->otEntryType)return;
	}
	ii->parm[ii->item]=dir->lpszEntryName;
	ii->parm[ii->type]=objtype[dir->otEntryType];
	bret=ii->f(ii->parm);
	ii->parm[ii->item]=item;
	ii->parm[ii->type]=type;
	if( !bret ){
		ii->ret=false;
		return;
	}
}


BOOL _itemIterator(ORCA_FUNC of,TCHAR ** parm, int lib, int item, int type){
	ITEMITERATOR ii;
	TCHAR cmt[PBORCA_MAXCOMMENT+1];
	ii.parm=parm;
	ii.item=item;
	ii.type=type;
	ii.f   =of;
	ii.ret =true;
	int ret=XORCA_LibraryDirectory(session,parm[lib],cmt,PBORCA_MAXCOMMENT+1,_cbItemIterator,(void*)&ii);
	if(orcaError(ret))return false;
	
	if(!ii.ret)return false;
	return true;
}




BOOL orcaSetLiblistStart(TCHAR**parm){
	if(liblist){
		printline();
		_tprintf(TEXT("\"set liblist start\" without \"session close\"\n"));
		return false;
	}
	libcount=0;
	return true;
}
BOOL orcaSetLiblistAdd(TCHAR*param){
	if(isLibListSet){
		printline();
		_tprintf(TEXT("you must restart session before setting new library list."));
		return false;
	}

	int  parmcount;
	TCHAR *pbd=NULL;
	parmcount=getParmCount(param);
	if(parmcount>3 || parmcount<1){
		wrongformat:
		printline();
		_tprintf(TEXT("wrong library list format.\n"));
		return false;
	}
	
	LIBLIST *n;
	n=new LIBLIST;
	n->lib=getParm(param,1);
	if(parmcount>1){
		n->pbd=-99;
		pbd=getParm(param,2);
		if(pbd[0]){
			if(!pbd[1]){
				if(pbd[0]=='1')n->pbd=1;
				if(pbd[0]=='0')n->pbd=0;
			}
		}
	}else n->pbd=1;
	//pbr file specified
	if(parmcount>2){
		n->pbr=getParm(param,3);
	}else{
		n->pbr=new TCHAR[1];
		n->pbr[0]=0;
	}
	n->next=NULL;
	
	if(liblist){
		LIBLIST *i=liblist;
		while(i->next)i=(LIBLIST*)i->next;
		i->next=n;
	}else{
		liblist=n;
	}
	libcount++;
	if(n->pbd==-99)goto wrongformat;
	if(n->pbd==0 && n->pbr[0]){
		printline();
		_tprintf(TEXT("you can't specify PBR file if PBL included into exe.\n"));
		return false;
	}
	return true;
}
BOOL orcaSetLiblistEnd(TCHAR**parm){ //no parameters
	_tprintf(TEXT("set liblist\n"));
	if(!liblist){
		printline();
		_tprintf(TEXT("\"set liblist\" with empy library list\n"));
		return false;
	}
	
	//convert list to array
	TCHAR**c=new TCHAR*[libcount];
	
	int k=0;
	LIBLIST *i=liblist;
	while(i){
		c[k]=i->lib;
		i=(LIBLIST*)i->next;
		k++;
	}
	int ret = XORCA_SessionSetLibraryList ( session, c, libcount);
	delete []c;
	if( orcaError(ret) )return false;
	isLibListSet=true;
	return true;
}

BOOL orcaSys(TCHAR**parm){//TCHAR*cmd){
	if(_tsystem(parm[0])){
		printline();
		_tprintf(TEXT("error calling system command\n"));
		return false;
	}
	return true;
}

BOOL orcaSessionBegin(TCHAR**parm){//no params
	_tprintf(TEXT("session begin\n"));
	if(!_pborcLoadDll(parm[0]))return false;
	session=XORCA_SessionOpen();
	exeInfoReset();
	if(session)return true;
	orcaError();
	return false;
}

BOOL orcaSessionEnd(TCHAR**parm){//no params
	if(session){
		_tprintf(TEXT("session end\n"));
		XORCA_SessionClose(session);
		session=0;
	}
	LIBLIST *i;
	while(liblist){
		i=liblist;
		liblist=(LIBLIST*)i->next;
		delete []i->lib;
		delete []i->pbr;
		delete i;
	}
	isLibListSet=false;
	libcount=0;
	_pborcFreeDll();
	return true;
}

BOOL orcaLibraryCreate(TCHAR**parm){//TCHAR*libname,TCHAR*comment){
	_tprintf(TEXT("create library \"%s\"\n"),parm[0]);
	int ret=XORCA_LibraryCreate ( session, parm[0], parm[1] );
	if(orcaError(ret))return false;
	return true;
}



BOOL fiOrcaLibraryEntryCopy (TCHAR**parm){
	// TCHAR*libsrc,TCHAR*objname,TCHAR * type,TCHAR*libdst
	PBORCA_TYPE t;
	int ret=0;
	TCHAR*libsrc =parm[0];
	TCHAR*objname=parm[1];
	TCHAR*type   =parm[2];
	TCHAR*libdst =parm[3];
	
	if(!objname[0] || !type[0]){
		return _itemIterator(fiOrcaLibraryEntryCopy,parm,0,1,2);
	}else{
		t=ctype2orca(type);
		if(t==-1)return false;
		ret=XORCA_LibraryEntryCopy ( session,libsrc,libdst,objname,t);
		if(orcaError(ret))return false;
	}
	return true;
}


BOOL orcaLibraryEntryCopy (TCHAR**parm){
	// TCHAR*libsrc,TCHAR*objname,TCHAR * type,TCHAR*libdst
	return _fileIterator(fiOrcaLibraryEntryCopy,parm,0);
}



BOOL orcaSetApplication(TCHAR**parm){//TCHAR*libname,TCHAR*appname){
	_tprintf(TEXT("set application \"%s\"\n"),parm[1]);
	
	int ret=XORCA_SessionSetCurrentAppl ( session, (parm[0][0]?parm[0]:NULL), (parm[1][0]?parm[1]:NULL) );
	if(orcaError(ret))return false;
	return true;
}

void __stdcall cbOrcaErrProc(PPBORCA_COMPERR err, void*uData){
	if(uData)_tprintf(TEXT("%s\n"),err->lpszMessageText);
}

BOOL orcaBuildApplication(TCHAR**parm){//TCHAR*type){
	_tprintf(TEXT("rebuild: %s\n"),parm[0]);
	PBORCA_REBLD_TYPE eRebldType=(PBORCA_REBLD_TYPE)-1;
	if( !_tcscmp(parm[0], TEXT("full")) )eRebldType=PBORCA_FULL_REBUILD;
	else if( !_tcscmp(parm[0], TEXT("migrate")) )eRebldType=PBORCA_MIGRATE;
	else if( !_tcscmp(parm[0], TEXT("incremental")) )eRebldType=PBORCA_INCREMENTAL_REBUILD;
	
	int ret=XORCA_ApplicationRebuild ( session, eRebldType, cbOrcaErrProc, (void*)1 );
	if(orcaError(ret))return false;
	return true;
}

void __stdcall cbOrcaLinkProc(PPBORCA_LINKERR err,LPVOID uData){
	_tprintf(TEXT("%s\n"),err->lpszMessageText);
}

//expect a|b|c as input string
//split it to separate strings
long _getExeFlags(TCHAR*parm){
	TCHAR *pos0=parm;
	TCHAR *pos1=_tcschr(pos0,'|');
	long flags=0;
	
	while(pos0){
		if(pos1){pos1[0]=0;pos1++;}
		pos0=trim(pos0);
		
		if(!_tcscmp(pos0,TEXT("pcode"))){
		}else if(!_tcscmp(pos0,TEXT("machinecode"))){
			flags|=PBORCA_MACHINE_DEFAULT;
		}else if(!_tcscmp(pos0,TEXT("stylexp"))){
			flags|=PBORCA_NEW_VISUAL_STYLE_CONTROLS;
		}else{
			printline();
			_tprintf(TEXT("wrong exe type: \"%s\"\n"),pos0);
			return -1;
		}
		pos0=pos1;
		if(pos1)pos1=_tcschr(pos1,'|');
	}
	
	return flags;
}

BOOL orcaBuildExecutable(TCHAR**parm){
	int ret=0;
	int eFlag;
	
	TCHAR*exeName=parm[0];
	TCHAR*iconName=parm[1];
	TCHAR*pbrName=parm[2];
	TCHAR*type=parm[3];
	
	log(TEXT("build executable"));
	if(type==NULL)type=TEXT("<NULL>");
	
	eFlag=_getExeFlags(type);
	if(eFlag==-1)return false;
	
	if(!liblist){
		printline();
		_tprintf(TEXT("no library list\n"));
		return false;
	}
	
	if( !checkPBR(pbrName) )return false;
	
	if( iconName[0] && _taccess(iconName,0) ){
		printline();
		_tprintf(TEXT("can't find ICON file \"%s\"."),iconName);
		return false;
	}
	
	if(exeinfo){
		ret=XORCA_SetExeInfo(session,exeinfo);
		if(orcaError(ret))return false;
	}
	
	int *pbd=new int[libcount];
	LIBLIST *ill=liblist;
	for(int i=0;i<libcount;i++){
		if(ill->pbd){
			pbd[i]=1;//pbd/dll from pbl
			if( !_tcsicmp(__T(".pbl"), ill->lib+(_tcslen(ill->lib)-4) ) ) {
				if(!checkPBR(ill->pbr))goto error;
_tprintf(TEXT("eFlag=%d %d\n"),eFlag, eFlag&PBORCA_MACHINE_DEFAULT );
				ret=XORCA_DynamicLibraryCreate (session,ill->lib,ill->pbr,  (eFlag&PBORCA_MACHINE_DEFAULT?eFlag:0) ); //for  pcode eFlag could be only 0
				if(orcaError(ret))goto error;
			}
		}else{
			pbd[i]=0;//include pbl into exe
		}
		ill=(LIBLIST*)ill->next;
	}
	_tprintf(TEXT("pbr file \"%s\".\n"),pbrName);
	ret=XORCA_ExecutableCreate ( session, exeName, iconName, pbrName,
		cbOrcaLinkProc, NULL, pbd, libcount, eFlag );
	if(orcaError(ret))goto error;
	delete []pbd;
	return true;
	error:
	delete []pbd;
	return false;
}
BOOL orcaBuildLibrary(TCHAR**parm){
	TCHAR*lib=parm[0];
	TCHAR*pbr=parm[1];
	TCHAR*type=parm[2];
	int eFlag,ret;
	if(!_tcscmp(type,TEXT("pbd")))eFlag=0;
	else if(!_tcscmp(type,TEXT("dll")))eFlag=PBORCA_MACHINE_DEFAULT;
	else{
		printline();
		_tprintf(TEXT("wrong exe type: \"%s\"\n"),type);
		return false;
	}
	if(!checkPBR(pbr))return false;
	ret=XORCA_DynamicLibraryCreate (session,lib,pbr,eFlag );	
	if(orcaError(ret))return false;
	return true;
}

BOOL orcaTimestamp(TCHAR**parm){//no parameters
	GetLocalTime(&timestamp);
	_tprintf(TEXT("%04i.%02i.%02i %02i:%02i:%02i.%03i\n"),
		timestamp.wYear,
		timestamp.wMonth,
		timestamp.wDay,
		timestamp.wHour,
		timestamp.wMinute,
		timestamp.wSecond,
		timestamp.wMilliseconds
		);
	return true;
}

BOOL orcaDeleteItem(TCHAR**parm){
	TCHAR*pbl=parm[0];
	TCHAR*item=parm[1];
	TCHAR*type=parm[2];
	
	PBORCA_TYPE t=ctype2orca(type);
	if(t==-1)return false;
	int ret=XORCA_LibraryEntryDelete ( session,pbl,item,t);
	if(orcaError(ret))return false;
	return true;
}

BOOL orcaEcho(TCHAR**parm){
	_tprintf(TEXT("%s\n"),parm[0]);
	return true;
}

void __stdcall cbOrcaDeleteDuplex(PPBORCA_DIRENTRY dir, void*uData){
	int ret=XORCA_LibraryEntryDelete ( session,(TCHAR*)uData,dir->lpszEntryName,dir->otEntryType);
	orcaError(ret);
}

BOOL orcaDeleteDuplex(TCHAR**parm){
	TCHAR*primLib=parm[0];
	TCHAR*delLib=parm[1];
	
	TCHAR cmt[PBORCA_MAXCOMMENT+1];
	int ret=XORCA_LibraryDirectory(session,primLib,cmt,PBORCA_MAXCOMMENT+1,
				cbOrcaDeleteDuplex,(void*)delLib);
	if(orcaError(ret))return false;
	return true;
}


BOOL orcaRegenerate(TCHAR**parm){
	//library, entry, type
	TCHAR*lib= parm[0];
	TCHAR*item=parm[1];
	TCHAR*type=parm[2];
	int ret;

	if(!lib[0]){  //libname omitted
		LIBLIST *ill=liblist;
		BOOL bret=true;
		while(ill && bret){
			parm[0]=ill->lib;
			bret=orcaRegenerate(parm);
			parm[0]=lib;
			ill=(LIBLIST*)ill->next;
		}
		return bret;
	}else if( !item[0] || !type[0] ){
		return _itemIterator(orcaRegenerate,parm,0,1,2);
	}else{
		PBORCA_TYPE t=ctype2orca(type);
		if(t==-1)return false;
		ret=XORCA_CompileEntryRegenerate ( session, lib, item, t, cbOrcaErrProc, (void*)1);
		if(orcaError(ret))return false;
	}
	return true;
}

BOOL _orcaExport(TCHAR*lib, TCHAR*item, TCHAR*type, TCHAR*dir){
	PBORCA_TYPE t=ctype2orca(type);
	if(t==-1)return false;
	int ret;
	int dirlen;
	FILE *f=NULL;
	TCHAR *fn=NULL;
	BOOL has_bin=false;
	BOOL bret=true;
	PBORCA_ENTRYINFO ei;
	PBORCA_ENTRYINFO eib; //bin info for binary object extension
	//get the text of the object
	memset(&ei,0x00, sizeof(ei));
	memset(&eib,0x00, sizeof(ei));
	
	ret=XORCA_LibraryEntryInformation ( session, lib, item, t,&ei);
	if(orcaError(ret))return false;
	
	if(t!=PBORCA_BINARY){
		ret=XORCA_LibraryEntryInformation ( session, lib, item, PBORCA_BINARY,&eib);
		if(ret!=PBORCA_OBJNOTFOUND && ret!=PBORCA_OK) return false;
		if(ret==PBORCA_OK) has_bin=true;
	}
	
	TCHAR*buf=new TCHAR[ei.lSourceSize+eib.lSourceSize+1000]; //1000 just in case
	ret=XORCA_LibraryEntryExport ( session,lib,item,t,buf,ei.lSourceSize+500);
	if(orcaError(ret))goto error;
	
	if(has_bin){
		int offset=_tcslen(buf);
		ret=XORCA_LibraryEntryExport ( session,lib,item,PBORCA_BINARY,buf+offset,eib.lSourceSize+500);
		if(orcaError(ret))goto error;
	}
	
	
	dirlen=_tcslen(dir);
	fn=new TCHAR[dirlen+1+_tcslen(item)+4+1];
	if(dirlen && (dir[dirlen-1]=='\\' || dir[dirlen-1]=='/') )dir[dirlen-1]=0;
	if(t==PBORCA_BINARY) _stprintf(fn,TEXT("%s%s%s.bin"),dir,((dirlen && dir[dirlen-1]!='\\' && dir[dirlen-1]!='/' )?"\\":""),item);
	else _stprintf(fn,TEXT("%s%s%s.sr%c"),dir,((dirlen && dir[dirlen-1]!='\\' && dir[dirlen-1]!='/' )?"\\":""),item,objext[t]);
	f=_tfopen(fn,TEXT("wb"));
	if(f){
		if(t!=PBORCA_BINARY){
			_orcaWrite(f,item,t,ei.szComments,buf,IMPEXP_HASCII);
		}else{
			fwrite( buf,1,ei.lSourceSize,f);
		}
		fflush(f);
		fclose(f);
	}else{
		printline();
		_tprintf(TEXT("can't open file \"%s\"\n"),fn);
		goto error;
	}
	
	
	goto end;
	error:
	bret=false;
	end:
	delete [] buf;
	if(fn)delete [] fn;
	return bret;
}



BOOL fiOrcaExport(TCHAR**parm){ 
	//library, entry, type, folder
	TCHAR*lib =parm[0];
	TCHAR*item=parm[1];
	TCHAR*type=parm[2];
	TCHAR*dir =parm[3];
	if(!item[0] || !type[0]){
		return _itemIterator(fiOrcaExport,parm, 0, 1, 2);
	}else{
		return _orcaExport(lib, item, type, dir);
	}
	return true;
}

BOOL orcaExport(TCHAR**parm){ 
	//library, entry, type, folder
	return _fileIterator(fiOrcaExport,parm,0);
}

#define ORCA_IMP_ERR_SHOW 0x01
#define ORCA_IMP_DEL_NCMP 0x02
#define ORCA_IMP_IGN_CMPL 0x04
#define ORCA_IMP_IGN_FILE 0x08



BOOL _OrcaImport( TCHAR * fname,TCHAR * lib, int flags ){
	//srcFile, dstLib
	TCHAR * buf=NULL;
	ORCAIMPORTREAD ir;
	BOOL bret=false;
	TCHAR *fpLib=NULL;//full path library name
	DWORD  fpLen;
	TCHAR *fnLib=NULL;
	
	//checks of file presence
	if( _taccess(lib,0) ){
		if( ( (flags&ORCA_IMP_IGN_FILE)&&(flags&ORCA_IMP_ERR_SHOW) ) || !(flags&ORCA_IMP_IGN_FILE) ){
			printline();
			_tprintf(TEXT("can't find library \"%s\".\n"),lib);
		}
		return flags&ORCA_IMP_IGN_FILE;
	}
	if( _taccess(fname,0) ){
		if( ( (flags&ORCA_IMP_IGN_FILE)&&(flags&ORCA_IMP_ERR_SHOW) ) || !(flags&ORCA_IMP_IGN_FILE) ){
			printline();
			_tprintf(TEXT("can't find file \"%s\".\n"),fname);
		}
		return flags&ORCA_IMP_IGN_FILE;
	}
	
	
	fpLen=GetFullPathName(lib,0,fpLib,&fnLib);
	if(fpLen==0){
		_tprintf(TEXT("GetFullPathName failed for \"%s\"\n"),lib);
		return false;
	}
	fpLib=new TCHAR[fpLen+1];
	GetFullPathName(lib,fpLen+1,fpLib,&fnLib);
	if( _orcaRead(fname,&ir) ){
		int ret= XORCA_CompileEntryImport ( session,
				fpLib,
				ir.item,
				ir.t,ir.comm,
				ir.ptr,
				_tcslen(ir.ptr)*sizeof(TCHAR), //stupid but it's number of bytes (not chars)
				cbOrcaErrProc,
				(void*)(flags&ORCA_IMP_ERR_SHOW) );

		if(ret==0)bret=true;
		else if(ret==PBORCA_COMPERROR){
			if(flags&ORCA_IMP_IGN_CMPL)bret=true; //ignore compile import error
			if(flags&ORCA_IMP_ERR_SHOW)orcaError(ret);
		}else{
			//show the errors other then compile
			orcaError(ret);
		}
		
		if( (flags&ORCA_IMP_DEL_NCMP) && ret!=0){
			XORCA_LibraryEntryDelete ( session,lib,ir.item,ir.t);
		} else if(ir.bin && ir.bin[0]){
			//import binary data
			bret=false;
			ret= XORCA_CompileEntryImport ( session,
					fpLib,
					ir.item,
					PBORCA_BINARY,NULL,
					ir.bin,
					_tcslen(ir.bin)*sizeof(TCHAR), //stupid but it's number of bytes (not chars)
					cbOrcaErrProc,
					(void*)(flags&ORCA_IMP_ERR_SHOW) );
			
			if(ret==0)bret=true;
			else if(ret==PBORCA_COMPERROR){
				if(flags&ORCA_IMP_IGN_CMPL)bret=true; //ignore compile import error
				if(flags&ORCA_IMP_ERR_SHOW)orcaError(ret);
			}else{
				//show the errors other then compile
				orcaError(ret);
			}
			
			if( (flags&ORCA_IMP_DEL_NCMP) && ret!=0){
				XORCA_LibraryEntryDelete ( session,lib,ir.item,PBORCA_BINARY);
				XORCA_LibraryEntryDelete ( session,lib,ir.item,ir.t);
			}
		}
		
	}
	if(ir.buf)delete []ir.buf;
	delete []fpLib;
	return bret;
	
}


//imports only non binary items
BOOL fiOrcaImport(TCHAR**parm){
	//srcFile, dstLib
	TCHAR * lib=parm[1];
	TCHAR * fname=parm[0];
	
	return _OrcaImport(fname,lib,ORCA_IMP_IGN_CMPL|ORCA_IMP_ERR_SHOW);
}

BOOL orcaImport(TCHAR**parm){ 
	//srcFile, library
	return _fileIterator(fiOrcaImport,parm,0);
}


BOOL fiOrcaImportMove(TCHAR**parm){
	//srcFile, dstLib
	TCHAR * lib=parm[1];
	TCHAR * fname=parm[0];
	
	if( _OrcaImport(fname,lib,ORCA_IMP_IGN_CMPL|ORCA_IMP_ERR_SHOW) ){
		DeleteFile(fname);
		return true;
	}
	return false;
}


BOOL orcaImportMove(TCHAR**parm){ 
	//srcFile, library
	return _fileIterator(fiOrcaImportMove,parm,0);
}


BOOL fiOrcaImportMoveOk(TCHAR**parm){
	//srcFile, dstLib
	TCHAR * lib=parm[1];
	TCHAR * fname=parm[0];
	
	if( _OrcaImport(fname,lib,ORCA_IMP_DEL_NCMP) ){
		_tprintf(TEXT("\t%s\n"),fname);
		DeleteFile(fname);
		return true;
	}
	return true;
}


BOOL orcaImportMoveOk(TCHAR**parm){ 
	//srcFile, library
	return _fileIterator(fiOrcaImportMoveOk,parm,0);
}



BOOL orcaCD(TCHAR**parm){
	if(SetCurrentDirectory(parm[0]))return true;
	printsyserr();
	return false;
}


typedef struct {
	TCHAR *lib;
	TCHAR *item;
	PBORCA_TYPE  type;
	void *next;
} REGENITEM;


REGENITEM * regenAllRoot;
BOOL regenAllErr;

void __stdcall _cbOrcaRegenerateAllCompile(PPBORCA_COMPERR err, void*uData){
	//do nothing
	//_tprintf(TEXT("%s\n"),err->lpszMessageText);
}

void __stdcall _cbOrcaRegenerateAll(PPBORCA_DIRENTRY dir, void*lib){
	//don't continue on any error
	if(regenAllErr)return;
	int ret=XORCA_CompileEntryRegenerate ( session, (TCHAR*)lib, dir->lpszEntryName, dir->otEntryType, _cbOrcaRegenerateAllCompile, NULL);
	switch(ret){
		case PBORCA_COMPERROR:{
			//on compile error remember the item in order to regenerate it later
			REGENITEM * rnew=new REGENITEM;
			rnew->lib=(TCHAR*)lib;
			rnew->item=new TCHAR[_tcslen(dir->lpszEntryName)+1];
			_tcscpy(rnew->item,dir->lpszEntryName);
			rnew->type=dir->otEntryType;
			rnew->next=regenAllRoot;
			regenAllRoot=rnew;
			break;
		}
		case PBORCA_OK:{
			//do nothing
			_tprintf(TEXT("\t%s.sr%c\n"),dir->lpszEntryName,objext[dir->otEntryType]);
			break;
		}
		default :{
			//on all other errors return false!!!
			regenAllErr=true;
			orcaError(ret);
			break;
		}
	}
}

//no parameters
BOOL orcaRegenerateAll(TCHAR**parm){
	
	int ret;
	int tcount=1;
	BOOL bret=true;
	REGENITEM * ri;
	
	//let's create a list of all items
	TCHAR cmt[PBORCA_MAXCOMMENT+1];

	LIBLIST *ill=liblist;
	regenAllErr=false;
	regenAllRoot=NULL;
	while(ill){
		ret=XORCA_LibraryDirectory(session,ill->lib,cmt,PBORCA_MAXCOMMENT+1,_cbOrcaRegenerateAll,(void*)ill->lib);
		if(orcaError(ret))goto error;
		if(regenAllErr)goto error;
		ill=(LIBLIST*)ill->next;
	}
	
	ri=regenAllRoot;
	int count;
	do{
		_tprintf(TEXT("turn %i\n"),tcount);
		tcount++;
		count=0;
		while(ri){
			if(ri->lib){
				ret=XORCA_CompileEntryRegenerate ( session, ri->lib, ri->item, ri->type, _cbOrcaRegenerateAllCompile, NULL);
				switch(ret){
					case PBORCA_COMPERROR:
						//do nothing
						break;
					case PBORCA_OK:{
						count++;
						_tprintf(TEXT("\t%s.sr%c\n"),ri->item,objext[ri->type]);
						//this will mark entry as OK.
						ri->lib=NULL;
						break;
					}
					default :{
						//on all other errors return false!!!
						regenAllErr=true;
						orcaError(ret);
						break;
					}
				}
			}
			ri=(REGENITEM*)ri->next;
		}
		_tprintf(TEXT("turn count=%i\n"),count);
	}while(count>0);
	
	
	goto end;
	error:
	bret=false;
	end:
	//free the resources
	ri=regenAllRoot;
	while(regenAllRoot){
		ri=regenAllRoot;
		regenAllRoot=(REGENITEM*)regenAllRoot->next;
		delete [](ri->item);
		delete ri;
	}
	
	
	return bret;
}


BOOL _cbFiOrcaImportPbg2Pbl( TCHAR*file, TCHAR*pbl, void*udata ){
	return _OrcaImport(file,pbl, (int)udata);
}

BOOL fiOrcaImportPbg2Pbl(TCHAR**parm){
	//pbg file(s), dst folder
	TCHAR * pbgpath=parm[0];
	TCHAR * folder=parm[1];

	BOOL ret=_readPbgFile( pbgpath, folder, _cbFiOrcaImportPbg2Pbl, (void*)ORCA_IMP_IGN_CMPL );
	return ret;
}

BOOL orcaImportPbg2Pbl(TCHAR**parm){
	//pbg file(s), dst folder
	return _fileIterator(fiOrcaImportPbg2Pbl,parm,0);
}

BOOL fiOrcaImportPbg2PblAny(TCHAR**parm){
	//pbg file(s), dst folder
	TCHAR * pbgpath=parm[0];
	TCHAR * folder=parm[1];

	BOOL ret=_readPbgFile( pbgpath, folder, _cbFiOrcaImportPbg2Pbl, (void*)(ORCA_IMP_IGN_CMPL|ORCA_IMP_IGN_FILE) );
	return ret;
}

BOOL orcaImportPbg2PblAny(TCHAR**parm){
	//pbg file(s), dst folder
	return _fileIterator(fiOrcaImportPbg2Pbl,parm,0);
}

BOOL cbOrcaTargetCreateLib(int type, TCHAR*param, TCHAR*pbtDir, void*udata){
	if(type==TARGET_INFO_LIB){
		TCHAR * pbl=new TCHAR[_tcslen(pbtDir)+_tcslen(param)+5];
		int ret=0;
		_stprintf(pbl,TEXT("%s\\%s"),pbtDir,param);
		if( _taccess(pbl,0) ){
			ret=XORCA_LibraryCreate ( session, pbl, TEXT("") );
		}
		delete []pbl;
		if(orcaError(ret))return false;
	}
	return true;
}

BOOL orcaTargetCreateLib(TCHAR**parm){
	TCHAR * pbtfile=parm[0];
	return _readTargetFile(pbtfile, cbOrcaTargetCreateLib, NULL);	
}

typedef struct {
    TCHAR*      srcdir;
    int         flags;
}ORCA_TARGET_IMP_PARAM;

BOOL _cbOrcaTargetImport(int type, TCHAR*param, TCHAR*pbtDir, void*udata){
	if(type==TARGET_INFO_LIB){
		
		if( !_tcsicmp(__T(".pbl"), param+(_tcslen(param)-4) ) ) {  //library must be pbl (end with 'l')
			BOOL ret;
			int i;
			ORCA_TARGET_IMP_PARAM * impparam=(ORCA_TARGET_IMP_PARAM*)udata;
			
			TCHAR * pbg=new TCHAR[_tcslen(impparam->srcdir)+_tcslen(param)+5];
			TCHAR * dst=new TCHAR[_tcslen(pbtDir)+_tcslen(param)+5];
			TCHAR *Pbg2Pbl[]={pbg,dst};
			
			_stprintf(pbg,TEXT("%s\\%s"),impparam->srcdir,param);
			_stprintf(dst,TEXT("%s\\%s"),pbtDir,param);
			
			pbg[_tcslen(pbg)-1]='g';
			for(i=_tcslen(dst)-1;i>=0;i--)if(dst[i]=='\\' || dst[i]=='/'){dst[i]=0;break;} //keep only directory name
			
			ret=_readPbgFile( pbg, dst, _cbFiOrcaImportPbg2Pbl, (void*)impparam->flags );
			
			delete []pbg;
			delete []dst;
			if(!ret)return false;
		}
	}
	return true;
}



BOOL orcaTargetImport(TCHAR**parm){
	ORCA_TARGET_IMP_PARAM impparm;
	TCHAR * pbtfile=parm[0];
	impparm.srcdir=parm[1];
	impparm.flags=ORCA_IMP_IGN_CMPL;
	
	unify_dirname(impparm.srcdir);
	if(!_readTargetFile(pbtfile, _cbOrcaTargetImport, (void*)&impparm))return false;
	
	return true;
}


BOOL orcaTargetImportAny(TCHAR**parm){
	ORCA_TARGET_IMP_PARAM impparm;
	TCHAR * pbtfile=parm[0];
	impparm.srcdir=parm[1];
	impparm.flags=ORCA_IMP_IGN_CMPL|ORCA_IMP_IGN_FILE;
	
	unify_dirname(impparm.srcdir);
	if(!_readTargetFile(pbtfile, _cbOrcaTargetImport, (void*)&impparm))return false;
	
	return true;
}


BOOL _cbOrcaTargetSetLibList(int type, TCHAR*param, TCHAR*pbtDir, void*udata){
	if(type==TARGET_INFO_LIB){
		TCHAR * pbl=new TCHAR[_tcslen(pbtDir)+_tcslen(param)+5];
		BOOL ret;
		
		_stprintf(pbl,TEXT("%s\\%s"),pbtDir,param);
		ret=orcaSetLiblistAdd(pbl);
		delete []pbl;
		if(!ret)return false;
	}
	return true;
}

BOOL orcaTargetSetLiblist(TCHAR**parm){
	TCHAR * pbtfile=parm[0];
	if(!orcaSetLiblistStart(NULL))return false;
	if(!_readTargetFile(pbtfile, _cbOrcaTargetSetLibList, NULL))return false;
	if(!orcaSetLiblistEnd(NULL))return false;
	return true;
}


BOOL _cbOrcaTargetSetApp(int type, TCHAR*param, TCHAR*pbtDir, void*udata){
	TCHAR ** udata_parm = (TCHAR ** )udata;
	if(type==TARGET_INFO_APP){
		if(_tcslen(param)>MAX_PATH){
			printline();
			_tprintf(TEXT("application name too long \"%s\".\n"),param);
			return false;
		}
		_tcscpy(udata_parm[1],param);
	}
	if(type==TARGET_INFO_APP_LIB){
		if(_tcslen(pbtDir)+_tcslen(param)+3>4096){
			printline();
			_tprintf(TEXT("library path too long \"%s\\%s\".\n"),pbtDir,param);
			return false;
		}
		_stprintf(udata_parm[0],TEXT("%s\\%s"),pbtDir,param);
	}
	return true;
}

BOOL orcaTargetSetApp(TCHAR**parm){
	TCHAR * pbtfile=parm[0];
	TCHAR app[MAX_PATH+1];
	TCHAR lib[4097];
	TCHAR * udata[]={lib,app,NULL};
	
	if(!_readTargetFile(pbtfile, _cbOrcaTargetSetApp, (void*)udata))return false;
	return orcaSetApplication(udata);
}

BOOL orcaSetExeInfo(TCHAR**parm){
	TCHAR * propname=parm[0];
	TCHAR * propvalue=parm[1];
	return exeInfoSetProperty(propname,propvalue);
}

BOOL orcaProfileString(TCHAR**parm){
	//file, section, key, varname
	TCHAR * file=parm[0];
	TCHAR * section=parm[1];
	TCHAR * key=parm[2];
	
	TCHAR value[1025]=__T("");
	
	GetPrivateProfileString(section,key,__T(""),value,1024, file );
	//_tprintf(TEXT("%s[%s].%s=%s\n"),file,section,key,value);
	
	if(SetEnvironmentVariable(key,value))return true;
	
	printline();
	_tprintf(TEXT("can't set environment variable \"%s\".\n"),key);
	return false;
}

BOOL orcaProfileExeInfo(TCHAR**parm){
	TCHAR * file=parm[0];
	TCHAR * section=parm[1];
	
	TCHAR value[1025]=__T("");
	
	for(int i=0; i<EXEINFO_PROPCOUNT; i++){
		GetPrivateProfileString(section,exeinfo_props[i],__T(""),value,1024, file );
		if(!exeInfoSetProperty(exeinfo_props[i],value))return false;
	}
	return true;
}


