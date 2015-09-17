#include "logparser.h"
#include "QThread"
#include "QRegularExpression"
#include "QFile"
#include "QDebug"
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


}

LogParser::~LogParser()
{
    delete m_thread;
}

void LogParser::start(QString path)
{
    m_LogDir = QDir(path);
    //m_thread->start(QThread::NormalPriority);
    parse();

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
    emit sigFinished(true);
}
