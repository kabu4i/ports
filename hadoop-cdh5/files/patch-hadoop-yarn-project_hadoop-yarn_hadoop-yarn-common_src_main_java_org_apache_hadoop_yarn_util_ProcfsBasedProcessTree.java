--- hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/util/ProcfsBasedProcessTree.java.orig	2015-07-21 12:44:12 UTC
+++ hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/util/ProcfsBasedProcessTree.java
@@ -53,7 +53,7 @@ public class ProcfsBasedProcessTree exte
   static final Log LOG = LogFactory
       .getLog(ProcfsBasedProcessTree.class);
 
-  private static final String PROCFS = "/proc/";
+  private static final String PROCFS = "/compat/linux/proc/";
 
   private static final Pattern PROCFS_STAT_FILE_FORMAT = Pattern .compile(
     "^([0-9-]+)\\s([^\\s]+)\\s[^\\s]\\s([0-9-]+)\\s([0-9-]+)\\s([0-9-]+)\\s" +
@@ -110,7 +110,7 @@ public class ProcfsBasedProcessTree exte
     long jiffiesPerSecond = -1;
     long pageSize = -1;
     try {
-      if(Shell.LINUX) {
+      if(Shell.LINUX || Shell.FREEBSD) {
         ShellCommandExecutor shellExecutorClk = new ShellCommandExecutor(
             new String[] { "getconf", "CLK_TCK" });
         shellExecutorClk.execute();
@@ -186,9 +186,9 @@ public class ProcfsBasedProcessTree exte
    */
   public static boolean isAvailable() {
     try {
-      if (!Shell.LINUX) {
+      if (!Shell.LINUX || !Shell.FREEBSD) {
         LOG.info("ProcfsBasedProcessTree currently is supported only on "
-            + "Linux.");
+            + "Linux and FreeBSD.");
         return false;
       }
     } catch (SecurityException se) {
@@ -332,7 +332,7 @@ public class ProcfsBasedProcessTree exte
         ret.append(String.format(PROCESSTREE_DUMP_FORMAT, p.getPid(), p
             .getPpid(), p.getPgrpId(), p.getSessionId(), p.getName(), p
             .getUtime(), p.getStime(), p.getVmem(), p.getRssmemPage(), p
-            .getCmdLine(procfsDir)));
+            .getCmdLine()));
       }
     }
     return ret.toString();
@@ -694,7 +694,7 @@ public class ProcfsBasedProcessTree exte
       return children;
     }
 
-    public String getCmdLine(String procfsDir) {
+    public String getCmdLine() {
       String ret = "N/A";
       if (pid == null) {
         return ret;
@@ -703,7 +703,7 @@ public class ProcfsBasedProcessTree exte
       FileReader fReader = null;
       try {
         fReader =
-            new FileReader(new File(new File(procfsDir, pid.toString()),
+            new FileReader(new File(new File("/proc/", pid.toString()),
                 PROCFS_CMDLINE_FILE));
       } catch (FileNotFoundException f) {
         // The process vanished in the interim!
