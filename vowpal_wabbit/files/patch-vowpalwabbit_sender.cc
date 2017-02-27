--- vowpalwabbit/sender.cc.orig	2016-12-08 08:45:39 UTC
+++ vowpalwabbit/sender.cc
@@ -19,6 +19,8 @@
 #include "cache.h"
 #include "network.h"
 #include "reductions.h"
+#include <sys/types.h>
+#include <sys/socket.h>
 
 using namespace std;
 struct sender
