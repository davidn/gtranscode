## Process this file with automake to produce Makefile.in
## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

SUBDIRS = po include src

gtranscodedocdir = ${prefix}/doc/gtranscode
gtranscodedoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS\
	TODO

EXTRA_DIST = $(gtranscodedoc_DATA)

gnomemenudir = $(prefix)/@NO_PREFIX_PACKAGE_MENU_DIR@/Multimedia
gnomemenu_DATA = gtranscode.desktop

gtranscode_glade_filedir = $(prefix)/@NO_PREFIX_PACKAGE_DATA_DIR@/@PACKAGE@/glade
gtranscode_glade_file_DATA = gtranscode.glade

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

