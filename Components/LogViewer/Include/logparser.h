#ifndef LOGPARSER_H
#define LOGPARSER_H
#include "QStringList"
#include "QList"
#include <QObject>
#include <QDir>
#include <QThread>
#include <QDateTime>
struct ControlKey
{
    ControlKey(QString nam, QString sta, QString sto, bool fla,QStringList inter):
        name(nam),startKey(sta),stopKey(sto),flag(fla),interruptKey(inter)
    {
        gaps.clear();
    }
    qint64 average();
    void add(QString& line);
    QString toString();
    QString name;
    QString startKey;
    quint64 startTime;
    QString stopKey;
    quint64 stopTime;
    quint64 tempTime;
    bool flag;
    QString desc;
    QStringList interruptKey;
    QStringList ignoreErrors;
    QList<qint64> gaps;
};

class LogParser : public QObject
{
    Q_OBJECT
public:
    explicit LogParser();
    ~LogParser();

signals:
    void sigFinished(bool success);

public slots:
    void start(QString path);
    void parse();

private:
    void writeCSV();

private:
    QDir m_LogDir;
    QString m_ResultFile;
    QList<ControlKey> m_items;
    QThread* m_thread;
};

#endif // LOGPARSER_H
