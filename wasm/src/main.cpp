#include <QApplication>
#include <QMainWindow>
#include <QString>
#include <qlogging.h>
#include <qmainwindow.h>
#include "AbstractEditor.hpp"
#include "word/WordEditor.hpp"



int main(int argc, char** argv) {
#ifdef __EMSCRIPTEN__
    // VS Code 扩展逻辑
#else
    // 测试用途
    int cargc = 1;
    char* cargv[] = {argv[0], nullptr};
    QApplication app = QApplication(cargc, cargv);
    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            AbstractEditor* editor;
            QMainWindow *win = new QMainWindow();
            QString p(argv[i]);
            if (p.endsWith(".docx")) {
                editor = new WordEditor();
                win->setCentralWidget(editor);
            } else {
                qCritical() << "Unsupport format for:" << argv[i];
                return 1;
            }
            win->setMouseTracking(true);
            win->resize(900, 600);
            win->show();
            editor->load(p);
        }
    }
    app.exec();
#endif
}