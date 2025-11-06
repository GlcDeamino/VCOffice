#ifndef EDITOR_STATUS_HPP
#define EDITOR_STATUS_HPP

#include <cstdint>
#include <QColor>
#include <qcolor.h>
#include <qnamespace.h>
#include <sstream>

enum class SubscriptStatus : uint8_t {
    NONE = 0,
    UPPER = 1,
    LOWER = 2
};

enum class UnderlineStatus : uint8_t {
    NONE = 0,
    SINGLE = 1,
    DOUBLE = 2,
    DOTTED = 3,
    DASH = 4
};

enum class ShaderStatus : uint8_t {
    NONE = 0,
    SOLID = 1
};

struct RunProperties {
public:
    RunProperties() = default;
    
    bool bold = false;
    bool italic = false;
    UnderlineStatus underline = UnderlineStatus::NONE;
    bool strikethrough = false;
    ShaderStatus shader = ShaderStatus::NONE;
    bool boardered = false;
    SubscriptStatus subscriptStatus = SubscriptStatus::NONE;
    double fontSize = 10.5;
    QColor bgColor = Qt::transparent;
    QColor fgColor = QColor(0, 0, 0);
    QString font = "宋体";

    QString toStr() {
        QString s;
        s.append("RunProperties:\n");
        s.append("   - bold:       [bol]\n");
        s.append("   - italic:     [ita]\n");
        s.append("   - underline:  [unde]\n");
        s.append("   - strike:     [sti]\n");
        s.append("   - shader:     [sha]\n");
        s.append("   - boardered:  [bor]\n");
        s.append("   - subscript:  [sub]\n");
        s.append("   - fontsize:   [fos]\n");
        s.append("   - font:       [font]\n");
        s.append("   - background: [bgcol]\n");
        s.append("   - foreground: [fgcol]");

        if (bold) {
            s.replace("[bol]", "TRUE");
        }
        else {
            s.replace("[bol]", "FALSE");
        }

        if (italic) {
            s.replace("[ita]", "TRUE");
        } else {
            s.replace("[ita]", "FALSE");
        }

        switch (underline) {
            case UnderlineStatus::NONE:
                s.replace("[unde]", "NONE");
            case UnderlineStatus::SINGLE:
                s.replace("[unde]", "SINGLE");
            case UnderlineStatus::DOUBLE:
                s.replace("[unde]", "DOUBLE");
            case UnderlineStatus::DOTTED:
                s.replace("[unde]", "DOTTED");
            case UnderlineStatus::DASH:
                s.replace("[unde]", "DASH");
        }

        if (strikethrough) {
            s.replace("[sti]", "TRUE");
        } else {
            s.replace("[sti]", "FALSE");
        }

        switch (shader) {
            case ShaderStatus::NONE:
                s.replace("[sha]", "NONE");
            case ShaderStatus::SOLID:
                s.replace("[sha]", "SOLID");
        }

        if (boardered) {
            s.replace("[bor]", "TRUE");
        } else {
            s.replace("[bor]", "FALSE");
        }

        switch (subscriptStatus) {
            case SubscriptStatus::NONE:
                s.replace("[sub]", "NONE");
            case SubscriptStatus::UPPER:
                s.replace("[sub]", "UPPER");
            case SubscriptStatus::LOWER:
                s.replace("[sub]", "LOWER");
        }

        s.replace("[fos]", formatDouble(fontSize));

        s.replace("[font]", font);

        s.replace("[bgcol]", bgColor.name());

        s.replace("[fgcol]", fgColor.name());

        return s;
    }
private:
    QString formatDouble(double d) {
        std::ostringstream oss;
        oss << d;
        std::string s = oss.str();
        if (s.length() > 5) s = s.substr(0, 5);
        return (std::string(5 - s.length(), ' ') + s).c_str();
    }
};

#endif