#pragma once

#include <string>
#include <iostream>
#include <stdlib.h>

namespace med_pdf {

using String = std::string;
using CString = std::string;

#define LOGE(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) fprintf(stdout, fmt, ##__VA_ARGS__)

#define MED_ASSERT(condition)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(condition))                                                   \
        {                                                                   \
            std::cout << "Assertion failure: " << #condition << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)

#define MED_ASSERT_X(condition, msg)                                                   \
    do                                                                      \
    {                                                                       \
        if (!(condition))                                                   \
        {                                                                   \
            std::cout << "Assertion failure: " << #condition << ", " << msg << std::endl;  \
            abort();                                                        \
        }                                                                   \
    } while (0)

struct PdfInfo{
    String checkNum;
    String checkInNum;
    String patientName;
    String gender;
    int age;
    String checkPart;
    String checkDate;
    String UltrasoundDesc;
    String UltrasoundTip;
    String doctorName;
    String reportDate;
};

}
