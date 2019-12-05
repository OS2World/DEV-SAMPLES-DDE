/*==============================================================================
**
** Module:     Common.c
**
** Remarks:    This module contains common variables and definitions used by
**             both the 'Server' and 'Client' sample applications.
**
**============================================================================*/
/*
** System specific defines.
*/
   #define     INCL_DOSNLS
   #define     INCL_WIN
/*
** System specific include files.
*/
   #include    <os2.h>
   #include    <main.h>
   #include    <resource.h>

   #define     MAX_CODEPAGES  3

   CONVCONTEXT ConvContext;
   COUNTRYCODE CtryCode =  { 0L, 0L };
   COUNTRYINFO CtryInfo;

   LONG        cyBar                = 0L;
   SHORT       cxChar               = 0;
   SHORT       cyChar               = 0;
   SHORT       cyDesc               = 0;
   LONG        cxDiget              = 0L;
   LONG        cyLine               = 0L;
   LONG        cyMenu               = 0L;
   LONG        cxMinTrackSize       = 0L;
   LONG        cyMinTrackSize       = 0L;
   LONG        cxWindow             = 0L;
   LONG        cyWindow             = 0L;

   BOOL        fDataLinkExisted     = FALSE;

   HATOMTBL    hAtomTable           = (HATOMTBL) 0;
   HAB         hab                  = (HAB) NULL;
   HMQ         hmq                  = (HMQ) NULL;
   HDC         hdcMemory            = (HDC) NULL;
   HDC         hdcMetafile          = (HDC) NULL;
   HDC         hdcWindow            = (HDC) NULL;
   HPS         hpsMicro             = (HPS) NULL;

   HWND        hwndDesktop          = (HWND) 0;
   HWND        hwndClient           = (HWND) 0;
   HWND        hwndDialog           = (HWND) 0;
   HWND        hwndFrame            = (HWND) 0;
   HWND        hwndHorzScroll       = (HWND) 0;
   HWND        hwndListBox          = (HWND) 0;
   HWND        hwndMenu             = (HWND) 0;
   HWND        hwndMenuCodepages    = (HWND) 0;
   HWND        hwndMenuDisplay      = (HWND) 0;
   HWND        hwndMenuServers      = (HWND) 0;
   HWND        hwndMenuStatus       = (HWND) 0;
   HWND        hwndMenuTopics       = (HWND) 0;
   HWND        hwndStatus           = (HWND) 0;
   HWND        hwndTopic            = (HWND) 0;
   HWND        hwndVertScroll       = (HWND) 0;

   PFNWP       pfnOldFrameWindowProc  = NULL;
   PFNWP       pfnOldClientWindowProc = NULL;
   PFNWP       pfnDdeConvWindowProc   = NULL;

   PSZ         pszNull              = "";
   PVOID       pHeap                = NULL;

   SHORT       sAppSelected         = 0;
   SHORT       sTopicSelected       = 0;
   SHORT       sItemSelected        = 0;

   CHAR        szAppName  [MAX_TEXTLENGTH];
   CHAR        szItem     [MAX_TEXTLENGTH];
   CHAR        szItemText [MAX_TEXTLENGTH];
   CHAR        szText     [MAX_TEXTLENGTH];
   CHAR        szTopic    [MAX_TEXTLENGTH];

   ULONG       ulCpSize             = 0L;
   ULONG       ulMsgTime            = 0L;

   USHORT      usCpCurrent          = 1;
   USHORT      usFmtText            = 0;
   USHORT      usFmtCPText          = 0;
   USHORT      usFmtBitmap          = 0;
   USHORT      usFmtMetaFile        = 0;
   USHORT      usFmtPalette         = 0;
   USHORT      usFmtLink            = 0;
   USHORT      usFmtTime            = 0;

