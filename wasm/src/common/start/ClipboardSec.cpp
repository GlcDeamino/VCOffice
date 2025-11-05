#include "ClipboardSec.hpp"
#include <qevent.h>
#include <QPainter>
#include <qpainter.h>
#include <qwindowdefs.h>


ClipboardSec::ClipboardSec() {
    cut_i = QIcon(":/res/svg/content-cut.svg");
    cpy_i = QIcon(":/res/svg/content-copy.svg");
    pst_i = QIcon(":/res/svg/content-paste.svg");
    cut_i.setIsMask(true);
    cpy_i.setIsMask(true);
    pst_i.setIsMask(true);
    cut.setParent(this);
    cpy.setParent(this);
    pst.setParent(this);
    pst_a_k.setText("Keep format");
    pst_a_m.setText("Merge format");
    pst_a_t.setText("Raw text");
    pst_m.addAction(&pst_a_k);
    pst_m.addAction(&pst_a_m);
    pst_m.addAction(&pst_a_t);
    pst.setMenu(&pst_m);
}

void ClipboardSec::resizeEvent(QResizeEvent* e) {
    QSize s = e->size();
    size_t w = s.width();
    size_t h = s.height();
    if (!(w * h)) {
        return;
    } 
    pst.setGeometry(0, 0, w / 3 * 2, h);
    cut.setGeometry(w / 3 * 2, 0, w / 3, h / 2);
    cpy.setGeometry(w / 3 * 2, h / 2, w / 3, h / 2);
    QPixmap pst_p = pst_i.pixmap(QSize(std::min(w / 2, h / 4 * 3), std::min(w / 2, h / 4 * 3)));
    QPixmap cut_p = cut_i.pixmap(QSize(std::min(w / 4, h / 3), std::min(w / 4, h / 3)));
    QPixmap cpy_p = cpy_i.pixmap(QSize(std::min(w / 4, h / 3), std::min(w / 4, h / 3)));
    QPainter pst_pa(&pst_p);
    QPainter cut_pa(&cut_p);
    QPainter cpy_pa(&cpy_p);
    pst_pa.setCompositionMode(QPainter::CompositionMode_SourceIn);
    cut_pa.setCompositionMode(QPainter::CompositionMode_SourceIn);
    cpy_pa.setCompositionMode(QPainter::CompositionMode_SourceIn);
    pst_pa.fillRect(pst_p.rect(), pst.palette().color(pst.foregroundRole())); 
    cut_pa.fillRect(cut_p.rect(), cut.palette().color(cut.foregroundRole())); 
    cpy_pa.fillRect(cpy_p.rect(), cpy.palette().color(cpy.foregroundRole())); 
    pst_pa.end();
    cut_pa.end();
    cpy_pa.end();
    pst.setIcon(QIcon(pst_p));
    cut.setIcon(QIcon(cut_p));
    cpy.setIcon(QIcon(cpy_p));
    pst.setIconSize(QSize(std::min(w / 2, h / 4 * 3), std::min(w / 2, h / 4 * 3)));
    cut.setIconSize(QSize(std::min(w / 4, h / 3), std::min(w / 4, h / 3)));
    cpy.setIconSize(QSize(std::min(w / 4, h / 3), std::min(w / 4, h / 3)));
}