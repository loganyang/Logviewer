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
#include "../Include/legend.h"
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
class Picker : public QwtPlotZoomer
{
  public:
    Picker(QWidget* widget):QwtPlotZoomer(widget)
    {
        setRubberBandPen( QColor( Qt::darkGreen ) );
        //setRubberBand(QwtPicker::CrossRubberBand);
        setTrackerMode( QwtPlotPicker::AlwaysOn );
        setMousePattern( QwtEventPattern::MouseSelect1,
            Qt::LeftButton,Qt::ControlModifier);
        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton,Qt::ControlModifier);
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton);
    }
    virtual QwtText trackerTextF(const QPointF & point) const
    {
        QwtText text =  QwtText(QString("%1 %2").arg(QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss.zzz"))
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

        setDateFormat( QwtDate::Millisecond, "hh:mm:ss:zzz\nyyyy-MM-dd" );
        setDateFormat( QwtDate::Second, "hh:mm:ss\nyyyy-MM-dd" );
        setDateFormat( QwtDate::Minute, "hh:mm\nyyyy-MM-dd" );
        setDateFormat( QwtDate::Hour, "hh:mm\nyyyy-MM-dd" );
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
    ui(new Ui::MainWindow)
{

    //init tableview
    ui->setupUi(this);
    setAutoFillBackground( true );
    setWindowTitle("Himalaya Logviewer");
//    setPalette(QPalette( Qt::darkGray));
//    QPalette pal = palette();

//    const QColor buttonColor = pal.color( QPalette::Button );

//    QLinearGradient gradient( rect().topLeft(), rect().bottomLeft() );
//    gradient.setColorAt( 0.0, Qt::white );
//    gradient.setColorAt( 0.7, buttonColor );
//    gradient.setColorAt( 1.0, buttonColor );
//    pal.setBrush( QPalette::Window, gradient );
//    setPalette( pal );
    //init central widget
    ui->centralWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tabWidget->setTabOrder(ui->event,ui->sensor);
    ui->tabWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableView->setAttribute(Qt::WA_TranslucentBackground);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setWordWrap(true);
    //ui->tableView->setPalette(QPalette(Qt::gray));
    // draw sensor data plot
    connect(ui->tableView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(DrawCurves(QModelIndex)));



    //init plot
    ui->qwtPlot->setAutoReplot(false);
//    QwtPlotCanvas *canvas = new QwtPlotCanvas();
//   // canvas->setPalette( Qt::lightGray );
//    //canvas->setBorderRadius( 10 );
//    ui->qwtPlot->setCanvas(canvas);

    DateScaleDraw* xAxisScalreDraw = new DateScaleDraw();
    ui->qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, xAxisScalreDraw);
    ui->qwtPlot->setAxisScaleEngine(QwtPlot::xBottom, new QwtDateScaleEngine());





    Legend *legend = new Legend;
    ui->qwtPlot->insertLegend( legend, QwtPlot::LeftLegend );
    connect( legend, SIGNAL( checked( QwtPlotItem *, bool, int ) ),
        SLOT(OnLegendChecked(QwtPlotItem *,bool, int)) );
    connect( legend, SIGNAL( doubleClicked(QwtPlotItem*,QColor,int)),
        SLOT(OnLegendDoubleClicked(QwtPlotItem*,QColor,int)));


    //init menu
     QAction *ccfg = new QAction("&Open log",this);
     connect(ccfg,SIGNAL(triggered()),this,SLOT(loadLogs()));

     QAction *Anls = new QAction("&Analysize log",this);
     connect(Anls,SIGNAL(triggered()),this,SLOT(AnalysizeLog()));

     QAction *lpkg = new QAction("&LPKG",this);
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
}

MainWindow::~MainWindow()
{
    delete m_lpkg;
    delete ui;
}

quint32 MainWindow::loadLogs()
{
    QString file = QFileDialog::getOpenFileName(this,"select file",m_LogDir.absolutePath(),"log(HISTOCORE*_*_*.log)");
    if(file.isEmpty())
    {
        return 1;
    }
    emit sigNewLogsSet(file);

    m_LogDir = QFileInfo(file).absoluteDir();

    return 0;
}

void MainWindow::AnalysizeLog()
{
    QString dirstr = QFileDialog::getExistingDirectory(this,"select a directory",m_LogDir.absolutePath());
    QDir dir(dirstr);
    if(dir.exists())
    {
        m_LogDir = dir;
        emit sigAnalysizeLog(dirstr);
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

void MainWindow::SetModel(QAbstractTableModel *model)
{
    delete ui->tableView->model();
    ui->tableView->setModel(model);
    AddLogLevels();
    init();
    dynamic_cast<UserTableModel*>(model)->setView(ui->tableView);
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
    m_sditf.load(m_LogDir, Datetime);
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
    QwtPlotPanner *panner = new QwtPlotPanner( canvas );
    panner->setMouseButton( Qt::LeftButton );
    QwtPlotMagnifier* mag =  new QwtPlotMagnifier( canvas );
    mag->setAxisEnabled(QwtPlot::yLeft,false);


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
    const UserTableModel *UserModel =
        qobject_cast<UserTableModel *>( ui->tableView->model() );

    const QList<QPair<QString,QStringList> >& navigator = UserModel->GetLogNavigator();
    for(int i = 0; i < navigator.size(); i++)
    {
        QStandardItem *program = new QStandardItem(navigator[i].first.section("##",0,0));
        program->setData(navigator[i].first.section("##",1,1), Qt::DecorationRole);
        program->setEditable(false);
        program->setSelectable(true);
        foreach (QString stepstr, navigator[i].second)
        {
            QStandardItem *step = new QStandardItem(stepstr.section("##",0,0));
            step->setData(stepstr.section("##",1,1),Qt::DecorationRole);
            step->setEditable(false);
            step->setSelectable(true);
            program->appendRow(step);
        }
        model->appendRow(program);
    }

    connect(ui->treeView,SIGNAL(clicked(QModelIndex)), this, SLOT(OnClickLogFilter(QModelIndex)));
//    connect(ui->treeView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnDoubleClickedNavi(QModelIndex)));

}

void MainWindow::OnDoubleClickedNavi(QModelIndex index)
{
    if(!index.isValid())
    {
        return;
    }
    const QStandardItemModel *model =
        qobject_cast<QStandardItemModel *>( ui->treeView->model() );
    bool ok = false;

    quint32 row = model->data(index,Qt::DecorationRole).toUInt(&ok);
    if(ok)
    {
        ui->tableView->scrollTo(ui->tableView->model()->index(row,0,QModelIndex()));
        ui->tableView->selectRow(row);
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
                UpdateView();
            }
        }
    }
    else
    {
        bool ok = false;
        quint32 row = model->data(index,Qt::DecorationRole).toUInt(&ok);
        if(ok)
        {
            ui->tableView->scrollTo(ui->tableView->model()->index(row,0,QModelIndex()));
            ui->tableView->selectRow(row);
        }
    }

}

void MainWindow::OnLegendChecked(QwtPlotItem * item, bool on, int index)
{
    item->setVisible(on);
//    if(on)
//    {
//        QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
//        QPen pen = curve->pen();
//        pen.setWidth(4);
//        curve->setPen(pen);
//    }
    ui->qwtPlot->replot();
}

void MainWindow::OnLegendDoubleClicked(QwtPlotItem * item, QColor color, int index)
{
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
    curve->setPen(QPen(color));
    ui->qwtPlot->replot();
}

void MainWindow::OnFinishAnalyzingLog(bool flag)
{
    QMessageBox::information(ui->centralWidget,"Log Parser","finish analyzing all log files",QMessageBox::Ok );
}

void MainWindow::OnZoomed(QRectF point)
{
    qDebug()<<point.x()<<" , " << point.y()<<" , " << point.width()<<" , " << point.height();
}

void MainWindow::UpdateView()
{
    const UserTableModel *model =
        qobject_cast<UserTableModel *>( ui->tableView->model() );

    for(int i = 0; i< model->rowCount(QModelIndex()); i++)
    {
        bool show = false;
        QString line =QString("%1;%2; %3").arg(model->data(model->index(i,2),Qt::DisplayRole).toString())
                                       .arg(model->data(model->index(i,3),Qt::DisplayRole).toString())
                                      .arg(model->data(model->index(i,4),Qt::DisplayRole).toString());
        QHashIterator<QString, Qt::CheckState> it(m_logFilter);
        while(it.hasNext())
        {
            it.next();
            if(it.value() == Qt::Checked)
            {
                if(GlobalDefines::Instance().interest(line,it.key()))
                {
                    show = true;
                    break;
                }
            }
        }
        if(show)
        {
            ui->tableView->showRow(i);
        }
        else
        {
            ui->tableView->hideRow(i);
        }

    }
}
