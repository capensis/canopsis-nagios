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
static time_t last_pop = 0;
static unsigned int ini_lock = FALSE;
static int lastid = 0;
static unsigned int pop_lock = FALSE;
static const char *tkey = NULL, *tmsg = NULL;
static unsigned int purge_cache = FALSE;
int c_size = -10000;

static int compare (const void * a, const void * b)
{
    /* The pointers point to offsets into "array", so we need to
       dereference them to get at the strings. */
    const char *aa, *bb;
    aa = *(const char **) a;
    bb = *(const char **) b;
    int ret = 0;
    if (strncmp (aa, bb, 8) != 0) {
        ret = strcmp (aa, bb);
    } else {
        char *aptr = strchr (aa, '_');
        char *bptr = strchr (bb, '_');
        int ia, ib;
        ia = strtol (aptr+1, NULL, 10);
        ib = strtol (bptr+1, NULL, 10);
        ret = ia - ib;
    }

    return ret;
}

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
    unsigned int force = TRUE;
    n2a_flush_cache (&force);
    iniparser_freedict (ini);
}

#ifdef DEBUG
static void
alarm_handler (int sig)
{
    n2a_logger (LG_DEBUG, "Got SIGALRM");
    unsigned int force = TRUE;
    n2a_pop_all_cache ((void *)&force);
    signal (SIGALRM, alarm_handler);
}
#endif

void
n2a_init_cache (void)
{
    purge_cache = g_options.purge;
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
        n2a_logger (LG_CRIT, "cannot parse file: %s", g_options.cache_file);
        return;
    }
 
    if (!iniparser_find_entry (ini, "cache")) {
        n2a_logger (LG_CRIT, "invalid cache file! No 'cache' entry found");
        iniparser_set (ini, "cache", NULL);
    }
    int n = iniparser_getsecnkeys (ini, "cache");
    if (n > 0) {
        char **keys = iniparser_getseckeys (ini, "cache");
        /* sort the returned keys */
        qsort (keys, (size_t) n, sizeof (char *), compare);
        char *index_key = keys[n-1];
        /* then free the list although the doc says not to... */
        xfree (keys);
        char *m = strchr (index_key, '_');
        lastid = strtol (m+1, NULL, 10);
        n2a_logger (LG_INFO, "retrieved %d messages from cache", n/2);
    }

    dbsetup = TRUE;
    unsigned int force = FALSE;
#ifdef DEBUG
    signal (SIGALRM, alarm_handler);
    alarm (g_options.autopop);
#else
    time_t now = time (NULL);
    schedule_new_event(EVENT_USER_FUNCTION,
                       TRUE,
                       now+g_options.autopop,
                       FALSE,
                       g_options.autopop,
                       NULL,
                       TRUE,
                       (void *)n2a_pop_all_cache,
                       (void *)&force,
                       0);
    schedule_new_event(EVENT_USER_FUNCTION,
                       TRUE,
                       now+g_options.autoflush,
                       FALSE,
                       g_options.autoflush,
                       NULL,
                       TRUE,
                       (void *)n2a_flush_cache,
                       (void *)&force,
                       0);
#endif
}

void
n2a_flush_cache (void *pf)
{
    unsigned int force = *(int *)pf;
    unsigned int f = FALSE;
    time_t now = 0;
    if ((!dbsetup || g_options.autoflush < 0) && !force)
        return;
    /* i know... gotos are a mess, but here i wanna avoid this comparison when
     * it's useless */
    if (g_options.autoflush == 0)
        goto do_it;
    now = time (NULL);
    if ((int) difftime (now, last_flush) < g_options.autoflush && !force)
        return;

do_it:
    last_flush = now;
    FILE *db = fopen (g_options.cache_file, "w");
    if (db != NULL) {
        if (c_size == -10000)
            c_size = iniparser_getsecnkeys (ini, "cache");
        iniparser_dump_ini (ini, db);
        n2a_logger (LG_INFO, "syncing %d messages from cache to disk (into: '%s')",
                    c_size / 2, g_options.cache_file);

        fclose (db);
    } else {
        n2a_logger (LG_CRIT, "CACHE: flush error: %s", strerror (errno));
    }
#ifndef DEBUG
    now = time (NULL);
    schedule_new_event(EVENT_USER_FUNCTION,
                       TRUE,
                       now+g_options.autoflush,
                       FALSE,
                       g_options.autoflush,
                       NULL,
                       TRUE,
                       (void *)n2a_flush_cache,
                       (void *)&f,
                       0);
#endif
}

void
n2a_record_cache (const char *key, const char *message)
{
    char index[256];
    /* avoid caching the message twice */
    if (key == tkey && message == tmsg)
        return;
    int n = iniparser_getsecnkeys (ini, "cache") / 2;
    if (n > g_options.cache_size) {
        n2a_logger (LG_CRIT, "cache size exceded! Replacing oldest messages");
        char **keys = iniparser_getseckeys (ini, "cache");
        /* sort the returned keys */
        qsort (keys, (size_t) n, sizeof (char *), compare);
        char *index_key = keys[0];
        /* then free the list although the doc says not to... */
        xfree (keys);
        char *m = strchr (index_key, '_');
        int first = strtol (m+1, NULL, 10);
        snprintf (index, 256, "cache:key_%d", first);
        iniparser_unset (ini, index);
        snprintf (index, 256, "cache:message_%d", first);
        iniparser_unset (ini, index);
    }
    lastid++;
    snprintf (index, 256, "cache:key_%d", lastid);
    iniparser_set (ini, index, key);
    snprintf (index, 256, "cache:message_%d", lastid);
    iniparser_set (ini, index, message);
    n2a_logger (LG_INFO, "add message in cache: '%s' (%d)", key, lastid);
}

void
n2a_pop_all_cache (void *pf)
{
    time_t now = 0;
    unsigned int force = *(int *)pf;
    unsigned int f = FALSE;

    n2a_logger (LG_INFO, "pop lock: %s, %d", pop_lock ? "TRUE" : "FALSE",
    last_pop);

    if (pop_lock)
        return;

    if (g_options.autopop < 0 && !force)
        return;

    if (g_options.autopop == 0)
        goto do_it;

    now = time (NULL);
    if ((int) difftime (now, last_pop) < g_options.autopop && !force)
        return;

do_it:
    last_pop = now;
    int r = 0;
    c_size = iniparser_getsecnkeys (ini, "cache");
    int storm, cpt = 0;
    size_t l;
    char convert[128];
    if ((c_size / 2) <= 0)
        return;
    if (purge_cache) {
        purge_cache = FALSE;
        storm = c_size / 2;
        goto proceed;
    }
    if (g_options.flush > 0) {
        storm = xmin (c_size/2, g_options.flush);
        goto proceed;
    }
    snprintf (convert, 128, "%d", c_size);
    /* in order to avoid flush storming the AMQP bus, evaluate the number of
     * messages to flush */
    switch ((l = xstrlen (convert))) {
        case 1:
        case 2:
            storm = c_size/2;
            break;
        case 3:
            storm = c_size/4;
            break;
        case 4:
            storm = c_size/20;
            break;
        case 5:
            storm = c_size/200;
            break;
        default:
            storm = c_size/(20 * (10^(l-3)));
            break;
    }
proceed:
    n2a_logger (LG_INFO, "depiling %d/%d messages from cache", storm, c_size/2);

    pop_lock = TRUE;
    do {
        char **keys = iniparser_getseckeys (ini, "cache");
        /* sort the returned keys */
        qsort (keys, (size_t) c_size, sizeof (char *), compare);
        char *index_key = keys[0];
        /* then free the list although the doc says not to... */
        xfree (keys);
        char *m = strchr (index_key, '_');
        char index_message[256];
        snprintf (index_message, 256, "cache:message_%s", m+1);
        ini_lock = TRUE;
        char *key = iniparser_getstring (ini, index_key, NULL);
        char *message = iniparser_getstring (ini, index_message, NULL);
        tkey = key;
        tmsg = message;
        int r = amqp_publish (key, message);
        ini_lock = FALSE;
        if (r < 0) {
            n2a_logger (LG_CRIT, "error while purging cache from message '%s'", key);
            break;
        }
        iniparser_unset (ini, index_key);
        iniparser_unset (ini, index_message);
        cpt++;
        n2a_logger (LG_INFO, "cache successfuly purged from message '%s' (%d/%d)",
                    index_message, cpt, storm);
        if (cpt >= storm)
            break;
        usleep (g_options.rate);
    } while (r == 0 && ((c_size = iniparser_getsecnkeys (ini, "cache")) / 2) > 0);
    pop_lock = FALSE;
    if (r == 0 && (c_size / 2) == 0)
        lastid = 0;
    if (cpt >= storm)
        c_size = iniparser_getsecnkeys (ini, "cache");
    last_pop = time (NULL);
    if (c_size / 2 != 0)
        n2a_logger (LG_INFO, "There is still %d messages in cache", c_size / 2);
    else
        n2a_logger (LG_INFO, "No more messages in cache");
#ifdef DEBUG
    alarm (g_options.autopop);
#else
    schedule_new_event(EVENT_USER_FUNCTION,
                       TRUE,
                       last_pop+g_options.autopop,
                       FALSE,
                       g_options.autopop,
                       NULL,
                       TRUE,
                       (void *)n2a_pop_all_cache,
                       (void *)&f,
                       0);
#endif
}
