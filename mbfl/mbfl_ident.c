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
 * The source code included in this files was separated from mbfilter.c
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>

#include "mbfl_ident.h"
#include "mbfl_allocators.h"
#include "mbfilter_pass.h"
#include "mbfilter_8bit.h"
#include "mbfilter_wchar.h"

#include "filters/mbfilter_euc_cn.h"
#include "filters/mbfilter_hz.h"
#include "filters/mbfilter_euc_tw.h"
#include "filters/mbfilter_big5.h"
#include "filters/mbfilter_uhc.h"
#include "filters/mbfilter_euc_kr.h"
#include "filters/mbfilter_iso2022_kr.h"
#include "filters/mbfilter_sjis.h"
#include "filters/mbfilter_jis.h"
#include "filters/mbfilter_euc_jp.h"
#include "filters/mbfilter_euc_jp_win.h"
#include "filters/mbfilter_ascii.h"
#include "filters/mbfilter_koi8r.h"
#include "filters/mbfilter_cp866.h"
#include "filters/mbfilter_cp932.h"
#include "filters/mbfilter_cp936.h"
#include "filters/mbfilter_cp1251.h"
#include "filters/mbfilter_cp1252.h"
#include "filters/mbfilter_iso8859_1.h"
#include "filters/mbfilter_iso8859_2.h"
#include "filters/mbfilter_iso8859_3.h"
#include "filters/mbfilter_iso8859_4.h"
#include "filters/mbfilter_iso8859_5.h"
#include "filters/mbfilter_iso8859_6.h"
#include "filters/mbfilter_iso8859_7.h"
#include "filters/mbfilter_iso8859_8.h"
#include "filters/mbfilter_iso8859_9.h"
#include "filters/mbfilter_iso8859_10.h"
#include "filters/mbfilter_iso8859_13.h"
#include "filters/mbfilter_iso8859_14.h"
#include "filters/mbfilter_iso8859_15.h"
#include "filters/mbfilter_base64.h"
#include "filters/mbfilter_qprint.h"
#include "filters/mbfilter_uuencode.h"
#include "filters/mbfilter_7bit.h"
#include "filters/mbfilter_utf7.h"
#include "filters/mbfilter_utf7imap.h"
#include "filters/mbfilter_utf8.h"
#include "filters/mbfilter_utf16.h"
#include "filters/mbfilter_utf32.h"
#include "filters/mbfilter_byte2.h"
#include "filters/mbfilter_byte4.h"
#include "filters/mbfilter_ucs4.h"
#include "filters/mbfilter_ucs2.h"
#include "filters/mbfilter_htmlent.h"

static const mbfl_identify_vtbl vtbl_identify_false = {
	mbfl_encoding_id_pass,
	mbfl_filt_ident_false_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_false };


static const mbfl_identify_vtbl *mbfl_identify_filter_list[] = {
	&vtbl_identify_utf8,
	&vtbl_identify_utf7,
	&vtbl_identify_ascii,
	&vtbl_identify_eucjp,
	&vtbl_identify_sjis,
	&vtbl_identify_eucjpwin,
	&vtbl_identify_sjiswin,
	&vtbl_identify_jis,
	&vtbl_identify_2022jp,
	&vtbl_identify_euccn,
	&vtbl_identify_cp936,
	&vtbl_identify_hz,
	&vtbl_identify_euctw,
	&vtbl_identify_big5,
	&vtbl_identify_euckr,
	&vtbl_identify_uhc,
	&vtbl_identify_2022kr,
	&vtbl_identify_cp1251,
	&vtbl_identify_cp866,
	&vtbl_identify_koi8r,
	&vtbl_identify_cp1252,
	&vtbl_identify_8859_1,
	&vtbl_identify_8859_2,
	&vtbl_identify_8859_3,
	&vtbl_identify_8859_4,
	&vtbl_identify_8859_5,
	&vtbl_identify_8859_6,
	&vtbl_identify_8859_7,
	&vtbl_identify_8859_8,
	&vtbl_identify_8859_9,
	&vtbl_identify_8859_10,
	&vtbl_identify_8859_13,
	&vtbl_identify_8859_14,
	&vtbl_identify_8859_15,
	&vtbl_identify_false,
	NULL
};



/*
 * identify filter
 */

void mbfl_identify_filter_set_vtbl(mbfl_identify_filter *filter, const mbfl_identify_vtbl *vtbl)
{
	if (filter && vtbl) {
		filter->filter_ctor = vtbl->filter_ctor;
		filter->filter_dtor = vtbl->filter_dtor;
		filter->filter_function = vtbl->filter_function;
	}
}

const mbfl_identify_vtbl * mbfl_identify_filter_get_vtbl(mbfl_encoding_id encoding)
{
	const mbfl_identify_vtbl * vtbl;
	int i;

	i = 0;
	while ((vtbl = mbfl_identify_filter_list[i++]) != NULL) {
		if (vtbl->encoding == encoding) {
			break;
		}
	}

	return vtbl;
}

void mbfl_identify_filter_select_vtbl(mbfl_identify_filter *filter)
{
	const mbfl_identify_vtbl *vtbl;

	vtbl = mbfl_identify_filter_get_vtbl(filter->encoding->no_encoding);
	if (vtbl == NULL) {
		vtbl = &vtbl_identify_false;
	}
	mbfl_identify_filter_set_vtbl(filter, vtbl);
}

mbfl_identify_filter *mbfl_identify_filter_new(mbfl_encoding_id encoding)
{
	mbfl_identify_filter * filter;

	/* allocate */
	filter = (mbfl_identify_filter *)mbfl_malloc(sizeof(mbfl_identify_filter));
	if (filter == NULL) {
		return NULL;
	}

	/* encoding structure */
	filter->encoding = mbfl_get_encoding_by_id(encoding);
	if (filter->encoding == NULL) {
		filter->encoding = &mbfl_encoding_pass;
	}

	filter->status = 0;
	filter->flag = 0;
	filter->score = 0;

	/* setup the function table */
	mbfl_identify_filter_select_vtbl(filter);

	/* constructor */
	(*filter->filter_ctor)(filter);

	return filter;
}

void mbfl_identify_filter_delete(mbfl_identify_filter *filter)
{
	if (filter) {
		(*filter->filter_dtor)(filter);
		mbfl_free((void*)filter);
	}
}

void mbfl_filt_ident_common_ctor(mbfl_identify_filter *filter)
{
	filter->status = 0;
	filter->flag = 0;
}

void mbfl_filt_ident_common_dtor(mbfl_identify_filter *filter)
{
	filter->status = 0;
}

int mbfl_filt_ident_false(int c, mbfl_identify_filter *filter)
{
	filter->flag = 1;	/* bad */
	return c;
}

void mbfl_filt_ident_false_ctor(mbfl_identify_filter *filter)
{
	filter->status = 0;
	filter->flag = 1;
}

int mbfl_filt_ident_true(int c, mbfl_identify_filter *filter)
{
	return c;
}
