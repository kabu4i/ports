# Created by: a.babutin@rambler-co.ru

PORTNAME=	kafka
PORTVERSION=	0.10.1.1
CATEGORIES=	devel java
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	just kafka

LICENSE=	APACHE20
RUN_DEPENDS=	bash:${PORTSDIR}/shells/bash \
		scala:${PORTSDIR}/lang/scala

USE_JAVA=	yes
JAVA_VERSION=	1.7
JAVA_VENDOR=	openjdk
JAVA_RUN=	yes

NO_BUILD=	yes

USE_RC_SUBR=	kafka_server

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/${PORTNAME}
	${MKDIR} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin
.for f in ${SCRIPTS}
	${INSTALL_SCRIPT} ${WRKSRC}/bin/${f} ${STAGEDIR}${PREFIX}/${PORTNAME}/bin
.endfor
	cd ${WRKSRC} && ${COPYTREE_SHARE} config ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} libs ${STAGEDIR}${PREFIX}/${PORTNAME}/
	cd ${WRKSRC} && ${COPYTREE_SHARE} bin ${STAGEDIR}${PREFIX}/${PORTNAME}/

.include <bsd.port.mk>
