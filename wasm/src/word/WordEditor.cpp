#include "WordEditor.hpp"
#include "./../decoder/docx_decoder.hpp"


WordEditor::WordEditor(QString p) {
    if (p.endsWith(".docx")) {
        DocxDecoder::decode(p);
        m_fns.setStyleSheet("border: 1px;");
    }
}

void WordEditor::resizeEvent(QResizeEvent *e) {
    m_fns.setGeometry(0, 0, e->size().width(), e->size().height() / 10);
}