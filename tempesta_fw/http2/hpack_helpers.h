/**
 *		Tempesta FW
 *
 * Common macro definitions used by HPACK decoder modules.
 *
 * Copyright (C) 2017 Tempesta Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef HPACK_HELPERS_H
#define HPACK_HELPERS_H

#include "common.h"
#include "netconv.h"

#define HPACK_LIMIT (Bit_Capacity / 7) * 7
#define HPACK_LAST ((1 << (Bit_Capacity % 7)) - 1)

/* Flexible integer decoding as specified */
/* in the HPACK RFC-7541: */

#define GET_FLEXIBLE(x) 				       \
do {							       \
	ufast __m = 0;					       \
	ufast __c;					       \
	do {						       \
		if (unlikely(m == 0)) { 		       \
			if (n) {			       \
				src = buffer_next(source, &m); \
			}				       \
			else {				       \
				hp->shift = __m;	       \
				hp->saved = x;		       \
				goto Incomplete;	       \
			}				       \
		}					       \
		__c = * src++;				       \
		n--;					       \
		m--;					       \
		if (__m <  HPACK_LIMIT ||		       \
		   (__m == HPACK_LIMIT &&		       \
		    __c <= HPACK_LAST)) 		       \
		{					       \
			x += (__c & 127) << __m;	       \
			__m += 7;			       \
		}					       \
		else if (__c) { 			       \
			goto Overflow;			       \
		}					       \
	} while (__c > 127);				       \
} while (0)

/* Continue decoding after interruption due */
/* to absence of the next fragment: */

#define GET_CONTINUE(x) 				       \
do {							       \
	ufast __m = hp->shift;				       \
	ufast __c = * src++;				       \
	x = hp->saved;					       \
	n--;						       \
	m--;						       \
	if (__m <  HPACK_LIMIT ||			       \
	   (__m == HPACK_LIMIT &&			       \
	    __c <= HPACK_LAST)) 			       \
	{						       \
		x += (__c & 127) << __m;		       \
		__m += 7;				       \
	}						       \
	else if (__c) { 				       \
		goto Overflow;				       \
	}						       \
	while (__c > 127) {				       \
		if (unlikely(m == 0)) { 		       \
			if (n) {			       \
				src = buffer_next(source, &m); \
			}				       \
			else {				       \
				hp->shift = __m;	       \
				hp->saved = x;		       \
				goto Incomplete;	       \
			}				       \
		}					       \
		__c = * src++;				       \
		n--;					       \
		m--;					       \
		if (__m <  HPACK_LIMIT ||		       \
		   (__m == HPACK_LIMIT &&		       \
		    __c <= HPACK_LAST)) 		       \
		{					       \
			x = Bit_Join(__c & 127, __m, x);       \
			__m += 7;			       \
		}					       \
		else if (__c) { 			       \
			goto Overflow;			       \
		}					       \
	}						       \
} while (0)

#endif