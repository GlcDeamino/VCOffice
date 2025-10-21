#ifndef DECOMPRESSOR_HPP
#define DECOMPRESSOR_HPP

#include <QString>

enum class DecompressStatus : int {
    SUCCESS = 0,
    FILE_NOT_FOUND = 1,
    CANNOT_OPEN_ZIP = 2,
    CACHE_DIR_NOT_EXISTS = 3,
    IO_ERROR = 4,
    ZIP_INTERNAL_ERROR = 5
};

namespace Decompressor {
    DecompressStatus decompress(QString path);
}

#endif