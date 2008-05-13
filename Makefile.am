## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

SUBDIRS = src po

gtranscodedocdir = ${prefix}/doc/gtranscode
gtranscodedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

gnomemenu_in_files = gtranscode.desktop.in
gnomemenudir = $(prefix)/@NO_PREFIX_PACKAGE_MENU_DIR@/Multimedia
gnomemenu_DATA = $(gtranscode_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

gtranscode_glade_filedir = $(prefix)/@NO_PREFIX_PACKAGE_DATA_DIR@/@PACKAGE@/glade
gtranscode_glade_file_DATA = gtranscode.glade

AM_GCJFLAGS = 

AM_CXXFLAGS = 

AM_CFLAGS = \
	-O0

EXTRA_DIST = gtranscode.glade intltool-extract.in intltool-merge.in intltool-update.in

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

