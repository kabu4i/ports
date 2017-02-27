# Created by: Alexey Babutin <a.babutin@rambler-co.ru>
# $FreeBSD$

PORTNAME=	pathos
PORTVERSION=	0.2
PORTREVISION=	3
CATEGORIES=	devel python
MASTER_SITES=	http://freebsd.dev.rambler.ru/distfiles/rambler/
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	a.babutin@rambler-co.ru
COMMENT=	Framework for parallel graph management and execution in heterogeneous computing

LIB_DEPENDS=	lib_multiprocessing.so:${PORTSDIR}/devel/py-multiprocess \
		lib_multiprocess.so:${PORTSDIR}/devel/py-multiprocess
BUILD_DEPENDS=	get_objgraph.py:${PORTSDIR}/textproc/py-dill \
		unpickle.py:${PORTSDIR}/textproc/py-dill \
		ppserver.py:${PORTSDIR}/devel/py-ppft \
		pox_launcher.py:${PORTSDIR}/devel/py-pox \
		cython:${PORTSDIR}/lang/cython \
		ipython:${PORTSDIR}/devel/ipython
RUN_DEPENDS=	get_objgraph.py:${PORTSDIR}/textproc/py-dill \
		unpickle.py:${PORTSDIR}/textproc/py-dill \
		ppserver.py:${PORTSDIR}/devel/py-ppft \
		pox_launcher.py:${PORTSDIR}/devel/py-pox \
		cython:${PORTSDIR}/lang/cython \
		ipython:${PORTSDIR}/devel/ipython

USES=		python
USE_PYTHON=	distutils autoplist

.include <bsd.port.options.mk>

#LATEST_LINK=	py-${PORTNAME}
#PKGBASE==	py-${PORTNAME}

.include <bsd.port.mk>
