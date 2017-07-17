#include "../include/curvesetting.h"
#include "ui_curvesetting.h"
CurveSetting::CurveSetting(QDialog *parent) :
    QDialog(parent),
    ui(new Ui::CurveSetting)
{
    ui->setupUi(this);
    dlgColorDialog = new QColorDialog(this);
    dlgColorDialog->setWindowFlags(Qt::Widget);
    /* a few options that we must set for it to work nicely */
    dlgColorDialog->setOptions(
                /* do not use native dialog */
                QColorDialog::DontUseNativeDialog
                /* you don't need to set it, but if you don't set this
                    the "OK" and "Cancel" buttons will show up, I don't
                    think you'd want that. */
                | QColorDialog::NoButtons
    );
    ui->wgtColor = dlgColorDialog;
    connect(ui->btnOk,SIGNAL(clicked(bool)),this, SLOT(onPressOkButton(bool)));
    connect(ui->btnCancel,SIGNAL(clicked(bool)),this, SLOT(onPressCancellButton(bool)));

}

CurveSetting::~CurveSetting()
{
    delete ui;
    delete dlgColorDialog;
}

bool CurveSetting::getCurveSettings(tCurveSettingData& setting)
{

    if(exec() == 0)
    {
        setting.color = dlgColorDialog->selectedColor();
        setting.yAaxisZoomFactor = ui->yAxisScale->value();
        return true;
    }
    return false;
}

void CurveSetting::onPressOkButton(bool clicked)
{
    if(clicked)
    {
        exit(0);
    }
}

void CurveSetting::onPressCancellButton(bool clicked)
{
    if(clicked)
    {
        exit(1);
    }
}
