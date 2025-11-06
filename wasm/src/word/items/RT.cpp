#include "RT.hpp"
#include "RunProperties.hpp"
#include "WordEditor.hpp"
#include "word_item/RichText.hpp"
#include <QPainter>
#include <qfontmetrics.h>


RT::RT(RichText* n, WordEditor* editor, QString s) :
    node(n), p_editor(editor), s(s) {

}

void RT::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!painter || !widget || widget->width() * widget->height() == 0 || !scene()) return;

    painter->save();
    RunProperties& rPr = node->rPr;

    QFont font;
    font.setFamily(rPr.font);
    font.setBold(rPr.bold);
    font.setItalic(rPr.italic);
    font.setPointSizeF(rPr.fontSize);

    QFontMetricsF fm(font);
    qreal ascent = fm.ascent();
    qreal descent = fm.descent();
    if (rPr.subscriptStatus != SubscriptStatus::NONE) {
        font.setPointSizeF(rPr.fontSize / 2);
    }
    qreal x = 0.5;
    qreal y = 0.5 + ascent; // 基线 = 左上角 y + ascent

    if (rPr.subscriptStatus == SubscriptStatus::UPPER) {
        y -= ascent / 2; // 上标：基线抬高
    } else if (rPr.subscriptStatus == SubscriptStatus::LOWER) {
        y += descent / 2; // 下标：基线降低
    }

    // 背景填充
    if (!(rPr.bgColor == Qt::transparent)) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(rPr.bgColor);
        painter->drawRect(boundingRect());
    }

    // 设置前景色和字体
    painter->setPen(rPr.fgColor);
    painter->setBrush(rPr.fgColor);
    painter->setFont(font);

    // 绘制文字（基线位置）
    painter->drawText(QPointF(x, y), s);

    // 删除线和下划线
    if (rPr.strikethrough) {
        qreal strikeY = y - fm.ascent() * 0.3;
        painter->drawLine(QLineF(x, strikeY, x + boundingRect().width(), strikeY));
    }
    if (rPr.underline == UnderlineStatus::SINGLE) {
        qreal underlineY = y + fm.descent() * 0.5;
        painter->drawLine(QLineF(x, underlineY, x + boundingRect().width(), underlineY));
    }

    painter->restore();
}

QRectF RT::boundingRect() const {
    RunProperties& rPr = node->rPr;
    QFont font;
    font.setFamily(rPr.font);
    font.setBold(rPr.bold);
    font.setItalic(rPr.italic);
    font.setPointSizeF(rPr.fontSize);

    QFontMetricsF fm(font);
    qreal h = fm.ascent() + fm.descent();

    if (rPr.subscriptStatus != SubscriptStatus::NONE) {
        font.setPointSizeF(rPr.fontSize / 2);
    }

    QFontMetricsF nfm(font);
    QRectF textBound = nfm.boundingRect(s);
    
    qreal w = textBound.width();

    return QRectF(0, 0, w + 1, h + 1); // +1 是为了 anti-aliasing 或边框留空间
}

qreal RT::strWidth(QString str, RunProperties rPr) {
    QFont font;
    font.setFamily(rPr.font);
    font.setBold(rPr.bold);
    font.setItalic(rPr.italic);
    font.setPointSizeF(rPr.fontSize);

    if (rPr.subscriptStatus != SubscriptStatus::NONE) {
        font.setPointSizeF(rPr.fontSize / 2);
    }

    QFontMetricsF fm(font);
    QRectF textBound = fm.boundingRect(str);
    
    qreal w = textBound.width();
    return w;
}

qreal RT::fontHeight(RunProperties rPr) {
    QFont font;
    font.setFamily(rPr.font);
    font.setBold(rPr.bold);
    font.setItalic(rPr.italic);
    font.setPointSizeF(rPr.fontSize);

    QFontMetricsF fm(font);
    qreal h = fm.ascent() + fm.descent() + fm.leading();
    return h;
}