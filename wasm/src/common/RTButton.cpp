#include "RTButton.hpp"
#include <qevent.h>
#include <qnamespace.h>


RTButton::RTButton() {
    m_l.setParent(this);
    m_l.setTextFormat(Qt::RichText);
    m_l.setAlignment(Qt::AlignCenter);
    m_l.setAttribute(Qt::WA_TranslucentBackground);
}

void RTButton::setText(const QString& s) {
    m_l.setText(s);
}

void RTButton::resizeEvent(QResizeEvent* e) {
    QSize s = e->size();
    m_l.setGeometry(0, 0, s.width() - 1, s.height() - 1);
    QFont f;
    int fs = std::min(s.width(), s.height());
    if (fs >= 1) {
        f.setPixelSize(fs);
    }
    f.setFamily("JetBrains Mono");
    m_l.setFont(f);
}