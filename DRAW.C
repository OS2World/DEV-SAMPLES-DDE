/*==============================================================================
**
** Module:     Draw.c
**
** Usage:      This module contains routines that facilitate drawing DDE
**             data items in any of following DDE formats.
**
**                Text
**                Bitmap
**                Metafile
**                Time (private)
**
**             In addition, each format can be drawn in either its native
**             "image" mode or "binary" mode.
**
**
** Remarks:    These drawing functions combine the power of GPI and ease of PM
**             to draw and scroll images.  Rather than having a unique (and
**             complicated) method to scroll each format, a simpler more robust
**             method was used; GPI Viewing Transforms.  All drawing functions
**             in GPI are based on a 'current position' based from an 'origin'.
**
**             When a presentation space is created or obtained, the current
**             position and origin are both set to (0,0) corresponding to the
**             lower-left corner of a window.  By shifting this origin left,
**             right, up or down, drawing begins at different positions
**             relative to the lower-left corner of a window.  This technique
**             simulates scrolling, and provides a consistent manner in which
**             to scroll a window regardless of the format being drawn.  In
**             addition, the drawing functions become much simpler since they
**             always start drawing at position (0,0) (which may have been
**             shifted).  The amount of shifting required is determined by the
**             current positions of the Vertical and Horizontal scroll bars
**             which is update each time user invokes a scrolling event.
**
** Scrolling:  Scrolling sometimes seems backwards.  For example, when you cause
**             a horizontal scroll-bar to move to the right, you must shift the
**             corresponding data to the left!  Similarly, if you move the
**             vertical scroll-bar down you must shift the corresponding data
**             up! In conclusion, you must shift data in the opposite direction
**             in which the scroll-bar was moved.  Also, regardless of which
**             format is being rendered, the horizontal scroll-bar range and
**             position represents one character of text and the vertical
**             scroll-bar represents one line of text (in pels).
**
**============================================================================*/
/*
** System specific defines.
*/
   #define     INCL_DOS
   #define     INCL_GPI
   #define     INCL_WIN
/*
** System specific include files.
*/
   #include    <os2.h>
   #include    <string.h>
   #include    <stdlib.h>
   #include    <ctype.h>
/*
** Application specific include files.
*/
   #include    <main.h>
   #include    <draw.h>
   #include    <common.h>
/*
** The following diagram illustrates how a digit is constructed.  The numbers
** shown identify one of seven 'bars' used to draw a digit.
**             _________
**             |  5    |
**             |       |
**           4 |       |6
**             |_______|
**             |   3   |
**             |       |
**           2 |       |7
**             |_______|
**                1
**
** The following (BAR_) definitions correspond to the 'bars' above and are
** used in determining if a 'bar' is required to display the digit as specified
** in the (DIGIT_) definitions below:
*/
   #define     BAR_ONE        0x00000001L
   #define     BAR_TWO        0x00000010L
   #define     BAR_THREE      0x00000100L
   #define     BAR_FOUR       0x00001000L
   #define     BAR_FIVE       0x00010000L
   #define     BAR_SIX        0x00100000L
   #define     BAR_SEVEN      0x01000000L
/*
** The following (bit-mask) definitions identify the combination of 'bars'
** required to draw a digit:
*/
   #define     DIGIT_ZERO     0X01111011L
   #define     DIGIT_ONE      0X01100000L
   #define     DIGIT_TWO      0X00110111L
   #define     DIGIT_THREE    0X01110101L
   #define     DIGIT_FOUR     0X01101100L
   #define     DIGIT_FIVE     0X01011101L
   #define     DIGIT_SIX      0X01001111L
   #define     DIGIT_SEVEN    0X01110000L
   #define     DIGIT_EIGHT    0X01111111L
   #define     DIGIT_NINE     0X01111100L
/*
** Zero-based array of bit-masks associating the above definitions to its
** corresponding numerical number:
*/
   ULONG       aulDigitMask[] = { DIGIT_ZERO
                                , DIGIT_ONE
                                , DIGIT_TWO
                                , DIGIT_THREE
                                , DIGIT_FOUR
                                , DIGIT_FIVE
                                , DIGIT_SIX
                                , DIGIT_SEVEN
                                , DIGIT_EIGHT
                                , DIGIT_NINE
                                };
/*
** Number of pixels between 'bars':
*/
   USHORT      cxGap          = 3;
   POINTL      ptlZero        = {0,0};
/*
** Array of 'fixed' values specifing angles required to display digit:
*/
   FIXED       afxAngle[]     = { MAKEFIXED (0,0)
                                , MAKEFIXED (90,0)
                                , MAKEFIXED (180,0)
                                , MAKEFIXED (270,0)
                                };

/*==============================================================================
**
** Function:   DrwASCII
**
** Usage:      Draws an (CR/LF delimited) ASCII text string on a Presentation
**             Space.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    DrwASCII
(
   HPS         hps                  /* handle of presentation space to draw in*/
,  PBYTE       pabText              /* pointer to buffer containing ASCII text*/
,  ULONG       cbText               /* size of buffer (in bytes)              */
,  ULONG       cLines               /* number of lines of text                */
)
{
   ULONG       cch         = 0;
   BOOL        fResult     = FALSE;
   ULONG       flResult    = 0L;
   POINTL      ptl;
/*
** Establish an end-of-buffer pointer (or pointer to last byte in buffer...
*/
   PBYTE       pabTextMax  = pabText + cbText;
   BYTE        szString [255];
   PBYTE       pszString   = szString;
/*
** Establish the starting point to draw text (left and top)...
*/
   ptl.x = 0L;
   ptl.y = cyLine                   /* height of a line (in pixels)           */
         * cLines;                  /* number of lines                        */
/*
** While more text...
*/
   while (pabTextMax >= pabText)
   {
/*
**    If end-of-line character encountered...
*/
      if (*pabText == CR            /* carriage return   (0x0d)               */
      ||  *pabText == LF            /* line feed         (0x0a)               */
      ||  *pabText == EOT)          /* end-of-text       (0x00)               */
      {
/*
**       Regardless of which end-of-line character encountered, terminate the
**       temporary buffer with an end-of-text character...
*/
         *pszString = EOT;
/*
**       Adjust drawing origin down one line...
*/
         ptl.x    = 0L;
         ptl.y   -= cyLine;
/*
**       Set new drawing origin...
*/
         fResult  = GpiMove
                  ( hps
                  , &ptl
                  );
/*
**       Output the string...
*/
         flResult = GpiCharString
                  ( hps
                  , strlen (szString)
                  , szString
                  );
/*
**       Skip current character...
*/
         pabText++;
/*
**       If the next character is also an end-of-line character (as in the
**       case of a CR/LF pair), skip it too...
*/
         if (*pabText == CR
         ||  *pabText == LF)
         {
            pabText++;
         }
/*
**       Reset buffer pointer to beginning of buffer...
*/
         pszString = szString;
      }
      else
      {
/*
**       Copy the character into the temporary buffer...
*/
         *pszString = *pabText;
/*
**       Replace all 'Tab' (0x09) with spaces...
*/
         if (*pszString == TAB)
         {
            *pszString = ' ';
         }
/*
**       Advance both current character and temporary buffer pointers to next
**       character position in buffer...
*/
         pabText++;
         pszString++;
      }
   }
/*
** Return success indicator...
*/
   return (fResult);
}
/*==============================================================================
**
** Function:   DrwTime
**
** Usage:      Takes a 'system' defined 'time' format and displays it
**             graphically on a Presentation Space.
**
** Remarks:    The Viewing transform of the specified presentation space must
**             be set to the 'Identify' matrix prior to invocation of this
**             function.
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwTime
(
   HPS         hps                  /* handle of presentation space           */
,  PPOINTL     pptl                 /* contains position to start drawing     */
,  ULONG       ulTime               /* actual system time                     */
,  ULONG       ulWidth              /* width of a single bar in the digit     */
,  ULONG       ulRatio              /* ration of bar width do digit width     */
)
{
   BOOL        fResult;
/*
** Calculate the current hour, minute and second...
*/
   ULONG       ulHours   =   ulTime / 3600000L;
   ULONG       ulMinutes =  (ulTime % 3600000L) / 60000L;
   ULONG       ulSeconds = ((ulTime % 3600000L) % 60000L) / 1000;
/*
** Calculate width and height of a digit...
*/
   ULONG       cxDiget   = ulWidth * ulRatio;
   ULONG       cyDiget   = 2 * cxDiget;
/*
** Set drawing pattern in presentation space to a 'hatched' pattern...
*/
   fResult  = GpiSetPattern
            ( hps
            , PATSYM_HATCH
            );
/*
** Draw first digit of hours...
*/
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulHours / 10L         /* numerical digit to draw                */
            );
/*
** Adjust drawing position and draw second digit of hours...
*/
   pptl->x += cxDiget + (cxDiget / 2);
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulHours % 10L         /* numerical digit to draw                */
            );
/*
** Adjust drawing position and draw first digit of minutes...
*/
   pptl->x += cxDiget * 2;
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulMinutes / 10L       /* numerical digit to draw                */
            );
/*
** Adjust drawing position and draw second digit of minutes...
*/
   pptl->x += cxDiget + (cxDiget / 2);
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulMinutes % 10L       /* numerical digit to draw                */
            );
/*
** Adjust drawing position and draw first digit of seconds...
*/
   pptl->x += cxDiget * 2;
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulSeconds / 10L       /* numerical digit to draw                */
            );
/*
** Adjust drawing position and draw second digit of seconds...
*/
   pptl->x += cxDiget + (cxDiget / 2);
   fResult  = DrwDigit
            ( hps                   /* handle presentation space to drawn in  */
            , pptl                  /* origin from which drawing starts       */
            , ulWidth               /* width of a bars drawn in digit         */
            , ulRatio               /* height of digit in multiples of this...*/
            , ulSeconds % 10L       /* numerical digit to draw                */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwDigit
**
** Usage:      Draws a single digit.
**
** Remarks:    The following diagram illustrates how a digit is constructed.
**             The numbers shown identify one of seven 'bars' used to draw a
**             digit.
**                ___________
**             |\ \    5    / /|    Four different bars are used to draw a
**             | \ \_______/ / |    digit: a horizontal (top/bottom) bar
**             |  \         /  |    (#1 and #5), two different vertical bars
**             |   |       |   |    (#2 and #6) and (#4 and #7), and finally
**             | 4 |       | 6 |    a center bar (#3).
**             |   |       |   |    Why 4 bars and not just 3? Well...
**             |  / _______ \  |    because of visual appearance.  The angle
**             | / /       \ \ |    of the center is different the the angle of
**             |/ /    3    \ \|    the top and bottom bars.  Therefore, the
**             |\ \         / /|    angles of a vertical bar are different
**             | \ \_______/ / |    depending on which horizontal bar it
**             |  \         /  |    borders.  This minor adjustment gives
**             |   |       |   |    a clean mitered look to the entire digit.
**             | 2 |       | 7 |
**             |   |       |   |
**             |  / _______ \  |
**             | / /   1   \ \ |
**             |/ /_________\ \|
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwDigit
(
   HPS         hps                  /* handle of presentation space           */
,  PPOINTL     pptl                 /* contains position to start drawing     */
,  ULONG       ulWidth              /* width of a single bar in the digit     */
,  ULONG       ulRatio              /* ration of bar width do digit width     */
,  ULONG       ulDigit              /* numeric value of digit to draw         */
)
{
   BOOL        fResult;
   MATRIXLF    matlf;
   MATRIXLF    matlfOld;
   POINTL      ptl = *pptl;
/*
** Obtain the bit-mask associated with the digit to be drawn...
*/
   ULONG       ulBarMask = aulDigitMask [ulDigit];
/*
** Query current model transform matrix and save it...
*/
   fResult  = GpiQueryModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlfOld             /* transformation matrix                  */
            );
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform        */
            );
/*
** Check if the digit being drawn requires bar #1...
*/
   if (ulBarMask & BAR_ONE)
   {
      fResult  = DrwHorzBar
               ( hps
               , ulWidth
               , ulRatio
               );
   }
/*
** Check if the digit being drawn requires bar #2...
*/
   if (ulBarMask & BAR_TWO)
   {
      fResult  = DrwVertBar1
               ( hps
               , ulWidth
               , ulRatio
               );
   }
/*
** Calculate new origin to center of digit...
*/
   ptl.x = 0;
   ptl.y = ulWidth * ulRatio;
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform        */
            );
/*
** Check if the digit being drawn requires bar #3...
*/
   if (ulBarMask & BAR_THREE)
   {
      fResult  = DrwCenterBar
               ( hps
               , ulWidth
               , ulRatio
               );
   }
/*
** Check if the digit being drawn requires bar #4...
*/
   if (ulBarMask & BAR_FOUR)
   {
      fResult  = DrwVertBar2
               ( hps
               , ulWidth
               , ulRatio
               );
   }
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlfOld             /* transformation matrix                  */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            );
/*
** Calculate new origins to top of digit...
*/
   ptl.x = pptl->x + ulWidth * ulRatio ;
   ptl.y = pptl->y + 2 * ulWidth * ulRatio ;
/*
** Apply rotation operation to transform...
*/
   fResult  = GpiRotate
            ( hps                   /* handle presentation space              */
            , &matlf                /* transformation matrix to rotate        */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            , afxAngle[2]           /* rotation angle                         */
            , &ptlZero              /* point at which rotation occurs         */
            );
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_ADD         /* combine with existing transform        */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform        */
            );
/*
** Check if the digit being drawn requires bar #5...
*/
   if (ulBarMask & BAR_FIVE)
   {
      fResult  = DrwHorzBar
               ( hps                /* handle presentation space              */
               , ulWidth            /* width of a bars drawn in digit         */
               , ulRatio            /* height of digit in multiples of this...*/
               );
   }
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlfOld             /* transformation matrix                  */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            );
/*
** Apply rotation operation to transform...
*/
   fResult  = GpiRotate
            ( hps                   /* handle presentation space              */
            , &matlf                /* transformation matrix to rotate        */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            , afxAngle[2]           /* rotation angle                         */
            , &ptlZero              /* point at which rotation occurs         */
            );
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_ADD         /* combine with existing transform        */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform        */
            );
/*
** Check if the digit being drawn requires bar #6...
*/
   if (ulBarMask & BAR_SIX)
   {
      fResult  = DrwVertBar1
               ( hps                /* handle presentation space              */
               , ulWidth            /* width of a bars drawn in digit         */
               , ulRatio            /* height of digit in multiples of this...*/
               );
   }
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlfOld             /* transformation matrix                  */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            );
/*
** Calculate new origins back to middle-right...
*/
   ptl.y -= ulWidth * ulRatio;
/*
** Apply rotation operation to transform...
*/
   fResult  = GpiRotate
            ( hps                   /* handle presentation space              */
            , &matlf                /* transformation matrix to rotate        */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            , afxAngle[2]           /* rotation angle                         */
            , &ptlZero              /* point at which rotation occurs         */
            );
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_ADD         /* combine with existing transform        */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform        */
            );
/*
** Check if the digit being drawn requires bar #7...
*/
   if (ulBarMask & BAR_SEVEN)
   {
      fResult  = DrwVertBar2
               ( hps                /* handle presentation space              */
               , ulWidth            /* width of a bars drawn in digit         */
               , ulRatio            /* height of digit in multiples of this...*/
               );
   }
/*
** Set the Model transform to the adjusted matrix...
*/
   fResult  = GpiSetModelTransformMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlfOld             /* transformation matrix                  */
            , TRANSFORM_REPLACE     /* replace existing transform             */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwHorzBar.
**
** Usage:      Draw a (top/bottom) horizontal bar of a digit.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwHorzBar
(
   HPS         hps                  /* handle presentation space              */
,  ULONG       ulWidth
,  ULONG       ulRatio
)
{
   BOOL        fResult;
   LONG        flResult;
   POINTL      ptl[4];

   ptl[0].x = 3;
   ptl[0].y = 0;

   ptl[1].x = ulWidth * ulRatio - 3;
   ptl[1].y = 0;

   ptl[2].x = ptl[1].x - ulWidth;
   ptl[2].y = ulWidth;

   ptl[3].x = ptl[0].x + ulWidth;
   ptl[3].y = ulWidth;
/*
** Begin construction of an area...
*/
   fResult  = GpiBeginArea
            ( hps                   /* presentation space handle              */
            , BA_BOUNDARY           /* draw boundary lines                    */
            );
/*
** Move GPI 'current' position to point specified in first element of array...
*/
   fResult  = GpiMove
            ( hps                   /* presentation space handle              */
            , &ptl[0]               /* position (in world coordinates)        */
            );
/*
** Draw series of straight lines starting at 'current' position and connecting
** points specified in array of points...
*/
   flResult = GpiPolyLine
            ( hps                   /* presentation space handle              */
            , 3L
            , &ptl[1]
            );
/*
** Close figure (by a line drawn to the start point of the figure)...
*/
   fResult  = GpiCloseFigure
            ( hps                   /* presentation space handle              */
            );
/*
** Close area associated with presentation space...
*/
   flResult = GpiEndArea
            ( hps                   /* presentation space handle              */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwCenterBar
**
** Usage:      Draws center bar of a digit.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwCenterBar
(
   HPS         hps
,  ULONG       ulWidth
,  ULONG       ulRatio
)
{
   BOOL        fResult;
   LONG        flResult;
   POINTL      ptl[6];
   ULONG       uly1 = ulWidth >> 1;
   ULONG       uly2 = ulWidth - uly1;

   ptl[0].x = 3;
   ptl[0].y = 0;

   ptl[1].x = ulWidth;
   ptl[1].y = - uly1;

   ptl[2].x = ulWidth * ulRatio - ulWidth - 3;
   ptl[2].y = -uly1;

   ptl[3].x = ulWidth * ulRatio - 3;
   ptl[3].y = 0;

   ptl[4].x = ptl[2].x;
   ptl[4].y = uly2;

   ptl[5].x = ptl[1].x + 3;
   ptl[5].y = ptl[4].y;
/*
** Begin construction of an area...
*/
   fResult  = GpiBeginArea
            ( hps                   /* presentation space handle              */
            , BA_BOUNDARY           /* draw boundary lines                    */
            );
/*
** Move GPI 'current' position to point specified in first element of array...
*/
   fResult  = GpiMove
            ( hps                   /* presentation space handle              */
            , &ptl[0]               /* position (in world coordinates)        */
            );
/*
** Draw series of straight lines starting at 'current' position and connecting
** points specified in array of points...
*/
   flResult = GpiPolyLine
            ( hps                   /* presentation space handle              */
            , 5L
            , &ptl[1]
            );
/*
** Close figure (by a line drawn to the start point of the figure)...
*/
   fResult  = GpiCloseFigure
            ( hps                   /* presentation space handle              */
            );
/*
** Close area associated with presentation space...
*/
   flResult = GpiEndArea
            ( hps                   /* presentation space handle              */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwVertBar1
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwVertBar1
(
   HPS         hps
,  ULONG       ulWidth
,  ULONG       ulRatio
)
{
   BOOL        fResult;
   LONG        flResult;
   POINTL      ptl[4];

   ptl[0].x = 0;
   ptl[0].y = 3;

   ptl[1].x = 0;
   ptl[1].y = ulWidth * ulRatio - 3;

   ptl[2].x = ulWidth;
   ptl[2].y = ptl[1].y - (ulWidth >> 1);

   ptl[3].x = ptl[2].x;
   ptl[3].y = ptl[0].y + ulWidth;
/*
** Begin construction of an area...
*/
   fResult  = GpiBeginArea
            ( hps                   /* presentation space handle              */
            , BA_BOUNDARY           /* draw boundary lines                    */
            );
/*
** Move GPI 'current' position to point specified in first element of array...
*/
   fResult  = GpiMove
            ( hps                   /* presentation space handle              */
            , &ptl[0]               /* position (in world coordinates)        */
            );
/*
** Draw series of straight lines starting at 'current' position and connecting
** points specified in array of points...
*/
   flResult = GpiPolyLine
            ( hps                   /* presentation space handle              */
            , 3L
            , &ptl[1]
            );
/*
** Close figure (by a line drawn to the start point of the figure)...
*/
   fResult  = GpiCloseFigure
            ( hps                   /* presentation space handle              */
            );
/*
** Close area associated with presentation space...
*/
   flResult = GpiEndArea
            ( hps                   /* presentation space handle              */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwVertBar2
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwVertBar2
(
   HPS         hps
,  ULONG       ulWidth
,  ULONG       ulRatio
)
{
   BOOL        fResult;
   LONG        flResult;
   POINTL      ptl[4];

   ptl[0].x = 0;
   ptl[0].y = 3;

   ptl[1].x = 0;
   ptl[1].y = ulWidth * ulRatio - 3;

   ptl[2].x = ulWidth;
   ptl[2].y = ptl[1].y - ulWidth;

   ptl[3].x = ptl[2].x;
   ptl[3].y = ptl[0].y + (ulWidth - (ulWidth >> 1));
/*
** Begin construction of an area...
*/
   fResult  = GpiBeginArea
            ( hps                   /* presentation space handle              */
            , BA_BOUNDARY           /* draw boundary lines                    */
            );
/*
** Move GPI 'current' position to point specified in first element of array...
*/
   fResult  = GpiMove
            ( hps                   /* presentation space handle              */
            , &ptl[0]               /* position (in world coordinates)        */
            );
/*
** Draw series of straight lines starting at 'current' position and connecting
** points specified in array of points...
*/
   flResult = GpiPolyLine
            ( hps                   /* presentation space handle              */
            , 3L
            , &ptl[1]
            );
/*
** Close figure (by a line drawn to the start point of the figure)...
*/
   fResult  = GpiCloseFigure
            ( hps                   /* presentation space handle              */
            );
/*
** Close area associated with presentation space...
*/
   flResult = GpiEndArea
            ( hps                   /* presentation space handle              */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwText
**
** Usage:      Draws CF_TEXT formatted data on the specified presentation space.
**
** Remarks:    The Viewing transform of the specified presentation space must
**             be set to the 'Identify' matrix prior to invocation of this
**             function.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwText
(
   HPS         hps                  /* handle of presentation space to draw in*/
,  PBYTE       pabText              /* buffer containing text                 */
,  ULONG       cbText               /* length of text to draw (in bytes)      */
,  HWND        hwnd                 /* handle of window to draw to            */
,  HWND        hwndVertScroll       /* handle of vertical scroll-bar          */
,  HWND        hwndHorzScroll       /* handle of horizontal scroll-bar        */
)
{
   BOOL        fResult     = FALSE;
   MATRIXLF    matlf;
   ULONG       cLines      = 0;
   ULONG       cxText      = 0;
   ULONG       cyText      = 0;
   ULONG       cxWindow    = 0;
   ULONG       cyWindow    = 0;
   SHORT       ySliderPos  = 0;
   SHORT       xSliderPos  = 0;
   RECTL       rcl;
   POINTL      ptl;
/*
** Obtain current dimensions of the window text is being drawn to...
*/
   fResult  = WinQueryWindowRect
            ( hwnd
            , &rcl
            );
/*
** Calculate the width and height of this window...
*/
   cxWindow = rcl.xRight - rcl.xLeft;
   cyWindow = rcl.yTop - rcl.yBottom;
/*
** Obtain the current vertical scroll-bar position...
*/
   ySliderPos  = (SHORT) WinSendMsg
               ( hwndVertScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current horizontal scroll-bar position...
*/
   xSliderPos  = (SHORT) WinSendMsg
               ( hwndHorzScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current (default) viewing matrix...
*/
   fResult  = GpiQueryDefaultViewMatrix
            ( hps
            , 9L                    /* number of elements to return           */
            , &matlf                /* address of matrix to receive elements  */
            );
/*
** Determine maximum number of lines, width and height...
*/
   cLines   = DdeQueryTextDimension
            ( hps                   /* handle presentation space              */
            , pabText               /* address of text to draw                */
            , cbText                /* length of text to draw (in bytes)      */
            , &cxText               /* receives maximum text width (in pels)  */
            , &cyText               /* receives maximum text height (in pels) */
            );
/*
** Calculate the horizontal and vertical distance required to shift the 'origin'
** of the presentation space to simulate scrolled text...
**
** Note: See 'Scrolling' in the in the beginning of this module for additional
**       comments regarding scrolling.
*/
   ptl.x = -1                       /* shift is inverse of,                   */
         * (xSliderPos * cxChar);   /* current position * width of character  */
   ptl.y = cyWindow - cyText        /* one line down from top of window       */
         + (ySliderPos * cyLine);   /* plus, current position * height of line*/
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Default viewing transform to the adjusted matrix...
*/
   fResult  = GpiSetDefaultViewMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform, see   */
            );                      /*   remarks in function prologue...      */
/*
** Draw ASCII text string...
*/
   fResult  = DrwASCII
            ( hps                   /* presentation space handle              */
            , pabText               /* address of text to draw                */
            , cbText                /* length of text (in bytes)              */
            , cLines                /* number of lines to draw                */
            );
/*
** Update both horizontal and vertical scroll bars with current information...
*/
   fResult  = DrwSetScrollBars
            ( hwndVertScroll        /* handle vertical scroll-bar window      */
            , hwndHorzScroll        /* handle horizontal scroll-bar window    */
            , xSliderPos            /* horizontal scroll-bar position         */
            , ySliderPos            /* vertical scroll-bar position           */
            , cxChar                /* width of a character (in pels)         */
            , cyLine                /* height of line (in pels)               */
            , cxWindow              /* width of window (in pels)              */
            , cyWindow              /* height of window (in pels)             */
            , cxText                /* maximum width of text line (in pels)   */
            , cyText                /* height of text line (in pels)          */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwBitmap
**
** Usage:      Draws CF_BITMAP formatted data on the specified presentation
**             space.
**
** Remarks:    The Viewing transform of the specified presentation space must
**             be set to the 'Identify' matrix prior to invocation of this
**             function.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwBitmap
(
   HPS         hps                  /* handle of presentation space to draw in*/
,  HBITMAP     hbmp                 /* handle of bitmap to draw               */
,  HWND        hwnd                 /* handle of window to draw to            */
,  HWND        hwndVertScroll       /* handle of vertical scroll-bar          */
,  HWND        hwndHorzScroll       /* handle of horizontal scroll-bar        */
)
{
   POINTL      aptl [3];
   BOOL        fResult     = FALSE;
   ULONG       flResult    = GPI_OK;
   MATRIXLF    matlf;
   ULONG       cxWindow    = 0;
   ULONG       cyWindow    = 0;
   SHORT       ySliderPos  = 0;
   SHORT       xSliderPos  = 0;
   BITMAPINFOHEADER bmp;
   RECTL       rcl;
   POINTL      ptl;
/*
** Obtain current dimensions of the window text is being drawn to...
*/
   fResult  = WinQueryWindowRect
            ( hwnd
            , &rcl
            );
/*
** Calculate the width and height of this window...
*/
   cxWindow = rcl.xRight - rcl.xLeft;
   cyWindow = rcl.yTop - rcl.yBottom;
/*
** Obtain the current vertical scroll-bar position...
*/
   ySliderPos  = (SHORT) WinSendMsg
               ( hwndVertScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current horizontal scroll-bar position...
*/
   xSliderPos  = (SHORT) WinSendMsg
               ( hwndHorzScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current (default) viewing matrix...
*/
   fResult  = GpiQueryDefaultViewMatrix
            ( hps
            , 9L                    /* number of elements to return           */
            , &matlf                /* address of matrix to receive elements  */
            );
/*
** Set size of information to be queried...
*/
   bmp.cbFix   = sizeof(BITMAPINFOHEADER);
/*
** Obtain bitmap information...
*/
   fResult  = GpiQueryBitmapParameters
            ( hbmp                  /* handle of bitmap to query              */
            , &bmp                  /* address of buffer to receive bitmap    */
            );
/*
** Calculate the horizontal and vertical distance required to shift the 'origin'
** of the presentation space to simulate scrolled text...
**
** Note: See 'Scrolling' in the in the beginning of this module for additional
**       comments regarding scrolling.
*/
   ptl.x = -1                       /* shift is inverse of,                   */
         * (xSliderPos * cxChar);   /* current position * width of character  */
   ptl.y = cyWindow - bmp.cy        /* one scan line down from top of window  */
         + (ySliderPos * cyLine);   /* plus, current position * height of line*/
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Default viewing transform to the adjusted matrix...
*/
   fResult  = GpiSetDefaultViewMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform, see   */
            );                      /*   remarks in function prologue...
*/
/*
** Initialize array of POINTL structures specifying target and source
** rectangles (in world coordinates) for bitblt...
*/
   aptl[0].x = 0L;      aptl[0].y = 0L;         /* bottom-left of target      */
   aptl[1].x = bmp.cx;  aptl[1].y = bmp.cy;     /* top-right of target        */
   aptl[2].x = 0L;      aptl[2].y = 0L;         /* bottom-left of source      */
/*
** Blit the bitmap to the presentation space in 'World Coordinates'.  As stated
** in the prologue at the beginning of this file, GPI Transforms are used to
** facilitate scrolling.  To affect the bitblt by the transform, it must be done
** in world coordinates.
*/
   flResult = GpiWCBitBlt
            ( hps                   /* handle presentation space              */
            , hbmp                  /* handle of bitmap to blit               */
            , 3L                    /* count of POINTL structures in array    */
            , aptl                  /* address of POINTL array               */
            , ROP_SRCCOPY           /* mix by copying source bits             */
            , BBO_OR                /* logical-OR eliminated rows and columns */
            );
/*
** Release bitmap from the presentation space...
*/
   fResult  = GpiDeleteBitmap
            ( hbmp                  /* handle bitmap                          */
            );
/*
** Update both horizontal and vertical scroll bars with current information...
*/
   fResult  = DrwSetScrollBars
            ( hwndVertScroll        /* handle vertical scroll-bar window      */
            , hwndHorzScroll        /* handle horizontal scroll-bar window    */
            , xSliderPos            /* horizontal scroll-bar position         */
            , ySliderPos            /* vertical scroll-bar position           */
            , cxChar                /* width of a character (in pels)         */
            , cyLine                /* height of line (in pels)               */
            , cxWindow              /* width of window (in pels)              */
            , cyWindow              /* height of window (in pels)             */
            , bmp.cx                /* maximum width of bitmap (in pels)      */
            , bmp.cy                /* height of bitmap line (in pels)        */
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwMetafile
**
** Usage:      Draws CF_METAFILE formatted data on the specified presentation
**             space.
**
** Remarks:    The Viewing transform of the specified presentation space must
**             be set to the 'Identify' matrix prior to invocation of this
**             function.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwMetafile
(
   HPS         hps                  /* handle presentation space              */
,  HMF         hmf                  /* handle of metafile to draw             */
,  HWND        hwnd                 /* handle of window to draw to            */
,  HWND        hwndVertScroll       /* handle of vertical scroll-bar          */
,  HWND        hwndHorzScroll       /* handle of horizontal scroll-bar        */
)
{
  #define COUNT_OPTIONS (LONG) (PMF_DEFAULTS + 1)
  #define LDESC    256L

   BYTE        abDesc    [LDESC];
   LONG        alOptions [COUNT_OPTIONS];
   BOOL        fResult     = FALSE;
   ULONG       flResult    = GPI_OK;
   LONG        lSegCount   = 0;
   MATRIXLF    matlf;
   ULONG       cxMetafile  = 0;
   ULONG       cyMetafile  = 0;
   ULONG       cxWindow    = 0;
   ULONG       cyWindow    = 0;
   SHORT       ySliderPos  = 0;
   SHORT       xSliderPos  = 0;
   SIZEL       sizeL;
   ULONG       ulOptions   = 0L;
   RECTL       rcl;
   POINTL      ptl;
/*
** Obtain current dimensions of the window text is being drawn to...
*/
   fResult  = WinQueryWindowRect
            ( hwnd
            , &rcl
            );
/*
** Calculate the width and height of this window...
*/
   cxWindow = rcl.xRight - rcl.xLeft;
   cyWindow = rcl.yTop - rcl.yBottom;
/*
** Obtain the current vertical scroll-bar position...
*/
   ySliderPos  = (SHORT) WinSendMsg
               ( hwndVertScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current horizontal scroll-bar position...
*/
   xSliderPos  = (SHORT) WinSendMsg
               ( hwndHorzScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current (default) viewing matrix...
*/
   fResult  = GpiQueryDefaultViewMatrix
            ( hps
            , 9L                    /* number of elements to return           */
            , &matlf                /* address of matrix to receive elements  */
            );
/*
** Obtain the current size of the presentation space...
*/
   fResult  = GpiQueryPS
             ( hps
             , &sizeL
             );
/*
** Set metafile dimensions to equal the size of the presentation space, and
** reset SIZEL structure...
*/
   cxMetafile = sizeL.cx;
   cyMetafile = sizeL.cy;
   sizeL.cx   = 0L;
   sizeL.cy   = 0L;
/*
** Set presentation space size, units and format...
*/
   fResult  = GpiSetPS
            ( hps                   /* handle presentation space              */
            , &sizeL                /* set to maximum (screen) device size    */
            , PU_PELS               /* size units                             */
            );
/*
** Calculate the horizontal and vertical distance required to shift the 'origin'
** of the presentation space to simulate scrolled text...
**
** Note: See 'Scrolling' in the in the beginning of this module for additional
**       comments regarding scrolling.
*/
   ptl.x = -1                       /* shift is inverse of,                   */
         * (xSliderPos * cxChar);   /* current position * width of character  */
   ptl.y = cyWindow - cyMetafile    /* one line down from top of window       */
         + (ySliderPos * cyLine);   /* plus, current position * height of line*/
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Default viewing transform to the adjusted matrix...
*/
   fResult  = GpiSetDefaultViewMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform, see   */
            );                      /*   remarks in function prologue...
*/
/*
** Initialize array of options used to play the metafile into the presentation
** space...
*/
   alOptions [PMF_SEGBASE]         = 0L;
   alOptions [PMF_LOADTYPE]        = LT_DEFAULT;
   alOptions [PMF_RESOLVE]         = 0L;
   alOptions [PMF_LCIDS]           = LC_LOADDISC;
   alOptions [PMF_RESET]           = RES_NORESET;
   alOptions [PMF_SUPPRESS]        = SUP_NOSUPPRESS;
   alOptions [PMF_COLORTABLES]     = CTAB_REPLACE;
   alOptions [PMF_COLORREALIZABLE] = CREA_DEFAULT;
   alOptions [PMF_DEFAULTS]        = DDEF_DEFAULT;
/*
** Play the metafile into presentation space...
*/
   fResult  = (BOOL) GpiPlayMetaFile
            ( hps                   /* handle presentation space              */
            , hmf                   /* handle of metafile containing data     */
            , COUNT_OPTIONS         /* count of elements in alOptions         */
            , alOptions             /* array of options for playing          */
            , &lSegCount            /* reserved                               */
            , LDESC                 /* size of 'abDesc' (in bytes)            */
            , abDesc                /* descriptive record                     */
            );
/*
** Update both horizontal and vertical scroll bars with current information...
*/
   fResult  = DrwSetScrollBars
            ( hwndVertScroll        /* handle vertical scroll-bar window      */
            , hwndHorzScroll        /* handle horizontal scroll-bar window    */
            , xSliderPos            /* horizontal scroll-bar position         */
            , ySliderPos            /* vertical scroll-bar position           */
            , cxChar                /* width of a character (in pels)         */
            , cyLine                /* height of line (in pels)               */
            , cxWindow              /* width of window (in pels)              */
            , cyWindow              /* height of window (in pels)             */
            , cxMetafile            /* maximum width of bitmap (in pels)      */
            , cyMetafile            /* height of bitmap line (in pels)        */
            );
/*
** Reset presentation space...
*/
   fResult  = GpiResetPS
            ( hps
            , GRES_ALL
            );

   return (TRUE);
}
/*==============================================================================
**
** Function:   DrwBinary
**
** Usage:      Draws data in hexadecimal format.
**
** Remarks:    Drawing is done in columns.  That is, the address of each line
**             is calculated first, converted to ASCII text then Drawn.  Next,
**             the data itself is converted to hexadecimal format and drawn.
**             Finally, the data is drawn in ASCII format and drawn at the end
**             of the line.
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwBinary
(
   HPS         hps                  /* handle of presentation space           */
,  HWND        hwnd                 /* handle of window to draw to            */
,  HWND        hwndVertScroll       /* handle of vertical scroll-bar          */
,  HWND        hwndHorzScroll       /* handle of horizontal scroll-bar        */
,  PVOID       pvData               /* address of buffer containing data      */
,  ULONG       cbData               /* size of data (in bytes)                */
)
{
   ULONG       cbFormat;
   ULONG       cchText;
   ULONG       cchMax;
   ULONG       cxWindow    = 0;
   ULONG       cyWindow    = 0;
   BOOL        fResult;
   ULONG       flResult;
   MATRIXLF    matlf;
   POINTL      ptlDraw;
   PBYTE       pbData      = (PBYTE) pvData;
   PBYTE       pbDataLast  = pbData + cbData;
   PBYTE       pbVisible;
   PBYTE       pbVisibleLast;
   PCHAR       pchText;
   RECTL       rcl;
   SHORT       xSliderPos  = 0;
   SHORT       ySliderPos  = 0;
   POINTL      ptl;
/*
** Obtain current dimensions of the window text is being drawn to...
*/
   fResult  = WinQueryWindowRect
            ( hwnd
            , &rcl
            );
/*
** Calculate the width and height of this window...
*/
   cxWindow = rcl.xRight - rcl.xLeft;
   cyWindow = rcl.yTop - rcl.yBottom;
/*
** Obtain the current vertical scroll-bar position...
*/
   ySliderPos  = (SHORT) WinSendMsg
               ( hwndVertScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current horizontal scroll-bar position...
*/
   xSliderPos  = (SHORT) WinSendMsg
               ( hwndHorzScroll
               , SBM_QUERYPOS
               , MPVOID
               , MPVOID
               );
/*
** Obtain the current (default) viewing matrix...
*/
   fResult  = GpiQueryDefaultViewMatrix
            ( hps
            , 9L                    /* number of elements to return           */
            , &matlf                /* address of matrix to receive elements  */
            );
/*
** Calculate the horizontal and vertical distance required to shift the 'origin'
** of the presentation space to simulate scrolled text...
**
** Note: See 'Scrolling' in the in the beginning of this module for additional
**       comments regarding scrolling.
*/
   ptl.x = -1                       /* shift is inverse of,                   */
         * (xSliderPos * cxChar);   /* current position * width of character  */
   ptl.y = cyWindow
         - (((cbData / 16) + 1) * cyLine)
         + (ySliderPos * cyLine);   /* plus, current position * height of line*/
/*
** Apply adjustments to 'Identify' transformation matrix...
*/
   fResult  = GpiTranslate
            ( hps                   /* handle presentation space              */
            , &matlf                /* receives new transform matrix          */
            , TRANSFORM_REPLACE     /* replace previous transform             */
            , &ptl                  /* shifted point relative to origin       */
            );
/*
** Set the Default viewing transform to the adjusted matrix...
*/
   fResult  = GpiSetDefaultViewMatrix
            ( hps                   /* handle presentation space              */
            , 9L                    /* number of elements                     */
            , &matlf                /* transformation matrix                  */
            , TRANSFORM_ADD         /* combine with existing transform, see   */
            );                      /*   remarks in function prologue...
*/
/*
** Allocate a buffer for formatting text string...
*/
   pchText  = malloc
            ( MAX_TEXTLENGTH
            );
/*
** Setup initial drawing positions...
*/
   rcl.xLeft   = 0;
   rcl.xRight  = 0;
   rcl.yBottom = 0;
   rcl.yTop    = ((cbData + 15) / 16) * cyLine;
   ptlDraw.x   = rcl.xRight + cxChar;
   ptlDraw.y   = rcl.yTop   - cyLine;
/*
** Set color attribute of presentation space to 'Red'...
*/
   fResult  = GpiSetColor
            ( hps
            , CLR_RED
            );
/*
** Calculate the first and last lines of text that, when drawn, are visible
** on the window.  This is a slight improvement in drawing time.
*/
   pbVisible      = pbData + (ySliderPos * 16L);
   pbVisibleLast  = pbVisible + ((cyWindow / cyLine) * 16L);
/*
** Draw data...
*/
   for ( pbData = (PBYTE) pvData    /* set to beginning of data buffer        */
       , cchMax = 0                 /* maximum number of characters in a line */
       ; pbData < pbDataLast        /* while more data,                       */
       ; pbData += 16               /* advance pointer to next logical line,  */
       , ptlDraw.y -= cyLine)       /* and decrement window drawing position  */
   {
      if (pbData < pbVisible)       /* if not yet visible,                    */
      {
         continue;                  /* skip to next line                      */
      }
      if (pbData > pbVisibleLast)   /* if no long visible,                    */
      {
         break;                     /* quit drawing text.                     */
      }
/*
**    Set the size of data to be drawn equal to the size of 'pointer'...
*/
      cbFormat = sizeof (PBYTE);
/*
**    Draw the address of the current line...
*/
      cchText  = DrwFormatHexLine
               ( pchText            /* buffer to receive formatted text       */
               , &pbData            /* address of data buffer to format       */
               , &cbFormat          /* size of data buffer (in bytes)         */
               , sizeof (PBYTE)     /* size of data elements                  */
               );
/*
**    Draw the formatted text to the presentation space...
*/
      flResult = GpiCharStringAt
               ( hps                /* handle presentation space              */
               , &ptlDraw           /* drawing origin                         */
               , cchText            /* size of text to draw (in bytes)        */
               , pchText            /* formatted text buffer                  */
               );
   }
/*
** Adjust horizontal starting position...
*/
   rcl.xRight  += (cchText + 1)     /* size of text just drawn (plus null)    */
               * cxChar;            /* times width of character (in pels)     */
/*
** Calculate new drawing origins...
*/
   ptlDraw.x   = rcl.xRight;
   ptlDraw.y   = rcl.yTop - cyLine; /* decrement one line                     */
/*
** Set color attribute of presentation space to 'Blue'...
*/
   fResult  = GpiSetColor
            ( hps
            , CLR_BLUE
            );
/*
** Draw hexadecimal digits...
*/
   for ( pbData = (PBYTE) pvData    /* set to beginning of data buffer        */
       , cchMax = 0                 /* maximum number of characters in a line */
       ; pbData < pbDataLast        /* while more data,                       */
       ; pbData += 16               /* advance pointer to next logical line,  */
       , ptlDraw.y -= cyLine )      /* and decrement window drawing position  */
   {
      if (pbData < pbVisible)       /* if not yet visible,                    */
      {
         continue;                  /* skip to next line                      */
      }
      if (pbData > pbVisibleLast)   /* if no long visible,                    */
      {
         break;                     /* quit drawing text.                     */
      }
/*
**    Calculate size of data to be formatted.  This is the smaller of either
**    the remaining number of bytes to draw, or the size of a line...
*/
      cbFormat = min ((pbDataLast - pbData), 16);
/*
**    Format a line of hexadecimal data...
*/
      cchText  = DrwFormatHexLine
               ( pchText            /* buffer to receive formatted text       */
               , pbData             /* address of data buffer to format       */
               , &cbFormat          /* size of data buffer (in bytes)         */
               , 1L                 /* size of data elements                  */
               );
/*
**    Draw the formatted text to the presentation space...
*/
      flResult = GpiCharStringAt
               ( hps                /* handle presentation space              */
               , &ptlDraw           /* drawing origin                         */
               , cchText            /* size of text to draw (in bytes)        */
               , pchText            /* formatted text buffer                  */
               );
/*
**    If the count of characters found on this line exceed the maximum number
**    of characters found so far...
*/
      if (cchText > cchMax)
      {
         cchMax = cchText;          /* ...save this count                     */
      }
   }
/*
** Advance the starting horizontal drawing position to the right skip
** hexadecimal text...
*/
   rcl.xRight += cchMax * cxChar;
/*
** Calculate new drawing origins...
*/
   ptlDraw.x = rcl.xRight;
   ptlDraw.y = rcl.yTop - cyLine;
/*
** Set color attribute of presentation space to 'Black'...
*/
   fResult  = GpiSetColor
            ( hps
            , CLR_BLACK
            );
/*
** Draw hexadecimal digits...
*/
   for ( pbData = (PBYTE) pvData    /* set to beginning of data buffer        */
       ; pbData < pbDataLast        /* while more data,                       */
       ; pbData += 16               /* advance pointer to next logical line,  */
       , ptlDraw.y -= cyLine )      /* and decrement window drawing position  */
   {
      if (pbData < pbVisible)       /* if not yet visible,                    */
      {
         continue;                  /* skip to next line                      */
      }
      if (pbData > pbVisibleLast)   /* if no long visible,                    */
      {
         break;                     /* quit drawing text.                     */
      }
/*
**    Calculate size of data to be formatted.  This is the smaller of either
**    the remaining number of bytes to draw, or the size of a line...
*/
      cbFormat = min ((pbDataLast - pbData), 16);
/*
**    Format a line of ASCII text data...
*/
      cchText  = DrwFormatASCIILine
               ( pchText            /* buffer to receive formatted text       */
               , pbData             /* address of data buffer to format       */
               , &cbFormat );       /* size of data buffer (in bytes)         */
/*
**    Draw the formatted text to the presentation space...
*/
      flResult = GpiCharStringAt
               ( hps                /* handle presentation space              */
               , &ptlDraw           /* drawing origin                         */
               , cchText            /* size of text to draw (in bytes)        */
               , pchText            /* formatted text buffer                  */
               );
   }
/*
** Update both horizontal and vertical scroll bars with current information...
*/
   fResult  = DrwSetScrollBars
            ( hwndVertScroll        /* handle vertical scroll-bar window      */
            , hwndHorzScroll        /* handle horizontal scroll-bar window    */
            , xSliderPos            /* horizontal scroll-bar position         */
            , ySliderPos            /* vertical scroll-bar position           */
            , cxChar                /* width of a character (in pels)         */
            , cyLine                /* height of line (in pels)               */
            , cxWindow              /* width of window (in pels)              */
            , cyWindow              /* height of window (in pels)             */
            , rcl.xRight            /* maximum width of bitmap (in pels)      */
            , rcl.yTop              /* height of bitmap line (in pels)        */
            );
/*
** Free the formatting buffer
*/
   free( pchText );

   return (TRUE);
}
/*=============================================================================
**
** Function:   DrwFormatASCIILine
**
** Usage:      Formats a buffer into an ASCII string.
**
** Remarks:    Non-printable characters are replaced with '.' characters.  The
**             string is NOT null terminated on return and the last character
**             is always a space.
**
==============================================================================*/
   ULONG                            /* count bytes formatted
*/
   APIENTRY    DrwFormatASCIILine
(
   PVOID       pvBuf                /* buffer to receive formatted text       */
,  PVOID       pvData               /* address of data buffer to format       */
,  PULONG      pcbData              /* size of data buffer (in bytes)         */
)
{
/*
** Assign and calculate buffer pointers for formatting operation...
*/
   PCHAR       pchBuf      = (PCHAR) pvBuf;
   PBYTE       pbData      = (PBYTE) pvData;
   PBYTE       pbDataEnd   = (PBYTE) pbData + *pcbData;
/*
** Parse though data buffer one character at a time until the end-of-the-buffer
** is reached.
*/
   while (pbData < pbDataEnd)
   {
/*
** Copy printable chars, use a '.' for non-printable chars
*/
      if (isprint( *pbData ))
      {
         *pchBuf = *pbData;
      }
      else
      {
         *pchBuf = '.';
      }
/*
**    Skip to next character in buffers...
*/
      pchBuf++;
      pbData++;
   }
/*
** Append a space and increment pointer...
*/
   *pchBuf = ' ';
   pchBuf++;
/*
** Update number of bytes copied from pvData...
*/
   *pcbData = pbData - (PCHAR)pvData;
/*
** Return number of characters copied to formatted buffer...
*/
   return (pchBuf - ((PCHAR)pvBuf));
}
/*==============================================================================
**
** Function:   DrwFormatHexLine
**
** Usage:      Formats a buffer into an hexadecimal text string.
**
** Remarks:    Data can be formatted into groups of any size.  For example,
**             one (BYTE), two (USHORT) or four (ULONG and pointers).  Each
**             group is converted in as little endian (Intel).  The string is
**             not null (0x00) terminated on return and the last last character
**             is always a space.
**
==============================================================================*/
   ULONG                            /* count bytes formatted                  */
   APIENTRY    DrwFormatHexLine
(
   PVOID       pvBuf                /* buffer to receive formatted text       */
,  PVOID       pvData               /* address of data buffer to format       */
,  PULONG      pcbData              /* size of data buffer (in bytes)         */
,  ULONG       cbFormat             /* group size; 1,2 or 4...                */
)
{
/*
** Assign and calculate buffer pointers for formatting operation...
*/
   PCHAR       pchBuf      = (PCHAR) pvBuf;
   PBYTE       pbData      = (PBYTE) pvData;
   PBYTE       pbDataEnd   = (PBYTE) pbData + *pcbData;
   PBYTE       pbFormat;
/*
** Round *pcbData down to a multiple of cbFormat
*/
   *pcbData /= cbFormat;
   *pcbData *= cbFormat;
/*
** Parse though data buffer one character at a time until the end-of-the-buffer
** is reached.
*/
   while (pbData < pbDataEnd)
   {
/*
**    Format 'cbFormat' bytes of data, working from high to low since we are
**    little endian...
*/
      pbFormat = pbData + (cbFormat - 1);
/*
**    Now, for each element in the group, convert its value into its
**    hexadecimal equivalent and place it into the output buffer...
*/
      while (pbFormat >= pbData)
      {
         *pchBuf = ACH_HEXDIGIT[*pbFormat >> 4];
         *pchBuf++;
         *pchBuf = ACH_HEXDIGIT[*pbFormat & 0x0f];
         *pchBuf++;

         pbFormat--;
      }
/*
**    Append a space and move onto next 'cbFormat' block of bytes
*/
      *pchBuf = ' ';
      pchBuf++;
      pbData += cbFormat;
   }
/*
** Update number of bytes copied from pvData and return number of chars copied
** to pvBuf...
*/
   *pcbData = pbData - (PBYTE)pvData;
/*
** Return number of characters copied to formatted buffer...
*/
   return (pchBuf - ((PCHAR)pvBuf));
}
/*==============================================================================
**
** Function:   DrwSetScrollBars
**
** Usage:      Set Horizontal and Vertical scroll bar attributes to represent
**             data being displayed.
**
==============================================================================*/
   BOOL                             /* fResult: Error indicator               */
   APIENTRY    DrwSetScrollBars
(
   HWND        hwndVertScroll       /* handle vertical scroll-bar window      */
,  HWND        hwndHorzScroll       /* handle horizontal scroll-bar window    */
,  USHORT      usXPos               /* horizontal scroll-bar position         */
,  USHORT      usYPos               /* vertical scroll-bar position           */
,  ULONG       cxLine               /* width of a character (in pels)         */
,  ULONG       cyLine               /* height of line (in pels)               */
,  ULONG       cxWindow             /* width of window (in pels)              */
,  ULONG       cyWindow             /* height of window (in pels)             */
,  ULONG       cxData               /* maximum width of data (in pels)        */
,  ULONG       cyData               /* maximum height of date (in pels)       */
)
{
   BOOL        fResult;
/*
** Set vertical 'Thumb' slider to represent the amount of data displayed in
** relationship to the amount of data available...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndVertScroll
            , SBM_SETTHUMBSIZE
            , MPFROM2SHORT (cyWindow / cyLine, cyData / cyLine)
            , MPVOID
            );
/*
** Set the 'current' position and 'range' of vertical scroll-bar...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndVertScroll
            , SBM_SETSCROLLBAR
            , MPFROMSHORT  (usYPos)
            , MPFROM2SHORT (0, (cyData - cyWindow) / cyLine)
            );
/*
** Set horizontal 'Thumb' slider to represent the amount of data displayed in
** relationship to the amount of data available.
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndHorzScroll
            , SBM_SETTHUMBSIZE
            , MPFROM2SHORT (cxWindow / cxLine, cxData / cxLine)
            , MPVOID
            );
/*
** Set the 'current' position and 'range' of horizontal scroll-bar...
*/
   fResult  = (BOOL) WinSendMsg
            ( hwndHorzScroll
            , SBM_SETSCROLLBAR
            , MPFROMSHORT  (usXPos)
            , MPFROM2SHORT (0, (cxData - cxWindow) / cxLine)
            );

   return (TRUE);
}
