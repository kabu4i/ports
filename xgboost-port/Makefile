# Created by: <a.babutin@rambler-co.ru>

PORTNAME=	xgboost
PORTVERSION=	0.42
CATEGORIES=	math
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/
DISTFILES=	${PORTNAME}-${PORTVERSION}${EXTRACT_SUFX}

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	Fast out-of-core learning system
BUILD_DEPENDS=	${LOCALBASE}/bin/gmake:${PORTSDIR}/devel/gmake
USE_GCC=	5

do-build:
	${GMAKE} -C ${WRKSRC} cxx11=1

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/xgboost ${STAGEDIR}${PREFIX}/bin

.include <bsd.port.mk>
