--- pandas/tools/plotting.py.orig	2015-10-14 10:14:22 UTC
+++ pandas/tools/plotting.py
@@ -127,7 +127,7 @@ def _mpl_ge_1_4_0():
 def _mpl_ge_1_5_0():
     try:
         import matplotlib
-        return (matplotlib.__version__  >= LooseVersion('1.5')
+        return (str(matplotlib.__version__)  >= LooseVersion('1.5')
                 or matplotlib.__version__[0] == '0')
     except ImportError:
         return False
