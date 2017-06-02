#ifndef USERTABLEMODEL_H
#define USERTABLEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QStringList>
#include <QDateTime>
#include <QDir>
#include <QMap>
#include <QPair>
#include <QDate>
#include <QTableView>
typedef QPair<QString,QStringList> step_t;
typedef QPair<QString, QList<step_t > > program_t;
struct Event
{
    Event(QString t, QString i, QString ty, QString strid, QString desc):
        time(t),id(i),type(ty),stringid(strid),description(desc){}
    QString time;
    QString id;
    QString type;
    QString stringid;
    QString description;
    bool isMached(QRegExp reg)
    {
        return 0<= reg.indexIn(QString("%1;%2;%3;%4;%5").arg(time).arg(id).arg(type).arg(stringid).arg(description));
    }
};

class UserTableModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class DataItem;
public:
    explicit UserTableModel(QObject *parent = 0);
    void AddMsgInfro();
    bool isInterest(QString line) const;

    int rowCount(const QModelIndex &parent) const{ return m_items.count();}
    int columnCount(const QModelIndex &parent) const{ return 5;}
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setView(QTableView* view){m_view = view;}
    bool isStartProgram(QString line); //the first level
    bool isStartBottleCheck(QString line); // the first level
    bool isStartSelfttest(QString line); // the first level
    bool isShutDown(QString line); // the first level
    bool isPowerFail(QString line); // power failure
    bool isStartStep(QString line); // the second level
    bool isStartScenario(QString line); // the third level
    bool isUserOperation(QString line); // the third level
    const QList<program_t > * GetLogNavigator() const
    {
        return &m_navigator;
    }
    void setTypeFilter(const QHash<QString, Qt::CheckState>& filter){m_TypeFilter = filter;}
//    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
signals:
    void sigUpdateModel();
public slots:
    void LoadNewLogs(QString logfile);
    void FilterEvent(QString EventLogs);
    void DrawData(QString EventLogs);
    void AnalyzeLog();
    QModelIndex IndexByDate(const QDateTime& dt, QString key);
private:
    void ResetLogs();

private:
    QHash<QString, Qt::CheckState> m_TypeFilter;
public:
    QList<Event> m_items;
    QList<program_t > m_navigator;
    QDate m_Current;
    QDir m_LogDir;
    QString m_LogLevel;
    QTableView* m_view;
};

#endif // USERTABLEMODEL_H
