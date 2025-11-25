TARGETNAME=lzx_compress
TARGETPATH=../../bin
TARGETTYPE=APPLICATION
CC = i686-pc-mingw32-gcc
AR = i686-pc-mingw32-ar
RANLIB = i686-pc-mingw32-ranlib
CFLAGS = -O2 -I../inc
LDFLAGS =
RM = rm -f
INSTALL = install -m 755
INSTALL_DIR = /usr

OBJS = \
    lzx_compress.o

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

all: $(TARGETNAME).exe

$(TARGETNAME).exe: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ ../lzx/encoder/liblci.a

clean:
	$(RM) $(OBJS) $(TARGETNAME).exe

install:
	$(INSTALL) $(TARGETNAME).exe $(INSTALL_DIR)/bin/$(TARGETNAME).exe
