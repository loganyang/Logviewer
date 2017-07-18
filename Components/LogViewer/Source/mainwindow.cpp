#include "../Include/mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include "QDebug"
#include "qwt_legend.h"
#include "QGroupBox"
#include "QCheckBox"
#include "qwt_plot_zoomer.h"
#include "qwt_plot_panner.h"
#include "qwt_plot_canvas.h"
#include "qwt_legend.h"
#include "qwt_date_scale_draw.h"
#include "qwt_scale_div.h"
#include "qwt_plot_magnifier.h"
#include "QColorDialog"
#include "qwt_date_scale_engine.h"
#include "qwt_picker_machine.h"
#include "qwt_plot_picker.h"
#include "qwt_plot_grid.h"
#include "QMessageBox"
#include "QRadioButton"
#include "QStandardItemModel"
#include "QItemDelegate"
#include "../Include/globaldefines.h"
#include "../Include/usertablemodel.h"
#include "QThread"
#include "qwt_plot_zoneitem.h"
#include "../Include/lpkgreader.h"
#include "qwt_plot_renderer.h"
#include "qstatusbar.h"
#include "QListWidget"
#include "../Include/finddlg.h"
#include "QScrollBar"
#include "QPixmap"
class Picker : public QwtPlotZoomer
{
  public:
    Picker(QWidget* widget):QwtPlotZoomer(widget)
    {
        setRubberBandPen( QColor(Qt::black) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
        setRubberBand( QwtPicker::RectRubberBand );
        setMousePattern( QwtEventPattern::MouseSelect1,
            Qt::LeftButton,Qt::ControlModifier);
        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton,Qt::ControlModifier);
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton);
    }
    virtual QwtText trackerTextF(const QPointF & point) const
    {
        QwtText text =  QwtText(QString("%1, %2").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss.zzz"))
                                            .arg(point.y()));

        text.setColor( Qt::white );

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );
        return text;
    }
};

class DateScaleDraw: public QwtDateScaleDraw
{
public:
    DateScaleDraw( Qt::TimeSpec timeSpec = Qt::LocalTime ):
        QwtDateScaleDraw( timeSpec )
    {
        // as we have dates from 2010 only we use
        // format strings without the year

        setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz" ); //hh:mm:ss:zzz\nyyyy-MM-dd
        setDateFormat( QwtDate::Second, "hh:mm:ss" );
        setDateFormat( QwtDate::Minute, "hh:mm" );
        setDateFormat( QwtDate::Hour, "hh:mm" );
        setDateFormat( QwtDate::Day, "yyyy-MM-dd" );
        //setDateFormat( QwtDate::Week, "Www" );
        setDateFormat( QwtDate::Month, "MMM" );
    }
};


class ZoneItem: public QwtPlotZoneItem
{
public:
    ZoneItem( const QString &title )
    {
        setTitle( title );
        setZ( 11 ); // on top the the grid
        setOrientation( Qt::Vertical );
//        setItemAttribute( QwtPlotItem::Legend, false );

    }

    void setColor( const QColor &color )
    {
        QColor c = color;

        c.setAlpha( 100 );
        setPen( c );

        c.setAlpha( 20 );
        setBrush( c );
    }

    void setInterval( const QDateTime &date1, const QDateTime &date2 )
    {
        QwtPlotZoneItem::setInterval( QwtDate::toDouble( date1 ),
            QwtDate::toDouble( date2 ) );
    }
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_model(NULL),
    m_LogSortFilter(NULL),
    m_ManualWgt(NULL),
    m_CurrentEventFileName(""),
    ui(new Ui::MainWindow)
{

    //init tableview
    ui->setupUi(this);
    QFile styleSheet(":/qss/logviewer.qss");
    if (styleSheet.open(QIODevice::ReadOnly))
    {
        setStyleSheet(styleSheet.readAll());
    }

    setAutoFillBackground( true );
    setWindowTitle("Himalaya Logviewer");
    setWindowState(Qt::WindowMaximized);
    ui->centralWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tabWidget->setTabOrder(ui->event,ui->sensor);
    ui->tabWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   // ui->tableView->setStyleSheet("QTableView{selection-background-color:blue;}");
    //setStyleSheet("*[flat=false]");
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(DrawCurves(QModelIndex)));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(onClickTabeView(QModelIndex)));
    connect(ui->tableView,SIGNAL(clicked(QModelIndex)),this,SLOT(UpdateStatusbar(QModelIndex)));
    //init plot
    ui->qwtPlot->setAutoReplot(false);
    DateScaleDraw* xAxisScalreDraw = new DateScaleDraw();
    xAxisScalreDraw->setLabelRotation(-60);
    xAxisScalreDraw->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, xAxisScalreDraw);
    ui->qwtPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtDateScaleEngine());
    ui->qwtPlot->setAxisMaxMajor(QwtPlot::xBottom,36);
    ui->qwtPlot->setAxisMaxMinor(QwtPlot::xBottom,10);
    //init legend
    Legend *legend = new Legend;
    ui->qwtPlot->insertLegend( legend, QwtPlot::LeftLegend );
    connect( legend, SIGNAL( checked( QwtPlotItem *, bool, int ) ),
        SLOT(OnLegendChecked(QwtPlotItem *,bool, int)) );
    connect( legend, SIGNAL( changeCurveSettings(QwtPlotItem*,tCurveSettingData,int)),
        SLOT(OnChangeCurveSettings(QwtPlotItem*,tCurveSettingData,int)));
    //init menu
     QAction *ccfg = new QAction("&Open log",this);
     connect(ccfg,SIGNAL(triggered()),this,SLOT(loadLogs()));
     ccfg->setShortcut(tr("Ctrl+O"));

     QAction *Anls = new QAction("&Analyse log",this);
     connect(Anls,SIGNAL(triggered()),this,SLOT(AnalysizeLog()));
     Anls->setShortcut(tr("Ctrl+A"));

     QAction *lpkg = new QAction(QIcon(":/img/lpkg.svg"),"&Unzip LPKG",this);
     connect(lpkg,SIGNAL(triggered()),this,SLOT(OpenLPKG()));
     lpkg->setShortcut(tr("Ctrl+U"));

     QAction *SaveCurves = new QAction("&Save Curves",this);
     connect(SaveCurves,SIGNAL(triggered()),this,SLOT(SaveCurve()));

     QAction *Search = new QAction(QIcon(":/img/find.svg"),"&Find",this);
     connect(Search,SIGNAL(triggered()),this,SLOT(Find()));
     Search->setShortcut(tr("Ctrl+F"));

     QMenu *File = menuBar()->addMenu("&File");
     File->addAction(ccfg);

     QMenu *Tool = menuBar()->addMenu("&Tool");
     Tool->addAction(Search);
     Tool->addAction(SaveCurves);
     Tool->addAction(Anls);
     Tool->addAction(lpkg);

     QMenu *About = menuBar()->addMenu("&About");
     QAction *version = new QAction("&Version",this);
     About->addAction(version);
     connect(version,SIGNAL(triggered()),this,SLOT(ShowVersion()));

     QAction *help = new QAction("&Help",this);
     About->addAction(help);
     connect(help,SIGNAL(triggered()),this,SLOT(ShowManual()));

     m_waitingBox = new QMessageBox(QMessageBox::Information,"Info", "please waiting",
                      QMessageBox::NoButton, ui->tableView, Qt::Popup);
     m_waitingBox->setStandardButtons(QMessageBox::NoButton);
     m_waitingBox->setDefaultButton(QMessageBox::NoButton);

     m_lpkg = new LPKGReader();
     m_lpkg->setVisible(false);
     m_dlgres = new DlgParserResult();
     connect(m_dlgres,SIGNAL(positionAt(QString, QDateTime, QString)),this,SLOT(OnPositionItem(QString, QDateTime, QString)));
     m_dlgres->setVisible(false);

     m_FindDlg = new FindDlg(this);
     m_FindDlg->setVisible(false);
    statusBar()->setVisible(true);
    statusBar()->setSizeGripEnabled(false);
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{

    delete m_lpkg;
    delete m_dlgres;
    delete ui;
}

void MainWindow::UpdateStatusbar(QModelIndex index)
{
    Q_UNUSED(index);
    QItemSelectionModel *select = ui->tableView->selectionModel();
    if(select->hasSelection())
    {
        QModelIndexList gaps = select->selectedRows(1);
        bool ok = false;
        quint64 sum = 0;
        quint64 tmp = 0;
        foreach (QModelIndex gap, gaps) {
            tmp = 0;
            tmp = m_model->data(gap, Qt::EditRole).toULongLong(&ok);
            if(ok)
            {
                sum += tmp;
            }
        }
        statusBar()->showMessage(QString("sum: %1h:%2m:%3s").arg(sum/3600).arg( (sum%3600)/60).arg((sum%3600)%60));
    }
}
QDir MainWindow::getRecentDir()
{
    if(QFile::exists(".data"))
    {
        QFile tmpf(".data");
        if(tmpf.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            while (!tmpf.atEnd())
            {
                QString ReadData = tmpf.readLine();
                if(ReadData.startsWith("LOG_PATH##"))
                {
                    return QFileInfo(ReadData.section("##",1)).absoluteDir();
                }
            }
            tmpf.close();
        }

    }
    return QDir::current();
}
void MainWindow::setRecentDir(QString path)
{
    QFile tmpf(".data");
    if(tmpf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        tmpf.write(QString("LOG_PATH##%1").arg(path).toUtf8());
        tmpf.close();
    }
}

quint32 MainWindow::loadLogs()
{
    QDir recentDir = getRecentDir();
    QString file = QFileDialog::getOpenFileName(this,"select file",recentDir.absolutePath(),"log(HISTOCORE*_*_*.log)");
    if(file.isEmpty())
    {
        return 0;
    }

    openEventLog(file);
    return 0;
}
void MainWindow::openEventLog(QString eventfile)
{
    m_waitingBox->show();
    if(m_model == NULL)
    {
        m_model = new UserTableModel();
    }
    if(m_LogSortFilter == NULL)
    {
        m_LogSortFilter = new EventLogSortFilter();
        connect(ui->treeView,SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickLogFilter(QModelIndex)));
    }
    setWindowTitle("Himalaya Logviewer : " + eventfile);
    resetSensorCurves();
    m_model->LoadNewLogs(eventfile);
    m_LogSortFilter->applyFilter(QRegExp(".*"));
    m_LogSortFilter->setSourceModel(m_model);
    ui->tableView->setModel(m_LogSortFilter);
    ui->tableView->setWordWrap(true);
    AddLogLevels();
    init();
    m_model->setView(ui->tableView);

    m_EventLogDir = QFileInfo(eventfile).absoluteDir();
    m_CurrentEventFileName = QFileInfo(eventfile).fileName();

    setRecentDir(eventfile);
    m_waitingBox->hide();
}

void MainWindow::AnalysizeLog()
{
    QDir recentDir = getRecentDir();
    if(m_dlgres->isVisible())
    {
        m_dlgres->activateWindow();
        return;
    }

    QString dirstr = QFileDialog::getExistingDirectory(this,"select a directory",recentDir.absolutePath());
    if(dirstr.isEmpty()) //user click "cancel"
        return;
    QDir dir(dirstr);
    if(dir.exists())
    {
        setRecentDir(dirstr);
        emit sigAnalysizeLog(dirstr);
        m_dlgres->setWindowTitle(dirstr);
    }
    else
    {
        QMessageBox::information(ui->centralWidget,"Log Parser","The directory is not existed",QMessageBox::Ok );
    }
}
void MainWindow::OnFinishAnalyzingLog(Analyze_Res result)
{
    switch(result.code)
    {
    case Analyze_Res::PAR_CANCEL:
        QMessageBox::information(ui->centralWidget,"Log Parser","There is no any log files in your directory!!",QMessageBox::Ok );
        break;
    case Analyze_Res::PAR_SUCCESSFULE:
    case Analyze_Res::PAR_FAIL:
        m_dlgres->setResult(result);
        m_dlgres->show();
        break;
    default:
        ;
    }
}
void MainWindow::OpenLPKG()
{
    m_lpkg->Reset();
    m_lpkg->show();
}

void MainWindow::SaveCurve()
{
    QwtPlotRenderer renderer;
     renderer.exportTo( ui->qwtPlot, "Curves.png" );
}
void MainWindow::Find()
{
    m_FindDlg->show();
}

void MainWindow::setManualPage(int page)
{
    if(m_CachedImageManual.contains(page))
    {
        m_ManualLabel->setPixmap(QPixmap::fromImage(m_CachedImageManual.value(page)));
    }
}

void MainWindow::ShowManual()
{
    if(m_ManualWgt)
    {
        m_ManualWgt->setVisible(true);
        m_ManualWgt->activateWindow();
        return;
    }
    if(true)
    {
        QString name = QCoreApplication::applicationDirPath() + "/Manual.pdf";
        Poppler::Document* doc = Poppler::Document::load(name);
        if(doc && !doc->isLocked())
        {
           int numPage = doc->numPages();
           m_CachedImageManual.clear();
           for(int i = 0; i < numPage; i++)
           {
               Poppler::Page* pdfPage = doc->page(i);
               if(pdfPage != 0)
               {
                   QImage image = pdfPage->renderToImage();
                   m_CachedImageManual.insert(i,image);
                   delete pdfPage;
               }
           }
           delete doc;
           if(! m_CachedImageManual.isEmpty())
           {
                m_ManualWgt = new QWidget();
                m_ManualWgt->setWindowTitle("Help");
                m_ManualWgt->setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
                QScrollBar* scrollbar = new QScrollBar(Qt::Vertical,m_ManualWgt);
                scrollbar->setRange(0,numPage);
                m_ManualLabel = new QLabel(m_ManualWgt);
                QHBoxLayout *layout = new QHBoxLayout(m_ManualWgt);
                layout->addWidget(m_ManualLabel);
                layout->addWidget(scrollbar);
                m_ManualWgt->setLayout(layout);
                setManualPage(0);
                connect(scrollbar, SIGNAL(valueChanged(int)), this, SLOT(setManualPage(int)));
                m_ManualWgt->setVisible(true);
           }
        }
    }
}

void MainWindow::ShowVersion()
{
    QMessageBox::about(this,"Version information", VERSION_LOGVIEWER);
}

void MainWindow::init()
{
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    ui->tableView->horizontalHeader()->resizeSection(0,180);
    ui->tableView->horizontalHeader()->resizeSection(1,60);
    ui->tableView->horizontalHeader()->resizeSection(2,90);
    ui->tableView->horizontalHeader()->resizeSection(3,60);
//    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::onClickTabeView(QModelIndex index)
{
    if(!index.isValid())
        return;

    QModelIndex dIndex = m_LogSortFilter->mapToSource(index);
    OnPositionTreeView(dIndex.row());
}
void MainWindow::resetSensorCurves()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->qwtPlot->detachItems();
    m_sditf.reset();
}

void MainWindow::DrawCurves(QModelIndex index)
{
    if(!index.isValid())
        return;

    m_waitingBox->show();
    ui->tabWidget->setCurrentIndex(1);
    QCoreApplication::processEvents(QEventLoop::AllEvents,1000);
    ui->qwtPlot->detachItems();
    QAbstractItemModel *model = ui->tableView->model();
    QModelIndex xi = model->index(index.row(),0,QModelIndex());
    QDateTime Datetime = QDateTime::fromString(model->data(xi).toString(),"yyyy-MM-dd hh:mm:ss.zzz");

    if(!m_sditf.load(m_EventLogDir, Datetime))
    {
        m_waitingBox->hide();
        m_sditf.reset();
        QMessageBox::information(this,"Info", "No sensor data for this event log", QMessageBox::Ok);
        return;
    }
    QStringList CurveNames = m_sditf.curves();

    const double* time;
    const double* data;
    int size;
    int colIndex = 0;
    const Qt::GlobalColor colors[] =
    {
        Qt::black,
        Qt::red,
        Qt::darkRed,
        Qt::green,
        Qt::darkGreen,
        Qt::blue,
        Qt::darkBlue,
        Qt::cyan,
        Qt::darkCyan,
        Qt::magenta,
        Qt::darkMagenta,
        Qt::gray,
        Qt::darkGray,
    };
    foreach (const QString &key, CurveNames)
    {
        if(m_sditf.data(key,&time,&data,size))
        {
            QwtPlotCurve* curve = new QwtPlotCurve(key);
            m_curves.insert(key,curve);
            curve->setRawSamples(time,data,size);
            curve->setPen(QPen(colors[(colIndex++)%13],2));
            curve->attach(ui->qwtPlot);
        }
    }
    ZoneItem *zone = new ZoneItem( "Zone");
    zone->setColor( Qt::darkBlue );
    zone->setInterval( Datetime.addMSecs(-100),  Datetime.addMSecs(100) );
    zone->setVisible( true );
    zone->attach( ui->qwtPlot );

    ui->qwtPlot->setAxisScale(QwtPlot::xBottom,Datetime.toMSecsSinceEpoch() - 90000,
                              Datetime.toMSecsSinceEpoch() + 90000);
    ui->qwtPlot->setAxisScale(QwtPlot::yLeft,-50.0,150.00);

    // zoom in/out
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    ui->qwtPlot->setCanvas(canvas);



    Picker* zoomer = new Picker( canvas);
    connect(zoomer,SIGNAL(zoomed(QRectF)),this, SLOT(OnZoomed(QRectF)));
    zoomer->setZoomBase(QRectF(time[0],150,time[size - 1] - time[0], 200));

    QwtPlotPicker * d_picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
        QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
        canvas );
    d_picker->setStateMachine( new QwtPickerDragPointMachine() );
    d_picker->setRubberBandPen( QColor( Qt::black ) );
    d_picker->setRubberBand( QwtPicker::CrossRubberBand );
    d_picker->setTrackerMode(QwtPlotPicker::AlwaysOff);


    QwtPlotPanner *panner = new QwtPlotPanner( canvas );
    panner->setMouseButton( Qt::LeftButton );
    QwtPlotMagnifier* mag =  new QwtPlotMagnifier( canvas );
    //mag->setAxisEnabled(QwtPlot::yLeft,false);


    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( true );
    grid->setZ(10);
    grid->attach( ui->qwtPlot );

    ui->qwtPlot->replot();
    m_waitingBox->hide();
}

void MainWindow::AddLogLevels()
{


    ui->treeView->setFrameStyle( QTreeView::NoFrame );
//    ui->treeView->viewport()->setBackgroundRole(QPalette::Background);
    ui->treeView->viewport()->setAutoFillBackground( false );

    ui->treeView->setRootIsDecorated( true );
    ui->treeView->setHeaderHidden( true );

    QStandardItemModel *model = new QStandardItemModel();

    ui->treeView->setModel( model );

    // add log filter
    ui->treeView->setItemDelegate( new QItemDelegate( ui->treeView ) );
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    QStandardItem *filter = new QStandardItem("Filter");
    filter->setEditable(false);
    QMap<QString, QStringList>& LogLevelType = GlobalDefines::Instance().GetLogFilter();
    foreach (QString key, LogLevelType.keys())
    {
        QStandardItem *item = new QStandardItem(key);
        item->setCheckable(true);
        item->setEditable(false);
        item->setCheckState(Qt::Checked);
        m_logFilter.insert(key,Qt::Checked);
        item->setSelectable(false);
        filter->appendRow(item);
    }
    model->appendRow( filter );
    ui->treeView->setExpanded( model->index( model->rowCount() - 1, 0 ), true );


    //add navigator
    const QList<program_t > * navigator = m_model->GetLogNavigator();
    for(int i = 0; i < navigator->size(); i++)
    {
        QStandardItem *program = new QStandardItem(navigator->at(i).first.section("##",0,0));
        program->setData(navigator->at(i).first.section("##",1,1), Qt::DecorationRole);
        program->setEditable(false);
        program->setSelectable(true);
        foreach (step_t step, navigator->at(i).second)
        {
            QStandardItem *stepitem = new QStandardItem(step.first.section("##",0,0));

            stepitem->setData(step.first.section("##",1,1),Qt::DecorationRole);
            stepitem->setEditable(false);
            stepitem->setSelectable(true);
            foreach (QString scenario, step.second) {
               QStandardItem *scenitem = new QStandardItem(scenario.section("##",0,0));
               scenitem->setData(scenario.section("##",1,1),Qt::DecorationRole);
               scenitem->setEditable(false);
               scenitem->setSelectable(true);
               stepitem->appendRow(scenitem);
            }
            program->appendRow(stepitem);
        }
        model->appendRow(program);
    }
}
void MainWindow::OnPositionItem(QModelIndex index)
{
    ui->tableView->selectionModel()->clear();
    if(!index.isValid())
        return;
    QModelIndex ind = m_LogSortFilter->mapFromSource(index);
    if(ui->tableView->isRowHidden(ind.row()))
    {
        return;
    }
    if(ind.isValid())
    {
        ui->tableView->selectionModel()->select(ind,QItemSelectionModel::Select | QItemSelectionModel::Rows);
        ui->tableView->scrollTo(ind,QAbstractItemView::PositionAtTop);
    }
    OnPositionTreeView(index.row());
}

void MainWindow::OnPositionItem(const QString key, QDateTime dt, QString logDir)
{

    if(!logDir.isEmpty()) // log dir has been changed
    {
        QDir dir(logDir);
        QStringList files = dir.entryList(QStringList()<<QString("HISTOCORE*_*_%1.log")
                         .arg(dt.toString("yyyyMMdd")));
        if(files.size() > 0)
        {
            QString tgtFileName = logDir + "/" + files[0];
            if(m_CurrentEventFileName.isEmpty() || //never open a event log
                    tgtFileName.compare(m_EventLogDir.absolutePath() + "/" + m_CurrentEventFileName) != 0)
            {
                openEventLog(tgtFileName);
            }
        }
    }

    QModelIndex index = m_model->IndexByDate(dt, key);
    ui->tableView->selectionModel()->clear();
    if(index.isValid())
    {
        //position table view
        OnPositionItem(index);
    }
}

// value is the row that the tree item mapped in the tabe model
QStandardItem* MainWindow::findInTree(int value)
{
    const QStandardItemModel *model =
        qobject_cast<QStandardItemModel *>( ui->treeView->model() );

    bool ok = false;
    QList<QStandardItem*> items;
    QStandardItem* itemLev1;
    QStandardItem* itemLev2;
    for(int rowLev1 = 1; rowLev1 < model->rowCount(); rowLev1++)
    {
        itemLev1 = model->item(rowLev1);
        if(itemLev1)
        {
            items.append(itemLev1);
            for(int rowLev2 = 0; rowLev2 < itemLev1->rowCount(); rowLev2++)
            {
                itemLev2 = itemLev1->child(rowLev2);
                if(itemLev2)
                {
                    items.append(itemLev2);
                    for(int rowLev3 = 0; rowLev3 < itemLev2->rowCount(); rowLev3++)
                    {
                        items.append(itemLev2->child(rowLev3));
                    }
                }
            }
        }
    } // end for rowLev1
    if(items.size() <= 0 || value < items[0]->data(Qt::DecorationRole).toUInt(&ok))
    {
        return NULL;
    }
    for (int it = 0; it < items.size(); it++)
    {
        if(items[it])
        {
           int va = items[it]->data(Qt::DecorationRole).toUInt(&ok);
           if(value == va) return items[it];
           else if(value < va && it > 0) return items[it - 1];
           else if(value > va && it == (items.size() - 1)) return items[it];
        }
    }

}

void MainWindow::OnPositionTreeView(int row)
{
    ui->treeView->selectionModel()->clear();
    QStandardItem* selected = findInTree(row);
    if(selected)
    {
        ui->treeView->selectionModel()->select(selected->index(),QItemSelectionModel::Select);
        //ui->treeView->expand(selected->index());
        //ui->treeView->setCurrentIndex(selected->index());
        ui->treeView->scrollTo(selected->index());
    }
}

void MainWindow::OnClickLogFilter(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    const QStandardItemModel *model =
        qobject_cast<QStandardItemModel *>( ui->treeView->model() );
    const QStandardItem *item = model->itemFromIndex( index );
    if (item && item->isCheckable() )
    {
        if(m_logFilter.contains(item->text()))
        {
            if(m_logFilter.value(item->text()) != item->checkState())
            {
                m_logFilter.remove(item->text());
                m_logFilter.insert(item->text(),item->checkState());
            }
            QHashIterator<QString, Qt::CheckState> it(m_logFilter);
            QStringList filterValues = QStringList();
            filterValues <<"NULL=NotMatchAnyOne";
            while(it.hasNext())
            {
                it.next();
                if(it.value() == Qt::Checked)
                {
                    filterValues << GlobalDefines::Instance().getFilterValue(it.key());
                }
            }
            if(m_LogSortFilter)
            {
                m_LogSortFilter->applyFilter(QRegExp(filterValues.join("|")));
            }
        }
    }
    else
    {
        bool ok = false;
        quint32 row = model->data(index,Qt::DecorationRole).toUInt(&ok);
        ui->tableView->selectionModel()->clear();
        if(ok)
        {
            QModelIndex index = m_LogSortFilter->mapFromSource(m_model->index(row,0,QModelIndex()));
            if(index.isValid())
            {
                ui->tableView->selectionModel()->select(index,QItemSelectionModel::Select | QItemSelectionModel::Rows);
                ui->tableView->scrollTo(index,QAbstractItemView::PositionAtTop);
            }
        }
    }

}

void MainWindow::OnLegendChecked(QwtPlotItem * item, bool on, int index)
{
    item->setVisible(on);
    ui->qwtPlot->replot();
}

void MainWindow::OnChangeCurveSettings(QwtPlotItem * item, tCurveSettingData data, int index)
{
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
    curve->setPen(QPen(data.color,2));
    ui->qwtPlot->replot();
}



void MainWindow::OnZoomed(QRectF point)
{
    qDebug()<<point.x()<<" , " << point.y()<<" , " << point.width()<<" , " << point.height();
}
