#include <QApplication>
#include "AuthWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    AuthWindow authWindow;
    authWindow.show();
    return app.exec();
}
