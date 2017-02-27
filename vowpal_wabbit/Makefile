# Created by: Dmitry Sivachenko <demon@FreeBSD.org>
# $FreeBSD: head/math/vowpal_wabbit/Makefile 390634 2015-06-26 13:23:01Z amdmi3 $

PORTNAME=	vowpal_wabbit83
PORTVERSION=	8.3.0
CATEGORIES=	math
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/
DISTFILES=	m2_vowpal_wabbit.tar.gz vowpal_wabbit-${PORTVERSION}.tar.gz 
WRKSRC=		${WRKDIR}/vowpal_wabbit-8.3.0

MAINTAINER=	demon@FreeBSD.org
COMMENT=	Fast out-of-core learning system

BUILD_DEPENDS=	${LOCALBASE}/bin/gmake:${PORTSDIR}/devel/gmake \
		${JAVASHAREDIR}/maven3/bin/mvn:${PORTSDIR}/devel/maven3
LIB_DEPENDS=	libboost_thread.so:${PORTSDIR}/devel/boost-libs

BROKEN_FreeBSD_9=	does not build

.include <bsd.port.options.mk>

USES=		compiler:c++11-lang libtool
GNU_CONFIGURE=	yes
USE_LDCONFIG=	yes
USE_JAVA=	yes
JAVA_VERSION=	1.7
JAVA_VENDOR=	openjdk

CONFIGURE_ARGS=	--enable-shared=no --with-boost=${LOCALBASE}

post-build:
	(cd ${WRKSRC}/java && gmake)

post-install:
	${CP} ${WRKSRC}/java/target/*.jar ${STAGEDIR}${PREFIX}/lib/
	${MV} ${STAGEDIR}${PREFIX}/bin/vw ${STAGEDIR}${PREFIX}/bin/vw83
	${MV} ${STAGEDIR}${PREFIX}/bin/active_interactor ${STAGEDIR}${PREFIX}/bin/active_interactor83
	${MV} ${STAGEDIR}${PREFIX}/bin/ezexample_predict ${STAGEDIR}${PREFIX}/bin/ezexample_predict83
	${MV} ${STAGEDIR}${PREFIX}/bin/ezexample_train ${STAGEDIR}${PREFIX}/bin/ezexample_train83
	${MV} ${STAGEDIR}${PREFIX}/bin/library_example ${STAGEDIR}${PREFIX}/bin/library_example83
	${MV} ${STAGEDIR}${PREFIX}/bin/spanning_tree ${STAGEDIR}${PREFIX}/bin/spanning_tree83

.include <bsd.port.mk>
