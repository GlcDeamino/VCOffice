#ifndef WORD_EDITOR_HPP
#define WORD_EDITOR_HPP

#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <qcursor.h>
#include <qevent.h>
#include "AbstractEditor.hpp"
#include "fntabs/StartTab.hpp"
#include "OfficeItem.hpp"
#include "WordScene.hpp"

class WordEditor : public AbstractEditor {
    Q_OBJECT
public:
    WordEditor();

    void append(OfficeItem* ctx);
    double scale = 1;
    bool load(QString p) override;
protected:
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    void reformat();
private:
    QTabWidget m_fns;
    StartTab m_stt;
    QGraphicsView m_view;
    WordScene m_scene;
    QVector<OfficeItem*> m_item;
    QLabel m_mouse_pos;
public slots:
    void updateMousePos(QCursor cursor);
};

#endif