/*==============================================================================
**
** Module:     Dde.h
**
** Remarks:    This module contains definitions used to implement DDE
**             functionality.
**
**============================================================================*/
   #ifndef DDE_INCLUDED
   #define DDE_INCLUDED
/*
** Private Window Class strings:
*/
   #define WC_DdeAdvise  "WC_DdeAdvise"
   #define WC_DdeConv    "WC_DdeConv"
   #define WC_DdeItem    "WC_DdeItem"
   #define WC_DdeTopic   "WC_DdeTopic"
/*
** Item Style (IS) flags:
*/
   #define IS_NoFree      0x0001L   /* Do not free corresponding item         */
   #define IS_Menu        0x0002L   /* Item data is stored in a menu structure*/
   #define IS_Text        0x0004L   /* Item data is stored in a text string   */
   #define IS_Time        0x0008L   /* Item data is stored as time (in MS)    */

/*
** Private structure definitions:
*/
/*
** tagCONVID
**
** This structure is used to hold the pair of window handles used to
** identify a DDE Conversation.
*/
   typedef struct tagCONVID
   {
      HWND        hwndServer;       /* handle of 'Server' window              */
      HWND        hwndClient;
   }  CONVID                        /* typedef for structure                  */
   , *PCONVID;                      /* typedef for pointer to structure       */
/*
** tagCONVINFO
**
** This structure is defines "WC_DdeConv" class information.
*/
   typedef struct tagCONVINFO
   {
      ULONG       cb;               /* Size of this structure in bytes        */
      USHORT      fsState;
  #define  CONVINFO_State_None         0x0000
  #define  CONVINFO_State_Alive        0x0001
  #define  CONVINFO_State_Zombie       0x0002
  #define  CONVINFO_State_Busy         0x0004
  #define  CONVINFO_State_Terminating  0x0008
      HWND         hwndPartner;     /* Window handle of conversation partner  */
      PCONVCONTEXT pConvContext;    /* Pointer to original CONVCONTEXT structu*/
      CHAR         szAppName [MAX_TEXTLENGTH];
      CHAR         szTopic   [MAX_TEXTLENGTH];
      CHAR         szItem    [MAX_TEXTLENGTH];
   }  CONVINFO                      /* typedef for structure                  */
   , *PCONVINFO;                    /* typedef for pointer to structure       */
/*
** tagDDEADVISE
**
** This structure is used by "WC_DdeAdvise" class windows.  This structure is
** used to store information regarding a permanent "data-link" created when
** a DDE client application issues a WM_DDE_ADVISE request.
*/
   typedef struct tagDDEADVISE      /* DdeAdvise                              */
   {
      ULONG       cb;               /* Size of structure (in bytes)           */
      USHORT      fsStatus;         /* DDE status flags of originating request*/
      BOOL        fAckPending;      /* TRUE= if waiting for WM_DDE_ACK        */
      HWND        hwndClient;       /* handle of window originating request   */
      HWND        hwndConv;         /* handle of window originating link      */
   }  DDEADVISE                     /* typedef for structure                  */
   , *PDDEADVISE;                   /* typedef for pointer to structure       */
/*
** tagDDEDATA
**
** This structure is created by the "owner" of a "WC_DdeItem" class window
** when it is requested to render a data item.
*/
   typedef struct tagDDEDATA        /* DdeData                                */
   {
      ULONG       cbData;           /* size of structure (in bytes)           */
      union                         /* defines data item as...                */
      {
         CHAR     abData [1];       /*    an array of bytes, or...            */
         ULONG    ulData;           /*    an unsigned long value              */
      }           pData;            /* Start of data                          */
   }  DDEDATA                       /* typedef for structure                  */
   , *PDDEDATA;                     /* typedef for pointer to structure       */
/*
** tagDDEITEM
**
** This structure is defines "WC_DdeItem" class information.
*/
   typedef struct tagDDEITEM        /* DdeItem                                */
   {
      ULONG       cb;               /* Size of structure (in bytes)           */
      HWND        hwndMenuItems;    /* handle of menu containing 'Item' text  */
      ULONG       hItem;            /* user defined handle                    */
   }  DDEITEM                       /* typedef for structure                  */
   , *PDDEITEM;                     /* typedef for pointer to structure       */
/*
** tagDDETOPIC
**
** This structure is defines "WC_DdeTopic" class information.
*/
   typedef struct  tagDDETOPIC      /* DdeTopic                               */
   {
      ULONG        cb;              /* Size of this structure in bytes        */
      HWND         hwndMenu;        /* handle of menu containing 'Topic' text */
      HWND         hwndMenuItems;   /* handle of menu containing 'Items'      */
      ULONG        cConv;           /* number of current active conversations */
   }  DDETOPIC                      /* typedef for structure                  */
   , *PDDETOPIC;                    /* typedef for pointer to structure       */
/*
** tagDLGITEM
**
** This structure is used to display and obtain 'Item' information from a
** dialog window.
*/
   typedef struct tagDLGITEM
   {
      ULONG       flType;           /* type of item (ie, menu, text, time)    */
      ULONG       hItem;            /* handle corresponding to 'flType'       */
      ULONG       cbData;           /* size of 'abData' data (in bytes)       */
      ULONG       ulMsgId;          /* message corresponding to request       */
      ULONG       ulOptions;        /* transaction options to use for request */
      USHORT      fsStatus;         /* DDE status flags                       */
      USHORT      usFormat;         /* DDE registered format identifier       */
      CHAR        szItemName [MAX_TEXTLENGTH];  /* Item name                  */
      CHAR        abData     [MAX_TEXTLENGTH];  /* Item data (if any)         */
   }  DLGITEM                       /* typedef for structure                  */
   , *PDLGITEM;                     /* typedef for pointer to structure       */

/*
** Macros to extract structure pointers from a message parameter:
*/
   #define PDDESTRUCTFROMMP(mp) ((PDDESTRUCT)(mp))
   #define PCONVIDFROMMP(mp)    ((PCONVID)(mp))

/*
** Window Procedures:
*/
   MRESULT  EXPENTRY DdeAdviseWindowProc (HWND,ULONG,MPARAM,MPARAM);
   MRESULT  EXPENTRY DdeConvWindowProc (HWND,ULONG,MPARAM,MPARAM);
   MRESULT  EXPENTRY DdeClientConvWindowProc (HWND,ULONG,MPARAM,MPARAM);
   MRESULT  EXPENTRY DdeItemWindowProc (HWND,ULONG,MPARAM,MPARAM);
   MRESULT  EXPENTRY DdeServerConvWindowProc (HWND,ULONG,MPARAM,MPARAM);
   MRESULT  EXPENTRY DdeTopicWindowProc (HWND,ULONG,MPARAM,MPARAM);
/*
** Functions:
*/
   MRESULT  APIENTRY DdeAdviseUmAdviseItem (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeAdviseUmUnAdviseItem (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeAdviseWmCreate (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeAdviseWmDestroy (HWND);
   MRESULT  APIENTRY DdeAdviseWmDdeAck (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeAdviseWmQueryWindowParams(HWND,MPARAM,MPARAM);
   PVOID    APIENTRY DdeAllocMem (ULONG);
   BOOL     APIENTRY DdeBroadcastMsg (HWND,ULONG,MPARAM,MPARAM,ULONG);
   BOOL     APIENTRY DdeCompareStrings (PSZ,PSZ,BOOL,ULONG,ULONG);
   PCONVCONTEXT APIENTRY DdeCopyConvContext (PCONVCONTEXT);
   MRESULT  APIENTRY DdeConvUmShowItemDialog (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvUmSetState (HWND,MPARAM);
   MRESULT  APIENTRY DdeConvUmSetItemText (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmClose (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmCreate (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeAck (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeAdvise (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeData (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeExecute (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdePoke (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeRequest (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeTerminate (HWND,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeInitiateAck (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDdeUnAdvise (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeConvWmDestroy (HWND);
   HWND     APIENTRY DdeCreateAdviseWindow (HWND,HWND,PCONVID,USHORT,USHORT);
   HBITMAP  APIENTRY DdeCreateBitmap (HPS,PDDESTRUCT);
   HMF      APIENTRY DdeCreateMetafile (HPS,PDDESTRUCT);
   HPAL     APIENTRY DdeCreatePalette (HPS,PDDESTRUCT);
   HWND     APIENTRY DdeCreateConvWindow (HWND,HWND,PSZ,PSZ,PCONVCONTEXT);
   HWND     APIENTRY DdeCreateTopicWindow
(HWND,HWND,HWND,PSZ,SHORT,SHORT,SHORT);
   HWND     APIENTRY DdeCreateItemWindow (HWND,HWND,HWND,ULONG,ULONG,PSZ);
   BOOL     APIENTRY DdeDeleteItem (PDDEITEM,PDDEITEM);
   BOOL     APIENTRY DdeFreeMem (PVOID,ULONG);
   MRESULT  APIENTRY DdeItemUmAdviseItem (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeItemUmRenderItem (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeItemUmShowItemDialog (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeItemWmCreate (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeItemWmDdeAdvise (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeItemWmDestroy (HWND);
   MRESULT  APIENTRY DdeItemWmQueryWindowParams (HWND,MPARAM,MPARAM);
   PDDESTRUCT APIENTRY DdeMakeDdeStruct (PSZ,PVOID,ULONG,USHORT,USHORT);
   PDDEITEM APIENTRY DdeQueryItem (PDDEITEM,PSZ);
   PDDEITEM APIENTRY DdeQueryItemNext (PDDEITEM,PDDEITEM);
   HWND     APIENTRY DdeQueryItemWindow (HWND,PSZ,USHORT,BOOL,ULONG);
   HWND     APIENTRY DdeQueryConvWindow (HWND,PSZ,PSZ);
   ULONG    APIENTRY DdeQueryTextDimension (HPS,PBYTE,ULONG,PULONG,PULONG);
   USHORT   APIENTRY DdeRegisterFmt (HATOMTBL,PSZ);
   BOOL     APIENTRY DdeScanCommandString (PSZ,PSZ,BOOL, ...);
   BOOL     APIENTRY DdeSetPasteLink (PSZ,PSZ,PSZ);
   MRESULT  APIENTRY DdeTopicUmAdviseItem (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeTopicUmShowItemDialog(HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeTopicWmClose (HWND);
   MRESULT  APIENTRY DdeTopicWmCreate (HWND,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeTopicWmDestroy (HWND);
   MRESULT  APIENTRY DdeTopicWmDdeAck (HWND,MPARAM,MPARAM,PCONVINFO);
   MRESULT  APIENTRY DdeTopicWmDdeAdvise (HWND,MPARAM,MPARAM,PCONVINFO);
   MRESULT  APIENTRY DdeTopicWmDdePoke (HWND,MPARAM,MPARAM,PCONVINFO);
   MRESULT  APIENTRY DdeTopicWmDdeRequest (HWND,MPARAM,MPARAM,PCONVINFO);
   MRESULT  APIENTRY DdeTopicWmDdeTerminate (HWND,MPARAM,MPARAM,PCONVINFO);
   MRESULT  APIENTRY DdeTopicWmDdeUnsupported(HWND,ULONG,MPARAM,MPARAM);
   MRESULT  APIENTRY DdeTopicWmQueryWindowParams(HWND,MPARAM,MPARAM);
   BOOL     APIENTRY DdeUnRegisterFmt (HATOMTBL,USHORT);

#endif
