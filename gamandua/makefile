include ../make_options
LDFLAGS += -ltd_base -ltd_core
LDFLAGS += $(shell pkg-config --libs gtk+-2.0 glib gdk)
CFLAGS += $(shell pkg-config --cflags gtk+-2.0 glib gdk)
NAME = gamandua
FILES = gamandua.c gamandua_callbacks.c
HEADERSDEPS = $(PREFIX)/include/td_base.h $(PREFIX)/include/td_core.h

include ../make_definitions
build: $(BINBUILD)
install: $(BININSTALL)
clean: $(BINCLEAN)
uninstall: $(BINUNINSTALL)
