#!/usr/sbin/svdef
name = "getty-${TTY}"
before_run = "export TERM=xterm"
run = "/usr/libexec/getty Pc ${TTY}"
finish = "utmpset -w ${TTY}"
