# DAEMONS

## Overview:

A daemon is a process with the following characteristics:
* Long-lived. Often, a daemon is created at system startup and runs until the system is shut down.
* Running in the background and has no controlling terminal. Kernel never automatically generates any job-control
or terminal-related signals (such as SIGINT, SIGTSTP, and SIGHUP) for a daemon

Daemons are written to carry out specific tasks.

## Ways to Create a Daemon

Here are some ways I’d recommend creating a daemon. This isn’t an exhaustive list.
* Programmatically, using glibc (daemon() function) or using fork() (mainly)
* Using a systemd service.