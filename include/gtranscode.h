/***************************************************************************
 *            gtranscode.h
 *
 *  Sun Jan 27 22:59:09 2008
 *  Copyright  2008  David Newgas
 *  david@newgas.net
 ****************************************************************************/

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

#ifndef _GTRANSCODE_H
#define _GTRANSCODE_H

void gtranscode_object_set (gchar ** option_pair, gpointer object);

void
element_factory_add_to_gtk_list_store (GstElementFactory * element_factory,
                                       GtkListStore * group);

void
element_factory_add_to_gtk_list_store_with_children (GstElementFactory * element_factory,
                                                     GtkListStore * group);

void
gtranscode_static_pad_template_get_to_list (GstStaticPadTemplate * static_pad_template,
                                            GList ** pad_template_list);

gboolean
gtranscode_pad_templates_is_sink (GstPadTemplate * pad_template,
                                  gpointer user_data);

void
gtranscode_static_pad_template_get_to_list (GstStaticPadTemplate * static_pad_template,
                                            GList ** pad_template_list);

void
gtranscode_pad_template_get_caps_to_list (GstPadTemplate * pad_template,
                                          GList ** caps_list);

gboolean
gtranscode_element_factory_can_src_caps (GstElementFactory * element_factory,
                                         GList * caps );

void
pad_added (GstElement * element1,
           GstPad * pad, gboolean arg1,
           GstElement * element2);

GstElement * transcode (GstElementFactory * source_factory,
                        GList * source_opts,
                        GstElementFactory * container_factory,
                        GList * container_opts,
                        GstElementFactory * audio_codec_factory,
                        GList * audio_codec_opts,
                        GstElementFactory * video_codec_factory,
                        GList * video_codec_opts,
                        GstElementFactory * sink_factory,
                        GList * sink_opts);

void gtranscode_ui_update (GtkComboBox * combo_box);

void transcode_button_clicked (GtkButton * button);

void
gtranscode_options_button_clicked (GtkButton * button, GtkComboBox * widget);

int main (int argc, char *argv[]);

void
gtranscode_message_eos (GstBus * bus, GstMessage * message, gpointer data);

void
gtranscode_message_error (GstBus * bus, GstMessage * message, gpointer data);

void
gtranscode_stop_button_clicked (GtkButton * button);

void
gtranscode_message_state_changed (GstBus * bus, GstMessage *, gpointer data);

gboolean
gtranscode_feature_filter_by_klass (GstPluginFeature * feature, gchar * class);
#endif /* _GTRANSCODE_H */
