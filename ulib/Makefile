
MAKEFLAGS += -s --no-print-directory

BASEDIR = .
INCDIR = $(BASEDIR)/include
OBJDIR = $(BASEDIR)/aso
SRCDIR = $(BASEDIR)/src

# Production
#CFLAGS=-fPIC -DNDEBUG -Os -fvisibility-inlines-hidden -g0 -gtoggle

#Dev
CFLAGS=-fPIC -g3 -O0 -fno-inline -DFD_SETSIZE=1024000

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

SRCFILES=$(wildcard $(SRCDIR)/*$(CPP))
FILES=$(patsubst $(SRCDIR)/%$(CPP),%,$(SRCFILES))
OBJFILES=$(patsubst %,$(OBJDIR)/%$(O),$(FILES))
HDRFILES=$(patsubst %,$(INCDIR)/%$(H),$(FILES))

$(OBJDIR)/%$(O): $(SRCDIR)/%$(CPP) $(INCDIR)/%
	echo "[C] $*"
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

all: libu$(SO) libu$(A)
	echo "[`date`] Done."

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
	rm aso/* >&2 2>/dev/null || true
	rm libu$(A) >&2 2>/dev/null || true
	rm libu$(SO) >&2 2>/dev/null || true
	rm libu$(SO).$(VERSION) >&2 2>/dev/null || true

