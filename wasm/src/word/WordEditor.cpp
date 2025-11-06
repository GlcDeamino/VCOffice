#include "WordEditor.hpp"
#include "OfficeItem.hpp"
#include "RunProperties.hpp"
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
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qthread.h>
#include <qvariant.h>
#include <qwidget.h>
#include <qwindowdefs.h>
#include <QMessageBox>
#include <QThread>

class RTLine {
public:
    RTLine(QString s, RichText* rt) {
        newRT(rt);
        (*this) += s;
    }
    RTLine() {}
    RTLine(const RTLine& other) = default;
    RTLine& operator=(const RTLine& other) = default;
    auto begin() {
        return strs.begin();
    }
    auto end() {
        return strs.end();
    }
    qreal width() {
        qreal l;
        for (QString s : strs) {
            RichText* rt = map.value(s);
            l += RT::strWidth(s, rt->rPr);
        }
        return l;
    }
    RTLine operator+(QString s) {
        RTLine newLine(*this);
        newLine += s;
        return newLine;
    }
    void operator+=(QString s) {
        if (strs.isEmpty()) {
            strs.append(s);
            map.insert(s, rts.last());
        } else {
            QString old = strs.last();
            map.remove(old);
            strs.removeLast();
            QString new_ = old + s;
            strs.append(new_);
            map.insert(new_, rts.last());
        }
    }
    void newRT(RichText* rt) {
        rts.append(rt);
    }
    bool isEmpty() {
        return rts.isEmpty() || map.isEmpty() || strs.isEmpty();
    }
    void clear() {
        rts.clear();
        map.clear();
        strs.clear();
    }
    RichText* node(QString s) {
        return map.value(s);
    }
    void dumpContent() {
        for (QString s : strs) {
            qDebug() << s << "->" << map.value(s) << ":";
            for (QString l : map.value(s)->rPr.toStr().split("\n")) {
                qDebug() << "    " << l.toStdString().c_str();
            }
        }
    }
private:
    QVector<RichText*> rts;
    QHash<QString, RichText*> map;
    QVector<QString> strs;
};


class DecodeThread : public QThread {
public:
    DecodeThread(WordEditor* e) {
        editor = e;
    }
    WordEditor* editor;
    QString file;
    bool result = false;
    void run() override {
        auto r = DocxDecoder::decode(file, *editor);
        switch (r) {
        case DocxDecodeStatus::FAIL_OPEN_FILE:
            QMessageBox::critical(editor, "解析失败", "无法打开“" + file + "”", QMessageBox::Ok);
        case DocxDecodeStatus::FAIL_CREATE_TMP:
            QMessageBox::critical(editor, "解析失败", "无法创建缓存“" + file + ".cache”", QMessageBox::Ok);
        case DocxDecodeStatus::FAIL_DECOMP: 
        case DocxDecodeStatus::FORMAT_ERROR:
            QMessageBox::critical(editor, "解析失败", "文件“" + file + "”的格式不合法", QMessageBox::Ok);
        case DocxDecodeStatus::SUCCESS:
            result = true;
        }
        QThread::sleep(1);
    }
};


WordEditor::WordEditor() {
    m_status = Status::NOFILE;
    m_fns.setParent(this);
    m_fns.addTab(static_cast<QWidget*>(&m_stt), "Start");
    m_view.setParent(this);
    m_view.setScene(&m_scene);
    m_scene.setBackgroundBrush(palette().base());
    connect(&m_scene, &WordScene::mouseMoved, this, &WordEditor::updateMousePos);
    m_mouse_pos.setParent(this);
    setMouseTracking(true);
    m_fns.setMouseTracking(true);
    m_view.setMouseTracking(true);
    m_mouse_pos.setMouseTracking(true);
    dt = new DecodeThread(this);
    connect(dt, &QThread::finished, this, &WordEditor::onFinishDecode);
}

void WordEditor::load(QString p) {
    setCursor(QCursor(Qt::WaitCursor));
    m_status = Status::DECODING;

    // 初始化加载提示文本（不含动画字符）
    m_mouse_pos.setText("正在解析：" + p);
    update();

    dt->file = p;

    // 启动加载动画定时器
    if (m_loadingTimer == nullptr) {
        m_loadingTimer = new QTimer(this);
        connect(m_loadingTimer, &QTimer::timeout, this, &WordEditor::updateLoadingAnimation);
    }
    m_loadingFrameIndex = 0;
    m_loadingTimer->start(150); // 每150毫秒切换一次

    // 连接线程结束信号：当 dt 完成时停止动画
    connect(dt, &QThread::finished, this, &WordEditor::onFinishDecode);

    dt->start();
}

void WordEditor::updateLoadingAnimation() {
    QString animChar = m_loadingChars[m_loadingFrameIndex % m_loadingChars.size()];
    m_mouse_pos.setText("正在解析：" + dt->file + " " + animChar);
    update(); // 触发重绘（如果 m_mouse_pos 是 QLabel 或类似控件，可能不需要 update()）
    m_loadingFrameIndex++;
}

void WordEditor::onFinishDecode() {
    m_loadingTimer->stop();
    m_mouse_pos.setText("正在计算布局");
    update();
    m_status = Status::FORMATTING;
    reformat();
    m_status = Status::NORMAL;
    updateMousePos(QCursor(Qt::CursorShape::ArrowCursor));
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
    if (m_status != Status::NORMAL) return;
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
                    RTLine currentLine;
                    qreal currentLineHeight = 0.0;
                    qreal lastLineEndX = leftMargin;

                    auto flushLine = [&]() {
                        if (!currentLine.isEmpty()) {
                            for (QString s : currentLine) {
                                RT* rt = new RT(currentLine.node(s), this, s);
                                rt->setPos(QPointF(cursor.x(), cursor.y()));
                                rt->setZValue(0);
                                m_scene.addItem(rt);
                                cursor.rx() += RT::strWidth(s, rt->node->rPr);
                            }

                            cursor.setX(leftMargin);
                            cursor.ry() += currentLineHeight;
                            para->h += currentLineHeight;
                            pageUsed += currentLineHeight;

                            currentLine.clear();
                            currentLineHeight = 0.0;
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
                            currentLine.newRT(rt_node);
                            qreal charH = RT::fontHeight(rt_node->rPr);
                            for (QChar ch : rt_node->t) {
                                RTLine test = currentLine + ch;
                                if (!currentLine.isEmpty() &&
                                    test.width() > availableWidth) {
                                    flushLine();
                                    // Start new line with this char
                                    currentLine = RTLine(ch, rt_node);
                                    currentLineHeight = charH;
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
                                }
                                lastLineEndX = leftMargin + currentLine.width();
                            }
                        }
                    }

                    // Flush last line of paragraph
                    flushLine();

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
                    para->h += paraSpace;
                    pageUsed += paraSpace;
                    m_scene.addItem(para);

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