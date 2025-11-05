#include "Section.hpp"
#include "OfficeItem.hpp"
#include "isType.hpp"
#include "word_item/Paragraph.hpp"


void Section::append(OfficeItem* item) {
    if (isType<
        Paragraph
    >(item)) append_p(item);
    return;
}