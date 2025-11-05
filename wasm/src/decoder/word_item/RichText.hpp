#pragma once
#ifndef RICH_TEXT_HPP
#define RICH_TEXT_HPP

#include "OfficeItem.hpp"
#include "FontFlags.hpp"
#include "word_item/Paragraph.hpp"

class RichText : public OfficeItem {
public:
    RichText(Paragraph* para);
    virtual ~RichText();
    virtual void append(OfficeItem* item) override;
    FontFlags rPr;
    QString t;
    Paragraph* p;
};
#endif