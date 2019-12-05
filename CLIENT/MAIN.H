/*==============================================================================
**
** Module:     Main.h
**
** Remarks:    This module contains definitions and function prototypes
**             used by the "main' application module.
**
**============================================================================*/
   #ifndef MAIN_INCLUDED
   #define MAIN_INCLUDED

   #define EXIT_OK                  FALSE
   #define EXIT_ERROR               TRUE
   #define fOK                      (BOOL) FALSE
   #define fERROR                   (BOOL) !fOK

   #define COUNT_OPTIONS            (LONG) (PMF_DEFAULTS + 1)
   #define LDESC                    256L

   #define MAX_CODEPAGES            3
   #define MAX_ITEMS                10
   #define MAX_TEXTLENGTH           64
   #define MAX_TOPICS               9
   #define MIN_HEAPSIZE             (4 * 1024)
   #define WC_Client                "WC_Client"
   #define WC_DialogOwner           "WC_DialogOwner"
/*
** Function prototypes.
*/
   BOOL    APIENTRY CreateWindows         (HAB);
   MRESULT EXPENTRY ClientWindowProc      (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT EXPENTRY FrameWindowProc       (HWND, ULONG, MPARAM, MPARAM);

   VOID             ExitProc              (ULONG);

   BOOL    APIENTRY IsLinkActive          (PSZ, PSZ);
   BOOL    APIENTRY RegisterWindowClasses (HAB);

   MRESULT APIENTRY UmInitiateAck         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY UmTerminate           (HWND, MPARAM, MPARAM);

   MRESULT APIENTRY WmClose               (HWND);
   MRESULT APIENTRY WmCommand             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmControl             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmCreate              (HWND);
   MRESULT APIENTRY WmDdeAck              (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeAdvise           (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY UmData                (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeData             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeInitiate         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeInitiateAck      (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdePoke             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeRequest          (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeTerminate        (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeUnadvise         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDestroy             (HWND);
   MRESULT APIENTRY WmFormatFrame         (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY WmMenuSelect          (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmPaint               (HWND);
   BOOL    APIENTRY WmPaintText           (HPS,  PDDESTRUCT);
   BOOL    APIENTRY WmPaintBitmap         (HPS,  PDDESTRUCT);
   BOOL    APIENTRY WmPaintMetafile       (HPS,  PDDESTRUCT);
   BOOL    APIENTRY WmPaintPalette        (HPS,  PDDESTRUCT);
   BOOL    APIENTRY WmPaintTime           (HPS,  PDDESTRUCT);
   MRESULT APIENTRY WmQueryFrameCtlCount  (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY WmQueryTrackInfo      (HWND, ULONG, MPARAM, MPARAM);
   MRESULT APIENTRY WmSize                (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmVScroll             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmHScroll             (HWND, MPARAM, MPARAM);

#endif   /* ifndef MAIN_INCLUDED    */
