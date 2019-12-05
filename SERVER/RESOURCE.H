/*==============================================================================
**
** Module:     Resource.h
**
** Remarks:    This module contains application resource definitions.
**
**============================================================================*/
   #ifndef RESOURCE_INCLUDED
   #define RESOURCE_INCLUDED
/*
** Resource Id's (ID Resource)
*/
   #define IDR_MAIN                       1
   #define IDR_MAIN_MENU_SYSFORMATS       2
   #define IDR_MAIN_MENU_SYSITEMFORMATS   3
   #define IDR_MAIN_MENU_SYSPROTOCOLS     4
/*
** Dialog Id's (ID of Dialog)
*/
   #define IDD_ABOUT                   2000
   #define IDD_ITEM                    3000
   #define IDD_TERMINATE               5000
   #define IDD_STRUCT                  6000
/*
** Dialog Control Id's (ID Control)
*/
   #define IDC_NULL                   65535

   #define IDC_ABOUT_ICON              2001
   #define IDC_ABOUT_CANCEL            2002

   #define IDC_ITEM_DATA               3001
   #define IDC_ITEM_OK                 3095
   #define IDC_ITEM_PASTE_LINK         3096
   #define IDC_ITEM_POST               3097
   #define IDC_ITEM_TERMINATE          3098
   #define IDC_ITEM_CANCEL             3099

   #define IDC_TERMINATE_SERVER        5001
   #define IDC_TERMINATE_TOPIC         5002
   #define IDC_TERMINATE_OK            5003
   #define IDC_TERMINATE_CANCEL        5004

   #define IDC_STRUCT_CB               6001
   #define IDC_STRUCT_STRING1          6002
   #define IDC_STRUCT_STRING2          6003
   #define IDC_STRUCT_FORMAT_ID        6004
   #define IDC_STRUCT_COUNTRY_ID       6005
   #define IDC_STRUCT_CODEPAGE_ID      6006
   #define IDC_STRUCT_LANGUAGE_ID      6007
   #define IDC_STRUCT_SUBLANGUAGE_ID   6008
   #define IDC_STRUCT_APPSTATUS        6009
   #define IDC_STRUCT_FLAGS            6010
   #define IDC_STRUCT_FACK             6011
   #define IDC_STRUCT_FACKREQ          6012
   #define IDC_STRUCT_FBUSY            6013
   #define IDC_STRUCT_FCASESENSITIVE   6014
   #define IDC_STRUCT_FRESPONSE        6015
   #define IDC_STRUCT_FNODATA          6016
   #define IDC_STRUCT_FNOTPROCESSED    6017
   #define IDC_STRUCT_TEXT_STRING1     6018
   #define IDC_STRUCT_TEXT_STRING2     6019
   #define IDC_STRUCT_TEXT_CB          6020
   #define IDC_STRUCT_TEXT_COUNTRY     6021
   #define IDC_STRUCT_TEXT_CODEPAGE    6022
   #define IDC_STRUCT_TEXT_SUBLANG     6023
   #define IDC_STRUCT_TEXT_LANGUAGE    6024
   #define IDC_STRUCT_TEXT_FORMAT      6025
   #define IDC_STRUCT_TEXT_FLAGS       6026
/*
** Menu Item Id's (ID Menu item)
*/
   #define IDM_NULL                      -1
   #define IDM_INCREMENT                 10

   #define IDM_LINKS                    100
   #define IDM_LINKS_TERMINATE_ALL      101

   #define IDM_TOPICS_FIRST             200
   #define IDM_TOPICS                   IDM_TOPICS_FIRST - 1           /* 199    */
   #define IDM_TOPICS_LAST              290

   #define IDM_STATUS                   300
   #define IDM_STATUS_FIRST             300
   #define IDM_STATUS_READY             301
   #define IDM_STATUS_BUSY              302
   #define IDM_STATUS_SECURITY          303
   #define IDM_STATUS_LAST              399

   #define IDM_DISPLAY                  400
   #define IDM_DISPLAY_AUTO             401
   #define IDM_DISPLAY_CLEAR            402
   #define IDM_DISPLAY_IMAGE            403
   #define IDM_DISPLAY_BINARY           404

   #define IDM_HELP                     600
   #define IDM_HELP_ABOUT               601
/*
** String Id's (ID String)
*/
   #define IDS_APP_NAME                  50
   #define IDS_APP_TERMINATING           51
   #define IDS_DDE_POSITIVE_ACK          52
   #define IDS_DDE_NEGATIVE_ACK          53
   #define IDS_DDE_FORMAT_NOT_SUPPORTED  54
   #define IDS_DDE_NO_SERVERS            55
   #define IDS_DDE_BUSY                  56
   #define IDS_DDE_NOTPROCESSED          57
   #define IDS_DDE_NODATA                58
   #define IDS_DDE_RESPONSE              59
   #define IDS_DDE_APPSTATUS             60
   #define IDS_DDE_GENERAL               61
   #define IDS_NO_MEM_FOR_DDESTRUCT      62
   #define IDS_CANNOT_FREE_USERDATA      63
   #define IDS_INIT_FORMATS_ERROR        64
   #define IDS_DDE_INITIATE_FAILED       65
   #define IDS_API_FAILED                66
   #define IDS_CLOCK                     67
   #define IDS_TIME                      68
   #define IDS_DDESYS_ITEM_RTNMSG        69
   #define IDS_DDESYS_ITEM_RESTART       70
   #define IDS_DDESYS_ITEM_STATUS        71
   #define IDS_DDE_FORMAT_TIME           72
   #define IDS_DDE_FORMAT_TEXT           73
   #define IDS_DDE_FORMAT_CPTEXT         74
   #define IDS_DDE_FORMAT_BITMAP         75
   #define IDS_DDE_FORMAT_METAFILE       76
   #define IDS_DDE_FORMAT_PALETTE        77
   #define IDS_DDE_FORMAT_LINK           78
   #define IDS_DDE_FORMAT_NONE           79
   #define IDS_READY                     80
   #define IDS_BUSY                      81

   #endif
