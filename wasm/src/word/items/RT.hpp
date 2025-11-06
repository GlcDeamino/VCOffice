#ifndef RT_HPP
#define RT_HPP

#include <QGraphicsItem>
#include "RunProperties.hpp"
#include "WordEditor.hpp"
#include "word_item/RichText.hpp"

class RT : public QGraphicsItem {
public:
    RT(RichText* n, WordEditor* editor, QString s);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QRectF boundingRect() const override;
    RichText* node;
    WordEditor* p_editor;
    QString s;
    static qreal strWidth(QString str, RunProperties rPr);
    static qreal fontHeight(RunProperties rPr);
};

#endif