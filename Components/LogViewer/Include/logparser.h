#ifndef LOGPARSER_H
#define LOGPARSER_H
#include "QStringList"
#include "QList"
#include <QObject>
#include <QDir>
#include <QThread>
#include <QDateTime>
#include <QTextStream>
#include <QWidget>
enum PARSER_Result
{
    PAR_CANCEL,
    PAR_SUCCESSFULE,
    PAR_FAIL
};

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
namespace Ui {
class DlgParserResult;
}

class DlgParserResult : public QWidget
{
    Q_OBJECT

public:
    explicit DlgParserResult(QWidget *parent = 0);
    ~DlgParserResult();
    void addItem(QString item);
    void clear();

public slots:
    void onOk(bool ok);

private:
    Ui::DlgParserResult *ui;
};


class LogParser : public QObject
{
    Q_OBJECT
public:
    explicit LogParser();
    ~LogParser();

signals:
    void sigFinished(PARSER_Result result, QStringList res);

public slots:
    void start(QString path);
    void parse();
    bool CheckErrors();

private:
    void writeCSV();
    void Log(QString log);

private:
    QDir m_LogDir;
    QString m_ResultFile;
    QList<ControlKey> m_items;
    QThread* m_thread;
    QFile* mp_LogFile;
    QString m_LogfileName;
    QTextStream m_log;
};


#endif // LOGPARSER_H
