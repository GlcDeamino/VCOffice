#pragma once


#include <QWidget>

class AbstractEditor : public QWidget {
public:
    AbstractEditor() {};
    virtual bool load(QString p) = 0;
};