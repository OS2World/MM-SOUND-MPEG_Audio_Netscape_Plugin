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
/* マルチメディア・インストール時に呼ばれるサービス関数           */
/* ************************************************************** */
NPError NP_LOADDS _export NP_MMInstall( HWND hwndOwnerHandle, PSZ pszSourcePath, PSZ pszTargetDrive,   PSZ pszMyParms, HWND hwndMinstallHandle, PSZ pszResponseFile )     {
ULONG ulRc;   
   ulRc=NspInstallPlugIN( hwndOwnerHandle, pszSourcePath, pszTargetDrive, pszMyParms, hwndMinstallHandle, pszResponseFile);
return ulRc; }

