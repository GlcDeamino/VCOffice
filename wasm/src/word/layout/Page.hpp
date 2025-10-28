#ifndef PAGE_HPP
#define PAGE_HPP

#include <QGraphicsItem>
class Section;

class Page : public QGraphicsItem {
public:
    Page();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    Section* sec;
};

#endif