/*==============================================================================
**
** Module:     Dialogs.c
**
** Remarks:    Contains routines for dialog window management.
**
**============================================================================*/
/*
** System specific defines.
*/
   #define  INCL_DOS
   #define  INCL_WIN
/*
** System specific include files.
*/
   #include <os2.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <string.h>
/*
** Application specific include files.
*/
   #include <main.h>
   #include <dde.h>
   #include <dialogs.h>
   #include <resource.h>
   #include <common.h>

/*==============================================================================
** Function:   DlgAboutWindowProc
**
** Usage:      Processes messages dispatched to a "About" dialog window.
**
**============================================================================*/
   MRESULT                          /* Message result                         */
   EXPENTRY      DlgAboutWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   switch (ulMsgId)
   {
      case WM_COMMAND:
         return DlgAboutWmCommand (hwnd, ulMsgId, mp1, mp2);
   }

   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
** Function:   DlgAboutWmCommand
**
** Usage:      Processes a WM_COMMAND message dispatched to the 'About' dialog
**             window.
**
** Remarks:    A WM_COMMAND messages occurs when a control has a significant
**             event to notify to its owner, such as a selection has been made.
**============================================================================*/
   MRESULT
   APIENTRY    DlgAboutWmCommand
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   USHORT      usCmd = SHORT1FROMMP (mp1);

   switch ( usCmd )
   {
      case IDC_ABOUT_CANCEL:
      {
         WinDismissDlg ( hwnd, TRUE );
         return MRFROMSHORT (0);
      }
   }

   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
** Function:   DlgItemWindowProc
**
** Usage:      Processes messages dispatched to a "Item" dialog window.
**
**============================================================================*/
   MRESULT
   EXPENTRY      DlgItemWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   switch (ulMsgId)
   {
      case WM_INITDLG:
         return DlgItemWmInitDlg (hwnd, ulMsgId, mp1, mp2);

      case WM_COMMAND:
         return DlgItemWmCommand (hwnd, ulMsgId, mp1, mp2);

      case WM_CONTROL:
         return DlgItemWmControl (hwnd, ulMsgId, mp1, mp2);
   }

   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
** Function:   DlgItemWmInitDlg
**
** Usage:      Processes a WM_INITDLG message dispatched to a "Item" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY      DlgItemWmInitDlg
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   PDLGITEM    pDlgItem       = (PDLGITEM) PVOIDFROMMP (mp2);
   BOOL        fResult        = WinSetWindowULong
                              ( hwnd
                              , QWL_USER
                              , LONGFROMMP (mp2)
                              );
   HWND        hwndItemData   = WinWindowFromID
                              ( hwnd
                              , IDC_ITEM_DATA
                              );
   SHORT       sItemInserted;
   SWP         swp;
   SWP         swpOwner;

   fResult  = WinSetWindowText
            ( hwnd
            , pDlgItem->szItemName
            );

   if (WinQueryWindowPos (hwnd, &swp)
   &&  WinQueryWindowPos (WinQueryWindow (hwnd, QW_OWNER), &swpOwner))
   {
      swp.x = (swpOwner.x + (swpOwner.cx / 2L)) - (swp.cx / 2L);
      swp.y = (swpOwner.y + (swpOwner.cy / 2L)) - (swp.cy / 2L);

      fResult  = WinSetWindowPos
               ( hwnd
               , HWND_TOP
               , swp.x
               , swp.y
               , swp.cx
               , swp.cy
               , SWP_MOVE
               );
   }

   if (pDlgItem->flType & IS_Menu)
   {
      USHORT cItems        = 0;
      USHORT sItemIndex    = 0;
      USHORT usItemId      = 0;
      USHORT cbItemText    = 0;

      do
      {
         usItemId = (USHORT) WinSendMsg
                  ( (HWND) pDlgItem->hItem
                  , MM_ITEMIDFROMPOSITION
                  , MPFROMSHORT (sItemIndex)
                  , MPVOID
                  );

         if (usItemId != (USHORT) MIT_ERROR)
         {
            cbItemText     = MnuQueryItemText
                           ( (HWND) pDlgItem->hItem
                           , usItemId
                           , szItemText
                           );
            sItemInserted  = (SHORT) WinSendMsg
                           ( hwndItemData
                           , LM_INSERTITEM
                           , MPFROMSHORT (LIT_SORTASCENDING)
                           , MPFROMP (szItemText)
                           );
         }
         sItemIndex++;

      } while (usItemId != (USHORT) MIT_ERROR);
   }
   else if (pDlgItem->flType & IS_Text)
   {
      sItemInserted  = (SHORT) WinSendMsg
                     ( hwndItemData
                     , LM_INSERTITEM
                     , MPFROMSHORT (LIT_SORTASCENDING)
                     , MPFROMP (pDlgItem->hItem)
                     );
   }
   else if (pDlgItem->flType & IS_Time)
   {
         USHORT cbWritten   = 0;
         ULONG  ulMsgTime   = (ULONG) pDlgItem->hItem;
         ULONG  ulHours     =   ulMsgTime / 3600000L;
         ULONG  ulMinutes   = ( ulMsgTime % 3600000L) / 60000L;
         ULONG  ulSeconds   = ((ulMsgTime % 3600000L) % 60000L) / 1000;
         ULONG  ulMS        = ((ulMsgTime % 3600000L) % 60000L) % 1000;
         CHAR   szFormat [] = "%02d:%02d:%02d:%03d";

         cbWritten      = sprintf
                        ( szText
                        , szFormat
                        , ulHours
                        , ulMinutes
                        , ulSeconds
                        , ulMS
                        );
         sItemInserted  = (SHORT) WinSendMsg
                        ( hwndItemData
                        , LM_INSERTITEM
                        , MPFROMSHORT (LIT_SORTASCENDING)
                        , MPFROMP (szText)
                        );
   }

   return MRFROMSHORT (0);
}
/*==============================================================================
** Function:   DlgItemWmControl
**
** Usage:      Processes a WM_CONTROL message dispatched to a "Item" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgItemWmControl
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
  BOOL        fResult;
  PDLGITEM    pDlgItem = (PDLGITEM) WinQueryWindowULong (hwnd, QWL_USER);
  USHORT      usId     = (USHORT) SHORT1FROMMP (mp1);
  USHORT      usCode   = (USHORT) SHORT2FROMMP (mp1);

  return WinDefDlgProc ( hwnd, ulMsgId, mp1, mp2 );
}
/*==============================================================================
** Function:   DlgItemWmCommand
**
** Usage:      Processes a WM_COMMAND message dispatched to a "Item" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgItemWmCommand
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fReturn;
   PDLGITEM    pDlgItem = (PDLGITEM) WinQueryWindowULong (hwnd, QWL_USER);
   USHORT      usCmd = SHORT1FROMMP (mp1);

   switch ( usCmd )
   {
      case IDC_ITEM_POST:
      {
         break;
      }
      case IDC_ITEM_PASTE_LINK:
      {
         break;
      }
      case IDC_ITEM_CANCEL:
      {
         fReturn  = WinDismissDlg   /* Hides the modeless dialog window       */
                  ( hwnd            /* Dialog window to hide                  */
                  , TRUE            /* Dialog successfully dismissed          */
                  );

         break;
      }
   }

   return WinDefDlgProc ( hwnd, ulMsgId, mp1, mp2 );
}
/*==============================================================================
** Function:   DlgTerminateWindowProc
**
** Usage:      Processes messages message dispatched to a "Terminate" dialog
**             window.
**
**============================================================================*/
   MRESULT
   EXPENTRY    DlgTerminateWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
