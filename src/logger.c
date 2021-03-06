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
// check_mk is free software;  you can redistribute it and/or modify it
// under the  terms of the  GNU General Public License  as published by
// the Free Software Foundation in version 2.  check_mk is  distributed
// in the hope that it will be useful, but WITHOUT ANY WARRANTY;  with-
// out even the implied warranty of  MERCHANTABILITY  or  FITNESS FOR A
// PARTICULAR PURPOSE. See the  GNU General Public License for more de-
// ails.  You should have  received  a copy of the  GNU  General Public
// License along with GNU Make; see the file  COPYING.  If  not,  write
// to the Free Software Foundation, Inc., 51 Franklin St,  Fifth Floor,
// Boston, MA 02110-1301 USA.

#include "module.h"
#include "logger.h"
#include "nagios.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "xutils.h"

extern struct options g_options;

void n2a_logger (int priority, const char *loginfo, ...)
{
    char *format = NULL;
    char *buffer = NULL;
    va_list ap;

    if (priority != 1 || g_options.log_level > 0)
    {
        priority = LG_INFO;
    }

    asprintf (&format, "neb2amqp: %s", loginfo);

    va_start (ap, loginfo);
    vasprintf (&buffer, format, ap);
    va_end (ap);

    write_to_all_logs (buffer, priority);
}
