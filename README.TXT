The example programs included on this diskette are provided for the sole
purpose of illustrating one possible implementation of the DDE protocol.  These
examples have not been thoroughly tested under all conditions. Therefore, the
reliability, serviceability, or function of these programs is not guaranteed.

All programs contained herein are provided to you "AS IS".  THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
EXPRESSLY DISCLAIMED.

The primary objective of the example programs is to show how DDE could be
implemented in both a Server and Client application.  In addition, I wanted the
example programs to be useful.  Both example applications can communicate with
any DDE capabile application, not just with each other.  The diagnostic (spy)
capabilities of these example programs allow them to be used as test and
debugging tools to aid in the development of their own DDE applications.

Another objective in the development of the example programs was to provide
code that was easy to understand.  Quick or optimized code was sacrificed for
clarity.  Also, they had to fully implement the DDE protocol and as many
formats as possible.

Much effort was used to utilize the existing resources of the OS/2 operating
system and Presentation Manager (PM).  System resources where used whenever
possible rather than creating obscure routines to support application specific
structures.  For example, rather than creating routines and structures to
maintain link-lists of information, the hierarchial architecture of PM windows
was exploited instead. Additional uses of system resources are as follows:

-    Window instance data (also called window words) is used to maintain
     information associated with a particular instance of an object.  For
     example, when windows are utilized as liked-lists, the instance data will
     contain the data associated with that particular element of the list.

-    WC_MENU class windows where utilized to store DDE data items that
     contained lists of text.  This allowed for the use of existing API's and
     messages in the management of these items.  In addition, menus could be
     pre-defined in resource files therefore eliminating the need for complex
     initialization code.


For the most part, DDE functionality has been implemented via window procedures
associated with a set of private window classes.  Both the "Server" and "Client"
applications create windows of these classes to support various DDE
functionality.  The major difference between the "Server" and "Client" example
program is the method in which these window classes are used.  The following
diagram illustrates the basic make-up of the "Server" and "Client" applications.



           DDE CLIENT                                 DDE SERVER
-------------------------------------    --------------------------------------

         |-----------|                               |-----------|
         | WC_Client |                               | WC_Client |
         |-----------|                               |-----------|
               |                                           |
       ------------------                        -------------------
       |                |                        |                 |
|------------|   |------------|           |-------------|   |-------------|
| WC_DdeConv |   | WC_DdeConv |           | WC_DdeTopic |   | WC_DdeTopic |
|------------|   |------------|           |-------------|   |-------------|
                        |                                          |
              ------------------                      -------------------
              |                |                      |                 |
       |-------------|  |-------------|        |-------------|   |-------------|
       | WC_DdeTopic |  | WC_DdeTopic |        | WC_DdeItem  |   | WC_DdeConv  |
       |-------------|  |-------------|        |-------------|   |-------------|
                                                      |
                                                -------------------
                                                |                 |
                                         |--------------|  |--------------|
                                         | WC_DdeAdvise |  | WC_DdeAdvise |
                                         |--------------|  |--------------|



WC_DdeConv

     This class handles DDE conversations.  One window of this class is created
     for each conversation the client or the server are engaged in.  In this
     class, specific information regarding a conversation-link is maintained,
     such as:

         The current state of the conversation.
         The window handle of its conversation partner.
         The Application and Topic name-strings of the conversation.
         Conversation context information (as specified in a CONVCONTEXT
          structure).


WC_DdeTopic

     This class handles DDE topics.  One window of this class-style is created
     for each topic a server application supports.  Also, the client application
     creates one window of this class-style for each topic it has engaged into
     a conversation-link with.

     In this class, specific information regarding a topic is maintained, such
     as:

         Where to find the topic name-string, and the name-strings of each
          item supported in the topic.  The example programs use a WC_MENU
          class window to maintain this information.
         A count of how many conversations this topic is engaged in.


WC_DdeItem

     This class handles DDE items.  One window of this class-style is created
     for each item supported by a WC_DdeTopic class window.  In this class,
     specific information regarding a DDE item is maintained, such as:

         A window handle of a WC_MENU class window.  The item name-string
          associated with this item is stored in a WC_MENU class window.
         A handle to an item specific object.  For example, a pointer to text,
          the handle of another WC_MENU class window (to support items
          containing a list of string), or Time.


WC_DdeAdvise

     This class handles permanent DDE data-links.  One window of this
     class-style is created for each DDE format and conversation pair
     requested by a WM_DDE_ADVISE transaction.  In this class, specific
     information regarding a data-link is maintained, such as:

         The DDE Status Flags of the original WM_DDE_ADVISE request.
         If a WM_DDE_ACK message is pending (in the case where DDE_FACKREQ was
          specified in the original request).
         The window handle of the original requesting (client) application.
          This window handle is used to posting WM_DDE_DATA messages.


General (and common) application information.

   When first executed, the user is presented with a frame with a white
   "Client" window to the left, a white "status" window along the bottom, a
   listbox in the upper right corner and a (read-only) dialog window in the
   lower right corner.

   The "Client" (FID_CLIENT) window is used to display DDE data items in either
   "Image" (text, bitmap, metafile, etc...) or "Binary" (hexadecimal) form. The
   "Status" (FID_Status) window is used to display descriptive messages to the
   user.  The "Listbox" (FID_Listbox) is used to retain each DDE message
   received for subsequent review and examination.  The "Dialog" window is used
   to display information corresponding to the message currently selected in
   the listbox.  The "Menu" of the DDE Client application is shown below with
   a brief description of each menu-item.


The DDE Client application

   Designed to communicate with any DDE capable server, it can engage in one
   DDE conversation per Server/Topic pair.  For example, if a server
   application supported 5 topics, the client could have up to 5 active
   conversations.

   Menu

     Links
       Codepages         Set active code page (only if no active-links)
       Enumerate         List all active servers
       Initiate...       Issue DDE initiate request
       Paste Special     Issue DDE initiate request (specified in LINK format)
       Terminate All     Issue terminate request for all active-links

     Servers
       [Name] [Topic]    One entry per application/topic pair. This can vary
              [Topic]    between invocations of "Enumerate", "Initiate" or
                         "Paste Special" menu-items.

     Display
       Auto              Automatically update display upon receipt of message
       Clear             Clear all windows (and free associated resources)
       Image             Display data item in image format (ie bitmap)
       Binary            Display data item in binary form.


   Establishing a DDE conversation

      Known server

         If the user knows then name of the application and topic, they can
         select the "Initiate" menu-item.  A dialog window will be displayed
         allowing the user to enter the desired application and topic names.

      Unknown server

         If the user does not know the name of the application or even what
         DDE servers are active, they can select the "Enumerate" menu-item.
         This item will enumerate (broadcast a WM_DDE_INITIATE message to) all
         active DDE servers.

      It both cases, a menu-entry is dynamically created under the "Servers"
      menu for each responding application.  Furthermore, an entry is created
      for each Topic supported by the responding application.  Initially, when
      these menu-items are selected, the "Initiate" dialog is displayed showing
      the application and topic names just selected.  If the user selects the
      "Initiate" button, a DDE initiation request will be issued.  If
      successful, the corresponding menu-items will be "Checked" indicating
      that it is currently engaged in an active conversation.

      If a menu-item pair is checked and subsequently selected, it is
      interpreted as a request to issue a DDE transaction and the user is shown
      an "Item..." dialog window.  From this dialog, any DDE transaction can
      be issued.  DDE transactions are issued to the DDE server identified by
      the application/topic menu-item pair.  In addition, the user can choose
      to "Terminate" the DDE conversation from this dialog.

      If the user double-clicks on the system-menu, all DDE conversations are
      automatically terminated.

   Paste Link

      If enabled, a "LINK" format exists on the PM clipboard.  When selected
      the client application automatically establishes a DDE conversation with
      the application and topic specified in the format.  Additionally, a
      dialog is shown indicating the name of the item specified in the format.
      At this point, the user can issue any DDE transaction as if had manually
      established a conversation-link as described above.


DDE Server

   Designed to communicate with any DDE capable Client, it can engage in
   multiple DDE conversations for each of its supported topic.

   Menu

     Links
       Terminate All     Issue terminate request for all active-links

     Topics
       [Name] [Item]     One entry per data-item supported.
              [Item]

         When selected, a dialog is displayed showing the current value of the
         item.  From this dialog the user can cause to server to act as if the
         data-item has changed.  This allows the user to test permanent
         data-links resulting in a WM_DDE_DATA message to be posted.  Also,
         by selecting the "Paste Link" button, a "LINK" format containing the
         application/topic/item combination identifying the current selection.

     Status
       Ready             If checked, indicates server is "Ready"
       Busy              If checked, indicates server is "Busy"

     Display
       Auto              Automatically update display upon receipt of message
       Clear             Clear all windows (and free associated resources)
       Image             Display data item in image format (ie bitmap)
       Binary            Display data item in binary form.


   Terminating DDE conversations

      Individual DDE conversation-links cannot be terminated.  All DDE
      conversation-links are terminated when the application terminates or
      the "Terminate All" menu-item is selected.


Coding Style

  The coding style used in the example programs is a style I have grown to like
  over the years.  It allows me to make a comment on each and every line of
  code.  In addition, it allows me to get what I call syntax 'noise' (ie commas,
  parenthesis and braces) out of the way and makes the actual code easier to
  read (at least to me).  To the traditional C-programmer the syntax looks
  strange at first, but once you get used to it (like hungarian-notation)
  programs (in my opinion) are be easier to read


Other neat stuff

  The example programs also exhibit other PM programming techniques of interest
  to developers, such as:

        - Adding custom Frame controls
        - Restricting the size of the Tracking rectangle
        - Window sub-classing
        - Dynamic Menus
        - Gpi Transforms
        - Metafiles
        - Bitmaps
        - NLS support
        - Timers
        - Exist lists
