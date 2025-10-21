#ifndef EDITOR_STATUS_HPP
#define EDITOR_STATUS_HPP

#include <cstdint>
#include <QColor>
#include <qcolor.h>

enum class SubscriptStatus : uint8_t {
    NONE = 0,
    UPPER = 1,
    LOWER = 2
};

enum class AlignFlags : uint8_t {
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2,
    BOTH = 3,
    DISCRETE = 4
};

struct WStatus {
    bool bold = false;
    bool italic = false;
    bool underlined = false;
    bool strikethroth = false;
    bool hasSdL = false;
    bool boardered = false;
    SubscriptStatus subscriptStatus = SubscriptStatus::NONE;
    double fontSize = 11;
    QColor bgColor = QColor(255, 255, 255);
    QColor fgColor = QColor(0, 0, 0);
    AlignFlags align = AlignFlags::LEFT;
    QString font = "Consolas";
};

#endif