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

#define DEBUG TRUE

#include <gnome.h>
#include <glade/glade.h>
#include <gst/gst.h>
#include <gst/gstfilter.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <libgnome/gnome-i18n.h>

#include "gtranscode.h"

// char* name, GstElementFactory* element_factory, GList* options ,GtkListStore allowed_audio_codecs, GtkListStore allowed_video_codecs)
GtkListStore * sources;
GtkListStore * containers;
gchar * source_filename;

GstElement *pipeline;

GladeXML *xml;

void
element_factory_add_to_gtk_list_store (GstElementFactory * element_factory, GtkListStore * group)
{
    GtkTreeIter iter;
    
    gtk_list_store_append( GTK_LIST_STORE (group), &iter);
    gtk_list_store_set ( GTK_LIST_STORE (group), &iter ,
                        0, gst_element_factory_get_longname (element_factory),
                        1, element_factory,
                        2, NULL,
                        3, NULL,
                        4, NULL, -1);
    /*initiase options*/
    :)
}

void
element_factory_add_to_gtk_list_store_with_children (GstElementFactory * element_factory, GtkListStore * group)
{
    GtkTreeIter iter;
    GtkListStore * audio_codecs_list_store;
    GtkListStore * video_codecs_list_store;
    GList * static_pad_templates = NULL;
    GList * pad_templates = NULL;
    GList * sink_pad_templates = NULL;
    GList * sink_caps = NULL;
    GList * audio_codecs = gst_registry_feature_filter( gst_registry_get_default(),
                                                       (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
                                                       FALSE,
                                                       "Codec/Encoder/Audio");
    GList * video_codecs = gst_registry_feature_filter( gst_registry_get_default(),
                                                       (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
                                                       FALSE,
                                                       "Codec/Encoder/Video");
    
    
    static_pad_templates = (GList *) gst_element_factory_get_static_pad_templates(element_factory);
    g_list_foreach (static_pad_templates,
                    (GFunc) gtranscode_static_pad_template_get_to_list,
                    &pad_templates);
    sink_pad_templates = gst_filter_run(pad_templates,
                                        (GstFilterFunc) gtranscode_pad_templates_is_sink,
                                        FALSE,
                                        NULL);
    g_list_foreach (sink_pad_templates,
                    (GFunc) gtranscode_pad_template_get_caps_to_list,
                    &sink_caps);
    
    
    audio_codecs = gst_filter_run (audio_codecs,
                                   (GstFilterFunc) gtranscode_element_factory_can_src_caps,
                                   FALSE,
                                   sink_caps);
    video_codecs = gst_filter_run (video_codecs,
                                   (GstFilterFunc) gtranscode_element_factory_can_src_caps,
                                   FALSE,
                                   sink_caps);
    
    if (g_list_length(audio_codecs) > 0 && g_list_length(video_codecs) > 0)
    {
        audio_codecs_list_store = gtk_list_store_new ( 5 ,  G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_OBJECT, G_TYPE_OBJECT );
        video_codecs_list_store = gtk_list_store_new ( 5 ,  G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER, G_TYPE_OBJECT, G_TYPE_OBJECT );
        g_list_foreach ( audio_codecs,
                        (GFunc) element_factory_add_to_gtk_list_store,
                        audio_codecs_list_store);
        g_list_foreach (video_codecs,
                        (GFunc) element_factory_add_to_gtk_list_store,
                        video_codecs_list_store);
        gtk_list_store_append( GTK_LIST_STORE (group), &iter);
        gtk_list_store_set ( GTK_LIST_STORE (group), &iter ,
                            0, gst_element_factory_get_longname (element_factory),
                            1, element_factory,
                            2, NULL,
                            3, audio_codecs_list_store,
                            4, video_codecs_list_store, -1);
    }
    
    g_list_free(static_pad_templates);
    g_list_free(pad_templates);
    g_list_free(sink_pad_templates);
    g_list_free(sink_caps);
    g_list_free(audio_codecs);
    g_list_free(video_codecs);
}

gboolean
gtranscode_pad_templates_is_sink (GstPadTemplate * pad_template, gpointer user_data)
{
    return (GST_PAD_TEMPLATE_DIRECTION(pad_template) == GST_PAD_SINK) ? TRUE : FALSE;
}

void
gtranscode_static_pad_template_get_to_list (GstStaticPadTemplate * static_pad_template,
                                            GList ** pad_template_list)
{
    *pad_template_list = g_list_append(*pad_template_list, gst_static_pad_template_get(static_pad_template));
}

void
gtranscode_pad_template_get_caps_to_list (GstPadTemplate * pad_template,
                                          GList ** caps_list)
{
    *caps_list = g_list_append(*caps_list, gst_pad_template_get_caps ( pad_template ));
}

gboolean
gtranscode_element_factory_can_src_caps (GstElementFactory * element_factory, GList * caps )
{
    do
    {
        if ( gst_element_factory_can_src_caps ( element_factory , caps->data ) == TRUE)
        {
            return TRUE;
        }
    }
    while ((caps = g_list_next(caps)) != NULL );
    return FALSE;
}

gboolean
gtranscode_feature_filter_by_klass (GstPluginFeature * feature, gchar * class)
{
    const gchar *klass;
    guint rank;
    
    if (!GST_IS_ELEMENT_FACTORY (feature))
        return FALSE;
    klass = gst_element_factory_get_klass (GST_ELEMENT_FACTORY (feature));
    if (g_strrstr (klass, class) == NULL )
    {
        return FALSE;
    }
    
    /* only select elements with autoplugging rank */
    rank = gst_plugin_feature_get_rank (feature);
    if (rank < GST_RANK_NONE)
        return FALSE;
    
    return TRUE;
}

int
main (int argc, char *argv[])
{
    /*GstRegistry *registry; */
    GtkWidget * file_chooser_dialog;
    GtkCellRenderer * text_renderer;
    GtkCellLayout * cell_layout;
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
    
    sources = gtk_list_store_new ( 5 ,  G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER , G_TYPE_OBJECT, G_TYPE_OBJECT);
    containers = gtk_list_store_new ( 5 ,  G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER , G_TYPE_OBJECT, G_TYPE_OBJECT);
    
    /* Detect Gstreamer elements avilable */
    g_list_foreach (
                    gst_registry_feature_filter( gst_registry_get_default(),
                                                (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
                                                FALSE,
                                                "Source/File"),
                    (GFunc) element_factory_add_to_gtk_list_store,
                    sources);
    g_list_foreach (
                    gst_registry_feature_filter( gst_registry_get_default(),
                                                (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
                                                FALSE,
                                                "Codec/Muxer"),
                    (GFunc) element_factory_add_to_gtk_list_store_with_children ,
                    containers);
    
    /* Set up ui for Gstreamer elements available */
    text_renderer = gtk_cell_renderer_text_new();
    cell_layout = GTK_CELL_LAYOUT (glade_xml_get_widget (xml, "sources_combobox"));
    gtk_cell_layout_pack_start(cell_layout, text_renderer,TRUE);
    gtk_cell_layout_set_attributes ( cell_layout ,
                                    text_renderer,
                                    "text",
                                    0,NULL);
    
    text_renderer = gtk_cell_renderer_text_new();
    cell_layout = GTK_CELL_LAYOUT (glade_xml_get_widget (xml, "container_combobox"));
    gtk_cell_layout_pack_start(cell_layout, text_renderer,TRUE);
    gtk_cell_layout_set_attributes ( cell_layout ,
                                    text_renderer,
                                    "text",
                                    0,NULL);
    
    text_renderer = gtk_cell_renderer_text_new();
    cell_layout = GTK_CELL_LAYOUT (glade_xml_get_widget (xml, "audio_codec_combobox"));
    gtk_cell_layout_pack_start(cell_layout, text_renderer,TRUE);
    gtk_cell_layout_set_attributes ( cell_layout ,
                                    text_renderer,
                                    "text",
                                    0,NULL);
    
    text_renderer = gtk_cell_renderer_text_new();
    cell_layout = GTK_CELL_LAYOUT (glade_xml_get_widget (xml, "video_codec_combobox"));
    gtk_cell_layout_pack_start(cell_layout, text_renderer,TRUE);
    gtk_cell_layout_set_attributes ( cell_layout ,
                                    text_renderer,
                                    "text",
                                    0,NULL);
    
    gtk_combo_box_set_model ( GTK_COMBO_BOX (glade_xml_get_widget (xml, "sources_combobox")),
                             GTK_TREE_MODEL (sources));
    gtk_combo_box_set_model ( GTK_COMBO_BOX (glade_xml_get_widget (xml, "container_combobox")),
                             GTK_TREE_MODEL (containers));
    
    file_chooser_dialog = gtk_file_chooser_dialog_new ("Open media file",
                                                       NULL,
                                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                                       GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                                       NULL);
  if (gtk_dialog_run (GTK_DIALOG (file_chooser_dialog)) == GTK_RESPONSE_ACCEPT)
  {
    source_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (file_chooser_dialog));
    gtk_widget_hide (file_chooser_dialog);
    gtk_widget_show (glade_xml_get_widget (xml, "gtranscode_app"));
    gtk_main ();
  }
  else
  {
      exit (1);
  }
    return 0;
}
