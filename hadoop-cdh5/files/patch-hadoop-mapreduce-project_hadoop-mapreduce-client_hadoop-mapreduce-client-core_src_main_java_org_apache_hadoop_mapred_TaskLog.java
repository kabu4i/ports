--- hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-core/src/main/java/org/apache/hadoop/mapred/TaskLog.java.orig	2015-07-21 12:21:16 UTC
+++ hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-core/src/main/java/org/apache/hadoop/mapred/TaskLog.java
@@ -542,7 +542,7 @@ public class TaskLog {
       mergedCmd.append("(");
     } else if(ProcessTree.isSetsidAvailable && useSetsid &&
         !Shell.WINDOWS) {
-      mergedCmd.append("exec setsid ");
+      mergedCmd.append("exec ssid ");
     } else {
       mergedCmd.append("exec ");
     }
