#include "mainwindow.h"
#include "project.h"
#include "console/console_utils.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    if(argc>1)
        return consoleMain(argc, argv);

    QApplication a(argc, argv);
    setlocale(LC_NUMERIC,"en_US.UTF-8");

    QCoreApplication::setOrganizationName("Balmer");
    QCoreApplication::setApplicationName("VNA");

    Project project;
    project.loadSettings();

    MainWindow w;
    w.show();


    int ret = a.exec();
    project.saveSettings();
    return ret;
}
