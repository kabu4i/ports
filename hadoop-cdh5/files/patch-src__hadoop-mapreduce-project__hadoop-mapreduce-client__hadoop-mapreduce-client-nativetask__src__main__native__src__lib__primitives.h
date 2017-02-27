--- ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/src/lib/primitives.h.orig	2014-10-12 00:56:38.000000000 +0400
+++ ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/src/lib/primitives.h	2014-10-27 13:32:15.145000821 +0300
@@ -101,22 +101,22 @@
   return val;
 }
 
-inline uint64_t bswap64(uint64_t val) {
-#ifdef __X64
-  __asm__("bswapq %0" : "=r" (val) : "0" (val));
-#else
-
-  uint64_t lower = val & 0xffffffffU;
-  uint32_t higher = (val >> 32) & 0xffffffffU;
-
-  lower = bswap(lower);
-  higher = bswap(higher);
-
-  return (lower << 32) + higher;
-
-#endif
-  return val;
-}
+//inline uint64_t bswap64(uint64_t val) {
+//#ifdef __X64
+//  __asm__("bswapq %0" : "=r" (val) : "0" (val));
+//#else
+//
+//  uint64_t lower = val & 0xffffffffU;
+//  uint32_t higher = (val >> 32) & 0xffffffffU;
+//
+//  lower = bswap(lower);
+//  higher = bswap(higher);
+//
+//  return (lower << 32) + higher;
+//
+//#endif
+//  return val;
+//}
 
 /**
  * Fast memcmp
