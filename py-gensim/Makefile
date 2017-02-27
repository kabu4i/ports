# Created by: Alexey Babutin <a.babutin@rambler-co.ru>
# $FreeBSD: $

PORTNAME=	gensim
PORTVERSION=	0.13.4.1
CATEGORIES=	science python
MASTER_SITES=	${MASTER_SITE_CHEESESHOP}
MASTER_SITE_SUBDIR=source/g/gensim
PKGNAMEPREFIX=	${PYTHON_PKGNAMEPREFIX}

MAINTAINER=	demon@FreeBSD.org
COMMENT=	Gensim is a library for topic modelling, document indexing and similarity retrieval with large corpora

BUILD_DEPENDS=	${PYNUMPY}\
		${PYTHON_PKGNAMEPREFIX}scipy>0.8.0:${PORTSDIR}/science/py-scipy
RUN_DEPENDS=	${PYNUMPY} \
		${PYTHON_PKGNAMEPREFIX}scipy>0.8.0:${PORTSDIR}/science/py-scipy \
		${PYTHON_PKGNAMEPREFIX}smart_open>0:${PORTSDIR}/devel/py-smart_open \
		${PYTHON_PKGNAMEPREFIX}six>=1.5:${PORTSDIR}/devel/py-six

USES=		python
USE_PYTHON=	distutils autoplist
USE_GCC=	5

.include <bsd.port.options.mk>

LATEST_LINK=	py-${PORTNAME}

.include <bsd.port.mk>
