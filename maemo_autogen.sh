#!/bin/sh
cp maemo_configure.ac configure.ac
cp ./data/maemo_Makefile.am  ./data/Makefile.am
cp ./data/maemo_gtklantetris.desktop.in ./data/gtklantetris.desktop.in
echo "Maemo options will be used"
./autogen.sh