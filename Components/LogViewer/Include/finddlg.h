#ifndef FINDDLG_H
#define FINDDLG_H

#include <QDialog>
#include <QAbstractItemModel>
#include <QDateTime>
namespace Ui {
class FindDlg;
}
class EventLogSortFilter;
class MainWindow;

//adapter if UsertalbeModel for Search
class SearchModel : public QAbstractTableModel
{
public:
    explicit SearchModel(QObject *parent = 0);
    ~SearchModel();
    int rowCount(const QModelIndex &parent) const{ return m_orgTableDataModel == NULL? 0: m_orgTableDataModel->rowCount();}
    int columnCount(const QModelIndex &parent) const{ return 1;}
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const{return "Search results";}
    Qt::ItemFlags flags(const QModelIndex &index) const { return m_orgTableDataModel == NULL? Qt::NoItemFlags: m_orgTableDataModel->flags(index);}

    void setOrgModel(QAbstractTableModel* model);
    QModelIndex getOrgIndex(QModelIndex index);
private:
   QAbstractTableModel* m_orgTableDataModel;
};

class FindDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindDlg(MainWindow *parent);
    ~FindDlg();
public slots:
    void search();
    void positionToTable(QModelIndex index);

signals:
    void sigPosition(QModelIndex index);

private:
    Ui::FindDlg *ui;
    EventLogSortFilter* filter;
    SearchModel* sModel;
    MainWindow* mainWin;
};

#endif // FINDDLG_H
