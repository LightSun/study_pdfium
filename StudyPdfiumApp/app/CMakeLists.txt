cmake_minimum_required(VERSION 3.4.1)

project(study_pdfium)
SET(APP_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/${ANDROID_ABI})
SET(SRC_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp)

set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -O3 -fexceptions -DHAVE_PTHREADS -DANDROID -DOPJ_STATIC -DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_FILE -DWIN32_LEAN_AND_MEAN -DU_I18N_IMPLEMENTATION -DU_ICUDATAENTRY_IN_COMMON -DU_COMMON_IMPLEMENTATION -DU_STATIC_IMPLEMENTATION -DFT2_BUILD_LIBRARY")
set(CMAKE_CXX_FLAGS_DEBUG " ${CMAKE_CXX_FLAGS_DEBUG}  -O3 -fexceptions -DHAVE_PTHREADS -DANDROID -DOPJ_STATIC -DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_FILE -DWIN32_LEAN_AND_MEAN -DU_I18N_IMPLEMENTATION -DU_ICUDATAENTRY_IN_COMMON -DU_COMMON_IMPLEMENTATION -DU_STATIC_IMPLEMENTATION -DFT2_BUILD_LIBRARY")
set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} -O3 -fexceptions -DHAVE_PTHREADS -DANDROID -DOPJ_STATIC -DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_FILE -DWIN32_LEAN_AND_MEAN -DU_I18N_IMPLEMENTATION -DU_ICUDATAENTRY_IN_COMMON -DU_COMMON_IMPLEMENTATION -DU_STATIC_IMPLEMENTATION -DFT2_BUILD_LIBRARY")

#add_library(
#        modft2
#        SHARED
#        IMPORTED
#)
#set_target_properties(modft2 PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libmodft2.so
#)

add_library(
    pdfium2
    SHARED
    ${SRC_DIR}/Test.cpp
)

#function (importLib arg)
#    message(STATUS "ARGN: ${ARGN}")
#    message(STATUS "ARGC: ${ARGC}")
 #   message(STATUS "ARGV: ${ARGV}")
 #   message(STATUS "ARGV0: ${ARGV0}")

 #   list(LENGTH ARGV  argv_len)
 #   message(STATUS "length of ARGV: ${argv_len}")
#    set(i 0)
  #  while( i LESS ${argv_len})
 #        list(GET ARGV ${i} argv_value)
 #        message(STATUS "argv${i}: ${argv_value}")
 #        add_library(
 #                ${argv_value}
 #                STATIC
 #                IMPORTED
 #        )
 #        set_target_properties(${argv_value} PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/lib${argv_value}Debug.a
  #       )
         #math(EXPR i "${i} + 1")
  #  endwhile()
#endfunction ()

#importLib(fx_base fx_icu drm libjpeg fx_libopenjpeg fx_lcms2 fx_zlib fpdfdoc fpdftext fxage fx_agg32 fx_freetype fxcodec fpdfapi fxcrt pdfium)

add_library(fx_base STATIC IMPORTED)
set_target_properties(fx_base PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_baseDebug.a)

add_library(fx_icu STATIC IMPORTED)
set_target_properties(fx_icu PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_icuDebug.a)

add_library(fdrm STATIC IMPORTED)
set_target_properties(fdrm PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfdrmDebug.a)

add_library(
     libjpeg
     STATIC
     IMPORTED
)
set_target_properties(libjpeg PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/liblibjpegDebug.a)

add_library(
     fx_libopenjpeg
     STATIC
     IMPORTED
)
set_target_properties(fx_libopenjpeg PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_libopenjpegDebug.a
)

add_library(
     fx_lcms2
     STATIC
     IMPORTED
)
set_target_properties(fx_lcms2 PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_lcms2Debug.a
)

add_library(
     fx_zlib
     STATIC
     IMPORTED
)
set_target_properties(fx_zlib PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_zlibDebug.a
)

add_library(fpdfdoc  STATIC IMPORTED )
set_target_properties(fpdfdoc PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfpdfdocDebug.a)

add_library(fpdftext  STATIC IMPORTED )
set_target_properties(fpdftext PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfpdftextDebug.a)

add_library(fxage  STATIC IMPORTED )
set_target_properties(fxage PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfxageDebug.a)

add_library(fx_agg32  STATIC IMPORTED )
set_target_properties(fx_agg32 PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_agg32Debug.a)

add_library(fx_freetype  STATIC IMPORTED )
set_target_properties(fx_freetype PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfx_freetypeDebug.a)

add_library(fxcodec  STATIC IMPORTED )
set_target_properties(fxcodec PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfxcodecDebug.a)

add_library(fpdfapi  STATIC IMPORTED )
set_target_properties(fpdfapi PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfpdfapiDebug.a)

add_library(fxcrt  STATIC IMPORTED )
set_target_properties(fxcrt PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libfxcrtDebug.a)

add_library(pdfium  STATIC IMPORTED )
set_target_properties(pdfium PROPERTIES IMPORTED_LOCATION ${APP_LIBS_DIR}/libpdfiumDebug.a)

target_link_libraries(
    pdfium2
    fx_base
    fx_icu
    fdrm
    libjpeg
    fx_libopenjpeg
    fx_lcms2
    fx_zlib
    fpdfdoc
    fpdftext
    fxage
    fx_agg32
    fx_freetype
    fxcodec
    fpdfapi
    fxcrt
    pdfium
)

