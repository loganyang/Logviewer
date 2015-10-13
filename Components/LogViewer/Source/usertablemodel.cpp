#include "../Include/usertablemodel.h"
#include <QDebug>
#include <QModelIndex>
#include "QRegularExpression"
#include "../Include/globaldefines.h"
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

bool UserTableModel::isStartProgram(QString line)
{
    return line.contains("67174418");//User start the program
}
bool UserTableModel::isStartStep(QString line)
{
    return line.contains("67175216");//Start Step
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
    if(!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole))
    {
        return QVariant();
    }
    if(index.row() >= m_items.count())
    {
        return QVariant();
    }
    const Event& ev = m_items[index.row()];

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
                return QString("%1:%2:%3").arg(secs/3600).arg( (secs%3600)/60).arg((secs%3600)%60);
            }


        }
    }
    else if(index.column() == 2)
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
    if(log.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!log.atEnd()) {
            QString ReadData = log.readLine();
            QStringList fields = ReadData.split(";");
            if(isInterest(ReadData))
            {
                if(fields.size() >= 5)
                {
                    m_items.push_back(Event(fields[0],fields[1],fields[2],fields[4]));
                }
            }
            if(isStartProgram(ReadData))
            {
                QString ProgramName = fields[4].section(" ",4,-9);
                m_navigator.push_back(QPair<QString, QStringList>(QString("%1##%2").arg(ProgramName).arg(m_items.size() - 1),
                                   QStringList()));
            }
            else if(isStartStep(ReadData))
            {
                if(m_navigator.size() == 0)
                {
                    m_navigator.push_back(QPair<QString, QStringList>("cont..",QStringList()));
                }
                QString StepName = fields[4].section("(",0,0).section(" ",1,2);
                QList<QPair<QString,QStringList> >::iterator it  = m_navigator.end() - 1;
                it->second.append(QString("%1##%2").arg(StepName).arg(m_items.size() - 1));
            }
        }
    }
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
