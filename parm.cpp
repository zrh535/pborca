#include <tchar.h>

#include <windows.h>
#include <stdio.h>



TCHAR*_getParmPtr(TCHAR*parms,int no){
	TCHAR*ptr=parms;
	int curNo=1;
	
	while(no>curNo){
		while(ptr[0]&&ptr[0]!=','){
			if(ptr[0]=='%')ptr++;
			if(ptr[0])ptr++;
		}
		if(ptr[0]==','){
			ptr++;
			curNo++;
		}else{
			break;
		}
	}
	if(curNo==no)return ptr;
	SetLastError(ERROR_INVALID_PARAMETER);
	return NULL;
}


/*TCHAR*_getParmPtr(TCHAR*parms,int no){
	TCHAR*ptr=parms;
	int curNo=0;
	
	while(ptr[0]&&no>0){
		if(++curNo==no)return ptr;
		//search for the end
		while(ptr[0]&&ptr[0]!=','){
			if(ptr[0]=='%')ptr++;
			if(ptr[0])ptr++;
		}
		if(ptr[0]==',')ptr++;
	}
	SetLastError(ERROR_INVALID_PARAMETER);
	return NULL;
}
*/
long _getParmSize(TCHAR*parm){
	TCHAR*ptr=parm;
	while(ptr[0]&&ptr[0]!=','){
		if(ptr[0]=='%')ptr++;
		if(ptr[0])ptr++;
	}
	return ptr-parm;
}

TCHAR*_parmExpandEnv(TCHAR*param){
	DWORD i=ExpandEnvironmentStrings(param,NULL,0)+2;
	TCHAR*tmp=new TCHAR[i];
	ExpandEnvironmentStrings(param,tmp,i);
	delete []param;
	return tmp;
}


TCHAR*_unescape(TCHAR*parm,BOOL trim){
	TCHAR*src=parm;
	TCHAR*dst=parm;
	TCHAR*rsp=NULL;//right spaces
	TCHAR c;
	if(trim){
		while(src[0]&&(src[0]==' '||src[0]=='\t'))src++;
	}
	while(src[0]){
		if(src[0]=='%'){
			src++;
			switch(src[0]){
				case 'n':{c='\n';break;}
				case 'r':{c='\r';break;}
				case 't':{c='\t';break;}
				case ',':{c=',';break;}
				case '%':{c='%';break;}
				default:{
					if(src[0])src++;
					continue;
				}
			}
		}else{
			c=src[0];
			if(c==' '||c=='\t'){
				if(!rsp)rsp=dst;
			}else{
				rsp=NULL;
			}
		}
		dst[0]=c;
		src++;
		dst++;
	}
	dst[0]=0;
	if(rsp&&trim)rsp[0]=0;
	return parm;
}

TCHAR*getParm(TCHAR*parms,int no,BOOL trim=true){
	TCHAR*src=_getParmPtr(parms,no);
	if(src){
		long l=_getParmSize(src);
		TCHAR*dst;
		dst=new TCHAR[l+1];
		if(dst){
			_tcsncpy(dst,src,l);
			dst[l]=0;
			return _parmExpandEnv(_unescape(dst,trim));
		}
	}
	return NULL;
}

int	getParmCount(TCHAR*parms){
	TCHAR*ptr=parms;
	int curNo=1;
	
	while(ptr[0]&&(ptr[0]==' '||ptr[0]=='\t'))ptr++;
	if(!ptr[0])return 0;
	while(ptr[0]){
		if(ptr[0]==',')curNo++;
		if(ptr[0]=='%'&&ptr[1])ptr++;
		ptr++;
	}
	return curNo;
}

//returns reference to parameters or NULL if cmd is not a cmdname
TCHAR*iscmd(TCHAR*cmd,TCHAR*cmdname){
	int len=_tcslen(cmdname);
	if( _tcsnccmp(cmd,cmdname,len) )return NULL;
	if(cmd[len]!=' '&&cmd[len]!='\t'&&cmd[len]!=0)return NULL;
	TCHAR*parm=cmd+len;
	//skip spaces
	while(parm[0]&&strchr(" \t",parm[0]))parm++;
	return parm;
}
