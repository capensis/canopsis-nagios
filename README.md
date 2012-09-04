# Nagios or Icinga Event broker for Canopsis #

## Requirements ##

Debian like (Debian, Ubuntu ...):

    apt-get install build-essential git-core


Redhat like (Centos ..):

    yum groupinstall "Development Tools"
    yum install kernel-devel kernel-headers git-core


## Download and Build ##

    git https://github.com/capensis/canopsis-nagios.git
    cd canopsis-nagios
    make

## Install ##

Copy NEB in your Nagios/Icinga installation:

    sudo cp src/neb2amqp.o /usr/local/nagios/bin/


## Configure ##

Edit your Nagios/Icinga configuration (nagios.cfg):

    event_broker_options=-1
    broker_module=/usr/local/nagios/bin/neb2amqp.o name=Central host=localhost

Options:

    host =          AMQP Server (127.0.0.1)
    port =          AMQP Port (5672)
    userid =        AMQP login (guest)
    password =      AMQP password (guest)
    virtual_host =  AMQP Virtual host (canopsis)
    exchange_name = AMQP Exchange (canopsis.events)
    name =          Poller name (Central)
    connector =     Connector name (nagios) (you can type "icinga" for icinga)
