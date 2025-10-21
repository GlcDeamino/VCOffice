#include "docx_decoder.hpp"
#include "decompressor.hpp"
#include <QDir>
#include <QLogging.h>
#include <qlogging.h>

DocxDecodeStatus DocxDecoder::decode(QString p) {
    QDir dir;
    QString tmp(p);
    tmp.append(".cache");
    if (!dir.exists(tmp)) {
        if (!dir.mkdir(tmp)) {
            qCritical() << "[Decoder] Unable to create:" << tmp;
            return DocxDecodeStatus::FAIL_CREATE_TMP;
        }
    }
    DecompressStatus decomResult = Decompressor::decompress(p);
    if (decomResult != DecompressStatus::SUCCESS) {
        qCritical() << "[Decoder] Failed to decompress:" << p;
        return DocxDecodeStatus::FAIL_DECOMP;
    }
    return DocxDecodeStatus::SUCCESS;
}
