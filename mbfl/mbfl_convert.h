/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public 
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.h
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.h is included in this package .
 *
 */

#ifndef MBFL_CONVERT_H
#define MBFL_CONVERT_H

#include "mbfl_encoding.h"
#include "mbfl_memory_device.h"

typedef struct _mbfl_convert_filter mbfl_convert_filter;

struct _mbfl_convert_filter {
	void (*filter_ctor)(mbfl_convert_filter *filter);
	void (*filter_dtor)(mbfl_convert_filter *filter);
	int (*filter_function)(int c, mbfl_convert_filter *filter);
	int (*filter_flush)(mbfl_convert_filter *filter);
	int (*output_function)(int c, void *data);
	int (*flush_function)(void *data);
	void *data;
	int status;
	int cache;
	const mbfl_encoding *from;
	const mbfl_encoding *to;
	int illegal_mode;
	int illegal_substchar;
};

typedef struct _mbfl_convert_vtbl mbfl_convert_vtbl;

struct _mbfl_convert_vtbl {
	mbfl_encoding_id from;
	mbfl_encoding_id to;
	void (*filter_ctor)(mbfl_convert_filter *filter);
	void (*filter_dtor)(mbfl_convert_filter *filter);
	int (*filter_function)(int c, mbfl_convert_filter *filter);
	int (*filter_flush)(mbfl_convert_filter *filter);
};

extern const mbfl_convert_vtbl *mbfl_convert_filter_list[];

mbfl_convert_filter *mbfl_convert_filter_new(
    mbfl_encoding_id from,
    mbfl_encoding_id to,
    int (*output_function)(int, void *),
    int (*flush_function)(void *),
    void *data );
void mbfl_convert_filter_delete(mbfl_convert_filter *filter);
int mbfl_convert_filter_feed(int c, mbfl_convert_filter *filter);
int mbfl_convert_filter_flush(mbfl_convert_filter *filter);
void mbfl_convert_filter_reset(mbfl_convert_filter *filter, mbfl_encoding_id from, mbfl_encoding_id to);
void mbfl_convert_filter_copy(mbfl_convert_filter *src, mbfl_convert_filter *dist);
int mbfl_filt_conv_illegal_output(int c, mbfl_convert_filter *filter);
void mbfl_convert_filter_select_vtbl(mbfl_convert_filter *filter);
const mbfl_convert_vtbl * mbfl_convert_filter_get_vtbl(mbfl_encoding_id from, mbfl_encoding_id to);

void mbfl_filt_conv_common_ctor(mbfl_convert_filter *filter);
int mbfl_filt_conv_common_flush(mbfl_convert_filter *filter);
void mbfl_filt_conv_common_dtor(mbfl_convert_filter *filter);

int mbfl_convert_filter_devcat(mbfl_convert_filter *filter, mbfl_memory_device *src);
int mbfl_convert_filter_strcat(mbfl_convert_filter *filter, const unsigned char *p);

#endif /* MBFL_CONVERT_H */
