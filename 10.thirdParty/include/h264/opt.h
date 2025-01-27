/*
 * AVOptions
 * copyright (c) 2005 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVOPT_H
#define AVOPT_H

/**
 * @file opt.h
 * AVOptions
 */

enum AVOptionType{
    FF_OPT_TYPE_FLAGS,
    FF_OPT_TYPE_INT,
    FF_OPT_TYPE_INT64,
    FF_OPT_TYPE_DOUBLE,
    FF_OPT_TYPE_FLOAT,
    FF_OPT_TYPE_STRING,
    FF_OPT_TYPE_RATIONAL,
    FF_OPT_TYPE_CONST=128,
};

/**
 * AVOption.
 */
typedef struct AVOption {
    const char *name;

    /**
     * short English text help.
     * @fixme what about other languages
     */
    const char *help;
    int offset;             ///< offset to context structure where the parsed value should be stored
    enum AVOptionType type;

    double default_val;
    double min;
    double max;

    int flags;
#define AV_OPT_FLAG_ENCODING_PARAM  1   ///< a generic parameter which can be set by the user for muxing or encoding
#define AV_OPT_FLAG_DECODING_PARAM  2   ///< a generic parameter which can be set by the user for demuxing or decoding
#define AV_OPT_FLAG_METADATA        4   ///< some data extracted or inserted into the file like title, comment, ...
#define AV_OPT_FLAG_AUDIO_PARAM     8
#define AV_OPT_FLAG_VIDEO_PARAM     16
#define AV_OPT_FLAG_SUBTITLE_PARAM  32
//FIXME think about enc-audio, ... style flags
    const char *unit;
} AVOption;


AVOption *av_set_string(void *obj, const char *name, const char *val);
AVOption *av_set_double(void *obj, const char *name, double n);
AVOption *av_set_q(void *obj, const char *name, AVRational n);
AVOption *av_set_int(void *obj, const char *name, int64_t n);
double av_get_double(void *obj, const char *name, AVOption **o_out);
AVRational av_get_q(void *obj, const char *name, AVOption **o_out);
int64_t av_get_int(void *obj, const char *name, AVOption **o_out);
const char *av_get_string(void *obj, const char *name, AVOption **o_out, char *buf, int buf_len);
AVOption *av_next_option(void *obj, AVOption *last);
int av_opt_show(void *obj, void *av_log_obj);
void av_opt_set_defaults(void *s);

#endif
