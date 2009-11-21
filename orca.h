//sizeof TCHAR buffer in TCHARs
#define _tsizeof(a) (sizeof(a)/sizeof(TCHAR))

void printline();

void printsyserr();


TCHAR * trim(TCHAR * c,TCHAR*space=TEXT(" \t\n\r"));
TCHAR * rtrim(TCHAR * c);
TCHAR* getFileDir(TCHAR*file);

typedef BOOL (*ORCA_FUNC) ( TCHAR**parm );

BOOL orcaSetLiblistStart(TCHAR**parm);
BOOL orcaSetLiblistAdd(TCHAR*parm);
BOOL orcaSetLiblistEnd(TCHAR**parm);

BOOL orcaSessionBegin(TCHAR**parm);
BOOL orcaSessionEnd(TCHAR**parm);
BOOL orcaLibraryEntryCopy ( TCHAR**parm);
BOOL orcaSetApplication(TCHAR**parm);
BOOL orcaBuildApplication(TCHAR**parm);
BOOL orcaBuildLibrary(TCHAR**parm);
BOOL orcaLibraryCreate(TCHAR**parm);


BOOL orcaBuildExecutable(TCHAR**parm);
BOOL orcaSys(TCHAR**parm);
BOOL orcaTimestamp(TCHAR**parm);
BOOL orcaDeleteItem(TCHAR**parm);
BOOL orcaEcho(TCHAR**parm);
BOOL orcaDeleteDuplex(TCHAR**parm);
BOOL orcaRegenerate(TCHAR**parm);
BOOL orcaRegenerateAll(TCHAR**parm);
BOOL orcaExport(TCHAR**parm);
BOOL orcaImport(TCHAR**parm);
BOOL orcaImportMove(TCHAR**parm);
BOOL orcaImportMoveOk(TCHAR**parm);

BOOL orcaCheckList(TCHAR**parm);

BOOL orcaCD(TCHAR**parm);
BOOL orcaImportPbg2Pbl(TCHAR**parm);
BOOL orcaImportPbg2PblAny(TCHAR**parm);

BOOL isSessionStarted();

BOOL orcaTargetCreateLib(TCHAR**parm);
BOOL orcaTargetImport(TCHAR**parm);
BOOL orcaTargetImportAny(TCHAR**parm);

BOOL orcaTargetSetApp(TCHAR**parm);
BOOL orcaTargetSetLiblist(TCHAR**parm);

BOOL orcaSetExeInfo(TCHAR**parm);

BOOL orcaProfileString(TCHAR**parm);
BOOL orcaProfileExeInfo(TCHAR**parm);

#define PBL_BLOCK_SIZE 512

/*
+--------------------------------------------------------------+
I Library Header Block (512 Byte)                              I
+-----------+------------+-------------------------------------+
I Pos.      I Type       I Information                         I
+-----------+------------+-------------------------------------+
I   1 - 4   I Char(4)    I 'HDR*'                              I
I   5 - 18  I String     I 'PowerBuilder' + 0x00 + 0x00        I
I  19 - 22  I Char(4)    I PBL Format Version? (0400/0500/0600)I
I  23 - 26  I Long       I Creation/Optimization Datetime      I
I  29 - ff  I String     I Library Comment                     I
I 285 - 288 I Long       I Offset of first SCC data block      I
I 289 - 292 I Long       I Size (Net size of SCC data)         I
+-----------+------------+-------------------------------------+
*/

typedef struct {
    char		hdr[4];
    char		pb[14];
	char		ver[4];
	long		date;
	char		comment[254];//??
	long		scc_entry;
	long		scc_len;
	char		other[224];
}PBL_BLOCK_HDR;

/*
+--------------------------------------------------------------+
I  Bitmap Block (512 Byte)                                     I
+-----------+------------+-------------------------------------+
I Pos.      I Type       I Information                         I
+-----------+------------+-------------------------------------+
I  1 - 4    I Char(4)    I 'FRE*'                              I
I  5 - 8    I Long       I Offset of next block or 0           I
I  9 - 512  I Bit(504)   I Bitmap, each Bit represents a block I
+-----------+------------+-------------------------------------+
(512 - 8) * 8 = 4032 Blocks are referenced
*/
typedef struct {
    char		hdr[4];
    long		next;
	char		bmp[504];
}PBL_BLOCK_FRE;


/*
+--------------------------------------------------------------+
I Node Block (3072 Byte)                                       I
+-----------+------------+-------------------------------------+
I Pos.      I Type       I Information                         I
+-----------+------------+-------------------------------------+
I   1 - 4   I Char(4)    I 'NOD*'                              I
I   5 - 8   I Long       I Offset of next (left ) block or 0   I
I   9 - 12  I Long       I Offset of parent block or 0         I
I  13 - 16  I Long       I Offset of next (right) block or 0   I
I  21 - 22  I Integer    I Count of entries in that node       I
I  33 - ff  I Chunks     I 'ENT*'-Chunks                       I
+-----------+------------+-------------------------------------+
*/

typedef struct {
    char		hdr[4];
    long		prev;
    long		parent;
    long		next;
	WORD		count;
}PBL_BLOCK_NOD;

/*
+--------------------------------------------------------------+
I Entry Chunk (Variable Length)                                I
+-----------+------------+-------------------------------------+
I Pos.      I Type       I Information                         I
+-----------+------------+-------------------------------------+
I   1 - 4   I Char(4)    I 'ENT*'                              I
I   5 - 8   I Char(4)    I PBL version? (0400/0500/0600)       I
I   9 - 12  I Long       I Offset of first data block          I
I  13 - 16  I Long       I Objectsize (Net size of data)       I
I  17 - 20  I Long       I Unix datetime                       I
I  21 - 22  I Integer    I Length of comment                   I
I  23 - 24  I Integer    I Length of objectname                I
I  25 - ff  I String     I Objectname                          I
+-----------+------------+-------------------------------------+
*/
typedef struct {
    char		hdr[4];
    char		ver[4];
    long		offset;
    long		size;
    long		date;
	WORD		comm_len;
	WORD		obj_len;
}PBL_BLOCK_ENT;

/*
+--------------------------------------------------------------+
I Data Block (512 Byte)                                        I
+-----------+------------+-------------------------------------+
I Pos.      I Type       I Information                         I
+-----------+------------+-------------------------------------+
I   1 - 4   I Char(4)    I 'DAT*'                              I
I   5 - 8   I Long       I Offset of next data block or 0      I
I   9 - 10  I Integer    I Length of data in block             I
I  11 - XXX I Blob{}     I Data (maximum Length is 502         I
+-----------+------------+-------------------------------------+
*/
typedef struct {
    char		hdr[4];
    long		next;
	WORD		size;
	char		data[502];
}PBL_BLOCK_DAT;

/*

Those functions are deprecated from PowerBuilder version 8.0

TCHAR checkOutSign(TCHAR c){
	switch(c){
		case 'r' : return '*';
		case 's' : return '<';
		case 'd' : return '>';
		case 'u' : return '?';
	}
	return c;
}

//prints a string (up to 0 character) and returns reference to the next TCHAR after 0.
int _printString(TCHAR*c,int i,int max,BOOL print){
	while( i<max ){
		if( !c[i] ){
			if(print)putc('\t',stdout);
			return i;
		}
		if(print)putc(c[i],stdout);
		i++;
	}
	return i;
}


//file iterator for libraries
BOOL fiOrcaCheckList(TCHAR**parm){
	TCHAR*lib=parm[0];
	int ret;
	TCHAR*errtext="i/o error";
	PBL_BLOCK_HDR * hdr;
	PBL_BLOCK_DAT * dat;
	TCHAR buf[PBL_BLOCK_SIZE];
	int i=0;
	//status: 0:library; 1:object; 2:user; 3status;
	int status=0;
	
	FILE*f=_tfopen(lib,"rb");
	if(!f){errtext="can't open file";goto error;}
	if(fread( buf,PBL_BLOCK_SIZE,1,f )!=1){errtext="error reading header";goto error;}
	hdr=(PBL_BLOCK_HDR*)buf;
	if(_tcsnccmp(hdr->hdr,"HDR*",4)){errtext="not a PB Library";goto error;}
	printf("\nlibrary %s :\n",lib);
	if(hdr->scc_entry){
		fseek( f, hdr->scc_entry, SEEK_SET );
		if(ftell(f)!=hdr->scc_entry){errtext="can't find scc entry";goto error;}
		ret=fread( buf,PBL_BLOCK_SIZE,1,f );
		dat=(PBL_BLOCK_DAT*)buf;
		while(ret==1){
			if(_tcsnccmp(dat->hdr,"DAT*",4)){errtext="error reading SCC block";goto error;}
			while(i<dat->size){
				switch(status){
					case 0:
						i=_printString(dat->data,i,dat->size,false);//skip library name
						if(i<dat->size)if(!dat->data[i]){status++;i++;}
						break;
					case 1:
						i=_printString(dat->data,i,dat->size,true);
						if(i<dat->size)if(!dat->data[i]){status++;i++;}
						break;
					case 2:
						i=_printString(dat->data,i,dat->size,true);
						if(i<dat->size)if(!dat->data[i]){status++;i++;}
						break;
					case 3:
						printf("%c\n",checkOutSign(dat->data[i]) );
						status=0;
						i++;
						break;
				}
			}
			if(!dat->next)break;
			fseek( f, dat->next, SEEK_SET );
			if(ftell(f)!=dat->next){errtext="can't find sequence page";goto error;}
			ret=fread( buf,PBL_BLOCK_SIZE,1,f );
			i=0;
		}
		if(ret!=1){goto error;}
		
	}
	putc('\n',stdout);
	fclose(f);
	return true;
	error:
	if(f)fclose(f);
	printline();
	printf("%s: \"%s\"\n",errtext,lib);
	return false;
	
}

BOOL orcaCheckList(TCHAR**parm){
	return _fileIterator(fiOrcaCheckList,parm,0);
}

*/

