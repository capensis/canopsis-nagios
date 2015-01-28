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

#ifndef nagios_h
#define nagios_h

#include "config.h"

#define NSCORE
#ifdef BUILD_NAGIOS_4X
#include "nagios-4x/objects.h"
#include "nagios-4x/nagios.h"
#include "nagios-4x/nebstructs.h"
#include "nagios-4x/broker.h"
#include "nagios-4x/nebmodules.h"
#include "nagios-4x/nebcallbacks.h"
#else
#include "nagios/objects.h"
#include "nagios/nagios.h"
#include "nagios/nebstructs.h"
#include "nagios/broker.h"
#include "nagios/nebmodules.h"
#include "nagios/nebcallbacks.h"
#endif

#endif // nagios_h

