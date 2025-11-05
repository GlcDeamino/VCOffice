#include "Paragraph.hpp"
#include "OfficeItem.hpp"
#include "RichText.hpp"
#include "LF.hpp"
#include "isType.hpp"
#include <stdexcept>


void Paragraph::append(OfficeItem* item) {
    if (isType<RichText, LF>(item)) {
        append_p(item);
    } else {
        throw std::logic_error("Illegal paragraph element.");
    }
}