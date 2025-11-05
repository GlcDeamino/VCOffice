#ifndef CLIPBOARD_SEC_HPP
#define CLIPBOARD_SEC_HPP

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <qevent.h>

class ClipboardSec : public QWidget {
    Q_OBJECT
public:
    ClipboardSec();
    void resizeEvent(QResizeEvent* e) override;
private:
    QPushButton cut;
    QPushButton cpy;
    QPushButton pst;
    QIcon cut_i;
    QIcon cpy_i;
    QIcon pst_i;
    QMenu pst_m;
    QAction pst_a_k;
    QAction pst_a_m;
    QAction pst_a_t;
};

#endif