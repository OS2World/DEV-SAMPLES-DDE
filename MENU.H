/*==============================================================================
**
** Module:     Menu.h
**
** Remarks:    This module contains definitions used to implement common
**             'Menu' window behaviour.
**
**============================================================================*/
   #ifndef MENU_INCLUDED
   #define MENU_INCLUDED

   #define MAX_ITEM_TEXT_LENGTH     32
/*
** An inclusive MIA (Menu Item Attribute) flag used to purge a menu-item
** having any of the following attributes:
*/
   #define MIA_ANY   ( MIA_NODISMISS     \
                     | MIA_FRAMED        \
                     | MIA_CHECKED       \
                     | MIA_DISABLED      \
                     | MIA_HILITED)
/*
** tagMENUTEMPLATE
**
** This structure defines a menu structure:
*/
   typedef struct tagMENUTEMPLATE
   {
      USHORT      cbTemplate;
      USHORT      usVersion;
      USHORT      usCodepage;
      USHORT      offami;
      USHORT      cMenuItems;
      USHORT      coffPresParams;
      char        ami[1];
   } MENUTEMPLATE
   ,*PMENUTEMPLATE;
/*
** Function prototypes.
*/
   SHORT  APIENTRY MnuAddItem (HWND, PMENUITEM, PSZ);
   SHORT  APIENTRY MnuDeleteItem (HWND, USHORT, BOOL);
   SHORT  APIENTRY MnuLookUpItemText (HWND, PSZ, BOOL);
   BOOL   APIENTRY MnuPurgeItems (HWND, USHORT);
   BOOL   APIENTRY MnuQueryItemsAttr (HWND, USHORT);
   SHORT  APIENTRY MnuQueryItemCount (HWND);
   USHORT APIENTRY MnuQueryItemText (HWND, USHORT, PSZ);
   USHORT APIENTRY MnuQueryItemTextLength (HWND, USHORT);
   HWND   APIENTRY MnuQuerySubMenu (HWND, USHORT);

#endif
