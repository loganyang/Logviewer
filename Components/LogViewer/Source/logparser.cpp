#include "../Include/logparser.h"
#include "ui_dlgparserresult.h"
#include "QThread"
#include "QRegularExpression"
#include "QFile"
#include "QDebug"
#include <QCoreApplication>
qint64 ControlKey::average()
{
    qint64 sum = 0;
    for(int i = 0; i < gaps.size(); i++)
    {
        sum += gaps[i];
    }
    if(gaps.size() > 0)
    {
        sum /= gaps.size();
    }  
    return sum;
}
QString ControlKey::toString()
{
    if(gaps.size() > 0)
    {
        qSort(gaps.begin(),gaps.end());
        return QString("%1,%2,%3,%4,%5\n").arg(name).arg(gaps[0]/1000).arg(average()/1000)
                .arg(gaps[gaps.size() - 1]/1000).arg(desc);
        gaps.clear();
    }
    else
    {
        return name + "\n";
    }
}

void ControlKey::add(QString &line)
{
    if(line.contains(startKey))
    {
        QStringList fields = line.split(";");
        if(fields.size() > 4)
        {
            flag = true;
            if(name.contains("ProgramEndTime"))
            {
                QString timestr = fields[4].section(" ",-5,-1);
                timestr.chop(1);
                stopTime = QDateTime::fromString(timestr).toMSecsSinceEpoch();
            }
            else
            {
                startTime = QDateTime::fromString(fields[0],"yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
            }
        }

    }
    else if(line.contains(stopKey))
    {
        if(flag)
        {
            QStringList fields = line.split(";");
            if(fields.size() > 4)
            {
                flag = false;
                if(name.contains("ProgramEndTime"))
                {
                    startTime = tempTime;
                    if(fields.size() > 7)
                    {
                        desc += QString("%1 %2 |").arg(fields[0]).arg(fields[7]);
                    }
                }
                else
                {
                    stopTime = QDateTime::fromString(fields[0],"yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
                }
                //if(stopTime >= startTime)
                {
                    gaps.push_back(stopTime - startTime);
                }
            }
        }
    }
    else if(line.contains(QRegularExpression(interruptKey.join("|"))) && !line.contains(QRegularExpression(ignoreErrors.join("|"))))
    {
        flag = false;
    }
    else if(line.contains("Debug message:  INFO: Start release pressure procedure"))
    {
        QStringList fields = line.split(";");
        if(fields.size() > 1)
        {
            tempTime = QDateTime::fromString(fields[0],"yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch();
        }
    }

}

DlgParserResult::DlgParserResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DlgParserResult)
{
    ui->setupUi(this);
    setWindowTitle("Select the directory with logs");
    resize(QSize(800,652));
    ui->tbRes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbRes->setSortingEnabled(true);
    ui->tbRes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbRes->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->btnOk,SIGNAL(clicked(bool)), this, SLOT(onOk(bool)));
    connect(ui->tbRes,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(onDoubleClicked(QTableWidgetItem*)));
}

DlgParserResult::~DlgParserResult()
{
    delete ui;
}


void DlgParserResult::show()
{
    ui->tbRes->clear();
    ui->tbRes->setColumnCount(2);
    ui->tbRes->setHorizontalHeaderLabels(QStringList()<<"Time"<<"Event");
    ui->tbRes->horizontalHeader()->setStretchLastSection(true);
    ui->tbRes->setColumnWidth(0,206);
    ui->tbRes->setColumnWidth(1,452);

    setWindowTitle(result.logdir);
    QMap<QString,QString> smap;
    for(int i = 0; i < result.items.size(); i++ )
    {
        QStringList fi = result.items[i].split(";;");
//        QDateTime pre = QDateTime::fromString(fi[0],"yyyy-MM-dd hh:mm:ss.zzz");
        smap.insert(fi[0],fi[1]); //ignore ss.zzz
    }
    QMapIterator<QString,QString> itor(smap);
    while(itor.hasNext())
    {
        itor.next();
        ui->tbRes->insertRow(0);
        ui->tbRes->setItem(0,0,new QTableWidgetItem(itor.key()));
        ui->tbRes->setItem(0,1,new QTableWidgetItem(itor.value()));
    }
    QWidget::show();
}

void DlgParserResult::onOk(bool ok)
{
    hide();
}
void DlgParserResult::onDoubleClicked(QTableWidgetItem *item)
{
    if(!item)
        return;
    QTableWidgetItem* dateitem = item;
    if(item->column() != 0)
    {
        dateitem = ui->tbRes->item(item->row(),0);
    }
    QDateTime dt = QDateTime::fromString(dateitem->data(Qt::DisplayRole).toString(),"yyyy-MM-dd hh:mm:ss.zzz");
    emit positionAt(dt, result.logdir);
}

LogParser::LogParser() : QObject(0)
{
    QStringList inter = QStringList() << ";Error;" << "User pause the program" << "User abort the program"<< "Primaris started";
    QStringList ignoreErrors = QStringList() << "Command timeout" << "Remote Enterprise server"<<"Unknown command reference"<<"Unsupported command";

    ControlKey pretestTime("ProtestTime", "Start the pretest.","PreTest success.",false, inter);
    pretestTime.ignoreErrors = ignoreErrors;
    m_items.push_back(pretestTime);

    ControlKey findInitialPosition("FindInitialPosition", "Move RV to initialize position.","Move RV to initialize position successfully.",false, inter);
    findInitialPosition.ignoreErrors = ignoreErrors;
    m_items.push_back(findInitialPosition);

    ControlKey sealingTest("SealingTest", "Sealing test.","Sealing test successfully.",false, inter);
    sealingTest.ignoreErrors = ignoreErrors;
    m_items.push_back(sealingTest);

    ControlKey HeatingLevelSensor("HeatingLevelSensor", "Heating the level sensor for filling","Start filling",false, inter);
    HeatingLevelSensor.ignoreErrors = ignoreErrors;
    m_items.push_back(HeatingLevelSensor);

    ControlKey filling("Filling", "Start filling","Filling successfully",false, inter);
    filling.ignoreErrors = ignoreErrors;
    m_items.push_back(filling);

    ControlKey moveToSealing("Sealing", "Moving the rotary valve to the sealing position","Start processing",false, inter);
    moveToSealing.ignoreErrors = ignoreErrors;
    m_items.push_back(moveToSealing);

    ControlKey moveToTube("Tube", "Moving the rotary valve to the tube position",";Draining.;",false, inter);
    moveToTube.ignoreErrors = ignoreErrors;
    m_items.push_back(moveToTube);

    ControlKey drain("Drain", ";Draining.;","Draining successfully",false, inter);
    drain.ignoreErrors = ignoreErrors;
    m_items.push_back(drain);

    ControlKey BiopsyEndTime("BiopsyProgramEndTime", "User start the program Leica Biopsy","User drain the last reagent of the program Leica Biopsy",false, inter);
    BiopsyEndTime.ignoreErrors = ignoreErrors;
    m_items.push_back(BiopsyEndTime);

    ControlKey OvernightEndTime("OvernightProgramEndTime", "User start the program Leica Overnight","User drain the last reagent of the program Leica Overnight",false, inter);
    OvernightEndTime.ignoreErrors = ignoreErrors;
    m_items.push_back(OvernightEndTime);

//    ControlKey UserProgramEndTime("UserProgramEndTime", "User start the program Zhongshan","User drain the last reagent of the program Zhongshan",false, inter);
//    m_items.push_back(UserProgramEndTime);

    ControlKey ALLProgramEndTime("AllProgramEndTime", "User start the program","User drain the last reagent of the program",false, inter);
    ALLProgramEndTime.ignoreErrors = ignoreErrors;
    m_items.push_back(ALLProgramEndTime);
    m_LogfileName = QCoreApplication::applicationDirPath() + QDir::separator() + QString("LOG_%1.csv").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    mp_LogFile = new QFile(m_LogfileName);

}

LogParser::~LogParser()
{
    delete m_thread;
    mp_LogFile->close();
}

void LogParser::start(QString path)
{
    //parse();
    bool res  = mp_LogFile->open(QIODevice::Text | QIODevice::Append);
    m_log.setDevice(mp_LogFile);
    (void)CheckErrors(path);

    mp_LogFile->close();

}
bool LogParser::CheckErrors(QString path)
{
    QString CfgErrorName = QCoreApplication::applicationDirPath() + QDir::separator() + "errors.csv";
    QFile ErrorListFile(CfgErrorName);
    QString regstr ="90909090909090";
    Analyze_Res res;
    res.logdir = path;
    QHash<QString,QString> err2des;
    if(ErrorListFile.exists() && ErrorListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Log("Open and Read the error list: " + CfgErrorName);
        while (!ErrorListFile.atEnd()) {
            QString ReadData = ErrorListFile.readLine();
            if(!ReadData.trimmed().isEmpty())
            {
                QStringList fs = ReadData.split(",");
                regstr += ("|" + fs[0]);
                if(fs.size() > 2) //Service string
                {
                   fs[2].chop(1);// remove \n
                   regstr += (".*" + fs[2]);
                }
                err2des.insert( fs[0], fs[1]);
            }
        }
        ErrorListFile.close();
    }
    else
    {
        Log("Cann't read the error list file: " + CfgErrorName);
        res.items<<"00;;Cann't read the error list file.";
        res.code = Analyze_Res::PAR_FAIL;
        emit sigFinished(res);
        return false;
    }
    QRegExp reg(regstr);
    QDir logDir(path);
    if(logDir.exists())
    {
        bool haveLogFile = false;
        Log("Try to check errors in " + logDir.absolutePath());
        foreach (QString logstr,logDir.entryList(QStringList() <<"HISTOCOREPEARL_*_*.log"<<"HISTOCOREPRIMARIS_*_*.log"
                                                   , QDir::NoFilter,QDir::Time))
        {
            QFile log(logDir.absolutePath() + "/" + logstr);
            if(log.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!log.atEnd())
                {
                    QString ReadData = log.readLine();
                    if(ReadData.contains(reg))
                    {
                        Log("There are errors in: " +  logDir.absolutePath() + "/" + logstr + ":  " + ReadData.trimmed());
                        QStringList fields = ReadData.split(";");
                        if(fields.size() > 1)
                        {
                            res.items<<fields[0] + ";;" + fields[1] + " : " + err2des.value(fields[1]);
                        }
                    }
                }
                log.close();
            }
            else
            {
                Log("Cann't open the log file: " + logstr);
                res.items<<"00;;Cann't open the log file: " + logstr;
            }
            haveLogFile = true;
        }
        if(haveLogFile)
        {
            if(res.items.size() == 0)
            {
                Log("There are no any errors in: " +  logDir.absolutePath());
                res.items<<"00;;There are no any errors in: " +  logDir.absolutePath();
            }
            res.code = Analyze_Res::PAR_SUCCESSFULE;
            emit sigFinished(res);
        }
        else
        {
            Log("There are no any log files in: " +  logDir.absolutePath());
            res.items<<"00;;There are no any log files in: " +  logDir.absolutePath();
            res.code = Analyze_Res::PAR_CANCEL;
            emit sigFinished(res);
        }
        return true;
    }
    else
    {
        Log("The log directory is not existed: " + logDir.absolutePath());
        res.items<<"00;;The log directory is not existed: " + logDir.absolutePath();
        res.code = Analyze_Res::PAR_FAIL;
        emit sigFinished(res);
        return false;
    }
}

void LogParser::parse(QString path)
{
    QDir logDir(path);
    if(logDir.exists())
    {
        foreach (QString logstr,logDir.entryList(QStringList() <<"HISTOCOREPEARL_*_*.log"<<"HISTOCOREPRIMARIS_*_*.log"
                                                   , QDir::NoFilter,QDir::Name))
        {
            QFile log(logDir.absolutePath() + "/" + logstr);
            if(log.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!log.atEnd()) {
                    QString ReadData = log.readLine();
                    for(int i = 0; i < m_items.size(); i++)
                    {
                        m_items[i].add(ReadData);
                    }
                }
                log.close();
            }
        }
        writeCSV(path);
    }
}

void LogParser::writeCSV(QString path)
{
    QFile csv(QDir(path).absolutePath() + "/time.csv" );
    if(csv.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QString header("item,min,average,max,desc\n");
        csv.write(header.toUtf8());
        for(int i = 0; i < m_items.size(); i++)
        {
            csv.write(m_items[i].toString().toUtf8());
        }
    }
    csv.close();
    Analyze_Res res;
    res.code = Analyze_Res::PAR_SUCCESSFULE;
    emit sigFinished(res);
}

void LogParser::Log(QString log)
{
    m_log<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz,")<<log<<endl;
    m_log.flush();
}
