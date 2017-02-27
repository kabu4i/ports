--- ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/include/gtest/gtest.h.orig	2014-10-12 00:56:38.000000000 +0400
+++ ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/include/gtest/gtest.h	2014-10-27 13:32:15.143001228 +0300
@@ -48,10 +48,11 @@
 // registration from Barthelemy Dagenais' (barthelemy@prologique.com)
 // easyUnit framework.
 
-#ifndef GTEST_INCLUDE_GTEST_GTEST_H_
+// #ifndef GTEST_INCLUDE_GTEST_GTEST_H_
 #define GTEST_INCLUDE_GTEST_GTEST_H_
 
 #include <limits>
+#include <ostream>
 #include <vector>
 
 // Copyright 2005, Google Inc.
@@ -127,6 +128,10 @@
 // Low-level types and utilities for porting Google Test to various
 // platforms.  They are subject to change without notice.  DO NOT USE
 // THEM IN USER CODE.
+//
+// This file is fundamental to Google Test.  All other Google Test source
+// files are expected to #include this.  Therefore, it cannot #include
+// any other Google Test header.
 
 #ifndef GTEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_H_
 #define GTEST_INCLUDE_GTEST_INTERNAL_GTEST_PORT_H_
@@ -167,6 +172,8 @@
 //                              Test's own tr1 tuple implementation should be
 //                              used.  Unused when the user sets
 //                              GTEST_HAS_TR1_TUPLE to 0.
+//   GTEST_LANG_CXX11         - Define it to 1/0 to indicate that Google Test
+//                              is building in C++11/C++98 mode.
 //   GTEST_LINKED_AS_SHARED_LIBRARY
 //                            - Define to 1 when compiling tests that use
 //                              Google Test as a shared library (known as
@@ -181,11 +188,16 @@
 // the given platform; otherwise undefined):
 //   GTEST_OS_AIX      - IBM AIX
 //   GTEST_OS_CYGWIN   - Cygwin
+//   GTEST_OS_FREEBSD  - FreeBSD
 //   GTEST_OS_HPUX     - HP-UX
 //   GTEST_OS_LINUX    - Linux
 //     GTEST_OS_LINUX_ANDROID - Google Android
 //   GTEST_OS_MAC      - Mac OS X
+//     GTEST_OS_IOS    - iOS
+//       GTEST_OS_IOS_SIMULATOR - iOS simulator
 //   GTEST_OS_NACL     - Google Native Client (NaCl)
+//   GTEST_OS_OPENBSD  - OpenBSD
+//   GTEST_OS_QNX      - QNX
 //   GTEST_OS_SOLARIS  - Sun Solaris
 //   GTEST_OS_SYMBIAN  - Symbian
 //   GTEST_OS_WINDOWS  - Windows (Desktop, MinGW, or Mobile)
@@ -270,7 +282,7 @@
 //   GTEST_FLAG()       - references a flag.
 //   GTEST_DECLARE_*()  - declares a flag.
 //   GTEST_DEFINE_*()   - defines a flag.
-//   GetArgvs()         - returns the command line as a vector of strings.
+//   GetInjectableArgvs() - returns the command line as a vector of strings.
 //
 // Environment variable utilities:
 //   GetEnv()             - gets the value of an environment variable.
@@ -283,11 +295,17 @@
 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
+#include <unistd.h>
 #ifndef _WIN32_WCE
 # include <sys/types.h>
 # include <sys/stat.h>
 #endif  // !_WIN32_WCE
 
+#if defined __APPLE__
+# include <AvailabilityMacros.h>
+# include <TargetConditionals.h>
+#endif
+
 #include <iostream>  // NOLINT
 #include <sstream>  // NOLINT
 #include <string>  // NOLINT
@@ -322,11 +340,19 @@
 # endif  // _WIN32_WCE
 #elif defined __APPLE__
 # define GTEST_OS_MAC 1
+# if TARGET_OS_IPHONE
+#  define GTEST_OS_IOS 1
+#  if TARGET_IPHONE_SIMULATOR
+#   define GTEST_OS_IOS_SIMULATOR 1
+#  endif
+# endif
+#elif defined __FreeBSD__
+# define GTEST_OS_FREEBSD 1
 #elif defined __linux__
 # define GTEST_OS_LINUX 1
-# ifdef ANDROID
+# if defined __ANDROID__
 #  define GTEST_OS_LINUX_ANDROID 1
-# endif  // ANDROID
+# endif
 #elif defined __MVS__
 # define GTEST_OS_ZOS 1
 #elif defined(__sun) && defined(__SVR4)
@@ -337,8 +363,25 @@
 # define GTEST_OS_HPUX 1
 #elif defined __native_client__
 # define GTEST_OS_NACL 1
+#elif defined __OpenBSD__
+# define GTEST_OS_OPENBSD 1
+#elif defined __QNX__
+# define GTEST_OS_QNX 1
 #endif  // __CYGWIN__
 
+#ifndef GTEST_LANG_CXX11
+// gcc and clang define __GXX_EXPERIMENTAL_CXX0X__ when
+// -std={c,gnu}++{0x,11} is passed.  The C++11 standard specifies a
+// value for __cplusplus, and recent versions of clang, gcc, and
+// probably other compilers set that too in C++11 mode.
+# if __GXX_EXPERIMENTAL_CXX0X__ || __cplusplus >= 201103L
+// Compiling in at least C++11 mode.
+#  define GTEST_LANG_CXX11 1
+# else
+#  define GTEST_LANG_CXX11 0
+# endif
+#endif
+
 // Brings in definitions for functions used in the testing::internal::posix
 // namespace (read, write, close, chdir, isatty, stat). We do not currently
 // use them on Windows Mobile.
@@ -347,20 +390,25 @@
 // is not the case, we need to include headers that provide the functions
 // mentioned above.
 # include <unistd.h>
-# if !GTEST_OS_NACL
-// TODO(vladl@google.com): Remove this condition when Native Client SDK adds
-// strings.h (tracked in
-// http://code.google.com/p/nativeclient/issues/detail?id=1175).
-#  include <strings.h>  // Native Client doesn't provide strings.h.
-# endif
+# include <strings.h>
 #elif !GTEST_OS_WINDOWS_MOBILE
 # include <direct.h>
 # include <io.h>
 #endif
 
+#if GTEST_OS_LINUX_ANDROID
+// Used to define __ANDROID_API__ matching the target NDK API level.
+#  include <android/api-level.h>  // NOLINT
+#endif
+
 // Defines this to true iff Google Test can use POSIX regular expressions.
 #ifndef GTEST_HAS_POSIX_RE
-# define GTEST_HAS_POSIX_RE (!GTEST_OS_WINDOWS)
+# if GTEST_OS_LINUX_ANDROID
+// On Android, <regex.h> is only available starting with Gingerbread.
+#  define GTEST_HAS_POSIX_RE (__ANDROID_API__ >= 9)
+# else
+#  define GTEST_HAS_POSIX_RE (!GTEST_OS_WINDOWS)
+# endif
 #endif
 
 #if GTEST_HAS_POSIX_RE
@@ -475,11 +523,27 @@
 # elif defined(__GNUC__) && (GTEST_GCC_VER_ >= 40302)
 
 #  ifdef __GXX_RTTI
-#   define GTEST_HAS_RTTI 1
+// When building against STLport with the Android NDK and with
+// -frtti -fno-exceptions, the build fails at link time with undefined
+// references to __cxa_bad_typeid. Note sure if STL or toolchain bug,
+// so disable RTTI when detected.
+#   if GTEST_OS_LINUX_ANDROID && defined(_STLPORT_MAJOR) && \
+       !defined(__EXCEPTIONS)
+#    define GTEST_HAS_RTTI 0
+#   else
+#    define GTEST_HAS_RTTI 1
+#   endif  // GTEST_OS_LINUX_ANDROID && __STLPORT_MAJOR && !__EXCEPTIONS
 #  else
 #   define GTEST_HAS_RTTI 0
 #  endif  // __GXX_RTTI
 
+// Clang defines __GXX_RTTI starting with version 3.0, but its manual recommends
+// using has_feature instead. has_feature(cxx_rtti) is supported since 2.7, the
+// first version with C++ support.
+# elif defined(__clang__)
+
+#  define GTEST_HAS_RTTI __has_feature(cxx_rtti)
+
 // Starting with version 9.0 IBM Visual Age defines __RTTI_ALL__ to 1 if
 // both the typeid and dynamic_cast features are present.
 # elif defined(__IBMCPP__) && (__IBMCPP__ >= 900)
@@ -512,7 +576,8 @@
 //
 // To disable threading support in Google Test, add -DGTEST_HAS_PTHREAD=0
 // to your compiler flags.
-# define GTEST_HAS_PTHREAD (GTEST_OS_LINUX || GTEST_OS_MAC || GTEST_OS_HPUX)
+# define GTEST_HAS_PTHREAD (GTEST_OS_LINUX || GTEST_OS_MAC || GTEST_OS_HPUX \
+    || GTEST_OS_QNX)
 #endif  // GTEST_HAS_PTHREAD
 
 #if GTEST_HAS_PTHREAD
@@ -528,8 +593,13 @@
 // this macro to 0 to prevent Google Test from using tuple (any
 // feature depending on tuple with be disabled in this mode).
 #ifndef GTEST_HAS_TR1_TUPLE
+# if GTEST_OS_LINUX_ANDROID && defined(_STLPORT_MAJOR)
+// STLport, provided with the Android NDK, has neither <tr1/tuple> or <tuple>.
+#  define GTEST_HAS_TR1_TUPLE 0
+# else
 // The user didn't tell us not to do it, so we assume it's OK.
-# define GTEST_HAS_TR1_TUPLE 1
+#  define GTEST_HAS_TR1_TUPLE 1
+# endif
 #endif  // GTEST_HAS_TR1_TUPLE
 
 // Determines whether Google Test's own tr1 tuple implementation
@@ -538,14 +608,28 @@
 // The user didn't tell us, so we need to figure it out.
 
 // We use our own TR1 tuple if we aren't sure the user has an
-// implementation of it already.  At this time, GCC 4.0.0+ and MSVC
-// 2010 are the only mainstream compilers that come with a TR1 tuple
-// implementation.  NVIDIA's CUDA NVCC compiler pretends to be GCC by
-// defining __GNUC__ and friends, but cannot compile GCC's tuple
-// implementation.  MSVC 2008 (9.0) provides TR1 tuple in a 323 MB
-// Feature Pack download, which we cannot assume the user has.
-# if (defined(__GNUC__) && !defined(__CUDACC__) && (GTEST_GCC_VER_ >= 40000)) \
-    || _MSC_VER >= 1600
+// implementation of it already.  At this time, libstdc++ 4.0.0+ and
+// MSVC 2010 are the only mainstream standard libraries that come
+// with a TR1 tuple implementation.  NVIDIA's CUDA NVCC compiler
+// pretends to be GCC by defining __GNUC__ and friends, but cannot
+// compile GCC's tuple implementation.  MSVC 2008 (9.0) provides TR1
+// tuple in a 323 MB Feature Pack download, which we cannot assume the
+// user has.  QNX's QCC compiler is a modified GCC but it doesn't
+// support TR1 tuple.  libc++ only provides std::tuple, in C++11 mode,
+// and it can be used with some compilers that define __GNUC__.
+# if (defined(__GNUC__) && !defined(__CUDACC__) && (GTEST_GCC_VER_ >= 40000) \
+      && !GTEST_OS_QNX && !defined(_LIBCPP_VERSION))
+#  define GTEST_ENV_HAS_TR1_TUPLE_ 1
+# endif
+
+// C++11 specifies that <tuple> provides std::tuple. Use that if gtest is used
+// in C++11 mode and libstdc++ isn't very old (binaries targeting OS X 10.6
+// can build with clang but need to use gcc4.2's libstdc++).
+# if GTEST_LANG_CXX11 && (!defined(__GLIBCXX__) || __GLIBCXX__ > 20110325)
+#  define GTEST_ENV_HAS_STD_TUPLE_ 1
+# endif
+
+# if GTEST_ENV_HAS_TR1_TUPLE_ || GTEST_ENV_HAS_STD_TUPLE_
 #  define GTEST_USE_OWN_TR1_TUPLE 0
 # else
 #  define GTEST_USE_OWN_TR1_TUPLE 1
@@ -559,7 +643,9 @@
 #if GTEST_HAS_TR1_TUPLE
 
 # if GTEST_USE_OWN_TR1_TUPLE
-// This file was GENERATED by a script.  DO NOT EDIT BY HAND!!!
+// This file was GENERATED by command:
+//     pump.py gtest-tuple.h.pump
+// DO NOT EDIT BY HAND!!!
 
 // Copyright 2009 Google Inc.
 // All Rights Reserved.
@@ -701,34 +787,54 @@
 struct TupleElement;
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 0, GTEST_10_TUPLE_(T)> { typedef T0 type; };
+struct TupleElement<true, 0, GTEST_10_TUPLE_(T) > {
+  typedef T0 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 1, GTEST_10_TUPLE_(T)> { typedef T1 type; };
+struct TupleElement<true, 1, GTEST_10_TUPLE_(T) > {
+  typedef T1 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 2, GTEST_10_TUPLE_(T)> { typedef T2 type; };
+struct TupleElement<true, 2, GTEST_10_TUPLE_(T) > {
+  typedef T2 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 3, GTEST_10_TUPLE_(T)> { typedef T3 type; };
+struct TupleElement<true, 3, GTEST_10_TUPLE_(T) > {
+  typedef T3 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 4, GTEST_10_TUPLE_(T)> { typedef T4 type; };
+struct TupleElement<true, 4, GTEST_10_TUPLE_(T) > {
+  typedef T4 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 5, GTEST_10_TUPLE_(T)> { typedef T5 type; };
+struct TupleElement<true, 5, GTEST_10_TUPLE_(T) > {
+  typedef T5 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 6, GTEST_10_TUPLE_(T)> { typedef T6 type; };
+struct TupleElement<true, 6, GTEST_10_TUPLE_(T) > {
+  typedef T6 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 7, GTEST_10_TUPLE_(T)> { typedef T7 type; };
+struct TupleElement<true, 7, GTEST_10_TUPLE_(T) > {
+  typedef T7 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 8, GTEST_10_TUPLE_(T)> { typedef T8 type; };
+struct TupleElement<true, 8, GTEST_10_TUPLE_(T) > {
+  typedef T8 type;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct TupleElement<true, 9, GTEST_10_TUPLE_(T)> { typedef T9 type; };
+struct TupleElement<true, 9, GTEST_10_TUPLE_(T) > {
+  typedef T9 type;
+};
 
 }  // namespace gtest_internal
 
@@ -1269,37 +1375,59 @@
 template <typename Tuple> struct tuple_size;
 
 template <GTEST_0_TYPENAMES_(T)>
-struct tuple_size<GTEST_0_TUPLE_(T)> { static const int value = 0; };
+struct tuple_size<GTEST_0_TUPLE_(T) > {
+  static const int value = 0;
+};
 
 template <GTEST_1_TYPENAMES_(T)>
-struct tuple_size<GTEST_1_TUPLE_(T)> { static const int value = 1; };
+struct tuple_size<GTEST_1_TUPLE_(T) > {
+  static const int value = 1;
+};
 
 template <GTEST_2_TYPENAMES_(T)>
-struct tuple_size<GTEST_2_TUPLE_(T)> { static const int value = 2; };
+struct tuple_size<GTEST_2_TUPLE_(T) > {
+  static const int value = 2;
+};
 
 template <GTEST_3_TYPENAMES_(T)>
-struct tuple_size<GTEST_3_TUPLE_(T)> { static const int value = 3; };
+struct tuple_size<GTEST_3_TUPLE_(T) > {
+  static const int value = 3;
+};
 
 template <GTEST_4_TYPENAMES_(T)>
-struct tuple_size<GTEST_4_TUPLE_(T)> { static const int value = 4; };
+struct tuple_size<GTEST_4_TUPLE_(T) > {
+  static const int value = 4;
+};
 
 template <GTEST_5_TYPENAMES_(T)>
-struct tuple_size<GTEST_5_TUPLE_(T)> { static const int value = 5; };
+struct tuple_size<GTEST_5_TUPLE_(T) > {
+  static const int value = 5;
+};
 
 template <GTEST_6_TYPENAMES_(T)>
-struct tuple_size<GTEST_6_TUPLE_(T)> { static const int value = 6; };
+struct tuple_size<GTEST_6_TUPLE_(T) > {
+  static const int value = 6;
+};
 
 template <GTEST_7_TYPENAMES_(T)>
-struct tuple_size<GTEST_7_TUPLE_(T)> { static const int value = 7; };
+struct tuple_size<GTEST_7_TUPLE_(T) > {
+  static const int value = 7;
+};
 
 template <GTEST_8_TYPENAMES_(T)>
-struct tuple_size<GTEST_8_TUPLE_(T)> { static const int value = 8; };
+struct tuple_size<GTEST_8_TUPLE_(T) > {
+  static const int value = 8;
+};
 
 template <GTEST_9_TYPENAMES_(T)>
-struct tuple_size<GTEST_9_TUPLE_(T)> { static const int value = 9; };
+struct tuple_size<GTEST_9_TUPLE_(T) > {
+  static const int value = 9;
+};
 
 template <GTEST_10_TYPENAMES_(T)>
-struct tuple_size<GTEST_10_TUPLE_(T)> { static const int value = 10; };
+struct tuple_size<GTEST_10_TUPLE_(T) > {
+  static const int value = 10;
+};
 
 template <int k, class Tuple>
 struct tuple_element {
@@ -1483,8 +1611,8 @@
 inline bool operator==(const GTEST_10_TUPLE_(T)& t,
                        const GTEST_10_TUPLE_(U)& u) {
   return gtest_internal::SameSizeTuplePrefixComparator<
-      tuple_size<GTEST_10_TUPLE_(T)>::value,
-      tuple_size<GTEST_10_TUPLE_(U)>::value>::Eq(t, u);
+      tuple_size<GTEST_10_TUPLE_(T) >::value,
+      tuple_size<GTEST_10_TUPLE_(U) >::value>::Eq(t, u);
 }
 
 template <GTEST_10_TYPENAMES_(T), GTEST_10_TYPENAMES_(U)>
@@ -1527,6 +1655,22 @@
 #undef GTEST_TUPLE_ELEMENT_
 
 #endif  // GTEST_INCLUDE_GTEST_INTERNAL_GTEST_TUPLE_H_
+# elif GTEST_ENV_HAS_STD_TUPLE_
+#  include <tuple>
+// C++11 puts its tuple into the ::std namespace rather than
+// ::std::tr1.  gtest expects tuple to live in ::std::tr1, so put it there.
+// This causes undefined behavior, but supported compilers react in
+// the way we intend.
+namespace std {
+namespace tr1 {
+using ::std::get;
+using ::std::make_tuple;
+using ::std::tuple;
+using ::std::tuple_element;
+using ::std::tuple_size;
+}
+}
+
 # elif GTEST_OS_SYMBIAN
 
 // On Symbian, BOOST_HAS_TR1_TUPLE causes Boost's TR1 tuple library to
@@ -1577,7 +1721,16 @@
 // The user didn't tell us, so we need to figure it out.
 
 # if GTEST_OS_LINUX && !defined(__ia64__)
-#  define GTEST_HAS_CLONE 1
+#  if GTEST_OS_LINUX_ANDROID
+// On Android, clone() is only available on ARM starting with Gingerbread.
+#    if defined(__arm__) && __ANDROID_API__ >= 9
+#     define GTEST_HAS_CLONE 1
+#    else
+#     define GTEST_HAS_CLONE 0
+#    endif
+#  else
+#   define GTEST_HAS_CLONE 1
+#  endif
 # else
 #  define GTEST_HAS_CLONE 0
 # endif  // GTEST_OS_LINUX && !defined(__ia64__)
@@ -1600,9 +1753,11 @@
 // Google Test does not support death tests for VC 7.1 and earlier as
 // abort() in a VC 7.1 application compiled as GUI in debug config
 // pops up a dialog window that cannot be suppressed programmatically.
-#if (GTEST_OS_LINUX || GTEST_OS_MAC || GTEST_OS_CYGWIN || GTEST_OS_SOLARIS || \
+#if (GTEST_OS_LINUX || GTEST_OS_CYGWIN || GTEST_OS_SOLARIS || \
+     (GTEST_OS_MAC && !GTEST_OS_IOS) || GTEST_OS_IOS_SIMULATOR || \
      (GTEST_OS_WINDOWS_DESKTOP && _MSC_VER >= 1400) || \
-     GTEST_OS_WINDOWS_MINGW || GTEST_OS_AIX || GTEST_OS_HPUX)
+     GTEST_OS_WINDOWS_MINGW || GTEST_OS_AIX || GTEST_OS_HPUX || \
+     GTEST_OS_FREEBSD || GTEST_OS_OPENBSD || GTEST_OS_QNX)
 # define GTEST_HAS_DEATH_TEST 1
 # include <vector>  // NOLINT
 #endif
@@ -1731,13 +1886,23 @@
 # define GTEST_NO_INLINE_
 #endif
 
+// _LIBCPP_VERSION is defined by the libc++ library from the LLVM project.
+#if defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
+# define GTEST_HAS_CXXABI_H_ 1
+#else
+# define GTEST_HAS_CXXABI_H_ 0
+#endif
+
 namespace testing {
 
 class Message;
 
 namespace internal {
 
-class String;
+// A secret type that Google Test users don't know about.  It has no
+// definition on purpose.  Therefore it's impossible to create a
+// Secret object, which is what we want.
+class Secret;
 
 // The GTEST_COMPILE_ASSERT_ macro can be used to verify that a compile time
 // expression is true. For example, you could use it to verify the
@@ -1759,8 +1924,8 @@
 };
 
 #define GTEST_COMPILE_ASSERT_(expr, msg) \
-  typedef ::testing::internal::CompileAssert<(bool(expr))> \
-      msg[bool(expr) ? 1 : -1]
+  typedef ::testing::internal::CompileAssert<(static_cast<bool>(expr))> \
+      msg[static_cast<bool>(expr) ? 1 : -1] GTEST_ATTRIBUTE_UNUSED_
 
 // Implementation details of GTEST_COMPILE_ASSERT_:
 //
@@ -1858,6 +2023,7 @@
       ptr_ = p;
     }
   }
+
  private:
   T* ptr_;
 
@@ -1920,10 +2086,9 @@
  private:
   void Init(const char* regex);
 
-  // We use a const char* instead of a string, as Google Test may be used
-  // where string is not available.  We also do not use Google Test's own
-  // String type here, in order to simplify dependencies between the
-  // files.
+  // We use a const char* instead of an std::string, as Google Test used to be
+  // used where std::string is not available.  TODO(wan@google.com): change to
+  // std::string.
   const char* pattern_;
   bool is_valid_;
 
@@ -2106,20 +2271,21 @@
 //   GetCapturedStderr - stops capturing stderr and returns the captured string.
 //
 GTEST_API_ void CaptureStdout();
-GTEST_API_ String GetCapturedStdout();
+GTEST_API_ std::string GetCapturedStdout();
 GTEST_API_ void CaptureStderr();
-GTEST_API_ String GetCapturedStderr();
+GTEST_API_ std::string GetCapturedStderr();
 
 #endif  // GTEST_HAS_STREAM_REDIRECTION
 
 
 #if GTEST_HAS_DEATH_TEST
 
-// A copy of all command line arguments.  Set by InitGoogleTest().
-extern ::std::vector<String> g_argvs;
+const ::std::vector<testing::internal::string>& GetInjectableArgvs();
+void SetInjectableArgvs(const ::std::vector<testing::internal::string>*
+                             new_argvs);
 
-// GTEST_HAS_DEATH_TEST implies we have ::std::string.
-const ::std::vector<String>& GetArgvs();
+// A copy of all command line arguments.  Set by InitGoogleTest().
+extern ::std::vector<testing::internal::string> g_argvs;
 
 #endif  // GTEST_HAS_DEATH_TEST
 
@@ -2146,22 +2312,37 @@
 // use it in user tests, either directly or indirectly.
 class Notification {
  public:
-  Notification() : notified_(false) {}
+  Notification() : notified_(false) {
+    GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_init(&mutex_, NULL));
+  }
+  ~Notification() {
+    pthread_mutex_destroy(&mutex_);
+  }
 
   // Notifies all threads created with this notification to start. Must
   // be called from the controller thread.
-  void Notify() { notified_ = true; }
+  void Notify() {
+    pthread_mutex_lock(&mutex_);
+    notified_ = true;
+    pthread_mutex_unlock(&mutex_);
+  }
 
   // Blocks until the controller thread notifies. Must be called from a test
   // thread.
   void WaitForNotification() {
-    while(!notified_) {
+    for (;;) {
+      pthread_mutex_lock(&mutex_);
+      const bool notified = notified_;
+      pthread_mutex_unlock(&mutex_);
+      if (notified)
+        break;
       SleepMilliseconds(10);
     }
   }
 
  private:
-  volatile bool notified_;
+  pthread_mutex_t mutex_;
+  bool notified_;
 
   GTEST_DISALLOW_COPY_AND_ASSIGN_(Notification);
 };
@@ -2269,21 +2450,23 @@
   void Lock() {
     GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_lock(&mutex_));
     owner_ = pthread_self();
+    has_owner_ = true;
   }
 
   // Releases this mutex.
   void Unlock() {
-    // We don't protect writing to owner_ here, as it's the caller's
-    // responsibility to ensure that the current thread holds the
+    // Since the lock is being released the owner_ field should no longer be
+    // considered valid. We don't protect writing to has_owner_ here, as it's
+    // the caller's responsibility to ensure that the current thread holds the
     // mutex when this is called.
-    owner_ = 0;
+    has_owner_ = false;
     GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_unlock(&mutex_));
   }
 
   // Does nothing if the current thread holds the mutex. Otherwise, crashes
   // with high probability.
   void AssertHeld() const {
-    GTEST_CHECK_(owner_ == pthread_self())
+    GTEST_CHECK_(has_owner_ && pthread_equal(owner_, pthread_self()))
         << "The current thread is not holding the mutex @" << this;
   }
 
@@ -2294,7 +2477,14 @@
   // have to be public.
  public:
   pthread_mutex_t mutex_;  // The underlying pthread mutex.
-  pthread_t owner_;  // The thread holding the mutex; 0 means no one holds it.
+  // has_owner_ indicates whether the owner_ field below contains a valid thread
+  // ID and is therefore safe to inspect (e.g., to use in pthread_equal()). All
+  // accesses to the owner_ field should be protected by a check of this field.
+  // An alternative might be to memset() owner_ to all zeros, but there's no
+  // guarantee that a zero'd pthread_t is necessarily invalid or even different
+  // from pthread_self().
+  bool has_owner_;
+  pthread_t owner_;  // The thread holding the mutex.
 };
 
 // Forward-declares a static mutex.
@@ -2302,8 +2492,13 @@
     extern ::testing::internal::MutexBase mutex
 
 // Defines and statically (i.e. at link time) initializes a static mutex.
+// The initialization list here does not explicitly initialize each field,
+// instead relying on default initialization for the unspecified fields. In
+// particular, the owner_ field (a pthread_t) is not explicitly initialized.
+// This allows initialization to work whether pthread_t is a scalar or struct.
+// The flag -Wmissing-field-initializers must not be specified for this to work.
 # define GTEST_DEFINE_STATIC_MUTEX_(mutex) \
-    ::testing::internal::MutexBase mutex = { PTHREAD_MUTEX_INITIALIZER, 0 }
+    ::testing::internal::MutexBase mutex = { PTHREAD_MUTEX_INITIALIZER, false }
 
 // The Mutex class can only be used for mutexes created at runtime. It
 // shares its API with MutexBase otherwise.
@@ -2311,7 +2506,7 @@
  public:
   Mutex() {
     GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_init(&mutex_, NULL));
-    owner_ = 0;
+    has_owner_ = false;
   }
   ~Mutex() {
     GTEST_CHECK_POSIX_SUCCESS_(pthread_mutex_destroy(&mutex_));
@@ -2461,6 +2656,8 @@
 class Mutex {
  public:
   Mutex() {}
+  void Lock() {}
+  void Unlock() {}
   void AssertHeld() const {}
 };
 
@@ -2591,6 +2788,10 @@
 inline bool IsXDigit(char ch) {
   return isxdigit(static_cast<unsigned char>(ch)) != 0;
 }
+inline bool IsXDigit(wchar_t ch) {
+  const unsigned char low_byte = static_cast<unsigned char>(ch);
+  return ch == low_byte && isxdigit(low_byte) != 0;
+}
 
 inline char ToLower(char ch) {
   return static_cast<char>(tolower(static_cast<unsigned char>(ch)));
@@ -2728,6 +2929,23 @@
 
 }  // namespace posix
 
+// MSVC "deprecates" snprintf and issues warnings wherever it is used.  In
+// order to avoid these warnings, we need to use _snprintf or _snprintf_s on
+// MSVC-based platforms.  We map the GTEST_SNPRINTF_ macro to the appropriate
+// function in order to achieve that.  We use macro definition here because
+// snprintf is a variadic function.
+#if _MSC_VER >= 1400 && !GTEST_OS_WINDOWS_MOBILE
+// MSVC 2005 and above support variadic macros.
+# define GTEST_SNPRINTF_(buffer, size, format, ...) \
+     _snprintf_s(buffer, size, size, format, __VA_ARGS__)
+#elif defined(_MSC_VER)
+// Windows CE does not define _snprintf_s and MSVC prior to 2005 doesn't
+// complain about _snprintf.
+# define GTEST_SNPRINTF_ _snprintf
+#else
+# define GTEST_SNPRINTF_ snprintf
+#endif
+
 // The maximum number a BiggestInt can represent.  This definition
 // works no matter BiggestInt is represented in one's complement or
 // two's complement.
@@ -2780,7 +2998,6 @@
 template <>
 class TypeWithSize<8> {
  public:
-
 #if GTEST_OS_WINDOWS
   typedef __int64 Int;
   typedef unsigned __int64 UInt;
@@ -2807,7 +3024,7 @@
 #define GTEST_DECLARE_int32_(name) \
     GTEST_API_ extern ::testing::internal::Int32 GTEST_FLAG(name)
 #define GTEST_DECLARE_string_(name) \
-    GTEST_API_ extern ::testing::internal::String GTEST_FLAG(name)
+    GTEST_API_ extern ::std::string GTEST_FLAG(name)
 
 // Macros for defining flags.
 #define GTEST_DEFINE_bool_(name, default_val, doc) \
@@ -2815,7 +3032,11 @@
 #define GTEST_DEFINE_int32_(name, default_val, doc) \
     GTEST_API_ ::testing::internal::Int32 GTEST_FLAG(name) = (default_val)
 #define GTEST_DEFINE_string_(name, default_val, doc) \
-    GTEST_API_ ::testing::internal::String GTEST_FLAG(name) = (default_val)
+    GTEST_API_ ::std::string GTEST_FLAG(name) = (default_val)
+
+// Thread annotations
+#define GTEST_EXCLUSIVE_LOCK_REQUIRED_(locks)
+#define GTEST_LOCK_EXCLUDED_(locks)
 
 // Parses 'str' for a 32-bit signed integer.  If successful, writes the result
 // to *value and returns true; otherwise leaves *value unchanged and returns
@@ -2843,7 +3064,12 @@
 # include <unistd.h>
 #endif  // GTEST_OS_LINUX
 
+#if GTEST_HAS_EXCEPTIONS
+# include <stdexcept>
+#endif
+
 #include <ctype.h>
+#include <float.h>
 #include <string.h>
 #include <iomanip>
 #include <limits>
@@ -2878,6 +3104,255 @@
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 //
+// Author: wan@google.com (Zhanyong Wan)
+//
+// The Google C++ Testing Framework (Google Test)
+//
+// This header file defines the Message class.
+//
+// IMPORTANT NOTE: Due to limitation of the C++ language, we have to
+// leave some internal implementation details in this header file.
+// They are clearly marked by comments like this:
+//
+//   // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
+//
+// Such code is NOT meant to be used by a user directly, and is subject
+// to CHANGE WITHOUT NOTICE.  Therefore DO NOT DEPEND ON IT in a user
+// program!
+
+#ifndef GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
+#define GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
+
+#include <limits>
+
+
+// Ensures that there is at least one operator<< in the global namespace.
+// See Message& operator<<(...) below for why.
+void operator<<(const testing::internal::Secret&, int);
+
+namespace testing {
+
+// The Message class works like an ostream repeater.
+//
+// Typical usage:
+//
+//   1. You stream a bunch of values to a Message object.
+//      It will remember the text in a stringstream.
+//   2. Then you stream the Message object to an ostream.
+//      This causes the text in the Message to be streamed
+//      to the ostream.
+//
+// For example;
+//
+//   testing::Message foo;
+//   foo << 1 << " != " << 2;
+//   std::cout << foo;
+//
+// will print "1 != 2".
+//
+// Message is not intended to be inherited from.  In particular, its
+// destructor is not virtual.
+//
+// Note that stringstream behaves differently in gcc and in MSVC.  You
+// can stream a NULL char pointer to it in the former, but not in the
+// latter (it causes an access violation if you do).  The Message
+// class hides this difference by treating a NULL char pointer as
+// "(null)".
+class GTEST_API_ Message {
+ private:
+  // The type of basic IO manipulators (endl, ends, and flush) for
+  // narrow streams.
+  typedef std::ostream& (*BasicNarrowIoManip)(std::ostream&);
+
+ public:
+  // Constructs an empty Message.
+  Message();
+
+  // Copy constructor.
+  Message(const Message& msg) : ss_(new ::std::stringstream) {  // NOLINT
+    *ss_ << msg.GetString();
+  }
+
+  // Constructs a Message from a C-string.
+  explicit Message(const char* str) : ss_(new ::std::stringstream) {
+    *ss_ << str;
+  }
+
+#if GTEST_OS_SYMBIAN
+  // Streams a value (either a pointer or not) to this object.
+  template <typename T>
+  inline Message& operator <<(const T& value) {
+    StreamHelper(typename internal::is_pointer<T>::type(), value);
+    return *this;
+  }
+#else
+  // Streams a non-pointer value to this object.
+  template <typename T>
+  inline Message& operator <<(const T& val) {
+    // Some libraries overload << for STL containers.  These
+    // overloads are defined in the global namespace instead of ::std.
+    //
+    // C++'s symbol lookup rule (i.e. Koenig lookup) says that these
+    // overloads are visible in either the std namespace or the global
+    // namespace, but not other namespaces, including the testing
+    // namespace which Google Test's Message class is in.
+    //
+    // To allow STL containers (and other types that has a << operator
+    // defined in the global namespace) to be used in Google Test
+    // assertions, testing::Message must access the custom << operator
+    // from the global namespace.  With this using declaration,
+    // overloads of << defined in the global namespace and those
+    // visible via Koenig lookup are both exposed in this function.
+    using ::operator <<;
+    *ss_ << val;
+    return *this;
+  }
+
+  // Streams a pointer value to this object.
+  //
+  // This function is an overload of the previous one.  When you
+  // stream a pointer to a Message, this definition will be used as it
+  // is more specialized.  (The C++ Standard, section
+  // [temp.func.order].)  If you stream a non-pointer, then the
+  // previous definition will be used.
+  //
+  // The reason for this overload is that streaming a NULL pointer to
+  // ostream is undefined behavior.  Depending on the compiler, you
+  // may get "0", "(nil)", "(null)", or an access violation.  To
+  // ensure consistent result across compilers, we always treat NULL
+  // as "(null)".
+  template <typename T>
+  inline Message& operator <<(T* const& pointer) {  // NOLINT
+    if (pointer == NULL) {
+      *ss_ << "(null)";
+    } else {
+      *ss_ << pointer;
+    }
+    return *this;
+  }
+#endif  // GTEST_OS_SYMBIAN
+
+  // Since the basic IO manipulators are overloaded for both narrow
+  // and wide streams, we have to provide this specialized definition
+  // of operator <<, even though its body is the same as the
+  // templatized version above.  Without this definition, streaming
+  // endl or other basic IO manipulators to Message will confuse the
+  // compiler.
+  Message& operator <<(BasicNarrowIoManip val) {
+    *ss_ << val;
+    return *this;
+  }
+
+  // Instead of 1/0, we want to see true/false for bool values.
+  Message& operator <<(bool b) {
+    return *this << (b ? "true" : "false");
+  }
+
+  // These two overloads allow streaming a wide C string to a Message
+  // using the UTF-8 encoding.
+  Message& operator <<(const wchar_t* wide_c_str);
+  Message& operator <<(wchar_t* wide_c_str);
+
+#if GTEST_HAS_STD_WSTRING
+  // Converts the given wide string to a narrow string using the UTF-8
+  // encoding, and streams the result to this Message object.
+  Message& operator <<(const ::std::wstring& wstr);
+#endif  // GTEST_HAS_STD_WSTRING
+
+#if GTEST_HAS_GLOBAL_WSTRING
+  // Converts the given wide string to a narrow string using the UTF-8
+  // encoding, and streams the result to this Message object.
+  Message& operator <<(const ::wstring& wstr);
+#endif  // GTEST_HAS_GLOBAL_WSTRING
+
+  // Gets the text streamed to this object so far as an std::string.
+  // Each '\0' character in the buffer is replaced with "\\0".
+  //
+  // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
+  std::string GetString() const;
+
+ private:
+
+#if GTEST_OS_SYMBIAN
+  // These are needed as the Nokia Symbian Compiler cannot decide between
+  // const T& and const T* in a function template. The Nokia compiler _can_
+  // decide between class template specializations for T and T*, so a
+  // tr1::type_traits-like is_pointer works, and we can overload on that.
+  template <typename T>
+  inline void StreamHelper(internal::true_type /*is_pointer*/, T* pointer) {
+    if (pointer == NULL) {
+      *ss_ << "(null)";
+    } else {
+      *ss_ << pointer;
+    }
+  }
+  template <typename T>
+  inline void StreamHelper(internal::false_type /*is_pointer*/,
+                           const T& value) {
+    // See the comments in Message& operator <<(const T&) above for why
+    // we need this using statement.
+    using ::operator <<;
+    *ss_ << value;
+  }
+#endif  // GTEST_OS_SYMBIAN
+
+  // We'll hold the text streamed to this object here.
+  const internal::scoped_ptr< ::std::stringstream> ss_;
+
+  // We declare (but don't implement) this to prevent the compiler
+  // from implementing the assignment operator.
+  void operator=(const Message&);
+};
+
+// Streams a Message to an ostream.
+inline std::ostream& operator <<(std::ostream& os, const Message& sb) {
+  return os << sb.GetString();
+}
+
+namespace internal {
+
+// Converts a streamable value to an std::string.  A NULL pointer is
+// converted to "(null)".  When the input value is a ::string,
+// ::std::string, ::wstring, or ::std::wstring object, each NUL
+// character in it is replaced with "\\0".
+template <typename T>
+std::string StreamableToString(const T& streamable) {
+  return (Message() << streamable).GetString();
+}
+
+}  // namespace internal
+}  // namespace testing
+
+#endif  // GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
+// Copyright 2005, Google Inc.
+// All rights reserved.
+//
+// Redistribution and use in source and binary forms, with or without
+// modification, are permitted provided that the following conditions are
+// met:
+//
+//     * Redistributions of source code must retain the above copyright
+// notice, this list of conditions and the following disclaimer.
+//     * Redistributions in binary form must reproduce the above
+// copyright notice, this list of conditions and the following disclaimer
+// in the documentation and/or other materials provided with the
+// distribution.
+//     * Neither the name of Google Inc. nor the names of its
+// contributors may be used to endorse or promote products derived from
+// this software without specific prior written permission.
+//
+// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
+// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
+// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
+// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
+// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
+// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
+// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
+// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
+// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
+// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
+// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
+//
 // Authors: wan@google.com (Zhanyong Wan), eefacm@gmail.com (Sean Mcafee)
 //
 // The Google C++ Testing Framework (Google Test)
@@ -2898,49 +3373,17 @@
 #endif
 
 #include <string.h>
-
 #include <string>
 
+
 namespace testing {
 namespace internal {
 
-// String - a UTF-8 string class.
-//
-// For historic reasons, we don't use std::string.
-//
-// TODO(wan@google.com): replace this class with std::string or
-// implement it in terms of the latter.
-//
-// Note that String can represent both NULL and the empty string,
-// while std::string cannot represent NULL.
-//
-// NULL and the empty string are considered different.  NULL is less
-// than anything (including the empty string) except itself.
-//
-// This class only provides minimum functionality necessary for
-// implementing Google Test.  We do not intend to implement a full-fledged
-// string class here.
-//
-// Since the purpose of this class is to provide a substitute for
-// std::string on platforms where it cannot be used, we define a copy
-// constructor and assignment operators such that we don't need
-// conditional compilation in a lot of places.
-//
-// In order to make the representation efficient, the d'tor of String
-// is not virtual.  Therefore DO NOT INHERIT FROM String.
+// String - an abstract class holding static string utilities.
 class GTEST_API_ String {
  public:
   // Static utility methods
 
-  // Returns the input enclosed in double quotes if it's not NULL;
-  // otherwise returns "(null)".  For example, "\"Hello\"" is returned
-  // for input "Hello".
-  //
-  // This is useful for printing a C string in the syntax of a literal.
-  //
-  // Known issue: escape sequences are not handled yet.
-  static String ShowCStringQuoted(const char* c_str);
-
   // Clones a 0-terminated C string, allocating memory using new.  The
   // caller is responsible for deleting the return value using
   // delete[].  Returns the cloned string, or NULL if the input is
@@ -2987,11 +3430,7 @@
   // NULL will be converted to "(null)".  If an error occurred during
   // the conversion, "(failed to convert from wide string)" is
   // returned.
-  static String ShowWideCString(const wchar_t* wide_c_str);
-
-  // Similar to ShowWideCString(), except that this function encloses
-  // the converted string in double quotes.
-  static String ShowWideCStringQuoted(const wchar_t* wide_c_str);
+  static std::string ShowWideCString(const wchar_t* wide_c_str);
 
   // Compares two wide C strings.  Returns true iff they have the same
   // content.
@@ -3025,174 +3464,27 @@
   static bool CaseInsensitiveWideCStringEquals(const wchar_t* lhs,
                                                const wchar_t* rhs);
 
-  // Formats a list of arguments to a String, using the same format
-  // spec string as for printf.
-  //
-  // We do not use the StringPrintf class as it is not universally
-  // available.
-  //
-  // The result is limited to 4096 characters (including the tailing
-  // 0).  If 4096 characters are not enough to format the input,
-  // "<buffer exceeded>" is returned.
-  static String Format(const char* format, ...);
-
-  // C'tors
-
-  // The default c'tor constructs a NULL string.
-  String() : c_str_(NULL), length_(0) {}
-
-  // Constructs a String by cloning a 0-terminated C string.
-  String(const char* a_c_str) {  // NOLINT
-    if (a_c_str == NULL) {
-      c_str_ = NULL;
-      length_ = 0;
-    } else {
-      ConstructNonNull(a_c_str, strlen(a_c_str));
-    }
-  }
-
-  // Constructs a String by copying a given number of chars from a
-  // buffer.  E.g. String("hello", 3) creates the string "hel",
-  // String("a\0bcd", 4) creates "a\0bc", String(NULL, 0) creates "",
-  // and String(NULL, 1) results in access violation.
-  String(const char* buffer, size_t a_length) {
-    ConstructNonNull(buffer, a_length);
-  }
-
-  // The copy c'tor creates a new copy of the string.  The two
-  // String objects do not share content.
-  String(const String& str) : c_str_(NULL), length_(0) { *this = str; }
-
-  // D'tor.  String is intended to be a final class, so the d'tor
-  // doesn't need to be virtual.
-  ~String() { delete[] c_str_; }
-
-  // Allows a String to be implicitly converted to an ::std::string or
-  // ::string, and vice versa.  Converting a String containing a NULL
-  // pointer to ::std::string or ::string is undefined behavior.
-  // Converting a ::std::string or ::string containing an embedded NUL
-  // character to a String will result in the prefix up to the first
-  // NUL character.
-  String(const ::std::string& str) {
-    ConstructNonNull(str.c_str(), str.length());
-  }
-
-  operator ::std::string() const { return ::std::string(c_str(), length()); }
-
-#if GTEST_HAS_GLOBAL_STRING
-  String(const ::string& str) {
-    ConstructNonNull(str.c_str(), str.length());
-  }
-
-  operator ::string() const { return ::string(c_str(), length()); }
-#endif  // GTEST_HAS_GLOBAL_STRING
-
-  // Returns true iff this is an empty string (i.e. "").
-  bool empty() const { return (c_str() != NULL) && (length() == 0); }
-
-  // Compares this with another String.
-  // Returns < 0 if this is less than rhs, 0 if this is equal to rhs, or > 0
-  // if this is greater than rhs.
-  int Compare(const String& rhs) const;
-
-  // Returns true iff this String equals the given C string.  A NULL
-  // string and a non-NULL string are considered not equal.
-  bool operator==(const char* a_c_str) const { return Compare(a_c_str) == 0; }
-
-  // Returns true iff this String is less than the given String.  A
-  // NULL string is considered less than "".
-  bool operator<(const String& rhs) const { return Compare(rhs) < 0; }
-
-  // Returns true iff this String doesn't equal the given C string.  A NULL
-  // string and a non-NULL string are considered not equal.
-  bool operator!=(const char* a_c_str) const { return !(*this == a_c_str); }
-
-  // Returns true iff this String ends with the given suffix.  *Any*
-  // String is considered to end with a NULL or empty suffix.
-  bool EndsWith(const char* suffix) const;
-
-  // Returns true iff this String ends with the given suffix, not considering
-  // case. Any String is considered to end with a NULL or empty suffix.
-  bool EndsWithCaseInsensitive(const char* suffix) const;
-
-  // Returns the length of the encapsulated string, or 0 if the
-  // string is NULL.
-  size_t length() const { return length_; }
-
-  // Gets the 0-terminated C string this String object represents.
-  // The String object still owns the string.  Therefore the caller
-  // should NOT delete the return value.
-  const char* c_str() const { return c_str_; }
+  // Returns true iff the given string ends with the given suffix, ignoring
+  // case. Any string is considered to end with an empty suffix.
+  static bool EndsWithCaseInsensitive(
+      const std::string& str, const std::string& suffix);
 
-  // Assigns a C string to this object.  Self-assignment works.
-  const String& operator=(const char* a_c_str) {
-    return *this = String(a_c_str);
-  }
+  // Formats an int value as "%02d".
+  static std::string FormatIntWidth2(int value);  // "%02d" for width == 2
 
-  // Assigns a String object to this object.  Self-assignment works.
-  const String& operator=(const String& rhs) {
-    if (this != &rhs) {
-      delete[] c_str_;
-      if (rhs.c_str() == NULL) {
-        c_str_ = NULL;
-        length_ = 0;
-      } else {
-        ConstructNonNull(rhs.c_str(), rhs.length());
-      }
-    }
+  // Formats an int value as "%X".
+  static std::string FormatHexInt(int value);
 
-    return *this;
-  }
+  // Formats a byte as "%02X".
+  static std::string FormatByte(unsigned char value);
 
  private:
-  // Constructs a non-NULL String from the given content.  This
-  // function can only be called when c_str_ has not been allocated.
-  // ConstructNonNull(NULL, 0) results in an empty string ("").
-  // ConstructNonNull(NULL, non_zero) is undefined behavior.
-  void ConstructNonNull(const char* buffer, size_t a_length) {
-    char* const str = new char[a_length + 1];
-    memcpy(str, buffer, a_length);
-    str[a_length] = '\0';
-    c_str_ = str;
-    length_ = a_length;
-  }
-
-  const char* c_str_;
-  size_t length_;
+  String();  // Not meant to be instantiated.
 };  // class String
 
-// Streams a String to an ostream.  Each '\0' character in the String
-// is replaced with "\\0".
-inline ::std::ostream& operator<<(::std::ostream& os, const String& str) {
-  if (str.c_str() == NULL) {
-    os << "(null)";
-  } else {
-    const char* const c_str = str.c_str();
-    for (size_t i = 0; i != str.length(); i++) {
-      if (c_str[i] == '\0') {
-        os << "\\0";
-      } else {
-        os << c_str[i];
-      }
-    }
-  }
-  return os;
-}
-
-// Gets the content of the stringstream's buffer as a String.  Each '\0'
+// Gets the content of the stringstream's buffer as an std::string.  Each '\0'
 // character in the buffer is replaced with "\\0".
-GTEST_API_ String StringStreamToString(::std::stringstream* stream);
-
-// Converts a streamable value to a String.  A NULL pointer is
-// converted to "(null)".  When the input value is a ::string,
-// ::std::string, ::wstring, or ::std::wstring object, each NUL
-// character in it is replaced with "\\0".
-
-// Declared here but defined in gtest.h, so that it has access
-// to the definition of the Message class, required by the ARM
-// compiler.
-template <typename T>
-String StreamableToString(const T& streamable);
+GTEST_API_ std::string StringStreamToString(::std::stringstream* stream);
 
 }  // namespace internal
 }  // namespace testing
@@ -3260,11 +3552,7 @@
   FilePath() : pathname_("") { }
   FilePath(const FilePath& rhs) : pathname_(rhs.pathname_) { }
 
-  explicit FilePath(const char* pathname) : pathname_(pathname) {
-    Normalize();
-  }
-
-  explicit FilePath(const String& pathname) : pathname_(pathname) {
+  explicit FilePath(const std::string& pathname) : pathname_(pathname) {
     Normalize();
   }
 
@@ -3277,7 +3565,7 @@
     pathname_ = rhs.pathname_;
   }
 
-  String ToString() const { return pathname_; }
+  const std::string& string() const { return pathname_; }
   const char* c_str() const { return pathname_.c_str(); }
 
   // Returns the current working directory, or "" if unsuccessful.
@@ -3310,8 +3598,8 @@
                                          const FilePath& base_name,
                                          const char* extension);
 
-  // Returns true iff the path is NULL or "".
-  bool IsEmpty() const { return c_str() == NULL || *c_str() == '\0'; }
+  // Returns true iff the path is "".
+  bool IsEmpty() const { return pathname_.empty(); }
 
   // If input name has a trailing separator character, removes it and returns
   // the name, otherwise return the name string unmodified.
@@ -3400,7 +3688,7 @@
   // separators. Returns NULL if no path separator was found.
   const char* FindLastPathSeparator() const;
 
-  String pathname_;
+  std::string pathname_;
 };  // class FilePath
 
 }  // namespace internal
@@ -3456,11 +3744,11 @@
 
 // #ifdef __GNUC__ is too general here.  It is possible to use gcc without using
 // libstdc++ (which is where cxxabi.h comes from).
-# ifdef __GLIBCXX__
+# if GTEST_HAS_CXXABI_H_
 #  include <cxxabi.h>
 # elif defined(__HP_aCC)
 #  include <acxx_demangle.h>
-# endif  // __GLIBCXX__
+# endif  // GTEST_HASH_CXXABI_H_
 
 namespace testing {
 namespace internal {
@@ -3469,24 +3757,24 @@
 // NB: This function is also used in Google Mock, so don't move it inside of
 // the typed-test-only section below.
 template <typename T>
-String GetTypeName() {
+std::string GetTypeName() {
 # if GTEST_HAS_RTTI
 
   const char* const name = typeid(T).name();
-#  if defined(__GLIBCXX__) || defined(__HP_aCC)
+#  if GTEST_HAS_CXXABI_H_ || defined(__HP_aCC)
   int status = 0;
   // gcc's implementation of typeid(T).name() mangles the type name,
   // so we have to demangle it.
-#   ifdef __GLIBCXX__
+#   if GTEST_HAS_CXXABI_H_
   using abi::__cxa_demangle;
-#   endif // __GLIBCXX__
+#   endif  // GTEST_HAS_CXXABI_H_
   char* const readable_name = __cxa_demangle(name, 0, 0, &status);
-  const String name_str(status == 0 ? readable_name : name);
+  const std::string name_str(status == 0 ? readable_name : name);
   free(readable_name);
   return name_str;
 #  else
   return name;
-#  endif  // __GLIBCXX__ || __HP_aCC
+#  endif  // GTEST_HAS_CXXABI_H_ || __HP_aCC
 
 # else
 
@@ -6707,7 +6995,9 @@
 // INSTANTIATE_TYPED_TEST_CASE_P().
 
 template <typename T>
-struct TypeList { typedef Types1<T> type; };
+struct TypeList {
+  typedef Types1<T> type;
+};
 
 template <typename T1, typename T2, typename T3, typename T4, typename T5,
     typename T6, typename T7, typename T8, typename T9, typename T10,
@@ -6747,36 +7037,6 @@
 #define GTEST_CONCAT_TOKEN_(foo, bar) GTEST_CONCAT_TOKEN_IMPL_(foo, bar)
 #define GTEST_CONCAT_TOKEN_IMPL_(foo, bar) foo ## bar
 
-// Google Test defines the testing::Message class to allow construction of
-// test messages via the << operator.  The idea is that anything
-// streamable to std::ostream can be streamed to a testing::Message.
-// This allows a user to use his own types in Google Test assertions by
-// overloading the << operator.
-//
-// util/gtl/stl_logging-inl.h overloads << for STL containers.  These
-// overloads cannot be defined in the std namespace, as that will be
-// undefined behavior.  Therefore, they are defined in the global
-// namespace instead.
-//
-// C++'s symbol lookup rule (i.e. Koenig lookup) says that these
-// overloads are visible in either the std namespace or the global
-// namespace, but not other namespaces, including the testing
-// namespace which Google Test's Message class is in.
-//
-// To allow STL containers (and other types that has a << operator
-// defined in the global namespace) to be used in Google Test assertions,
-// testing::Message must access the custom << operator from the global
-// namespace.  Hence this helper function.
-//
-// Note: Jeffrey Yasskin suggested an alternative fix by "using
-// ::operator<<;" in the definition of Message's operator<<.  That fix
-// doesn't require a helper function, but unfortunately doesn't
-// compile with MSVC.
-template <typename T>
-inline void GTestStreamToHelper(std::ostream* os, const T& val) {
-  *os << val;
-}
-
 class ProtocolMessage;
 namespace proto2 { class Message; }
 
@@ -6802,17 +7062,12 @@
 class UnitTestImpl;                    // Opaque implementation of UnitTest
 
 // How many times InitGoogleTest() has been called.
-extern int g_init_gtest_count;
+GTEST_API_ extern int g_init_gtest_count;
 
 // The text used in failure messages to indicate the start of the
 // stack trace.
 GTEST_API_ extern const char kStackTraceMarker[];
 
-// A secret type that Google Test users don't know about.  It has no
-// definition on purpose.  Therefore it's impossible to create a
-// Secret object, which is what we want.
-class Secret;
-
 // Two overloaded helpers for checking at compile time whether an
 // expression is a null pointer literal (i.e. NULL or any 0-valued
 // compile-time integral constant).  Their return values have
@@ -6843,8 +7098,23 @@
 #endif  // GTEST_ELLIPSIS_NEEDS_POD_
 
 // Appends the user-supplied message to the Google-Test-generated message.
-GTEST_API_ String AppendUserMessage(const String& gtest_msg,
-                                    const Message& user_msg);
+GTEST_API_ std::string AppendUserMessage(
+    const std::string& gtest_msg, const Message& user_msg);
+
+#if GTEST_HAS_EXCEPTIONS
+
+// This exception is thrown by (and only by) a failed Google Test
+// assertion when GTEST_FLAG(throw_on_failure) is true (if exceptions
+// are enabled).  We derive it from std::runtime_error, which is for
+// errors presumably detectable only at run time.  Since
+// std::runtime_error inherits from std::exception, many testing
+// frameworks know how to extract and print the message inside it.
+class GTEST_API_ GoogleTestFailureException : public ::std::runtime_error {
+ public:
+  explicit GoogleTestFailureException(const TestPartResult& failure);
+};
+
+#endif  // GTEST_HAS_EXCEPTIONS
 
 // A helper class for creating scoped traces in user programs.
 class GTEST_API_ ScopedTrace {
@@ -6865,77 +7135,6 @@
                             // c'tor and d'tor.  Therefore it doesn't
                             // need to be used otherwise.
 
-// Converts a streamable value to a String.  A NULL pointer is
-// converted to "(null)".  When the input value is a ::string,
-// ::std::string, ::wstring, or ::std::wstring object, each NUL
-// character in it is replaced with "\\0".
-// Declared here but defined in gtest.h, so that it has access
-// to the definition of the Message class, required by the ARM
-// compiler.
-template <typename T>
-String StreamableToString(const T& streamable);
-
-// The Symbian compiler has a bug that prevents it from selecting the
-// correct overload of FormatForComparisonFailureMessage (see below)
-// unless we pass the first argument by reference.  If we do that,
-// however, Visual Age C++ 10.1 generates a compiler error.  Therefore
-// we only apply the work-around for Symbian.
-#if defined(__SYMBIAN32__)
-# define GTEST_CREF_WORKAROUND_ const&
-#else
-# define GTEST_CREF_WORKAROUND_
-#endif
-
-// When this operand is a const char* or char*, if the other operand
-// is a ::std::string or ::string, we print this operand as a C string
-// rather than a pointer (we do the same for wide strings); otherwise
-// we print it as a pointer to be safe.
-
-// This internal macro is used to avoid duplicated code.
-#define GTEST_FORMAT_IMPL_(operand2_type, operand1_printer)\
-inline String FormatForComparisonFailureMessage(\
-    operand2_type::value_type* GTEST_CREF_WORKAROUND_ str, \
-    const operand2_type& /*operand2*/) {\
-  return operand1_printer(str);\
-}\
-inline String FormatForComparisonFailureMessage(\
-    const operand2_type::value_type* GTEST_CREF_WORKAROUND_ str, \
-    const operand2_type& /*operand2*/) {\
-  return operand1_printer(str);\
-}
-
-GTEST_FORMAT_IMPL_(::std::string, String::ShowCStringQuoted)
-#if GTEST_HAS_STD_WSTRING
-GTEST_FORMAT_IMPL_(::std::wstring, String::ShowWideCStringQuoted)
-#endif  // GTEST_HAS_STD_WSTRING
-
-#if GTEST_HAS_GLOBAL_STRING
-GTEST_FORMAT_IMPL_(::string, String::ShowCStringQuoted)
-#endif  // GTEST_HAS_GLOBAL_STRING
-#if GTEST_HAS_GLOBAL_WSTRING
-GTEST_FORMAT_IMPL_(::wstring, String::ShowWideCStringQuoted)
-#endif  // GTEST_HAS_GLOBAL_WSTRING
-
-#undef GTEST_FORMAT_IMPL_
-
-// The next four overloads handle the case where the operand being
-// printed is a char/wchar_t pointer and the other operand is not a
-// string/wstring object.  In such cases, we just print the operand as
-// a pointer to be safe.
-#define GTEST_FORMAT_CHAR_PTR_IMPL_(CharType)                       \
-  template <typename T>                                             \
-  String FormatForComparisonFailureMessage(CharType* GTEST_CREF_WORKAROUND_ p, \
-                                           const T&) { \
-    return PrintToString(static_cast<const void*>(p));              \
-  }
-
-GTEST_FORMAT_CHAR_PTR_IMPL_(char)
-GTEST_FORMAT_CHAR_PTR_IMPL_(const char)
-GTEST_FORMAT_CHAR_PTR_IMPL_(wchar_t)
-GTEST_FORMAT_CHAR_PTR_IMPL_(const wchar_t)
-
-#undef GTEST_FORMAT_CHAR_PTR_IMPL_
-
 // Constructs and returns the message for an equality assertion
 // (e.g. ASSERT_EQ, EXPECT_STREQ, etc) failure.
 //
@@ -6953,12 +7152,12 @@
 // be inserted into the message.
 GTEST_API_ AssertionResult EqFailure(const char* expected_expression,
                                      const char* actual_expression,
-                                     const String& expected_value,
-                                     const String& actual_value,
+                                     const std::string& expected_value,
+                                     const std::string& actual_value,
                                      bool ignoring_case);
 
 // Constructs a failure message for Boolean assertions such as EXPECT_TRUE.
-GTEST_API_ String GetBoolAssertionFailureMessage(
+GTEST_API_ std::string GetBoolAssertionFailureMessage(
     const AssertionResult& assertion_result,
     const char* expression_text,
     const char* actual_predicate_value,
@@ -7033,7 +7232,7 @@
   // bits.  Therefore, 4 should be enough for ordinary use.
   //
   // See the following article for more details on ULP:
-  // http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm.
+  // http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
   static const size_t kMaxUlps = 4;
 
   // Constructs a FloatingPoint from a raw floating-point number.
@@ -7060,6 +7259,9 @@
     return ReinterpretBits(kExponentBitMask);
   }
 
+  // Returns the maximum representable finite floating-point number.
+  static RawType Max();
+
   // Non-static methods
 
   // Returns the bits that represents this number.
@@ -7140,6 +7342,13 @@
   FloatingPointUnion u_;
 };
 
+// We cannot use std::numeric_limits<T>::max() as it clashes with the max()
+// macro defined by <windows.h>.
+template <>
+inline float FloatingPoint<float>::Max() { return FLT_MAX; }
+template <>
+inline double FloatingPoint<double>::Max() { return DBL_MAX; }
+
 // Typedefs the instances of the FloatingPoint template class that we
 // care to use.
 typedef FloatingPoint<float> Float;
@@ -7234,7 +7443,7 @@
 //   test_case_name:   name of the test case
 //   name:             name of the test
 //   type_param        the name of the test's type parameter, or NULL if
-//                     this is not  a typed or a type-parameterized test.
+//                     this is not a typed or a type-parameterized test.
 //   value_param       text representation of the test's value parameter,
 //                     or NULL if this is not a type-parameterized test.
 //   fixture_class_id: ID of the test fixture class
@@ -7244,7 +7453,8 @@
 //                     The newly created TestInfo instance will assume
 //                     ownership of the factory object.
 GTEST_API_ TestInfo* MakeAndRegisterTestInfo(
-    const char* test_case_name, const char* name,
+    const char* test_case_name,
+    const char* name,
     const char* type_param,
     const char* value_param,
     TypeId fixture_class_id,
@@ -7304,9 +7514,9 @@
 
 // Returns the prefix of 'str' before the first comma in it; returns
 // the entire string if it contains no comma.
-inline String GetPrefixUntilComma(const char* str) {
+inline std::string GetPrefixUntilComma(const char* str) {
   const char* comma = strchr(str, ',');
-  return comma == NULL ? String(str) : String(str, comma - str);
+  return comma == NULL ? str : std::string(str, comma);
 }
 
 // TypeParameterizedTest<Fixture, TestSel, Types>::Register()
@@ -7332,8 +7542,8 @@
     // First, registers the first type-parameterized test in the type
     // list.
     MakeAndRegisterTestInfo(
-        String::Format("%s%s%s/%d", prefix, prefix[0] == '\0' ? "" : "/",
-                       case_name, index).c_str(),
+        (std::string(prefix) + (prefix[0] == '\0' ? "" : "/") + case_name + "/"
+         + StreamableToString(index)).c_str(),
         GetPrefixUntilComma(test_names).c_str(),
         GetTypeName<Type>().c_str(),
         NULL,  // No value parameter.
@@ -7391,7 +7601,7 @@
 
 #endif  // GTEST_HAS_TYPED_TEST || GTEST_HAS_TYPED_TEST_P
 
-// Returns the current OS stack trace as a String.
+// Returns the current OS stack trace as an std::string.
 //
 // The maximum number of stack frames to be included is specified by
 // the gtest_stack_trace_depth flag.  The skip_count parameter
@@ -7401,8 +7611,8 @@
 // For example, if Foo() calls Bar(), which in turn calls
 // GetCurrentOsStackTraceExceptTop(..., 1), Foo() will be included in
 // the trace but Bar() and GetCurrentOsStackTraceExceptTop() won't.
-GTEST_API_ String GetCurrentOsStackTraceExceptTop(UnitTest* unit_test,
-                                                  int skip_count);
+GTEST_API_ std::string GetCurrentOsStackTraceExceptTop(
+    UnitTest* unit_test, int skip_count);
 
 // Helpers for suppressing warnings on unreachable code or constant
 // condition.
@@ -7477,13 +7687,19 @@
 // MSVC 8.0, Sun C++, and IBM XL C++ have a bug which causes the above
 // definition to fail to remove the const in 'const int[3]' and 'const
 // char[3][4]'.  The following specialization works around the bug.
-// However, it causes trouble with GCC and thus needs to be
-// conditionally compiled.
-#if defined(_MSC_VER) || defined(__SUNPRO_CC) || defined(__IBMCPP__)
 template <typename T, size_t N>
 struct RemoveConst<const T[N]> {
   typedef typename RemoveConst<T>::type type[N];
 };
+
+#if defined(_MSC_VER) && _MSC_VER < 1400
+// This is the only specialization that allows VC++ 7.1 to remove const in
+// 'const int[3] and 'const int[3][4]'.  However, it causes trouble with GCC
+// and thus needs to be conditionally compiled.
+template <typename T, size_t N>
+struct RemoveConst<T[N]> {
+  typedef typename RemoveConst<T>::type type[N];
+};
 #endif
 
 // A handy wrapper around RemoveConst that works when the argument
@@ -8072,11 +8288,11 @@
   // the last death test.
   static const char* LastMessage();
 
-  static void set_last_death_test_message(const String& message);
+  static void set_last_death_test_message(const std::string& message);
 
  private:
   // A string containing a description of the outcome of the last death test.
-  static String last_death_test_message_;
+  static std::string last_death_test_message_;
 
   GTEST_DISALLOW_COPY_AND_ASSIGN_(DeathTest);
 };
@@ -8162,12 +8378,23 @@
 // The symbol "fail" here expands to something into which a message
 // can be streamed.
 
+// This macro is for implementing ASSERT/EXPECT_DEBUG_DEATH when compiled in
+// NDEBUG mode. In this case we need the statements to be executed, the regex is
+// ignored, and the macro must accept a streamed message even though the message
+// is never printed.
+# define GTEST_EXECUTE_STATEMENT_(statement, regex) \
+  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
+  if (::testing::internal::AlwaysTrue()) { \
+     GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
+  } else \
+    ::testing::Message()
+
 // A class representing the parsed contents of the
 // --gtest_internal_run_death_test flag, as it existed when
 // RUN_ALL_TESTS was called.
 class InternalRunDeathTestFlag {
  public:
-  InternalRunDeathTestFlag(const String& a_file,
+  InternalRunDeathTestFlag(const std::string& a_file,
                            int a_line,
                            int an_index,
                            int a_write_fd)
@@ -8179,13 +8406,13 @@
       posix::Close(write_fd_);
   }
 
-  String file() const { return file_; }
+  const std::string& file() const { return file_; }
   int line() const { return line_; }
   int index() const { return index_; }
   int write_fd() const { return write_fd_; }
 
  private:
-  String file_;
+  std::string file_;
   int line_;
   int index_;
   int write_fd_;
@@ -8263,6 +8490,17 @@
 
 #if GTEST_HAS_DEATH_TEST
 
+namespace internal {
+
+// Returns a Boolean value indicating whether the caller is currently
+// executing in the context of the death test child process.  Tools such as
+// Valgrind heap checkers may need this to modify their behavior in death
+// tests.  IMPORTANT: This is an internal utility.  Using it may break the
+// implementation of death tests.  User code MUST NOT use it.
+GTEST_API_ bool InDeathTestChild();
+
+}  // namespace internal
+
 // The following macros are useful for writing death tests.
 
 // Here's what happens when an ASSERT_DEATH* or EXPECT_DEATH* is
@@ -8287,7 +8525,7 @@
 //   for (int i = 0; i < 5; i++) {
 //     EXPECT_DEATH(server.ProcessRequest(i),
 //                  "Invalid request .* in ProcessRequest()")
-//         << "Failed to die on request " << i);
+//                  << "Failed to die on request " << i;
 //   }
 //
 //   ASSERT_EXIT(server.ExitNow(), ::testing::ExitedWithCode(0), "Exiting");
@@ -8457,10 +8695,10 @@
 # ifdef NDEBUG
 
 #  define EXPECT_DEBUG_DEATH(statement, regex) \
-  do { statement; } while (::testing::internal::AlwaysFalse())
+  GTEST_EXECUTE_STATEMENT_(statement, regex)
 
 #  define ASSERT_DEBUG_DEATH(statement, regex) \
-  do { statement; } while (::testing::internal::AlwaysFalse())
+  GTEST_EXECUTE_STATEMENT_(statement, regex)
 
 # else
 
@@ -8493,234 +8731,6 @@
 }  // namespace testing
 
 #endif  // GTEST_INCLUDE_GTEST_GTEST_DEATH_TEST_H_
-// Copyright 2005, Google Inc.
-// All rights reserved.
-//
-// Redistribution and use in source and binary forms, with or without
-// modification, are permitted provided that the following conditions are
-// met:
-//
-//     * Redistributions of source code must retain the above copyright
-// notice, this list of conditions and the following disclaimer.
-//     * Redistributions in binary form must reproduce the above
-// copyright notice, this list of conditions and the following disclaimer
-// in the documentation and/or other materials provided with the
-// distribution.
-//     * Neither the name of Google Inc. nor the names of its
-// contributors may be used to endorse or promote products derived from
-// this software without specific prior written permission.
-//
-// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
-// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
-// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
-// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
-// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
-// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
-// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
-// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
-// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
-// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-//
-// Author: wan@google.com (Zhanyong Wan)
-//
-// The Google C++ Testing Framework (Google Test)
-//
-// This header file defines the Message class.
-//
-// IMPORTANT NOTE: Due to limitation of the C++ language, we have to
-// leave some internal implementation details in this header file.
-// They are clearly marked by comments like this:
-//
-//   // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
-//
-// Such code is NOT meant to be used by a user directly, and is subject
-// to CHANGE WITHOUT NOTICE.  Therefore DO NOT DEPEND ON IT in a user
-// program!
-
-#ifndef GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
-#define GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
-
-#include <limits>
-
-
-namespace testing {
-
-// The Message class works like an ostream repeater.
-//
-// Typical usage:
-//
-//   1. You stream a bunch of values to a Message object.
-//      It will remember the text in a stringstream.
-//   2. Then you stream the Message object to an ostream.
-//      This causes the text in the Message to be streamed
-//      to the ostream.
-//
-// For example;
-//
-//   testing::Message foo;
-//   foo << 1 << " != " << 2;
-//   std::cout << foo;
-//
-// will print "1 != 2".
-//
-// Message is not intended to be inherited from.  In particular, its
-// destructor is not virtual.
-//
-// Note that stringstream behaves differently in gcc and in MSVC.  You
-// can stream a NULL char pointer to it in the former, but not in the
-// latter (it causes an access violation if you do).  The Message
-// class hides this difference by treating a NULL char pointer as
-// "(null)".
-class GTEST_API_ Message {
- private:
-  // The type of basic IO manipulators (endl, ends, and flush) for
-  // narrow streams.
-  typedef std::ostream& (*BasicNarrowIoManip)(std::ostream&);
-
- public:
-  // Constructs an empty Message.
-  // We allocate the stringstream separately because otherwise each use of
-  // ASSERT/EXPECT in a procedure adds over 200 bytes to the procedure's
-  // stack frame leading to huge stack frames in some cases; gcc does not reuse
-  // the stack space.
-  Message() : ss_(new ::std::stringstream) {
-    // By default, we want there to be enough precision when printing
-    // a double to a Message.
-    *ss_ << std::setprecision(std::numeric_limits<double>::digits10 + 2);
-  }
-
-  // Copy constructor.
-  Message(const Message& msg) : ss_(new ::std::stringstream) {  // NOLINT
-    *ss_ << msg.GetString();
-  }
-
-  // Constructs a Message from a C-string.
-  explicit Message(const char* str) : ss_(new ::std::stringstream) {
-    *ss_ << str;
-  }
-
-#if GTEST_OS_SYMBIAN
-  // Streams a value (either a pointer or not) to this object.
-  template <typename T>
-  inline Message& operator <<(const T& value) {
-    StreamHelper(typename internal::is_pointer<T>::type(), value);
-    return *this;
-  }
-#else
-  // Streams a non-pointer value to this object.
-  template <typename T>
-  inline Message& operator <<(const T& val) {
-    ::GTestStreamToHelper(ss_.get(), val);
-    return *this;
-  }
-
-  // Streams a pointer value to this object.
-  //
-  // This function is an overload of the previous one.  When you
-  // stream a pointer to a Message, this definition will be used as it
-  // is more specialized.  (The C++ Standard, section
-  // [temp.func.order].)  If you stream a non-pointer, then the
-  // previous definition will be used.
-  //
-  // The reason for this overload is that streaming a NULL pointer to
-  // ostream is undefined behavior.  Depending on the compiler, you
-  // may get "0", "(nil)", "(null)", or an access violation.  To
-  // ensure consistent result across compilers, we always treat NULL
-  // as "(null)".
-  template <typename T>
-  inline Message& operator <<(T* const& pointer) {  // NOLINT
-    if (pointer == NULL) {
-      *ss_ << "(null)";
-    } else {
-      ::GTestStreamToHelper(ss_.get(), pointer);
-    }
-    return *this;
-  }
-#endif  // GTEST_OS_SYMBIAN
-
-  // Since the basic IO manipulators are overloaded for both narrow
-  // and wide streams, we have to provide this specialized definition
-  // of operator <<, even though its body is the same as the
-  // templatized version above.  Without this definition, streaming
-  // endl or other basic IO manipulators to Message will confuse the
-  // compiler.
-  Message& operator <<(BasicNarrowIoManip val) {
-    *ss_ << val;
-    return *this;
-  }
-
-  // Instead of 1/0, we want to see true/false for bool values.
-  Message& operator <<(bool b) {
-    return *this << (b ? "true" : "false");
-  }
-
-  // These two overloads allow streaming a wide C string to a Message
-  // using the UTF-8 encoding.
-  Message& operator <<(const wchar_t* wide_c_str) {
-    return *this << internal::String::ShowWideCString(wide_c_str);
-  }
-  Message& operator <<(wchar_t* wide_c_str) {
-    return *this << internal::String::ShowWideCString(wide_c_str);
-  }
-
-#if GTEST_HAS_STD_WSTRING
-  // Converts the given wide string to a narrow string using the UTF-8
-  // encoding, and streams the result to this Message object.
-  Message& operator <<(const ::std::wstring& wstr);
-#endif  // GTEST_HAS_STD_WSTRING
-
-#if GTEST_HAS_GLOBAL_WSTRING
-  // Converts the given wide string to a narrow string using the UTF-8
-  // encoding, and streams the result to this Message object.
-  Message& operator <<(const ::wstring& wstr);
-#endif  // GTEST_HAS_GLOBAL_WSTRING
-
-  // Gets the text streamed to this object so far as a String.
-  // Each '\0' character in the buffer is replaced with "\\0".
-  //
-  // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
-  internal::String GetString() const {
-    return internal::StringStreamToString(ss_.get());
-  }
-
- private:
-
-#if GTEST_OS_SYMBIAN
-  // These are needed as the Nokia Symbian Compiler cannot decide between
-  // const T& and const T* in a function template. The Nokia compiler _can_
-  // decide between class template specializations for T and T*, so a
-  // tr1::type_traits-like is_pointer works, and we can overload on that.
-  template <typename T>
-  inline void StreamHelper(internal::true_type /*dummy*/, T* pointer) {
-    if (pointer == NULL) {
-      *ss_ << "(null)";
-    } else {
-      ::GTestStreamToHelper(ss_.get(), pointer);
-    }
-  }
-  template <typename T>
-  inline void StreamHelper(internal::false_type /*dummy*/, const T& value) {
-    ::GTestStreamToHelper(ss_.get(), value);
-  }
-#endif  // GTEST_OS_SYMBIAN
-
-  // We'll hold the text streamed to this object here.
-  const internal::scoped_ptr< ::std::stringstream> ss_;
-
-  // We declare (but don't implement) this to prevent the compiler
-  // from implementing the assignment operator.
-  void operator=(const Message&);
-};
-
-// Streams a Message to an ostream.
-inline std::ostream& operator <<(std::ostream& os, const Message& sb) {
-  return os << sb.GetString();
-}
-
-}  // namespace testing
-
-#endif  // GTEST_INCLUDE_GTEST_GTEST_MESSAGE_H_
 // This file was GENERATED by command:
 //     pump.py gtest-param-test.h.pump
 // DO NOT EDIT BY HAND!!!
@@ -9059,8 +9069,8 @@
   // framework.
 
   // Join an existing circle.
-  // L < g_linked_ptr_mutex
-  void join(linked_ptr_internal const* ptr) {
+  void join(linked_ptr_internal const* ptr)
+      GTEST_LOCK_EXCLUDED_(g_linked_ptr_mutex) {
     MutexLock lock(&g_linked_ptr_mutex);
 
     linked_ptr_internal const* p = ptr;
@@ -9071,8 +9081,8 @@
 
   // Leave whatever circle we're part of.  Returns true if we were the
   // last member of the circle.  Once this is done, you can join() another.
-  // L < g_linked_ptr_mutex
-  bool depart() {
+  bool depart()
+      GTEST_LOCK_EXCLUDED_(g_linked_ptr_mutex) {
     MutexLock lock(&g_linked_ptr_mutex);
 
     if (next_ == this) return true;
@@ -9815,9 +9825,12 @@
   }
 }
 // This overload prints a (const) char array compactly.
-GTEST_API_ void UniversalPrintArray(const char* begin,
-                                    size_t len,
-                                    ::std::ostream* os);
+GTEST_API_ void UniversalPrintArray(
+    const char* begin, size_t len, ::std::ostream* os);
+
+// This overload prints a (const) wchar_t array compactly.
+GTEST_API_ void UniversalPrintArray(
+    const wchar_t* begin, size_t len, ::std::ostream* os);
 
 // Implements printing an array type T[N].
 template <typename T, size_t N>
@@ -9858,19 +9871,72 @@
 // Prints a value tersely: for a reference type, the referenced value
 // (but not the address) is printed; for a (const) char pointer, the
 // NUL-terminated string (but not the pointer) is printed.
+
 template <typename T>
-void UniversalTersePrint(const T& value, ::std::ostream* os) {
-  UniversalPrint(value, os);
-}
-inline void UniversalTersePrint(const char* str, ::std::ostream* os) {
-  if (str == NULL) {
-    *os << "NULL";
-  } else {
-    UniversalPrint(string(str), os);
+class UniversalTersePrinter {
+ public:
+  static void Print(const T& value, ::std::ostream* os) {
+    UniversalPrint(value, os);
   }
-}
-inline void UniversalTersePrint(char* str, ::std::ostream* os) {
-  UniversalTersePrint(static_cast<const char*>(str), os);
+};
+template <typename T>
+class UniversalTersePrinter<T&> {
+ public:
+  static void Print(const T& value, ::std::ostream* os) {
+    UniversalPrint(value, os);
+  }
+};
+template <typename T, size_t N>
+class UniversalTersePrinter<T[N]> {
+ public:
+  static void Print(const T (&value)[N], ::std::ostream* os) {
+    UniversalPrinter<T[N]>::Print(value, os);
+  }
+};
+template <>
+class UniversalTersePrinter<const char*> {
+ public:
+  static void Print(const char* str, ::std::ostream* os) {
+    if (str == NULL) {
+      *os << "NULL";
+    } else {
+      UniversalPrint(string(str), os);
+    }
+  }
+};
+template <>
+class UniversalTersePrinter<char*> {
+ public:
+  static void Print(char* str, ::std::ostream* os) {
+    UniversalTersePrinter<const char*>::Print(str, os);
+  }
+};
+
+#if GTEST_HAS_STD_WSTRING
+template <>
+class UniversalTersePrinter<const wchar_t*> {
+ public:
+  static void Print(const wchar_t* str, ::std::ostream* os) {
+    if (str == NULL) {
+      *os << "NULL";
+    } else {
+      UniversalPrint(::std::wstring(str), os);
+    }
+  }
+};
+#endif
+
+template <>
+class UniversalTersePrinter<wchar_t*> {
+ public:
+  static void Print(wchar_t* str, ::std::ostream* os) {
+    UniversalTersePrinter<const wchar_t*>::Print(str, os);
+  }
+};
+
+template <typename T>
+void UniversalTersePrint(const T& value, ::std::ostream* os) {
+  UniversalTersePrinter<T>::Print(value, os);
 }
 
 // Prints a value using the type inferred by the compiler.  The
@@ -9879,7 +9945,10 @@
 // NUL-terminated string.
 template <typename T>
 void UniversalPrint(const T& value, ::std::ostream* os) {
-  UniversalPrinter<T>::Print(value, os);
+  // A workarond for the bug in VC++ 7.1 that prevents us from instantiating
+  // UniversalPrinter with T directly.
+  typedef T T1;
+  UniversalPrinter<T1>::Print(value, os);
 }
 
 #if GTEST_HAS_TR1_TUPLE
@@ -9972,7 +10041,7 @@
 template <typename T>
 ::std::string PrintToString(const T& value) {
   ::std::stringstream ss;
-  internal::UniversalTersePrint(value, &ss);
+  internal::UniversalTersePrinter<T>::Print(value, &ss);
   return ss.str();
 }
 
@@ -10328,10 +10397,6 @@
 
   TestMetaFactory() {}
 
-  virtual ~TestMetaFactory () {
-
-  }
-
   virtual TestFactoryBase* CreateTestFactory(ParamType parameter) {
     return new ParameterizedTestFactory<TestCase>(parameter);
   }
@@ -10432,10 +10497,10 @@
         const string& instantiation_name = gen_it->first;
         ParamGenerator<ParamType> generator((*gen_it->second)());
 
-        Message test_case_name_stream;
+        string test_case_name;
         if ( !instantiation_name.empty() )
-          test_case_name_stream << instantiation_name << "/";
-        test_case_name_stream << test_info->test_case_base_name;
+          test_case_name = instantiation_name + "/";
+        test_case_name += test_info->test_case_base_name;
 
         int i = 0;
         for (typename ParamGenerator<ParamType>::iterator param_it =
@@ -10444,7 +10509,7 @@
           Message test_name_stream;
           test_name_stream << test_info->test_base_name << "/" << i;
           MakeAndRegisterTestInfo(
-              test_case_name_stream.GetString().c_str(),
+              test_case_name.c_str(),
               test_name_stream.GetString().c_str(),
               NULL,  // No type parameter.
               PrintToString(*param_it).c_str(),
@@ -10650,7 +10715,7 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_)};
     return ValuesIn(array);
   }
 
@@ -10669,7 +10734,8 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_)};
     return ValuesIn(array);
   }
 
@@ -10690,7 +10756,8 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_)};
     return ValuesIn(array);
   }
 
@@ -10712,7 +10779,8 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_)};
     return ValuesIn(array);
   }
 
@@ -10736,7 +10804,9 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_)};
     return ValuesIn(array);
   }
 
@@ -10761,7 +10831,9 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_)};
     return ValuesIn(array);
   }
 
@@ -10788,7 +10860,9 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_)};
     return ValuesIn(array);
   }
 
@@ -10816,7 +10890,10 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_)};
     return ValuesIn(array);
   }
 
@@ -10845,7 +10922,10 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_)};
     return ValuesIn(array);
   }
 
@@ -10876,7 +10956,10 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_)};
     return ValuesIn(array);
   }
 
@@ -10908,8 +10991,11 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_)};
     return ValuesIn(array);
   }
 
@@ -10943,8 +11029,11 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_)};
     return ValuesIn(array);
   }
 
@@ -10979,8 +11068,11 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_)};
     return ValuesIn(array);
   }
 
@@ -11016,8 +11108,12 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_)};
     return ValuesIn(array);
   }
 
@@ -11056,8 +11152,12 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_)};
     return ValuesIn(array);
   }
 
@@ -11097,8 +11197,12 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_)};
     return ValuesIn(array);
   }
 
@@ -11139,8 +11243,13 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_)};
     return ValuesIn(array);
   }
 
@@ -11182,8 +11291,13 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_)};
     return ValuesIn(array);
   }
 
@@ -11227,8 +11341,13 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_)};
     return ValuesIn(array);
   }
 
@@ -11274,8 +11393,14 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_)};
     return ValuesIn(array);
   }
 
@@ -11322,8 +11447,14 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_)};
     return ValuesIn(array);
   }
 
@@ -11372,9 +11503,14 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_,
-        v23_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_)};
     return ValuesIn(array);
   }
 
@@ -11424,9 +11560,15 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_)};
     return ValuesIn(array);
   }
 
@@ -11477,9 +11619,15 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_)};
     return ValuesIn(array);
   }
 
@@ -11532,9 +11680,15 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_)};
     return ValuesIn(array);
   }
 
@@ -11589,9 +11743,16 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_)};
     return ValuesIn(array);
   }
 
@@ -11647,9 +11808,16 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_)};
     return ValuesIn(array);
   }
 
@@ -11706,9 +11874,16 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_)};
     return ValuesIn(array);
   }
 
@@ -11767,9 +11942,17 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_)};
     return ValuesIn(array);
   }
 
@@ -11830,9 +12013,17 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_)};
     return ValuesIn(array);
   }
 
@@ -11894,9 +12085,17 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_)};
     return ValuesIn(array);
   }
 
@@ -11960,9 +12159,18 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_)};
     return ValuesIn(array);
   }
 
@@ -12027,9 +12235,18 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_)};
     return ValuesIn(array);
   }
 
@@ -12095,10 +12312,18 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_,
-        v35_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_)};
     return ValuesIn(array);
   }
 
@@ -12166,10 +12391,19 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_)};
     return ValuesIn(array);
   }
 
@@ -12239,10 +12473,19 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_)};
     return ValuesIn(array);
   }
 
@@ -12313,10 +12556,19 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_)};
     return ValuesIn(array);
   }
 
@@ -12388,10 +12640,20 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_)};
     return ValuesIn(array);
   }
 
@@ -12465,10 +12727,20 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_)};
     return ValuesIn(array);
   }
 
@@ -12544,10 +12816,20 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_)};
     return ValuesIn(array);
   }
 
@@ -12624,10 +12906,21 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_)};
     return ValuesIn(array);
   }
 
@@ -12705,10 +12998,21 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_)};
     return ValuesIn(array);
   }
 
@@ -12788,10 +13092,21 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_)};
     return ValuesIn(array);
   }
 
@@ -12872,10 +13187,22 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_)};
     return ValuesIn(array);
   }
 
@@ -12958,10 +13285,22 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_, v46_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_), static_cast<T>(v46_)};
     return ValuesIn(array);
   }
 
@@ -13046,11 +13385,22 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_, v46_,
-        v47_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_), static_cast<T>(v46_), static_cast<T>(v47_)};
     return ValuesIn(array);
   }
 
@@ -13136,11 +13486,23 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_, v46_, v47_,
-        v48_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_), static_cast<T>(v46_), static_cast<T>(v47_),
+        static_cast<T>(v48_)};
     return ValuesIn(array);
   }
 
@@ -13227,11 +13589,23 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_, v46_, v47_,
-        v48_, v49_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_), static_cast<T>(v46_), static_cast<T>(v47_),
+        static_cast<T>(v48_), static_cast<T>(v49_)};
     return ValuesIn(array);
   }
 
@@ -13319,11 +13693,23 @@
 
   template <typename T>
   operator ParamGenerator<T>() const {
-    const T array[] = {v1_, v2_, v3_, v4_, v5_, v6_, v7_, v8_, v9_, v10_, v11_,
-        v12_, v13_, v14_, v15_, v16_, v17_, v18_, v19_, v20_, v21_, v22_, v23_,
-        v24_, v25_, v26_, v27_, v28_, v29_, v30_, v31_, v32_, v33_, v34_, v35_,
-        v36_, v37_, v38_, v39_, v40_, v41_, v42_, v43_, v44_, v45_, v46_, v47_,
-        v48_, v49_, v50_};
+    const T array[] = {static_cast<T>(v1_), static_cast<T>(v2_),
+        static_cast<T>(v3_), static_cast<T>(v4_), static_cast<T>(v5_),
+        static_cast<T>(v6_), static_cast<T>(v7_), static_cast<T>(v8_),
+        static_cast<T>(v9_), static_cast<T>(v10_), static_cast<T>(v11_),
+        static_cast<T>(v12_), static_cast<T>(v13_), static_cast<T>(v14_),
+        static_cast<T>(v15_), static_cast<T>(v16_), static_cast<T>(v17_),
+        static_cast<T>(v18_), static_cast<T>(v19_), static_cast<T>(v20_),
+        static_cast<T>(v21_), static_cast<T>(v22_), static_cast<T>(v23_),
+        static_cast<T>(v24_), static_cast<T>(v25_), static_cast<T>(v26_),
+        static_cast<T>(v27_), static_cast<T>(v28_), static_cast<T>(v29_),
+        static_cast<T>(v30_), static_cast<T>(v31_), static_cast<T>(v32_),
+        static_cast<T>(v33_), static_cast<T>(v34_), static_cast<T>(v35_),
+        static_cast<T>(v36_), static_cast<T>(v37_), static_cast<T>(v38_),
+        static_cast<T>(v39_), static_cast<T>(v40_), static_cast<T>(v41_),
+        static_cast<T>(v42_), static_cast<T>(v43_), static_cast<T>(v44_),
+        static_cast<T>(v45_), static_cast<T>(v46_), static_cast<T>(v47_),
+        static_cast<T>(v48_), static_cast<T>(v49_), static_cast<T>(v50_)};
     return ValuesIn(array);
   }
 
@@ -16441,7 +16827,7 @@
 // Boolean flags:
 //
 // class FlagDependentTest
-//     : public testing::TestWithParam<tuple(bool, bool)> > {
+//     : public testing::TestWithParam<tuple<bool, bool> > {
 //   virtual void SetUp() {
 //     // Assigns external_flag_1 and external_flag_2 values from the tuple.
 //     tie(external_flag_1, external_flag_2) = GetParam();
@@ -16723,7 +17109,7 @@
                  int a_line_number,
                  const char* a_message)
       : type_(a_type),
-        file_name_(a_file_name),
+        file_name_(a_file_name == NULL ? "" : a_file_name),
         line_number_(a_line_number),
         summary_(ExtractSummary(a_message)),
         message_(a_message) {
@@ -16734,7 +17120,9 @@
 
   // Gets the name of the source file where the test part took place, or
   // NULL if it's unknown.
-  const char* file_name() const { return file_name_.c_str(); }
+  const char* file_name() const {
+    return file_name_.empty() ? NULL : file_name_.c_str();
+  }
 
   // Gets the line in the source file where the test part took place,
   // or -1 if it's unknown.
@@ -16757,21 +17145,22 @@
 
   // Returns true iff the test part fatally failed.
   bool fatally_failed() const { return type_ == kFatalFailure; }
+
  private:
   Type type_;
 
   // Gets the summary of the failure message by omitting the stack
   // trace in it.
-  static internal::String ExtractSummary(const char* message);
+  static std::string ExtractSummary(const char* message);
 
   // The name of the source file where the test part took place, or
-  // NULL if the source file is unknown.
-  internal::String file_name_;
+  // "" if the source file is unknown.
+  std::string file_name_;
   // The line in the source file where the test part took place, or -1
   // if the line number is unknown.
   int line_number_;
-  internal::String summary_;  // The test failure summary.
-  internal::String message_;  // The test failure message.
+  std::string summary_;  // The test failure summary.
+  std::string message_;  // The test failure message.
 };
 
 // Prints a TestPartResult object.
@@ -17182,25 +17571,15 @@
 class NoExecDeathTest;
 class FinalSuccessChecker;
 class GTestFlagSaver;
+class StreamingListenerTest;
 class TestResultAccessor;
 class TestEventListenersAccessor;
 class TestEventRepeater;
+class UnitTestRecordPropertyTestHelper;
 class WindowsDeathTest;
 class UnitTestImpl* GetUnitTestImpl();
 void ReportFailureInUnknownLocation(TestPartResult::Type result_type,
-                                    const String& message);
-
-// Converts a streamable value to a String.  A NULL pointer is
-// converted to "(null)".  When the input value is a ::string,
-// ::std::string, ::wstring, or ::std::wstring object, each NUL
-// character in it is replaced with "\\0".
-// Declared in gtest-internal.h but defined here, so that it has access
-// to the definition of the Message class, required by the ARM
-// compiler.
-template <typename T>
-String StreamableToString(const T& streamable) {
-  return (Message() << streamable).GetString();
-}
+                                    const std::string& message);
 
 }  // namespace internal
 
@@ -17420,20 +17799,21 @@
   // non-fatal) failure.
   static bool HasFailure() { return HasFatalFailure() || HasNonfatalFailure(); }
 
-  // Logs a property for the current test.  Only the last value for a given
-  // key is remembered.
-  // These are public static so they can be called from utility functions
-  // that are not members of the test fixture.
-  // The arguments are const char* instead strings, as Google Test is used
-  // on platforms where string doesn't compile.
-  //
-  // Note that a driving consideration for these RecordProperty methods
-  // was to produce xml output suited to the Greenspan charting utility,
-  // which at present will only chart values that fit in a 32-bit int. It
-  // is the user's responsibility to restrict their values to 32-bit ints
-  // if they intend them to be used with Greenspan.
-  static void RecordProperty(const char* key, const char* value);
-  static void RecordProperty(const char* key, int value);
+  // Logs a property for the current test, test case, or for the entire
+  // invocation of the test program when used outside of the context of a
+  // test case.  Only the last value for a given key is remembered.  These
+  // are public static so they can be called from utility functions that are
+  // not members of the test fixture.  Calls to RecordProperty made during
+  // lifespan of the test (from the moment its constructor starts to the
+  // moment its destructor finishes) will be output in XML as attributes of
+  // the <testcase> element.  Properties recorded from fixture's
+  // SetUpTestCase or TearDownTestCase are logged as attributes of the
+  // corresponding <testsuite> element.  Calls to RecordProperty made in the
+  // global context (before or after invocation of RUN_ALL_TESTS and from
+  // SetUp/TearDown method of Environment objects registered with Google
+  // Test) will be output as attributes of the <testsuites> element.
+  static void RecordProperty(const std::string& key, const std::string& value);
+  static void RecordProperty(const std::string& key, int value);
 
  protected:
   // Creates a Test object.
@@ -17502,7 +17882,7 @@
   // C'tor.  TestProperty does NOT have a default constructor.
   // Always use this constructor (with parameters) to create a
   // TestProperty object.
-  TestProperty(const char* a_key, const char* a_value) :
+  TestProperty(const std::string& a_key, const std::string& a_value) :
     key_(a_key), value_(a_value) {
   }
 
@@ -17517,15 +17897,15 @@
   }
 
   // Sets a new value, overriding the one supplied in the constructor.
-  void SetValue(const char* new_value) {
+  void SetValue(const std::string& new_value) {
     value_ = new_value;
   }
 
  private:
   // The key supplied by the user.
-  internal::String key_;
+  std::string key_;
   // The value supplied by the user.
-  internal::String value_;
+  std::string value_;
 };
 
 // The result of a single Test.  This includes a list of
@@ -17576,6 +17956,7 @@
 
  private:
   friend class TestInfo;
+  friend class TestCase;
   friend class UnitTest;
   friend class internal::DefaultGlobalTestPartResultReporter;
   friend class internal::ExecDeathTest;
@@ -17600,13 +17981,16 @@
   // a non-fatal failure if invalid (e.g., if it conflicts with reserved
   // key names). If a property is already recorded for the same key, the
   // value will be updated, rather than storing multiple values for the same
-  // key.
-  void RecordProperty(const TestProperty& test_property);
+  // key.  xml_element specifies the element for which the property is being
+  // recorded and is used for validation.
+  void RecordProperty(const std::string& xml_element,
+                      const TestProperty& test_property);
 
   // Adds a failure if the key is a reserved attribute of Google Test
   // testcase tags.  Returns true if the property is valid.
   // TODO(russr): Validate attribute names are legal and human readable.
-  static bool ValidateTestProperty(const TestProperty& test_property);
+  static bool ValidateTestProperty(const std::string& xml_element,
+                                   const TestProperty& test_property);
 
   // Adds a test part result to the list.
   void AddTestPartResult(const TestPartResult& test_part_result);
@@ -17679,9 +18063,9 @@
     return NULL;
   }
 
-  // Returns true if this test should run, that is if the test is not disabled
-  // (or it is disabled but the also_run_disabled_tests flag has been specified)
-  // and its full name matches the user-specified filter.
+  // Returns true if this test should run, that is if the test is not
+  // disabled (or it is disabled but the also_run_disabled_tests flag has
+  // been specified) and its full name matches the user-specified filter.
   //
   // Google Test allows the user to filter the tests by their full names.
   // The full name of a test Bar in test case Foo is defined as
@@ -17697,19 +18081,28 @@
   // contains the character 'A' or starts with "Foo.".
   bool should_run() const { return should_run_; }
 
+  // Returns true iff this test will appear in the XML report.
+  bool is_reportable() const {
+    // For now, the XML report includes all tests matching the filter.
+    // In the future, we may trim tests that are excluded because of
+    // sharding.
+    return matches_filter_;
+  }
+
   // Returns the result of the test.
   const TestResult* result() const { return &result_; }
 
  private:
-
 #if GTEST_HAS_DEATH_TEST
   friend class internal::DefaultDeathTestFactory;
 #endif  // GTEST_HAS_DEATH_TEST
   friend class Test;
   friend class TestCase;
   friend class internal::UnitTestImpl;
+  friend class internal::StreamingListenerTest;
   friend TestInfo* internal::MakeAndRegisterTestInfo(
-      const char* test_case_name, const char* name,
+      const char* test_case_name,
+      const char* name,
       const char* type_param,
       const char* value_param,
       internal::TypeId fixture_class_id,
@@ -17719,9 +18112,10 @@
 
   // Constructs a TestInfo object. The newly constructed instance assumes
   // ownership of the factory object.
-  TestInfo(const char* test_case_name, const char* name,
-           const char* a_type_param,
-           const char* a_value_param,
+  TestInfo(const std::string& test_case_name,
+           const std::string& name,
+           const char* a_type_param,   // NULL if not a type-parameterized test
+           const char* a_value_param,  // NULL if not a value-parameterized test
            internal::TypeId fixture_class_id,
            internal::TestFactoryBase* factory);
 
@@ -17807,9 +18201,15 @@
   // Gets the number of failed tests in this test case.
   int failed_test_count() const;
 
+  // Gets the number of disabled tests that will be reported in the XML report.
+  int reportable_disabled_test_count() const;
+
   // Gets the number of disabled tests in this test case.
   int disabled_test_count() const;
 
+  // Gets the number of tests to be printed in the XML report.
+  int reportable_test_count() const;
+
   // Get the number of tests in this test case that should run.
   int test_to_run_count() const;
 
@@ -17829,6 +18229,10 @@
   // total_test_count() - 1. If i is not in that range, returns NULL.
   const TestInfo* GetTestInfo(int i) const;
 
+  // Returns the TestResult that holds test properties recorded during
+  // execution of SetUpTestCase and TearDownTestCase.
+  const TestResult& ad_hoc_test_result() const { return ad_hoc_test_result_; }
+
  private:
   friend class Test;
   friend class internal::UnitTestImpl;
@@ -17881,11 +18285,22 @@
     return test_info->should_run() && test_info->result()->Failed();
   }
 
+  // Returns true iff the test is disabled and will be reported in the XML
+  // report.
+  static bool TestReportableDisabled(const TestInfo* test_info) {
+    return test_info->is_reportable() && test_info->is_disabled_;
+  }
+
   // Returns true iff test is disabled.
   static bool TestDisabled(const TestInfo* test_info) {
     return test_info->is_disabled_;
   }
 
+  // Returns true iff this test will appear in the XML report.
+  static bool TestReportable(const TestInfo* test_info) {
+    return test_info->is_reportable();
+  }
+
   // Returns true if the given test should run.
   static bool ShouldRunTest(const TestInfo* test_info) {
     return test_info->should_run();
@@ -17898,7 +18313,7 @@
   void UnshuffleTests();
 
   // Name of the test case.
-  internal::String name_;
+  std::string name_;
   // Name of the parameter type, or NULL if this is not a typed or a
   // type-parameterized test.
   const internal::scoped_ptr<const ::std::string> type_param_;
@@ -17917,6 +18332,9 @@
   bool should_run_;
   // Elapsed time, in milliseconds.
   TimeInMillis elapsed_time_;
+  // Holds test properties recorded during execution of SetUpTestCase and
+  // TearDownTestCase.
+  TestResult ad_hoc_test_result_;
 
   // We disallow copying TestCases.
   GTEST_DISALLOW_COPY_AND_ASSIGN_(TestCase);
@@ -18136,11 +18554,13 @@
 
   // Returns the TestCase object for the test that's currently running,
   // or NULL if no test is running.
-  const TestCase* current_test_case() const;
+  const TestCase* current_test_case() const
+      GTEST_LOCK_EXCLUDED_(mutex_);
 
   // Returns the TestInfo object for the test that's currently running,
   // or NULL if no test is running.
-  const TestInfo* current_test_info() const;
+  const TestInfo* current_test_info() const
+      GTEST_LOCK_EXCLUDED_(mutex_);
 
   // Returns the random seed used at the start of the current test run.
   int random_seed() const;
@@ -18150,7 +18570,8 @@
   // value-parameterized tests and instantiate and register them.
   //
   // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
-  internal::ParameterizedTestCaseRegistry& parameterized_test_registry();
+  internal::ParameterizedTestCaseRegistry& parameterized_test_registry()
+      GTEST_LOCK_EXCLUDED_(mutex_);
 #endif  // GTEST_HAS_PARAM_TEST
 
   // Gets the number of successful test cases.
@@ -18172,15 +18593,25 @@
   // Gets the number of failed tests.
   int failed_test_count() const;
 
+  // Gets the number of disabled tests that will be reported in the XML report.
+  int reportable_disabled_test_count() const;
+
   // Gets the number of disabled tests.
   int disabled_test_count() const;
 
+  // Gets the number of tests to be printed in the XML report.
+  int reportable_test_count() const;
+
   // Gets the number of all tests.
   int total_test_count() const;
 
   // Gets the number of tests that should run.
   int test_to_run_count() const;
 
+  // Gets the time of the test program start, in ms from the start of the
+  // UNIX epoch.
+  TimeInMillis start_timestamp() const;
+
   // Gets the elapsed time, in milliseconds.
   TimeInMillis elapsed_time() const;
 
@@ -18195,6 +18626,10 @@
   // total_test_case_count() - 1. If i is not in that range, returns NULL.
   const TestCase* GetTestCase(int i) const;
 
+  // Returns the TestResult containing information on test failures and
+  // properties logged outside of individual test cases.
+  const TestResult& ad_hoc_test_result() const;
+
   // Returns the list of event listeners that can be used to track events
   // inside Google Test.
   TestEventListeners& listeners();
@@ -18218,12 +18653,16 @@
   void AddTestPartResult(TestPartResult::Type result_type,
                          const char* file_name,
                          int line_number,
-                         const internal::String& message,
-                         const internal::String& os_stack_trace);
+                         const std::string& message,
+                         const std::string& os_stack_trace)
+      GTEST_LOCK_EXCLUDED_(mutex_);
 
-  // Adds a TestProperty to the current TestResult object. If the result already
-  // contains a property with the same key, the value will be updated.
-  void RecordPropertyForCurrentTest(const char* key, const char* value);
+  // Adds a TestProperty to the current TestResult object when invoked from
+  // inside a test, to current TestCase's ad_hoc_test_result_ when invoked
+  // from SetUpTestCase or TearDownTestCase, or to the global property set
+  // when invoked elsewhere.  If the result already contains a property with
+  // the same key, the value will be updated.
+  void RecordProperty(const std::string& key, const std::string& value);
 
   // Gets the i-th test case among all the test cases. i can range from 0 to
   // total_test_case_count() - 1. If i is not in that range, returns NULL.
@@ -18238,11 +18677,13 @@
   friend class Test;
   friend class internal::AssertHelper;
   friend class internal::ScopedTrace;
+  friend class internal::StreamingListenerTest;
+  friend class internal::UnitTestRecordPropertyTestHelper;
   friend Environment* AddGlobalTestEnvironment(Environment* env);
   friend internal::UnitTestImpl* internal::GetUnitTestImpl();
   friend void internal::ReportFailureInUnknownLocation(
       TestPartResult::Type result_type,
-      const internal::String& message);
+      const std::string& message);
 
   // Creates an empty UnitTest.
   UnitTest();
@@ -18252,10 +18693,12 @@
 
   // Pushes a trace defined by SCOPED_TRACE() on to the per-thread
   // Google Test trace stack.
-  void PushGTestTrace(const internal::TraceInfo& trace);
+  void PushGTestTrace(const internal::TraceInfo& trace)
+      GTEST_LOCK_EXCLUDED_(mutex_);
 
   // Pops a trace from the per-thread Google Test trace stack.
-  void PopGTestTrace();
+  void PopGTestTrace()
+      GTEST_LOCK_EXCLUDED_(mutex_);
 
   // Protects mutable state in *impl_.  This is mutable as some const
   // methods need to lock it too.
@@ -18310,24 +18753,101 @@
 
 namespace internal {
 
+// FormatForComparison<ToPrint, OtherOperand>::Format(value) formats a
+// value of type ToPrint that is an operand of a comparison assertion
+// (e.g. ASSERT_EQ).  OtherOperand is the type of the other operand in
+// the comparison, and is used to help determine the best way to
+// format the value.  In particular, when the value is a C string
+// (char pointer) and the other operand is an STL string object, we
+// want to format the C string as a string, since we know it is
+// compared by value with the string object.  If the value is a char
+// pointer but the other operand is not an STL string object, we don't
+// know whether the pointer is supposed to point to a NUL-terminated
+// string, and thus want to print it as a pointer to be safe.
+//
+// INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
+
+// The default case.
+template <typename ToPrint, typename OtherOperand>
+class FormatForComparison {
+ public:
+  static ::std::string Format(const ToPrint& value) {
+    return ::testing::PrintToString(value);
+  }
+};
+
+// Array.
+template <typename ToPrint, size_t N, typename OtherOperand>
+class FormatForComparison<ToPrint[N], OtherOperand> {
+ public:
+  static ::std::string Format(const ToPrint* value) {
+    return FormatForComparison<const ToPrint*, OtherOperand>::Format(value);
+  }
+};
+
+// By default, print C string as pointers to be safe, as we don't know
+// whether they actually point to a NUL-terminated string.
+
+#define GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(CharType)                \
+  template <typename OtherOperand>                                      \
+  class FormatForComparison<CharType*, OtherOperand> {                  \
+   public:                                                              \
+    static ::std::string Format(CharType* value) {                      \
+      return ::testing::PrintToString(static_cast<const void*>(value)); \
+    }                                                                   \
+  }
+
+GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(char);
+GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(const char);
+GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(wchar_t);
+GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_(const wchar_t);
+
+#undef GTEST_IMPL_FORMAT_C_STRING_AS_POINTER_
+
+// If a C string is compared with an STL string object, we know it's meant
+// to point to a NUL-terminated string, and thus can print it as a string.
+
+#define GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(CharType, OtherStringType) \
+  template <>                                                           \
+  class FormatForComparison<CharType*, OtherStringType> {               \
+   public:                                                              \
+    static ::std::string Format(CharType* value) {                      \
+      return ::testing::PrintToString(value);                           \
+    }                                                                   \
+  }
+
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(char, ::std::string);
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const char, ::std::string);
+
+#if GTEST_HAS_GLOBAL_STRING
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(char, ::string);
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const char, ::string);
+#endif
+
+#if GTEST_HAS_GLOBAL_WSTRING
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(wchar_t, ::wstring);
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const wchar_t, ::wstring);
+#endif
+
+#if GTEST_HAS_STD_WSTRING
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(wchar_t, ::std::wstring);
+GTEST_IMPL_FORMAT_C_STRING_AS_STRING_(const wchar_t, ::std::wstring);
+#endif
+
+#undef GTEST_IMPL_FORMAT_C_STRING_AS_STRING_
+
 // Formats a comparison assertion (e.g. ASSERT_EQ, EXPECT_LT, and etc)
 // operand to be used in a failure message.  The type (but not value)
 // of the other operand may affect the format.  This allows us to
 // print a char* as a raw pointer when it is compared against another
-// char*, and print it as a C string when it is compared against an
-// std::string object, for example.
-//
-// The default implementation ignores the type of the other operand.
-// Some specialized versions are used to handle formatting wide or
-// narrow C strings.
+// char* or void*, and print it as a C string when it is compared
+// against an std::string object, for example.
 //
 // INTERNAL IMPLEMENTATION - DO NOT USE IN A USER PROGRAM.
 template <typename T1, typename T2>
-String FormatForComparisonFailureMessage(const T1& value,
-                                         const T2& /* other_operand */) {
-  // C++Builder compiles this incorrectly if the namespace isn't explicitly
-  // given.
-  return ::testing::PrintToString(value);
+std::string FormatForComparisonFailureMessage(
+    const T1& value, const T2& /* other_operand */) {
+  return FormatForComparison<T1, T2>::Format(value);
 }
 
 // The helper function for {ASSERT|EXPECT}_EQ.
@@ -18339,7 +18859,7 @@
 #ifdef _MSC_VER
 # pragma warning(push)          // Saves the current warning state.
 # pragma warning(disable:4389)  // Temporarily disables warning on
-                               // signed/unsigned mismatch.
+                                // signed/unsigned mismatch.
 #endif
 
   if (expected == actual) {
@@ -18475,11 +18995,11 @@
 // Implements the helper function for {ASSERT|EXPECT}_LE
 GTEST_IMPL_CMP_HELPER_(LE, <=);
 // Implements the helper function for {ASSERT|EXPECT}_LT
-GTEST_IMPL_CMP_HELPER_(LT, < );
+GTEST_IMPL_CMP_HELPER_(LT, <);
 // Implements the helper function for {ASSERT|EXPECT}_GE
 GTEST_IMPL_CMP_HELPER_(GE, >=);
 // Implements the helper function for {ASSERT|EXPECT}_GT
-GTEST_IMPL_CMP_HELPER_(GT, > );
+GTEST_IMPL_CMP_HELPER_(GT, >);
 
 #undef GTEST_IMPL_CMP_HELPER_
 
@@ -18643,9 +19163,9 @@
         : type(t), file(srcfile), line(line_num), message(msg) { }
 
     TestPartResult::Type const type;
-    const char*        const file;
-    int                const line;
-    String             const message;
+    const char* const file;
+    int const line;
+    std::string const message;
 
    private:
     GTEST_DISALLOW_COPY_AND_ASSIGN_(AssertHelperData);
@@ -18704,7 +19224,12 @@
   // references static data, to reduce the opportunity for incorrect uses
   // like writing 'WithParamInterface<bool>::GetParam()' for a test that
   // uses a fixture whose parameter type is int.
-  const ParamType& GetParam() const { return *parameter_; }
+  const ParamType& GetParam() const {
+    GTEST_CHECK_(parameter_ != NULL)
+        << "GetParam() can only be called inside a value-parameterized test "
+        << "-- did you intend to write TEST_P instead of TEST_F?";
+    return *parameter_;
+  }
 
  private:
   // Sets parameter value. The caller is responsible for making sure the value
@@ -18750,12 +19275,6 @@
 // usually want the fail-fast behavior of FAIL and ASSERT_*, but those
 // writing data-driven tests often find themselves using ADD_FAILURE
 // and EXPECT_* more.
-//
-// Examples:
-//
-//   EXPECT_TRUE(server.StatusIsOK());
-//   ASSERT_FALSE(server.HasPendingRequest(port))
-//       << "There are still pending requests " << "on port " << port;
 
 // Generates a nonfatal failure with a generic message.
 #define ADD_FAILURE() GTEST_NONFATAL_FAILURE_("Failed")
@@ -18853,7 +19372,7 @@
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
-// This file is AUTOMATICALLY GENERATED on 09/24/2010 by command
+// This file is AUTOMATICALLY GENERATED on 10/31/2011 by command
 // 'gen_gtest_pred_impl.py 5'.  DO NOT EDIT BY HAND!
 //
 // Implements a family of generic predicate assertion macros.
@@ -18924,7 +19443,7 @@
 // Internal macro for implementing {EXPECT|ASSERT}_PRED_FORMAT1.
 // Don't use this in your code.
 #define GTEST_PRED_FORMAT1_(pred_format, v1, on_failure)\
-  GTEST_ASSERT_(pred_format(#v1, v1),\
+  GTEST_ASSERT_(pred_format(#v1, v1), \
                 on_failure)
 
 // Internal macro for implementing {EXPECT|ASSERT}_PRED1.  Don't use
@@ -18970,7 +19489,7 @@
 // Internal macro for implementing {EXPECT|ASSERT}_PRED_FORMAT2.
 // Don't use this in your code.
 #define GTEST_PRED_FORMAT2_(pred_format, v1, v2, on_failure)\
-  GTEST_ASSERT_(pred_format(#v1, #v2, v1, v2),\
+  GTEST_ASSERT_(pred_format(#v1, #v2, v1, v2), \
                 on_failure)
 
 // Internal macro for implementing {EXPECT|ASSERT}_PRED2.  Don't use
@@ -19023,7 +19542,7 @@
 // Internal macro for implementing {EXPECT|ASSERT}_PRED_FORMAT3.
 // Don't use this in your code.
 #define GTEST_PRED_FORMAT3_(pred_format, v1, v2, v3, on_failure)\
-  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, v1, v2, v3),\
+  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, v1, v2, v3), \
                 on_failure)
 
 // Internal macro for implementing {EXPECT|ASSERT}_PRED3.  Don't use
@@ -19083,7 +19602,7 @@
 // Internal macro for implementing {EXPECT|ASSERT}_PRED_FORMAT4.
 // Don't use this in your code.
 #define GTEST_PRED_FORMAT4_(pred_format, v1, v2, v3, v4, on_failure)\
-  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, #v4, v1, v2, v3, v4),\
+  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, #v4, v1, v2, v3, v4), \
                 on_failure)
 
 // Internal macro for implementing {EXPECT|ASSERT}_PRED4.  Don't use
@@ -19150,7 +19669,7 @@
 // Internal macro for implementing {EXPECT|ASSERT}_PRED_FORMAT5.
 // Don't use this in your code.
 #define GTEST_PRED_FORMAT5_(pred_format, v1, v2, v3, v4, v5, on_failure)\
-  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, #v4, #v5, v1, v2, v3, v4, v5),\
+  GTEST_ASSERT_(pred_format(#v1, #v2, #v3, #v4, #v5, v1, v2, v3, v4, v5), \
                 on_failure)
 
 // Internal macro for implementing {EXPECT|ASSERT}_PRED5.  Don't use
@@ -19286,7 +19805,7 @@
 # define ASSERT_GT(val1, val2) GTEST_ASSERT_GT(val1, val2)
 #endif
 
-// C String Comparisons.  All tests treat NULL and any non-NULL string
+// C-string Comparisons.  All tests treat NULL and any non-NULL string
 // as different.  Two NULLs are equal.
 //
 //    * {ASSERT|EXPECT}_STREQ(s1, s2):     Tests that s1 == s2
@@ -19527,15 +20046,20 @@
   GTEST_TEST_(test_fixture, test_name, test_fixture, \
               ::testing::internal::GetTypeId<test_fixture>())
 
-// Use this macro in main() to run all tests.  It returns 0 if all
+}  // namespace testing
+
+// Use this function in main() to run all tests.  It returns 0 if all
 // tests are successful, or 1 otherwise.
 //
 // RUN_ALL_TESTS() should be invoked after the command line has been
 // parsed by InitGoogleTest().
+//
+// This function was formerly a macro; thus, it is in the global
+// namespace and has an all-caps name.
+int RUN_ALL_TESTS() GTEST_MUST_USE_RESULT_;
 
-#define RUN_ALL_TESTS()\
-  (::testing::UnitTest::GetInstance()->Run())
+inline int RUN_ALL_TESTS() {
+  return ::testing::UnitTest::GetInstance()->Run();
+}
 
-}  // namespace testing
 
-#endif  // GTEST_INCLUDE_GTEST_GTEST_H_
