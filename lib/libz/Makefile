#
# $FreeBSD$
#

PACKAGE=	runtime
LIB=		z
SHLIBDIR?=	/lib
SHLIB_MAJOR=	6
MAN=		zopen.3
MLINKS+=	zopen.3 zdopen.3

ZLIBSRC=	${SRCTOP}/contrib/zlib-ng

.PATH:		${ZLIBSRC}

#CFLAGS=-O -DMAX_WBITS=14 -DMAX_MEM_LEVEL=7
#CFLAGS=-g -DDEBUG
#CFLAGS=-O3 -Wall -Wwrite-strings -Wpointer-arith -Wconversion \
#           -Wstrict-prototypes -Wmissing-prototypes

CFLAGS+=	-flto \
		-DZLIB_COMPAT -DWITH_GZFILEOP \
		-DHAVE_BUILTIN_CTZ -DHAVE_BUILTIN_CTZLL \
		-DHAVE_SYS_SDT_H \
		-DHAVE_VISIBILITY_HIDDEN -DHAVE_VISIBILITY_INTERNAL \
		-D_LARGEFILE64_SOURCE=1 -D__USE_LARGEFILE64 \
		-I${ZLIBSRC} -I${.CURDIR}

# ZLIBSRC ^^ is really only for the test

# Clang does this by default anyway, but let's be explicit like the CMakeFile is
CFLAGS+=	-fno-semantic-interposition

# -Wsystem-headers finds endianness redefinition that's harmless
WARNS?=		0

CLEANFILES+=	example.o example foo.gz minigzip.o minigzip

SRCS+=		adler32.c
SRCS+=		chunkset.c
SRCS+=		compare258.c
SRCS+=		compress.c
SRCS+=		crc32.c
SRCS+=		crc32_comb.c
SRCS+=		deflate.c
SRCS+=		deflate_fast.c
SRCS+=		deflate_medium.c
SRCS+=		deflate_quick.c
SRCS+=		deflate_slow.c
SRCS+=		functable.c
SRCS+=		gzlib.c
SRCS+=		gzread.c
SRCS+=		gzwrite.c
SRCS+=		infback.c
SRCS+=		inffast.c
SRCS+=		inflate.c
SRCS+=		inftrees.c
SRCS+=		insert_string.c
SRCS+=		trees.c
SRCS+=		uncompr.c
SRCS+=		zopen.c
SRCS+=		zutil.c

.if ${MACHINE_CPUARCH} == "i386" || ${MACHINE_CPUARCH} == "amd64"
CFLAGS+=	-DUNALIGNED_OK -DUNALIGNED64_OK -DX86_FEATURES
SRCS+=		arch/x86/x86.c

CFLAGS+=	-DX86_PCLMULQDQ_CRC
SRCS+=		arch/x86/crc_folding.c
CFLAGS.crc_folding.c=	-mssse3 -msse4 -mpclmul -fno-lto

CFLAGS+=	-DX86_SSSE3 -DX86_SSSE3_ADLER32
SRCS+=		arch/x86/adler32_ssse3.c
CFLAGS.adler32_ssse3.c=	-mssse3 -fno-lto

CFLAGS+=	-DX86_SSE2 -DX86_SSE2_CHUNKSET -DX86_SSE2_SLIDEHASH
SRCS+=		arch/x86/chunkset_sse.c
CFLAGS.chunkset_sse.c=	-msse2 -fno-lto
SRCS+=		arch/x86/slide_sse.c
CFLAGS.slide_sse.c=	-msse2 -fno-lto
.if ${MACHINE_CPUARCH} == "amd64"
CFLAGS+=	-DX86_NOCHECK_SSE2
.endif

CFLAGS+=	-DX86_SSE42_CMP_STR
SRCS+=		arch/x86/compare258_sse.c
CFLAGS.compare258_sse.c=	-msse4 -fno-lto

CFLAGS+=	-DX86_SSE42_CRC_HASH -DX86_SSE42_CRC_INTRIN
SRCS+=		arch/x86/insert_string_sse.c
CFLAGS.insert_string_sse.c=	-msse4 -fno-lto

CFLAGS+=	-DX86_AVX2 -DX86_AVX2_ADLER32 -DX86_AVX_CHUNKSET
SRCS+=		arch/x86/slide_avx.c
CFLAGS.slide_avx.c=	-mavx2 -fno-lto
SRCS+=		arch/x86/chunkset_avx.c
CFLAGS.chunkset_avx.c=	-mavx2 -fno-lto
SRCS+=		arch/x86/compare258_avx.c
CFLAGS.compare258_avx.c=	-mavx2 -fno-lto
SRCS+=		arch/x86/adler32_avx.c
CFLAGS.adler32_avx.c=	-mavx2 -fno-lto

.endif

.if ${MACHINE_CPUARCH} == "aarch64"
CFLAGS+=	-DUNALIGNED64_OK -DARM_FEATURES
SRCS+=		arch/arm/armfeature.c

CFLAGS+=	-DARM_ACLE_CRC_HASH
SRCS+=		arch/arm/crc32_acle.c
CFLAGS.crc32_acle.c=	-march=armv8-a+crc -fno-lto
SRCS+=		arch/arm/insert_string_acle.c
CFLAGS.insert_string_acle.c=	-march=armv8-a+crc -fno-lto

CFLAGS+=	-DARM_NEON_ADLER32 -DARM_NEON_CHUNKSET -DARM_NEON_SLIDEHASH
SRCS+=		arch/arm/adler32_neon.c
CFLAGS.adler32_neon.c=	-march=armv8-a+simd -fno-lto
SRCS+=		arch/arm/chunkset_neon.c
CFLAGS.chunkset_neon.c=	-march=armv8-a+simd -fno-lto
SRCS+=		arch/arm/slide_neon.c
CFLAGS.slide_neon.c=	-march=armv8-a+simd -fno-lto
.endif

VERSION_DEF=	${.CURDIR}/Versions.def
SYMBOL_MAPS=	${.CURDIR}/Symbol.map
CFLAGS+=	-DSYMBOL_VERSIONING

INCS=		zconf.h zlib.h

.PATH:		${ZLIBSRC}/test

ZLIB_VERSION!=	sed -n '/define.*ZLIB_VERSION/{s,[^0-9.],,gp;q;}' ${ZLIBSRC}/zlib.h

zlib.pc: zlib.pc.in
	sed -e 's,@prefix@,/usr,g ; \
		s,@exec_prefix@,$${prefix},g ; \
		s,@libdir@,$${exec_prefix}/lib,g ; \
		s,@sharedlibdir@,$${libdir},g ; \
		s,@includedir@,$${prefix}/include,g ; \
		s,@VERSION@,${ZLIB_VERSION},g ; \
		s,@SUFFIX@,,g ;' \
		${.ALLSRC} > ${.TARGET}

minigzip:	all minigzip.o
	$(CC) -o minigzip minigzip.o -L. -lz

example:	all example.o
	$(CC) -o example example.o -L. -lz

test: example minigzip
	(export LD_LIBRARY_PATH=. ; ./example )
	(export LD_LIBRARY_PATH=. ; \
		echo hello world | ./minigzip | ./minigzip -d )

PCFILES=	zlib.pc

.include <bsd.lib.mk>
