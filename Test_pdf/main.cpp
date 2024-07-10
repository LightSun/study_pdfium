#include <cstring>
#include <iomanip>
#include <iostream>
#include <vector>
#include <stdio.h>

#include "fpdf_attachment.h"
#include "fpdf_edit.h"
#include "fpdf_doc.h"
#include "fpdf_text.h"
#include "fpdf_structtree.h"
#include "fpdfview.h"
#include <fpdf_formfill.h>

#include "helpers.h"
#include "MedPdf/MedPdfReader.h"
#include "opencv2/opencv.hpp"

using String = std::string;

struct PdfImageHelper{
    FPDF_PAGEOBJECT img_obj;
    FPDF_BITMAP bitmap;
    PdfImageHelper(FPDF_PAGEOBJECT image_object):img_obj(image_object){
        bitmap = FPDFImageObj_GetBitmap(image_object);
    }
    ~PdfImageHelper(){
        if(bitmap){
            FPDFBitmap_Destroy(bitmap);
            bitmap = nullptr;
        }
    }

    void saveImage(int i){
        int w = FPDFBitmap_GetWidth(bitmap);
        int h = FPDFBitmap_GetHeight(bitmap);
        std::vector<uchar> buffer;
        auto fmt = FPDFBitmap_GetFormat(bitmap);
        int pixSize = -1;
        int type = -1;
        switch (fmt) {
        case FPDFBitmap_Gray:{
            pixSize = 1;
            type = CV_8U;
        }break;
        case FPDFBitmap_BGR:{
            pixSize = 3;
            type = CV_8UC3;
        }break;
        case FPDFBitmap_BGRA:{
            pixSize = 4;
            type = CV_8UC4;
        }break;
        case FPDFBitmap_BGRx:{
            fprintf(stderr, "FPDFBitmap_BGRx >> i = %d\n", i);
            abort();
        }break;
        case FPDFBitmap_Unknown:{
            fprintf(stderr, "FPDFBitmap_Unknown >> i = %d\n", i);
            abort();
        }break;
        }
        int size = h * w * pixSize * sizeof (uchar);
        buffer.resize(size);
        //
        auto stride = FPDFBitmap_GetStride(bitmap);
        auto buf = FPDFBitmap_GetBuffer(bitmap);
//        int decLen = FPDFImageObj_GetImageDataDecoded(img_obj,
//                            buffer.data(), buffer.size());
//        if(decLen <= 0){
//            fprintf(stderr, "FPDFImageObj_GetImageDataDecoded >> failed\n");
//            abort();
//        }
//        printf("size, decLen = {%d, %d}\n", size, decLen);
        //
        auto mat = cv::Mat(h, w, type, buf, stride);
        String img_name = std::to_string(i) + ".png";
        cv::imwrite(img_name, mat);
    }
};

void PrintPdfMetaData(FPDF_DOCUMENT doc) {
  static constexpr const char *kMetaTags[] = {
      "Title",   "Author",   "Subject",      "Keywords",
      "Creator", "Producer", "CreationDate", "ModDate"};
  for (const char *meta_tag : kMetaTags) {
    const unsigned long len = FPDF_GetMetaText(doc, meta_tag, nullptr, 0);
    if (!len)
      continue;

    std::vector<char16_t> buf(len);
    FPDF_GetMetaText(doc, meta_tag, buf.data(), buf.size());
    auto text = strings::FromUtf16(std::u16string(buf.data()));
    if (strcmp(meta_tag, "CreationDate") == 0 ||
        strcmp(meta_tag, "ModDate") == 0) {
      text = fpdf::DateToRFC3399(text);
    }
    std::cout << " meta_tag >> " << meta_tag << ": " << text << std::endl;
  }
}
bool PdfWritePng(const std::string &img_name, void *buffer,
                 int width, int height, int stride) {
  // BGRA -> RGBA
//  auto buf = reinterpret_cast<uint8_t *>(buffer);
//  for (int r = 0; r < height; ++r) {
//    for (int c = 0; c < width; ++c) {
//      auto pixel = buf + (r*stride) + (c*4);
//      auto b = pixel[0];
//      pixel[0] = pixel[2];  // b = r
//      pixel[2] = b;         // r = b
//    }
//  }
//  return stbi_write_png(img_name.c_str(), width, height, 4, buf, stride) != 0;
    auto mat = cv::Mat(height, width, CV_8UC4, buffer);
    cv::imwrite(img_name, mat);
    return true;
}

static void test_0();
static void testBylib();

int main(int argc, char const *argv[]) {
   setbuf(stdout, NULL);
   //test_0();
   testBylib();
   return 0;
}

//
void testBylib(){

//    std::string str = "123abc456";
//    size_t idx;

//    // 使用 std::stoi 提取前面的数字
//    int number = std::stoi(str, &idx);

//    std::cout << "提取的数字: " << number << std::endl;
//    std::cout << "剩余的字符串: " << str.substr(idx) << std::endl;

    med_pdf::initLib();
    //
    FPDF_STRING test_doc = "/home/heaven7/heaven7/work/share_folder/tmp4/siqi/Desktop2/"
                           "甲状腺结节/document.pdf";
//    FPDF_STRING test_doc = "/home/heaven7/heaven7/work/share_folder/tmp4/siqi/Desktop/"
//                           "乳腺徐娜娜/document.pdf";
    med_pdf::PdfInfo info;
    med_pdf::readPdfInfo(test_doc, &info);
    //
    med_pdf::destroyLib();
}
static void test_0(){
    FPDF_STRING test_doc = "/home/heaven7/heaven7/work/share_folder/tmp4/siqi/Desktop/"
                           "乳腺徐娜娜/document.pdf";
    printf("test_doc: %s\n", test_doc);

    FPDF_InitLibrary();

    FPDF_DOCUMENT doc = FPDF_LoadDocument(test_doc, NULL);
    if (!doc) {
      unsigned long err = FPDF_GetLastError();
      fprintf(stderr, "load pdf error: error = %ld\n", err);
      // Load pdf docs unsuccessful: ...
    }else{
      PrintPdfMetaData(doc);//empty.
  //    FPDF_FORMFILLINFO form_callbacks = {0};
  //    form_callbacks.version = 2;
  //    FPDF_FORMHANDLE form = FPDFDOC_InitFormFillEnvironment(doc, &form_callbacks);
      //
      int pc = FPDF_GetPageCount(doc);
      printf("   pc = %d\n", pc);
      FPDF_PAGE page = FPDF_LoadPage(doc, 0);
      {
          //std::vector<char16_t> buf(cc);
          auto len = FPDF_GetPageLabel(doc, 0, NULL, 0);
          if(len){
              std::vector<char16_t> buf(len);
              FPDF_GetPageLabel(doc, 0, buf.data(), len);
              auto text = strings::FromUtf16(std::u16string(buf.data()));
              printf("FPDF_GetPageLabel: %s\n", text.data());
          }
      }
      int c = FPDFPage_CountObjects(page);
      for(int i : MFOREACH(c)){
          auto obj = FPDFPage_GetObject(page, i);
          for(int j : MFOREACH(FPDFPageObj_CountMarks(obj))){
              auto mark = FPDFPageObj_GetMark(obj, j);
              FPDFPageObjMark_CountParams(mark);
          }
          int type = FPDFPageObj_GetType(obj);
          switch (type) {
          case FPDF_PAGEOBJ_TEXT:{
              printf("i = %d, type = text\n", i);
              auto textPge = FPDFText_LoadPage(page);
              // get every text rect
              int rectC = FPDFText_CountRects(textPge, 0, -1);
              String content;
              double lastTop = -1;
              double lastLeft = -1;
              for(int k : MFOREACH(rectC)){
                  double left, top, right, bottom;
                  if(!FPDFText_GetRect(textPge, k, &left, &top, &right, &bottom)){
                      fprintf(stderr, "FPDFText_GetRect: error.\n");
                      abort();
                  }
                  //1M
                  std::vector<char16_t> buf(1 << 20);
                  //FPDFText_get
                  int actSize = FPDFText_GetBoundedText(textPge, left, top, right, bottom,
                                          (unsigned short*)buf.data(), buf.size());
                  if(actSize <= 0){
                      fprintf(stderr, "FPDFText_GetBoundedText: error.\n");
                      abort();
                  }
                  buf.resize(actSize);
                  auto text = strings::FromUtf16(std::u16string(buf.data()));
                  auto dleft = std::abs(lastLeft - left);
                  auto dtop = std::abs(lastTop - top);
                  printf("rect: left, top, right, bottom. text, d_left, d_top ="
                         " %.1f, %.1f, %.1f, %.1f, '%s', %.1f, %.1f\n",
                         left, top, right, bottom, text.data(),
                         dtop, dleft
                         );
  //                if(lastTop < 0){
  //                    lastTop = top;
  //                }else if(std::abs(lastTop - top) &&  < 1){
  //                    content += text;
  //                }else{

  //                }
                  lastLeft = left;
                  lastTop = top;
                  content += text;
              }
              printf("i = %d, content = '%s'\n", i, content.data());
              goto PDF_FINAL;
              //get whole text
  //            int cc = FPDFText_CountChars(textPge);
  //            std::vector<char16_t> buf(cc);
  //            int ret = FPDFText_GetText(textPge, 0, cc, (unsigned short*)buf.data());
  //            auto text = strings::FromUtf16(std::u16string(buf.data()));
  //            printf("FPDF_PAGEOBJ_TEXT >> %s\n", text.data());
  //            FPDFText_ClosePage(textPge);
          }break;

          case FPDF_PAGEOBJ_PATH:
              printf("i = %d, type = path\n", i);
              break;
          case FPDF_PAGEOBJ_IMAGE:{
              printf("i = %d, type = image\n", i);
              PdfImageHelper pih(obj);
              pih.saveImage(i);
          }break;

          case FPDF_PAGEOBJ_SHADING:
              printf("i = %d, type = shading\n", i);
              break;
          case FPDF_PAGEOBJ_FORM:
              printf("i = %d, type = form\n", i);
              break;

          case FPDF_PAGEOBJ_UNKNOWN:
              printf("i = %d, type = unknown\n", i);
              break;
          }
          printf("\n");
         // FPDFPageObj_Destroy(obj);
      }
PDF_FINAL:
      //
     // FPDFDOC_ExitFormFillEnvironment(form);
      FPDF_ClosePage(page);
      FPDF_CloseDocument(doc);
    }

    FPDF_DestroyLibrary();
}

