#include "console_utils.h"
#include "consolewriteflash.h"
#include "consoleserial.h"

#include <QCoreApplication>
#include <QTimer>

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

int consoleMain(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_NUMERIC,"en_US.UTF-8");

    QStringList args = QCoreApplication::arguments();
    if(args.size()>2 && args[1]=="-flash")
    {
        ConsoleSerial* c=new ConsoleSerial();
        if(!c->openSerialPort())
            return 1;

        ConsoleWriteFlash* w = new ConsoleWriteFlash();
        w->setFilename(args[2]);
        QTimer::singleShot(0, w, SLOT(onStart()));
    }

    return a.exec();
}
