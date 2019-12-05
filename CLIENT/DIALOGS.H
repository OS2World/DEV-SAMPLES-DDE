/*==============================================================================
**
** Module:     Dialogs.h
**
** Remarks:    This module contains definitions and function prototypes
**             used to process dialog windows.
**
**============================================================================*/
   #ifndef DIALOGS_INCLUDED
   #define DIALOGS_INCLUDED
/*
** Dialog window procedure prototypes:
*/
   MRESULT EXPENTRY DlgAboutWindowProc         ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgInitiateWindowProc      ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgItemWindowProc          ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgOwnerWindowProc         ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgTerminateWindowProc     ( HWND, ULONG, MPARAM, MPARAM );
/*
** Dialog message function prototypes:
*/
   MRESULT APIENTRY DlgAboutWmCommand          ( HWND, ULONG, MPARAM, MPARAM );

   BOOL    APIENTRY DlgInitiateCommandOk       ( HWND );
   MRESULT APIENTRY DlgInitiateWmCommand       ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgInitiateWmInitDlg       ( HWND, ULONG, MPARAM, MPARAM );

   BOOL    APIENTRY DlgItemFixUp               ( HWND );
   MRESULT APIENTRY DlgItemWmInitDlg           ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgItemWmControl           ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgItemWmCommand           ( HWND, ULONG, MPARAM, MPARAM );

   BOOL    APIENTRY DlgTerminateCommandOk      ( HWND );
   MRESULT APIENTRY DlgTerminateWmCommand      ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgTerminateWmInitDlg      ( HWND, ULONG, MPARAM, MPARAM );

   MRESULT APIENTRY DlgShowWindow              ( HWND, ULONG, MPARAM, MPARAM );

   #endif   /* ifndef DIALOGS_INCLUDED    */
