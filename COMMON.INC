#-------------------------------------------------------------------------------
#   Sample application makefile,common definitions for the IBM C
#   compiler environment
#-------------------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .rc .res .obj .lst .c .asm .hlp .itl .ipf
#-------------------------------------------------------------------------------
# Default compilation macros for sample programs
#-------------------------------------------------------------------------------

BASE    = ..

CC      = icc
CINC    = /I. /I$(BASE) /I%%INCLUDE%%
CFLAGS  = /c /Gd- /Se /Re /Ss /Ms /Ti /Xi+

LFLAGS  = /CO /NOE /NOD /ALIGN:16 /EXEPACK /M:FULL /PM:PM /BASE:0x10000
LINK    = LINK386  $(LFLAGS)
LIBS    = DDE4MBS + OS2386

.c.lst:
    $(CC) $(CFLAGS) -Fc$*.lst -Fo$*.obj $*.c

.c.obj:
    $(CC) $CFLAGS) -Fo$*.obj $*.c

.ipf.hlp:
        ipfc $*.ipf /W3

.itl.hlp:
        cc  -P $*.itl
        ipfc $*.i
        del $*.i

.rc.res:
        rc -r -p -x $*.rc
