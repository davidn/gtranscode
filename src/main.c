/* Created by Anjuta version 1.2.4a */
/*	This file will not be overwritten */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define DEBUG TRUE

#include <gnome.h>
#include <glade/glade.h>
#include <gst/gst.h>
#include <glib.h>
#include <glib/gstdio.h>

void
gtranscode_object_set (gchar ** option_pair, gpointer object)
{
  g_object_set (G_OBJECT (object), option_pair[0], option_pair[1], NULL);
}

void
transcode (GstElementFactory * source_factory,
	   GList * source_opts,
	   GstElementFactory * container_factory,
	   GList * container_opts,
	   GstElementFactory * audio_codec_factory,
	   GList * audio_codec_opts,
	   GstElementFactory * video_codec_factory,
	   GList * video_codec_opts,
	   GstElementFactory * sink_factory, GList * sink_opts)
{
  GstElement *pipeline, *source, *container, *video_codec, *audio_codec,
    *sink;

  /* Set up gst elements */
  pipeline = gst_pipeline_new ("transcoder");
  source = gst_element_factory_create (source_factory, "source");
  container = gst_element_factory_create (container_factory, "muxer");
  video_codec =
    gst_element_factory_create (video_codec_factory, "video_codec");
  audio_codec =
    gst_element_factory_create (audio_codec_factory, "audio_codec");
  sink = gst_element_factory_create (sink_factory, "sink");

  if (!pipeline || !source || !container || !video_codec || !audio_codec
      || !sink)
    {
      g_printerr ("Could not initialise elements!\n");
    }

  g_list_foreach (source_opts, (GFunc) gtranscode_object_set, source);
  g_list_foreach (container_opts, (GFunc) gtranscode_object_set, container);
  g_list_foreach (video_codec_opts, (GFunc) gtranscode_object_set,
		  video_codec);
  g_list_foreach (audio_codec_opts, (GFunc) gtranscode_object_set,
		  audio_codec);
  g_list_foreach (sink_opts, (GFunc) gtranscode_object_set, sink);

  /* Set up events for when it's done */
  /* Set up progress indicator */
  /* Start pipline */
}

void
gtranscode_ui_add (GstElementFactory * elementfactory,
		   GtkContainer * container)
{
  GList *siblings;
  GtkWidget *radio;
  GSList *radiogroup;
#ifdef DEBUG
  g_printf ("adding %s\n", gst_element_factory_get_longname (elementfactory));
#endif
  siblings = gtk_container_get_children (container);
  if (g_list_length (siblings) == 0)
    {
      radiogroup = NULL;
    }
  else
    {
      radiogroup = gtk_radio_button_get_group (g_list_nth_data (siblings, 0));
    }
  radio =
    gtk_radio_button_new_with_label (radiogroup,
				     gst_element_factory_get_longname
				     (elementfactory));
  gtk_container_add (container, radio);
  gtk_widget_show (radio);
}

GstElementFactory *
find_GstElementFactory (GList * factory_list,
			GtkContainer * radiogroup_container)
{
  GList *siblings = gtk_container_get_children (radiogroup_container);
  while ((siblings = g_list_next (siblings)))
    {
      if (gtk_toggle_button_get_active (g_list_nth_data (siblings, 0)))
	{
	  while ((factory_list = g_list_next (factory_list)))
	    {
	      if (g_str_equal
		  (gtk_label_get_text
		   (GTK_LABEL
		    (gtk_bin_get_child (g_list_nth_data (siblings, 0)))),
		   gst_element_factory_get_longname (g_list_nth_data
						     (factory_list, 0))))
		{
		  return (g_list_nth_data (factory_list, 0));
		}
	    }
	  factory_list = g_list_first (factory_list);
	}
    }
  siblings = g_list_first (siblings);
  return NULL;
}

void
transcode_button_clicked (GtkButton * button, GList * sources,
			  GList * containers, GList * audio_codecs,
			  GList * video_codecs, GtkBox *sources_hbox, GtkBox *containers_hbox,
              GtkBox * audio_codecs_vbox, GtkBox * video_codecs_vbox)
{
  GstElementFactory *source_factory, *container_factory, *audio_codec_factory,
    *video_codec_factory, *sink_factory;
  GList *source_opts, *container_opts, *audio_codec_opts, *video_codec_opts,
    *sink_opts;
  GtkContainer *box;

  box = GTK_CONTAINER( sources_hbox);
  source_factory = find_GstElementFactory(sources,box);
  source_opts = NULL;

  box = GTK_CONTAINER( containers_hbox);
  container_factory = find_GstElementFactory(containers,box);
  container_opts = NULL;

  box = GTK_CONTAINER( audio_codecs_vbox);
  audio_codec_factory = find_GstElementFactory(audio_codecs,box);
  audio_codec_opts = NULL;

  box = GTK_CONTAINER( video_codecs_vbox);
  video_codec_factory = find_GstElementFactory(video_codecs,box);
  video_codec_opts = NULL;

  sink_factory = gst_element_factory_find ( "filesink");
  sink_opts = NULL;

  transcode (source_factory,
	   source_opts,
	   container_factory,
	   container_opts,
	   audio_codec_factory,
	   audio_codec_opts,
	   video_codec_factory,
	   video_codec_opts,
	   sink_factory,
	   sink_opts);
}

int
main (int argc, char *argv[])
{
  GtkWidget *window1;
  GladeXML *xml;
  GList *sources = NULL, *containers = NULL, *audio_codecs =
    NULL, *video_codecs = NULL;
  /*GstRegistry *registry; */
#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain (PACKAGE);
#endif
  gnome_init (PACKAGE, VERSION, argc, argv);
  glade_gnome_init ();
  gst_init (&argc, &argv);
  /*
   * The .glade filename should be on the next line.
   */
  xml = glade_xml_new (PACKAGE_SOURCE_DIR "/gtranscode.glade", NULL, NULL);
  /* This is important */
  glade_xml_signal_autoconnect (xml);
  window1 = glade_xml_get_widget (xml, "gtranscode_app");
  gtk_widget_show (window1);
  /* Detect Gstreamer elements avilable */
/*
	registry = gst_register_get_default();
	containers = gst_registry_feature_filter(registry, gstranscode_feature_is_container, FALSE, NULL);
	video_codecs = gst_registry_feature_filter(registry, gstranscode_feature_is_video_codec, FALSE, NULL);
	audio_codecs = gst_registry_feature_filter(registry, gstranscode_feature_is_audio_codec, FALSE, NULL);
*/
  sources = g_list_append (sources, gst_element_factory_find ("filesrc"));
  containers =
    g_list_append (containers, gst_element_factory_find ("oggmux"));
  video_codecs =
    g_list_append (video_codecs, gst_element_factory_find ("theoraenc"));
  audio_codecs =
    g_list_append (audio_codecs, gst_element_factory_find ("vorbisenc"));
  audio_codecs =
    g_list_append (audio_codecs, gst_element_factory_find ("speexenc"));
  audio_codecs =
    g_list_append (audio_codecs, gst_element_factory_find ("flacenc"));
  /* Set up ui for Gstreamer elements available */
  g_list_foreach (sources, (GFunc) gtranscode_ui_add,
		  glade_xml_get_widget (xml, "sources_hbox"));
  g_list_foreach (containers, (GFunc) gtranscode_ui_add,
		  glade_xml_get_widget (xml, "container_hbox"));
  g_list_foreach (video_codecs, (GFunc) gtranscode_ui_add,
		  glade_xml_get_widget (xml, "videocodec_vbox"));
  g_list_foreach (audio_codecs, (GFunc) gtranscode_ui_add,
		  glade_xml_get_widget (xml, "audiocodec_vbox"));
  gtk_main ();
  return 0;
}
