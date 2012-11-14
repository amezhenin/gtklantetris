#!/bin/sh
cp pc_configure.ac configure.ac
cp ./data/pc_Makefile.am  ./data/Makefile.am
cp ./data/pc_gtklantetris.desktop.in ./data/gtklantetris.desktop.in
echo "PC options will be used"
./autogen.sh
