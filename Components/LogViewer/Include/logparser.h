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
#include <QTableWidgetItem>

struct Analyze_Res
{
    enum Res_Code
    {
        PAR_CANCEL,
        PAR_SUCCESSFULE,
        PAR_FAIL
    };
    Res_Code code;
    QStringList items;
    QStringList condsMached; // datetime.*errorid.*stringid
    QString logdir;
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
    void setResult(Analyze_Res res){ result = res;}
    void show();

public slots:
    void onOk(bool ok);
    void onDoubleClicked(QTableWidgetItem* item);

signals:
    void positionAt(QString key, QDateTime dt, QString dir);

private:
    Ui::DlgParserResult *ui;
    Analyze_Res result;
};


class LogParser : public QObject
{
    Q_OBJECT
public:
    explicit LogParser();
    ~LogParser();

signals:
    void sigFinished(Analyze_Res result);

public slots:
    void start(QString path);
    void parse(QString path);
    bool CheckErrors(QString path);

private:
    void writeCSV(QString path);
    void Log(QString log);

private:
    QString m_ResultFile;
    QList<ControlKey> m_items;
    QThread* m_thread;
    QFile* mp_LogFile;
    QString m_LogfileName;
    QTextStream m_log;
};


#endif // LOGPARSER_H
