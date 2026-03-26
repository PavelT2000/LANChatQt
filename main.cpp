#include "mainwindow.h"
#include "enterance.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Enterance enterance;
    MainWindow w;
    QObject::connect(&enterance,&Enterance::Entered, &w, &MainWindow::initChat);
    if(enterance.exec()==QDialog::Accepted){

        w.show();
        return a.exec();
    }
    return QCoreApplication::exec();
}
