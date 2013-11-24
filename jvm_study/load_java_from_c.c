#include <stdio.h>
#include <jni.h>

JNIEnv* create_vm () {
    JavaVM* jvm;
    JNIEnv* env;
    JavaVMInitArgs args;
    JavaVMOption options[1];

    args.version = JNI_VERSION_1_4;
    args.nOptions = 1;
    options[0].optionString = "-Djava.class.path=/Users/felipecruz/Projects/mestrado/compiladores/jvm_study";
    args.options = options;
    args.ignoreUnrecognized = JNI_FALSE;

    JNI_CreateJavaVM (&jvm, (void **)&env, &args);
    return env;
}

void invoke_class (JNIEnv* env) {
    int i = 0;
    jclass test_class, string_class, map;
    jmethodID mainMethod, constructor, put, request;
    jobjectArray applicationArgs, map_instance;
    jstring applicationArg0, key, val;

    test_class = (*env)->FindClass (env, "Teste");
    mainMethod = (*env)->GetStaticMethodID (env, test_class,
                                            "main", "([Ljava/lang/String;)V");
    request = (*env)->GetStaticMethodID (env, test_class, "request",
                                                          "(Ljava/util/Map;)V");
    string_class = (*env)->FindClass (env, "java/lang/String");

    map = (*env)->FindClass (env, "java/util/HashMap");
    constructor = (*env)->GetMethodID (env, map, "<init>", "()V");

    put = (*env)->GetMethodID (env, map, "put",
                                         "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    for (i = 0; i < 300000; i++) {
        key = (*env)->NewStringUTF (env, "uri");
        val = (*env)->NewStringUTF (env, "http://loogica.net");
        map_instance = (*env)->NewObject (env, map, constructor);
        (*env)->CallVoidMethod (env, map_instance, put, key, val);
        (*env)->CallStaticVoidMethod (env, test_class, request, map_instance);
    }

    applicationArgs = (*env)->NewObjectArray (env, 1, string_class, NULL);
    applicationArg0 = (*env)->NewStringUTF (env, "From-C-program");

    (*env)->SetObjectArrayElement (env, applicationArgs, 0, applicationArg0);
    (*env)->CallStaticVoidMethod (env, test_class, mainMethod, applicationArgs);
}


int main(int argc, char **argv) {
    JNIEnv* env = create_vm();
    invoke_class (env);
    return 0;
}
