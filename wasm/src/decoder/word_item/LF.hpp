#pragma once

#include "FontFlags.hpp"
#include <OfficeItem.hpp>

class LF : public OfficeItem {
public:
    void append(OfficeItem* item) { throw std::logic_error("Illegal operation."); }
    FontFlags rPr;
};