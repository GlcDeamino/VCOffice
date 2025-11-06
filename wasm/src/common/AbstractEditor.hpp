#pragma once


#include <QWidget>

class AbstractEditor : public QWidget {
public:
    AbstractEditor() {};
    virtual void load(QString p) = 0;
};