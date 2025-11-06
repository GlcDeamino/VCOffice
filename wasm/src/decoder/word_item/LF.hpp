#pragma once

#include "RunProperties.hpp"
#include <OfficeItem.hpp>

class LF : public OfficeItem {
public:
    void append(OfficeItem* item) { throw std::logic_error("Illegal operation."); }
    RunProperties rPr;
};