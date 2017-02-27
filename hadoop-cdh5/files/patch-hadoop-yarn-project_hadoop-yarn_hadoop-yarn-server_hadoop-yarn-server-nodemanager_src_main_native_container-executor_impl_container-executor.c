--- hadoop-yarn-project/hadoop-yarn/hadoop-yarn-server/hadoop-yarn-server-nodemanager/src/main/native/container-executor/impl/container-executor.c.orig	2015-07-21 13:07:01 UTC
+++ hadoop-yarn-project/hadoop-yarn/hadoop-yarn-server/hadoop-yarn-server-nodemanager/src/main/native/container-executor/impl/container-executor.c
@@ -36,6 +36,8 @@
 #include <sys/mount.h>
 #include <sys/wait.h>
 
+static const int LOGIN_NAME_MAX = 256;
+
 static const int DEFAULT_MIN_USERID = 1000;
 
 static const char* DEFAULT_BANNED_USERS[] = {"mapred", "hdfs", "bin", 0};
@@ -1441,7 +1443,7 @@ int mount_cgroup(const char *pair, const
               pair);
     result = -1; 
   } else {
-    if (mount("none", mount_path, "cgroup", 0, controller) == 0) {
+    if (mount("none", mount_path, 0, controller) == 0) {
       char *buf = stpncpy(hier_path, mount_path, strlen(mount_path));
       *buf++ = '/';
       snprintf(buf, PATH_MAX - (buf - hier_path), "%s", hierarchy);
