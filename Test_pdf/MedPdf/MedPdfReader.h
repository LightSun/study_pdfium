#pragma once

#include "MedPdf/med_pdf_ctx.h"

namespace med_pdf {

void initLib();
void destroyLib();
bool readPdfInfo(CString file, PdfInfo* pi);

}
