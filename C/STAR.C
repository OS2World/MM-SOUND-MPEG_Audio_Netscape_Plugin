#include <LUCIER.H>    /* ←通常はOS2.H 等と置き換えてください */
#include <NSPLUG32.H>  /* NSPLUG32 ライブラリー */
#include "PLUGIN.H"

VOID APIENTRY StarThread(ULONG ulParam)   {
PUSERSTRUCT pUser;
PPLUGINMEMORY  pPlugINMemory;
HWND  hwnd;
HPS   hps;
ULONG i,j,ul;
HAB   hab;
HMQ   hmq;
PSTARSTRUCT pStar;
PRECTL      pRcl;
POINTL      ptl;

   hab=WinInitialize(0);  
   hmq=WinCreateMsgQueue(hab,0);

   pPlugINMemory=(PPLUGINMEMORY)ulParam;
   pUser=pPlugINMemory->pUserMemory;
   hwnd=pUser->hwnd;

   pRcl=&pPlugINMemory->rcl;
   WinQueryWindowRect(hwnd, pRcl);     

   pUser->tidStar=NULL;

   pUser->ulMaxStarStruct=20;
   memset(&pUser->star,0,sizeof(STARSTRUCT)*MAX_STAR_STRUCT);

   pRcl=&pPlugINMemory->rcl;

   while(1)  {
      DosRequestMutexSem(pUser->hMtxStar,SEM_INDEFINITE_WAIT);
      hwnd=pUser->hwnd;
      hps=WinGetPS(hwnd);
      GpiCreateLogColorTable(hps,0,LCOLF_RGB,0L,0L,NULL);
      pStar=&pUser->star[0];
      for(i=0;i<MAX_STAR_STRUCT;i++,pStar++)  {
         if(pStar->fEnable)  {
            if(pStar->ulBackColor==0x000080L)   {
               ptl.x=((pStar->x)>>8);
               ptl.y=((pStar->y)>>8);
               GpiSetColor(hps, 0x000080L);
               GpiSetPel(hps, &ptl);
            }
            /* 星を移動する */
            pStar->x=pStar->x+pStar->cx;
            pStar->y=pStar->y+pStar->cy;
            ptl.x=((pStar->x)>>8);
            ptl.y=((pStar->y)>>8);
            if(ptl.x<0 || ptl.x> pRcl->xRight || ptl.y<0 || ptl.y> pRcl->yTop)   {
               pStar->fEnable=FALSE;
            }
         }
         if(!pStar->fEnable)  {
            if(pUser->ulMaxStarStruct>i) {
               pStar->fEnable=TRUE;
               pStar->x=((pRcl->xRight)<<7);
               pStar->y=((pRcl->yTop)<<7);
               RECHECK:
               pStar->cz=1;
               pStar->cx=(rand()%4096)-2048;
               pStar->cy=(rand()%4096)-2048;
               if(pStar->cx*pStar->cx+pStar->cy*pStar->cy <= 128)    goto RECHECK; 
               pStar->ulColor=0xFFFFFFL;
               ptl.x=((pStar->x)>>8);
               ptl.y=((pStar->y)>>8);
            }
         }
         if(pStar->fEnable)  {
            if( (ptl.x>=pUser->ptlWindow.x && ptl.x<=pUser->ptlWindow2.x) && (ptl.y>=pUser->ptlWindow.y && ptl.y<=pUser->ptlWindow2.y) )   {
               pStar->ulBackColor=0xFFFFFFL;
            }  else  {
               pStar->ulBackColor=GpiQueryPel(hps,&ptl);
            }
            if(pStar->ulBackColor==0x000080L)   {
               GpiSetColor(hps, pStar->ulColor);
               GpiSetPel(hps, &ptl);
            }
         }
      }
      WinReleasePS(hps);
      DosReleaseMutexSem(pUser->hMtxStar);
      DosSleep(50);
   }

   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
return; }

