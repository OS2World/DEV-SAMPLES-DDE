/*==============================================================================
**
** Module:     Menu.c
**
** Remarks:    This module contains routines that facilitate Menu manipulation.
**
**============================================================================*/
/*
** System specific defines:
*/
   #define     INCL_DOS
   #define     INCL_WIN
/*
** System specific include files:
*/
   #include    <os2.h>
   #include    <string.h>
/*
** Application specific include files:
*/
   #include    <main.h>
   #include    <menu.h>
   #include    <common.h>

/*==============================================================================
**
** Function:   MnuDeleteItem
**
** Usage:      Deletes an Item from a menu (or sub-menu).
**
==============================================================================*/
   SHORT                            /* number of remaining menu items         */
   APIENTRY    MnuDeleteItem
(
   HWND        hwndMenu             /* base menu window handle                */
,  USHORT      usItemId             /* identifier of menu item to delete      */
,  BOOL        fIncludeSubMenus     /* sub-menu search, TRUE=yes, FALSE=no    */
)
{
   SHORT       cItems;
/*
** Delete the specified menu item...
*/
   cItems   = (SHORT) WinSendMsg
            ( hwndMenu
            , MM_DELETEITEM
            , MPFROM2SHORT (usItemId, (fIncludeSubMenus) ? TRUE : FALSE)
            , MPVOID
            );
/*
** Return the number of menu items left after the item is deleted...
*/
   return (cItems);
}
/*==============================================================================
**
** Function:   MnuQuerySubMenu
**
** Usage:      Queries the sub-menu window handle associated with a menu item.
**
==============================================================================*/
   HWND                             /* Sub-menu window handle : NULL          */
   APIENTRY    MnuQuerySubMenu
(
   HWND        hwndMenu             /* base menu window handle                */
,  USHORT      usItemId             /* identifier of menu item to query       */
)
{
   MENUITEM    mi;
/*
** Obtain menu item information...
*/
   BOOL        fResult  = (BOOL) WinSendMsg
                        ( hwndMenu
                        , MM_QUERYITEM
                        , MPFROM2SHORT (usItemId, TRUE)
                        , MPFROMP (&mi)
                        );
/*
** Return the window handle specified in the menu item structure...
*/
   return (fResult) ? mi.hwndSubMenu : (HWND) NULL;
}
/*==============================================================================
**
** Function:   MnuPurgeItems
**
** Usage:      Purges all menu items from a menu.
**
** Remarks:    All items of the specified menu are purged that do not have
**             the attributes specified in 'fsIgnoreAttrMask'.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    MnuPurgeItems
(
   HWND        hwndMenu             /* base menu window handle                */
,  USHORT      fsIgnoreAttrMask     /* item exclusion attributes              */
)
{
   USHORT      cItems       = 0;
   USHORT      sItemIndex   = 0;
   USHORT      usItemId     = 0;
   USHORT      fsState      = 0;
/*
** For each menu item...
*/
   do
   {
/*
**    Obtain the next menu item identifier...
*/
      usItemId = (USHORT) WinSendMsg
               ( hwndMenu
               , MM_ITEMIDFROMPOSITION
               , MPFROMSHORT (sItemIndex)
               , MPVOID
               );
/*
**    If menu item exists...
*/
      if (usItemId != (USHORT) MIT_ERROR)
      {
/*
**       Check if this menu item has any of the attributes specified in the
**       'fsIgnoreAttrMask'.  Such menu items are ignored from the purge.
*/
         fsState  = (USHORT) WinSendMsg
                  ( hwndMenu
                  , MM_QUERYITEMATTR
                  , MPFROM2SHORT (usItemId, TRUE)
                  , MPFROMSHORT  (fsIgnoreAttrMask)
                  );
/*
**       If the menu item does not contain any of the attributes that would
**       cause it to be ignored, delete it...
*/
         if (!fsState)
         {
            cItems   = (USHORT) WinSendMsg
                     ( hwndMenu
                     , MM_DELETEITEM
                     , MPFROM2SHORT (usItemId, TRUE)
                     , MPVOID
                     );
         }
         else
         {
/*
**       Skip past this item...
*/
            sItemIndex++;
         }
      }
   } while (usItemId != (USHORT) MIT_ERROR);
/*
** Return successful indicator...
*/
   return (TRUE);
}
/*==============================================================================
**
** Function:   MnuAddItem
**
** Usage:      Adds a menu item to a menu.
**
==============================================================================*/
   SHORT                            /* Item index : MIT_ERROR, MIT_MEMERROR   */
   APIENTRY    MnuAddItem
(
   HWND        hwndMenu             /* base menu window handle                */
,  PMENUITEM   pMenuItem            /* address of MENUITEM structure          */
,  PSZ         pszItemText          /* menu item text                         */
)
{
   USHORT      sItemIndex  = 0;
   PSZ         pszNull     =  "";
/*
** If a null pointer was specified for Item text, then adjust pointer to point
** to an empty string...
*/
   if (pszItemText == NULL)
   {
      pszItemText = pszNull;
   }
/*
** Insert new item...
*/
   sItemIndex  = (SHORT) WinSendMsg
               ( hwndMenu
               , MM_INSERTITEM
               , MPFROMP (pMenuItem)
               , MPFROMP (pszItemText)
               );
/*
** Return index of new item...
*/
   return (sItemIndex);
}
/*==============================================================================
**
** Function:   MnuQueryItemCount
**
** Usage:      Queries the number of items in a menu.
**
==============================================================================*/
   SHORT                            /* Number of items in a menu              */
   APIENTRY    MnuQueryItemCount
(
   HWND        hwndMenu             /* base menu window handle                */
)
{
  return (SHORT) WinSendMsg
               ( hwndMenu           /* Menu handle request is for             */
               , MM_QUERYITEMCOUNT  /* Message ID                             */
               , MPVOID             /* Reserved, must be zero (0)             */
               , MPVOID             /* Reserved, must be zero (0)             */
               );
}
/*==============================================================================
**
** Function:   MnuQueryItemText
**
** Usage:      Queries text of specified menu item.
**
** Remarks:    The length returned does NOT include the terminating null
**             character.
**
==============================================================================*/
   USHORT                           /* length of text (bytes): 0              */
   APIENTRY    MnuQueryItemText
(
   HWND        hwndMenu             /* base menu window handle                */
,  USHORT      usItemId             /* identifier of menu item to query       */
,  PSZ         pszItemText          /* address of buffer to copy text         */
)
{
   SHORT       cbItemText  = (SHORT) WinSendMsg
                           ( hwndMenu
                           , MM_QUERYITEMTEXT
                           , MPFROM2SHORT (usItemId, MAX_ITEM_TEXT_LENGTH)
                           , pszItemText
                           );
/*
** Return size of text (in bytes)...
*/
   return (cbItemText);
}
/*==============================================================================
**
** Function:   MnuQueryItemTextLength
**
** Usage:      Queries text length (in bytes) of specified menu item.
**
** Remarks:    The length returned does NOT include the terminating null
**             character.
**
==============================================================================*/
   USHORT                           /* length of text (bytes): 0              */
   APIENTRY    MnuQueryItemTextLength
(
   HWND        hwndMenu             /* base menu window handle                */
,  USHORT      usItemId             /* identifier of menu item to query       */
)
{
   SHORT       cbItemText  = (SHORT) WinSendMsg
                           ( hwndMenu
                           , MM_QUERYITEMTEXTLENGTH
                           , MPFROMSHORT (usItemId)
                           , MPVOID
                           );
/*
** Return size of text (in bytes)...
*/
   return (cbItemText);
}
/*==============================================================================
**
** Function:   MnuLookUpItemText
**
** Usage:      Search menu for item matching specified text.
**
==============================================================================*/
   SHORT                            /* Item identifier: MIT_ERROR             */
   APIENTRY    MnuLookUpItemText
(
   HWND        hwndMenu             /* base menu window handle                */
,  PSZ         pszItemText
,  BOOL        fCaseSensitive
)
{
   USHORT      cbItemText   = strlen (pszItemText);
   USHORT      cbMenuText;
   MENUITEM    mi;
   SHORT       sItemId;
   SHORT       sItemIndex   = 0;
   CHAR        szMenuText   [MAX_ITEM_TEXT_LENGTH];
/*
** For each item in a menu...
*/
   do
   {
/*
**    Get item identifier of next item in menu...
*/
      sItemId  = (SHORT) WinSendMsg
               ( hwndMenu
               , MM_ITEMIDFROMPOSITION
               , MPFROMSHORT (sItemIndex)
               , MPVOID
               );
/*
**    If more items...
*/
      if (sItemId != MIT_ERROR)
      {
/*
**       Obtain its corresponding text...
*/
         cbMenuText  = (SHORT) WinSendMsg
                     ( hwndMenu
                     , MM_QUERYITEMTEXT
                     , MPFROM2SHORT (sItemId, MAX_ITEM_TEXT_LENGTH)
                     , MPFROMP (szMenuText)
                     );
/*
**       If text obtained...
*/
         if (cbMenuText)
         {
/*
**          Optimization: continue only if the corresponding text length is
**                      equal to what is specified in the search...
*/
            if (cbMenuText == cbItemText)
            {
/*
**             If the comparison is case sensitive...
*/
               if (fCaseSensitive)
               {
                  if (0 == strcmp (pszItemText, szMenuText))
                  {
                     return (sItemId);
                  }
               }
               else
               {
                  if (0 == stricmp (pszItemText, szMenuText))
                  {
                     return (sItemId);
                  }
               }
            }
         }
      }
/*
**    Skip to next menu item...
*/
      sItemIndex++;

   } while (sItemId != MIT_ERROR);
/*
** Return menu item identifier...
*/
   return (sItemId);
}
/*==============================================================================
**
** Function:   MnuQueryItemsAttr
**
** Usage:      Queries attributes of an item.
**
==============================================================================*/
   BOOL                             /* fResult: Success indicator             */
   APIENTRY    MnuQueryItemsAttr
(
   HWND        hwndMenu
,  USHORT      fsAttributeMask
)
{
   USHORT      cItems       = 0;
   USHORT      sItemIndex   = 0;
   USHORT      usItemId     = 0;
   USHORT      usState      = 0;

   do
   {
/*
**    Get item identifier of next item in menu...
*/
      usItemId = (USHORT) WinSendMsg
               ( hwndMenu
               , MM_ITEMIDFROMPOSITION
               , MPFROMSHORT (sItemIndex)
               , MPVOID
               );
/*
**    If more items...
*/
      if (usItemId != (USHORT) MIT_ERROR)
      {
/*
**       Obtain its corresponding attributes...
*/
         usState  = (USHORT) WinSendMsg
                  ( hwndMenu
                  , MM_QUERYITEMATTR
                  , MPFROM2SHORT (usItemId, TRUE)
                  , MPFROMSHORT  (fsAttributeMask)
                  );
/*
**       If the item currently has any of the attributes specified in the
**       request...
*/
         if (usState)
         {
            return (TRUE);
         }
/*
**       Skip to next menu item...
*/
         sItemIndex++;
      }
   } while (usItemId != (USHORT) MIT_ERROR);
/*
** Return 'FALSE' indicating no item found...
*/
   return (FALSE);
}
