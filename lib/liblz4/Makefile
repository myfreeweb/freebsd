LIB=		lz4
SHLIB_MAJOR=	1

LZ4SRC=	${SRCTOP}/contrib/lz4/lib

.PATH:		${LZ4SRC}

CFLAGS+=	-flto \
		-DXXH_NAMESPACE=LZ4_ \
		-I${LZ4SRC} -I${.CURDIR}

SRCS=		lz4.c lz4frame.c lz4hc.c xxhash.c
INCS=		lz4.h lz4frame.h lz4frame_static.h lz4hc.h

LZ4_VER_MAJOR!=	sed -En '/define LZ4_VERSION_MAJOR/s/.*[[:blank:]]([0-9][0-9]*).*/\1/p' ${LZ4SRC}/lz4.h
LZ4_VER_MINOR!=	sed -En '/define LZ4_VERSION_MINOR/s/.*[[:blank:]]([0-9][0-9]*).*/\1/p' ${LZ4SRC}/lz4.h
LZ4_VER_PATCH!=	sed -En '/define LZ4_VERSION_RELEASE/s/.*[[:blank:]]([0-9][0-9]*).*/\1/p' ${LZ4SRC}/lz4.h
LZ4_VER=	${LZ4_VER_MAJOR}.${LZ4_VER_MINOR}.${LZ4_VER_PATCH}

liblz4.pc: liblz4.pc.in
	sed -e 's,@PREFIX@,/usr,g ; \
		s,@LIBDIR@,$${prefix}/lib,g ; \
		s,@INCLUDEDIR@,$${prefix}/include,g ; \
		s,@VERSION@,${LZ4_VER},g ;' \
		${.ALLSRC} > ${.TARGET}

PCFILES=	liblz4.pc

.include <bsd.lib.mk>
