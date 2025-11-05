#include "RichText.hpp"
#include "OfficeItem.hpp"
#include "word_item/Paragraph.hpp"


RichText::RichText(Paragraph* para) : p(para) {}

RichText::~RichText() {}

void RichText::append(OfficeItem* item) { throw std::logic_error("End of OfficeItem tree branch."); }