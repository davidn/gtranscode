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

typedef struct
{
  const gchar *name;
  GtkRadioButton *toggle;
  GstElementFactory *ElementFactory;
  GList *allowed_video_codecs;
  GList *allowed_audio_codecs;
} GtranscodeElementFactory;

GtranscodeElementFactory *
gtranscode_element_factory_find (gchar * identifier);

GtranscodeElementFactory *
gtanscode_element_factory_find_with_children (gchar * identifier);

void
gtranscode_object_set (gchar ** option_pair, gpointer object);

void
pad_added (GstElement
	   * element1, GstPad * pad, gboolean arg1, GstElement * element2);

void
  transcode
  (GstElementFactory *
   source_factory,
   GList * source_opts,
   GstElementFactory *
   container_factory,
   GList *
   container_opts,
   GstElementFactory *
   audio_codec_factory,
   GList *
   audio_codec_opts,
   GstElementFactory *
   video_codec_factory,
   GList *
   video_codec_opts, GstElementFactory * sink_factory, GList * sink_opts);

void
  gtranscode_ui_add
  (GtranscodeElementFactory * elementfactory, GtkContainer * container, gboolean updatesignal);

gint
  gtranscode_element_factory_is_enabled
  (GtranscodeElementFactory * element_factory, gpointer data);
  
void
  gtranscode_ui_update_toggles
  (GtkToggleButton * toggle_buttion);

void
transcode_button_clicked (GtkButton * button);

int
main (int argc, char *argv[]);


#endif /* _GTRANSCODE_H */
