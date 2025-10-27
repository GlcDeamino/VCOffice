#include "FontSec.hpp"
#include <qevent.h>
#include <qfontcombobox.h>
#include <qfontdatabase.h>
#include <qnamespace.h>
#include <QStyledItemDelegate>


FontSec::FontSec() {
    font_sel.setParent(this);
    font_s_sel.setParent(this);
    font_s_inc.setParent(this);
    font_s_dec.setParent(this);
    bold.setParent(this);
    bold.setCheckable(true);
    italic.setParent(this);
    italic.setCheckable(true);
    underlined.setParent(this);
    underlined.setCheckable(true);
    strikethrough.setParent(this);
    strikethrough.setCheckable(true);
    font_s_inc.setText("<code>+</code>");
    font_s_dec.setText("<code>-</code>");
    bold.setText("<code><b>B</b></code>");
    italic.setText("<code><i>I</i></code>");
    underlined.setText("<code><u>U</u></code>");
    strikethrough.setText("<code><s>A</s></code>");
    connect(&font_sel, &QFontComboBox::currentFontChanged, this, &FontSec::font_sel_e);
    font_sel.setCurrentFont(QFont("宋体"));
    QStringList fontSizes = {
        "初号", "小初",
        "一号", "小一",
        "二号", "小二",
        "三号", "小三",
        "四号", "小四",
        "五号", "小五",
        "六号", "小六",
        "七号", "八号",
        "5", "5.5",
        "6.5", "7.5",
        "8", "9",
        "10", "10.5",
        "11", "12",
        "14", "16",
        "18", "20",
        "22", "24", 
        "26", "28",
        "36", "48",
        "72"
    };
    font_s_sel.addItems(fontSizes);
    font_s_sel.setCurrentText("10.5");
    font_s_sel.setEditable(true);
}

void FontSec::resizeEvent(QResizeEvent* e) {
    size_t w = e->size().width();
    size_t h = e->size().height();
    size_t wu = w / 10;
    size_t hu = h / 2;
    font_sel.setGeometry(0, 0, wu * 6, hu);
    font_s_sel.setGeometry(wu * 6, 0, wu * 2, hu);
    font_s_inc.setGeometry(wu * 8, 0, wu, hu);
    font_s_dec.setGeometry(wu * 9, 0, wu, hu);
    bold.setGeometry(0, hu, wu, hu);
    italic.setGeometry(wu, hu, wu, hu);
    underlined.setGeometry(wu * 2, hu, wu, hu);
    strikethrough.setGeometry(wu * 3, hu, wu, hu);
}

void FontSec::font_sel_e() {
    font_sel.setFont(font_sel.currentFont());
}