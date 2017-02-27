--- hadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/util/Shell.java.orig	2015-07-21 12:10:32 UTC
+++ hadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/util/Shell.java
@@ -385,7 +385,7 @@ abstract public class Shell {
     ShellCommandExecutor shexec = null;
     boolean setsidSupported = true;
     try {
-      String[] args = {"setsid", "bash", "-c", "echo $$"};
+      String[] args = {"ssid", "bash", "-c", "echo $$"};
       shexec = new ShellCommandExecutor(args);
       shexec.execute();
     } catch (IOException ioe) {
