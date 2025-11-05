#pragma once


#include "word_item/Paragraph.hpp"
#include <QGraphicsItem>
class Para : public QGraphicsItem {
public:
    Para(Paragraph* sec);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    Paragraph* para;
    qreal w = 0;
    qreal h = 0;
};