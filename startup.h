#ifndef STARTUP_H
#define STARTUP_H
#include "QAbstractTableModel"
#include <QObject>

class Startup : public QObject
{
    Q_OBJECT
public:
    explicit Startup(QObject *parent = 0);
    ~Startup();

signals:
    void sigSetModel(QAbstractTableModel*);
public slots:
    void LoadNewLogs(QString logfile);
    void OnLogLevelChnaged(QString key, int state);
};

#endif // STARTUP_H
