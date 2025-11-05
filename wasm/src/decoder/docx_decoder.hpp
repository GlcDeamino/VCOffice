#ifndef DOCX_DECODER_HPP
#define DOCX_DECODER_HPP

#include "WordEditor.hpp"
#include <QString>
#include <WordEditor.hpp>

enum class DocxDecodeStatus : int {
    SUCCESS = 0,
    FAIL_CREATE_TMP = 1,
    FAIL_DECOMP = 2,
    FAIL_OPEN_FILE = 3,
    FORMAT_ERROR = 4
};

namespace DocxDecoder {
    DocxDecodeStatus decode(QString p, WordEditor& s);
}

#endif