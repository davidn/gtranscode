## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = \
	$(GLADE_CFLAGS)\
	$(GNOME_CFLAGS)\
	$(GSTREAMER_CFLAGS)

AM_CFLAGS = \
	 -Wall \
	 -g

bin_PROGRAMS = gtranscode

gtranscode_SOURCES = \
	main.c\
	gtranscode-transcode.c\
	gtranscode-ui-handlers.c\
	gtranscode-gst-handlers.c

gtranscode_LDFLAGS = \
	-export-dynamic

gtranscode_LDADD = \
	$(GLADE_LIBS) \
	$(GNOME_LIBS) \
	$(GSTREAMER_LIBS)

AM_GCJFLAGS = 

