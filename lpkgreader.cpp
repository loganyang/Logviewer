#include "lpkgreader.h"
#include "ui_lpkgreader.h"

LPKGReader::LPKGReader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LPKGReader)
{
    ui->setupUi(this);
    connect(ui->readPackage,SIGNAL(pressed()), this, SIGNAL(PressedReadPackageButton()));
    connect(ui->generateFile,SIGNAL(pressed()), this, SIGNAL(PressedUnpackButton()));
}

LPKGReader::~LPKGReader()
{
    delete ui;
}
