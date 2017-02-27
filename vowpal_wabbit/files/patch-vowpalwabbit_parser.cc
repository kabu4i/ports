--- vowpalwabbit/parser.cc.orig	2016-12-08 08:45:39 UTC
+++ vowpalwabbit/parser.cc
@@ -36,6 +36,7 @@ int getpid()
 
 #ifdef __FreeBSD__
 #include <netinet/in.h>
+#include <sys/socket.h>
 #endif
 
 #include <errno.h>
