TCHAR*getParm(TCHAR*parms,int no,BOOL trim=true);

int	getParmCount(TCHAR*parms);

//returns reference to parameters or NULL if cmd is not a cmdname
TCHAR*iscmd(TCHAR*cmd,TCHAR*cmdname);

#define freeParm(p)	if(p){delete []p;p=NULL;}