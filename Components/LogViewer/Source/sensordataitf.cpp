#include "../Include/sensordataitf.h"
#include "QHashIterator"
SensorDataItf::SensorDataItf(QObject *parent) : QObject(parent)
{

}

SensorDataItf::~SensorDataItf()
{
}

QStringList& SensorDataItf::curves()
{
    return m_curves;
}

bool SensorDataItf::data(QString name, const double** time, const double** data, int& size)
{
    if(m_Datas.contains(name))
    {
        *data = &(m_Datas.value(name)[0]);
        *time = &(m_DateTime[0]);
        size = m_DateTime.size();
        return true;
    }
    return false;
}

bool SensorDataItf::load(QDir dir, QDateTime date)
{

    bool ret = false;
    QRegExp sep("[_\.]");
    QString CurrentDateStr = date.date().toString("yyyyMMdd");
    QString file;
    if(! dir.exists())
    {
        return ret;
    }
    m_LogDir = dir;
    foreach (QString slog,m_LogDir.entryList(QStringList() <<"SensorData_*_*.log"))
    {
        QString datestr = slog.section(sep,-2,-2);
        if(datestr == CurrentDateStr)
        {
            file = m_LogDir.absoluteFilePath(slog);
            break;
        }
    }
    if(! QFile::exists(file))
    {
        return ret;
    }
    m_File = file;


    QFile log(m_File);
    if(log.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        bool flag = false;
        while (!log.atEnd()) {
            QString ReadData = log.readLine();
            QStringList fields = ReadData.split(" ",QString::SkipEmptyParts);
            if(fields.size() == 21 && fields[0] == "T_RV1")// sensor name
            {
                flag = true;
                reset();
                m_curves = fields;
                m_curves[20].remove("\n");
                foreach (QString str, m_curves) {
                   m_Datas.insert(str,QVector<double>());
                }
            }
            else if (fields.size() == 23 && flag) // sensor value
            {
                fields[1].chop(1);
                QString datestr = QString("%1 %2").arg(fields[0]).arg(fields[1]);
                m_DateTime.append(QDateTime::fromString(datestr,"yyyy-MM-dd hh:mm:ss.zzz").toMSecsSinceEpoch());
                for(int i = 2; i < fields.size(); i++)
                {
                    bool ok = false;
                    fields[i].remove("\n");
                    double sensor = fields[i].toDouble(&ok);
                    if(ok)
                    {
                        if(i >= 14 && i <= 19)  // change mA to A
                        {
                            sensor = sensor / 1000;
                        }
                        m_Datas[m_curves[i - 2]].append(sensor);
                    }
                }
            }
        }
    }

    return ret;
}

void SensorDataItf::reset()
{
    m_Datas.clear();
    m_curves.clear();
    m_DateTime.clear();
}
