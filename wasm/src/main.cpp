#include <QApplication>
#include <QMainWindow>
#include <QString>
#include <qlogging.h>
#include <qmainwindow.h>
#include "Theme.hpp"
#include "word/WordEditor.hpp"



int main(int argc, char** argv) {
#ifdef __EMSCRIPTEN__
    // 扩展逻辑
#else
    int cargc = 1;
    char* cargv[] = {argv[0], nullptr};
    QApplication app = QApplication(cargc, cargv);
    VSCodeTheme theme;
    for (int i = 0; i < argc; i++) {
        if (i > 0) {
            QMainWindow *win = new QMainWindow();
            QString p(argv[i]);
            if (p.endsWith(".docx")) {
                WordEditor *editor = new WordEditor(p);
                win->setCentralWidget(editor);
            } else {
                qCritical() << "Unsupport format for:" << argv[i];
                return 1;
            }
            win->resize(900, 600);
            win->show();
        }
    }
    app.exec();
#endif
}