#include <LUCIER.H>
#include <NSPLUG32.H>

#pragma library("NPMPA.LIB");

NPError NP_LOADDS _export NP_MMInstall( HWND hwndOwnerHandle, PSZ pszSourcePath, PSZ pszTargetDrive,   PSZ pszMyParms, HWND hwndMinstallHandle, PSZ pszResponseFile );

void main()   {
HAB   hab;
HMQ   hmq;
ULONG rc;

   hab=WinInitialize(0);
   hmq=WinCreateMsgQueue(hab,0);

   rc=NP_MMInstall( HWND_DESKTOP, ".", "", "NPMPA.DLL", HWND_DESKTOP,NULL);

   WinDestroyMsgQueue(hmq);   
   WinTerminate(hab);

exit(0); }

