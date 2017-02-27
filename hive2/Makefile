# Created by: a.babutin@rambler-co.ru
# $FreeBSD$

PORTNAME=	hive2
PORTVERSION=	2.1.1
CATEGORIES=	devel java
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	just hive

LICENSE=	APACHE20

RUN_DEPENDS=	bash:${PORTSDIR}/shells/bash

USE_JAVA=	yes
JAVA_VERSION=	1.7
JAVA_VENDOR=	openjdk
JAVA_RUN=	yes

HIVE_RUNDIR=	/var/run/hive

NO_BUILD=	yes

USE_RC_SUBR=	hiveserver2 metastore

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/${PORTNAME}
	${MKDIR} ${STAGEDIR}${HIVE_RUNDIR}
	cd ${WRKSRC} && ${COPYTREE_SHARE} conf ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_BIN} bin ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} lib ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} scripts ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} jdbc ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} hcatalog ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} examples ${STAGEDIR}${PREFIX}/${PORTNAME}/

.include <bsd.port.mk>
