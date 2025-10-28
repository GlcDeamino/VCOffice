#ifndef RT_HPP
#define RT_HPP

#include <QGraphicsItem>
#include "./../../common/FontFlags.hpp"

class RT : public QGraphicsItem {
public:
    RT();
    FontFlags f;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    QString ctx;
};

#endif