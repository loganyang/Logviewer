/****************************************************************************/
/*! \file ReadArchive.h
 *
 *  \brief read an archive - define a function
 *
 *  $Version:   $ 1.0
 *  $Date:      $ 2012-11-26
 *  $Author:    $ Raju
 *
 *  \b Company:
 *
 *       Leica Biosystems Nussloch GmbH.
 *
 *  (C) Copyright 2010 by Leica Biosystems Nussloch GmbH. All rights reserved.
 *  This is unpublished proprietary source code of Leica. The copyright notice
 *  does not evidence any actual or intended publication.
 *
 *
 */
/****************************************************************************/

#ifndef ENCRYPTIONDECRYPTION_READARCHIVE_H
#define ENCRYPTIONDECRYPTION_READARCHIVE_H

#include <QByteArray>
#include <QMap>
#include <QSet>
#include <QStringList>
#include "EncryptionDecryption/DecryptUncompress/Include/DecryptUncompress.h"
#include "EncryptionDecryption/AbstractFile/Include/AbstractFile.h"

namespace EncryptionDecryption {

/* HELP CLASS *********/

namespace {
/****************************************************************************/
/*!
 * \brief class definition for read and bufferring
 */
/****************************************************************************/
class ReadAndBuffer
{
    public:
        ReadAndBuffer(FailSafeOpen* fd);
        QByteArray read(int size);
        /****************************************************************************/
        /*!
         * \brief get the bufferred data
         *
         * \return bufferred data
         */
        /****************************************************************************/
        inline QByteArray getBuffer() {return m_buffer;}

    private:
        FailSafeOpen* mp_fd;        //!< file descriptor
        QByteArray m_buffer;        //!< data buffer
};

}


void ReadArchive(QByteArray archive_name,
       AbstractFile* fd,
       QByteArray purpose,
                 QByteArray keydata = QByteArray(),
                 QStringList importfilelist = QStringList(), QString filepath = QString());

void ReadKeyData(QByteArray &keydata, int &hashIndex, QByteArray &deviceID, QByteArray &purpose);

void ImportArchiveFiles(FailSafeOpen &fd, ReadAndBuffer &cs,
                        QStringList &importfilelist, bool &compressed,
                        bool &encryption, AbstractFile* fout, QString &filepath,
                        int &noentries, QByteArray &purpose, QByteArray &keydata, QByteArray &deviceID);

void ExtractFileToMemory(AbstractFile* fout, DecryptUncompress &fdr, QStringList &importfilelist, QString &filepath);

bool CheckFileRequiresImport(const QByteArray &filename, const QStringList &filelist, const QString &filepath);

}       // end namespace EncryptionDecryption

#endif  // ENCRYPTIONDECRYPTION_READARCHIVE_H
