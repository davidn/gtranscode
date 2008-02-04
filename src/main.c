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
#include <glib.h>
#include <glib/gstdio.h>

#include "../include/gtranscode.h"


GList *sources = NULL, *containers = NULL;

GstElement *pipeline;

GladeXML *xml;

GtranscodeElementFactory *
gtranscode_element_factory_add (GstElementFactory * element_factory, GList ** group)
{
  GtranscodeElementFactory *result = g_new (GtranscodeElementFactory, 1);
  result->ElementFactory = element_factory;
  result->name = gst_element_factory_get_longname (result->ElementFactory);
  result->allowed_audio_codecs = NULL;
  result->allowed_video_codecs = NULL;
  result->options = NULL;
  /*initiase options*/
  *group = g_list_append(*group, result);
  return result;
}

GtranscodeElementFactory *
gtranscode_element_factory_add_with_children (GstElementFactory * element_factory, GList ** group)
{
  GtranscodeElementFactory *result =
    gtranscode_element_factory_add (element_factory, group);
  return result;
}

void
  gtranscode_ui_add
  (GtranscodeElementFactory * elementfactory, GtkContainer * container)
{
  GList *siblings;
  GSList *radiogroup;
#ifdef DEBUG
  g_printf ("adding %s\n", elementfactory->name);
#endif
  siblings = gtk_container_get_children (container);
  if (g_list_length (siblings) == 0)
    {
      radiogroup = NULL;
    }
  else
    {
      radiogroup = gtk_radio_button_get_group (siblings->data);
    }
  elementfactory->toggle =
    GTK_RADIO_BUTTON (gtk_radio_button_new_with_label
		      (radiogroup, elementfactory->name));

  gtk_container_add (container, GTK_WIDGET (elementfactory->toggle));
  gtk_widget_show (GTK_WIDGET (elementfactory->toggle));
}

void
gtranscode_ui_set_signal_toggle (GtranscodeElementFactory * element_factory,
				 gpointer data)
{
  g_signal_connect (element_factory->toggle, "toggled",
		    (GCallback) gtranscode_ui_update_toggles, NULL);
}

gint
  gtranscode_element_factory_is_enabled
  (GtranscodeElementFactory * element_factory, gpointer data)
{
  return
    gtk_toggle_button_get_active
    (GTK_TOGGLE_BUTTON (element_factory->toggle)) ? 0 : 1;
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

void
gtranscode_ui_update_toggles (GtkToggleButton * toggle_buttion)
{
  GtranscodeElementFactory *container_factory =
    g_list_find_custom (containers, NULL,
			(GCompareFunc)
			gtranscode_element_factory_is_enabled)->data;
  g_list_foreach (gtk_container_get_children
		  (GTK_CONTAINER
		   (glade_xml_get_widget (xml, "audiocodec_vbox"))),
		  (GFunc) gtk_widget_destroy, NULL);
  g_list_foreach (gtk_container_get_children
		  (GTK_CONTAINER
		   (glade_xml_get_widget (xml, "videocodec_vbox"))),
		  (GFunc) gtk_widget_destroy, NULL);
  g_list_foreach (container_factory->allowed_audio_codecs,
		  (GFunc) gtranscode_ui_add, glade_xml_get_widget (xml,
								   "audiocodec_vbox"));
  g_list_foreach (container_factory->allowed_video_codecs,
		  (GFunc) gtranscode_ui_add, glade_xml_get_widget (xml,
								   "videocodec_vbox"));
}


int
main (int argc, char *argv[])
{
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
  gtk_widget_show (glade_xml_get_widget (xml, "gtranscode_app"));

  /* Detect Gstreamer elements avilable */
  g_list_foreach (
              gst_registry_feature_filter( gst_registry_get_default(),
			             (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
						 FALSE,
						 "Source/File"),
			  (GFunc) gtranscode_element_factory_add,
			  &sources);
  g_list_foreach (
              gst_registry_feature_filter( gst_registry_get_default(),
			             (GstPluginFeatureFilter) gtranscode_feature_filter_by_klass,
						 FALSE,
						 "Codec/Muxer"),
			  (GFunc) gtranscode_element_factory_add_with_children,
			  &containers);
			  
    /*g_list_append
    (containers, gtranscode_element_factory_find_with_children ("oggmux"));*/

  /* Set up ui for Gstreamer elements available */
  g_list_foreach
    (sources,
     (GFunc) gtranscode_ui_add, glade_xml_get_widget (xml, "sources_hbox"));
  g_list_foreach (containers, (GFunc) gtranscode_ui_add,
		  glade_xml_get_widget (xml, "container_hbox"));
  g_list_foreach (containers, (GFunc) gtranscode_ui_set_signal_toggle,
		  glade_xml_get_widget (xml, "container_hbox"));
  g_list_foreach (gtk_container_get_children
		  (GTK_CONTAINER
		   (glade_xml_get_widget (xml, "container_hbox"))),
		  (GFunc) gtranscode_ui_update_toggles, NULL);

  gtk_main ();
  return 0;
}
