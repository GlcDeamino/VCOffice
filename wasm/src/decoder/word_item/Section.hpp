#pragma once
#include <qmargins.h>
#ifndef SECTION_HPP
#define SECTION_HPP

#include "OfficeItem.hpp"
#include <QSizeF>

struct pgMagrin {
    qreal left;
    qreal top;
    qreal right;
    qreal bottom;
    qreal header;
    qreal footer;
    qreal gutter;
    pgMagrin(qreal l, qreal t, qreal r, qreal b, qreal h, qreal f, qreal g) : left(l), top(t), right(r), bottom(b), header(h), footer(f), gutter(g) {}
};

class Section : public OfficeItem {
public:
    virtual void append(OfficeItem* item) override;
    QSizeF pageSize = QSizeF(595.3, 841.9);
    pgMagrin pageMagrin = pgMagrin(90, 72, 90, 72, 42.55, 49.6, 0);
};

#endif