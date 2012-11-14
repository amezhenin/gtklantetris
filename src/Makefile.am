# List of the filenames of binaries that this project will produce.
bin_PROGRAMS = gtklantetris
HIGHSCORE_PATH = $(PREFIX)/share/gtklantetris
HIGHSCORE_FILE = $(HIGHSCORE_PATH)/highscore.dat
# For each binary file name specify the list the source files required
gtklantetris_SOURCES = interface.c \
	highscore.c \
	misc.c \
	tetris.c \
	network.c \
	botlogic.c \
	tetris.h \
	dialogs.c

AM_CFLAGS = -DDATADIR=\"$(datadir)\" -g -DAPP_ICON=\"$(prefix)/share/pixmaps/gtklantetris.png\"
# -DHIGHSCORE_FILE=\"$(HIGHSCORE_FILE)\"