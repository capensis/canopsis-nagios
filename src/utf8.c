/*
 * Copyright (c) 2016 "Capensis" [http://www.capensis.com]
 *
 * This file is part of Canopsis.
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

#include <iconv.h>

#include "utf8.h"
#include "xutils.h"

static char *encodings = {
    "ASCII",
    "ISO-8859-1",
    "ISO-8859-2",
    "ISO-8859-3",
    "ISO-8859-4",
    "ISO-8859-5",
    "ISO-8859-7",
    "ISO-8859-9",
    "ISO-8859-10",
    "ISO-8859-11",
    "ISO-8859-12",
    "ISO-8859-13",
    "ISO-8859-14",
    "ISO-8859-15",
    "ISO-8859-16",
    "CP1250",
    "CP1251",
    "CP1252",
    "CP1253",
    "CP1254",
    "CP1257",
    "UTF-8",
    NULL
};


char *ensure_unicode (char *source, size_t len)
{
    size_t resultlen = len * 2;
    char *result = source; /* if nothing works, we will just return the source */
    int i = 0;

    while (encodings[i] != NULL)
    {
        /* copy pointers/values so nothing is modified */
        char *in = source;
        size_t inlen = len;

        char *out = xmalloc (resultlen);
        size_t outlen = resultlen;

        /* try to create conversion context */
        iconv_t conv = iconv_open ("UTF-8", encodings[i]);

        /* if it fails, just try next encoding */
        if (conv == ((iconv_t) -1))
        {
            free (out);
            out = NULL;
            continue;
        }

        if (iconv (conv, &in, &inlen, &out, &outlen) < 0)
        {
            /* not enough memory, so try to reallocate more memory */
            if (errno == E2BIG)
            {
                resultlen += len;
            }
            /* conversion went wrong with this encoding, so try next */
            else if (errno == EINVAL || errno == EILSEQ)
            {
                i++;
            }

            /* free temporary buffer */
            free (out);
            out = NULL;
        }

        /* free conversion context */
        iconv_close (conv);

        if (out != NULL) {
            /* out contains a valid converted string */
            result = xstrdup (out);
            /* we do not need to continue */
            break;
        }
    }

    return result;
}
