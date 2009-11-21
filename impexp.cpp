#include <tchar.h>

#include <windows.h>
#include <stdio.h>
#include "pborca.h"
#include "orca.h"
#include "impexp.h"


extern TCHAR*objext;

/************************************EXPORT************************************/

int _orcaPutsHACount(TCHAR*c){
	int cnt=0;
	for(int i=0;c[i]>127U;i++){cnt++;}
	return cnt;
}


void _orcaPutCommentChar(TCHAR c,FILE*f){
	switch(c){
		case '\n':fputs("~n",f);break;
		case '\r':fputs("~r",f);break;
		case '\t':fputs("~t",f);break;
		default  :fputc(c,f);
	}
}

void _orcaPutsHA(FILE*f,TCHAR*c,BOOL bComment){
	int cnt;
	for(int i=0;c[i];i++){
		cnt=_orcaPutsHACount(c+i);
		if(cnt){
			fprintf(f,"$$HEX%i$$",cnt);
			//printf("$$HEX%i$$\n",cnt);
			for(int k=0;k<cnt;k++)fprintf(f,"%02x%02x",c[i+k]&0x00FF,(c[i+k]>>8)&0x00FF);
			fprintf(f,"$$ENDHEX$$");
			i+=cnt-1;
		}else{
			if(bComment)_orcaPutCommentChar(c[i],f);
			else fputc(c[i],f);
		}
	}
	
}
void _orcaWriteHA(FILE*f,TCHAR*item,PBORCA_TYPE t,TCHAR*comment,TCHAR*buf){
	fprintf(f,"HA$PBExportHeader$%S.sr%c\r\n",item,objext[t]);
	if(comment && comment[0]){
		fputs("$PBExportComments$",f);
		_orcaPutsHA(f,comment,true);
		fputs("\r\n",f);
	}
	_orcaPutsHA(f,buf,false);
}

void _orcaWriteASCII(FILE*f,TCHAR*item,PBORCA_TYPE t,TCHAR*comment,TCHAR*buf){
	fprintf(f,"$PBExportHeader$%s.sr%c\r\n",item,objext[t]);
	if(comment && comment[0]){
		fputs("$PBExportComments$",f);
		for(int i=0;comment[i];i++)_orcaPutCommentChar(comment[i],f);
		fputs("\r\n",f);
	}
	_fputts(buf,f);
}

BOOL _orcaWrite(FILE*f,TCHAR*item,PBORCA_TYPE t,TCHAR*comment,TCHAR*buf,int expType){
#ifdef _UNICODE
	_orcaWriteHA(f,item,t,comment,buf);
#else
	_orcaWriteASCII(f,item,t,comment,buf);
#endif
	return true;
}
/************************************IMPORT************************************/


int _orcaReadType(FILE*f,unsigned int * flen){
	int c1,c2;
	int ret=IMPEXP_HASCII;
	
	c1=fgetc(f);
	c2=fgetc(f);
	if( (c1==0x00FF) && (c2==0x00FE) && ((flen[0]&1)==0) ){
		ret=IMPEXP_UNI;
		flen[0]-=2;
	}else if (c1=='H' && c2=='A'){
		ret=IMPEXP_HASCII;
		flen[0]-=2;
	}else {
		//reset to beginning import as HASCII - almost the same except those stupid macros $$HEX$$
		fseek( f, 0, SEEK_SET); 
	}
	return ret;
}

TCHAR*DELIMHEXASCII	=_T("$$");
TCHAR*OPENHEXASCII	=_T("$$HEX");
TCHAR*CLOSEHEXASCII	=_T("$$ENDHEX$$");

BOOL HADecode(TCHAR * buf){
	TCHAR *p=buf;
	TCHAR *p2;
	int ch,b8;
	int b8shift[]={4,0,12,8};
	int count,i;
	while ((p = _tcsstr(p, OPENHEXASCII)) != NULL) {
		p2 = _tcsstr(p+_tcslen(OPENHEXASCII), DELIMHEXASCII);
		if(!p2)return false;
		p2[0]=0;
		count=_ttol(p+_tcslen(OPENHEXASCII));
		if(count==0)return false;
		p2[0]=DELIMHEXASCII[0];
		p2+=_tcslen(DELIMHEXASCII);
		while(count){
			ch=0;
			for(i=0;i<4;i++){
				switch(p2[0]){
					case '0':b8=0;break;
					case '1':b8=1;break;
					case '2':b8=2;break;
					case '3':b8=3;break;
					case '4':b8=4;break;
					case '5':b8=5;break;
					case '6':b8=6;break;
					case '7':b8=7;break;
					case '8':b8=8;break;
					case '9':b8=9;break;
					case 'a':
					case 'A':b8=10;break;
					case 'b':
					case 'B':b8=11;break;
					case 'c':
					case 'C':b8=12;break;
					case 'd':
					case 'D':b8=13;break;
					case 'e':
					case 'E':b8=14;break;
					case 'f':
					case 'F':b8=15;break;
					default: return false;
				}
				ch=ch | (b8<<b8shift[i]);
				p2++;
			}
			p[0]=ch;
			p++;
			count--;
		}
		if(_tcsncmp(CLOSEHEXASCII,p2,_tcslen(CLOSEHEXASCII)))return false;
		_tcscpy(p,p2+_tcslen(CLOSEHEXASCII));
	}
	
	return true;
}


BOOL _orcaRead(TCHAR*fname,ORCAIMPORTREAD*ir){
	BOOL bret=false;
	int ietype;
	unsigned int flen;
	unsigned int i=0;
	int c;
	
	ir->t=(PBORCA_TYPE)-1;
	FILE * f=_tfopen(fname,TEXT("rb"));
	if(f){
		fseek( f, 0, SEEK_END);
		flen=ftell( f);
		fseek( f, 0, SEEK_SET);
		ietype=_orcaReadType(f,&flen);
		
		i=0;
		if(ietype==IMPEXP_UNI){
			flen=flen/2;
			ir->buf=new TCHAR[flen+1];
			while( i<flen && (c=fgetwc(f)) !=-1 )ir->buf[i++]=c;
			ir->buf[flen]=0;
		}else{
			ir->buf=new TCHAR[flen+1];
			while( i<flen && (c=fgetc(f)) !=-1 )ir->buf[i++]=c;
			ir->buf[flen]=0;
			if(!HADecode(ir->buf))return false;
		}
		if(i==flen){
			if(!_tcsnccmp(TEXT("$PBExportHeader$"),ir->buf,16)){
				//get item name/type
				ir->ptr=ir->buf+16;
				i=0;
				while(ir->ptr[i] && i<ORCAIMPORT_ITEM_LEN){
					if(ir->ptr[i]=='.'){
						ir->item[i]=0;
						break;
					}
					ir->item[i]=ir->ptr[i];
					i++;
				}
				if(i>=ORCAIMPORT_ITEM_LEN)goto wrongheader;
				ir->ptr+=i;
				if(_tcsnccmp(TEXT(".sr"),ir->ptr,3))goto wrongheader;
				ir->ptr+=3;
				for(i=0;objext[i];i++)if(objext[i]==ir->ptr[0])ir->t=(PBORCA_TYPE)i;
				if(ir->t==-1)goto wrongheader;
				ir->ptr++;
				while(ir->ptr[0]==' '||ir->ptr[0]=='\t')ir->ptr++;
				if(ir->ptr[0]!='\r')goto wrongheader;
				ir->ptr++;
				if(ir->ptr[0]!='\n')goto wrongheader;
				ir->ptr++;
				
				if(!_tcsnccmp(TEXT("$PBExportComments$"),ir->ptr,18)){
					ir->ptr+=18;
					i=0;
					while(ir->ptr[i] && i<_tsizeof(ir->comm) ){
						if(ir->ptr[i]=='\n' || ir->ptr[i]=='\r'){
							ir->comm[i]=0;
							break;
						}
						ir->comm[i]=ir->ptr[i];
						i++;
					}
					ir->ptr+=i;
					if(ir->ptr[0]!='\r')goto wrongheader;
					ir->ptr++;
					if(ir->ptr[0]!='\n')goto wrongheader;
					ir->ptr++;
				}else ir->comm[0]=0;
				//try to find binary section:
				ir->bin=_tcsstr(ir->ptr,TEXT("Start of PowerBuilder Binary Data Section"));
				if(ir->bin){
					ir->bin[-1]=0;
				}
				
				bret=true;//ignore compile import error
			}else{
				wrongheader:
				printline();
				_tprintf(TEXT("wrong pb export header in \"%s\"\n"),fname);
			}
		}else{
			printline();
			_tprintf(TEXT("error reading file \"%s\".\n"),fname);
		}
		fclose(f);
	}else{
		printline();
		_tprintf(TEXT("can't open file \"%s\"\n"),fname);
	}
	
	return bret;
	
}

