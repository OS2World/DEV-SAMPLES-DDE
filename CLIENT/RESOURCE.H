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
/*
** Dialog Id's (ID of Dialog)
*/
   #define IDD_ABOUT                   2000
   #define IDD_ITEM                    3000
   #define IDD_INITIATE                4000
   #define IDD_TERMINATE               5000
   #define IDD_STRUCT                  6000
/*
** Dialog Control Id's (ID Control)
*/
   #define IDC_NULL                   65535

   #define IDC_ABOUT_ICON              2001
   #define IDC_ABOUT_OK                2002
   #define IDC_ABOUT_CANCEL            2005

   #define IDC_ITEM_FORMATS_FIRST      3010
   #define IDC_ITEM_TEXT               3011
   #define IDC_ITEM_CPTEXT             3012
   #define IDC_ITEM_BITMAP             3013
   #define IDC_ITEM_METAFILE           3014
   #define IDC_ITEM_PALETTE            3015
   #define IDC_ITEM_PRIVATE            3016
   #define IDC_ITEM_NONE               3017
   #define IDC_ITEM_FORMATS_LAST       3019
   #define IDC_ITEM_MESSAGES_FIRST     3020
   #define IDC_ITEM_REQUEST            3021
   #define IDC_ITEM_ADVISE             3022
   #define IDC_ITEM_UNADVISE           3023
   #define IDC_ITEM_POKE               3024
   #define IDC_ITEM_EXECUTE            3025
   #define IDC_ITEM_MESSAGES_LAST      3029

   #define IDC_ITEM_FACKREQ            3030
   #define IDC_ITEM_FNODATA            3031
   #define IDC_ITEM_NAME               3032
   #define IDC_ITEM_DATA               3033
   #define IDC_ITEM_FORMAT             3034
   #define IDC_ITEM_TEXT_FORMAT        3035
   #define IDC_ITEM_TEXT_DATA          3036

   #define IDC_ITEM_OK                 3095
   #define IDC_ITEM_PASTE_LINK         3096
   #define IDC_ITEM_TERMINATE          3098
   #define IDC_ITEM_CANCEL             3099

   #define IDC_INITIATE_SERVER         4001
   #define IDC_INITIATE_TOPIC          4002
   #define IDC_INITIATE_OK             4003
   #define IDC_INITIATE_CANCEL         4004

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
   #define IDM_INCREMENT                 10

   #define IDM_LINKS                    100
   #define IDM_LINKS_CODEPAGES          110
   #define IDM_LINKS_CODEPAGES_FIRST    110
   #define IDM_LINKS_CODEPAGES_LAST     119
   #define IDM_LINKS_ENUMERATE          120
   #define IDM_LINKS_PASTE_LINK         130
   #define IDM_LINKS_INITIATE           140
   #define IDM_LINKS_TERMINATE_ALL      150

   #define IDM_SERVERS_FIRST            200
   #define IDM_SERVERS                  IDM_SERVERS_FIRST - 1
   #define IDM_SERVERS_LAST             290
   #define IDM_TOPICS_FIRST             IDM_SERVERS_FIRST + 1
   #define IDM_TOPICS_LAST              IDM_SERVERS_LAST  + MAX_TOPICS

   #define IDM_DISPLAY                  400
   #define IDM_DISPLAY_AUTO             401
   #define IDM_DISPLAY_CLEAR            402
   #define IDM_DISPLAY_IMAGE            403
   #define IDM_DISPLAY_BINARY           404

   #define IDM_HELP                     900
   #define IDM_HELP_ABOUT               901
/*
** String Id's (ID String)
*/
   #define IDS_API_FAILED                16
   #define IDS_APP_NAME                  17
   #define IDS_DDE_POSITIVE_ACK          18
   #define IDS_DDE_NEGATIVE_ACK          19
   #define IDS_DDE_FORMAT_NOT_REGISTERED 20
   #define IDS_DDE_NODATA                21
   #define IDS_LINK_ALREADY_ACTIVE       22
   #define IDS_NO_SERVER_RESPONDED       23
   #define IDS_DDE_FORMAT_TIME           24
   #define IDS_DDE_FORMAT_TEXT           25
   #define IDS_DDE_FORMAT_CPTEXT         26
   #define IDS_DDE_FORMAT_BITMAP         27
   #define IDS_DDE_FORMAT_METAFILE       28
   #define IDS_DDE_FORMAT_PALETTE        29
   #define IDS_DDE_FORMAT_LINK           30
   #define IDS_DDE_FORMAT_NONE           31

#endif
