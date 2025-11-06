#include "docx_decoder.hpp"
#include <RunProperties.hpp>
#include "WordEditor.hpp"
#include "decompressor.hpp"
#include <QDir>
#include <QLoggingCategory>
#include <QFile>
#include <QDomDocument>
#include <QColor>
#include <qdom.h>
#include <qlogging.h>
#include <qnamespace.h>
#include "word_item/LF.hpp"
#include "word_item/Paragraph.hpp"
#include "word_item/RichText.hpp"
#include "word_item/Section.hpp"

static QColor parseColorValue(const QString& val) {
    if (val.isEmpty() || val == "auto") {
        return Qt::transparent; // 无效或自动色
    }
    for (QChar c : val) {
        if (!c.isLetterOrNumber() || (c.toLower() < 'a' || c.toLower() > 'f') && !c.isDigit()) {
            goto not_hex; // 这样写可以省去一个 bool 标记，从而省去 1 字节的内存开销
                          // 大文档可能会有大量颜色需要解析，且 bool 可能被对齐的更大
                          // 谁知道编译器会不会因为一些问题抽风没把标记位优化掉
        }
    }
    return QColor("#" + val);
    not_hex: // 禁止在其它位置 goto 本标签
    // 否则尝试 Qt 颜色名称（如 "red", "yellow"）
    QColor namedColor(val);
    if (namedColor.isValid()) {
        return namedColor;
    }
    // 默认返回透明
    qWarning() << "[Decoder] Unrecognized color value:" << val;
    return QColor();
}

// 辅助：解析 <w:rPr> 中的属性到 RunProperties（增量覆盖）
static void parseRunProperties(const QDomNode& rPrNode, RunProperties& rPr) {
    for (int i = 0; i < rPrNode.childNodes().count(); ++i) {
        QDomNode propNode = rPrNode.childNodes().at(i);
        QString name = propNode.nodeName();
        QString val = propNode.toElement().attribute("w:val", "1"); // 默认为 "1" 表示启用

        if (name == "w:b") {
            rPr.bold = (val != "0" && val != "false" && val != "off");
        } else if (name == "w:i") {
            rPr.italic = (val != "0" && val != "false" && val != "off");
        } else if (name == "w:u") {
            if (val == "none") rPr.underline = UnderlineStatus::NONE;
            else if (val == "single") rPr.underline = UnderlineStatus::SINGLE;
            // 可扩展其他类型
        } else if (name == "w:strike") {
            rPr.strikethrough = (val != "0" && val != "false" && val != "off");
        } else if (name == "w:vertAlign") {
            if (val == "superscript") rPr.subscriptStatus = SubscriptStatus::UPPER;
            else if (val == "subscript") rPr.subscriptStatus = SubscriptStatus::LOWER;
            else rPr.subscriptStatus = SubscriptStatus::NONE;
        } else if (name == "w:color") {
            rPr.fgColor = parseColorValue(val);
        } else if (name == "w:highlight") {
            rPr.bgColor = parseColorValue(val);
        } else if (name == "w:rFonts") {
            QDomElement elem = propNode.toElement();
            QString hint = elem.attribute("w:hint", "eastAsia"); // 默认 fallback
            QString fontAttr = "w:" + hint;
            if (elem.hasAttribute(fontAttr)) {
                rPr.font = elem.attribute(fontAttr);
            }
            // 可扩展 ascii, hAnsi 等
        } else if (name == "w:sz") {
            rPr.fontSize = val.toDouble() / 2;
        }
        // 其他属性可后续添加
    }
}

// 辅助：解析 styles.xml，提取字符样式和段落默认文本样式
static void parseStyles(const QDomDocument& stylesDom,
                        QHash<QString, RunProperties>& charStyles,
                        QHash<QString, RunProperties>& paraDefaultRPrs,
                        RunProperties& docDefaultRPr,
                        QHash<QString, QColor>& paraFills) {
    QDomNodeList styleNodes = stylesDom.elementsByTagName("w:style");
    for (int i = 0; i < styleNodes.count(); ++i) {
        QDomElement styleElem = styleNodes.at(i).toElement();
        bool isDefault = styleElem.attribute("w:default", "0").toInt();
        if (isDefault) {
            QDomElement rPrElem = styleElem.firstChildElement("w:rPr");
            if (rPrElem.isNull()) continue;

            RunProperties flags;
            parseRunProperties(rPrElem,flags);
            docDefaultRPr = flags;
        }
    }
    for (int i = 0; i < styleNodes.count(); ++i) {
        QDomElement styleElem = styleNodes.at(i).toElement();
        QString type = styleElem.attribute("w:type");
        QString styleId = styleElem.attribute("w:styleId");
        
        if (styleId.isEmpty()) continue;

        QDomElement rPrElem = styleElem.firstChildElement("w:rPr");
        if (rPrElem.isNull()) continue;

        RunProperties flags = docDefaultRPr;
        parseRunProperties(rPrElem,flags);

        if (type == "character") {
            charStyles.insert(styleId, flags);
        } else if (type == "paragraph") {
            paraDefaultRPrs.insert(styleId, flags);
        }

        QDomElement pPrElem = styleElem.firstChildElement("w:pPr");
        if (pPrElem.isNull()) continue;
        QDomElement pShdNode = pPrElem.firstChildElement("w:shd");
        if (!pShdNode.isNull()) {
            QColor fill = parseColorValue(pShdNode.attribute("w:fill", "auto"));
            paraFills.insert(styleId, fill);
        }
    }
}

// 辅助：解析单个 <w:r>（run）
static void parseRun(const QDomNode& runNode,
                          Paragraph* parent,
                          const QHash<QString, RunProperties>& charStyles,
                          const RunProperties& defaultParaRPr) {
    auto* richtext = new RichText(parent);
    richtext->rPr = defaultParaRPr; // 初始化为段落默认样式

    // 查找 w:rPr 节点（用于 rStyle 和显式属性）
    QDomElement rPrElem;
    QDomNode textNode;
    for (int k = 0; k < runNode.childNodes().count(); ++k) {
        QDomNode item = runNode.childNodes().at(k);
        if (item.nodeName() == "w:rPr") {
            rPrElem = item.toElement();
        } else if (item.nodeName() == "w:t") {
            textNode = item;
        } else if (item.nodeName() == "w:br") {
            auto* lf = new LF();
            lf->rPr = richtext->rPr; // 使用当前 run 的样式
            parent->append(lf);
        }
    }

    // 处理 w:rStyle（字符样式）
    if (!rPrElem.isNull()) {
        QDomElement rStyleNode = rPrElem.firstChildElement("w:rStyle");
        if (!rStyleNode.isNull()) {
            QString styleId = rStyleNode.attribute("w:val");
            if (charStyles.contains(styleId)) {
                richtext->rPr = charStyles.value(styleId);
            }
        }
    }

    // 应用显式的 w:rPr 属性（覆盖）
    if (!rPrElem.isNull()) {
        parseRunProperties(rPrElem, richtext->rPr);
    }

    // 设置文本内容
    if (!textNode.isNull()) {
        richtext->t = textNode.toElement().text();
        parent->append(richtext);
    }
}

// 辅助：解析 <w:sectPr> 到 Section
static void parseSectionProperties(const QDomNode& sectPrNode, Section* section) {
    for (int i = 0; i < sectPrNode.childNodes().count(); ++i) {
        QDomNode node = sectPrNode.childNodes().at(i);
        if (node.nodeName() == "w:pgSz") {
            auto elem = node.toElement();
            section->pageSize = QSizeF(
                elem.attribute("w:w").toDouble() / 20.0,
                elem.attribute("w:h").toDouble() / 20.0
            );
        } else if (node.nodeName() == "w:pgMar") {
            auto elem = node.toElement();
            section->pageMagrin = pgMagrin(
                elem.attribute("w:left").toDouble() / 20.0,
                elem.attribute("w:top").toDouble() / 20.0,
                elem.attribute("w:right").toDouble() / 20.0,
                elem.attribute("w:bottom").toDouble() / 20.0,
                elem.attribute("w:header").toDouble() / 20.0,
                elem.attribute("w:footer").toDouble() / 20.0,
                elem.attribute("w:gutter").toDouble() / 20.0
            );
        }
    }
}

// 主函数：DocxDecoder::decode
DocxDecodeStatus DocxDecoder::decode(QString p, WordEditor& editor) {
    QDir dir;
    QString tmpDir = p + ".cache";
    if (!dir.exists(tmpDir)) {
        if (!dir.mkdir(tmpDir)) {
            qCritical() << "[Decoder] Unable to create temp dir:" << tmpDir;
            return DocxDecodeStatus::FAIL_CREATE_TMP;
        }
    }

    DecompressStatus decomResult = Decompressor::decompress(p);
    if (decomResult != DecompressStatus::SUCCESS) {
        qCritical() << "[Decoder] Failed to decompress:" << p;
        return DocxDecodeStatus::FAIL_DECOMP;
    }

    // === Step 1: 加载并解析 styles.xml（如果存在）===
    QHash<QString, RunProperties> charStyles;
    QHash<QString, RunProperties> paraDefaultRPrs;
    QHash<QString, QColor> paraFills;

    RunProperties docDefaultRPr;

    QFile stylesFile(tmpDir + "/word/styles.xml");
    if (stylesFile.open(QIODevice::ReadOnly)) {
        QDomDocument stylesDom;
        if (stylesDom.setContent(&stylesFile)) {
            parseStyles(stylesDom, charStyles, paraDefaultRPrs, docDefaultRPr, paraFills);
        }
        stylesFile.close();
    } else {
        qWarning() << "[Decoder] styles.xml not found or unreadable. Using inline styles only.";
    }

    // === Step 2: 解析 document.xml ===
    QFile docFile(tmpDir + "/word/document.xml");
    if (!docFile.open(QIODevice::ReadOnly)) {
        qCritical() << "[Decoder] Failed to open document.xml:" << docFile.fileName();
        return DocxDecodeStatus::FAIL_OPEN_FILE;
    }

    QDomDocument docDom;
    if (!docDom.setContent(&docFile)) {
        qCritical() << "[Decoder] XML format error in:" << docFile.fileName();
        docFile.close();
        return DocxDecodeStatus::FORMAT_ERROR;
    }
    docFile.close();

    // 获取 <w:body>
    QDomNodeList bodyList = docDom.elementsByTagName("w:body");
    if (bodyList.isEmpty()) {
        qCritical() << "[Decoder] Missing <w:body> in document.xml";
        return DocxDecodeStatus::FORMAT_ERROR;
    }
    QDomNode docBody = bodyList.at(0);

    // 初始化第一个节
    Section* currentSection = new Section();
    for (QDomNode node : docBody.childNodes()) {
        if (node.nodeName() == "w:sectPr") {
            parseSectionProperties(node, currentSection);
        }
    }

    // 遍历 body 的所有子节点
    for (int i = 0; i < docBody.childNodes().count(); ++i) {
        QDomNode node = docBody.childNodes().at(i);

        if (node.nodeName() == "w:p") {
            // 解析段落
            Paragraph* paragraph = new Paragraph();

            // 确定段落默认文本样式（来自 w:pStyle）
            RunProperties paraDefaultRPr;
            QDomElement pPr = node.toElement().firstChildElement("w:pPr");
            if (!pPr.isNull()) {
                QDomElement pStyleNode = pPr.firstChildElement("w:pStyle");
                if (!pStyleNode.isNull()) {
                    QString styleId = pStyleNode.attribute("w:val");
                    if (paraDefaultRPrs.contains(styleId)) {
                        paraDefaultRPr = paraDefaultRPrs.value(styleId);
                    } else {
                        paraDefaultRPr = docDefaultRPr;
                    }
                    if (paraFills.contains(styleId)) {
                        paragraph->fill = paraFills.value(styleId);
                    } else {
                        paragraph->fill = parseColorValue("auto");
                    }
                } else {
                    paraDefaultRPr = docDefaultRPr;
                    paragraph->fill = parseColorValue("auto");
                }
                QDomElement pShdNode = pPr.firstChildElement("w:shd");
                if (!pShdNode.isNull()) {
                    paragraph->fill = parseColorValue(pShdNode.attribute("w:fill", "auto"));
                }
            }

            // 遍历段落内的 run 和其他元素
            for (int j = 0; j < node.childNodes().count(); ++j) {
                QDomNode child = node.childNodes().at(j);
                if (child.nodeName() == "w:r") {
                    parseRun(child, paragraph, charStyles, paraDefaultRPr);
                } else if (child.nodeName() == "w:br") {
                    // 段落级换行
                    auto* lf = new LF();
                    // LF 无上下文 run，使用默认样式（可考虑用 paraDefaultRPr）
                    lf->rPr = paraDefaultRPr;
                    paragraph->append(lf);
                }
                // 忽略其他如 w:tab, w:drawing 等
            }
            currentSection->append(paragraph);

        } else if (node.nodeName() == "w:sectPr") {
            // 遇到分节符：提交当前节，创建新节
            editor.append(currentSection);
            currentSection = new Section();
            parseSectionProperties(node, currentSection);
        }
        // 忽略其他节点（如 w:tbl, w:bookmarkStart 等）
    }

    // 提交最后一个节
    editor.append(currentSection);
    return DocxDecodeStatus::SUCCESS;
}