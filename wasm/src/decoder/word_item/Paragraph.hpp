#pragma once
#ifndef PARAGRAPH_HPP
#define PARAGRAPH_HPP

#include <QVector>
#include <QColor>
#include "OfficeItem.hpp"

enum class BorderStyle {
    NONE,
    SINGLE,
    THICK,
    DOUBLE,
    DOTTED,
    DASHED,
    // 可按需扩展
};

struct ParagraphBorder {
    BorderStyle style = BorderStyle::NONE;
    int size = 0;          // 单位：eighth of a point（1/8 pt），Word 常用
    QColor color;          // 边框颜色

    // 可选：提供一个是否有效的判断
    bool isValid() const { return style != BorderStyle::NONE && size > 0; }
};

class Paragraph : public OfficeItem {
public:
    Paragraph() = default;
    virtual ~Paragraph() {}
    virtual void append(OfficeItem* item) override;

    ParagraphBorder border;
    QColor fill;
};

#endif