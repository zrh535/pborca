


typedef BOOL (*ORCA_PBG_INFO) ( TCHAR*file, TCHAR*pbl, void*udata );

BOOL _readPbgFile( TCHAR * pbgpath, TCHAR * folder, ORCA_PBG_INFO cb, void*udata );

/*
BOOL fiOrcaImportPbg2Pbl(TCHAR**parm){
	//pbg file(s), dst folder
	TCHAR * pbgpath=parm[0];
	TCHAR * folder=parm[1];
	FILE * fpbg=_tfopen(pbgpath,TEXT("rt"));
	int status=0;// 0:wait for "@begin Objects", 1:processing lines, 2:found "@end;"
	TCHAR buf[4096];
	TCHAR file[4096];
	TCHAR *fileptr;
	TCHAR lib[4096];
	TCHAR *c;
	TCHAR *ce;
	
	BOOL ret=true;
	
	printf("\t%S\n",pbgpath);
	if(fpbg){
		_tcscpy(lib,folder);
		if(_tcslen(lib)<1)_tcscat(lib,TEXT("."));
		if(lib[_tcslen(lib)-1]!='\\')_tcscat(lib,TEXT("\\"));
		ce=NULL;
		for(int i=0;pbgpath[i];i++)
			if(pbgpath[i]=='\\' || pbgpath[i]=='/')
				ce=pbgpath+i;
		if(ce){
			_tcscat(lib,ce+1);
			_tcsncpy(file,pbgpath,ce-pbgpath+1);
			fileptr=file+(ce-pbgpath+1);
		}else{
			_tcscat(lib,pbgpath);
			fileptr=file;
		}
		lib[_tcslen(lib)-1]='l';
		
		while( ret && _fgetts(buf,_tsizeof(buf),fpbg) ){
			switch(status){
				case 0:
					if(!_tcsicmp(trim(buf),TEXT("@begin Objects")))status=1;
					break;
				case 1:
					c=trim(buf);
					if(!_tcsicmp(c,TEXT("@end;")))status=2;
					else{
						if(c[0]=='"'){
							c++;
							ce=_tcschr(c,'"');
							if(ce){
								ce[0]=0;
								while(ce!=c){
									ce--;
									if(ce[0]=='\\' || ce[0]=='/'){
										c=ce+1;
										break;
									}
								}
								
								_tcscpy(fileptr,c);
								if(!_OrcaImport(file,lib, true, false, false))ret=false;
							}
						}
					}
					break;
				case 2:
					status=3;
					break;
			}
			if(status==3)break;
			
		}
		fclose(fpbg);
	}else{
		printline();
		_tprintf(TEXT("can't open PBG file \"%s\"\n"),pbgpath);
		ret=false;
	}
	return ret;
}
*/

