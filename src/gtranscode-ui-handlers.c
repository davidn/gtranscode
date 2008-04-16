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

#include "gtranscode.h"

GstElement * pipeline;
GtkListStore * sources, *containers;
GladeXML * xml;

void
gtranscode_ui_update (GtkComboBox * combo_box)
{
    GtkTreeIter iter;
    GtkListStore* list_store;
    GValue value = {0, };
    gtk_combo_box_get_active_iter ( combo_box , &iter );
    gtk_tree_model_get_value ( GTK_TREE_MODEL (containers), &iter, 3, &value);
    list_store = g_value_get_object(&value);
    g_value_unset(&value);
    gtk_combo_box_set_model ( GTK_COMBO_BOX (glade_xml_get_widget (xml, "audio_codec_combobox")),
                             GTK_TREE_MODEL (list_store));
    gtk_tree_model_get_value ( GTK_TREE_MODEL (containers), &iter, 4, &value);
    list_store = g_value_get_object(&value);
    g_value_unset(&value);
    gtk_combo_box_set_model ( GTK_COMBO_BOX (glade_xml_get_widget (xml, "video_codec_combobox")),
                             GTK_TREE_MODEL (list_store));
}


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
    GstElement * pipeline;
    GstBus * bus;
    GValue value = {0, };
    GtkTreeIter iter;
    GList *source_opts,
    *container_opts, *audio_codec_opts, *video_codec_opts, *sink_opts;
    gchar *filesrcopts[] = {
        "location",
        "/home/david/films/Dogma.avi"
    };
    gchar *filesinkopts[] = {
        "location",
        "/tmp/gb.ogg"
    };
    gtk_combo_box_get_active_iter ( GTK_COMBO_BOX( glade_xml_get_widget (xml, "sources_combobox"))
                                   , &iter );
    gtk_tree_model_get_value ( GTK_TREE_MODEL (sources), &iter, 1, &value);
    source_factory = g_value_get_pointer( &value);
    source_opts = NULL;
    source_opts = g_list_append (source_opts, filesrcopts);
    
    
    gtk_combo_box_get_active_iter ( GTK_COMBO_BOX( glade_xml_get_widget (xml, "container_combobox"))
                                   , &iter );
    g_value_unset(&value);
    gtk_tree_model_get_value ( GTK_TREE_MODEL (containers), &iter, 1, &value);
    container_factory = g_value_get_pointer( &value);
    container_opts = NULL;
    
    
    gtk_combo_box_get_active_iter ( GTK_COMBO_BOX( glade_xml_get_widget (xml, "audio_codec_combobox"))
                                   , &iter );
    g_value_unset(&value);
    gtk_tree_model_get_value ( gtk_combo_box_get_model(GTK_COMBO_BOX( glade_xml_get_widget (xml, "audio_codec_combobox"))),
                              &iter, 1, &value);
    audio_codec_factory = g_value_get_pointer( &value);
    audio_codec_opts = NULL;
    
    
    gtk_combo_box_get_active_iter ( GTK_COMBO_BOX( glade_xml_get_widget (xml, "video_codec_combobox"))
                                   , &iter );
    g_value_unset(&value);
    gtk_tree_model_get_value ( gtk_combo_box_get_model(GTK_COMBO_BOX( glade_xml_get_widget (xml, "video_codec_combobox"))),
                              &iter, 1, &value);
    video_codec_factory = g_value_get_pointer( &value);
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
gtranscode_options_button_clicked (GtkButton * button, GtkComboBox * widget)
{
    GtkTreeIter iter;
    GstElementFactory * element_factory;
    GValue value = {0, };
    GList *options = NULL;
    GtkDialog * option_window;
    if (gtk_combo_box_get_active_iter (widget,
                                       &iter ) == FALSE )
    {
        return;
    }
    gtk_tree_model_get_value (gtk_combo_box_get_model(widget),
                              &iter, 2, &value);
    options = g_value_get_pointer(&value);
    g_value_unset(&value);
    gtk_tree_model_get_value (gtk_combo_box_get_model(widget),
                              &iter, 1, &value);
    element_factory = g_value_get_pointer(&value);
    
    /*initiase options in ui*/
    
    option_window = gtk_dialog_new_with_buttons (NULL,
                                                 NULL,
                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_STOCK_OK,
                                                 GTK_RESPONSE_ACCEPT,
                                                 GTK_STOCK_CANCEL,
                                                 GTK_RESPONSE_REJECT,
                                                 NULL);
    switch (gtk_dialog_run(option_window))
    {
        case GTK_RESPONSE_ACCEPT:
        break;
        default:
        break;
    }
    gtk_widget_destroy(option_window);
}
