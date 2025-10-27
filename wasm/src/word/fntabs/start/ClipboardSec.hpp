#ifndef CLIPBOARD_SEC_HPP
#define CLIPBOARD_SEC_HPP

#include <QWidget>
#include <QPushButton>

class ClipboardSec : public QWidget {
    Q_OBJECT
public:
    ClipboardSec();
private:
    QPushButton cut;
    QPushButton cpy;
    QPushButton pst;
};

#endif