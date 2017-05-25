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
    ui(new Ui::MainWindow)
{

    //init tableview
    ui->setupUi(this);
    setAutoFillBackground( true );
    setWindowTitle("Himalaya Logviewer");

    ui->centralWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tabWidget->setTabOrder(ui->event,ui->sensor);
    ui->tabWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(DrawCurves(QModelIndex)));
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

     QAction *Anls = new QAction("&Analyse log",this);
     connect(Anls,SIGNAL(triggered()),this,SLOT(AnalysizeLog()));

     QAction *lpkg = new QAction("&Unzip LPKG",this);
     connect(lpkg,SIGNAL(triggered()),this,SLOT(OpenLPKG()));

     QAction *SaveCurves = new QAction("&Save Curves",this);
     connect(SaveCurves,SIGNAL(triggered()),this,SLOT(SaveCurve()));

     QMenu *File = menuBar()->addMenu("&File");
     File->addAction(ccfg);

     QMenu *Tool = menuBar()->addMenu("&Tool");
     Tool->addAction(SaveCurves);
     Tool->addAction(Anls);
     Tool->addAction(lpkg);

     QMenu *About = menuBar()->addMenu("&About");
     QAction *version = new QAction("&Version",this);
     About->addAction(version);
     connect(version,SIGNAL(triggered()),this,SLOT(ShowVersion()));

     m_waitingBox = new QMessageBox(QMessageBox::Information,"Info", "please waiting",
                      QMessageBox::NoButton, ui->tableView, Qt::Popup);
     m_waitingBox->setStandardButtons(QMessageBox::NoButton);
     m_waitingBox->setDefaultButton(QMessageBox::NoButton);

     m_lpkg = new LPKGReader();
     m_lpkg->setVisible(false);
     m_dlgres = new DlgParserResult();
     m_dlgres->setVisible(false);

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

quint32 MainWindow::loadLogs()
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
                    m_LogDir = QFileInfo(ReadData.section("##",1)).absoluteDir();
                    break;
                }
            }
            tmpf.close();
        }

    }
    QString file = QFileDialog::getOpenFileName(this,"select file",m_LogDir.absolutePath(),"log(HISTOCORE*_*_*.log)");
    if(file.isEmpty())
    {
        return 1;
    }
    setWindowTitle("Himalaya Logviewer : " + file);
    openEventLog(file);
    m_LogDir = QFileInfo(file).absoluteDir();

    QFile tmpf(".data");
    if(tmpf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        tmpf.write(QString("LOG_PATH##%1").arg(file).toUtf8());
        tmpf.close();
    }
    return 0;
}
void MainWindow::openEventLog(QString eventfile)
{
    if(m_model == NULL)
    {
        m_model = new UserTableModel();
    }
    if(m_LogSortFilter == NULL)
    {
        m_LogSortFilter = new EventLogSortFilter();
        connect(ui->treeView,SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickLogFilter(QModelIndex)));
    }
    m_model->LoadNewLogs(eventfile);
    m_LogSortFilter->setSourceModel(m_model);
    ui->tableView->setModel(m_LogSortFilter);
    ui->tableView->setWordWrap(true);
    AddLogLevels();
    init();
    m_model->setView(ui->tableView);
}

void MainWindow::AnalysizeLog()
{
    m_dlgres->clear();
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
                    m_LogDir = QFileInfo(ReadData.section("##",1)).absoluteDir();
                    break;
                }
            }
            tmpf.close();
        }

    }

    QString dirstr = QFileDialog::getExistingDirectory(this,"select a directory",m_LogDir.absolutePath());
    if(dirstr.isEmpty()) //user click "cancel"
        return;
    QDir dir(dirstr);
    if(dir.exists())
    {
        m_LogDir = dir;
        QFile tmpf(".data");
        if(tmpf.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            tmpf.write(QString("LOG_PATH##%1").arg(dirstr).toUtf8());
            tmpf.close();
        }
        emit sigAnalysizeLog(dirstr);
        m_dlgres->setWindowTitle(dirstr);
    }
    else
    {
        QMessageBox::information(ui->centralWidget,"Log Parser","The directory is not existed",QMessageBox::Ok );
    }
}
void MainWindow::OnFinishAnalyzingLog(PARSER_Result result, QStringList res)
{
    switch(result)
    {
    case PAR_CANCEL:
        QMessageBox::information(ui->centralWidget,"Log Parser","There is no any log files in your directory!!",QMessageBox::Ok );
        break;
    case PAR_SUCCESSFULE:
    case PAR_FAIL:
        foreach (QString err, res)
        {
            m_dlgres->addItem(err);
        }
        m_dlgres->show();
        break;
    default:
        ;
    }
}
void MainWindow::OpenLPKG()
{
    m_lpkg->show();
}

void MainWindow::SaveCurve()
{
    QwtPlotRenderer renderer;
     renderer.exportTo( ui->qwtPlot, "Curves.png" );
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

    if(!m_sditf.load(m_LogDir, Datetime))
    {
        m_waitingBox->hide();
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
        Qt::yellow,
        Qt::darkYellow,
        Qt::gray,
        Qt::darkGray,
        Qt::lightGray
    };
    foreach (const QString &key, CurveNames)
    {
        if(m_sditf.data(key,&time,&data,size))
        {
            QwtPlotCurve* curve = new QwtPlotCurve(key);
            m_curves.insert(key,curve);
            curve->setRawSamples(time,data,size);
            curve->setPen(QPen(colors[(colIndex++)%16],2));
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
    ui->treeView->viewport()->setBackgroundRole(QPalette::Background);
    ui->treeView->viewport()->setAutoFillBackground( false );

    ui->treeView->setRootIsDecorated( true );
    ui->treeView->setHeaderHidden( true );

    QStandardItemModel *model = new QStandardItemModel();

    ui->treeView->setModel( model );

    // add log filter
    ui->treeView->setItemDelegate( new QItemDelegate( ui->treeView ) );
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
            while(it.hasNext())
            {
                it.next();
                if(it.value() == Qt::Checked)
                {
                    filterValues += GlobalDefines::Instance().getFilterValue(it.key());
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
