#ifndef CR_HPP
#define CR_HPP

#include <QGraphicsItem>
#include "WordEditor.hpp"

class CR : public QGraphicsItem {
public:
    CR(WordEditor* editor, bool isSoft);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    WordEditor* p_editor;
private:
    bool m_is_soft;
};

#endif