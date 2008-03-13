/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include <glade/glade.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <gst/gst.h>

#include "../include/gtranscode.h"

GstElement * pipeline;
GList * sources, *containers;
GladeXML * xml;


gboolean gtranscode_ui_update_position (gpointer data)
{
  gint64 pos, len;
  GstFormat fmt = GST_FORMAT_TIME;
  if (!GST_IS_ELEMENT(pipeline))
	  {
	  return FALSE;
	  }
  if (gst_element_query_position (pipeline, &fmt, &pos)
    && gst_element_query_duration (pipeline, &fmt, &len)) {
    gnome_appbar_set_progress_percentage ( GNOME_APPBAR(glade_xml_get_widget(xml,"appbar")),
	        (gfloat) pos / (gfloat) len);
  }

  /* call me again */
  return TRUE;
}
void
gtranscode_stop_button_clicked (GtkButton * button)
{
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);
          gtk_widget_hide ( glade_xml_get_widget (xml, "stop-button"));
          gtk_widget_show ( glade_xml_get_widget (xml, "start-button"));
  /* The gtk functions should really be called in gtranscode_message_state_changed
     but that doesn't seem to work...!!*/
}

void
gtranscode_transcode_button_clicked (GtkButton * button)
{
  GstElementFactory *source_factory,
    *container_factory,
    *audio_codec_factory, *video_codec_factory, *sink_factory;
  GtranscodeElementFactory *container_group;
  GstElement * pipeline;
  GstBus * bus;
  GList *source_opts,
    *container_opts, *audio_codec_opts, *video_codec_opts, *sink_opts;
  gchar *filesrcopts[] = {
    "location",
    "/home/david/films/Ghostbusters.avi"
  };
  gchar *filesinkopts[] = {
    "location",
    "/tmp/gb.ogg"
  };
  source_factory =
    ((GtranscodeElementFactory
      *) (g_list_find_custom (sources, NULL,
			      (GCompareFunc)
			      gtranscode_element_factory_is_enabled)->data))->
    ElementFactory;
  source_opts = NULL;
  source_opts = g_list_append (source_opts, filesrcopts);
  container_group =
    (GtranscodeElementFactory
     *) (g_list_find_custom (containers, NULL,
			     (GCompareFunc)
			     gtranscode_element_factory_is_enabled)->data);
  container_factory = container_group->ElementFactory;
  container_opts = NULL;
  audio_codec_factory =
    ((GtranscodeElementFactory
      *) (g_list_find_custom (container_group->allowed_audio_codecs, NULL,
			      (GCompareFunc)
			      gtranscode_element_factory_is_enabled)->data))->
    ElementFactory;
  audio_codec_opts = NULL;
  video_codec_factory =
    ((GtranscodeElementFactory
      *) (g_list_find_custom (container_group->allowed_video_codecs, NULL,
			      (GCompareFunc)
			      gtranscode_element_factory_is_enabled)->data))->
    ElementFactory;
  video_codec_opts = NULL;
  sink_factory = gst_element_factory_find ("filesink");
  sink_opts = NULL;
  sink_opts = g_list_append (sink_opts, filesinkopts);
  pipeline = transcode (source_factory, source_opts, container_factory, container_opts,
	     audio_codec_factory, audio_codec_opts, video_codec_factory,
	     video_codec_opts, sink_factory, sink_opts);
		 
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_signal_watch (bus);
  g_signal_connect (bus, "message::error", G_CALLBACK (gtranscode_message_error), NULL);
  g_signal_connect (bus, "message::eos", G_CALLBACK (gtranscode_message_eos), NULL);
  g_signal_connect (bus, "message::state-changed", G_CALLBACK( gtranscode_message_state_changed),NULL);
  gst_object_unref(bus);
  
  g_timeout_add (200, (GSourceFunc) gtranscode_ui_update_position, NULL);
  
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
}
void
gtranscode_options_button_clicked_id (GtkButton * button, gint groupid)
{
  GList *options = NULL;
/*remove options from ui*/
  switch (groupid)
    {
    case 0:
      options = ((GtranscodeElementFactory
		 *) (g_list_find_custom (sources, NULL,
					 (GCompareFunc)
					 gtranscode_element_factory_is_enabled)->
		     data))->options;
      break;
    case 1:
      options = ((GtranscodeElementFactory
		 *) (g_list_find_custom (containers, NULL,
					 (GCompareFunc)
					 gtranscode_element_factory_is_enabled)->
		     data))->options;
      break;
    case 2:
      options = ((GtranscodeElementFactory
		 *) (g_list_find_custom (((GtranscodeElementFactory
					  *) (g_list_find_custom (containers,
								  NULL,
								  (GCompareFunc)
								  gtranscode_element_factory_is_enabled)->
					      data))->allowed_video_codecs,
					 NULL,
					 (GCompareFunc)
					 gtranscode_element_factory_is_enabled)->
		     data))->options;
      break;
    case 3:
      options = ((GtranscodeElementFactory
		 *) (g_list_find_custom (((GtranscodeElementFactory
					  *) (g_list_find_custom (containers,
								  NULL,
								  (GCompareFunc)
								  gtranscode_element_factory_is_enabled)->
					      data))->allowed_audio_codecs,
					 NULL,
					 (GCompareFunc)
					 gtranscode_element_factory_is_enabled)->
		     data))->options;
      break;
    default:
      g_printf ("Invalid option button clicked: %d\n", groupid);
      return;
    }
/*initiase options in ui*/
  gtk_widget_show (glade_xml_get_widget (xml, "options_dialog"));
}
void
video_codec_options_button_clicked (GtkButton * button)
{
	gtranscode_options_button_clicked_id (button, 2);
}

void
auido_codec_options_button_clicked (GtkButton * button)
{
	gtranscode_options_button_clicked_id (button, 3);
}

void
containers_options_button_clicked (GtkButton * button)
{
	gtranscode_options_button_clicked_id (button, 1);
}

void
sources_options_button_clicked (GtkButton * button)
{
	gtranscode_options_button_clicked_id (button, 0);
}
