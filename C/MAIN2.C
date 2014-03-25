/* ******************************************************************** */
/* DIRECT AUDIO WAVE INTERFACE                                          */
/* Multimedia I/O Proc                                                  */
/*                                                        Version 1.1.0 */
/*                                  Copyrights(c) 1996-99, Yuuriru Mint */
/* ******************************************************************** */
/* Watcom C/C++ 10.0 ENGLISH + Warp 4 Toolkit                           */
/* Library: MMPM2.LIB LUCIER.LIB (2.1) LLWAVEEF.LIB                     */
/* ******************************************************************** */
#define INCL_ESC_NO_NETWORK
#define INCL_ESC_NO_WARP4
#include <LUCIER.H>

#include "MMIOPRO2.H"

/* ******************************************************************** */
/* DIRECT AUDIO EVENT FUNCTION                                          */
/* ******************************************************************** */
LONG APIENTRY WAVEEVENT(ULONG ulStatus,PMCI_MIX_BUFFER pMciMixBuffer,ULONG ulFlags) {
PWAVEIF           pWaveIF;
PESCWAVEIF_LOCAL  pWaveIFLocal;
ULONG             ulWrittenBufferSize,i;

ULONG             ulCelSize,ulWrite,ulWrittenSize,ulBufferSize;
PCHAR             ptrWrite,ptrRead;

   pWaveIF=(PWAVEIF)pMciMixBuffer->ulUserParm;
   pWaveIFLocal=pWaveIF->pExInfo;

   switch( ulFlags )   {
   /* ***************************************************************** */
   /* HARDWARE ERROR  or  READ COMPLETE EVENT                           */
   /* ***************************************************************** */
   case MIX_STREAM_ERROR | MIX_READ_COMPLETE :
   case MIX_STREAM_ERROR | MIX_WRITE_COMPLETE:
      if ( ulStatus == ERROR_DEVICE_UNDERRUN)      {
         return FALSE;
      }
      break;
   /* ***************************************************************** */
   /* PLAY EVENT FOR NEXT BUFFER                                        */
   /* ***************************************************************** */
   case MIX_WRITE_COMPLETE:
      ulWrittenBufferSize=pWaveIFLocal->BufferParms.ulBufferSize;

      ptrWrite=pWaveIFLocal->MixBuffers[pWaveIFLocal->Index].pBuffer;
      i=pWaveIFLocal->ulBufferSelect;
      ulWrittenSize=0L;
      ulBufferSize=pWaveIFLocal->BufferParms.ulBufferSize;
      while(1) {
         if(pWaveIFLocal->ulBufferSize[i]==0)   break;
         ptrRead=pWaveIFLocal->pBuffer[i];
         ulCelSize=pWaveIFLocal->ulBufferSize[i]-pWaveIFLocal->ulBufferPos;
         if(ulCelSize+ulWrittenSize>=ulBufferSize && !pWaveIFLocal->fClear)   {
            ulWrite=ulBufferSize-ulWrittenSize;
            memcpy(ptrWrite,&ptrRead[pWaveIFLocal->ulBufferPos],ulWrite);
            pWaveIFLocal->ulBufferPos+=ulWrite;
            ptrWrite+=ulWrite;
            ulWrittenSize+=ulWrite;
            pWaveIFLocal->lServiceWavePos+=ulWrite;
            break;
         }  else  {
            JmpLoop:
            if(pWaveIFLocal->fClear)   {
               ulWrite=0;
            }  else {
               ulWrite=ulCelSize;
               memcpy(ptrWrite,&ptrRead[pWaveIFLocal->ulBufferPos],ulWrite);
            }
            pWaveIFLocal->ulBufferPos=0;
            EscFreeMem(ptrRead);
            if(pWaveIFLocal->ulReservedBufferSize<pWaveIFLocal->ulBufferSize[i])   pWaveIFLocal->ulReservedBufferSize=0L;
            else pWaveIFLocal->ulReservedBufferSize-=pWaveIFLocal->ulBufferSize[i];
            pWaveIFLocal->pBuffer[i]=NULL;
            pWaveIFLocal->ulBufferSize[i]=0;

            i++;
            if(i>=MAX_DATAS)   i=0;

            if(pWaveIFLocal->fClear)   {
               if(pWaveIFLocal->ulBufferSize[i])   goto JmpLoop;
               pWaveIFLocal->fClear=FALSE;
            }

            ptrWrite+=ulWrite;
            ulWrittenSize+=ulWrite;
            pWaveIFLocal->lServiceWavePos+=ulWrite;
         }
      }

      if(ulBufferSize-ulWrittenSize)   {
         memset(ptrWrite, 0, ulBufferSize-ulWrittenSize);
      }

      pWaveIFLocal->ulBufferSelect=i;

      pWaveIFLocal->MixBuffers[pWaveIFLocal->Index].ulBufferLength = ulWrittenBufferSize;
      pWaveIFLocal->MixSetupParms.pmixWrite( pWaveIFLocal->MixSetupParms.ulMixHandle, &pWaveIFLocal->MixBuffers[pWaveIFLocal->Index], 1 );
      pWaveIFLocal->Index++;
      if(pWaveIFLocal->Index>=pWaveIFLocal->usMaxBuffers)   pWaveIFLocal->Index=0L;
      break;
   }
return TRUE;   }



/* ******************************************************************** */
/* MAIN COMMUNICATION FUNCTION                                          */
/* ******************************************************************** */
LONG EXPENTRY _export WAVEENTRY(PVOID pVoid,USHORT usMsg,LONG mp1,LONG mp2)  {
ULONG             lRC=MMIO_ERROR;
PWAVEIF           pWaveIF;
PMMIOINFO         pMMInfo;

PESCWAVEIF_LOCAL  pWaveIFLocal;
ULONG             i;
double            dbValue;
LONG              lSeekPos;

   pMMInfo=(PMMIOINFO)pVoid;

   switch(usMsg)  {
   /* ***************************************************************** */
   /* オープン                                                          */
   /* ***************************************************************** */
   case MMIOM_OPEN:
      if(!pMMInfo ) return MMIO_ERROR;
      pWaveIF=EscAllocMem(sizeof(WAVEIF));   
      if(!pWaveIF)   {
         pMMInfo->pExtraInfoStruct=NULL;
         return MMIO_ERROR;
      }
      memset(pWaveIF,0,sizeof(WAVEIF));
      pWaveIF->cbSize=sizeof(WAVEIF);
      pMMInfo->pExtraInfoStruct=(PVOID)pWaveIF;
      pWaveIFLocal=EscAllocMem(sizeof(ESCWAVEIF_LOCAL));
      if(!pWaveIFLocal)   {
         EscFreeMem(pWaveIF);
         pMMInfo->pExtraInfoStruct=NULL;
         return MMIO_ERROR;
      }
      memset(pWaveIFLocal, 0, sizeof(ESCWAVEIF_LOCAL));
      pWaveIF->pExInfo=pWaveIFLocal;
      pWaveIFLocal->ulReservedBufferSize=0L;
      pWaveIFLocal->lServiceFrontWavePos=0L;
      pWaveIFLocal->lServiceWavePos=0L;

      pWaveIFLocal->ulMaxBufferSize=PrfQueryProfileInt(HINI_USER, "LLWAVEIF", "BUFFER", 4000L); /* 省略時4MB */
      if(pWaveIFLocal->ulMaxBufferSize<100)  pWaveIFLocal->ulMaxBufferSize=100;
      /* 極小バッファ */
      if(mp2&WAVEIF_OPEN_CELBUFFER) {
         pWaveIFLocal->ulMaxBufferSize=200;
      }
      pWaveIFLocal->ulMaxBufferSize*=1000L;
      pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.ulSamplesPerSec=(mp2&WAVEIF_OPEN_SPS_MASK)>>WAVEIF_OPEN_SPS_SHIFT;
      pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usBitsPerSample=(mp2&WAVEIF_OPEN_BIT_MASK)>>WAVEIF_OPEN_BIT_SHIFT;
      pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usChannels=(mp2&WAVEIF_OPEN_CH_MASK)>>WAVEIF_OPEN_CH_SHIFT;
      ClcWaveInfoFromBasicInfo(&pWaveIF->mmAudioHdr, 0);

      pWaveIFLocal->usMaxBuffers=2;
      memset ( &pWaveIFLocal->AmpOpenParms, '\0', sizeof ( MCI_AMP_OPEN_PARMS ) );
      pWaveIFLocal->AmpOpenParms.usDeviceID = ( USHORT ) 0;
      pWaveIFLocal->AmpOpenParms.pszDeviceType = ( PSZ ) MCI_DEVTYPE_AUDIO_AMPMIX;
      if(mciSendCommand( 0, MCI_OPEN, MCI_WAIT|MCI_OPEN_TYPE_ID|MCI_OPEN_SHAREABLE,(PVOID)&pWaveIFLocal->AmpOpenParms, 0)) {
         EscFreeMem(pWaveIFLocal);
         EscFreeMem(pWaveIF);
         pMMInfo->pExtraInfoStruct=NULL;
         return 7;
      }
      memset( &pWaveIFLocal->MixSetupParms, '\0', sizeof( MCI_MIXSETUP_PARMS ) );
      pWaveIFLocal->MixSetupParms.ulDeviceType = MCI_DEVTYPE_WAVEFORM_AUDIO;
      pWaveIFLocal->MixSetupParms.ulFormatTag  = 1L;
      pWaveIFLocal->MixSetupParms.ulSamplesPerSec = pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.ulSamplesPerSec;
      pWaveIFLocal->MixSetupParms.ulBitsPerSample = pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usBitsPerSample;
      pWaveIFLocal->MixSetupParms.ulChannels      = pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usChannels;
      pWaveIFLocal->MixSetupParms.pmixEvent    = WAVEEVENT;
      pWaveIFLocal->MixSetupParms.ulFormatMode = MCI_PLAY;
      if(mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_MIXSETUP, MCI_WAIT|MCI_MIXSETUP_INIT,(PVOID)&pWaveIFLocal->MixSetupParms,0))   {
         EscFreeMem(pWaveIFLocal);
         EscFreeMem(pWaveIF);
         pMMInfo->pExtraInfoStruct=NULL;
         return 8;
      }
      pWaveIFLocal->BufferParms.ulNumBuffers = pWaveIFLocal->usMaxBuffers+2;
      pWaveIFLocal->BufferParms.ulBufferSize = pWaveIFLocal->MixSetupParms.ulBufferSize;
      pWaveIFLocal->BufferParms.pBufList = pWaveIFLocal->MixBuffers;
      if(mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_BUFFER, MCI_WAIT|MCI_ALLOCATE_MEMORY,(PVOID)&pWaveIFLocal->BufferParms, 0)) {
         EscFreeMem(pWaveIFLocal);
         EscFreeMem(pWaveIF);
         pMMInfo->pExtraInfoStruct=NULL;
         return 9;
      }
      pWaveIFLocal->Index=0L;

      for(i=0;i<MAX_DATAS;i++)   {
         pWaveIFLocal->pBuffer[i]=NULL;
         pWaveIFLocal->ulBufferSize[i]=0L;
      }
      pWaveIFLocal->ulBufferEnable=0L;
      pWaveIFLocal->ulBufferSelect=0L;
      pWaveIFLocal->ulBufferPos=0L;

      for(i=0L;i<pWaveIFLocal->usMaxBuffers;i++)   {
         pWaveIFLocal->MixBuffers[i].ulUserParm=(ULONG)pWaveIF;
         memset( pWaveIFLocal->MixBuffers[i].pBuffer, 0 , pWaveIFLocal->BufferParms.ulBufferSize );
         pWaveIFLocal->MixBuffers[i].ulBufferLength = pWaveIFLocal->BufferParms.ulBufferSize;
         pWaveIFLocal->lServiceWavePos-=pWaveIFLocal->MixBuffers[i].ulBufferLength;

      }
      pWaveIFLocal->MixBuffers[pWaveIFLocal->BufferParms.ulNumBuffers-1].ulFlags = MIX_BUFFER_EOS;
      pWaveIFLocal->MixSetupParms.pmixWrite( pWaveIFLocal->MixSetupParms.ulMixHandle, pWaveIFLocal->MixBuffers, pWaveIFLocal->usMaxBuffers );

      lRC=MMIO_SUCCESS;
      break;
   /* ***************************************************************** */
   /* クローズ                                                          */
   /* ***************************************************************** */
   case MMIOM_CLOSE:
      if(!pMMInfo || !pMMInfo->pExtraInfoStruct) return MMIO_ERROR;
      pWaveIF=pMMInfo->pExtraInfoStruct;

      if(pWaveIF) {
         pWaveIFLocal=pWaveIF->pExInfo;
         if(pWaveIFLocal) {
            mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_STOP,MCI_WAIT, (PVOID)&pWaveIFLocal->GenericParms, 0);
            mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_MIXSETUP, MCI_WAIT | MCI_MIXSETUP_DEINIT, (PVOID)&pWaveIFLocal->MixSetupParms, 0);
            pWaveIFLocal->MixSetupParms.ulFormatMode = MCI_PLAY;
            mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_MIXSETUP, MCI_WAIT|MCI_MIXSETUP_INIT, (PVOID)&pWaveIFLocal->MixSetupParms, 0);
            mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_BUFFER,MCI_WAIT|MCI_DEALLOCATE_MEMORY, (PVOID)&pWaveIFLocal->BufferParms, 0);
            mciSendCommand( pWaveIFLocal->AmpOpenParms.usDeviceID, MCI_CLOSE,MCI_WAIT, (PVOID)&pWaveIFLocal->GenericParms, 0);
            for(i=0;i<MAX_DATAS;i++)   {
               if(pWaveIFLocal->ulBufferSize[i])   {
                  EscFreeMem(pWaveIFLocal->pBuffer[i]);
                  pWaveIFLocal->pBuffer[i]=NULL;
                  pWaveIFLocal->ulBufferSize[i]=0L;
               }
            }
            EscFreeMem(pWaveIFLocal);
         }
         memset(pWaveIF,0,sizeof(WAVEIF));
         EscFreeMem(pWaveIF);
         pMMInfo->pExtraInfoStruct=NULL;
      }
      lRC=MMIO_SUCCESS;
      break;
   /* ***************************************************************** */
   /* CLEAR                                                             */
   /* ***************************************************************** */
   case WAVEIF_CLEAR:
      if(!pMMInfo || !pMMInfo->pExtraInfoStruct) return 0;
      pWaveIF=pMMInfo->pExtraInfoStruct;
      pWaveIFLocal=pWaveIF->pExInfo;
      if(!pWaveIFLocal)   return 0L;
      pWaveIFLocal->fClear=TRUE;

      i=0;
      while(pWaveIFLocal->fClear&&i<40)   {
         DosSleep(100);
         i++;
      }
      
      pWaveIFLocal->fClear=FALSE;
      break;
   /* ***************************************************************** */
   /* GET STATUS                                                        */
   /* ***************************************************************** */
   case WAVEIF_GETSTATUS:
      switch(mp1&0x0000FFFFL) {
      case WAVEIF_STATUS_SELECT_STANDARD:
         lRC=WAVEIF_STATUS;
      }
      break;
   /* ***************************************************************** */
   /* このサービスのフォーマット情報を返す                              */
   /* ***************************************************************** */
   case MMIOM_GETFORMATINFO:
      lRC=EscInsertGetFormatInfo((PMMFORMATINFO)mp1);
      break;
   /* ***************************************************************** */
   /* このサービスのフォーマット名を返す                                */
   /* ***************************************************************** */
   case MMIOM_GETFORMATNAME:
      lRC=EscInsertGetFormatName((PCHAR)mp1,mp2);
      break;
   /* ***************************************************************** */
   /* ヘッダを読みとる                                                  */
   /* ***************************************************************** */
   case MMIOM_GETHEADER:
      lRC=0L;
      break;
   /* ***************************************************************** */
   /* ヘッダを書き込む                                                  */
   /* ***************************************************************** */
   case MMIOM_SETHEADER:
      lRC=0L;
      break;
   /* ***************************************************************** */
   /* ヘッダサイズを返す                                                */
   /* ***************************************************************** */
   case MMIOM_QUERYHEADERLENGTH:
      lRC=sizeof(WAVEIF);
      break;
   /* ***************************************************************** */
   /* 位置収得(転送されたデーターは、すべてメモリ上にあるので収得可能)  */
   /* ***************************************************************** */
   case WAVEIF_GETPOSITION:
      if(!pMMInfo || !pMMInfo->pExtraInfoStruct) return 0;
      pWaveIF=pMMInfo->pExtraInfoStruct;
      pWaveIFLocal=pWaveIF->pExInfo;
      if(!pWaveIFLocal)   return 0L;

      switch(mp2) {
      case SEEK_CUR:
         lSeekPos=pWaveIFLocal->lServiceWavePos+mp2;
         break;
      default:
      case SEEK_SET:
         lSeekPos=mp2;
         break;
      case SEEK_END:
         lSeekPos=pWaveIFLocal->lServiceFrontWavePos-mp2;
         break;
      }
      lRC=lSeekPos;
      break;
   /* ***************************************************************** */
   /* 時間位置収得                                                      */
   /* ***************************************************************** */
   case WAVEIF_GETTIME:
      if(!pMMInfo || !pMMInfo->pExtraInfoStruct) return 0;
      pWaveIF=pMMInfo->pExtraInfoStruct;
      pWaveIFLocal=pWaveIF->pExInfo;
      if(!pWaveIFLocal)   return 0L;

      switch(mp2) {
      case SEEK_CUR:
         lSeekPos=pWaveIFLocal->lServiceWavePos+mp1;
         break;
      default:
      case SEEK_SET:
         lSeekPos=mp1;
         break;
      case SEEK_END:
         lSeekPos=pWaveIFLocal->lServiceFrontWavePos-mp1;
         break;
      }
      if(lSeekPos<0) lSeekPos=0;
      dbValue=(double)
         pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.ulSamplesPerSec*
         pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usBitsPerSample*
         pWaveIF->mmAudioHdr.mmXWAVHeader.WAVEHeader.usChannels/(double)8000;
      if(dbValue<1)  return 0L;
      lSeekPos/=dbValue;

      lRC=lSeekPos;
      break;
   /* ***************************************************************** */
   /* 書き込み(アプリ側からみて)                                        */
   /* ***************************************************************** */
   case MMIOM_WRITE:
      if(!pMMInfo || !pMMInfo->pExtraInfoStruct) return MMIO_ERROR;
      pWaveIF=pMMInfo->pExtraInfoStruct;
      if(!mp1 || !mp2)  return 0L;
      pWaveIFLocal=pWaveIF->pExInfo;
      if(pWaveIFLocal->ulMaxBufferSize<pWaveIFLocal->ulReservedBufferSize) {
         return 0L;
      }
      if(pWaveIFLocal->fClear)   return 0L;

      i=pWaveIFLocal->ulBufferEnable;
      if(i+1>=MAX_DATAS)   {
         if(0==pWaveIFLocal->ulBufferSelect)  {
            return 0L;
         }
      }  else  {
         if(i+1==pWaveIFLocal->ulBufferSelect)  {
            return 0L;
         }
      }
      pWaveIFLocal->pBuffer[i]=(PCHAR)mp1;
      pWaveIFLocal->ulBufferSize[i]=(ULONG)mp2;
      pWaveIFLocal->lServiceFrontWavePos+=pWaveIFLocal->ulBufferSize[i];
      pWaveIFLocal->ulReservedBufferSize+=pWaveIFLocal->ulBufferSize[i];
      i++;
      if(i>=MAX_DATAS)   i=0;
      pWaveIFLocal->ulBufferEnable=i;

      lRC=mp2;
      break;
   /* ***************************************************************** */
   /* その他                                                            */
   /* ***************************************************************** */
   default: 
      lRC=MMIOERR_UNSUPPORTED_MESSAGE;
      break;
   }  /* end of switch */
return lRC; }
       
