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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#include "xutils.h"

void
n2a_rstrip(char *c)
{
    char *w = c + strlen(c) - 1;
    while (w >= c && isspace(*w))
        *w-- = '\0';
}

char *
n2a_lstrip(char *c)
{
    while (isspace(*c))
        c++;
    return c;
}

/*
 *c points to a string containing
 white space separated columns. This method returns
 a pointer to the zero-terminated next field. That
 might be identical with *c itself. The pointer c
 is then moved to the possible beginning of the
 next field. */
char *
n2a_next_field(char **c)
{
    /*
     *c points to first character of field */
    char *begin = n2a_lstrip(*c);    // skip leading spaces
    if (!*begin) {
        *c = begin;
        return 0;                // found end of string -> no more field
    }

    char *end = begin;            // copy pointer, search end of
    // field
    while (*end && !isspace(*end))
        end++;                    // search for \0 or white space
    if (*end) {                    // string continues -> terminate field
        // with '\0'
        *end = '\0';
        *c = end + 1;            // skip to character right *after* '\0'
    } else
        *c = end;                // no more field, point to '\0'
    return begin;
}

/*
 * similar to next_field() but takes one character as delimiter 
 */
char *
n2a_next_token(char **c, char delim)
{
    char *begin = *c;
    if (!*begin) {
        *c = begin;
        return 0;
    }

    char *end = begin;
    while (*end && *end != delim)
        end++;
    if (*end) {
        *end = 0;
        *c = end + 1;
    } else
        *c = end;
    return begin;
}

/*
 * these functions are part of the shelldone project developped by Benjamin
 * "Ziirish" Sans under the BSD licence
 */

int
xmin(int a, int b)
{
    return a < b ? a : b;
}

int
xmax(int a, int b)
{
    return a > b ? a : b;
}

void
xfree(void *ptr)
{
    if (ptr != NULL) {
        free(ptr);
    }
}

size_t
xstrlen(const char *in)
{
    const char *s;
    size_t cpt;
    if (in == NULL) {
        return 0;
    }
    /*
     * we want to avoid an overflow in case the input string isn't null
     * terminated 
     */
    for (s = in, cpt = 0; *s && cpt < UINT_MAX; ++s, cpt++) ;
    return (s - in);
}

void *
xmalloc(size_t size)
{
    void *ret = malloc(size);
    if (ret == NULL)
        err(2, "xmalloc can not allocate %lu bytes", (u_long) size);
    return ret;
}

char *
xstrdup(const char *dup)
{
    size_t len;
    char *copy;

    len = xstrlen(dup);
    if (len == 0)
        return NULL;
    copy = xmalloc(len + 1);
    if (copy != NULL)
        strncpy(copy, dup, len + 1);
    return copy;
}

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

char *n2a_str_join (char *delim, ...)
{
    va_list args;

    char *ret = NULL;
    size_t length = 0;
    size_t delim_len = xstrlen (delim);

    char *p = NULL;

    va_start (args, delim);

    while ((p = va_arg (args, char *)) != NULL)
    {
        char *tmp = NULL;
        size_t len = (ret ? xstrlen (ret) : 0);

        /* calculate new string size */
        if (len > 0)
        {
            length += delim_len;
        }

        length += xstrlen (p);

        /* reallocate string */
        if (!(tmp = realloc (ret, length + 1)))
        {
            break;
        }

        ret = tmp;

        if (len > 0)
        {
            strcpy (ret + len, delim);
            len += delim_len;
        }

        strcpy (ret + len, p);
    }

    va_end (args);

    return ret;
}

int vasprintf (char **ret, const char *fmt, va_list args)
{
    va_list ap;
    char c = 0;
    int size = 0;

    if (ret == NULL)
    {
        return -1;
    }

    va_copy (ap, args);
    size = vsnprintf (&c, 1, fmt, args);
    *ret = calloc (size + 1, 1);

    if (!(*ret))
    {
        return -1;
    }

    size = vsprintf (*ret, fmt, ap);
    return size;
}

int asprintf (char **ret, const char *fmt, ...)
{
    va_list args;
    int sz = 0;

    va_start (args, fmt);
    sz = vasprintf (ret, fmt, args);
    va_end (args);

    return sz;
}
