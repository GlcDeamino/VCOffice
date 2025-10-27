#include "StartTab.hpp"
#include <qevent.h>


StartTab::StartTab() {
    clipboard_sec.setParent(this);
    font_sec.setParent(this);
}

void StartTab::resizeEvent(QResizeEvent* e) {
    size_t wu = e->size().width() / 8;
    size_t h = e->size().height();
    clipboard_sec.setGeometry(0, 0, wu, h);
    font_sec.setGeometry(wu, 0, wu * 3, h);
    font_sec.resize(wu * 3, h);
}