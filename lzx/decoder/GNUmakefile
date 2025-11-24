TARGETNAME=ldi
TARGETPATH=../../lib
TARGETTYPE=LIBRARY
CC = i686-pc-mingw32-gcc
AR = i686-pc-mingw32-ar
RANLIB = i686-pc-mingw32-ranlib
CFLAGS = -DUSE_NOLIBS=1 -O2 -I../../inc
LDFLAGS = -mdll
INSTALL = install -m 755
INSTALL_DIR = /usr

OBJS = \
    decverb.o \
    decalign.o \
    maketbl.o \
    dectree.o \
    decxlat.o \
    decin.o \
    decuncmp.o \
    decblk.o \
    decout.o \
    decinit.o \
    decapi.o \
    ldi.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETNAME).dll lib$(TARGETNAME).a

$(TARGETNAME).dll: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

lib$(TARGETNAME).a: $(OBJS)
	$(AR) cru $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(OBJS) $(TARGETNAME).dll lib$(TARGETNAME).a

install:
	$(INSTALL) ../../inc/ldi.h $(INSTALL_DIR)/include/ldi.h
	$(INSTALL) $(TARGETNAME).dll $(INSTALL_DIR)/lib/$(TARGETNAME).dll
	$(INSTALL) lib$(TARGETNAME).a $(INSTALL_DIR)/lib/lib$(TARGETNAME).a
