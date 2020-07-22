/*
** Crytpo adapter layer
*/

#include <iostream>
#include <fstream>

#include "Crypto.h"

// include from CryptoPP
#include "aes.h"
#include "modes.h"
#include "filters.h"
#include "files.h"

// CryptoPP key and IV context
class CryptoPP_key_iv
{
public:
    //Key and IV setup
    //AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-   
    //bit). This key is secretly exchanged between two parties before communication   
    //begins. DEFAULT_KEYLENGTH= 16 bytes
    CryptoPP::byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];

    CryptoPP_key_iv()
    {
        memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
        memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);
    }

    //convert passphrase to AES fixed key
    //TODO - hash pp
    const int make_Key(string pp)
    {
        int len = pp.length();
        len = (len < CryptoPP::AES::DEFAULT_KEYLENGTH) ? len : CryptoPP::AES::DEFAULT_KEYLENGTH;

        memcpy(key, pp.c_str(), len);
        return len;
    }

    const int key_Size = CryptoPP::AES::DEFAULT_KEYLENGTH;
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
int Crypto::Encrypt(string infilename, string outfilename, string passphrase)
{
    CryptoPP_key_iv KIV;
    KIV.make_Key(passphrase);
    unsigned fileLength = getFileLength(infilename.c_str());

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
        inFile.read( (char*)inBuf, fileLength);

        //
        // Create Cipher 
        // <name_space>::<class>::<Member function>
        //

        CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(KIV.key, KIV.key_Size, KIV.iv);
        cfbEncryption.ProcessData(outBuf, inBuf, fileLength);
        outFile.write( (const char*)outBuf, fileLength);

        //close fstreams
        inFile.close();
        outFile.close();

        delete[] outBuf;
        delete[] inBuf;
    }

    return 0;
}

int Crypto::Decrypt(string infilename, string outfilename, string passphrase)
{
    CryptoPP_key_iv KIV;
    KIV.make_Key(passphrase);
    unsigned fileLength = getFileLength(infilename.c_str());

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
        // Create Cipher 
        // <name_space>::<class>::<Member function>
        //

        CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(KIV.key, KIV.key_Size, KIV.iv);
        cfbDecryption.ProcessData(outBuf, inBuf, fileLength);
        outFile.write((const char*)outBuf, fileLength);

        //close fstreams
        inFile.close();
        outFile.close();

        delete[] outBuf;
        delete[] inBuf;
    }

    return 0;
}
