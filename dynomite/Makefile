# Created by: Alexey Babutin <babutin84@rambler.ru>
# $FreeBSD $

PORTNAME=	dynomite
PORTVERSION=	0.5.7.1
CATEGORIES=	databases
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

MAINTAINER=	babutin84@rambler.ru
COMMENT=	A generic dynamo implementation for different k-v storage engines

LICENSE=	APACHE20

USES=		execinfo autoreconf libtool
USE_GCC=	yes

CFLAGS+=	-ggdb3 -O0 -I${LOCALBASE}/include
LDFLAGS+=	-L${LOCALBASE}/lib -lexecinfo

USE_RC_SUBR+=	dynomite

PLIST_FILES=	sbin/dynomite \
		man/man8/dynomite.8.gz \
		etc/dynomite/dynomite.yml.sample \
		etc/dynomite/dynomite.pem

HAS_CONFIGURE=	yes

do-install:
	${MKDIR} ${STAGEDIR}${ETCDIR}/dynomite
	${INSTALL_PROGRAM} ${WRKSRC}/src/dynomite ${STAGEDIR}${PREFIX}/sbin
	${INSTALL_MAN} ${WRKSRC}/man/dynomite.8 ${STAGEDIR}${MANPREFIX}/man/man8/
	${INSTALL_DATA} ${WRKSRC}/conf/dynomite.yml ${STAGEDIR}${PREFIX}/etc/dynomite/dynomite.yml.sample
	${INSTALL_DATA} ${WRKSRC}/conf/dynomite.pem ${STAGEDIR}${PREFIX}/etc/dynomite/dynomite.pem

.include <bsd.port.mk>
