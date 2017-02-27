# Created by: Nikita Ilyushkin <nilyushkin@rambler-co.ru>
# $FreeBSD$

PORTNAME=	fastFM
PORTVERSION=	0.2.9
CATEGORIES=	devel python
MASTER_SITES=	http://pypi.park.rambler.ru/fastfm/
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	nilyushkin@rambler-co.ru
COMMENT=	

LICENSE=	BSD

USE_GCC=	yes

#BUILD_DEPENDS=	cython:${PORTSDIR}/lang/cython \
#		${PYTHON_PKGNAMEPREFIX}numpy>=0:${PORTSDIR}/math/py-numpy \
#		${PYTHON_PKGNAMEPREFIX}scikit-learn>=0:${PORTSDIR}/science/py-scikit-learn \
#		${PYTHON_PKGNAMEPREFIX}scipy>=0:${PORTSDIR}/science/py-scipy

#LIB_DEPENDS=	libblas.so:${PORTSDIR}/math/blas

RUN_DEPENDS=	${PYTHON_PKGNAMEPREFIX}numpy>=0:${PORTSDIR}/math/py-numpy \
		${PYTHON_PKGNAMEPREFIX}scikit-learn>=0:${PORTSDIR}/science/py-scikit-learn \
		${PYTHON_PKGNAMEPREFIX}scipy>=0:${PORTSDIR}/science/py-scipy

USES=		python
USE_PYTHON=	autoplist distutils

#pre-build:
#	(cd ${WRKSRC} && gmake TARGET_CPU_ARCH=NEHALEM)

#do-install:
#	${CD} ${WRKSRC};python2 setup.py install

.include <bsd.port.mk>
