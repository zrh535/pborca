

#define TARGET_INFO_LIB     0
#define TARGET_INFO_APP     1
#define TARGET_INFO_APP_LIB 2


typedef BOOL (*ORCA_TARGET_INFO) ( int info_type, TCHAR*parm, TCHAR*srcdir, void*udata);

BOOL _readTargetFile( TCHAR *fname, ORCA_TARGET_INFO cb, void*udata );
