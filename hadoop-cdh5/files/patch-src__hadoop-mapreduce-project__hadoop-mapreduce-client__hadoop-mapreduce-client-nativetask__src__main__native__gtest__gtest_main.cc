--- ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/gtest_main.cc.orig	2014-10-12 00:56:38.000000000 +0400
+++ ./hadoop-mapreduce-project/hadoop-mapreduce-client/hadoop-mapreduce-client-nativetask/src/main/native/gtest/gtest_main.cc	2014-10-27 13:32:15.127001927 +0300
@@ -27,13 +27,12 @@
 // (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 // OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
-#include <iostream>
+#include <stdio.h>
 
 #include "gtest/gtest.h"
 
 GTEST_API_ int main(int argc, char **argv) {
-  std::cout << "Running main() from gtest_main.cc\n";
-
+  printf("Running main() from gtest_main.cc\n");
   testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
 }
