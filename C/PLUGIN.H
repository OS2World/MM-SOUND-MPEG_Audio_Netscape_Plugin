/* ************************************************************** */
/* ネットケープ・プラグインの作成                                 */
/* -------------------------------------------------------------- */
/* 開発ユーザー用のヘッダファイル                                 */
/* 【PLUGIN.H】                                                   */
/* -------------------------------------------------------------- */
/*                   このファイルを、独自にカスタムしてください。 */
/* ************************************************************** */

#define PLUGIN_DLLNAME        "NPMPA.DLL"
#define PLUGIN_PROFILEAPP     "NPMPA"

#define STREAMBUFSIZE         0X0FFFFFFFL
MRESULT APIENTRY  SubclassWindowProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY MainDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#define  WM_DIALOG_UPDATE           (WM_USER+120)

#define  ID_MENU_SETUP              405
#define  ID_MENU_CONNECTTOTM2LABO   409
#define  ID_MENU_PRODUCTINFO        410
#define  ID_MENU_CONNECTTOOFFER     412
#define  ID_MENU_ABOUTTHEFILE       413
#define  ID_MENU_INDEX              414

#define  ID_MENU_WEBSITE            1000
#define  ID_MENU_MAILSITE           2000

#define  ID_MENU_SAVE               420
#define  ID_MENU_SAVEAS             421
#define  ID_MENU_OPEN               422
#define  ID_MENU_PLAY               423
#define  ID_MENU_STOP               424
#define  ID_MENU_PAUSE              425
#define  ID_MENU_RESUME             426

#define  MAX_ICON_STRUCT            20

#define  ID_ICONMENU_VOLUME         5
#define  ID_ICONMENU_FOLDER         6
#define  ID_ICONMENU_NEWBROWSER     7

typedef struct {
   BOOL        fEnable;
   SWP         swp;
   CHAR        szName[20];
}  ICONSTRUCT, *PICONSTRUCT, **PPICONSTRUCT;

#define  MAX_STAR_STRUCT            512

typedef struct {
   BOOL        fEnable;
   LONG        x,y,z,cx,cy,cz;
   ULONG       ulColor;
   ULONG       ulBackColor;
}  STARSTRUCT, *PSTARSTRUCT, **PPSTARSTRUCT;

typedef struct {
   CHAR           szTitle[CCHMAXPATH];
   HWND           hwndMenu,hwndMenu2;
   POINTL         mousePos;

   TID            tid;
   TID            tidSave;
   TID            tidSaveAs;
   TID            tidStar;

   CHAR           szAlias[60];

   ULONG          ulMaxIconStruct;
   ICONSTRUCT     icon[MAX_ICON_STRUCT];

   ULONG          ulMaxStarStruct;
   STARSTRUCT     star[MAX_STAR_STRUCT];

   BOOL           fJapanese;
   CHAR           szFileName[CCHMAXPATH];
   CHAR           szSaveFileName[CCHMAXPATH];
   CHAR           szFileNameBuffer[CCHMAXPATH];
   PCHAR          pBuffer;

   ULONG          ulTimerID;
   ULONG          ulTimerMessage;
   ULONG          ulDefaultMessage;

   ULONG          ulTimerCount;
   ULONG          ulEyeCount;

   ULONG          ulTimerLogo;
   ULONG          ulLogoID;

   LHANDLE        lhMpaDecode;
   MMIOINFO       mmWaveInfo;
   BOOL           fAudioOpened;
   PSHORT         psWaveBuffer;
   MMAUDIOHEADER  mmAudioHdr;
   MMAUDIOHEADER  mmAudioCddaHdr;

   BOOL           fPause;


   USHORT      usMode;

   HWND        hwnd;

   HWND        hwndStatus;
   HWND           hwndATFDialog;
   POINTL         ptlWindow;
   POINTL         ptlWindow2;
   /* ファイル情報 */
   ULONG          ulFileSize;
   ULONG          ulLayer;
   ULONG          ulBitRate;
   ULONG          ulFrequency;
   ULONG          ulChannels;
   ULONG          ulTimeNow;

   HMTX           hMtx,hMtxStar;

   LHANDLE        lhBndWindow,lhBndIcon;
}  USERSTRUCT, *PUSERSTRUCT;

typedef struct {
   ULONG       ulNumberAll;
   PCHAR       pBuffer;
   PCHAR       pPointer[100];
   ULONG       ulNumber[100];
}  PTRL, *PPTRL, **PPPTRL;

ULONG MoveWindows(PPLUGINMEMORY  pPlugINMemory, HWND hwnd) ;
ULONG CreateWindows(PPLUGINMEMORY  pPlugINMemory, HWND hwnd) ;
ULONG GetUrl(PPLUGINMEMORY  pPlugINMemory, HWND hwnd, PCHAR pszUrl, PCHAR pszTarget) ;
ULONG SetStatus(PPLUGINMEMORY  pPlugINMemory, HWND hwnd, PCHAR pszJpnMessage, PCHAR pszEngMessage) ;
ULONG    EscDrawPointerRes( HPS hps, HMODULE hmod, ULONG ulID, PPOINTL pPtl)  ;
ULONG DrawString(HPS hps, PPOINTL pptl, PCHAR pszBuffer)  ;
ULONG DrawIcon(PPLUGINMEMORY  pPlugINMemory, ULONG ulIconMenuID, HPS hps, PPOINTL pptl, PPOINTL pptlSize, HMODULE hmod, ULONG ulIconID, PCHAR pszJpnBuffer, PCHAR pszBuffer)  ;
ULONG TimeDraw( HPS hps, PUSERSTRUCT pUser)  ;
VOID APIENTRY PlayThread(ULONG ulParam)   ;
VOID APIENTRY SaveThread(ULONG ulParam)   ;
VOID APIENTRY InterfacePlayThread(ULONG ulParam)   ;

VOID APIENTRY StarThread(ULONG ulParam)   ;

