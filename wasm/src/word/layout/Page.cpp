#include "Page.hpp"
#include "word_item/Section.hpp"
#include <QPainter>
#include <QWidget>
#include <QGraphicsScene>
#include <qnamespace.h>

Page::Page(Section* sec) : sec(sec) {
}

QRectF Page::boundingRect() const {
    return QRectF(0, 0, sec->pageSize.width(), sec->pageSize.height());
}

void Page::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!painter || !widget || widget->width() * widget->height() == 0 || !scene()) return;
    pgMagrin m = sec->pageMagrin;
    QSizeF s = sec->pageSize;
    qreal ws = s.width();
    qreal hs = s.height();
    qreal l = m.left;
    qreal t = m.top;
    qreal r = m.right;
    qreal b = m.bottom;
    qreal h = m.header;
    qreal f = m.footer;
    qreal g = m.gutter;


    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255));
    painter->drawRect(boundingRect());
    painter->restore();
    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QColor(100, 100, 100));
    painter->drawLine(l - 10, t + h, l, t + h);
    painter->drawLine(ws - r, t + h, ws - r + 10, t + h);
    painter->drawLine(l, t + h - 10, l, t + h);
    painter->drawLine(ws - r, t + h - 10, ws - r, t + h);
    painter->drawLine(l - 10, hs - f - b, l, hs - f - b);
    painter->drawLine(ws - r, hs - f - b, ws - r + 10, hs - f - b);
    painter->drawLine(l, hs - f - b, l, hs - f - b + 10);
    painter->drawLine(ws - r, hs - f - b, ws - r, hs - f - b + 10);
    painter->restore();
}