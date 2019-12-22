# Microracers Makefile
# type 'make' to compile
# 'make clean' to remove compilation results (ie. for packaging)

# This Makefile should be the one used.
# It will use the other (inside src/) to compile the game.

# Directories where to install stuff
# (notice that Installing is not necessary!)
PREFIX=/usr

BIN_PREFIX=$(PREFIX)/bin
DATA_PREFIX=$(PREFIX)/share/microracers
DOCS_PREFIX=$(DATA_PREFIX)/docs
ICON_PREFIX=$(PREFIX)/share/icons

all: src/
	export DATA_PREFIX=$(DATA_PREFIX) && cd src/ && make

clean: src/
	rm -f *~ */*~ */*/*~ */*/*/*~ && cd src/ && make clean

dist:
	make clean
	rm -fr tools
	rm -fr CVS */CVS */*/CVS */*/*/CVS
	rm -f .cvsignore

install:
	mkdir -p $(DATA_PREFIX)
	mkdir -p $(DOCS_PREFIX)
	cp -r ./data/* $(DATA_PREFIX)
	install AUTHORS ChangeLog COPYING INSTALL \
          README TODO $(DOCS_PREFIX)
	install data/icon.xpm $(ICON_PREFIX)/microracers.xpm
	install microracers $(BIN_PREFIX)

uninstall:
	rm -rf $(DOCS_PREFIX)
	rm -rf $(DATA_PREFIX)
	rm -f $(BIN_PREFIX)/microracers
	rm -f $(ICON_PREFIX)/microracers.xpm
