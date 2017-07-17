/****************************************************************************/
/*! \file Aes.h
 *
 *  \brief AES implementation for CTR mode
 *
 *  This is an adaption of the original NIST reference implementation and
 *  implements AES-128 encryption used for CTR mode encryption/decryption.
 *  Each key is used only once, so the counter block can always start with
 *  null (consisting of null bytes only). Because of CTR mode, no decryption
 *  has to be implemented.
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

#ifndef ENCRYPTIONDECRYPTION_AES_H
#define ENCRYPTIONDECRYPTION_AES_H

#include <QtGlobal>
#include <QByteArray>
#include <string>

namespace EncryptionDecryption {

/****************************************************************************/
/*!
 * \brief class for advace encyption standard
 */
/****************************************************************************/
class AES
{
    friend class TestAes;

    public:
        AES();
        void init(QByteArray& key);
        void AesCtr(QByteArray& data);

        static const int AES_SIZE = 16;           ///< AES block and key size

    private:
        quint8 Mul(quint8 elementone, quint8 elementtwo);
        void KeyAddition(quint8 state[4][4], const quint8 roundkey[4][4]);
        void ShiftRow(quint8 state[4][4]);
        void Substitution(quint8 state[4][4], const quint8 box[256]);
        void MixColumn(quint8 state[4][4]);
        void RijndaelKeySched(const quint8 key[AES_SIZE]);
        void RijndaelEncrypt(quint8 block[AES_SIZE]);

        // data
        static const int ROUNDS = 10;              ///< number of rounds
        bool initialized; ///< flag for initialization
        quint8 m_roundkeys[ROUNDS+1][4][4]; ///< round keys array

        quint8 m_keystream[AES_SIZE];  ///< the keystream
        quint8 *m_keyptr;               ///< next keystream byte to process

        quint8 m_ctrblock[AES_SIZE];   ///< the CTR block, encryption
                                        ///< generates the keystream
        int m_streamlen;                ///< the number of bytes in keystream
                                        ///< which are still unprocessed
};

}       // end namespace EncryptionDecryption

#endif                  // ENCRYPTIONDECRYPTION_AES_H
