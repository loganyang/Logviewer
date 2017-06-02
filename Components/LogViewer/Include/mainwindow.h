#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QMap>
#include <QPair>
#include <QDate>
#include <QDir>
#include <QAbstractTableModel>
#include "sensordataitf.h"
#include "qwt_plot_curve.h"
#include "QMessageBox"
#include "QTimer"
#include "QThread"
#include "QDir"
#include "lpkgreader.h"
#include "logparser.h"
#include "../Include/legend.h"
#include "../Include/eventlogsortfilter.h"
#include "../include/usertablemodel.h"
namespace Ui {
class MainWindow;
}

class FindDlg;
class Picker;
const QString VERSION_LOGVIEWER = "1.0 HIM_0.022\n1.1 Integrated with LPKG reader\n1.2 Added scenario filters \n1.3 Checked log errors";
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

signals:
    void sigNewLogsSet(QString logfile);
    void sigLogFilterChanged(QString, int);
    void sigAnalysizeLog(QString);

public slots:
    quint32 loadLogs();
    void openEventLog(QString eventfile);
    void AnalysizeLog();
    void OpenLPKG();
    void SaveCurve();
    void Find();
    void ShowVersion();
    void DrawCurves(QModelIndex index);
    void onClickTabeView(QModelIndex index);
    void AddLogLevels();
    void OnClickLogFilter(QModelIndex index);
    void OnChangeCurveSettings(QwtPlotItem *item, tCurveSettingData data, int index);
    void OnLegendChecked(QwtPlotItem *item, bool on, int index);
    void OnZoomed(QRectF point);
    void OnFinishAnalyzingLog(Analyze_Res result);
    void UpdateStatusbar(QModelIndex index);
    void OnPositionItem(const QString key, QDateTime dt, QString logDir);
    void OnPositionItem(QModelIndex index);
    void OnPositionTreeView(int row);
    QAbstractTableModel* getTabeModel(){return m_model;}
private:
    QStandardItem* findInTree(int value);
    QDir getRecentDir();
    void setRecentDir(QString path);
    void resetSensorCurves();
private:
    Ui::MainWindow *ui;
    QDir m_EventLogDir; // the dir for the event log showed now
    SensorDataItf m_sditf;
    QHash<QString, QwtPlotCurve*> m_curves;
    Picker* m_zoom;
    QHash<QString, Qt::CheckState> m_logFilter;
    QMessageBox* m_waitingBox;
    LPKGReader* m_lpkg;
    FindDlg* m_FindDlg;
    DlgParserResult* m_dlgres;
    EventLogSortFilter* m_LogSortFilter;
    UserTableModel* m_model;
    QString m_CurrentEventFileName;//
};

#endif // MAINWINDOW_H
