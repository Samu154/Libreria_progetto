#include <QApplication>
#include <QDebug>
#include "MainWindow.h"


int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    qDebug() << "Avvio applicazione GUI";
    MainWindow w;
    w.show();

    return app.exec();
}
