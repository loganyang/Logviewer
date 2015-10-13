#include "../Include/mainwindow.h"
#include <QApplication>
#include "../Include/startup.h"
#include "../Include/usertablemodel.h"
#include "QAbstractTableModel"
#include "../Include/logparser.h"
#include "QThread"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Startup start;
    QThread* thread = new QThread();
    LogParser* parser = new LogParser;
    parser->moveToThread(thread);
    thread->start(QThread::NormalPriority);

    QObject::connect(&w,SIGNAL(sigNewLogsSet(QString)),
            &start,SLOT(LoadNewLogs(QString)));
    QObject::connect(&start,SIGNAL(sigSetModel(QAbstractTableModel*)),
            &w,SLOT(SetModel(QAbstractTableModel*)));
    QObject::connect(&w, SIGNAL(sigAnalysizeLog(QString)),parser,SLOT(start(QString)));
    QObject::connect(parser,SIGNAL(sigFinished(bool)), &w, SLOT(OnFinishAnalyzingLog(bool)));

    w.init();
    w.show();



    return a.exec();
}
