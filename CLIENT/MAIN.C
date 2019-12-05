/*==============================================================================
**
** Module:     Main.c
**
** Remarks:    DDE Client main module.
**
**============================================================================*/
/*
** System specific defines.
*/
   #define  INCL_DOS
   #define  INCL_DEV
   #define  INCL_ERRORS
   #define  INCL_GPI
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
   #include <menu.h>
   #include <common.h>
   #include <draw.h>
   #include <dde.h>
   #include <resource.h>
   #include <dialogs.h>
   #include <frame.h>
   #include <client.h>
/*
** Private data items and structures.
*/
   ULONG    aulCpList [MAX_CODEPAGES];
   CHAR     szText [MAX_TEXTLENGTH];

/*==============================================================================
** Function:   Main
**============================================================================*/
   VOID                             /* returns no value                       */
               main                 /* main program entry point               */
(
   VOID                             /* no formal parameters                   */
)
{
   APIRET      ApiRet;
   ULONG       cbReturned;
   BOOL        fResult;
   MRESULT     MResult;
   QMSG        qmsg;
/*
** Allocate a piece of memory that we will use as a heap.  We allocate various
** DDE control structures from this heap as needed.
*/
   ApiRet   = DosAllocMem
            ( &pHeap
            , (ULONG) MIN_HEAPSIZE
            , PAG_WRITE
            | PAG_READ
            );
/*
** If heap allocation failed...
*/
   if (ApiRet != NO_ERROR)
   {
      return;                       /* exit application                       */
   }
/*
** Initialize the heap...
*/
   ApiRet   = DosSubSetMem
            ( pHeap
            , DOSSUB_INIT
            | DOSSUB_SPARSE_OBJ
            , (ULONG) MIN_HEAPSIZE
            );
/*
** If Initialization failed...
*/
   if (ApiRet != NO_ERROR)
   {
      return;                       /* exit application                       */
   }
/*
** Registration of exist list routine.  This routine will be called
** whenever this application exits.  This will be our last chance to clean
** up resources.
*/
   if (DosExitList (EXLST_ADD, (PFNEXITLIST) ExitProc))
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Query the handle to the system atom table.  This will be required later
** for data formats (which are registered in the system atom table).
*/
   if ((hAtomTable = WinQuerySystemAtomTable ()) == (HATOMTBL) NULL)
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Register each of the DDE formats that we are going to support.  If any
** of the reqistrations fail, exit the program.
*/
   if (!(usFmtText     = DdeRegisterFmt (hAtomTable, SZFMT_TEXT))
   ||  !(usFmtCPText   = DdeRegisterFmt (hAtomTable, SZFMT_CPTEXT))
   ||  !(usFmtBitmap   = DdeRegisterFmt (hAtomTable, SZFMT_BITMAP))
   ||  !(usFmtPalette  = DdeRegisterFmt (hAtomTable, SZFMT_PALETTE))
   ||  !(usFmtMetaFile = DdeRegisterFmt (hAtomTable, SZFMT_METAFILE))
   ||  !(usFmtLink     = DdeRegisterFmt (hAtomTable, SZFMT_LINK))
   ||  !(usFmtTime     = DdeRegisterFmt (hAtomTable, "DDEFMT_Time")) )
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Initialize PM facilities for this application.  If initialization fails
** (indicated by hab having a value of zero (0)), exit this application.
*/
   hab   = WinInitialize
         ( 0                        /* options. Reserved must be zero (0)     */
         );
/*
** If Initialization failed, exit the application.
*/
   if (hab == (HAB) 0)
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Query the window handle of the desktop window.  The Desktop window handle
** is used as the parent of any message window displayed by this program.
*/
   hwndDesktop = WinQueryDesktopWindow
               ( hab                /* handle Anchor-block                   */
               , NULLHANDLE         /* handle device-context (screen)         */
               );
/*
** Create application message queue...
*/
   hmq   = WinCreateMsgQueue
         ( hab                      /* handle Anchor-block                   */
         , 0                        /* use the system default queue size      */
         );
/*
** If creation of message queue failed, exit the application.
*/
   if (hmq == (HMQ) NULL)
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Query current country and code page information of client application...
*/
   CtryCode.country  = 0L;          /* Query default system country code      */
   CtryCode.codepage = 0L;          /* Query current process codepage         */

   ApiRet   = DosQueryCtryInfo      /* Query country-dependent information    */
            ( sizeof (CtryInfo)     /* Size in bytes to hold returned info    */
            , &CtryCode             /* specifies query criteria               */
            , &CtryInfo             /* buffer to receive queried information  */
            , &cbReturned           /* size (in bytes) of returned information*/
            );
/*
** If query failed, exit the application.
*/
   if (ApiRet)
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Setup the Clients's default conversation context structure.  These values
** define the context for NLS conversations.
*/
   ConvContext.cb          = sizeof (CONVCONTEXT);
   ConvContext.fsContext   = 0L;
   ConvContext.idCountry   = CtryInfo.country;
   ConvContext.usCodepage  = WinQueryCp (hmq);
   ConvContext.usLangID    = 0L;
   ConvContext.usSubLangID = 0L;
/*
** Register all window classes and create initial windows required by the
** client application.  If either function fails, exit the application.
*/
   if (!RegisterWindowClasses (hab)
   ||  !CreateWindows (hab))
   {
      DosExit (EXIT_PROCESS, EXIT_ERROR);
   }
/*
** Get the string containing the name of this application from the resource
** file.
*/
   cbReturned  = WinLoadString      /* load a string from a resource          */
               ( hab                /* anchor-block handle of this application*/
               , (HMODULE) 0        /* load string from executable            */
               , IDS_APP_NAME       /* identifier of string to be loaded      */
               , MAX_TEXTLENGTH     /* size of buffer to receive string       */
               , szAppName          /* address of buffer to receive string    */
               );
/*
** If Application name-string loaded...
*/
   if (cbReturned)
   {
      fResult  = WinSetWindowText   /* set window text of main frame window   */
               ( hwndFrame          /* window handle of main frame window     */
               , szAppName          /* window text                            */
               );
   }
/*
** Notify the frame window that controls have been added...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndFrame
            , WM_UPDATEFRAME
            , MPVOID                /* frame-creation flags (not used)        */
            , MPVOID                /* reserved value, zero (0)               */
            );
/*
** Show the frame window with updated controls...
*/
   fResult  = WinShowWindow         /* set visibility state of...             */
            ( hwndFrame             /*    this window to...                   */
            , TRUE                  /*    visible.                            */
            );
/*
** While message is not WM_QUIT...
*/
   while ( WinGetMsg                /* get message from threads message queue */
         ( hab                      /* handle to anchor-block                 */
         , &qmsg                    /* address of structure to hold message   */
         , (HWND) 0                 /* window handle of message filter (none) */
         , 0L                       /* start of message filter range (any)    */
         , 0L                       /* end of message filter range (any)      */
         ))
   {
/*
**    Save time message was received.  Later, when a DDE message is received,
**    this time is included in the text of a listbox entry.  This serves as a
**    method to determine the sequence of messages in a DDE conversation.
*/
      ulMsgTime = qmsg.time;

      MResult  = WinDispatchMsg
               ( hab                /* handle Anchor-block                    */
               , &qmsg              /* location of queue message received     */
               );
   }
/*
** Exit the application...
*/
   DosExit (EXIT_PROCESS, EXIT_OK);
}
/*==============================================================================
**
** Function:   ExitProc
**
** Usage:      Exit list processing.
**
** Remarks:    This function is called when the applications process is
**             exiting.  This allows and application to clean up any resources
**             in a single function regardless of how or where this application
**             exits.
**============================================================================*/
   VOID        ExitProc
(
   ULONG       ulExitCode
)
{
   APIRET      ApiRet;
   BOOL        fResult;
/*
** If a Micro presentation-space was created, remove any device-context that
** is associated with the presentation-space.
*/
   if (hpsMicro)
   {
      fResult  = GpiAssociate
               ( hpsMicro           /* handle Micro presentation-space  (ps)  */
               , (HDC) NULL         /* handle device-context                  */
               );
      fResult  = GpiDestroyPS
               ( hpsMicro           /* handle presentation-space              */
               );
   }
/*
** If a memory device-context was created...
*/
   if (hdcMemory)
   {
      DevCloseDC (hdcMemory);
   }
/*
** If a listbox exists, purge all items and its associated memory objects...
*/
   if (hwndListBox)
   {
      fResult  = FrmPurgeListboxItems
               ( hwndListBox
               );
   }
/*
** If a frame window was created, destroy it...
*/
   if (hwndFrame)
   {
      WinDestroyWindow (hwndFrame);
   }
/*
** If the system atom-table was queried, unregister any "public" DDE formats
** that might have been created...
*/
  if (hAtomTable)
  {
     if (usFmtText)     DdeUnRegisterFmt (hAtomTable, usFmtText);
     if (usFmtCPText)   DdeUnRegisterFmt (hAtomTable, usFmtCPText);
     if (usFmtBitmap)   DdeUnRegisterFmt (hAtomTable, usFmtBitmap);
     if (usFmtPalette)  DdeUnRegisterFmt (hAtomTable, usFmtPalette);
     if (usFmtLink)     DdeUnRegisterFmt (hAtomTable, usFmtLink);
     if (usFmtMetaFile) DdeUnRegisterFmt (hAtomTable, usFmtMetaFile);
     if (usFmtTime)     DdeUnRegisterFmt (hAtomTable, usFmtTime);
  }
/*
** If a message queue was created, destroy it...
*/
   if (hmq)
   {
      fResult  = WinDestroyMsgQueue
               ( hmq                /* handle message-queue to destroy        */
               );
   }
/*
** If application successfully initialized, terminate this applications use of
** Presentation Manger and release all resources...
*/
   if (hab)
   {
      fResult  = WinTerminate
               ( hab                /* handle Anchor-block                   */
               );
   }
/*
** If private heap created....
*/
   if (pHeap)
   {
      ApiRet   = DosFreeMem         /* free memory object                     */
               ( pHeap              /* base address to free                   */
               );
   }
/*
** Finally, exit the application...
*/
   DosExitList (EXLST_EXIT, (PFNEXITLIST) NULL);
}
/*==============================================================================
**
** Function:   CreateWindows
**
** Usage:      Creates all required windows to support this application.
**
** Remarks:    This function is called when the application is starting.  It
**             creates all required windows and obtains static information
**             about its windows and the system necessary for proper execution.
**============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    CreateWindows
(
   HAB         hab
)
{
   APIRET      ApiRet;
   BOOL        fResult;
   FRAMECDATA  fcData;
   ULONG       cbReturned;
   PCONVINFO   pConvInfo;
   RECTL       rclDialog;
   RECTL       rclStatus;
/*
** Allocate a memory object large enough to contain a conversation information
** (CONVINFO) structure.
*/
   if ((pConvInfo = (PCONVINFO) DdeAllocMem (sizeof (CONVINFO))) == NULL)
   {
      return (FALSE);               /* allocation failed, return error        */
   }
/*
** Initialize frame creation structure...
*/
   fcData.cb            = sizeof (fcData);
   fcData.flCreateFlags = FCF_ICON          /* Frame has associated icon      */
                        | FCF_MENU          /* Frame has menu control         */
                        | FCF_MINMAX        /* Frame has min/max buttons      */
                        | FCF_SHELLPOSITION /* Shell determines frame size    */
                        | FCF_SIZEBORDER    /* Frame is sizeable              */
                        | FCF_SYSMENU       /* System menu                    */
                        | FCF_TASKLIST      /* Enter in task list             */
                        | FCF_TITLEBAR      /* Title bar                      */
                        | FCF_VERTSCROLL    /* Vertical scroll bar            */
                        | FCF_HORZSCROLL;   /* Horizontal scroll bar          */
   fcData.hmodResources = (HMODULE) 0;      /* Use resource in .exe file      */
   fcData.idResources   = IDR_MAIN;         /* Resource identifier            */
/*
** Create the application "Frame" window...
*/
   hwndFrame   = WinCreateWindow
               ( HWND_DESKTOP       /* Our parent window handle               */
               , WC_FRAME           /* Window class                           */
               , NULL               /* Window title                           */
               , 0L                 /* Window style                           */
               , 0                  /* Horizontal window position             */
               , 0                  /* Vertical window postion                */
               , 0                  /* Window width                           */
               , 0                  /* Window height                          */
               , (HWND) 0           /* Owner-window handle                    */
               , HWND_TOP           /* Z-Order placement (sibling)            */
               , IDR_MAIN           /* Window Identifier                      */
               , &fcData            /* Pointer to creation data               */
               , NULL               /* Presentation parameters                */
               );
/*
** Check if Frame window was created successfully...
*/
   if (hwndFrame == (HWND) 0)
   {
      return (FALSE);               /* if not, return error indicator         */
   }
/*
** We subclass the frame window to limit the size of the tracking rectangle
** and to add our own controls.  Save the original frame window procedure for
** later use during frame window processing...
*/
   pfnOldFrameWindowProc   = WinSubclassWindow
                           ( hwndFrame
                           , FrameWindowProc
                           );
/*
** Obtain handles of commonly referenced windows...
*/
   hwndMenu          = WinWindowFromID
                     ( hwndFrame
                     , (USHORT) FID_MENU
                     );
   hwndMenuServers   = MnuQuerySubMenu
                     ( hwndMenu
                     , (USHORT) IDM_SERVERS
                     );
   hwndMenuCodepages = MnuQuerySubMenu
                     ( hwndMenu
                     , (USHORT) IDM_LINKS_CODEPAGES
                     );
   hwndMenuDisplay   = MnuQuerySubMenu
                     ( hwndMenu
                     , (USHORT) IDM_DISPLAY
                     );
   hwndHorzScroll    = WinWindowFromID
                     ( hwndFrame
                     , (USHORT) FID_HORZSCROLL
                     );
   hwndVertScroll    = WinWindowFromID
                     ( hwndFrame
                     , (USHORT) FID_VERTSCROLL
                     );
/*
** Check if any required window failed to be created...
*/
   if (hwndMenu          == (HWND) 0
   ||  hwndMenuCodepages == (HWND) 0
   ||  hwndMenuDisplay   == (HWND) 0
   ||  hwndHorzScroll    == (HWND) 0
   ||  hwndVertScroll    == (HWND) 0)
   {
      return (FALSE);               /* if any failed, return error indicator  */
   }
/*
** Create 'client' window...
*/
   hwndClient  = WinCreateWindow
               ( hwndFrame          /* This windows parent                    */
               , WC_Client          /* Window class name                      */
               , NULL               /* Window title - not used                */
               , WS_DISABLED        /* Window style - disabled                */
               | WS_SYNCPAINT       /* Window style - paint when re-sized     */
               , 0L                 /* Horizontal (x position)                */
               , 0L                 /* Vertical   (y postion)                 */
               , 0L                 /* Width      (cx)                        */
               , 0L                 /* Height     (cy)                        */
               , (HWND) 0           /* Owner window - not used                */
               , HWND_BOTTOM        /* Place on BOTTOM of Z-ORDER             */
               , FID_CLIENT         /* Client window ID                       */
               , pConvInfo          /* Creation data                          */
               , NULL               /* Presentation parameters - not used     */
               );
/*
** Check if window creation failed...
*/
   if (hwndClient == (HWND) 0)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }
/*
** We subclass the client window to implement DDE functionality, so save the
** original client window procedure for later use during window processing...
*/
   pfnOldClientWindowProc  = WinSubclassWindow
                           ( hwndClient
                           , ClientWindowProc
                           );
/*
** Create a "listbox" window to capture incoming DDE messages...
*/
   hwndListBox = WinCreateWindow
               ( hwndFrame          /* This windows parent                    */
               , WC_LISTBOX         /* Window class name                      */
               , NULL               /* Window title - not used                */
               , LS_NOADJUSTPOS
               | WS_SYNCPAINT
               | WS_VISIBLE
               , 0L                 /* Horizontal (x position)                */
               , 0L                 /* Vertical   (y postion)                 */
               , 0L                 /* Width      (cx)                        */
               , 0L                 /* Height     (cy)                        */
               , hwndClient
               , HWND_BOTTOM        /* Place on BOTTOM of Z-ORDER             */
               , FID_ListBox
               , NULL               /* Creation data - not used               */
               , NULL               /* Presentation parameters - not used     */
               );
/*
** Check if window creation failed...
*/
  if (hwndListBox == (HWND) 0)
  {
     return (FALSE);                /* if it did, return error indicator      */
  }
/*
** Load a "Dialog" template from our resource file and create a window to
** capture DDE message specific data...
*/
   hwndDialog  = WinLoadDlg
               ( hwndFrame
               , hwndFrame
               , WinDefDlgProc
               , 0L
               , IDD_STRUCT
               , NULL
               );
/*
** Check if window creation failed...
*/
   if (hwndDialog == (HWND) 0)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }
/*
** Create a "Static" window to display messages...
*/
   hwndStatus  = WinCreateWindow
               ( hwndFrame          /* This windows parent                    */
               , WC_STATIC          /* Window class name                      */
               , NULL               /* Window title - not used                */
               , WS_SYNCPAINT
               | WS_VISIBLE
               | SS_TEXT
               , 0L                 /* Horizontal (x position)                */
               , 0L                 /* Vertical   (y postion)                 */
               , 0L                 /* Width      (cx)                        */
               , 0L                 /* Height     (cy)                        */
               , hwndFrame
               , HWND_BOTTOM        /* Place on BOTTOM of Z-ORDER             */
               , FID_Status
               , NULL               /* Creation data - not used               */
               , NULL               /* Presentation parameters - not used     */
               );
/*
** Check if window creation failed...
*/
   if (hwndStatus == (HWND) 0)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }
/*
** Obtain window dimensions of "Dialog" and "Status" windows...
*/
   fResult  = WinQueryWindowRect
            ( hwndDialog
            , &rclDialog
            );
   fResult  = WinQueryWindowRect
            ( hwndStatus
            , &rclStatus
            );
/*
** Calculate minimum size of tracking rectangle...  (to prevent loss of data)
*/
   cxMinTrackSize  = WinQuerySysValue (HWND_DESKTOP, SV_CXBORDER    ) * 2;
   cxMinTrackSize += WinQuerySysValue (HWND_DESKTOP, SV_CXSIZEBORDER) * 2;
   cxMinTrackSize += WinQuerySysValue (HWND_DESKTOP, SV_CXVSCROLL);
   cxMinTrackSize += rclDialog.xRight - rclDialog.xLeft;
   cxMinTrackSize += 3 * cxChar;
   cyMinTrackSize  = WinQuerySysValue (HWND_DESKTOP, SV_CYMENU      );
   cyMinTrackSize += WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR  );
   cyMinTrackSize += WinQuerySysValue (HWND_DESKTOP, SV_CYSIZEBORDER) * 2;
   cyMinTrackSize += WinQuerySysValue (HWND_DESKTOP, SV_CYBORDER    ) * 2;
   cyMinTrackSize += rclDialog.yTop - rclDialog.yBottom;
   cyMinTrackSize += rclStatus.yTop - rclStatus.yBottom;
   cyMinTrackSize += 3 * cyLine;
/*
** Obtain current codepage information...
*/
   ApiRet   = DosQueryCp
            ( sizeof(aulCpList)     /* size (in bytes) of buffer to hold list */
            , (PULONG) &aulCpList   /* address of buffer to receive list      */
            , &ulCpSize             /* receives size (in bytes) of actual list*/
            );
/*
** Check if codepages were obtained and the "Codepage" menu was created...
*/
   if (ApiRet == NO_ERROR           /* if DosQueryCp was successful,          */
   &&  hwndMenuCodepages)           /* and our sub-menu exists...             */
   {                                /*   add them to our sub-menu.            */
      MENUITEM mi;                  /* menu-item structure                    */
      SHORT    sItemId;             /* menu-item identifier                   */
      USHORT   usCpList;            /* index into Codepage list               */
      CHAR     szCodepage  [16];    /* buffer to hold converted codepage value*/
      PSZ      pszCodepage;         /* pointer to converted codepage buffer   */

      mi.iPosition    = MIT_END;    /* insert new menu-item at end of the list*/
      mi.afStyle      = MIS_TEXT;   /* item style is NULL terminated text     */
      mi.afAttribute  = MIA_CHECKED;/* item is Enabled                        */
      mi.hwndSubMenu  = (HWND) 0;   /* item has no sub-menus                  */
      mi.hItem        = 0L;         /* not applicable to MIS_TEXT items       */
/*
**    Add system prepared codepages to our "Codepages" menu item.  We start
**    at the second entry in the list, this is the first prepared system
**    codepage (the first is the current codepage).  Continue until
*/
      for ( usCpList = 1; usCpList < MAX_CODEPAGES; usCpList++)
      {
/*
**       Convert codepage value into a base 10 text string...
*/
         pszCodepage = _ultoa
                     ( aulCpList[usCpList]
                     , szCodepage
                     , 10
                     );
/*
**       Menu-item identifier becomes the current index added to the base
**       sub-menu identifier for the "Codepages" menu-item.
*/
         mi.id   = IDM_LINKS_CODEPAGES_FIRST + usCpList;
/*
**       Add the codepage to our menu...
*/
         sItemId = MnuAddItem          /* add an item to a sub-menu           */
                 ( hwndMenuCodepages   /* window handle of sub-menu           */
                 , &mi                 /* menu-item structure                 */
                 , pszCodepage         /* menu-item text                      */
                 );

         mi.afAttribute = 0;
      }
   }

   return (TRUE);
}
/*==============================================================================
** Function:   FrameWindowProc
**
** Usage:      Provides application-specific behavior for WC_FRAME class
**             windows.
**
** Remarks:    This window procedure sub-classes the original window procedure
**             of a WC_FRAME class window.  This is required to control the
**             size of the "Tracking" rectangle, add custom controls, and to
**             initialize menu items.
**============================================================================*/
   MRESULT
   EXPENTRY    FrameWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   MRESULT     MResult;

   switch (ulMsgId)
   {
/*
**    If a menu is selected...
*/
      case WM_INITMENU:
      {
/*
**       ... and it was the "Links" menu-item...
*/
         if (SHORT1FROMMP (mp1) == IDM_LINKS)
         {
/*
**          Check if the "Link" format exists in the "clipboard"...
*/
            ULONG ulFmtInfo;
/*
**          Open the clipboard...
*/
            fResult  = WinOpenClipbrd
                     ( hab          /* handle anchor-block                    */
                     );
/*
**          See if the "Link" format exists...
*/
            fResult  = WinQueryClipbrdFmtInfo
                     ( hab          /* handle anchor-block                    */
                     , usFmtLink    /* format identifier                      */
                     , &ulFmtInfo   /* buffer to hold format information      */
                     );
/*
**          Enable the "Paste Special..." menu item accordingly...
*/
            fResult  = WinEnableMenuItem
                     ( HWNDFROMMP (mp2)      /* menu window handle            */
                     , IDM_LINKS_PASTE_LINK  /* menu item identifier          */
                     , (fResult)             /* result of query               */
                     ? TRUE                  /* TRUE = format exists,         */
                     : FALSE                 /* FALSE = format does not exist */
                     );
/*
**          Close the clipboard...
*/
            fResult  = WinCloseClipbrd
                     ( hab          /* handle anchor-block                    */
                     );
         }
         break;
      }
/*
**    Frame is formatting...
*/
      case WM_FORMATFRAME:
      {
         MResult  = FrmWmFormatFrame
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
/*
**    Query count of frame controls...
*/
      case WM_QUERYFRAMECTLCOUNT:
      {
         MResult  = FrmWmQueryFrameCtlCount
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
/*
**    Query minimum size of tracking rectangle...
*/
      case WM_QUERYTRACKINFO:
      {
         MResult  = FrmWmQueryTrackInfo
                  ( hwnd
                  , ulMsgId
                  , mp1
                  , mp2
                  );
         break;
      }
/*
**    All other messages...
*/
      default:
      {
         MResult = (*pfnOldFrameWindowProc) (hwnd, ulMsgId, mp1, mp2);
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
** Function:   ClientWindowProc
**
** Usage:      Provides application-specific behavior for WC_Client class
**             windows.
**
** Remarks:    This window procedure sub-classes the original window procedure
**             of a WC_Client class window.  This is required to add DDE Client
**             or DDE Server specific functionality to the "common" client
**             window procedure.
**============================================================================*/
   MRESULT
   EXPENTRY    ClientWindowProc
(
   HWND        hwnd
,  ULONG       ulMsgId
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   MRESULT     MResult;

   switch (ulMsgId)
   {
      case UM_InitiateAck:
      {
         MResult  = UmInitiateAck
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_Terminate:
      {
         MResult  = UmTerminate
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_COMMAND:
      {
         MResult  = WmCommand
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DDE_INITIATEACK:
      {
         MResult  = WmDdeInitiateAck
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_DESTROY:
      {
         MResult  = WmDestroy
                  ( hwnd
                  );
         break;
      }
      case WM_MENUSELECT:
      {
         MResult  = WmMenuSelect
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      default:
      {
         MResult = (*pfnOldClientWindowProc) (hwnd, ulMsgId, mp1, mp2);
         break;
      }
   }
   return (MResult);
}
/*==============================================================================
** Function:   WmCommand
**
** Usage:      Processes a WM_COMMAND message dispatched to a "WC_Client" class
**             window.
**
** Remarks:    A WM_COMMAND messages occurs when a control has a significant
**             event to notify to its owner, such as a selection has been made.
**============================================================================*/
   MRESULT
   APIENTRY    WmCommand
(
   HWND        hwnd
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   SHORT       cItems;
   BOOL        fChecked;
   BOOL        fResult;
   HWND        hwndConv    = hwndClient;
   PSZ         pszAppName  = pszNull;
   PSZ         pszTopic    = pszNull;
   ULONG       ulReply;
   USHORT      usCmd       = SHORT1FROMMP (mp1);
   USHORT      usSource    = SHORT1FROMMP (mp2);
   USHORT      usPointer   = SHORT2FROMMP (mp2);
   USHORT      usResponse;
/*
** If the user has selected the "Initiate..." menu-item or any of the
** Server/Topic name entries...
*/
   if (usCmd == IDM_LINKS_INITIATE
   || (usCmd >= IDM_TOPICS_FIRST && usCmd <= IDM_TOPICS_LAST))
   {
/*
**    Query the current state of the menu-item...
*/
      fChecked = WinIsMenuItemChecked
               ( hwndMenuServers    /* handle of base menu window             */
               , usCmd              /* menu-item identifier                   */
               );
/*
**    If the menu-item is checked, then the Application/Topic name pair is
**    engaged in a DDE conversation.  At this point, such a selection is
**    interpreted as a request on the behalve of the user for a data item
**    associated with that conversation link.  If this case, broadcast the
**    request to all child windows.  Conversation (child) windows process
**    this request.
*/
      if (fChecked)
      {
         fResult  = WinBroadCastMsg
                  ( hwnd
                  , UM_ShowItemDialog
                  , MPFROMHWND (hwndMenuServers)
                  , MPFROM2SHORT (sTopicSelected, sAppSelected)
                  , BMSG_POST
                  );
      }
      else
      {
/*
**    Otherwise, the menu-item is not checked.  This indicates that the
**    Application/Topic name pair is not engaged in a DDE conversation. At
**    this point the selection is interpreted as a request for initiation of
**    a DDE conversation link.
*/
         ulReply  = WinDlgBox
                  ( HWND_DESKTOP                   /* handle of parent-window */
                  , hwnd                           /* handle of owner-window  */
                  , (PFNWP) DlgInitiateWindowProc  /* dialog window procedure */
                  , 0                              /* resource identifier
*/
                  , IDD_INITIATE                   /* dialog identifier
*/
                  , NULL                           /* creation parameters     */
                  );
/*
**       If the user's reply is to proceed with the initiation request...
*/
         if (ulReply == IDC_INITIATE_OK)
         {
/*
**          Obtain lengths of application and topic names...
*/
            USHORT cbAppName = strlen (szAppName);
            USHORT cbTopic   = strlen (szTopic);
/*
**          Check if names were entered, if not, set pointers to a zero-length
**          ASCII text string...
*/
            pszAppName  = (cbAppName)
                        ? (PSZ) szAppName
                        : pszNull;
            pszTopic    = (cbTopic)
                        ? (PSZ) szTopic
                        : pszNull;
/*
**          The application and topic name fields can be modified.  If if both
**          fields contain text, the user is requesting that a specific
**          conversation link be established.  Currently, the example program
**          can only handle one conversation link per application/topic name
**          pair.  Therefore, check if this application is already engaged in a
**          conversation link with the requested application/topic name pair...
*/
            if (cbAppName && cbTopic)
            {
               if (IsLinkActive (pszAppName, pszTopic))
               {
/*
**                If the links is already active, notify the user...
*/
                  fResult  = FrmSetStatusText
                           ( hab
                           , hwndStatus
                           , NULL
                           , IDS_LINK_ALREADY_ACTIVE
                           , WA_ERROR
                           );

                  return MRFROMSHORT (0);
               }
               else
               {
/*
**                The application/topic name pair is not engaged in a DDE
**                conversation, so create a 'Conversation-window' to support
**                the link.
*/
                  hwndConv = DdeCreateConvWindow
                          ( hwnd
                          , NULLHANDLE
                          , pszAppName
                          , pszTopic
                          , &ConvContext
                          );
/*
**                If conversation-window successfully created...
*/
                  if (hwndConv)
                  {
/*
**                   Issue the DDE Initiation request from this newly created
**                   window...
*/
                     fResult  = WinDdeInitiate
                              ( hwndConv
                              , pszAppName
                              , pszTopic
                              , &ConvContext
                              );
/*
**                   Check the state of the conversation-window after issuing
**                   the request.  If the window is "Enabled", then at least
**                   one server responded, otherwise no servers responded.  If
**                   no server responded, notify the user...
*/
                     if (WinIsWindowEnabled (hwndConv) == FALSE)
                     {
                        usResponse  = FrmSetStatusText
                                    ( hab
                                    , hwndStatus
                                    , NULL
                                    , IDS_NO_SERVER_RESPONDED
                                    , WA_NOTE
                                    );
/*
**                      Destroy the conversation-window since it is no longer
**                      needed.
*/
                        WinDestroyWindow (hwndConv);
                     }
                     else
                     {
/*
**                      At this point, the conversation-window is "Enabled"
**                      indicating a conversation link was established.  Add
**                      DDE Client behavior to the conversation-window by
**                      sub-classing it with the DDE Client window procedure...
*/
                        pfnDdeConvWindowProc = WinSubclassWindow
                                             ( hwndConv
                                             , DdeClientConvWindowProc
                                             );
                     }
                  }
               }
            }
            else
            {
              usCmd = IDM_LINKS_ENUMERATE;
           }
        }
     }
   }
/*
** If the user has selected the "Enumerate" menu-item...
*/
   else if (usCmd == IDM_LINKS_ENUMERATE)
   {
/*
**    Clear all menu-items not currently 'Checked'...
*/
      fResult  = MnuPurgeItems
               ( hwndMenuServers
               , MIA_CHECKED
               );
/*
**    Query the current count (of active) conversations...
*/
      cItems   = MnuQueryItemCount
               ( hwndMenuServers
               );
/*
**    If no conversations currently active...
*/
      if (cItems == 0)
      {
/*
**       Disable "Servers" menu-item...
*/
         fResult  = WinEnableMenuItem
                  ( hwndMenu        /* menu window handle                     */
                  , IDM_SERVERS     /* menu item identifier                   */
                  , FALSE           /* disabled                               */
                  );
      }
/*
**    Reset the current state of this (the WC_Client class) window to disabled.
**    The enabled state of this window is used to determine the success or
**    failure of a DDE Initiate request...
*/
      fResult  = WinEnableWindow    /* sets enabled state of...               */
               ( hwnd               /*    this window to...                   */
               , FALSE              /*    disabled.                           */
               );
/*
**    Issue the DDE Initiate Request...
*/
      fResult  = WinDdeInitiate
               ( hwnd               /* originating window                     */
               , pszAppName         /* initialized to a zero-length string    */
               , pszTopic           /* initialized to a zero-length string    */
               , &ConvContext       /* initialized at application start-up    */
               );
/*
**    Check the state of the conversation-window after issuing the request.
**    If the window is "Enabled", then at least one server responded, otherwise
**    no servers responded.  If no server responded, notify the user...
*/
      if (WinIsWindowEnabled (hwnd) == FALSE)
      {
         usResponse  = FrmSetStatusText
                     ( hab
                     , hwndStatus
                     , NULL
                     , IDS_NO_SERVER_RESPONDED
                     , WA_NOTE
                     );
      }
   }
/*
** If the user has selected the "Paste Special..." menu-item...
*/
   else if (usCmd == IDM_LINKS_PASTE_LINK)
   {
      PSZ      pszAppName;
      PSZ      pszTopic;
      PSZ      pszItem;
/*
**    Open the 'clipboard'...
*/
      fResult     = WinOpenClipbrd
                  ( hab
                  );
/*
**    Obtain a pointer to shared memory containing the "Link" format...
*/
      pszAppName  = (PSZ) WinQueryClipbrdData
                  ( hab
                  , usFmtLink
                  );
/*
**    The "Link" format is defined as 'Application Name', "Topic Name" and
**    "Item Name".  Therefore, the beginning of the buffer must point to an
**    Application Name.  No check is made for a NULL pointer since this menu
**    item could not have been selected unless it was enabled, and it can not
**    be enabled unless a "Link" format existed in the clipboard (See
**    WM_INITMENU logic in 'FrameWindowProc').  Skip past Application name...
*/
      for ( pszTopic = pszAppName   /* set to beginning of buffer             */
          ; *pszTopic               /* while not a null (0x00) character,     */
          ; pszTopic++              /* skip character.                        */
          );
/*
**    Here again, a check must be made if the Application/Topic pair is
**    currently engaged in a conversation link, since this program can only
**    handle one conversation link per Application/Topic pair.
*/
      fResult  = IsLinkActive
               ( pszAppName
               , ++pszTopic
               );
/*
**    The application/topic name pair is not engaged in a DDE conversation, so
**    create a 'Conversation-window' to support the link.
*/
      if (fResult == FALSE)
      {
         hwndConv = DdeCreateConvWindow
                  ( hwnd
                  , NULLHANDLE
                  , pszAppName
                  , pszTopic
                  , &ConvContext
                  );
/*
**       If conversation-window successfully created...
*/
         if (hwndConv)
         {
/*
**          Issue the DDE Initiation request from this newly created window...
*/
            fResult  = WinDdeInitiate
                     ( hwndConv
                     , pszAppName
                     , pszTopic
                     , &ConvContext
                     );
/*
**          Check the state of the conversation-window after issuing the
**          request.  If the window is "Enabled", then at least one server
**          responded, otherwise no servers responded.
*/
            fResult  = WinIsWindowEnabled
                     ( hwndConv
                     );
/*
**          If no server responded, notify the user...
*/
            if (fResult == FALSE)
            {
               usResponse  = FrmSetStatusText
                           ( hab
                           , hwndStatus
                           , NULL
                           , IDS_NO_SERVER_RESPONDED
                           , WA_NOTE
                           );
/*
**            Destroy the conversation-window since it is no longer needed.
*/
              WinDestroyWindow (hwndConv);
            }
            else
            {
/*
**             At this point, the conversation-window is "Enabled" indicating
**             a conversation link was established.  Add DDE Client behavior
**             to the conversation-window by sub-classing it with the
**             DDE Client window procedure...
*/
               pfnDdeConvWindowProc = WinSubclassWindow
                                    ( hwndConv
                                    , DdeClientConvWindowProc
                                    );
            }
         }
      }
/*
**    At this point, either the Application/Topic name pair identified in the
**    "Link" format was currently engaged in a conversation link, or a
**    conversation link was successfully established...
*/
      if (fResult == TRUE)
      {
/*
**       Obtain the menu-identifier associated with the Application Name...
*/
         sAppSelected   = MnuLookUpItemText
                        ( hwndMenuServers
                        , pszAppName
                        , FALSE
                        );
/*
**       Obtain the menu-identifier associated with the Application Name...
*/
         sTopicSelected = MnuLookUpItemText
                        ( MnuQuerySubMenu (hwndMenuServers, sAppSelected)
                        , pszTopic
                        , FALSE
                        );
/*
**       Obtain the handle the WC_DdeConv class window corresponding to the
**       Application/Topic name pair...
*/
         hwndConv = DdeQueryConvWindow
                  ( hwnd
                  , pszAppName
                  , pszTopic
                  );
/*
**       Establish a pointer to the beginning of the Item name-string specified
**       in the 'Link' format...
*/
         for ( pszItem = pszTopic   /* start at the beginning of the Topic,   */
             ; *pszItem             /* looking for a null (0x00) character,   */
             ; pszItem++            /* skipping all the rest.                 */
             );
/*
**       Update the conversation link window with the Item name-string.  This
**       information is used to display and retain the last item requested by
**       the user...
*/
         fResult  = (BOOL) WinSendMsg
                  ( hwndConv
                  , UM_SetItemText
                  , MPFROMP (++pszItem)
                  , MPVOID
                  );
/*
**       Now, display the "Item..." dialog allowing the user to specify or
**       modify DDE transaction parameters...
*/
         fResult  = WinPostMsg
                  ( hwndConv
                  , UM_ShowItemDialog
                  , MPFROMHWND (hwndMenuServers)
                  , MPFROM2SHORT (sTopicSelected, sAppSelected)
                  );
      }
/*
**    Close the 'clipboard'...
*/
      fResult  = WinCloseClipbrd
               ( hab
               );
   }
/*
** If the user has selected a "Codepages" menu-item...
**
** Note: This menu item is disabled once a DDE conversation link has been
**       established.
*/
   else if (usCmd >= IDM_LINKS_CODEPAGES_FIRST
         && usCmd <= IDM_LINKS_CODEPAGES_LAST)
   {
      APIRET     ApiRet;
/*
**    Remove the check attribute from the previously selected code page item...
*/
      fResult  = WinCheckMenuItem
               ( hwndMenu
               , IDM_LINKS_CODEPAGES_FIRST + usCpCurrent
               , FALSE
               );
/*
**    Calculate new code page entry by subtracting the 'base' code page entry
**    value from the selected value.  The difference is used as a zero-based
**    offset into the array of code pages currently active in the system.  This
**    array was obtained during application start-up.
*/
      usCpCurrent = usCmd - IDM_LINKS_CODEPAGES_FIRST;
/*
**    Set the code page of the current process to the selected code page...
*/
      ApiRet   = DosSetProcessCp
               ( aulCpList [usCpCurrent]
               );
/*
**    If successfully set, update the Conversation Context structure of this
**    application and 'Check' the corresponding code page menu item...
*/
      if (ApiRet == NO_ERROR)
      {
/*
**       Update the Conversation Context structure of this application...
*/
         ConvContext.usCodepage = aulCpList [usCpCurrent];
/*
**       Set the menu item attribute corresponding to the newly selected
**       code page to 'Checked'...
*/
         fResult  = WinCheckMenuItem
                  ( hwndMenu
                  , IDM_LINKS_CODEPAGES_FIRST + usCpCurrent
                  , TRUE
                  );
      }
   }
/*
** If the user has selected a "Terminate ~All" menu-item, terminate all
** conversation links this application is currently engaged in...
*/
   else if (usCmd == IDM_LINKS_TERMINATE_ALL)
   {
      fResult  = WinBroadCastMsg
               ( hwnd
               , WM_CLOSE
               , MPVOID
               , MPVOID
               , BMSG_SEND
               );
   }
   else if (usCmd == IDM_DISPLAY_IMAGE)
   {
      fResult  = WinCheckMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_IMAGE
               , TRUE
               );
      fResult  = WinCheckMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_BINARY
               , FALSE
               );
      fResult  = WinEnableMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_IMAGE
               , FALSE
               );
      fResult  = WinEnableMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_BINARY
               , TRUE
               );
      fResult  = WinInvalidateRect
               ( hwnd
               , NULL
               , FALSE
               );
   }
   else if (usCmd == IDM_DISPLAY_BINARY)
   {
      fResult  = WinCheckMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_BINARY
               , TRUE
               );
      fResult  = WinCheckMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_IMAGE
               , FALSE
               );
      fResult   = WinEnableMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_BINARY
               , FALSE
               );
      fResult  = WinEnableMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_IMAGE
               , TRUE
               );
      fResult  = WinInvalidateRect
               ( hwnd
               , NULL
               , FALSE
               );
   }
/*
** If the user has selected the 'Clear' menu item, reset, purge and repaint
** all display windows...
*/
   else if (usCmd == IDM_DISPLAY_CLEAR)
   {
      fResult  = FrmResetDialogItems ();
      fResult  = FrmResetScrollBar
               ( hwndHorzScroll
               );
      fResult  = FrmResetScrollBar
               ( hwndVertScroll
               );
      fResult  = WinSetWindowText
               ( hwndStatus
               , pszNull
               );
      fResult  = FrmPurgeListboxItems
               ( hwndListBox
               );
      fResult  = WinSetWindowULong
               ( hwnd
               , QWL_USER
               , 0L
               );
      fResult  = WinInvalidateRect
               ( hwnd
               , NULL
               , FALSE
               );
   }
   else if (usCmd == IDM_DISPLAY_AUTO)
   {
      fChecked = WinIsMenuItemChecked
               ( hwndMenuDisplay
               , IDM_DISPLAY_AUTO
               );
      fResult  = WinCheckMenuItem
               ( hwndMenuDisplay
               , IDM_DISPLAY_AUTO
               , (fChecked)
               ? FALSE
               : TRUE
               );
   }
   else if (usCmd == IDM_HELP_ABOUT)
   {
      ulReply  = WinDlgBox
               ( HWND_DESKTOP
               , hwnd
               , (PFNWP) DlgAboutWindowProc
               , 0
               , IDD_ABOUT
               , NULL
               );
   }
   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   WmDestroy
**
** Usage:      Processes a "WM_DESTROY" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_DESTROY message is sent to a window after it has been
**             hidden offering it an opportunity to perform termination actions.
**
**============================================================================*/
   MRESULT                          /* flReply: Reserved value, zero (0)      */
   APIENTRY    WmDestroy
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
/*
** Query pointer to currently selected DDESTRUCT structure...
*/
   PDDESTRUCT  pDdeStruct  = (PDDESTRUCT) WinQueryWindowULong
                           ( hwnd
                           , QWL_USER);
/*
** Free the memory object...
*/
   if (pDdeStruct)
   {
      DosFreeMem (pDdeStruct);
   }

   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   WmMenuSelect
**
** Usage:      Processes a "WM_MENUSELECT" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_MENUSELECT message is sent to the owner of a menu when one
**             of its items has been selected. This behavior is utilized to
**             maintain which menu-items the user has most recently selected.
**             This information is used later when determining application and
**             topic names.
**============================================================================*/
   MRESULT
   APIENTRY    WmMenuSelect
(
   HWND        hwnd
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   HWND        hwndMenu     = HWNDFROMMP   (mp2);
   SHORT       sMenuItemId  = SHORT1FROMMP (mp1);
   SHORT       sPostCommand = SHORT2FROMMP (mp1);
/*
** Check if the selected menu-item falls into the application and topic name
** range.
*/
   if (sMenuItemId >= IDM_SERVERS_FIRST
   &&  sMenuItemId <= IDM_SERVERS_LAST + MAX_TOPICS)
   {
      USHORT   cbItemText;
/*
**    Server (application) names are added to menu controls in multiples of
**    IDM_INCREMENT. Topic names are added in a consecutive and sequential
**    sequence beginning with the base value of the application menu-item
**    and incrementing by 1 until MAX_TOPICS is reached.  Subsequently,
**    application and topic menu-items can be identified by the result of a
**    modular (%) evaluation of the menu-item identifier. That is, if sItemId
**    is an even multiple of IDM_INCREMENT it is an application name,
**    otherwise it is a topic menu-item.
*/
      if (sMenuItemId % IDM_INCREMENT)
      {
         cbItemText  = MnuQueryItemText
                     ( hwndMenuServers
                     , sMenuItemId
                     , szTopic
                     );
/*
**       save selected topic identifier...
*/
         sTopicSelected = sMenuItemId;
      }
      else
      {
        cbItemText = MnuQueryItemText
                   ( hwndMenuServers
                   , sMenuItemId
                   , szAppName
                   );
/*
**       Save selected Application identifier...
*/
         sAppSelected = sMenuItemId;
      }
   }
/*
** We return the same value the menu control has notified us with as to the
** subsequent behavior.  That is, we do not alter the expected behavior.
*/
   return MRFROMSHORT (sPostCommand);
}
/*==============================================================================
** Function:   WmDdeInitiateAck
**
** Usage:      Processes a "WM_DDE_INITIATEACK" message dispatched to a
**             "WC_Client" class window.
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
**             returned in response to this message.
**
**  This message is always sent.
**============================================================================*/
   MRESULT                          /* fResult: Error indicator               */
   APIENTRY    WmDdeInitiateAck
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle of requesting application*/
,  MPARAM      mp2                  /* pointer to a DDEINIT structure         */
)
{
   BOOL         fResult      = TRUE;
/*
** Obtain and set DDE transaction information...
*/
   HWND         hwndServer   = HWNDFROMMP (mp1);
   PDDEINIT     pDdeInit     = (PDDEINIT) PVOIDFROMMP (mp2);
   PCONVCONTEXT pConvContext = (pDdeInit) ? DDEI_PCONVCONTEXT (pDdeInit) : NULL;
   PSZ          pszAppName   = (pDdeInit) ? pDdeInit->pszAppName : NULL;
   PSZ          pszTopic     = (pDdeInit) ? pDdeInit->pszTopic   : NULL;
   CHAR         szCpAppName  [MAX_TEXTLENGTH];
   CHAR         szCpTopic    [MAX_TEXTLENGTH];
/*
** Obtain and set menu information...
*/
   HWND         hwndSubMenu  = (HWND) 0;
   MENUITEM     mi;
   SHORT        sItemCount   = 0;
   SHORT        sItemId      = 0;
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
                            , WM_DDE_INITIATEACK
                            , ulMsgTime
                            , (ULONG) pDdeInit
                            );
/*
** Set the 'enabled' state of this window to TRUE (enabled).  The state of
** the window indicates whether or not any DDE servers responded.
*/
   fResult  = WinEnableWindow       /* sets enabled state of...               */
            ( hwnd                  /*    this window to...                   */
            , TRUE                  /*    enabled.                            */
            );
/*
** If a conversation context was included in the response and the code pages
** do not match, translate the Application and Topic name-strings...
*/
   if (pDdeInit->offConvContext
   &&  pConvContext->usCodepage != ConvContext.usCodepage)
   {
/*
**    Translate Application name...
*/
      fResult  = WinCpTranslateString
               ( hab
               , pConvContext->usCodepage
               , pDdeInit->pszAppName
               , ConvContext.usCodepage
               , MAX_TEXTLENGTH
               , szCpAppName
               );
/*
**    If translation successful, set pointer to Application name to the
**    translated version...
*/
      if (fResult)
      {
         pszAppName = szCpAppName;
      }
/*
**    Translate Topic...
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
**    If translation successful, set pointer to Topic name to the translated
**    version...
*/
      if (fResult)
      {
         pszTopic = szCpTopic;
      }
   }
/*
** Obtain identifier of menu-item corresponding to the responding
** application...
*/
   sItemId  = MnuLookUpItemText
            ( hwndMenuServers
            , pszAppName
            , FALSE
            );
/*
** Obtain count of current number of entries...
*/
   sItemCount  = MnuQueryItemCount
               ( hwndMenuServers
               );
/*
** If sItemId is equal to MIT_ERROR, then no entry matched the application
** name; so, add an entry now.
*/
   if (sItemId == MIT_ERROR)
   {
      MENUTEMPLATE mt;
/*
**    Calculate new menu-item identifier...
*/
      sItemId  = ((SHORT)IDM_SERVERS_FIRST)
               + (sItemCount * (SHORT)IDM_INCREMENT);
/*
**    If new identifier does not exceed the maximum number of allowed entries,
**    add the new entry...
*/
      if (sItemId <= (USHORT) IDM_SERVERS_LAST)
      {
/*
**       Initialize menu-template...
*/
         mt.cbTemplate  = 0;
         mt.usVersion   = 0;
         mt.usCodepage  = 0;
         mt.cMenuItems  = 0;
         mt.offami      = sizeof (MENUTEMPLATE) - 1;
/*
**       Menu entries corresponding to (Server) Applications, will contain
**       sub-menus (ie Topics) this requires the creation of a "WC_MENU" class
**       window.
*/
         hwndSubMenu    = WinCreateWindow
                        ( HWND_OBJECT
                        , WC_MENU
                        , NULL
                        , WS_CLIPSIBLINGS | WS_SAVEBITS
                        , 0,0,0,0
                        , hwndMenuServers
                        , HWND_BOTTOM
                        , sItemId
                        , (PCH)&mt
                        , 0);
/*
**       Initialize menu-item structure...
*/
         mi.iPosition   = MIT_END;
         mi.afStyle     = MIS_TEXT | MIS_SUBMENU;
         mi.afAttribute = 0;
         mi.id          = sItemId;
         mi.hwndSubMenu = hwndSubMenu;
         mi.hItem       = 0L;
/*
**       Add entry to base "Servers" menu...
*/
         sItemId  = MnuAddItem
                  ( hwndMenuServers
                  , &mi
                  , pszAppName
                  );
/*
**       Enable base "Servers" menu-item allowing further selection by the
**       user...
*/
         fResult  = WinEnableMenuItem
                  ( hwndMenu
                  , IDM_SERVERS
                  , TRUE
                  );
      }
   }
   else
   {
/*
** An entry was found matching the Application name.  Obtain the menu
** information associated with the entry...
*/
      fResult = (BOOL) WinSendMsg
              ( hwndMenuServers
              , MM_QUERYITEM
              , MPFROM2SHORT (sItemId, FALSE)
              , MPFROMP (&mi)
              );

      if (!fResult)
      {
         sItemId = MIT_ERROR;
      }
   }
/*
** If we have valid menu-item identifier...
*/
   if (sItemId != MIT_ERROR)
   {
      sItemCount  = MnuQueryItemCount
                  ( mi.hwndSubMenu
                  );
/*
**    Check if another Topic can be added...
*/
      if (sItemCount < MAX_TOPICS)
      {
/*
**       Obtain the window handle of our parent...
*/
         hwndSubMenu = mi.hwndSubMenu;
/*
**       Check if the topic already exists...
*/
         sItemId  = MnuLookUpItemText
                  ( hwndSubMenu
                  , pszTopic
                  , FALSE
                  );
/*
**       If it doesn't, add it...
*/
         if (sItemId == MIT_ERROR)
         {
/*
**          Initialize a MENUITEM structure...
*/
            mi.iPosition   = MIT_END;
            mi.afStyle     = MIS_TEXT;
            mi.afAttribute = 0;
            mi.id         += (sItemCount + 1);
            mi.hwndSubMenu = (HWND) 0;
            mi.hItem       = 0L;
/*
**          Insert item into menu...
*/
            MnuAddItem ( hwndSubMenu
                       , &mi
                       , pszTopic
                       );
         }
      }
   }
/*
** It is the responsibility of the receiving window procedure to free the
** memory object.
**
** DosFreeMem (pDdeInit);
*/
/*
** Terminate the unwanted conversation link.  In addition, we also return
** FALSE in response to the WM_DDE_INITIATEACK message. This is a
** pre-notification that the link is unwanted.  The value returned from
** processing this message, is the value returned by PM to the caller of
** the WinDdeRespond API.  Upon receiving a FALSE return code, a server
** application typically marks the conversation link as being incomplete
** due to an error but does not destroy the associated resources (due to
** pending WM_DDE_TERMINATE message).
*/
   fResult  = WinDdePostMsg
            ( hwndServer
            , hwnd
            , WM_DDE_TERMINATE
            , NULL
            , DDEPM_RETRY
            );

   return MRFROMSHORT (FALSE);
}
/*==============================================================================
** Function:   RegisterWindowClasses
**
** Usage:      Register all window-classes required by this application.
**============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    RegisterWindowClasses
(
   HAB         hab                  /* handle anchor-block of this appl.      */
)
{
   BOOL        fResult;             /* window-class-registration indicator    */
/*
** Register the "WC_Client" class.  This class handles the base functionality
** of an application...
*/
   fResult  = WinRegisterClass      /* register a window class                */
            ( hab                   /* handle anchor-block                    */
            , WC_Client             /* class name-string                      */
            , CliWindowProc         /* window Procedure                       */
            , CS_SIZEREDRAW         /* repaint window if size changes         */
            | CS_CLIPSIBLINGS       /* exclude siblings from drawing area     */
            , 3 * sizeof (PVOID)    /* window instance data                   */
            );
/*
** If class could not be registered...
*/
   if (fResult == FALSE)
   {
      return (FALSE);               /* exit this function...                  */
   }
/*
** Register the "WC_DdeConv" class.  This class handles DDE conversations...
*/
   fResult  = WinRegisterClass
            ( hab                   /* handle anchor-block                    */
            , WC_DdeConv            /* class name-string                      */
            , DdeConvWindowProc     /* window Procedure                       */
            , 0L                    /* class style (default)                  */
            , sizeof (PVOID)        /* window instance data                   */
            );
/*
** If class could not be registered...
*/
   if (fResult == FALSE)
   {
      return (FALSE);               /* exit this function...                  */
   }

   return (TRUE);                   /* this function successfully completed   */
}
/*==============================================================================
** Function:   IsLinkAcktive
**
** Usage:      Determines if the Application/Topic name pair is currently
**             engaged in a DDE conversation.
**
** Remarks:    Application/Topic name pairs that are currently engaged in a DDE
**             conversation link are indicated by state of the corresponding
**             menu-item.  If the menu-item is checked, the item is engaged,
**             otherwise it is not.
**============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    IsLinkActive
(
   PSZ         pszAppName           /* address of application name-string     */
,  PSZ         pszTopic             /* address of topic name-string           */
)
{
   SHORT       sItemId;
/*
** First, search the menu for a match on the Application name...
*/
   sItemId  = MnuLookUpItemText
            ( hwndMenuServers
            , pszAppName
            , FALSE
            );
/*
** Second, check if the Application name was found...
*/
   if (sItemId != MIT_ERROR)
   {
      BOOL     fResult;
      MENUITEM mi;
      USHORT   usItemState;
/*
**    Third, query the menu-item information to obtain the sub-menu
**    window handle.  This window handle is used to search for topic name.
*/
      fResult  = (USHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEM
               , MPFROM2SHORT (sItemId, TRUE)
               , MPFROMP (&mi)
               );
/*
**    If the Item exists...
*/
      if (fResult)
      {
/*
**       If a non-null pointer to a topic name-string was given...
*/
         if (pszTopic)
         {
            sItemId  = MnuLookUpItemText
                     ( mi.hwndSubMenu
                     , pszTopic
                     , FALSE
                     );
/*
**          Fourth, if a Topic item was found...
*/
            if (sItemId != MIT_ERROR)
            {
/*
**             Fifth, query the state of the Topic menu-item.
*/
               usItemState = (USHORT) WinSendMsg
                           ( mi.hwndSubMenu
                           , MM_QUERYITEMATTR
                           , MPFROM2SHORT (sItemId, TRUE)
                           , MPFROMSHORT  (MIA_CHECKED)
                           );
/*
**             Finally, if the state of this menu-item is checked, then
**             it is currently engaged in a DDE conversation.
*/
               if (usItemState)
               {
                  return (TRUE);
               }
            }
         }
/*
**       otherwise, a null pointer to a topic name-string was given.  This
**       specifies that this function should return TRUE if any
**       Application/Topic name pairs are engaged in a DDE Conversation.
*/
         else
         {
            USHORT   sItemIndex   = 0;
            USHORT   usItemId     = 0;
            USHORT   usState      = 0;
/*
**          Loop through each menu-item...
*/
            do
            {
               usItemId = (USHORT) WinSendMsg
                        ( mi.hwndSubMenu
                        , MM_ITEMIDFROMPOSITION
                        , MPFROMSHORT (sItemIndex)
                        , MPVOID
                        );
/*
**             If an item was found, see if has been 'Checked' (indicating that
**             it is currently engaged in a conversation.
*/
               if (usItemId != (USHORT) MIT_ERROR)
               {
                  usState  = (USHORT) WinSendMsg
                           ( mi.hwndSubMenu
                           , MM_QUERYITEMATTR
                           , MPFROM2SHORT (usItemId, TRUE)
                           , MPFROMSHORT  (MIA_CHECKED)
                           );
/*
**                If it is checked, return a successful indicator...
*/
                  if (usState)
                  {
                     return (TRUE);
                  }
/*
**                Skip to next menu-item...
*/
                  sItemIndex++;
               }
            } while (usItemId != (USHORT) MIT_ERROR);
         }
      }
   }

   return (FALSE);
}
/*==============================================================================
** Function:   UmTermiante
**
** Usage:      Processes a UM_Terminate message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A UM_Terminate message is sent to a WC_Client class window when
**             the user as requested that the specified conversation link is
**             to be terminated.
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    UmTerminate
(
   HWND        hwnd
,  MPARAM      mp1                  /* pointer to Application name-string     */
,  MPARAM      mp2                  /* pointer to Topic name-string           */
)
{
   BOOL        fResult;
   PSZ         pszAppName   = (PSZ) PVOIDFROMMP (mp1);
   PSZ         pszTopic     = (PSZ) PVOIDFROMMP (mp2);
   SHORT       sItemIdAppName;
   SHORT       sItemIdTopic;
/*
** Obtain menu-item identifier associated with Application name-string...
*/
   sItemIdAppName = MnuLookUpItemText
                  ( hwndMenuServers
                  , pszAppName
                  , FALSE
                  );
/*
** If found...
*/
   if (sItemIdAppName != MIT_ERROR)
   {
      MENUITEM mi;
/*
**    Obtain menu-item information associated with Application name-string...
*/
      fResult  = (USHORT) WinSendMsg
               ( hwndMenuServers
               , MM_QUERYITEM
               , MPFROM2SHORT (sItemIdAppName, TRUE)
               , MPFROMP (&mi)
               );
/*
**    If found menu-item information obtained, obtain menu-item identifier
**    associated with topic name-string...
*/
      if (fResult)
      {
         sItemIdTopic   = MnuLookUpItemText
                        ( mi.hwndSubMenu
                        , pszTopic
                        , FALSE
                        );
/*
**       If found, remove the 'Check' attribute from the Topic entry...
*/
         if (sItemIdTopic != MIT_ERROR)
         {
            fResult  = WinCheckMenuItem
                     ( hwndMenuServers
                     , sItemIdTopic
                     , FALSE
                     );
/*
**          Query if this application has any remaining conversation links.  If
**          it does not, then remove the 'Check' attribute from the Application
**          name entry.  This will allow the entire menu to be purged later...
*/
            if (IsLinkActive (pszAppName, NULL) == FALSE)
            {
               fResult  = WinCheckMenuItem
                        ( hwndMenuServers
                        , sItemIdAppName
                        , FALSE
                        );
            }
         }
      }
   }
/*
** Check if conversation links still exist in the application...
*/
   fResult  = MnuQueryItemsAttr
            ( hwndMenuServers
            , MIA_CHECKED
            );
/*
** Check if conversation links still exist in the application...
*/
   if (fResult == FALSE)
   {
/*
**    If no further conversation links exist, reenable "Codepages" and
**    "Terminate All" menu-items...
*/
      fResult  = WinEnableMenuItem
               ( hwndMenu
               , IDM_LINKS_CODEPAGES
               , TRUE
               );
      fResult  = WinEnableMenuItem
               ( hwndMenu
               , IDM_LINKS_TERMINATE_ALL
               , FALSE
               );
   }

   return MRFROMSHORT (TRUE);
}
/*==============================================================================
** Function:   UmInitiateAck
**
** Usage:      Processes a UM_InitiateAck message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A UM_Terminate message is sent to a WC_Client class window by
**             a WC_DdeConv class window when/if it receive a WM_DDE_INITIATEACK
**             message.
**============================================================================*/
   MRESULT
   APIENTRY    UmInitiateAck
(
   HWND        hwnd
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
   HWND        hwndSubMenu;
   MENUITEM    mi;
   MENUTEMPLATE mt;
   PSZ         pszAppName   = (PSZ) PVOIDFROMMP (mp1);
   PSZ         pszTopic     = (PSZ) PVOIDFROMMP (mp2);
   SHORT       sItemId;
   SHORT       sItemCount;
   SHORT       sItemIdAppName;
   SHORT       sItemIdTopic;
/*
** Obtain menu-item information associated with Application name-string...
*/
   sItemIdAppName = MnuLookUpItemText
                  ( hwndMenuServers
                  , pszAppName
                  , FALSE
                  );
/*
** If not found...
*/
   if (sItemIdAppName == MIT_ERROR)
   {
      sItemCount  = MnuQueryItemCount
                  ( hwndMenuServers
                  );
/*
**    ...Calculate new menu-item identifier
*/
      sItemIdAppName = ((SHORT)IDM_SERVERS_FIRST)
                     + (sItemCount * (SHORT)IDM_INCREMENT);
/*
**    If there is more room to add the responding servers name in the "Servers"
**    menu...
*/
      if (sItemIdAppName <= (USHORT) IDM_SERVERS_LAST)
      {
         mt.cbTemplate  = 0;
         mt.usVersion   = 0;
         mt.usCodepage  = 0;
         mt.cMenuItems  = 0;
         mt.offami      = sizeof (MENUTEMPLATE) - 1;
/*
**       Create a WM_MENU class window to hold Topics...
*/
         hwndSubMenu = WinCreateWindow
                     ( HWND_OBJECT
                     , WC_MENU
                     , NULL
                     , WS_CLIPSIBLINGS | WS_SAVEBITS
                     , 0,0,0,0
                     , hwndMenuServers
                     , HWND_BOTTOM
                     , sItemIdAppName
                     , (PCH)&mt
                     , 0
                     );
/*
**       Initialize MENUITEM structure...
*/
         mi.iPosition   = MIT_END;
         mi.afStyle     = MIS_TEXT | MIS_SUBMENU;
         mi.afAttribute = 0;
         mi.id          = sItemIdAppName;
         mi.hwndSubMenu = hwndSubMenu;
         mi.hItem       = 0L;
/*
**       Add the new entry...
*/
         sItemId  = MnuAddItem
                  ( hwndMenuServers
                  , &mi
                  , pszAppName
                  );
/*
**       If successfully added, enable the "Server" menu...
*/
         fResult  = WinEnableMenuItem
                  ( hwndMenu
                  , IDM_SERVERS
                  , TRUE
                  );
      }
   }
/*
** otherwise, the Application name-string already exists...
*/
   else
   {
/*
**    Obtain it corresponding sub-menu window handle...
*/
      hwndSubMenu = MnuQuerySubMenu
                  ( hwndMenuServers
                  , sItemIdAppName
                  );
   }
/*
** Check if the Topic already exists...
*/
   sItemIdTopic   = MnuLookUpItemText
                  ( hwndSubMenu
                  , pszTopic
                  , FALSE
                  );
/*
** If it does not...
*/
   if (sItemIdTopic == MIT_ERROR)
   {
/*
**    Obtain the current count of items associated with the Application...
*/
      sItemCount  = MnuQueryItemCount
                  ( hwndSubMenu
                  );
/*
**    If more room to add another entry...
*/
      if (sItemCount < MAX_TOPICS)
      {
         if (sItemIdTopic == MIT_ERROR)
         {
            mi.iPosition   = MIT_END;
            mi.afStyle     = MIS_TEXT;
            mi.afAttribute = 0;
            mi.id         += (sItemCount + 1);
            mi.hwndSubMenu = (HWND) 0;
            mi.hItem       = 0L;
/*
**          Add the topic name-string...
*/
            sItemId = MnuAddItem
                    ( hwndSubMenu
                    , &mi
                    , pszTopic
                    );
/*
**          Obtain the corresponding menu-item identifier...
*/
            sItemIdTopic = (SHORT) WinSendMsg
                         ( hwndSubMenu
                         , MM_ITEMIDFROMPOSITION
                         , MPFROMSHORT (sItemId)
                         , MPVOID
                         );
         }
      }
   }
/*
** Update corresponding menu-item entries to reflect new state of this
** application...
*/
   fResult  = WinCheckMenuItem
            ( hwndMenuServers
            , sItemIdAppName
            , TRUE
            );
   fResult  = WinCheckMenuItem
            ( hwndSubMenu
            , sItemIdTopic
            , TRUE
            );
/*
** Once engaged in a DDE conversation, the user can no longer change the
** active code page...
*/
   fResult  = WinEnableMenuItem
            ( hwndMenu
            , IDM_LINKS_CODEPAGES
            , FALSE
            );
/*
** Once engaged in a DDE conversation, allow the user to terminate all links...
*/
   fResult  = WinEnableMenuItem
            ( hwndMenu
            , IDM_LINKS_TERMINATE_ALL
            , TRUE
            );

   return MRFROMSHORT (TRUE);
}
