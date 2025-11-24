TARGETNAME=lci
TARGETPATH=../../lib
TARGETTYPE=LIBRARY
CC = i686-pc-mingw32-gcc
DLLWRAP = i686-pc-mingw32-dllwrap
AR = i686-pc-mingw32-ar
RANLIB = i686-pc-mingw32-ranlib
CFLAGS = -DMULTIPLE_SEARCH_TREES=1 -O2 -I../../inc
LDFLAGS = -mdll
INSTALL = install -m 755
INSTALL_DIR = /usr

OBJS = \
    bsearch.o \
    optenc.o \
    encstats.o \
    tree.o \
    encdata.o \
    enctree.o \
    block.o \
    blkinit.o \
    io.o \
    lci.o \
    init.o \
    encapi.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETNAME).dll lib$(TARGETNAME).a

$(TARGETNAME).dll: $(OBJS)
	$(DLLWRAP) --output-def $(TARGETNAME).def --implib lib$(TARGETNAME).dll.a -o $@ $^

lib$(TARGETNAME).a: $(OBJS)
	$(AR) cru $@ $^
	$(RANLIB) $@

clean:
	$(RM) $(OBJS) $(TARGETNAME).dll $(TARGETNAME).def lib$(TARGETNAME).dll.a lib$(TARGETNAME).a

install:
	$(INSTALL) ../../inc/ldi.h $(INSTALL_DIR)/include/ldi.h
	$(INSTALL) $(TARGETNAME).dll $(INSTALL_DIR)/lib/$(TARGETNAME).dll
	$(INSTALL) lib$(TARGETNAME).a $(INSTALL_DIR)/lib/lib$(TARGETNAME).a
	$(INSTALL) lib$(TARGETNAME).dll.a $(INSTALL_DIR)/lib/lib$(TARGETNAME).dll.a
