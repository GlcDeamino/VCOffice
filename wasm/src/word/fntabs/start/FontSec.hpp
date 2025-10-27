#ifndef FONT_SEC_HPP
#define FONT_SEC_HPP

#include <qevent.h>
#include "./../../../common/RTbutton.hpp"
#include <qtmetamacros.h>
#include <QWidget>
#include <QFontComboBox>
#include <QComboBox>
class FontSec : public QWidget {
    Q_OBJECT
public:
    FontSec();
    void resizeEvent(QResizeEvent* e) override;
private:
    QFontComboBox font_sel;
    QComboBox font_s_sel;
    RTButton font_s_inc;
    RTButton font_s_dec;
    RTButton bold;
    RTButton italic;
    RTButton underlined;
    RTButton strikethrough;
private slots:
    void font_sel_e();
};
#endif