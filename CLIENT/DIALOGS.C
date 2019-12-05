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

   CHAR     szItemText  [255]    = "";
   extern   HATOMTBL hAtomTable;

/*==============================================================================
** Function:   DlgAboutWindowProc
**
** Usage:      Processes messages dispatched to a "About" dialog window.
**
**============================================================================*/
   MRESULT                          /* Message result                         */
   EXPENTRY    DlgAboutWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   if (ulMsgId == WM_COMMAND)
   {
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
      case IDC_ABOUT_OK:
      case MBID_CANCEL:
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
   EXPENTRY    DlgItemWindowProc
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
   APIENTRY    DlgItemWmInitDlg
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   SWP         swp;
   SWP         swpOwner;
   PDLGITEM    pDlgItem = PVOIDFROMMP (mp2);

   fResult  = WinSetWindowULong
            ( hwnd
            , QWL_USER
            , LONGFROMMP (mp2)
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

   fResult  = WinCheckButton
            ( hwnd
            , IDC_ITEM_REQUEST
            , TRUE
            );
   fResult  = WinCheckButton
            ( hwnd
            , IDC_ITEM_TEXT
            , TRUE
            );
   fResult  = WinSetDlgItemText
            ( hwnd
            , IDC_ITEM_NAME
            , pDlgItem->szItemName
            );

   pDlgItem->ulMsgId  = IDC_ITEM_REQUEST;
   pDlgItem->usFormat = IDC_ITEM_TEXT;

   return MRFROMSHORT (0);              /* Focus window is not changed        
*/
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
   ULONG       cbText      = 0L;
   BOOL        fResult     = FALSE;
   HWND        hwndControl = HWNDFROMMP (mp1);
   PDLGITEM    pDlgItem    = (PDLGITEM) WinQueryWindowULong (hwnd, QWL_USER);
   USHORT      usId        = (USHORT) SHORT1FROMMP (mp1);
   USHORT      usCode      = (USHORT) SHORT2FROMMP (mp1);

   if (usId >= IDC_ITEM_MESSAGES_FIRST
   &&  usId <= IDC_ITEM_MESSAGES_LAST)
   {
      pDlgItem->ulMsgId = usId;

      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_NONE
               , (usId == IDC_ITEM_UNADVISE)
               ? TRUE
               : FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_CPTEXT
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? FALSE
               : TRUE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_BITMAP
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? FALSE
               : TRUE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_METAFILE
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? FALSE
               : TRUE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_PRIVATE
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? FALSE
               : TRUE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_DATA
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? TRUE
               : FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_TEXT_DATA
               , (usId == IDC_ITEM_POKE || usId == IDC_ITEM_EXECUTE)
               ? TRUE
               : FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_FNODATA
               , (usId == IDC_ITEM_ADVISE)
               ? TRUE
               : FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_FACKREQ
               , (usId == IDC_ITEM_ADVISE)
               ? TRUE
               : FALSE
               );
   }

   if (usId >= IDC_ITEM_FORMATS_FIRST
   &&  usId <= IDC_ITEM_FORMATS_LAST)
   {
      pDlgItem->usFormat = usId;

      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_FORMAT
               , (usId == IDC_ITEM_PRIVATE)
               ? TRUE
               : FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_TEXT_FORMAT
               , (usId == IDC_ITEM_PRIVATE)
               ? TRUE
               : FALSE
               );
   }

   if (usId == IDC_ITEM_POKE
   ||  usId == IDC_ITEM_EXECUTE)
   {
      fResult  = WinCheckButton
               ( hwnd
               , IDC_ITEM_TEXT
               , TRUE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_FORMAT
               , FALSE
               );
      fResult  = WinEnableControl
               ( hwnd
               , IDC_ITEM_TEXT_FORMAT
               , FALSE
               );
      }

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
   USHORT      usCmd = SHORT1FROMMP (mp1);

   switch ( usCmd )                 /* Dialog control ID                      */
   {
      case IDC_ITEM_OK:             /* OK button pressed                      */
      {
         fReturn = DlgItemFixUp (hwnd);
      }
      case IDC_ITEM_TERMINATE:      /* Terminate button pressed               */
      case IDC_ITEM_CANCEL:         /* Cancel button pressed                  */
      {
         fReturn  = WinDismissDlg   /* Hides the modeless dialog window       */
                  ( hwnd            /* Dialog window to hide                  */
                  , (ULONG) usCmd   /* Dialog successfully dismissed          */
                  );

         break;                     /* Break from IDC_ITEM_CANCEL             */
      }
   }

   fReturn  = WinEnableWindow       /* Set physical input queuing state...    */
            ( hwndFrame             /*    of the owner of this dialog,        */
            , TRUE                  /*    to an enabled state.                */
            );

   return WinDefDlgProc ( hwnd, ulMsgId, mp1, mp2 );
}
/*==============================================================================
** Function:   DlgItemFixUp
**
** Usage:      Map user selections to corresponding DDE transaction parameters.
**
**============================================================================*/
   BOOL
   APIENTRY    DlgItemFixUp
(
   HWND        hwnd
)
{
   ULONG       cbItemText  = 0L;
   BOOL        fResult     = FALSE;
   USHORT      flChecked   = 0L;
   PDLGITEM    pDlgItem    = (PDLGITEM) WinQueryWindowULong
                           ( hwnd
                           , QWL_USER
                           );

   pDlgItem->cbData = 0;

   switch (pDlgItem->ulMsgId)
   {
      case IDC_ITEM_REQUEST:
      {
         pDlgItem->ulMsgId = WM_DDE_REQUEST;
         break;
      }
      case IDC_ITEM_ADVISE:
      {
         pDlgItem->ulMsgId = WM_DDE_ADVISE;

         flChecked   = WinQueryButtonCheckstate
                     ( hwnd
                     , IDC_ITEM_FACKREQ
                     );

         if (flChecked)
            pDlgItem->fsStatus |= DDE_FACKREQ;
         else
            pDlgItem->fsStatus &= ~DDE_FACKREQ;

         flChecked   = WinQueryButtonCheckstate
                     ( hwnd
                     , IDC_ITEM_FNODATA
                     );
         if (flChecked)
            pDlgItem->fsStatus |= DDE_FNODATA;
         else
            pDlgItem->fsStatus &= ~DDE_FNODATA;

         break;
      }
      case IDC_ITEM_UNADVISE:
      {
         pDlgItem->ulMsgId = WM_DDE_UNADVISE;
         break;
      }
      case IDC_ITEM_POKE:
      {
         pDlgItem->ulMsgId = WM_DDE_POKE;
         break;
      }
      case IDC_ITEM_EXECUTE:
      {
         pDlgItem->ulMsgId = WM_DDE_EXECUTE;
         break;
      }
   }

   switch (pDlgItem->usFormat)
   {
      case IDC_ITEM_TEXT:
      {
         pDlgItem->usFormat = usFmtText;
         break;
      }
      case IDC_ITEM_CPTEXT:
      {
         pDlgItem->usFormat = usFmtCPText;
         break;
      }
      case IDC_ITEM_BITMAP:
      {
         pDlgItem->usFormat = usFmtBitmap;
         break;
      }
      case IDC_ITEM_METAFILE:
      {
         pDlgItem->usFormat = usFmtMetaFile;
         break;
      }
      case IDC_ITEM_PALETTE:
      {
         pDlgItem->usFormat = usFmtPalette;
         break;
      }
      case IDC_ITEM_PRIVATE:
      {
         cbItemText  = WinQueryDlgItemText
                     ( hwnd
                     , IDC_ITEM_FORMAT
                     , MAX_TEXTLENGTH
                     , szItemText
                     );
         pDlgItem->usFormat   = WinFindAtom
                              ( hAtomTable
                              , szItemText
                              );
         break;
      }
      case IDC_ITEM_NONE:
      {
         pDlgItem->usFormat = 0;
         break;
      }
   }

   pDlgItem->ulOptions = DDEPM_RETRY;

   cbItemText  = WinQueryDlgItemText
               ( hwnd
               , IDC_ITEM_NAME
               , MAX_TEXTLENGTH
               , (PSZ) pDlgItem->szItemName
               );

   if (cbItemText == 0)
   {
      *pDlgItem->szItemName = '\0';
   }

   if (pDlgItem->ulMsgId == WM_DDE_POKE
   ||  pDlgItem->ulMsgId == WM_DDE_EXECUTE)
   {
      pDlgItem->cbData  = WinQueryDlgItemText
                        ( hwnd
                        , IDC_ITEM_DATA
                        , MAX_TEXTLENGTH
                        , (PSZ) pDlgItem->abData
                        );
   }

   return (TRUE);
}
/*==============================================================================
** Function:   DlgInitiateWindowProc
**
** Usage:      Processes messages dispatched to the 'Initiate' dialog window.
**
**============================================================================*/
   MRESULT
   EXPENTRY    DlgInitiateWindowProc
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
         return DlgInitiateWmCommand (hwnd, ulMsgId, mp1, mp2);

      case WM_INITDLG:
         return DlgInitiateWmInitDlg (hwnd, ulMsgId, mp1, mp2);
   }

   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
** Function:   DlgInitiateWmCommand
**
** Usage:      Processes a WM_COMMAND message dispatched to a "Initiate" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgInitiateWmCommand
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   USHORT      usCmd = SHORT1FROMMP (mp1);

   if (usCmd == IDC_INITIATE_OK)
   {
      fResult  = DlgInitiateCommandOk
               ( hwnd
               );
   }

   fResult  = WinDismissDlg
            ( hwnd
            , (ULONG) usCmd
            );

   return MRFROMSHORT (usCmd);
}
/*==============================================================================
** Function:   DlgInitiateCommandOk
**
** Usage:      Process the positive "OK" push button on the "Initiate" dialog
**             window.
**
**============================================================================*/
   BOOL
   APIENTRY    DlgInitiateCommandOk
(
   HWND        hwndDlg
)
{
   BOOL        fResult     = FALSE;
   ULONG       ccAppName   = WinQueryDlgItemText
                           ( hwndDlg
                           , IDC_INITIATE_SERVER
                           , MAX_TEXTLENGTH
                           , szAppName    /* Global variable                  */
                           );
   ULONG       ccTopic     = WinQueryDlgItemText
                           ( hwndDlg
                           , IDC_INITIATE_TOPIC
                           , MAX_TEXTLENGTH
                           , szTopic      /* Global variable                  */
                           );

   return TRUE;                     /* Return TRUE indicating success         */
}
/*==============================================================================
** Function:   DlgInitiateWmCommand
**
** Usage:      Processes a WM_INITDLG message dispatched to a "Initiate" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgInitiateWmInitDlg
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   SHORT       sTextLength;
   SWP         swp;
   SWP         swpOwner;
   CHAR        szItemText [MAX_TEXTLENGTH];

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

   sTextLength = (SHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEMTEXT
               , MPFROM2SHORT (sAppSelected, MAX_TEXTLENGTH - 1)
               , MPFROMP (szItemText)
               );

   if (sTextLength)
   {
      fResult  = WinSetDlgItemText
               ( hwnd
               , IDC_INITIATE_SERVER
               , szItemText
               );
   }

   sTextLength = (SHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEMTEXT
               , MPFROM2SHORT (sTopicSelected, MAX_TEXTLENGTH - 1)
               , MPFROMP (szItemText)
               );

   if (sTextLength)
   {
      fResult  = WinSetDlgItemText
               ( hwnd
               , IDC_INITIATE_TOPIC
               , szItemText
               );
   }

   return MRFROMSHORT (0);          /* Focus window is not changed            */
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
   switch (ulMsgId)
   {
      case WM_COMMAND:
         return DlgTerminateWmCommand (hwnd, ulMsgId, mp1, mp2);

      case WM_INITDLG:
         return DlgTerminateWmInitDlg (hwnd, ulMsgId, mp1, mp2);
   }

   return WinDefDlgProc (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
** Function:   DlgTerminateWmCommand
**
** Usage:      Processes a WM_COMMAND message dispatched to a "Terminate" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgTerminateWmCommand
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   USHORT      usCmd = SHORT1FROMMP (mp1);

   if (usCmd == IDC_TERMINATE_OK)
   {
      fResult  = DlgTerminateCommandOk
               ( hwnd
               );
   }

   fResult  = WinDismissDlg
            ( hwnd
            , (ULONG) usCmd
            );

   return MRFROMSHORT (usCmd);
}
/*==============================================================================
** Function:   DlgTerminateCommandOk
**
** Usage:      Process the positive "OK" push button on the "Terminate" dialog
**             window.
**
**============================================================================*/
   BOOL
   APIENTRY    DlgTerminateCommandOk
(
   HWND        hwndDlg
)
{
   BOOL        fResult     = FALSE;
   ULONG       ccAppName   = WinQueryDlgItemText
                           ( hwndDlg
                           , IDC_TERMINATE_SERVER
                           , MAX_TEXTLENGTH
                           , szAppName
                           );
  ULONG       ccTopic      = WinQueryDlgItemText
                           ( hwndDlg
                           , IDC_TERMINATE_TOPIC
                           , MAX_TEXTLENGTH
                           , szTopic
                           );
   return (TRUE);
}
/*==============================================================================
** Function:   DlgTerminteWmInitDlg
**
** Usage:      Processes a WM_INITDLG message dispatched to a "Terminate" dialog
**             window.
**
**============================================================================*/
   MRESULT
   APIENTRY    DlgTerminateWmInitDlg
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   SHORT       sTextLength;
   SWP         swp;
   SWP         swpOwner;
   CHAR        szItemText [MAX_TEXTLENGTH];

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

   sTextLength = (SHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEMTEXT
               , MPFROM2SHORT (sAppSelected, MAX_TEXTLENGTH - 1)
               , MPFROMP (szItemText)
               );

   if (sTextLength)
   {
      fResult  = WinSetDlgItemText
               ( hwnd
               , IDC_TERMINATE_SERVER
               , szItemText
               );
   }

   sTextLength = (SHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEMTEXT
               , MPFROM2SHORT (sTopicSelected, MAX_TEXTLENGTH - 1)
               , MPFROMP (szItemText)
               );

   if (sTextLength)
   {
      fResult  = WinSetDlgItemText
               ( hwnd
               , IDC_TERMINATE_TOPIC
               , szItemText
               );
   }

   return MRFROMSHORT (0);          /* Focus window is not changed            */
}
