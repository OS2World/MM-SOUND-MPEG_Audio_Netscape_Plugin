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
/* �}���`���f�B�A�E�C���X�g�[�����ɌĂ΂��T�[�r�X�֐�           */
/* ************************************************************** */
NPError NP_LOADDS _export NP_MMInstall( HWND hwndOwnerHandle, PSZ pszSourcePath, PSZ pszTargetDrive,   PSZ pszMyParms, HWND hwndMinstallHandle, PSZ pszResponseFile )     {
ULONG ulRc;   
   ulRc=NspInstallPlugIN( hwndOwnerHandle, pszSourcePath, pszTargetDrive, pszMyParms, hwndMinstallHandle, pszResponseFile);
return ulRc; }
