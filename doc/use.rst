.. _connectors_neb2amqp_use:


Use
===

Start / Stop
------------

As this connector is a nativ NEB, **start** and **stop** actions depend on monitoring master process (Nagios, Icinga, ...).

So to start it 

.. code-block:: bash

    service nagios|icinga start

And to stop it 

.. code-block:: bash

    service nagios|icinga stop


Troubleshooting / Log
---------------------

When master process is started, you'll get some log messages

You have to wait for the `initialized successfully.` message.

.. code-block:: bash

	neb2amqp: Setting hostname to localhost
	neb2amqp: Setting userid to cpsrabbit
	neb2amqp: Setting password to canopsis
	neb2amqp: Setting hostgroups to 'true'
	neb2amqp: Setting custom_variables to 'true'
	neb2amqp: Setting servicegroups to 'true'
	neb2amqp: Setting urls to 'true'
	neb2amqp: NEB2amqp 0.6-fifo by Capensis. (connector: nagios)
	neb2amqp: Please visit us at http://www.canopsis.org/
	neb2amqp: Initialize FIFO: /tmp/neb2amqp.cache (maximum size: 10000
	neb2amqp: FIFO: Load events from file
	neb2amqp: FIFO: Open fifo file '/tmp/neb2amqp.cache'
	neb2amqp: FIFO: File successfully opened
	neb2amqp: FIFO: Push (0)
	neb2amqp: FIFO: Close file
	neb2amqp: FIFO: 1 events loaded
	neb2amqp: AMQP: Re-connect to amqp ...
	neb2amqp: AMQP: Init connection
	neb2amqp: AMQP: Creating socket
	neb2amqp: AMQP: Opening socket
	neb2amqp: AMQP: Login
	neb2amqp: AMQP: Open channel
	neb2amqp: AMQP: Successfully connected
	neb2amqp: Register callbacks
	neb2amqp: successfully finished initialization
	Event broker module '/usr/lib/icinga/neb2amqp.o' initialized successfully.


If NEB cannot connect to amqp bus, It will buffer messages into a file and a log message will appear in the master process's log

.. code-block:: bash

	neb2amqp: AMQP: Re-connect to amqp ...
	neb2amqp: AMQP: Init connection
	neb2amqp: AMQP: Creating socket
	neb2amqp: AMQP: Opening socket
	neb2amqp: AMQP: Opening socket: (unknown error)
	neb2amqp: FIFO: Sync 285 events to file
	neb2amqp: FIFO: Remove fifo file
	neb2amqp: FIFO: Open fifo file '/tmp/neb2amqp.cache'
	neb2amqp: FIFO: File successfully opened
	neb2amqp: FIFO: 285 events written to file


When connection recovers, you'll see see the queue's shift in master process's log

.. code-block:: bash

	neb2amqp: AMQP: Re-connect to amqp ...
	neb2amqp: AMQP: Init connection
	neb2amqp: AMQP: Creating socket
	neb2amqp: AMQP: Opening socket
	neb2amqp: AMQP: Login
	neb2amqp: AMQP: Open channel
	neb2amqp: AMQP: Successfully connected
	neb2amqp: AMQP: Shift queue, size: 734
	neb2amqp: AMQP: 734/734 events shifted from Queue, new size: 0
	neb2amqp: FIFO: Sync 0 events to file
	neb2amqp: FIFO: Remove fifo file
	neb2amqp: FIFO: Open fifo file '/tmp/neb2amqp.cache'
	neb2amqp: FIFO: File successfully opened
	neb2amqp: FIFO: 0 events written to file

