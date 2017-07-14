#include "../Include/lpkgreader.h"
#include "ui_lpkgreader.h"
#include "EncryptionDecryption/ReadArchive/Include/ReadArchive.h"
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include <QDir>

LPKGReader::LPKGReader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LPKGReader)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/lpkg.svg"));
    setWindowTitle("LPKG Reader");
    connect(ui->readPackage,SIGNAL(pressed()), this, SLOT(ReadPackage()));
    connect(ui->generateFile,SIGNAL(pressed()), this, SLOT(UnpackTheFiles()));
}

LPKGReader::~LPKGReader()
{
    delete ui;
}

void LPKGReader::Reset()
{
    ui->filelistWidget->clear();
}

/****************************************************************************/
/**
 * \brief Slot for the reading package.
 */
/****************************************************************************/
void LPKGReader::ReadPackage()
{
    QMessageBox MsgDialog;
    QFileDialog FileDialog(this);
    ui->filelistWidget->clear();
    m_RFile.getFiles().clear();

    // we use different keys for Leica and Viewer (Viewer with value 1)
    QByteArray KeyBytes(EncryptionDecryption::Constants::KEYFILESIZE, 0);
    KeyBytes[2*EncryptionDecryption::Constants::HASH_SIZE-1] = 1;


    FileDialog.setNameFilter("lpkg files (*.lpkg)");

    if (FileDialog.exec() == (int)QDialog::Accepted) {
        QStringList Files = FileDialog.selectedFiles();

        if (Files.value(0).contains(".lpkg")) {
            try {
                QFileInfo FileName(Files.value(0));
                m_FileName = FileName.baseName();
                m_FileDirPath = FileName.absolutePath();
                // read the archive file - add try catch
                ReadArchive(qPrintable(Files.value(0)), &m_RFile, "Import", KeyBytes);
                ui->generateFile->setEnabled(true);
            }
            catch (EncryptionDecryption::ExceptionNumber ExNumber) {
                MsgDialog.setIcon(QMessageBox::Critical);
                MsgDialog.setText("Archive file is tampered");
                (void)MsgDialog.exec();
                ui->filelistWidget->clear();
                m_RFile.getFiles().clear();
                return;
                // error occured
            }

            // write all the files
            foreach(QString KeyName, m_RFile.getFiles().uniqueKeys()) {
                QString FileName = KeyName.mid((KeyName.lastIndexOf('/') == 0 ? 0 :  KeyName.lastIndexOf('/') + 1));
                ui->filelistWidget->addItem(FileName);
            }

        }
    }
}


/****************************************************************************/
/**
 * \brief Slot for unpacking the files.
 */
/****************************************************************************/
void LPKGReader::UnpackTheFiles()
{
    QMessageBox MsgDialog;

    if (m_RFile.getFiles().count() != 0) {

        QDir Directory(m_FileDirPath + QDir::separator() + m_FileName);
        if (Directory.exists()) {
            QStringList DirFileNames = Directory.entryList(QStringList("*.*"), QDir::Files, QDir::Name | QDir::Time);
            foreach (QString Name, DirFileNames) {
                Directory.remove(Directory.absolutePath() + QDir::separator() + Name);
            }
        }
        else {
            Directory.mkdir(m_FileDirPath + QDir::separator() + m_FileName);
        }

        // write all the files
        foreach(QString KeyName, m_RFile.getFiles().uniqueKeys()) {
            foreach (QByteArray FileData, m_RFile.getFiles().values(KeyName)) {
                // try to create the files
                try {
                    if (KeyName.lastIndexOf(QDir::separator()) != -1) {
                        KeyName = KeyName.mid(KeyName.lastIndexOf(QDir::separator()));
                    }
                    else if(KeyName.lastIndexOf("/") != -1) {
                        KeyName = KeyName.mid(KeyName.lastIndexOf("/"));
                    }
                    KeyName = Directory.absolutePath() + QDir::separator() + KeyName;
                    // remove the full file path
                    EncryptionDecryption::FailSafeOpen FileWrite(KeyName, 'w');
                    FileWrite.write(FileData);
                    FileWrite.close();
                }
                catch (...) {
                    MsgDialog.setIcon(QMessageBox::Critical);
                    MsgDialog.setText("Unable to extract files");
                    (void)MsgDialog.exec();
                    // got a exception
                    return;
                }

            }
        }

        MsgDialog.setIcon(QMessageBox::Information);
        MsgDialog.setText("Files are extracted where lpkg file placed");
        (void)MsgDialog.exec();
    }
    else {
        MsgDialog.setIcon(QMessageBox::Critical);
        MsgDialog.setText("Please click on 'Read Package' button then click on 'Unpack files' button");
        (void)MsgDialog.exec();
    }
}
