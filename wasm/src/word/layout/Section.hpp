#ifndef SECTION_HPP
#define SECTION_HPP

#include <QVector>
#include "Page.hpp"

class Section : public QVector<Page> {
public:
    Section();
};

#endif