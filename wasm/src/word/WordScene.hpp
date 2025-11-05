#pragma once
#ifndef WORD_SCENE_HPP
#define WORD_SCENE_HPP

#include <QGraphicsScene>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>

class WordScene : public QGraphicsScene {
    Q_OBJECT
protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
public:
    WordScene();
    QPointF mousePos;
    QCursor arrow;
    QCursor mirroredArrow;
signals:
    void mouseMoved(QCursor cursor);
};

#endif