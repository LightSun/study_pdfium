cmake_minimum_required(VERSION 3.5)

project(Test_pdf LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(./)
## pdfium

set(PDFIUM_DIR /media/heaven7/Elements_SE/study/work/pdf/pdfium-linux-x64)
include(${PDFIUM_DIR}/PDFiumConfig.cmake)
include_directories(${PDFium_INCLUDE_DIR})

## opencv
SET(OpenCV_DIR /home/heaven7/heaven7/libs/opencv-3.4.7/opencv-4.5.4/_install/lib64/cmake/opencv4)
find_package( OpenCV 4 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DI1RS} )

## lib
file(GLOB_RECURSE MED_PDF_SRC
    MedPdf/*.h, MedPdf/*.hpp MedPdf/*.cpp
    )
add_library(MedPdfReader
    ${MED_PDF_SRC}
    )
target_link_libraries(MedPdfReader PUBLIC
    ${PDFium_LIBRARY}
    )

## demo
add_executable(Test_pdf
    helpers.h
    helpers.cpp
    main.cpp
    )

target_link_libraries(Test_pdf PUBLIC
    MedPdfReader
    ${PDFium_LIBRARY}
    ${OpenCV_LIBS}
    )
