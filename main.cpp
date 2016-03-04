#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

QString filename;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void    MessageHandlerToFile(QtMsgType type, const QMessageLogContext &, const QString & str)
{
    const char * msg = str.toStdString().c_str();
#else
void    MessageHandlerToFile(QtMsgType type, const char *msg)
{
#endif
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
    break;
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
    break;
    }
    QFile outFile(filename);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main( int argc, char * argv[] )
{
//    QDateTime curdt = QDateTime::currentDateTime();
//    filename = "./logs/"+curdt.toString(Qt::ISODate)+".txt";
//#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
//    qInstallMessageHandler(MessageHandlerToFile);
//#else
//    qInstallMsgHandler(MessageHandlerToFile);
//#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
