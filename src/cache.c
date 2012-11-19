/*--------------------------------
# Copyright (c) 2011 "Capensis" [http://www.capensis.com]
#
# This file is part of Canopsis.
#
# Canopsis is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Canopsis is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with Canopsis.  If not, see <http://www.gnu.org/licenses/>.
# ---------------------------------*/

#include "nagios.h"
#include "logger.h"
#include "xutils.h"
#include "module.h"
#include "cache.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include "iniparser.h"

extern struct options g_options;

static dictionary *ini = NULL;
static unsigned int dbsetup = FALSE;
static time_t last_flush = 0;
static unsigned int ini_lock = FALSE;

static unsigned int
file_exists (const char *file)
{
    int status;
    struct stat s;
    status = stat (file, &s);
    if (status == -1)
        n2a_logger (LG_CRIT, "CACHE: stat: %s\n", strerror(errno));
    return (status == 0);
}

static int
create_empty_file (const char *file)
{
    int fd;
    int flags = O_CREAT|O_WRONLY|O_TRUNC;
    fd = open (file, flags, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
    if (fd < 0) {
        n2a_logger (LG_CRIT, "CACHE: %s: %s\n", file, strerror (errno));
        return -1;
    }
    close (fd);
    return 0;
}

void
n2a_clear_cache (void)
{
    n2a_flush_cache (TRUE);
    iniparser_freedict (ini);
}

void
n2a_init_cache (void)
{
    /* test if the db file already exists */
    if (!file_exists (g_options.cache_file)) {
        /* if it does not, create an empty one */
        int r = create_empty_file (g_options.cache_file);
        if (r < 0)
            return;
        /* now we are gonna fill it with default values */
        int fd[2];
        /* here is a little hack to fill it thru a pipe in order to avoid
         * writing data on the disk twice. Thus the data will be stored directly
         * in memory */
        pipe (fd);
        /* NOTE: this operation is only performed at neb startup */
        pid_t pid = fork ();
        if (pid == 0) {
            FILE *tmp;
            int i = 1;
            tmp = fdopen (fd[1], "w");
            /* we only create an empty section */
            fprintf (tmp, "[cache]\n");
/*
            fprintf (tmp, "size=0\n");
            for (; i <= g_options.cache_size; i++) {
                fprintf (tmp, "message_%d=\"\"\n", i);
                fprintf (tmp, "key_%d=\"\"\n", i);
            }
*/
            fclose (tmp);
            close (fd[1]);
            close (fd[0]);
            exit (0);
        }
        FILE *input;
        close (fd[1]);
        input = fdopen (fd[0], "r");
        ini = iniparser_load_fd (input);
        fclose (input);
        close (fd[0]);
    } else {
        ini = iniparser_load (g_options.cache_file);
    }

    if (ini==NULL) {
        n2a_logger (LG_CRIT, "cannot parse file: %s\n", g_options.cache_file);
        return;
    }

    dbsetup = TRUE;
}

void
n2a_flush_cache (unsigned int force)
{
    time_t now = 0;
    if ((!dbsetup || g_options.autoflush < 0) && !force)
        return;
    /* i know... gotos are a mess, but here i wanna avoid this comparison when
     * it's useless */
    if (g_options.autoflush == 0)
        goto do_it;
    now = time (NULL);
    if ((int) difftime (now, last_flush) < g_options.autoflush)
        return;

do_it:
    last_flush = now;
    FILE *db = fopen (g_options.cache_file, "w");
    if (db != NULL) {
        iniparser_dump_ini (ini, db);

        fclose (db);
    } else {
        n2a_logger (LG_CRIT, "CACHE: flush error: %s", strerror (errno));
    }
}

void
n2a_record_cache (const char *key, const char *message)
{
    char index[256];
    int count;
    /* avoid caching the message twice */
    if (ini_lock)
        return;
    count = iniparser_getsecnkeys (ini, "cache") / 2;
    if (count > g_options.cache_size) {
        n2a_logger (LG_CRIT, "cache size exceded! Unable to cache new messages");
        return;
    }
    snprintf (index, 256, "cache:key_%d", count+1);
    iniparser_set (ini, index, key);
    snprintf (index, 256, "cache:message_%d", count+1);
    iniparser_set (ini, index, message);
    n2a_logger (LG_INFO, "add message in cache: '%s'", key);
}

static int compare (const void * a, const void * b)
{
    /* The pointers point to offsets into "array", so we need to
       dereference them to get at the strings. */

   return strcmp (*(const char **) a, *(const char **) b);
}

void
n2a_pop_all_cache (void)
{
    int i = 0;
    int n;
    for (; i < ((n = iniparser_getsecnkeys (ini, "cache")) / 2); i++) {
        char **keys = iniparser_getseckeys (ini, "cache");
        /* sort the returned keys */
        qsort (keys, (size_t) n, sizeof (char *), compare);
        char *index_key = keys[0];
        /* then free the list although the doc says not to... */
        xfree (keys);
        char *m = strchr (index_key, '_');
        char index_message[256];
        snprintf (index_message, 256, "cache:message_%s", m+1);
        ini_lock = TRUE;
        char *key = iniparser_getstring (ini, index_key, NULL);
        char *message = iniparser_getstring (ini, index_message, NULL);
        int r = amqp_publish (key, message);
        ini_lock = FALSE;
        if (r < 0) {
            n2a_logger (LG_CRIT, "error while purging cache from message '%s'",
            key);
            break;
        }
        iniparser_unset (ini, index_key);
        iniparser_unset (ini, index_message);
        n2a_logger (LG_INFO, "cache successfuly purged from message '%s'",
        index_message);
    }
}
