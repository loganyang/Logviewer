#ifndef LPKGREADER_H
#define LPKGREADER_H

#include <QWidget>

namespace Ui {
class LPKGReader;
}

class LPKGReader : public QWidget
{
    Q_OBJECT

public:
    explicit LPKGReader(QWidget *parent = 0);
    ~LPKGReader();
signals:
    void PressedReadPackageButton();
    void PressedUnpackButton();
private:
    Ui::LPKGReader *ui;
};

#endif // LPKGREADER_H
