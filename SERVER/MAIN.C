/*==============================================================================
**
** Module:     Main.c
**
** Remarks:    DDE Server main module.
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
   #include <memory.h>
/*
** Application specific include files.
*/
   #include <main.h>
   #include <menu.h>
   #include <draw.h>
   #include <dde.h>
   #include <resource.h>
   #include <dialogs.h>
   #include <common.h>
   #include <frame.h>
   #include <client.h>
/*
** Private data items and structures.
*/
USHORT        cLinks            = 0;

HWND          hwndTopicClock    = (HWND) 0;
HWND          hwndTopicSystem   = (HWND) 0;
ULONG         ulMainTimerId     = 0L;
ULONG         ulTimeCurrent     = 0L;
ULONG         ulTimeElapsed     = 0L;
ULONG         ulTimeId          = 0L;
ULONG         ulTimeInterval    = 5000L;

ULONG         ulStatusId;
CHAR          szText            [MAX_TEXTLENGTH];
CHAR          szSysItemStatus   [MAX_TEXTLENGTH] = "Ready";
CHAR          szSysItemRtnMsg   [MAX_TEXTLENGTH] = "Return Message";
CHAR          szSysItemRestart  [MAX_TEXTLENGTH] = "DdeSrvr.exe";

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
** Query current country and code page information of server application...
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
** Setup the Server's default conversation context structure.  These values
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
   ||  !CreateWindows (hab)
   ||  !CreateTopicClock ()
   ||  !CreateTopicSystem ())
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
** Enable the client window.  The enabled state of the client window indicates
** the initialization state of this server application.  That is, this
** application will not respond to any DDE message until this application has
** completed its initialization process and enabled the client window.
*/
  fResult = WinEnableWindow         /* sets enabled state of...               */
          ( hwndClient              /*    this window to...                   */
          , TRUE                    /*    enabled. (Initialization done)      */
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
               ( hab                /* handle anchor-block,                   */
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
   RECTL       rclDialog;
   RECTL       rclStatus;
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
   hwndMenuStatus    = MnuQuerySubMenu
                     ( hwndMenu
                     , (USHORT) IDM_STATUS
                     );
   hwndMenuTopics    = MnuQuerySubMenu
                     ( hwndMenu
                     , (USHORT) IDM_TOPICS
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
   if (hwndMenu        == (HWND) 0
   ||  hwndMenuStatus  == (HWND) 0
   ||  hwndMenuTopics  == (HWND) 0
   ||  hwndMenuDisplay == (HWND) 0
   ||  hwndHorzScroll  == (HWND) 0
   ||  hwndVertScroll  == (HWND) 0)
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
               , NULL               /* Creation data - none                   */
               , NULL               /* Presentation parameters - none         */
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
   MRESULT     MResult;

   switch (ulMsgId)
   {
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
      case UM_ExecuteCommand:
      {
         MResult  = UmExecuteCommand
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case UM_RenderItem:
      {
         MResult  = UmRenderItem
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
      case WM_DDE_INITIATE:
      {
         MResult  = WmDdeInitiate
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
      case WM_TIMER:
      {
         MResult  = WmTimer
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      default:
      {
         MResult  = (*pfnOldClientWindowProc) (hwnd, ulMsgId, mp1, mp2);
         break;
      }
   }

   return (MResult);
}
/*==============================================================================
** Function:   ClientTopicClock
**
** Usage:      Creates application resources to support a topic named "Clock".
**
**============================================================================*/
   BOOL
   APIENTRY CreateTopicClock
(
   VOID
)
{
   HWND        hwndTopic   = (HWND) 0;
   ULONG       cbReturned  = 0L;
   CHAR        szClock [MAX_TEXTLENGTH] = "Clock";
   CHAR        szTime  [MAX_TEXTLENGTH] = "Time";
/*
** Get the string identifying the Clock topic
*/
   cbReturned  = WinLoadString
               ( hab
               , (HMODULE) 0
               , IDS_CLOCK
               , MAX_TEXTLENGTH
               , szClock
               );
/*
** Create a "WC_DdeTopic" class window to support the topic...
*/
   hwndTopic   = DdeCreateTopicWindow
               ( hwndClient         /* window handle of parent                */
               , hwndClient         /* window handle of owner                 */
               , hwndMenuTopics     /* window handle of menu to insert into   */
               , szClock            /* topic name and menu item text          */
               , IDM_TOPICS_FIRST   /* Identifier of first data item          */
               , IDM_TOPICS_LAST    /* Identifier of last data item           */
               , IDM_INCREMENT      /* Increment between item identifiers     */
               );
/*
** Check if window creation failed...
*/
   if (hwndTopic == (HWND) NULL)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }
/*
** Get the string identifying the Time Item name. If this api fails, we'll
** use the default string of "Time".
*/
   cbReturned  = WinLoadString
               ( hab
               , (HMODULE) 0
               , IDS_TIME
               , MAX_TEXTLENGTH
               , szTime
               );
/*
** Create a "WC_DdeItem" class window to support the item...
*/
   ulTimeId = DdeCreateItemWindow
            ( hwndTopic
            , hwndClient
            , hwndMenuTopics
            , WinGetCurrentTime (hab)
            , IS_Time
            , szTime
            );
/*
** Check if window creation failed...
*/
  if (ulTimeId == 0L)
  {
      return (FALSE);               /* if it did, return error indicator      */
  }
/*
** Start a timer corresponding to this Item identifier.  When the timer
** expires, a WM_TIMER message is posted to the window identified by
** 'hwndClient'.  Upon receiving a WM_TIMER message, the identifier specified
** in the message is used to determine which data item has changed.  Currently,
** only one timer is needed since the server application only supports one
** clock.
*/
   ulMainTimerId  = WinStartTimer
                  ( hab             /* anchor-block handle                    */
                  , hwndClient      /* handle of window to get WM_TIMER msgs  */
                  , ulTimeId        /* timer identity                         */
                  , ONE_TIMERTICK   /* delay time in milliseconds (5 sec.)    */
                  );
/*
** Check if timer creation failed...
*/
   if (ulMainTimerId == 0L)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }

   return TRUE;
}
/*==============================================================================
** Function:   ClientTopicSystem
**
** Usage:      Creates application resources to support a topic named "System".
**
**============================================================================*/
   BOOL
   APIENTRY    CreateTopicSystem
(
   VOID
)
{
   BOOL        fResult    = TRUE;
   HWND        hwndTopic  = (HWND) 0;
   HWND        hwndMenu   = (HWND) 0;
   PSZ         szString   [MAX_TEXTLENGTH];
   ULONG       ulItemId   = 0L;
   ULONG       cbReturned;
/*
** Create a "WC_DdeTopic" class window to support the topic...
*/
   hwndTopic   = DdeCreateTopicWindow
               ( hwndClient
               , hwndClient
               , hwndMenuTopics
               , SZDDESYS_TOPIC
               , IDM_TOPICS_FIRST
               , IDM_TOPICS_LAST
               , IDM_INCREMENT
               );
/*
** Check if window creation failed...
*/
   if (hwndTopic == (HWND) NULL)
   {
      return (FALSE);               /* if it did, return error indicator      */
   }
/*
** Create a "WC_DdeItem" class window to support the "Topics" item...
*/
   ulItemId = DdeCreateItemWindow
            ( hwndTopic
            , hwndClient
            , hwndMenuTopics
            , (ULONG) hwndMenuTopics
            , IS_NoFree
            | IS_Menu
            , SZDDESYS_ITEM_TOPICS
            );
/*
** Obtain the menu-item identifier associated with this topic.  Each item
** contained in this menu becomes the list of "SysItems" items.
*/
   ulItemId = WinQueryWindowUShort
            ( hwndTopic
            , QWS_ID
            );
/*
** Create a "WC_DdeItem" class window to support the "SysItems" item...
*/
   ulItemId = DdeCreateItemWindow
            ( hwndTopic
            , hwndClient
            , hwndMenuTopics
            , (ULONG) MnuQuerySubMenu (hwndMenuTopics, ulItemId)
            , IS_NoFree
            | IS_Menu
            , SZDDESYS_ITEM_SYSITEMS
            );
/*
** Load the menu resource containing a list of supported system item formats...
*/
   hwndMenu = WinLoadMenu
            ( HWND_OBJECT
            , (HMODULE) 0
            , IDR_MAIN_MENU_SYSITEMFORMATS
            );
/*
** If resource successfully created...
*/
   if (hwndMenu)
   {
/*
**    Create a "WC_DdeItem" class window to support the "ItemFormats" item...
*/
      ulItemId = DdeCreateItemWindow
               ( hwndTopic
               , hwndClient
               , hwndMenuTopics
               , (ULONG) hwndMenu
               , IS_Menu
               , SZDDESYS_ITEM_ITEMFORMATS
               );
/*
**    Check if window creation failed...
*/
      if (ulItemId == 0L)
      {
         WinDestroyWindow (hwndMenu);  /* if it did, destroy the menu resource*/
      }
   }
/*
** Load the menu resource containing a list of supported item formats...
*/
   hwndMenu = WinLoadMenu
            ( HWND_OBJECT
            , (HMODULE) 0
            , IDR_MAIN_MENU_SYSFORMATS
            );
/*
** If resource successfully created...
*/
   if (hwndMenu)
   {
/*
**    Create a "WC_DdeItem" class window to support the "Formats" item...
*/
      ulItemId = DdeCreateItemWindow
               ( hwndTopic
               , hwndClient
               , hwndMenuTopics
               , (ULONG) hwndMenu
               , IS_Menu
               , SZDDESYS_ITEM_FORMATS
               );
/*
**    Check if window creation failed...
*/
      if (ulItemId == 0L)
      {
         WinDestroyWindow (hwndMenu);  /* if it did, destroy the menu resource*/
      }
   }
/*
** Load the menu resource containing a list of supported protocols
*/
   hwndMenu = WinLoadMenu
            ( HWND_OBJECT
            , (HMODULE) 0
            , IDR_MAIN_MENU_SYSPROTOCOLS
            );
/*
** If resource successfully created...
*/
   if (hwndMenu)
   {
/*
**    Create a "WC_DdeItem" class window to support the "Protocols" item...
*/
      ulItemId = DdeCreateItemWindow
               ( hwndTopic
               , hwndClient
               , hwndMenuTopics
               , (ULONG) hwndMenu
               , IS_Menu
               , SZDDESYS_ITEM_PROTOCOLS
               );
/*
**    Check if window creation failed...
*/
      if (ulItemId == 0L)
      {
         WinDestroyWindow (hwndMenu);  /* if it did, destroy the menu resource*/
      }
   }
/*
** Create a "WC_DdeItem" class window to support the "ReturnMessage" item...
*/
   ulItemId = DdeCreateItemWindow
            ( hwndTopic
            , hwndClient
            , hwndMenuTopics
            , (ULONG) szSysItemRtnMsg
            , IS_Text
            , SZDDESYS_ITEM_RTNMSG
            );
/*
** Create a "WC_DdeItem" class window to support the "Restart" item...
*/
   ulItemId = DdeCreateItemWindow
            ( hwndTopic
            , hwndClient
            , hwndMenuTopics
            , (ULONG) szSysItemRestart
            , IS_Text
            , SZDDESYS_ITEM_RESTART
            );
/*
** Create a "WC_DdeItem" class window to support the "Status" item...
*/
   ulStatusId  = DdeCreateItemWindow
               ( hwndTopic
               , hwndClient
               , hwndMenuTopics
               , (ULONG) szSysItemStatus
               , IS_Text
               , SZDDESYS_ITEM_STATUS
               );

   return (TRUE);
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
   BOOL        fChecked;
   BOOL        fResult;
   ULONG       ulReply;
   ULONG       cbString;
   USHORT      usCmd      = SHORT1FROMMP (mp1);
   USHORT      usSource   = SHORT1FROMMP (mp2);
   USHORT      usPointer  = SHORT2FROMMP (mp2);
/*
** If the user has selected any "Topics" item...
*/
   if (usCmd  >= IDM_TOPICS_FIRST
   &&  usCmd  <= IDM_TOPICS_LAST)
   {
      fResult  = WinBroadCastMsg
               ( hwnd
               , UM_ShowItemDialog
               , MPFROMHWND (hwndMenuTopics)
               , MPFROM2SHORT (sItemSelected, sTopicSelected)
               , BMSG_POST
               );
   }
/*
** else, if the user has selected the "Status" menu-item...
*/
   else if (usCmd == IDM_STATUS_READY)
   {
     cbString = WinLoadString       /* load a string from a resource          */
              ( hab                 /* anchor-block handle of this application*/
              , (HMODULE) 0         /* load string from executable            */
              , IDS_READY           /* identifier of string to be loaded      */
              , MAX_TEXTLENGTH      /* size of buffer to receive string       */
              , szSysItemStatus     /* address of buffer to receive string    */
              );
     fResult  = WinBroadCastMsg
              ( hwnd
              , UM_AdviseItem
              , MPFROMLONG (ulStatusId)
              , MPFROMP (szSysItemStatus)
              , BMSG_POST
              );
     fResult  = WinCheckMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_READY
              , TRUE
              );
     fResult  = WinCheckMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_BUSY
              , FALSE
              );
     fResult  = WinEnableMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_READY
              , FALSE
              );
     fResult  = WinEnableMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_BUSY
              , TRUE
              );
     fResult  = (BOOL) WinBroadCastMsg
              ( hwnd
              , UM_SetState
              , MPFROM2SHORT (CONVINFO_State_Busy, SS_REMOVE)
              , MPVOID
              , BMSG_SEND
              | BMSG_DESCENDANTS
              );
  }
/*
** else, if the user has selected the "Busy" menu-item...
*/
  else if (usCmd == IDM_STATUS_BUSY)
  {
     cbString = WinLoadString       /* load a string from a resource          */
              ( hab                 /* anchor-block handle of this application*/
              , (HMODULE) 0         /* load string from executable            */
              , IDS_BUSY            /* identifier of string to be loaded      */
              , MAX_TEXTLENGTH      /* size of buffer to receive string       */
              , szSysItemStatus     /* address of buffer to receive string    */
              );
     fResult  = WinBroadCastMsg
              ( hwnd
              , UM_AdviseItem
              , MPFROMSHORT (ulStatusId)
              , MPFROMP (szSysItemStatus)
              , BMSG_POST
              );
     fResult  = WinCheckMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_BUSY
              , TRUE
              );
     fResult  = WinCheckMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_READY
              , FALSE
              );
     fResult  = WinEnableMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_BUSY
              , FALSE
              );
     fResult  = WinEnableMenuItem
              ( hwndMenuStatus
              , IDM_STATUS_READY
              , TRUE
              );
     fResult  = (BOOL) WinBroadCastMsg
              ( hwnd
              , UM_SetState
              , MPFROM2SHORT (CONVINFO_State_Busy, SS_ADD)
              , MPVOID
              , BMSG_SEND
              | BMSG_DESCENDANTS
              );
  }
/*
** else, if the user has selected the "Clear" menu-item...
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
     fResult  = WinEnableMenuItem
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
   BOOL        fResult;

   if (ulMainTimerId)
   {
      fResult  = WinStopTimer
               ( hab
               , hwnd
               , ulMainTimerId
               );
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
** Check if the selected menu-item is one of the supported topics...
*/
   if (sMenuItemId >= IDM_TOPICS_FIRST
   &&  sMenuItemId <= IDM_TOPICS_LAST + MAX_TOPICS)
   {
/*
**    Topics are added to menu controls in multiples of IDM_INCREMENT.  Item
**    names are added in a consecutive and sequential sequence beginning with
**    the base value of the Topic and incrementing by 1 until MAX_TOPICS is
**    reached.  Subsequently, Topic and item menu-items can be identified by
**    the result of a modular (%) evaluation of the menu-item identifier.
**    That is, if sItemSelected is an even multiple of IDM_INCREMENT it is a
**    Topic, otherwise, it is an Item.
*/
      if (sMenuItemId % IDM_INCREMENT)
      {
         sItemSelected  = sMenuItemId;
      }
      else
      {
         sTopicSelected = sMenuItemId;
      }
   }
/*
** We return the same value the menu control has notified us with as to the
** subsequent behavior.  That is, we do not alter the expected behavior.
*/
   return MRFROMSHORT (sPostCommand);
}
/*==============================================================================
**
** Function:   WmTimer
**
** Usage:      Processes a "WM_TIMER" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_TIMER message is posted to the owner of a Timer when its
**             time interval has elapsed.  A timer is used by the server
**             application to simulate a changing data item.  This is used to
**             supplement the implementation of (hot and warm) data-links.
**============================================================================*/
   MRESULT
   APIENTRY    WmTimer
(
   HWND        hwnd
,  MPARAM      mp1
,  MPARAM      mp2
)
{
   BOOL        fResult;
/*
** Obtain the current time of the system...
*/
   ulTimeCurrent = WinGetCurrentTime (hab);
/*
** increment the amount of time elapsed since last interval occurred.  if the
** elapsed time is equal to or greater than our time interval, post a message
** indicating the the "time' item has changed.
*/
   if ((ulTimeElapsed += ONE_TIMERTICK) >= ulTimeInterval)
   {
      fResult  = WinBroadCastMsg
               ( hwnd
               , UM_AdviseItem
               , MPFROMLONG (ulTimeId)       /* item identifier               */
               , MPFROMLONG (ulTimeCurrent)  /* new item value                */
               , BMSG_SEND
               );

      ulTimeElapsed = 0L;
   }

   return MRFROMLONG (0L);
}
/*==============================================================================
**
** Function:   WmDdeInitiate
**
** Usage:      Processes a "WM_DDE_INITIATE" message dispatched to a
**             "WC_Client" class window.
**
** Remarks:    A WM_DDE_INITIATE message is sent via the WinDdeInitiate API
**             issued by a client application.
**
** Guidelines:
**
**    1)       If the status of the server is currently busy, the do not
**             respond to this request.
**
**    2)       Determine if the request included a Conversation Context
**             structure.
**
**             a) If this structure is present, determine if string
**                comparisons are case sensitive.  String comparisons are case
**                insensitive by default.
**
**             b) If this structure is present, determine if the codepage
**                identified in this structure is different than the servers
**                codepage.  If it is, translate these strings and compare
**                them to the corresponding string or strings supported by this
**                server.
**
**    3)       If the AppName is a zero-length string, then any DDE server may
**             respond.  If the AppName is not a zero-length string, then
**             compare the string in the request to the string identifying this
**             server according to the conversation context parameters
**             described above.
**
**    4)       If the Topic is zero-length string, respond once for each topic
**             currently supported by this server.  It is possible that a topic
**             is supported at various times.  A server application is not
**             required to respond to previous requests for a topic that was
**             not previously available.
**
**    5)       For each response, create a separate and unique window to handle
**             the DDE conversation.  This ensures that a window handle pair
**             will uniquely identify the conversation.
**
**    6)       Free the DDEINIT structure.  NOTE: The example programs found in
**             this book retain all DDE messages for later replay, however, the
**             location at which this structure is freed is specified in the
**             code but is commented out.
**
**  This message is always sent.
**============================================================================*/
   MRESULT                          /* fResult: Error indicator               */
   APIENTRY    WmDdeInitiate
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* window handle of requesting application*/
,  MPARAM      mp2                  /* pointer to a DDEINIT structure         */
)
{
   ULONG       cbString       = 0L;
   BOOL        fResult        = FALSE;
   BOOL        fCaseSensitive = FALSE;
   BOOL        fStringsMatch  = TRUE;
   HENUM       henum          = (HENUM) 0;
   HWND        hwndConv       = (HWND) 0;
   HWND        hwndPartner    = HWNDFROMMP (mp1);
   HWND        hwndTopic      = (HWND) 0;
/*
** Obtain and set DDE transaction information...
*/
   PDDEINIT    pDdeInit       = (PDDEINIT) PVOIDFROMMP (mp2);
   PCONVCONTEXT pInitContext  = DDEI_PCONVCONTEXT (pDdeInit);
   PCONVCONTEXT pConvContext  = NULL;
   PSZ         pszAppName     = pDdeInit->pszAppName;
   PSZ         pszTopic       = pDdeInit->pszTopic;
   USHORT      usAppName      = strlen (pDdeInit->pszAppName);
   USHORT      usTopic        = strlen (pDdeInit->pszTopic);
   ULONG       ulCountry      = ConvContext.idCountry;
   ULONG       ulCodePage     = ConvContext.usCodepage;
   CHAR        szCpAppName [MAX_TEXTLENGTH];
   CHAR        szCpTopic   [MAX_TEXTLENGTH];
   CHAR        szTopic     [MAX_TEXTLENGTH];
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
                              , WM_DDE_INITIATE
                              , ulMsgTime
                              , (ULONG) pDdeInit
                              );
/*
** Check the enabled state of the main window...
*/
   fResult  = WinIsWindowEnabled
            ( hwnd
            );
/*
** if the main window is not enabled, the server is not yet ready to accept
** initiate requests...
*/
   if (fResult == FALSE)
   {
      return (MRFROMSHORT (0));
   }
/*
** Check if the server is busy...
*/
   fResult  = WinIsMenuItemChecked
            ( hwndMenuStatus
            , IDM_STATUS_BUSY
            );
/*
** If it is, a server application is not required to respond...
*/
   if (fResult)
   {
      return (MRFROMSHORT (0));
   }
/*
** If a Conversation Context structure is present, determine if Topic name
** and Item name-strings are to be compared case-sensitive...
*/
   if (pDdeInit->offConvContext)
   {
      fCaseSensitive = pInitContext->fsContext & DDECTXT_CASESENSITIVE;
   }
/*
** If an application name was specified, compare the requested application
** name-string against our server name-string...
*/
   if (usAppName)
   {
/*
**    Check if code page translation is required...
*/
      if (pDdeInit->offConvContext
      &&  pInitContext->usCodepage != ConvContext.usCodepage)
      {
         fResult  = WinCpTranslateString
                  ( hab
                  , pInitContext->usCodepage
                  , pszAppName
                  , ConvContext.usCodepage
                  , MAX_TEXTLENGTH
                  , szCpAppName
                  );
/*
**       If successful translation, adjust Application name-string pointer to
**       address translated string...
*/
         if (fResult)
         {
            pszAppName = szCpAppName;
         }
      }
/*
**    Compare the requested Application name-string to our server
**    name-string...
*/
      fStringsMatch  = DdeCompareStrings
                     ( (PSZ) szAppName
                     , pszAppName
                     , fCaseSensitive
                     , ConvContext.idCountry
                     , ConvContext.usCodepage
                     );
   }
/*
** If this server is not an instance of the requested application, then return
** FALSE indicating a non-fatal error occurred.
*/
   if (fStringsMatch == FALSE)
   {
/*
**    Typically, the memory object associated with the message would be freed
**    here.  However, the example program retains this information in a listbox
**    for subsequent examination by the user.  The following code statement
**    is shown here to illustrate how a typical application might function...
**
**    DosFreeMem (pDdeInit);
**
*/
      return ((MRESULT) FALSE);
   }
/*
** If a topic was specified...
*/
   if (usTopic)
   {
/*
**    If a conversation context structure was included in the request and the
**    specified codepage is different than the codepage of the this server
**    application, then translate the string to the servers codepage.
*/
      if (pDdeInit->offConvContext
      &&  pInitContext->usCodepage != ConvContext.usCodepage)
      {
         fResult  = WinCpTranslateString
                  ( hab
                  , pInitContext->usCodepage
                  , pszTopic
                  , ConvContext.usCodepage
                  , MAX_TEXTLENGTH
                  , szCpTopic
                  );
/*
**       If successful translation, adjust topic name-string pointer to
**       address translated string...
*/
         if (fResult)
         {
            pszTopic = szCpTopic;
         }
         else
         {
/*
**          Typically, the memory object associated with the message would be
**          freed here.  However, the example program retains this information
**          in a listbox for subsequent examination by the user.  The following
**          code statement is shown here to illustrate how a typical
**          application might function...
**
**          DosFreeMem (pDdeInit);
**
*/
            return ((MRESULT) FALSE);
         }
      }
   }
/*
** At this point it has been determined that the initiate request is for this
** server application and all translations (if any) have occurred.  Next, the
** scope of the response must be determined.  That is, if a topic was specified,
** does this server support that topic?  If the topic was a zero-length string,
** then this server must respond once for each supported topic.  In this
** example server application, topics are supported by the existence of a
** WC_DdeTopic class window.  This class of window is ALWAYS a child of the
** applications main window.  Therefore, a list of supported topics can be
** obtained by enumerating the applications main window as shown here...
*/
   henum = WinBeginEnumWindows (hwnd);
/*
** for each window found...
*/
   while (hwndTopic = WinGetNextWindow (henum))
   {
/*
**    The Topic name-string associated with a WC_DdeTopic class window can be
**    obtained by querying the window text of the window...
*/
      cbString = WinQueryWindowText
               ( hwndTopic
               , sizeof (szTopic)
               , szTopic
               );
/*
**    Check the length of the returned string...
*/
      if (cbString == 0)            /* this should never fail!  If it does,   */
         continue;                  /* continue to the next child window...   */
/*
**    If usTopic is greater than zero (0), then the originator of the DDE
**    initialization request specified a specific topic name.  In this case
**    we have to check for a match on topic name-strings.  Note: the topic
**    name will have already been translated above.
*/
      if (usTopic)
      {
         fStringsMatch  = DdeCompareStrings
                        ( szTopic
                        , pszTopic
                        , fCaseSensitive
                        , ConvContext.idCountry
                        , ConvContext.usCodepage
                        );
/*
**       If strings did not match...
*/
         if (fStringsMatch == FALSE)
         {
            continue;               /* continue to next child window...       */
         }
      }
/*
**    At this point it has been determined that a response is required.  So copy
**    the contents of the Conversation Context structure.  We do this so that
**    if we FREE the original DDEINIT memory object at the end of this function
**    and still retain the conversation information.  If no conversation
**    context structure was included, then set this variable to NULL...
*/
      pConvContext   = (pDdeInit->offConvContext)
                     ? DdeCopyConvContext (pInitContext)
                     : NULL;
/*
**    Create a new window to handle the conversation link...
*/
      hwndConv = DdeCreateConvWindow
               ( hwndTopic          /* parent (topic) window handle           */
               , hwndPartner        /* requesting DDE client window handle    */
               , (PSZ) szAppName    /* application name                       */
               , (PSZ) szTopic      /* topic name                             */
               , pConvContext       /* pointer to CONVCONTEXT structure       */
               );
/*
** If a conversation window was successfully created, respond to the
** requesting clients initiation request.  Otherwise, display an error
** message to the user indicating an application error.
*/
      if (hwndConv)
      {
         USHORT  fsState;
/*
**       The WinDdeRespond API creates a new DDEINIT structure and sends a
**       WM_DDE_INITIATEACK message back to the requesting application.  A
**       pointer to this newly created DDEINIT structure is passed in message
**       parameter two (mp2) of the WM_DDE_INITIATEACK message.  The value
**       returned by the WinDdeRespond API is the value returned by the client
**       application when it processed the WM_DDE_INITIATEACK message.
*/
         fResult  = (BOOL) WinDdeRespond
                  ( hwndPartner
                  , hwndConv
                  , (PSZ) szAppName
                  , (PSZ) szTopic
                  , (PCONVCONTEXT) &ConvContext
                  );
/*
**       Depending on the return value, the Conversation window is set to
**       an 'Active' state if the return value was TRUE, or to an 'Zombie'
**       state if the WinDdeRespond failed or the client application
**       indicated that an error occurred.  In either case, we set the
**       conversation window to a non-functional state.
*/
         if (fResult)
         {
/*
**          Keep a count of how many conversation links this server is
**          currently engaged in...
*/
           cLinks++;
/*
**          Enable the "Terminate All" menu-item for subsequent termination
**          of conversation links...
*/
            fResult  = WinEnableMenuItem
                     ( hwndMenu
                     , IDM_LINKS_TERMINATE_ALL
                     , TRUE
                     );
/*
**          Here we sub-class the conversation window with a new window
**          procedure.  This window procedure allows for Server specific
**          behavior of a conversation link.
*/
            pfnDdeConvWindowProc = WinSubclassWindow
                                 ( hwndConv
                                 , DdeServerConvWindowProc
                                 );
/*
**          Set the state of the conversation to "Alive".  In this state, a
**          conversation responds to all client requests.
*/
            fsState = CONVINFO_State_Alive;
         }
         else
         {
/*
**          In this case, either the client application responded with a zero
**          (0) or the WinDdeRespond API failed.  Here we set the state of
**          the conversation to that of a "Zombie" (neither dead or alive).
**          In this state, a conversation window does not respond to any
**          client requests except a WM_DDE_TERMINATE.  If should be noted
**          that in the case of the API failing, no WM_DDE_TERMINATE message
**          is forthcoming and the conversation link window will exist until
**          the server application terminates...
*/
            fsState = CONVINFO_State_Zombie;
         }
/*
**       Notify Conversation window of its new state...
*/
         fResult  = (BOOL) WinSendMsg
                  ( hwndConv
                  , UM_SetState
                  , MPFROM2SHORT (fsState, SS_REPLACE)
                  , MPVOID
                  );
      }
   }
/*
** Free system resources used for enumeration...
*/
   fResult  = WinEndEnumWindows
            ( henum
            );
/*
** A Server application is responsible for freeing the DDEINIT memory object.
** These example programs retain this information for subsequent replay.  A
** pointer to this memory object is stored in its corresponding listbox entry
** and is freed when the listbox entry is deleted.  The following API is
** listed here for completeness of the example code and should be uncommented
** if used by an application that does not need to preserve this data.
**
** DosFreeMem (pDdeInit);
*/

   return MRFROMSHORT (TRUE);
}
/*==============================================================================
** Function:   RegisterWindowClasses
**
** Usage:      Register all window-classes required by this application.
**============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    RegisterWindowClasses
(
   HAB         hab                  /* handle anchor-block of this app.       */
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
/*
** Register the "WC_DdeTopic" class.  This class handles topics supported by a
** server application...
*/
   fResult  = WinRegisterClass
            ( hab                   /* handle anchor-block                    */
            , WC_DdeTopic           /* class name-string                      */
            , DdeTopicWindowProc    /* window Procedure                       */
            , 0L                    /* class style (default)                  */
            , sizeof (PVOID)        /* no reserved storage required           */
            );
/*
** If class could not be registered...
*/
   if (fResult == FALSE)
   {
      return (FALSE);               /* exit this function...                  */
   }
/*
** Register the "WC_DdeItem" class.  This class handles items supported by a
** topic...
*/
   fResult = WinRegisterClass
            ( hab                   /* handle anchor-block                    */
            , WC_DdeItem            /* class name-string                      */
            , DdeItemWindowProc     /* window Procedure                       */
            , 0L                    /* class style (default)                  */
            , sizeof (PVOID)        /* reserve storage for a pointer          */
            );
/*
** If class could not be registered...
*/
   if (fResult == FALSE)
   {
      return (FALSE);               /* exit this function...                  */
   }
/*
** Register the "WC_DdeAdvise" class.  This class handles data-links supported
** by an item...
*/
   fResult  = WinRegisterClass
            ( hab                   /* handle anchor-block                    */
            , WC_DdeAdvise          /* class name-string                      */
            , DdeAdviseWindowProc   /* window Procedure                       */
            , 0L                    /* class style (default)                  */
            , sizeof (PVOID)        /* reserved storage for a pointer         */
            );
/*
** If class could not be registered...
*/
   if (fResult == FALSE)
   {
      return (FALSE);               /* exit this function...                  */
   }

   return (TRUE);                   /* this function successfully completed.  */
}
/*==============================================================================
** Function:   CreateTimeMetaFile
**
** Usage:      Creates metafile corresponding to the current system time (in
**             milliseconds).
**============================================================================*/
   HMF                              /* Metafile handle                        */
   APIENTRY    CreateTimeMetaFile
(
   HPS          hps                 /* handle micro presentation space        */
,  ULONG        ulTime              /* time (in milliseconds)                 */
)
{
   #define COUNT_OPTIONS (LONG) (PMF_DEFAULTS + 1)
   #define LDESC    256L

   BOOL         fResult   = FALSE;
   HMF          hmf;
   HDC          hdc;
   DEVOPENSTRUC dop;
   POINTL       ptl;
   SIZEL        sizl;
/*
** Initialize 'Device Open Structure'...
*/
   dop.pszLogAddress = NULL;        /* logical address                        */
   dop.pszDriverName = "DISPLAY";   /* driver name                            */
/*
** Open a 'Metafile' compatible Device Context...
*/
   hdc   = DevOpenDC                /* Create a Device context                */
         ( hab                      /* Handle to Anchor-block                 */
         , OD_METAFILE              /* Context used to hold metafiles         */
         , "*"                      /* No device info from init file          */
         , 2L                       /* # of items in pdopData parameter       */
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
** Initialize SIZEL structure to desired height and width of metafile...
*/
   sizl.cx   = cxWindow;
   sizl.cy   = cxDiget * 2;
/*
** Create a presentation space to record metafile into...
*/
   hps   = GpiCreatePS
         ( hab
         , hdc
         , &sizl
         , GPIA_ASSOC
         | PU_PELS
         );
/*
** Check if presentation-space was created...
*/
   if (hps == (HPS) 0)
   {
      return (NULLHANDLE);          /* if not, exit function...               */
   }
/*
** Set location to start drawing...
*/
   ptl.x = cxDiget;
   ptl.y = 0;
/*
** Draw system time in graphical form...
*/
   fResult  = Drwtime
            ( hps
            , &ptl
            , ulTime
            , cyBar
            , 4
            );
/*
** Disassociate Device Context with presentation-space...
*/
   fResult  = GpiAssociate
            ( hps
            , NULLHANDLE
            );
/*
** Close Device Context. Closure of a Device Context returns a handle to a
** Metafile...
*/
   hmf      = DevCloseDC
            ( hdc
            );
/*
** Destroy temporary presentation-space...
*/
   fResult  = GpiDestroyPS
            ( hps
            );

   return (hmf);
}
/*==============================================================================
** Function:   CreateTimeBitmap
**
** Usage:      Creates bitmap corresponding to the current system time (in
**             milliseconds).
**============================================================================*/
   HBITMAP                          /* bitmap handle                          */
   APIENTRY    CreateTimeBitmap
(
   HPS          hps                 /* handle micro presentation space        */
,  ULONG        ulTime              /* time (in milliseconds)                 */
)
{
   BITMAPINFOHEADER2 bmp;
   ERRORID      errId;
   BOOL         fResult   = FALSE;
   HBITMAP      hbmp;
   HBITMAP      hbmpOld;
   POINTL       ptl;
   LONG         alFormat [2];
/*
** Check for valid hps handle...
*/
   if (hps == (HPS) 0)
   {
      return (NULLHANDLE);
   }
/*
** Obtain a list of bitmaps supported by the current device...
*/
   fResult  = GpiQueryDeviceBitmapFormats
            ( hps
            , 2L
            , alFormat
            );
/*
** Initialize bitmap information header...
*/
   memset (&bmp, 0, sizeof (BITMAPINFOHEADER2));

   bmp.cbFix     = sizeof (BITMAPINFOHEADER2);
   bmp.cx        = cxWindow;
   bmp.cy        = cxDiget * 2;
   bmp.cPlanes   = alFormat[0];
   bmp.cBitCount = alFormat[1];
/*
** Create an empty bitmap...
*/
   hbmp     = GpiCreateBitmap
            ( hps
            , &bmp
            , 0L
            , NULL
            , NULL
            );
/*
** Associate bitmap with the presentation-space...
*/
   hbmpOld  = GpiSetBitmap
            ( hps
            , hbmp
            );
/*
** Clear presentaion-space attributes...
*/
   fResult = GpiErase
            ( hps
            );
/*
** Set location to start drawing...
*/
   ptl.x   = cxDiget;
   ptl.y   = 0;
/*
** Draw system time in graphical form...
*/
   fResult  = Drwtime
            ( hps
            , &ptl
            , ulTime
            , cyBar
            , 4
            );
/*
** Return handle to bitmap containing system time...
*/
   return (hbmp);
}
/*==============================================================================
**
** Function:   UmExecuteCommand
**
** Usage:      Processes a "UM_ExecuteCommand" message dispatched to a
**             WC_DdeClient class window.
**
** Remarks:    The UM_ExecuteCommand message is sent by a WC_DdeConv class
**             window in response to a WM_DDE_EXECUTE message.
**
**============================================================================*/
   MRESULT                          /* flResult: success indicator            */
   APIENTRY      UmExecuteCommand
(
   HWND        hwnd
,  MPARAM      mp1                  /* case sensitivity indicator             */
,  MPARAM      mp2                  /* pointer to command string buffer       */
)
{
  BOOL        fCaseSensitive = SHORT1FROMMP (mp1);
  BOOL        fResult        = FALSE;
  ULONG       ulInterval     = 0L;
/*
** Currently on one command-string (protocol) is supported.  The user can set
** rate at which automatic updates are generated for data-links.  The function
** "DdeScanCommandString" takes an address to a buffer containing the
** command-string, verifies its syntax and extracts a new time interval if
** everything is successful...
*/
   fResult  = DdeScanCommandString
            ( (PSZ) PVOIDFROMMP (mp2)        /* command-string buffer         */
            , " [ Set ( Interval , %d ) ] "  /* syntax template               */
            , fCaseSensitive                 /* case sensitivity indicator    */
            , &ulInterval                    /* new time interval             */
            );
/*
** Check if scan was successful...
*/
   if (fResult)
   {
/*
**    If new interval less than minimum supported interval, return error,
**    Otherwise, round new interval to a multiple of 1 second and save it...
*/
      if (ulInterval < 5000L)
      {
         fResult = FALSE;
      }
      else
      {
         ulTimeInterval = (ulInterval / 1000L) * 1000L;
      }
   }

   return (MRFROMSHORT (fResult));
}
/*==============================================================================
**
** Function:   UmTerminate
**
** Usage:      Processes a "UM_Terminate" message dispatched to a
**             WC_DdeClient class window.
**
** Remarks:    The UM_Terminate message is sent by a WC_DdeConv class window in
**             response to a WM_DDE_TERMINATE message.
**
**============================================================================*/
   MRESULT                          /* flResult: error indicator              */
   APIENTRY    UmTerminate
(
   HWND        hwnd
,  MPARAM      mp1                  /* application name-string                */
,  MPARAM      mp2                  /* topic name-string                      */
)
{
   BOOL        fResult;
/*
** Decrement the current number of active links...
*/
   cLinks--;
/*
** Check if any links still exist.  If not, disable the "Terminate All"
** menu-item...
*/
   if (cLinks == 0)
   {
      fResult  = WinEnableMenuItem
               ( hwndMenu
               , IDM_LINKS_TERMINATE_ALL
               , FALSE
               );
   }
/*
** Notify all descendants associated with the terminating conversation link to
** terminate any permanent data-links and destroy any associated resources
** (WC_DdeAdvise class windows).
*/
   fResult  = WinBroadCastMsg
            ( hwnd
            , UM_UnAdviseItem
            , mp1
            , MPFROMSHORT (0)
            , BMSG_SEND
            | BMSG_DESCENDANTS
            );

   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   UmRenderItem
**
** Usage:      Processes a "UM_RenderItem" message dispatched to a
**             WC_DdeClient class window.
**
** Remarks:    The UM_RenderItem message is sent by a WC_DdeItem class window
**             in response to a WM_DDE_REQUEST message or a data item
**             associated with a permanent data-link changed.
**
**============================================================================*/
   MRESULT                          /* flResult: error indicator              */
   APIENTRY    UmRenderItem
(
   HWND        hwnd
,  MPARAM      mp1                  /* item format identifier/status flags    */
,  MPARAM      mp2                  /* item handle                            */
)
{
   ULONG       cbData     = 0;
   APIRET      ApiRet     = (APIRET) 0;
   BOOL        fResult    = FALSE;
   ULONG       flResult   = 0L;
   ULONG       hItem      = LONGFROMMP (mp2);
   HMF         hmf        = (HMF) 0;
   PDDEDATA    pDdeData   = NULL;
   PVOID       pvResult   = NULL;
/*
** Obtain transaction information...
*/
   USHORT      usFormat   = SHORT1FROMMP (mp1);
   USHORT      usStyle    = SHORT2FROMMP (mp1);
/*
** Check if format requested is "Time" (a private format)...
*/
   if (usFormat == usFmtTime)
   {
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Time", then
**    hItem specifies the current system time (in milliseconds).
*/
      if (usStyle & IS_Time)
      {
         cbData   = sizeof (ULONG);
         pDdeData = DdeAllocMem
                  ( sizeof (DDEDATA) + cbData
                  );
         pDdeData->cbData = cbData;
         pDdeData->pData.ulData = hItem;
      }
   }
/*
** Check if format requested is "Metafile" format...
*/
   else if (usFormat == usFmtMetaFile)
   {
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Time", then
**    hItem specifies the current system time (in milliseconds).
*/
      if (usStyle & IS_Time)
      {
         hmf      = CreateTimeMetaFile
                  ( hpsMicro
                  , hItem
                  );
/*
**       Calculate size of buffer (in bytes) to hold rendered data item...
*/
         cbData   = GpiQueryMetaFileLength
                  ( hmf
                  );
/*
**       Allocate memory-object large enough to hold rendered data item...
*/
         ApiRet   = DosAllocMem
                  ( (PVOID) &pDdeData
                  , sizeof (DDEDATA) + cbData
                  , PAG_COMMIT
                  | PAG_WRITE
                  | PAG_READ
                  );
/*
**       Fill memory-object with rendered data item...
*/
         fResult  = GpiQueryMetaFileBits
                  ( hmf
                  , 0L
                  , cbData
                  , pDdeData->pData.abData
                  );
/*
**       Set the size of the rendered data in the DDEDATA structure...
*/
         pDdeData->cbData = cbData;
      }
   }
/*
** Check if format requested is "Bitmap"...
*/
   else if (usFormat == usFmtBitmap)
   {
      ERRORID   errId;
      BITMAPINFOHEADER2 bmp;
      PBITMAPINFO2 pbmi;
      HBITMAP   hbmp;
      HPS       hps;
      HDC       hdc;
      ULONG     cbBits;
      ULONG     cbInfo;
      ULONG     cbColorArray;
      PBYTE     pbBits;
      PBYTE     pbInfo;
      SIZEL     sizl;
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Time", then
**    hItem specifies the current system time (in milliseconds).
*/
      if (usStyle & IS_Time)
      {
         sizl.cx  = cxWindow;
         sizl.cy  = cxDiget * 2;

         hdc   = DevOpenDC
               ( hab
               , OD_MEMORY
               , "*"
               , 0L
               , NULL
               , NULLHANDLE
               );
         hps  = GpiCreatePS
               ( hab
               , hdc
               , &sizl
               , GPIA_ASSOC
               | PU_PELS
               );
         hbmp = CreateTimeBitmap
               ( hps
               , hItem
               );

         bmp.cbFix = sizeof (BITMAPINFOHEADER2);

         fResult   = GpiQueryBitmapInfoHeader
                     ( hbmp
                     , &bmp
                     );

         cbColorArray = (bmp.cBitCount == 24)
                        ? 256
                        : sizeof(RGB2) * (1 << bmp.cBitCount);

         cbInfo = sizeof (BITMAPINFO2)
                  + cbColorArray;

         if (bmp.cbImage)
         {
            cbBits = bmp.cbImage;
         }
         else
            cbBits = ((((bmp.cBitCount * bmp.cx) + 31) / 32) * 4 * bmp.cPlanes)
                   * bmp.cy;

         cbData    = cbInfo + cbBits;
         ApiRet    = DosAllocMem
                     ( (PVOID) &pDdeData
                     , sizeof (DDEDATA) + cbData
                     , PAG_COMMIT
                     | PAG_WRITE
                     | PAG_READ
                     );

         pbInfo    = (PBYTE) pDdeData->pData.abData;
         pbBits    = pbInfo + cbInfo;

         pvResult  = memcpy
                     ( pbInfo
                     , &bmp
                     , bmp.cbFix
                     );
         flResult  = GpiQueryBitmapBits
                     ( hps
                     , 0L
                     , bmp.cy
                     , pbBits
                     , (PBITMAPINFO2) pbInfo
                     );

         pDdeData->cbData = cbData;

         hbmp    = GpiSetBitmap
                  ( hps
                  , NULLHANDLE
                  );
         fResult = GpiDeleteBitmap
                  ( hbmp
                  );
         fResult = GpiDestroyPS
                  ( hps
                  );
         hmf     = DevCloseDC
                  ( hdc
                  );
      }
   }
/*
** Check if format requested is "Text" format...
*/
   else if (usFormat == usFmtText)
   {
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Text", then
**    hItem specifies a pointer to a null (0x00) terminated text string.
*/
      if (usStyle & IS_Text)
      {
         cbData   = strlen ((PSZ)hItem) + 1;
         pDdeData = DdeAllocMem
                  ( sizeof (DDEDATA) + cbData
                  );
         pDdeData->cbData = cbData;
         pvResult = memcpy
                  ((PVOID)pDdeData->pData.abData
                  , (PSZ) hItem
                  , cbData
                  );
      }
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Menu", then
**    hItem specifies a handle to a WC_MENU class window.
*/
      else if (usStyle & IS_Menu)
      {
         PCHAR  pData;
         ULONG  cbText        = 0;
         USHORT usItemId      = 0;
         USHORT usItemIdFirst = (USHORT) WinSendMsg
                              ( (HWND) hItem
                              , MM_ITEMIDFROMPOSITION
                              , MPFROMSHORT (0)
                              , MPVOID
                              );
         USHORT usIncrement   = (usItemIdFirst % IDM_INCREMENT)
                              ? 1
                              : IDM_INCREMENT;

         for ( cbData         = 0
               , usItemId       = usItemIdFirst
               ; cbText         = MnuQueryItemTextLength
                              ( (HWND) hItem
                              , usItemId
                              )
               ; cbData         += (cbText + 1)
               , usItemId++
               );

         if (cbData)
         {
            pDdeData = DdeAllocMem
                     ( sizeof (DDEDATA) + cbData + 1
                     );
            pDdeData->cbData = cbData + 1;
            pData = (PCHAR) pDdeData->pData.abData;

            for ( usItemId  = usItemIdFirst
                  ; cbText    = MnuQueryItemText
                              ( (HWND) hItem
                              , usItemId
                              , pData
                              )
                  ; pData    += cbText
                  , *pData    = TAB
                  , pData++
                  , usItemId += usIncrement
                  );

            *pData = EOT;
         }
      }
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Time", then
**    hItem specifies the current system time (in milliseconds).
*/
      else if (usStyle & IS_Time)
      {
         USHORT cbWritten   = 0;
         ULONG  ulMsgTime   = (ULONG) hItem;
         ULONG  ulHours     =   ulMsgTime / 3600000L;
         ULONG  ulMinutes   = ( ulMsgTime % 3600000L) / 60000L;
         ULONG  ulSeconds   = ((ulMsgTime % 3600000L) % 60000L) / 1000;
         ULONG  ulMS        = ((ulMsgTime % 3600000L) % 60000L) % 1000;
         CHAR   szFormat [] = "%02d:%02d:%02d:%03d";

         cbWritten = sprintf
                     ( szText
                     , szFormat
                     , ulHours
                     , ulMinutes
                     , ulSeconds
                     , ulMS
                     );
         cbData    = strlen (szText) + 1;
         pDdeData  = DdeAllocMem
                     ( sizeof (DDEDATA) + cbData
                     );

         pDdeData->cbData = cbData;

         pvResult  = memcpy
                     ((PVOID)pDdeData->pData.abData
                     , (PSZ) szText
                     , cbData
                     );
      }
   }
/*
** Check if format requested is "Code page" format...
*/
   else if (usFormat == usFmtCPText)
   {
      PCPTEXT     pCpText;
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Text", then
**    hItem specifies a pointer to a null (0x00) terminated text string.
*/
      if (usStyle & IS_Text)
      {
         cbData   = sizeof (CPTEXT)
                  + strlen ((PSZ)hItem);
         pDdeData = DdeAllocMem
                  ( sizeof (DDEDATA) - 1
                  + cbData
                  );
         pDdeData->cbData = cbData;

         pCpText = (PCPTEXT) pDdeData->pData.abData;
         pCpText->idCountry    = ConvContext.idCountry;
         pCpText->usCodepage   = ConvContext.usCodepage;
         pCpText->usLangID     = ConvContext.usLangID;
         pCpText->usSubLangID  = ConvContext.usSubLangID;

         pvResult = memcpy
                  ( (PVOID)pCpText->abText
                  , (PSZ) hItem
                  , strlen((PSZ) hItem) + 1
                  );
      }
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Menu", then
**    hItem specifies a handle to a WC_MENU class window.
*/
      else if (usStyle & IS_Menu)
      {
         PCHAR  pData;
         ULONG  cbText        = 0;
         USHORT usItemId      = 0;
         USHORT usItemIdFirst = (USHORT) WinSendMsg
                              ( (HWND) hItem
                              , MM_ITEMIDFROMPOSITION
                              , MPFROMSHORT (0)
                              , MPVOID
                              );
         USHORT usIncrement   = (usItemIdFirst % IDM_INCREMENT)
                              ? 1
                              : IDM_INCREMENT;

         for ( cbData         = 0
               , usItemId       = usItemIdFirst
               ; cbText         = MnuQueryItemTextLength
                              ( (HWND) hItem
                              , usItemId
                              )
               ; cbData         += (cbText + 1)
               , usItemId++
               );

         if (cbData)
         {
            pDdeData = DdeAllocMem
                     ( sizeof (DDEDATA)
                     + sizeof (CPTEXT)
                     + cbData
                     );
            pDdeData->cbData = cbData;

            pCpText  = (PCPTEXT) pDdeData->pData.abData;
            pCpText->idCountry   = ConvContext.idCountry;
            pCpText->usCodepage  = ConvContext.usCodepage;
            pCpText->usLangID    = ConvContext.usLangID;
            pCpText->usSubLangID = ConvContext.usSubLangID;

            for ( usItemId  = usItemIdFirst
                  , pData     = (PCHAR) pCpText->abText[1]
                  ; cbText    = MnuQueryItemText
                              ( (HWND) hItem
                              , usItemId
                              , pData
                              )
                  ; pData    += cbText
                  , *pData    = TAB
                  , pData++
                  , usItemId += usIncrement
                  );

            *pData = EOT;
         }
      }
/*
**    If the style of the requesting WC_DdeItem class window is "IS_Time", then
**    hItem specifies the current system time (in milliseconds).
*/
      else if (usStyle & IS_Time)
      {
         USHORT cbWritten   = 0;
         ULONG  ulMsgTime   = (ULONG) hItem;
         ULONG  ulHours     =   ulMsgTime / 3600000L;
         ULONG  ulMinutes   = ( ulMsgTime % 3600000L) / 60000L;
         ULONG  ulSeconds   = ((ulMsgTime % 3600000L) % 60000L) / 1000;
         ULONG  ulMS        = ((ulMsgTime % 3600000L) % 60000L) % 1000;
         CHAR   szFormat [] = "%02d:%02d:%02d:%03d";

         cbWritten   = sprintf
                     ( szText
                     , szFormat
                     , ulHours
                     , ulMinutes
                     , ulSeconds
                     , ulMS
                     );
         cbData      = sizeof (CPTEXT)
                     + cbWritten;
         pDdeData    = DdeAllocMem
                     ( sizeof (DDEDATA)
                     + cbData
                     );

         pDdeData->cbData     = cbData;
         pCpText              = (PCPTEXT) pDdeData->pData.abData;
         pCpText->idCountry   = ConvContext.idCountry;
         pCpText->usCodepage  = ConvContext.usCodepage;
         pCpText->usLangID    = ConvContext.usLangID;
         pCpText->usSubLangID = ConvContext.usSubLangID;

         pvResult = memcpy
                  ( (PVOID)pCpText->abText
                  , (PSZ) szText
                  , cbWritten + 1
                  );
      }
   }

   return (MRFROMP (pDdeData));
}
