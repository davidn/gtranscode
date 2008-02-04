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

#include <gst/gst.h>
#include <gnome.h>
#include <glade/glade.h>

#include "../include/gtranscode.h"

GstElement * pipeline;
GList * sources, *containers;
GladeXML * xml;

void
gtranscode_message_eos (GstBus * bus, GstMessage * message, gpointer data)
{
 GtkMessageDialog * dialog;
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  dialog = GTK_MESSAGE_DIALOG (gtk_message_dialog_new (GTK_WINDOW( glade_xml_get_widget(xml,"gtranscode_app")),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_INFO,
                                  GTK_BUTTONS_CLOSE,
                                  "Transcoding Complete!"));
 gtk_dialog_run (GTK_DIALOG (dialog));
 gtk_widget_destroy (GTK_WIDGET(dialog));
}

void
gtranscode_message_error (GstBus * bus, GstMessage * message, gpointer data)
{
 GError *err;
 gchar *debug;
 GtkMessageDialog * dialog;
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  gst_message_parse_error (message, &err, &debug);
  dialog = GTK_MESSAGE_DIALOG(gtk_message_dialog_new (GTK_WINDOW(glade_xml_get_widget(xml,"gtranscode_app")),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  "Error transcoding: %s",
                                  err->message));
 gtk_dialog_run (GTK_DIALOG (dialog));
 gtk_widget_destroy (GTK_WIDGET(dialog));
}

void
gtranscode_message_state_changed (GstBus * bus, GstMessage * message, gpointer data)
{
  GstState oldstate, newstate, pendingstate;
  gst_message_parse_state_changed ( message, &oldstate, &newstate, &pendingstate);
  switch( newstate )
	  {
	  case GST_STATE_READY:
      case GST_STATE_PAUSED:	
		  gst_element_set_state( pipeline, GST_STATE_PLAYING);		  
	      break;
	  case GST_STATE_PLAYING:
	      gtk_widget_hide ( glade_xml_get_widget (xml, "start-button"));
          gtk_widget_show ( glade_xml_get_widget (xml, "stop-button"));
	      break;
	  case GST_STATE_VOID_PENDING:
		  break;
	  case GST_STATE_NULL:
          gtk_widget_hide ( glade_xml_get_widget (xml, "stop-button"));
          gtk_widget_show ( glade_xml_get_widget (xml, "start-button"));
	  }
}
