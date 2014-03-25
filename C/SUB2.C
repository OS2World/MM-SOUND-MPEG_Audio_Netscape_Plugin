/* ******************************************************************** */
/* Multimedia I/O Proc ENTRY部 サブプログラム                           */
/*                                  Copyrights(c) 1996-99, Yuuriru Mint */
/* ******************************************************************** */
/* Watcom C/C++ 10.0 英語版 + Warp 4 Toolkit                            */
/* Library: MMPM2.LIB LUCIER.LIB (2.1) LLWAVEEF.LIB                     */
/* ******************************************************************** */
#define INCL_ESC_NO_NETWORK
#define INCL_ESC_NO_WARP4
#include <LUCIER.H>

#include "MMIOPRO2.H"

ULONG    EscInsertGetFormatInfo(PMMFORMATINFO pMMFmtInfo)   {
   if(!pMMFmtInfo)   return MMIO_ERROR;
   memset(pMMFmtInfo,0,sizeof (MMFORMATINFO));
   pMMFmtInfo->ulStructLen  = sizeof(MMFORMATINFO);
   pMMFmtInfo->fccIOProc    = FOURCC_MMIOPROC;
   pMMFmtInfo->ulIOProcType = MMIO_IOPROC_FILEFORMAT;
   pMMFmtInfo->ulMediaType  = MEDIATYPE;
   pMMFmtInfo->ulFlags      = SUPPORTFLAG;
   strcpy((PSZ)pMMFmtInfo->szDefaultFormatExt,EXT);
   pMMFmtInfo->ulCodePage=MMIO_DEFAULT_CODE_PAGE;
   pMMFmtInfo->ulLanguage=MMIO_LC_US_ENGLISH;
   pMMFmtInfo->lNameLength=strlen(NAME)+1;
return MMIO_SUCCESS; }

ULONG    EscInsertGetFormatName(PCHAR pszName, ULONG ulSize)   {
LONG  lRC;
   if(!pszName)   return 0;
   if(strlen(NAME)>=ulSize)   strcpy(pszName,EXT);
   else  strcpy(pszName,NAME);
   lRC=strlen(pszName)+1;
return lRC; }

