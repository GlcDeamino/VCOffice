#ifndef WORD_EDITOR_HPP
#define WORD_EDITOR_HPP

#include <QWidget>
#include <QTabWidget>
#include <QResizeEvent>

class WordEditor : public QWidget {
    Q_OBJECT
public:
    WordEditor(QString p);
protected:
    virtual void resizeEvent(QResizeEvent *e) override;
private:
    QTabWidget m_fns;
};

#endif