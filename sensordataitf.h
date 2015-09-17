#ifndef SENSORDATAITF_H
#define SENSORDATAITF_H

#include <QObject>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QDir>
#include "QVector"
#include "QPointF"
#include "QHash"
class SensorDataItf : public QObject
{
    Q_OBJECT
public:
    explicit SensorDataItf(QObject *parent = 0);
    ~SensorDataItf();
    bool load(QDir dir, QDateTime time);

signals:

public slots:
    QStringList& curves();
    bool data(QString name, const double** time, const double** data, int& size);
    void reset();
private:
    QDir m_LogDir;
    QString m_File;
    QStringList m_curves;
    QVector<double> m_DateTime;
    QHash<QString, QVector<double> > m_Datas;
};

#endif // SENSORDATAITF_H
