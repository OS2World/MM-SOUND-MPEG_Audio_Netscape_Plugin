/* ******************************************************************** */
/* MMIOPROC HEADER                                                      */
/* ******************************************************************** */

#define  FOURCC_MMIOPROC   mmioFOURCC('I','F','D','A')
#define  MEDIATYPE         MMIO_MEDIATYPE_WAVEIF
#define  TEXT_MMIOPROC     "Direct Audio WAVE Interface / Under the npmpalwv MMIO Ver 1.0.0"
#define  NAME              "Direct Audio WAVE Interface / Under the npmpalwv"
#define  EXT               "MIF"
#define  APPNAME           "NPMPA"

#define  SUPPORTFLAG       MMIO_CANREADTRANSLATED | MMIO_CANREADUNTRANSLATED 

#define  WAVEIF_STATUS     WAVEIF_STATUS_ENABLE          | \
                           WAVEIF_STATUS_REALDEVICE      | \
                           WAVEIF_STATUS_REALTIME        | \
                           WAVEIF_STATUS_WRITE           | \
                           WAVEIF_STATUS_SYNC            | \
                           WAVEIF_STATUS_AHEAD           | \
                           WAVEIF_STATUS_WAIT_OVERFLOW   | \
                           WAVEIF_STATUS_IDLE_SLEEP      | \
                           WAVEIF_STATUS_NEED_MULTIMEDIA | \
                           WAVEIF_STATUS_NEED_BUFFER     | \
                           WAVEIF_STATUS_NEED_STACK      | \
                           WAVEIF_STATUS_CAN_GETPOSITION | \
                           WAVEIF_STATUS_CAN_GETTIME     | \
                           WAVEIF_STATUS_MEMORYAUTOFREE  | \
                           WAVEIF_STATUS_TM2FLAG

ULONG    EscInsertGetFormatInfo(PMMFORMATINFO pMMFmtInfo);
ULONG    EscInsertGetFormatName(PCHAR pszName, ULONG ulSize);

#define  MAX_DATAS         1000

typedef struct {
   MCI_AMP_OPEN_PARMS      AmpOpenParms;
   MCI_MIX_BUFFER          MixBuffers[100+1];         /* Device buffers     */
   MCI_MIXSETUP_PARMS      MixSetupParms;             /* Mixer parameters     */
   MCI_BUFFER_PARMS        BufferParms;               /* Device buffer parms  */
   MCI_GENERIC_PARMS       GenericParms;

   USHORT                  usMaxBuffers;
   ULONG                   Index;

   PCHAR                   pBuffer[MAX_DATAS+1];
   ULONG                   ulBufferSize[MAX_DATAS+1];

   ULONG                   ulBufferEnable;
   ULONG                   ulBufferSelect;
   ULONG                   ulBufferPos;

   ULONG                   ulMaxBufferSize;
   ULONG                   ulReservedBufferSize;

   LONG                    lServiceFrontWavePos;   /* 先行書き込み済みウエーブサイズ */
   LONG                    lServiceWavePos;        /* 現在時間までに装置に出力された正確なウエーブサイズ */

   BOOL                    fTimeClitical;
   BOOL                    fClear;
}  ESCWAVEIF_LOCAL, *PESCWAVEIF_LOCAL, **PPESCWAVEIF_LOCAL;

