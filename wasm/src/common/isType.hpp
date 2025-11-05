#pragma once
#ifndef ISTYPE_HPP
#define ISTYPE_HPP

#include "Object.hpp"
#include <QObject>
#include <QGraphicsItem>

template<typename... Types>
bool isType(Object* p) {
    if (!p) return false;
    return ((dynamic_cast<Types*>(p) != nullptr) || ...);
}

template<typename... Types>
bool isType(QObject* p) {
    if (!p) return false;
    return ((dynamic_cast<Types*>(p) != nullptr) || ...);
}

template<typename... Types>
bool isType(QGraphicsItem* p) {
    if (!p) return false;
    return ((dynamic_cast<Types*>(p) != nullptr) || ...);
}

#endif