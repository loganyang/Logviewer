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
    Event(QString t, QString i, QString ty, QString desc):time(t),id(i),type(ty),description(desc){}
    QString time;
    QString id;
    QString type;
    QString description;
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
    bool isStartProgram(QString line);
    bool isStartStep(QString line);
    bool isStartScenario(QString line);
    bool isUserOperation(QString line);
    const QList<program_t > & GetLogNavigator() const
    {
        return m_navigator;
    }
//    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
signals:
    void sigUpdateModel();
public slots:
    void LoadNewLogs(QString logfile);
    void FilterEvent(QString EventLogs);
    void DrawData(QString EventLogs);
    void AnalyzeLog();
private:
    void ResetLogs();
public:
    QList<Event> m_items;
    QList<program_t > m_navigator;
    QDate m_Current;
    QDir m_LogDir;
    QString m_LogLevel;
    QTableView* m_view;
};

#endif // USERTABLEMODEL_H
