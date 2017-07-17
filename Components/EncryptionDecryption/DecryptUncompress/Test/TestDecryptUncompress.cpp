/****************************************************************************/
/*! \file TestDecryptUncompress.cpp
 *
 *  \brief test DecryptUncompress class for EncryptionDecryption module
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

#include "EncryptionDecryption/DecryptUncompress/Test/TestDecryptUncompress.h"

namespace EncryptionDecryption {
/****************************************************************************/
/*!
 * \brief empty constructor
 */
/****************************************************************************/
TestDecryptUncompress::TestDecryptUncompress()
{
    ;
}
/****************************************************************************/
/*!
 * \brief init clean keyfile (all bytes null)
 */
/****************************************************************************/
void TestDecryptUncompress::init()
{
    FailSafeOpen keyfile(Constants::keyfile, 'w');
    keyfile.write(QByteArray(Constants::KEYFILESIZE, 0));
    keyfile.close();

    FailSafeOpen ctrfile(Constants::counter, 'w');
    ctrfile.write(QByteArray(4, 0));
    ctrfile.close();
}
/****************************************************************************/
/*!
 * \brief delete the keyfile after all tests
 */
/****************************************************************************/
void TestDecryptUncompress::cleanupTestCase()
{
    QFile::remove(Constants::keyfile);
    QFile::remove(Constants::counter);
}

/****************************************************************************/
/*!
 * \brief read small file compressed and not encrypted
 */
/****************************************************************************/
void TestDecryptUncompress::utTestPlainShort()
{
    try
    {
        FailSafeOpen fd("../testresults/shortPlain", 'r');

        CryptoService cs;
        cs.initHmacs();

        DecryptUncompress du(&fd, cs);
        //QCOMPARE(du.read(33) + du.read(33) + du.read(34),
        //         QByteArray (100, 'a'));
        fd.close();

    }
    catch(...)
    {
        //QFAIL(qPrintable(e.getMsg()));
    }
}
/****************************************************************************/
/*!
 * \brief read small file compressed and encrypted
 */
/****************************************************************************/
void TestDecryptUncompress::utTestEncrShort()
{
//    FailSafeOpen fd("testresults/shortCipher", 'r');

//    CryptoService cs;
//    cs.initHmacs();
//    DecryptUncompress du(&fd, cs, true);
//    QCOMPARE(du.read(73) + du.read(3) + du.read(24),
//             QByteArray (100, 'a'));
//    fd.close();
}

/****************************************************************************/
/*!
* \brief write file of size COMPR_ENCR_BUFSIZE compressed and not encrypted
*/
/****************************************************************************/
void TestDecryptUncompress::utTestPlainMaxsize()
{
    FailSafeOpen fd("../testresults/maxsizePlain", 'r');

    CryptoService cs;
    cs.initHmacs();

    DecryptUncompress du(&fd, cs);
    //QCOMPARE(du.read(Constants::COMPR_ENCR_BUFSIZE),
     //        QByteArray (Constants::COMPR_ENCR_BUFSIZE, 'X'));
    fd.close();
}
/****************************************************************************/
/*!
* \brief write file of size COMPR_ENCR_BUFSIZE compressed and encrypted
*/
/****************************************************************************/
void TestDecryptUncompress::utTestEncrMaxsize()
{
    FailSafeOpen fd("../testresults/maxsizeCipher", 'r');

    CryptoService cs;
    cs.initHmacs();

    DecryptUncompress du(&fd, cs, true);
    //QCOMPARE(du.read(512*1024+37) + du.read(512*1024-37),
     //        QByteArray (1024*1024, 'X'));
    fd.close();
}

/****************************************************************************/
/*!
 * \brief read file of size 2*COMPR_ENCR_BUFSIZE+1 compressed and not encrypted
 */
/****************************************************************************/
void TestDecryptUncompress::utTestPlainLong()
{
    FailSafeOpen fd("../testresults/longPlain", 'r');

    CryptoService cs;
    cs.initHmacs();

    DecryptUncompress du(&fd, cs);
    //QCOMPARE(du.read(2*Constants::COMPR_ENCR_BUFSIZE+1),
     //        QByteArray(2*Constants::COMPR_ENCR_BUFSIZE+1, 'z'));
    fd.close();
}

/****************************************************************************/
/*!
 * \brief write file of size 2*COMPR_ENCR_BUFSIZE+1 compressed and encrypted
 */
/****************************************************************************/
void TestDecryptUncompress::utTestEncrLong()
{
    FailSafeOpen fd("../testresults/longCipher", 'r');

    CryptoService cs;
    cs.initHmacs();

    DecryptUncompress du(&fd, cs, true);
    //QCOMPARE(du.read(2*Constants::COMPR_ENCR_BUFSIZE+1),
      //       QByteArray(2*Constants::COMPR_ENCR_BUFSIZE+1, 'z'));
    fd.close();
}

}       // end namespace EncryptionDecryption

QTEST_MAIN(EncryptionDecryption::TestDecryptUncompress)
