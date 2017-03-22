
MAKEFLAGS += -s --no-print-directory

BASEDIR = .
INCDIR = $(BASEDIR)/include
OBJDIR = $(BASEDIR)/aso
SRCDIR = $(BASEDIR)/src

DEV=true
ifdef NDEBUG
DEV=false
endif

ifndef CFLAGS
ifeq ($(DEV), true)
CFLAGS=-DFPS=30
else
CFLAGS=-DFPS=1000
endif
endif

CFLAGS += -std=c++11 -fPIC  -DFD_SETSIZE=1024000

ifeq ($(DEV), true)
	#Dev
	CFLAGS+=-g3 -O0 -fno-inline
else
	# Production
	CFLAGS+=-DNDEBUG -Os -fvisibility-inlines-hidden -g0 -gtoggle
endif

LDFLAGS=-shared -Wl,-soname,libu.so
CC=g++ $(CFLAGS)
INCLUDE=-I$(INCDIR)
VERSION=1.0
#EXTLIBS=pthread z xml2

H=
CPP=.cpp
O=.o
A=.a
SO=.so

SRCFILES=$(wildcard $(SRCDIR)/*$(CPP)) $(wildcard $(SRCDIR)/**/*$(CPP))
FILES=$(patsubst $(SRCDIR)/%$(CPP),%,$(SRCFILES))
OBJFILES=$(patsubst %,$(OBJDIR)/%$(O),$(FILES))
HDRFILES=$(patsubst %,$(INCDIR)/%$(H),$(FILES))

$(OBJDIR)/%$(O): $(SRCDIR)/%$(CPP) $(INCDIR)/%
	echo "[C] $*"
	mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all: libu$(SO) libu$(A) $(OBJFILES)
	echo "[`date`] Done. > $(OBJFILES)"

install: libu$(SO)
	sudo cp libu$(SO) /lib/
	sudo ln -sf libu$(SO) /lib/libu$(SO).$(VERSION)

libu$(SO): $(OBJFILES)
	echo "[S] $@"
	$(CC) $(LDFLAGS) -o $@ $(OBJFILES)

libu$(SO).$(VERSION): libu$(SO)
	ln -sf libu$(SO) libu$(SO).$(VERSION)
	
libu$(A): $(OBJFILES)
	echo "[A] $@"
	ar rs $@ $(OBJFILES) 	

clean:
	rm -r $(OBJDIR) >&2 2>/dev/null || true
	rm libu$(A) >&2 2>/dev/null || true
	rm libu$(SO) >&2 2>/dev/null || true
	rm libu$(SO).$(VERSION) >&2 2>/dev/null || true

