#ifndef DLGCURVESETTINGS_H
#define DLGCURVESETTINGS_H

#include <QDialog>
#include <QColorDialog>
#include "qwt_slider.h"
namespace Ui {
class dlgCurveSettings;
}

struct tCurveSettingData
{
    QColor color;
    int yAaxisZoomFactor;
};

class dlgCurveSettings : public QDialog
{
    Q_OBJECT

public:
    explicit dlgCurveSettings(QWidget *parent = 0);
    ~dlgCurveSettings();
    bool getCurveSettingData(tCurveSettingData& data);
private:
    Ui::dlgCurveSettings *ui;
    QColorDialog* dlgCurveColor;
    QwtSlider* d_sliderFrequency;
};

#endif // DLGCURVESETTINGS_H
