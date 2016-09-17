
ifndef TYPE
TYPE=dynamic
#TYPE=static
endif

MAKEFLAGS += -s --no-print-directory
BASEDIR = .
SRCDIR = $(BASEDIR)/src
APPDIR = $(BASEDIR)/app
ULIBDIR=$(BASEDIR)/../ulib
INCDIR=$(BASEDIR)/include
OBJDIR=$(BASEDIR)/aso
ifeq ($(TYPE),static)
	ULIB=libu.a
else
	ULIB=libu.so.1.0
endif

# Production
#CFLAGS=-fPIC -DNDEBUG -Os -fvisibility-inlines-hidden -g0 -gtoggle -DFD_SETSIZE=1024000

# Dev
CFLAGS=-fPIC -g3 -O0 -fno-inline -DFD_SETSIZE=1024000

ifeq ($(TYPE),static)
	LFLAGS=-static -static-libgcc
	LDLIBS = $(ULIBDIR)/$(ULIB) \
	`g++ -print-file-name=libz.a` `g++ -print-file-name=libpthread.a` 

else
	LFLAGS=-Wl,-L,$(ULIBDIR)/ -Wl,-rpath,$(ULIBDIR) -lpthread -lz -lxml2 -ldl -lstdc++ -lu
	LDLIBS = $(ULIBDIR)/$(ULIB) \
	`g++ -print-file-name=libz.a` `g++ -print-file-name=libpthread.so` 

endif
CC=g++ $(CFLAGS)
INCLUDE=-I$(ULIBDIR)/include -I$(INCDIR)

H=
CPP=.cpp
O=.o

APPS=warp10server warp10admin
SRCFILES=$(wildcard $(SRCDIR)/*$(CPP))
APPFILES=$(patsubst $(APPDIR)/%$(CPP),%,$(APPS))
FILES=$(patsubst $(SRCDIR)/%$(CPP),%,$(SRCFILES))
OBJFILES=$(patsubst %,$(OBJDIR)/%$(O),$(FILES))

all: $(APPS)
	echo "[`date`] Done."

warp10%: $(ULIBDIR)/$(ULIB) $(OBJFILES)
	echo "[C] $@"
	$(CC) $(LFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $(APPDIR)/$@$(CPP) $(OBJFILES) $(LDLIBS) 

$(OBJDIR)/%$(O): $(SRCDIR)/%$(CPP) $(INCDIR)/%
	echo "[C] $*"
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm aso/* >&2 2>/dev/null || true
	rm $(APPS) >&2 2>/dev/null || true
	$(MAKE) -C $(ULIBDIR) clean

$(ULIBDIR)/$(ULIB): $(wildcard $(ULIBDIR)/include/*) $(wildcard $(ULIBDIR)/src/*.cpp)
	$(MAKE) -C $(ULIBDIR) $(ULIB)

lc:
	$(MAKE) all
	valgrind --leak-check=full --track-fds=yes --track-origins=yes ./warp10server

sync:
	rsync -rav --delete ../../drive.ww root@drive.ww:www/
