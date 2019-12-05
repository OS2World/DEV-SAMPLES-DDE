/*==============================================================================
**
** Module:     Frame.h
**
** Remarks:    This module contains definitions used to implement common
**             'Frame' window behaviour.
**
**============================================================================*/
   #ifndef FRAME_INCLUDED
   #define FRAME_INCLUDED

   #define FID_ListBox     0xFFFF
   #define FID_Status      0xFFFE
/*
** Function prototypes.
*/
   SHORT   APIENTRY FrmInsertListboxItem (HWND, ULONG, ULONG, ULONG);
   BOOL    APIENTRY FrmPurgeListboxItems (HWND);
   BOOL    APIENTRY FrmSetDialogItems (ULONG);
   BOOL    APIENTRY FrmSetStatusText (HAB, HWND, PSZ, USHORT, USHORT);
   BOOL    APIENTRY FrmResetDialogItems ();
   BOOL    APIENTRY FrmResetScrollBar ();
   MRESULT APIENTRY FrmWmInitMenu (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY FrmWmFormatFrame (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY FrmWmQueryFrameCtlCount (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY FrmWmQueryTrackInfo (HWND, ULONG, MPARAM, MPARAM);

#endif
