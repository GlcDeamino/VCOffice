#ifndef RTBUTTON_HPP
#define RTBUTTON_HPP

#include <qevent.h>
#include <QPushButton>
#include <QLabel>
class RTButton : public QPushButton {
public:
    RTButton();
    void setText(const QString& s);
    void resizeEvent(QResizeEvent* e);
private:
    QLabel m_l;
};

#endif