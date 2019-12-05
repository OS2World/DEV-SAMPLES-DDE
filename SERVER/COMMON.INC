#===================================================================
#
#   Sample application makefile,common definitions for the IBM C
#   compiler environment
#===================================================================
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#===================================================================
# Default compilation macros for sample programs
#
# Compile switchs  that are enabled
# /c      compile don't link
# /Ss     allow  "//" for comment lines
# /Ms     use the system calling convention and not optilink as the default
# /Gd-    Disable optimization
# /Se     allow cset extensions
#

CC         = icc /c /Gd- /Se /Re /Ss /Ms /Ti
#CC        = icc /c /Gd- /Se /Re /Ss /Fl /Ls /Lx /La /Ti /Fm /Ms

!ifdef DEBUG
CC         = icc /c /Gd- /Se /Re /Ss /Ms /Ti -DDEBUG
!endif

AFLAGS  = /Mx -t -z
ASM     = ml /c /Zm
#LFLAGS   = /CO /LI /NOE /NOD /ALIGN:16 /EXEPACK /M:FULL /PM:PM /BASE:0x10000
LFLAGS   = /CO /NOE /NOD /ALIGN:16 /EXEPACK /M:FULL /PM:PM /BASE:0x10000
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386
STLIBS  = DDE4SBS + OS2386
MTLIBS  = DDE4MBS + os2386
DLLLIBS = DDE4NBS + os2386
VLIBS   = DDE4SBS + vdh + os2386

.c.lst:
    $(CC) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    $(CC) -Fo$*.obj $*.c

.asm.obj:
    $(ASM)   $*.asm

.ipf.hlp:
        ipfc $*.ipf /W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r -p -x $*.rc
