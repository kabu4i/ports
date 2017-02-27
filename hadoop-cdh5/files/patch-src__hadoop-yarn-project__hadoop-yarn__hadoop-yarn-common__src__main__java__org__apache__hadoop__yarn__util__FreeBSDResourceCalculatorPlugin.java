--- /dev/null	2014-10-27 17:22:00.000000000 +0300
+++ ./hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/util/FreeBSDResourceCalculatorPlugin.java	2014-10-27 14:38:40.750999502 +0300
@@ -0,0 +1,315 @@
+/**
+ * Licensed to the Apache Software Foundation (ASF) under one
+ * or more contributor license agreements.  See the NOTICE file
+ * distributed with this work for additional information
+ * regarding copyright ownership.  The ASF licenses this file
+ * to you under the Apache License, Version 2.0 (the
+ * "License"); you may not use this file except in compliance
+ * with the License.  You may obtain a copy of the License at
+ *
+ *     http://www.apache.org/licenses/LICENSE-2.0
+ *
+ * Unless required by applicable law or agreed to in writing, software
+ * distributed under the License is distributed on an "AS IS" BASIS,
+ * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
+ * See the License for the specific language governing permissions and
+ * limitations under the License.
+ */
+
+package org.apache.hadoop.yarn.util;
+
+import java.io.BufferedReader;
+import java.io.FileNotFoundException;
+import java.io.FileReader;
+import java.io.IOException;
+import java.util.regex.Matcher;
+import java.util.regex.Pattern;
+import java.lang.*;
+import java.util.*;
+import java.io.*;
+
+import org.apache.commons.logging.Log;
+import org.apache.commons.logging.LogFactory;
+import org.apache.hadoop.classification.InterfaceAudience;
+import org.apache.hadoop.classification.InterfaceStability;
+import org.apache.hadoop.util.Shell.ShellCommandExecutor;
+
+/**
+ * Plugin to calculate resource information on FreeBSD systems.
+ */
+@InterfaceAudience.Private
+@InterfaceStability.Unstable
+public class FreeBSDResourceCalculatorPlugin extends ResourceCalculatorPlugin {
+  private static final Log LOG =
+      LogFactory.getLog(FreeBSDResourceCalculatorPlugin.class);
+
+  public static final int UNAVAILABLE = -1;
+
+
+  // We need the values for the following keys in meminfo
+  private static final String MEMTOTAL_STRING = "MemTotal";
+  private static final String SWAPTOTAL_STRING = "SwapTotal";
+  private static final String MEMFREE_STRING = "MemFree";
+  private static final String SWAPFREE_STRING = "SwapFree";
+  private static final String INACTIVE_STRING = "Inactive";
+
+  /**
+   * Patterns for parsing /proc/cpuinfo
+   */
+  private static final String PROCFS_CPUINFO = "/proc/cpuinfo";
+  private static final Pattern PROCESSOR_FORMAT =
+      Pattern.compile("^processor[ \t]:[ \t]*([0-9]*)");
+  private static final Pattern FREQUENCY_FORMAT =
+      Pattern.compile("^cpu MHz[ \t]*:[ \t]*([0-9.]*)");
+
+  /**
+   * Pattern for parsing /proc/stat
+   */
+  private static final String PROCFS_STAT = "/proc/stat";
+  private static final Pattern CPU_TIME_FORMAT =
+    Pattern.compile("^cpu[ \t]*([0-9]*)" +
+    		            "[ \t]*([0-9]*)[ \t]*([0-9]*)[ \t]*([0-9]*)[ \t].*");
+
+  private String procfsMemFile;
+  private String procfsCpuFile;
+  private String procfsStatFile;
+  long jiffyLengthInMillis;
+
+  private int pageSize = 0;
+  private long ramSize = 0;
+  private long swapSize = 0;
+  private long ramSizeInactive = 0;
+  private long ramSizeFree = 0;  // free ram space on the machine (kB)
+  private long swapSizeFree = 0; // free swap space on the machine (kB)
+  private long inactiveSize = 0; // inactive cache memory (kB)
+  private int numProcessors = 0; // number of processors on the system
+  private long cpuFrequency = 0L; // CPU frequency on the system (kHz)
+  private long cumulativeCpuTime = 0L; // CPU used time since system is on (ms)
+  private long lastCumulativeCpuTime = 0L; // CPU used time read last time (ms)
+  // Unix timestamp while reading the CPU time (ms)
+  private float cpuUsage = UNAVAILABLE;
+  private long sampleTime = UNAVAILABLE;
+  private long lastSampleTime = UNAVAILABLE;
+
+  boolean readMemInfoFile = false;
+  boolean readCpuInfoFile = false;
+
+  /**
+   * Get current time
+   * @return Unix time stamp in millisecond
+   */
+  long getCurrentTime() {
+    return System.currentTimeMillis();
+  }
+
+  public FreeBSDResourceCalculatorPlugin() {
+    jiffyLengthInMillis = ProcfsBasedProcessTree.JIFFY_LENGTH_IN_MILLIS;
+  }
+
+  /**
+   * @param jiffyLengthInMillis fake jiffy length value
+   */
+  public FreeBSDResourceCalculatorPlugin(long jiffyLengthInMillis){
+    this.jiffyLengthInMillis = jiffyLengthInMillis;
+  }
+
+  /**
+   * Read /proc/meminfo, parse and compute memory information
+   * @param readAgain if false, read only on the first time
+   */
+  private void readProcMemInfoFile() {
+
+    try {
+    
+      String[] pagesize = { "/bin/sh", "-c", "sysctl -n vm.stats.vm.v_page_size" };
+      String[] inactivepages = { "/bin/sh", "-c", "sysctl -n vm.stats.vm.v_inactive_count" };
+      String[] freepages = { "/bin/sh", "-c", "sysctl -n vm.stats.vm.v_free_count" };
+      String[] wiredpages = { "/bin/sh", "-c", "sysctl -n vm.stats.vm.v_wire_count" };
+      String[] cachepages = { "/bin/sh", "-c", "sysctl -n vm.stats.vm.v_cache_count" };
+      String[] totalmem = { "/bin/sh", "-c", "sysctl -n hw.physmem" };
+      String[] totalswap = { "/bin/sh", "-c", "sysctl -n vm.swap_total" };
+
+      ShellCommandExecutor get_pagesize = new ShellCommandExecutor(pagesize);
+      ShellCommandExecutor get_inactivepages = new ShellCommandExecutor(inactivepages);
+      ShellCommandExecutor get_freepages = new ShellCommandExecutor(freepages);
+      ShellCommandExecutor get_wiredpages = new ShellCommandExecutor(wiredpages);
+      ShellCommandExecutor get_cachepages = new ShellCommandExecutor(cachepages);
+      ShellCommandExecutor get_totalmem = new ShellCommandExecutor(totalmem);
+      ShellCommandExecutor get_totalswap = new ShellCommandExecutor(totalswap);
+
+      get_pagesize.execute();
+      get_inactivepages.execute();
+      get_freepages.execute();
+      get_wiredpages.execute();
+      get_cachepages.execute();
+      get_totalmem.execute();
+      get_totalswap.execute();
+
+      pageSize = Integer.parseInt(get_pagesize.getOutput().replace("\n", ""));
+      ramSize = Long.parseLong(get_totalmem.getOutput().replace("\n", ""));
+      swapSize = Long.parseLong(get_totalswap.getOutput().replace("\n", ""));
+      ramSizeFree = Long.parseLong(get_freepages.getOutput().replace("\n", "")) * pageSize;
+      swapSizeFree = Long.parseLong(get_totalswap.getOutput().replace("\n", ""));
+      inactiveSize = Long.parseLong(get_inactivepages.getOutput().replace("\n", "")) * pageSize;
+         }
+      
+     catch (IOException io) {
+      LOG.warn("Error reading the stream " + io);
+    } 
+  }
+
+  private void readProcCpuInfoFile() {
+    try {
+      String[] ncpu = { "/bin/sh", "-c", "sysctl -n hw.ncpu" };
+      String[] freq = { "/bin/sh", "-c", "sysctl -n dev.cpu.0.freq" };
+
+      ShellCommandExecutor get_ncpu = new ShellCommandExecutor(ncpu);
+      ShellCommandExecutor get_freq = new ShellCommandExecutor(freq);
+
+      get_ncpu.execute();
+      get_freq.execute();
+
+      numProcessors =  Integer.parseInt(get_ncpu.getOutput().replace("\n", ""));
+      cpuFrequency = (long)(Long.parseLong(get_freq.getOutput().replace("\n", "")) * 1000); // kHz
+        }
+     catch (IOException io) {
+     LOG.warn("Error reading the stream " + io);
+    } 
+  }
+
+  /**
+   * Read /proc/stat file, parse and calculate cumulative CPU
+   */
+  private void readProcStatFile() {
+    // Read "/proc/stat" file
+    BufferedReader in = null;
+    FileReader fReader = null;
+    try {
+      fReader = new FileReader(procfsStatFile);
+      in = new BufferedReader(fReader);
+    } catch (FileNotFoundException f) {
+      // shouldn't happen....
+      return;
+    }
+
+    Matcher mat = null;
+    try {
+      String[] totalTime = { "/bin/sh", "-c", "sysctl -n kern.cp_time" };
+      ShellCommandExecutor get_totalTime = new ShellCommandExecutor(totalTime);
+      get_totalTime.execute();
+      String str = get_totalTime.getOutput().replace("\n", "");
+      List<String> timeList = new ArrayList<String>(Arrays.asList(str.split(" ")));
+      long uTime = Long.parseLong(timeList.get(0));
+      long nTime = Long.parseLong(timeList.get(1));
+      long kTime = Long.parseLong(timeList.get(2));
+      long iTime = Long.parseLong(timeList.get(3));
+      cumulativeCpuTime = uTime + nTime + kTime + iTime; // milliseconds
+    
+      cumulativeCpuTime *= jiffyLengthInMillis;
+    } catch (IOException io) {
+      LOG.warn("Error reading the stream " + io);
+    }
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getPhysicalMemorySize() {
+    readProcMemInfoFile();
+    return ramSize;
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getVirtualMemorySize() {
+    readProcMemInfoFile();
+    return (ramSize + swapSize);
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getAvailablePhysicalMemorySize() {
+    readProcMemInfoFile();
+    return (ramSizeFree + inactiveSize);
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getAvailableVirtualMemorySize() {
+    readProcMemInfoFile();
+    return (ramSizeFree + swapSizeFree + inactiveSize);
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public int getNumProcessors() {
+    readProcCpuInfoFile();
+    return numProcessors;
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getCpuFrequency() {
+    readProcCpuInfoFile();
+    return cpuFrequency;
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public long getCumulativeCpuTime() {
+    readProcStatFile();
+    return cumulativeCpuTime;
+  }
+
+  /** {@inheritDoc} */
+  @Override
+  public float getCpuUsage() {
+    readProcStatFile();
+    sampleTime = getCurrentTime();
+    if (lastSampleTime == UNAVAILABLE ||
+        lastSampleTime > sampleTime) {
+      // lastSampleTime > sampleTime may happen when the system time is changed
+      lastSampleTime = sampleTime;
+      lastCumulativeCpuTime = cumulativeCpuTime;
+      return cpuUsage;
+    }
+    // When lastSampleTime is sufficiently old, update cpuUsage.
+    // Also take a sample of the current time and cumulative CPU time for the
+    // use of the next calculation.
+    final long MINIMUM_UPDATE_INTERVAL = 10 * jiffyLengthInMillis;
+    if (sampleTime > lastSampleTime + MINIMUM_UPDATE_INTERVAL) {
+	    cpuUsage = (float)(cumulativeCpuTime - lastCumulativeCpuTime) * 100F /
+	               ((float)(sampleTime - lastSampleTime) * getNumProcessors());
+	    lastSampleTime = sampleTime;
+      lastCumulativeCpuTime = cumulativeCpuTime;
+    }
+    return cpuUsage;
+  }
+
+  /**
+   * Test the {@link FreeBSDResourceCalculatorPlugin}
+   *
+   * @param args
+   */
+  public static void main(String[] args) {
+    FreeBSDResourceCalculatorPlugin plugin = new FreeBSDResourceCalculatorPlugin();
+    System.out.println("Physical memory Size (bytes) : "
+        + plugin.getPhysicalMemorySize());
+    System.out.println("Total Virtual memory Size (bytes) : "
+        + plugin.getVirtualMemorySize());
+    System.out.println("Available Physical memory Size (bytes) : "
+        + plugin.getAvailablePhysicalMemorySize());
+    System.out.println("Total Available Virtual memory Size (bytes) : "
+        + plugin.getAvailableVirtualMemorySize());
+    System.out.println("Number of Processors : " + plugin.getNumProcessors());
+    System.out.println("CPU frequency (kHz) : " + plugin.getCpuFrequency());
+    System.out.println("Cumulative CPU time (ms) : " +
+            plugin.getCumulativeCpuTime());
+    try {
+      // Sleep so we can compute the CPU usage
+      Thread.sleep(500L);
+    } catch (InterruptedException e) {
+      // do nothing
+    }
+    System.out.println("CPU usage % : " + plugin.getCpuUsage());
+  }
+}
