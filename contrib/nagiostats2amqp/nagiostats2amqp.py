#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

from collections import OrderedDict
from socket import gethostname

from kombu import Connection
from kombu.pools import producers

import subprocess
import time


DAEMON_NAME = 'nagiostats2amqp'


class NagiosStats(object):
     def __init__(self, host='localhost', port=5672, userid='guest', password='guest', virtual_host='canopsis', exchange='canopsis.events'):
          self.amqp_uri = 'amqp://{0}:{1}@{2}:{3}/{4}'.format(
               userid,
               password,
               host,
               port,
               virtual_host
          )

          self.exchange = exchange

          # Generate metrics table
          self.metrics = OrderedDict()

          self.metrics['PROGRUNTIMETT'] = None         # time Nagios process has been running
          self.metrics['STATUSFILEAGETT'] = None       # age of status data file
          self.metrics['NAGIOSVERPID'] = None          # Nagios version and PID

          self.metrics['TOTCMDBUF'] = None             # total number of external command buffer slots available
          self.metrics['USEDCMDBUF'] = None            # number of external command buffer slots currently in use
          self.metrics['HIGHCMDBUF'] = None            # highest number of external command buffer slots ever in use

          self.metrics['NUMSERVICES'] = None           # total number of services
          self.metrics['NUMHOSTS'] = None              # total number of hosts

          self.metrics['NUMSVCOK'] = None              # number of services OK
          self.metrics['NUMSVCWARN'] = None            # number of services WARNING
          self.metrics['NUMSVCUNKN'] = None            # number of services UNKNOWN
          self.metrics['NUMSVCCRIT'] = None            # number of services CRITICAL
          self.metrics['NUMSVCPROB'] = None            # number of services problems (WARNING, UNKNOWN, CRITICAL)
          self.metrics['NUMSVCCHECKED'] = None         # number of services that have been checked since start
          self.metrics['NUMSVCSCHEDULED'] = None       # number of services that are currently scheduled to be checked
          self.metrics['NUMSVCFLAPPING'] = None        # number of services that are currently flapping
          self.metrics['NUMSVCDOWNTIME'] = None        # number of services that are currently in downtime

          self.metrics['NUMHSTUP'] = None              # number of hosts UP
          self.metrics['NUMHSTDOWN'] = None            # number of hosts DOWN
          self.metrics['NUMHSTUNR'] = None             # number of hosts UNREACHABLE
          self.metrics['NUMHSTPROB'] = None            # number of host problems (DOWN or UNREACHABLE)
          self.metrics['NUMHSTCHECKED'] = None         # number of hosts that have been checked since start
          self.metrics['NUMHSTSCHEDULED'] = None       # number of hosts that are currently scheduled to be checked
          self.metrics['NUMHSTFLAPPING'] = None        # number of hosts that are currently flapping
          self.metrics['NUMHSTDOWNTIME'] = None        # number of hosts that are currently in downtime

          self.metrics['NUMHSTACTCHK1M'] = None        # number of hosts actively checked in last 1 minutes
          self.metrics['NUMHSTACTCHK5M'] = None        # number of hosts actively checked in last 5 minutes
          self.metrics['NUMHSTACTCHK15M'] = None       # number of hosts actively checked in last 15 minutes
          self.metrics['NUMHSTACTCHK60M'] = None       # number of hosts actively checked in last 60 minutes

          self.metrics['NUMHSTPSVCHK1M'] = None        # number of hosts passively checked in last 1 minutes
          self.metrics['NUMHSTPSVCHK5M'] = None        # number of hosts passively checked in last 5 minutes
          self.metrics['NUMHSTPSVCHK15M'] = None       # number of hosts passively checked in last 15 minutes
          self.metrics['NUMHSTPSVCHK60M'] = None       # number of hosts passively checked in last 60 minutes

          self.metrics['NUMSVCACTCHK1M'] = None        # number of services actively checked in last 1 minutes
          self.metrics['NUMSVCACTCHK5M'] = None        # number of services actively checked in last 5 minutes
          self.metrics['NUMSVCACTCHK15M'] = None       # number of services actively checked in last 15 minutes
          self.metrics['NUMSVCACTCHK60M'] = None       # number of services actively checked in last 60 minutes

          self.metrics['NUMSVCPSVCHK1M'] = None        # number of services passively checked in last 1 minutes
          self.metrics['NUMSVCPSVCHK5M'] = None        # number of services passively checked in last 5 minutes
          self.metrics['NUMSVCPSVCHK15M'] = None       # number of services passively checked in last 15 minutes
          self.metrics['NUMSVCPSVCHK60M'] = None       # number of services passively checked in last 60 minutes

          self.metrics['MINACTSVCLAT'] = None          # MIN active service check latency (ms)
          self.metrics['MAXACTSVCLAT'] = None          # MAX active service check latency (ms)
          self.metrics['AVGACTSVCLAT'] = None          # AVG active service check latency (ms)

          self.metrics['MINACTSVCEXT'] = None          # MIN active service check execution time (ms)
          self.metrics['MAXACTSVCEXT'] = None          # MAX active service check execution time (ms)
          self.metrics['AVGACTSVCEXT'] = None          # AVG active service check execution time (ms)

          self.metrics['MINACTSVCPSC'] = None          # MIN active service check % state change
          self.metrics['MAXACTSVCPSC'] = None          # MAX active service check % state change
          self.metrics['AVGACTSVCPSC'] = None          # AVG active service check % state change

          self.metrics['MINPSVSVCLAT'] = None          # MIN passive service check latency (ms)
          self.metrics['MAXPSVSVCLAT'] = None          # MAX passive service check latency (ms)
          self.metrics['AVGPSVSVCLAT'] = None          # AVG passive service check latency (ms)

          self.metrics['MINPSVSVCPSC'] = None          # MIN passive service check % state change
          self.metrics['MAXPSVSVCPSC'] = None          # MAX passive service check % state change
          self.metrics['AVGPSVSVCPSC'] = None          # AVG passive service check % state change

          self.metrics['MINSVCPSC'] = None             # MIN service check % state change
          self.metrics['MAXSVCPSC'] = None             # MAX service check % state change
          self.metrics['AVGSVCPSC'] = None             # AVG service check % state change

          self.metrics['MINACTHSTLAT'] = None          # MIN active host check latency (ms)
          self.metrics['MAXACTHSTLAT'] = None          # MAX active host check latency (ms)
          self.metrics['AVGACTHSTLAT'] = None          # AVG active host check latency (ms)

          self.metrics['MINACTHSTEXT'] = None          # MIN active host check execution time (ms)
          self.metrics['MAXACTHSTEXT'] = None          # MAX active host check execution time (ms)
          self.metrics['AVGACTHSTEXT'] = None          # AVG active host check execution time (ms)

          self.metrics['MINACTHSTPSC'] = None          # MIN active host check % state change
          self.metrics['MAXACTHSTPSC'] = None          # MAX active host check % state change
          self.metrics['AVGACTHSTPSC'] = None          # AVG active host check % state change

          self.metrics['MINPSVHSTLAT'] = None          # MIN passive host check latency (ms)
          self.metrics['MAXPSVHSTLAT'] = None          # MAX passive host check latency (ms)
          self.metrics['AVGPSVHSTLAT'] = None          # AVG passive host check latency (ms)

          self.metrics['MINPSVHSTPSC'] = None          # MIN passive host check % state change
          self.metrics['MAXPSVHSTPSC'] = None          # MAX passive host check % state change
          self.metrics['AVGPSVHSTPSC'] = None          # AVG passive host check % state change

          self.metrics['MINHSTPSC'] = None             # MIN host check % state change
          self.metrics['MAXHSTPSC'] = None             # MAX host check % state change
          self.metrics['AVGHSTPSC'] = None             # AVG host check % state change

          self.metrics['NUMACTHSTCHECKS1M'] = None     # number of total active host checks occurring in last 1 minutes
          self.metrics['NUMACTHSTCHECKS5M'] = None     # number of total active host checks occurring in last 5 minutes
          self.metrics['NUMACTHSTCHECKS15M'] = None    # number of total active host checks occurring in last 15 minutes

          self.metrics['NUMOACTHSTCHECKS1M'] = None    # number of on-demand active host checks occurring in last 1 minutes
          self.metrics['NUMOACTHSTCHECKS5M'] = None    # number of on-demand active host checks occurring in last 5 minutes
          self.metrics['NUMOACTHSTCHECKS15M'] = None   # number of on-demand active host checks occurring in last 15 minutes

          self.metrics['NUMCACHEDHSTCHECKS1M'] = None  # number of cached host checks occurring in last 1 minutes
          self.metrics['NUMCACHEDHSTCHECKS5M'] = None  # number of cached host checks occurring in last 5 minutes
          self.metrics['NUMCACHEDHSTCHECKS15M'] = None # number of cached host checks occurring in last 15 minutes

          self.metrics['NUMSACTHSTCHECKS1M'] = None    # number of scheduled active host checks occurring in last 1 minutes
          self.metrics['NUMSACTHSTCHECKS5M'] = None    # number of scheduled active host checks occurring in last 5 minutes
          self.metrics['NUMSACTHSTCHECKS15M'] = None   # number of scheduled active host checks occurring in last 15 minutes

          self.metrics['NUMPARHSTCHECKS1M'] = None     # number of parallel host checks occurring in last 1 minutes
          self.metrics['NUMPARHSTCHECKS5M'] = None     # number of parallel host checks occurring in last 5 minutes
          self.metrics['NUMPARHSTCHECKS15M'] = None    # number of parallel host checks occurring in last 15 minutes

          self.metrics['NUMSERHSTCHECKS1M'] = None     # number of serial host checks occurring in last 1 minutes
          self.metrics['NUMSERHSTCHECKS5M'] = None     # number of serial host checks occurring in last 5 minutes
          self.metrics['NUMSERHSTCHECKS15M'] = None    # number of serial host checks occurring in last 15 minutes

          self.metrics['NUMPSVHSTCHECKS1M'] = None     # number of passive host checks occurring in last 1 minutes
          self.metrics['NUMPSVHSTCHECKS5M'] = None     # number of passive host checks occurring in last 5 minutes
          self.metrics['NUMPSVHSTCHECKS15M'] = None    # number of passive host checks occurring in last 15 minutes

          self.metrics['NUMACTSVCCHECKS1M'] = None     # number of total active service checks occurring in last 1 minutes
          self.metrics['NUMACTSVCCHECKS5M'] = None     # number of total active service checks occurring in last 5 minutes
          self.metrics['NUMACTSVCCHECKS15M'] = None    # number of total active service checks occurring in last 15 minutes

          self.metrics['NUMOACTSVCCHECKS1M'] = None    # number of on-demand active service checks occurring in last 1 minutes
          self.metrics['NUMOACTSVCCHECKS5M'] = None    # number of on-demand active service checks occurring in last 5 minutes
          self.metrics['NUMOACTSVCCHECKS15M'] = None   # number of on-demand active service checks occurring in last 15 minutes

          self.metrics['NUMCACHEDSVCCHECKS1M'] = None  # number of cached service checks occurring in last 1 minutes
          self.metrics['NUMCACHEDSVCCHECKS5M'] = None  # number of cached service checks occurring in last 5 minutes
          self.metrics['NUMCACHEDSVCCHECKS15M'] = None # number of cached service checks occurring in last 15 minutes

          self.metrics['NUMSACTSVCCHECKS1M'] = None    # number of scheduled active service checks occurring in last 1 minutes
          self.metrics['NUMSACTSVCCHECKS5M'] = None    # number of scheduled active service checks occurring in last 5 minutes
          self.metrics['NUMSACTSVCCHECKS15M'] = None   # number of scheduled active service checks occurring in last 15 minutes

          self.metrics['NUMPSVSVCCHECKS1M'] = None     # number of passive service checks occurring in last 1 minutes
          self.metrics['NUMPSVSVCCHECKS5M'] = None     # number of passive service checks occurring in last 5 minutes
          self.metrics['NUMPSVSVCCHECKS15M'] = None    # number of passive service checks occurring in last 15 minutes

          self.metrics['NUMEXTCMDS1M'] = None          # number of external commands processed in last 1 minutes
          self.metrics['NUMEXTCMDS5M'] = None          # number of external commands processed in last 5 minutes
          self.metrics['NUMEXTCMDS15M'] = None         # number of external commands processed in last 15 minutes

     def run(self):
          # Fetch all results
          p = subprocess.Popen([
               '/usr/local/nagios/bin/nagiostats',
               '-m',
               '-d',
               ','.join(self.metrics.keys())
          ], stdout=subprocess.PIPE)

          # Put results in dict
          results = p.stdout.readlines()
          i_result = iter(results)

          for key in self.metrics.keys():
               try:
                    result = i_result.next().strip()

               except StopIteration:
                    break

               self.metrics[key] = result

          # Generate perfdata
          perf_data_array = [
               {'type': 'GAUGE', 'metric': 'nagios_runtime',                         'value': int(self.metrics['PROGRUNTIMETT']),                'unit': 's'},
               {'type': 'GAUGE', 'metric': 'statusfile_age',                         'value': int(self.metrics['STATUSFILEAGETT']),              'unit': 's'},

               {'type': 'GAUGE', 'metric': 'total_cmd_buf',                          'value': int(self.metrics['TOTCMDBUF']),                    'unit': 'cmdbuf'},
               {'type': 'GAUGE', 'metric': 'used_cmd_buf',                           'value': int(self.metrics['USEDCMDBUF']),                   'unit': 'cmdbuf'},
               {'type': 'GAUGE', 'metric': 'high_cmd_buf',                           'value': int(self.metrics['HIGHCMDBUF']),                   'unit': 'cmdbuf'},

               {'type': 'GAUGE', 'metric': 'nb_services',                            'value': int(self.metrics['NUMSERVICES']),                  'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_hosts',                               'value': int(self.metrics['NUMHOSTS']),                     'unit': 'host'},

               {'type': 'GAUGE', 'metric': 'nb_services_ok',                         'value': int(self.metrics['NUMSVCOK']),                     'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_warning',                    'value': int(self.metrics['NUMSVCWARN']),                   'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_unknown',                    'value': int(self.metrics['NUMSVCUNKN']),                   'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_critical',                   'value': int(self.metrics['NUMSVCCRIT']),                   'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_problem',                    'value': int(self.metrics['NUMSVCPROB']),                   'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_checked',                    'value': int(self.metrics['NUMSVCCHECKED']),                'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_scheduled',                  'value': int(self.metrics['NUMSVCSCHEDULED']),              'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_flapping',                   'value': int(self.metrics['NUMSVCFLAPPING']),               'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_downtime',                   'value': int(self.metrics['NUMSVCDOWNTIME']),               'unit': 'service'},

               {'type': 'GAUGE', 'metric': 'nb_hosts_up',                            'value': int(self.metrics['NUMHSTUP']),                     'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_down',                          'value': int(self.metrics['NUMHSTDOWN']),                   'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_unreachable',                   'value': int(self.metrics['NUMHSTUNR']),                    'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_problem',                       'value': int(self.metrics['NUMHSTPROB']),                   'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_checked',                       'value': int(self.metrics['NUMHSTCHECKED']),                'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_scheduled',                     'value': int(self.metrics['NUMHSTSCHEDULED']),              'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_flapping',                      'value': int(self.metrics['NUMHSTFLAPPING']),               'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_downtime',                      'value': int(self.metrics['NUMHSTDOWNTIME']),               'unit': 'host'},

               {'type': 'GAUGE', 'metric': 'nb_hosts_actively_checked_1m',           'value': int(self.metrics['NUMHSTACTCHK1M']),               'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_actively_checked_5m',           'value': int(self.metrics['NUMHSTACTCHK5M']),               'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_actively_checked_15m',          'value': int(self.metrics['NUMHSTACTCHK15M']),              'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_actively_checked_60m',          'value': int(self.metrics['NUMHSTACTCHK60M']),              'unit': 'host'},

               {'type': 'GAUGE', 'metric': 'nb_hosts_passively_checked_1m',          'value': int(self.metrics['NUMHSTPSVCHK1M']),               'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_passively_checked_5m',          'value': int(self.metrics['NUMHSTPSVCHK5M']),               'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_passively_checked_15m',         'value': int(self.metrics['NUMHSTPSVCHK15M']),              'unit': 'host'},
               {'type': 'GAUGE', 'metric': 'nb_hosts_passively_checked_60m',         'value': int(self.metrics['NUMHSTPSVCHK60M']),              'unit': 'host'},              

               {'type': 'GAUGE', 'metric': 'nb_services_actively_checked_1m',        'value': int(self.metrics['NUMSVCACTCHK1M']),               'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_actively_checked_5m',        'value': int(self.metrics['NUMSVCACTCHK5M']),               'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_actively_checked_15m',       'value': int(self.metrics['NUMSVCACTCHK15M']),              'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_actively_checked_60m',       'value': int(self.metrics['NUMSVCACTCHK60M']),              'unit': 'service'},

               {'type': 'GAUGE', 'metric': 'nb_services_passively_checked_1m',       'value': int(self.metrics['NUMSVCPSVCHK1M']),               'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_passively_checked_5m',       'value': int(self.metrics['NUMSVCPSVCHK5M']),               'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_passively_checked_15m',      'value': int(self.metrics['NUMSVCPSVCHK15M']),              'unit': 'service'},
               {'type': 'GAUGE', 'metric': 'nb_services_passively_checked_60m',      'value': int(self.metrics['NUMSVCPSVCHK60M']),              'unit': 'service'},

               {'type': 'GAUGE', 'metric': 'min_active_service_check_latency',       'value': float(self.metrics['MINACTSVCLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_active_service_check_latency',       'value': float(self.metrics['MAXACTSVCLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_active_service_check_latency',       'value': float(self.metrics['AVGACTSVCLAT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_active_service_check_exectime',      'value': float(self.metrics['MINACTSVCEXT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_active_service_check_exectime',      'value': float(self.metrics['MAXACTSVCEXT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_active_service_check_exectime',      'value': float(self.metrics['AVGACTSVCEXT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_active_service_check_statechange',   'value': int(self.metrics['MINACTSVCPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_active_service_check_statechange',   'value': int(self.metrics['MAXACTSVCPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_active_service_check_statechange',   'value': int(self.metrics['AVGACTSVCPSC']),                 'unit': '%'},

               {'type': 'GAUGE', 'metric': 'min_passive_service_check_latency',      'value': float(self.metrics['MINPSVSVCLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_passive_service_check_latency',      'value': float(self.metrics['MAXPSVSVCLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_passive_service_check_latency',      'value': float(self.metrics['AVGPSVSVCLAT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_passive_service_check_statechange',  'value': int(self.metrics['MINPSVSVCPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_passive_service_check_statechange',  'value': int(self.metrics['MAXPSVSVCPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_passive_service_check_statechange',  'value': int(self.metrics['AVGPSVSVCPSC']),                 'unit': '%'},

               {'type': 'GAUGE', 'metric': 'min_service_check_statechange',          'value': int(self.metrics['MINSVCPSC']),                    'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_service_check_statechange',          'value': int(self.metrics['MAXSVCPSC']),                    'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_service_check_statechange',          'value': int(self.metrics['AVGSVCPSC']),                    'unit': '%'},

               {'type': 'GAUGE', 'metric': 'min_active_host_check_latency',          'value': float(self.metrics['MINACTHSTLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_active_host_check_latency',          'value': float(self.metrics['MAXACTHSTLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_active_host_check_latency',          'value': float(self.metrics['AVGACTHSTLAT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_active_host_check_exectime',         'value': float(self.metrics['MINACTHSTEXT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_active_host_check_exectime',         'value': float(self.metrics['MAXACTHSTEXT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_active_host_check_exectime',         'value': float(self.metrics['AVGACTHSTEXT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_active_host_check_statechange',      'value': int(self.metrics['MINACTHSTPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_active_host_check_statechange',      'value': int(self.metrics['MAXACTHSTPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_active_host_check_statechange',      'value': int(self.metrics['AVGACTHSTPSC']),                 'unit': '%'},

               {'type': 'GAUGE', 'metric': 'min_passive_host_check_latency',         'value': float(self.metrics['MINPSVHSTLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'max_passive_host_check_latency',         'value': float(self.metrics['MAXPSVHSTLAT']) / 1000.0,      'unit': 's'},
               {'type': 'GAUGE', 'metric': 'avg_passive_host_check_latency',         'value': float(self.metrics['AVGPSVHSTLAT']) / 1000.0,      'unit': 's'},

               {'type': 'GAUGE', 'metric': 'min_passive_host_check_statechange',     'value': int(self.metrics['MINPSVHSTPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_passive_host_check_statechange',     'value': int(self.metrics['MAXPSVHSTPSC']),                 'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_passive_host_check_statechange',     'value': int(self.metrics['AVGPSVHSTPSC']),                 'unit': '%'},

               {'type': 'GAUGE', 'metric': 'min_host_check_statechange',             'value': int(self.metrics['MINHSTPSC']),                    'unit': '%'},
               {'type': 'GAUGE', 'metric': 'max_host_check_statechange',             'value': int(self.metrics['MAXHSTPSC']),                    'unit': '%'},
               {'type': 'GAUGE', 'metric': 'avg_host_check_statechange',             'value': int(self.metrics['AVGHSTPSC']),                    'unit': '%'},

               {'type': 'GAUGE', 'metric': 'nb_total_active_host_checks_1m',         'value': int(self.metrics['NUMACTHSTCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_total_active_host_checks_5m',         'value': int(self.metrics['NUMACTHSTCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_total_active_host_checks_15m',        'value': int(self.metrics['NUMACTHSTCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_host_checks_1m',      'value': int(self.metrics['NUMOACTHSTCHECKS1M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_host_checks_5m',      'value': int(self.metrics['NUMOACTHSTCHECKS5M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_host_checks_15m',     'value': int(self.metrics['NUMOACTHSTCHECKS15M']),          'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_cached_host_checks_1m',               'value': int(self.metrics['NUMCACHEDHSTCHECKS1M']),         'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_cached_host_checks_5m',               'value': int(self.metrics['NUMCACHEDHSTCHECKS5M']),         'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_cached_host_checks_15m',              'value': int(self.metrics['NUMCACHEDHSTCHECKS15M']),        'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_host_checks_1m',     'value': int(self.metrics['NUMSACTHSTCHECKS1M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_host_checks_5m',     'value': int(self.metrics['NUMSACTHSTCHECKS5M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_host_checks_15m',    'value': int(self.metrics['NUMSACTHSTCHECKS15M']),          'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_parallel_host_checks_1m',             'value': int(self.metrics['NUMPARHSTCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_parallel_host_checks_5m',             'value': int(self.metrics['NUMPARHSTCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_parallel_host_checks_15m',            'value': int(self.metrics['NUMPARHSTCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_serial_host_checks_1m',               'value': int(self.metrics['NUMSERHSTCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_serial_host_checks_5m',               'value': int(self.metrics['NUMSERHSTCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_serial_host_checks_15m',              'value': int(self.metrics['NUMSERHSTCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_passive_host_checks_1m',              'value': int(self.metrics['NUMPSVHSTCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_passive_host_checks_5m',              'value': int(self.metrics['NUMPSVHSTCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_passive_host_checks_15m',             'value': int(self.metrics['NUMPSVHSTCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_total_active_service_checks_1m',      'value': int(self.metrics['NUMACTSVCCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_total_active_service_checks_5m',      'value': int(self.metrics['NUMACTSVCCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_total_active_service_checks_15m',     'value': int(self.metrics['NUMACTSVCCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_service_checks_1m',   'value': int(self.metrics['NUMOACTSVCCHECKS1M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_service_checks_5m',   'value': int(self.metrics['NUMOACTSVCCHECKS5M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_ondemand_active_service_checks_15m',  'value': int(self.metrics['NUMOACTSVCCHECKS15M']),          'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_cached_service_checks_1m',            'value': int(self.metrics['NUMCACHEDSVCCHECKS1M']),         'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_cached_service_checks_5m',            'value': int(self.metrics['NUMCACHEDSVCCHECKS5M']),         'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_cached_service_checks_15m',           'value': int(self.metrics['NUMCACHEDSVCCHECKS15M']),        'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_service_checks_1m',  'value': int(self.metrics['NUMSACTSVCCHECKS1M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_service_checks_5m',  'value': int(self.metrics['NUMSACTSVCCHECKS5M']),           'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_scheduled_active_service_checks_15m', 'value': int(self.metrics['NUMSACTSVCCHECKS15M']),          'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_passive_service_checks_1m',           'value': int(self.metrics['NUMPSVSVCCHECKS1M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_passive_service_checks_5m',           'value': int(self.metrics['NUMPSVSVCCHECKS5M']),            'unit': 'check'},
               {'type': 'GAUGE', 'metric': 'nb_passive_service_checks_15m',          'value': int(self.metrics['NUMPSVSVCCHECKS15M']),           'unit': 'check'},

               {'type': 'GAUGE', 'metric': 'nb_command_1m',                          'value': int(self.metrics['NUMEXTCMDS1M']),                 'unit': 'command'},
               {'type': 'GAUGE', 'metric': 'nb_command_5m',                          'value': int(self.metrics['NUMEXTCMDS5M']),                 'unit': 'command'},
               {'type': 'GAUGE', 'metric': 'nb_command_15m',                         'value': int(self.metrics['NUMEXTCMDS15M']),                'unit': 'command'},
          ]

          # Generate event
          event = {
               'time':             int(time.time()),
               'event_type':       'log',
               'source_type':      'resource',

               'connector':        'nagiostats',
               'connector_name':   'nagiostats2amqp',
               'component':        gethostname(),
               'resource':         'nagiostats',

               'state':            0,
               'output':           self.metrics['NAGIOSVERPID'],

               'perf_data_array':  perf_data_array,

               'tags':             ['nagios', 'nagiostats']
          }

          rk = '{0}.{1}.{2}.{3}.{4}'.format(
               event['connector'],
               event['connector_name'],
               event['event_type'],
               event['source_type'],
               event['component']
          )

          if event['source_type'] == 'resource':
               rk = '{0}.{1}'.format(rk, event['resource'])

          # Send event
          with Connection(self.amqp_uri) as conn:
               with producers[conn].acquire(block=True) as producer:
                    producer.publish(
                         event,
                         serializer='json',
                         exchange=self.exchange,
                         routing_key=rk
                    )


if __name__ == "__main__":
     runner = NagiosStats()
     runner.run()
