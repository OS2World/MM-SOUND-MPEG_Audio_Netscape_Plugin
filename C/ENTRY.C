/* ************************************************************** */
/* �l�b�g�P�[�v�E�v���O�C���̍쐬                                 */
/* -------------------------------------------------------------- */
/* �G���g���[�|�C���g(�V�X�e���Ƃ̒ʐM)�v���O����                 */
/* �yENTRY.C�z                                                    */
/* -------------------------------------------------------------- */
/*                   ���̃t�@�C�����A�Ǝ��ɃJ�X�^�����Ă��������B */
/* ************************************************************** */

#include <LUCIER.H>    /* ���ʏ��OS2.H ���ƒu�������Ă������� */
#include <NSPLUG32.H>  /* NSPLUG32 ���C�u�����[ */
#include "PLUGIN.H"

/* ************************************************************** */
/* �v���O�C�����������ɌĂ΂��֐� (�K�{)                        */
/* ************************************************************** */
NPError NP_LOADDS NPP_Initialize(void)     {

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* �v���O�C���I�����ɌĂ΂��֐� (�K�{)                          */
/* ************************************************************** */
void NP_LOADDS NPP_Shutdown(void) {

return; }

/* ************************************************************** */
/* �v���O�C�����������ɌĂ΂��֐� (�K�{)                        */
/* ************************************************************** */
/* �Q�l��� mode=NP_FULL �S��ʃ��[�h                             */
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
   /* NSPLUG32���C�u�����[�̃������[������U��                   */
   /* *********************************************************** */
   pPlugINMemory=NspCreateNewInstance(PLUGIN_DLLNAME,NULL,
            pluginType,instance,mode,argc,argn,argv,saved,&rc);
   if(!pPlugINMemory) return rc;

   /* *********************************************************** */
   /* �����ւ���ɊJ�����[�U�[�Ǝ��̃������[������U�� (�C��)     */
   /*    �v���O�C���͓����ɋN������Ă���ꍇ������܂��A���̏ꍇ */
   /*    �E�C���h�E���Ƃɕۑ����Ȃ���΂Ȃ�Ȃ��f�[�^�[�́A���̂� */
   /*    ���Ɋ���U�����������[���ɂ����Ȃ���΂����܂���B������ */
   /*    �Ȃ��ƁA���݂��Ƀf�[�^�[�������Ⴎ����ɂ��Ă��܂��܂��B */
   /* *********************************************************** */
   pPlugINMemory->pUserMemory=NPN_MemAlloc(sizeof(USERSTRUCT));
   memset(pPlugINMemory->pUserMemory,0,sizeof(USERSTRUCT));

   /* *********************************************************** */
   /* ����ł́A����U�����������[�ɕK�v�ȏ��������s���Ă�������  */
   /* *********************************************************** */

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* �v���O�C���j�󎞂ɌĂ΂��֐� (�K�{)                          */
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
   /* �E�C���h�E�̔j�󓙂̕K�v�ȏI�������������������ōs��     */
   /* ******************************************************** */
   WinSubclassWindow(pPlugINMemory->hwnd, pPlugINMemory->lpfnOldWndProc);
   /* ******************************************************** */
   /* �����p�����[�^�[���̕ۑ����K�v�Ȃ��͂肱���ōs��       */
   /* *save = �ۑ�����f�[�^�[�ւ̃|�C���^                     */
   /* ******************************************************** */

   /* ******************************************************** */
   /* �J�����[�U�[�Ǝ��̃������[�����                         */
   /* ******************************************************** */
   NPN_MemFree(pPlugINMemory->pUserMemory);
   /* ******************************************************** */
   /* NSPLUG32 ���C�u�����[�̃������[����� (Nsp�֐��ő�s��)  */
   /* ******************************************************** */
   NspDestroyInstance(pPlugINMemory);
   instance->pdata=NULL;

return NPERR_NO_ERROR;  }


/* *********************************************************** */
/* �E�C���h�E���\�z����C�x���g                                */
/* *********************************************************** */
NPError NP_LOADDS NPP_SetWindow(NPP instance, NPWindow* window)   {
PPLUGINMEMORY pPlugINMemory;

HAB   hab;  /* �����`! PM�v���O�����A���Ȃ��݂̕ϐ��ł���! */
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
      /* �����A�u���E�U���ɂo�l�E�C���h�E������܂��傤      */
      /*  �������[�ɃV�X�e������̏���ۑ����܂��傤         */
      /* ***************************************************** */
      pPlugINMemory->fWindow = window;
      hwnd = (HWND)pPlugINMemory->fWindow->window;
      hab=WinQueryAnchorBlock(hwnd);
      pPlugINMemory->hwnd=hwnd;

      /* ***************************************************** */
      /* �Â��E�C���h�E��񓙂�ۑ�     (�����͕ύX�̕K�v�Ȃ�) */
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
      /* ���ɍ쐬�ς݁H                                        */
      /* ***************************************************** */

      /* �E�C���h�E�n���h�����ς����? */
      if(hwnd != (HWND)window->window) {
         WinSendMsg(hwnd,WM_NSPLUG32_DESTROY,0,0);
         /* �V�����E�C���h�E��o�^ */
         pPlugINMemory->fWindow = window;
         /* �Â��E�C���h�E�̐��䉺�Ɉ�x�߂� */
         WinSubclassWindow(hwnd, pPlugINMemory->lpfnOldWndProc);
         /* �V�����n���h�������炤 */
         hwnd = (HWND)pPlugINMemory->fWindow->window;
         pPlugINMemory->hwnd=hwnd;
         if(hwnd) {
            /* �ēo�^ */
            pPlugINMemory->lpfnOldWndProc = WinSubclassWindow(hwnd, SubclassWindowProc);
            NspAssociateInstance(hwnd, pPlugINMemory);
         }
         /* ************************************************** */
         /* �����E�C���h�E�n���h���ˑ��̃T�[�r�X�����s���Ȃ�   */
         /* �����ŃT�[�r�X���Ƃ߂čĎn�����Ă�������           */
         /*   ��: WinStartTimer ���̃^�C�}�[�T�[�r�X           */
         /* ************************************************** */

         /* ************************************************** */
         /* �Â� HPS �͔j�󂵂āA�V���� HPS ���\�z���Ă������� */
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
/* �X�g���[���\�z�C�x���g                                         */
/* ************************************************************** */
NPError NP_LOADDS NPP_NewStream(NPP instance, NPMIMEType type,
            NPStream *stream, NPBool seekable, uint16 *stype)  {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

   /* *********************************************************** */
   /* �t�@�C���x�[�X�̏ꍇ�́A *stype=NP_ASFILE �Ƃ��Ă��������B  */
   /* ��������ƁA�l�b�g���[�N�����x�_�E�����[�h���āA�ł܂��  */
   /* �n���Ă���܂��B MIDI�t�@�C���Ȃ�Ă̂́A�t�@�C���x�[�X���� */
   /* �ׂ��ł��ˁB                                              */
   /* *********************************************************** */
   *stype=NP_ASFILE;

return NPERR_NO_ERROR;  }

/* ************************************************************** */
/* �������݃��f�B�H�C�x���g                                       */
/*  �ۑ����Ɉ�C�ɏ�������ł�����T�C�Y���V�X�e���ɋ����Ă�����  */
/*  ���傤�I                                                      */
/* ************************************************************** */
int32 NP_LOADDS   NPP_WriteReady(NPP instance, NPStream *stream)  {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

return STREAMBUFSIZE; }


/* ************************************************************** */
/* �������݃C�x���g                                               */
/*  NP_ASFILE�C�x���g�ŃI�[�v�������t�@�C���ցA�w�肳�ꂽ�T�C�Y�� */
/*  �f�[�^�[���t�@�C���֏�������ł����Ă�������                  */
/* ************************************************************** */
int32 NP_LOADDS   NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer) {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;

return len; }


/* ************************************************************** */
/* �X�g���[���j��C�x���g                                         */
/* ************************************************************** */
NPError NP_LOADDS NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason) {
PPLUGINMEMORY pPlugINMemory;

   if(!instance)  return NPERR_INVALID_INSTANCE_ERROR;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return NPERR_NO_ERROR;

return NPERR_NO_ERROR;  }

// BOOL  LdaPlayEventFunction(PCHAR pWaveBuffer,ULONG ulWaveSize,PULONG pulWrittenBufferSize);

/* ************************************************************** */
/* �f�B�X�N�ɕۑ����ꂽ�t�@�C�������n�����C�x���g               */
/*  �t�@�C������������Ƃ��́A�V�X�e�����t�@�C�����_�E�����[�h��  */
/*  ����A���̃C�x���g���Ăт܂��B �������ANPP_NewStream �֐��ɂ� */
/*  *stype=NP_ASFILE; ���w�肵�Ă����Ă��������B                  */
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
/* �v�����g (�K�{)                                                */
/* ************************************************************** */
void NP_LOADDS NPP_Print(NPP instance, NPPrint* printInfo)  {
PPLUGINMEMORY pPlugINMemory;

   if(!printInfo) return;
   if(!instance)  return;

   pPlugINMemory = (PPLUGINMEMORY) instance->pdata;
   if(!pPlugINMemory)   return; 

   /* *********************************************************** */
   /* �b���ȒP�ɂ��邽�߁A�v�����g���Ȃ�                          */
   /* *********************************************************** */
   printInfo->print.fullPrint.pluginPrinted = FALSE;

return; }


/* ************************************************************** */
/* �n���h���C�x���g (�}�b�L���g�b�V���̂�)                        */
/* ************************************************************** */
int16 NP_LOADDS NPP_HandleEvent(NPP instance, void* event)  {
NPBool eventHandled = FALSE;

   if(!instance)  return eventHandled;

return eventHandled; }

