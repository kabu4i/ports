--- hadoop-common-project/hadoop-common/src/test/java/org/apache/hadoop/metrics2/impl/TestGraphiteMetrics.java.orig	2015-12-02 18:30:05 UTC
+++ hadoop-common-project/hadoop-common/src/test/java/org/apache/hadoop/metrics2/impl/TestGraphiteMetrics.java
@@ -18,23 +18,7 @@
 
 package org.apache.hadoop.metrics2.impl;
 
-import static org.junit.Assert.assertEquals;
-import static org.mockito.Mockito.mock;
-import static org.mockito.Mockito.verify;
-import static org.mockito.Mockito.when;
-
-import java.io.ByteArrayOutputStream;
-import java.io.IOException;
-import java.io.OutputStreamWriter;
-import java.io.Writer;
-import java.util.ArrayList;
-import java.util.Collections;
-import java.util.HashSet;
-import java.util.List;
-import java.util.Set;
-
 import org.apache.hadoop.metrics2.AbstractMetric;
-import org.apache.hadoop.metrics2.MetricsException;
 import org.apache.hadoop.metrics2.MetricsRecord;
 import org.apache.hadoop.metrics2.MetricsTag;
 import org.apache.hadoop.metrics2.sink.GraphiteSink;
@@ -42,6 +26,23 @@ import org.junit.Test;
 import org.mockito.ArgumentCaptor;
 import org.mockito.internal.util.reflection.Whitebox;
 
+import java.io.IOException;
+import java.util.ArrayList;
+import java.util.HashSet;
+import java.util.List;
+import java.util.Set;
+import java.util.Collections;
+
+import static org.junit.Assert.assertEquals;
+import static org.mockito.Matchers.anyString;
+import static org.mockito.Matchers.eq;
+import static org.mockito.Mockito.mock;
+import static org.mockito.Mockito.verify;
+import static org.mockito.Mockito.when;
+import static org.mockito.Mockito.doThrow;
+import static org.mockito.Mockito.reset;
+
+
 public class TestGraphiteMetrics {
     private AbstractMetric makeMetric(String name, Number value) {
         AbstractMetric metric = mock(AbstractMetric.class);
@@ -50,6 +51,12 @@ public class TestGraphiteMetrics {
         return metric;
     }
 
+    private GraphiteSink.Graphite makeGraphite() {
+      GraphiteSink.Graphite mockGraphite = mock(GraphiteSink.Graphite.class);
+      when(mockGraphite.isConnected()).thenReturn(true);
+      return mockGraphite;
+    }
+
     @Test
     public void testPutMetrics() {
         GraphiteSink sink = new GraphiteSink();
@@ -61,18 +68,18 @@ public class TestGraphiteMetrics {
         metrics.add(makeMetric("foo2", 2.25));
         MetricsRecord record = new MetricsRecordImpl(MsInfo.Context, (long) 10000, tags, metrics);
 
-        OutputStreamWriter mockWriter = mock(OutputStreamWriter.class);
         ArgumentCaptor<String> argument = ArgumentCaptor.forClass(String.class);
-        Whitebox.setInternalState(sink, "writer", mockWriter);
+        final GraphiteSink.Graphite mockGraphite = makeGraphite();
+        Whitebox.setInternalState(sink, "graphite", mockGraphite);
         sink.putMetrics(record);
 
         try {
-            verify(mockWriter).write(argument.capture());
+          verify(mockGraphite).write(argument.capture());
         } catch (IOException e) {
-            e.printStackTrace();
+          e.printStackTrace();
         }
 
-        String result = argument.getValue().toString();
+        String result = argument.getValue();
 
         assertEquals(true,
             result.equals("null.all.Context.Context=all.Hostname=host.foo1 1.25 10\n" +
@@ -86,24 +93,25 @@ public class TestGraphiteMetrics {
         GraphiteSink sink = new GraphiteSink();
         List<MetricsTag> tags = new ArrayList<MetricsTag>();
         tags.add(new MetricsTag(MsInfo.Context, "all"));
-        tags.add(new MetricsTag(MsInfo.Hostname, null));
+      tags.add(new MetricsTag(MsInfo.Hostname, null));
         Set<AbstractMetric> metrics = new HashSet<AbstractMetric>();
         metrics.add(makeMetric("foo1", 1));
         metrics.add(makeMetric("foo2", 2));
         MetricsRecord record = new MetricsRecordImpl(MsInfo.Context, (long) 10000, tags, metrics);
 
-        OutputStreamWriter mockWriter = mock(OutputStreamWriter.class);
+
         ArgumentCaptor<String> argument = ArgumentCaptor.forClass(String.class);
-        Whitebox.setInternalState(sink, "writer", mockWriter);
+        final GraphiteSink.Graphite mockGraphite = makeGraphite();
+        Whitebox.setInternalState(sink, "graphite", mockGraphite);
         sink.putMetrics(record);
 
         try {
-            verify(mockWriter).write(argument.capture());
+            verify(mockGraphite).write(argument.capture());
         } catch (IOException e) {
             e.printStackTrace();
         }
 
-        String result = argument.getValue().toString();
+        String result = argument.getValue();
 
         assertEquals(true,
             result.equals("null.all.Context.Context=all.foo1 1 10\n" + 
@@ -120,8 +128,8 @@ public class TestGraphiteMetrics {
 
       // setup GraphiteSink
       GraphiteSink sink = new GraphiteSink();
-      ByteArrayOutputStream out = new ByteArrayOutputStream();
-      Whitebox.setInternalState(sink, "writer", new OutputStreamWriter(out));
+      final GraphiteSink.Graphite mockGraphite = makeGraphite();
+      Whitebox.setInternalState(sink, "graphite", mockGraphite);
 
       // given two metrics records with timestamps 1000 milliseconds apart.
       List<MetricsTag> tags = Collections.emptyList();
@@ -141,15 +149,16 @@ public class TestGraphiteMetrics {
       }
 
       // then the timestamps in the graphite stream should differ by one second.
-      String expectedOutput
-        = "null.default.Context.foo1 1 1000000000\n"
-        + "null.default.Context.foo1 1 1000000001\n";
-      assertEquals(expectedOutput, out.toString());
+      try {
+        verify(mockGraphite).write(eq("null.default.Context.foo1 1 1000000000\n"));
+        verify(mockGraphite).write(eq("null.default.Context.foo1 1 1000000001\n"));
+      } catch (IOException e) {
+        e.printStackTrace();
+      }
     }
 
-
-    @Test(expected=MetricsException.class)
-    public void testCloseAndWrite() throws IOException {
+    @Test
+    public void testFailureAndPutMetrics() throws IOException {
       GraphiteSink sink = new GraphiteSink();
       List<MetricsTag> tags = new ArrayList<MetricsTag>();
       tags.add(new MetricsTag(MsInfo.Context, "all"));
@@ -159,18 +168,38 @@ public class TestGraphiteMetrics {
       metrics.add(makeMetric("foo2", 2.25));
       MetricsRecord record = new MetricsRecordImpl(MsInfo.Context, (long) 10000, tags, metrics);
 
-      OutputStreamWriter writer = mock(OutputStreamWriter.class);
+      final GraphiteSink.Graphite mockGraphite = makeGraphite();
+      Whitebox.setInternalState(sink, "graphite", mockGraphite);
 
-      Whitebox.setInternalState(sink, "writer", writer);
-      sink.close();
+      // throw exception when first try
+      doThrow(new IOException("IO exception")).when(mockGraphite).write(anyString());
+
+      sink.putMetrics(record);
+      verify(mockGraphite).write(anyString());
+      verify(mockGraphite).close();
+
+      // reset mock and try again
+      reset(mockGraphite);
+      when(mockGraphite.isConnected()).thenReturn(false);
+
+      ArgumentCaptor<String> argument = ArgumentCaptor.forClass(String.class);
       sink.putMetrics(record);
+
+      verify(mockGraphite).write(argument.capture());
+      String result = argument.getValue();
+
+      assertEquals(true,
+          result.equals("null.all.Context.Context=all.Hostname=host.foo1 1.25 10\n" +
+          "null.all.Context.Context=all.Hostname=host.foo2 2.25 10\n") ||
+          result.equals("null.all.Context.Context=all.Hostname=host.foo2 2.25 10\n" +
+          "null.all.Context.Context=all.Hostname=host.foo1 1.25 10\n"));
     }
 
     @Test
     public void testClose(){
         GraphiteSink sink = new GraphiteSink();
-        Writer mockWriter = mock(Writer.class);
-        Whitebox.setInternalState(sink, "writer", mockWriter);
+        final GraphiteSink.Graphite mockGraphite = makeGraphite();
+        Whitebox.setInternalState(sink, "graphite", mockGraphite);
         try {
             sink.close();
         } catch (IOException ioe) {
@@ -178,7 +207,7 @@ public class TestGraphiteMetrics {
         }
 
         try {
-            verify(mockWriter).close();
+            verify(mockGraphite).close();
         } catch (IOException ioe) {
             ioe.printStackTrace();
         }
