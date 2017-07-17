#ifndef CURVESETTING_H
#define CURVESETTING_H

#include <QWidget>
#include <QColorDialog>
namespace Ui {
class CurveSetting;
}
struct tCurveSettingData
{
    QColor color;
    int yAaxisZoomFactor;
};
class CurveSetting : public QDialog
{
    Q_OBJECT
public:
    explicit CurveSetting(QDialog *parent = 0);
    ~CurveSetting();
    bool getCurveSettings(tCurveSettingData& setting);

signals:
    void SettingChanged(QColor,int);
public slots:
    void onPressOkButton(bool clicked);
    void onPressCancellButton(bool clicked);


private:
    Ui::CurveSetting *ui;
    QColorDialog* dlgColorDialog;
};

#endif // CURVESETTING_H
