
ifndef TYPE
TYPE=static
endif

DEV=true
ifdef NDEBUG
DEV=false
endif

MAKEFLAGS += -s --no-print-directory -j 16
BASEDIR = .
SRCDIR = $(BASEDIR)/app
ULIBDIR=$(BASEDIR)/ulib
INCDIR=$(ULIBDIR)/include
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

CFLAGS += -std=c++11 -fPIC  -DFD_SETSIZE=1024000

# Produktion
ifeq ($(DEV), true)
	CFLAGS+=-g3 -O0 -fno-inline
else
	CFLAGS+=-DNDEBUG -Os -fvisibility-inlines-hidden -g0 -gtoggle
endif

ifeq ($(TYPE), static)
	LFLAGS=-Wl,-L$(ULIBDIR) -lz -lxml2 -lpthread -lu -static 
else
	LFLAGS=-Wl,-L$(ULIBDIR) -Wl,-rpath,$(ULIBDIR) -lpthread -lz -lxml2 -ldl -lstdc++ 
endif
CC=g++ $(CFLAGS)
INCLUDE=-I$(INCDIR)

H=
CPP=.cpp
O=.o

SRCFILES=$(wildcard $(SRCDIR)/*$(CPP))
FILES=$(patsubst $(SRCDIR)/%$(CPP),%,$(SRCFILES))

all: $(FILES)
	$(MAKE) -C warp10server
	echo "[`date`] Done."

#$(OBJDIR)/%$(O): $(SRCDIR)/%$(CPP)
#	echo "[C] $*"
#	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%: $(SRCDIR)/%$(CPP) $(ULIBDIR)/$(ULIB)
	echo "[C] $@"
ifeq ($(TYPE),static)
	$(CC) $(LFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $< $(ULIBDIR)/$(ULIB) `g++ -print-file-name=libpthread.a`
else
	$(CC) $(LFLAGS) $(CFLAGS) $(INCLUDE) -o $@ $< $(ULIBDIR)/$(ULIB) `g++ -print-file-name=libpthread.so` `g++ -print-file-name=libz.a`
endif

test: $(FILES)
	if test -z "$(APP)"; then for i in $(FILES); do	lc ./$$i; done; else lc ./$(APP); fi

clean:
	rm $(FILES) >&2 2>/dev/null || true
	$(MAKE) -C $(ULIBDIR) clean
	$(MAKE) -C warp10server clean

$(ULIBDIR)/$(ULIB): $(wildcard $(ULIBDIR)/include/*) $(wildcard $(ULIBDIR)/include/**/*) $(wildcard $(ULIBDIR)/src/*.cpp)  $(wildcard $(ULIBDIR)/src/**/*.cpp)
	$(MAKE) -C $(ULIBDIR) $(ULIB)

ea:
	rm -rf doc/EA 2>/dev/null || true
	mkdir -p doc/EA/Framework 2>/dev/null || true
	mkdir -p doc/EA/Server 2>/dev/null || true
	for i in ulib/include/*; do	dest=`echo $$i | sed "s:ulib/include/:doc/EA/Framework/:"`; cp $$i $$dest.h; done
	for i in warp10server/include/*; do	dest=`echo $$i | sed "s:warp10server/include/:doc/EA/Server/:"`; cp $$i $$dest.h; done

doc: /tmp/make_ulib_doc
	echo "[$$(date)] Done."

/tmp/make_ulib_doc: $(wildcard $(ULIBDIR)/include/*) $(wildcard $(ULIBDIR)/include/**/*) $(wildcard warp10server/include/*) $(wildcard warp10server/include/**/*)
	echo "[DOC] libu.sh"
	cd tools/cppdoc2 && ./libu.sh
	> $@

