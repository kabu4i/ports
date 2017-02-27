--- ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/gtest-all.cc.orig	2014-10-12 00:56:38.000000000 +0400
+++ ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/gtest-all.cc	2014-10-27 13:32:15.125002069 +0300
@@ -296,7 +296,7 @@
         (substr));\
     {\
       ::testing::ScopedFakeTestPartResultReporter gtest_reporter(\
-          ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ALL_THREADS,\
+          ::testing::ScopedFakeTestPartResultReporter::INTERCEPT_ALL_THREADS, \
           &gtest_failures);\
       if (::testing::internal::AlwaysTrue()) { statement; }\
     }\
@@ -309,10 +309,13 @@
 #include <stdarg.h>
 #include <stdio.h>
 #include <stdlib.h>
+#include <time.h>
 #include <wchar.h>
 #include <wctype.h>
 
 #include <algorithm>
+#include <iomanip>
+#include <limits>
 #include <ostream>  // NOLINT
 #include <sstream>
 #include <vector>
@@ -458,6 +461,11 @@
 #include <vector>
 
 
+#if GTEST_CAN_STREAM_RESULTS_
+# include <arpa/inet.h>  // NOLINT
+# include <netdb.h>  // NOLINT
+#endif
+
 #if GTEST_OS_WINDOWS
 # include <windows.h>  // NOLINT
 #endif  // GTEST_OS_WINDOWS
@@ -510,6 +518,12 @@
 // Formats the given time in milliseconds as seconds.
 GTEST_API_ std::string FormatTimeInMillisAsSeconds(TimeInMillis ms);
 
+// Converts the given time in milliseconds to a date string in the ISO 8601
+// format, without the timezone information.  N.B.: due to the use the
+// non-reentrant localtime() function, this function is not thread safe.  Do
+// not use it in any code that can be called from multiple threads.
+GTEST_API_ std::string FormatEpochTimeInMillisAsIso8601(TimeInMillis ms);
+
 // Parses a string for an Int32 flag, in the form of "--flag=value".
 //
 // On success, stores the value of the flag in *value, and returns
@@ -588,37 +602,35 @@
     GTEST_FLAG(stream_result_to) = stream_result_to_;
     GTEST_FLAG(throw_on_failure) = throw_on_failure_;
   }
+
  private:
   // Fields for saving the original values of flags.
   bool also_run_disabled_tests_;
   bool break_on_failure_;
   bool catch_exceptions_;
-  String color_;
-  String death_test_style_;
+  std::string color_;
+  std::string death_test_style_;
   bool death_test_use_fork_;
-  String filter_;
-  String internal_run_death_test_;
+  std::string filter_;
+  std::string internal_run_death_test_;
   bool list_tests_;
-  String output_;
+  std::string output_;
   bool print_time_;
-  bool pretty_;
   internal::Int32 random_seed_;
   internal::Int32 repeat_;
   bool shuffle_;
   internal::Int32 stack_trace_depth_;
-  String stream_result_to_;
+  std::string stream_result_to_;
   bool throw_on_failure_;
 } GTEST_ATTRIBUTE_UNUSED_;
 
 // Converts a Unicode code point to a narrow string in UTF-8 encoding.
 // code_point parameter is of type UInt32 because wchar_t may not be
 // wide enough to contain a code point.
-// The output buffer str must containt at least 32 characters.
-// The function returns the address of the output buffer.
 // If the code_point is not a valid Unicode code point
-// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be output
-// as '(Invalid Unicode 0xXXXXXXXX)'.
-GTEST_API_ char* CodePointToUtf8(UInt32 code_point, char* str);
+// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be converted
+// to "(Invalid Unicode 0xXXXXXXXX)".
+GTEST_API_ std::string CodePointToUtf8(UInt32 code_point);
 
 // Converts a wide string to a narrow string in UTF-8 encoding.
 // The wide string is assumed to have the following encoding:
@@ -633,7 +645,7 @@
 // as '(Invalid Unicode 0xXXXXXXXX)'. If the string is in UTF16 encoding
 // and contains invalid UTF-16 surrogate pairs, values in those pairs
 // will be encoded as individual Unicode characters from Basic Normal Plane.
-GTEST_API_ String WideStringToUtf8(const wchar_t* str, int num_chars);
+GTEST_API_ std::string WideStringToUtf8(const wchar_t* str, int num_chars);
 
 // Reads the GTEST_SHARD_STATUS_FILE environment variable, and creates the file
 // if the variable is present. If a file already exists at this location, this
@@ -737,16 +749,15 @@
   // Constructor.
   //
   // TestPropertyKeyIs has NO default constructor.
-  explicit TestPropertyKeyIs(const char* key)
-      : key_(key) {}
+  explicit TestPropertyKeyIs(const std::string& key) : key_(key) {}
 
   // Returns true iff the test name of test property matches on key_.
   bool operator()(const TestProperty& test_property) const {
-    return String(test_property.key()).Compare(key_) == 0;
+    return test_property.key() == key_;
   }
 
  private:
-  String key_;
+  std::string key_;
 };
 
 // Class UnitTestOptions.
@@ -764,12 +775,12 @@
   // Functions for processing the gtest_output flag.
 
   // Returns the output format, or "" for normal printed output.
-  static String GetOutputFormat();
+  static std::string GetOutputFormat();
 
   // Returns the absolute path of the requested output file, or the
   // default (test_detail.xml in the original working directory) if
   // none was explicitly specified.
-  static String GetAbsolutePathToOutputFile();
+  static std::string GetAbsolutePathToOutputFile();
 
   // Functions for processing the gtest_filter flag.
 
@@ -782,8 +793,8 @@
 
   // Returns true iff the user-specified filter matches the test case
   // name and the test name.
-  static bool FilterMatchesTest(const String &test_case_name,
-                                const String &test_name);
+  static bool FilterMatchesTest(const std::string &test_case_name,
+                                const std::string &test_name);
 
 #if GTEST_OS_WINDOWS
   // Function for supporting the gtest_catch_exception flag.
@@ -796,7 +807,7 @@
 
   // Returns true if "name" matches the ':' separated list of glob-style
   // filters in "filter".
-  static bool MatchesFilter(const String& name, const char* filter);
+  static bool MatchesFilter(const std::string& name, const char* filter);
 };
 
 // Returns the current application's name, removing directory path if that
@@ -809,13 +820,13 @@
   OsStackTraceGetterInterface() {}
   virtual ~OsStackTraceGetterInterface() {}
 
-  // Returns the current OS stack trace as a String.  Parameters:
+  // Returns the current OS stack trace as an std::string.  Parameters:
   //
   //   max_depth  - the maximum number of stack frames to be included
   //                in the trace.
   //   skip_count - the number of top frames to be skipped; doesn't count
   //                against max_depth.
-  virtual String CurrentStackTrace(int max_depth, int skip_count) = 0;
+  virtual string CurrentStackTrace(int max_depth, int skip_count) = 0;
 
   // UponLeavingGTest() should be called immediately before Google Test calls
   // user code. It saves some information about the current stack that
@@ -830,8 +841,11 @@
 class OsStackTraceGetter : public OsStackTraceGetterInterface {
  public:
   OsStackTraceGetter() : caller_frame_(NULL) {}
-  virtual String CurrentStackTrace(int max_depth, int skip_count);
-  virtual void UponLeavingGTest();
+
+  virtual string CurrentStackTrace(int max_depth, int skip_count)
+      GTEST_LOCK_EXCLUDED_(mutex_);
+
+  virtual void UponLeavingGTest() GTEST_LOCK_EXCLUDED_(mutex_);
 
   // This string is inserted in place of stack frames that are part of
   // Google Test's implementation.
@@ -853,7 +867,7 @@
 struct TraceInfo {
   const char* file;
   int line;
-  String message;
+  std::string message;
 };
 
 // This is the default global test part result reporter used in UnitTestImpl.
@@ -937,15 +951,25 @@
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
+  TimeInMillis start_timestamp() const { return start_timestamp_; }
+
   // Gets the elapsed time, in milliseconds.
   TimeInMillis elapsed_time() const { return elapsed_time_; }
 
@@ -994,7 +1018,7 @@
   // getter, and returns it.
   OsStackTraceGetterInterface* os_stack_trace_getter();
 
-  // Returns the current OS stack trace as a String.
+  // Returns the current OS stack trace as an std::string.
   //
   // The maximum number of stack frames to be included is specified by
   // the gtest_stack_trace_depth flag.  The skip_count parameter
@@ -1004,7 +1028,7 @@
   // For example, if Foo() calls Bar(), which in turn calls
   // CurrentOsStackTraceExceptTop(1), Foo() will be included in the
   // trace but Bar() and CurrentOsStackTraceExceptTop() won't.
-  String CurrentOsStackTraceExceptTop(int skip_count);
+  std::string CurrentOsStackTraceExceptTop(int skip_count) GTEST_NO_INLINE_;
 
   // Finds and returns a TestCase with the given name.  If one doesn't
   // exist, creates one and returns it.
@@ -1094,6 +1118,12 @@
     ad_hoc_test_result_.Clear();
   }
 
+  // Adds a TestProperty to the current TestResult object when invoked in a
+  // context of a test or a test case, or to the global property set. If the
+  // result already contains a property with the same key, the value will be
+  // updated.
+  void RecordProperty(const TestProperty& test_property);
+
   enum ReactionToSharding {
     HONOR_SHARDING_PROTOCOL,
     IGNORE_SHARDING_PROTOCOL
@@ -1278,6 +1308,10 @@
   // Our random number generator.
   internal::Random random_;
 
+  // The time of the test program start, in ms from the start of the
+  // UNIX epoch.
+  TimeInMillis start_timestamp_;
+
   // How long the test took to run, in milliseconds.
   TimeInMillis elapsed_time_;
 
@@ -1333,7 +1367,7 @@
 
 // Returns the message describing the last system error, regardless of the
 // platform.
-GTEST_API_ String GetLastErrnoDescription();
+GTEST_API_ std::string GetLastErrnoDescription();
 
 # if GTEST_OS_WINDOWS
 // Provides leak-safe Windows kernel handle ownership.
@@ -1416,8 +1450,9 @@
 class TestResultAccessor {
  public:
   static void RecordProperty(TestResult* test_result,
+                             const std::string& xml_element,
                              const TestProperty& property) {
-    test_result->RecordProperty(property);
+    test_result->RecordProperty(xml_element, property);
   }
 
   static void ClearTestPartResults(TestResult* test_result) {
@@ -1430,6 +1465,154 @@
   }
 };
 
+#if GTEST_CAN_STREAM_RESULTS_
+
+// Streams test results to the given port on the given host machine.
+class StreamingListener : public EmptyTestEventListener {
+ public:
+  // Abstract base class for writing strings to a socket.
+  class AbstractSocketWriter {
+   public:
+    virtual ~AbstractSocketWriter() {}
+
+    // Sends a string to the socket.
+    virtual void Send(const string& message) = 0;
+
+    // Closes the socket.
+    virtual void CloseConnection() {}
+
+    // Sends a string and a newline to the socket.
+    void SendLn(const string& message) {
+      Send(message + "\n");
+    }
+  };
+
+  // Concrete class for actually writing strings to a socket.
+  class SocketWriter : public AbstractSocketWriter {
+   public:
+    SocketWriter(const string& host, const string& port)
+        : sockfd_(-1), host_name_(host), port_num_(port) {
+      MakeConnection();
+    }
+
+    virtual ~SocketWriter() {
+      if (sockfd_ != -1)
+        CloseConnection();
+    }
+
+    // Sends a string to the socket.
+    virtual void Send(const string& message) {
+      GTEST_CHECK_(sockfd_ != -1)
+          << "Send() can be called only when there is a connection.";
+
+      const int len = static_cast<int>(message.length());
+      if (write(sockfd_, message.c_str(), len) != len) {
+        GTEST_LOG_(WARNING)
+            << "stream_result_to: failed to stream to "
+            << host_name_ << ":" << port_num_;
+      }
+    }
+
+   private:
+    // Creates a client socket and connects to the server.
+    void MakeConnection();
+
+    // Closes the socket.
+    void CloseConnection() {
+      GTEST_CHECK_(sockfd_ != -1)
+          << "CloseConnection() can be called only when there is a connection.";
+
+      close(sockfd_);
+      sockfd_ = -1;
+    }
+
+    int sockfd_;  // socket file descriptor
+    const string host_name_;
+    const string port_num_;
+
+    GTEST_DISALLOW_COPY_AND_ASSIGN_(SocketWriter);
+  };  // class SocketWriter
+
+  // Escapes '=', '&', '%', and '\n' characters in str as "%xx".
+  static string UrlEncode(const char* str);
+
+  StreamingListener(const string& host, const string& port)
+      : socket_writer_(new SocketWriter(host, port)) { Start(); }
+
+  explicit StreamingListener(AbstractSocketWriter* socket_writer)
+      : socket_writer_(socket_writer) { Start(); }
+
+  void OnTestProgramStart(const UnitTest& /* unit_test */) {
+    SendLn("event=TestProgramStart");
+  }
+
+  void OnTestProgramEnd(const UnitTest& unit_test) {
+    // Note that Google Test current only report elapsed time for each
+    // test iteration, not for the entire test program.
+    SendLn("event=TestProgramEnd&passed=" + FormatBool(unit_test.Passed()));
+
+    // Notify the streaming server to stop.
+    socket_writer_->CloseConnection();
+  }
+
+  void OnTestIterationStart(const UnitTest& /* unit_test */, int iteration) {
+    SendLn("event=TestIterationStart&iteration=" +
+           StreamableToString(iteration));
+  }
+
+  void OnTestIterationEnd(const UnitTest& unit_test, int /* iteration */) {
+    SendLn("event=TestIterationEnd&passed=" +
+           FormatBool(unit_test.Passed()) + "&elapsed_time=" +
+           StreamableToString(unit_test.elapsed_time()) + "ms");
+  }
+
+  void OnTestCaseStart(const TestCase& test_case) {
+    SendLn(std::string("event=TestCaseStart&name=") + test_case.name());
+  }
+
+  void OnTestCaseEnd(const TestCase& test_case) {
+    SendLn("event=TestCaseEnd&passed=" + FormatBool(test_case.Passed())
+           + "&elapsed_time=" + StreamableToString(test_case.elapsed_time())
+           + "ms");
+  }
+
+  void OnTestStart(const TestInfo& test_info) {
+    SendLn(std::string("event=TestStart&name=") + test_info.name());
+  }
+
+  void OnTestEnd(const TestInfo& test_info) {
+    SendLn("event=TestEnd&passed=" +
+           FormatBool((test_info.result())->Passed()) +
+           "&elapsed_time=" +
+           StreamableToString((test_info.result())->elapsed_time()) + "ms");
+  }
+
+  void OnTestPartResult(const TestPartResult& test_part_result) {
+    const char* file_name = test_part_result.file_name();
+    if (file_name == NULL)
+      file_name = "";
+    SendLn("event=TestPartResult&file=" + UrlEncode(file_name) +
+           "&line=" + StreamableToString(test_part_result.line_number()) +
+           "&message=" + UrlEncode(test_part_result.message()));
+  }
+
+ private:
+  // Sends the given message and a newline to the socket.
+  void SendLn(const string& message) { socket_writer_->SendLn(message); }
+
+  // Called at the start of streaming to notify the receiver what
+  // protocol we are using.
+  void Start() { SendLn("gtest_streaming_protocol_version=1.0"); }
+
+  string FormatBool(bool value) { return value ? "1" : "0"; }
+
+  const scoped_ptr<AbstractSocketWriter> socket_writer_;
+
+  GTEST_DISALLOW_COPY_AND_ASSIGN_(StreamingListener);
+};  // class StreamingListener
+
+#endif  // GTEST_CAN_STREAM_RESULTS_
+
 }  // namespace internal
 }  // namespace testing
 
@@ -1483,6 +1666,10 @@
 
 }  // namespace internal
 
+static const char* GetDefaultFilter() {
+  return kUniversalFilter;
+}
+
 GTEST_DEFINE_bool_(
     also_run_disabled_tests,
     internal::BoolFromGTestEnv("also_run_disabled_tests", false),
@@ -1505,11 +1692,11 @@
     "Whether to use colors in the output.  Valid values: yes, no, "
     "and auto.  'auto' means to use colors if the output is "
     "being sent to a terminal and the TERM environment variable "
-    "is set to xterm, xterm-color, xterm-256color, linux or cygwin.");
+    "is set to a terminal type that supports colors.");
 
 GTEST_DEFINE_string_(
     filter,
-    internal::StringFromGTestEnv("filter", kUniversalFilter),
+    internal::StringFromGTestEnv("filter", GetDefaultFilter()),
     "A colon-separated list of glob (not regex) patterns "
     "for filtering the tests to run, optionally followed by a "
     "'-' and a : separated list of negative patterns (tests to "
@@ -1609,7 +1796,7 @@
 // Test.  g_init_gtest_count is set to the number of times
 // InitGoogleTest() has been called.  We don't protect this variable
 // under a mutex as it is only accessed in the main thread.
-int g_init_gtest_count = 0;
+GTEST_API_ int g_init_gtest_count = 0;
 static bool GTestIsInitialized() { return g_init_gtest_count != 0; }
 
 // Iterates over a vector of TestCases, keeping a running sum of the
@@ -1664,10 +1851,10 @@
 }
 
 // Mutex for linked pointers.
-GTEST_DEFINE_STATIC_MUTEX_(g_linked_ptr_mutex);
+GTEST_API_ GTEST_DEFINE_STATIC_MUTEX_(g_linked_ptr_mutex);
 
 // Application pathname gotten in InitGoogleTest.
-String g_executable_path;
+std::string g_executable_path;
 
 // Returns the current application's name, removing directory path if that
 // is present.
@@ -1686,29 +1873,29 @@
 // Functions for processing the gtest_output flag.
 
 // Returns the output format, or "" for normal printed output.
-String UnitTestOptions::GetOutputFormat() {
+std::string UnitTestOptions::GetOutputFormat() {
   const char* const gtest_output_flag = GTEST_FLAG(output).c_str();
-  if (gtest_output_flag == NULL) return String("");
+  if (gtest_output_flag == NULL) return std::string("");
 
   const char* const colon = strchr(gtest_output_flag, ':');
   return (colon == NULL) ?
-      String(gtest_output_flag) :
-      String(gtest_output_flag, colon - gtest_output_flag);
+      std::string(gtest_output_flag) :
+      std::string(gtest_output_flag, colon - gtest_output_flag);
 }
 
 // Returns the name of the requested output file, or the default if none
 // was explicitly specified.
-String UnitTestOptions::GetAbsolutePathToOutputFile() {
+std::string UnitTestOptions::GetAbsolutePathToOutputFile() {
   const char* const gtest_output_flag = GTEST_FLAG(output).c_str();
   if (gtest_output_flag == NULL)
-    return String("");
+    return "";
 
   const char* const colon = strchr(gtest_output_flag, ':');
   if (colon == NULL)
-    return String(internal::FilePath::ConcatPaths(
-               internal::FilePath(
-                   UnitTest::GetInstance()->original_working_dir()),
-               internal::FilePath(kDefaultOutputFile)).ToString() );
+    return internal::FilePath::ConcatPaths(
+        internal::FilePath(
+            UnitTest::GetInstance()->original_working_dir()),
+        internal::FilePath(kDefaultOutputFile)).string();
 
   internal::FilePath output_name(colon + 1);
   if (!output_name.IsAbsolutePath())
@@ -1721,12 +1908,12 @@
         internal::FilePath(colon + 1));
 
   if (!output_name.IsDirectory())
-    return output_name.ToString();
+    return output_name.string();
 
   internal::FilePath result(internal::FilePath::GenerateUniqueFileName(
       output_name, internal::GetCurrentExecutableName(),
       GetOutputFormat().c_str()));
-  return result.ToString();
+  return result.string();
 }
 
 // Returns true iff the wildcard pattern matches the string.  The
@@ -1751,7 +1938,8 @@
   }
 }
 
-bool UnitTestOptions::MatchesFilter(const String& name, const char* filter) {
+bool UnitTestOptions::MatchesFilter(
+    const std::string& name, const char* filter) {
   const char *cur_pattern = filter;
   for (;;) {
     if (PatternMatchesString(cur_pattern, name.c_str())) {
@@ -1769,31 +1957,26 @@
     // Skips the pattern separater (the ':' character).
     cur_pattern++;
   }
-  return false;
 }
 
-// TODO(keithray): move String function implementations to gtest-string.cc.
-
 // Returns true iff the user-specified filter matches the test case
 // name and the test name.
-bool UnitTestOptions::FilterMatchesTest(const String &test_case_name,
-                                        const String &test_name) {
-  const String& full_name = String::Format("%s.%s",
-                                           test_case_name.c_str(),
-                                           test_name.c_str());
+bool UnitTestOptions::FilterMatchesTest(const std::string &test_case_name,
+                                        const std::string &test_name) {
+  const std::string& full_name = test_case_name + "." + test_name.c_str();
 
   // Split --gtest_filter at '-', if there is one, to separate into
   // positive filter and negative filter portions
   const char* const p = GTEST_FLAG(filter).c_str();
   const char* const dash = strchr(p, '-');
-  String positive;
-  String negative;
+  std::string positive;
+  std::string negative;
   if (dash == NULL) {
     positive = GTEST_FLAG(filter).c_str();  // Whole string is a positive filter
-    negative = String("");
+    negative = "";
   } else {
-    positive = String(p, dash - p);  // Everything up to the dash
-    negative = String(dash+1);       // Everything after the dash
+    positive = std::string(p, dash);   // Everything up to the dash
+    negative = std::string(dash + 1);  // Everything after the dash
     if (positive.empty()) {
       // Treat '-test1' as the same as '*-test1'
       positive = kUniversalFilter;
@@ -1913,7 +2096,7 @@
                               const TestPartResultArray& results,
                               TestPartResult::Type type,
                               const string& substr) {
-  const String expected(type == TestPartResult::kFatalFailure ?
+  const std::string expected(type == TestPartResult::kFatalFailure ?
                         "1 fatal failure" :
                         "1 non-fatal failure");
   Message msg;
@@ -2036,11 +2219,22 @@
   return SumOverTestCaseList(test_cases_, &TestCase::failed_test_count);
 }
 
+// Gets the number of disabled tests that will be reported in the XML report.
+int UnitTestImpl::reportable_disabled_test_count() const {
+  return SumOverTestCaseList(test_cases_,
+                             &TestCase::reportable_disabled_test_count);
+}
+
 // Gets the number of disabled tests.
 int UnitTestImpl::disabled_test_count() const {
   return SumOverTestCaseList(test_cases_, &TestCase::disabled_test_count);
 }
 
+// Gets the number of tests to be printed in the XML report.
+int UnitTestImpl::reportable_test_count() const {
+  return SumOverTestCaseList(test_cases_, &TestCase::reportable_test_count);
+}
+
 // Gets the number of all tests.
 int UnitTestImpl::total_test_count() const {
   return SumOverTestCaseList(test_cases_, &TestCase::total_test_count);
@@ -2051,7 +2245,7 @@
   return SumOverTestCaseList(test_cases_, &TestCase::test_to_run_count);
 }
 
-// Returns the current OS stack trace as a String.
+// Returns the current OS stack trace as an std::string.
 //
 // The maximum number of stack frames to be included is specified by
 // the gtest_stack_trace_depth flag.  The skip_count parameter
@@ -2061,9 +2255,9 @@
 // For example, if Foo() calls Bar(), which in turn calls
 // CurrentOsStackTraceExceptTop(1), Foo() will be included in the
 // trace but Bar() and CurrentOsStackTraceExceptTop() won't.
-String UnitTestImpl::CurrentOsStackTraceExceptTop(int skip_count) {
+std::string UnitTestImpl::CurrentOsStackTraceExceptTop(int skip_count) {
   (void)skip_count;
-  return String("");
+  return "";
 }
 
 // Returns the current time in milliseconds.
@@ -2120,41 +2314,7 @@
 
 // Utilities
 
-// class String
-
-// Returns the input enclosed in double quotes if it's not NULL;
-// otherwise returns "(null)".  For example, "\"Hello\"" is returned
-// for input "Hello".
-//
-// This is useful for printing a C string in the syntax of a literal.
-//
-// Known issue: escape sequences are not handled yet.
-String String::ShowCStringQuoted(const char* c_str) {
-  return c_str ? String::Format("\"%s\"", c_str) : String("(null)");
-}
-
-// Copies at most length characters from str into a newly-allocated
-// piece of memory of size length+1.  The memory is allocated with new[].
-// A terminating null byte is written to the memory, and a pointer to it
-// is returned.  If str is NULL, NULL is returned.
-static char* CloneString(const char* str, size_t length) {
-  if (str == NULL) {
-    return NULL;
-  } else {
-    char* const clone = new char[length + 1];
-    posix::StrNCpy(clone, str, length);
-    clone[length] = '\0';
-    return clone;
-  }
-}
-
-// Clones a 0-terminated C string, allocating memory using new.  The
-// caller is responsible for deleting[] the return value.  Returns the
-// cloned string, or NULL if the input is NULL.
-const char * String::CloneCString(const char* c_str) {
-  return (c_str == NULL) ?
-                    NULL : CloneString(c_str, strlen(c_str));
-}
+// class String.
 
 #if GTEST_OS_WINDOWS_MOBILE
 // Creates a UTF-16 wide string from the given ANSI string, allocating
@@ -2211,11 +2371,6 @@
 // encoding, and streams the result to the given Message object.
 static void StreamWideCharsToMessage(const wchar_t* wstr, size_t length,
                                      Message* msg) {
-  // TODO(wan): consider allowing a testing::String object to
-  // contain '\0'.  This will make it behave more like std::string,
-  // and will allow ToUtf8String() to return the correct encoding
-  // for '\0' s.t. we can get rid of the conditional here (and in
-  // several other places).
   for (size_t i = 0; i != length; ) {  // NOLINT
     if (wstr[i] != L'\0') {
       *msg << WideStringToUtf8(wstr + i, static_cast<int>(length - i));
@@ -2232,6 +2387,26 @@
 
 }  // namespace internal
 
+// Constructs an empty Message.
+// We allocate the stringstream separately because otherwise each use of
+// ASSERT/EXPECT in a procedure adds over 200 bytes to the procedure's
+// stack frame leading to huge stack frames in some cases; gcc does not reuse
+// the stack space.
+Message::Message() : ss_(new ::std::stringstream) {
+  // By default, we want there to be enough precision when printing
+  // a double to a Message.
+  *ss_ << std::setprecision(std::numeric_limits<double>::digits10 + 2);
+}
+
+// These two overloads allow streaming a wide C string to a Message
+// using the UTF-8 encoding.
+Message& Message::operator <<(const wchar_t* wide_c_str) {
+  return *this << internal::String::ShowWideCString(wide_c_str);
+}
+Message& Message::operator <<(wchar_t* wide_c_str) {
+  return *this << internal::String::ShowWideCString(wide_c_str);
+}
+
 #if GTEST_HAS_STD_WSTRING
 // Converts the given wide string to a narrow string using the UTF-8
 // encoding, and streams the result to this Message object.
@@ -2250,6 +2425,12 @@
 }
 #endif  // GTEST_HAS_GLOBAL_WSTRING
 
+// Gets the text streamed to this object so far as an std::string.
+// Each '\0' character in the buffer is replaced with "\\0".
+std::string Message::GetString() const {
+  return internal::StringStreamToString(ss_.get());
+}
+
 // AssertionResult constructors.
 // Used in EXPECT_TRUE/FALSE(assertion_result).
 AssertionResult::AssertionResult(const AssertionResult& other)
@@ -2302,8 +2483,8 @@
 // be inserted into the message.
 AssertionResult EqFailure(const char* expected_expression,
                           const char* actual_expression,
-                          const String& expected_value,
-                          const String& actual_value,
+                          const std::string& expected_value,
+                          const std::string& actual_value,
                           bool ignoring_case) {
   Message msg;
   msg << "Value of: " << actual_expression;
@@ -2323,10 +2504,11 @@
 }
 
 // Constructs a failure message for Boolean assertions such as EXPECT_TRUE.
-String GetBoolAssertionFailureMessage(const AssertionResult& assertion_result,
-                                      const char* expression_text,
-                                      const char* actual_predicate_value,
-                                      const char* expected_predicate_value) {
+std::string GetBoolAssertionFailureMessage(
+    const AssertionResult& assertion_result,
+    const char* expression_text,
+    const char* actual_predicate_value,
+    const char* expected_predicate_value) {
   const char* actual_message = assertion_result.message();
   Message msg;
   msg << "Value of: " << expression_text
@@ -2473,8 +2655,8 @@
 
   return EqFailure(expected_expression,
                    actual_expression,
-                   String::ShowCStringQuoted(expected),
-                   String::ShowCStringQuoted(actual),
+                   PrintToString(expected),
+                   PrintToString(actual),
                    false);
 }
 
@@ -2489,8 +2671,8 @@
 
   return EqFailure(expected_expression,
                    actual_expression,
-                   String::ShowCStringQuoted(expected),
-                   String::ShowCStringQuoted(actual),
+                   PrintToString(expected),
+                   PrintToString(actual),
                    true);
 }
 
@@ -2654,7 +2836,7 @@
   // want inserts expanded.
   const DWORD kFlags = FORMAT_MESSAGE_FROM_SYSTEM |
                        FORMAT_MESSAGE_IGNORE_INSERTS;
-  const DWORD kBufSize = 4096;  // String::Format can't exceed this length.
+  const DWORD kBufSize = 4096;
   // Gets the system's human readable message string for this HRESULT.
   char error_text[kBufSize] = { '\0' };
   DWORD message_length = ::FormatMessageA(kFlags,
@@ -2664,7 +2846,7 @@
                                           error_text,  // output buffer
                                           kBufSize,  // buf size
                                           NULL);  // no arguments for inserts
-  // Trims tailing white space (FormatMessage leaves a trailing cr-lf)
+  // Trims tailing white space (FormatMessage leaves a trailing CR-LF)
   for (; message_length && IsSpace(error_text[message_length - 1]);
           --message_length) {
     error_text[message_length - 1] = '\0';
@@ -2672,10 +2854,10 @@
 
 # endif  // GTEST_OS_WINDOWS_MOBILE
 
-  const String error_hex(String::Format("0x%08X ", hr));
+  const std::string error_hex("0x" + String::FormatHexInt(hr));
   return ::testing::AssertionFailure()
       << "Expected: " << expr << " " << expected << ".\n"
-      << "  Actual: " << error_hex << error_text << "\n";
+      << "  Actual: " << error_hex << " " << error_text << "\n";
 }
 
 }  // namespace
@@ -2732,12 +2914,15 @@
 // Converts a Unicode code point to a narrow string in UTF-8 encoding.
 // code_point parameter is of type UInt32 because wchar_t may not be
 // wide enough to contain a code point.
-// The output buffer str must containt at least 32 characters.
-// The function returns the address of the output buffer.
 // If the code_point is not a valid Unicode code point
-// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be output
-// as '(Invalid Unicode 0xXXXXXXXX)'.
-char* CodePointToUtf8(UInt32 code_point, char* str) {
+// (i.e. outside of Unicode range U+0 to U+10FFFF) it will be converted
+// to "(Invalid Unicode 0xXXXXXXXX)".
+std::string CodePointToUtf8(UInt32 code_point) {
+  if (code_point > kMaxCodePoint4) {
+    return "(Invalid Unicode 0x" + String::FormatHexInt(code_point) + ")";
+  }
+
+  char str[5];  // Big enough for the largest valid code point.
   if (code_point <= kMaxCodePoint1) {
     str[1] = '\0';
     str[0] = static_cast<char>(code_point);                          // 0xxxxxxx
@@ -2750,22 +2935,12 @@
     str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
     str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
     str[0] = static_cast<char>(0xE0 | code_point);                   // 1110xxxx
-  } else if (code_point <= kMaxCodePoint4) {
+  } else {  // code_point <= kMaxCodePoint4
     str[4] = '\0';
     str[3] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
     str[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
     str[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));  // 10xxxxxx
     str[0] = static_cast<char>(0xF0 | code_point);                   // 11110xxx
-  } else {
-    // The longest string String::Format can produce when invoked
-    // with these parameters is 28 character long (not including
-    // the terminating nul character). We are asking for 32 character
-    // buffer just in case. This is also enough for strncpy to
-    // null-terminate the destination string.
-    posix::StrNCpy(
-        str, String::Format("(Invalid Unicode 0x%X)", code_point).c_str(), 32);
-    str[31] = '\0';  // Makes sure no change in the format to strncpy leaves
-                     // the result unterminated.
   }
   return str;
 }
@@ -2806,7 +2981,7 @@
 // as '(Invalid Unicode 0xXXXXXXXX)'. If the string is in UTF16 encoding
 // and contains invalid UTF-16 surrogate pairs, values in those pairs
 // will be encoded as individual Unicode characters from Basic Normal Plane.
-String WideStringToUtf8(const wchar_t* str, int num_chars) {
+std::string WideStringToUtf8(const wchar_t* str, int num_chars) {
   if (num_chars == -1)
     num_chars = static_cast<int>(wcslen(str));
 
@@ -2824,27 +2999,17 @@
       unicode_code_point = static_cast<UInt32>(str[i]);
     }
 
-    char buffer[32];  // CodePointToUtf8 requires a buffer this big.
-    stream << CodePointToUtf8(unicode_code_point, buffer);
+    stream << CodePointToUtf8(unicode_code_point);
   }
   return StringStreamToString(&stream);
 }
 
-// Converts a wide C string to a String using the UTF-8 encoding.
+// Converts a wide C string to an std::string using the UTF-8 encoding.
 // NULL will be converted to "(null)".
-String String::ShowWideCString(const wchar_t * wide_c_str) {
-  if (wide_c_str == NULL) return String("(null)");
-
-  return String(internal::WideStringToUtf8(wide_c_str, -1).c_str());
-}
-
-// Similar to ShowWideCString(), except that this function encloses
-// the converted string in double quotes.
-String String::ShowWideCStringQuoted(const wchar_t* wide_c_str) {
-  if (wide_c_str == NULL) return String("(null)");
+std::string String::ShowWideCString(const wchar_t * wide_c_str) {
+  if (wide_c_str == NULL)  return "(null)";
 
-  return String::Format("L\"%s\"",
-                        String::ShowWideCString(wide_c_str).c_str());
+  return internal::WideStringToUtf8(wide_c_str, -1);
 }
 
 // Compares two wide C strings.  Returns true iff they have the same
@@ -2872,8 +3037,8 @@
 
   return EqFailure(expected_expression,
                    actual_expression,
-                   String::ShowWideCStringQuoted(expected),
-                   String::ShowWideCStringQuoted(actual),
+                   PrintToString(expected),
+                   PrintToString(actual),
                    false);
 }
 
@@ -2888,8 +3053,8 @@
 
   return AssertionFailure() << "Expected: (" << s1_expression << ") != ("
                             << s2_expression << "), actual: "
-                            << String::ShowWideCStringQuoted(s1)
-                            << " vs " << String::ShowWideCStringQuoted(s2);
+                            << PrintToString(s1)
+                            << " vs " << PrintToString(s2);
 }
 
 // Compares two C strings, ignoring case.  Returns true iff they have
@@ -2940,135 +3105,69 @@
 #endif  // OS selector
 }
 
-// Compares this with another String.
-// Returns < 0 if this is less than rhs, 0 if this is equal to rhs, or > 0
-// if this is greater than rhs.
-int String::Compare(const String & rhs) const {
-  const char* const lhs_c_str = c_str();
-  const char* const rhs_c_str = rhs.c_str();
-
-  if (lhs_c_str == NULL) {
-    return rhs_c_str == NULL ? 0 : -1;  // NULL < anything except NULL
-  } else if (rhs_c_str == NULL) {
-    return 1;
-  }
-
-  const size_t shorter_str_len =
-      length() <= rhs.length() ? length() : rhs.length();
-  for (size_t i = 0; i != shorter_str_len; i++) {
-    if (lhs_c_str[i] < rhs_c_str[i]) {
-      return -1;
-    } else if (lhs_c_str[i] > rhs_c_str[i]) {
-      return 1;
-    }
-  }
-  return (length() < rhs.length()) ? -1 :
-      (length() > rhs.length()) ? 1 : 0;
+// Returns true iff str ends with the given suffix, ignoring case.
+// Any string is considered to end with an empty suffix.
+bool String::EndsWithCaseInsensitive(
+    const std::string& str, const std::string& suffix) {
+  const size_t str_len = str.length();
+  const size_t suffix_len = suffix.length();
+  return (str_len >= suffix_len) &&
+         CaseInsensitiveCStringEquals(str.c_str() + str_len - suffix_len,
+                                      suffix.c_str());
 }
 
-// Returns true iff this String ends with the given suffix.  *Any*
-// String is considered to end with a NULL or empty suffix.
-bool String::EndsWith(const char* suffix) const {
-  if (suffix == NULL || CStringEquals(suffix, "")) return true;
-
-  if (c_str() == NULL) return false;
-
-  const size_t this_len = strlen(c_str());
-  const size_t suffix_len = strlen(suffix);
-  return (this_len >= suffix_len) &&
-         CStringEquals(c_str() + this_len - suffix_len, suffix);
+// Formats an int value as "%02d".
+std::string String::FormatIntWidth2(int value) {
+  std::stringstream ss;
+  ss << std::setfill('0') << std::setw(2) << value;
+  return ss.str();
 }
 
-// Returns true iff this String ends with the given suffix, ignoring case.
-// Any String is considered to end with a NULL or empty suffix.
-bool String::EndsWithCaseInsensitive(const char* suffix) const {
-  if (suffix == NULL || CStringEquals(suffix, "")) return true;
-
-  if (c_str() == NULL) return false;
-
-  const size_t this_len = strlen(c_str());
-  const size_t suffix_len = strlen(suffix);
-  return (this_len >= suffix_len) &&
-         CaseInsensitiveCStringEquals(c_str() + this_len - suffix_len, suffix);
+// Formats an int value as "%X".
+std::string String::FormatHexInt(int value) {
+  std::stringstream ss;
+  ss << std::hex << std::uppercase << value;
+  return ss.str();
 }
 
-// Formats a list of arguments to a String, using the same format
-// spec string as for printf.
-//
-// We do not use the StringPrintf class as it is not universally
-// available.
-//
-// The result is limited to 4096 characters (including the tailing 0).
-// If 4096 characters are not enough to format the input, or if
-// there's an error, "<formatting error or buffer exceeded>" is
-// returned.
-String String::Format(const char * format, ...) {
-  va_list args;
-  va_start(args, format);
-
-  char buffer[4096];
-  const int kBufferSize = sizeof(buffer)/sizeof(buffer[0]);
-
-  // MSVC 8 deprecates vsnprintf(), so we want to suppress warning
-  // 4996 (deprecated function) there.
-#ifdef _MSC_VER  // We are using MSVC.
-# pragma warning(push)          // Saves the current warning state.
-# pragma warning(disable:4996)  // Temporarily disables warning 4996.
-
-  const int size = vsnprintf(buffer, kBufferSize, format, args);
-
-# pragma warning(pop)           // Restores the warning state.
-#else  // We are not using MSVC.
-  const int size = vsnprintf(buffer, kBufferSize, format, args);
-#endif  // _MSC_VER
-  va_end(args);
-
-  // vsnprintf()'s behavior is not portable.  When the buffer is not
-  // big enough, it returns a negative value in MSVC, and returns the
-  // needed buffer size on Linux.  When there is an output error, it
-  // always returns a negative value.  For simplicity, we lump the two
-  // error cases together.
-  if (size < 0 || size >= kBufferSize) {
-    return String("<formatting error or buffer exceeded>");
-  } else {
-    return String(buffer, size);
-  }
+// Formats a byte as "%02X".
+std::string String::FormatByte(unsigned char value) {
+  std::stringstream ss;
+  ss << std::setfill('0') << std::setw(2) << std::hex << std::uppercase
+     << static_cast<unsigned int>(value);
+  return ss.str();
 }
 
-// Converts the buffer in a stringstream to a String, converting NUL
+// Converts the buffer in a stringstream to an std::string, converting NUL
 // bytes to "\\0" along the way.
-String StringStreamToString(::std::stringstream* ss) {
+std::string StringStreamToString(::std::stringstream* ss) {
   const ::std::string& str = ss->str();
   const char* const start = str.c_str();
   const char* const end = start + str.length();
 
-  // We need to use a helper stringstream to do this transformation
-  // because String doesn't support push_back().
-  ::std::stringstream helper;
+  std::string result;
+  result.reserve(2 * (end - start));
   for (const char* ch = start; ch != end; ++ch) {
     if (*ch == '\0') {
-      helper << "\\0";  // Replaces NUL with "\\0";
+      result += "\\0";  // Replaces NUL with "\\0";
     } else {
-      helper.put(*ch);
+      result += *ch;
     }
   }
 
-  return String(helper.str().c_str());
+  return result;
 }
 
 // Appends the user-supplied message to the Google-Test-generated message.
-String AppendUserMessage(const String& gtest_msg,
-                         const Message& user_msg) {
+std::string AppendUserMessage(const std::string& gtest_msg,
+                              const Message& user_msg) {
   // Appends the user message if it's non-empty.
-  const String user_msg_string = user_msg.GetString();
+  const std::string user_msg_string = user_msg.GetString();
   if (user_msg_string.empty()) {
     return gtest_msg;
   }
 
-  Message msg;
-  msg << gtest_msg << "\n" << user_msg_string;
-
-  return msg.GetString();
+  return gtest_msg + "\n" + user_msg_string;
 }
 
 }  // namespace internal
@@ -3116,8 +3215,9 @@
 // Adds a test property to the list. If a property with the same key as the
 // supplied property is already represented, the value of this test_property
 // replaces the old value for that key.
-void TestResult::RecordProperty(const TestProperty& test_property) {
-  if (!ValidateTestProperty(test_property)) {
+void TestResult::RecordProperty(const std::string& xml_element,
+                                const TestProperty& test_property) {
+  if (!ValidateTestProperty(xml_element, test_property)) {
     return;
   }
   internal::MutexLock lock(&test_properites_mutex_);
@@ -3131,21 +3231,94 @@
   property_with_matching_key->SetValue(test_property.value());
 }
 
-// Adds a failure if the key is a reserved attribute of Google Test
-// testcase tags.  Returns true if the property is valid.
-bool TestResult::ValidateTestProperty(const TestProperty& test_property) {
-  internal::String key(test_property.key());
-  if (key == "name" || key == "status" || key == "time" || key == "classname") {
-    ADD_FAILURE()
-        << "Reserved key used in RecordProperty(): "
-        << key
-        << " ('name', 'status', 'time', and 'classname' are reserved by "
-        << GTEST_NAME_ << ")";
+// The list of reserved attributes used in the <testsuites> element of XML
+// output.
+static const char* const kReservedTestSuitesAttributes[] = {
+  "disabled",
+  "errors",
+  "failures",
+  "name",
+  "random_seed",
+  "tests",
+  "time",
+  "timestamp"
+};
+
+// The list of reserved attributes used in the <testsuite> element of XML
+// output.
+static const char* const kReservedTestSuiteAttributes[] = {
+  "disabled",
+  "errors",
+  "failures",
+  "name",
+  "tests",
+  "time"
+};
+
+// The list of reserved attributes used in the <testcase> element of XML output.
+static const char* const kReservedTestCaseAttributes[] = {
+  "classname",
+  "name",
+  "status",
+  "time",
+  "type_param",
+  "value_param"
+};
+
+template <int kSize>
+std::vector<std::string> ArrayAsVector(const char* const (&array)[kSize]) {
+  return std::vector<std::string>(array, array + kSize);
+}
+
+static std::vector<std::string> GetReservedAttributesForElement(
+    const std::string& xml_element) {
+  if (xml_element == "testsuites") {
+    return ArrayAsVector(kReservedTestSuitesAttributes);
+  } else if (xml_element == "testsuite") {
+    return ArrayAsVector(kReservedTestSuiteAttributes);
+  } else if (xml_element == "testcase") {
+    return ArrayAsVector(kReservedTestCaseAttributes);
+  } else {
+    GTEST_CHECK_(false) << "Unrecognized xml_element provided: " << xml_element;
+  }
+  // This code is unreachable but some compilers may not realizes that.
+  return std::vector<std::string>();
+}
+
+static std::string FormatWordList(const std::vector<std::string>& words) {
+  Message word_list;
+  for (size_t i = 0; i < words.size(); ++i) {
+    if (i > 0 && words.size() > 2) {
+      word_list << ", ";
+    }
+    if (i == words.size() - 1) {
+      word_list << "and ";
+    }
+    word_list << "'" << words[i] << "'";
+  }
+  return word_list.GetString();
+}
+
+bool ValidateTestPropertyName(const std::string& property_name,
+                              const std::vector<std::string>& reserved_names) {
+  if (std::find(reserved_names.begin(), reserved_names.end(), property_name) !=
+          reserved_names.end()) {
+    ADD_FAILURE() << "Reserved key used in RecordProperty(): " << property_name
+                  << " (" << FormatWordList(reserved_names)
+                  << " are reserved by " << GTEST_NAME_ << ")";
     return false;
   }
   return true;
 }
 
+// Adds a failure if the key is a reserved attribute of the element named
+// xml_element.  Returns true if the property is valid.
+bool TestResult::ValidateTestProperty(const std::string& xml_element,
+                                      const TestProperty& test_property) {
+  return ValidateTestPropertyName(test_property.key(),
+                                  GetReservedAttributesForElement(xml_element));
+}
+
 // Clears the object.
 void TestResult::Clear() {
   test_part_results_.clear();
@@ -3221,12 +3394,12 @@
 }
 
 // Allows user supplied key value pairs to be recorded for later output.
-void Test::RecordProperty(const char* key, const char* value) {
-  UnitTest::GetInstance()->RecordPropertyForCurrentTest(key, value);
+void Test::RecordProperty(const std::string& key, const std::string& value) {
+  UnitTest::GetInstance()->RecordProperty(key, value);
 }
 
 // Allows user supplied key value pairs to be recorded for later output.
-void Test::RecordProperty(const char* key, int value) {
+void Test::RecordProperty(const std::string& key, int value) {
   Message value_message;
   value_message << value;
   RecordProperty(key, value_message.GetString().c_str());
@@ -3235,7 +3408,7 @@
 namespace internal {
 
 void ReportFailureInUnknownLocation(TestPartResult::Type result_type,
-                                    const String& message) {
+                                    const std::string& message) {
   // This function is a friend of UnitTest and as such has access to
   // AddTestPartResult.
   UnitTest::GetInstance()->AddTestPartResult(
@@ -3243,7 +3416,7 @@
       NULL,  // No info about the source file where the exception occurred.
       -1,    // We have no info on which line caused the exception.
       message,
-      String());  // No stack trace, either.
+      "");   // No stack trace, either.
 }
 
 }  // namespace internal
@@ -3320,22 +3493,24 @@
 // function returns its result via an output parameter pointer because VC++
 // prohibits creation of objects with destructors on stack in functions
 // using __try (see error C2712).
-static internal::String* FormatSehExceptionMessage(DWORD exception_code,
-                                                   const char* location) {
+static std::string* FormatSehExceptionMessage(DWORD exception_code,
+                                              const char* location) {
   Message message;
   message << "SEH exception with code 0x" << std::setbase(16) <<
     exception_code << std::setbase(10) << " thrown in " << location << ".";
 
-  return new internal::String(message.GetString());
+  return new std::string(message.GetString());
 }
 
 #endif  // GTEST_HAS_SEH
 
+namespace internal {
+
 #if GTEST_HAS_EXCEPTIONS
 
 // Adds an "exception thrown" fatal failure to the current test.
-static internal::String FormatCxxExceptionMessage(const char* description,
-                                                  const char* location) {
+static std::string FormatCxxExceptionMessage(const char* description,
+                                             const char* location) {
   Message message;
   if (description != NULL) {
     message << "C++ exception with description \"" << description << "\"";
@@ -3347,23 +3522,15 @@
   return message.GetString();
 }
 
-static internal::String PrintTestPartResultToString(
+static std::string PrintTestPartResultToString(
     const TestPartResult& test_part_result);
 
-// A failed Google Test assertion will throw an exception of this type when
-// GTEST_FLAG(throw_on_failure) is true (if exceptions are enabled).  We
-// derive it from std::runtime_error, which is for errors presumably
-// detectable only at run time.  Since std::runtime_error inherits from
-// std::exception, many testing frameworks know how to extract and print the
-// message inside it.
-class GoogleTestFailureException : public ::std::runtime_error {
- public:
-  explicit GoogleTestFailureException(const TestPartResult& failure)
-      : ::std::runtime_error(PrintTestPartResultToString(failure).c_str()) {}
-};
+GoogleTestFailureException::GoogleTestFailureException(
+    const TestPartResult& failure)
+    : ::std::runtime_error(PrintTestPartResultToString(failure).c_str()) {}
+
 #endif  // GTEST_HAS_EXCEPTIONS
 
-namespace internal {
 // We put these helper functions in the internal namespace as IBM's xlC
 // compiler rejects the code if they were declared static.
 
@@ -3383,7 +3550,7 @@
     // We create the exception message on the heap because VC++ prohibits
     // creation of objects with destructors on stack in functions using __try
     // (see error C2712).
-    internal::String* exception_message = FormatSehExceptionMessage(
+    std::string* exception_message = FormatSehExceptionMessage(
         GetExceptionCode(), location);
     internal::ReportFailureInUnknownLocation(TestPartResult::kFatalFailure,
                                              *exception_message);
@@ -3429,9 +3596,10 @@
 #if GTEST_HAS_EXCEPTIONS
     try {
       return HandleSehExceptionsInMethodIfSupported(object, method, location);
-    } catch (const GoogleTestFailureException&) {  // NOLINT
-      // This exception doesn't originate in code under test. It makes no
-      // sense to report it as a test failure.
+    } catch (const internal::GoogleTestFailureException&) {  // NOLINT
+      // This exception type can only be thrown by a failed Google
+      // Test assertion with the intention of letting another testing
+      // framework catch it.  Therefore we just re-throw it.
       throw;
     } catch (const std::exception& e) {  // NOLINT
       internal::ReportFailureInUnknownLocation(
@@ -3490,10 +3658,8 @@
 
 // Constructs a TestInfo object. It assumes ownership of the test factory
 // object.
-// TODO(vladl@google.com): Make a_test_case_name and a_name const string&'s
-// to signify they cannot be NULLs.
-TestInfo::TestInfo(const char* a_test_case_name,
-                   const char* a_name,
+TestInfo::TestInfo(const std::string& a_test_case_name,
+                   const std::string& a_name,
                    const char* a_type_param,
                    const char* a_value_param,
                    internal::TypeId fixture_class_id,
@@ -3532,7 +3698,8 @@
 //                     The newly created TestInfo instance will assume
 //                     ownership of the factory object.
 TestInfo* MakeAndRegisterTestInfo(
-    const char* test_case_name, const char* name,
+    const char* test_case_name,
+    const char* name,
     const char* type_param,
     const char* value_param,
     TypeId fixture_class_id,
@@ -3587,11 +3754,11 @@
 
   // Returns true iff the test name of test_info matches name_.
   bool operator()(const TestInfo * test_info) const {
-    return test_info && internal::String(test_info->name()).Compare(name_) == 0;
+    return test_info && test_info->name() == name_;
   }
 
  private:
-  internal::String name_;
+  std::string name_;
 };
 
 }  // namespace
@@ -3670,10 +3837,21 @@
   return CountIf(test_info_list_, TestFailed);
 }
 
+// Gets the number of disabled tests that will be reported in the XML report.
+int TestCase::reportable_disabled_test_count() const {
+  return CountIf(test_info_list_, TestReportableDisabled);
+}
+
+// Gets the number of disabled tests in this test case.
 int TestCase::disabled_test_count() const {
   return CountIf(test_info_list_, TestDisabled);
 }
 
+// Gets the number of tests to be printed in the XML report.
+int TestCase::reportable_test_count() const {
+  return CountIf(test_info_list_, TestReportable);
+}
+
 // Get the number of tests in this test case that should run.
 int TestCase::test_to_run_count() const {
   return CountIf(test_info_list_, ShouldRunTest);
@@ -3761,6 +3939,7 @@
 
 // Clears the results of all tests in this test case.
 void TestCase::ClearResult() {
+  ad_hoc_test_result_.Clear();
   ForEach(test_info_list_, TestInfo::ClearTestResult);
 }
 
@@ -3781,20 +3960,20 @@
 //
 // FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
 // FormatCountableNoun(5, "book", "books") returns "5 books".
-static internal::String FormatCountableNoun(int count,
-                                            const char * singular_form,
-                                            const char * plural_form) {
-  return internal::String::Format("%d %s", count,
-                                  count == 1 ? singular_form : plural_form);
+static std::string FormatCountableNoun(int count,
+                                       const char * singular_form,
+                                       const char * plural_form) {
+  return internal::StreamableToString(count) + " " +
+      (count == 1 ? singular_form : plural_form);
 }
 
 // Formats the count of tests.
-static internal::String FormatTestCount(int test_count) {
+static std::string FormatTestCount(int test_count) {
   return FormatCountableNoun(test_count, "test", "tests");
 }
 
 // Formats the count of test cases.
-static internal::String FormatTestCaseCount(int test_case_count) {
+static std::string FormatTestCaseCount(int test_case_count) {
   return FormatCountableNoun(test_case_count, "test case", "test cases");
 }
 
@@ -3819,8 +3998,10 @@
   }
 }
 
-// Prints a TestPartResult to a String.
-static internal::String PrintTestPartResultToString(
+namespace internal {
+
+// Prints a TestPartResult to an std::string.
+static std::string PrintTestPartResultToString(
     const TestPartResult& test_part_result) {
   return (Message()
           << internal::FormatFileLocation(test_part_result.file_name(),
@@ -3831,7 +4012,7 @@
 
 // Prints a TestPartResult.
 static void PrintTestPartResult(const TestPartResult& test_part_result) {
-  const internal::String& result =
+  const std::string& result =
       PrintTestPartResultToString(test_part_result);
   printf("%s\n", result.c_str());
   fflush(stdout);
@@ -3850,8 +4031,6 @@
 
 // class PrettyUnitTestResultPrinter
 
-namespace internal {
-
 enum GTestColor {
   COLOR_DEFAULT,
   COLOR_RED,
@@ -3875,14 +4054,13 @@
 
 // Returns the ANSI color code for the given color.  COLOR_DEFAULT is
 // an invalid input.
-char* GetAnsiColorCode(GTestColor color) {
+const char* GetAnsiColorCode(GTestColor color) {
   switch (color) {
     case COLOR_RED:     return "1";
     case COLOR_GREEN:   return "2";
     case COLOR_YELLOW:  return "3";
     default:            return NULL;
   };
-  return NULL;
 }
 
 #endif  // GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
@@ -3904,6 +4082,7 @@
         String::CStringEquals(term, "xterm-color") ||
         String::CStringEquals(term, "xterm-256color") ||
         String::CStringEquals(term, "screen") ||
+        String::CStringEquals(term, "screen-256color") ||
         String::CStringEquals(term, "linux") ||
         String::CStringEquals(term, "cygwin");
     return stdout_is_tty && term_supports_color;
@@ -3927,7 +4106,7 @@
   va_list args;
   va_start(args, fmt);
 
-#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_SYMBIAN || GTEST_OS_ZOS
+#if GTEST_OS_WINDOWS_MOBILE || GTEST_OS_SYMBIAN || GTEST_OS_ZOS || GTEST_OS_IOS
   const bool use_color = false;
 #else
   static const bool in_color_mode =
@@ -3969,6 +4148,11 @@
   va_end(args);
 }
 
+// Text printed in Google Test's text output and --gunit_list_tests
+// output to label the type parameter and value parameter for a test.
+static const char kTypeParamLabel[] = "TypeParam";
+static const char kValueParamLabel[] = "GetParam()";
+
 void PrintFullTestCommentIfPresent(const TestInfo& test_info) {
   const char* const type_param = test_info.type_param();
   const char* const value_param = test_info.value_param();
@@ -3976,12 +4160,12 @@
   if (type_param != NULL || value_param != NULL) {
     printf(", where ");
     if (type_param != NULL) {
-      printf("TypeParam = %s", type_param);
+      printf("%s = %s", kTypeParamLabel, type_param);
       if (value_param != NULL)
         printf(" and ");
     }
     if (value_param != NULL) {
-      printf("GetParam() = %s", value_param);
+      printf("%s = %s", kValueParamLabel, value_param);
     }
   }
 }
@@ -4013,8 +4197,6 @@
 
  private:
   static void PrintFailedTests(const UnitTest& unit_test);
-
-  internal::String test_case_name_;
 };
 
   // Fired before each iteration of tests starts.
@@ -4027,7 +4209,7 @@
 
   // Prints the filter if it's not *.  This reminds the user that some
   // tests may be skipped.
-  if (!internal::String::CStringEquals(filter, kUniversalFilter)) {
+  if (!String::CStringEquals(filter, kUniversalFilter)) {
     ColoredPrintf(COLOR_YELLOW,
                   "Note: %s filter = %s\n", GTEST_NAME_, filter);
   }
@@ -4061,22 +4243,21 @@
 }
 
 void PrettyUnitTestResultPrinter::OnTestCaseStart(const TestCase& test_case) {
-  test_case_name_ = test_case.name();
-  const internal::String counts =
+  const std::string counts =
       FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
   ColoredPrintf(COLOR_GREEN, "[----------] ");
-  printf("%s from %s", counts.c_str(), test_case_name_.c_str());
+  printf("%s from %s", counts.c_str(), test_case.name());
   if (test_case.type_param() == NULL) {
     printf("\n");
   } else {
-    printf(", where TypeParam = %s\n", test_case.type_param());
+    printf(", where %s = %s\n", kTypeParamLabel, test_case.type_param());
   }
   fflush(stdout);
 }
 
 void PrettyUnitTestResultPrinter::OnTestStart(const TestInfo& test_info) {
   ColoredPrintf(COLOR_GREEN,  "[ RUN      ] ");
-  PrintTestName(test_case_name_.c_str(), test_info.name());
+  PrintTestName(test_info.test_case_name(), test_info.name());
   printf("\n");
   fflush(stdout);
 }
@@ -4099,7 +4280,7 @@
   } else {
     ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
   }
-  PrintTestName(test_case_name_.c_str(), test_info.name());
+  PrintTestName(test_info.test_case_name(), test_info.name());
   if (test_info.result()->Failed())
     PrintFullTestCommentIfPresent(test_info);
 
@@ -4115,12 +4296,11 @@
 void PrettyUnitTestResultPrinter::OnTestCaseEnd(const TestCase& test_case) {
   if (!GTEST_FLAG(print_time)) return;
 
-  test_case_name_ = test_case.name();
-  const internal::String counts =
+  const std::string counts =
       FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
   ColoredPrintf(COLOR_GREEN, "[----------] ");
   printf("%s from %s (%s ms total)\n\n",
-         counts.c_str(), test_case_name_.c_str(),
+         counts.c_str(), test_case.name(),
          internal::StreamableToString(test_case.elapsed_time()).c_str());
   fflush(stdout);
 }
@@ -4181,7 +4361,7 @@
                         num_failures == 1 ? "TEST" : "TESTS");
   }
 
-  int num_disabled = unit_test.disabled_test_count();
+  int num_disabled = unit_test.reportable_disabled_test_count();
   if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
     if (!num_failures) {
       printf("\n");  // Add a spacer if no FAILURE banner is displayed.
@@ -4335,18 +4515,27 @@
   // is_attribute is true, the text is meant to appear as an attribute
   // value, and normalizable whitespace is preserved by replacing it
   // with character references.
-  static String EscapeXml(const char* str, bool is_attribute);
+  static std::string EscapeXml(const std::string& str, bool is_attribute);
 
   // Returns the given string with all characters invalid in XML removed.
-  static string RemoveInvalidXmlCharacters(const string& str);
+  static std::string RemoveInvalidXmlCharacters(const std::string& str);
 
   // Convenience wrapper around EscapeXml when str is an attribute value.
-  static String EscapeXmlAttribute(const char* str) {
+  static std::string EscapeXmlAttribute(const std::string& str) {
     return EscapeXml(str, true);
   }
 
   // Convenience wrapper around EscapeXml when str is not an attribute value.
-  static String EscapeXmlText(const char* str) { return EscapeXml(str, false); }
+  static std::string EscapeXmlText(const char* str) {
+    return EscapeXml(str, false);
+  }
+
+  // Verifies that the given attribute belongs to the given element and
+  // streams the attribute as XML.
+  static void OutputXmlAttribute(std::ostream* stream,
+                                 const std::string& element_name,
+                                 const std::string& name,
+                                 const std::string& value);
 
   // Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
   static void OutputXmlCDataSection(::std::ostream* stream, const char* data);
@@ -4357,19 +4546,21 @@
                                 const TestInfo& test_info);
 
   // Prints an XML representation of a TestCase object
-  static void PrintXmlTestCase(FILE* out, const TestCase& test_case);
+  static void PrintXmlTestCase(::std::ostream* stream,
+                               const TestCase& test_case);
 
   // Prints an XML summary of unit_test to output stream out.
-  static void PrintXmlUnitTest(FILE* out, const UnitTest& unit_test);
+  static void PrintXmlUnitTest(::std::ostream* stream,
+                               const UnitTest& unit_test);
 
   // Produces a string representing the test properties in a result as space
   // delimited XML attributes based on the property key="value" pairs.
-  // When the String is not empty, it includes a space at the beginning,
+  // When the std::string is not empty, it includes a space at the beginning,
   // to delimit this attribute from prior attributes.
-  static String TestPropertiesAsXmlAttributes(const TestResult& result);
+  static std::string TestPropertiesAsXmlAttributes(const TestResult& result);
 
   // The output file.
-  const String output_file_;
+  const std::string output_file_;
 
   GTEST_DISALLOW_COPY_AND_ASSIGN_(XmlUnitTestResultPrinter);
 };
@@ -4411,7 +4602,9 @@
     fflush(stderr);
     exit(EXIT_FAILURE);
   }
-  PrintXmlUnitTest(xmlout, unit_test);
+  std::stringstream stream;
+  PrintXmlUnitTest(&stream, unit_test);
+  fprintf(xmlout, "%s", StringStreamToString(&stream).c_str());
   fclose(xmlout);
 }
 
@@ -4427,42 +4620,43 @@
 // most invalid characters can be retained using character references.
 // TODO(wan): It might be nice to have a minimally invasive, human-readable
 // escaping scheme for invalid characters, rather than dropping them.
-String XmlUnitTestResultPrinter::EscapeXml(const char* str, bool is_attribute) {
+std::string XmlUnitTestResultPrinter::EscapeXml(
+    const std::string& str, bool is_attribute) {
   Message m;
 
-  if (str != NULL) {
-    for (const char* src = str; *src; ++src) {
-      switch (*src) {
-        case '<':
-          m << "&lt;";
-          break;
-        case '>':
-          m << "&gt;";
-          break;
-        case '&':
-          m << "&amp;";
-          break;
-        case '\'':
-          if (is_attribute)
-            m << "&apos;";
-          else
-            m << '\'';
-          break;
-        case '"':
-          if (is_attribute)
-            m << "&quot;";
+  for (size_t i = 0; i < str.size(); ++i) {
+    const char ch = str[i];
+    switch (ch) {
+      case '<':
+        m << "&lt;";
+        break;
+      case '>':
+        m << "&gt;";
+        break;
+      case '&':
+        m << "&amp;";
+        break;
+      case '\'':
+        if (is_attribute)
+          m << "&apos;";
+        else
+          m << '\'';
+        break;
+      case '"':
+        if (is_attribute)
+          m << "&quot;";
+        else
+          m << '"';
+        break;
+      default:
+        if (IsValidXmlCharacter(ch)) {
+          if (is_attribute && IsNormalizableWhitespace(ch))
+            m << "&#x" << String::FormatByte(static_cast<unsigned char>(ch))
+              << ";";
           else
-            m << '"';
-          break;
-        default:
-          if (IsValidXmlCharacter(*src)) {
-            if (is_attribute && IsNormalizableWhitespace(*src))
-              m << String::Format("&#x%02X;", unsigned(*src));
-            else
-              m << *src;
-          }
-          break;
-      }
+            m << ch;
+        }
+        break;
     }
   }
 
@@ -4472,10 +4666,11 @@
 // Returns the given string with all characters invalid in XML removed.
 // Currently invalid characters are dropped from the string. An
 // alternative is to replace them with certain characters such as . or ?.
-string XmlUnitTestResultPrinter::RemoveInvalidXmlCharacters(const string& str) {
-  string output;
+std::string XmlUnitTestResultPrinter::RemoveInvalidXmlCharacters(
+    const std::string& str) {
+  std::string output;
   output.reserve(str.size());
-  for (string::const_iterator it = str.begin(); it != str.end(); ++it)
+  for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
     if (IsValidXmlCharacter(*it))
       output.push_back(*it);
 
@@ -4505,6 +4700,32 @@
   return ss.str();
 }
 
+// Converts the given epoch time in milliseconds to a date string in the ISO
+// 8601 format, without the timezone information.
+std::string FormatEpochTimeInMillisAsIso8601(TimeInMillis ms) {
+  // Using non-reentrant version as localtime_r is not portable.
+  time_t seconds = static_cast<time_t>(ms / 1000);
+#ifdef _MSC_VER
+# pragma warning(push)          // Saves the current warning state.
+# pragma warning(disable:4996)  // Temporarily disables warning 4996
+                                // (function or variable may be unsafe).
+  const struct tm* const time_struct = localtime(&seconds);  // NOLINT
+# pragma warning(pop)           // Restores the warning state again.
+#else
+  const struct tm* const time_struct = localtime(&seconds);  // NOLINT
+#endif
+  if (time_struct == NULL)
+    return "";  // Invalid ms value
+
+  // YYYY-MM-DDThh:mm:ss
+  return StreamableToString(time_struct->tm_year + 1900) + "-" +
+      String::FormatIntWidth2(time_struct->tm_mon + 1) + "-" +
+      String::FormatIntWidth2(time_struct->tm_mday) + "T" +
+      String::FormatIntWidth2(time_struct->tm_hour) + ":" +
+      String::FormatIntWidth2(time_struct->tm_min) + ":" +
+      String::FormatIntWidth2(time_struct->tm_sec);
+}
+
 // Streams an XML CDATA section, escaping invalid CDATA sequences as needed.
 void XmlUnitTestResultPrinter::OutputXmlCDataSection(::std::ostream* stream,
                                                      const char* data) {
@@ -4525,45 +4746,63 @@
   *stream << "]]>";
 }
 
+void XmlUnitTestResultPrinter::OutputXmlAttribute(
+    std::ostream* stream,
+    const std::string& element_name,
+    const std::string& name,
+    const std::string& value) {
+  const std::vector<std::string>& allowed_names =
+      GetReservedAttributesForElement(element_name);
+
+  GTEST_CHECK_(std::find(allowed_names.begin(), allowed_names.end(), name) !=
+                   allowed_names.end())
+      << "Attribute " << name << " is not allowed for element <" << element_name
+      << ">.";
+
+  *stream << " " << name << "=\"" << EscapeXmlAttribute(value) << "\"";
+}
+
 // Prints an XML representation of a TestInfo object.
 // TODO(wan): There is also value in printing properties with the plain printer.
 void XmlUnitTestResultPrinter::OutputXmlTestInfo(::std::ostream* stream,
                                                  const char* test_case_name,
                                                  const TestInfo& test_info) {
   const TestResult& result = *test_info.result();
-  *stream << "    <testcase name=\""
-          << EscapeXmlAttribute(test_info.name()).c_str() << "\"";
+  const std::string kTestcase = "testcase";
+
+  *stream << "    <testcase";
+  OutputXmlAttribute(stream, kTestcase, "name", test_info.name());
 
   if (test_info.value_param() != NULL) {
-    *stream << " value_param=\"" << EscapeXmlAttribute(test_info.value_param())
-            << "\"";
+    OutputXmlAttribute(stream, kTestcase, "value_param",
+                       test_info.value_param());
   }
   if (test_info.type_param() != NULL) {
-    *stream << " type_param=\"" << EscapeXmlAttribute(test_info.type_param())
-            << "\"";
+    OutputXmlAttribute(stream, kTestcase, "type_param", test_info.type_param());
   }
 
-  *stream << " status=\""
-          << (test_info.should_run() ? "run" : "notrun")
-          << "\" time=\""
-          << FormatTimeInMillisAsSeconds(result.elapsed_time())
-          << "\" classname=\"" << EscapeXmlAttribute(test_case_name).c_str()
-          << "\"" << TestPropertiesAsXmlAttributes(result).c_str();
+  OutputXmlAttribute(stream, kTestcase, "status",
+                     test_info.should_run() ? "run" : "notrun");
+  OutputXmlAttribute(stream, kTestcase, "time",
+                     FormatTimeInMillisAsSeconds(result.elapsed_time()));
+  OutputXmlAttribute(stream, kTestcase, "classname", test_case_name);
+  *stream << TestPropertiesAsXmlAttributes(result);
 
   int failures = 0;
   for (int i = 0; i < result.total_part_count(); ++i) {
     const TestPartResult& part = result.GetTestPartResult(i);
     if (part.failed()) {
-      if (++failures == 1)
+      if (++failures == 1) {
         *stream << ">\n";
-      *stream << "      <failure message=\""
-              << EscapeXmlAttribute(part.summary()).c_str()
-              << "\" type=\"\">";
+      }
       const string location = internal::FormatCompilerIndependentFileLocation(
           part.file_name(), part.line_number());
-      const string message = location + "\n" + part.message();
-      OutputXmlCDataSection(stream,
-                            RemoveInvalidXmlCharacters(message).c_str());
+      const string summary = location + "\n" + part.summary();
+      *stream << "      <failure message=\""
+              << EscapeXmlAttribute(summary.c_str())
+              << "\" type=\"\">";
+      const string detail = location + "\n" + part.message();
+      OutputXmlCDataSection(stream, RemoveInvalidXmlCharacters(detail).c_str());
       *stream << "</failure>\n";
     }
   }
@@ -4575,49 +4814,73 @@
 }
 
 // Prints an XML representation of a TestCase object
-void XmlUnitTestResultPrinter::PrintXmlTestCase(FILE* out,
+void XmlUnitTestResultPrinter::PrintXmlTestCase(std::ostream* stream,
                                                 const TestCase& test_case) {
-  fprintf(out,
-          "  <testsuite name=\"%s\" tests=\"%d\" failures=\"%d\" "
-          "disabled=\"%d\" ",
-          EscapeXmlAttribute(test_case.name()).c_str(),
-          test_case.total_test_count(),
-          test_case.failed_test_count(),
-          test_case.disabled_test_count());
-  fprintf(out,
-          "errors=\"0\" time=\"%s\">\n",
-          FormatTimeInMillisAsSeconds(test_case.elapsed_time()).c_str());
+  const std::string kTestsuite = "testsuite";
+  *stream << "  <" << kTestsuite;
+  OutputXmlAttribute(stream, kTestsuite, "name", test_case.name());
+  OutputXmlAttribute(stream, kTestsuite, "tests",
+                     StreamableToString(test_case.reportable_test_count()));
+  OutputXmlAttribute(stream, kTestsuite, "failures",
+                     StreamableToString(test_case.failed_test_count()));
+  OutputXmlAttribute(
+      stream, kTestsuite, "disabled",
+      StreamableToString(test_case.reportable_disabled_test_count()));
+  OutputXmlAttribute(stream, kTestsuite, "errors", "0");
+  OutputXmlAttribute(stream, kTestsuite, "time",
+                     FormatTimeInMillisAsSeconds(test_case.elapsed_time()));
+  *stream << TestPropertiesAsXmlAttributes(test_case.ad_hoc_test_result())
+          << ">\n";
+
   for (int i = 0; i < test_case.total_test_count(); ++i) {
-    ::std::stringstream stream;
-    OutputXmlTestInfo(&stream, test_case.name(), *test_case.GetTestInfo(i));
-    fprintf(out, "%s", StringStreamToString(&stream).c_str());
+    if (test_case.GetTestInfo(i)->is_reportable())
+      OutputXmlTestInfo(stream, test_case.name(), *test_case.GetTestInfo(i));
   }
-  fprintf(out, "  </testsuite>\n");
+  *stream << "  </" << kTestsuite << ">\n";
 }
 
 // Prints an XML summary of unit_test to output stream out.
-void XmlUnitTestResultPrinter::PrintXmlUnitTest(FILE* out,
+void XmlUnitTestResultPrinter::PrintXmlUnitTest(std::ostream* stream,
                                                 const UnitTest& unit_test) {
-  fprintf(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
-  fprintf(out,
-          "<testsuites tests=\"%d\" failures=\"%d\" disabled=\"%d\" "
-          "errors=\"0\" time=\"%s\" ",
-          unit_test.total_test_count(),
-          unit_test.failed_test_count(),
-          unit_test.disabled_test_count(),
-          FormatTimeInMillisAsSeconds(unit_test.elapsed_time()).c_str());
+  const std::string kTestsuites = "testsuites";
+
+  *stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
+  *stream << "<" << kTestsuites;
+
+  OutputXmlAttribute(stream, kTestsuites, "tests",
+                     StreamableToString(unit_test.reportable_test_count()));
+  OutputXmlAttribute(stream, kTestsuites, "failures",
+                     StreamableToString(unit_test.failed_test_count()));
+  OutputXmlAttribute(
+      stream, kTestsuites, "disabled",
+      StreamableToString(unit_test.reportable_disabled_test_count()));
+  OutputXmlAttribute(stream, kTestsuites, "errors", "0");
+  OutputXmlAttribute(
+      stream, kTestsuites, "timestamp",
+      FormatEpochTimeInMillisAsIso8601(unit_test.start_timestamp()));
+  OutputXmlAttribute(stream, kTestsuites, "time",
+                     FormatTimeInMillisAsSeconds(unit_test.elapsed_time()));
+
   if (GTEST_FLAG(shuffle)) {
-    fprintf(out, "random_seed=\"%d\" ", unit_test.random_seed());
+    OutputXmlAttribute(stream, kTestsuites, "random_seed",
+                       StreamableToString(unit_test.random_seed()));
   }
-  fprintf(out, "name=\"AllTests\">\n");
-  for (int i = 0; i < unit_test.total_test_case_count(); ++i)
-    PrintXmlTestCase(out, *unit_test.GetTestCase(i));
-  fprintf(out, "</testsuites>\n");
+
+  *stream << TestPropertiesAsXmlAttributes(unit_test.ad_hoc_test_result());
+
+  OutputXmlAttribute(stream, kTestsuites, "name", "AllTests");
+  *stream << ">\n";
+
+  for (int i = 0; i < unit_test.total_test_case_count(); ++i) {
+    if (unit_test.GetTestCase(i)->reportable_test_count() > 0)
+      PrintXmlTestCase(stream, *unit_test.GetTestCase(i));
+  }
+  *stream << "</" << kTestsuites << ">\n";
 }
 
 // Produces a string representing the test properties in a result as space
 // delimited XML attributes based on the property key="value" pairs.
-String XmlUnitTestResultPrinter::TestPropertiesAsXmlAttributes(
+std::string XmlUnitTestResultPrinter::TestPropertiesAsXmlAttributes(
     const TestResult& result) {
   Message attributes;
   for (int i = 0; i < result.test_property_count(); ++i) {
@@ -4632,112 +4895,6 @@
 
 #if GTEST_CAN_STREAM_RESULTS_
 
-// Streams test results to the given port on the given host machine.
-class StreamingListener : public EmptyTestEventListener {
- public:
-  // Escapes '=', '&', '%', and '\n' characters in str as "%xx".
-  static string UrlEncode(const char* str);
-
-  StreamingListener(const string& host, const string& port)
-      : sockfd_(-1), host_name_(host), port_num_(port) {
-    MakeConnection();
-    Send("gtest_streaming_protocol_version=1.0\n");
-  }
-
-  virtual ~StreamingListener() {
-    if (sockfd_ != -1)
-      CloseConnection();
-  }
-
-  void OnTestProgramStart(const UnitTest& /* unit_test */) {
-    Send("event=TestProgramStart\n");
-  }
-
-  void OnTestProgramEnd(const UnitTest& unit_test) {
-    // Note that Google Test current only report elapsed time for each
-    // test iteration, not for the entire test program.
-    Send(String::Format("event=TestProgramEnd&passed=%d\n",
-                        unit_test.Passed()));
-
-    // Notify the streaming server to stop.
-    CloseConnection();
-  }
-
-  void OnTestIterationStart(const UnitTest& /* unit_test */, int iteration) {
-    Send(String::Format("event=TestIterationStart&iteration=%d\n",
-                        iteration));
-  }
-
-  void OnTestIterationEnd(const UnitTest& unit_test, int /* iteration */) {
-    Send(String::Format("event=TestIterationEnd&passed=%d&elapsed_time=%sms\n",
-                        unit_test.Passed(),
-                        StreamableToString(unit_test.elapsed_time()).c_str()));
-  }
-
-  void OnTestCaseStart(const TestCase& test_case) {
-    Send(String::Format("event=TestCaseStart&name=%s\n", test_case.name()));
-  }
-
-  void OnTestCaseEnd(const TestCase& test_case) {
-    Send(String::Format("event=TestCaseEnd&passed=%d&elapsed_time=%sms\n",
-                        test_case.Passed(),
-                        StreamableToString(test_case.elapsed_time()).c_str()));
-  }
-
-  void OnTestStart(const TestInfo& test_info) {
-    Send(String::Format("event=TestStart&name=%s\n", test_info.name()));
-  }
-
-  void OnTestEnd(const TestInfo& test_info) {
-    Send(String::Format(
-        "event=TestEnd&passed=%d&elapsed_time=%sms\n",
-        (test_info.result())->Passed(),
-        StreamableToString((test_info.result())->elapsed_time()).c_str()));
-  }
-
-  void OnTestPartResult(const TestPartResult& test_part_result) {
-    const char* file_name = test_part_result.file_name();
-    if (file_name == NULL)
-      file_name = "";
-    Send(String::Format("event=TestPartResult&file=%s&line=%d&message=",
-                        UrlEncode(file_name).c_str(),
-                        test_part_result.line_number()));
-    Send(UrlEncode(test_part_result.message()) + "\n");
-  }
-
- private:
-  // Creates a client socket and connects to the server.
-  void MakeConnection();
-
-  // Closes the socket.
-  void CloseConnection() {
-    GTEST_CHECK_(sockfd_ != -1)
-        << "CloseConnection() can be called only when there is a connection.";
-
-    close(sockfd_);
-    sockfd_ = -1;
-  }
-
-  // Sends a string to the socket.
-  void Send(const string& message) {
-    GTEST_CHECK_(sockfd_ != -1)
-        << "Send() can be called only when there is a connection.";
-
-    const int len = static_cast<int>(message.length());
-    if (write(sockfd_, message.c_str(), len) != len) {
-      GTEST_LOG_(WARNING)
-          << "stream_result_to: failed to stream to "
-          << host_name_ << ":" << port_num_;
-    }
-  }
-
-  int sockfd_;   // socket file descriptor
-  const string host_name_;
-  const string port_num_;
-
-  GTEST_DISALLOW_COPY_AND_ASSIGN_(StreamingListener);
-};  // class StreamingListener
-
 // Checks if str contains '=', '&', '%' or '\n' characters. If yes,
 // replaces them by "%xx" where xx is their hexadecimal value. For
 // example, replaces "=" with "%3D".  This algorithm is O(strlen(str))
@@ -4752,7 +4909,7 @@
       case '=':
       case '&':
       case '\n':
-        result.append(String::Format("%%%02x", static_cast<unsigned char>(ch)));
+        result.append("%" + String::FormatByte(static_cast<unsigned char>(ch)));
         break;
       default:
         result.push_back(ch);
@@ -4762,7 +4919,7 @@
   return result;
 }
 
-void StreamingListener::MakeConnection() {
+void StreamingListener::SocketWriter::MakeConnection() {
   GTEST_CHECK_(sockfd_ == -1)
       << "MakeConnection() can't be called when there is already a connection.";
 
@@ -4810,8 +4967,8 @@
 
 // Pushes the given source file location and message onto a per-thread
 // trace stack maintained by Google Test.
-// L < UnitTest::mutex_
-ScopedTrace::ScopedTrace(const char* file, int line, const Message& message) {
+ScopedTrace::ScopedTrace(const char* file, int line, const Message& message)
+    GTEST_LOCK_EXCLUDED_(&UnitTest::mutex_) {
   TraceInfo trace;
   trace.file = file;
   trace.line = line;
@@ -4821,35 +4978,64 @@
 }
 
 // Pops the info pushed by the c'tor.
-// L < UnitTest::mutex_
-ScopedTrace::~ScopedTrace() {
+ScopedTrace::~ScopedTrace()
+    GTEST_LOCK_EXCLUDED_(&UnitTest::mutex_) {
   UnitTest::GetInstance()->PopGTestTrace();
 }
 
 
 // class OsStackTraceGetter
 
-// Returns the current OS stack trace as a String.  Parameters:
+// Returns the current OS stack trace as an std::string.  Parameters:
 //
 //   max_depth  - the maximum number of stack frames to be included
 //                in the trace.
 //   skip_count - the number of top frames to be skipped; doesn't count
 //                against max_depth.
 //
-// L < mutex_
-// We use "L < mutex_" to denote that the function may acquire mutex_.
-String OsStackTraceGetter::CurrentStackTrace(int, int) {
-  return String("");
+string OsStackTraceGetter::CurrentStackTrace(int /* max_depth */,
+                                             int /* skip_count */)
+    GTEST_LOCK_EXCLUDED_(mutex_) {
+  return "";
 }
 
-// L < mutex_
-void OsStackTraceGetter::UponLeavingGTest() {
+void OsStackTraceGetter::UponLeavingGTest()
+    GTEST_LOCK_EXCLUDED_(mutex_) {
 }
 
 const char* const
 OsStackTraceGetter::kElidedFramesMarker =
     "... " GTEST_NAME_ " internal frames ...";
 
+// A helper class that creates the premature-exit file in its
+// constructor and deletes the file in its destructor.
+class ScopedPrematureExitFile {
+ public:
+  explicit ScopedPrematureExitFile(const char* premature_exit_filepath)
+      : premature_exit_filepath_(premature_exit_filepath) {
+    // If a path to the premature-exit file is specified...
+    if (premature_exit_filepath != NULL && *premature_exit_filepath != '\0') {
+      // create the file with a single "0" character in it.  I/O
+      // errors are ignored as there's nothing better we can do and we
+      // don't want to fail the test because of this.
+      FILE* pfile = posix::FOpen(premature_exit_filepath, "w");
+      fwrite("0", 1, 1, pfile);
+      fclose(pfile);
+    }
+  }
+
+  ~ScopedPrematureExitFile() {
+    if (premature_exit_filepath_ != NULL && *premature_exit_filepath_ != '\0') {
+      remove(premature_exit_filepath_);
+    }
+  }
+
+ private:
+  const char* const premature_exit_filepath_;
+
+  GTEST_DISALLOW_COPY_AND_ASSIGN_(ScopedPrematureExitFile);
+};
+
 }  // namespace internal
 
 // class TestEventListeners
@@ -4936,7 +5122,7 @@
 // We don't protect this under mutex_ as a user is not supposed to
 // call this before main() starts, from which point on the return
 // value will never change.
-UnitTest * UnitTest::GetInstance() {
+UnitTest* UnitTest::GetInstance() {
   // When compiled with MSVC 7.1 in optimized mode, destroying the
   // UnitTest object upon exiting the program messes up the exit code,
   // causing successful tests to appear failed.  We have to use a
@@ -4986,17 +5172,33 @@
 // Gets the number of failed tests.
 int UnitTest::failed_test_count() const { return impl()->failed_test_count(); }
 
+// Gets the number of disabled tests that will be reported in the XML report.
+int UnitTest::reportable_disabled_test_count() const {
+  return impl()->reportable_disabled_test_count();
+}
+
 // Gets the number of disabled tests.
 int UnitTest::disabled_test_count() const {
   return impl()->disabled_test_count();
 }
 
+// Gets the number of tests to be printed in the XML report.
+int UnitTest::reportable_test_count() const {
+  return impl()->reportable_test_count();
+}
+
 // Gets the number of all tests.
 int UnitTest::total_test_count() const { return impl()->total_test_count(); }
 
 // Gets the number of tests that should run.
 int UnitTest::test_to_run_count() const { return impl()->test_to_run_count(); }
 
+// Gets the time of the test program start, in ms from the start of the
+// UNIX epoch.
+internal::TimeInMillis UnitTest::start_timestamp() const {
+    return impl()->start_timestamp();
+}
+
 // Gets the elapsed time, in milliseconds.
 internal::TimeInMillis UnitTest::elapsed_time() const {
   return impl()->elapsed_time();
@@ -5015,6 +5217,12 @@
   return impl()->GetTestCase(i);
 }
 
+// Returns the TestResult containing information on test failures and
+// properties logged outside of individual test cases.
+const TestResult& UnitTest::ad_hoc_test_result() const {
+  return *impl()->ad_hoc_test_result();
+}
+
 // Gets the i-th test case among all the test cases. i can range from 0 to
 // total_test_case_count() - 1. If i is not in that range, returns NULL.
 TestCase* UnitTest::GetMutableTestCase(int i) {
@@ -5050,12 +5258,12 @@
 // assertion macros (e.g. ASSERT_TRUE, EXPECT_EQ, etc) eventually call
 // this to report their results.  The user code should use the
 // assertion macros instead of calling this directly.
-// L < mutex_
-void UnitTest::AddTestPartResult(TestPartResult::Type result_type,
-                                 const char* file_name,
-                                 int line_number,
-                                 const internal::String& message,
-                                 const internal::String& os_stack_trace) {
+void UnitTest::AddTestPartResult(
+    TestPartResult::Type result_type,
+    const char* file_name,
+    int line_number,
+    const std::string& message,
+    const std::string& os_stack_trace) GTEST_LOCK_EXCLUDED_(mutex_) {
   Message msg;
   msg << message;
 
@@ -5102,7 +5310,7 @@
 #endif  // GTEST_OS_WINDOWS
     } else if (GTEST_FLAG(throw_on_failure)) {
 #if GTEST_HAS_EXCEPTIONS
-      throw GoogleTestFailureException(result);
+      throw internal::GoogleTestFailureException(result);
 #else
       // We cannot call abort() as it generates a pop-up in debug mode
       // that cannot be suppressed in VC 7.1 or below.
@@ -5112,12 +5320,14 @@
   }
 }
 
-// Creates and adds a property to the current TestResult. If a property matching
-// the supplied value already exists, updates its value instead.
-void UnitTest::RecordPropertyForCurrentTest(const char* key,
-                                            const char* value) {
-  const TestProperty test_property(key, value);
-  impl_->current_test_result()->RecordProperty(test_property);
+// Adds a TestProperty to the current TestResult object when invoked from
+// inside a test, to current TestCase's ad_hoc_test_result_ when invoked
+// from SetUpTestCase or TearDownTestCase, or to the global property set
+// when invoked elsewhere.  If the result already contains a property with
+// the same key, the value will be updated.
+void UnitTest::RecordProperty(const std::string& key,
+                              const std::string& value) {
+  impl_->RecordProperty(TestProperty(key, value));
 }
 
 // Runs all tests in this UnitTest object and prints the result.
@@ -5126,20 +5336,44 @@
 // We don't protect this under mutex_, as we only support calling it
 // from the main thread.
 int UnitTest::Run() {
+  const bool in_death_test_child_process =
+      internal::GTEST_FLAG(internal_run_death_test).length() > 0;
+
+  // Google Test implements this protocol for catching that a test
+  // program exits before returning control to Google Test:
+  //
+  //   1. Upon start, Google Test creates a file whose absolute path
+  //      is specified by the environment variable
+  //      TEST_PREMATURE_EXIT_FILE.
+  //   2. When Google Test has finished its work, it deletes the file.
+  //
+  // This allows a test runner to set TEST_PREMATURE_EXIT_FILE before
+  // running a Google-Test-based test program and check the existence
+  // of the file at the end of the test execution to see if it has
+  // exited prematurely.
+
+  // If we are in the child process of a death test, don't
+  // create/delete the premature exit file, as doing so is unnecessary
+  // and will confuse the parent process.  Otherwise, create/delete
+  // the file upon entering/leaving this function.  If the program
+  // somehow exits before this function has a chance to return, the
+  // premature-exit file will be left undeleted, causing a test runner
+  // that understands the premature-exit-file protocol to report the
+  // test as having failed.
+  const internal::ScopedPrematureExitFile premature_exit_file(
+      in_death_test_child_process ?
+      NULL : internal::posix::GetEnv("TEST_PREMATURE_EXIT_FILE"));
+
   // Captures the value of GTEST_FLAG(catch_exceptions).  This value will be
   // used for the duration of the program.
   impl()->set_catch_exceptions(GTEST_FLAG(catch_exceptions));
 
 #if GTEST_HAS_SEH
-  const bool in_death_test_child_process =
-      internal::GTEST_FLAG(internal_run_death_test).length() > 0;
-
   // Either the user wants Google Test to catch exceptions thrown by the
   // tests or this is executing in the context of death test child
   // process. In either case the user does not want to see pop-up dialogs
   // about crashes - they are expected.
   if (impl()->catch_exceptions() || in_death_test_child_process) {
-
 # if !GTEST_OS_WINDOWS_MOBILE
     // SetErrorMode doesn't exist on CE.
     SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT |
@@ -5170,7 +5404,6 @@
           0x0,                                    // Clear the following flags:
           _WRITE_ABORT_MSG | _CALL_REPORTFAULT);  // pop-up window, core dump.
 # endif
-
   }
 #endif  // GTEST_HAS_SEH
 
@@ -5188,16 +5421,16 @@
 
 // Returns the TestCase object for the test that's currently running,
 // or NULL if no test is running.
-// L < mutex_
-const TestCase* UnitTest::current_test_case() const {
+const TestCase* UnitTest::current_test_case() const
+    GTEST_LOCK_EXCLUDED_(mutex_) {
   internal::MutexLock lock(&mutex_);
   return impl_->current_test_case();
 }
 
 // Returns the TestInfo object for the test that's currently running,
 // or NULL if no test is running.
-// L < mutex_
-const TestInfo* UnitTest::current_test_info() const {
+const TestInfo* UnitTest::current_test_info() const
+    GTEST_LOCK_EXCLUDED_(mutex_) {
   internal::MutexLock lock(&mutex_);
   return impl_->current_test_info();
 }
@@ -5208,9 +5441,9 @@
 #if GTEST_HAS_PARAM_TEST
 // Returns ParameterizedTestCaseRegistry object used to keep track of
 // value-parameterized tests and instantiate and register them.
-// L < mutex_
 internal::ParameterizedTestCaseRegistry&
-    UnitTest::parameterized_test_registry() {
+    UnitTest::parameterized_test_registry()
+        GTEST_LOCK_EXCLUDED_(mutex_) {
   return impl_->parameterized_test_registry();
 }
 #endif  // GTEST_HAS_PARAM_TEST
@@ -5227,15 +5460,15 @@
 
 // Pushes a trace defined by SCOPED_TRACE() on to the per-thread
 // Google Test trace stack.
-// L < mutex_
-void UnitTest::PushGTestTrace(const internal::TraceInfo& trace) {
+void UnitTest::PushGTestTrace(const internal::TraceInfo& trace)
+    GTEST_LOCK_EXCLUDED_(mutex_) {
   internal::MutexLock lock(&mutex_);
   impl_->gtest_trace_stack().push_back(trace);
 }
 
 // Pops a trace from the per-thread Google Test trace stack.
-// L < mutex_
-void UnitTest::PopGTestTrace() {
+void UnitTest::PopGTestTrace()
+    GTEST_LOCK_EXCLUDED_(mutex_) {
   internal::MutexLock lock(&mutex_);
   impl_->gtest_trace_stack().pop_back();
 }
@@ -5271,9 +5504,9 @@
       post_flag_parse_init_performed_(false),
       random_seed_(0),  // Will be overridden by the flag before first use.
       random_(0),  // Will be reseeded before first use.
+      start_timestamp_(0),
       elapsed_time_(0),
 #if GTEST_HAS_DEATH_TEST
-      internal_run_death_test_flag_(NULL),
       death_test_factory_(new DefaultDeathTestFactory),
 #endif
       // Will be overridden by the flag before first use.
@@ -5291,6 +5524,28 @@
   delete os_stack_trace_getter_;
 }
 
+// Adds a TestProperty to the current TestResult object when invoked in a
+// context of a test, to current test case's ad_hoc_test_result when invoke
+// from SetUpTestCase/TearDownTestCase, or to the global property set
+// otherwise.  If the result already contains a property with the same key,
+// the value will be updated.
+void UnitTestImpl::RecordProperty(const TestProperty& test_property) {
+  std::string xml_element;
+  TestResult* test_result;  // TestResult appropriate for property recording.
+
+  if (current_test_info_ != NULL) {
+    xml_element = "testcase";
+    test_result = &(current_test_info_->result_);
+  } else if (current_test_case_ != NULL) {
+    xml_element = "testsuite";
+    test_result = &(current_test_case_->ad_hoc_test_result_);
+  } else {
+    xml_element = "testsuites";
+    test_result = &ad_hoc_test_result_;
+  }
+  test_result->RecordProperty(xml_element, test_property);
+}
+
 #if GTEST_HAS_DEATH_TEST
 // Disables event forwarding if the control is currently in a death test
 // subprocess. Must not be called before InitGoogleTest.
@@ -5303,7 +5558,7 @@
 // Initializes event listeners performing XML output as specified by
 // UnitTestOptions. Must not be called before InitGoogleTest.
 void UnitTestImpl::ConfigureXmlOutput() {
-  const String& output_format = UnitTestOptions::GetOutputFormat();
+  const std::string& output_format = UnitTestOptions::GetOutputFormat();
   if (output_format == "xml") {
     listeners()->SetDefaultXmlGenerator(new XmlUnitTestResultPrinter(
         UnitTestOptions::GetAbsolutePathToOutputFile().c_str()));
@@ -5315,13 +5570,13 @@
 }
 
 #if GTEST_CAN_STREAM_RESULTS_
-// Initializes event listeners for streaming test results in String form.
+// Initializes event listeners for streaming test results in string form.
 // Must not be called before InitGoogleTest.
 void UnitTestImpl::ConfigureStreamingOutput() {
-  const string& target = GTEST_FLAG(stream_result_to);
+  const std::string& target = GTEST_FLAG(stream_result_to);
   if (!target.empty()) {
     const size_t pos = target.find(':');
-    if (pos != string::npos) {
+    if (pos != std::string::npos) {
       listeners()->Append(new StreamingListener(target.substr(0, pos),
                                                 target.substr(pos+1)));
     } else {
@@ -5375,7 +5630,7 @@
 class TestCaseNameIs {
  public:
   // Constructor.
-  explicit TestCaseNameIs(const String& name)
+  explicit TestCaseNameIs(const std::string& name)
       : name_(name) {}
 
   // Returns true iff the name of test_case matches name_.
@@ -5384,7 +5639,7 @@
   }
 
  private:
-  String name_;
+  std::string name_;
 };
 
 // Finds and returns a TestCase with the given name.  If one doesn't
@@ -5416,7 +5671,7 @@
       new TestCase(test_case_name, type_param, set_up_tc, tear_down_tc);
 
   // Is this a death test case?
-  if (internal::UnitTestOptions::MatchesFilter(String(test_case_name),
+  if (internal::UnitTestOptions::MatchesFilter(test_case_name,
                                                kDeathTestCaseFilter)) {
     // Yes.  Inserts the test case after the last death test case
     // defined so far.  This only works when the test cases haven't
@@ -5502,6 +5757,7 @@
 
   TestEventListener* repeater = listeners()->repeater();
 
+  start_timestamp_ = GetTimeInMillis();
   repeater->OnTestProgramStart(*parent_);
 
   // How many times to repeat the tests?  We don't want to repeat them
@@ -5694,12 +5950,12 @@
   int num_selected_tests = 0;
   for (size_t i = 0; i < test_cases_.size(); i++) {
     TestCase* const test_case = test_cases_[i];
-    const String &test_case_name = test_case->name();
+    const std::string &test_case_name = test_case->name();
     test_case->set_should_run(false);
 
     for (size_t j = 0; j < test_case->test_info_list().size(); j++) {
       TestInfo* const test_info = test_case->test_info_list()[j];
-      const String test_name(test_info->name());
+      const std::string test_name(test_info->name());
       // A test is disabled if test case name or test name matches
       // kDisableTestFilter.
       const bool is_disabled =
@@ -5733,8 +5989,33 @@
   return num_selected_tests;
 }
 
+// Prints the given C-string on a single line by replacing all '\n'
+// characters with string "\\n".  If the output takes more than
+// max_length characters, only prints the first max_length characters
+// and "...".
+static void PrintOnOneLine(const char* str, int max_length) {
+  if (str != NULL) {
+    for (int i = 0; *str != '\0'; ++str) {
+      if (i >= max_length) {
+        printf("...");
+        break;
+      }
+      if (*str == '\n') {
+        printf("\\n");
+        i += 2;
+      } else {
+        printf("%c", *str);
+        ++i;
+      }
+    }
+  }
+}
+
 // Prints the names of the tests matching the user-specified filter flag.
 void UnitTestImpl::ListTestsMatchingFilter() {
+  // Print at most this many characters for each type/value parameter.
+  const int kMaxParamLength = 250;
+
   for (size_t i = 0; i < test_cases_.size(); i++) {
     const TestCase* const test_case = test_cases_[i];
     bool printed_test_case_name = false;
@@ -5745,9 +6026,23 @@
       if (test_info->matches_filter_) {
         if (!printed_test_case_name) {
           printed_test_case_name = true;
-          printf("%s.\n", test_case->name());
+          printf("%s.", test_case->name());
+          if (test_case->type_param() != NULL) {
+            printf("  # %s = ", kTypeParamLabel);
+            // We print the type parameter on a single line to make
+            // the output easy to parse by a program.
+            PrintOnOneLine(test_case->type_param(), kMaxParamLength);
+          }
+          printf("\n");
         }
-        printf("  %s\n", test_info->name());
+        printf("  %s", test_info->name());
+        if (test_info->value_param() != NULL) {
+          printf("  # %s = ", kValueParamLabel);
+          // We print the value parameter on a single line to make the
+          // output easy to parse by a program.
+          PrintOnOneLine(test_info->value_param(), kMaxParamLength);
+        }
+        printf("\n");
       }
     }
   }
@@ -5811,7 +6106,7 @@
   }
 }
 
-// Returns the current OS stack trace as a String.
+// Returns the current OS stack trace as an std::string.
 //
 // The maximum number of stack frames to be included is specified by
 // the gtest_stack_trace_depth flag.  The skip_count parameter
@@ -5821,8 +6116,8 @@
 // For example, if Foo() calls Bar(), which in turn calls
 // GetCurrentOsStackTraceExceptTop(..., 1), Foo() will be included in
 // the trace but Bar() and GetCurrentOsStackTraceExceptTop() won't.
-String GetCurrentOsStackTraceExceptTop(UnitTest* /*unit_test*/,
-                                       int skip_count) {
+std::string GetCurrentOsStackTraceExceptTop(UnitTest* /*unit_test*/,
+                                            int skip_count) {
   // We pass skip_count + 1 to skip this wrapper function in addition
   // to what the user really wants to skip.
   return GetUnitTestImpl()->CurrentOsStackTraceExceptTop(skip_count + 1);
@@ -5870,7 +6165,7 @@
   if (str == NULL || flag == NULL) return NULL;
 
   // The flag must start with "--" followed by GTEST_FLAG_PREFIX_.
-  const String flag_str = String::Format("--%s%s", GTEST_FLAG_PREFIX_, flag);
+  const std::string flag_str = std::string("--") + GTEST_FLAG_PREFIX_ + flag;
   const size_t flag_len = flag_str.length();
   if (strncmp(str, flag_str.c_str(), flag_len) != 0) return NULL;
 
@@ -5935,7 +6230,7 @@
 //
 // On success, stores the value of the flag in *value, and returns
 // true.  On failure, returns false without changing *value.
-bool ParseStringFlag(const char* str, const char* flag, String* value) {
+bool ParseStringFlag(const char* str, const char* flag, std::string* value) {
   // Gets the value of the flag as a string.
   const char* const value_str = ParseFlagValue(str, flag, false);
 
@@ -5987,7 +6282,7 @@
       return;
     }
 
-    ColoredPrintf(color, "%s", String(str, p - str).c_str());
+    ColoredPrintf(color, "%s", std::string(str, p).c_str());
 
     const char ch = p[1];
     str = p + 2;
@@ -6077,7 +6372,7 @@
 template <typename CharType>
 void ParseGoogleTestFlagsOnlyImpl(int* argc, CharType** argv) {
   for (int i = 1; i < *argc; i++) {
-    const String arg_string = StreamableToString(argv[i]);
+    const std::string arg_string = StreamableToString(argv[i]);
     const char* const arg = arg_string.c_str();
 
     using internal::ParseBoolFlag;
@@ -6245,6 +6540,11 @@
 # include <errno.h>
 # include <fcntl.h>
 # include <limits.h>
+
+# if GTEST_OS_LINUX
+#  include <signal.h>
+# endif  // GTEST_OS_LINUX
+
 # include <stdarg.h>
 
 # if GTEST_OS_WINDOWS
@@ -6254,6 +6554,10 @@
 #  include <sys/wait.h>
 # endif  // GTEST_OS_WINDOWS
 
+# if GTEST_OS_QNX
+#  include <spawn.h>
+# endif  // GTEST_OS_QNX
+
 #endif  // GTEST_HAS_DEATH_TEST
 
 
@@ -6299,13 +6603,42 @@
     "Indicates the file, line number, temporal index of "
     "the single death test to run, and a file descriptor to "
     "which a success code may be sent, all separated by "
-    "colons.  This flag is specified if and only if the current "
+    "the '|' characters.  This flag is specified if and only if the current "
     "process is a sub-process launched for running a thread-safe "
     "death test.  FOR INTERNAL USE ONLY.");
 }  // namespace internal
 
 #if GTEST_HAS_DEATH_TEST
 
+namespace internal {
+
+// Valid only for fast death tests. Indicates the code is running in the
+// child process of a fast style death test.
+static bool g_in_fast_death_test_child = false;
+
+// Returns a Boolean value indicating whether the caller is currently
+// executing in the context of the death test child process.  Tools such as
+// Valgrind heap checkers may need this to modify their behavior in death
+// tests.  IMPORTANT: This is an internal utility.  Using it may break the
+// implementation of death tests.  User code MUST NOT use it.
+bool InDeathTestChild() {
+# if GTEST_OS_WINDOWS
+
+  // On Windows, death tests are thread-safe regardless of the value of the
+  // death_test_style flag.
+  return !GTEST_FLAG(internal_run_death_test).empty();
+
+# else
+
+  if (GTEST_FLAG(death_test_style) == "threadsafe")
+    return !GTEST_FLAG(internal_run_death_test).empty();
+  else
+    return g_in_fast_death_test_child;
+#endif
+}
+
+}  // namespace internal
+
 // ExitedWithCode constructor.
 ExitedWithCode::ExitedWithCode(int exit_code) : exit_code_(exit_code) {
 }
@@ -6340,7 +6673,7 @@
 
 // Generates a textual description of a given exit code, in the format
 // specified by wait(2).
-static String ExitSummary(int exit_code) {
+static std::string ExitSummary(int exit_code) {
   Message m;
 
 # if GTEST_OS_WINDOWS
@@ -6375,7 +6708,7 @@
 // one thread running, or cannot determine the number of threads, prior
 // to executing the given statement.  It is the responsibility of the
 // caller not to pass a thread_count of 1.
-static String DeathTestThreadWarning(size_t thread_count) {
+static std::string DeathTestThreadWarning(size_t thread_count) {
   Message msg;
   msg << "Death tests use fork(), which is unsafe particularly"
       << " in a threaded context. For this test, " << GTEST_NAME_ << " ";
@@ -6409,7 +6742,7 @@
 // message is propagated back to the parent process.  Otherwise, the
 // message is simply printed to stderr.  In either case, the program
 // then exits with status 1.
-void DeathTestAbort(const String& message) {
+void DeathTestAbort(const std::string& message) {
   // On a POSIX system, this function may be called from a threadsafe-style
   // death test child process, which operates on a very small stack.  Use
   // the heap for any additional non-minuscule memory requirements.
@@ -6433,9 +6766,10 @@
 # define GTEST_DEATH_TEST_CHECK_(expression) \
   do { \
     if (!::testing::internal::IsTrue(expression)) { \
-      DeathTestAbort(::testing::internal::String::Format( \
-          "CHECK failed: File %s, line %d: %s", \
-          __FILE__, __LINE__, #expression)); \
+      DeathTestAbort( \
+          ::std::string("CHECK failed: File ") + __FILE__ +  ", line " \
+          + ::testing::internal::StreamableToString(__LINE__) + ": " \
+          + #expression); \
     } \
   } while (::testing::internal::AlwaysFalse())
 
@@ -6453,15 +6787,16 @@
       gtest_retval = (expression); \
     } while (gtest_retval == -1 && errno == EINTR); \
     if (gtest_retval == -1) { \
-      DeathTestAbort(::testing::internal::String::Format( \
-          "CHECK failed: File %s, line %d: %s != -1", \
-          __FILE__, __LINE__, #expression)); \
+      DeathTestAbort( \
+          ::std::string("CHECK failed: File ") + __FILE__ + ", line " \
+          + ::testing::internal::StreamableToString(__LINE__) + ": " \
+          + #expression + " != -1"); \
     } \
   } while (::testing::internal::AlwaysFalse())
 
 // Returns the message describing the last system error in errno.
-String GetLastErrnoDescription() {
-    return String(errno == 0 ? "" : posix::StrError(errno));
+std::string GetLastErrnoDescription() {
+    return errno == 0 ? "" : posix::StrError(errno);
 }
 
 // This is called from a death test parent process to read a failure
@@ -6511,11 +6846,11 @@
   return last_death_test_message_.c_str();
 }
 
-void DeathTest::set_last_death_test_message(const String& message) {
+void DeathTest::set_last_death_test_message(const std::string& message) {
   last_death_test_message_ = message;
 }
 
-String DeathTest::last_death_test_message_;
+std::string DeathTest::last_death_test_message_;
 
 // Provides cross platform implementation for some death functionality.
 class DeathTestImpl : public DeathTest {
@@ -6690,7 +7025,7 @@
   if (!spawned())
     return false;
 
-  const String error_message = GetCapturedStderr();
+  const std::string error_message = GetCapturedStderr();
 
   bool success = false;
   Message buffer;
@@ -6872,22 +7207,19 @@
       FALSE,   // The initial state is non-signalled.
       NULL));  // The even is unnamed.
   GTEST_DEATH_TEST_CHECK_(event_handle_.Get() != NULL);
-  const String filter_flag = String::Format("--%s%s=%s.%s",
-                                            GTEST_FLAG_PREFIX_, kFilterFlag,
-                                            info->test_case_name(),
-                                            info->name());
-  const String internal_flag = String::Format(
-    "--%s%s=%s|%d|%d|%u|%Iu|%Iu",
-      GTEST_FLAG_PREFIX_,
-      kInternalRunDeathTestFlag,
-      file_, line_,
-      death_test_index,
-      static_cast<unsigned int>(::GetCurrentProcessId()),
-      // size_t has the same with as pointers on both 32-bit and 64-bit
+  const std::string filter_flag =
+      std::string("--") + GTEST_FLAG_PREFIX_ + kFilterFlag + "=" +
+      info->test_case_name() + "." + info->name();
+  const std::string internal_flag =
+      std::string("--") + GTEST_FLAG_PREFIX_ + kInternalRunDeathTestFlag +
+      "=" + file_ + "|" + StreamableToString(line_) + "|" +
+      StreamableToString(death_test_index) + "|" +
+      StreamableToString(static_cast<unsigned int>(::GetCurrentProcessId())) +
+      // size_t has the same width as pointers on both 32-bit and 64-bit
       // Windows platforms.
       // See http://msdn.microsoft.com/en-us/library/tcxf1dw6.aspx.
-      reinterpret_cast<size_t>(write_handle),
-      reinterpret_cast<size_t>(event_handle_.Get()));
+      "|" + StreamableToString(reinterpret_cast<size_t>(write_handle)) +
+      "|" + StreamableToString(reinterpret_cast<size_t>(event_handle_.Get()));
 
   char executable_path[_MAX_PATH + 1];  // NOLINT
   GTEST_DEATH_TEST_CHECK_(
@@ -6895,10 +7227,9 @@
                                             executable_path,
                                             _MAX_PATH));
 
-  String command_line = String::Format("%s %s \"%s\"",
-                                       ::GetCommandLineA(),
-                                       filter_flag.c_str(),
-                                       internal_flag.c_str());
+  std::string command_line =
+      std::string(::GetCommandLineA()) + " " + filter_flag + " \"" +
+      internal_flag + "\"";
 
   DeathTest::set_last_death_test_message("");
 
@@ -7015,6 +7346,7 @@
     // Event forwarding to the listeners of event listener API mush be shut
     // down in death test subprocesses.
     GetUnitTestImpl()->listeners()->SuppressEventForwarding();
+    g_in_fast_death_test_child = true;
     return EXECUTE_TEST;
   } else {
     GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
@@ -7034,6 +7366,11 @@
       ForkingDeathTest(a_statement, a_regex), file_(file), line_(line) { }
   virtual TestRole AssumeRole();
  private:
+  static ::std::vector<testing::internal::string>
+  GetArgvsForDeathTestChildProcess() {
+    ::std::vector<testing::internal::string> args = GetInjectableArgvs();
+    return args;
+  }
   // The name of the file in which the death test is located.
   const char* const file_;
   // The line number on which the death test is located.
@@ -7068,6 +7405,7 @@
   char* const* Argv() {
     return &args_[0];
   }
+
  private:
   std::vector<char*> args_;
 };
@@ -7093,6 +7431,7 @@
 inline char** GetEnviron() { return environ; }
 #  endif  // GTEST_OS_MAC
 
+#  if !GTEST_OS_QNX
 // The main function for a threadsafe-style death test child process.
 // This function is called in a clone()-ed process and thus must avoid
 // any potentially unsafe operations like malloc or libc functions.
@@ -7107,9 +7446,8 @@
       UnitTest::GetInstance()->original_working_dir();
   // We can safely call chdir() as it's a direct system call.
   if (chdir(original_dir) != 0) {
-    DeathTestAbort(String::Format("chdir(\"%s\") failed: %s",
-                                  original_dir,
-                                  GetLastErrnoDescription().c_str()));
+    DeathTestAbort(std::string("chdir(\"") + original_dir + "\") failed: " +
+                   GetLastErrnoDescription());
     return EXIT_FAILURE;
   }
 
@@ -7119,12 +7457,12 @@
   // invoke the test program via a valid path that contains at least
   // one path separator.
   execve(args->argv[0], args->argv, GetEnviron());
-  DeathTestAbort(String::Format("execve(%s, ...) in %s failed: %s",
-                                args->argv[0],
-                                original_dir,
-                                GetLastErrnoDescription().c_str()));
+  DeathTestAbort(std::string("execve(") + args->argv[0] + ", ...) in " +
+                 original_dir + " failed: " +
+                 GetLastErrnoDescription());
   return EXIT_FAILURE;
 }
+#  endif  // !GTEST_OS_QNX
 
 // Two utility routines that together determine the direction the stack
 // grows.
@@ -7135,25 +7473,75 @@
 // GTEST_NO_INLINE_ is required to prevent GCC 4.6 from inlining
 // StackLowerThanAddress into StackGrowsDown, which then doesn't give
 // correct answer.
-bool StackLowerThanAddress(const void* ptr) GTEST_NO_INLINE_;
-bool StackLowerThanAddress(const void* ptr) {
+void StackLowerThanAddress(const void* ptr, bool* result) GTEST_NO_INLINE_;
+void StackLowerThanAddress(const void* ptr, bool* result) {
   int dummy;
-  return &dummy < ptr;
+  *result = (&dummy < ptr);
 }
 
 bool StackGrowsDown() {
   int dummy;
-  return StackLowerThanAddress(&dummy);
+  bool result;
+  StackLowerThanAddress(&dummy, &result);
+  return result;
 }
 
-// A threadsafe implementation of fork(2) for threadsafe-style death tests
-// that uses clone(2).  It dies with an error message if anything goes
-// wrong.
-static pid_t ExecDeathTestFork(char* const* argv, int close_fd) {
+// Spawns a child process with the same executable as the current process in
+// a thread-safe manner and instructs it to run the death test.  The
+// implementation uses fork(2) + exec.  On systems where clone(2) is
+// available, it is used instead, being slightly more thread-safe.  On QNX,
+// fork supports only single-threaded environments, so this function uses
+// spawn(2) there instead.  The function dies with an error message if
+// anything goes wrong.
+static pid_t ExecDeathTestSpawnChild(char* const* argv, int close_fd) {
   ExecDeathTestArgs args = { argv, close_fd };
   pid_t child_pid = -1;
 
-#  if GTEST_HAS_CLONE
+#  if GTEST_OS_QNX
+  // Obtains the current directory and sets it to be closed in the child
+  // process.
+  const int cwd_fd = open(".", O_RDONLY);
+  GTEST_DEATH_TEST_CHECK_(cwd_fd != -1);
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(fcntl(cwd_fd, F_SETFD, FD_CLOEXEC));
+  // We need to execute the test program in the same environment where
+  // it was originally invoked.  Therefore we change to the original
+  // working directory first.
+  const char* const original_dir =
+      UnitTest::GetInstance()->original_working_dir();
+  // We can safely call chdir() as it's a direct system call.
+  if (chdir(original_dir) != 0) {
+    DeathTestAbort(std::string("chdir(\"") + original_dir + "\") failed: " +
+                   GetLastErrnoDescription());
+    return EXIT_FAILURE;
+  }
+
+  int fd_flags;
+  // Set close_fd to be closed after spawn.
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(fd_flags = fcntl(close_fd, F_GETFD));
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(fcntl(close_fd, F_SETFD,
+                                        fd_flags | FD_CLOEXEC));
+  struct inheritance inherit = {0};
+  // spawn is a system call.
+  child_pid = spawn(args.argv[0], 0, NULL, &inherit, args.argv, GetEnviron());
+  // Restores the current working directory.
+  GTEST_DEATH_TEST_CHECK_(fchdir(cwd_fd) != -1);
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(close(cwd_fd));
+
+#  else   // GTEST_OS_QNX
+#   if GTEST_OS_LINUX
+  // When a SIGPROF signal is received while fork() or clone() are executing,
+  // the process may hang. To avoid this, we ignore SIGPROF here and re-enable
+  // it after the call to fork()/clone() is complete.
+  struct sigaction saved_sigprof_action;
+  struct sigaction ignore_sigprof_action;
+  memset(&ignore_sigprof_action, 0, sizeof(ignore_sigprof_action));
+  sigemptyset(&ignore_sigprof_action.sa_mask);
+  ignore_sigprof_action.sa_handler = SIG_IGN;
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(sigaction(
+      SIGPROF, &ignore_sigprof_action, &saved_sigprof_action));
+#   endif  // GTEST_OS_LINUX
+
+#   if GTEST_HAS_CLONE
   const bool use_fork = GTEST_FLAG(death_test_use_fork);
 
   if (!use_fork) {
@@ -7163,21 +7551,37 @@
     void* const stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE,
                              MAP_ANON | MAP_PRIVATE, -1, 0);
     GTEST_DEATH_TEST_CHECK_(stack != MAP_FAILED);
+
+    // Maximum stack alignment in bytes:  For a downward-growing stack, this
+    // amount is subtracted from size of the stack space to get an address
+    // that is within the stack space and is aligned on all systems we care
+    // about.  As far as I know there is no ABI with stack alignment greater
+    // than 64.  We assume stack and stack_size already have alignment of
+    // kMaxStackAlignment.
+    const size_t kMaxStackAlignment = 64;
     void* const stack_top =
-        static_cast<char*>(stack) + (stack_grows_down ? stack_size : 0);
+        static_cast<char*>(stack) +
+            (stack_grows_down ? stack_size - kMaxStackAlignment : 0);
+    GTEST_DEATH_TEST_CHECK_(stack_size > kMaxStackAlignment &&
+        reinterpret_cast<intptr_t>(stack_top) % kMaxStackAlignment == 0);
 
     child_pid = clone(&ExecDeathTestChildMain, stack_top, SIGCHLD, &args);
 
     GTEST_DEATH_TEST_CHECK_(munmap(stack, stack_size) != -1);
   }
-#  else
+#   else
   const bool use_fork = true;
-#  endif  // GTEST_HAS_CLONE
+#   endif  // GTEST_HAS_CLONE
 
   if (use_fork && (child_pid = fork()) == 0) {
       ExecDeathTestChildMain(&args);
       _exit(0);
   }
+#  endif  // GTEST_OS_QNX
+#  if GTEST_OS_LINUX
+  GTEST_DEATH_TEST_CHECK_SYSCALL_(
+      sigaction(SIGPROF, &saved_sigprof_action, NULL));
+#  endif  // GTEST_OS_LINUX
 
   GTEST_DEATH_TEST_CHECK_(child_pid != -1);
   return child_pid;
@@ -7205,16 +7609,16 @@
   // it be closed when the child process does an exec:
   GTEST_DEATH_TEST_CHECK_(fcntl(pipe_fd[1], F_SETFD, 0) != -1);
 
-  const String filter_flag =
-      String::Format("--%s%s=%s.%s",
-                     GTEST_FLAG_PREFIX_, kFilterFlag,
-                     info->test_case_name(), info->name());
-  const String internal_flag =
-      String::Format("--%s%s=%s|%d|%d|%d",
-                     GTEST_FLAG_PREFIX_, kInternalRunDeathTestFlag,
-                     file_, line_, death_test_index, pipe_fd[1]);
+  const std::string filter_flag =
+      std::string("--") + GTEST_FLAG_PREFIX_ + kFilterFlag + "="
+      + info->test_case_name() + "." + info->name();
+  const std::string internal_flag =
+      std::string("--") + GTEST_FLAG_PREFIX_ + kInternalRunDeathTestFlag + "="
+      + file_ + "|" + StreamableToString(line_) + "|"
+      + StreamableToString(death_test_index) + "|"
+      + StreamableToString(pipe_fd[1]);
   Arguments args;
-  args.AddArguments(GetArgvs());
+  args.AddArguments(GetArgvsForDeathTestChildProcess());
   args.AddArgument(filter_flag.c_str());
   args.AddArgument(internal_flag.c_str());
 
@@ -7225,7 +7629,7 @@
   // is necessary.
   FlushInfoLog();
 
-  const pid_t child_pid = ExecDeathTestFork(args.Argv(), pipe_fd[0]);
+  const pid_t child_pid = ExecDeathTestSpawnChild(args.Argv(), pipe_fd[0]);
   GTEST_DEATH_TEST_CHECK_SYSCALL_(close(pipe_fd[1]));
   set_child_pid(child_pid);
   set_read_fd(pipe_fd[0]);
@@ -7251,9 +7655,10 @@
 
   if (flag != NULL) {
     if (death_test_index > flag->index()) {
-      DeathTest::set_last_death_test_message(String::Format(
-          "Death test count (%d) somehow exceeded expected maximum (%d)",
-          death_test_index, flag->index()));
+      DeathTest::set_last_death_test_message(
+          "Death test count (" + StreamableToString(death_test_index)
+          + ") somehow exceeded expected maximum ("
+          + StreamableToString(flag->index()) + ")");
       return false;
     }
 
@@ -7282,9 +7687,9 @@
 # endif  // GTEST_OS_WINDOWS
 
   else {  // NOLINT - this is more readable than unbalanced brackets inside #if.
-    DeathTest::set_last_death_test_message(String::Format(
-        "Unknown death test style \"%s\" encountered",
-        GTEST_FLAG(death_test_style).c_str()));
+    DeathTest::set_last_death_test_message(
+        "Unknown death test style \"" + GTEST_FLAG(death_test_style)
+        + "\" encountered");
     return false;
   }
 
@@ -7322,8 +7727,8 @@
                                                    FALSE,  // Non-inheritable.
                                                    parent_process_id));
   if (parent_process_handle.Get() == INVALID_HANDLE_VALUE) {
-    DeathTestAbort(String::Format("Unable to open parent process %u",
-                                  parent_process_id));
+    DeathTestAbort("Unable to open parent process " +
+                   StreamableToString(parent_process_id));
   }
 
   // TODO(vladl@google.com): Replace the following check with a
@@ -7343,9 +7748,10 @@
                                  // DUPLICATE_SAME_ACCESS is used.
                          FALSE,  // Request non-inheritable handler.
                          DUPLICATE_SAME_ACCESS)) {
-    DeathTestAbort(String::Format(
-        "Unable to duplicate the pipe handle %Iu from the parent process %u",
-        write_handle_as_size_t, parent_process_id));
+    DeathTestAbort("Unable to duplicate the pipe handle " +
+                   StreamableToString(write_handle_as_size_t) +
+                   " from the parent process " +
+                   StreamableToString(parent_process_id));
   }
 
   const HANDLE event_handle = reinterpret_cast<HANDLE>(event_handle_as_size_t);
@@ -7356,17 +7762,18 @@
                          0x0,
                          FALSE,
                          DUPLICATE_SAME_ACCESS)) {
-    DeathTestAbort(String::Format(
-        "Unable to duplicate the event handle %Iu from the parent process %u",
-        event_handle_as_size_t, parent_process_id));
+    DeathTestAbort("Unable to duplicate the event handle " +
+                   StreamableToString(event_handle_as_size_t) +
+                   " from the parent process " +
+                   StreamableToString(parent_process_id));
   }
 
   const int write_fd =
       ::_open_osfhandle(reinterpret_cast<intptr_t>(dup_write_handle), O_APPEND);
   if (write_fd == -1) {
-    DeathTestAbort(String::Format(
-        "Unable to convert pipe handle %Iu to a file descriptor",
-        write_handle_as_size_t));
+    DeathTestAbort("Unable to convert pipe handle " +
+                   StreamableToString(write_handle_as_size_t) +
+                   " to a file descriptor");
   }
 
   // Signals the parent that the write end of the pipe has been acquired
@@ -7403,9 +7810,8 @@
       || !ParseNaturalNumber(fields[3], &parent_process_id)
       || !ParseNaturalNumber(fields[4], &write_handle_as_size_t)
       || !ParseNaturalNumber(fields[5], &event_handle_as_size_t)) {
-    DeathTestAbort(String::Format(
-        "Bad --gtest_internal_run_death_test flag: %s",
-        GTEST_FLAG(internal_run_death_test).c_str()));
+    DeathTestAbort("Bad --gtest_internal_run_death_test flag: " +
+                   GTEST_FLAG(internal_run_death_test));
   }
   write_fd = GetStatusFileDescriptor(parent_process_id,
                                      write_handle_as_size_t,
@@ -7416,9 +7822,8 @@
       || !ParseNaturalNumber(fields[1], &line)
       || !ParseNaturalNumber(fields[2], &index)
       || !ParseNaturalNumber(fields[3], &write_fd)) {
-    DeathTestAbort(String::Format(
-        "Bad --gtest_internal_run_death_test flag: %s",
-        GTEST_FLAG(internal_run_death_test).c_str()));
+    DeathTestAbort("Bad --gtest_internal_run_death_test flag: "
+        + GTEST_FLAG(internal_run_death_test));
   }
 
 # endif  // GTEST_OS_WINDOWS
@@ -7470,8 +7875,8 @@
 #elif GTEST_OS_WINDOWS
 # include <direct.h>
 # include <io.h>
-#elif GTEST_OS_SYMBIAN || GTEST_OS_NACL
-// Symbian OpenC and NaCl have PATH_MAX in sys/syslimits.h
+#elif GTEST_OS_SYMBIAN
+// Symbian OpenC has PATH_MAX in sys/syslimits.h
 # include <sys/syslimits.h>
 #else
 # include <limits.h>
@@ -7546,9 +7951,10 @@
 // FilePath("dir/file"). If a case-insensitive extension is not
 // found, returns a copy of the original FilePath.
 FilePath FilePath::RemoveExtension(const char* extension) const {
-  String dot_extension(String::Format(".%s", extension));
-  if (pathname_.EndsWithCaseInsensitive(dot_extension.c_str())) {
-    return FilePath(String(pathname_.c_str(), pathname_.length() - 4));
+  const std::string dot_extension = std::string(".") + extension;
+  if (String::EndsWithCaseInsensitive(pathname_, dot_extension)) {
+    return FilePath(pathname_.substr(
+        0, pathname_.length() - dot_extension.length()));
   }
   return *this;
 }
@@ -7577,7 +7983,7 @@
 // On Windows platform, '\' is the path separator, otherwise it is '/'.
 FilePath FilePath::RemoveDirectoryName() const {
   const char* const last_sep = FindLastPathSeparator();
-  return last_sep ? FilePath(String(last_sep + 1)) : *this;
+  return last_sep ? FilePath(last_sep + 1) : *this;
 }
 
 // RemoveFileName returns the directory path with the filename removed.
@@ -7588,9 +7994,9 @@
 // On Windows platform, '\' is the path separator, otherwise it is '/'.
 FilePath FilePath::RemoveFileName() const {
   const char* const last_sep = FindLastPathSeparator();
-  String dir;
+  std::string dir;
   if (last_sep) {
-    dir = String(c_str(), last_sep + 1 - c_str());
+    dir = std::string(c_str(), last_sep + 1 - c_str());
   } else {
     dir = kCurrentDirectoryString;
   }
@@ -7607,11 +8013,12 @@
                                 const FilePath& base_name,
                                 int number,
                                 const char* extension) {
-  String file;
+  std::string file;
   if (number == 0) {
-    file = String::Format("%s.%s", base_name.c_str(), extension);
+    file = base_name.string() + "." + extension;
   } else {
-    file = String::Format("%s_%d.%s", base_name.c_str(), number, extension);
+    file = base_name.string() + "_" + StreamableToString(number)
+        + "." + extension;
   }
   return ConcatPaths(directory, FilePath(file));
 }
@@ -7623,8 +8030,7 @@
   if (directory.IsEmpty())
     return relative_path;
   const FilePath dir(directory.RemoveTrailingPathSeparator());
-  return FilePath(String::Format("%s%c%s", dir.c_str(), kPathSeparator,
-                                 relative_path.c_str()));
+  return FilePath(dir.string() + kPathSeparator + relative_path.string());
 }
 
 // Returns true if pathname describes something findable in the file-system,
@@ -7768,7 +8174,7 @@
 // On Windows platform, uses \ as the separator, other platforms use /.
 FilePath FilePath::RemoveTrailingPathSeparator() const {
   return IsDirectory()
-      ? FilePath(String(pathname_.c_str(), pathname_.length() - 1))
+      ? FilePath(pathname_.substr(0, pathname_.length() - 1))
       : *this;
 }
 
@@ -7860,6 +8266,11 @@
 # include <mach/vm_map.h>
 #endif  // GTEST_OS_MAC
 
+#if GTEST_OS_QNX
+# include <devctl.h>
+# include <sys/procfs.h>
+#endif  // GTEST_OS_QNX
+
 
 // Indicates that this translation unit is part of Google Test's
 // implementation.  It must come before gtest-internal-inl.h is
@@ -7902,6 +8313,26 @@
   }
 }
 
+#elif GTEST_OS_QNX
+
+// Returns the number of threads running in the process, or 0 to indicate that
+// we cannot detect it.
+size_t GetThreadCount() {
+  const int fd = open("/proc/self/as", O_RDONLY);
+  if (fd < 0) {
+    return 0;
+  }
+  procfs_info process_info;
+  const int status =
+      devctl(fd, DCMD_PROC_INFO, &process_info, sizeof(process_info), NULL);
+  close(fd);
+  if (status == EOK) {
+    return static_cast<size_t>(process_info.num_threads);
+  } else {
+    return 0;
+  }
+}
+
 #else
 
 size_t GetThreadCount() {
@@ -8026,7 +8457,7 @@
 }
 
 // Helper function used by ValidateRegex() to format error messages.
-String FormatRegexSyntaxError(const char* regex, int index) {
+std::string FormatRegexSyntaxError(const char* regex, int index) {
   return (Message() << "Syntax error at index " << index
           << " in simple regular expression \"" << regex << "\": ").GetString();
 }
@@ -8233,15 +8664,15 @@
 // Formats a source file path and a line number as they would appear
 // in an error message from the compiler used to compile this code.
 GTEST_API_ ::std::string FormatFileLocation(const char* file, int line) {
-  const char* const file_name = file == NULL ? kUnknownFile : file;
+  const std::string file_name(file == NULL ? kUnknownFile : file);
 
   if (line < 0) {
-    return String::Format("%s:", file_name).c_str();
+    return file_name + ":";
   }
 #ifdef _MSC_VER
-  return String::Format("%s(%d):", file_name, line).c_str();
+  return file_name + "(" + StreamableToString(line) + "):";
 #else
-  return String::Format("%s:%d:", file_name, line).c_str();
+  return file_name + ":" + StreamableToString(line) + ":";
 #endif  // _MSC_VER
 }
 
@@ -8252,12 +8683,12 @@
 // to the file location it produces, unlike FormatFileLocation().
 GTEST_API_ ::std::string FormatCompilerIndependentFileLocation(
     const char* file, int line) {
-  const char* const file_name = file == NULL ? kUnknownFile : file;
+  const std::string file_name(file == NULL ? kUnknownFile : file);
 
   if (line < 0)
     return file_name;
   else
-    return String::Format("%s:%d", file_name, line).c_str();
+    return file_name + ":" + StreamableToString(line);
 }
 
 
@@ -8292,8 +8723,7 @@
 class CapturedStream {
  public:
   // The ctor redirects the stream to a temporary file.
-  CapturedStream(int fd) : fd_(fd), uncaptured_fd_(dup(fd)) {
-
+  explicit CapturedStream(int fd) : fd_(fd), uncaptured_fd_(dup(fd)) {
 # if GTEST_OS_WINDOWS
     char temp_dir_path[MAX_PATH + 1] = { '\0' };  // NOLINT
     char temp_file_path[MAX_PATH + 1] = { '\0' };  // NOLINT
@@ -8310,10 +8740,29 @@
                                     << temp_file_path;
     filename_ = temp_file_path;
 # else
-    // There's no guarantee that a test has write access to the
-    // current directory, so we create the temporary file in the /tmp
-    // directory instead.
+    // There's no guarantee that a test has write access to the current
+    // directory, so we create the temporary file in the /tmp directory
+    // instead. We use /tmp on most systems, and /sdcard on Android.
+    // That's because Android doesn't have /tmp.
+#  if GTEST_OS_LINUX_ANDROID
+    // Note: Android applications are expected to call the framework's
+    // Context.getExternalStorageDirectory() method through JNI to get
+    // the location of the world-writable SD Card directory. However,
+    // this requires a Context handle, which cannot be retrieved
+    // globally from native code. Doing so also precludes running the
+    // code as part of a regular standalone executable, which doesn't
+    // run in a Dalvik process (e.g. when running it through 'adb shell').
+    //
+    // The location /sdcard is directly accessible from native code
+    // and is the only location (unofficially) supported by the Android
+    // team. It's generally a symlink to the real SD Card mount point
+    // which can be /mnt/sdcard, /mnt/sdcard0, /system/media/sdcard, or
+    // other OEM-customized locations. Never rely on these, and always
+    // use /sdcard.
+    char name_template[] = "/sdcard/gtest_captured_stream.XXXXXX";
+#  else
     char name_template[] = "/tmp/captured_stream.XXXXXX";
+#  endif  // GTEST_OS_LINUX_ANDROID
     const int captured_fd = mkstemp(name_template);
     filename_ = name_template;
 # endif  // GTEST_OS_WINDOWS
@@ -8326,7 +8775,7 @@
     remove(filename_.c_str());
   }
 
-  String GetCapturedString() {
+  std::string GetCapturedString() {
     if (uncaptured_fd_ != -1) {
       // Restores the original stream.
       fflush(NULL);
@@ -8336,14 +8785,14 @@
     }
 
     FILE* const file = posix::FOpen(filename_.c_str(), "r");
-    const String content = ReadEntireFile(file);
+    const std::string content = ReadEntireFile(file);
     posix::FClose(file);
     return content;
   }
 
  private:
-  // Reads the entire content of a file as a String.
-  static String ReadEntireFile(FILE* file);
+  // Reads the entire content of a file as an std::string.
+  static std::string ReadEntireFile(FILE* file);
 
   // Returns the size (in bytes) of a file.
   static size_t GetFileSize(FILE* file);
@@ -8363,7 +8812,7 @@
 }
 
 // Reads the entire content of a file as a string.
-String CapturedStream::ReadEntireFile(FILE* file) {
+std::string CapturedStream::ReadEntireFile(FILE* file) {
   const size_t file_size = GetFileSize(file);
   char* const buffer = new char[file_size];
 
@@ -8379,7 +8828,7 @@
     bytes_read += bytes_last_read;
   } while (bytes_last_read > 0 && bytes_read < file_size);
 
-  const String content(buffer, bytes_read);
+  const std::string content(buffer, bytes_read);
   delete[] buffer;
 
   return content;
@@ -8402,8 +8851,8 @@
 }
 
 // Stops capturing the output stream and returns the captured string.
-String GetCapturedStream(CapturedStream** captured_stream) {
-  const String content = (*captured_stream)->GetCapturedString();
+std::string GetCapturedStream(CapturedStream** captured_stream) {
+  const std::string content = (*captured_stream)->GetCapturedString();
 
   delete *captured_stream;
   *captured_stream = NULL;
@@ -8422,21 +8871,37 @@
 }
 
 // Stops capturing stdout and returns the captured string.
-String GetCapturedStdout() { return GetCapturedStream(&g_captured_stdout); }
+std::string GetCapturedStdout() {
+  return GetCapturedStream(&g_captured_stdout);
+}
 
 // Stops capturing stderr and returns the captured string.
-String GetCapturedStderr() { return GetCapturedStream(&g_captured_stderr); }
+std::string GetCapturedStderr() {
+  return GetCapturedStream(&g_captured_stderr);
+}
 
 #endif  // GTEST_HAS_STREAM_REDIRECTION
 
 #if GTEST_HAS_DEATH_TEST
 
 // A copy of all command line arguments.  Set by InitGoogleTest().
-::std::vector<String> g_argvs;
+::std::vector<testing::internal::string> g_argvs;
 
-// Returns the command line as a vector of strings.
-const ::std::vector<String>& GetArgvs() { return g_argvs; }
+static const ::std::vector<testing::internal::string>* g_injected_test_argvs =
+                                        NULL;  // Owned.
 
+void SetInjectableArgvs(const ::std::vector<testing::internal::string>* argvs) {
+  if (g_injected_test_argvs != argvs)
+    delete g_injected_test_argvs;
+  g_injected_test_argvs = argvs;
+}
+
+const ::std::vector<testing::internal::string>& GetInjectableArgvs() {
+  if (g_injected_test_argvs != NULL) {
+    return *g_injected_test_argvs;
+  }
+  return g_argvs;
+}
 #endif  // GTEST_HAS_DEATH_TEST
 
 #if GTEST_OS_WINDOWS_MOBILE
@@ -8451,8 +8916,8 @@
 // Returns the name of the environment variable corresponding to the
 // given flag.  For example, FlagToEnvVar("foo") will return
 // "GTEST_FOO" in the open-source version.
-static String FlagToEnvVar(const char* flag) {
-  const String full_flag =
+static std::string FlagToEnvVar(const char* flag) {
+  const std::string full_flag =
       (Message() << GTEST_FLAG_PREFIX_ << flag).GetString();
 
   Message env_var;
@@ -8509,7 +8974,7 @@
 //
 // The value is considered true iff it's not "0".
 bool BoolFromGTestEnv(const char* flag, bool default_value) {
-  const String env_var = FlagToEnvVar(flag);
+  const std::string env_var = FlagToEnvVar(flag);
   const char* const string_value = posix::GetEnv(env_var.c_str());
   return string_value == NULL ?
       default_value : strcmp(string_value, "0") != 0;
@@ -8519,7 +8984,7 @@
 // variable corresponding to the given flag; if it isn't set or
 // doesn't represent a valid 32-bit integer, returns default_value.
 Int32 Int32FromGTestEnv(const char* flag, Int32 default_value) {
-  const String env_var = FlagToEnvVar(flag);
+  const std::string env_var = FlagToEnvVar(flag);
   const char* const string_value = posix::GetEnv(env_var.c_str());
   if (string_value == NULL) {
     // The environment variable is not set.
@@ -8541,7 +9006,7 @@
 // Reads and returns the string environment variable corresponding to
 // the given flag; if it's not set, returns default_value.
 const char* StringFromGTestEnv(const char* flag, const char* default_value) {
-  const String env_var = FlagToEnvVar(flag);
+  const std::string env_var = FlagToEnvVar(flag);
   const char* const value = posix::GetEnv(env_var.c_str());
   return value == NULL ? default_value : value;
 }
@@ -8603,14 +9068,6 @@
 
 using ::std::ostream;
 
-#if GTEST_OS_WINDOWS_MOBILE  // Windows CE does not define _snprintf_s.
-# define snprintf _snprintf
-#elif _MSC_VER >= 1400  // VC 8.0 and later deprecate snprintf and _snprintf.
-# define snprintf _snprintf_s
-#elif _MSC_VER
-# define snprintf _snprintf
-#endif  // GTEST_OS_WINDOWS_MOBILE
-
 // Prints a segment of bytes in the given object.
 void PrintByteSegmentInObjectTo(const unsigned char* obj_bytes, size_t start,
                                 size_t count, ostream* os) {
@@ -8625,7 +9082,7 @@
       else
         *os << '-';
     }
-    snprintf(text, sizeof(text), "%02X", obj_bytes[j]);
+    GTEST_SNPRINTF_(text, sizeof(text), "%02X", obj_bytes[j]);
     *os << text;
   }
 }
@@ -8732,16 +9189,16 @@
         *os << static_cast<char>(c);
         return kAsIs;
       } else {
-        *os << String::Format("\\x%X", static_cast<UnsignedChar>(c));
+        *os << "\\x" + String::FormatHexInt(static_cast<UnsignedChar>(c));
         return kHexEscape;
       }
   }
   return kSpecialEscape;
 }
 
-// Prints a char c as if it's part of a string literal, escaping it when
+// Prints a wchar_t c as if it's part of a string literal, escaping it when
 // necessary; returns how c was formatted.
-static CharFormat PrintAsWideStringLiteralTo(wchar_t c, ostream* os) {
+static CharFormat PrintAsStringLiteralTo(wchar_t c, ostream* os) {
   switch (c) {
     case L'\'':
       *os << "'";
@@ -8756,8 +9213,9 @@
 
 // Prints a char c as if it's part of a string literal, escaping it when
 // necessary; returns how c was formatted.
-static CharFormat PrintAsNarrowStringLiteralTo(char c, ostream* os) {
-  return PrintAsWideStringLiteralTo(static_cast<unsigned char>(c), os);
+static CharFormat PrintAsStringLiteralTo(char c, ostream* os) {
+  return PrintAsStringLiteralTo(
+      static_cast<wchar_t>(static_cast<unsigned char>(c)), os);
 }
 
 // Prints a wide or narrow character c and its code.  '\0' is printed
@@ -8776,7 +9234,7 @@
   // obvious).
   if (c == 0)
     return;
-  *os << " (" << String::Format("%d", c).c_str();
+  *os << " (" << static_cast<int>(c);
 
   // For more convenience, we print c's code again in hexidecimal,
   // unless c was already printed in the form '\x##' or the code is in
@@ -8784,8 +9242,7 @@
   if (format == kHexEscape || (1 <= c && c <= 9)) {
     // Do nothing.
   } else {
-    *os << String::Format(", 0x%X",
-                          static_cast<UnsignedChar>(c)).c_str();
+    *os << ", 0x" << String::FormatHexInt(static_cast<UnsignedChar>(c));
   }
   *os << ")";
 }
@@ -8803,48 +9260,63 @@
   PrintCharAndCodeTo<wchar_t>(wc, os);
 }
 
-// Prints the given array of characters to the ostream.
-// The array starts at *begin, the length is len, it may include '\0' characters
-// and may not be null-terminated.
-static void PrintCharsAsStringTo(const char* begin, size_t len, ostream* os) {
-  *os << "\"";
+// Prints the given array of characters to the ostream.  CharType must be either
+// char or wchar_t.
+// The array starts at begin, the length is len, it may include '\0' characters
+// and may not be NUL-terminated.
+template <typename CharType>
+static void PrintCharsAsStringTo(
+    const CharType* begin, size_t len, ostream* os) {
+  const char* const kQuoteBegin = sizeof(CharType) == 1 ? "\"" : "L\"";
+  *os << kQuoteBegin;
   bool is_previous_hex = false;
   for (size_t index = 0; index < len; ++index) {
-    const char cur = begin[index];
+    const CharType cur = begin[index];
     if (is_previous_hex && IsXDigit(cur)) {
       // Previous character is of '\x..' form and this character can be
       // interpreted as another hexadecimal digit in its number. Break string to
       // disambiguate.
-      *os << "\" \"";
+      *os << "\" " << kQuoteBegin;
     }
-    is_previous_hex = PrintAsNarrowStringLiteralTo(cur, os) == kHexEscape;
+    is_previous_hex = PrintAsStringLiteralTo(cur, os) == kHexEscape;
   }
   *os << "\"";
 }
 
+// Prints a (const) char/wchar_t array of 'len' elements, starting at address
+// 'begin'.  CharType must be either char or wchar_t.
+template <typename CharType>
+static void UniversalPrintCharArray(
+    const CharType* begin, size_t len, ostream* os) {
+  // The code
+  //   const char kFoo[] = "foo";
+  // generates an array of 4, not 3, elements, with the last one being '\0'.
+  //
+  // Therefore when printing a char array, we don't print the last element if
+  // it's '\0', such that the output matches the string literal as it's
+  // written in the source code.
+  if (len > 0 && begin[len - 1] == '\0') {
+    PrintCharsAsStringTo(begin, len - 1, os);
+    return;
+  }
+
+  // If, however, the last element in the array is not '\0', e.g.
+  //    const char kFoo[] = { 'f', 'o', 'o' };
+  // we must print the entire array.  We also print a message to indicate
+  // that the array is not NUL-terminated.
+  PrintCharsAsStringTo(begin, len, os);
+  *os << " (no terminating NUL)";
+}
+
 // Prints a (const) char array of 'len' elements, starting at address 'begin'.
 void UniversalPrintArray(const char* begin, size_t len, ostream* os) {
-  PrintCharsAsStringTo(begin, len, os);
+  UniversalPrintCharArray(begin, len, os);
 }
 
-// Prints the given array of wide characters to the ostream.
-// The array starts at *begin, the length is len, it may include L'\0'
-// characters and may not be null-terminated.
-static void PrintWideCharsAsStringTo(const wchar_t* begin, size_t len,
-                                     ostream* os) {
-  *os << "L\"";
-  bool is_previous_hex = false;
-  for (size_t index = 0; index < len; ++index) {
-    const wchar_t cur = begin[index];
-    if (is_previous_hex && isascii(cur) && IsXDigit(static_cast<char>(cur))) {
-      // Previous character is of '\x..' form and this character can be
-      // interpreted as another hexadecimal digit in its number. Break string to
-      // disambiguate.
-      *os << "\" L\"";
-    }
-    is_previous_hex = PrintAsWideStringLiteralTo(cur, os) == kHexEscape;
-  }
-  *os << "\"";
+// Prints a (const) wchar_t array of 'len' elements, starting at address
+// 'begin'.
+void UniversalPrintArray(const wchar_t* begin, size_t len, ostream* os) {
+  UniversalPrintCharArray(begin, len, os);
 }
 
 // Prints the given C string to the ostream.
@@ -8870,7 +9342,7 @@
     *os << "NULL";
   } else {
     *os << ImplicitCast_<const void*>(s) << " pointing to ";
-    PrintWideCharsAsStringTo(s, wcslen(s), os);
+    PrintCharsAsStringTo(s, wcslen(s), os);
   }
 }
 #endif  // wchar_t is native
@@ -8889,13 +9361,13 @@
 // Prints a ::wstring object.
 #if GTEST_HAS_GLOBAL_WSTRING
 void PrintWideStringTo(const ::wstring& s, ostream* os) {
-  PrintWideCharsAsStringTo(s.data(), s.size(), os);
+  PrintCharsAsStringTo(s.data(), s.size(), os);
 }
 #endif  // GTEST_HAS_GLOBAL_WSTRING
 
 #if GTEST_HAS_STD_WSTRING
 void PrintWideStringTo(const ::std::wstring& s, ostream* os) {
-  PrintWideCharsAsStringTo(s.data(), s.size(), os);
+  PrintCharsAsStringTo(s.data(), s.size(), os);
 }
 #endif  // GTEST_HAS_STD_WSTRING
 
@@ -8950,10 +9422,10 @@
 
 // Gets the summary of the failure message by omitting the stack trace
 // in it.
-internal::String TestPartResult::ExtractSummary(const char* message) {
+std::string TestPartResult::ExtractSummary(const char* message) {
   const char* const stack_trace = strstr(message, internal::kStackTraceMarker);
-  return stack_trace == NULL ? internal::String(message) :
-      internal::String(message, stack_trace - message);
+  return stack_trace == NULL ? message :
+      std::string(message, stack_trace);
 }
 
 // Prints a TestPartResult object.
@@ -9068,10 +9540,10 @@
   registered_tests = SkipSpaces(registered_tests);
 
   Message errors;
-  ::std::set<String> tests;
+  ::std::set<std::string> tests;
   for (const char* names = registered_tests; names != NULL;
        names = SkipComma(names)) {
-    const String name = GetPrefixUntilComma(names);
+    const std::string name = GetPrefixUntilComma(names);
     if (tests.count(name) != 0) {
       errors << "Test " << name << " is listed more than once.\n";
       continue;
@@ -9103,7 +9575,7 @@
     }
   }
 
-  const String& errors_str = errors.GetString();
+  const std::string& errors_str = errors.GetString();
   if (errors_str != "") {
     fprintf(stderr, "%s %s", FormatFileLocation(file, line).c_str(),
             errors_str.c_str());
