

#define IMPEXP_ASCII  1
#define IMPEXP_HASCII 2
#define IMPEXP_UNI    3
#define IMPEXP_UTF8   4

#define ORCAIMPORT_ITEM_LEN	300
#define ORCAIMPORT_COMM_LEN	3000


typedef struct {
    TCHAR*	buf;
    TCHAR*	ptr;
    TCHAR*	bin;
    TCHAR	item[ORCAIMPORT_ITEM_LEN];
	TCHAR	comm[ORCAIMPORT_COMM_LEN];
	PBORCA_TYPE t;
}ORCAIMPORTREAD;


BOOL _orcaWrite(FILE*f,TCHAR*item,PBORCA_TYPE t,TCHAR*comment,TCHAR*buf,int expType);
BOOL _orcaRead(TCHAR*fname,ORCAIMPORTREAD*ir);
