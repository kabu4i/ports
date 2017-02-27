--- java/src/main/java/vowpalWabbit/jni/NativeUtils.java.orig	2016-12-08 08:45:39 UTC
+++ java/src/main/java/vowpalWabbit/jni/NativeUtils.java
@@ -94,6 +94,9 @@ public class NativeUtils {
         if (osName.toLowerCase().contains("mac")) {
             primaryName = "Darwin";
         }
+        else if (osName.toLowerCase().contains("freebsd")) {
+            primaryName = "FreeBSD";
+        }
         else if (osName.toLowerCase().contains("linux")) {
             String distro = getDistroName();
             if (distro == null) {
