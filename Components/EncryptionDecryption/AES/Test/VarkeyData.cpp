/****************************************************************************/
/*! \file VarkeyData.cpp
 *
 *  \brief Test data for AES variable key tests.
 *
 *  The data are taken from the original NIST submission of AES, file vk.txt.
 *  The plaintext is always 0 here.
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

#include "EncryptionDecryption/AES/Test/TestAES.h"

namespace EncryptionDecryption {

VARKEYTEST VarData[] =
{
    {"80000000000000000000000000000000",
        "0edd33d3c621e546455bd8ba1418bec8"},

    {"40000000000000000000000000000000",
        "c0cc0c5da5bd63acd44a80774fad5222"},

    {"20000000000000000000000000000000",
        "2f0b4b71bc77851b9ca56d42eb8ff080"},

    {"10000000000000000000000000000000",
        "6b1e2fffe8a114009d8fe22f6db5f876"},

    {"08000000000000000000000000000000",
        "9aa042c315f94cbb97b62202f83358f5"},

    {"04000000000000000000000000000000",
        "dbe01de67e346a800c4c4b4880311de4"},

    {"02000000000000000000000000000000",
        "c117d2238d53836acd92ddcdb85d6a21"},

    {"01000000000000000000000000000000",
        "dc0ed85df9611abb7249cdd168c5467e"},

    {"00800000000000000000000000000000",
        "807d678fff1f56fa92de3381904842f2"},

    {"00400000000000000000000000000000",
        "0e53b3fcad8e4b130ef73aeb957fb402"},

    {"00200000000000000000000000000000",
        "969ffd3b7c35439417e7bde923035d65"},

    {"00100000000000000000000000000000",
        "a99b512c19ca56070491166a1503bf15"},

    {"00080000000000000000000000000000",
        "6e9985252126ee344d26ae369d2327e3"},

    {"00040000000000000000000000000000",
        "b85f4809f904c275491fcdcd1610387e"},

    {"00020000000000000000000000000000",
        "ed365b8d7d20c1f5d53fb94dd211df7b"},

    {"00010000000000000000000000000000",
        "b3a575e86a8db4a7135d604c43304896"},

    {"00008000000000000000000000000000",
        "89704bcb8e69f846259eb0accbc7f8a2"},

    {"00004000000000000000000000000000",
        "c56ee7c92197861f10d7a92b90882055"},

    {"00002000000000000000000000000000",
        "92f296f6846e0eaf9422a5a24a08b069"},

    {"00001000000000000000000000000000",
        "e67e32bb8f11deb8699318bee9e91a60"},

    {"00000800000000000000000000000000",
        "b08eef85eaf626dd91b65c4c3a97d92b"},

    {"00000400000000000000000000000000",
        "661083a6addce79bb4e0859ab5538013"},

    {"00000200000000000000000000000000",
        "55dfe2941e0eb10afc0b333bd34de1fe"},

    {"00000100000000000000000000000000",
        "6bfe5945e715c9662609770f8846087a"},

    {"00000080000000000000000000000000",
        "79848e9c30c2f8cda8b325f7fed2b139"},

    {"00000040000000000000000000000000",
        "7a713a53b99fef34ac04deef80965bd0"},

    {"00000020000000000000000000000000",
        "18144a2b46620d32c3c32ce52d49257f"},

    {"00000010000000000000000000000000",
        "872e827c70887c80749f7b8bb1847c7e"},

    {"00000008000000000000000000000000",
        "6b86c6a4fe6a60c59b1a3102f8de49f3"},

    {"00000004000000000000000000000000",
        "9848bb3dfdf6f532f094679a4c231a20"},

    {"00000002000000000000000000000000",
        "925ad528e852e329b2091cd3f1c2bcee"},

    {"00000001000000000000000000000000",
        "80df436544b0dd596722e46792a40cd8"},

    {"00000000800000000000000000000000",
        "525daf18f93e83e1e74bbbdde4263bba"},

    {"00000000400000000000000000000000",
        "f65c9d2ee485d24701ffa3313b9d5be6"},

    {"00000000200000000000000000000000",
        "e4fc8d8bca06425bdf94afa40fcc14ba"},

    {"00000000100000000000000000000000",
        "a53f0a5ca1e4e6440bb975ff320de6f8"},

    {"00000000080000000000000000000000",
        "d55313b9394080462e87e02899b553f0"},

    {"00000000040000000000000000000000",
        "34a71d761f71bcd344384c7f97d27906"},

    {"00000000020000000000000000000000",
        "233f3d819599612ebc89580245c996a8"},

    {"00000000010000000000000000000000",
        "b4f1374e5268dbcb676e447529e53f89"},

    {"00000000008000000000000000000000",
        "0816bd27861d2ba891d1044e39951e96"},

    {"00000000004000000000000000000000",
        "f3be9ea3f10c73ca64fde5db13a951d1"},

    {"00000000002000000000000000000000",
        "2448086a8106fbd03048ddf857d3f1c8"},

    {"00000000001000000000000000000000",
        "670756e65bec8b68f03d77cdcdce7b91"},

    {"00000000000800000000000000000000",
        "ef968cf0d36fd6c6effd225f6fb44ca9"},

    {"00000000000400000000000000000000",
        "2e8767157922e3826ddcec1b0cc1e105"},

    {"00000000000200000000000000000000",
        "78ce7eec670e45a967bab17e26a1ad36"},

    {"00000000000100000000000000000000",
        "3c5cee825655f098f6e81a2f417da3fb"},

    {"00000000000080000000000000000000",
        "67bfdb431dce1292200bc6f5207adb12"},

    {"00000000000040000000000000000000",
        "7540fd38e447c0779228548747843a6f"},

    {"00000000000020000000000000000000",
        "b85e513301f8a936ea9ec8a21a85b5e6"},

    {"00000000000010000000000000000000",
        "04c67dbf16c11427d507a455de2c9bc5"},

    {"00000000000008000000000000000000",
        "03f75eb8959e55079cffb4ff149a37b6"},

    {"00000000000004000000000000000000",
        "74550287f666c63bb9bc7838433434b0"},

    {"00000000000002000000000000000000",
        "7d537200195ebc3aefd1eaab1c385221"},

    {"00000000000001000000000000000000",
        "ce24e4d40c68a82b535cbd3c8e21652a"},

    {"00000000000000800000000000000000",
        "ab20072405aa8fc40265c6f1f3dc8bc0"},

    {"00000000000000400000000000000000",
        "6cfd2cf688f566b093f67b9b3839e80a"},

    {"00000000000000200000000000000000",
        "bd95977e6b7239d407a012c5544bf584"},

    {"00000000000000100000000000000000",
        "df9c0130ac77e7c72c997f587b46dbe0"},

    {"00000000000000080000000000000000",
        "e7f1b82cadc53a648798945b34efeff2"},

    {"00000000000000040000000000000000",
        "932c6dbf69255cf13edcdb72233acea3"},

    {"00000000000000020000000000000000",
        "5c76002bc7206560efe550c80b8f12cc"},

    {"00000000000000010000000000000000",
        "f6b7bdd1caeebab574683893c4475484"},

    {"00000000000000008000000000000000",
        "a920e37cc6dc6b31da8c0169569f5034"},

    {"00000000000000004000000000000000",
        "919380ecd9c778bc513148b0c28d65fd"},

    {"00000000000000002000000000000000",
        "ee67308dd3f2d9e6c2170755e5784be1"},

    {"00000000000000001000000000000000",
        "3cc73e53b85609023a05e149b223ae09"},

    {"00000000000000000800000000000000",
        "983e8af7cf05ebb28d71eb841c9406e6"},

    {"00000000000000000400000000000000",
        "0f3099b2d31fa5299ee5bf43193287fc"},

    {"00000000000000000200000000000000",
        "b763d84f38c27fe6931dceb6715d4db6"},

    {"00000000000000000100000000000000",
        "5ae3c9b0e3cc29c0c61565cd01f8a248"},

    {"00000000000000000080000000000000",
        "f58083572cd90981958565d48d2dee25"},

    {"00000000000000000040000000000000",
        "7e6255eef8f70c0ef10337aab1cccef8"},

    {"00000000000000000020000000000000",
        "aad4bac34db22821841ce2f631961902"},

    {"00000000000000000010000000000000",
        "d7431c0409bb1441ba9c6858dc7d4e81"},

    {"00000000000000000008000000000000",
        "ef9298c65e339f6e801a59c626456993"},

    {"00000000000000000004000000000000",
        "53fe29f68ff541abc3f0ef3350b72f7e"},

    {"00000000000000000002000000000000",
        "f6bba5c10db02529e2c2da3fb582cc14"},

    {"00000000000000000001000000000000",
        "e4239aa37fc531a386dad1126fc0e9cd"},

    {"00000000000000000000800000000000",
        "8f7758f857d15bbe7bfd0e416404c365"},

    {"00000000000000000000400000000000",
        "d273eb57c687bcd1b4ea7218a509e7b8"},

    {"00000000000000000000200000000000",
        "65d64f8d76e8b3423fa25c4eb58a210a"},

    {"00000000000000000000100000000000",
        "623d802b4ec450d66a16625702fcdbe0"},

    {"00000000000000000000080000000000",
        "7496460cb28e5791baeaf9b68fb00022"},

    {"00000000000000000000040000000000",
        "34ea600f18bb0694b41681a49d510c1d"},

    {"00000000000000000000020000000000",
        "5f8ff0d47d5766d29b5d6e8f46423bd8"},

    {"00000000000000000000010000000000",
        "225f9286c5928bf09f84d3f93f541959"},

    {"00000000000000000000008000000000",
        "b21e90d25df383416a5f072cebeb1ffb"},

    {"00000000000000000000004000000000",
        "4aefcda089318125453eb9e8eb5e492e"},

    {"00000000000000000000002000000000",
        "4d3e75c6cd40ec4869bc85158591adb8"},

    {"00000000000000000000001000000000",
        "63a8b904405436a1b99d7751866771b7"},

    {"00000000000000000000000800000000",
        "64f0daae47529199792eae172ba53293"},

    {"00000000000000000000000400000000",
        "c3eef84bea18225d515a8c852a9047ee"},

    {"00000000000000000000000200000000",
        "a44ac422b47d47b81af73b3e9ac9596e"},

    {"00000000000000000000000100000000",
        "d16e04a8fbc435094f8d53adf25f5084"},

    {"00000000000000000000000080000000",
        "ef13dc34bab03e124eead8b6bf44b532"},

    {"00000000000000000000000040000000",
        "d94799075c24dcc067af0d392049250d"},

    {"00000000000000000000000020000000",
        "14f431771eddce4764c21a2254b5e3c8"},

    {"00000000000000000000000010000000",
        "7039329f36f2ed682b02991f28d64679"},

    {"00000000000000000000000008000000",
        "124ee24ede5551639db8b8b941f6141d"},

    {"00000000000000000000000004000000",
        "c2852879a34d5184e478ec918b993fee"},

    {"00000000000000000000000002000000",
        "86a806a3525b93e432053c9ab5abbedf"},

    {"00000000000000000000000001000000",
        "c1609bf5a4f07e37c17a36366ec23ecc"},

    {"00000000000000000000000000800000",
        "7e81e7cb92159a51ffcea331b1e8ea53"},

    {"00000000000000000000000000400000",
        "37a7be002856c5a59a6e03eafce7729a"},

    {"00000000000000000000000000200000",
        "bdf98a5a4f91e890c9a1d1e5faab138f"},

    {"00000000000000000000000000100000",
        "4e96acb66e051f2bc739cc3d3e34a26b"},

    {"00000000000000000000000000080000",
        "ee996cdd120eb86e21ecfa49e8e1fcf1"},

    {"00000000000000000000000000040000",
        "61b9e6b579dbf6070c351a1440dd85ff"},

    {"00000000000000000000000000020000",
        "ac369e484316440b40dfc83aa96e28e7"},

    {"00000000000000000000000000010000",
        "0a2d16de985c76d45c579c1159413bbe"},

    {"00000000000000000000000000008000",
        "da3fdc38da1d374fa4802cda1a1c6b0f"},

    {"00000000000000000000000000004000",
        "b842523d4c41c2211afe43a5800adce3"},

    {"00000000000000000000000000002000",
        "9e2cda90d8e992dba6c73d8229567192"},

    {"00000000000000000000000000001000",
        "d49583b781d9e20f5be101415957fc49"},

    {"00000000000000000000000000000800",
        "ef09da5c12b376e458b9b8670032498e"},

    {"00000000000000000000000000000400",
        "a96be0463da774461a5e1d5a9dd1ac10"},

    {"00000000000000000000000000000200",
        "32cee3341060790d2d4b1362ef397090"},

    {"00000000000000000000000000000100",
        "21cea416a3d3359d2c4d58fb6a035f06"},

    {"00000000000000000000000000000080",
        "172aeab3d507678ecaf455c12587adb7"},

    {"00000000000000000000000000000040",
        "b6f897941ef8ebff9fe80a567ef38478"},

    {"00000000000000000000000000000020",
        "a9723259d94a7dc662fb0c782ca3f1dd"},

    {"00000000000000000000000000000010",
        "2f91c984b9a4839f30001b9f430493b4"},

    {"00000000000000000000000000000008",
        "0472406345a610b048cb99ee0ef3fa0f"},

    {"00000000000000000000000000000004",
        "f5f39086646f8c05ed16efa4b617957c"},

    {"00000000000000000000000000000002",
        "26d50f485a30408d5af47a5736292450"},

    {"00000000000000000000000000000001",
        "0545aad56da2a97c3663d1432a3d1c84"}
};

int VarSize = sizeof(VarData)/sizeof(VARKEYTEST);
}                       // end namespace EncryptionDecryption