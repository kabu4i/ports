--- hadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/metrics2/sink/GraphiteSink.java.orig	2015-12-02 18:30:05 UTC
+++ hadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/metrics2/sink/GraphiteSink.java
@@ -18,24 +18,24 @@
 
 package org.apache.hadoop.metrics2.sink;
 
-import java.io.IOException;
-import java.io.OutputStreamWriter;
-import java.io.Writer;
-import java.io.Closeable;
-import java.net.Socket;
-
 import org.apache.commons.configuration.SubsetConfiguration;
+import org.apache.commons.io.Charsets;
 import org.apache.commons.logging.Log;
 import org.apache.commons.logging.LogFactory;
 import org.apache.hadoop.classification.InterfaceAudience;
 import org.apache.hadoop.classification.InterfaceStability;
-import org.apache.hadoop.io.IOUtils;
 import org.apache.hadoop.metrics2.AbstractMetric;
 import org.apache.hadoop.metrics2.MetricsException;
 import org.apache.hadoop.metrics2.MetricsRecord;
 import org.apache.hadoop.metrics2.MetricsSink;
 import org.apache.hadoop.metrics2.MetricsTag;
 
+import java.io.Closeable;
+import java.io.IOException;
+import java.io.OutputStreamWriter;
+import java.io.Writer;
+import java.net.Socket;
+
 /**
  * A metrics sink that writes to a Graphite server
  */
@@ -46,29 +46,22 @@ public class GraphiteSink implements Met
     private static final String SERVER_HOST_KEY = "server_host";
     private static final String SERVER_PORT_KEY = "server_port";
     private static final String METRICS_PREFIX = "metrics_prefix";
-    private Writer writer = null;
     private String metricsPrefix = null;
-    private Socket socket = null;
+    private Graphite graphite = null;
 
     @Override
     public void init(SubsetConfiguration conf) {
         // Get Graphite host configurations.
-        String serverHost = conf.getString(SERVER_HOST_KEY);
-        Integer serverPort = Integer.parseInt(conf.getString(SERVER_PORT_KEY));
+        final String serverHost = conf.getString(SERVER_HOST_KEY);
+        final int serverPort = Integer.parseInt(conf.getString(SERVER_PORT_KEY));
 
         // Get Graphite metrics graph prefix.
         metricsPrefix = conf.getString(METRICS_PREFIX);
         if (metricsPrefix == null)
             metricsPrefix = "";
 
-        try {
-            // Open an connection to Graphite server.
-            socket = new Socket(serverHost, serverPort);
-            writer = new OutputStreamWriter(socket.getOutputStream());
-        } catch (Exception e) {
-            throw new MetricsException("Error creating connection, "
-                    + serverHost + ":" + serverPort, e);
-        }
+        graphite = new Graphite(serverHost, serverPort);
+        graphite.connect();
     }
 
     @Override
@@ -102,39 +95,111 @@ public class GraphiteSink implements Met
         }
 
         try {
-            if(writer != null){
-              writer.write(lines.toString());
-            } else {
-              throw new MetricsException("Writer in GraphiteSink is null!");
-            }
+          graphite.write(lines.toString());
         } catch (Exception e) {
-            throw new MetricsException("Error sending metrics", e);
+          LOG.warn("Error sending metrics to Graphite", e);
+          try {
+            graphite.close();
+          } catch (Exception e1) {
+            throw new MetricsException("Error closing connection to Graphite", e1);
+          }
         }
     }
 
     @Override
     public void flush() {
+      try {
+        graphite.flush();
+      } catch (Exception e) {
+        LOG.warn("Error flushing metrics to Graphite", e);
         try {
-            writer.flush();
-        } catch (Exception e) {
-            throw new MetricsException("Error flushing metrics", e);
+          graphite.close();
+        } catch (Exception e1) {
+          throw new MetricsException("Error closing connection to Graphite", e1);
         }
+      }
     }
 
     @Override
     public void close() throws IOException {
-      try {
-        IOUtils.closeStream(writer);
-        writer = null;
-        LOG.info("writer in GraphiteSink is closed!");
-      } catch (Throwable e){
-        throw new MetricsException("Error closing writer", e);
-      } finally {
-        if (socket != null && !socket.isClosed()) {
-          socket.close();
+      graphite.close();
+    }
+
+    public static class Graphite {
+      private final static int MAX_CONNECTION_FAILURES = 5;
+
+      private String serverHost;
+      private int serverPort;
+      private Writer writer = null;
+      private Socket socket = null;
+      private int connectionFailures = 0;
+
+      public Graphite(String serverHost, int serverPort) {
+        this.serverHost = serverHost;
+        this.serverPort = serverPort;
+      }
+
+      public void connect() {
+        if (isConnected()) {
+          throw new MetricsException("Already connected to Graphite");
+        }
+        if (tooManyConnectionFailures()) {
+          // return silently (there was ERROR in logs when we reached limit for the first time)
+          return;
+        }
+        try {
+          // Open a connection to Graphite server.
+          socket = new Socket(serverHost, serverPort);
+          writer = new OutputStreamWriter(socket.getOutputStream(), Charsets.UTF_8);
+        } catch (Exception e) {
+          connectionFailures++;
+          if (tooManyConnectionFailures()) {
+            // first time when connection limit reached, report to logs
+            LOG.error("Too many connection failures, would not try to connect again.");
+          }
+          throw new MetricsException("Error creating connection, "
+              + serverHost + ":" + serverPort, e);
+        }
+      }
+
+      public void write(String msg) throws IOException {
+        if (!isConnected()) {
+          connect();
+        }
+        if (isConnected()) {
+          writer.write(msg);
+        }
+      }
+
+      public void flush() throws IOException {
+        if (isConnected()) {
+          writer.flush();
+        }
+      }
+
+      public boolean isConnected() {
+        return socket != null && socket.isConnected() && !socket.isClosed();
+      }
+
+      public void close() throws IOException {
+        try {
+          if (writer != null) {
+            writer.close();
+          }
+        } catch (IOException ex) {
+          if (socket != null) {
+            socket.close();
+          }
+        } finally {
           socket = null;
-          LOG.info("socket in GraphiteSink is closed!");
+          writer = null;
         }
       }
+
+      private boolean tooManyConnectionFailures() {
+        return connectionFailures > MAX_CONNECTION_FAILURES;
+      }
+
     }
+
 }
