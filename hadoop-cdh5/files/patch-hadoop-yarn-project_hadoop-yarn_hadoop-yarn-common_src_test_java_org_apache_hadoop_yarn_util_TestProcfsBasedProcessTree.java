--- hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/test/java/org/apache/hadoop/yarn/util/TestProcfsBasedProcessTree.java.orig	2015-07-21 13:03:00 UTC
+++ hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/test/java/org/apache/hadoop/yarn/util/TestProcfsBasedProcessTree.java
@@ -74,7 +74,7 @@ public class TestProcfsBasedProcessTree 
       try {
         Vector<String> args = new Vector<String>();
         if (isSetsidAvailable()) {
-          args.add("setsid");
+          args.add("ssid");
         }
         args.add("bash");
         args.add("-c");
@@ -807,7 +807,7 @@ public class TestProcfsBasedProcessTree 
     ShellCommandExecutor shexec = null;
     boolean setsidSupported = true;
     try {
-      String[] args = { "setsid", "bash", "-c", "echo $$" };
+      String[] args = { "ssid", "bash", "-c", "echo $$" };
       shexec = new ShellCommandExecutor(args);
       shexec.execute();
     } catch (IOException ioe) {
