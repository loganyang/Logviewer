#include "../Include/dlgcurvesettings.h"
#include "ui_dlgcurvesettings.h"
#include "qcolordialog.h"
#include <QVBoxLayout>
#include "qwt_scale_draw.h"
#include "qwt_scale_engine.h"

class RealScaleDraw: public QwtScaleDraw

{
public:
    RealScaleDraw()
    {
    }

    virtual QwtText label(double v) const
    {
        if(v <= 100.00)
        {
            return QwtText(QString::number(v/100));
        }
        else
        {
            return QwtText(QString::number(v -100));
        }
    }
};


dlgCurveSettings::dlgCurveSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgCurveSettings)
{
    ui->setupUi(this);
    setFixedSize(520,480);
    dlgCurveColor = new QColorDialog(this);
    dlgCurveColor->setWindowFlags(Qt::Widget);
    /* a few options that we must set for it to work nicely */
    dlgCurveColor->setOptions(
                /* do not use native dialog */
                QColorDialog::DontUseNativeDialog
                /* you don't need to set it, but if you don't set this
                    the "OK" and "Cancel" buttons will show up, I don't
                    think you'd want that. */
                | QColorDialog::NoButtons
    );

    d_sliderFrequency = new QwtSlider( this );
    d_sliderFrequency->setOrientation( Qt::Horizontal );
    d_sliderFrequency->setScalePosition( QwtSlider::TrailingScale );
    d_sliderFrequency->setScale( 1, 200 );
    d_sliderFrequency->setScaleMaxMinor( 5 );
    d_sliderFrequency->setScaleMaxMajor( 5 );

    d_sliderFrequency->setTotalSteps(200);
    d_sliderFrequency->setSingleSteps( 1 );
    d_sliderFrequency->setPageSteps( 10 );
    d_sliderFrequency->setHandleSize( QSize( 80, 20 ) );
    d_sliderFrequency->setBorderWidth( 1 );
    d_sliderFrequency->setValue(1);


    RealScaleDraw* sd = new RealScaleDraw();
    d_sliderFrequency->setScaleDraw(sd);


    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->setMargin(5);
    vlayout->addWidget(dlgCurveColor);
    vlayout->addWidget(d_sliderFrequency);
    vlayout->addWidget(ui->buttonBox);

    setLayout(vlayout);
}

dlgCurveSettings::~dlgCurveSettings()
{
    delete d_sliderFrequency;
    delete dlgCurveColor;
    delete ui;
}

bool dlgCurveSettings::getCurveSettingData(tCurveSettingData &data)
{
    exec();
    if(result() == QDialog::Accepted)
    {
        data.color = dlgCurveColor->currentColor();
        data.yAaxisZoomFactor = d_sliderFrequency->value();
        return true;
    }
    return false;

}
