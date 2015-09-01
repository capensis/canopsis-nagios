.. _connectors_neb2amqp_setup:


Setup
=====

Requirements
------------

Debian like (Debian, Ubuntu ...):

.. code-block:: bash

    apt-get install build-essential git-core


Redhat like (Centos ..):

.. code-block:: bash

    yum groupinstall "Development Tools"
    yum install git-core


Download and Build
------------------

via git:
 
.. code-block:: bash

    git clone https://git.canopsis.net/canopsis-connectors/connector-neb2canopsis.git
    cd connector-neb2amqp
    make

via HTTP:

.. code-block:: bash

    wget https://git.canopsis.net/canopsis-connectors/connector-neb2canopsis/repository/archive.tar.gz?ref=master
    tar xfz connector-neb2amqp.tar.gz
    cd connector-neb2amqp
    make

Install
-------

Copy NEB in your Nagios/Icinga installation:

.. code-block:: bash

    sudo cp neb2amqp.o /usr/local/nagios/bin/


Configure
---------

Edit your Nagios/Icinga configuration (nagios.cfg):

.. code-block:: bash

    event_broker_options=-1
    broker_module=/usr/local/nagios/bin/neb2amqp.o name=Central host=localhost

Options:

.. code-block:: bash

    host =              AMQP Server (127.0.0.1)
    port =              AMQP Port (5672)
    userid =            AMQP login (guest)
    password =          AMQP password (guest)
    virtual_host =      AMQP Virtual host (canopsis)
    exchange_name =     AMQP Exchange (canopsis.events)
    name =              Poller name (Central)
    connector =         Connector name (nagios) (you can type "icinga" for icinga)
    max_size =          Maximum message size to send to the AMQP bus (8192)
    cache_file =        File in which faulty messages are stored (/usr/local/nagios/var/canopsis.cache)
                        (note: if we cannot read/create the file, the cache will
                        only run in memory)
    cache_size =        Number of messages to store in cache (1000)
    autosync =          Delay in seconds between two automatic sync of the cache into 'cache_file'.
                        If < 0 disable autosync (note: the cache will always be stored when the module
                        is unloaded). If = 0 cache every time (this is not recommended as it may consumes
                        lot of I/O) (default: 60)
    autoflush =         Delay in seconds between two automatic flush of the cache into the AMQP bus
                        if it is available (60)
    rate =              Delay in ms between two messages when depiling (5)
    flush =             Number of messages to send when depiling (-1: means it is calculated at runtime)
    purge =             If 1, purge cache at startup. /!\ This will increase Nagios' startup time. (default: 0)

    hostgroups =        If 1, send host groups on host-check events (default: 0)
    servicegroups =     If 1, send service groups on service-check events (default: 0)
    acknowledgement =   If 1, handle acknowledgement events (default: 0)
    downtime =          If 1, handle downtime events (default: 0)
    custom_variables =  If 1, add Nagios macros to event (default: 0)
    urls =              If 1, add action_url and notes_url to event (default: 0)

    amqp_wait_time =    Number of seconds before a reconnection to AMQP

Tools
-----

Other tools are available in the folder ``contrib``.
