# Created by: a.babutin@rambler-co.ru
# $FreeBSD$

PORTNAME=	apache-tez
PORTVERSION=	0.8.4
CATEGORIES=	devel java
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	engine for hive

LICENSE=	APACHE20

USE_JAVA=	yes
JAVA_VERSION=	1.7
JAVA_VENDOR=	openjdk
JAVA_RUN=	yes

NO_BUILD=	yes

do-install:
	${MKDIR} ${STAGEDIR}${DATADIR}/lib ${STAGEDIR}${DATADIR}/conf
	${INSTALL_DATA} ${WRKSRC}/lib/*.jar ${STAGEDIR}${DATADIR}/lib/
	${INSTALL_DATA} ${WRKSRC}/*.jar ${STAGEDIR}${DATADIR}/
	${INSTALL_DATA} ${WRKSRC}/conf/*-template.xml ${STAGEDIR}${DATADIR}/conf/

.include <bsd.port.mk>
