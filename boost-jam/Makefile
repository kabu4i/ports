# Created by: Alexander Churanov <churanov.port.maintainer@gmail.com>
# $FreeBSD: head/devel/boost-jam/Makefile 366834 2014-08-31 23:01:31Z bapt $

PORTNAME=	boost-jam
COMMENT=	Build tool from the boost.org

PLIST_FILES=	bin/bjam


.include "${.CURDIR}/../boost-all/common.mk"

do-build:
	cd ${WRKSRC}/tools/build/src/engine/ && \
		${MAKE_ENV} ./build.sh cc

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/bin
	${INSTALL_PROGRAM} ${WRKSRC}/tools/build/src/engine/bin.*/bjam \
		${STAGEDIR}${PREFIX}/bin/

.include <bsd.port.mk>
