.include <src.opts.mk>

CONFGROUPS= SVDEFS

SVDEFS=        devd syslogd dhcpcd ntpd getty.tpl
SVDEFSDIR=     /etc/run
SVDEFSMODE=    755

SCRIPTSDIR= ${LIBEXECDIR}
SCRIPTS= dank-init-1 dank-init-2 dank-init-3 dank-init-ctrlaltdel

SYMLINKS= \
	../../usr/libexec/dank-init-1 /etc/runit/1 \
	../../usr/libexec/dank-init-2 /etc/runit/2 \
	../../usr/libexec/dank-init-3 /etc/runit/3 \
	../../usr/libexec/dank-init-ctrlaltdel /etc/runit/ctrlaltdel \
	../../var/run/runit-reboot /etc/runit/reboot \
	../../var/run/runit-stopit /etc/runit/stopit

.include <bsd.prog.mk>
