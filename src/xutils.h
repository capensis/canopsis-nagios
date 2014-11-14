// +------------------------------------------------------------------+
// |             ____ _               _        __  __ _  __           |
// |            / ___| |__   ___  ___| | __   |  \/  | |/ /           |
// |           | |   | '_ \ / _ \/ __| |/ /   | |\/| | ' /            |
// |           | |___| | | |  __/ (__|   <    | |  | | . \            |
// |            \____|_| |_|\___|\___|_|\_\___|_|  |_|_|\_\           |
// |                                                                  |
// | Copyright Mathias Kettner 2010             mk@mathias-kettner.de |
// +------------------------------------------------------------------+
// 
// This file is part of Check_MK.
// The official homepage is at http://mathias-kettner.de/check_mk.
// 
// check_mk is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation in version 2.  check_mk is distributed
// in the hope that it will be useful, but WITHOUT ANY WARRANTY; with-
// out even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more de-
// ails.  You should have received a copy of the GNU General Public
// License along with GNU Make; see the file COPYING.  If not, write
// to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
// Boston, MA 02110-1301 USA.

#ifndef strutil_h
#define strutil_h

#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif
    void n2a_rstrip(char *);
    char *n2a_lstrip(char *line);
#ifdef __cplusplus
    char *n2a_next_token(char **c, char delim = ',');
#else
    char *n2a_next_token(char **c, char delim);
#endif
    char *n2a_next_field(char **line);
#ifdef __cplusplus
}
#endif
/**
 * these functions are part of the shelldone project developped by Benjamin
 * "Ziirish" Sans under the BSD licence
 */

/** 
 * hate to do that, but I don't have the choice because of my alloca
 * macro 
 */
#include <err.h>
#include <alloca.h>
/**
* Returns the smallest value of the given parameters
* @param a Value to compare
* @param b Value to compare
* @return Smallest value between a and b
*/
int xmin(int a, int b);

/**
* Returns the biggest value of the given parameters
* @param a Value to compare
* @param b Value to compare
* @return Biggest value between a and b
*/
int xmax(int a, int b);

/**
* Frees the memory used by the given pointer
* @param ptr Pointer to free. If NULL the function returns
*/
void xfree(void *ptr);

/**
* Allocates memory or exit and display an error
* @param size Requested size to allocate
* @return The address pointing to the memory-space
*/
void *xmalloc(size_t size);

/**
* Allocates temporary memory or exit and display an error
* @param size Requested size to allocate
*/
/**
* alloca() is allocated within the stack frame, that space is automatically
* freed if the function returns. that is the reason why we use a macro here
*/
#define xalloca(ret,size) \
do{ \
  ret = alloca (size); \
  if (ret == NULL) \
    err (2, "xalloca can not allocate %lu bytes",(u_long) size); \
}while(0)

/**
* Duplicates a string
* @param dup String to copy
* @return A pointer to a copy of the given string
*/
char *xstrdup(const char *dup);

/**
* Gives the size of a string
* @param src String we want the size of
* @return Size of src, 0 if NULL
*/
size_t xstrlen(const char *src);

/*
 * Copyright (c) 2011 "Capensis" [http://www.capensis.com]
 *
 * The following content is part of Canopsis.
 *
 * Canopsis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Canopsis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Canopsis.  If not, see <http://www.gnu.org/licenses/>.
 */

char *n2a_str_join (char *delim, ...);

#include <stdarg.h>

#ifndef va_copy
#   define va_copy(d,s)      __va_copy(d,s)
#endif

int vasprintf (char **ret, const char *fmt, va_list args);
int asprintf (char **ret, const char *fmt, ...);

#endif                            // strutil_h
