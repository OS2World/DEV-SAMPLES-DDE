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
** Local function prototypes
*/
   MRESULT EXPENTRY DlgAboutWindowProc         ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgItemWindowProc          ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT EXPENTRY DlgTerminateWindowProc     ( HWND, ULONG, MPARAM, MPARAM );

   MRESULT APIENTRY DlgAboutWmCommand          ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgItemWmInitDlg           ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgItemWmControl           ( HWND, ULONG, MPARAM, MPARAM );
   MRESULT APIENTRY DlgItemWmCommand           ( HWND, ULONG, MPARAM, MPARAM );


   #endif   /* ifndef DIALOGS_INCLUDED    */
