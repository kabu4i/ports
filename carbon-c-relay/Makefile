# Created by:  kabu4i <demon@FreeBSD.org>
# $FreeBSD $

PORTNAME=	carbon-c-relay
PORTVERSION=	1.11.1
CATEGORIES=	net
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	Enhanced C implementation of Carbon relay

USE_RC_SUBR=	relay

do-build:
	cd ${WRKSRC} && gmake

do-install:
	${CP} ${WRKSRC}/relay  ${STAGEDIR}${LOCALBASE}/bin/

.include <bsd.port.mk>
