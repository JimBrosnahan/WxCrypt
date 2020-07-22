#pragma once
using namespace std;

#include <string>

class Crypto
{

public:
    int Encrypt(string infile, string outfile, string passphrase);
    int Decrypt(string infile, string outfile, string passphrase);
    unsigned getFileLength(const char* filename);

};

