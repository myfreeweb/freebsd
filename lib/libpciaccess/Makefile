LIB=		pciaccess
SHLIB_MAJOR=	1
LIBADD=	z
WARNS=	0

PCIACCESSSRC=	${SRCTOP}/contrib/libpciaccess

.PATH:		${PCIACCESSSRC} ${PCIACCESSSRC}/src ${PCIACCESSSRC}/include

CFLAGS+=	-flto \
		-DPCIIDS_PATH=\"/usr/share/pciids/pci.ids\" \
		-DHAVE_ZLIB=1 -DHAVE_ERR_H=1 -DHAVE_STDINT_H=1 -DHAVE_STRING_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 \
		-I${PCIACCESSSRC}/src -I${PCIACCESSSRC}/include -I${.CURDIR}

SRCS=		common_bridge.c common_iterator.c common_init.c common_interface.c \
		common_io.c common_capability.c common_device_name.c common_map.c \
		freebsd_pci.c common_vgaarb_stub.c
INCS=		pciaccess.h

pciaccess.pc: pciaccess.pc.in
	sed -e 's,@prefix@,/usr,g ; \
		s,@exec_prefix@,$${prefix}/lib,g ; \
		s,@libdir@,$${prefix}/lib,g ; \
		s,@includedir@,$${prefix}/include,g ; \
		s,@PACKAGE_VERSION@,0.16,g ; \
		s,@PCIACCESS_LIBS@,-lz,g ;' \
		${.ALLSRC} > ${.TARGET}

PCFILES=	pciaccess.pc

.include <bsd.lib.mk>
