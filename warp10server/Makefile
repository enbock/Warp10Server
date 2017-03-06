
ifndef TYPE
#TYPE=dynamic
TYPE=static
endif

DEV=true
ifdef NDEBUG
DEV=false
endif

MAKEFLAGS += -s --no-print-directory
# -j 16
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


ifndef CFLAGS
ifeq ($(DEV), true)
CFLAGS=-DFPS=30
else
CFLAGS=-DFPS=1000
endif
endif
CFLAGS += -fPIC  -DFD_SETSIZE=1024000

ifeq ($(DEV), true)
	# Dev
	CFLAGS+=-g3 -O0 -fno-inline
else
	# Production
	CFLAGS+=-DNDEBUG -Os -fvisibility-inlines-hidden -g0 -gtoggle
endif

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
SRCFILES=$(wildcard $(SRCDIR)/*$(CPP)) $(wildcard $(SRCDIR)/**/*$(CPP))
APPFILES=$(patsubst $(APPDIR)/%$(CPP),%,$(APPS))
FILES=$(patsubst $(SRCDIR)/%$(CPP),%,$(SRCFILES))
OBJFILES=$(patsubst %,$(OBJDIR)/%$(O),$(FILES))

all: $(APPS)
	echo "[`date`] Done."

# Keep object files.
.PRECIOUS: $(OBJFILES)

warp10%: $(ULIBDIR)/$(ULIB) $(OBJFILES)
	echo "[C] $@"
	$(CC) $(LFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $(APPDIR)/$@$(CPP) $(OBJFILES) $(LDLIBS) 

$(OBJDIR)/%$(O): $(SRCDIR)/%$(CPP) $(INCDIR)/%
	echo "[C] $*"
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -r $(OBJDIR) >&2 2>/dev/null || true
	rm $(APPS) >&2 2>/dev/null || true
	$(MAKE) -C $(ULIBDIR) clean

$(ULIBDIR)/$(ULIB): $(wildcard $(ULIBDIR)/include/*) $(wildcard $(ULIBDIR)/src/*.cpp)
	$(MAKE) -C $(ULIBDIR) $(ULIB)

lc:
	$(MAKE) all
	valgrind --leak-check=full --track-fds=yes --track-origins=yes ./warp10server

sync:
	rsync -rav --delete ../../drive.ww root@drive.ww:www/
