#ifndef WORD_EDITOR_HPP
#define WORD_EDITOR_HPP

#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <qcursor.h>
#include <qevent.h>
#include <qthread.h>
#include "AbstractEditor.hpp"
#include "fntabs/StartTab.hpp"
#include "OfficeItem.hpp"
#include "WordScene.hpp"
#include <QProgressBar>

class WordEditor;

class DecodeThread : public QThread {
    Q_OBJECT
public:
    DecodeThread(WordEditor* e) : editor(e) {};
    QString file;
    bool result = false;
    void run() override;
signals:
    void error(QString title, QString text);
private:
    WordEditor* editor;
};

class WordEditor : public AbstractEditor {
    Q_OBJECT
public:
    WordEditor();

    double scale = 1;
    virtual void load(QString p) override;
    void reformat();
    QVector<OfficeItem*>* m_item;
protected:
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
private:
    QProgressBar m_progress;
    QTabWidget m_fns;
    StartTab m_stt;
    QGraphicsView m_view;
    WordScene m_scene;
    QLabel m_mouse_pos;
    enum Status {
        NOFILE,
        DECODING,
        DECODE_ERR,
        FORMATTING,
        FORMAT_ERR,
        NORMAL
    };
    Status m_status;
    DecodeThread* dt;
public slots:
    void updateMousePos(QCursor cursor);
    void onFinishDecode();
    void decodeError(QString title, QString text);
};

#endif