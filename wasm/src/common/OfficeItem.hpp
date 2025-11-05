#pragma once
#ifndef OFFICE_ITEM_HPP
#define OFFICE_ITEM_HPP

#include <QVector>
#include "Object.hpp"

// Abstract
class OfficeItem : public Object {
protected:
    OfficeItem() = default;
    virtual ~OfficeItem();
    void append_p(OfficeItem* item);
    void remove_p(OfficeItem* item);
    QVector<OfficeItem*> m_item;

public:
    OfficeItem* operator[](size_t idx);
    OfficeItem* at(size_t idx);
    size_t len();
    virtual void append(OfficeItem* item) = 0;
};

#endif