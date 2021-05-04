LIB=		ffi
SHLIB_MAJOR=	8
MAN=		man/ffi.3 man/ffi_call.3 man/ffi_prep_cif.3 man/ffi_prep_cif_var.3

LIBFFISRC=	${SRCTOP}/contrib/libffi

.PATH:		${LIBFFISRC} ${LIBFFISRC}/src

CFLAGS+=	-flto \
		-I${LIBFFISRC}/src -I${LIBFFISRC}/include -I${.OBJDIR} -I${.CURDIR}

WARNS?=		0

INCS=		ffi.h
SRCS+=		closures.c java_raw_api.c prep_cif.c raw_api.c types.c

.if ${MACHINE_CPUARCH} == "i386"
CFLAGS+=	-I${LIBFFISRC}/src/x86
SRCS+=		x86/ffi.c x86/sysv.S
INCS+=		x86/ffitarget.h
.endif

.if ${MACHINE_CPUARCH} == "amd64"
CFLAGS+=	-I${LIBFFISRC}/src/x86
SRCS+=		x86/ffi64.c x86/ffiw64.c x86/win64.S x86/unix64.S
INCS+=		x86/ffitarget.h
.endif

.if ${MACHINE_CPUARCH} == "aarch64"
CFLAGS+=	-I${LIBFFISRC}/src/aarch64
SRCS+=		aarch64/ffi.c aarch64/sysv.S
INCS+=		aarch64/ffitarget.h
.endif

VERSION_MAP=	libffi.map

*.o: ffi.h

ffi.h: include/ffi.h.in
	sed -e 's,@TARGET@,${MACHINE_ARCH:S/i386/X86/:S/amd64/X86_64/:S/aarch64/AARCH64/},g ; \
		s,@HAVE_LONG_DOUBLE@,1,g ; \
		s,@FFI_EXEC_TRAMPOLINE_TABLE@,0,g ; \
		s,@VERSION@,3.3,g ; ' \
		${.ALLSRC} > ${.TARGET}

libffi.map: libffi.map.in
	${CPP} ${CFLAGS} ${.ALLSRC} > ${.TARGET}

libffi.pc: libffi.pc.in
	sed -e 's,@prefix@,/usr,g ; \
		s,@exec_prefix@,$${prefix},g ; \
		s,@libdir@,$${exec_prefix}/lib,g ; \
	s,@toolexeclibdir@,$${libdir},g ; \
		s,@includedir@,$${prefix}/include,g ; \
		s,@PACKAGE_NAME@,libffi,g ; \
		s,@PACKAGE_VERSION@,3.3,g ;' \
		${.ALLSRC} > ${.TARGET}

PCFILES=	libffi.pc

.include <bsd.lib.mk>
