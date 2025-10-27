#include "WordEditor.hpp"
#include "./../decoder/docx_decoder.hpp"
#include <qwidget.h>


WordEditor::WordEditor(QString p) {
    if (p.endsWith(".docx")) {
        DocxDecoder::decode(p);
        m_fns.setParent(this);
        m_fns.addTab(static_cast<QWidget*>(&m_stt), "Start");
    }
}

void WordEditor::resizeEvent(QResizeEvent *e) {
    m_fns.setGeometry(0, 0, e->size().width(), e->size().height() / 8);
}