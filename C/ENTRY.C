/* ************************************************************** */
/* ネットケープ・プラグインの作成                                 */
/* -------------------------------------------------------------- */
/* エントリーポイント(システムとの通信)プログラム                 */
/* 【ENTRY.C】                                                    */
/* -------------------------------------------------------------- */
/*                   このファイルを、独自にカスタムしてください。 */
/* ************************************************************** */

#include <LUCIER.H>    /* ←通常はOS2.H 等と置き換えてください */
#include <NSPLUG32.H>  /* NSPLUG32 ライブラリー */
#include "PLUGIN.H"

/* ************************************************************** */
/* プラグイン初期化時に呼ばれる関数 (必須)                        */
/* ************************************************************** */
NPError NP_LOADDS NPP_Initialize(void)     {

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* プラグイン終了時に呼ばれる関数 (必須)                          */
/* ************************************************************** */
void NP_LOADDS NPP_Shutdown(void) {

return; }

/* ************************************************************** */
/* プラグイン初期化時に呼ばれる関数 (必須)                        */
/* ************************************************************** */
/* 参考情報 mode=NP_FULL 全画面モード                             */
/* ************************************************************** */
NPError NP_LOADDS NPP_New(NPMIMEType pluginType,NPP instance,uint16 mode,
                  int16 argc,char *argn[],char *argv[], NPSavedData* saved)   {
PPLUGINMEMORY pPlugINMemory;

CHAR  szFilename[CCHMAXPATH];
int i;
ULONG ulSize,rc;
PCHAR ptr;

   if (!instance) return NPERR_INVALID_INSTANCE_ERROR;
   /* *********************************************************** */
   /* NSPLUG32ライブラリーのメモリーを割り振る                   */
   /* *********************************************************** */
   pPlugINMemory=NspCreateNewInstance(PLUGIN_DLLNAME,NULL,
            pluginType,instance,mode,argc,argn,argv,saved,&rc);
   if(!pPlugINMemory) return rc;

   /* *********************************************************** */
   /* そこへさらに開発ユーザー独自のメモリーを割り振る (任意)     */
   /*    プラグインは同時に起動されている場合があります、その場合 */
   /*    ウインドウごとに保存しなければならないデーターは、下のよ */
   /*    うに割り振ったメモリー内におかなければいけません。そうし */
   /*    ないと、お互いにデーターをぐしゃぐしゃにしてしまいます。 */
   /* *********************************************************** */
   pPlugINMemory->pUserMemory=NPN_MemAlloc(sizeof(USERSTRUCT));
   memset(pPlugINMemory->pUserMemory,0,sizeof(USERSTRUCT));

   /* *********************************************************** */
   /* それでは、割り振ったメモリーに必要な初期化を行ってください  */
   /* *********************************************************** */

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* プラグイン破壊時に呼ばれる関数 (必須)                          */
/* ************************************************************** */
NPError NP_LOADDS NPP_Destroy(NPP instance, NPSavedData** save)   {
PPLUGINMEMORY pPlugINMemory;
HWND   hwnd;
   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return NPERR_NO_ERROR;

   hwnd=pPlugINMemory->hwnd;
   WinSendMsg(hwnd,WM_NSPLUG32_DESTROY,0,0);
   /* ******************************************************** */
   /* ウインドウの破壊等の必要な終了初期化処理をここで行う     */
   /* ******************************************************** */
   WinSubclassWindow(pPlugINMemory->hwnd, pPlugINMemory->lpfnOldWndProc);
   /* ******************************************************** */
   /* もしパラメーター等の保存が必要ならやはりここで行う       */
   /* *save = 保存するデーターへのポインタ                     */
   /* ******************************************************** */

   /* ******************************************************** */
   /* 開発ユーザー独自のメモリーを解放                         */
   /* ******************************************************** */
   NPN_MemFree(pPlugINMemory->pUserMemory);
   /* ******************************************************** */
   /* NSPLUG32 ライブラリーのメモリーを解放 (Nsp関数で代行可)  */
   /* ******************************************************** */
   NspDestroyInstance(pPlugINMemory);
   instance->pdata=NULL;

return NPERR_NO_ERROR;  }


/* *********************************************************** */
/* ウインドウを構築するイベント                                */
/* *********************************************************** */
NPError NP_LOADDS NPP_SetWindow(NPP instance, NPWindow* window)   {
PPLUGINMEMORY pPlugINMemory;

HAB   hab;  /* ←も～! PMプログラム、おなじみの変数ですね! */
HWND  hwnd;
HDC   hdc;  
HPS   hps;
SIZEL siz={0,0};

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return NPERR_NO_ERROR;

   hwnd = (HWND)pPlugINMemory->hwnd;
   hab=WinQueryAnchorBlock(hwnd);
   hps = pPlugINMemory->hps;

   if(window->window && !pPlugINMemory->hwnd)   {
      /* ***************************************************** */
      /* さあ、ブラウザ内にＰＭウインドウをつくりましょう      */
      /*  メモリーにシステムからの情報を保存しましょう         */
      /* ***************************************************** */
      pPlugINMemory->fWindow = window;
      hwnd = (HWND)pPlugINMemory->fWindow->window;
      hab=WinQueryAnchorBlock(hwnd);
      pPlugINMemory->hwnd=hwnd;

      /* ***************************************************** */
      /* 古いウインドウ情報等を保存     (ここは変更の必要なし) */
      /* ***************************************************** */
      pPlugINMemory->lpfnOldWndProc = WinSubclassWindow(pPlugINMemory->hwnd, SubclassWindowProc);
      NspAssociateInstance(pPlugINMemory->hwnd, pPlugINMemory);

      if(!hps)  {
         hdc = WinQueryWindowDC(hwnd);
         if(!hdc) hdc = WinOpenWindowDC(hwnd);
         hps = GpiCreatePS(hab, hdc, &siz, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
         pPlugINMemory->hps = hps;
        }
      WinSendMsg(hwnd,WM_NSPLUG32_CREATE,0,0);
    }   else {
      /* ***************************************************** */
      /* 既に作成済み？                                        */
      /* ***************************************************** */

      /* ウインドウハンドルが変わった? */
      if(hwnd != (HWND)window->window) {
         WinSendMsg(hwnd,WM_NSPLUG32_DESTROY,0,0);
         /* 新しいウインドウを登録 */
         pPlugINMemory->fWindow = window;
         /* 古いウインドウの制御下に一度戻す */
         WinSubclassWindow(hwnd, pPlugINMemory->lpfnOldWndProc);
         /* 新しいハンドルをもらう */
         hwnd = (HWND)pPlugINMemory->fWindow->window;
         pPlugINMemory->hwnd=hwnd;
         if(hwnd) {
            /* 再登録 */
            pPlugINMemory->lpfnOldWndProc = WinSubclassWindow(hwnd, SubclassWindowProc);
            NspAssociateInstance(hwnd, pPlugINMemory);
         }
         /* ************************************************** */
         /* もしウインドウハンドル依存のサービスが実行中なら   */
         /* ここでサービスをとめて再始動してください           */
         /*   例: WinStartTimer 等のタイマーサービス           */
         /* ************************************************** */

         /* ************************************************** */
         /* 古い HPS は破壊して、新しい HPS を構築してください */
         /* ************************************************** */
         if (!hps)  GpiDestroyPS(hps);
         hdc = WinQueryWindowDC(hwnd);
         if (! hdc)
            hdc = WinOpenWindowDC(hwnd);
         hps = GpiCreatePS(hab, hdc, &siz, PU_TWIPS | GPIT_MICRO | GPIA_ASSOC);
         pPlugINMemory->hps = hps;
         WinSendMsg(hwnd,WM_NSPLUG32_CREATE,0,0);
      }
   }

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* ストリーム構築イベント                                         */
/* ************************************************************** */
NPError NP_LOADDS NPP_NewStream(NPP instance, NPMIMEType type,
            NPStream *stream, NPBool seekable, uint16 *stype)  {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

   /* *********************************************************** */
   /* ファイルベースの場合は、 *stype=NP_ASFILE としてください。  */
   /* そうすると、ネットワークから一度ダウンロードして、固まりを  */
   /* 渡してくれます。 MIDIファイルなんてのは、ファイルベースだと */
   /* べんりですね。                                              */
   /* *********************************************************** */
   *stype=NP_ASFILE;

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* 書き込みレディ？イベント                                       */
/*  保存時に一気に書き込んでいけるサイズをシステムに教えてあげま  */
/*  しょう！                                                      */
/* ************************************************************** */
int32 NP_LOADDS   NPP_WriteReady(NPP instance, NPStream *stream)  {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

return STREAMBUFSIZE; }


/* ************************************************************** */
/* 書き込みイベント                                               */
/*  NP_ASFILEイベントでオープンしたファイルへ、指定されたサイズの */
/*  データーをファイルへ書き込んであげてください                  */
/* ************************************************************** */
int32 NP_LOADDS   NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer) {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

return len; }


/* ************************************************************** */
/* ストリーム破壊イベント                                         */
/* ************************************************************** */
NPError NP_LOADDS NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason) {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return NPERR_NO_ERROR;

return NPERR_NO_ERROR;  }

// BOOL  LdaPlayEventFunction(PCHAR pWaveBuffer,ULONG ulWaveSize,PULONG pulWrittenBufferSize);

/* ************************************************************** */
/* ディスクに保存されたファイル名が渡されるイベント               */
/*  ファイルを処理するときは、システムがファイルをダウンロードし  */
/*  た後、このイベントを呼びます。 ただし、NPP_NewStream 関数にて */
/*  *stype=NP_ASFILE; を指定しておいてください。                  */
/* ************************************************************** */
void NP_LOADDS NPP_StreamAsFile(NPP instance, NPStream *stream, const char* pszFilename)  {
PPLUGINMEMORY pPlugINMemory;

HWND hwnd;
static CHAR szFilename[CCHMAXPATH];

   if(!instance)  return;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return;

   hwnd=pPlugINMemory->hwnd;

   if(pszFilename)   {
      strcpy(szFilename,pszFilename);
      WinSendMsg(hwnd,WM_NSPLUG32_ASFILE, szFilename, stream);
   }

   WinInvalidateRect(hwnd, 0, TRUE);

return; }


/* ************************************************************** */
/* プリント (必須)                                                */
/* ************************************************************** */
void NP_LOADDS NPP_Print(NPP instance, NPPrint* printInfo)  {
PPLUGINMEMORY pPlugINMemory;

   if(!printInfo) return;
   if(!instance)  return;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return; 

   /* *********************************************************** */
   /* 話を簡単にするため、プリントしない                          */
   /* *********************************************************** */
   printInfo->print.fullPrint.pluginPrinted = FALSE;

return; }


/* ************************************************************** */
/* ハンドルイベント (マッキントッシュのみ)                        */
/* ************************************************************** */
int16 NP_LOADDS NPP_HandleEvent(NPP instance, void* event)  {
NPBool eventHandled = FALSE;

   if(!instance)  return eventHandled;

return eventHandled; }


