/*==============================================================================
**
** Module:     Dde.c
**
** Remarks:    This module contains routines that facilitate Dynamic Data
**             Exchange functionality.
**
**============================================================================*/
/*
** System specific defines:
*/
   #define     INCL_DOS
   #define     INCL_DOSERRORS
   #define     INCL_GPI
   #define     INCL_WIN
/*
** System specific include files:
*/
   #include    <os2.h>
   #include    <string.h>
   #include    <stdio.h>
   #include    <stdarg.h>
   #include    <ctype.h>
/*
** Application specific include files:
*/
   #include    <main.h>
   #include    <menu.h>
   #include    <common.h>
   #include    <dde.h>
   #include    <dialogs.h>
   #include    <resource.h>
/*
** External dependencies:
*/
   extern   HATOMTBL    hAtomTable;
   extern   CONVCONTEXT ConvContext;
   extern   PFNWP       pfnDdeConvWindowProc;

/*==============================================================================
**
** Function:   DdeAdviseWindowProc
**
** Usage:      Processes messages dispatched to a "WC_DdeAdvise" class window.
**
** Remarks:    This function is specified as the "Window Procedure" when
**             registering (WinRegisterClass) a "WC_DdeAdvise" window class.
**
**============================================================================*/
   MRESULT                          /* Message result                         */
   EXPENTRY    DdeAdviseWindowProc
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* message identifier                     */
,  MPARAM      mp1                  /* message parameter one                  */
,  MPARAM      mp2                  /* message parameter two                  */
)
{
   MRESULT     MResult;             /* message result                         */

   switch (ulMsgId)
   {
      case UM_AdviseItem:
      {
         MResult = DdeAdviseUmAdviseItem
                 ( hwnd
                 , mp1
                 , mp2
                 );
         break;
      }
      case UM_UnAdviseItem:
      {
         MResult = DdeAdviseUmUnAdviseItem
                 ( hwnd
                 , mp1
                 , mp2
                 );
         break;
      }
      case WM_CREATE:
      {
         MResult = DdeAdviseWmCreate
                 ( hwnd
                 , mp1
                 , mp2
                 );
         break;
      }
      case WM_DDE_ACK:
      {
         MResult = DdeAdviseWmDdeAck
                 ( hwnd
                 , mp1
                 , mp2
                 );
         break;
      }
      case WM_DESTROY:
      {
         MResult = DdeAdviseWmDestroy
                 ( hwnd
                 );
         break;
      }
      case WM_QUERYWINDOWPARAMS:
      {
         MResult = DdeAdviseWmQueryWindowParams
                 ( hwnd
                 , mp1
                 , mp2
                 );
         break;
      }
      case WM_CLOSE:
      {
         MResult = MRFROMSHORT (0);
         break;
      }
      default:
      {
         MResult = WinDefWindowProc
                 ( hwnd
                 , ulMsgId
                 , mp1
                 , mp2
                 );
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
**
** Function:   DdeAdviseWmCreate
**
** Usage:      Processes a "WM_CREATE" message dispatched to a "WC_DdeAdvise"
**             class window.
**
** Remarks:    A WM_CREATE message is 'sent' to a window procedure after the
**             window is created but before is it visible.  This offers the
**             window procedure an opportunity to perform initialization
**             actions.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeAdviseWmCreate
(
   HWND        hwnd                 /* handle of window being created         */
,  MPARAM      mp1                  /* pointer to DDEADVISE structure         */
,  MPARAM      mp2                  /* pointer to CREATESTRUCT structure      */
)
{
   BOOL        fResult     = FALSE;
   PDDEADVISE  pDdeAdvise  = (PDDEADVISE) PVOIDFROMMP (mp1);
/*
** Store pointer to "WC_DdeAdvise" class information in window extra-words...
*/
   fResult  = WinSetWindowPtr       /* set window pointer of...               */
            ( hwnd                  /*   this window at,                      */
            , QWL_USER              /*   this zero-based offset to,           */
            , pDdeAdvise            /*   this DDEADVISE structure pointer.    */
            );
/*
** If creation processing successful...
*/
   if (fResult)
   {
      return (MRFROMSHORT (FALSE)); /* no errors, continue...                 */
   }
   else
   {
      return (MRFROMSHORT (TRUE));  /* error occurred, discontinue...         */
   }
}
/*==============================================================================
**
** Function:   DdeAdviseWmDestroy
**
** Usage:      Processes a "WM_DESTROY" message dispatched to a "WC_DdeAdvise"
**             class window.
**
** Remarks:    A WM_DESTROY message is sent to a window after it has been
**             hidden offering it an opportunity to perform termination actions.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeAdviseWmDestroy
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult;
/*
** Query pointer to "WC_DdeAdvise" class information...
*/
   PDDEADVISE  pDdeAdvise  = (PDDEADVISE) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Free window instance data...
*/
   fResult  = DdeFreeMem            /* free memory object at...               */
            ( pDdeAdvise            /*   this location,                       */
            , pDdeAdvise->cb        /*   and size (in bytes).                 */
            );

   return (MRFROMLONG (0L));
}
/*==============================================================================
**
** Function:   DdeAdviseWmDdeAck
**
** Usage:      Processes a "WM_DDE_ACK" message dispatched to a "WC_DdeAdvise"
**             class window.
**
** Remarks:    A WM_DDE_ACK message is 'posted' by an application in response
**             to previously issued WM_DDE_DATA message if the DDE_FACKREQ
**             status flag was set.
**
** Guidelines:
**
**    1)       Update the fAckPending flag. This flag is used as flow control
**             between a server and client application.  This flag is only
**             used if the client application specified DDE_FACKREQ on the
**             original WM_DDE_ADVISE request. In this case the server
**             application does not post subsequent WM_DDE_DATA messages
**             until the client application has responded to the previous
**             WM_DDE_DATA message.
**
**    2)       Free the DDESTRUCT structure.
**
**             NOTE:    The example programs found in this book retain all
**                      DDE messages for later replay, however, the location
**                      at which this structure is freed is specified in the
**                      code but is commented out.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeAdviseWmDdeAck
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle of responding application*/
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
/*
** Query pointer to "WC_DdeAdvise" class information...
*/
   PDDEADVISE  pDdeAdvise  = (PDDEADVISE) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain pointer to associated DDESTRUCT structure and item name...
*/
   PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) PVOIDFROMMP (mp2);
   PSZ         pszItemName = (PSZ) DDES_PSZITEMNAME (pDdeStruct);
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
  SHORT        sItemIndex  = FrmInsertListBoxItem
                           ( hwndListBox
                           , WM_DDE_ACK
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** Clear the fAckPending flag.  The state of this flag determines if
** subsequent WM_DDE_DATA messages are posted if/when a data item changes.
*/
   pDdeAdvise->fAckPending = FALSE;
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeAdviseUmAdviseItem
**
** Usage:      Processes a "UM_AdviseItem" dispatched to a "WC_DdeAdvise" class
**             window.
**
** Remarks:    A "UM_AdviseItem" message is issued by a DDE server application
**             to all of its "WC_DdeTopic" class windows.  A "WC_DdeTopic"
**             class window issues the 'same' message to of its (WC_DdeItem)
**             children windows.  A "WC_DdeItem" window determines if the
**             'Advise' notification corresponds to the 'Item' it is supporting.
**             If it is, it broadcasts the message to all of its (WC_DdeAdvise)
**             children windows.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   EXPENTRY    DdeAdviseUmAdviseItem
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* item identifier                        */
,  MPARAM      mp2                  /* item handle (hwndMenu, pszText or Time)*/
)
{
   BOOL        fResult     = FALSE;
   PDDESTRUCT  pDdeStruct;
/*
** Query pointer to "WC_DdeAdvise" class information...
*/
   PDDEADVISE  pDdeAdvise  = (PDDEADVISE) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain the associated DDE format identifier by querying the window
** identifier of the "WC_DdeAdvise" class window.  Windows of this class
** set their window identifier equal to the format it is supporting.
*/
   USHORT      usFormat    = (USHORT) WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
** If this data-link is currently waiting for a WM_DDE_ACK message in response
** to a previous request, ignore discard the update notification.
*/
   if (pDdeAdvise->fAckPending == TRUE)
   {
      return (MRFROMSHORT (FALSE));
   }
/*
** Ask our owner to render the data item in the requested format,
*/
   pDdeStruct  = (PDDESTRUCT) WinSendMsg
               ( WinQueryWindow (hwnd, QW_OWNER)
               , UM_RenderItem
               , MPFROM2SHORT (usFormat, pDdeAdvise->fsStatus)
               , mp2
               );
/*
** If our owner successfully rendered the item...
*/
   if (pDdeStruct)
   {
      HWND  hwndClient  = pDdeAdvise->hwndClient;
/*
**    Notify client application that the data item changed...
*/
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndClient         /*   this requesting window handle,       */
               , hwnd               /*   from the "Advise" window handle,     */
               , WM_DDE_DATA        /*   a "Data" message,                    */
               , pDdeStruct         /*   as defined in this memory object     */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
/*
**    If message was successfully posted, set the fAckPending (control-flow)
**    flag according to value specified in the original WM_DDE_ADVISE message.
**    Otherwise, we failed and need to free any allocated resources.
*/
      if (fResult)
      {
         pDdeAdvise->fAckPending = (pDdeAdvise->fsStatus & DDE_FACKREQ);
      }
      else
      {
         DosFreeMem (pDdeStruct);
      }
   }

   return MRFROMSHORT (fResult);
}
/*==============================================================================
**
** Function:   DdeAdviseUmUnAdviseItem
**
** Usage:      Processes a 'UM_UnAdviseItem' message dispatched to a
**             "WC_DdeAdvise" class window.
**
** Remarks:    A 'UM_UnAdvise' message is broadcasted to all "WC_DdeAdvise"
**             class windows by a "WC_DdeItem" class window when it receives
**             a 'UM_UnAdvise message.  A 'UM_UnAdvise' message is dispatched
**             to a "WC_DdeItem" class window when a "WC_DdeConv" class
**             window receives a "WM_DDE_UNADVISE" message.
**
** Guidelines:
**
**    1)       Determine if the notification pertains to this data-link by
**             comparing the window handle specified in message parameter one
**             (mp1) to that stored in the "WC_DdeAdvise" window class data.
**
**    2)       Determine if the notification pertains to the format supported
**             by this data-link.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   EXPENTRY    DdeAdviseUmUnAdviseItem
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle of conversation link     */
,  MPARAM      mp2                  /* DDE format identifier                  */
)
{
   BOOL        fResult     = FALSE;
/*
** Obtain conversation link window handle and associated DDE format...
*/
   HWND        hwndConv    = HWNDFROMMP   (mp1);
   USHORT      usFormat    = SHORT1FROMMP (mp2);
/*
** Query pointer to "WC_DdeAdvise" class information...
*/
   PDDEADVISE  pDdeAdvise  = (PDDEADVISE) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Check if notification is for this data-link...
*/
   if (pDdeAdvise->hwndConv == hwndConv)
   {
/*
**    Obtain the associated DDE format identifier by querying the window
**    identifier of the "WC_DdeAdvise" class window.  Windows of this class
**    set their window identifier equal to the format it is supporting.
*/
      USHORT   usWindowId  = (USHORT) WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
**    Determine scope of UnAdvise notification...
*/
      if (usFormat == 0             /* All formats,                           */
      ||  usFormat == usWindowId)   /* or matches this data-link format.      */
      {
         fResult  = WinDestroyWindow
                  ( hwnd
                  );
/*
**       Indicate that a data-link was found.  This is necessary in order for
**       a server application post the correct (positive or negative)
**       WM_DDE_ACK message in response to a WM_DDE_UNADVISE message.
*/
         fDataLinkExisted = TRUE;
      }
   }

   return (MRFROMSHORT (fResult));
}
/*==============================================================================
**
** Function:   DdeAdviseWmQueryWindowParams
**
** Usage:      Processes a "WM_QUERYWINDOWPARAMS" message dispatched to a
**             "WC_DdeAdvise" class window.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeAdviseWmQueryWindowParams
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* pointer to WNDPARAMS structure         */
,  MPARAM      mp2                  /* reserved value, zero (0)               */
)
{
   PWNDPARAMS  pWndParams  = (PWNDPARAMS) PVOIDFROMMP (mp1);
/*
** Obtain registered DDE format identifier associated with this data-link by:
** Querying the window identifier of this "Advise" window.  The window
** identifier of a "WC_DdeAdvise" class window identifies which DDE format it
** is supporting.
*/
   USHORT      usFormatId  = WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
** If window text or window text length is requested...
*/
   if (pWndParams->fsStatus & WPM_TEXT
   ||  pWndParams->fsStatus & WPM_CCHTEXT)
   {
      ULONG    cbText;
      ULONG    idText   = IDS_DDE_FORMAT_NONE;
      CHAR     szText   [MAX_TEXTLENGTH];
      PSZ      pszFrom  = szText;
      PSZ      pszTo    = pWndParams->pszText;
/*
**    Obtain the associated DDE format identifier by querying the window
**    identifier of the "WC_DdeAdvise" class window.  Windows of this class
**    set their window identifier equal to the format it is supporting.
*/
      USHORT   usId     = WinQueryWindowUShort
                        ( hwnd
                        , QWS_ID
                        );
/*
**    Determine the correct string identifier associated with format...
*/
      if (usFormatId == usFmtText)           idText = IDS_DDE_FORMAT_TEXT;
      else if (usFormatId == usFmtCPText)    idText = IDS_DDE_FORMAT_CPTEXT;
      else if (usFormatId == usFmtBitmap)    idText = IDS_DDE_FORMAT_BITMAP;
      else if (usFormatId == usFmtPalette)   idText = IDS_DDE_FORMAT_PALETTE;
      else if (usFormatId == usFmtMetaFile)  idText = IDS_DDE_FORMAT_METAFILE;
      else if (usFormatId == usFmtLink)      idText = IDS_DDE_FORMAT_LINK;
      else if (usFormatId == usFmtTime)      idText = IDS_DDE_FORMAT_TIME;
/*
**    Load associated text string...
*/
      cbText   = WinLoadString         /* Load string from a resource         */
               ( hab                   /* Anchor-block handle                 */
               , (HMODULE) 0           /* extract from applications .exe file */
               , idText                /* string identifier                   */
               , MAX_TEXTLENGTH        /* size of buffer to receive string    */
               , szText                /* address of buffer to receive string */
               );
/*
**    If window text requested...
*/
      if (pWndParams->fsStatus & WPM_TEXT)
      {
/*
**       Copy the item name-string to the location indicated in the WNDPARAMS
**       structure.
*/
         while (*pszTo++ = *pszFrom++);
/*
**       Clear this flag in the fsStatus field of the WNDPARAMS structure.
**       This indicates that item was processed.
*/
         pWndParams->fsStatus &= ~WPM_TEXT;
      }
/*
**    If window text length requested...
*/
      if (pWndParams->fsStatus & WPM_CCHTEXT)
      {
/*
**       Calculate the length of the topic name-string and set the appropriate
**       field in the WNDPARAMS structure with this value.  Clear corresponding
**       flag in the fsStatus field indicating this attribute was processed...
*/
         pWndParams->cchText   = strlen (szText);
         pWndParams->fsStatus &= ~WPM_CCHTEXT;
      }
      return (MRFROMSHORT (TRUE));
   }
/*
** Pass unsupported attributes to the default window procedure...
*/
   return (WinDefWindowProc (hwnd, WM_QUERYWINDOWPARAMS, mp1, mp2));
}
/*==============================================================================
**
** Function:   DdeAllocMem
**
** Usage:
**
** Remarks:
**
**============================================================================*/
   PVOID                            /* pvMemory: Address of allocated memory  */
   APIENTRY    DdeAllocMem
(
   ULONG       ulSize               /* size, in bytes to allocated            */
)
{
   APIRET      ApiRet;              /* Api return code                        */
   PVOID       pvMemBlock;          /* receives address of allocated memory   */
/*
** Allocate a piece of memory from our heap...
*/
   ApiRet   = DosSubAllocMem        /* Allocate block of memory               */
            ( pHeap                 /* Address of our (heap) memory pool      */
            , &pvMemBlock           /* address to receive address of block    */
            , ulSize                /* size, in bytes, of block to allocate   */
            );
/*
** Check the results of memory sub-allocation...
*/
   if (ApiRet)                      /* if an error occurred, return...        */
   {
      return (NULL);                /* a NULL pointer indicating failure.     */
   }
   else
   {
      return (pvMemBlock);          /* a pointer to allocated memory block.   */
   }
}
/*==============================================================================
**
** Function:   DdeCompareStrings
**
** Usage:      Compares two strings with sensitivity to case, country and
**             codepage.
**
** Remarks:
**
**============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DdeCompareStrings
(
   PSZ         pszString1           /* pointer to null terminated string      */
,  PSZ         pszString2           /* pointer to null terminated string      */
,  ULONG       fCaseSensitive       /* compares are case sensitive:TRUE/FALSE */
,  ULONG       ulCountry            /* country code used in comparisons      */
,  ULONG       ulCodepage           /* codepage used in comparisons          */
)
{
   ULONG       flResult;
   ULONG       cbString;
   CHAR        szSTRING1 [MAX_TEXTLENGTH];
   CHAR        szSTRING2 [MAX_TEXTLENGTH];
/*
** If comparison of strings is not casesensitive, then convert each string
** to uppercase.  This allows for a mixed-case strings to be compared
** equally.
*/
   if (!fCaseSensitive)
   {
/*
**    Make a copy of string1 and string2.  This allows us to convert the
**    strings to upper case while preserving the original strings...
*/
      pszString1  = strcpy          /* copy a null (0x00) terminated string...*/
                  ( (PSZ) szSTRING1 /*   to this location,                    */
                  , pszString1      /*   from this location.                  */
                  );
      pszString2  = strcpy          /* copy a null (0x00) terminated string...*/
                  ( (PSZ) szSTRING2 /*   to this location,                    */
                  , pszString2      /*   from this location.                  */
                  );
/*
**    Convert string1 to upper case...
*/
      cbString    = WinUpper        /* convert a string to uppercase          */
                  ( hab             /* handle to anchor block                 */
                  , ulCodepage      /* codepage identifier                    */
                  , ulCountry       /* country identifier                     */
                  , (PSZ) szSTRING1 /* location of string to convert          */
                  );
/*
**    If the string could not be converted, return indicating failure...
*/
      if (cbString == 0)
      {
         return (FALSE);
      }
/*
**    Convert string1 to upper case...
*/
      cbString    = WinUpper        /* convert a string to uppercase         */
                  ( hab             /* handle to anchor block                 */
                  , ulCodepage      /* Codepage identifier                   */
                  , ulCountry       /* Country identifier                    */
                  , (PSZ) szSTRING2 /* location of string to convert          */
                  );
/*
**    if the string could not be converted, return indicating failure...
*/
      if (cbString == 0)
      {
         return (FALSE);
      }
   }
/*
** Compare strings...
*/
   flResult = WinCompareStrings
            ( hab                   /* Anchor-block handle                    */
            , ulCodepage            /* Code page identity of both strings     */
            , ulCountry             /* Country code identity of both strings  */
            , pszString1            /* Pointer to first comparison string     */
            , pszString2            /* Pointer to second comparison string    */
            , 0L                    /* Options:  Not used must be zero (0)    */
            );
/*
** Check the result of the comparison...
*/
   if (flResult == WCS_EQ)          /* if strings were equal...               */
   {
      return (TRUE);
   }
   else
   {
      return (FALSE);
   }
}
/*==============================================================================
**
** Function:   DdeCopyConvContext
**
** Usage:      Creates a copy of a CONVCONTEXT structure.
**
**============================================================================*/
   PCONVCONTEXT                     /* pointer to CONVCONTEXT structure       */
   APIENTRY    DdeCopyConvContext
(
  PCONVCONTEXT pOldContext          /* pointer of source CONVCONTEXT structure*/
)
{
   BOOL        fResult;
   USHORT      cbConvContext  = pOldContext->cb;
/*
** Allocate a block memory for new CONVCONTEXT structure...
*/
   PCONVCONTEXT pNewContext   = DdeAllocMem
                              ( cbConvContext
                              );
/*
** If memory allocation was successful...
*/
   if (pNewContext)
   {
      pOldContext = memcpy          /* copy memory to...                      */
                  ( pNewContext     /*   this memory location from,           */
                  , pOldContext     /*   this memory location for,            */
                  , cbConvContext   /*   this number of bytes.                */
                  );
/*
**    If memory copy failed...
*/
      if (!pOldContext)
      {
         fResult  = DdeFreeMem      /* Free newly allocated memory object     */
                  ( pNewContext     /*   at this memory location for,         */
                  , cbConvContext   /*   this number of bytes.                */
                  );
/*
**       Set returned variable to indicate an error occurred.
*/
         pNewContext = NULL;
      }
   }
/*
** Return pointer to new CONVCONTEXT structure, NULL if an error occurred.
*/
   return (pNewContext);
}
/*==============================================================================
**
** Function:   DdeClientConvWindowProc
**
** Usage:      Sub-classes a conversation link window procedure created by a
**             DDE Client application.
**
** Remarks:    A DDE Client application sub-classes a conversation link window
**             with this function to implement DDE Client specific behavior.
**
** Guidelines:
**
**    1)       Respond with a negative WM_DDE_ACK message to unsupported DDE
**             messages.
**
**    2)       Terminate any permanent data-links that might have been
**             requested.
**
**============================================================================*/
   MRESULT                          /* Message result                         */
   EXPENTRY    DdeClientConvWindowProc
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* message identifier                     */
,  MPARAM      mp1                  /* message parameter one                  */
,  MPARAM      mp2                  /* message parameter two                  */
)
{
   APIRET      ApiRet;
   BOOL        fResult;
   HWND        hwndClient;
   PCONVINFO   pConvInfo;
   PDDESTRUCT  pDdeStruct;
   PVOID       pvData;
   ULONG       cbData;
/*
** The following DDE messages are not supported by a DDE Client application.
** If one of these messages are received, a negative WM_DDE_ACK message is
** posted in response.  The DDE_NOTPROCESSED status flag is set indicating that
** the message was not processed.
*/
   if (ulMsgId == WM_DDE_ADVISE
   ||  ulMsgId == WM_DDE_EXECUTE
   ||  ulMsgId == WM_DDE_POKE
   ||  ulMsgId == WM_DDE_REQUEST
   ||  ulMsgId == WM_DDE_UNADVISE)
   {
      hwndClient  = HWNDFROMMP (mp1);
      pDdeStruct  = PDDESTRUCTFROMMP (mp2);
      pvData      = NULL;
      cbData      = 0L;
/*
**    Acknowledgement messages in response to an 'Execute' request must contain
**    the original command strings of the request.
*/
      if (ulMsgId == WM_DDE_EXECUTE)
      {
         pvData   = (PVOID) DDES_PABDATA (pDdeStruct);
         cbData   = pDdeStruct->cbData;
      }
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ((PSZ) DDES_PSZITEMNAME (pDdeStruct)
                  , pvData
                  , cbData
                  , pDdeStruct->usFormat
                  , DDE_NOTPROCESSED
                  );
/*
**    If shared memory object successfully created...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message...                  */
                  ( hwndClient      /*   to this requesting window handle,    */
                  , hwnd            /*   from the "Client" window handle,     */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this structure.  And,  */
                  , DDEPM_RETRY     /*   retry if receiving queue full        */
                  );
      }
/*
**    Free original shared memory object...
*/
      ApiRet   = DosFreeMem
               ( (PVOID) mp2
               );

     return (MRFROMSHORT (0));
   }
   if (ulMsgId == WM_CLOSE)
   {
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( NULL            /* item name-string (all items)           */
                  , NULL            /* pointer to data (not applicable        */
                  , 0               /* size of data (not applicable)          */
                  , 0               /* Registered DDE format identifier       */
                  , 0               /* DDE status flags                       */
                  );
/*
**    If shared memory object allocated...
*/
      if (pDdeStruct)
      {
/*
**       Obtain "WC_DdeConv" window class info...
*/
         pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                     ( hwnd
                     , QWL_USER
                     );
/*
**       Obtain window handle of our conversation partner...
*/
         hwndClient  = pConvInfo->hwndPartner;
/*
**       Notify our conversation partner to terminate any permanent data-links
**       established by this conversation link.
*/
         fResult  = WinDdePostMsg   /* post a DDE message...                  */
                  ( hwndClient      /*   to this window handle,               */
                  , hwnd            /*   from this window handle,             */
                  , WM_DDE_UNADVISE /*   an "Unadvise" message as defined in, */
                  , pDdeStruct      /*   this structure,                      */
                  , DDEPM_RETRY     /*   and retry if receiving queue if full.*/
                  );
     }
  }
  return (*pfnDdeConvWindowProc) (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
**
** Function:   DdeServerConvWindowProc
**
** Usage:      Sub-classes a conversation link window procedure created by a
**             DDE Server application.
**
** Remarks:    A DDE server application sub-classes a conversation link window
**             with this function to implement server specific behavior.
**             Currently, no specific behavior is required for a DDE Server
**             application.  However, it exists for consistency with the method
**             used by a DDE Client application.
**
**============================================================================*/
   MRESULT
   EXPENTRY    DdeServerConvWindowProc
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
  return (*pfnDdeConvWindowProc) (hwnd, ulMsgId, mp1, mp2);
}
/*==============================================================================
**
** Function:   DdeConvWindowProc
**
** Usage:      Processes messages dispatched to a "WC_DdeConv" class window.
**
** Remarks:    A "WC_DdeConv" class window is created by either the DDE Server
**             or the DDE Client application to support its half of a DDE
**             conversation link.
**============================================================================*/
   MRESULT                       /* Return: Message specific result           */
   EXPENTRY    DdeConvWindowProc
(
   HWND        hwnd              /* Window handle of conversation window      */
,  ULONG       ulMsgId           /* Message identifier                        */
,  MPARAM      mp1               /* Message parameter one (1)                 */
,  MPARAM      mp2               /* Message parameter two (2)                 */
)
{
   SHORT       sItemIndex;
   MRESULT     MResult;
/*
** common pre-processing for all DDE messages...
*/
   if (ulMsgId > WM_DDE_FIRST
   &&  ulMsgId < WM_DDE_LAST)
   {
      BOOL        fResult;
/*
**    Obtain WC_DdeConv class information...
*/
      PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                              ( hwnd
                              , QWL_USER
                              );
      PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) PVOIDFROMMP (mp2);
/*
**    Clear status window...
*/
      fResult  = FrmSetStatusText
               ( hab                /* anchor-block handle                   */
               , hwndStatus         /* window handle of status window         */
               , NULL
               , 0
               , 0
               );
/*
**    If this conversation-link window is a 'Zombie' (as is the case when it
**    has received a WM_DDE_TERMINATE message or a value of zero was returned
**    from WinDdeRespond.  In this case, we ignore any subsequent DDE messages.
*/
      if (pConvInfo->fsState & CONVINFO_State_Zombie)
      {
         if (ulMsgId != WM_DDE_TERMINATE)
         {
            DosFreeMem (PVOIDFROMMP (mp2));
            return (MRFROMSHORT (0));
         }
      }
/*
**    If we have already posted a WM_DDE_TERMINATE message we are waiting for
**    our partner to post the corresponding WM_DDE_TERMINATE message indicating
**    that it is ok to destroy our associated resources.  If we receive any
**    other DDE message before we get the WM_DDE_TERMINATE message, we ignore
**    it!  We choose to ignore subsequent messages for the following reason:
**
**    Since we promised not to send any further messages (this is what posting
**    a WM_DDE_TERMINATE message means), we cannot acknowledge (WM_DDE_ACK) the
**    processing of these messages.
*/
      if (pConvInfo->fsState & CONVINFO_State_Terminating)
      {
         if (ulMsgId == WM_DDE_TERMINATE)
         {
            fResult  = (BOOL) WinSendMsg
                     ( hwndClient
                     , UM_Terminate
                     , MPFROMP (pConvInfo->szAppName)
                     , MPFROMP (pConvInfo->szTopic)
                     );
/*
**       Insert message and associated information into listbox...
**
**       Note: Typically, an application receiving a DDE message will free the
**             associated shared memory object after processing this message
**             (as shown at the end of this function.  However, we save the
**             original shared memory object for subsequent retrieval by the
**             user.  The shared memory object is freed when the listbox is
**             destroyed or cleared.
*/
         sItemIndex  = FrmInsertListboxItem
                     ( hwndListBox
                     , WM_DDE_TERMINATE
                     , ulMsgTime
                     , (ULONG) pDdeStruct
                     );
/*
**       Destroy corresponding conversation link window...
*/
         fResult  = WinDestroyWindow
                  ( hwnd
                  );
         }
         return (MRFROMSHORT (0));
      }
   }
/*
** Dispatch message to corresponding function:
*/
   switch (ulMsgId)
   {
      case UM_ShowItemDialog:
      {
         MResult  = DdeConvUmShowItemDialog
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_SetItemText:
      {
         MResult  = DdeConvUmSetItemText
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_SetState:
      {
         MResult  = DdeConvUmSetState
                  ( hwnd
                  , mp1
                  );
         break;
      }
      case WM_CLOSE:
      {
         MResult  = DdeConvWmClose
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CREATE:
      {
         MResult  = DdeConvWmCreate
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DESTROY:
      {
         MResult  = DdeConvWmDestroy
                  ( hwnd
                  );
         break;
      }
      case WM_DDE_ACK:
      {
         MResult  = DdeConvWmDdeAck
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_DATA:
      {
         MResult  = DdeConvWmDdeData
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_INITIATEACK:
      {
         MResult  = DdeConvWmDdeInitiateAck
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_REQUEST:
      {
         MResult = DdeConvWmDdeRequest
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_ADVISE:
      {
         MResult  = DdeConvWmDdeAdvise
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_UNADVISE:
      {
        MResult   = DdeConvWmDdeUnAdvise
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_POKE:
      {
         MResult  = DdeConvWmDdePoke
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_EXECUTE:
      {
         MResult  = DdeConvWmDdeExecute
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_TERMINATE:
      {
         MResult  = DdeConvWmDdeTerminate
                  ( hwnd
                  , mp1
                  );
         break;
      }
      default:
      {
         MResult  = WinDefWindowProc
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
**
** Function:   DdeConvUmSetItemText
**
** Usage:      Processes a "UM_SetItemText" message dispatched to a
**             "WC_DdeConv" class window.
**
** Remarks:    A "UM_SetItemText" message is issued to update the 'szItem'
**             field in "WC_DdeConv" class information.
**
**             The 'szItem' is used to retain the last Item requested by a user.
**             This function called during the processing of a "Paste Link"
**             request, such that when the "Item" dialog is shown, the
**             "Item name" entry field contains the name of the item specified
**             in the "Link" format.
**
**============================================================================*/
   MRESULT
   EXPENTRY    DdeConvUmSetItemText
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* address of string to be copied
*/
,  MPARAM      mp2
)
{
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Copy input string...
*/
   PSZ         pszResult   = strcpy
                           ( pConvInfo->szItem
                           , PVOIDFROMMP (mp1)
                           );

   return (MRFROMSHORT (TRUE));
}
/*==============================================================================
**
** Function:   DdeConvUmShowItemDialog
**
** Usage:      Processes a "UM_ShowItemDialog" message dispatched to a
**             "WC_DdeConv" class window.
**
** Remarks:    A "UM_ShowItemDialog" message is issued to show the "Item"
**             dialog window.  This dialog window is used to issue DDE
**             transactions corresponding to a conversation link.
**
**============================================================================*/
   MRESULT
   EXPENTRY    DdeConvUmShowItemDialog
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle root menu                */
,  MPARAM      mp2                  /* 'selected' App/Topic menu indices     */
)
{
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
   USHORT      cbText;
   BOOL        fResult;
   PSZ         pszResult;
   CHAR        szAppName [MAX_TEXTLENGTH];
   CHAR        szTopic   [MAX_TEXTLENGTH];
   ULONG       ulReply;
   USHORT      usCmd;
/*
** Query the text associated with selected Application and Topic...
*/
   cbText   = MnuQueryItemText      /* query menu item text                   */
            ( HWNDFROMMP (mp1)      /* menu window handle                     */
            , SHORT2FROMMP (mp2)    /* menu item identifier                   */
            , szAppName             /* buffer to receive menu item text       */
            );
   cbText   = MnuQueryItemText      /* query menu item text                   */
            ( HWNDFROMMP (mp1)      /* menu window handle                     */
            , SHORT1FROMMP (mp2)    /* menu item identifier                   */
            , szTopic               /* buffer to receive menu item text       */
            );
/*
** Compare requested application and topic name-strings to those located in a
** conversation link.  If either comparison fails, exit function.
*/
   if (strcmp (szAppName, pConvInfo->szAppName)
   ||  strcmp (szTopic, pConvInfo->szTopic))
   {
      return (MRFROMSHORT (FALSE));
   }
/*
** If the current state of the conversation link is "alive", then show the
** item dialog-window.
*/
   if (pConvInfo->fsState & CONVINFO_State_Alive)
   {
      DLGITEM  DlgItem;

      DlgItem.flType    = 0L;       /* defines the contents of hItem          */
      DlgItem.hItem     = 0L;       /* pointer, window handle, or time        */
      DlgItem.cbData    = 0L;       /* size of data (in bytes)                */
      DlgItem.ulOptions = 0L;       /* options for posting DDE message        */
      DlgItem.fsStatus  = 0;        /* DDE status flags                       */
      DlgItem.usFormat  = 0;        /* registered DDE format identifier       */

      pszResult   = strcpy                /* copy string to...                */
                  ( DlgItem.szItemName    /*   this location from,            */
                  , pConvInfo->szItem     /*   this location.                 */
                  );
/*
**    At this point, this is a valid request for this conversation link.
**    Display the 'Item...' dialog window.
*/
      ulReply  = WinDlgBox             /* Load and process dialog-window      */
               ( HWND_DESKTOP          /* parent window handle                */
               , hwnd                  /* owner window handle                 */
               , (PFNWP) DlgItemWindowProc   /* dialog window procedure       */
               , 0                     /* module handle (use executable       */
               , IDD_ITEM              /* dialog identifier                   */
               , &DlgItem              /* pointer to creation data            */
               );
/*
**    If user selected the "Post" push button...
*/
      if (ulReply == IDC_ITEM_OK)
      {
         PDDESTRUCT  pDdeStruct;
         PSZ         pszResult;
/*
**       Create a DDE shared memory object...
*/
         pDdeStruct  = DdeMakeDdeStruct
                     ( (PSZ) DlgItem.szItemName    /* item name-string        */
                     , (DlgItem.cbData)            /* if non-zero data length,*/
                     ? DlgItem.abData              /*    pointer to data,     */
                     : NULL                        /* else NULL pointer       */
                     , DlgItem.cbData              /* size of data (in bytes) */
                     , (USHORT) DlgItem.usFormat   /* registered DDE format   */
                     , (USHORT) DlgItem.fsStatus   /* DDE status flags        */
                     );
/*
**       Issue DDE transaction request...
*/
         fResult     = WinDdePostMsg               /* post a DDE message to...*/
                     ( pConvInfo->hwndPartner      /* this window handle from,*/
                     , hwnd                        /* this window handle, the */
                     , DlgItem.ulMsgId             /* following message as,   */
                     , pDdeStruct                  /* defined in...           */
                     , DlgItem.ulOptions
                     );
/*
**       Update conversation information with the last Item name requested...
*/
         pszResult   = strcpy                      /* copy string to...       */
                     ( pConvInfo->szItem           /*   this location from,   */
                     , DlgItem.szItemName          /*   this location.        */
                     );
      }
      else if (ulReply == IDC_ITEM_TERMINATE)
      {
/*
**       Load and process the "Terminate" dialog...
*/
         ulReply  = WinDlgBox
                  ( HWND_DESKTOP    /* parent window handle                   */
                  , hwnd            /* owner window handle                    */
                  , (PFNWP) DlgTerminateWindowProc
                  , 0               /* module handle (use executable          */
                  , IDD_TERMINATE   /* dialog identifier                      */
                  , NULL            /* pointer to creation data               */
                  );
/*
**       If the user responded by selecting the "Terminate" push-button...
*/
         if (ulReply == IDC_TERMINATE_OK)
         {
            fResult  = WinDdePostMsg
                     ( pConvInfo->hwndPartner
                     , hwnd
                     , WM_DDE_TERMINATE
                     , NULL
                     , DDEPM_RETRY
                     );
/*
**          If the WM_DDE_TERMINATE message was successfully posted...
*/
            if (fResult)
            {
/*
**             Set the 'state' of the conversation link to "Terminating".  In
**             this state, only the corresponding WM_DDE_TERMINATE DDE message
**             is accepted, all other DDE messages are ignored.
*/
               pConvInfo->fsState = CONVINFO_State_Terminating;
            }
         }
      }
   }
   return (MRFROMSHORT (FALSE));
}
/*==============================================================================
**
** Function:   DdeConvUmSetState
**
** Usage:      Processes a "UM_SetState" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    A "UM_SetState" message is issued to update the 'state' of
**             conversation link.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   EXPENTRY    DdeConvUmSetState
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* state / command                        */
)
{
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo = (PCONVINFO) WinQueryWindowPtr
                         ( hwnd
                         , QWL_USER
                         );
/*
** Obtain 'state' and command from message parameter...
*/
   USHORT      fsState   = SHORT1FROMMP (mp1);
   USHORT      fsCmd     = SHORT2FROMMP (mp1);

/*
** Update 'state' information as defined by the command...
*/
   if (fsCmd == SS_ADD)
   {
      pConvInfo->fsState |= fsState;
   }
   else if (fsCmd == SS_REMOVE)
   {
      pConvInfo->fsState &= (~fsState);
   }
   else if (fsCmd == SS_REPLACE)
   {
      pConvInfo->fsState = fsState;
   }
/*
** Return 'No errors'...
*/
   return (MRFROMSHORT (FALSE));
}
/*==============================================================================
**
** Function:   DdeConvWmClose
**
** Usage:      Processes a "WM_CLOSE" message dispatched to "WC_DdeConv" class
**             window.
**
** Remarks:    A "WM_CLOSE" message is posted to a "WC_DdeConv" class window
**             when the user has asked to 'Terminate' the link, or if the
**             application is terminating.
**
==============================================================================*/
   MRESULT                          /* flReply: reserved value, zero (0)     */
   EXPENTRY    DdeConvWmClose
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult   = FALSE;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** If this conversation link is currently engaged in a DDE conversation,
** post a DDE terminate transaction to its conversation partner...
*/
   if (pConvInfo->hwndPartner)
   {
      fResult  = WinDdePostMsg
               ( pConvInfo->hwndPartner
               , hwnd
               , WM_DDE_TERMINATE
               , NULL
               , DDEPM_RETRY
               );
/*
**    If the 'post' was successful, set the 'state' of this conversation link
**    to 'Terminating'.  In this state, the conversation link does not respond
**    to any further DDE transactions (other than the corresponding) terminate
**    transaction.
*/
      if (fResult)
      {
         pConvInfo->fsState = CONVINFO_State_Terminating;
      }
   }
/*
** Return reserved value...
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmCreate
**
** Usage:      Processes a "WM_CREATE" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    A WM_CREATE message is 'sent' to an application after the window
**             is created but before is it visible.  This offers the window
**             procedure an opportunity to perform initialization if any.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeConvWmCreate
(
   HWND        hwnd                 /* handle of window receiving  message    */
,  MPARAM      mp1                  /* pointer to window control data         */
,  MPARAM      mp2                  /* pointer to CREATESTRUCT structure      */
)
{
   BOOL        fResult;
   PCONVINFO   pConvInfo = (PCONVINFO) PVOIDFROMMP (mp1);

   fResult  = WinSetWindowPtr       /* set window pointer of...               */
            ( hwnd                  /*   this window at,                      */
            , QWL_USER              /*   this zero-based offset to,           */
            , pConvInfo             /*   this DDECONV structure pointer.      */
            );
/*
** If creation processing successful...
*/
   if (fResult)
   {
      return (MRFROMSHORT (FALSE)); /* no errors, continue...                 */
   }
   else
   {
      return (MRFROMSHORT (TRUE));  /* error occurred, discontinue...         */
   }
}

/*==============================================================================
**
** Function:   DdeConvWmDdeAck
**
** Usage:      Processes a "WM_DDE_ACK" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    The WM_DDE_ACK message is posted by an application in response
**             to a previous WM_DDE_EXECUTE, WM_DDE_DATA, WM_DDE_ADVISE,
**             WM_DDE_UNADVISE or WM_DDE_POKE message.  It is also possible to
**             message in response to a WM_DDE_REQUEST message.  A positive
**             WM_DDE_ACK message is never posted in response to a
**             WM_DDE_REQUEST, rather the receipt of a WM_DDE_DATA message
**             indicates a positive response.
**
**             It is possible for a WM_DDE_ACK message to contain data.  The
**             presence of data in a WM_DDE_ACK message is only valid if the
**             WM_DDE_ACK was issued in response to a previous WM_DDE_EXECUTE
**             request.  In this case the original command string is contained
**             in the data portion.  The receiving application must then
**             compare this string to any previously issued command strings in
**             order to determine which command string the server is
**             acknowledging.
**
**             Comparing command strings is the only way a client application
**             can determine which execute request the acknowledgement is for.
**             Since a WM_DDE_EXECUTE does not specify an item name.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       Typically, cbData will be zero.  However, it is legal for an
**             application to re-use the original memory object for posting
**             acknowledgements.  In this case, the memory object would contain
**             the original data.
**
**    3)       Free the DDESTRUCT structure.
**
**             NOTE: The example programs found in this book retain all DDE
**             messages for later replay, however, the location at which this
**             structure is freed is specified in the code but is commented out.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeAck
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fResult;
/*
** Obtain pointer to shared memory object containing DDESTRUCT structure...
*/
   PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) PVOIDFROMMP (mp2);
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_ACK
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** Assume WM_DDE_ACK message is negative acknowledgement...
*/
   USHORT   usStringId     = IDS_DDE_NEGATIVE_ACK;
   USHORT   fsNote         = WA_ERROR;
/*
** If the WM_DDE_ACK message is a positive acknowledgement...
*/
   if (pDdeStruct->fsStatus & DDE_FACK)
   {
      usStringId  = IDS_DDE_POSITIVE_ACK;
      fsNote      = 0;
   }
/*
** Display message information in our status area...
*/
   fResult  = FrmSetStatusText
            ( hab                   /* handle Anchor-block                   */
            , hwndStatus            /* handle of 'Status' window              */
            , NULL                  /* pointer to text string (not applicable)*/
            , usStringId            /* resource string Identifier             */
            , fsNote                /* alarm note                             */
            );
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free the
** memory object.  The example programs retain the original memory object for
** subsequent retrieval.  The following statement would normally be executed at
** this point and is shown here for completeness.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeAdvise
**
** Usage:      Processes a "WM_DDE_ADVISE" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    The WM_DDE_ADVISE message is posted by a client application to
**             request that a server application update or notify it each time
**             the specified data item changes.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       If the server is able to satisfy the request, it acknowledges
**             the request by responding with a positive WM_DDE_ACK message and
**             establishes a permanent data-link between the two applications.
**             If the server is unable to satisfy the request, it responds with
**             a negative WM_DDE_ACK message.
**
**    3)       Once such a link is established, the server advises the
**             requesting application when the data item changes by posting a
**             WM_DDE_DATA message.
**
**    4)       The WM_DDE_DATA message may or may not contain the updated data
**             item.  Subsequent WM_DDE_DATA messages contain data depending on
**             the status flag of the original WM_DDE_ADVISE message.  If the
**             DDE_FNODATA status flag was set, subsequent WM_DDE_DATA messages
**             contain no data.  This type of a data-link is commonly known as
**             a "warm-link".
**
**    5)       Upon receiving a (Warm-Link) notification, the client application
**             can either ignore the notification, or request the data item by
**             performing a one-time (WM_DDE_REQUEST) request transaction.
**
**    6)       If the DDE_FNODATA status flag was not set, subsequent
**             WM_DDE_DATA messages contain the rendered data item in the format
**             specified in the original WM_DDE_ADVISE message.  This type of
**             data-link is commonly known as a "Hot-link".
**
**    7)       If the DDE_FACKREQ status flag was set on the original
**             WM_DDE_ADVISE request, a server is required to set the
**             DDE_FACKREQ status flag on subsequent WM_DDE_DATA messages.
**             In addition, the server agrees not to post any subsequent
**             WM_DDE_DATA messages until the client application has responded
**             with a WM_DDE_ACK message.
**
**             This offers a form of flow control whereby the client application
**             can control the rate at which it receives WM_DDE_DATA messages.
**             This prevents the client's message queue from being flooded with
**             messages quicker than it can process them.
**
**    8)       It is legal for a client application to request a data item in
**             which it already has an active data-link.  An advise request for
**             a data item in which the requesting client is currently engaged
**             in a data-link is interpreted as follows:
**
**             a) If the request if for a format other than those currently
**                established, then the request is interpreted as a new request
**                and a new data-link is created.
**
**             b) If the request is for a format for which a data-link already
**                exists, the servers responds positively to the request, but
**                no new link is established.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeAdvise
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fCaseSensitive = FALSE;
   BOOL        fResult        = FALSE;
/*
** Obtain window handle of requesting application...
*/
   HWND        hwndClient     = HWNDFROMMP (mp1);
   HWND        hwndItem       = NULLHANDLE;
/*
** Query pointer to "WC_DdeAdvise" class information...
*/
   PCONVINFO   pConvInfo      = (PCONVINFO) WinQueryWindowPtr
                              ( hwnd
                              , QWL_USER
                              );
/*
** Obtain pointer to shared memory object containing DDESTRUCT structure,
** and extract the DDE format and Item name being requested...
*/
   PDDESTRUCT  pDdeStruct     = (PDDESTRUCT) PVOIDFROMMP (mp2);
   PSZ         pszItemName    = DDES_PSZITEMNAME (pDdeStruct);
   USHORT      fsStatus       = 0;
   USHORT      usFormat       = pDdeStruct->usFormat;
/*
** Set default codepage for string comparisons...
*/
   USHORT      usCodepage     = ConvContext.usCodepage;
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex     = FrmInsertListboxItem
                              ( hwndListBox
                              , WM_DDE_ADVISE
                              , ulMsgTime
                              , (ULONG) pDdeStruct
                              );
/*
** If the server application is busy, respond with a negative WM_DDE_ACK
** message indicating this state.
*/
   if (pConvInfo->fsState & CONVINFO_State_Busy)
   {
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( pszItemName     /* item name-string                       */
                  , NULL            /* pointer to data (not applicable)       */
                  , 0L              /* size of data (not applicable)          */
                  , usFormat        /* registered DDE format identifier       */
                  , DDE_FBUSY       /* DDE status flags                       */
                  );
/*
**    If shared memory object was successfully allocated...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Advise" window handle,     */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
/*
**       If posting of the WM_DDE_ACK DDE message failed...
*/
         if (fResult == FALSE)
         {
            DosFreeMem (pDdeStruct); /* free shared memory object             */
         }
      }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
      return (MRFROMSHORT (0));
   }
/*
** If a conversation context is associated with this conversation, determine
** context specific information...
*/
   if (pConvInfo->pConvContext)
   {
/*
**    Determine if string comparisons are case sensitive.  By default, all
**    strings comparisons are not case sensitive.
*/
      if (pConvInfo->pConvContext->fsContext & DDECTXT_CASESENSITIVE)
      {
         fCaseSensitive = TRUE;
      }
/*
**    Determine the codepage the requesting application is communicating in.
**    this is used to translate strings before comparisons are made.  If
**    the requesting application is using a different codepage than this
**    application, then strings are first translated to the codepage in use
**    by this application.
*/
      usCodepage = pConvInfo->pConvContext->usCodepage;
   }
/*
** Find corresponding item window by searching each descendant of our parent.
** The parent of a conversation window (a topic window) is also the parent of
** any item that might be requested by a conversation.  The architecture of
** these example programs guarantee this relationship.
*/
   hwndItem = DdeQueryItemWindow
            ( WinQueryWindow (hwnd, QW_PARENT)
            , DDES_PSZITEMNAME (pDdeStruct)
            , usCodepage
            , fCaseSensitive
            , BMSG_DESCENDANTS
            );
/*
** If a matching item window was found...
*/
   if (hwndItem)
   {
      CONVID  ConvId;

      ConvId.hwndServer = hwnd;         /* identify responding window handle  */
      ConvId.hwndClient = hwndClient;   /* identify requesting window handle  */
/*
**    Send a WM_DDE_ADVISE message to the item window.  Upon receipt of this
**    message an item window will create a data-link (Advise) window.
*/
      fResult  = (BOOL) WinSendMsg
               ( hwndItem           /* item window handle                     */
               , WM_DDE_ADVISE      /* re-use same message identifier         */
               , MPFROMP (&ConvId)  /* pointer to CONVID structure            */
               , mp2                /* pointer to DDESTRUCT structure         */
               );
   }
/*
** If 'fResult' is TRUE at this point, then the WM_DDE_ADVISE message has
** successfully been processed and we respond with a positive WM_DDE_ACK
** message.  If 'fResult' is FALSE, then an error occurred and we respond with
** a negative WM_DDE_ACK message.
*/
   fsStatus    = (fResult)
               ? DDE_FACK           /* positive acknowledgement               */
               : 0;                 /* negative acknowledgement               */
/*
** Create a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct
               ( pszItemName
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* zero (no data)                         */
               , usFormat           /* registered DDE format identifier       */
               , fsStatus           /* DDE status flags                       */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndClient         /*   this requesting window handle,       */
               , hwnd               /*   from the "Advise" window handle,     */
               , WM_DDE_ACK         /*   an "Acknowledgement" message,        */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
   }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeExecute
**
** Usage:      Processes a "WM_DDE_EXECUTE" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    The WM_DDE_EXECUTE message is posted by a client to request one
**             or more commands be executed. If the server is able, it attempts
**             to execute the string of command(s).  If the server is
**             successful, it posts a positive WM_DDE_ACK message in response.
**
**             If the executing fails, or if the server is unable to execute
**             the commands, a negative WM_DDE_ACK message is posted in
**             response.  In either case, the original command string is
**             included in the data portion of the message.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       The command string must be some agreed upon protocol.
**
**    3)       A server should provide a list of supported protocols
**             (command strings).  A list of supported protocols should be made
**             available by a server application through the
**             SZDDESYS_ITEM_PROTOCOLS item of the System topic.
**
**    4)       Post a WM_DDE_ACK message to the requesting application.
**
**             a) If the server is busy and cannot respond, the the DDE_FBUSY
**                status flag must be set in the WM_DDE_ACK message.
**
**    5)       Free or re-use the DDESTRUCT structure.
**
**             NOTE: The example programs found in this book retain all DDE
**             messages for later replay.  Typically, the original DDESTRUCT
**             structure is reused since it already contain the necessary
**             acknowledgement information.
**
** Syntax
** Conventions:
**
**    1)       The left bracket ( [ ) denotes a single syntax element.
**
**    2)       Optional syntax elements are enclosed in a pair of
**             curly-brackets {}.
**
**    3)       An ellipse (...) denotes multiple occurrences of the element are
**             possible.
**
** Syntax:
**
**    a)       A command string is a null (0x00) terminated string.  If the
**             data item contains more than one command string, the entire data
**             item is null (0x00) terminated.  In other words the data item is
**             double null terminated.
**
**             A command string is defined as follows:
**
**                Command string := [opcodestring] { [opcodestring] } ...
**
**    b)       A command string contains one or more opcodestrings.  A
**             opcodestring is one or more optional parameters and is defined
**             as follows:
**
**                opcodestring :=  opcode { ( parameter { , parameter } ... ) }
**
**    c)       An opcode is any application defined single token.  It may not
**             include spaces, commas, parentheses, or quotation marks.
**
**    d)       A parameter is any application defined value.  Multiple
**             parameters are separated by commas, and the entire parameter
**             list is enclosed in parentheses.  The parameter may not include
**             commas or parentheses except inside a quoted string.  If a
**             bracket or parentheses character is required in a quoted string,
**             it must be doubled as follows: '(('.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeExecute
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fResult;
   BOOL        fCaseSensitive = FALSE;
/*
** Obtain DDE transaction information...
*/
   HWND        hwndClient     = HWNDFROMMP  (mp1);
   PDDESTRUCT  pDdeStruct     = PVOIDFROMMP (mp2);
   PBYTE       pbCommands     = DDES_PABDATA (pDdeStruct);
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo      = (PCONVINFO) WinQueryWindowPtr
                              ( hwnd
                              , QWL_USER
                              );
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_EXECUTE
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** Buffer to hold converted commands (if required)...
*/
   CHAR        szCpCommands      [MAX_TEXTLENGTH];
/*
** If the server application is busy, respond with a negative WM_DDE_ACK
** message indicating this state.
*/
   if (pConvInfo->fsState & CONVINFO_State_Busy)
   {
/*
**    A WM_DDE_ACK message in response to a WM_DDE_EXECUTE request must contain
**    the original command string(s).  This is accomplished by re-using the
**    original DDESTRUCT structure as follows:
*/
      pDdeStruct->fsStatus = DDE_FBUSY;

      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndClient         /*   this requesting window handle,       */
               , hwnd               /*   from the "Advise" window handle,     */
               , WM_DDE_ACK         /*   an "Acknowledgement" message,        */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full,*/
               | DDEPM_NOFREE       /*   but do not free the memory object.   */
               );

      return (MRFROMSHORT (0));
   }
/*
** If a conversation context is associated with this conversation, determine
** context specific information.
*/
   if (pConvInfo->pConvContext)
   {
/*
**    Determine if string comparisons are case sensitive.  By default, all
**    strings comparisons are not case sensitive.
*/
      if (pConvInfo->pConvContext->fsContext & DDECTXT_CASESENSITIVE)
      {
         fCaseSensitive = TRUE;
      }
/*
**    If the current codepage is not equal to the conversation codepage,
**    translate the command string from the conversation codepage to the
**    current codepage.
*/
      if (ConvContext.usCodepage != pConvInfo->pConvContext->usCodepage)
      {
         fResult  = WinCpTranslateString
                  ( hab
                  , pConvInfo->pConvContext->usCodepage
                  , (PSZ) pbCommands
                  , ConvContext.usCodepage
                  , MAX_TEXTLENGTH
                  , (PSZ) szCpCommands
                  );

         if (fResult)
         {
            pbCommands = szCpCommands;
         }
      }
   }
/*
** Send request to our owner for command execution.
*/
   fResult  = (BOOL) WinSendMsg
            ( WinQueryWindow (hwnd, QW_OWNER)
            , UM_ExecuteCommand
            , MPFROMSHORT (fCaseSensitive)
            , MPFROMP (pbCommands)
            );
/*
** Set DDE status flag to reflect the results of command execution...
*/
   if (fResult)
   {
      pDdeStruct->fsStatus = DDE_FACK; /* positive acknowledgement            */
   }
   else
   {
      pDdeStruct->fsStatus = 0;        /* negative acknowledgement            */
   }
/*
** A WM_DDE_ACK message in response to a WM_DDE_EXECUTE request must contain
** the original command string(s).  This is accomplished by re-using the
** original DDESTRUCT structure...
*/
   fResult  = WinDdePostMsg         /* post a DDE message to...               */
            ( HWNDFROMMP (mp1)      /*   this requesting window handle,       */
            , hwnd                  /*   from the "Advise" window handle,     */
            , WM_DDE_ACK            /*   an "Acknowledgement" message,        */
            , pDdeStruct            /*   as defined in this memory object,    */
            , DDEPM_RETRY           /*   and retry if receiving queue is full,*/
            | DDEPM_NOFREE          /*   do not free memory object.           */
            );

   return (MRFROMSHORT (0));
}
/*==============================================================================
** Function:   DdeConvWmDdePoke
**
** Usage:      Processes a "WM_DDE_POKE" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    A WM_DDE_POKE message is posted by a client application
**             requesting that a server accept an unsolicited data item.  If
**             the server can accept the data item in the format specified, it
**             updates the data item and responds by posting a positive
**             WM_DDE_ACK message.  If the server cannot accept the data item
**             it responds with a negative WM_DDE_ACK message.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       Determine if it can update the data item. If it can, respond
**             with a positive WM_DDE_ACK message.  If it cannot, respond with
**             a negative WM_DDE_ACK message.  In either case, the response is
**             issued after completion of the operation.
**
**    3)       Free or re-use the DDESTRUCT structure.
**
**             NOTE: The example programs found in this book retain all DDE
**             messages for later replay.  Typically, the original DDESTRUCT
**             structure is reused since it already contain the necessary
**             acknowledgement information.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdePoke
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fResult     = TRUE;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain DDE transaction information...
*/
   HWND        hwndClient  = HWNDFROMMP (mp1);
   PDDESTRUCT  pDdeStruct  = PVOIDFROMMP (mp2);
   PSZ         pszItemName = (PSZ) DDES_PSZITEMNAME (pDdeStruct);
   USHORT      usFormat    = pDdeStruct->usFormat;
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_POKE
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** If the server application is busy, respond with a negative WM_DDE_ACK
** message indicating this state.
*/
   if (pConvInfo->fsState & CONVINFO_State_Busy)
   {
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( pszItemName     /* item name-string                       */
                  , NULL            /* pointer to data (not applicable)       */
                  , 0L              /* size of data (not applicable)          */
                  , usFormat        /* registered DDE format identifier       */
                  , DDE_FBUSY       /* DDE status flags                       */
                  );
/*
**    If shared memory object successfully created...
*/
      if (pDdeStruct)
      {
         fResult = WinDdePostMsg    /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Conv" window handle,       */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
/*
**       If the 'post' failed, free shared memory object...
*/
         if (fResult == FALSE)
         {
            DosFreeMem (pDdeStruct);
         }
      }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
      return (MRFROMSHORT (0));
   }
/*
** Create a shared memory-object to post acknowledgement to requesting
** application.  For the purpose of this example program we accept all Pokes.
** We simple display the data, no actual updating occurs.  If updates were
** required, it would done here before the acknowledgement was posted.
*/
   pDdeStruct  = DdeMakeDdeStruct
               ( pszItemName        /* item name-string                       */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (not applicable)          */
               , usFormat           /* registered DDE format                  */
               , DDE_FACK           /* positive acknowledgement               */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndClient         /*   this requesting window handle,       */
               , hwnd               /*   from the "Conv" window handle,       */
               , WM_DDE_ACK         /*   an "Acknowledgement" message,        */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
   }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeUnAdvise
**
** Usage:      Processes a "WM_DDE_UNADVISE" message dispatched to a
**             "WC_DdeConv" class window.
**
** Remarks:    The WM_DDE_UNADVISE message is posted by a client application to
**             notify a server application that it should discontinue posting
**             updates for a previously established data-link in the specified
**             format for the specified item.
**
**             It is possible that a client application has established more
**             an one data-link with a server at a time.  A client application
**             does not need to terminate each data-link with a separate
**             request.  That is, updates can be discontinued for multiple
**             items and formats with a single request.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       Determine the scope and degree of the request.  The scope is
**             determined by the presence of a item name-string and the degree
**             is determined by the value contained in the usFormat field in
**             the DDESTRUCT structure.
**
**             a) If the item name-string is zero-length, then the scope is
**                all links associated with the conversation.  Otherwise, the
**                scope is limited to a specific item-name.
**
**             b) If the contents of the usFormat field of the DDESTRUCT
**                structure is zero (0), then the degree is all formats
**                associated with the conversation.  Otherwise, the degree is
**                limited to a specific format.
**
**             Given the above criteria, the following combinations are
**             possible:
**
**             a) If item name-string is a non zero-length string and a non
**                zero format identifier was specified, then the request is to
**                discontinue updates for a specific item and format.
**
**             b) if item name-string is a non zero-length string and zero was
**                specified for the format identifier, then the request is to
**                discontinue updates for all formats corresponding to the
**                specified item.
**
**             c) if item name-string is a zero-length string and a non zero
**                format identifier was specie, then the request is to
**                discontinue updates for a specific format in all items
**                associated in a conversation.
**
**             d) if item name-string is a zero-length string and zero was
**                specified for the format identifier, then the request is to
**                discontinue updates for all formats and all items associated
**                in a conversation.
**
**    3)       Determine if the requesting client currently had a data-link
**             established.  If it did, respond with a positive WM_DDE_ACK
**             message after completion of the unadvise operation.  If the
**             requesting client did not have an established data-link, respond
**             with a negative WM_DDE_ACK message.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeUnAdvise
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fResult     = FALSE;
/*
** Determine the scope of the request.  Initially, the scope is defined to be
** all data items.  This is established by setting hwndScope to be the parent
** of this conversation window.  In a server configuration, the parent of a
** conversation window is a topic window.  A topic window is always the
** parent of an item window.  Therefore any and all items associated with a
** particular conversation will share a common parent with a conversation
** window.
*/
   HWND        hwndScope   = WinQueryWindow
                           ( hwnd
                           , QW_PARENT
                           );
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain and set DDE transaction information...
*/
   HWND        hwndClient  = HWNDFROMMP  (mp1);
   PDDESTRUCT  pDdeStruct  = PVOIDFROMMP (mp2);
   PSZ         pszItemName = (PSZ) DDES_PSZITEMNAME (pDdeStruct);
   USHORT      usFormat    = pDdeStruct->usFormat;
   USHORT      fsStatus    = 0;
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_UNADVISE
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** The DDE protocol states that if no link existed for the specified
** item/format pair then the server must respond with a negative WM_DDE_ACK
** message.  To determine the success or failure of this request, the global
** variable (fDataLinkExisted) is set to FALSE.  Later, when the
** UM_UnAdviseItem message is broadcasted to each WC_DdeAdvise class window,
** the 'Advise' window will set this flag to TRUE.  Finally, the state of this
** flag will be checked and either a positive or negative WM_DDE_ACK message
** will be posted in response.
*/
   fDataLinkExisted = FALSE;
/*
** If the ItemName string is a non zero-length string, then this request is for
** a specific item.
*/
   if (strlen (pszItemName))
   {
/*
**    Set default query parameters: String comparisons are not case sensitive,
**    and are in the same codepage as the server.
*/
      BOOL     fCaseSensitive = FALSE;
      USHORT   usCodepage     = ConvContext.usCodepage;

/*
**    If a conversation context is associated with this conversation,
**    determine context specific information.
*/
      if (pConvInfo->pConvContext)
      {
/*
**       Determine if string comparisons are case sensitive.  By default, all
**       strings comparisons are not case sensitive.
*/
         if (pConvInfo->pConvContext->fsContext & DDECTXT_CASESENSITIVE)
         {
            fCaseSensitive = TRUE;
         }
/*
**       Determine the codepage the requesting application is using.
**       This is used to translate strings before comparisons are made.  If
**       the requesting application is using a different codepage than this
**       application, then strings are first translated to the codepage in use
**       by this application.
*/
         usCodepage = pConvInfo->pConvContext->usCodepage;
      }
/*
**    See if an item window exists whose text matches the item name-string
**    contained in the request.
*/
      hwndScope   = DdeQueryItemWindow
                  ( WinQueryWindow (hwnd, QW_PARENT)
                  , pszItemName
                  , usCodepage
                  , fCaseSensitive
                  , BMSG_DESCENDANTS
                  );
   }
/*
** At this point the scope of the request has been determined.  Now, the
** degree to which data-links are terminated is dependant on the value
** specified in the usFormat field of the DDESTRUCT structure.
*/
   if (hwndScope)
   {
      fResult  = WinBroadcastMsg       /* broadcast a message to,             */
               ( hwndScope             /* the children of this parent window  */
               , UM_UnAdviseItem       /* message identifier                  */
               , MPFROMHWND  (hwnd)    /* message parameter one (mp1)         */
               , MPFROMSHORT (usFormat)/* message parameter two (mp2)         */
               , BMSG_SEND             /* send the message to all children    */
               | BMSG_DESCENDANTS      /* and their descendants.              */
               );
   }
/*
** If 'unadvise' transaction successfully processed...
*/
   if (fDataLinkExisted)
   {
      fsStatus = DDE_FACK;
   }
/*
** Create a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct
               ( pszItemName        /* item name-string                       */
               , NULL               /* pointer to data (not applicable        */
               , 0L                 /* size of data (not applicable)          */
               , usFormat           /* Registered DDE format identifier       */
               , fsStatus           /* DDE status flags                       */
               );
/*
** If shared memory object successfully created...
*/
   fResult     = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndClient         /*   this requesting window handle,       */
               , hwnd               /*   from the "Conv" window handle,       */
               , WM_DDE_ACK         /*   an "Acknowledgement" message,        */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeData
**
** Usage:      Process a "WM_DDE_DATA" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    The WM_DDE_DATA message is posted by a server application in
**             response to a previous WM_DDE_REQUEST message, or as an update
**             to a previously established (WM_DDE_ADVISE) data-link.  It is
**             possible, and valid for the WM_DDE_DATA message to contain no
**             data.  A WM_DDE_DATA message containing no data is considered to
**             be a notification rather than update. Notifications, like
**             updates are posted by the server when a previously established
**             data-link has changed.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       If the DDE_FACKREQ flag is present in the fsStatus flag, the
**             receiving application is required to post a WM_DDE_ACK message
**             in response. If this is not present, the receiving application
**             posts no response. the response (if any) is posted after the
**             application has processed the WM_DDE_DATA message.
**
**    3)       Free or re-use the DDESTRUCT structure.
**
**             NOTE: The example programs found in this book retain all DDE
**             messages for later replay, however, the location at which this
**             structure is freed is specified in the code but is commented out.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeData
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of window originating message   */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
   BOOL        fResult     = FALSE;
/*
** Obtain and set DDE transaction information...
*/
   HWND        hwndClient  = HWNDFROMMP (mp1);
   PDDESTRUCT  pDdeStruct  = PVOIDFROMMP (mp2);
   PSZ         pszItemName = (PSZ) DDES_PSZITEMNAME (pDdeStruct);
   USHORT      usFormat    = pDdeStruct->usFormat;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
   HWND        hwndOwner   = WinQueryWindow
                           ( hwnd
                           , QW_OWNER
                           );
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_DATA
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** If the DDE_FNODATA flag is on, then the message is considered to be a
** notification that data is available.  Here we simply display a message in
** the status window indicating data is available.  This example program
** processes the WM_DDE_DATA message by inserting it into a Listbox for later
** replay and display.
*/
   if (pDdeStruct->fsStatus & DDE_FNODATA)
   {
      fResult  = FrmSetStatusText
               ( hab
               , hwndStatus
               , NULL
               , IDS_DDE_NODATA
               , WA_NOTE
               );
   }
/*
** If the DDE_FACKREQ flag is present in the fsStatus flag, the receiving
** application is expected to post a WM_DDE_ACK message in response.  If this
** flag is not present, the receiving application should not post any
** response. The response (if any) is posted after processing the message.
*/
   if (pDdeStruct->fsStatus & DDE_FACKREQ)
   {
      pDdeStruct  = DdeMakeDdeStruct
                  ( pszItemName     /* requested item name-string             */
                  , NULL            /* pointer to data (not applicable)       */
                  , 0L              /* size of data (not applicable) set to 0 */
                  , usFormat        /* requested format                       */
                  , DDE_FACK        /* positive acknowledgement               */
                  );
/*
**    If shared memory object successfully created...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Conv" window handle,       */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
      }
   }
/*
** The receiving application is responsible for freeing the DDE memory object.
** These example programs retain this information for subsequent replay.  A
** pointer to this memory object is stored in its corresponding listbox entry
** and is freed when the listbox entry is deleted.  The following API is
** listed here for completeness of the example code and should be uncommented
** if used by an application that does not need to preserve this data.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeInitiateAck
**
** Usage:      Processes a "WM_DDE_INITIATEACK" message dispatched to a
**             "WC_DdeConv" class window.
**
** Remarks:    A "WM_DDE_INITIATEACK" message is sent when the WinDdeRespond
**             API is issued.
**
** Guidelines:
**
**    1)       The value returned from processing this message is the value
**             returned to the (server) application that issued the
**             WinDdeRespond API.  A return code of FALSE indicates that the
**             receiving application can not or does not want to engage in the
**             conversation.
**
**    2)       The receiving application is expected to post a WM_DDE_TERMINATE
**             message for all unwanted conversations regardless of the value
**             returned by processing this message.
**
**============================================================================*/
   MRESULT                          /* fResult: Error indicator               */
   APIENTRY    DdeConvWmDdeInitiateAck
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle of requesting application*/
,  MPARAM      mp2                  /* pointer to a DDEINIT structure         */
)
{
   BOOL         fCaseSensitive = FALSE;
   BOOL         fResult        = FALSE;
/*
** Obtain and set DDE transaction information...
*/
   HWND         hwndServer    = HWNDFROMMP (mp1);
   PDDEINIT     pDdeInit      = (PDDEINIT) PVOIDFROMMP (mp2);
   PSZ          pszAppName    = pDdeInit->pszAppName;
   PSZ          pszTopic      = pDdeInit->pszTopic;
   PCONVCONTEXT pConvContext  = DDEI_PCONVCONTEXT (pDdeInit);
   CHAR         szCpAppName [MAX_TEXTLENGTH];
   CHAR         szCpTopic   [MAX_TEXTLENGTH];
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO    pConvInfo     = (PCONVINFO) WinQueryWindowPtr
                              ( hwnd
                              , QWL_USER
                              );
/*
** Obtain and set menu information...
*/
   HWND         hwndSubMenu   = NULLHANDLE;
   MENUITEM     mi;
   SHORT        sItemCount    = 0;
   SHORT        sItemId       = 0;
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT        sItemIndex    = FrmInsertListboxItem
                              ( hwndListBox
                              , WM_DDE_INITIATEACK
                              , ulMsgTime
                              , (ULONG) pDdeInit
                              );
/*
** If the state of this window is enabled, then it is already engaged in a DDE
** conversation. Once engaged, any subsequent acknowledgements are terminated.
*/
   if (WinIsWindowEnabled (hwnd))
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwnd               /*   from the "Conv" window handle,       */
               , WM_DDE_TERMINATE   /*   an "Acknowledgement" message,        */
               , NULL               /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
/*
**    The receiver of a DDE message is responsible for the memory object
**    associated with a DDE message.  The receiver must either reuse or free
**    the memory object.  The example programs retain the original memory
**    object for subsequent retrieval.  The following statement would normally
**    be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
      return  (MRFROMSHORT (FALSE));
   }
/*
** If the 'state' of the conversation link is 'none' (its initial state), then
** then compare Application and Topic strings in the response to that of the
** conversation link.
*/
   if (pConvInfo->fsState == CONVINFO_State_None)
   {
/*
**    If a conversation context is associated with this conversation,
**    determine context specific information.
*/
      if (pDdeInit->offConvContext)
      {
/*
**       Determine if string comparisons are case sensitive.  By default,
**       all strings comparisons are not case sensitive.
*/
         fCaseSensitive = pConvContext->fsContext & DDECTXT_CASESENSITIVE;
/*
**       If the current codepage is not equal to the conversation codepage,
**       translate the AppName string from the conversation codepage to the
**       current codepage.
*/
         if (pConvContext->usCodepage != ConvContext.usCodepage)
         {
            fResult  = WinCpTranslateString
                     ( hab
                     , pConvContext->usCodepage
                     , pDdeInit->pszAppName
                     , ConvContext.usCodepage
                     , MAX_TEXTLENGTH
                     , szCpAppName
                     );
/*
**          If translation successful, adjust AppName pointer to translated
**          string...
*/
            if (fResult)
            {
               pszAppName = szCpAppName;
            }
/*
**          If the current codepage is not equal to the conversation codepage,
**          translate the Topic string from the conversation codepage to the
**          current codepage.
*/
            fResult  = WinCpTranslateString
                     ( hab
                     , pConvContext->usCodepage
                     , pDdeInit->pszTopic
                     , ConvContext.usCodepage
                     , MAX_TEXTLENGTH
                     , szCpTopic
                     );
/*
**          If translation successful, adjust Topic pointer to translated
**          string...
*/
            if (fResult)
            {
               pszTopic = szCpTopic;
            }
         }
      }
/*
**
*/
      fResult  = DdeCompareStrings
               ( pConvInfo->szAppName
               , pszAppName
               , fCaseSensitive
               , ConvContext.idCountry
               , ConvContext.usCodepage
               );
/*
**    If Application name-strings matched, compare Topic strings...
*/
      if (fResult)
      {
         fResult  = DdeCompareStrings
                  ( pConvInfo->szTopic
                  , pszTopic
                  , fCaseSensitive
                  , ConvContext.idCountry
                  , ConvContext.usCodepage
                  );
/*
**       If Topic strings matched...
*/
         if (fResult)
         {
/*
**          Set the 'state' of the conversation link to 'Alive' and set the
**          window handle of the responding application...
*/
            pConvInfo->fsState     = CONVINFO_State_Alive;
            pConvInfo->hwndPartner = hwndServer;
/*
**          Notify our 'owner' of the acknowledgement so that it can add it to
**          the 'Servers' menu item...
*/
            fResult  = (BOOL) WinSendMsg
                     ( WinQueryWindow (hwnd, QW_PARENT)
                     , UM_InitiateAck
                     , MPFROMP (pConvInfo->szAppName)
                     , MPFROMP (pConvInfo->szTopic)
                     );
/*
**          Enable conversation link window...
*/
            fResult  = WinEnableWindow    /* sets enabled state of...         */
                     ( hwnd               /*    this window to...             */
                     , TRUE               /*    enabled.                      */
                     );
         }
      }
   }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (TRUE));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeRequest
**
** Usage:      Processes a "WM_DDE_REQUEST" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    The WM_DDE_REQUEST message is posted by a client application to
**             request that a server application provide a data item rendered
**             in the specified format.  This type of a request is commonly
**             known as a "Cold Link".
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       If the server is able to satisfy the request, it acknowledges
**             the request by responding with a WM_DDE_DATA message containing
**             the rendered data item.  If the server is unable to satisfy the
**             request, it responds with a negative WM_DDE_ACK message.
**
**    3)       Status flags are undefined in the context of this message.  If
**             present, their meaning is not defined by the protocol.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   APIENTRY    DdeConvWmDdeRequest
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* originator window handle               */
,  MPARAM      mp2                  /* pointer to a DDESTRUCT structure       */
)
{
   BOOL        fCaseSensitive = FALSE;
   BOOL        fResult        = FALSE;
/*
** Obtain and set DDE transaction information...
*/
   HWND        hwndClient  = HWNDFROMMP (mp1);
   PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) PVOIDFROMMP (mp2);
   PSZ         pszItemName = DDES_PSZITEMNAME (pDdeStruct);
   USHORT      usFormat    = pDdeStruct->usFormat;
   USHORT      usCodepage  = ConvContext.usCodepage;
   HWND        hwndItem    = NULLHANDLE;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
  SHORT       sItemIndex   = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_REQUEST
                           , ulMsgTime
                           , (ULONG) pDdeStruct
                           );
/*
** If the server application is busy, respond with a negative WM_DDE_ACK
** message indicating this state.
*/
   if (pConvInfo->fsState & CONVINFO_State_Busy)
   {
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( pszItemName     /* item name-string                       */
                  , NULL            /* pointer to data (not applicable)       */
                  , 0L              /* size of data (not applicable)          */
                  , usFormat        /* requested format identifier            */
                  , DDE_FBUSY       /* status flags                           */
                  );
/*
**    If shared memory object successfully created...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Conv" window handle,       */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
/*
**       If 'post' failed...
*/
         if (fResult == FALSE)
         {
            DosFreeMem (pDdeStruct);/*   free the shared memory object.       */
         }
      }
/*
**    The receiver of a DDE message is responsible for the memory object
**    associated with a DDE message.  The receiver must either reuse or free
**    the memory object.  The example programs retain the original memory
**    object for subsequent retrieval.  The following statement would normally
**    be executed at this point and is shown here for completeness.
**
**    DosFreeMem (PVOIDFROMMP (mp2));
*/
      return (MRFROMSHORT (0));
   }
/*
** If a conversation context is associated with this conversation,
** determine context specific information...
*/
   if (pConvInfo->pConvContext)
   {
/*
**    Determine if string comparisons are case sensitive.  By default,
**    all strings comparisons are not case sensitive.
*/
      if (pConvInfo->pConvContext->fsContext & DDECTXT_CASESENSITIVE)
      {
         fCaseSensitive = TRUE;
      }
/*
**    Determine the codepage the requesting application is using.
**    This is used to translate strings before comparisons are made.  If
**    the requesting application is using a different codepage than this
**    application, then strings are first translated to the codepage in use
**    by this application.
*/
      usCodepage = pConvInfo->pConvContext->usCodepage;
   }
/*
** Query corresponding 'Item' window...
*/
   hwndItem = DdeQueryItemWindow
            ( WinQueryWindow (hwnd, QW_PARENT)
            , DDES_PSZITEMNAME (pDdeStruct)
            , usCodepage
            , fCaseSensitive
            , BMSG_DESCENDANTS
            );
/*
** If 'Item' window found...
*/
   if (hwndItem)
   {
      pDdeStruct  = (PDDESTRUCT) WinSendMsg
                  ( hwndItem
                  , UM_RenderItem
                  , MPFROM2SHORT (usFormat, DDE_FRESPONSE)
                  , MPVOID
                  );
/*
**    If data item successfully rendered, respond to requesting application...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Conv" window handle,       */
                  , WM_DDE_DATA     /*   an "Data" message,                   */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
/*
**       If 'post' failed...
*/
         if (fResult == FALSE)
         {
            DosFreeMem (pDdeStruct);/*    free shared memory object           */
         }
      }
   }
/*
** If request failed...
*/
   if (fResult == FALSE)
   {
/*
**    Create a DDE shared memory object...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( pszItemName     /* item name-string                       */
                  , NULL            /* pointer to data (not applicable)       */
                  , 0L              /* size of data (not applicable)          */
                  , usFormat        /* DDE format identifier                  */
                  , 0               /* DDE status flags                       */
                  );
/*
**    If shared memory object successfully created...
*/
      if (pDdeStruct)
      {
         fResult  = WinDdePostMsg   /* post a DDE message to...               */
                  ( hwndClient      /*   this requesting window handle,       */
                  , hwnd            /*   from the "Conv" window handle,       */
                  , WM_DDE_ACK      /*   an "Acknowledgement" message,        */
                  , pDdeStruct      /*   as defined in this memory object,    */
                  , DDEPM_RETRY     /*   and retry if receiving queue is full.*/
                  );
/*
**       If 'post' failed, free memory object...
*/
         if (fResult == FALSE)
         {
            DosFreeMem (pDdeStruct);
         }
      }
   }
/*
** The receiver of a DDE message is responsible for the memory object
** associated with a DDE message.  The receiver must either reuse or free
** the memory object.  The example programs retain the original memory
** object for subsequent retrieval.  The following statement would normally
** be executed at this point and is shown here for completeness.
**
** DosFreeMem (PVOIDFROMMP (mp2));
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDdeTerminate
**
** Usage:      Processes a "WM_DDE_TERMINATE" message dispatched to a
**             "WC_DdeConv" class window.
**
** Remarks:    A "WM_DDE_TERMINATE" message is an agreement (a promise) that
**             the originator will post no further DDE messages.  The receiver
**             is free to destroy any resources that have been created for the
**             conversation link.  For example, windows and structures.
**
**             The receiving application is expected to respond to this message
**             by promptly posting a "WM_DDE_TERMINATE" message in response.
**             It is not legal to post a positive, negative, or busy
**             "WM_DDE_ACK" message in response to a 'Terminate' request.
**
** Guidelines:
**
**    1)       Any message posted in response should be to the window handle
**             specified in message parameter one (mp1).
**
**    2)       A "WM_DDE_TERMINATE" message can be issued by either the client
**             or the server application at any time.
**
**    3)       An applications must be able to process the "WM_DDE_TERMINATE"
**             message at any time.  That is, it is not permissible to respond
**             with a negative, busy or positive "WM_DDE_ACK" message.
**
**    4)       Before an application terminates, it must terminate any and all
**             active data and conversation links.  Failure to terminate these
**             links will prohibit applications engaged in conversation from
**             discarding resources associated with such links.
**
**    5)       The originator agrees not to issue any further transactions
**             associated with this conversation link.
**
**    6)       Once issued an application should ignore any subsequent messages
**             from this conversation partner.  Additional messages are
**             possible if messages where posted prior to the receipt of a
**             "WM_DDE_TERMINATE" message.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDdeTerminate
(
   HWND        hwnd                 /* handle of window this message          */
,  MPARAM      mp1                  /* window handle originating message      */
)
{
   BOOL        fResult     = FALSE;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Insert message and associated information into listbox.
**
** Note: Typically, an application receiving a DDE message will free the
**       associated shared memory object after processing this message (as
**       shown at the end of this function.  However, we save the original
**       shared memory object for subsequent retrieval by the user.  The
**       shared memory object is freed when the listbox is destroyed or
**       cleared.
*/
   SHORT       sItemIndex  = FrmInsertListboxItem
                           ( hwndListBox
                           , WM_DDE_TERMINATE
                           , ulMsgTime
                           , 0L
                           );
/*
** Promptly respond by 'posting' a "WM_DDE_TERMINATE" message...
*/
   fResult  = WinDdePostMsg         /* post a DDE message to...               */
            ( HWNDFROMMP (mp1)      /*   this requesting window handle,       */
            , hwnd                  /*   from the "Conv" window handle,       */
            , WM_DDE_TERMINATE      /*   an "Terminate" message,              */
            , NULL                  /*   as defined in this memory object,    */
            , DDEPM_RETRY           /*   and retry if receiving queue is full.*/
            );
/*
** Notify all child (WC_DdeItem and WC_DdeAdvise class) windows to 'terminate'
** their data-links...
**
** Note: The 'parent' of a "WC_DdeConv" class window, is also the 'parent'
**       of 'Topics' and 'Items' associated with the conversation link. This
**       relationship is guaranteed by the design of the example programs.
*/
   fResult  = WinBroadCastMsg
            ( WinQueryWindow (hwnd, QW_PARENT)
            , UM_UnAdviseItem
            , MPFROMHWND (hwnd)
            , MPFROMSHORT (0)
            , BMSG_SEND
            | BMSG_DESCENDANTS
            );
/*
** Notify our 'owner' the this conversation link terminated...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndClient
            , UM_Terminate
            , MPFROMP (pConvInfo->szAppName)
            , MPFROMP (pConvInfo->szTopic)
            );
/*
** Finally, destroy this conversation link window and all its children...
*/
   fResult  = WinDestroyWindow
            ( hwnd                  /* handle of window to destroy            */
            );

   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeConvWmDestroy
**
** Usage:      Processes a "WM_DESTROY" message dispatched to a "WC_DdeConv"
**             class window.
**
** Remarks:    A "WM_DESTROY" message is sent to a window after it has been
**             hidden offering it an opportunity to perform some termination
**             action for this window.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   EXPENTRY    DdeConvWmDestroy
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult     = FALSE;
/*
** Obtain "WC_DdeConv" window class info...
*/
   PCONVINFO   pConvInfo   = (PCONVINFO) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain pointer to conversation context structure...
**
*/
   PCONVCONTEXT pConvContext = pConvInfo->pConvContext;
/*
** If this conversation link is engaged, then notify the DDE conversation
** partner that it is terminating...
*/
   if (pConvInfo->fsState == CONVINFO_State_Alive
   ||  pConvInfo->fsState == CONVINFO_State_Busy)
   {
      fResult  = WinDdePostMsg
               ( pConvInfo->hwndPartner
               , hwnd
               , WM_DDE_TERMINATE
               , NULL
               , DDEPM_RETRY
               );
   }
/*
** If a conversation context structure exists...
*/
   if (pConvContext)
   {
      fResult  = DdeFreeMem         /* free the context structure             */
               ( pConvContext       /* address of context structure           */
               , pConvContext->cb   /* count bytes of context structure       */
               );
   }
/*
** Finally, free the "WC_DdeConv" class information...
*/
   fResult  = DdeFreeMem            /* free the information structure         */
            ( pConvInfo             /* address of information structure       */
            , pConvInfo->cb         /* count bytes of information structure   */
            );

  return (MRFROMLONG (0L));
}
/*==============================================================================
**
** Function:   DdeCreateAdviseWindow
**
** Usage:      Creates a "WC_DdeAdvise" class window.
**
==============================================================================*/
   HWND                             /* hwndAdviseWindow                       */
   EXPENTRY    DdeCreateAdviseWindow
(
   HWND        hwndParent           /* handle of parent window                */
,  HWND        hwndOwner            /* handle of owner window                 */
,  PCONVID     pConvId              /* conversation identification            */
,  USHORT      fsStatus             /* DDE status flags                       */
,  USHORT      usFormatId           /* DDE format identifier                  */
)
{
   ULONG       cbWindowText   = 0;
   HWND        hwndAdvise     = NULLHANDLE;
/*
** Allocate memory for "WC_DdeAdvise" class information...
*/
   PDDEADVISE  pDdeAdvise     = DdeAllocMem
                              ( sizeof (DDEADVISE)
                              );
/*
** If memory allocation successful...
*/
   if (pDdeAdvise)
   {
      pDdeAdvise->cb          = sizeof (DDEADVISE);
      pDdeAdvise->fAckPending = FALSE;
      pDdeAdvise->fsStatus    = fsStatus;
      pDdeAdvise->hwndClient  = pConvId->hwndClient;
      pDdeAdvise->hwndConv    = pConvId->hwndServer;

      hwndAdvise  = WinCreateWindow
                  ( hwndParent      /* handle of parent window                */
                  , WC_DdeAdvise    /* window class name                      */
                  , NULL            /* window text string (not applicable)    */
                  , WS_DISABLED     /* window style (initially disabled)      */
                  , 0L              /* horizontal (x position)                */
                  , 0L              /* vertical   (y portion)                 */
                  , 0L              /* width      (cx)                        */
                  , 0L              /* height     (cy)                        */
                  , hwndOwner       /* handle of owner window                 */
                  , HWND_TOP        /* place on TOP of Z-order                */
                  , usFormatId      /* window Id (DDE format identifier)      */
                  , pDdeAdvise      /* creation data                          */
                  , NULL            /* presentation parameters (not used)     */
                  );
   }
/*
** Return results of window creation...
*/
   return (hwndAdvise);
}
/*==============================================================================
**
** Function:   DdeCreateConvWindow
**
** Usage:      Creates a "WC_DdeConv" class window.
**
==============================================================================*/
   HWND                             /* hwndConvWindow                         */
   APIENTRY    DdeCreateConvWindow
(
   HWND        hwndParent           /* handle of parent window                */
,  HWND        hwndPartner          /* handle of DDE conversation partner     */
,  PSZ         pszAppName           /* application name-string                */
,  PSZ         pszTopic             /* topic string                           */
,  PCONVCONTEXT pConvContext        /* conversation context structure         */
)
{
   HWND        hwndConv      = NULLHANDLE;
   PSZ         pResult       = NULL;
/*
** Allocate memory for "WC_DdeConv" class information...
*/
   PCONVINFO   pConvInfo     = DdeAllocMem
                             ( sizeof (CONVINFO)
                             );
/*
** If memory allocation successful...
*/
   if (pConvInfo)
   {
      pConvInfo->cb           = sizeof (CONVINFO);
      pConvInfo->fsState      = CONVINFO_State_None;
      pConvInfo->hwndPartner  = hwndPartner;
      pConvInfo->pConvContext = pConvContext;

      pResult  = strcpy
               ( pConvInfo->szAppName
               , pszAppName
               );
      pResult  = strcpy
               ( pConvInfo->szTopic
               , pszTopic
               );

      hwndConv = WinCreateWindow
               ( hwndParent   /* parent window is acknowledging topic window  */
               , WC_DdeConv   /* window class name                            */
               , NULL         /* window title (not used)                      */
               , WS_DISABLED  /* window style (initially disabled)            */
               , 0            /* horizontal (x position)                      */
               , 0            /* vertical   (y portion)                       */
               , 0            /* width      (cx)                              */
               , 0            /* height     (cy)                              */
               , hwndParent   /* owner window is acknowledging topic window   */
               , HWND_TOP     /* place on TOP of Z-order                      */
               , 0            /* window id; Conversation windows use zero (0) */
               , pConvInfo    /* creation data                                */
               , NULL         /* presentation parameters (not used)           */
               );
   }
   return (hwndConv);
}
/*==============================================================================
** Function:   DdeCreateItemWindow
**
** Usage:      Creates a "WC_DdeItem" class window and adds a menu-item to its
**             corresponding 'Topic'.
**
** Remarks:    A "WC_DdeItem" class window is created for a 'Item' supported by
**             a 'Topic.  The actual data corresponding to the item is stored
**             in a "WC_MENU" class window, an ASCII text string, or 'Time'
**             value.
**
**             This class window is always created as a child of a "WC_DdeTopic"
**             class window.  This dependency is assumed when determining which
**             sub-menu to insert the new item-window.  The window-identifier
**             of a "WC_DdeTopic" class window its corresponding menu
**             item-identifier are the same and can be used to extract
**             corresponding menu information.
**
**============================================================================*/
   ULONG                            /* Menu item identifier
*/
   APIENTRY    DdeCreateItemWindow
(
   HWND        hwndParent           /* parent window handle                   */
,  HWND        hwndOwner            /* owner window handle                    */
,  HWND        hwndMenu             /* menu window handle to add item         */
,  ULONG       hItem                /* handle to item specific data           */
,  ULONG       ulStyle              /* user defined window-style flags        */
,  PSZ         pszText              /* address of Item text                   */
)
{
   BOOL        fResult     = FALSE;
   HWND        hwndItem    = NULLHANDLE;
   HWND        hwndSubMenu = NULLHANDLE;
   MENUITEM    mi;
   PDDEITEM    pDdeItem    = NULL;
   SHORT       cItems      = 0;
   SHORT       sItemId     = 0;
   SHORT       sItemIndex  = 0;
   USHORT      usWindowId  = 0;
/*
** Obtain the sub-menu window handle to insert the new menu-item into by
** obtaining the window-identifier of the parent window handle and using this
** value to query the corresponding menu-item entry and extracting the sub-menu
**      window handle.
*/
   usWindowId  = (USHORT) WinQueryWindowUShort
               ( hwndParent
               , QWS_ID
               );
/*
** Obtain the window handle of the 'menu' to which the item will be added...
*/
   hwndSubMenu = MnuQuerySubMenu    /* Query sub-window handle of...
*/
               ( hwndMenu           /*   root menu corresponding to...
*/
               , usWindowId         /*   this identifier.
*/
               );
/*
** If the sub-menu window handle was successfully extracted...
*/
   if (hwndSubMenu)
   {
      cItems = MnuQueryItemCount    /* Query current count of menu items
*/
             ( hwndSubMenu          /* for this menu window handle
*/
             );
/*
**    If there is room for another menu-item entry...
*/
      if (cItems < MAX_ITEMS)
      {
         sItemId = usWindowId          /* take the base menu identifier,      */
                 + cItems              /*  add the current number of entries, */
                 + 1;                  /*  and add 1 to it.                   */

         mi.iPosition   = MIT_END;     /* insert item at the end of the menu  */
         mi.afStyle     = MIS_TEXT;    /* menu item style is "Text"           */
         mi.afAttribute = 0;           /* visible, enabled, and not checked   */
         mi.id          = sItemId;     /* new menu-item identifier            */
         mi.hwndSubMenu = NULLHANDLE;  /* item contains no sub-entries        */
         mi.hItem       = 0L;          /* user defined handle                 */

         sItemIndex = MnuAddItem       /* add a menu-item entry,              */
                    ( hwndSubMenu      /*   to this base menu,                */
                    , &mi              /*   as defined in this structure,     */
                    , pszText          /*   with this text string.            */
                    );
         pDdeItem   = DdeAllocMem      /* allocate a piece of memory,         */
                    ( sizeof (DDEITEM) /*   the size of a DDEITEM structure.  */
                    );
/*
**       If the shared memory object was successfully allocated...
*/
         if (pDdeItem)
         {
            pDdeItem->cb               = sizeof (DDEITEM);
            pDdeItem->hwndMenuItems    = hwndMenu;
            pDdeItem->hItem            = hItem;
/*
**          Create "Item" window...
*/
            hwndItem = WinCreateWindow
                     ( hwndParent      /* window handle of parent             */
                     , WC_DdeItem      /* window-class string                 */
                     , NULL            /* window text                         */
                     , WS_DISABLED     /* window style, disabled and...       */
                     | ulStyle         /*   any user defined style            */
                     , 0L              /* x-coordinate                        */
                     , 0L              /* y-coordinate                        */
                     , 0L              /* width of window                     */
                     , 0L              /* height of window                    */
                     , hwndOwner       /* window handle of owner              */
                     , HWND_BOTTOM     /* sibling window handle               */
                     , (ULONG) sItemId /* window-identifier                   */
                     , pDdeItem        /* control data, DDEITEM structure     */
                     , NULL            /* presentation parameters             */
                     );
         }
/*
**       If creation failed, clean up resources.
*/
         if (hwndItem == NULLHANDLE)
         {
            if (pDdeItem != NULL)      /* if DDESTRUCT was allocated          */
            {
               fResult = DdeFreeMem    /* free allocated memory-object        */
                       ( pDdeItem      /* pointer to memory-object            */
                       , sizeof (DDEITEM)
                       );
               cItems  = MnuDeleteItem /* Remove item from menu               */
                       ( hwndSubMenu   /* menu window handle                  */
                       , sItemId       /* menu item identifier                */
                       , TRUE          /* check sub-menus                     */
                       );
            }
         }
      }
   }
/*
** If "Item" window created, return new menu identifier.  Otherwise return 0
*/
   return (hwndItem) ? (ULONG) sItemId : 0L;
}
/*==============================================================================
**
** Function:   DdeCreateTopicWindow
**
** Usage:      Creates a "WC_DdeTopic" class window and adds the 'Topic' to the
**             'Main' menu.
**
** Remarks:    A "WC_DdeTopic" class window is created for a 'Topic' supported
**             by a server application.
**
**============================================================================*/
   HWND                             /* handle of created window               */
   APIENTRY    DdeCreateTopicWindow
(
   HWND        hwndParent
,  HWND        hwndOwner
,  HWND        hwndMenu
,  PSZ         pszTopic
,  SHORT       sItemIdFirst
,  SHORT       sItemIdLast
,  SHORT       sItemIncrement
)
{
   HWND        hwndSubMenu = NULLHANDLE;
   HWND        hwndTopic   = NULLHANDLE;
/*
** Obtain the current count of items in the 'root' menu...
*/
   SHORT       sItemCount  = MnuQueryItemCount (hwndMenu);
/*
** Obtain the menu item-identifier matching the 'Topic' text string...
*/
   SHORT       sItemId     = MnuLookUpItemText
                           ( hwndMenu
                           , pszTopic
                           , FALSE
                           );
/*
** If the 'Topic' does not already exist...
*/
   if (sItemId == MIT_ERROR)
   {
/*
**    Calculate next menu item-identifier by:
**
**       Taking the 'First' identifier value, and adding...
**       the result of multiplying the current number of entries, times
**       the increment in which menu items are given values.  For example,
**
**       If item identifiers are in increments of 10, adding a 2nd entry
**       would yield a results of 10 plus whatever was specified as the
**       starting (sItemIdFirst) value, because the current (sItemCount)
**       number of entries would be 1.
*/
      sItemId  = sItemIdFirst + (sItemCount * sItemIncrement);
/*
**    If there is room to add another entry...
*/
      if (sItemId <= sItemIdLast)
      {
         MENUTEMPLATE   mt;

         mt.cbTemplate  = 0;
         mt.usVersion   = 0;
         mt.usCodepage  = 0;
         mt.cMenuItems  = 0;
         mt.offami      = 10;
/*
**       Create a 'menu' class window.  This window will be used to add 'Items'
**       later...
*/
         hwndSubMenu    = WinCreateWindow
                        ( HWND_OBJECT
                        , WC_MENU
                        , NULL
                        , WS_CLIPSIBLINGS | WS_SAVEBITS
                        , 0L
                        , 0L
                        , 0L
                        , 0L
                        , hwndMenu
                        , HWND_BOTTOM
                        , (ULONG) sItemId
                        , (PCH)&mt
                        , 0);
/*
**       If window successfully created, add the menu item...
*/
         if (hwndSubMenu)
         {
            MENUITEM    mi;
            SHORT       sItemIndex;

            mi.iPosition   = MIT_END;
            mi.afStyle     = MIS_TEXT | MIS_SUBMENU;
            mi.afAttribute = MIA_DISABLED;
            mi.id          = sItemId;
            mi.hwndSubMenu = hwndSubMenu;
            mi.hItem       = 0L;

            sItemIndex = MnuAddItem
                       ( hwndMenu
                       , &mi
                       , pszTopic
                       );
         }
      }
   }
/*
** If menu item successfully added...
*/
   if (hwndSubMenu)
   {
/*
**    Allocate a memory object to contain the "WC_DdeTopic" class information...
*/
      PDDETOPIC pDdeTopic  = DdeAllocMem
                           ( sizeof (DDETOPIC)
                           );
/*
**    If memory successfully allocated...
*/
      if (pDdeTopic)
      {
/*
**       Initialize 'WC_DdeTopic' class information...
*/
         pDdeTopic->cb            = sizeof (DDETOPIC);
         pDdeTopic->cConv         = 0L;
         pDdeTopic->hwndMenu      = hwndMenu;
         pDdeTopic->hwndMenuItems = hwndSubMenu;
/*
**       Create 'Topic' window...
*/
         hwndTopic  = WinCreateWindow
                    ( hwndParent      /* This windows parent                  */
                    , WC_DdeTopic     /* Window class name                    */
                    , pszTopic        /* Window title                         */
                    , WS_DISABLED     /* Window style                         */
                    , 0L              /* Horizontal (x position)              */
                    , 0L              /* Vertical   (y portion)               */
                    , 0L              /* Width      (cx)                      */
                    , 0L              /* Height     (cy)                      */
                    , hwndOwner       /* Owner window                         */
                    , HWND_TOP        /* Place on TOP of Z-order              */
                    , (ULONG) sItemId /* window identifier maps to menu Id    */
                    , pDdeTopic       /* Creation data                        */
                    , NULL            /* Presentation parameters              */
                    );
/*
**       If 'Topic' window created...
*/
         if (hwndTopic)
         {
/*
**          Enable the corresponding menu item entry...
*/
            BOOL fResult = WinEnableMenuItem
                         ( hwndMenu
                         , sItemId
                         , TRUE
                         );
         }
      }
   }
   return (hwndTopic);
}
/*==============================================================================
**
** Function:   DdeFreeMem
**
** Usage:      Frees a memory block from the application DDE heap.
**
** Remarks:    A single heap is created by both the Server and Client sample
**             programs.  A pointer to this heap is stored the global variable
**             'pHeap'.
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DdeFreeMem
(
   PVOID       pMemBlock            /* address of memory block to free        */
,  ULONG       ulSize               /* size (in bytes) of memory to allocate  */
)
{
   APIRET      ApiRet   = FALSE;
/*
** If a non-null pointer passed...
*/
   if (pMemBlock)
   {
      ApiRet = DosSubFreeMem        /* free a memory object...                */
             ( pHeap                /* from this base address,                */
             , pMemBlock            /* at this offset,                        */
             , ulSize               /* and size.                              */
             );
   }
   return ((ApiRet) ? FALSE : TRUE);
}
/*==============================================================================
**
** Function:   DdeItemWindowProc
**
** Usage:      This function is called when a message is dispatched to a
**             "WC_DdeItem" class window.  It is specified as the
**             "Window Procedure" when registering the "WC_DdeItem"
**             window class.
**
==============================================================================*/
   MRESULT                          /* Message specific result                */
   EXPENTRY    DdeItemWindowProc
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* Message identifier                     */
,  MPARAM      mp1                  /* Message parameter one (1)              */
,  MPARAM      mp2                  /* Message parameter two (2)              */
)
{
   MRESULT     MResult;

   switch (ulMsgId)
   {
      case UM_RenderItem:
      {
         MResult  = DdeItemUmRenderItem
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_AdviseItem:
      {
         MResult  = DdeItemUmAdviseItem
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_ShowItemDialog:
      {
         MResult  = DdeItemUmShowItemDialog
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CREATE:
      {
         MResult  = DdeItemWmCreate
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DESTROY:
      {
         MResult  = DdeItemWmDestroy
                  ( hwnd
                  );
         break;
      }
      case WM_DDE_ADVISE:
      {
         MResult  = DdeItemWmDdeAdvise
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_QUERYWINDOWPARAMS:
      {
         MResult  = DdeItemWmQueryWindowParams
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CLOSE:
      {
         MResult  = MRFROMSHORT (0);
         break;
      }
      default:
      {
         MResult  = WinDefWindowProc
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
**
** Function:   DdeItemUmAdviseItem
**
** Usage:      Processes a "UM_UnAdviseItem" message dispatched to a
**             "WC_DdeItem" class window.
**
** Remarks:    A "UM_UnAdviseItem" message is broadcasted by a "WC_DdeConv"
**             class window when it processes a "WM_DDE_UNADVISE" message.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeItemUmAdviseItem
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* item identifier                        */
,  MPARAM      mp2                  /* item specific handle                   */
)
{
   BOOL        fResult;
/*
** Obtain "WC_DdeItem" window class info...
*/
   PDDEITEM    pDdeItem    = (PDDEITEM) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain item identifier corresponding to the request...
*/
   USHORT      usItemId    = SHORT1FROMMP (mp1);
/*
** Obtain item identifier corresponding to this 'Item' class window...
*/
   USHORT      usWindowId  = (USHORT) WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
** If the 'Unadvise' transaction is for this 'Item', then...
*/
   if (usItemId == usWindowId)
   {
      pDdeItem->hItem = LONGFROMMP (mp2);

      fResult  = WinBroadCastMsg
               ( hwnd
               , UM_AdviseItem
               , mp1
               , mp2
               , BMSG_SEND
               );
   }
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeItemWmQueryWindowParams
**
** Usage:      Processes a "WM_QUERYWINDOWPARAMS" message dispatched to a
**             "WC_DdeItem" class window.
**
** Remarks:    A "WM_QUERYWINDOWPARAMS" message is sent as a result of issuing
**             a 'WinQueryWindowText' API.  The text string corresponding to an
**             'Item' is contained in its associated menu-item.  The window
**             identifier of a "WC_DdeItem" class window is also the menu-item
**             identifier of the 'Item'.
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeItemWmQueryWindowParams
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* address of a WNDPARAMS structure       */
,  MPARAM      mp2                  /* not used                               */
)
{
/*
** Obtain "WC_DdeItem" window class info...
*/
   PDDEITEM    pDdeItem    = (PDDEITEM) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain address of 'Window Parameters' structure...
*/
   PWNDPARAMS  pWndParams = (PWNDPARAMS) PVOIDFROMMP (mp1);
/*
** If either Window text or size of Window text is being requested...
*/
   if (pWndParams->fsStatus & WPM_TEXT
   ||  pWndParams->fsStatus & WPM_CCHTEXT)
   {
      ULONG    cbWindowText;
      CHAR     szWindowText [MAX_TEXTLENGTH];
      PSZ      pszFrom     = szWindowText;
      PSZ      pszTo       = pWndParams->pszText;
/*
**    Obtain the Window Identifier associated with this window...  The Window
**    Identifier of a "WC_DdeItem" class window corresponds to its Menu
**    Identifier.  The actual text of an 'Item' is maintained in a Menu Item.
*/
      USHORT   usId  = WinQueryWindowUShort
                     ( hwnd
                     , QWS_ID
                     );
/*
**    Obtain 'Item' text and size from menu...
*/
      cbWindowText   = MnuQueryItemText
                     ( pDdeItem->hwndMenuItems
                     , usId
                     , szWindowText
                     );
/*
**    If query failed, return indicating failure...
*/
      if (cbWindowText == 0)
      {
         return (MRFROMSHORT (FALSE));
      }
/*
**    If 'Item' text requested...
*/
      if (pWndParams->fsStatus & WPM_TEXT)
      {
/*
**       Copy the item name-string to the location indicated in the WNDPARAMS
**       structure...
*/
         while (*pszTo++ = *pszFrom++);

/*
**       Clear this flag in the fsStatus field of the WNDPARAMS structure,
**       this indicates that item text was processed...
*/
         pWndParams->fsStatus &= ~WPM_TEXT;
      }
/*
**    If size of 'Item' text was requested...
*/
      if (pWndParams->fsStatus & WPM_CCHTEXT)
      {
/*
**       Calculate the length of the topic name-string and set the appropriate
**       field in the WNDPARAMS structure with this value.
*/
         pWndParams->cchText  = strlen
                              ( szWindowText
                              );
/*
**       Clear this flag in the fsStatus field of the WNDPARAMS structure,
**       this indicates that item was processed...
*/
         pWndParams->fsStatus &= ~WPM_CCHTEXT;
      }
      return (MRFROMSHORT (TRUE));
   }
/*
** If any other Window Parameter requested, let the default window procedure
** handle it...
*/
   return WinDefWindowProc (hwnd, WM_QUERYWINDOWPARAMS, mp1, mp2);
}
/*==============================================================================
** Function:   DdeItemUmRenderItem
**
** Usage:      Processes a "UM_RenderItem" message dispatched to a
**             "WC_DdeItem" class window.
**
** Remarks:    A "UM_RenderItem" is issued by either a "WC_DdeConv" or
**             "WC_DdeAdvise" class window when a data item has been requested
**             or if the data item changed and a permanent (hot or cold) data
**             link has been established.
**
**============================================================================*/
   MRESULT                          /* PDDESTRUCT: Null if an error occurred  */
   EXPENTRY    DdeItemUmRenderItem
(
   HWND        hwnd                 /* handle of window receiving  message    */
,  MPARAM      mp1                  /* format identifier and type of request  */
,  MPARAM      mp2                  /* item handle                            */
)
{
   BOOL        fResult     = FALSE;
   PDDEDATA    pDdeData    = NULL;
   PDDESTRUCT  pDdeStruct  = NULL;
   USHORT      cbItemName  = 0;
   PSZ         szItemName  [MAX_ITEM_TEXT_LENGTH];
/*
** Obtain 'Request' parameters...
*/
   USHORT      usFormat    = SHORT1FROMMP (mp1);
   USHORT      fsStatus    = SHORT2FROMMP (mp1);
/*
** Obtain "WC_DdeItem" window class info...
*/
   PDDEITEM    pDdeItem    = WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain the 'Style' of the "WC_DdeItem" class window (Menu, Text or Time)...
*/
   ULONG       ulStyle     = WinQueryWindowULong
                           ( hwnd
                           , QWL_STYLE
                           );
/*
** Obtain corresponding 'Item' text...
*/
   cbItemName  = MnuQueryItemText
               ( pDdeItem->hwndMenuItems
               , WinQueryWindowUShort (hwnd, QWS_ID)
               , (PSZ) szItemName
               );
/*
** If request does not require 'Item' to be rendered...
*/
   if (fsStatus & DDE_FNODATA)
   {
/*
**    Create a DDE shared memory object containing no data...
*/
      pDdeStruct  = DdeMakeDdeStruct
                  ( (PSZ) szItemName   /* item name-string                   */
                  , NULL               /* pointer to data (not applicable)   */
                  , 0                  /* size of data (not applicable)      */
                  , usFormat           /* registered DDE format identifier   */
                  , fsStatus           /* DDE status flags                   */
                  );
   }
   else
   {
/*
**    Ask the 'owner' of this item to render it...
*/
      pDdeData    = (PDDEDATA) WinSendMsg
                  ( WinQueryWindow (hwnd, QW_OWNER)
                  , UM_RenderItem
                  , MPFROM2SHORT (usFormat, (USHORT) ulStyle)
                  , MPFROMLONG (pDdeItem->hItem)
                  );
/*
**    If 'Item' was successfully rendered...
*/
      if (pDdeData)
      {
/*
**       Create a DDE shared memory object containing rendered data item...
*/
         pDdeStruct  = DdeMakeDdeStruct
                     ( (PSZ) szItemName
                     , (PVOID) pDdeData->pData.abData
                     , pDdeData->cbData
                     , usFormat
                     , fsStatus
                     );
/*
**       Free the temporary memory object...
*/
         DosFreeMem (pDdeData);
      }
   }
/*
** Return pointer to DDESTRUCT structure...
*/
   return (MRFROMP (pDdeStruct));
}
/*==============================================================================
**
** Function:   DdeItemUmShowItemDialog
**
** Usage:      Processes a "UM_ShowItemDialog" message dispatched to a
**             "WC_DdeItem" class window.
**
** Remarks:    A "UM_ShowItemDialog" message is posted to a "WC_DdeItem" class
**             to display its corresponding dialog window.  From this window
**             a user view, update or select (paste-link) a DDE item.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeItemUmShowItemDialog
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* pointer to window control data         */
,  MPARAM      mp2                  /* pointer to CREATESTRUCT structure      */
)
{
   BOOL        fResult;
   ULONG       cbResult;
   DLGITEM     DlgItem;
/*
** Obtain 'request' parameters...
*/
   HWND        hwndMenu       = HWNDFROMMP   (mp1);
   SHORT       sMenuItemId    = SHORT1FROMMP (mp2);
   SHORT       sMenuTopicId   = SHORT2FROMMP (mp2);
/*
** Obtain "WC_DdeItem" window class info...
*/
   PDDEITEM    pDdeItem    = WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Obtain 'Item' text...
*/
   ULONG       ulReply     = WinQueryWindowText
                           ( hwnd
                           , MAX_TEXTLENGTH
                           , DlgItem.szItemName
                           );
/*
** Obtain the 'Style' of the "WC_DdeItem" class window (Menu, Text or Time)...
*/
   USHORT      ulStyle     = WinQueryWindowULong
                           ( hwnd
                           , QWL_STYLE
                           );
/*
** Obtain window identifier of "WC_DdeItem" class window...
*/
   USHORT      usWindowId  = WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
** If the identifier specified in the request does not match the identifier of
** this 'Item'', then exit this function...
*/
   if (usWindowId != sMenuItemId)
   {
      return (MRFROMSHORT (FALSE));
   }
/*
** Initialize the dialog item (DLGITEM) structure...
*/
   DlgItem.flType    = ulStyle;
   DlgItem.hItem     = pDdeItem->hItem;
   DlgItem.cbData    = 0L;
   DlgItem.ulOptions = 0L;
   DlgItem.fsStatus  = 0;
   DlgItem.usFormat  = 0;
/*
** Show and process application modal dialog window...
*/
   ulReply  = WinDlgBox
            ( HWND_DESKTOP                /* handle of parent window          */
            , hwnd                        /* handle of owner window           */
            , (PFNWP) DlgItemWindowProc   /* dialog window procedure          */
            , 0                           /* resource identifier (from .exe)  */
            , IDD_ITEM                    /* dialog-template identifier       */
            , &DlgItem                    /* creation parameters              */
            );
/*
** Did the user press the "Post Advise" push button?  If so, "Advise" all
** existing 'data-links' that the 'Item' has changed...
*/
   if (ulReply == IDC_ITEM_OK)
   {
      fResult  = WinBroadCastMsg
               ( hwnd
               , UM_AdviseItem
               , MPFROMSHORT (usWindowId)
               , MPFROMLONG  (pDdeItem->hItem)
               , BMSG_SEND
               );
   }
/*
** Or, Did the user press the "Paste Link" push button?  If so, update the
** 'Clipboard' with the 'Link' format...
*/
   else if (ulReply == IDC_ITEM_PASTE_LINK)
   {
      CHAR     szTopic [MAX_TEXTLENGTH];
/*
**    Obtain corresponding 'Topic' name...
*/
      cbResult = MnuQueryItemText
               ( hwndMenu
               , sMenuTopicId
               , szTopic
               );
/*
**    Copy 'Link' format to clipboard...
*/
      fResult  = DdeSetPasteLink
               ( szAppName
               , szTopic
               , DlgItem.szItemName
               );
   }
/*
** Return successful indicator...
*/
   return  (MRFROMSHORT (TRUE));
}
/*==============================================================================
** Function:   DdeItemWmCreate
**
** Usage:      Processes a "WM_CREATE" message dispatched to a "WC_DdeItem"
**             class window.
**
** Remarks:    A WM_CREATE message is 'sent' to an application after the window
**             is created but before is it visible.  This offers the window
**             procedure an opportunity to perform initialization if any.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeItemWmCreate
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* pointer to window control data         */
,  MPARAM      mp2                  /* pointer to CREATESTRUCT structure      */
)
{
   BOOL        fResult;
/*
** Obtain pointer to window creation data...
*/
   PDDEITEM    pDdeItem = (PDDEITEM) PVOIDFROMMP (mp1);
/*
** Save this pointer in window instance data...
*/
   fResult  = WinSetWindowPtr       /* set window pointer of...               */
            ( hwnd                  /*   this window at,                      */
            , QWL_USER              /*   this zero-based offset to,           */
            , pDdeItem              /*   this DDEITEM structure pointer.      */
            );
/*
** If creation processing successful...
*/
   if (fResult)
   {
      return (MRFROMSHORT (FALSE)); /* no errors, continue...                 */
   }
   else
   {
      return (MRFROMSHORT (TRUE));  /* error occurred, discontinue...         */
   }
}
/*==============================================================================
**
** Function:   DdeItemWmDestroy
**
** Usage:      Processes a "WM_DESTROY" message dispatched to a "WC_DdeItem"
**             class window.
**
** Remarks:    A "WM_DESTROY" message is sent to a window after it has been
**             hidden offering it an opportunity to perform some termination
**             action for this window.
**
==============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   APIENTRY    DdeItemWmDestroy
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult;
/*
** Obtain "WC_DdeItem" window class info...
*/
   PDDEITEM    pDdeItem   = (PDDEITEM) WinQueryWindowPtr (hwnd, QWL_USER);
/*
** Obtain the 'Style' of the "WC_DdeItem" class window (Menu, Text or Time)...
*/
   ULONG       ulStyle    = WinQueryWindowULong
                           ( hwnd
                           , QWL_STYLE
                           );
/*
** If class information exists...
*/
   if (pDdeItem)
   {
/*
**    If the DDE item window has a type of 'Menu' and it is OK to destroy the
**    associated menu, then destroy it...
**
**    Note: some items are associated with 'Static' menu-items such as the
**    'Status' menu-item of a 'Server' application.  In these cases we never
**    want to destroy the associated menu.
*/
      if ( ulStyle & IS_Menu
      && !(ulStyle & IS_NoFree))
      {
         WinDestroyWindow ((HWND) pDdeItem->hItem);
      }
/*
**    Free "WC_DdeItem" class information...
*/
      fResult  = DdeFreeMem         /* free a memory-object...                */
               ( pDdeItem           /*   at this base address,                */
               , pDdeItem->cb       /*   and this size (in bytes).            */
               );
   }
/*
** Return reserved value, zero (0)...
*/
   return (MRFROMLONG (0L));
}
/*==============================================================================
**
** Function:   DdeItemWmDdeAdvise
**
** Usage:      Processes a "WM_DDE_ADVISE" message dispatched to a "WC_Dde_Item"
**             class window.
**
** Remarks:    A "WM_DDE_ADVISE" message is sent by a "WC_DdeConv" class window
**             after it has accepted the 'Advise' request.  Upon receipt of this
**             message, a "WC_DdeAdvise" class window will be created to
**             support the requested 'data-link'.
**
==============================================================================*/
   MRESULT
   APIENTRY    DdeItemWmDdeAdvise
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* pointer to CONVID structure            */
,  MPARAM      mp2                  /* pointer to DDESTRUCT structure         */
)
{
/*
** Obtain a pointer to the DDESTRUCT structure specified in the request...
*/
   PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) PVOIDFROMMP (mp2);
/*
** Obtain the handle of a "WC_DdeAdvise" class window (if any) currently
** supporting this format...
*/
   HWND        hwndAdvise  = WinWindowFromID
                           ( hwnd
                           , pDdeStruct->usFormat
                           );
/*
** If no 'data-link' currently exists...
*/
   if (hwndAdvise == NULLHANDLE)
   {
/*
**    Obtain "WC_DdeItem" window class info...
*/
      PDDEITEM pDdeItem = (PDDEITEM) WinQueryWindowPtr
                        ( hwnd
                        , QWL_USER
                        );
/*
**    Create a "WC_DdeAdvise" class window...
*/
      hwndAdvise  = DdeCreateAdviseWindow
                  ( hwnd                        /* parent window handle       */
                  , hwnd                        /* owner window handle        */
                  , (PCONVID) PVOIDFROMMP (mp1) /* 'Conversation Information' */
                  , pDdeStruct->fsStatus        /* original DDE status flags  */
                  , pDdeStruct->usFormat        /* DDE format identifier      */
                  );
   }
/*
** Return 'TRUE' if "WC_DdeAdvise" class window created, otherwise return
** 'FALSE'.
*/
   return MRFROMSHORT ((hwndAdvise) ? TRUE : FALSE);
}
/*==============================================================================
**
** Function:   DdeMakeDdeStruct
**
** Usage:      Allocates and initializes a shared memory object.
**
** Remarks:
**
==============================================================================*/
   PDDESTRUCT                       /* pointer to DDESTRUCT structure         */
   APIENTRY    DdeMakeDdeStruct
(
   PSZ         pszItemName          /* pointer to item name-string            */
,  PVOID       pvData               /* pointer to rendered data (if any)      */
,  ULONG       cbData               /* size of rendered data (if any)         */
,  USHORT      usFormat             /* registered DDE format identifier
*/
,  USHORT      fsStatus             /* DDE status flags                       */
)
{
   APIRET      ApiRet;
   USHORT      cbItemName;
   PDDESTRUCT  pDdeStruct;
   PSZ         pszNull     = "";
/*
** If a NULL value is passed in as a pointer to an Item name, then set
** pszItemName to point to a NULL terminated string.  This guarantees that
** the pszItemName always points to a NULL (0x00) terminated string.
*/
   if (pszItemName == (PSZ) NULL)
   {
      pszItemName = pszNull;
   }
/*
** Calculate the size of the Item name (including the NULL character)...
*/
   cbItemName = strlen (pszItemName) + 1L;
/*
** Allocate shared memory object...
*/
   ApiRet   = DosAllocSharedMem
            ( (PPVOID) &pDdeStruct  /* base address of shared memory object   */
            , (PSZ) NULL            /* un-named memory object                 */
            , sizeof (DDESTRUCT)    /* size of memory object (in bytes),      */
            + cbItemName            /* size of item name-string,              */
            + cbData                /* size of data (if any),                 */
            , PAG_COMMIT            /* all pages are initially committed
*/
            | OBJ_GIVEABLE          /* memory object is giveable              */
            | PAG_READ              /* read access allowed                    */
            | PAG_WRITE             /* write access allowed                   */
            );
/*
** If shared memory object creation failed, return NULL pointer...
*/
   if (ApiRet != NO_ERROR)
   {
      return ((PDDESTRUCT) NULL);
   }
/*
** Initialize shared memory object...
*/
   pDdeStruct->cbData        = cbData;
   pDdeStruct->fsStatus      = fsStatus;
   pDdeStruct->usFormat      = usFormat;
   pDdeStruct->offszItemName = sizeof (DDESTRUCT);
   pDdeStruct->offabData     = sizeof (DDESTRUCT) + cbItemName;
/*
** Copy Item name-string to shared memory object...
*/
   pszNull  = memcpy
            ( DDES_PSZITEMNAME (pDdeStruct)
            , pszItemName
            , cbItemName
            );
/*
** If rendered data exists, copy data to shared memory object...
*/
   if (pvData && cbData)
   {
      pszNull  = memcpy
               ( DDES_PABDATA (pDdeStruct)
               , pvData
               , cbData
               );
   }
/*
** Return pointer to shared memory object containing DDESTRUCT structure...
*/
   return ((PDDESTRUCT) pDdeStruct);
}
/*==============================================================================
** Function:   DdeRegisterFmt
**
** Usage:      Registers DDE format string with system atom-table.
**
** Remarks:    DDE format name-strings are added to the atom table to
**             ensure that the count associated with the format name-string
**             stays greater than zero.  If we were to query the atom table for
**             a match on the format name-string and only add the format-name
**             string if it was not found, it would be possible that the usage
**             count for the atom would go to zero while we were still using it.
**             The would occur if the application (possibly another DDE server
**             or client) that added it then deleted it; a very bad situation.
**
==============================================================================*/
   USHORT                           /* Atom identifier                        */
   APIENTRY    DdeRegisterFmt
(
   HATOMTBL    hAtomTable           /* handle atom-table                      */
,  PSZ         pszFormatName        /* DDE format name-string                 */
)
{
   return ((USHORT) WinAddAtom (hAtomTable, pszFormatName));
}
/*==============================================================================
** Function:   DdeUnRegisterFmt
**
** Usage:      De-registers DDE format string with system atom-table.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DdeUnRegisterFmt
(
   HATOMTBL    hAtomTable           /* handle atom-table                      */
,  USHORT      usFormatId           /* previously registered atom-identifier  */
)
{
   ATOM        atom;
/*
** Delete the specified atom-identifier...
*/
   atom  = WinDeleteAtom            /* Delete an atom from an Atom-table      */
         ( hAtomTable               /* Handle of atom-table                   */
         ,(ATOM) usFormatId         /* Identifier of Atom to be deleted       */
         );
/*
** If WinDeleteAtom was successful, atom will have a value of zero (0).
** If WinDeleteAtom was unsuccessful, atom will contain a value equal to
** usFormatId.  Depending on contents of the atom variable, return TRUE if
** the atom was successfully deleted, otherwise return FALSE
*/
   return (atom) ? FALSE : TRUE;
}
/*==============================================================================
** Function:   DdeTopicWindowProc
**
** Usage:      Processes messages dispatched to a "WC_DdeAdvise" class window.
**
** Remarks:    This function is specified as the "Window Procedure" when
**             registering (WinRegisterClass) a "WC_DdeAdvise" window class.
**
==============================================================================*/
   MRESULT                          /* Message specific result                */
   EXPENTRY    DdeTopicWindowProc
(
   HWND        hwnd                 /* handle of window receiving message     */
,  ULONG       ulMsgId              /* Message identifier                     */
,  MPARAM      mp1                  /* Message parameter one                  */
,  MPARAM      mp2                  /* Message parameter two                  */
)
{
   MRESULT     MResult;

   switch (ulMsgId)
   {
      case UM_AdviseItem:
      {
         MResult  = DdeTopicUmAdviseItem
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_ExecuteCommand:
      {
         MResult  = WinSendMsg
                  ( WinQueryWindow (hwnd, QW_OWNER)
                  , UM_ExecuteCommand
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_ShowItemDialog:
      {
         MResult  = DdeTopicUmShowItemDialog
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CREATE:
      {
         MResult  = DdeTopicWmCreate
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CLOSE:
      {
         MResult  = DdeTopicWmClose
                  ( hwnd
                  );
         break;
      }
      case WM_DESTROY:
      {
         MResult  = DdeTopicWmDestroy
                  ( hwnd
                  );
         break;
      }
      case WM_QUERYWINDOWPARAMS:
      {
         MResult  = DdeTopicWmQueryWindowParams
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      default:
      {
         MResult  = WinDefWindowProc
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
**
** Function:   DdeTopicUmAdviseItem
**
** Usage:      Processes a "UM_AdviseItem" dispatched to a "WC_DdeTopic" class
**             window.
**
** Remarks:    A "UM_AdviseItem" message is issued by a DDE server application
**             to all of its "WC_DdeTopic" class windows when a 'Item'
**             changes.  This message is in-turn broadcasted to all children
**             of this window, which include "WC_DdeItem" class windows.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeTopicUmAdviseItem
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* item identifier                        */
,  MPARAM      mp2                  /* item handle (hwndMenu, pszText or Time)*/
)
{
   BOOL        fResult;
/*
** Broadcast message to all immediate children...
*/
   fResult  = WinBroadCastMsg
            ( hwnd
            , UM_AdviseItem
            , mp1
            , mp2
            , BMSG_SEND
            );
/*
** Return indicating no errors...
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   DdeTopicUmShowItemDialog
**
** Usage:      Processes a "UM_ShowItemDialog" message dispatched to a
**             "WC_DdeTopic" class window.
**
** Remarks:    A "UM_ShowItemDialog" message is posted by 'Server' application
**             when the user has selected a 'Topic' and 'Item' from the menu.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeTopicUmShowItemDialog
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* handle of base menu containing items   */
,  MPARAM      mp2                  /* selected 'Topic' and 'Item' identifiers*/
)
{
   USHORT      cbText;
   BOOL        fResult;
   ULONG       ulReply;
   USHORT      usCmd;
/*
** Obtain requested, 'Topic' and 'Item'...
*/
   HWND        hwndMenu     = HWNDFROMMP   (mp1);
   SHORT       sMenuItemId  = SHORT1FROMMP (mp2);
   SHORT       sMenuTopicId = SHORT2FROMMP (mp2);
/*
** Obtain window-identifier of this window.  The window-identifier of a
** "WC_DdeTopic" class window matches its menu-identifier in the associated
** menu.
*/
   USHORT      usWindowId  = WinQueryWindowUShort
                           ( hwnd
                           , QWS_ID
                           );
/*
** If the window-identifier of this window does not match the menu-identifier
** of the specified 'Topic', exit this function...
*/
   if (usWindowId != sMenuTopicId)
   {
      return (MRFROMSHORT (TRUE));
   }
/*
** At this point it is has been determined that the request for this topic. As
** a result, a "UM_ShowItemDialog" is posted to all immediate children of this
** window.
*/
   fResult  = WinBroadCastMsg
            ( hwnd
            , UM_ShowItemDialog
            , mp1
            , mp2
            , BMSG_POST
            );
/*
** Return indicating no errors...
*/
   return MRFROMSHORT (FALSE);
}
/*==============================================================================
**
** Function:   DdeTopicWmCreate
**
** Usage:      Processes a "WM_CREATE" message dispatched to a "WC_DdeTopic"
**             class window.
**
** Remarks:    A WM_CREATE message is 'sent' to an application after the window
**             is created but before is it visible.  This offers the window
**             procedure an opportunity to perform initialization if any.
**
==============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY      DdeTopicWmCreate
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* Message parameter one                  */
,  MPARAM      mp2                  /* Message parameter two                  */
)
{
   BOOL        fResult;
/*
** Obtain pointer to "WC_DdeTopic" class information...
*/
   PDDETOPIC   pDdeTopic = (PDDETOPIC) PVOIDFROMMP (mp1);
/*
** Store pointer to "WC_DdeTopic" class information in window extra-words...
*/
   fResult  = WinSetWindowPtr       /* set window pointer of...               */
            ( hwnd                  /*   this window at,                      */
            , QWL_USER              /*   this zero-based offset to,           */
            , pDdeTopic             /*   this DDETOPIC structure pointer.     */
            );
/*
** If creation processing successful...
*/
   if (fResult)
   {
      return (MRFROMSHORT (FALSE)); /* no errors, continue...                 */
   }
   else
   {
      return (MRFROMSHORT (TRUE));  /* error occurred, discontinue...         */
   }
}
/*==============================================================================
**
** Function:   DdeTopicWmClose
**
** Usage:      Processes a "WM_CLOSE" message dispatched to "WC_DdeConv" class
**             window.
**
** Remarks:    A "WM_CLOSE" message is posted to a "WC_DdeTopic" class window
**             when a 'Server' application is terminating.
**
==============================================================================*/
   MRESULT                             /* flReply: reserved value, zero (0)  */
   APIENTRY    DdeTopicWmClose
(
   HWND        hwnd                    /* handle of window receiving message  */
)
{
   BOOL        fResult;
/*
** Send this message to all immediate children of this window so they may free
** their associated class information...
*/
   fResult  = WinBroadCastMsg
            ( hwnd
            , WM_CLOSE
            , MPVOID
            , MPVOID
            , BMSG_SEND
            );
/*
** Return reserved value...
*/
   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   DdeTopicWmDestroy
**
** Usage:      Processes a "WM_DESTROY" message dispatched to a "WC_DdeTopic"
**             class window.
**
** Remarks:    a "WM_DESTROY" is sent to a window after it has been hidden
**             offering it an opportunity to perform some termination action.
**             In this context, the 'class information' is freed.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   APIENTRY    DdeTopicWmDestroy
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult;
/*
** Obtain "WC_DdeTopic" window class info...
*/
   PDDETOPIC   pDdeTopic   = (PDDETOPIC) WinQueryWindowPtr
                           ( hwnd
                           , QWL_USER
                           );
/*
** Free "WC_DdeTopic" class instance data...
*/
   fResult  = DdeFreeMem            /* free memory object at...               */
            ( pDdeTopic             /*   this location,                       */
            , pDdeTopic->cb         /*   and size (in bytes).                 */
            );

   return (MRFROMLONG (0L));
}
/*==============================================================================
**
** Function:   DdeTopicWmQueryWindowParams
**
** Usage:      Processes a "WM_QUERYWINDOWPARAMS" message dispatched to a
**             "WC_DdeTopic" class window.
**
** Remarks:    This message is sent as a result of a WinQueryWindowText API
**             call.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    DdeTopicWmQueryWindowParams
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* pointer to a WNDPARAMS structure       */
,  MPARAM      mp2                  /* not used                               */
)
{
   PWNDPARAMS  pWndParams = (PWNDPARAMS) PVOIDFROMMP (mp1);
/*
** Obtain "WC_DdeTopic" window class info...
*/
   PDDETOPIC   pDdeTopic  = (PDDETOPIC) WinQueryWindowPtr (hwnd, QWL_USER);
/*
** If window-text or window-text length is being queried...
*/
   if (pWndParams->fsStatus & WPM_TEXT
   ||  pWndParams->fsStatus & WPM_CCHTEXT)
   {
      ULONG    cbWindowText;
      CHAR     szWindowText [MAX_TEXTLENGTH];
      PSZ      pszFrom    = szWindowText;
      PSZ      pszTo      = pWndParams->pszText;
      USHORT   usId       = WinQueryWindowUShort (hwnd, QWS_ID);
/*
**    Obtain window-text and length...
*/
      cbWindowText   = MnuQueryItemText
                     ( pDdeTopic->hwndMenu
                     , usId
                     , szWindowText
                     );
/*
**    If query failed...
*/
      if (cbWindowText == 0)
      {
         return (MRFROMSHORT (FALSE));
      }
/*
**    If window-text queried...
*/
      if (pWndParams->fsStatus & WPM_TEXT)
      {
/*
**       Copy the topic name-string to the location indicated in the WNDPARAMS
**       structure.
*/
         while (*pszTo++ = *pszFrom++);
/*
**       Clear this flat in the fsStatus field of the WNDPARAMS structure.
**       This indicates that item was processed.
*/
         pWndParams->fsStatus &= ~WPM_TEXT;
      }
/*
**    If window-text length queried...
*/
      if (pWndParams->fsStatus & WPM_CCHTEXT)
      {
/*
**       Calculate the length of the topic name-string and set the appropriate
**       field in the WNDPARAMS structure with this value.
*/
         pWndParams->cchText = strlen (szWindowText);
/*
**       Clear this flag in the fsStatus field of the WNDPARAMS structure.
**       This indicates that item was processed.
*/
         pWndParams->fsStatus &= ~WPM_CCHTEXT;
      }
/*
**    Return successful indicator...
*/
      return (MRFROMSHORT (TRUE));
   }
/*
** If any other window-parameter requested, pass the request to the default
** window procedure...
*/
   return WinDefWindowProc (hwnd, WM_QUERYWINDOWPARAMS, mp1, mp2);
}
/*==============================================================================
**
** Function:   DdeQueryConvWindow
**
** Usage:      Returns handle of "WC_DdeConv" class window matching specified
**             application and topic name-strings.
**
** Remarks:    This function enumerates all immediate children of the window
**             indicated in 'hwndParent'.  If a "WC_DdeConv" class window
**             is found and its associated Application and Topic name-stings
**             match, its corresponding window handle is returned.
**
==============================================================================*/
   HWND                             /* Window handle                          */
   APIENTRY    DdeQueryConvWindow
(
   HWND        hwndParent
,  PSZ         pszAppName
,  PSZ         pszTopic
)
{
   BOOL        fResult;
   HENUM       henum;
   HWND        hwndChild;
   HWND        hwndConv   = NULLHANDLE;
   PCONVINFO   pConvInfo;
   CHAR        szText [MAX_TEXTLENGTH];
   ULONG       cbText;
/*
** Enumerate all children of the specified parent...
*/
   henum = WinBeginEnumWindows
         ( hwndParent
         );
/*
** If enumeration was successful...
*/
   if (henum)
   {
/*
**    Cycle through all children...
*/
      while (hwndChild = WinGetNextWindow (henum))
      {
/*
**       Obtain class name...
*/
         cbText   = WinQueryClassName
                  ( hwndChild
                  , MAX_ITEM_TEXT_LENGTH
                  , (PSZ) szText
                  );
/*
**       If class name matches "WC_DdeConv"...
*/
         if (strcmp (szText, WC_DdeConv) == 0)
         {
/*
**          Obtain "WC_DdeConv" class information...
*/
            pConvInfo   = (PCONVINFO) WinQueryWindowULong
                        ( hwndChild
                        , QWL_USER
                        );
/*
**          If a match is found, return associated window handle...
*/
            if (pConvInfo
            && (strcmp (pszAppName, pConvInfo->szAppName) == 0)
            && (strcmp (pszTopic,   pConvInfo->szTopic)   == 0))
            {
               hwndConv = hwndChild;
               break;
            }
         }
      }
/*
**    Clean-up enumeration resources...
*/
      fResult  = WinEndEnumWindows
               ( henum
               );
   }
/*
** Return handle of "WC_DdeConv" window, or NULLHANDLE if none found...
*/
   return (hwndConv);
}
/*==============================================================================
**
** Function:   DdeQueryItemWindow
**
** Usage:      Returns handle of "WC_DdeItem" class window matching specified
**             application and topic name-strings.
**
** Remarks:    This function enumerates all immediate children of the window
**             indicated in 'hwndParent'.  If a "WC_DdeItem" class window
**             is found and its associated Application and Topic name-stings
**             match, its corresponding window handle is returned.
**
==============================================================================*/
   HWND                             /* Window handle                          */
   APIENTRY    DdeQueryItemWindow
(
   HWND        hwndParent
,  PSZ         pszItemText
,  USHORT      usCodepage
,  BOOL        fCaseSensitive
,  ULONG       flCmd
)
{
   BOOL        fResult;
   HENUM       henum;
   HWND        hwndChild;
   HWND        hwndItem = NULLHANDLE;
   ULONG       cbText;
   CHAR        szText   [MAX_TEXTLENGTH];
   CHAR        szCpText [MAX_TEXTLENGTH];

/*
** If the current codepage is not equal to the conversation codepage,
** translate the AppName string from the conversation codepage to the
** current codepage.
*/
   if (usCodepage != ConvContext.usCodepage)
   {
      fResult  = WinCpTranslateString
               ( hab
               , usCodepage
               , pszItemText
               , ConvContext.usCodepage
               , MAX_TEXTLENGTH
               , szCpText
               );
/*
**    If string successfully translated...
*/
      if (fResult)
      {
         pszItemText = szCpText;
      }
      else
      {
         return (hwndItem);
      }
   }
/*
** Enumerate all children of the specified parent...
*/
   henum = WinBeginEnumWindows
         ( hwndParent
         );
/*
** If enumeration was successful...
*/
   if (henum)
   {
      while (hwndChild = WinGetNextWindow (henum))
      {
/*
**       Obtain class name...
*/
         cbText   = WinQueryClassName
                  ( hwndChild
                  , MAX_ITEM_TEXT_LENGTH
                  , (PSZ) szText
                  );
/*
**       If non "WC_DdeItem" found, check...
*/
         if (strcmp (szText, WC_DdeItem))
         {
/*
**          If scope of search includes descendants...
*/
            if (flCmd & BMSG_DESCENDANTS)
            {
               hwndItem = DdeQueryItemWindow
                        ( hwndChild
                        , pszItemText
                        , ConvContext.usCodepage
                        , fCaseSensitive
                        , BMSG_DESCENDANTS
                        );
/*
**             If window found, discontinue search...
*/
               if (hwndItem)
               {
                  break;
               }
            }
         }
         else
         {
/*
**          Obtain 'Item' text...
*/
            cbText   = WinQueryWindowText
                     ( hwndChild
                     , MAX_ITEM_TEXT_LENGTH
                     , (PSZ) szText
                     );
/*
**          If text returned...
*/
            if (cbText)
            {
/*
**             Compare this text to the requested text...
*/
               fResult  = DdeCompareStrings
                        ( pszItemText
                        , (PSZ) szText
                        , fCaseSensitive
                        , ConvContext.idCountry
                        , ConvContext.usCodepage
                        );
/*
**             If strings matched, we have our window...
*/
               if (fResult)
               {
                  hwndItem = hwndChild;
                  break;
               }
            }
         }
      }
/*
**    Clean-up enumeration resources...
*/
      fResult  = WinEndEnumWindows
               ( henum              /* enumeration handle                     */
               );
   }
/*
** Return result of search...
*/
   return (hwndItem);
}
/*==============================================================================
**
** Function:   DdeCreateMetafile
**
** Usage:      Creates a metafile from data in a DDESTRUCT structure.
**
==============================================================================*/
   HMF                              /* Metafile handle                        */
   APIENTRY    DdeCreateMetafile
(
   HPS         hps                  /* handle to presentation-space           */
,  PDDESTRUCT  pDdeStruct           /* address of DDESTRUCT structure         */
)
{
   #define COUNT_OPTIONS (LONG) (PMF_DEFAULTS + 1)
   #define LDESC    256L

   BOOL         fResult   = FALSE;
   HMF          hmf;
   HDC          hdc;
   PBYTE        pMFData   = (PBYTE) DDES_PABDATA (pDdeStruct);
   DEVOPENSTRUC dop;
/*
** Initialize 'Device Open Structure'...
*/
   dop.pszLogAddress      = NULL;             /* logical address              */
   dop.pszDriverName      = "DISPLAY";        /* driver name                  */
   dop.pdriv              = NULL;             /* DRIVDATA structure           */
   dop.pszDataType        = NULL;             /* data type                    */
   dop.pszComment         = NULL;             /* any comments                 */
   dop.pszQueueProcName   = NULL;             /* use default                  */
   dop.pszQueueProcParams = NULL;             /* no parameters required       */
   dop.pszSpoolerParams   = NULL;             /* no parameters required       */
   dop.pszNetworkParams   = NULL;             /* not used                     */
/*
** Open a 'Metafile' compatible Device Context...
*/
   hdc   = DevOpenDC                /* Create a Device context                */
         ( hab                      /* Handle to Anchor-block                 */
         , OD_METAFILE              /* Context used to hold metafiles         */
         , "*"                      /* No device info from init file          */
         , 9L                       /* # of items in pdopData parameter       */
         , (PDEVOPENDATA) &dop      /* Open device context data area          */
         , (HDC) NULL               /* Context compatible with screen         */
         );
/*
** If DC failed to be opened...
*/
   if (hdc == DEV_ERROR)
   {
      return (NULLHANDLE);
   }
/*
** Close the DC to obtain a handle to the empty metafile...
*/
   hmf   = DevCloseDC
         ( hdc                      /* handle to metafile compatible DC       */
         );
/*
** If closure failed, return indicating failure...
*/
   if (hmf == DEV_ERROR)
   {
      return (NULLHANDLE);
   }
/*
** Set the metafile-bits into the empty metafile
*/
   fResult  = GpiSetMetaFileBits
            ( hmf                   /* handle to empty metafile               */
            , 0L                    /* offset into metafile to begin          */
            , pDdeStruct->cbData    /* length of metafile data                */
            , pMFData               /* address of metafile data               */
            );
/*
** If Metafile-bits successfully set...
*/
   if (fResult)
   {
      BYTE   abDesc    [LDESC];
      LONG   alOptions [COUNT_OPTIONS];
      LONG   lSegCount;
      SIZEL  sizl;

      alOptions [PMF_SEGBASE]         = 0L;
      alOptions [PMF_LOADTYPE]        = LT_ORIGINALVIEW;
      alOptions [PMF_RESOLVE]         = 0L;
      alOptions [PMF_LCIDS]           = LC_LOADDISC;
      alOptions [PMF_RESET]           = RES_RESET;
      alOptions [PMF_SUPPRESS]        = SUP_SUPPRESS;
      alOptions [PMF_COLORTABLES]     = CTAB_REPLACE;
      alOptions [PMF_COLORREALIZABLE] = CREA_DEFAULT;
      alOptions [PMF_DEFAULTS]        = DDEF_DEFAULT;
/*
**    Play the Metafile into the specified Presentation Space...
*/
      fResult  = (BOOL) GpiPlayMetaFile
               ( hps                /* handle of Presentation Space           */
               , hmf                /* handle of Metafile                     */
               , COUNT_OPTIONS      /* count of options                       */
               , alOptions          /* list of options                        */
               , &lSegCount         /* reserved, zero always returned         */
               , LDESC              /* length of bytes in 'abDesc'            */
               , abDesc             /* descriptive record                     */
               );
   }
/*
** Return resulting Metafile handle...
*/
   return (hmf);
}
/*==============================================================================
**
** Function:   DdeQueryTextDimension
**
** Usage:      Determines the Dimensions (height and width) of a CF_TEXT
**             formatted buffer (in pels).
==============================================================================*/
   ULONG                            /* Count of lines                         */
   APIENTRY    DdeQueryTextDimension
(
   HPS         hps
,  PBYTE       pabText
,  ULONG       cbText
,  PULONG      pulWidth
,  PULONG      pulHeight
)
{
   ULONG       cch         = 0L;
   ULONG       cchMax      = 0L;
   ULONG       cLines      = 0L;
   ULONG       flResult    = 0L;
   BOOL        fResult     = FALSE;
   FONTMETRICS fmFontInfo;
/*
** Determine the last byte in the buffer...
*/
   PBYTE       pabTextMax  = pabText + cbText;
/*
** Parse buffer one byte at a time until end-of-buffer reached...
*/
   while (pabTextMax >= pabText)
   {
/*
**    If an end-of-line character found...
*/
      if (*pabText == CR
      ||  *pabText == LF
      ||  *pabText == EOT)
      {
/*
**       If the current count-of-characters exceed the largest line so far...
*/
         if (cch > cchMax)
         {
            cchMax = cch;           /* Save count to determine Max line size  */
         }
/*
**       Update counts...
*/
         cch  = 0;                  /* count-characters                       */
         cLines++;                  /* count-lines                            */
/*
**       Check if next character is also an end-of-line character, this occurs
**       if and when a line includes a CR/LF pair...
*/
         if (*(pabText + 1) == CR
         ||  *(pabText + 1) == LF)
         {
            pabText++;              /* skip this character...                 */
         }
      }
/*
**    Update counts...
*/
      cch++;                        /* count-characters                       */
      pabText++;                    /* pointer to next byte in buffer         */
   }
/*
** Calculate width and height...
*/
   *pulWidth   = cchMax             /* Maximum line width...                  */
               * cxChar;            /* multiplied by width of a character.    */
   *pulHeight  = cLines             /* Number of lines...                     */
               * cyLine;            /* multiplied by height of a line         */
/*
** Return number of lines...
*/
   return (cLines);
}
/*==============================================================================
**
** Function:   DdeCreateBitmap
**
** Usage:      Creates a bitmap from data in a DDESTRUCT structure.
**
==============================================================================*/
   HBITMAP                          /* Handle of Bitmap                       */
   APIENTRY    DdeCreateBitmap
(
   HPS         hps                  /* handle to presentation-space           */
,  PDDESTRUCT  pDdeStruct           /* address of DDESTRUCT structure         */
)
{
   BOOL        fResult;
   USHORT      cColors = 1;
   USHORT      cbRGB;
   HBITMAP     hbmp  = NULLHANDLE;
   RECTL       rcl;
   PBYTE       pbData;
/*
** Establish pointers to required bitmap structures...
*/
   PBITMAPINFO  pbmi  = (PBITMAPINFO)  DDES_PABDATA (pDdeStruct);
   PBITMAPINFO2 pbmi2 = (PBITMAPINFO2) DDES_PABDATA (pDdeStruct);
/*
** Check that bitmap data exists...
*/
   if (pDdeStruct->cbData)
   {
/*
**    Check if bitmap data is in 1.2 format...
*/
      if ( pbmi->cbFix == 12 )
      {
/*
**       Determine the number of colors supported...
**
**       Note: If the bitmap is 24-bits per/pel then no color table exists...
*/
         if (pbmi->cBitCount != 24)
         {
            cColors <<= pbmi->cBitCount;
         }
/*
**       Calculate color table information...
*/
         cbRGB   = cColors * ( (USHORT) sizeof (RGB) );
         pbData  = (PBYTE) &(pbmi->argbColor);
         pbData += cbRGB;
/*
**       Create a bitmap from the given data...
*/
         hbmp  = GpiCreateBitmap
               ( hps                         /* handle Presentation-space     */
               , (PBITMAPINFOHEADER2) pbmi   /* bit-map information header    */
               , CBM_INIT                    /* initialize the bitmap         */
               , pbData                      /* buffer address                */
               , (PBITMAPINFO2) pbmi         /* bit-map information table     */
               );
      }
/*
**    Otherwise bitmap data is in 2.0 format...
*/
      else
      {
/*
**       Determine the number of colors supported...
**
**       Note: If the bitmap is 24-bits per/pel then no color table exists...
*/
         if (pbmi2->cBitCount != 24)
         {
            cColors <<= pbmi2->cBitCount;
         }
/*
**       Calculate color table information...
*/
         cbRGB   = cColors * ( (USHORT) sizeof (RGB2) );
         pbData  = (PBYTE) &(pbmi2->argbColor);
         pbData += cbRGB;
/*
**       Create a bitmap from the given data...
*/
         hbmp  = GpiCreateBitmap
               ( hps                         /* handle Presentation-space     */
               , (PBITMAPINFOHEADER2) pbmi2  /* bit-map information header    */
               , CBM_INIT                    /* initialize the bitmap         */
               , pbData                      /* buffer address                */
               , pbmi2                       /* bit-map information table     */
               );
      }
   }
/*
** Return resulting bitmap handle...
*/
   return (hbmp);
}
/*==============================================================================
**
** Function:   DdeScanCommandString
**
** Usage:      Extract DDE commands strings from a CF_TEXT formatted buffer.
**
** Remarks:    This function is similar to the C-runtime sscanf function. It
**             reads pszBuffer, matching delimiters with pszFormat and extracts
**             tokens into a variable number of arguments.
**
**             The format string is matched with the buffer as follows:
**             following ways:
**
**             Format Buffer                     Action
**             ------ -------------------------- ------
**             Space  0 or more spaces           None
**             %s     1 or more alphabetic chars Copy to next arg (PSZ)
**             %d     1 or more numeric chars    Copy to next arg (PULONG)
**             Other  Exact match                Chars must match
**
**             pszBuffer      - Buffer to be tokenized
**             pszFormat      - Format string
**             fCaseSensitive - Case sensitivity flag
**             args...        - 0 or more ptrs to tokens to be extracted
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeScanCommandString
(
   PSZ         pszBuffer
,  PSZ         pszFormat
,  BOOL        fCaseSensitive
,  ...
)
{
#define FSCANNING       0x0000
#define FTOKEN_VALID    0x0001
#define FTOKEN_INVALID  0x0002

   USHORT     fsState  = FSCANNING;
   PSZ        pszArg;
   PULONG     pulArg;
   va_list    valArgs;

   va_start (valArgs, fCaseSensitive);
/*
** Extract tokens until told to stop...
*/
   while (fsState == FSCANNING)
   {
/*
**    What should we be looking for?
*/
      switch (*pszFormat)
      {
         case SPACE:
         {
/*
**          Expect 0 or more spaces, just skip over them...
*/
            while (*pszBuffer == SPACE)
            {
               pszBuffer++;
            }

            pszFormat++;
            break;
         }
         case '%':
         {
/*
**          Expect a token, but what type?
*/
            pszFormat++;
            switch (*pszFormat)
            {
               case 's':
               case 'S':
               {
/*
**                Expect a string, is it there?
*/
                  if (isalpha(*pszBuffer))
                  {
                     pszArg = va_arg( valArgs, PSZ );

                     while (isalpha(*pszBuffer))
                     {
                        *pszArg = *pszBuffer;
                        pszArg++;
                        pszBuffer++;
                     }
/*
**                   null terminate string...
*/
                     *pszArg = '\0';

/*
**                   Skip to next field...
*/
                     pszFormat++;
                  }
                  else
                  {
                     fsState = FTOKEN_INVALID;
                  }

                  break;
               }
               case 'd':
               case 'D':
               {
/*
**                Expect a number, is it there?
*/
                  if (isdigit(*pszBuffer))
                  {
                     pulArg  = va_arg( valArgs, PULONG );
                     *pulArg = 0;

                     while (isdigit(*pszBuffer))
                     {
                        *pulArg = (10 * *pulArg) + (ULONG)(*pszBuffer - '0');
                        pszBuffer++;
                     }

                     pszFormat++;
                  }
                  else
                  {
                     fsState = FTOKEN_INVALID;
                  }
                  break;
               }
               default:
               {
                  fsState = FTOKEN_INVALID;
                  break;
               }
            }
            break;
         }
         case '\0':
         {
            fsState = (*pszBuffer == '\0')
                    ? FTOKEN_VALID
                    : FTOKEN_INVALID;
            break;
         }
         default:
         {
/*
**          Expect a particular delimiter, is it there?
*/
            if ((fCaseSensitive && (*pszBuffer == *pszFormat))
            || (!fCaseSensitive && (toupper(*pszBuffer) ==
toupper(*pszFormat))))
            {
               pszBuffer++;
               pszFormat++;
            }
            else
            {
               fsState = FTOKEN_INVALID;
            }
            break;
         }
      }
   }
/*
** Return TRUE/FALSE depending on final state.
*/
   return (fsState == FTOKEN_VALID);
}
/*==============================================================================
**
** Function:   DdeExecuteCommands
**
** Usage:      Execute a string of commands contained in a CF_TEXT buffer.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeExecuteCommands
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
,  USHORT      usFormat             /* desired format                         */
,  PVOID       pvData               /* pointer to data                        */
,  ULONG       cbData               /* size of data (in bytes)                */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;

/*
** Create a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( NULL               /* item name-string (not applicable)      */
               , pvData             /* pointer to data                        */
               , cbData             /* size of data (in bytes)                */
               , DDEFMT_TEXT        /* text format                            */
               , 0                  /* DDE status flags (not applicable)      */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_EXECUTE     /*   an "Execute" message,                */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdePokeData
**
** Usage:      Poke data to DDE 'Client' window.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdePokeData
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
,  PSZ         pszItemName          /* desired item name-string               */
,  USHORT      usFormat             /* desired format                         */
,  PVOID       pvData               /* pointer to data                        */
,  ULONG       cbData               /* size of data (in bytes)                */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;
/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( pszItemName        /* null (0x00) terminated ASCII string    */
               , pvData             /* pointer to data                        */
               , cbData             /* size of data (in bytes)                */
               , usFormat           /* registered DDE format identifier       */
               , 0                  /* DDE status flags (not applicable)      */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_POKE        /*   a "Poke" message,                    */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
   }
   return (fResult);
}
/*==============================================================================
**
** Function:   DdeRequestColdLink
**
** Usage:      Requests a data item from a DDE 'Server' window.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeRequestColdLink
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
,  PSZ         pszItemName          /* desired item name-string               */
,  USHORT      usFormat             /* desired format                         */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;
/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( pszItemName        /* null (0x00) terminated ASCII string    */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (in bytes)                */
               , usFormat           /* registered DDE format identifier       */
               , 0                  /* DDE status flags (not applicable)      */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_REQUEST     /*   a "Request" message,                 */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdeRequestWarmLink
**
** Usage:      Requests that a permanent data-link be established with the
**             specified 'Server' window, and that the 'Server' post no data
**             in the resulting WM_DDE_DATA messages.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY   DdeRequestWarmLink
(
   HWND       hwndServer            /* desired server window handle           */
,  HWND       hwndClient            /* requesting window handle               */
,  PSZ        pszItemName           /* desired item name-string               */
,  USHORT     usFormat              /* desired format                         */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;
/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( pszItemName        /* null (0x00) terminated ASCII string    */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (in bytes)                */
               , usFormat           /* registered DDE format identifier       */
               , DDE_FNODATA        /* send no data in updates                */
               | DDE_FACKREQ        /* set DDE_FACKREQ status flag in         */
               );                   /*   subsequent WM_DDE_DATA messages      */
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_ADVISE      /*   a "UnAdvise" message,                */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdeRequestHotLink
**
** Usage:      Requests that a permanent data-link be established with the
**             specified 'Server' window, and that the 'Server' post data
**             in the resulting WM_DDE_DATA messages.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeRequestHotLink
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
,  PSZ         pszItemName          /* desired item name-string               */
,  USHORT      usFormat             /* desired format                         */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;

/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( pszItemName        /* null (0x00) terminated ASCII string    */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (in bytes)                */
               , usFormat           /* registered DDE format identifier       */
               , DDE_FACKREQ        /* set DDE_FACKREQ status flag in         */
               );                   /*   subsequent WM_DDE_DATA messages      */
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_ADVISE      /*   a "Advise" message,                  */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdeTerminateLink
**
** Usage:      Terminates a previously established data-link.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeTerminateLink
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
,  PSZ         pszItemName          /* desired item name-string               */
,  USHORT      usFormat             /* desired format                         */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;
/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( pszItemName        /* null (0x00) terminated ASCII string    */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (in bytes)                */
               , usFormat           /* registered DDE format identifier       */
               , 0                  /* DDE status flags (not applicable)      */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_UNADVISE    /*   a "UnAdvise" message,                */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdeTerminate
**
** Usage:      Terminates a previously established conversation link.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeTerminate
(
   HWND        hwndServer           /* desired server window handle           */
,  HWND        hwndClient           /* requesting window handle               */
)
{
   BOOL        fResult    = FALSE;
   PDDESTRUCT  pDdeStruct = NULL;
/*
** Creates a DDE shared memory object...
*/
   pDdeStruct  = DdeMakeDdeStruct   /* create a shared memory object          */
               ( NULL               /* item name-string (not applicable)      */
               , NULL               /* pointer to data (not applicable)       */
               , 0L                 /* size of data (in bytes)                */
               , 0                  /* registered DDE format identifier       */
               , 0                  /* DDE status flags (not applicable)      */
               );
/*
** If shared memory object successfully created...
*/
   if (pDdeStruct)
   {
      fResult  = WinDdePostMsg      /* post a DDE message to...               */
               ( hwndServer         /*   this requesting window handle,       */
               , hwndClient         /*   from this responding window handle,  */
               , WM_DDE_TERMINATE   /*   a "Terminate" message,               */
               , pDdeStruct         /*   as defined in this memory object,    */
               , DDEPM_RETRY        /*   and retry if receiving queue is full.*/
               );
  }
  return (fResult);
}
/*==============================================================================
**
** Function:   DdeSetPastLink
**
** Usage:      Sets the 'Link' format in the system clipboard.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DdeSetPasteLink
(
   PSZ         pszAppName
,  PSZ         pszTopic
,  PSZ         pszItemName
)
{
   APIRET      ApiRet         = 0;
   BOOL        fResult        = FALSE;
   PVOID       pvResult       = NULL;
   USHORT      cbResult       = 0;
   USHORT      cbLinkData     = strlen (pszAppName)
                              + strlen (pszTopic)
                              + strlen (pszItemName)
                              + 4;
   CHAR        szLinkFmt []   = "%s@%s@%s@";
   PVOID       pvLinkData;
/*
** Open system clipboard...
*/
   fResult  = WinOpenClipbrd
            ( hab                   /* handle anchor-block                   */
            );
/*
** If clipboard opened...
*/
   if (fResult)
   {
/*
**    Allocate shared memory object to hold clipboard format...
*/
      ApiRet   = DosAllocSharedMem
               ( &pvLinkData        /* address to receive memory address      */
               , NULL               /* use unnamed shared memory              */
               , cbLinkData         /* size of memory to allocate (in bytes)  */
               , PAG_COMMIT         /* commit all pages                       */
               | PAG_WRITE          /* memory object is writable             */
               | PAG_READ           /* memory object is readable              */
               | OBJ_GIVEABLE       /* memory object can be given             */
               );
/*
**    If shared memory object allocated...
*/
      if (ApiRet == NO_ERROR)
      {
         PBYTE    pbLinkData = (PBYTE) pvLinkData;
/*
**       Expand and translate link format parameters into buffer...
**
**       Note: Buffer will contain tokens ('@') that are replaced later with
**             a null (0x00) character.  This (token) technique is used in
**             order to utilize the 'sprintf' function.  That is, a null
**             character could not be used since it would be interpreted as the
**             end-of-the format string.
*/
         cbResult = sprintf
                  ( pvLinkData
                  , szLinkFmt
                  , pszAppName
                  , pszTopic
                  , pszItemName
                  );
/*
**       Scan buffer...
*/
         while (*pbLinkData)
         {
/*
**          For place holder tokens and replace if found...
*/
            if (*pbLinkData == '@')
            {
               *pbLinkData = '\0';
            }
            pbLinkData++;
         }
/*
**       Empty the system clipboard...
*/
         fResult  = WinEmptyClipbrd
                  ( hab
                  );
/*
**       Set the 'Link' format on the system clipboard...
*/
         fResult  = WinSetClipbrdData
                  ( hab
                  , (ULONG) pvLinkData
                  , usFmtLink
                  , CFI_POINTER
                  );
      }
      else
      {
         fResult = FALSE;
      }
   }
/*
** Close system clipboard...
*/
   WinCloseClipbrd (hab);
/*
** Return success indicator...
*/
  return (fResult);
}
