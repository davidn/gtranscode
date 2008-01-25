## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = \
	$(PACKAGE_CFLAGS)\
	 -I/usr/include/gstreamer-0.10 -I/usr/include/glib-2.0

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gtranscode

gtranscode_SOURCES = \
	main.c

gtranscode_LDFLAGS = \
	-export-dynamic

gtranscode_LDADD =  \
	$(PACKAGE_LIBS)\
	 -lgstreamer-0.10 -lglib-2.0

