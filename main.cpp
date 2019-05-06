#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSharedMemory singleton(a.applicationName());
    if(!singleton.create(1))
    {
        QMessageBox::warning(NULL, "警告", "程序已经在运行中!");
        return false;
    }
    MainWindow w;
    w.show();

    //std::map<std::string, int>& _nameID;
    return a.exec();
}
