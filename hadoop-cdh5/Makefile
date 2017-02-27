# Created by: Alexey Babutin <a.babutin@rambler-co.ru>
# $FreeBSD$

PORTNAME=	hadoop
PORTVERSION=	2.6.0
PORTREVISION=	7
CATEGORIES=	java devel
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/:cloumain \
		http://archive.cloudera.com/cdh5/cdh/5/:cloumain \
		http://freebsd.dev.rambler.ru/distfiles/rambler/:m2 \
		http://archive.apache.org/dist/tomcat/tomcat-6/v${TOMCAT_VERSION}/bin/:tomcat \
		http://archive.cloudera.com/tarballs/:cloutars

PKGNAMESUFFIX=	-cdh5.8.2
DISTNAME=	hadoop-2.6.0-cdh5.8.2
DISTFILES=	apache-tomcat-${TOMCAT_VERSION}.tar.gz:tomcat hadoop-2.6.0-cdh5.8.2-src.tar.gz:cloumain \
		m2repo-0.0.12.tar.gz:m2

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	Apache Map/Reduce framework

LICENSE=	APACHE20

BUILD_DEPENDS=	${LOCALBASE}/bin/gmake:${PORTSDIR}/devel/gmake \
		${LOCALBASE}/bin/cmake:${PORTSDIR}/devel/cmake \
		${JAVASHAREDIR}/maven33/bin/mvn:${PORTSDIR}/devel/maven33 \
		${LOCALBASE}/protobuf25/bin/protoc:${PORTSDIR}/devel/protobuf25 \
		ant:${PORTSDIR}/devel/apache-ant
LIB_DEPENDS=	libsnappy.so:${PORTSDIR}/archivers/snappy
RUN_DEPENDS=	${LOCALBASE}/bin/bash:${PORTSDIR}/shells/bash \
		${LOCALBASE}/bin/ssid:${PORTSDIR}/sysutils/ssid

OPTIONS_DEFINE=	\
		NAMENODE \
		DATANODE \
		RESOURCE_MANAGER \
		NODE_MANAGER \
		ZKFC \
		JOB_HISTORY_SERVER \
		JOURNAL_NODE

OPTIONS_DEFAULT=	NAMENODE DATANODE RESOURCE_MANAGER NODE_MANAGER ZKFC JOB_HISTORY_SERVER JOURNAL_NODE

NAMENODE_DESC=			Enable namenode
DATANODE_DESC=			Enable datanode
RESOURCE_MANAGER_DESC=		Enable resourcemanager
NODE_MANAGER_DESC=		Enable nodemanager
ZKFC_DESC=			Enable DFS_ZK_Failover_Controller
JOB_HISTORY_SERVER_DESC=	Enable history server
JOURNAL_NODE_DESC=		Enable journal

.include <bsd.port.options.mk>

USE_JAVA=	yes
JAVA_VERSION=	1.7
JAVA_VENDOR=	openjdk
MVN=		mvn
MVN_ARGS=	-X -Dmaven.repo.local=${WRKDIR}/.m2 \
		-Pdist,native \
		-DskipTests
MVN_TARGET=	package
MVN_CLEAN_TARGET=clean
USE_AUTOTOOLS=	autoconf:env aclocal:env automake:env
ONLY_FOR_ARCHS=	amd64 i386
USE_LDCONFIG=	${PREFIX}/lib/native

TOMCAT_VERSION=	6.0.44

HADOOP_USER=	hadoop
HADOOP_GROUP=	hadoop

HADOOP_RUNDIR=	/var/run/${PORTNAME}
HADOOP_LOGDIR=	/var/log/${PORTNAME}
HADOOP_TARGET=	${WRKSRC}/${PORTNAME}-dist/target

.if ${PORT_OPTIONS:MNAMENODE}
USE_RC_SUBR+=	namenode
.endif

.if ${PORT_OPTIONS:MDATANODE}
USE_RC_SUBR+=	datanode
.endif

.if ${PORT_OPTIONS:MRESOURCE_MANAGER}
USE_RC_SUBR+=	resourcemanager
.endif

.if ${PORT_OPTIONS:MNODE_MANAGER}
USE_RC_SUBR+=	nodemanager
.endif

.if ${PORT_OPTIONS:MZKFC}
USE_RC_SUBR+=	zkfc
.endif

.if ${PORT_OPTIONS:MJOB_HISTORY_SERVER}
USE_RC_SUBR+=	history
.endif

.if ${PORT_OPTIONS:MJOURNAL_NODE}
USE_RC_SUBR+=	journalnode
.endif

SUB_LIST+=	HADOOP_USER="${HADOOP_USER}" \
		HADOOP_GROUP="${HADOOP_GROUP}" \
		HADOOP_CONF="${ETCDIR}" \
		HADOOP_RUNDIR="${HADOOP_RUNDIR}"

PLIST_SUB=	HADOOP_RUNDIR=${HADOOP_RUNDIR} \
		HADOOP_LOGDIR=${HADOOP_LOGDIR} \
		HADOOP_CONF="${ETCDIR:C/${PREFIX}\///}" \
		HADOOP_USER=${HADOOP_USER} \
		HADOOP_GROUP=${HADOOP_GROUP}

.include <bsd.port.pre.mk>

.if ${ARCH} == "amd64" || ${ARCH} == "ia64"
DATAMODEL=	64
.else
DATAMODEL=	32
.endif

do-build:
	${LN} -s ${LOCALBASE}/protobuf25/bin/protoc ${LOCALBASE}/bin/protoc
	${MKDIR} ${WRKSRC}/hadoop-hdfs-project/hadoop-hdfs-httpfs/downloads
	${CP} ${DISTDIR}/${DIST_SUBDIR}/apache-tomcat-${TOMCAT_VERSION}.tar.gz ${WRKSRC}/hadoop-hdfs-project/hadoop-hdfs-httpfs/downloads/
	${MKDIR} ${WRKSRC}/hadoop-common-project/hadoop-kms/downloads
	${CP} ${DISTDIR}/${DIST_SUBDIR}/apache-tomcat-${TOMCAT_VERSION}.tar.gz ${WRKSRC}/hadoop-common-project/hadoop-kms/downloads/
	@(cd ${WRKSRC} && LC_ALL=C ${MVN} ${MVN_ARGS} ${MVN_TARGET})

do-install:
	${MKDIR} ${STAGEDIR}${PREFIX}/lib/native
	${MKDIR} ${STAGEDIR}${ETCDIR}
	${MKDIR} ${STAGEDIR}${DATADIR}
	${MKDIR} ${STAGEDIR}${DOCSDIR}
	${MKDIR} ${STAGEDIR}${EXAMPLESDIR}
	${INSTALL_SCRIPT} ${HADOOP_TARGET}/${DISTNAME}/bin/* ${STAGEDIR}${PREFIX}/bin/
	${INSTALL_SCRIPT} ${HADOOP_TARGET}/${DISTNAME}/sbin/* ${STAGEDIR}${PREFIX}/sbin/
	${INSTALL_DATA} ${HADOOP_TARGET}/${DISTNAME}/lib/native/* ${STAGEDIR}${PREFIX}/lib/native/
	${INSTALL_DATA} ${HADOOP_TARGET}/${DISTNAME}/libexec/* ${STAGEDIR}${PREFIX}/libexec/
	${INSTALL_DATA} ${HADOOP_TARGET}/${DISTNAME}/include/* ${STAGEDIR}${PREFIX}/include/
	@(cd ${HADOOP_TARGET}/${DISTNAME}/share/${PORTNAME} && ${COPYTREE_SHARE} \* ${STAGEDIR}${DATADIR}/ "! -name doc")
.if ${PORT_OPTIONS:MDOCS}
	@(cd ${HADOOP_TARGET}/${DISTNAME}/share/doc/${PORTNAME} && ${COPYTREE_SHARE} \* ${STAGEDIR}${DOCSDIR}/)
.endif

post-install:
	@${MKDIR} ${STAGEDIR}${HADOOP_RUNDIR} ${STAGEDIR}${HADOOP_LOGDIR}
#	@${CHOWN} ${HADOOP_USER}:${HADOOP_GROUP} ${STAGEDIR}${HADOOP_RUNDIR} ${STAGEDIR}${HADOOP_LOGDIR}

.include <bsd.port.post.mk>
