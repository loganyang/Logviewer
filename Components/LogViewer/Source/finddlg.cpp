#include "../Include/finddlg.h"
#include "ui_finddlg.h"
#include "../Include/eventlogsortfilter.h"
#include "../Include/mainwindow.h"

SearchModel::SearchModel(QObject *parent):
    QAbstractTableModel(parent),
    m_orgTableDataModel(NULL)
{}
SearchModel::~SearchModel(){}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || (role != Qt::DisplayRole && role != Qt::EditRole)
            || m_orgTableDataModel == NULL)
    {
        return QVariant();
    }
    int row = index.row();
    QString value = m_orgTableDataModel->data(m_orgTableDataModel->index(row,0)).toString() + ";" +
            m_orgTableDataModel->data(m_orgTableDataModel->index(row,2)).toString() + ";" +
            m_orgTableDataModel->data(m_orgTableDataModel->index(row,3)).toString() + ";" +
            m_orgTableDataModel->data(m_orgTableDataModel->index(row,4)).toString();

    return value;

}
QModelIndex SearchModel::getOrgIndex(QModelIndex index)
{
    return m_orgTableDataModel->index(index.row(),index.column());
}
void SearchModel::setOrgModel(QAbstractTableModel *model)
{
    m_orgTableDataModel = model;
}

FindDlg::FindDlg(MainWindow *parent) :
    QDialog(parent),
    sModel(NULL),
    mainWin(parent),
    ui(new Ui::FindDlg)
{
    ui->setupUi(this);
    setWindowTitle("find");
    setWindowIcon(QIcon(":/img/find.svg"));
    ui->results->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->results->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->results->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->results->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->results->horizontalHeader()->setStretchLastSection(true);

    filter = new EventLogSortFilter();
    sModel = new SearchModel();
    connect(ui->find,SIGNAL(pressed()),this,SLOT(search()));
    connect(ui->results,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(positionToTable(QModelIndex)));
    connect(this,SIGNAL(sigPosition(QModelIndex)),mainWin,SLOT(OnPositionItem(QModelIndex)));

}

FindDlg::~FindDlg()
{
    delete ui;
    delete filter;
    delete sModel;
    sModel = NULL;
}


void FindDlg::search()
{
    filter->applyFilter(QRegExp(ui->keyTxt->text()));
    sModel->setOrgModel(mainWin->getTabeModel());
    filter->setSourceModel(sModel);
    ui->results->setModel(filter);

}

void FindDlg::positionToTable(QModelIndex index)
{
    if(!index.isValid())
        return;
    QModelIndex ind = filter->mapToSource(index);
    emit sigPosition(sModel->getOrgIndex(ind));
//    QStringList fs = sModel->data(ind).toString().split(";");
//    QDateTime dt = QDateTime::fromString(fs[0], "yyyy-MM-dd hh:mm:ss.zzz");
//    emit sigPosition(dt,"");
}
