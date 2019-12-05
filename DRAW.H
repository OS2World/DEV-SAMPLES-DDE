/*==============================================================================
**
** Module:     Draw.h
**
** Remarks:    This module contains definitions used to implement common
**             'Drawing' functions:
**
**============================================================================*/
   #ifndef DRAW_INCLUDED
   #define DRAW_INCLUDED

   #define ACH_HEXDIGIT    "0123456789abcdef"
   #define CH_TAB          '\t'
   #define CH_CRLF         '\n'
/*
** Function prototypes.
*/
   BOOL  APIENTRY DrwASCII (HPS,PBYTE,ULONG,ULONG);
   BOOL  APIENTRY DrwBinary (HPS,HWND,HWND,HWND,PVOID,ULONG);
   BOOL  APIENTRY DrwBitmap (HPS,HBITMAP,HWND,HWND,HWND);
   BOOL  APIENTRY DrwCenterBar (HPS,ULONG,ULONG);
   BOOL  APIENTRY DrwDigit (HPS,PPOINTL,ULONG,ULONG,ULONG);
   ULONG APIENTRY DrwFormatASCIILine (PVOID,PVOID,PULONG);
   ULONG APIENTRY DrwFormatHexLine (PVOID,PVOID,PULONG,ULONG);
   BOOL  APIENTRY DrwHorzBar (HPS,ULONG,ULONG);
   BOOL  APIENTRY DrwMetafile (HPS,HMF,HWND,HWND,HWND);
   BOOL  APIENTRY DrwPalette (HPS,HPAL,HWND,HWND,HWND);
   BOOL  APIENTRY DrwSetScrollBars (HWND,HWND,USHORT,USHORT,ULONG,ULONG,ULONG, ULONG, ULONG, ULONG);
   BOOL  APIENTRY DrwText (HPS,PBYTE,ULONG,HWND,HWND,HWND);
   BOOL  APIENTRY DrwTime (HPS,PPOINTL,ULONG,ULONG,ULONG);
   BOOL  APIENTRY DrwVertBar1 (HPS,ULONG,ULONG);
   BOOL  APIENTRY DrwVertBar2 (HPS,ULONG,ULONG);

#endif
