#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

char *trans(char *);

JNIEXPORT jstring JNICALL
Java_com_example_sodemo_MainActivity_transMsg(JNIEnv *env, jobject obj, jstring inputMsg) {
    char *inputStr = (char *) (*env)->GetStringUTFChars(env, inputMsg, 0);
    char *outputStr = trans(inputStr);
    (*env)->ReleaseStringUTFChars(env, inputMsg, inputStr);
    return (*env)->NewStringUTF(env, outputStr);
}

char *trans(char *msg) {
    size_t msg_len = strlen(msg);
    char *result = malloc(msg_len + 1); //+1 for the zero-terminator
    for (int i = 0; i < msg_len; i++) {
        result[i] = msg[msg_len - i - 1];
    }
    return result;
}
