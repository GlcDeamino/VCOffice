#include "Para.hpp"
#include "word_item/Paragraph.hpp"
#include <QPainter>
#include <QWidget>
#include <QGraphicsScene>
#include <qnamespace.h>

Para::Para(Paragraph* para) : para(para) {
}

QRectF Para::boundingRect() const {
    return QRectF(0, 0, w, h);
}

void Para::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!painter || !widget || widget->width() * widget->height() == 0 || !scene()) return;
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(para->border.color);
    painter->drawRect(boundingRect());
    painter->restore();
}