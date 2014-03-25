/* ************************************************************** */
/* ネットケープ・プラグインの作成                                 */
/* -------------------------------------------------------------- */
/* ウインドウ・プロシージャー部(ＰＭイベント)プログラム           */
/* 【WINDOW.C】                                                   */
/* -------------------------------------------------------------- */
/*                   このファイルを、独自にカスタムしてください。 */
/* ************************************************************** */
#define INCL_WINWORKPLACE
#include <LUCIER.H>    /* ←通常はOS2.H 等と置き換えてください */
#include <NSPLUG32.H>  /* NSPLUG32 ライブラリー */
#include "PLUGIN.H"
#include "DIALOG.H"


MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
static PPLUGINMEMORY  pPlugINMemory;
static PUSERSTRUCT pUser;
   switch(msg) {
   case WM_INITDLG:
      pPlugINMemory=mp2;
      pUser=pPlugINMemory->pUserMemory;
      if(pUser->fJapanese) {
         WinSetDlgItemText(hwnd, ID_MF_SS_TITLE,"オーディオファイルの表題 (MPAフォーマットのみ有効)");
      }
      break;
   case WM_DIALOG_UPDATE:
      pPlugINMemory=mp2;
      pUser=pPlugINMemory->pUserMemory;
      break;
   case WM_QUIT:
      pUser->hwndATFDialog=NULL;
      return FALSE;
   case WM_CLOSE:
      pUser->hwndATFDialog=NULL;
      WinDismissDlg( hwnd, TRUE );
//    WinPostMsg(hwnd,WM_QUIT,0,0);
      return FALSE;
   case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))  {
      case ID_MF_OK:
         pUser->hwndATFDialog=NULL;
         WinDismissDlg( hwnd, TRUE );
//       WinPostMsg(hwnd,WM_QUIT,0,0);
         break;
      }
      return 0L;
   }
return WinDefDlgProc( hwnd, msg, mp1, mp2 );  }

/* ************************************************************** */
/* サブクラス・ウインドウ・プロシージャー                         */
/* ************************************************************** */
MRESULT APIENTRY  SubclassWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)  {
PPLUGINMEMORY  pPlugINMemory;
PUSERSTRUCT pUser;
NPStream    *pStream;
RECTL    invalidRcl,rcl;
PRECTL   pRcl;
POINTL   ptl,ptlw,ptlSize;
HPS      hps;
CHAR     szBuffer[CCHMAXPATH];
ULONG    rc,i;
PICONSTRUCT pIcon;
HOBJECT  hObject;
SWP      swp;

   pPlugINMemory = NspGetInstance(hwnd);
   pUser=pPlugINMemory->pUserMemory;
   pUser->hwnd=hwnd;

   switch(msg) {
   case WM_NSPLUG32_CREATE:
      strcpy(pUser->szTitle,"MPEG Audio-1 PlugIN");
      hps=pPlugINMemory->hps;
      pUser->fJapanese=EscReadyForJapanese();
      pUser->hwndMenu=WinLoadMenu(hwnd, pPlugINMemory->hModule, 1+pUser->fJapanese);
      pUser->hwndMenu2=WinLoadMenu(hwnd, pPlugINMemory->hModule, 5+pUser->fJapanese);
      if(!pUser->hwndMenu)   {
         SetStatus(  pPlugINMemory, hwnd, "エラー: ＤＬＬ参照不能エラー。プラグインのインストールが正常でない可能性があります(\\OS2\\DLLディレクトリにもコピーしましたか？)。", "Error: DLL error. Installation of the plug-in is missed? (Did you copy this plug-in to \\OS2\\DLL directory too?)");
      }
      hmodMpaTable=pPlugINMemory->hModule;

      SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 起動", "MPEG Audio: Startup");

      pUser->ulMaxIconStruct=MAX_ICON_STRUCT;

      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_INDEX, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_ABOUTTHEFILE, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_CONNECTTOOFFER, FALSE);

      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, TRUE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, FALSE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, FALSE);

      pUser->ulEyeCount=120;
      pUser->ulTimerCount=50;
      pUser->ulTimerMessage=50;
      pUser->ulTimerLogo=60;
      pUser->ulLogoID=rand()%3;

      pUser->ulTimerID=WinStartTimer(WinQueryAnchorBlock(hwnd),hwnd,0,100L);

//    BndWinInsertIcon( pUser->lhBndWindow, 0L);
//    BndWinInsertIcon( pUser->lhBndWindow, 0L);

      if(pUser->hwndATFDialog)
         WinPostMsg(pUser->hwndATFDialog,WM_DIALOG_UPDATE,0L,pPlugINMemory);

      pUser->hwnd=hwnd;
      DosCreateMutexSem(NULL, &pUser->hMtx, DC_SEM_SHARED, FALSE);
      DosCreateMutexSem(NULL, &pUser->hMtxStar, DC_SEM_SHARED, FALSE);
      DosCreateThread(&pUser->tidStar,StarThread,(ULONG)pPlugINMemory,CREATE_READY,24000L);

      break;
   case WM_NSPLUG32_DESTROY:
      WinStopTimer(WinQueryAnchorBlock(hwnd),hwnd,pUser->ulTimerID);
      DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
      DosRequestMutexSem(pUser->hMtxStar,SEM_INDEFINITE_WAIT);

      if(pUser->hwndATFDialog)   {
         WinDismissDlg( pUser->hwndATFDialog, TRUE );
         WinDestroyWindow(pUser->hwndATFDialog);
         pUser->hwndATFDialog=NULL;
      }
      if(pUser->tidStar) {
         DosResumeThread(pUser->tidStar);
         DosKillThread(pUser->tidStar);
         DosWaitThread(&pUser->tidStar, DCWW_WAIT);
         pUser->tidStar=NULL;
      }
      DosCloseMutexSem(pUser->hMtxStar);
      if(pUser->tidSave) {
         DosResumeThread(pUser->tidSave);
         DosKillThread(pUser->tidSave);
         DosWaitThread(&pUser->tidSave, DCWW_WAIT);
         pUser->tidSave=NULL;
      }
      if(pUser->tid) {
         DosResumeThread(pUser->tid);
         DosKillThread(pUser->tid);
         DosWaitThread(&pUser->tid, DCWW_WAIT);
         pUser->tid=NULL;
         if(pUser->lhMpaDecode)  MpaCloseDecoder( pUser->lhMpaDecode);
         if(pUser->lhMpaDecode)  MpaDestroyDecoder( pUser->lhMpaDecode);
         pUser->lhMpaDecode=FALSE;
         if(pUser->fAudioOpened) {
            EscDestroyWaveInterface( &pUser->mmWaveInfo);
            pUser->fAudioOpened=FALSE;
         }
      }


      DosCloseMutexSem(pUser->hMtx);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_ABOUTTHEFILE, FALSE);
      pUser->pBuffer=NULL;
//    WinDestroyWindow(pUser->hwndList);
//    WinDestroyWindow(pUser->hwndMle);
//    WinDestroyWindow(pUser->hwndStatus);
      break;
   case WM_NSPLUG32_ARGV:
      if(!mp1 || !mp2)  {
         break;
      }
      break;
   case WM_TIMER:
      if(!pUser->lhMpaDecode)   {
         i=EscQueryBusyLevel();
         if(i>1000)   i=1000;
         if(i>=940)   {
            if(pUser->ulMaxStarStruct>36)
               pUser->ulMaxStarStruct-=16;
         }  else  {
            if(pUser->ulMaxStarStruct<MAX_STAR_STRUCT-5) {
               if(i<=540)   {
                  pUser->ulMaxStarStruct+=4;
               }
            }
         }
      }

      if(pUser->ulTimerLogo)  {
         pUser->ulTimerLogo--;
      }
      if(!pUser->ulTimerLogo)  {
         pUser->ulTimerLogo=300;
         pUser->ulEyeCount=2;
         rc=pUser->ulLogoID;
         for(i=0;i<50;i++) {
            pUser->ulLogoID=rand()%3;
            if(rc!=pUser->ulLogoID)   break;
         }
         if(rc!=pUser->ulLogoID)   {
            pUser->ulEyeCount=120;
            invalidRcl.xLeft=pUser->ptlWindow.x;
            invalidRcl.yBottom=pUser->ptlWindow.y;
            invalidRcl.xRight=pUser->ptlWindow.x+320;
            invalidRcl.yTop=pUser->ptlWindow.y+240;
            WinInvalidateRect(hwnd, &invalidRcl, TRUE);
            WinUpdateWindow(hwnd);
         }
      }

      if(pUser->ulLogoID==1)  { /* ELV FACE */
         if(pUser->ulEyeCount)   pUser->ulEyeCount--;
         if(pUser->ulEyeCount<9)   {
            ptl.x=pUser->ptlWindow.x+40;
            ptl.y=pUser->ptlWindow.y+120;
            hps=WinGetPS(hwnd);
            GpiCreateLogColorTable(hps,0,LCOLF_RGB,0L,0L,NULL);
            switch(pUser->ulEyeCount)  {
            case 0:
               pUser->ulEyeCount=120-(rand()%80);
               break;
            case 3:
               if( (rand()%2)==0 )  pUser->ulEyeCount=80-(rand()%50);
               break;
            case 1:
            case 4:
               EscDrawBitmapRes(hps,pPlugINMemory->hModule, 200+pUser->ulLogoID+60, ptl);
               break;
            case 2:
            case 6:
               EscDrawBitmapRes(hps,pPlugINMemory->hModule, 200+pUser->ulLogoID+40, ptl);
               break;
            case 7:
            case 5:
               EscDrawBitmapRes(hps,pPlugINMemory->hModule, 200+pUser->ulLogoID+20, ptl);
               break;
            }
            WinReleasePS(hps);
         }
      }

      if(pUser->ulTimerMessage)  {
         pUser->ulTimerMessage--;
      }
      if(!pUser->ulTimerMessage)  {
         pUser->ulTimerMessage=50;
         switch(pUser->ulDefaultMessage)   {
         case 0:
            break;
         case 1:
            SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 完了", "MPEG Audio: done");
            break;
         case 2:
            SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオを演奏中です...", "Now playing the MPEG Audio...");
            break;
         case 3:
            SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオの形式変換を行いながら演奏中です...", "Now playing & converting the MPEG Audio...");
            break;
         case 4:
            SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオの形式変換を行いながら保存中です...", "Now converting  & saveing the MPEG Audio...");
            break;
         }
      }

      if(pUser->ulTimerCount)  {
         pUser->ulTimerCount--;
      }
      if(!pUser->ulTimerCount)  {
         pUser->ulTimerCount=4;
         hps=WinGetPS(hwnd);
         GpiCreateLogColorTable(hps,0 /* LCOL_PURECOLOR */ ,LCOLF_RGB,0L,0L,NULL);
         TimeDraw( hps, pUser);
         WinReleasePS(hps);
      }
      break;
   case WM_SIZE:
      pRcl=&pPlugINMemory->rcl;
      WinQueryWindowRect(hwnd, pRcl);
      WinInvalidateRect(hwnd, 0, TRUE);
      WinUpdateWindow(hwnd);
      break;
   case WM_MOUSEMOVE:
      if(!pUser)  break;
      pUser->mousePos.x=SHORT1FROMMP(mp1);   pUser->mousePos.y=SHORT2FROMMP(mp1);
      for(i=0;i<pUser->ulMaxIconStruct;i++)  {
         pIcon=&pUser->icon[i];
         if(pIcon->fEnable)   {
            if(pUser->mousePos.x>pIcon->swp.x && pUser->mousePos.x<pIcon->swp.x+pIcon->swp.cx)  {
               if(pUser->mousePos.y>pIcon->swp.y && pUser->mousePos.y<pIcon->swp.y+pIcon->swp.cy)  {
                  SetStatus(  pPlugINMemory, hwnd, pIcon->szName,  pIcon->szName);
                  break;
               }
            }
         }
      }
      break;
      break;
   case WM_BUTTON1DBLCLK:
      if(!pUser)  break;
      pUser->mousePos.x=SHORT1FROMMP(mp1);   pUser->mousePos.y=SHORT2FROMMP(mp1);
      for(i=0;i<pUser->ulMaxIconStruct;i++)  {
         pIcon=&pUser->icon[i];
         if(pIcon->fEnable)   {
            if(pUser->mousePos.x>pIcon->swp.x && pUser->mousePos.x<pIcon->swp.x+pIcon->swp.cx)  {
               if(pUser->mousePos.y>pIcon->swp.y && pUser->mousePos.y<pIcon->swp.y+pIcon->swp.cy)  {
                  WinPostMsg(hwnd, WM_COMMAND, MPFROMSHORT(i), 0L);
                  break;
               }
            }
         }
      }
      break;
   case WM_BUTTON2UP:
      if(!pUser)  break;
      pUser->mousePos.x=SHORT1FROMMP(mp1);   pUser->mousePos.y=SHORT2FROMMP(mp1);

      if( (pUser->ptlWindow.x<pUser->mousePos.x && pUser->ptlWindow2.x>pUser->mousePos.x) && (pUser->ptlWindow.y<pUser->mousePos.y && pUser->ptlWindow2.y>pUser->mousePos.y) )   {
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 主制御メニュー", "MPEG Audio: Main control menu");
         rc=WinPopupMenu(hwnd, hwnd, pUser->hwndMenu, pUser->mousePos.x, pUser->mousePos.y, 0, PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD );
         if(!rc)   {
            SetStatus(  pPlugINMemory, hwnd, "エラー: 主制御メニューがオープンできません。プラグインのインストールが正常でない可能性があります。", "Error: Popup menu cannot open. Installation of the plug-in is missed?");
         }
      }  else  {
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: メニュー", "MPEG Audio: Control menu");
         rc=WinPopupMenu(hwnd, hwnd, pUser->hwndMenu2, pUser->mousePos.x, pUser->mousePos.y, 0, PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1 | PU_MOUSEBUTTON2 | PU_KEYBOARD );
         if(!rc)   {
            SetStatus(  pPlugINMemory, hwnd, "エラー: メニューがオープンできません。プラグインのインストールが正常でない可能性があります。", "Error: Popup menu cannot open. Installation of the plug-in is missed?");
         }
      }
      return 0L;
   case WM_CONTROL:
      break;
   case WM_CHAR:
      if(( SHORT1FROMMP(mp1) & KC_CHAR) /* && (SHORT1FROMMP(mp1) & KC_LONEKEY) */ )  {
         if( (SHORT1FROMMP(mp1) & KC_ALT) )     break;
         if( (SHORT1FROMMP(mp1) & KC_SHIFT) )   break;
         if( (SHORT1FROMMP(mp1) & KC_CTRL) )    {
            switch(SHORT1FROMMP(mp2) & 0x00FF)  {
            case 'P':   case 'p':
               DosBeep(800,50);
               break;
            case 'S':   case 's':
               DosBeep(800,50);
               break;
            }  /* end of switch */
         }
      }
      if(( SHORT1FROMMP(mp1) & KC_VIRTUALKEY) && (SHORT1FROMMP(mp1) & KC_LONEKEY) )  {
         if( (SHORT1FROMMP(mp1) & KC_ALT) )     break;
         if( (SHORT1FROMMP(mp1) & KC_CTRL) )    break;
         if( (SHORT1FROMMP(mp1) & KC_SHIFT) )   {
            switch( SHORT2FROMMP(mp2)) {
            case VK_F10:
               pUser->mousePos.x=0;
               pUser->mousePos.y=0;
               WinSendMsg(hwnd, WM_BUTTON2UP, NULL, NULL);
               break;
            }  /* end of switch */
         }
      }
      break;
   case WM_COMMAND:
      switch(SHORT1FROMMP(mp1))  {
      case ID_ICONMENU_VOLUME:
         hObject=WinQueryObject("<MMPM2_VOL>");
         if(!hObject)   {
            SetStatus(  pPlugINMemory, hwnd, "エラー: 音量調整が起動できません", "Error: Cannot open the volume object.");
         }  else  {
            WinOpenObject( hObject, 0, TRUE);
         }
         break;
      case ID_ICONMENU_FOLDER:
         hObject=WinQueryObject("<MMPM2_MASTER>");
         if(!hObject)   {
            SetStatus(  pPlugINMemory, hwnd, "エラー: 音量調整が起動できません", "Error: Cannot open the volume object.");
         }  else  {
            WinOpenObject( hObject, 0, TRUE);
         }
         break;
      case ID_ICONMENU_NEWBROWSER:
         rc=GetUrl(pPlugINMemory, hwnd, "", "");
         break;
      case ID_MENU_CONNECTTOTM2LABO:
         rc=GetUrl(pPlugINMemory, hwnd, "http://www7.big.or.jp/~os2warp/", "");
         break;
      case ID_MENU_PRODUCTINFO:
         if(pUser->fJapanese) {
            WinMessageBox(HWND_DESKTOP, hwnd, 
               "MPEGオーディオ1 第１～３階層(レイヤー1,2,3)\nネットスケープ・ナビゲーターOS/2版プラグイン\n\nCopyrights (c) 2000,\nTeam MMOS/2 Tokyo Multimedia Communications!\nAll Rights Reserved.","製品情報　MPEGオーディオ1 第１～３階層",
              0, MB_OK|MB_MOVEABLE|MB_INFORMATION);
         }  else  {
            WinMessageBox(HWND_DESKTOP, hwnd, 
               "MPEG Audio-1 Layer 1/2/3 Plug-in\nfor OS/2 Netscape Navigator\n\nCopyrights (c) 2000,\nTeam MMOS/2 Tokyo Multimedia Communications!\nAll Rights Reserved.","Product Info - MPEG Audio-1 Layer 1/2/3",
              0, MB_OK|MB_MOVEABLE|MB_INFORMATION);
         }
         break;
      case ID_MENU_STOP:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, TRUE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
         pUser->ulDefaultMessage=1L;   /* 標準ステータスメッセージの変更 */
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 停止", "MPEG Audio: Stop");
         if(pUser->tid) {
            DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
            DosResumeThread(pUser->tid);
            DosKillThread(pUser->tid);
            DosWaitThread(&pUser->tid, DCWW_WAIT);
            DosReleaseMutexSem(pUser->hMtx);
            pUser->tid=NULL;
            if(pUser->lhMpaDecode)  MpaCloseDecoder( pUser->lhMpaDecode);
            if(pUser->lhMpaDecode)  MpaDestroyDecoder( pUser->lhMpaDecode);
            pUser->lhMpaDecode=FALSE;
            if(pUser->fAudioOpened) {
               EscDestroyWaveInterface( &pUser->mmWaveInfo);
               pUser->fAudioOpened=FALSE;
            }
         }
         pUser->fPause=FALSE;
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, TRUE);
         break;
      case ID_MENU_PLAY:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 演奏", "MPEG Audio: Play");
         if(pUser->tid) {
            DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
            DosResumeThread(pUser->tid);
            DosKillThread(pUser->tid);
            DosWaitThread(&pUser->tid, DCWW_WAIT);
            DosReleaseMutexSem(pUser->hMtx);
            pUser->tid=NULL;
         }
         pUser->hwnd=hwnd;
         DosCreateThread(&pUser->tid,InterfacePlayThread,(ULONG)pPlugINMemory,CREATE_READY,48000L);
         break;
      case ID_MENU_PAUSE:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, TRUE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, TRUE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
         pUser->ulDefaultMessage=0L;   /* 標準ステータスメッセージの変更 */
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 一時停止", "MPEG Audio: Pause");
         pUser->fPause=TRUE;
         break;
      case ID_MENU_RESUME:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, TRUE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
         WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, TRUE);
         pUser->ulDefaultMessage=2L;   /* 標準ステータスメッセージの変更 */
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 演奏再開", "MPEG Audio: Resume");
         pUser->fPause=FALSE;
         break;
      case ID_MENU_OPEN:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, FALSE);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ファイルのオープン", "MPEG Audio: Open");

         strcpy( pUser->szFileNameBuffer, "*.MP?");
         rc=EscOpenSaveFileDlg( hwnd, pUser->szFileNameBuffer, "Open the MPEG Audio file", NULL, NULL, NULL);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 完了", "MPEG Audio: done");

         if(rc)   {
            WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, TRUE);
            break;
         }
         WinSendMsg(hwnd, WM_LUCIER_DROPFILE, pUser->szFileNameBuffer, NULL);
         break;
      case ID_MENU_SAVE:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, FALSE);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ファイルの保存", "MPEG Audio: Save");

         strcpy( pUser->szFileNameBuffer, "AUDIO001.MPA");
         rc=EscOpenSaveFileDlg( hwnd, pUser->szFileNameBuffer, "Save the MPEG Audio file", NULL, NULL, NULL);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 完了", "MPEG Audio: done");
         if(rc)   {
            WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);
            break;
         }
         WinSendMsg(hwnd, WM_USER, pUser->szFileNameBuffer, NULL);
         break;
      case ID_MENU_SAVEAS:
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, FALSE);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ファイル形式を変換して保存", "MPEG Audio: Save as...");

         strcpy( pUser->szFileNameBuffer, "AUDIO001.MPA");
         rc=EscOpenSaveFileDlg( hwnd, pUser->szFileNameBuffer, "Save the Multimedia WAVE Audio file", NULL, NULL, NULL);
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 完了", "MPEG Audio: done");
         if(rc)   {
            WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, TRUE);
            break;
         }
         WinSendMsg(hwnd, WM_USER+1, pUser->szFileNameBuffer, NULL);
         break;
      case ID_MENU_ABOUTTHEFILE:
         if(pUser->hwndATFDialog)   break;
         pUser->hwndATFDialog=WinLoadDlg(HWND_DESKTOP, hwnd , ( PFNWP )MainDialogProc, pPlugINMemory->hModule, ID_MPEGFILE_DIALOG, (PVOID)pPlugINMemory);
         break;
      }
      return 0;
   case DM_DROP:
      return EscDrop(hwnd,(PDRAGINFO) mp1);
   case DM_DRAGOVER:
      return EscDragOver((PDRAGINFO) mp1);
   case WM_USER:
      if(!mp1) return 0L;
      pUser->hwnd=hwnd;
      strcpy(pUser->szSaveFileName,mp1);
      DosCreateThread(&pUser->tidSave,SaveThread,(ULONG)pPlugINMemory,CREATE_READY,48000L);
      break;
   case WM_NSPLUG32_ASFILE:
      if(!mp1) return 0L;

      if(pUser->hwndATFDialog)   {
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ダイアログ情報を更新中...", "MPEG Audio: Updating dialog informations...");
         WinPostMsg(pUser->hwndATFDialog,WM_DIALOG_UPDATE,0L,pPlugINMemory);
      }

      SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ファイルがキャストされました", "MPEG Audio: Casted the file");

      pStream=(NPStream *)mp2;
      if(pStream && pStream->url)  {
         WinSetWindowText(pUser->hwndStatus,pStream->url);
      }  else  {
         WinSetWindowText(pUser->hwndStatus, mp1);
      }
      if(strlen(mp1) >= sizeof(pUser->szFileName)+2) {
         SetStatus(  pPlugINMemory, hwnd, "エラー: 演奏不能", "Error: Cannot play");
      }  else  {
         SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 演奏スレッドの開始", "MPEG Audio: Create the player thread");
         if(pUser->tid) {
            DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
            DosResumeThread(pUser->tid);
            DosKillThread(pUser->tid);
            DosWaitThread(&pUser->tid, DCWW_WAIT);
            DosReleaseMutexSem(pUser->hMtx);
            pUser->tid=NULL;
         }
         pUser->hwnd=hwnd;
         strcpy(pUser->szFileName,mp1);
         DosCreateThread(&pUser->tid,InterfacePlayThread,(ULONG)pPlugINMemory,CREATE_READY,48000L);
      }
      break;
   case WM_LUCIER_DROPFILE:
      SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: ファイルのドロップ", "MPEG Audio: Dropped the file");
      sprintf(szBuffer,"file:///%s",mp1);
      rc=GetUrl(pPlugINMemory, hwnd, szBuffer, NULL);
//    if(!rc)  WinSetWindowText( pUser->hwndStatus, szBuffer);
      return 0;
   case WM_PAINT:
      hps=pPlugINMemory->hps;
      pRcl=&pPlugINMemory->rcl;

      hps=WinBeginPaint(hwnd, NULL /* hps */ , &invalidRcl);
//    GpiErase(hps);

      DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
      DosRequestMutexSem(pUser->hMtxStar,SEM_INDEFINITE_WAIT);

      GpiCreateLogColorTable(hps, 0L ,LCOLF_RGB,0L,0L,NULL);

      WinQueryWindowRect(hwnd, pRcl);
      memcpy(&rcl,pRcl,sizeof(RECTL));

      /* DESKTOP */
      if(mp2!=1)        {
         WinFillRect(hps,pRcl,0x000080);
      }

      ptlw.x=(rcl.xRight)/2-160;
      ptlw.y=(rcl.yTop)/2-120;
      if(ptlw.x<0) ptlw.x=0;
      if(ptlw.y<0) ptlw.y=0;
      memcpy(&pUser->ptlWindow, &ptlw, sizeof(POINTL));
      pUser->ptlWindow2.x=pUser->ptlWindow.x+320;
      pUser->ptlWindow2.y=pUser->ptlWindow.y+240;

      /* DESKTOP - LOGO */
      ptl.x=rcl.xRight-80;
      ptl.y=rcl.yTop-300;
      EscDrawBitmapRes(hps,pPlugINMemory->hModule, 211, ptl);

      /* DESKTOP - ICONS */
      ptl.x=ptlw.x;
      ptl.y=ptlw.y;
      ptlSize.x=320;
      ptlSize.y=240;     
      DrawIcon(pPlugINMemory, 0, hps, &ptl, &ptlSize, pPlugINMemory->hModule, 0, "MPEGオーディオ", "MPEG Audio");

      ptl.x=10;
      ptl.y=10;
      ptlSize.x=40;
      ptlSize.y=40;
//    DrawIcon(pPlugINMemory, ID_ICONMENU_VOLUME, hps, &ptl, &ptlSize, pPlugINMemory->hModule, 100, "音量調整", "Volume");
//    DrawIcon(pPlugINMemory, ID_ICONMENU_FOLDER, hps, &ptl, &ptlSize, pPlugINMemory->hModule, 110, "フォルダ", "Sounds");
//    DrawIcon(pPlugINMemory, ID_ICONMENU_NEWBROWSER, hps, &ptl, &ptlSize, pPlugINMemory->hModule, 120, "ウインドウ", "Window");


      /* BASE PICTURE */
      ptl.x=ptlw.x;
      ptl.y=ptlw.y;
      EscDrawBitmapRes(hps,pPlugINMemory->hModule, 200+ pUser->ulLogoID, ptl);

      /* BLUE WALL */
      GpiSetPattern( hps, PATSYM_HALFTONE);
      ptl.x=ptlw.x+125;
      ptl.y=ptlw.y+83;
      GpiSetColor( hps, 0x0000FFL);
      GpiMove( hps, &ptl);
      ptl.x+=175;
      ptl.y+=115;
      GpiBox( hps, DRO_FILL, &ptl, 0,0);

      ptl.x=ptlw.x+302;
      ptl.y=ptlw.y+83;
      GpiSetColor( hps, 0x0000FFL);
      GpiMove( hps, &ptl);
      ptl.x+=6;
      ptl.y+=115;
      GpiBox( hps, DRO_FILL, &ptl, 0,0);

      ptl.x=ptlw.x+125;
      ptl.y=ptlw.y+200;
      GpiSetColor( hps, 0x0000FFL);
      GpiMove( hps, &ptl);
      ptl.x+=175;
      ptl.y+=6;
      GpiBox( hps, DRO_FILL, &ptl, 0,0);

      ptl.x=ptlw.x+302;
      ptl.y=ptlw.y+200;
      GpiSetColor( hps, 0x0000FFL);
      GpiMove( hps, &ptl);
      ptl.x+=6;
      ptl.y+=6;
      GpiBox( hps, DRO_FILL, &ptl, 0,0);

      /* DATA DRAW */
      ptl.x=ptlw.x;
      ptl.y=ptlw.y;

      ptl.x+=130;
      ptl.y+=182;

      if(pUser->ulLayer)   {
         sprintf(szBuffer,"MPEG Audio 1 Layer %ld",pUser->ulLayer);
         DrawString(hps, &ptl, szBuffer);
      }

      if(pUser->ulBitRate) {
         sprintf(szBuffer,"Bit Rate %ldkbps",pUser->ulBitRate/1000L);
         DrawString(hps, &ptl, szBuffer);
      }

      if(pUser->ulChannels)   {
         sprintf(szBuffer,"Channels %ld",pUser->ulChannels);
         DrawString(hps, &ptl, szBuffer);
      }

      if(pUser->ulFrequency)  {
         sprintf(szBuffer,"Sampling Rate %ldHz",pUser->ulFrequency);
         DrawString(hps, &ptl, szBuffer);
      }

      if(pUser->ulFileSize)   {
         sprintf(szBuffer,"File size %ld k-bytes",pUser->ulFileSize/1024L);
         DrawString(hps, &ptl, szBuffer);
      }

      /* BLACK WINDOW */
      ptl.x=ptlw.x;
      ptl.y=ptlw.y;
      ptl.x+=130;
      ptl.y+=88;

      EscDrawBitmapRes(hps,pPlugINMemory->hModule, 210, ptl);

      /* TIME DRAW */
      TimeDraw( hps, pUser);

      DosReleaseMutexSem(pUser->hMtxStar);
      DosReleaseMutexSem(pUser->hMtx);

      WinEndPaint(hps);
      break;
   case WM_ERASEBACKGROUND:
      return (MRESULT)(1);
   }  /* end of switch */
return NspDefWindowProc(hwnd,msg,mp1,mp2);   }

