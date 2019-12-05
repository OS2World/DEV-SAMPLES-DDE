/*==============================================================================
**
** Module:     Client.h
**
** Remarks:    This module contains definitions used to implement common
**             'Client' window behaviour.
**
**============================================================================*/
   #ifndef CLIENT_INCLUDED
   #define CLIENT_INCLUDED

   MRESULT EXPENTRY CliWindowProc (HWND, ULONG,  MPARAM, MPARAM);
   MRESULT APIENTRY CliWmChar (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY CliWmClose (HWND);
   MRESULT APIENTRY CliWmControl (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY CliWmCreate (HWND);
   MRESULT APIENTRY CliWmPaint (HWND);
   MRESULT APIENTRY CliWmSize (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY CliWmVScroll (HWND, MPARAM, MPARAM);
   MRESULT APIENTRY CliWmHScroll (HWND, MPARAM, MPARAM);

#endif   /* ifndef MAIN_INCLUDED    */
