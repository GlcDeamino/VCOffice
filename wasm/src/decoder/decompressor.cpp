// decompressor.cpp
#include "decompressor.hpp"
#include <qlogging.h>

#ifndef Q_OS_WASM
    #include <QDir>
    #include <QFile>
    #include <QProcess>
    #include <QSysInfo>
#else
    #include <emscripten.h>
#endif

namespace Decompressor {

#ifndef Q_OS_WASM

DecompressStatus decompress(QString path)
{
    // 1. 检查源文件是否存在
    if (!QFile::exists(path)) {
        return DecompressStatus::FILE_NOT_FOUND;
    }

    // 2. 构造缓存目录路径
    QString cacheDirPath = path + ".cache";
    QDir cacheDir(cacheDirPath);

    if (!cacheDir.exists()) {
        return DecompressStatus::CACHE_DIR_NOT_EXISTS;
    }

    // 3. 根据平台选择命令
    QStringList args;

#ifdef Q_OS_WIN
    args << "xf" << path << "-C" << cacheDirPath;
    const char* program = "tar";

#elif defined(Q_OS_MAC)
    args << "-o" << path << "-d" << cacheDirPath;
    const char* program = "unzip";

#else
    args << "-o" << path << "-d" << cacheDirPath;
    const char* program = "unzip";
#endif

    // 4. 执行命令
    QProcess process;
    process.start(program, args);

    bool started = process.waitForStarted();
    if (!started) {
        return DecompressStatus::CANNOT_OPEN_ZIP;
    }

    process.waitForFinished(30000); // 30秒超时

    if (process.exitCode() != 0) {
        QString errInf;
        errInf.append(program);
        for (int i = 0; i < args.length(); i++) {
            errInf.append(" ");
            errInf.append(args[i]);
        }
        qCritical() << "[Decompressor] Failed to execute:" << errInf << "with exit code:" << process.exitCode() << "in cwd:" << process.workingDirectory();
        return DecompressStatus::IO_ERROR;
    }

    return DecompressStatus::SUCCESS;
}

#else // Q_OS_WASM

// 声明外部 JS 函数
extern "C" {
    int emscripten_run_script_decompress(const char* path_utf8);
}

DecompressStatus decompress(QString path)
{
    // 转为 UTF-8 C 字符串传给 JS
    QByteArray pathUtf8 = path.toUtf8();
    int jsResult = emscripten_run_script_decompress(pathUtf8.constData());

    // 约定：JS 返回 0 表示成功，非 0 表示失败
    switch (jsResult) {
        case 0:
            return DecompressStatus::SUCCESS;
        case 1:
            return DecompressStatus::FILE_NOT_FOUND;
        case 2:
            return DecompressStatus::CACHE_DIR_NOT_EXISTS;
        default:
            return DecompressStatus::IO_ERROR;
    }
}

#endif // Q_OS_WASM

} // namespace Decompressor