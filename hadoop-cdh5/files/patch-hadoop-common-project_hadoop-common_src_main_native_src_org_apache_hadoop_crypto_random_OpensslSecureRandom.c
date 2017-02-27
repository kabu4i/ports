--- hadoop-common-project/hadoop-common/src/main/native/src/org/apache/hadoop/crypto/random/OpensslSecureRandom.c.orig	2015-07-21 12:15:23 UTC
+++ hadoop-common-project/hadoop-common/src/main/native/src/org/apache/hadoop/crypto/random/OpensslSecureRandom.c
@@ -245,7 +245,7 @@ static void locks_setup(void)
     pthread_mutex_init(&(lock_cs[i]), NULL);
   }
   
-  dlsym_CRYPTO_set_id_callback((unsigned long (*)())pthreads_thread_id);
+  dlsym_CRYPTO_set_id_callback((unsigned long (*)())pthread_self);
   dlsym_CRYPTO_set_locking_callback((void (*)())pthreads_locking_callback);
 }
 
@@ -272,11 +272,6 @@ static void pthreads_locking_callback(in
   }
 }
 
-static unsigned long pthreads_thread_id(void)
-{
-  return (unsigned long)syscall(SYS_gettid);
-}
-
 #endif /* UNIX */
 
 /**
