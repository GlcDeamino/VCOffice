#ifndef DOCX_DECODER_HPP
#define DOCX_DECODER_HPP

#include <QString>

enum class DocxDecodeStatus : int {
    SUCCESS = 0,
    FAIL_CREATE_TMP = 1,
    FAIL_DECOMP = 2
};

namespace DocxDecoder {
    DocxDecodeStatus decode(QString p);
}

#endif