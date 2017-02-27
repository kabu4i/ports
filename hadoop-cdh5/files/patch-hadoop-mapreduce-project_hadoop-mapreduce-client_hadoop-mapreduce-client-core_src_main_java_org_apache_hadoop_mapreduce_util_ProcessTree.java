--- hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-core/src/main/java/org/apache/hadoop/mapreduce/util/ProcessTree.java.orig	2015-07-21 12:22:26 UTC
+++ hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-core/src/main/java/org/apache/hadoop/mapreduce/util/ProcessTree.java
@@ -54,7 +54,7 @@ public class ProcessTree {
     ShellCommandExecutor shexec = null;
     boolean setsidSupported = true;
     try {
-      String[] args = {"setsid", "bash", "-c", "echo $$"};
+      String[] args = {"ssid", "bash", "-c", "echo $$"};
       shexec = new ShellCommandExecutor(args);
       shexec.execute();
     } catch (IOException ioe) {
