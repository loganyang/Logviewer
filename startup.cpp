#include "startup.h"
#include "usertablemodel.h"
#include "logparser.h"
Startup::Startup(QObject *parent) : QObject(parent)
{

}

Startup::~Startup()
{

}

void Startup::LoadNewLogs(QString logfile)
{
    UserTableModel* model = new UserTableModel();
    model->LoadNewLogs(logfile);
    emit sigSetModel(model);
}

void Startup::OnLogLevelChnaged(QString key, int state)
{

}


