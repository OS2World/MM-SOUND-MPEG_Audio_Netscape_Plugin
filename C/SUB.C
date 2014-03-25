#include <LUCIER.H>    /* ←通常はOS2.H 等と置き換えてください */
#include <NSPLUG32.H>  /* NSPLUG32 ライブラリー */
#include "\PROJECT\LUCIER\C\LLMPACTL\COMMON.H"
#include "PLUGIN.H"
#include "DIALOG.H"

ULONG MoveWindows(PPLUGINMEMORY  pPlugINMemory, HWND hwnd) {
return 0L; }

ULONG CreateWindows(PPLUGINMEMORY  pPlugINMemory, HWND hwnd) {
PUSERSTRUCT pUser;
PRECTL      pRcl;
CHAR        szBuffer[40];
ULONG       ulColor;
HWND           hwndc;
   pUser=pPlugINMemory->pUserMemory;

   pRcl=&pPlugINMemory->rcl;
   WinQueryWindowRect(hwnd, pRcl);

   if(pUser->fJapanese)  strcpy(szBuffer,"8.MINCHO");
   else  strcpy(szBuffer,"8.Courier");
   if(!WinSetPresParam(hwnd,PP_FONTNAMESIZE,strlen(szBuffer)+1,szBuffer))  {
      if(pUser->fJapanese)  strcpy(szBuffer,"6.MINCHO");
      else  strcpy(szBuffer,"6.Courier");
      WinSetPresParam(hwnd,PP_FONTNAMESIZE,strlen(szBuffer)+1,szBuffer);
   }

   ulColor=0xCCCCCC;   WinSetPresParam(hwnd,PP_BACKGROUNDCOLOR,sizeof(ulColor),&ulColor);
   ulColor=0x222222;   WinSetPresParam(hwnd,PP_FOREGROUNDCOLOR,sizeof(ulColor),&ulColor);
   /* ***************************************************************** */
   /* 基本ウインドウ (ステータス表示は常にON)                           */
   /* ***************************************************************** */
   pUser->hwndStatus = WinCreateWindow(hwnd,WC_STATIC ,"MPEG Audio-1 Layer 1/2/3 Ver.1.00.", SS_TEXT|DT_CENTER|DT_VCENTER, 0, 0, 0, 0,hwnd,HWND_TOP,997,NULL,NULL);
   ulColor=0xCCCCCC;   WinSetPresParam(pUser->hwndStatus,PP_BACKGROUNDCOLOR,sizeof(ulColor),&ulColor);
   if(pUser->fJapanese)    WinSetWindowText(pUser->hwndStatus,"MPEGオーディオ1 第１～３階層 Ver.1.00");

return 0L; }


ULONG GetUrl(PPLUGINMEMORY  pPlugINMemory, HWND hwnd, PCHAR pszUrl, PCHAR pszTarget) {
PUSERSTRUCT pUser;
CHAR        szBuffer2[50],szUrl[512];
PCHAR ptr;
ULONG len;

   pUser=pPlugINMemory->pUserMemory;
   if(!pszUrl)  {
      SetStatus(pPlugINMemory, hwnd,"エラー:宛先不明。","Error:Unknown post url.");
      return 2L;
   }
   strcpy(szUrl,pszUrl);

   if(pszUrl[0]=='@')   {
      SetStatus(pPlugINMemory, hwnd, "専用情報への外部アクセスは禁止されています。","Cannot access to internal information.");
      return 1L;
   }

   SetStatus(pPlugINMemory, hwnd, szUrl, szUrl);
   NPN_GetURL(pPlugINMemory->instance, szUrl, pszTarget);

return 0L; }


ULONG SetStatus(PPLUGINMEMORY  pPlugINMemory, HWND hwnd, PCHAR pszJpnMessage, PCHAR pszEngMessage) {
PCHAR pszMessage;
PUSERSTRUCT pUser;

   if(!pPlugINMemory)   return 2L;
   pUser=pPlugINMemory->pUserMemory;
   if(!pUser)  {
      NPN_Status(pPlugINMemory->instance, "Npmpawv.dll Plug-in internal error / SetStatus().");
      return 1L;
   }

   if(pUser->fJapanese)    pszMessage=pszJpnMessage;
   else  pszMessage=pszEngMessage;

   pUser->ulTimerMessage=50;
// WinSetWindowText( pUser->hwndStatus, pszMessage);

   if(pPlugINMemory->mode==NP_FULL) {
      NPN_Status(pPlugINMemory->instance, pszMessage);
   }

return 0L;  }


ULONG    EscDrawPointerRes( HPS hps, HMODULE hmod, ULONG ulID, PPOINTL pPtl)  {
HPOINTER hptr;
   hptr = WinLoadPointer(HWND_DESKTOP, hmod, ulID);
   WinDrawPointer(hps, pPtl->x, pPtl->y, hptr, DP_NORMAL); 
   WinDestroyPointer(hptr);
return 0L; }


ULONG DrawString(HPS hps, PPOINTL pptl, PCHAR pszBuffer)  {
ULONG len;
   len=strlen(pszBuffer);

   pptl->y++;
   GpiSetColor(hps,0x205040L);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->y--;
   GpiSetColor(hps,0x205040L);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->x++;              
   GpiSetColor(hps,0x50A0FFL);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->x--;
   pptl->y-=14;       

return NO_ERROR; }

ULONG DrawIcon(PPLUGINMEMORY  pPlugINMemory, ULONG ulIconMenuID, HPS hps, PPOINTL pptl, PPOINTL pptlSize, HMODULE hmod, ULONG ulIconID, PCHAR pszJpnBuffer, PCHAR pszBuffer)  {
PUSERSTRUCT pUser;
ULONG len;
POINTL   ptl;

   pUser=pPlugINMemory->pUserMemory;

   if(pUser->fJapanese)    pszBuffer=pszJpnBuffer;
   len=strlen(pszBuffer);

   if(ulIconMenuID>=pUser->ulMaxIconStruct)  return 2L;
   pUser->icon[ulIconMenuID].fEnable=TRUE;
   pUser->icon[ulIconMenuID].swp.x=pptl->x;
   pUser->icon[ulIconMenuID].swp.y=pptl->y;
   pUser->icon[ulIconMenuID].swp.cx=pptlSize->x;
   pUser->icon[ulIconMenuID].swp.cy=pptlSize->y;
   strcpy(pUser->icon[ulIconMenuID].szName, pszBuffer);
/*
   pptl->y++;
   GpiSetColor(hps,0x808080L);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->y--;
   GpiSetColor(hps,0x808080L);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->x++;              
   GpiSetColor(hps,0xA0A0E0L);
   GpiCharStringAt( hps, pptl, len, pszBuffer);

   pptl->x--;
   pptl->y+=14;
*/
   if(ulIconID)   {
      EscDrawPointerRes(hps, hmod, ulIconID, pptl);
      pptl->y+=pptlSize->y+10;
   }

return NO_ERROR; }


ULONG TimeDraw( HPS hps, PUSERSTRUCT pUser)  {
POINTL   ptl;
CHAR     szBuffer[30];
      GpiSetPattern( hps, PATSYM_DEFAULT);
      GpiSetColor( hps, 0x000000L);
      ptl.x=pUser->ptlWindow.x+130+9;
      ptl.y=pUser->ptlWindow.y+88+5;
      GpiMove( hps, &ptl);
      ptl.x+=60;
      ptl.y+=15;
      GpiBox( hps, DRO_FILL, &ptl, 0,0);

      ptl.x-=59;
      ptl.y-=14;
      sprintf(szBuffer,"%ld:%02ld",pUser->ulTimeNow/60,pUser->ulTimeNow%60);
      DrawString(hps, &ptl, szBuffer);
return 0L; }


VOID APIENTRY PlayThread(ULONG ulParam)   {
PUSERSTRUCT pUser;
HMQ   hmq;
HAB   hab;
CHAR  szBuffer[CCHMAXPATH];
HWND  hwnd;
ULONG rc,rc2;
   pUser=(PUSERSTRUCT)ulParam;
   hwnd=pUser->hwnd;
   WinInitialize(hab);
   WinCreateMsgQueue(hab,0);

   strcpy(pUser->szAlias,"ALIASWAVE");

   sprintf(szBuffer,"open %s alias %s wait",pUser->szFileName,pUser->szAlias);
   rc=EscMciSendString(szBuffer,NULL,0,0,0);
   if((rc&0x0000FFFFL)==MCIERR_SUCCESS)   {
      sprintf(szBuffer,"play %s wait",pUser->szAlias);
      EscMciSendString(szBuffer,NULL,0,0,0);
      sprintf(szBuffer,"close %s wait",pUser->szAlias);
      EscMciSendString(szBuffer,NULL,0,0,0);
   }  else  {
      sprintf(szBuffer,"Cannot play the MPEG Audio file\nReturn code=%ld",rc);
      WinMessageBox(HWND_DESKTOP, hwnd, szBuffer,"Error - MPEG Audio-1 Layer 1/2/3 Plug-in", 0, MB_OK|MB_MOVEABLE|MB_ERROR);
   }
   pUser->tid=0;
   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
return; }

VOID APIENTRY SaveThread(ULONG ulParam)   {
PUSERSTRUCT pUser;
PPLUGINMEMORY  pPlugINMemory;
HWND  hwnd;
ULONG rc;
HAB   hab;
HMQ   hmq;

   hab=WinInitialize(0);  
   hmq=WinCreateMsgQueue(hab,0);

   pPlugINMemory=(PPLUGINMEMORY)ulParam;
   pUser=pPlugINMemory->pUserMemory;
   hwnd=pUser->hwnd;

   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);

   DosSetPriority(PRTYS_THREAD,PRTYC_IDLETIME,0L,0L);
   rc=EscCopyFile( pUser->szSaveFileName, pUser->szFileName);
   if(rc)   {
      SetStatus(  pPlugINMemory, hwnd, "エラー: ファイルの保存に失敗しました", "Error: Cannot save the MPEG Audio file");
   }  else  {
      SetStatus(  pPlugINMemory, hwnd, "ファイルの保存作業を完了しました", "Complete to save the MPEG Audio file");
   }

   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_SAVE, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);

   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
   pUser->tidSave=FALSE;

return; }

VOID APIENTRY InterfacePlayThread(ULONG ulParam)   {
PUSERSTRUCT pUser;
HMQ   hmq;
HAB   hab;
CHAR  szBuffer[CCHMAXPATH];
HWND  hwnd;
ULONG    rc,rc2,ulSendSize,i,ulDataSize,busy,ulCastSize,ulCastPos;
LLDATA   ldWaveData,ldDest;
PMMMPADECODE      pMpaDec;
PPLUGINMEMORY  pPlugINMemory;
BOOL           fOK=FALSE;

   hab=WinInitialize(0);
   hmq=WinCreateMsgQueue(hab,0);

   pPlugINMemory=(PPLUGINMEMORY)ulParam;
   pUser=pPlugINMemory->pUserMemory;
   hwnd=pUser->hwnd;


   pUser->fPause=FALSE;

   if(pUser->pBuffer)   DosFreeMem(pUser->pBuffer);
   pUser->pBuffer=NULL;

   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);

   DosSetPriority(PRTYS_THREAD,PRTYC_IDLETIME,0L,0L);

   pUser->ulLayer=0;
   pUser->ulFrequency=0;
   pUser->ulChannels=0;
   pUser->ulBitRate=0;
   pUser->ulFileSize=0;
   pUser->ulTimeNow=0L;

   pUser->ulMaxStarStruct=0L;

   /* デコーダーが起動済みの場合はクローズする */
   if(pUser->lhMpaDecode)  {
      MpaCloseDecoder( pUser->lhMpaDecode);
      MpaDestroyDecoder( pUser->lhMpaDecode);
      pUser->lhMpaDecode=NULL;
   }
   /* オーディオが起動済みの場合はクローズする */
   if(pUser->fAudioOpened) {
      EscDestroyWaveInterface( &pUser->mmWaveInfo);
      pUser->fAudioOpened=FALSE;
   }
   /* デコーダーを起動する */
   SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオファイルをオープンしています...", "Now opening MPEG Audio file...");

   if(rc=MpaCreateDecoder( &pUser->lhMpaDecode, 0L))   {
      pUser->ulDefaultMessage=0L;   /* 標準ステータスメッセージの変更 */
      SetStatus(  pPlugINMemory, hwnd, "エラー: MPEGオーディオデコーダが使用できません", "Error: Cannot use the MPEG Audio decoder.");
      sprintf(szBuffer,"Cannot offer MPEG Audio service. System error.\nReturn code=%ld",rc);
      WinMessageBox(HWND_DESKTOP, hwnd, szBuffer,"Error - MPEG Audio-1 Layer 1/2/3 Plug-in", 0, MB_OK|MB_MOVEABLE|MB_ERROR);
      goto Exit3;
   }
   if(rc=MpaOpenDecoder( pUser->lhMpaDecode, pUser->szFileName))  {
      pUser->ulDefaultMessage=0L;   /* 標準ステータスメッセージの変更 */
      SetStatus(  pPlugINMemory, hwnd, "エラー: MPEGオーディオファイルがオープンできません", "Error: Cannot open the MPEG Audio file.");
      sprintf(szBuffer,"Cannot open MPEG Audio file.\nReturn code=%ld",rc);
      WinMessageBox(HWND_DESKTOP, hwnd, szBuffer,"Error - MPEG Audio-1 Layer 1/2/3 Plug-in", 0, MB_OK|MB_MOVEABLE|MB_ERROR);
      goto Exit2;
   }
   /* ヘッダ情報の収得 */
   MpaGetHeader( pUser->lhMpaDecode, &pUser->mmAudioHdr);
   /* 詳細情報の収得 */
   pMpaDec=(PMMMPADECODE)pUser->lhMpaDecode;
   pUser->ulLayer=pMpaDec->info.lay;
   pUser->ulFrequency=pMpaDec->mmAudioHdr.mmXWAVHeader.WAVEHeader.ulSamplesPerSec;
   pUser->ulChannels=pMpaDec->mmAudioHdr.mmXWAVHeader.WAVEHeader.usChannels;
   pUser->ulBitRate=pMpaDec->ulBitRate;
   pUser->ulFileSize=pMpaDec->ulMpaFileSize;
   pUser->ulTimeNow=0L;

   WinInvalidateRect(hwnd, 0, TRUE);
   WinUpdateWindow(hwnd);

   /* ウエーブ・オーディオが起動されていない場合は起動する */
   if(!pUser->fAudioOpened)   {
      SetStatus(  pPlugINMemory, hwnd, "演奏装置に接続中です...", "Now connecting to WAVE Audio device...");
      memset(&pUser->mmWaveInfo, 0, sizeof(MMAUDIOHEADER));
      rc=EscCreateWaveInterface(&pUser->mmWaveInfo, "NPMPA.DLL", NULL, WAVEIF_OPEN_SPS_44100|WAVEIF_OPEN_BIT_16|WAVEIF_OPEN_CH_2);  // |WAVEIF_OPEN_CELBUFFER
      if(rc)   {
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);
         WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, TRUE);
         pUser->ulDefaultMessage=0L;   /* 標準ステータスメッセージの変更 */
         SetStatus(  pPlugINMemory, hwnd, "エラー: このシステムではダイレクトオーディオ装置が使用できません", "Error: Cannot use the Direct Audio Device at the system.");
         sprintf(szBuffer,"Cannot open Direct Audio Interface. Please check LLWaveIF Interface sub system.\nReturn code=%ld",rc);
         WinMessageBox(HWND_DESKTOP, hwnd, szBuffer,"Error - MPEG Audio-1 Layer 1/2/3 Plug-in", 0, MB_OK|MB_MOVEABLE|MB_ERROR);
         goto Exit;
      } 
      pUser->fAudioOpened=TRUE;
   }

   /* メニューを適切にセットする */
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);

   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_ABOUTTHEFILE, TRUE);

   SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオを演奏中です...", "Now playing the MPEG Audio...");
   pUser->ulDefaultMessage=2L;   /* 標準ステータスメッセージの変更 */

   if(pUser->fAudioOpened)   {
      /* 一秒間の先行バッファリングをおこなう */
      DosAllocMem(&pUser->psWaveBuffer,44100L*4L,PAG_WRITE|PAG_READ);
      DosSetMem(pUser->psWaveBuffer,44100L*4L,PAG_DEFAULT|PAG_COMMIT);
      memset(pUser->psWaveBuffer,0,44100L*4L);
      pUser->pBuffer=pUser->psWaveBuffer;
      ulSendSize=EscSendWave(&pUser->mmWaveInfo, pUser->pBuffer, 44100*4L);
   }

   /* 演奏ループ */
   ulDataSize=163840*3L;
   ulCastSize=8192;
   ulCastPos=0L;
   do {
      /* ウエーブ展開先メモリーを割り振る */
      DosAllocMem(&pUser->psWaveBuffer,ulDataSize*4L,PAG_WRITE|PAG_READ);
      DosSetMem(pUser->psWaveBuffer,ulDataSize*4L,PAG_DEFAULT|PAG_COMMIT);
      /* 小さな圧縮解除ループを用いて、ＣＰＵ負荷を睨む */
      retry:
      busy=EscQueryBusyLevel();
      if(busy>1000)   busy=1000;
      if(busy>=940)   {
         DosSleep((busy-940)/4);
         if(pUser->ulMaxStarStruct>20)
            pUser->ulMaxStarStruct-=2;
         if(pUser->ulMaxStarStruct>80)
            pUser->ulMaxStarStruct-=20;
      }  else  {
         if(pUser->ulMaxStarStruct<MAX_STAR_STRUCT-1)
            pUser->ulMaxStarStruct++;
      }
      rc2=EscWaveEntry(&pUser->mmWaveInfo, WAVEIF_GETTIME, 0, SEEK_CUR);
      pUser->ulTimeNow=rc2/1000L;
      /* 指定サイズを圧縮解除 */
      DosRequestMutexSem(pUser->hMtx,SEM_INDEFINITE_WAIT);
      rc=MpaRead( pUser->lhMpaDecode, (PCHAR)&pUser->psWaveBuffer[ulCastPos/2L], ulCastSize);
      DosReleaseMutexSem(pUser->hMtx);
      /* 圧縮解除されたサイズから処理を選択 */
      if(rc)   {
         ulCastPos+=rc;
         if(ulCastPos<ulDataSize-ulCastSize) goto retry;    
      }
      if(ulCastPos)  {
         /* CDDAフォーマットに変換 */
         ldWaveData.pBuffer=(PCHAR)pUser->psWaveBuffer;
         ldWaveData.ulSize=ulCastPos;
         ldWaveData.pStruct=&pUser->mmAudioHdr;
         ldDest.pStruct=&pUser->mmAudioCddaHdr;
         /* CDDA品質に変換 */
         if(CnvWaveToCdda(&ldDest, &ldWaveData, NULL))   {
            /* 変換失敗 */
            ldDest.pBuffer=ldWaveData.pBuffer;
            ldDest.ulSize=ldWaveData.ulSize;
            ldDest.pStruct=ldWaveData.pStruct;
         }  else  {
            DosFreeMem(ldWaveData.pBuffer);
            ldWaveData.pBuffer=NULL;
         }
         ulCastPos=0L;
         DosSetPriority(PRTYS_THREAD,PRTYC_REGULAR,0L,0L);
         /* ウエーブ転送のためのループ処理 */
         RetryWave:
         if(pUser->fAudioOpened)   {
            /* 演奏デバイスがオープンされている場合のみ参照 */
            rc2=EscWaveEntry(&pUser->mmWaveInfo, WAVEIF_GETTIME, 0, SEEK_CUR);
         }
         pUser->ulTimeNow=rc2/1000L;
         /* ポーズ処理 */
         if(pUser->fPause)    {
            DosSleep(250);
            goto RetryWave;
         }
         /* ウエーブデーターの転送 */
         pUser->pBuffer=ldDest.pBuffer;
         if(pUser->fAudioOpened)   {
            /* 演奏デバイスがオープンされている場合のみ転送 */
            if(pUser->pBuffer)   {
               ulSendSize=EscSendWave(&pUser->mmWaveInfo, pUser->pBuffer, ldDest.ulSize);
            }
            if(!ulSendSize)   { 
               DosSleep(250); goto RetryWave;
            }
            pUser->pBuffer=NULL;
         }  else  {
            if(pUser->pBuffer)   DosFreeMem(pUser->pBuffer);
            pUser->pBuffer=NULL;
         }
         DosSetPriority(PRTYS_THREAD,PRTYC_IDLETIME,0L,0L);
      }
      DosSleep(32);
   }  while(rc==ulCastSize);
   Exit:
   /* オーディオ・メモリーを解放する */
   // if(pUser->psWaveBuffer) EscFreeMem(pUser->psWaveBuffer);
   // 自動解放なので解放しない
   pUser->psWaveBuffer=NULL;
   /* デコーダーが起動済みの場合はクローズする */
   if(pUser->lhMpaDecode)  MpaCloseDecoder( pUser->lhMpaDecode);

   pUser->ulDefaultMessage=0L;   /* 標準ステータスメッセージの変更 */
   SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオのデコードを完了しました、演奏の終了を待機しております...", "All of the MPEG Audio is decoded. Now waiting to complete...");
   fOK=TRUE;

   Exit2:
   if(pUser->lhMpaDecode)  MpaDestroyDecoder( pUser->lhMpaDecode);
   Exit3:
   pUser->lhMpaDecode=NULL;

   /* オーディオが起動済みの場合はクローズする */
   if(pUser->fAudioOpened) {
      /* 演奏終了まで待機 */
      rc=EscWaveEntry(&pUser->mmWaveInfo, WAVEIF_GETPOSITION, 0, SEEK_CUR);
      DosSleep(500);
      do  {
         i=rc;
         rc2=EscWaveEntry(&pUser->mmWaveInfo, WAVEIF_GETTIME, 0, SEEK_CUR);
         pUser->ulTimeNow=rc2/1000L;
         rc=EscWaveEntry(&pUser->mmWaveInfo, WAVEIF_GETPOSITION, 0, SEEK_CUR);
         DosSleep(500);
      }  while(i!=rc); /* end of while */
      /* クローズ処理 */
      DosSleep(2000);
      EscDestroyWaveInterface( &pUser->mmWaveInfo);
      pUser->fAudioOpened=FALSE;
   }

   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);

   if(fOK)  {
      pUser->ulDefaultMessage=1L;   /* 標準ステータスメッセージの変更 */
      SetStatus(  pPlugINMemory, hwnd, "MPEGオーディオ: 完了", "MPEG Audio: done");
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PLAY, TRUE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVE, TRUE);
      WinEnableMenuItem( pUser->hwndMenu,ID_MENU_SAVEAS, TRUE);
   }


   pUser->tid=0;

   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
   WinCheckMenuItem( pUser->hwndMenu,ID_MENU_PLAY, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_OPEN, TRUE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_STOP, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_PAUSE, FALSE);
   WinEnableMenuItem( pUser->hwndMenu,ID_MENU_RESUME, FALSE);

   WinDestroyMsgQueue(hmq);
   WinTerminate(hab);
return; }

