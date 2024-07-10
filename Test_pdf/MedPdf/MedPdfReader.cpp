#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
#include <stdio.h>

#include <regex>

#include "fpdf_attachment.h"
#include "fpdf_edit.h"
#include "fpdf_doc.h"
#include "fpdf_text.h"
#include "fpdf_structtree.h"
#include "fpdfview.h"
#include <fpdf_formfill.h>
#include "MedPdfReader.h"

#include "helpers.h"

using namespace med_pdf;
using U16Str = std::u16string;
using CU16Str = const U16Str&;
template <typename T>
using List = std::vector<T>;
using ListU16Str = std::vector<U16Str>;

#ifdef _WIN32
#define NEW_LINE "\r\n"
#else
#define NEW_LINE "\n"
#endif

// 将UTF-16字符串按指定分隔符进行切割
static std::vector<U16Str> utf16Split(CU16Str str, CU16Str delimiter) {
    std::vector<U16Str> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::u16string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }

    tokens.push_back(str.substr(start));
    return tokens;
}
static inline int utf16ContainsCount(CU16Str str, CU16Str sub){
    size_t start = 0;
    size_t end = str.find(sub);

    int count = 0;
    while (end != std::u16string::npos) {
        //tokens.push_back(str.substr(start, end - start));
        count ++;
        start = end + sub.length();
        end = str.find(sub, start);
    }
    return count;
}
static inline String listU16ToStr(const std::vector<U16Str>& l, String sep){
    String content;
    for(auto i : MFOREACH(l.size())){
        content += strings::FromUtf16(l[i]);
        if(i != l.size() - 1){
            content += sep;
        }
    }
    return content;
}

namespace med_pdf {
    struct StrHelper0{
        ListU16Str* beforeDesc;
        ListU16Str* afterTip;
        ListU16Str* desc;
        ListU16Str* tip;

        StrHelper0(ListU16Str* beforeDesc, ListU16Str* afterTip,
                   ListU16Str* desc, ListU16Str* tip):
            beforeDesc(beforeDesc), afterTip(afterTip), desc(desc), tip(tip){
        }
        void populate(PdfInfo* pi){
            MED_ASSERT(beforeDesc->size() == 3);
            auto spaceStr = strings::Utf8ToUtf16(" ");
            {
                auto& str = beforeDesc->at(0);
                auto strs = utf16Split(str, spaceStr);
                if(strs.size() > 0){
                    if(strs.size() < 2){
                        pi->checkNum = strings::FromUtf16(strs[0]);
                    }else{
                        //MED_ASSERT(strs.size() == 2);
                        pi->checkNum = strings::FromUtf16(strs[0]);
                        pi->checkInNum = strings::FromUtf16(strs[1]);
                    }
                }
            }
            {
                auto& str = beforeDesc->at(1);
                auto strs = utf16Split(str, spaceStr);
                pi->age = 0;
                if(strs.size() > 0){
                    if(strs.size() >= 3){
                        //MED_ASSERT(strs.size() == 3);
                        pi->patientName = strings::FromUtf16(strs[0]);
                        pi->gender = strings::FromUtf16(strs[1]);
                        String ageStr = strings::FromUtf16(strs[2]);
                        pi->age = std::stoi(ageStr, nullptr);
                    }else{
                        pi->patientName = strings::FromUtf16(strs[0]);
                    }
                }
            }
            {
                auto& str = beforeDesc->at(2);
                auto strs = utf16Split(str, spaceStr);
                if(strs.size() > 0){
                    if(strs.size() < 3){
                        pi->checkPart = strings::FromUtf16(strs[0]);
                        pi->checkDate = "";
                    }else{
                        //MED_ASSERT(strs.size() == 3);
                        pi->checkPart = strings::FromUtf16(strs[0]);
                        pi->checkDate = strings::FromUtf16(strs[1] + spaceStr + strs[2]);
                    }
                }
            }
            pi->ultrasoundDesc = listU16ToStr(*desc, NEW_LINE);
            pi->ultrasoundTip = listU16ToStr(*tip, NEW_LINE);
            {
                //MED_ASSERT(afterTip->size() >= 5);
                if(afterTip->size() > 0){
                    pi->doctorName = strings::FromUtf16(afterTip->at(0));
                    if(afterTip->size() >= 5){
                        pi->reportDate = strings::FromUtf16(afterTip->at(4));
                    }
                }
            }
        }
    };

    struct MedPdfReaderCtx0{
        FPDF_PAGE page {nullptr};
        ListU16Str beforeDesc;
        ListU16Str afterTip;
        ListU16Str descTip;
        List<U16Str> u16Strs;
        int chStart {-1};
        int chEnd {-1};

        void clear(){
            beforeDesc.clear();
            afterTip.clear();
            descTip.clear();
            u16Strs.clear();
        }
        //may be failed.
        bool readPdfInfo(CString file, PdfInfo* pi){
            auto doc = FPDF_LoadDocument(file.data(), NULL);
            if(doc == nullptr){
                LOGE("open doc failed. %s\n", file.data());
                return false;
            }
            int pc = FPDF_GetPageCount(doc);
            if(pc != 1){
                LOGE("readPdfInfo >> failed by page count != 1. \n");
                FPDF_CloseDocument(doc);
                return false;
            }
            page = FPDF_LoadPage(doc, 0);
            getWholeContent();
            //
            auto ret = readPdfInfoImpl(pi);
            FPDF_ClosePage(page);
            page = nullptr;
            FPDF_CloseDocument(doc);
            return ret;
        }
        bool readPdfInfoImpl(PdfInfo* pi){
            auto textPge = FPDFText_LoadPage(page);
            //must start from 0. or-else cause bug.
            int rectC = FPDFText_CountRects(textPge, 0, -1);
            //
            String content;
            double lastTop = -1;
            double lastLeft = -1;
            //the position of last-one, second-last. of ',/.'
            int last_pos = -1;
            for(int i = 0 ; i < rectC ; ++i){
                if(i < chStart || i > chEnd){
                    continue;
                }
                int si = i;
                double left, top, right, bottom;
                if(!FPDFText_GetRect(textPge, si, &left, &top, &right, &bottom)){
                   LOGE("FPDFText_GetRect: error. start_i = %d\n", si);
                   MED_ASSERT(false);
                }
                //1M
                std::vector<char16_t> buf(1024);
                //FPDFText_get
                int actSize = FPDFText_GetBoundedText(textPge, left, top, right, bottom,
                                       (unsigned short*)buf.data(), buf.size());
                if(actSize <= 0){
                   LOGE("FPDFText_GetBoundedText: error.\n");
                   MED_ASSERT(false);
                }
                buf.resize(actSize);
                auto text = strings::FromUtf16(std::u16string(buf.data()));
                auto dleft = std::abs(lastLeft - left);
                auto dtop = std::abs(lastTop - top);
//                LOGI("rect(start_i = %d): left, top, right, bottom. text, d_left, d_top ="
//                                       " %.1f, %.1f, %.1f, %.1f, '%s', %.1f, %.1f\n",
//                                       si, left, top, right, bottom, text.data(),
//                                       dtop, dleft
//                                       );
                if(lastTop >= 0 && dleft >= 100 && dtop >= 100){
                   // LOGI("desc content: '%s'\n", content.data());
                    break; //JUST test
                }
                last_pos = i;
                lastLeft = left;
                lastTop = top;
                content += text;
            }
            FPDFText_ClosePage(textPge);
            MED_ASSERT(last_pos != -1);
            //
            int u16CharC = 0;
            auto spaceStr = strings::Utf8ToUtf16(" ");
            int featureLine = -1;
            for(auto i : MFOREACH(u16Strs.size())){
                auto& str = u16Strs[i];
                int spaceC = utf16ContainsCount(str, spaceStr);
                int actLen = str.length() - spaceC * spaceStr.length();
                if(last_pos >= u16CharC && last_pos <= u16CharC + actLen){
                    featureLine = i;
                    break;
                }
                u16CharC += actLen;
            }
            //LOGI("featureLine = %d\n", featureLine);
            auto& featureStr = u16Strs[featureLine];
            List<U16Str> desc;
            List<U16Str> tip;
            bool descEnough = false;
            for(auto i : MFOREACH(descTip.size())){
                auto& str = descTip[i];
                if(str == featureStr){
                    desc.push_back(str);
                    descEnough = true;
                }else if(descEnough){
                    tip.push_back(str);
                }else{
                    desc.push_back(str);
                }
            }
            StrHelper0 sh(&beforeDesc, &afterTip, &desc, &tip);
            sh.populate(pi);
            clear();
            return true;
        }
        void getWholeContent(){
            //space: 8. '\r\n': 16 (16*2)
            auto textPge = FPDFText_LoadPage(page);
            int cc = FPDFText_CountChars(textPge);
            int rects = FPDFText_CountRects(textPge, 0, -1);
            //LOGI("CountChars, CountRects = %d, %d\n", cc, rects);
            MED_ASSERT(cc > 0);
            std::vector<char16_t> buf(cc + 1);
            FPDFText_GetText(textPge, 0, cc, (unsigned short*)buf.data());
            FPDFText_ClosePage(textPge);
            //
            auto u16Str = std::u16string(buf.data(), cc);
            // LOGI("u16_len = %d, [unsigned short*] len = %d\n", (int)u16Str.length(), cc);
            auto text = strings::FromUtf16(u16Str);
            //LOGI("u16_len = %d, u8_len = %d\n", (int)u16Str.length(), (int)text.length());
            //
            auto spaceStr = strings::Utf8ToUtf16(" ");
            //
            chStart = 0;
            chEnd = 0;
            //
            auto spStr = strings::Utf8ToUtf16("\r\n");
            //LOGI("split_str: len = %d\n", (int)spStr.length());
            u16Strs = utf16Split(u16Str, spStr);
            int last_start = u16Strs.size() - 6;
            for(int i : MFOREACH(u16Strs.size())){
                auto u8Str = strings::FromUtf16(u16Strs[i]);
                //int len = u8Str.length();
                if(i < 3){
                    chStart += u16Strs[i].size() - spStr.length();
                    chEnd += u16Strs[i].size() - spStr.length();
                    beforeDesc.push_back(u16Strs[i]);
                }else if(i >= last_start){
                    afterTip.push_back(u16Strs[i]);
                }else{
                    chEnd += u16Strs[i].size();
                    descTip.push_back(u16Strs[i]);
                }
                //LOGI("u16Strs(FromUtf16) >> i = %d, (u8_len,u16_len) = (%d, %d)\n",
                //     i, len, (int)u16Strs[i].size());
               // trimLastR(strs[i]);
            }
            chStart += 1;
            //LOGI("chStart, chEnd = %d, %d\n", chStart, chEnd);
        }
    };
}

//---------------------------------
//may be failed.
bool med_pdf::readPdfInfo(CString file,PdfInfo* pi){
    MedPdfReaderCtx0 mpr;
    return mpr.readPdfInfo(file, pi);
}

void med_pdf::initLib(){
    FPDF_InitLibrary();
}
void med_pdf::destroyLib(){
    FPDF_DestroyLibrary();
}
