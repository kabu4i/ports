--- hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/util/ResourceCalculatorPlugin.java.orig	2015-07-21 13:01:48 UTC
+++ hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/util/ResourceCalculatorPlugin.java
@@ -125,6 +125,9 @@ public abstract class ResourceCalculator
       if (Shell.LINUX) {
         return new LinuxResourceCalculatorPlugin();
       }
+      if (Shell.FREEBSD) {
+        return new FreeBSDResourceCalculatorPlugin();
+      }
       if (Shell.WINDOWS) {
         return new WindowsResourceCalculatorPlugin();
       }
