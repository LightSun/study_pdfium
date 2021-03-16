//
// Created by yangl on 2017/6/9.
//
//

#ifndef _UTIL_HPP_
#define _UTIL_HPP_

extern "C" {
#include <stdlib.h>
}

#ifdef ANDROID
#include <android/log.h>
#include <jni.h>

#define JIN_PASSWORD_EXCEPTION "com/shockwave/pdfium/PdfPasswordException"
#define JNI_FUNC(retType, bindClass, name)  JNIEXPORT retType JNICALL Java_com_shockwave_pdfium_##bindClass##_##name
#define JNI_ARGS    JNIEnv *env, jobject thiz

#define LOG_TAG "jniPdfium"
#define LOGI(...)   __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)   __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...)   __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGI(...) printf(__VA_ARGS__);
#define LOGE(...) printf(__VA_ARGS__);
#define LOGD(...) printf(__VA_ARGS__);
#endif

#endif // _UTIL_HPP_
