PORTNAME=	spark2
PORTVERSION=	2.1.0
CATEGORIES=	devel java
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/

PKGNAMEPREFIX=	apache-
DISTFILES=	spark-${PORTVERSION}.tgz #$spark-m2.${PORTVERSION}.tar.gz
DIST_SUBDIR=	hadoop

MAINTAINER=	n.ilyushkin@rambler-co.ru
COMMENT=	Fast big data processing engine

LICENSE=	APACHE20

BUILD_DEPENDS=	${LOCALBASE}/share/java/maven33/bin/mvn:${PORTSDIR}/devel/maven33 \
		${LOCALBASE}/lib/libsnappyjava.so:${PORTSDIR}/archivers/snappy-java
RUN_DEPENDS=	bash:${PORTSDIR}/shells/bash \
		yarn:${PORTSDIR}/java/hadoop-cdh5

USES=		python:2
USE_JAVA=	yes
JAVA_VERSION=	1.7
MAKE_ENV+=	JAVA_HOME=${JAVA_HOME} MAVEN_OPTS="-Xmx4g -XX:MaxPermSize=512M -XX:ReservedCodeCacheSize=512m"

USERS=		spark
GROUPS=		spark

USE_RC_SUBR=	spark_master spark_worker
PLIST_SUB+=	SPARK_USER=spark SPARK_GROUP=spark VER=${PORTVERSION}
SUB_LIST+=	SPARK_USER=spark SPARK_GROUP=spark

.include <bsd.port.pre.mk>

.if ${ARCH} == "amd64"
JAVA_ARCH = x86_64
.elif ${ARCH} == "i386"
JAVA_ARCH = x86
.else
JAVA_ARCH = ${ARCH}
.endif

do-build:
	${MKDIR} ${WRKDIR}/snappy/org/xerial/snappy/native/${OPSYS}/${JAVA_ARCH}
	${CP} ${LOCALBASE}/lib/libsnappyjava.so ${WRKDIR}/snappy/org/xerial/snappy/native/${OPSYS}/${JAVA_ARCH}
#	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${LOCALBASE}/share/java/maven33/bin/mvn -Dmaven.repo.local=${WRKDIR}/m2 clean package -Dhadoop.version=2.6.0-cdh5.5.1 -Pyarn -Phadoop-2.6 -Phive -Phive-thriftserver -DskipTests
	cd ${WRKSRC} && ${SETENV} ${MAKE_ENV} ${LOCALBASE}/share/java/maven33/bin/mvn -Dmaven.repo.local=${WRKDIR}/m2 clean package -Dhadoop.version=2.7.0 -Pyarn -Phadoop-2.6 -Phive -Phive-thriftserver -DskipTests
#	${JAR} uvf ${WRKSRC}/assembly/target/scala*/*assembly*hadoop*.jar -C ${WRKDIR}/snappy org

post-build:
	${RM} ${WRKSRC}/bin/*.cmd ${WRKSRC}/sbin/spark-daemon.sh.orig

do-install:
	${MKDIR} ${STAGEDIR}${DATADIR}/lib ${STAGEDIR}${DATADIR}/jars ${STAGEDIR}${DATADIR}/examples ${STAGEDIR}${DATADIR}/bin ${STAGEDIR}${DATADIR}/sbin ${STAGEDIR}${DATADIR}/conf
	${ECHO_CMD} "Spark $PORTVERSION built for Hadoop 2.7.2" > ${STAGEDIR}${DATADIR}/RELEASE
	${INSTALL_DATA} ${WRKSRC}/examples/target/scala*/jars/*.jar ${STAGEDIR}${DATADIR}/lib/
	${INSTALL_DATA} ${WRKSRC}/assembly/target/scala*/jars/*.jar ${STAGEDIR}${DATADIR}/jars/
	cd ${WRKSRC}/examples && ${COPYTREE_SHARE} src ${STAGEDIR}${DATADIR}/examples/
	cd ${WRKSRC}/bin && ${INSTALL_SCRIPT} * ${STAGEDIR}${DATADIR}/bin/
	cd ${WRKSRC}/sbin && ${INSTALL_SCRIPT} * ${STAGEDIR}${DATADIR}/sbin/
	cd ${WRKSRC} && ${COPYTREE_SHARE} python ${STAGEDIR}${DATADIR}/
	${INSTALL_DATA} ${WRKSRC}/conf/*.template ${STAGEDIR}${DATADIR}/conf/
	${MKDIR} ${STAGEDIR}/var/run/spark2
	${MKDIR} ${STAGEDIR}/var/log/spark2

.include <bsd.port.post.mk>
