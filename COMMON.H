/*==============================================================================
**
** Module:     Common.h
**
** Remarks:    This module contains common definitions used in both sample
**             applications.
**
**============================================================================*/
   #ifndef COMMON_INCLUDED          /* Check if already included              */
   #define COMMON_INCLUDED          /* Mark as included...                    */

   #define FDDEINIT    0x00000001

   #define TAB        (BYTE) 0x09
   #define EOT        (BYTE) 0x00
   #define CR         (BYTE) 0x0D
   #define LF         (BYTE) 0x0A
   #define SPACE      (BYTE) 0x20

   extern CONVCONTEXT ConvContext;
   extern COUNTRYCODE CtryCode;
   extern COUNTRYINFO CtryInfo;

   extern LONG        cyBar;
   extern SHORT       cxChar;
   extern SHORT       cyChar;
   extern LONG        cxDiget;
   extern SHORT       cyDesc;
   extern LONG        cyLine;
   extern LONG        cyMenu;
   extern LONG        cxMinTrackSize;
   extern LONG        cyMinTrackSize;
   extern LONG        cxWindow;
   extern LONG        cyWindow;

   extern BOOL        fDataLinkExisted;

   extern HATOMTBL    hAtomTable;
   extern HAB         hab;
   extern HMQ         hmq;
   extern HDC         hdcMemory;
   extern HDC         hdcMetafile;
   extern HDC         hdcWindow;
   extern HPS         hpsMicro;

   extern HWND        hwndDesktop;
   extern HWND        hwndClient;
   extern HWND        hwndDialog;
   extern HWND        hwndFrame;
   extern HWND        hwndHorzScroll;
   extern HWND        hwndListBox;
   extern HWND        hwndMenu;
   extern HWND        hwndMenuCodepages;
   extern HWND        hwndMenuDisplay;
   extern HWND        hwndMenuServers;
   extern HWND        hwndMenuStatus;
   extern HWND        hwndMenuTopics;
   extern HWND        hwndStatus;
   extern HWND        hwndTopic;
   extern HWND        hwndVertScroll;

   extern PFNWP       pfnOldFrameWindowProc;
   extern PFNWP       pfnOldClientWindowProc;
   extern PFNWP       pfnDdeConvWindowProc;

   extern PSZ         pszNull;
   extern PVOID       pHeap;

   extern SHORT       sAppSelected;
   extern SHORT       sTopicSelected;
   extern SHORT       sItemSelected;

   extern CHAR        szAppName  [];
   extern CHAR        szItem     [];
   extern CHAR        szItemText [];
   extern CHAR        szText     [];
   extern CHAR        szTopic    [];

   extern ULONG       ulCpSize;
   extern ULONG       ulMsgTime;

   extern USHORT      usCpCurrent;
   extern USHORT      usFmtText;
   extern USHORT      usFmtCPText;
   extern USHORT      usFmtBitmap;
   extern USHORT      usFmtMetaFile;
   extern USHORT      usFmtPalette;
   extern USHORT      usFmtLink;
   extern USHORT      usFmtTime;

   #define UM_First           WM_USER + 1
   #define UM_AckItem         WM_USER + 2
   #define UM_AdviseItem      WM_USER + 3
   #define UM_ExecuteCommand  WM_USER + 5
   #define UM_InitiateAck     WM_USER + 6
   #define UM_IsRequestValid  WM_USER + 7
   #define UM_PokeItem        WM_USER + 8
   #define UM_RenderItem      WM_USER + 9
   #define UM_RequestItem     WM_USER + 10
   #define UM_SetState        WM_USER + 11
   #define SS_ADD       0x0001
   #define SS_REMOVE    0x0002
   #define SS_REPLACE   0x0003
   #define UM_ShowItem        WM_USER + 12
   #define UM_ShowItemDialog  WM_USER + 13
   #define UM_Terminate       WM_USER + 14
   #define UM_UnAdviseItem    WM_USER + 15
   #define UM_SetItemText     WM_USER + 16
   #define UM_Last            WM_USER + 99

#endif
