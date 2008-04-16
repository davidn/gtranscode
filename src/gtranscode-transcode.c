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

#include <gst/gst.h>
#include <glib.h>
#include <gnome.h>
#ifdef DEBUG
#include <glib/gstdio.h>
#endif

#include "gtranscode.h"

GstElement *pipeline;

void
gtranscode_object_set (gchar ** option_pair, gpointer object)
{
    g_object_set (G_OBJECT (object), option_pair[0], option_pair[1], NULL);
}

void
pad_added (GstElement
           * src, GstPad * src_pad, gboolean arg1, GstElement * sink)
{
    GstPad *sink_pad;
    GstPadLinkReturn success = GST_PAD_LINK_OK;
    sink_pad = gst_element_get_pad (sink, "sink");
    
    if (gst_pad_can_link (src_pad, sink_pad))
    {
        success = gst_pad_link (src_pad, sink_pad);
    }
    return;
}

GstElement *
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
 video_codec_opts, GstElementFactory * sink_factory, GList * sink_opts)
{
    GstElement *source, *container, *video_codec, *audio_convert, *audio_codec, *sink, *decoder;
    /* Set up gst elements */
    pipeline = gst_pipeline_new ("transcoder");
    source = gst_element_factory_create (source_factory, "source");
    decoder = gst_element_factory_make ("decodebin2", "decoder");
    container = gst_element_factory_create (container_factory, "muxer");
    video_codec =
        gst_element_factory_create (video_codec_factory, "video_codec");
    audio_convert = gst_element_factory_make ("audioconvert" , "audio_convert");
    audio_codec =
        gst_element_factory_create (audio_codec_factory, "audio_codec");
    sink = gst_element_factory_create (sink_factory, "sink");
    if (!pipeline || !audio_convert
        || !source || !container || !video_codec || !audio_codec || !sink)
    {
        g_printerr ("Could not initialise elements!\n");
    }
    
    g_list_foreach (source_opts, (GFunc) gtranscode_object_set, source);
    g_list_foreach (container_opts, (GFunc) gtranscode_object_set, container);
    g_list_foreach
        (video_codec_opts, (GFunc) gtranscode_object_set, video_codec);
    g_list_foreach
        (audio_codec_opts, (GFunc) gtranscode_object_set, audio_codec);
    g_list_foreach (sink_opts, (GFunc) gtranscode_object_set, sink);
    gst_bin_add_many
        (GST_BIN (pipeline),audio_convert,
         source, decoder, video_codec, audio_codec, container, sink, NULL);
    gst_element_link_many (video_codec, container, sink, NULL);
    gst_element_link_many (audio_convert, audio_codec, container, NULL);
    gst_element_link_many (source, decoder, NULL);
    g_signal_connect
        (decoder, "new-decoded-pad", G_CALLBACK (pad_added), video_codec);
    g_signal_connect
        (decoder, "new-decoded-pad", G_CALLBACK (pad_added), audio_convert);
    /* Set up events for when it's done */
    /* Set up progress indicator */
    return pipeline;
}
