#ifndef START_TAB_HPP
#define START_TAB_HPP

#include <qevent.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "start/ClipboardSec.hpp"
#include "start/FontSec.hpp"
class StartTab : public QWidget {
    Q_OBJECT
public:
    StartTab();
    void resizeEvent(QResizeEvent* e) override;
private:
    ClipboardSec clipboard_sec;
    FontSec font_sec;
};
#endif