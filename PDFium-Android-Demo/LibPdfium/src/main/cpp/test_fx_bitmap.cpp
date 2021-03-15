//
// Created by Administrator on 2021/3/10 0010.
//

#include <PDFIUM/core/fxge/agg/fx_agg_driver.h>
#include <PDFIUM/core/fxge/cfx_pathdata.h>
#include <PDFIUM/core/fxge/cfx_renderdevice.h>
#include <PDFIUM/core/fxge/cfx_defaultrenderdevice.h>
#include <PDFIUM/core/fxge/dib/cfx_dibitmap.h>
#include <PDFIUM/core/fpdfapi/render/cpdf_textrenderer.h>
#include <PDFIUM/core/fpdfapi/render/cpdf_charposlist.h>
#include <PDFIUM/core/fxge/fx_font.h>
#include <PDFIUM/core/fxge/text_char_pos.h>
#include <PDFIUM/core/fxge/text_glyph_pos.h>
#include <PDFIUM/core/fpdfapi/page/cpdf_textobject.h>
#include "PDFIUM/core/fpdfapi/render/cpdf_renderoptions.h"
#include "util.h"
#include "android/bitmap.h"

#include "TestFont.h"

static CFX_Font* GetFont(CPDF_Font* pFont, int32_t position) {
    return position == -1 ? pFont->GetFont() : pFont->GetFontFallback(position);
}
static bool DrawNormalText(CFX_RenderDevice* pDevice,
                           const std::vector<uint32_t>& charCodes,
                           const std::vector<float>& charPos,
                           CPDF_Font* pFont,
                           float font_size,
                           const CFX_Matrix& mtText2Device,
                           FX_ARGB fill_argb,
                           const CPDF_RenderOptions& options);

static void DrawTextString(CFX_RenderDevice* pDevice,
                                       float origin_x,
                                       float origin_y,
                                       CPDF_Font* pFont,
                                       float font_size,
                                       const CFX_Matrix& matrix,
                                       const ByteString& str,
                                       FX_ARGB fill_argb,
                                       const CPDF_RenderOptions& options) {
    if (pFont->IsType3Font())
        return;

    int nChars = pFont->CountChar(str.AsStringView());
    if (nChars <= 0)
        return;

    size_t offset = 0;
    std::vector<uint32_t> codes;
    std::vector<float> positions;
    codes.resize(nChars);
    positions.resize(nChars - 1);
    float cur_pos = 0;
    for (int i = 0; i < nChars; i++) {
        codes[i] = pFont->GetNextChar(str.AsStringView(), &offset);
        if (i)
            positions[i - 1] = cur_pos;
        cur_pos += pFont->GetCharWidthF(codes[i]) * font_size / 1000;
    }
    CFX_Matrix new_matrix = matrix;
    new_matrix.e = origin_x;
    new_matrix.f = origin_y;
    DrawNormalText(pDevice, codes, positions, pFont, font_size, new_matrix,
                   fill_argb, options);
}

// static
static bool DrawNormalText(CFX_RenderDevice* pDevice,
                                       const std::vector<uint32_t>& charCodes,
                                       const std::vector<float>& charPos,
                                       CPDF_Font* pFont,
                                       float font_size,
                                       const CFX_Matrix& mtText2Device,
                                       FX_ARGB fill_argb,
                                       const CPDF_RenderOptions& options) {
    const CPDF_CharPosList CharPosList(charCodes, charPos, pFont, font_size);
    const std::vector<TextCharPos>& pos = CharPosList.Get();
    if (pos.empty())
        return true;

    int fxge_flags = 0;
    if (options.GetOptions().bClearType) {
        fxge_flags |= FXTEXT_CLEARTYPE;
        if (options.GetOptions().bBGRStripe)
            fxge_flags |= FXTEXT_BGR_STRIPE;
    }
    if (options.GetOptions().bNoTextSmooth)
        fxge_flags |= FXTEXT_NOSMOOTH;
    if (options.GetOptions().bPrintGraphicText)
        fxge_flags |= FXTEXT_PRINTGRAPHICTEXT;
    if (options.GetOptions().bNoNativeText)
        fxge_flags |= FXTEXT_NO_NATIVETEXT;
    if (options.GetOptions().bPrintImageText)
        fxge_flags |= FXTEXT_PRINTIMAGETEXT;

   /* if (pFont->IsCIDFont())
        fxge_flags |= FXFONT_CIDFONT;*/

    bool bDraw = true;
    int32_t fontPosition = pos[0].m_FallbackFontPosition;
    size_t startIndex = 0;
    for (size_t i = 0; i < pos.size(); ++i) {
        int32_t curFontPosition = pos[i].m_FallbackFontPosition;
        if (fontPosition == curFontPosition)
            continue;

        CFX_Font* font = GetFont(pFont, fontPosition);
        if (!pDevice->DrawNormalText(i - startIndex, &pos[startIndex], font,
                                     font_size, mtText2Device, fill_argb,
                                     fxge_flags)) {
            bDraw = false;
        }
        fontPosition = curFontPosition;
        startIndex = i;
    }
    CFX_Font* font = GetFont(pFont, fontPosition);
    if (!pDevice->DrawNormalText(pos.size() - startIndex, &pos[startIndex], font,
                                 font_size, mtText2Device, fill_argb,
                                 fxge_flags)) {
        bDraw = false;
    }
    return bDraw;
}

static void drawPath(JNIEnv *env, CFX_DefaultRenderDevice *device, jobject jBitmap) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, jBitmap, &info)) < 0) {
        LOGE("Fetching bitmap info failed: %s", strerror(ret * -1));
        return;
    }
    if (info.format != AndroidBitmapFormat::ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("bitmap format only support rgba 8888");
        return;
    }

    auto bitmap = pdfium::MakeRetain<CFX_DIBitmap>();
    if (!bitmap->Create(info.width, info.height, FXDIB_Argb)) {
        LOGE("create bitmap failed");
        return;
    }
    // auto backdrop = pdfium::MakeRetain<CFX_DIBitmap>();
    // CFX_DefaultRenderDevice bitmap_device;
    //device->Attach(bitmap, false, backdrop, true);
    device->Attach(bitmap, false, nullptr, false);
    // stroke: 画笔宽度的中心点是坐标。 所以在(100, 0) 处开始绘制stroke = 10.时， 最左边的渲染坐标是100 - 10/2
    CFX_PointF p1 = {100, 0};
    CFX_PointF p2 = {100, 300};
    CFX_Matrix matrix;
    auto color = ArgbEncode(0xff, 0xff, 0, 0);
    device->DrawStrokeLine(&matrix, p1, p2, color, 10);
    //-- draw text
    CPDF_TextObject textObj;

    ByteString str("我A。+6**(");
    textObj.SetText(str);
    color = ArgbEncode(0xff, 0, 0xff, 0);
    CPDF_RenderOptions ro;
    ro.SetColorMode(CPDF_RenderOptions::kNormal);
   // CPDF_TextRenderer::DrawTextString(device, 100, 300, )
   //How to get font?
   // DrawTextString(device, 100, 300, textObj.GetFont().Get(), 20, matrix, str, color, ro);

    // argb
    uint8_t *buffer = bitmap->GetBuffer();
    uint8_t *addr; //android bitmap native is rgba
    AndroidBitmap_lockPixels(env, jBitmap, reinterpret_cast<void **>(&addr));
    memcpy(addr, buffer, info.width * info.height * 4); //argb
    AndroidBitmap_unlockPixels(env, jBitmap);
}

extern "C" void initLibraryIfNeed();

extern "C"
JNIEXPORT void JNICALL
Java_com_heaven7_android_pdfium_TestFx_nInit(JNIEnv *env, jclass clazz) {
    initLibraryIfNeed();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_heaven7_android_pdfium_TestFx_nDraw(JNIEnv *env, jclass clazz, jobject bitmap) {
    CFX_DefaultRenderDevice device;
    drawPath(env, &device, bitmap);
}