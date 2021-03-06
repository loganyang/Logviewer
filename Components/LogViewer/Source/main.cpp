#include "../Include/mainwindow.h"
#include <QApplication>
#include "../Include/startup.h"
#include "../Include/usertablemodel.h"
#include "QAbstractTableModel"
#include "../Include/logparser.h"
#include "QThread"
#include "QMetaType"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qRegisterMetaType<Analyze_Res>("Analyze_Res");
    qRegisterMetaType<tCurveSettingData>("tCurveSettingData");
    QThread* thread = new QThread();
    LogParser* parser = new LogParser;
    parser->moveToThread(thread);
    thread->start(QThread::NormalPriority);

    QObject::connect(&w, SIGNAL(sigAnalysizeLog(QString)),parser,SLOT(start(QString)));
    QObject::connect(parser,SIGNAL(sigFinished(Analyze_Res)), &w, SLOT(OnFinishAnalyzingLog(Analyze_Res)));
    w.init();
    w.show();



    return a.exec();
}
