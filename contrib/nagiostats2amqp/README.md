# Nagiostats connector #

The command ``nagiostats`` is used to retrieve statistics about the supervised infrastructure.
This connector allows you to format and send those informations to Canopsis.

## Dependencies ##

Install the dependencies via ``pip`` :

```
# pip install kombu
```

## Installation ##

Add a new crontab entry with the path, for example :

```
* * * * * /path/to/nagiostats2amqp.py
```

This will execute the script every minutes.
