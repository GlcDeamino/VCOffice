#include "OfficeItem.hpp"
#include <qurl.h>
#include <stdexcept>
#include <string>

OfficeItem::~OfficeItem() {
    for (size_t i = 0; i < m_item.length(); i++) {
        OfficeItem* curr_item = m_item[i];
        if (curr_item != nullptr) {
            delete curr_item;
            curr_item = nullptr;
        }
    }
}

size_t OfficeItem::len() {
    return m_item.length();
}

OfficeItem* OfficeItem::operator[](size_t idx) {
    OfficeItem* curr_item = m_item[idx];
    if (curr_item != nullptr) {
        return curr_item;
    } else {
        throw std::invalid_argument("Currently, pointer of index " + std::to_string(idx) + " is null.");
    }
}

OfficeItem* OfficeItem::at(size_t idx) {
    if (idx >= len()) {
        throw std::out_of_range("Index " + std::to_string(idx) + " out of range.");
    }
    return (*this)[idx];
}

void OfficeItem::append_p(OfficeItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("Pointer of item is null.");
    }
    m_item.append(item);
}

void OfficeItem::remove_p(OfficeItem* item) {
    for (size_t i = 0; i < m_item.length(); i++) {
        if (m_item[i] == item) {
            m_item.removeAt(i);
            delete item;
            item = nullptr;
        }
    }
}