//
// Created by Administrator on 2021/3/15 0015.
//

#ifndef PDFIUM_ANDROID_DEMO_TESTFONT_H
#define PDFIUM_ANDROID_DEMO_TESTFONT_H

#include <vector>

#include "../PDFIUM/core/fxge/cfx_font.h"
#include "../PDFIUM/core/fxcrt/string_view_template.h"
#include "../PDFIUM/core/fpdfapi/font/cpdf_cidfont.h"
#include "../PDFIUM/core/fpdfapi/font/cpdf_cmapmanager.h"
#include "../PDFIUM/core/fpdfapi/font/cpdf_fontglobals.h"
#include "../PDFIUM/core/fxcrt/retain_ptr.h"
#include "../PDFIUM/core/fxcrt/unowned_ptr.h"
#include "../PDFIUM/core/fxcrt/fx_memory_wrappers.h"
#include "../PDFIUM/core/fpdfapi/font/cpdf_cmap.h"
#include "../PDFIUM/core/fpdfapi/parser/cpdf_dictionary.h"
#include "../PDFIUM/third_party/base/span.h"
#include "../PDFIUM/core/fxge/cfx_gemodule.h"
#include "../PDFIUM/core/fxge/cfx_fontmgr.h"

/*#ifndef size_t
typedef unsigned int size_t;
#endif*/

#ifndef uint8_t
typedef unsigned char uint8_t;
#endif

class TestFont{
public:
    CFX_Font* GetFont() { return &m_Font; }
    const CFX_Font* GetFont() const { return &m_Font; }
    size_t CountChar(ByteStringView pString) const{
        return m_pCMap->CountChar(pString);
    }
    void LoadGB2312() {
       // m_BaseFontName = m_pFontDict->GetStringFor("BaseFont");
        m_Charset = CIDSET_GB1;

        CPDF_CMapManager* manager = CPDF_FontGlobals::GetInstance()->GetCMapManager();
        m_pCMap = manager->GetPredefinedCMap("GBK-EUC-H");
        m_pCID2UnicodeMap = manager->GetCID2UnicodeMap(m_Charset);
        /*const CPDF_Dictionary* pFontDesc = m_pFontDict->GetDictFor("FontDescriptor");
        if (pFontDesc)
            LoadFontDescriptor(pFontDesc);

        if (!IsEmbedded())
            LoadSubstFont();
        CheckFontMetrics();
        m_bAnsiWidthsFixed = true;*/
    }

    //load font from raw data. src_span contains 2: data and size.
    bool LoadEmbedded(pdfium::span<const uint8_t> src_span,
                                bool bForceAsVertical) {
        if (bForceAsVertical)
            m_bVertical = true;
        m_FontDataAllocation = std::vector<uint8_t, FxAllocAllocator<uint8_t>>(
                src_span.begin(), src_span.end());
        m_Face = CFX_GEModule::Get()->GetFontMgr()->NewFixedFace(
                nullptr, m_FontDataAllocation, 0);
        m_bEmbedded = true;
        m_FontData = m_FontDataAllocation;
        return !!m_Face;
    }

private:
    CFX_Font m_Font;
    CIDSet m_Charset;
    RetainPtr<const CPDF_CMap> m_pCMap;
    UnownedPtr<const CPDF_CID2UnicodeMap> m_pCID2UnicodeMap;

    bool m_bVertical;
    std::vector<uint8_t, FxAllocAllocator<uint8_t>> m_FontDataAllocation;
    std::vector<uint8_t, FxAllocAllocator<uint8_t>> m_FontData;
    RetainPtr<CFX_Face> m_Face;
    bool m_bEmbedded;
};

#endif //PDFIUM_ANDROID_DEMO_TESTFONT_H
