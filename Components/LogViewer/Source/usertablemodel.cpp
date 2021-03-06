#include "../Include/usertablemodel.h"
#include <QDebug>
#include <QModelIndex>
#include "QRegularExpression"
#include "../Include/globaldefines.h"
#include "QToolTip"
UserTableModel::UserTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
//   m_InterestKey << "Start Step" << "User start the program";
}

void UserTableModel::AddMsgInfro()
{
}

bool UserTableModel::isInterest(QString line) const
{
    return GlobalDefines::Instance().interest(line);
}

bool UserTableModel::isStartBottleCheck(QString line)
{
    return line.contains("67174927"); // start bottle check
}

bool UserTableModel::isStartProgram(QString line)
{
     return line.contains("67174418");//User start the program

}
bool UserTableModel::isStartSelfttest(QString line)
{
    return line.contains("67174656"); // start selftest
}
bool UserTableModel::isShutDown(QString line)
{
    return line.contains("67174416"); // user shutdown instrument
}
bool UserTableModel::isPowerFail(QString line)
{
    return line.contains("67176214"); //power failure
}
bool UserTableModel::isStartStep(QString line)
{
    QStringList fields = line.split(";");
    if(fields.size() < 4)
    {
        return false;
    }
    QStringList steps = QStringList()<<"67175216"<<"67174912" // start step; start pre-test
                                       <<"67175218" <<"67175224" //dry step ; cooling down
                                       ; //
    return steps.contains(fields.at(3)) ;//Start Step
}

bool UserTableModel::isStartScenario(QString line)
{
    QStringList fields = line.split(";");
    if(fields.size() < 4)
    {
        return false;
    }
                      ;
    QStringList scenario = QStringList() << "67174995" //Start heating level sensor
                               << "67175210"<<"67175212"      //Start filling" ; "Moveing the rotary valve to the sealing position"
                               << "67175213"<< "67175211"//"Start processing" << "Moving the rotary valve to the tube position"
                               << "67175207" //";Draining."
                               <<"67174420" // pause
                               <<"67174421" // abort
                                 ;
    return scenario.contains(fields.at(3));
}

bool UserTableModel::isUserOperation(QString line)
{
    return false;
}
QVariant UserTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if(section == 0)
        {
            return tr("Time");
        }
        else if(section == 1)
        {
            return tr("Gap");
        }
        else if(section == 2)
        {
            return tr("Event");
        }
        else if(section == 3)
        {
            return tr("type");
        }
        else if(section == 4)
        {
            return tr("Description");
        }
    }
    return QVariant();
}

QVariant UserTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole && role != Qt::ToolTipRole))
    {
        return QVariant();
    }
    if(index.row() >= m_items.count())
    {
        return QVariant();
    }
    const Event& ev = m_items[index.row()];
    if(m_TypeFilter.contains(ev.type) && m_TypeFilter.value(ev.type))
    {
        return QVariant();
    }

    if(role == Qt::ToolTipRole)
    {
        if(index.column() == 4)
            return ev.description;
        return QVariant();
    }
    if(index.column() == 0)
    {
        return ev.time;
    }
    else if(index.column() == 1)
    {
        if(m_view->isRowHidden(index.row()) || index.row() == 0)
        {
            return "";
        }
        else
        {
            QDateTime now = QDateTime::fromString(m_items[index.row()].time,"yyyy-MM-dd hh:mm:ss.zzz");

            int i = index.row() - 1;
            while(i >= 0 && m_view->isRowHidden(i)) i--;
            if(i < 0)
            {
                return "";
            }
            else
            {
                QDateTime pre = QDateTime::fromString(m_items[i].time,"yyyy-MM-dd hh:mm:ss.zzz");
                quint64 secs = pre.secsTo(now);
                if(role ==  Qt::DisplayRole )
                {
                     return QString("%1h:%2m:%3s").arg(secs/3600).arg( (secs%3600)/60).arg((secs%3600)%60);
                }
                else
                {
                    return secs;
                }
            }


        }
    }
    else if( index.column() == 2)
    {
        return ev.id;
    }
    else if(index.column() == 3)
    {
        return ev.type;
    }
    else if(index.column() == 4)
    {
         return ev.description;
    }

}


Qt::ItemFlags UserTableModel::flags(const QModelIndex &index) const
{

    if (!index.isValid())
        return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void UserTableModel::LoadNewLogs(QString logfile)
{
    m_LogDir = QFileInfo(logfile).absoluteDir();

    QRegExp sep("[_\.]");
    QString CurrentDateStr = logfile.section(sep,-2,-2);
    QString sensorlog;
    foreach (QString slog,m_LogDir.entryList(QStringList() <<"SensorData_*_*.log"))
    {
        QString datestr = slog.section(sep,-2,-2);
        if(datestr == CurrentDateStr)
        {
            sensorlog = m_LogDir.absoluteFilePath(slog);
            break;
        }
    }
    if(QFile::exists(logfile))
    {
        FilterEvent(logfile);
        DrawData(sensorlog);
    }
}

void UserTableModel::FilterEvent(QString EventLogs)
{
    QFile log(EventLogs);
    m_items.clear();
    m_navigator.clear();
    bool newStartProgram = false;
    bool newStartStep = false;
    if(log.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!log.atEnd()) {
            QString ReadData = log.readLine();
            QStringList fields = ReadData.split(";");
            //if(isInterest(ReadData))
            {
                if(fields.size() >= 5)
                {
                    m_items.push_back(Event(fields[0],fields[1],fields[2],fields[3],fields[4]));
                }
            }
            if(isStartProgram(ReadData)) // the first level
            {
                newStartProgram = true;
                QString Name = fields[4].section(" ",4,-9);
                m_navigator.push_back(program_t(QString("%1##%2").arg(Name).arg(m_items.size() - 1),
                                   QList<step_t > ()));
            }
            else if(isStartBottleCheck(ReadData)) // the first level
            {
                newStartProgram = false;
                newStartStep = false;
                QString Name = "Bottle check";
                m_navigator.push_back(program_t(QString("%1##%2").arg(Name).arg(m_items.size() - 1),
                                   QList<step_t > ()));
            }
            else if(isStartSelfttest(ReadData)) //the first level
            {
                newStartProgram = false;
                newStartStep = false;
                QString Name = "Selftest";
                m_navigator.push_back(program_t(QString("%1##%2").arg(Name).arg(m_items.size() - 1),
                                   QList<step_t > ()));
            }
            else if(isPowerFail(ReadData))
            {
                newStartProgram = false;
                newStartStep = false;
                QString Name = "Power failure";//.section(" ",4,-9);
                m_navigator.push_back(program_t(QString("%1##%2").arg(Name).arg(m_items.size() - 1),
                                   QList<step_t > ()));
            }
            else if(isShutDown(ReadData))
            {
                newStartProgram = false;
                newStartStep = false;
                QString Name = "Shutdown";//.section(" ",4,-9);
                m_navigator.push_back(program_t(QString("%1##%2").arg(Name).arg(m_items.size() - 1),
                                   QList<step_t > ()));
            }
            else if(isStartStep(ReadData))
            {
                if(!newStartProgram)
                {
                   m_navigator.push_back(program_t("cont..",QList<step_t >()));
                   newStartProgram = true;
                }
                newStartStep = true;
                QString StepName = fields[4].section("(",0,0).section(" ",1,2);
                QList<program_t >::iterator pit  = m_navigator.end() - 1;
                pit->second.push_back(step_t(QString("%1##%2").arg(StepName).arg(m_items.size() - 1), QStringList()));
            }
            else if(isStartScenario(ReadData))
            {
                if(!newStartProgram)
                {
                    m_navigator.push_back(program_t("cont..",QList<step_t >()));
                    newStartProgram = true;
                }
                if(!newStartStep)
                {
                    QList<program_t >::iterator pit = m_navigator.end() - 1;
                    pit->second.push_back(step_t("cont...",QStringList()));
                    newStartStep = true;
                }
                QList<program_t >::iterator pit = m_navigator.end() - 1;
                QList<step_t >::iterator sit = pit->second.end() - 1;
                sit->second.push_back(QString("%1##%2").arg(fields[4]).arg(m_items.size() - 1));
            }
        }
    }
}

QModelIndex UserTableModel::IndexByDate(const QDateTime& dt, QString key)
{
    int start = 0;
    int end = m_items.size() - 1;
    int row = -1;
    int mid;
    QString str = dt.toString("yyyy-MM-dd hh:mm:ss.zzz");
    while(start < end)
    {
        mid = start + (end - start) / 2;
       if(str.compare(m_items[mid].time) > 0)
       {
           start = mid;
       }
       else if(str.compare(m_items[mid].time) < 0)
       {
           end = mid;
       }
       else
       {
           row = mid;
           break;
       }
    }
    bool mached = false;
    int in = row;
    while(!mached && (in < m_items.size()) && (str.compare(m_items[in].time) == 0))
    {
        if(m_items[in].isMached(QRegExp(key)))
        {
            mached = true;
            break;
        }
        in++;
    }
    if(!mached)
    {
        in = row-1;
        while(!mached && in >=0 && str.compare(m_items[in].time) == 0)
        {
            if(m_items[in].isMached(QRegExp(key)))
            {
                mached = true;
                break;
            }
            in--;
        }
    }
    if(mached)
    {
        row =  in;
    }
    if(row < 0)
    {
        row = end;
    }
    return index(row,0);
}

void UserTableModel::AnalyzeLog()
{
    if(m_LogDir.exists())
    {
//        foreach (QString log,m_LogDir.entryList(QStringList() <<"HISTOCOREPEARL_*_*.log", NoFilter,QDir::Name))
//        {
//        }
    }
}
void UserTableModel::DrawData(QString EventLogs)
{

}

void UserTableModel::ResetLogs()
{


}
