#include "WordScene.hpp"
#include "isType.hpp"
#include "items/RT.hpp"
#include "layout/Page.hpp"
#include <qbitmap.h>
#include <qcursor.h>
#include <qgraphicsitem.h>
#include <qnamespace.h>
#include <QSvgRenderer>
#include <QFile>
#include <qpixmap.h>
#include <qtransform.h>
#include <stdexcept>

WordScene::WordScene() {
    QPixmap arr(32, 32);
    arr.fill(Qt::transparent);
    QByteArray b;
    QFile f(":/res/svg/CursorArrow.svg");
    if (!f.open(QFile::OpenModeFlag::ReadOnly)) {
        throw std::runtime_error("Resource \":/res/svg/CursorArrow.svg\" not found.");
    }
    b = f.readAll();
    QSvgRenderer s;
    s.load(b);
    QPainter p(&arr);
    s.render(&p, arr.rect());
    QTransform t;
    t.scale(-0.5, 0.5);
    QPixmap marr = arr.transformed(t);
    QTransform at;
    at.scale(0.5, 0.5);
    QPixmap narr = arr.transformed(at);
    arrow = QCursor(narr, 0, 0);
    mirroredArrow = QCursor(marr, 16, 0);
}


void WordScene::mouseMoveEvent(QGraphicsSceneMouseEvent *e) { 
    mousePos = e->scenePos();
    QGraphicsScene::mouseMoveEvent(e);
    QGraphicsItem* hoveringItem = itemAt(mousePos, QTransform());
    QCursor cursor;
    if (isType<RT>(hoveringItem)) {
        cursor.setShape(Qt::CursorShape::IBeamCursor);
    } else if (isType<Page>(hoveringItem)) {
        if (mousePos.x() <= static_cast<Page*>(hoveringItem)->sec->pageMagrin.left) {
            cursor = mirroredArrow;
        } else {
            cursor = arrow;
        }
    } else {
        cursor.setShape(Qt::CursorShape::ArrowCursor);
    }
    emit mouseMoved(cursor);
}