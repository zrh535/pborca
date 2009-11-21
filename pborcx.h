//redefinition of static 

#ifdef    __cplusplus
extern "C" {
#endif    /* __cplusplus */


/* *********************************************************************** */
/* Session function prototypes                                             */
/* *********************************************************************** */

#define PBFUNDEF_(x,y)	typedef x(WINAPI*y) 

PBFUNDEF_(VOID, _XORCA_SessionClose          )( HPBORCA hORCASession );
PBFUNDEF_(VOID, _XORCA_SessionGetError       )( HPBORCA hORCASession,
                                                  LPTSTR   lpszErrorBuffer,
                                                  INT     iErrorBufferSize );

PBFUNDEF_(HPBORCA, _XORCA_SessionOpen        )( void );

PBFUNDEF_(INT, _XORCA_SessionSetCurrentAppl  )( HPBORCA hORCASession,
                                                  LPTSTR   lpszApplLibName,  
                                                  LPTSTR   lpszApplName );

PBFUNDEF_(INT, _XORCA_SessionSetLibraryList )( HPBORCA    hORCASession,
                                                  LPTSTR FAR *pLibNames, 
                                                  INT        iNumberOfLibs );

PBFUNDEF_(INT, _XORCA_SetExeInfo )( HPBORCA    hORCASession,
                                                  PBORCA_EXEINFO * pExeInfo );


/* *********************************************************************** */
/* Library management function prototypes                                  */
/* *********************************************************************** */

PBFUNDEF_(INT, _XORCA_LibraryCommentModify )( HPBORCA hORCASession,
                                             LPTSTR   lpszLibName,  
                                             LPTSTR   lpszLibComments );

PBFUNDEF_(INT, _XORCA_LibraryCreate    )( HPBORCA hORCASession,
                                         LPTSTR   lpszLibName,  
                                         LPTSTR   lpszLibComments );

PBFUNDEF_(INT, _XORCA_LibraryDelete    )( HPBORCA hORCASession,
                                         LPTSTR   lpszLibName );   

PBFUNDEF_(INT, _XORCA_LibraryDirectory )( HPBORCA         hORCASession,
                                         LPTSTR           lpszLibName,       
                                         LPTSTR           lpszLibComments,  
                                         INT             iCmntsBuffSize, 
                                         PBORCA_LISTPROC pListProc, 
                                         LPVOID          pUserData );

PBFUNDEF_(INT, _XORCA_LibraryEntryCopy )( HPBORCA     hORCASession,
                                         LPTSTR       lpszSourceLibName,  
                                         LPTSTR       lpszDestLibName,  
                                         LPTSTR       lpszEntryName, 
                                         PBORCA_TYPE otEntryType );

PBFUNDEF_(INT, _XORCA_LibraryEntryDelete )( HPBORCA     hORCASession,
                                           LPTSTR       lpszLibName,  
                                           LPTSTR       lpszEntryName, 
                                           PBORCA_TYPE otEntryType );

PBFUNDEF_(INT, _XORCA_LibraryEntryExport )( HPBORCA     hORCASession,
                                           LPTSTR       lpszLibName,  
                                           LPTSTR       lpszEntryName, 
                                           PBORCA_TYPE otEntryType,    
                                           LPTSTR       lpszExportBuffer, 
                                           LONG        lExportBufferSize );

PBFUNDEF_(INT, _XORCA_LibraryEntryInformation )( HPBORCA           hORCASession,
                                                LPTSTR             lpszLibName,  
                                                LPTSTR             lpszEntryName, 
                                                PBORCA_TYPE       otEntryType,    
                                                PPBORCA_ENTRYINFO pEntryInfo );

PBFUNDEF_(INT, _XORCA_LibraryEntryMove )( HPBORCA     hORCASession,
                                         LPTSTR       lpszSourceLibName,  
                                         LPTSTR       lpszDestLibName,  
                                         LPTSTR       lpszEntryName, 
                                         PBORCA_TYPE otEntryType );


                                         
/* *********************************************************************** */
/* Source management function prototypes                                   */
/* *********************************************************************** */

PBFUNDEF_(INT, _XORCA_CheckOutEntry )(	HPBORCA     hORCASession,
	  									LPTSTR       lpszEntryName,
	  									LPTSTR       lpszSourceLibName,
	  									LPTSTR       lpszDestLibName,
	  									LPTSTR       lpszUserID,
                                    	PBORCA_TYPE otEntryType,
                                    	INT 		bMakeCopy);

PBFUNDEF_(INT, _XORCA_CheckInEntry )(	HPBORCA     hORCASession,
	  									LPTSTR       lpszEntryName,
	  									LPTSTR       lpszSourceLibName,
	  									LPTSTR       lpszDestLibName,
	  									LPTSTR       lpszUserID,
                                    	PBORCA_TYPE otEntryType,
                                    	INT 		bMoveEntry);

PBFUNDEF_(INT, _XORCA_ListCheckOutEntries )(	HPBORCA     		hORCASession,
	  										LPTSTR       		lpszLibraryName,
	  										PBORCA_CHECKPROC	lpCallbackFunction,
	  										LPVOID      		pUserData);

/* *********************************************************************** */
/* Compilation function prototypes                                         */
/* *********************************************************************** */

PBFUNDEF_(INT, _XORCA_CompileEntryImport )( HPBORCA        hORCASession,
                                           LPTSTR          lpszLibraryName,  
                                           LPTSTR          lpszEntryName, 
                                           PBORCA_TYPE    otEntryType, 
                                           LPTSTR          lpszComments,  
                                           LPTSTR          lpszEntrySyntax,
                                           LONG           lEntrySyntaxBuffSize, 
                                           PBORCA_ERRPROC pCompErrProc, 
                                           LPVOID         pUserData );

PBFUNDEF_(INT, _XORCA_CompileEntryImportList )( HPBORCA          hORCASession,
                                               LPTSTR far       *pLibraryNames,  
                                               LPTSTR far       *pEntryNames, 
                                               PBORCA_TYPE far *otEntryType, 
                                               LPTSTR far       *pComments,  
                                               LPTSTR far       *pEntrySyntaxBuffers,
                                               LONG far        *pEntrySyntaxBuffSizes, 
                                               INT              iNumberOfEntries, 
                                               PBORCA_ERRPROC   pCompErrProc, 
                                               LPVOID           pUserData );

PBFUNDEF_(INT, _XORCA_CompileEntryRegenerate )( HPBORCA        hORCASession,
                                               LPTSTR          lpszLibraryName,  
                                               LPTSTR          lpszEntryName, 
                                               PBORCA_TYPE    otEntryType, 
                                               PBORCA_ERRPROC pCompErrProc, 
                                               LPVOID         pUserData );

PBFUNDEF_(INT, _XORCA_ApplicationRebuild )( 	HPBORCA        		hORCASession,
                                           	PBORCA_REBLD_TYPE	eRebldType,  
                                           	PBORCA_ERRPROC 		pCompErrProc, 
                                            LPVOID         		pUserData );

/* *********************************************************************** */
/* Executable construction function prototypes                             */
/* *********************************************************************** */
PBFUNDEF_(INT, _XORCA_ExecutableCreate     )( HPBORCA        hORCASession,
                                             LPTSTR          lpszExeName,  
                                             LPTSTR          lpszIconName,        
                                             LPTSTR          lpszPBRName,        
	                                         PBORCA_LNKPROC pLinkErrProc,           
	                                         LPVOID         pUserData,              
                                             INT FAR       *iPBDFlags,  
                                             INT            iNumberOfPBDFlags,       
                                             LONG           lFlags );

PBFUNDEF_(INT, _XORCA_DynamicLibraryCreate )( HPBORCA        hORCASession,           
                                             LPTSTR          lpszLibraryName,        
                                             LPTSTR          lpszPBRName,            
                                             LONG           lFlags );
                                                              
/* *********************************************************************** */
/* Object query function prototypes                                        */
/* *********************************************************************** */

PBFUNDEF_(INT, _XORCA_ObjectQueryHierarchy )( HPBORCA         hORCASession,
                                             LPTSTR           lpszLibraryName,  
                                             LPTSTR           lpszEntryName, 
                                             PBORCA_TYPE     otEntryType, 
                                             PBORCA_HIERPROC pHierarchyProc, 
                                             LPVOID          pUserData );

PBFUNDEF_(INT, _XORCA_ObjectQueryReference )( HPBORCA         hORCASession,
                                             LPTSTR           lpszLibraryName,  
                                             LPTSTR           lpszEntryName, 
                                             PBORCA_TYPE     otEntryType, 
                                             PBORCA_REFPROC  pReferenceProc, 
                                             LPVOID          pUserData );

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif	/* __cplusplus */

