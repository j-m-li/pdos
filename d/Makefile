# Produce Windows executables
# links with PDPCLIB created by makefile.std

CC=sccwin
LD=pdld
LDFLAGS=--no-insert-timestamp
AS=pdas
AR=xar

all: clean edit.exe

edit.exe: main.obj edit.obj tk.obj tk_text.obj tk_inline.obj tk_block.obj tk_range.obj tk_style.obj
  $(AR) s temp.lib
  $(LD) $(LDFLAGS) -s -o edit.exe ../devel/pdos/pdpclib/w32start.obj temp.lib ../devel/pdos/pdpclib/msvcrt.lib ../devel/pdos/pdpclib/winsupa.obj


.c.obj:
	pdcc -E -D__SUBC__ -D__WIN32__ -Dunsigned= -Dlong=int -Dshort=int -Dconst= -Ddouble=int -I ../devel/pdos/pdpclib -I . -o $*.i $<
  $(CC) -S $*.i
  rm -f $*.i
  $(AS) -o $@ --oformat coff $*.s
  rm -f $*.s
  $(AR) r temp.lib $*.obj

clean:
  rm -f *.obj
  rm -f edit.exe
  rm -f temp.lib

