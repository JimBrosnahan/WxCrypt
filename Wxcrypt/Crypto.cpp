/*
** Copyright (c) 2020 - Jim Brosnahan
** Crytpo adapter interface to AES CFB functions
**
** This demonstration source is distributed in the hope it will be useful,
** BUT WITHOUT ANY WARRANTY.
**
** CryptoPP retain their respective CopyRights
** https://www.cryptopp.com/
**
*/

#include <iostream>
#include <fstream>


// include from CryptoPP library
#include "cryptlib.h"
#include "aes.h"
#include "modes.h"
#include "sha.h"
#include "hmac.h"
#include "pwdbased.h"

using CryptoPP::byte;
using CryptoPP::AES;
using CryptoPP::CFB_Mode;
using CryptoPP::SHA1;
using CryptoPP::HMAC;
using CryptoPP::PKCS5_PBKDF2_HMAC;


#include "Crypto.h"

// CryptoPP key, IV, secret and salt
class CryptoPP_key_iv
{
public:
    //Key and IV setup using Key and block length of 16 bytes
    byte key[AES::DEFAULT_KEYLENGTH], iv[AES::BLOCKSIZE];
    byte secret[AES::DEFAULT_KEYLENGTH], pwsalt[AES::DEFAULT_KEYLENGTH];

    CryptoPP_key_iv()
    {
        memset(key, 0x00, AES::DEFAULT_KEYLENGTH);
        memset(pwsalt, 0xA5, AES::DEFAULT_KEYLENGTH);
        memset(secret, 0x00, AES::DEFAULT_KEYLENGTH);
        memset(iv, 0x00, AES::BLOCKSIZE);
    }

    //sha1 password converting to AES fixed key
    const int derive_Key(string pwd)
    {
        int len = pwd.length();
        len = (len < AES::DEFAULT_KEYLENGTH) ? len : AES::DEFAULT_KEYLENGTH;
        memcpy(secret, pwd.c_str(), len);

        PKCS5_PBKDF2_HMAC<SHA1> pbkdf;
        pbkdf.DeriveKey(
            // derived 128bit key
            key, AES::DEFAULT_KEYLENGTH,
            // unused
            0x00,
            // password bytes and size
            secret, len,
            // salt bytes - using const array
            pwsalt, AES::DEFAULT_KEYLENGTH,
            // iterations
            7
        );

        return len;
    }

    const int key_Size = AES::DEFAULT_KEYLENGTH;
};

// use C runtime for getting file length in bytes
unsigned Crypto::getFileLength(const char* filename)
{
    struct stat results;

    if (stat(filename, &results) == 0)
        return results.st_size;
    else
        return 0;
}


/*
** using https://www.cryptopp.com/wiki/Advanced_Encryption_Standard
*/
unsigned Crypto::Process(Modes mode, string infilename, string outfilename, string password)
{
    CryptoPP_key_iv KIV;
    KIV.derive_Key(password);
    unsigned fileLength = getFileLength(infilename.c_str());
    unsigned result = 0;

    // open input source
    ifstream inFile(infilename.c_str(), ios::in | ios::binary);

    // create output sink
    ofstream outFile(outfilename.c_str(), ios::out | ios::binary);

    // check everything is valid
    if ((fileLength > 0) && (inFile) && (outFile))
    {
        // allocate work buffers
        CryptoPP::byte* inBuf = new CryptoPP::byte[fileLength];
        CryptoPP::byte* outBuf = new CryptoPP::byte[fileLength];

        // slurp
        inFile.read((char*)inBuf, fileLength);

        //
        // Create Ciphers and process
        // <class>::<Member function>
        //

        if (mode == Modes::ENCRYPT) {
            CFB_Mode<AES>::Encryption cfbEncryption(KIV.key, KIV.key_Size, KIV.iv);
            cfbEncryption.ProcessData(outBuf, inBuf, fileLength);
        }
        else {
            CFB_Mode<AES>::Decryption cfbDecryption(KIV.key, KIV.key_Size, KIV.iv);
            cfbDecryption.ProcessData(outBuf, inBuf, fileLength);
        }

        // write result
        outFile.write((const char*)outBuf, fileLength);

        //close fstreams
        inFile.close();
        outFile.close();

        delete[] outBuf;
        delete[] inBuf;
        result = fileLength;
    }

    return result;
}
