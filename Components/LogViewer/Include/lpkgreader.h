#ifndef LPKGREADER_H
#define LPKGREADER_H

#include <QWidget>
#include "EncryptionDecryption/AbstractFile/Include/AbstractFile.h"
namespace Ui {
class LPKGReader;
}

class LPKGReader : public QWidget
{
    Q_OBJECT

public:
    explicit LPKGReader(QWidget *parent = 0);
    ~LPKGReader();
    void Reset();
public slots:
    void ReadPackage();
    void UnpackTheFiles();
private:
    Ui::LPKGReader *ui;
    EncryptionDecryption::RAMFile m_RFile;
    QString m_FileName;
    QString m_FileDirPath;
};

#endif // LPKGREADER_H
