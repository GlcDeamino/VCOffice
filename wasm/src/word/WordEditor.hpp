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
#include <QTimer>

class DecodeThread;

class WordEditor : public AbstractEditor {
    Q_OBJECT
public:
    WordEditor();

    void append(OfficeItem* ctx);
    double scale = 1;
    virtual void load(QString p) override;
    void reformat();
protected:
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
private:
    QTabWidget m_fns;
    StartTab m_stt;
    QGraphicsView m_view;
    WordScene m_scene;
    QVector<OfficeItem*> m_item;
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
    QTimer* m_loadingTimer = nullptr;
    int m_loadingFrameIndex = 0;
    const QStringList m_loadingChars = {"⠇⠀", "⠋⠀",  "⠉⠁",  "⠈⠉", 
                                        "⠀⠙", "⠀⠸",  "⠀⢰",  "⠀⣠", 
                                        "⢀⣀", "⣀⡀", "⣄⠀", "⡆⠀"};
public slots:
    void updateMousePos(QCursor cursor);
    void onFinishDecode();
private slots:
    void updateLoadingAnimation();
};

#endif