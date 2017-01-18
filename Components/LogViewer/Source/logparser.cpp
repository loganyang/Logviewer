#include "../Include/logparser.h"
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
    m_LogDir = QDir(path);
    //parse();
    bool res  = mp_LogFile->open(QIODevice::Text | QIODevice::Append);
    m_log.setDevice(mp_LogFile);
    (void)CheckErrors();

    mp_LogFile->close();

}
bool LogParser::CheckErrors()
{
    QString CfgErrorName = QCoreApplication::applicationDirPath() + QDir::separator() + "errors.ini";
    QFile ErrorListFile(CfgErrorName);
    QString regstr ="90909090909090";
    if(ErrorListFile.exists() && ErrorListFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Log("Open and Read the error list: " + CfgErrorName);
        while (!ErrorListFile.atEnd()) {
            QString ReadData = ErrorListFile.readLine();
            if(!ReadData.trimmed().isEmpty())
            {
                regstr += ("|" + ReadData.split(",").at(0));
            }
        }
        ErrorListFile.close();
    }
    else
    {
        Log("Cann't read the error list file: " + CfgErrorName);
        emit sigFinished(PAR_FAIL);
        return false;
    }
    QRegExp reg(regstr);
    if(m_LogDir.exists())
    {
        bool haveLogFile = false;
        Log("Try to check errors in " + m_LogDir.absolutePath());
        foreach (QString logstr,m_LogDir.entryList(QStringList() <<"HISTOCOREPEARL_*_*.log"<<"HISTOCOREPRIMARIS_*_*.log"
                                                   , QDir::NoFilter,QDir::Name))
        {
            QFile log(m_LogDir.absolutePath() + "/" + logstr);
            if(log.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                while (!log.atEnd())
                {
                    QString ReadData = log.readLine();
                    if(ReadData.contains(reg))
                    {
                        Log("There are errors in: " +  m_LogDir.absolutePath() + "/" + logstr + ":  " + ReadData.trimmed());
                        emit sigFinished(PAR_FAIL);
                        return false;
                    }
                }
                log.close();
            }
            else
            {
                Log("Cann't open the log file: " + logstr);
                emit sigFinished(PAR_FAIL);
                return false;
            }
            haveLogFile = true;
        }
        if(haveLogFile)
        {
            Log("There are no any errors in: " +  m_LogDir.absolutePath());
            emit sigFinished(PAR_SUCCESSFULE);
        }
        else
        {
            Log("There are no any log files in: " +  m_LogDir.absolutePath());
            emit sigFinished(PAR_CANCEL);
        }
        return true;
    }
    else
    {
        Log("The log directory is not existed: " + m_LogDir.absolutePath());
        emit sigFinished(PAR_FAIL);
        return false;
    }
}

void LogParser::parse()
{
    if(m_LogDir.exists())
    {
        foreach (QString logstr,m_LogDir.entryList(QStringList() <<"HISTOCOREPEARL_*_*.log"<<"HISTOCOREPRIMARIS_*_*.log"
                                                   , QDir::NoFilter,QDir::Name))
        {
            QFile log(m_LogDir.absolutePath() + "/" + logstr);
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
        writeCSV();
    }
}

void LogParser::writeCSV()
{
    QFile csv(m_LogDir.absolutePath() + "/time.csv" );
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
    emit sigFinished(PAR_SUCCESSFULE);
}

void LogParser::Log(QString log)
{
    m_log<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz,")<<log<<endl;
    m_log.flush();
}
