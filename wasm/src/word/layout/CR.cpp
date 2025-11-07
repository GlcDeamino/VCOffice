#include "CR.hpp"
#include "WordEditor.hpp"
#include <QPainter>
#include <qfontmetrics.h>
#include <qmath.h>


CR::CR(WordEditor* editor, bool isSoft) :
    p_editor(editor), m_is_soft(isSoft) {
        m_is_soft = isSoft;
}

void CR::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!painter || !widget || widget->width() * widget->height() == 0 || !scene()) return;

    painter->save();

    QFont font;
    qreal x = 0;
    qreal y = 0;

    font.setPixelSize(12);
    painter->setPen(QColor(100, 100, 100));
    painter->setFont(font);
    painter->drawText(QPointF(x, y), m_is_soft ? "↓" : "↵");

    painter->restore();
}

QRectF CR::boundingRect() const {
    QFont font;
    font.setPixelSize(12);

    QFontMetricsF fm(font);
    QRectF textBound = fm.boundingRect(m_is_soft ? "↓" : "↵");
    
    qreal w = textBound.width();
    qreal h = textBound.height();

    return QRectF(0, 0, w + 1, h + 1); // +1 是为了 anti-aliasing 或边框留空间
}