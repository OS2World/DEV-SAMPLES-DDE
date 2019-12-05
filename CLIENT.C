/*==============================================================================
**
** Module:     Client.c
**
** Remarks:    This module contains common 'Client' window functionality.  This
**             is not to be confused with 'Client' DDE functionality.
**
**============================================================================*/
/*
** System specific defines:
*/
   #define     INCL_DOS
   #define     INCL_DEV
   #define     INCL_ERRORS
   #define     INCL_GPI
   #define     INCL_WIN
/*
** System specific include files:
*/
   #include    <os2.h>
   #include    <stdio.h>
   #include    <stdlib.h>
   #include    <string.h>
/*
** Application specific include files:
*/
   #include    <main.h>
   #include    <resource.h>
   #include    <common.h>
   #include    <draw.h>
   #include    <client.h>

/*==============================================================================
**
** Function:   CliWindowProc
**
** Usage:      Processes messages dispatched to a "WC_Client" class window.
**
** Remarks:    This function is specified as the "Window Procedure" when
**             registering (WinRegisterClass) a "WC_Client" window class.
**
**============================================================================*/
   MRESULT                          /* Message result                         */
   EXPENTRY    CliWindowProc
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
      case WM_CHAR:
      {
         MResult  = CliWmChar
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CLOSE:
      {
         MResult  = CliWmClose
                  ( hwnd
                  );
         break;
      }
      case WM_CONTROL:
      {
         MResult  = CliWmControl
                  (  hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_CREATE:
      {
         MResult  = CliWmCreate
                  ( hwnd
                  );
         break;
      }
      case WM_PAINT:
      {
         MResult  = CliWmPaint
                  ( hwnd
                  );
         break;
      }
      case WM_SIZE:
      {
         MResult  = CliWmSize
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_VSCROLL:
      {
         MResult  = CliWmVScroll
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      case WM_HSCROLL:
      {
         MResult  = CliWmHScroll
                  ( hwnd
                  , mp1
                  , mp2
                  );
         break;
      }
      default:
      {
         MResult  = WinDefWindowProc
                  (  hwnd
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
** Function:   CliWmChar
**
** Usage:      Processes a "WM_CHAR" message dispatched to a "WC_Client"
**             class window.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL) : Success indicator     */
   APIENTRY    CliWmChar
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* keyboard control code, count, scan code*/
,  MPARAM      mp2                  /* character and virtual key codes        */
)
{
   BOOL        fResult;
   MRESULT     MResult  = MRFROMSHORT (0);
/*
** Obtain keyboard control and virtual key codes...
*/
   USHORT      fsFlags  = SHORT1FROMMP (mp1);
   USHORT      usVkCode = SHORT2FROMMP (mp2);
/*
** Given the following virtual key code...
*/
   switch (usVkCode)
   {
      case VK_LEFT:
      case VK_RIGHT:
      {
/*
**       Pass the message to the Horizonal Scroll Bar window...
*/
         MResult  = WinSendMsg
                  ( hwndHorzScroll
                  , WM_CHAR
                  , mp1
                  , mp2
                  );
         break;
      }
      case VK_UP:
      case VK_DOWN:
      case VK_PAGEUP:
      case VK_PAGEDOWN:
      {
/*
**       Pass the message to the Vertical Scroll Bar window...
*/
         MResult  = WinSendMsg
                  ( hwndVertScroll
                  , WM_CHAR
                  , mp1
                  , mp2
                  );
         break;
      }
   }
/*
** Return the result message processing...
*/
   return (MResult);
}
/*==============================================================================
**
** Function:   CliConvWmClose
**
** Usage:      Processes a "WM_CLOSE" message dispatched to "WC_Client" class
**             window.
**
** Remarks:    A "WM_CLOSE" message is posted to a "WC_Client" class window
**             when the user has asked to 'Terminate' the application
**
==============================================================================*/
   MRESULT                          /* flReply: reserved value, zero (0)     */
   APIENTRY    CliWmClose
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult;
/*
** Tell all of our children to perform there (WM_CLOSE) close logic...
*/
   fResult  = WinBroadCastMsg
            ( hwnd
            , WM_CLOSE
            , MPVOID
            , MPVOID
            , BMSG_SEND
            );
/*
** Post ourselves a WM_QUIT message to exit the message dispatch loop...
*/
   fResult  = WinPostMsg
            ( hwnd
            , WM_QUIT
            , MPVOID
            , MPVOID
            );
/*
** Return reserved value...
*/
   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   CliWmControl
**
** Usage:      Processes a "WM_CONTROL" message dispatched to "WC_Client" class
**             window.
**
==============================================================================*/
   MRESULT                          /* flReply: reserved value, zero (0)     */
   APIENTRY    CliWmControl
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* Command (menu-identifier) value        */
,  MPARAM      mp2                  /* Source type/pointer-device indicator   */
)
{
   BOOL        fResult;
   ULONG       ulItemHandle;
   SHORT       sItemSelected;
/*
** Obtain 'Control' information...
*/
   HWND        hwndControl  = HWNDFROMMP   (mp2);
   USHORT      usControlId  = SHORT1FROMMP (mp1);
   USHORT      usNotifyCode = SHORT2FROMMP (mp1);

   if (usNotifyCode == LN_SELECT)
   {
/*
**    Obtain identifier of currently selected item...
*/
      sItemSelected  = (SHORT) WinSendMsg
                     ( hwndControl
                     , LM_QUERYSELECTION
                     , MPFROMLONG (LIT_FIRST)
                     , MPVOID
                     );
/*
**    If an item is currently selected...
*/
      if (sItemSelected != LIT_NONE)
      {
/*
**       Obtain handle associated with item...
*/
         ulItemHandle   = (ULONG) WinSendMsg
                        ( hwndControl
                        , LM_QUERYITEMHANDLE
                        , MPFROMSHORT (sItemSelected)
                        , MPVOID
                        );
/*
**       Store handle in window instance data...
*/
         fResult  = WinSetWindowULong
                  ( hwnd
                  , QWL_USER
                  , ulItemHandle
                  );
/*
**       Set dialog items to default values...
*/
         fResult = FrmResetDialogItems ();
/*
**       Update dialog items corresponding to selected item...
*/
         if (ulItemHandle)
         {
            fResult  = FrmSetDialogItems
                     ( ulItemHandle
                     );
         }
/*
**       Update Horizontal scroll bar...
*/
         fResult  = FrmResetScrollBar
                  ( hwndHorzScroll
                  );
/*
**       Update Horizontal scroll bar...
*/
         fResult  = FrmResetScrollBar
                  ( hwndVertScroll
                  );
      }
/*
**    Clear status window...
*/
      fResult  = FrmSetStatusText
               ( hab
               , hwndStatus
               , NULL
               , 0
               , 0
               );
/*
**    Cause 'Client' window to re-paint...
*/
      fResult  = WinInvalidateRect
               ( hwnd
               , NULL
               , FALSE
               );
   }
/*
** Return reserved value zero (0)...
*/
   return (MRFROMSHORT (0));
}
/*==============================================================================
**
** Function:   CliWmCreate
**
** Usage:      Processes a "WM_CREATE" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_CREATE message is 'sent' to a window procedure after the
**             window is created but before is it visible.  This offers the
**             window procedure an opportunity to perform initialization
**             actions.
**
**============================================================================*/
   MRESULT                          /* fResult (BOOL): Error indicator        */
   APIENTRY    CliWmCreate
(
   HWND        hwnd                 /* handle of window being created         */
)
{
   BOOL        fResult;
   ULONG       flResult;
   FATTRS      fAttr;
   FONTMETRICS fmFontInfo;
   SIZEL       sizl;

/*
** Create a screen compatible DC (Device Context)...
*/
   hdcMemory   = DevOpenDC          /* Create a Device context                */
               ( hab                /* Handle to Anchor-block                 */
               , OD_MEMORY          /* Context used to hold bitmaps           */
               , "*"                /* No device info from init file          */
               , 0L                 /* # of items in pdopData parameter       */
               , NULL               /* Open device context data area          */
               , (HDC) NULL         /* Context compatible with screen         */
               );
/*
** If open of 'Device Context' failed...
*/
   if (hdcMemory == (HDC) 0)
   {
      return MRFROMSHORT (1);       /* Discontinue window creation            */
   }
/*
** Set up Presentation page to use default dimensions...
*/
   sizl.cx  = 0L;
   sizl.cy  = 0L;
/*
** Create a Presentation Space compatible with the screen...
*/
   hpsMicro = GpiCreatePS
            ( hab                   /* Anchor-block handle                    */
            , WinOpenWindowDC (hwnd)
            , &sizl                 /* Presentation-page size structure       */
            , GPIA_ASSOC            /* Associate PS with Device-context       */
            | GPIF_DEFAULT          /* Coordinate format (4-byte integers)    */
            | GPIT_MICRO            /* Presentation-space type                */
            | PU_PELS               /* Presentation-page size units           */
            );
/*
** If creation failed...
*/
   if (hpsMicro == (HPS) 0)
   {
      return MRFROMSHORT (1);           /* Discontinue window creation        */
   }
/*
** Initialize 'Font' attributes...
*/
   fAttr.usRecordLength  = sizeof (FATTRS);
   fAttr.fsSelection     = 0;
   fAttr.lMatch          = 0L;
   fAttr.idRegistry      = 0;
   fAttr.usCodePage      = ConvContext.usCodepage;
   fAttr.lMaxBaselineExt = 6L;
   fAttr.lAveCharWidth   = 6L;
   fAttr.fsType          = 0;
   fAttr.fsFontUse       = 0;

   strcpy (fAttr.szFacename, "Courier");
/*
** Establish a logical font for the Presentation Space...
*/
   flResult = GpiCreateLogFont
            ( hpsMicro              /* handle presentation space              */
            , NULL                  /* logical font name                      */
            , 1L                    /* logical font identifier                */
            , &fAttr                /* address of font attribute structure    */
            );
/*
** Set character set of Presentation Space...
*/
   fResult  = GpiSetCharSet
            ( hpsMicro              /* handle presentation space              */
            , 1L                    /* logical font identifier                */
            );
/*
** Obtain complete font metric information...
*/
   fResult  = GpiQueryFontMetrics
            ( hpsMicro
            , (LONG) sizeof (fmFontInfo)
            , &fmFontInfo
            );
/*
** If failed to obtain font metrics...
*/
   if (!fResult)
   {
      return MRFROMSHORT (1);           /* Discontinue window creation
*/
   }
/*
** Obtain dimensions used for displaying text and positioning windows...
*/
   cyMenu   = WinQuerySysValue
            ( HWND_DESKTOP             /* Return values for desktop-window    */
            , SV_CYMENU                /* Height of menu bar in pels          */
            );
   cxChar   = (SHORT) fmFontInfo.lAveCharWidth;
   cyChar   = (SHORT) fmFontInfo.lMaxBaselineExt;
   cyDesc   = (SHORT) fmFontInfo.lMaxDescender;
   cyLine   = cyChar + cyDesc;

/*
** Return indicating no errors...
*/
   return MRFROMSHORT (0);              /* Continue window creation
*/
}
/*==============================================================================
**
** Function:   CliWmPaint
**
** Usage:      Processes a "WM_PAINT" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_PAINT message is dispatched to a window procedure when
**             a window needs repainting.
**
**============================================================================*/
   MRESULT                          /* fReply:  reserved value, zero          */
   APIENTRY    CliWmPaint
(
   HWND        hwnd                 /* handle of window receiving message     */
)
{
   BOOL        fResult;
   HPS         hps;
   ULONG       hItem;
   MATRIXLF    matlfIdentity;
   POINTL      ptl;
   RECTL       rcl;
/*
** Obtain a Presentation Space containing region requiring updating...
*/
   hps      = WinBeginPaint
            ( hwnd                  /* handle of window requiring updating    */
            , hpsMicro              /* handle presentation space              */
            , NULL                  /* address to receive bounding rectangle  */
            );
/*
** Obtain the currently selected item handle...
*/
   hItem    = WinQueryWindowULong
            ( hwnd
            , QWL_USER
            );
/*
** Obtain size of the window...
*/
   fResult  = WinQueryWindowRect
            ( hwnd
            , &rcl
            );
/*
** Reset the 'Default View Matrix' to the 'Identify' matrix...
*/
   fResult  = GpiSetDefaultViewMatrix
            ( hps                   /* handle presentation space              */
            , 0L                    /* number of elements supplied in matrix  */
            , &matlfIdentity        /* transformation matrix                  */
            , TRANSFORM_REPLACE     /* discard previous transform             */
            );
/*
** Clear output devise associated with presentation space...
*/
   fResult  = GpiErase
            ( hps                   /* handle presentation space              */
            );
/*
** If a item handle selected into 'Client' window...
*/
   if (hItem)
   {
/*
**    Check if the item handle corresponds to a DDEINIT structure...
*/
      if (hItem & FDDEINIT)
      {
         hItem &= (~FDDEINIT);      /* mask out DDEINIT structure indicator   */
      }
      else
      {
         PDDESTRUCT  pDdeStruct = (PDDESTRUCT) hItem;
         RECTL       rclBounds;
/*
**       Check if display mode is 'Binary'...
*/
         if (WinIsMenuItemChecked (hwndMenu, IDM_DISPLAY_BINARY))
         {
            fResult  = DrwBinary
                     ( hps
                     , hwnd
                     , hwndVertScroll
                     , hwndHorzScroll
                     , DDES_PABDATA (pDdeStruct)
                     , pDdeStruct->cbData
                     );
         }
         else if (pDdeStruct->usFormat == usFmtText)
         {
            fResult  = DrwText
                     ( hps
                     , DDES_PABDATA (pDdeStruct)
                     , pDdeStruct->cbData
                     , hwnd
                     , hwndVertScroll
                     , hwndHorzScroll
                     );
         }
         else if (pDdeStruct->usFormat == usFmtCPText)
         {
            PCPTEXT  pCPText = (PCPTEXT) DDES_PABDATA (pDdeStruct);

            fResult  = DrwText
                     ( hps
                     , pCPText->abText
                     , pDdeStruct->cbData - sizeof (CPTEXT)
                     , hwnd
                     , hwndVertScroll
                     , hwndHorzScroll
                     );
         }
         else if (pDdeStruct->usFormat == usFmtBitmap)
         {
            HBITMAP hbmp   = DdeCreateBitmap
                           ( hps
                           , pDdeStruct
                           );
/*
**          If bitmap successfully created...
*/
            if (hbmp)
            {
               fResult  = DrwBitmap
                        ( hps
                        , hbmp
                        , hwnd
                        , hwndVertScroll
                        , hwndHorzScroll
                        );
           }
        }
        else if (pDdeStruct->usFormat == usFmtMetaFile)
        {
           HMF    hmf   = DdeCreateMetafile
                        ( hps
                        , pDdeStruct
                        );
/*
**          If bitmap successfully created...
*/
            if (hmf)
            {
                fResult = DrwMetafile
                        ( hps
                        , hmf
                        , hwnd
                        , hwndVertScroll
                        , hwndHorzScroll
                        );
            }
/*
**          Delete the temporary metafile...
*/
            fResult = GpiDeleteMetafile
                   ( hmf
                   );
         }
         else if (pDdeStruct->usFormat == usFmtTime)
         {
            USHORT  cxClient = rcl.xRight - rcl.xLeft;
            USHORT  cyClient = rcl.yTop   - rcl.yBottom;
            USHORT  cxDiget  = cxClient / 11;
            USHORT  cyBar    = cxDiget  / 4;
            ULONG   ulTime   = *((PULONG)(DDES_PABDATA (pDdeStruct)));
            POINTL  ptl;
/*
**          Set coordinates to start drawing...
*/
            ptl.x    = cxDiget;
            ptl.y    = 0;
/*
**          Draw associated data in 'Time' format...
*/
            fResult  = Drwtime
                     ( hps          /* handle presentation space              */
                     , &ptl         /* coordinates to begin drawing operation */
                     , ulTime       /* actual time (in milliseconds)          */
                     , cyBar        /* thickness of bar to draw digit         */
                     , 4            /* ratio of thickness to digit height     */
                     );
         }
      }
   }
/*
** Indicate painting has completed, and restore presentation space state...
*/
   fResult  = WinEndPaint
            ( hps
            );
/*
** Return reserved value...
*/
   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   CliWmSize
**
** Usage:      Processes a "WM_SIZE" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_SIZE message is dispatched to a window procedure when
**             a window changes its size.
**
**============================================================================*/
   MRESULT                          /* fReply:  reserved value, zero          */
   APIENTRY    CliWmSize
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* old size of window                     */
,  MPARAM      mp2                  /* new size of window                     */
)
{
   BOOL        fResult;
   USHORT      cxNew  = SHORT1FROMMP (mp2);
   USHORT      cyNew  = SHORT2FROMMP (mp2);
/*
** If the new size of a window contains both a height and width...
*/
   if (cxNew && cyNew)
   {
      cxWindow = cxNew;
      cyWindow = cyNew;
      cxDiget  = cxWindow / 11;
      cyBar    = cxDiget  / 4;
   }
/*
** Reset Horizontal Scroll Bar...
*/
   fResult  = FrmResetScrollBar
            ( hwndHorzScroll
            );
/*
** Reset Vertical Scroll Bar...
*/
   fResult  = FrmResetScrollBar
            ( hwndVertScroll
            );
/*
** Return reserved value...
*/
   return MRFROMLONG (0L);
}
/*==============================================================================
**
** Function:   CliWmHScroll
**
** Usage:      Processes a "WM_HSCROLL" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_HSCROLL message is sent by a "WC_SCROLLBAR" when it has a
**             significant event to notify its owner.
**
**============================================================================*/
   MRESULT                          /* fReply:  reserved value, zero          */
   APIENTRY    CliWmHScroll
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* scroll bar identifier                  */
,  MPARAM      mp2                  /* slider position and command identifier */
)
{
   BOOL        fResult     = FALSE;
   SHORT       sCmd        = SHORT2FROMMP (mp2);
   SHORT       sSliderPos  = 0;
/*
** Ignore insignificant events...
*/
   if (sCmd == SB_SLIDERTRACK
   ||  sCmd == SB_ENDSCROLL)
   {
      return MRFROMSHORT (0);
   }
/*
** Obtain current slider position...
*/
   sSliderPos  = (SHORT) WinSendMsg
               ( hwndHorzScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Given the following command...
*/
   switch (sCmd)
   {
      case SB_LINERIGHT:
      {
         sSliderPos += 1;
         break;
      }
      case SB_PAGERIGHT:
      {
         sSliderPos += cxWindow / cxChar;
         break;
      }
      case SB_LINELEFT:
      {
        sSliderPos -= 1;
        break;
      }
      case SB_PAGELEFT:
      {
         sSliderPos -= cxWindow / cxChar;
         break;
      }
      case SB_SLIDERPOSITION:
      {
         sSliderPos = SHORT1FROMMP (mp2);
         break;
      }
   }

   fResult  = (BOOL) WinSendMsg
            ( hwndHorzScroll
            , SBM_SETPOS
            , MPFROMSHORT (sSliderPos)
            , MPVOID
            );
   fResult  = WinInvalidateRect
            ( hwnd
            , NULL
            , FALSE
            );

   return MRFROMSHORT (0);
}
/*==============================================================================
**
** Function:   CliWmVScroll
**
** Usage:      Processes a "WM_VSCROLL" message dispatched to a "WC_Client"
**             class window.
**
** Remarks:    A WM_VSCROLL message is sent by a "WC_SCROLLBAR" when it has a
**             significant event to notify its owner.
**
**============================================================================*/
   MRESULT                          /* fReply:  reserved value, zero          */
   APIENTRY    CliWmVScroll
(
   HWND        hwnd                 /* handle of window receiving message     */
,  MPARAM      mp1                  /* scroll bar identifier                  */
,  MPARAM      mp2                  /* slider position and command identifier */
)
{
   BOOL        fResult     = FALSE;
   SHORT       sCmd        = SHORT2FROMMP (mp2);
   SHORT       sSliderPos  = 0;
/*
** Ignore insignificant events...
*/
   if (sCmd == SB_SLIDERTRACK
   ||  sCmd == SB_ENDSCROLL)
   {
      return MRFROMSHORT (0);
   }
/*
** Obtain current slider position...
*/
   sSliderPos  = (SHORT) WinSendMsg
               ( hwndVertScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Given the following command...
*/
   switch (sCmd)
   {
      case SB_LINEDOWN:
      {
         sSliderPos += 1;
         break;
      }
      case SB_PAGEDOWN:
      {
         sSliderPos += cyWindow / cyLine;
         break;
      }
      case SB_LINEUP:
      {
         sSliderPos -= 1;
         break;
      }
      case SB_PAGEUP:
      {
         sSliderPos -= cyWindow / cyLine;
         break;
      }
      case SB_SLIDERPOSITION:
      {
         sSliderPos = SHORT1FROMMP (mp2);
         break;
      }
   }

   fResult  = (BOOL) WinSendMsg
            ( hwndVertScroll
            , SBM_SETPOS
            , MPFROMSHORT (sSliderPos)
            , MPVOID
            );
   fResult  = WinInvalidateRect
            ( hwnd
            , NULL
            , FALSE
            );

   return MRFROMSHORT (0);
}
