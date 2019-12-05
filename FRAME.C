/*==============================================================================
**
** Module:     Frame.c
**
** Remarks:    This module contains common 'Frame' window functionality.
**
**============================================================================*/
/*
** System specific defines:
*/
   #define     INCL_DOS
   #define     INCL_GPI
   #define     INCL_WIN
/*
** System specific include files:
*/
   #include    <os2.h>
   #include    <stdio.h>
   #include    <string.h>
/*
** Application specific include files:
*/
   #include    <main.h>
   #include    <common.h>
   #include    <frame.h>
   #include    <resource.h>

   #define     MAX_BUFFER_SIZE   128

   PSZ         apszDdeMessage [] =
                                 { "WM_DDE_INITIATE"
                                 , "WM_DDE_REQUEST"
                                 , "WM_DDE_ACK"
                                 , "WM_DDE_DATA"
                                 , "WM_DDE_ADVISE"
                                 , "WM_DDE_UNADVISE"
                                 , "WM_DDE_POKE"
                                 , "WM_DDE_EXECUTE"
                                 , "WM_DDE_TERMINATE"
                                 , "WM_DDE_INITIATEACK"
                                 };
   CHAR        szFormatString [] = "%02d:%02d:%02d:%03d  %s";

/*==============================================================================
**
** Function:   FrmInsertListboxItem
**
** Usage:      Inserts an Item into the Listbox control associated with a
**             Frame window.
**
** Remarks:    The example applications insert additional controls into a
**             "WC_FRAME" class window.  This function is used to insert items
**             into this control.
**
**============================================================================*/
   SHORT                            /* Item Index: LIT_ERROR, LIT_MEMERROR    */
   APIENTRY    FrmInsertListboxItem
(
   HWND        hwndListBox          /* handle of listbox control window       */
,  ULONG       ulMsgId              /* DDE message identifier being inserted  */
,  ULONG       ulMsgTime            /* time message was received              */
,  ULONG       ulHandle             /* associated item handle                 */
)
{
   APIRET      ApiRet;
   BOOL        fResult    = FALSE;
   USHORT      cbWritten  = 0;
   SHORT       sItemIndex = LIT_ERROR;
   CHAR        szBuffer   [MAX_BUFFER_SIZE];
/*
** Break down the message time into Hours, Minutes, Seconds and Milliseconds...
*/
   ULONG       ulHours    =   ulMsgTime / 3600000L;
   ULONG       ulMinutes  = ( ulMsgTime % 3600000L) / 60000L;
   ULONG       ulSeconds  = ((ulMsgTime % 3600000L) % 60000L) / 1000;
   ULONG       ulMS       = ((ulMsgTime % 3600000L) % 60000L) % 1000;
/*
** Check that the message being inserted are DDE messages...
*/
   if (ulMsgId >= WM_DDE_FIRST
   &&  ulMsgId <= WM_DDE_LAST)
   {
/*
**    Format information into a printable string...
*/
      cbWritten   = sprintf
                  ( szBuffer
                  , szFormatString
                  , ulHours
                  , ulMinutes
                  , ulSeconds
                  , ulMS
                  , apszDdeMessage [ulMsgId - WM_DDE_FIRST]
                  );
/*
**    Insert the formatted string into the Listbox...
*/
      sItemIndex  = (USHORT) WinSendMsg
                  ( hwndListBox
                  , LM_INSERTITEM
                  , MPFROMSHORT (LIT_END)
                  , MPFROMP (szBuffer)
                  );
/*
**    If the Item was successfully inserted...
*/
      if (sItemIndex != LIT_MEMERROR && sItemIndex != LIT_ERROR)
      {
/*
**       If the item handle specified is associated with a WM_DDE_INITIATE
**       or WM_DDE_INITIATEACK message, then the handle is a pointer to a
**       DDEINIT structure rather than a DDESTRUCT structure.  Mark the handle
**       as such.  Later when the sample 'Client' windows paint, it can use
**       this indicator to identify which structure it is processing...
*/
         if (ulMsgId == WM_DDE_INITIATE || ulMsgId == WM_DDE_INITIATEACK)
         {
            ulHandle |= FDDEINIT;
         }
/*
**       Associate the handle with the Listbox entry...
*/
         fResult  = (USHORT) WinSendMsg
                  ( hwndListBox
                  , LM_SETITEMHANDLE
                  , MPFROMSHORT (sItemIndex)
                  , MPFROMLONG  (ulHandle)
                  );
      }
/*
**    If the user has specified that the application should automatically
**    update its window (ie, selecting 'Auto' from the 'Display' menu), then
**    select the current item in the Listbox.  This will cause the Listbox
**    to notify its owner (the 'Client' window) with a WM_CONTROL message and
**    the 'Client' window will repaint...
*/
      if (WinIsMenuItemChecked (hwndMenuDisplay, IDM_DISPLAY_AUTO))
      {
         fResult  = (BOOL) WinSendMsg
                  ( hwndListBox
                  , LM_SELECTITEM
                  , MPFROMSHORT (sItemIndex)
                  , MPFROMSHORT (TRUE)
                  );
      }
/*
**    If the insertion of the Listbox item exceeds the maximum number of items
**    desired...
**
**    NOTE: Since we keep the associated DDEINIT and/or DDESTRUCT around for
**          subsequent retrieval, a significant amount of memory can be
**          consumed.  In order to minimize this impact, the example programs
**          implement a maximum number of entries a Listbox will hold.
*/
      if (sItemIndex > MAX_ITEMS)
      {
/*
**       Obtain the handle associated with the oldest (first) Listbox entry...
*/
         ULONG ulItemHandle   = (ULONG) WinSendMsg
                              ( hwndListBox
                              , LM_QUERYITEMHANDLE
                              , MPFROMSHORT (0)
                              , MPVOID
                              );
/*
**       Delete the entry from the Listbox...
*/
         SHORT sItemsLeft     = (USHORT) WinSendMsg
                              ( hwndListBox
                              , LM_DELETEITEM
                              , MPFROMSHORT (0)
                              , MPFROMSHORT (0)
                              );
/*
**       If a memory object was associated with the entry...
*/
         if (ulItemHandle)
         {
/*
**          Mask off the special indicator bit...
*/
            ulItemHandle &= ~FDDEINIT;
/*
**          Free the associate memory object...
*/
            ApiRet   = DosFreeMem
                     ( (PVOID)ulItemHandle
                     );
         }
      }
   }
/*
** Return the index of the new Listbox entry...
*/
   return (sItemIndex);
}
/*==============================================================================
**
** Function:   FrmPurgeListboxItems
**
** Usage:      Deletes all entries in a Listbox control associated with a
**             Frame window.
**
** Remarks:    Memory objects are associated with each Listbox entry.  These
**             memory objects must be Freed when an entry is deleted.
**
**============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    FrmPurgeListboxItems
(
   HWND        hwndListBox
)
{
   APIRET      ApiRet;
   LONG        cItems;
   BOOL        fResult    = TRUE;
   ULONG       ulItemHandle;
   SHORT       sItemIndex = 0;
/*
** Obtain the current number of entries in the Listbox...
*/
   cItems   = (ULONG) WinSendMsg
            ( hwndListBox
            , LM_QUERYITEMCOUNT
            , MPVOID
            , MPVOID
            );
/*
** If Listbox is not empty...
*/
   if (cItems)
   {
      while (--cItems)
      {
/*
**       Obtain the handle associated with the oldest (first) Listbox entry...
*/
         ulItemHandle   = (ULONG) WinSendMsg
                        ( hwndListBox
                        , LM_QUERYITEMHANDLE
                        , MPFROMSHORT (cItems)
                        , MPVOID
                        );
/*
**       If a memory object was associated with the entry...
*/
         if (ulItemHandle)
         {
/*
**          Mask off the special indicator bit...
*/
            ulItemHandle &= ~FDDEINIT;
/*
**          Free the associate memory object...
*/
            ApiRet   = DosFreeMem
                     ( (PVOID)ulItemHandle
                     );
         }
      }
/*
**    Delete the entry from the Listbox...
*/
      fResult  = (ULONG) WinSendMsg
               ( hwndListBox
               , LM_DELETEALL
               , MPVOID
               , MPVOID
               );

   }
/*
** Return result of deletion...
*/
   return (fResult);
}
/*==============================================================================
**
** Function:   FrmWmFormatFrame
**
** Usage:      Processes a "WM_FORMATFRAME" message dispatched to "WC_FRAME" a
**             class window.
**
** Remarks:    A "WM_FORMATFRAME" message is sent to a 'Frame' window when its
**             size changes or it is updated.  Additional controls can be added
**             during the processing of this message.
**
==============================================================================*/
   MRESULT                          /* Number of controls                     */
   APIENTRY    FrmWmFormatFrame
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* message identifier                     */
,  MPARAM      mp1                  /* pointer to an array of SWP structures  */
,  MPARAM      mp2                  /* pointer to RECTL structure or NULL     */
)
{
   BOOL        fResult;
/*
** First, pass this message to the original frame window procedure to get
** the current information (count and SWP's)...
*/
   USHORT      cswp        = (USHORT) (*pfnOldFrameWindowProc)
                           ( hwnd
                           , ulMsgId
                           , mp1
                           , mp2
                           );
   PSWP        paswp       = (PSWP) PVOIDFROMMP (mp1);
   ULONG       cxDlgBox;
   ULONG       cyDlgBox;
   USHORT      iClient     = cswp - 1;
   USHORT      iHorzScroll = cswp - 2;
   USHORT      iVertScroll = cswp - 3;
   USHORT      iListBox    = cswp;
   USHORT      iDlgBox     = cswp + 1;
   USHORT      iStatus     = cswp + 2;
/*
** Obtain handle of Listbox window...
*/
   HWND        hwndListBox = WinWindowFromID
                           ( hwnd
                           , FID_ListBox
                           );
/*
** Obtain handle of status window...
*/
   HWND        hwndStatus  = WinWindowFromID
                           ( hwnd
                           , FID_Status
                           );
/*
** Obtain handle of dialog window...
*/
   HWND        hwndDlgBox  = WinWindowFromID
                           ( hwnd
                           , IDD_STRUCT
                           );
   RECTL       rclDlgBox;
/*
** Obtain the dimensions of the dialog window...  The dialog window forms the
** basis in which all other controls are positioned.
*/
   fResult  = WinQueryWindowRect
            ( hwndDlgBox
            , &rclDlgBox
            );
/*
** Obtain height and width of dialog window...
*/
  cxDlgBox = rclDlgBox.xRight - rclDlgBox.xLeft;
  cyDlgBox = rclDlgBox.yTop   - rclDlgBox.yBottom;
/*
** Initialize the Dialog, Listbox and Status SWP structures with the
** contents associated with the 'Client' window...
*/
   paswp [iDlgBox]   = paswp [iClient];
   paswp [iListBox]  = paswp [iClient];
   paswp [iStatus]   = paswp [iClient];
/*
** Adjust Status window dimensions...
*/
   paswp [iStatus].y    = paswp [iHorzScroll].y;
   paswp [iStatus].cy   = cyMenu;
   paswp [iStatus].cx  += paswp [iVertScroll].cx;
   paswp [iStatus].hwnd = hwndStatus;
   paswp [iStatus].ulReserved1 = 0L;
   paswp [iStatus].ulReserved2 = 0L;
/*
** Adjust Client window dimensions...
*/
   paswp [iClient].y    += cyMenu;
   paswp [iClient].cy   -= cyMenu;
   paswp [iClient].cx   -= cxDlgBox;
   paswp [iClient].hwndInsertBehind = hwndStatus;
/*
** Adjust Horizontal Scroll Bar window dimensions...
*/
   paswp [iHorzScroll].y  += cyMenu;
   paswp [iHorzScroll].cx -= cxDlgBox;
/*
** Adjust Vertical Scroll Bar window dimensions...
*/
   paswp [iVertScroll].x  -= cxDlgBox;
   paswp [iVertScroll].y  += cyMenu;
   paswp [iVertScroll].cy -= cyMenu;
/*
** Adjust Dialog window dimensions...
*/
   paswp [iDlgBox].y    = paswp [iHorzScroll].y;
   paswp [iDlgBox].x    = paswp [iVertScroll].x
                        + paswp [iVertScroll].cx;
   paswp [iDlgBox].cy   = cyDlgBox;
   paswp [iDlgBox].cx   = cxDlgBox;
   paswp [iDlgBox].hwnd = hwndDlgBox;
   paswp [iDlgBox].hwndInsertBehind = hwndStatus;
   paswp [iDlgBox].ulReserved1 = 0L;
   paswp [iDlgBox].ulReserved2 = 0L;
/*
** Adjust Listbox window dimensions...
*/
   paswp [iListBox].y   = paswp [iDlgBox].y + cyDlgBox;
   paswp [iListBox].x   = paswp [iDlgBox].x;
   paswp [iListBox].cx  = cxDlgBox;
   paswp [iListBox].cy  = paswp [iVertScroll].cy
                        + paswp [iHorzScroll].cy
                        - cyDlgBox;
   paswp [iListBox].hwnd = hwndListBox;
   paswp [iListBox].hwndInsertBehind = hwndDlgBox;
   paswp [iListBox].ulReserved1 = 0L;
   paswp [iListBox].ulReserved2 = 0L;
/*
** Return the total number of controls...
*/
   return MRFROMSHORT (cswp + 3);
}
/*==============================================================================
**
** Function:   FrmWmQueryFrameCtlCount
**
** Usage:      Processes a "WM_QUERYFORMATCTLCOUNT" message dispatched to
**             a "WC_FRAME" class window.
**
** Remarks:    A "WM_QUERYFORMATCTLCOUNT" message is sent to a 'Frame' window
**             when its size changes or it is updated.  It is used to determine
**             how many controls the frame contains and allocates memory large
**             enough to hold and array of SWP structures (one for each
**             control).
**
==============================================================================*/
   MRESULT                          /* number of controls supported           */
   APIENTRY    FrmWmQueryFrameCtlCount
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* message identifier                     */
,  MPARAM      mp1                  /* reserved                               */
,  MPARAM      mp2                  /* reserved                               */
)
{
   USHORT      usCtlCount;          /* current control count                  */
/*
** First, pass this message on to the original frame window procedure to
** obtain the standard count of controls supported...
*/
   usCtlCount = (USHORT) (*pfnOldFrameWindowProc) (hwnd, ulMsgId, mp1, mp2);
/*
** Return the standard count plus the number of controls we are adding...
*/
   return MRFROMSHORT (usCtlCount + 3);
}
/*==============================================================================
**
** Function:   FrmWmQueryTrackInfo
**
** Usage:      Processes a "WM_QUERYTRACKINFO" message dispatched to "WC_FRAME"
**             a class window.
**
** Remarks:    A "WM_QUERYTRACKINFO" message is sent to a 'Frame' window when
**             its size changes or it is updated.  Additional controls can be
**             added during the processing of this message.
**
==============================================================================*/
   MRESULT                          /* fResult: Continue indicator            */
   APIENTRY    FrmWmQueryTrackInfo
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* message identifier                     */
,  MPARAM      mp1                  /* tracking flags                         */
,  MPARAM      mp2                  /* pointer to TRACKINFO structure         */
)
{
   BOOL        fResult;
   PTRACKINFO  pti = (PTRACKINFO) PVOIDFROMMP (mp2);
/*
** Call the old Frame window procedure to fill in the default values of the
** TRACKINFO structure.
*/
   fResult = (BOOL) (*pfnOldFrameWindowProc) (hwnd, ulMsgId, mp1, mp2);
/*
** Since we know that our frame has a sizing border, if cxBorder and cyBorder
** come back as non-zero values the default frame procedure has determined
** that no tracking rectangle is required. For example, we are minimized.
** Also, ptlMinTrackSize.x and ptlMinTrackSize.y include the size of the
** sizing border; this is done as a result of the Frame procedure sending a
** WM_QUERYBORDERSIZE during the processing of this message.
*/
   if (pti->cxBorder && pti->cyBorder)
   {
      pti->ptlMinTrackSize.x = cxMinTrackSize;
      pti->ptlMinTrackSize.y = cyMinTrackSize;
   }
/*
** Return indicating that tracking should continue...
*/
   return (MRFROMSHORT (TRUE));
}
/*==============================================================================
**
** Function:   FrmSetDialogItems
**
** Usage:      Sets items in the dialog window with the contents contained in
**             the object specified in 'ulHandle'.
**
** Remarks:    The dialog window displays information associated with the DDE
**             memory-object specified in 'ulHandle'.  Both the label and the
**             data itself is updated according to the type of memory-object
**             being displayed.
**
==============================================================================*/
   BOOL                             /* fResult:                               */
   APIENTRY    FrmSetDialogItems
(
   ULONG       ulHandle             /* pointer to DDE memory object           */
)
{
   BOOL        fResult;
   BOOL        fDdeInit   = ulHandle & FDDEINIT;
   PDDEINIT    pDdeInit   = (PDDEINIT) (ulHandle &= (~FDDEINIT));
   PDDESTRUCT  pDdeStruct = (PDDESTRUCT) ulHandle;
   HWND        hwndItem;
   CHAR        szBuffer [MAX_TEXTLENGTH];
   ULONG       cbBuffer;
   ULONG       ulResult;
/*
** Set items...
*/
   fResult = WinEnableControl
           ( hwndDialog
           , IDC_STRUCT_TEXT_CB
           , TRUE
           );
   fResult = WinSetDlgItemText
           ( hwndDialog
           , IDC_STRUCT_TEXT_CB
           , (fDdeInit)
           ? "cb"
           : "cbData"
           );
   fResult = WinSetDlgItemShort
           ( hwndDialog
           , IDC_STRUCT_CB
           , (fDdeInit)
           ? pDdeInit->cb
           : pDdeStruct->cbData
           , FALSE
           );
   fResult = WinEnableControl
           ( hwndDialog
           , IDC_STRUCT_TEXT_STRING1
           , TRUE
           );
   fResult = WinSetDlgItemText
           ( hwndDialog
           , IDC_STRUCT_TEXT_STRING1
           , (fDdeInit)
           ? "Application:"
           : "Item:"
           );
   fResult = WinSetDlgItemText
           ( hwndDialog
           , IDC_STRUCT_STRING1
           , (fDdeInit)
           ? pDdeInit->pszAppName
           : (PSZ) DDES_PSZITEMNAME (pDdeStruct)
           );
   fResult = WinEnableControl
           ( hwndDialog
           , IDC_STRUCT_TEXT_STRING2
           , (fDdeInit)
           ? TRUE
           : FALSE
           );
   fResult = WinSetDlgItemText
           ( hwndDialog
           , IDC_STRUCT_TEXT_STRING2
           , (fDdeInit)
           ? "Topic:"
           : "Format"
           );
   fResult = WinSetDlgItemText
           ( hwndDialog
           , IDC_STRUCT_STRING2
           , (fDdeInit)
           ? pDdeInit->pszTopic
           : pszNull
           );
/*
** If the memory-object is a DDEINIT structure...
*/
   if (fDdeInit)
   {
/*
**    If a 'Conversation Context' structure included...
*/
      if (pDdeInit->offConvContext)
      {
         PCONVCONTEXT pCC = DDEI_PCONVCONTEXT (pDdeInit);

         fResult = WinEnableControl
                 ( hwndDialog
                 , IDC_STRUCT_TEXT_COUNTRY
                 , TRUE
                 );
         fResult = WinSetDlgItemShort
                 ( hwndDialog
                 , IDC_STRUCT_COUNTRY_ID
                 , pCC->idCountry
                 , FALSE
                 );
         fResult = WinEnableControl
                 ( hwndDialog
                 , IDC_STRUCT_TEXT_CODEPAGE
                 , TRUE
                 );
         fResult = WinSetDlgItemShort
                 ( hwndDialog
                 , IDC_STRUCT_CODEPAGE_ID
                 , pCC->usCodepage
                 , FALSE
                 );
         fResult = WinEnableControl
                 ( hwndDialog
                 , IDC_STRUCT_TEXT_LANGUAGE
                 , TRUE
                 );
         fResult = WinSetDlgItemShort
                 ( hwndDialog
                 , IDC_STRUCT_LANGUAGE_ID
                 , pCC->usLangID
                 , FALSE
                 );
         fResult = WinEnableControl
                 ( hwndDialog
                 , IDC_STRUCT_TEXT_SUBLANG
                 , TRUE
                 );
         fResult = WinSetDlgItemShort
                 ( hwndDialog
                 , IDC_STRUCT_SUBLANGUAGE_ID
                 , pCC->usSubLangID
                 , FALSE
                 );
/*
**       If the 'Conversation Context' specified case sensitive string
**       comparisons...
*/
         if (pCC->fsContext & DDECTXT_CASESENSITIVE)
         {
            sprintf ( szBuffer
                    , "0x%04X"
                    , pCC->fsContext
                    );
            fResult = WinSetDlgItemText
                    ( hwndDialog
                    , IDC_STRUCT_FLAGS
                    , szBuffer
                    );
            ulResult = WinEnableControl
                     ( hwndDialog
                     , IDC_STRUCT_FCASESENSITIVE
                     , TRUE
                     );
         }
      }
   }
   else
/*
** The memory-object is a DDESTRUCT structure...
*/
   {
      sprintf ( szBuffer
              , "0x%04X"
              , pDdeStruct->usFormat
              );
      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_FORMAT
              , TRUE
              );
      fResult = WinSetDlgItemText
              ( hwndDialog
              , IDC_STRUCT_FORMAT_ID
              , szBuffer
              );
      sprintf ( szBuffer
              , "0x%04X"
              , pDdeStruct->fsStatus
              );
      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_FLAGS
              , TRUE
              );
      fResult = WinSetDlgItemText
              ( hwndDialog
              , IDC_STRUCT_FLAGS
              , szBuffer
              );
/*
**    If a DDE format identifier specified...
*/
      if (pDdeStruct->usFormat)
      {
         switch (pDdeStruct->usFormat)
         {
            case CF_TEXT:
            {
               cbBuffer = WinLoadString
                        ( hab
                        , (HMODULE) 0
                        , IDS_DDE_FORMAT_TEXT
                        , MAX_TEXTLENGTH
                        , szBuffer
                        );
               break;
            }
            case CF_METAFILE:
            {
               cbBuffer = WinLoadString
                        ( hab
                        , (HMODULE) 0
                        , IDS_DDE_FORMAT_METAFILE
                        , MAX_TEXTLENGTH
                        , szBuffer
                        );
               break;
            }
            case CF_PALETTE:
            {
               cbBuffer = WinLoadString
                        ( hab
                        , (HMODULE) 0
                        , IDS_DDE_FORMAT_PALETTE
                        , MAX_TEXTLENGTH
                        , szBuffer
                        );
               break;
            }
            case CF_BITMAP:
            {
               cbBuffer = WinLoadString
                        ( hab
                        , (HMODULE) 0
                        , IDS_DDE_FORMAT_BITMAP
                        , MAX_TEXTLENGTH
                        , szBuffer
                        );
               break;
            }
            default:
            {
               cbBuffer = WinQueryAtomName
                        ( hAtomTable
                        , (ATOM) pDdeStruct->usFormat
                        , szBuffer
                        , MAX_TEXTLENGTH
                        );
               break;
            }
         }
         fResult = WinEnableControl
                 ( hwndDialog
                 , IDC_STRUCT_TEXT_STRING2
                 , TRUE
                 );
         fResult = WinSetDlgItemText
                 ( hwndDialog
                 , IDC_STRUCT_STRING2
                 , szBuffer
                 );
      }
      if (pDdeStruct->fsStatus & DDE_FACK)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FACK
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_FBUSY)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FBUSY
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_FNODATA)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FNODATA
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_FACKREQ)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FACKREQ
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_FRESPONSE)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FRESPONSE
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_NOTPROCESSED)
      {
         ulResult = WinEnableControl
                  ( hwndDialog
                  , IDC_STRUCT_FNOTPROCESSED
                  , TRUE
                  );
      }
      if (pDdeStruct->fsStatus & DDE_FAPPSTATUS)
      {
         sprintf ( szBuffer
                 , "0x%04X"
                 , pDdeStruct->usFormat & DDE_FAPPSTATUS
                 );
         fResult = WinSetDlgItemText
                 ( hwndDialog
                 , IDC_STRUCT_APPSTATUS
                 , szBuffer
                 );
      }
   }

   if (pDdeStruct->usFormat == usFmtCPText)
   {
      PCPTEXT  pCPText = (PCPTEXT) DDES_PABDATA (pDdeStruct);

      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_COUNTRY
              , TRUE
              );
      fResult = WinSetDlgItemShort
              ( hwndDialog
              , IDC_STRUCT_COUNTRY_ID
              , pCPText->idCountry
              , FALSE
              );
      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_CODEPAGE
              , TRUE
              );
      fResult = WinSetDlgItemShort
              ( hwndDialog
              , IDC_STRUCT_CODEPAGE_ID
              , pCPText->usCodepage
              , FALSE
              );
      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_LANGUAGE
              , TRUE
              );
      fResult = WinSetDlgItemShort
              ( hwndDialog
              , IDC_STRUCT_LANGUAGE_ID
              , pCPText->usLangID
              , FALSE
              );
      fResult = WinEnableControl
              ( hwndDialog
              , IDC_STRUCT_TEXT_SUBLANG
              , TRUE
              );
      fResult = WinSetDlgItemShort
              ( hwndDialog
              , IDC_STRUCT_SUBLANGUAGE_ID
              , pCPText->usSubLangID
              , FALSE
              );
   }

   return (TRUE);
}
/*==============================================================================
**
** Function:   FrmResetDialogItems
**
** Usage:      Sets items in the dialog window to default values.
**
==============================================================================*/
   BOOL                             /* fResult:                               */
   APIENTRY    FrmResetDialogItems
(
)
{
   BOOL        fResult;
   ULONG       ulDlgCode;
   HWND        hwndNext;
/*
** Obtain a list of child windows contained in the dialog window...
*/
   HENUM       henum = WinBeginEnumWindows (hwndDialog);
/*
** For each child window...
*/
   while (hwndNext = WinGetNextWindow (henum))
   {
/*
**    Determine its type...
*/
      ulDlgCode   = (ULONG) WinSendMsg
                  ( hwndNext
                  , WM_QUERYDLGCODE
                  , MPVOID
                  , MPVOID
                  );
/*
**    If the item is a static (label) item, disable it...
*/
      if (ulDlgCode & DLGC_STATIC)
      {
         fResult = WinEnableWindow
                 ( hwndNext
                 , FALSE
                 );
      }
/*
**    If the item is an entry item, clear it...
*/
      if (ulDlgCode & DLGC_ENTRYFIELD)
      {
         fResult = WinSetWindowText
                 ( hwndNext
                 , pszNull
                 );
      }
   }
/*
** Release enumeration structures...
*/
   fResult = WinEndEnumWindows
           ( henum
           );
/*
** Return successful indicator...
*/
   return (TRUE);
}
/*==============================================================================
**
** Function:   FrmResetScrollBar
**
** Usage:      Resets "WC_SCROLLBAR" class window to default values.
**
==============================================================================*/
   BOOL                             /* fResult:                               */
   APIENTRY    FrmResetScrollBar
(
   HWND        hwndScrollBar        /* handle of scroll bar window            */
)
{
   BOOL        fResult;
/*
** Reset the 'Thumb' size to zero...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndScrollBar
            , SBM_SETTHUMBSIZE
            , MPFROM2SHORT (0, 0)
            , MPVOID
            );
/*
** Reset the 'Range' and 'Positions' to zero...
*/
  fResult   = (BOOL) WinSendMsg
            ( hwndScrollBar
            , SBM_SETSCROLLBAR
            , MPFROMSHORT  (0)
            , MPFROM2SHORT (0, 0)
            );
/*
** Return success indicator...
*/
   return (TRUE);
}
/*==============================================================================
**
** Function:   FrmSetStatusText
**
** Usage:      Sets the 'Text' of a "WC_STATIC" class window.
**
==============================================================================*/
   BOOL                             /* fResult:                               */
   APIENTRY    FrmSetStatusText
(
   HAB         hab                  /* handle anchor-block                    */
,  HWND        hwndStatus           /* handle status window                   */
,  PSZ         pszText              /* pointer to null terminate text string  */
,  USHORT      usStringId           /* string identifier (optional)           */
,  USHORT      fsNote               /* tone of alarm (optional)               */
)
{
   BOOL        fResult;
   CHAR        szString [MAX_TEXTLENGTH] = "";
   SHORT       cbString = 1;
/*
** If a pointer to a null terminated string was given...
*/
   if (pszText)
   {
/*
**    Obtain its size...
*/
      cbString = strlen
               ( pszText
               );
   }
   else
   {
/*
**    Reset pointer to a zero-length string...
*/
      pszText  = szString;
   }
/*
** If a string identifier specified, obtain the associate text from the
** applications executable file...
*/
   if (usStringId)
   {
      cbString = WinLoadString
               ( hab
               , (HMODULE) 0
               , usStringId
               , (SHORT) sizeof (szString)
               , szString
               );
   }
/*
** If the corresponding string was found...
*/
   if (cbString)
   {
/*
**    Set this text into the status window...
*/
      fResult  = WinSetWindowText
               ( hwndStatus
               , pszText
               );
/*
**    If the caller specified desired sound...
*/
      if (fsNote)
      {
         fResult  = WinAlarm
                  ( HWND_DESKTOP
                  , fsNote
                  );
      }
   }
/*
** Return success indicator...
*/
   return (TRUE);
}
