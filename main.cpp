#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <crtdbg.h>
#include "Disc.h"
int main(int argc, char *argv[])
{
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}
