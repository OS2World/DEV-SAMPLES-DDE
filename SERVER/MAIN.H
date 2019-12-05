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

   #define MAX_ITEMS                10
   #define MAX_TEXTLENGTH           64
   #define MAX_TOPICS               10
   #define MIN_HEAPSIZE             (8 * 1024)
   #define ONE_TIMERTICK            5000  /* value in milliseconds (5 seconds)*/

   #define WC_Client                "WC_Client"
/*
** Function prototypes.
*/
   BOOL    APIENTRY CreateTopicClock     ();
   BOOL    APIENTRY CreateTopicSystem    ();
   HMF     APIENTRY CreateTimeMetaFile   (HPS,  ULONG);
   HBITMAP APIENTRY CreateTimeBitmap     (HPS,  ULONG);
   BOOL    APIENTRY CreateWindows        (HAB);

   VOID             ExitProc             (ULONG);

   MRESULT EXPENTRY ClientWindowProc     (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT EXPENTRY FrameWindowProc      (HWND, ULONG, MPARAM, MPARAM);

   BOOL    APIENTRY RegisterWindowClasses(HAB);

   MRESULT APIENTRY UmData               (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY UmExecuteCommand     (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY UmTerminate          (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY UmRenderItem         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmClose              (HWND);
   MRESULT APIENTRY WmCommand            (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmControl            (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmCreate             (HWND);
   MRESULT APIENTRY WmDdeAck             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeAdvise          (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeData            (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeInitiate        (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdePoke            (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeRequest         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeTerminate       (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDdeUnadvise        (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmDestroy            (HWND);
   MRESULT APIENTRY WmFormatFrame        (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT APIENTRY WmMenuSelect         (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmPaint              (HWND);
   MRESULT APIENTRY WmQueryFrameCtlCount (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT APIENTRY WmQueryTrackInfo     (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT APIENTRY WmSize               (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmTimer              (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmVScroll             (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY WmHScroll             (HWND, MPARAM, MPARAM);

   #endif   /* ifndef MAIN_INCLUDED    */
