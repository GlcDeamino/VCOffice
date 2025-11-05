#include "WordEditor.hpp"
#include "OfficeItem.hpp"
#include "WordScene.hpp"
#include "isType.hpp"
#include "layout/Para.hpp"
#include "word_item/LF.hpp"
#include "word_item/Paragraph.hpp"
#include "word_item/RichText.hpp"
#include "word_item/Section.hpp"
#include <docx_decoder.hpp>
#include "items/RT.hpp"
#include "layout/CR.hpp"
#include "layout/Page.hpp"
#include <qcursor.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <qwidget.h>


WordEditor::WordEditor() {
    m_fns.setParent(this);
    m_fns.addTab(static_cast<QWidget*>(&m_stt), "Start");
    m_view.setParent(this);
    m_view.setScene(&m_scene);
    m_scene.setBackgroundBrush(palette().base());
    m_mouse_pos.setParent(this);
    setMouseTracking(true);
    m_fns.setMouseTracking(true);
    m_view.setMouseTracking(true);
    m_mouse_pos.setMouseTracking(true);
}

bool WordEditor::load(QString p) {
    setCursor(QCursor(Qt::CursorShape::WaitCursor));
    connect(&m_scene, &WordScene::mouseMoved, this, &WordEditor::updateMousePos);
    if (DocxDecoder::decode(p, (*this)) != DocxDecodeStatus::SUCCESS) {
        return false;
    }
    reformat();
    setCursor(QCursor(Qt::CursorShape::ArrowCursor));
    return true;
}

void WordEditor::append(OfficeItem* ctx) {
    m_item.append(ctx);
}

void WordEditor::resizeEvent(QResizeEvent *e) {
    QSize s = e->size();
    double w = s.width();
    double h = s.height();
    double hu = h / 32;
    m_fns.setGeometry(0, 0, w, hu * 4);
    m_view.setGeometry(0, hu * 4, w, hu * 27);
    m_mouse_pos.setGeometry(0, hu * 31, w, hu);
}

void WordEditor::mouseMoveEvent(QMouseEvent *e) {
    if (!m_view.geometry().contains(e->position().toPoint())) {
        m_mouse_pos.setText("");
        setCursor(QCursor());
    }
}

void WordEditor::updateMousePos(QCursor cursor) {
    m_mouse_pos.setText(QString("x: %1, y: %2").arg(m_scene.mousePos.x()).arg(m_scene.mousePos.y()));
    setCursor(cursor);
}

void WordEditor::reformat() {
    m_scene.clear();

    // 初始化页面相关变量
    qreal defaultLineHeight = 12.0;  // 默认行高
    bool showFormattingMarks = true; // 显示格式标记开关
    qreal totalHeight = 0;

    for (size_t i = 0; i < m_item.length(); /* no i++ here */) {
        if (m_item[i] == nullptr) {
            m_item.removeAt(i);
            continue; // don't increment i
        }

        if (isType<Section>(m_item[i])) {
            Section* curr_sec = static_cast<Section*>(m_item[i]);
            qreal topUsed = curr_sec->pageMagrin.top + curr_sec->pageMagrin.header;
            qreal bottomUsed = curr_sec->pageMagrin.bottom + curr_sec->pageMagrin.footer;
            qreal pageHeight = curr_sec->pageSize.height();
            qreal pageWidth = curr_sec->pageSize.width();
            qreal leftMargin = curr_sec->pageMagrin.left;
            qreal rightMargin = curr_sec->pageMagrin.right;
            qreal availableWidth = pageWidth - leftMargin - rightMargin;

            QPointF cursor(leftMargin, topUsed);
            qreal pageUsed = topUsed + bottomUsed; // already used by margins

            Page* currentPage = new Page(curr_sec);
            currentPage->setZValue(-3);
            currentPage->setPos(0, 0);
            m_scene.addItem(currentPage);

            for (size_t j = 0; j < curr_sec->len(); j++) {
                if (isType<Paragraph>((*curr_sec)[j])) {
                    Paragraph* curr_para = static_cast<Paragraph*>((*curr_sec)[j]);
                    Para* para = new Para(curr_para);
                    para->setZValue(-2);
                    para->setPos(leftMargin, cursor.y());
                    para->w = availableWidth;

                    // Line state
                    QString currentLine;
                    qreal currentLineHeight = 0.0;
                    const RichText* currentRun = nullptr;
                    qreal lastLineEndX = leftMargin;

                    auto flushLine = [&]() {
                        if (!currentLine.isEmpty() && currentRun) {
                            RT* rt = new RT(const_cast<RichText*>(currentRun), this, currentLine);
                            rt->setPos(QPointF(leftMargin, cursor.y()));
                            rt->setZValue(0);
                            m_scene.addItem(rt);

                            cursor.ry() += currentLineHeight;
                            para->h += currentLineHeight;
                            pageUsed += currentLineHeight;

                            currentLine.clear();
                            currentLineHeight = 0.0;
                            currentRun = nullptr;
                        }
                    };

                    for (size_t k = 0; k < curr_para->len(); k++) {
                        OfficeItem* item = (*curr_para)[k];

                        if (isType<LF>(item)) {
                            CR* cr = new CR(this);
                            cr->setPos(lastLineEndX, cursor.y());
                            cr->setZValue(-1);
                            m_scene.addItem(cr);
                            flushLine();
                            qreal h = RT::fontHeight(static_cast<LF*>(item)->rPr);
                            cursor.ry() += h;
                            pageUsed += h;
                            cursor.setX(leftMargin); // 重置 x 到左边界
                            // Check if we need a new page
                            if (pageUsed > pageHeight) {
                                totalHeight += currentPage->sec->pageSize.height() + 10;
                                // Create new page and reset cursor
                                currentPage = new Page(curr_sec);
                                currentPage->setZValue(-2);
                                currentPage->setPos(0, totalHeight);
                                m_scene.addItem(currentPage);

                                // Reset cursor for new page
                                cursor.setY(topUsed + totalHeight);
                                pageUsed = topUsed + bottomUsed;
                            }
                            cursor.setX(leftMargin);
                        } else if (isType<RichText>(item)) {
                            RichText* rt_node = static_cast<RichText*>(item);
                            qreal charH = RT::fontHeight(rt_node->rPr);
                            for (QChar ch : rt_node->t) {
                                QString test = currentLine + ch;
                                if (!currentLine.isEmpty() &&
                                    RT::strWidth(test, rt_node->rPr) > availableWidth) {
                                    flushLine();
                                    // Start new line with this char
                                    currentLine = ch;
                                    currentLineHeight = charH;
                                    currentRun = rt_node;
                                    // Check if we need a new page
                                    if (pageUsed > pageHeight) {
                                        totalHeight += currentPage->sec->pageSize.height() + 10;
                                        // Create new page and reset cursor
                                        currentPage = new Page(curr_sec);
                                        currentPage->setZValue(-2);
                                        currentPage->setPos(0, totalHeight);
                                        m_scene.addItem(currentPage);

                                        // Reset cursor for new page
                                        cursor.setY(topUsed + totalHeight);
                                        pageUsed = topUsed + bottomUsed;
                                    }
                                    cursor.setX(leftMargin);
                                } else {
                                    currentLine += ch;
                                    currentLineHeight = qMax(currentLineHeight, charH);
                                    currentRun = rt_node;
                                }
                                lastLineEndX = leftMargin + RT::strWidth(currentLine, rt_node->rPr);
                            }
                        }
                    }

                    // Flush last line of paragraph
                    flushLine();
                    m_scene.addItem(para);

                    // Add paragraph mark "↵" at the end of paragraph
                    if (showFormattingMarks) {
                        CR* cr = new CR(this);
                        cr->setPos(lastLineEndX, cursor.y() - currentLineHeight);
                        cr->setZValue(-1);
                        m_scene.addItem(cr);
                    }

                    // Move to next paragraph position
                    qreal paraSpace = 6.0; // or from pPr->spacing->after
                    cursor.ry() += paraSpace;
                    pageUsed += paraSpace;

                    // Check if we need a new page
                    if (pageUsed > pageHeight) {
                        totalHeight += currentPage->sec->pageSize.height() + 10;
                        // Create new page and reset cursor
                        currentPage = new Page(curr_sec);
                        currentPage->setZValue(-2);
                        currentPage->setPos(0, totalHeight);
                        m_scene.addItem(currentPage);

                        // Reset cursor for new page
                        cursor.setY(topUsed + totalHeight);
                        pageUsed = topUsed + bottomUsed;
                    }
                    cursor.setX(leftMargin);
                }
            }
        }
        ++i; // only increment when not removing
    }

    // Adjust scene rect to fit all items
    m_scene.setSceneRect(m_scene.itemsBoundingRect().adjusted(-10, -10, 10, 10));
}